/*
*******************************************************************************
*                                              usb host module
*
*                             Copyright(C), 2006-2008, SoftWinners Co., Ltd.
*											       All Rights Reserved
*
* File Name :
*
* Author : GLHuang(HoLiGun)
*
* Version : 1.0
*
* Date : 2008.07.xx
*
* Description :
*
* History :
*******************************************************************************
*/
#include "usb_host_config.h"

#include "usb_host_base_types.h"
#include "usb_os_platform.h"

#include "usb_host_common.h"

#include "usb_list.h"
#include  "list_head_ext.h"
#include "usb_host_hub.h"
#include "usb_utils_find_zero_bit.h"
#include "usb_core_base.h"
#include "usb_msg.h"

#include "usb_virt_bus.h"
#include "usb_gen_dev_mod.h"
#include "usb_gen_hcd.h"
#include "usb_gen_hub.h"
#include "usb_core_config.h"
#include "urb.h"
#include  "usbh_debug.h"

#include "usb_gen_hub.h"
#include "usb_core_interface.h"


/* cycle leds on hubs that aren't blinking for attention */
static int blinkenlights = 0;
static __cpu_sr hub_event_lock;
static struct list_head hub_event_list;
static struct hub_thread_context hub_thread_cont;

static u8 hub_thread_id = 0;



//static int use_both_schemes = 1;
static int old_scheme_first = 0;

#define PORT_RESET_TRIES	5
#define SET_ADDRESS_TRIES	2
#define GET_DESCRIPTOR_TRIES	2
//#define SET_CONFIG_TRIES	(2 * (use_both_schemes + 1))	//--FIXME--这个是旧的
#define SET_CONFIG_TRIES	(2)								//这个是为了加速
#define USE_NEW_SCHEME(i)	((i) / 2 == old_scheme_first)

#define HUB_ROOT_RESET_TIME		50	/* times are in msec */
#define HUB_SHORT_RESET_TIME 	10
#define HUB_LONG_RESET_TIME		200
#define HUB_RESET_TIMEOUT		500

#define usb_sndaddr0pipe()	(PIPE_CONTROL << 30)
#define usb_rcvaddr0pipe()	((PIPE_CONTROL << 30) | USB_DIR_IN)

static struct usb_host_func_drv  hub_driver;


static void ep0_reinit(struct usb_host_virt_dev *udev);
static void hub_pre_reset(struct usb_hub *hub);

/* Note that hdev or one of its children must be locked! */
static struct usb_hub *hdev_to_hub(struct usb_host_virt_dev *hdev)
{
    return usb_mod_usb_get_intf_priv_data(hdev->actconfig->interfac[0]);
}


static void recursively_mark_NOTATTACHED(struct usb_host_virt_dev *udev)
{
    int i;

    for (i = 0; i < udev->maxchild; ++i)
    {
        if (udev->children[i])
            recursively_mark_NOTATTACHED(udev->children[i]);
    }
    udev->state = USB_STATE_NOTATTACHED;
}


/*
********************************************************************************
*                     kick_khubd
*
* Description:
*     把需要处理的hub加入到hub_event_list里, 并且唤醒hub线程
*
* Arguments:
*     hub  :  input. 待处理的hub
* Return value:
*     void
* note:
*     void
*********************************************************************************
*/
void kick_khubd(struct usb_hub *hub)
{
    USB_OS_ENTER_CRITICAL(hub_event_lock);
    if (hub)
    {
        list_head_malloc_and_add(hub, &hub_event_list);
        hub_thread_wakeup(&hub_thread_cont);
    }
    USB_OS_EXIT_CRITICAL(hub_event_lock);
}

void hub_activate(struct usb_hub *hub)
{
    int	status;

    hub->quiescing = 0;
    hub->activating = 1;

    //--<1>--发送get status urb
    status = usb_submit_urb(hub->urb, 0);
    if (status < 0)
        DMSG_PANIC("PANIC : activate --> %d\n", status);
    if (hub->has_indicators && blinkenlights)
    {
        //schedule_delayed_work(&hub->leds, LED_CYCLE_PERIOD);
    }

    //--<2>--
    kick_khubd(hub);
}

static void choose_address(struct usb_host_virt_dev *udev)
{
    int		devnum;
    struct usb_virt_bus	 *bus = udev->bus;

    /* If khubd ever becomes multithreaded, this will need a lock */

    /* Try to allocate the next devnum beginning at bus->devnum_next. */
    devnum = find_next_zero_bit((u32 *)(bus->devmap.devicemap), 128,
                                bus->devnum_next);
    if (devnum >= 128)
    {
        devnum = find_next_zero_bit((u32 *)(bus->devmap.devicemap), 128, 1);
    }
    bus->devnum_next = ( devnum >= 127 ? 1 : devnum + 1);

    if (devnum < 128)
    {
        usb_atomic_set_bit(devnum, bus->devmap.devicemap);;
        udev->devnum = devnum;
    }
}

static void release_address(struct usb_host_virt_dev *udev)
{
    if (udev->devnum > 0)
    {
        //usb_clear_bit32(udev->devnum, udev->bus->devmap.devicemap);
        usb_atomic_clear_bit(udev->devnum, udev->bus->devmap.devicemap);
        udev->devnum = -1;
    }
}

/* USB 2.0 spec Section 11.24.4.5 */
static int get_hub_descriptor(struct usb_host_virt_dev *hdev, void *data, int size)
{
    int i, ret;

    for (i = 0; i < 3; i++)
    {
        ret = usb_control_msg(hdev,
                              usb_rcvctrlpipe(hdev, 0),
                              USB_REQ_GET_DESCRIPTOR, USB_DIR_IN | USB_RT_HUB,
                              USB_DT_HUB << 8, 0, data, size,
                              USB_CTRL_GET_TIMEOUT);
        if (ret >= (USB_DT_HUB_NONVAR_SIZE + 2))
            return ret;
    }

    return -EINVAL;
}

/*
 * USB 2.0 spec Section 11.24.2.1
 */
static int clear_hub_feature(struct usb_host_virt_dev *hdev, int feature)
{
    return usb_control_msg(hdev,
                           usb_sndctrlpipe(hdev, 0),
                           USB_REQ_CLEAR_FEATURE,
                           USB_RT_HUB,
                           feature,
                           0,
                           NULL,
                           0,
                           1000);
}

/*
 * USB 2.0 spec Section 11.24.2.2
 */
static int clear_port_feature(struct usb_host_virt_dev *hdev, int port1, int feature)
{
    return usb_control_msg(hdev,
                           usb_sndctrlpipe(hdev, 0),
                           USB_REQ_CLEAR_FEATURE,
                           USB_RT_PORT,
                           feature,
                           port1,
                           NULL,
                           0,
                           1000);
}

/*
 * USB 2.0 spec Section 11.24.2.13
 */
static int set_port_feature(struct usb_host_virt_dev *hdev, int port1, int feature)
{
    return usb_control_msg( hdev,
                            usb_sndctrlpipe(hdev, 0),
                            USB_REQ_SET_FEATURE,
                            USB_RT_PORT,
                            feature,
                            port1,
                            NULL,
                            0,
                            1000);
}

static int hub_port_disable(struct usb_hub *hub, int port1, int set_state)
{
    struct usb_host_virt_dev *hdev = hub->hdev;
    int ret;

    if (hdev->children[port1 - 1] && set_state)
    {
        usb_set_device_state(hdev->children[port1 - 1], USB_STATE_NOTATTACHED);
    }

    ret = clear_port_feature(hdev, port1, USB_PORT_FEAT_ENABLE);
    if (ret)
    {
        DMSG_PANIC("ERR: cannot disable port %d (err = %d)\n", port1, ret);
    }

    return ret;
}

static void hub_power_on(struct usb_hub *hub)
{
    int port1;

    /* if hub supports power switching, enable power on each port */
    if ((hub->descriptor->wHubCharacteristics & HUB_CHAR_LPSM) < 2)
    {
        for (port1 = 1; port1 <= hub->descriptor->bNbrPorts; port1++)
            set_port_feature(hub->hdev, port1, USB_PORT_FEAT_POWER);
    }

    /* Wait for power to be enabled */
    USB_OS_DELAY_x_MS(hub->descriptor->bPwrOn2PwrGood * 2);
}




/* USB 2.0 spec, 7.1.7.3 / fig 7-29:
 *
 * Between connect detection and reset signaling there must be a delay
 * of 100ms at least for debounce and power-settling.  The corresponding
 * timer shall restart whenever the downstream port detects a disconnect.
 *
 * Apparently there are some bluetooth and irda-dongles and a number of
 * low-speed devices for which this debounce period may last over a second.
 * Not covered by the spec - but easy to deal with.
 *
 * This implementation uses a 1500ms total debounce timeout; if the
 * connection isn't stable by then it returns -ETIMEDOUT.  It checks
 * every 25ms for transient disconnects.  When the port status has been
 * unchanged for 100ms it returns the port status.
 */

#define HUB_DEBOUNCE_TIMEOUT	1500
#define HUB_DEBOUNCE_STEP	  25
#define HUB_DEBOUNCE_STABLE	 100

static int hub_port_debounce(struct usb_hub *hub, int port1)

{
    int ret = 0;
    int total_time = 0, stable_time = 0;
    u16 portchange = 0, portstatus = 0;
    u32 connection = 0xffff;

    for (total_time = 0; ; total_time += HUB_DEBOUNCE_STEP)
    {
        ret = hub_port_status(hub, port1, &portstatus, &portchange);
        if (ret < 0)
            return ret;

        if (!(portchange & USB_PORT_STAT_C_CONNECTION) &&
                (portstatus & USB_PORT_STAT_CONNECTION) == connection)
        {
            stable_time += HUB_DEBOUNCE_STEP;
            if (stable_time >= HUB_DEBOUNCE_STABLE)
                break;
        }
        else
        {
            stable_time = 0;
            connection = portstatus & USB_PORT_STAT_CONNECTION;
        }

        if (portchange & USB_PORT_STAT_C_CONNECTION)
        {
            clear_port_feature(hub->hdev, port1, USB_PORT_FEAT_C_CONNECTION);
        }

        if (total_time >= HUB_DEBOUNCE_TIMEOUT)
        {
            break;
        }

        USB_OS_DELAY_x_MS(HUB_DEBOUNCE_STEP);
    }

    if (stable_time < HUB_DEBOUNCE_STABLE)
        return -ETIMEDOUT;
    return portstatus;
}

/* 延迟一定时候后，读取port status,由此判断其
   1,是否connect
   2,握手后的速度。
*/
static int hub_port_wait_reset(struct usb_hub *hub, int port1,
                               struct usb_host_virt_dev *udev, unsigned int delay)
{
    int delay_time, ret;
    u16 portstatus;
    u16 portchange;

    for (delay_time = 0; delay_time < HUB_RESET_TIMEOUT; delay_time += delay)
    {
        /* wait to give the device a chance to reset */
        USB_OS_DELAY_x_MS(delay);

        /* read and decode port status */
        ret = hub_port_status(hub, port1, &portstatus, &portchange);
        if (ret < 0)
        {
            return ret;
        }

        /* Device went away? */
        if (!(portstatus & USB_PORT_STAT_CONNECTION))
        {
            return -ENOTCONN;
        }

        /* bomb out completely if something weird happened */
        if ((portchange & USB_PORT_STAT_C_CONNECTION))
        {
            return -EINVAL;
        }

        /* if we`ve finished resetting, then break out of the loop */
        if (!(portstatus & USB_PORT_STAT_RESET) &&
                (portstatus & USB_PORT_STAT_ENABLE))
        {
            if (portstatus & USB_PORT_STAT_HIGH_SPEED)
            {
                udev->speed = USB_SPEED_HIGH;
            }
            else if (portstatus & USB_PORT_STAT_LOW_SPEED)
            {
                udev->speed = USB_SPEED_LOW;
            }
            else
            {
                udev->speed = USB_SPEED_FULL;
            }

            return 0;
        }

        /* switch to the long delay after two short delay failures */
        if (delay_time >= 2 * HUB_SHORT_RESET_TIME)
        {
            delay = HUB_LONG_RESET_TIME;
        }
    }

    return -EBUSY;
}

/* 要求port reset,并判定速度.给hub发送port reset
   延迟一定时候后，读取port status,由此判断其
   1,是否connect . 2,握手后的速度
*/
int hub_port_reset( struct usb_hub *hub,
                    int port1,
                    struct usb_host_virt_dev *udev,
                    unsigned int delay)
{
    int i = 0, status = 0;

    /* Reset the port */
    for (i = 0; i < PORT_RESET_TRIES; i++)
    {
        //--<1>--给hub发送port reset
        status = set_port_feature(hub->hdev, port1, USB_PORT_FEAT_RESET);
        if (status)
        {
            DMSG_PANIC("cannot reset port %d (err = %d)\n", port1, status);
        }
        else
        {
            //--<2>--延迟一定时候后，读取port status,由此判断其
            //1,是否connect . 2,握手后的速度?
            status = hub_port_wait_reset(hub, port1, udev, delay);
            if (status)
            {
                DMSG_PANIC("port_wait_reset: err = %d\n", status);
            }
        }

        /* return on disconnect or reset */
        switch (status)
        {
        case 0:
            /* TRSTRCY = 10 ms */
            USB_OS_DELAY_x_MS(10);
        /* FALL THROUGH */
        case -ENOTCONN:
        case -ENODEV:
            clear_port_feature(hub->hdev,
                               port1,
                               USB_PORT_FEAT_C_RESET);

            /* FIXME need disconnect() for NOTATTACHED device */
            usb_set_device_state(udev, status ? USB_STATE_NOTATTACHED : USB_STATE_DEFAULT);
            return status;
        }

        delay = HUB_LONG_RESET_TIME;
    }

    DMSG_PANIC ("Cannot enable port %i.  Maybe the USB cable is bad?\n", port1);

    return status;
}

static int hub_set_address(struct usb_host_virt_dev *udev)
{
    int retval = 0;

    if (udev->devnum == 0)
    {
        DMSG_PANIC("ERR: devicee Address on USB bus is zero\n");
        return -EINVAL;
    }

    if (udev->state == USB_STATE_ADDRESS)
    {
        DMSG_PANIC("ERR: devicee state is in USB_STATE_ADDRESS\n");
        return 0;
    }

    if (udev->state != USB_STATE_DEFAULT)
    {
        DMSG_PANIC("ERR: devicee is in other state(%x)\n", udev->state);
        return -EINVAL;
    }

    DMSG_INFO("\n usb hub set new address(%d)\n\n", udev->devnum);

    retval = usb_control_msg(udev,
                             usb_sndaddr0pipe(),
                             USB_REQ_SET_ADDRESS,
                             0,
                             udev->devnum,
                             0,
                             NULL,
                             0,
                             USB_CTRL_SET_TIMEOUT);
    if (retval == 0)
    {
        usb_set_device_state(udev, USB_STATE_ADDRESS);
        ep0_reinit(udev);
    }

    return retval;
}

//port1上的设备时候present
static s32 _dev_is_present(struct usb_hub *hub, int port1)
{
    s32 ret = 0;
    u16 portchange, portstatus;

    ret = hub_port_status(hub, port1, &portstatus, &portchange);
    if (ret < 0)
    {
        return ret;
    }

    if(portstatus & USB_PORT_STAT_CONNECTION)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

USB_OS_KERNEL_EVENT *usb_address0_sem = NULL;

/* Reset device, (re)assign address, get device descriptor.
 * Device connection must be stable, no more debouncing needed.
 * Returns device in USB_STATE_ADDRESS, except on error.
 *
 * If this is called for an already-existing device (as part of
 * usb_reset_device), the caller must own the device lock.  For a
 * newly detected device that is not accessible through any global
 * pointers, it's not necessary to lock the device.
 */

/*
********************************************************************************
*                     hub_port_init
* Description:
*     usb host stop working
*
* Arguments:
*     hcd    		: input.
*     udev   		: input.
*     port1   		: input.
*     retry_counter : input.
*
* Return value:
*     void
*
* note:
*     这里完成了3件大事，
* 1,reset
* 2,set addr
* 3,get desc
*
*********************************************************************************
*/
static int hub_port_init(struct usb_hub *hub,
                         struct usb_host_virt_dev *udev,
                         int port1,
                         int retry_counter)
{
    struct usb_host_virt_dev *hdev 	= NULL;
    u8  err 						= 0;
    u32 buff_size 					= 64;
    u32	i = 0, j = 0;
    s32 retval 						= 0;
    u32	delay						= HUB_SHORT_RESET_TIME;
    enum usb_device_speed oldspeed 	= USB_SPEED_UNKNOWN;

    if(hub == NULL || udev == NULL)
    {
        DMSG_PANIC("ERR: hub_port_init, input == NULL\n");
        return -1;
    }

    oldspeed = udev->speed;

    hdev = hub->hdev;
    if(hdev == NULL)
    {
        DMSG_PANIC("ERR: hub_port_init, hdev == NULL\n");
        return -1;
    }

    /* root hub ports have a slightly longer reset period
     * (from USB 2.0 spec, section 7.1.7.5)
     */
    if (!hdev->parent)
    {
        delay = HUB_ROOT_RESET_TIME;
        if (port1 == hdev->bus->otg_port)
        {
            hdev->bus->b_hnp_enable = 0;
        }
    }

    /* Some low speed devices have problems with the quick delay, so */
    /*  be a bit pessimistic with those devices. RHbug #23670 */
    if (oldspeed == USB_SPEED_LOW)
    {
        delay = HUB_LONG_RESET_TIME;
    }

    if(usb_address0_sem)
    {
        USB_OS_SEMI_PEND(usb_address0_sem, 0, &err);
    }
    else
    {
        DMSG_PANIC("PANIC : usb_addr_semi == NULL\n");
    }

    DMSG_INFO("hub_port_init: udev address = %d\n", udev->devnum);

    release_address(udev);
    udev->devnum = 0;

    //--<1>--发送port reset,并判定速度
    /* Reset the device; full speed may morph to high speed */
    retval = hub_port_reset(hub, port1, udev, delay);
    if (retval < 0) 		/* error or disconnect */
    {
        DMSG_PANIC("ERR: hub_port_reset failed!\n");
        goto fail;
    }

    /* set hub port to device */
    udev->hub_port = port1;

    /* success, speed is known */
    retval = -ENODEV;

    if (oldspeed != USB_SPEED_UNKNOWN && oldspeed != udev->speed)
    {
        DMSG_PANIC("device reset changed speed!\n");
        goto fail;
    }

    oldspeed = udev->speed;

    /* USB 2.0 section 5.5.3 talks about ep0 maxpacket ...
     * it's fixed size except for full speed devices.
     */
    switch (udev->speed)
    {
    case USB_SPEED_HIGH:		/* fixed at 64 */
        udev->ep0.desc.wMaxPacketSize = 64;
        break;

    case USB_SPEED_FULL:		/* 8, 16, 32, or 64 */
        /* to determine the ep0 maxpacket size, try to read
         * the device descriptor to get bMaxPacketSize0 and
         * then correct our initial guess.
         */
        udev->ep0.desc.wMaxPacketSize = 64;
        break;

    case USB_SPEED_LOW:		/* fixed at 8 */
        udev->ep0.desc.wMaxPacketSize = 8;
        break;

    default:
        goto fail;
    }

    DMSG_INFO("[hub_port_init]: %s %s speed USB device  address %d\n",
              (udev->config) ? "reset" : "new",
              (udev->speed == USB_SPEED_LOW ? "low" :
               (udev->speed == USB_SPEED_FULL ? "full" :
                (udev->speed == USB_SPEED_HIGH ? "high" : "???"))),
              udev->devnum);
#if	0
    /* Set up TT records, if needed  */
    if (hdev->tt)
    {
        udev->tt = hdev->tt;
        udev->ttport = hdev->ttport;
    }
    else if (udev->speed != USB_SPEED_HIGH
             && hdev->speed == USB_SPEED_HIGH)
    {
        udev->tt = &hub->tt;
        udev->ttport = port1;
    }

#endif

    //--<2>--获得设备desc
    /* Why interleave GET_DESCRIPTOR and SET_ADDRESS this way?
     * Because device hardware and firmware is sometimes buggy in
     * this area, and this is how Linux has done it for ages.
     * Change it cautiously.
     *
     * NOTE:  If USE_NEW_SCHEME() is true we will start by issuing
     * a 64-byte GET_DESCRIPTOR request.  This is what Windows does,
     * so it may help with some non-standards-compliant devices.
     * Otherwise we start with SET_ADDRESS and then try to read the
     * first 8 bytes of the device descriptor to get the ep0 maxpacket
     * value.
     */
    for (i = 0; (i < GET_DESCRIPTOR_TRIES) && (_dev_is_present(hub, port1) == 1); (++i, USB_OS_DELAY_x_MS(100)))
    {
        if (USE_NEW_SCHEME(retry_counter))
        {
            struct usb_device_descriptor *buf;
            int r = 0;

            buf = USB_OS_MALLOC(buff_size, USB_MEM_FILE_TRIGGER , USB_MEM_LINE_TRIGGER );
            if (!buf)
            {
                retval = -ENOMEM;
                DMSG_PANIC("ERR: malloc failed\n");
                continue;
            }

            USB_OS_MEMSET(buf, 0, buff_size);

            DMSG_HUB_TEST("hub_port_init--1--\n");

            //获得设备desc
            /* Use a short timeout the first time through,
             * so that recalcitrant full-speed devices with
             * 8- or 16-byte ep0-maxpackets won't slow things
             * down tremendously by NAKing the unexpectedly
             * early status stage.  Also, retry on all errors;
             * some devices are flakey.
             */
            for (j = 0; (j < 3) && (_dev_is_present(hub, port1) == 1); ++j)
            {
                buf->bMaxPacketSize0 = 0;

                DMSG_HUB_TEST("hub_port_init--2--\n");

                r = usb_control_msg(udev,
                                    usb_rcvaddr0pipe(),
                                    USB_REQ_GET_DESCRIPTOR,
                                    USB_DIR_IN,
                                    USB_DT_DEVICE << 8,
                                    0,
                                    (void *)buf,
                                    buff_size,
                                    (i ? 5000 : 1000));

                switch (buf->bMaxPacketSize0)
                {
                case 8:
                case 16:
                case 32:
                case 64:
                    if (buf->bDescriptorType == USB_DT_DEVICE)
                    {
                        r = 0;
                        break;
                    }

                /* FALL THROUGH */
                default:
                    if (r == 0)
                    {
                        DMSG_PANIC("ERR: unkown bMaxPacketSize0(%d)\n", buf->bMaxPacketSize0);
                        r = -EPROTO;
                    }
                    break;
                }

                if (r == 0)
                {
                    break;
                }
            }

            DMSG_HUB_TEST("hub_port_init--3--\n");

            print_device_descriptor(buf);

            udev->descriptor.bMaxPacketSize0 = buf->bMaxPacketSize0;

            USB_OS_FREE((void *)buf);
            buf = NULL;

            //重新reset
            retval = hub_port_reset(hub, port1, udev, delay);
            if (retval < 0) 		/* error or disconnect */
            {
                goto fail;
            }

            if (oldspeed != udev->speed)
            {
                retval = -ENODEV;
                goto fail;
            }

            if(r)
            {
                DMSG_PANIC( "device descriptor read/%s, error %d\n", "64", r);
                retval = -EMSGSIZE;
                continue;
            }
        }

        DMSG_HUB_TEST("hub_port_init--4--\n");

        choose_address(udev);
        if (udev->devnum <= 0)
        {
            DMSG_PANIC("ERR: choose_address failed!\n");
            goto fail;
        }

        //--<3>--设置地址
        for (j = 0; j < SET_ADDRESS_TRIES; ++j)
        {
            retval = hub_set_address(udev);
            if (retval >= 0)
            {
                break;
            }

            USB_OS_DELAY_x_MS(200);
        }

        DMSG_HUB_TEST("hub_port_init--5--\n");

        if (retval < 0)
        {
            DMSG_PANIC("device not accepting address %d, error %d\n",
                       udev->devnum, retval);
            goto fail;
        }

        /* cope with hardware quirkiness:
         *  - let SET_ADDRESS settle, some device hardware wants it
         *  - read ep0 maxpacket even for high and low speed,
         */
        USB_OS_DELAY_x_MS(10);
        if (USE_NEW_SCHEME(retry_counter))
        {
            break;
        }

        DMSG_HUB_TEST("hub_port_init--6--\n");


        //--<4>--获得设备desc,会自动拷贝到usb_host_virt_dev->设备描述符中
        //这里只要求获取8字节，只是为了获得bMaxPacketSize0
        retval = usb_get_device_descriptor(udev, 8);
        if(retval < 8)
        {
            DMSG_PANIC( "device descriptor "
                        "read/%s, error %d\n",
                        "8", retval);
            if (retval >= 0)
            {
                retval = -EMSGSIZE;
            }
        }
        else
        {
            retval = 0;
            break;
        }
    }

    DMSG_HUB_TEST("hub_port_init--7--\n");


    if (retval)
    {
        goto fail;
    }

    i = udev->descriptor.bMaxPacketSize0;
    if (le16_to_cpu(udev->ep0.desc.wMaxPacketSize) != i)
    {
        if (udev->speed != USB_SPEED_FULL ||
                !(i == 8 || i == 16 || i == 32 || i == 64))
        {
            DMSG_PANIC( "ep0 maxpacket = %d\n", i);
            retval = -EMSGSIZE;
            goto fail;
        }

        udev->ep0.desc.wMaxPacketSize = cpu_to_le16(i);
        ep0_reinit(udev);
    }

    DMSG_HUB_TEST("hub_port_init--8--\n");


    //--<5>--正式获得设备描述符
    retval = usb_get_device_descriptor(udev, USB_DT_DEVICE_SIZE);
    if (retval < (signed)sizeof(udev->descriptor))
    {
        DMSG_PANIC("device descriptor read/%s, error %d\n", "all", retval);
        if (retval >= 0)
        {
            retval = -ENOMSG;
        }

        goto fail;
    }

    DMSG_HUB_TEST("hub_port_init--9--\n");


    retval = 0;

fail:
    /*临时删除,不禁止该port
    if (retval){
    	hub_port_disable(hub, port1, 0);
    }
    */

    /* if port initialize failed, then release device address */
    if(retval != 0)
    {
        release_address(udev);
    }

    if(usb_address0_sem)
    {
        USB_OS_SEMI_POST(usb_address0_sem);
    }
    else
    {
        DMSG_PANIC("PANIC : usb_addr_semi == NULL\n");
    }

    return retval;
}

static void get_string(struct usb_host_virt_dev *udev, char **string, u8 index)
{
    char *buf = NULL;

    if (!index)
    {
        DMSG_PANIC("ERR: input error, index = %d\n", index);
        return;
    }

    buf = USB_OS_MALLOC(256, USB_MEM_FILE_TRIGGER , USB_MEM_LINE_TRIGGER );
    if (!buf)
    {
        DMSG_PANIC("ERR: malloc failed\n");
        return;
    }
    USB_OS_MEMSET(buf, 0, 256);

    if (usb_string(udev, index, buf, 256) > 0)
    {
        *string = buf;
    }
    else
    {
        USB_OS_FREE(buf);
        buf = NULL;
    }
}

/* 如果多余1个config的话，我们将按次序挑选一个标准类 */
static int choose_configuration(struct usb_host_virt_dev *udev)
{
    int c = 0, i = 0;

    /* NOTE: this should interact with hub power budgeting */
    c = udev->config[0].desc.bConfigurationValue;
    if (udev->descriptor.bNumConfigurations != 1)
    {
        for (i = 0; i < udev->descriptor.bNumConfigurations; i++)
        {
            struct usb_interface_descriptor	*desc;

            /* heuristic:  Linux is more likely to have class
             * drivers, so avoid vendor-specific interfaces.
             */
            desc = &udev->config[i].intf_cache[0]->altsetting_array->desc;
            if (desc->bInterfaceClass == USB_CLASS_VENDOR_SPEC)
            {
                continue;
            }

            /* COMM/2/all is CDC ACM, except 0xff is MSFT RNDIS.
             * MSFT needs this to be the first config; never use
             * it as the default unless Linux has host-side RNDIS.
             * A second config would ideally be CDC-Ethernet, but
             * may instead be the "vendor specific" CDC subset
             * long used by ARM Linux for sa1100 or pxa255.
             */
            if (desc->bInterfaceClass == USB_CLASS_COMM
                    && desc->bInterfaceSubClass == 2
                    && desc->bInterfaceProtocol == 0xff)
            {
                c = udev->config[1].desc.bConfigurationValue;
                continue;
            }

            c = udev->config[i].desc.bConfigurationValue;

            break;
        }

        DMSG_INFO("[hub]: configuration #%d chosen from %d choices\n",
                  c, udev->descriptor.bNumConfigurations);
    }

    return c;
}

/*
*******************************************************************************
*                     usb_new_device
*
* Description:
*    枚举一个新设备。获得该设备的所有描述符，并且匹配相应的驱动。
*
* Parameters:
*    udev  ： input. 新设备信息
*
* Return value:
*    0	：成功
*	!0	：失败
*
* note:
*    无
*
*******************************************************************************
*/
int usb_new_device(struct usb_host_virt_dev *udev)
{
    int err = 0;
    int c = 0;

    DMSG_PANIC("******usb_new_device test0******\n");


    /* get device configuration and parser it */
    err = usb_get_all_config_desc_and_parser(udev);
    if (err < 0)
    {
        DMSG_PANIC( "can't read configurations, error %d\n", err);
        goto fail;
    }

    DMSG_PANIC("******usb_new_device test1******\n");


    /* read the standard strings and cache them if present */
    get_string(udev, &udev->product, udev->descriptor.iProduct);
    get_string(udev, &udev->manufacturer, udev->descriptor.iManufacturer);
    get_string(udev, &udev->serial, udev->descriptor.iSerialNumber);

    /* choose a configuration, end set configuration */
    c = choose_configuration(udev);

    DMSG_PANIC("******usb_new_device test2******\n");


    err = usb_set_configuration(udev, c);

    DMSG_PANIC("******usb_new_device test3******\n");


    if (err)
    {
        DMSG_PANIC( "can't set config #%d, error %d\n", c, err);
        goto fail;
    }

    return 0;

fail:
    usb_set_device_state(udev, USB_STATE_NOTATTACHED);

    return err;
}

static u32  highspeed_hubs;

static void check_highspeed (struct usb_hub *hub, struct usb_host_virt_dev  *udev, int port1)
{
    struct usb_qualifier_descriptor	*qual = NULL;
    int	status = 0;

    qual = USB_OS_MALLOC(sizeof (struct usb_qualifier_descriptor), USB_MEM_FILE_TRIGGER , USB_MEM_LINE_TRIGGER );
    if (qual == NULL)
    {
        DMSG_PANIC("ERR: malloc failed\n");
        return;
    }
    USB_OS_MEMSET(qual, 0, sizeof (struct usb_qualifier_descriptor));

    status = usb_get_descriptor (udev, USB_DT_DEVICE_QUALIFIER, 0, (void *)qual, sizeof (struct usb_qualifier_descriptor));
    if (status == sizeof (struct usb_qualifier_descriptor))
    {
        DMSG_INFO("[hub]: not running at top speed; connect to a high speed hub\n");
        /* hub LEDs are probably harder to miss than syslog */
        /*
        	if (hub->has_indicators) {
        		hub->indicator[port1-1] = INDICATOR_GREEN_BLINK;
        		schedule_work (&hub->leds);
        	}
        */
    }

    USB_OS_FREE((void *)qual);
    qual = NULL;

    return;
}


static u32 hub_power_remaining (struct usb_hub *hub)
{
    struct usb_host_virt_dev *hdev = hub->hdev;
    int remaining;
    u32  i;

    remaining = hub->power_budget;
    if (!remaining)		/* self-powered */
        return 0;

    for (i = 0; i < hdev->maxchild; i++)
    {
        struct usb_host_virt_dev	*udev = hdev->children[i];
        int	delta, ceiling;

        if (!udev)
        {
            continue;
        }

        /* 100mA per-port ceiling, or 8mA for OTG ports */
        if (i != (udev->bus->otg_port - 1) || hdev->parent)
        {
            ceiling = 50;
        }
        else
        {
            ceiling = 4;
        }

        if (udev->actconfig)
        {
            delta = udev->actconfig->desc.bMaxPower;
        }
        else
        {
            delta = ceiling;
        }

        if (delta > ceiling)
        {
            DMSG_INFO("[hub]: %dmA over %dmA budget!\n", 2 * (delta - ceiling), 2 * ceiling);
        }

        remaining -= delta;
    }

    if (remaining < 0)
    {
        DMSG_INFO("[hub]: %dmA over power budget!\n", -2 * remaining);
        remaining = 0;
    }

    return remaining;
}

static void ep0_reinit(struct usb_host_virt_dev *udev)
{
    usb_disable_endpoint(udev, 0 + USB_DIR_IN);
    usb_disable_endpoint(udev, 0 + USB_DIR_OUT);
    udev->ep_in[0] = udev->ep_out[0] = &udev->ep0;
}

/* Handle physical or logical connection change events.
 * This routine is called when:
 * 	a port connection-change occurs;
 *	a port enable-change occurs (often caused by EMI);
 *	usb_reset_device() encounters changed descriptors (as from
 *		a firmware download)
 * caller already locked the hub
 */
static void hub_port_connect_change(struct usb_hub *hub, u32 port1, u16 portstatus, u16 portchange)
{
    struct usb_host_virt_dev *hdev = hub->hdev;
    __cpu_sr sr;
    u8 err = 0;
    int status = 0, i = 0;

#if	0
    if (hub->has_indicators)
    {
        set_port_led(hub, port1, HUB_LED_AUTO);
        hub->indicator[port1 - 1] = INDICATOR_AUTO;
    }
#endif

    //--<1>--先断开该port下的设备
    /* Disconnect any existing devices under this port */
    if (hdev->children[port1 - 1])
    {
        usb_disconnect(&hdev->children[port1 - 1]);
    }
    usb_clear_bit32(port1, hub->change_bits);

#ifdef	CONFIG_USB_OTG
    /* during HNP, don't repeat the debounce */
    if (hdev->bus->is_b_host)
    {
        portchange &= ~USB_PORT_STAT_C_CONNECTION;
    }
#endif

    //--<2>--如果是连接改变，则软件debounce
    if (portchange & USB_PORT_STAT_C_CONNECTION)
    {
        status = hub_port_debounce(hub, port1);
        if (status < 0)
        {
            DMSG_PANIC ("connect-debounce failed, port %d disabled\n", port1);
            goto done;
        }

        portstatus = status;
    }

    // Return now if nothing is connected
    if (!(portstatus & USB_PORT_STAT_CONNECTION))
    {
        /* maybe switch power back on (e.g. root hub was reset) */
        if ((hub->descriptor->wHubCharacteristics & HUB_CHAR_LPSM) < 2
                && !(portstatus & (1 << USB_PORT_FEAT_POWER)))
        {
            set_port_feature(hdev, port1, USB_PORT_FEAT_POWER);
        }

        if (portstatus & USB_PORT_STAT_ENABLE)
        {
            goto done;
        }

        return;
    }

#ifdef  CONFIG_USB_SUSPEND
    /* If something is connected, but the port is suspended, wake it up. */
    if (portstatus & USB_PORT_STAT_SUSPEND)
    {
        status = hub_port_resume(hub, port1, NULL);
        if (status < 0)
        {
            DMSG_PANIC("can't clear suspend on port %d; %d\n", port1, status);
            goto done;
        }
    }
#endif

    for (i = 0; i < SET_CONFIG_TRIES; i++)
    {
        struct usb_host_virt_dev *udev = NULL;

        /* reallocate for each attempt, since references
         * to the previous one can escape in various ways
         */
        udev = usb_host_alloc_virt_dev(hdev, hdev->bus, port1);
        if (!udev)
        {
            DMSG_PANIC ("couldn't allocate port %d usb_device\n", port1);
            goto done;
        }

        usb_set_device_state(udev, USB_STATE_POWERED);
        udev->speed = USB_SPEED_UNKNOWN;
        /*
        		//--<3_1>--选地址
        		choose_address(udev);
        		if (udev->devnum <= 0) {
        			status = -ENOTCONN;	// Don't retry
        			goto loop;
        		}
        */
        //--<3_2>--reset,set addr,get device desc
        /* reset and get descriptor */
        status = hub_port_init(hub, udev, port1, i);
        if (status < 0)
        {
            DMSG_PANIC("ERR: hub_port_init failed, retry....\n");
            goto loop;
        }

        /* consecutive bus-powered hubs aren't reliable; they can
         * violate the voltage drop budget.  if the new child has
         * a "powered" LED, users should notice we didn't enable it
         * (without reading syslog), even without per-port LEDs
         * on the parent.
         */
#if	0
        //如果是hub设备，则特殊处理
        if (udev->descriptor.bDeviceClass == USB_CLASS_HUB
                && hub->power_budget)
        {
            u16	devstat;

            status = usb_get_status(udev, USB_RECIP_DEVICE, 0,
                                    &devstat);
            if (status < 0)
            {
                DMSG_INFO("[hub]: get status %d ?\n", status);
                goto loop_disable;
            }
            cpu_to_le16s(&devstat);
            if ((devstat & (1 << USB_DEVICE_SELF_POWERED)) == 0)
            {
                DMSG_PANIC("can't connect bus-powered hub to this port\n");
                if (hub->has_indicators)
                {
                    hub->indicator[port1 - 1] =
                        INDICATOR_AMBER_BLINK;
                    schedule_work (&hub->leds);
                }
                status = -ENOTCONN;	/* Don't retry */
                goto loop_disable;
            }
        }
#endif
        /* check for devices running slower than they could */
        if (le16_to_cpu(udev->descriptor.bcdUSB) >= 0x0200
                && udev->speed == USB_SPEED_FULL
                && highspeed_hubs != 0)
        {
            check_highspeed (hub, udev, port1);
        }

        /* Store the parent's children[] pointer.  At this point
         * udev becomes globally accessible, although presumably
         * no one will look at it until hdev is unlocked.
         */
        USB_OS_SEMI_PEND(udev->usb_virt_dev_semi, 0, &err);
        status = 0;

        /* We mustn't add new devices if the parent hub has
         * been disconnected; we would race with the
         * recursively_mark_NOTATTACHED() routine.
         */
        USB_OS_ENTER_CRITICAL(sr);
        if (hdev->state == USB_STATE_NOTATTACHED)
        {
            status = -ENOTCONN;
        }
        else
        {
            hdev->children[port1 - 1] = udev;
        }
        USB_OS_EXIT_CRITICAL(sr);



        //--<3_4>--完整的获取desc，并向usb_virt_bus注册本usb_host_virt_dev
        //这个暂时不注册了，因为无实际价值，
        /* Run it through the hoops (find a driver, etc) */
        if(!status)
        {
            status = usb_new_device(udev);
            if (status)
            {
                USB_OS_ENTER_CRITICAL(sr);
                hdev->children[port1 - 1] = NULL;
                USB_OS_EXIT_CRITICAL(sr);
            }
        }


        USB_OS_SEMI_POST(udev->usb_virt_dev_semi);
        if (status)
        {
            goto loop_disable;
        }

        status = hub_power_remaining(hub);
        if (status)
        {
            DMSG_INFO("[hub]: %dmA power budget left\n", 2 * status);
        }

        return;

loop_disable:
        /*临时删除,不禁止该port
        	hub_port_disable(hub, port1, 1);
        */
loop:
        ep0_reinit(udev);
        //		release_address(udev);

        if(udev)
        {
            usb_host_free_virt_dev(udev);
            udev = NULL;
        }

        if (status == -ENOTCONN)
        {
            break;
        }
    }

done:
    /*临时删除,不禁止该port
    hub_port_disable(hub, port1, 1);
    */

    return;
}

static int config_descriptors_changed(struct usb_host_virt_dev *udev)
{
    u32			index;
    u32			len = 0;
    struct usb_config_descriptor	*buf = NULL;

    for (index = 0; index < udev->descriptor.bNumConfigurations; index++)
    {
        if (len < le16_to_cpu(udev->config[index].desc.wTotalLength))
        {
            len = le16_to_cpu(udev->config[index].desc.wTotalLength);
        }
    }

    buf = USB_OS_MALLOC(len, USB_MEM_FILE_TRIGGER , USB_MEM_LINE_TRIGGER );
    if (buf == NULL)
    {
        DMSG_PANIC("PANIC : no mem to re-read configs after reset\n");
        /* assume the worst */
        return 1;
    }
    USB_OS_MEMSET(buf, 0, len);

    for (index = 0; index < udev->descriptor.bNumConfigurations; index++)
    {
        int length = 0;
        int old_length = le16_to_cpu(udev->config[index].desc.wTotalLength);

        length = usb_get_descriptor(udev, USB_DT_CONFIG, index, (void *)buf, old_length);
        if (length < old_length)
        {
            DMSG_INFO("[hub]:  config index %d, error %d\n", index, length);
            break;
        }

        if (USB_OS_MEM_CMP ((void *)buf, (void *)(udev->rawdescriptors[index]), old_length) != EPDK_TRUE)
        {
            DMSG_INFO("[hub]: config index %d changed (#%d)\n", index, buf->bConfigurationValue);
            break;
        }
    }

    USB_OS_FREE((void *)buf);
    buf = NULL;

    return index != udev->descriptor.bNumConfigurations;
}

/* caller has locked the hub device */
static void hub_post_reset(struct usb_hub *hub)
{
    hub_activate(hub);
    hub_power_on(hub);
}

/*
 * Disable a port and mark a logical connnect-change event, so that some
 * time later khubd will disconnect() any existing usb_device on the port
 * and will re-enumerate if there actually is a device attached.
 */
static void hub_port_logical_disconnect(struct usb_hub *hub, int port1)
{
    if(hub == NULL)
    {
        DMSG_PANIC("ERR: invalid argment\n");
        return ;
    }

    hub_port_disable(hub, port1, 1);

    /* FIXME let caller ask to power down the port:
     *  - some devices won't enumerate without a VBUS power cycle
     *  - SRP saves power that way
     *  - usb_suspend_device(dev, PMSG_SUSPEND)
     * That's easy if this hub can switch power per-port, and
     * khubd reactivates the port later (timer, SRP, etc).
     * Powerdown must be optional, because of reset/DFU.
     */

    usb_atomic_set_bit(port1, hub->change_bits);
    kick_khubd(hub);
}

void usb_enable_root_hub_irq (struct usb_virt_bus *bus)
{
    struct hc_gen_dev *hcd = NULL;

    hcd = bus->point_gen_hcd;
    if (hcd->driver->hub_irq_enable && !hcd->poll_rh &&
            hcd->state != HC_GEN_DEV_STATE_HALT)
    {
        hcd->driver->hub_irq_enable (hcd);
    }
}

void usb_root_hub_notify (struct usb_virt_bus *bus , u32 hub_event)
{
    struct hc_gen_dev *hcd = NULL;

    hcd = bus->point_gen_hcd;
    if (hcd->driver->hub_notify && !hcd->poll_rh &&
            hcd->state != HC_GEN_DEV_STATE_HALT)
    {
        hcd->driver->hub_notify (hcd , hub_event);
    }
}

static void hub_events(u32 flag)
{
    struct list_head *tmp = NULL;
    struct usb_host_virt_dev *hdev = NULL;
    struct usb_hub *hub = NULL;

    u16 hubstatus  = 0;
    u16 hubchange  = 0;
    u16 portstatus = 0;
    u16 portchange = 0;
    int i = 0, ret = 0;
    int connect_change = 0;

    /*
     *  We restart the list every time to avoid a deadlock with
     * deleting hubs downstream from this one. This should be
     * safe since we delete the hub from the event list.
     * Not the most efficient, but avoids deadlocks.
     */
    while (1)
    {

        DMSG_HUB_TEST("hub_events--1--\n");

        //--<1>--退出
        if(flag == USB_HUB_THREAD_EXIT_FLAG_TRUE)
        {
            DMSG_PANIC("hub thread is exit, hub_events must exit\n");
            list_destroy_whole_list(&hub_event_list);
            break;
        }

        //--<2>--从hub_event_list里取出设备
        USB_OS_ENTER_CRITICAL(hub_event_lock);
        if (list_empty(&hub_event_list))
        {
            USB_OS_EXIT_CRITICAL(hub_event_lock);
            break;
        }

        /* delete the current hub event from list */
        tmp = hub_event_list.next;
        hub = (struct usb_hub *)list_head_unlink_and_del(tmp);
        if(hub == NULL)
        {
            DMSG_PANIC("ERR: hub == NULL\n");
            USB_OS_EXIT_CRITICAL(hub_event_lock);
            break;
        }

        hdev = hub->hdev;
        if(hdev == NULL)
        {
            DMSG_PANIC("ERR: hdev == NULL\n");
            USB_OS_EXIT_CRITICAL(hub_event_lock);
            break;
        }

        i = hub->resume_root_hub;
        USB_OS_EXIT_CRITICAL(hub_event_lock);

        DMSG_HUB_TEST("hub_events--2--\n");


        //--<3>--唤醒hub上的设备
        if(i)
        {
            usb_resume_device(hdev);
        }

        DMSG_HUB_TEST("hub_events--3--\n");

        /* If the hub has died, clean up after it */
        if (hdev->state == USB_STATE_NOTATTACHED)
        {
            hub_pre_reset(hub);
            goto loop;
        }

        DMSG_HUB_TEST("hub_events--4--\n");

        /* If this is an inactive or suspended hub, do nothing */
        if (hub->quiescing)
        {
            goto loop;
        }

        DMSG_HUB_TEST("hub_events--5--\n");

        if (hub->error)
        {
            ret = usb_reset_device(hdev);
            if (ret)
            {
                DMSG_INFO("[hub]: error resetting hub: %d\n", ret);
                goto loop;
            }

            hub->nerrors = 0;
            hub->error = 0;
        }

        DMSG_HUB_TEST("hub_events--6--\n");

        //--<4>--遍历各个port，处理各个port的status change
        for (i = 1; i <= hub->descriptor->bNbrPorts; i++)
        {
            if(flag == USB_HUB_THREAD_EXIT_FLAG_TRUE)
            {
                DMSG_PANIC("hub_events must exit\n");
                goto loop;
            }

            DMSG_HUB_TEST("hub_events--6-1--\n");

            if (usb_atomic_test_bit(i, hub->busy_bits))
            {
                continue;
            }

            connect_change = usb_atomic_test_bit(i, hub->change_bits);
            if (!usb_atomic_test_and_clear_bit(i, hub->event_bits) &&
                    !connect_change && !hub->activating)
            {
                continue;
            }

            DMSG_HUB_TEST("hub_events--6-2--\n");

            //获得port state
            ret = hub_port_status(hub, i, &portstatus, &portchange);
            if (ret < 0)
            {
                continue;
            }

            if (hub->activating && !hdev->children[i - 1]
                    && (portstatus & USB_PORT_STAT_CONNECTION))
            {
                connect_change = 1;
            }

            DMSG_HUB_TEST("hub_events--6-3--\n");

            //清除一些state
            if (portchange & USB_PORT_STAT_C_CONNECTION)
            {
                clear_port_feature(hdev, i, USB_PORT_FEAT_C_CONNECTION);
                connect_change = 1;
            }

            DMSG_HUB_TEST("hub_events--6-4--\n");

            if (portchange & USB_PORT_STAT_C_ENABLE)
            {
                if (!connect_change)
                {
                    DMSG_INFO("[hub]: port %d enable change, status %x\n", i, portstatus);
                }

                clear_port_feature(hdev, i, USB_PORT_FEAT_C_ENABLE);

                DMSG_HUB_TEST("hub_events--6-5--\n");

                /*
                 * EM interference sometimes causes badly
                 * shielded USB devices to be shutdown by
                 * the hub, this hack enables them again.
                 * Works at least with mouse driver.
                 */
                if (!(portstatus & USB_PORT_STAT_ENABLE)
                        && !connect_change
                        && hdev->children[i - 1])
                {
                    DMSG_PANIC ("[hub]: port %i "
                                "disabled by hub (EMI?), "
                                "re-enabling...\n",
                                i);
                    connect_change = 1;
                }
            }

            DMSG_HUB_TEST("hub_events--6-6--\n");

            if (portchange & USB_PORT_STAT_C_SUSPEND)
            {
                clear_port_feature(hdev, i, USB_PORT_FEAT_C_SUSPEND);
                if (hdev->children[i - 1])
                {
                    ret = remote_wakeup(hdev->children[i - 1]);
                    if (ret < 0)
                    {
                        connect_change = 1;
                    }
                }
                else
                {
                    ret = -ENODEV;
                    hub_port_disable(hub, i, 1);
                }
            }

            DMSG_HUB_TEST("hub_events--6-7--\n");

            if (portchange & USB_PORT_STAT_C_OVERCURRENT)
            {
                DMSG_PANIC("over-current change on port %d\n", i);
                clear_port_feature(hdev, i, USB_PORT_FEAT_C_OVER_CURRENT);
                hub_power_on(hub);
            }

            DMSG_HUB_TEST("hub_events--6-8--\n");

            if (portchange & USB_PORT_STAT_C_RESET)
            {
                DMSG_INFO("[hub]: reset change on port %d\n", i);
                clear_port_feature(hdev, i, USB_PORT_FEAT_C_RESET);
            }

            DMSG_HUB_TEST("hub_events--6-9--\n");

            if(flag == USB_HUB_THREAD_EXIT_FLAG_TRUE)
            {
                DMSG_PANIC("hub_events must exit\n");
                goto loop;
            }

            //处理port connect chang状态
            if (connect_change)
            {
                hub_port_connect_change(hub, i, portstatus, portchange);
            }

            DMSG_HUB_TEST("hub_events--6-10--\n");
        } /* end for i */

        DMSG_HUB_TEST("hub_events--7--\n");


        /* deal with hub status changes */
        if (usb_atomic_test_and_clear_bit(0, hub->event_bits) == 0)
            ;	/* do nothing */
        else if (hub_hub_status(hub, &hubstatus, &hubchange) < 0)
        {
            DMSG_PANIC("get_hub_status failed\n");
        }
        else
        {
            if (hubchange & HUB_CHANGE_LOCAL_POWER)
            {
                DMSG_INFO("[hub]: power change\n");
                clear_hub_feature(hdev, C_HUB_LOCAL_POWER);
            }
            if (hubchange & HUB_CHANGE_OVERCURRENT)
            {
                DMSG_INFO("[hub]: overcurrent change\n");
                USB_OS_DELAY_x_MS(500);	/* Cool down */
                clear_hub_feature(hdev, C_HUB_OVER_CURRENT);
                hub_power_on(hub);
            }
        }

        DMSG_HUB_TEST("hub_events--8--\n");


        hub->activating = 0;

        /* If this is a root hub, tell the HCD it's okay to
         * re-enable port-change interrupts now. */
        if (!hdev->parent)
        {
            usb_enable_root_hub_irq(hdev->bus);
        }

        DMSG_HUB_TEST("hub_events--9--\n");


loop:
        usb_unlock_device(hdev);

        usb_root_hub_notify(hdev->bus , HUB_EVNET_PLUGIN_COMPLETE);
    } /* end while (1) */
}


/**
 * usb_reset_device - perform a USB port reset to reinitialize a device
 * @udev: device to reset (not in SUSPENDED or NOTATTACHED state)
 *
 * WARNING - don't reset any device unless drivers for all of its
 * interfaces are expecting that reset!  Maybe some driver->reset()
 * method should eventually help ensure sufficient cooperation.
 *
 * Do a port reset, reassign the device's address, and establish its
 * former operating configuration.  If the reset fails, or the device's
 * descriptors change from their values before the reset, or the original
 * configuration and altsettings cannot be restored, a flag will be set
 * telling khubd to pretend the device has been disconnected and then
 * re-connected.  All drivers will be unbound, and the device will be
 * re-enumerated and probed all over again.
 *
 * Returns 0 if the reset succeeded, -ENODEV if the device has been
 * flagged for logical disconnection, or some other negative error code
 * if the reset wasn't even attempted.
 *
 * The caller must own the device lock.  For example, it's safe to use
 * this from a driver probe() routine after downloading new firmware.
 * For calls that might not occur during probe(), drivers should lock
 * the device using usb_lock_device_for_reset().
 */
int usb_reset_device(struct usb_host_virt_dev *udev)
{
    struct usb_host_virt_dev *parent_hdev = udev->parent;
    struct usb_hub *parent_hub = NULL;
    struct usb_device_descriptor descriptor = udev->descriptor;
    struct usb_hub *hub = NULL;
    int i, ret = 0, port1 = -1;

    if (udev->state == USB_STATE_NOTATTACHED || udev->state == USB_STATE_SUSPENDED)
    {
        DMSG_PANIC( "device reset not allowed in state %d\n", udev->state);
        return -EINVAL;
    }

    DMSG_HUB_TEST("usb reset device, manufacturer = %s, product = %s, serial = %s\n",
                  udev->manufacturer, udev->product, udev->serial);

    if (!parent_hdev)
    {
        /* this requires hcd-specific logic; see OHCI hc_restart() */
        DMSG_PANIC("usb_reset_device for root hub!\n");
        return -EISDIR;
    }

    for (i = 0; i < parent_hdev->maxchild; i++)
    {
        if (parent_hdev->children[i] == udev)
        {
            port1 = i + 1;
            break;
        }
    }

    if (port1 < 0)
    {
        /* If this ever happens, it's very bad */
        DMSG_PANIC( "Can't locate device's port!\n");
        return -ENOENT;
    }
    parent_hub = hdev_to_hub(parent_hdev);

    /* If we're resetting an active hub, take some special actions */
    if(!udev->actconfig
            || !(udev->actconfig->interfac[0])
            || !(udev->actconfig->interfac[0]->virt_sub_dev))
    {
        DMSG_PANIC( "PANIC : Can't access func_drv from virt_dev\n");
        return -ENOENT;
    }

    if (udev->actconfig
            && udev->actconfig->interfac[0]->virt_sub_dev->func_drv == (&hub_driver)
            && (hub = hdev_to_hub(udev)) != NULL)
    {
        hub_pre_reset(hub);
    }

    usb_atomic_set_bit(port1, parent_hub->busy_bits);
    for (i = 0; i < SET_CONFIG_TRIES; ++i)
    {

        /* ep0 maxpacket size may change; let the HCD know about it.
         * Other endpoints will be handled by re-enumeration. */
        ep0_reinit(udev);
        ret = hub_port_init(parent_hub, udev, port1, i);
        if (ret >= 0)
            break;
    }

    usb_atomic_clear_bit(port1, parent_hub->busy_bits);
    if (ret < 0)
        goto re_enumerate;

    /* Device might have changed firmware (DFU or similar) */
    if (USB_OS_MEM_CMP((void *)(&udev->descriptor), (void *)(&descriptor), sizeof(struct usb_device_descriptor)) != EPDK_TRUE
            || config_descriptors_changed (udev))
    {
        DMSG_PANIC("Wrn: device firmware changed\n");
        udev->descriptor = descriptor;	/* for disconnect() calls */
        goto re_enumerate;
    }

    if (!udev->actconfig)
        goto done;

    ret = usb_control_msg(udev,
                          usb_sndctrlpipe(udev, 0),
                          USB_REQ_SET_CONFIGURATION,
                          0,
                          udev->actconfig->desc.bConfigurationValue,
                          0,
                          NULL,
                          0,
                          USB_CTRL_SET_TIMEOUT);
    if (ret < 0)
    {
        DMSG_PANIC("PANIC :  can't restore configuration #%d (error=%d)\n",
                   udev->actconfig->desc.bConfigurationValue, ret);
        goto re_enumerate;
    }

    usb_set_device_state(udev, USB_STATE_CONFIGURED);

    for (i = 0; i < udev->actconfig->desc.bNumInterfaces; i++)
    {
        struct usb_interface *intf = udev->actconfig->interfac[i];
        struct usb_interface_descriptor *desc;

        /* set_interface resets host side toggle even
         * for altsetting zero.  the interface may have no driver.
         */
        desc = &intf->cur_altsetting->desc;
        ret = usb_set_interface(udev, desc->bInterfaceNumber,
                                desc->bAlternateSetting);
        if (ret < 0)
        {
            DMSG_PANIC("PANIC :  failed to restore interface %d "
                       "altsetting %d (error=%d)\n",
                       desc->bInterfaceNumber,
                       desc->bAlternateSetting,
                       ret);
            goto re_enumerate;
        }
    }

done:
    if (hub)
        hub_post_reset(hub);
    return 0;

re_enumerate:
    hub_port_logical_disconnect(parent_hub, port1);
    return -ENODEV;
}



/**
 * usb_set_device_state - change a device's current state (usbcore, hcds)
 * @udev: pointer to device whose state should be changed
 * @new_state: new state value to be stored
 *
 * udev->state is _not_ fully protected by the device lock.  Although
 * most transitions are made only while holding the lock, the state can
 * can change to USB_STATE_NOTATTACHED at almost any time.  This
 * is so that devices can be marked as disconnected as soon as possible,
 * without having to wait for any semaphores to be released.  As a result,
 * all changes to any device's state must be protected by the
 * device_state_lock spinlock.
 *
 * Once a device has been added to the device tree, all changes to its state
 * should be made using this routine.  The state should _not_ be set directly.
 *
 * If udev->state is already USB_STATE_NOTATTACHED then no change is made.
 * Otherwise udev->state is set to new_state, and if new_state is
 * USB_STATE_NOTATTACHED then all of udev's descendants' states are also set
 * to USB_STATE_NOTATTACHED.
 */
void usb_set_device_state(struct usb_host_virt_dev *udev,
                          enum usb_device_state new_state)
{
    u32 flags;

    USB_OS_ENTER_CRITICAL(flags);
    if (udev->state == USB_STATE_NOTATTACHED)
        ;	/* do nothing */
    else if (new_state != USB_STATE_NOTATTACHED)
        udev->state = new_state;
    else
        recursively_mark_NOTATTACHED(udev);
    USB_OS_EXIT_CRITICAL( flags);
}


/**
 * usb_disable_endpoint -- Disable an endpoint by address
 * @dev: the device whose endpoint is being disabled
 * @epaddr: the endpoint's address.  Endpoint number for output,
 *	endpoint number + USB_DIR_IN for input
 *
 * Deallocates hcd/hardware state for this endpoint ... and nukes all
 * pending urbs.
 *
 * If the HCD hasn't registered a disable() function, this sets the
 * endpoint's maxpacket size to 0 to prevent further submissions.
 */
void usb_disable_endpoint(struct usb_host_virt_dev *dev, unsigned int epaddr)
{
    unsigned int epnum = epaddr & USB_ENDPOINT_NUMBER_MASK;
    struct usb_host_virt_endpoint *ep;

    if (!dev)
    {
        DMSG_PANIC("PANIC : usb disable ep: input dev == NULL\n");
        return;
    }

    if(usb_endpoint_out(epaddr))
    {
        ep = dev->ep_out[epnum];
        dev->ep_out[epnum] = NULL;
    }
    else
    {
        ep = dev->ep_in[epnum];
        dev->ep_in[epnum] = NULL;
    }

    if(ep == NULL)
    {
        DMSG_WRN("WARNING : usb disable ep: ep == NULL\n");
        return;
    }

    hcd_ops_endpoint_disable(dev, ep);
}
/*
 * usb_disable_device - Disable all the endpoints for a USB device
 * @dev: the device whose endpoints are being disabled
 * @skip_ep0: 0 to disable endpoint 0, 1 to skip it.
 *
 * Disables all the device's endpoints, potentially including endpoint 0.
 * Deallocates hcd/hardware state for the endpoints (nuking all or most
 * pending urbs) and usbcore state for the interfaces, so that usbcore
 * must usb_set_configuration() before any interfaces could be used.
 */
void usb_disable_device(struct usb_host_virt_dev *dev, int skip_ep0)

{
    int i;

    for (i = skip_ep0; i < 16; ++i)
    {
        usb_disable_endpoint(dev, i);
        usb_disable_endpoint(dev, i + USB_DIR_IN);
    }
    dev->toggle[0] = dev->toggle[1] = 0;

    /* getting rid of interfaces will disconnect
     * any drivers bound to them (a key side effect)
     */
    //一个接口，对应一个逻辑device
    if (dev->actconfig)
    {
        for (i = 0; i < dev->actconfig->desc.bNumInterfaces; i++)
        {
            struct usb_interface	*my_interface = NULL;

            /* remove this interface if it has been registered */
            my_interface = dev->actconfig->interfac[i];
            if(my_interface)
            {
                if(my_interface->cur_altsetting->string)
                {
                    USB_OS_FREE(my_interface->cur_altsetting->string);
                    my_interface->cur_altsetting->string = NULL;
                }
                else
                {
                    DMSG_PANIC("ERR: parameter is NULL, can't free\n");
                }

                if(my_interface->virt_sub_dev)
                {
                    usb_virt_bus_dev_del(my_interface->virt_sub_dev);
                }
                else
                {
                    DMSG_PANIC("ERR: disable dev : my_interface->virt_sub_dev == NULL\n");
                }
            }
        }

        /* Now that the interfaces are unbound, nobody should
         * try to access them.
         */
        for (i = 0; i < dev->actconfig->desc.bNumInterfaces; i++)
        {
            if(dev->actconfig->interfac[i])
            {
                USB_OS_FREE(dev->actconfig->interfac[i]);
                dev->actconfig->interfac[i] = NULL;
            }
            else
            {
                DMSG_PANIC("ERR: parameter is NULL, can't free\n");
            }
        }

        dev->actconfig = NULL;
        if (dev->state == USB_STATE_CONFIGURED)
        {
            usb_set_device_state(dev, USB_STATE_ADDRESS);
        }
    }
}




/**
 * usb_disconnect - disconnect a device (usbcore-internal)
 * @pdev: pointer to device being disconnected
 * Context: !in_interrupt ()
 *
 * Something got disconnected. Get rid of it and all of its children.
 *
 * If *pdev is a normal device then the parent hub must already be locked.
 * If *pdev is a root hub then this routine will acquire the
 * usb_bus_list_lock on behalf of the caller.
 *
 * Only hub drivers (including virtual root hub drivers for host
 * controllers) should ever call this.
 *
 * This call is synchronous, and may not be used in an interrupt context.
 */
/*
***********************************************************
*
* Description	:
*				disconnect a device (usbcore-internal)
* Arguments  	:
*	@pdev		:
* Returns    	:
*
* Notes      	:
*
***********************************************************
*/
void usb_disconnect(struct usb_host_virt_dev **pdev)
{
    struct usb_host_virt_dev	*udev = *pdev;
    int			i;
    u32 sr;
    u8 err;
    if (!udev)
    {
        DMSG_PANIC ("usb_disconnect() input == NULL \n");
        return;
    }

    /* mark the device as inactive, so any further urb submissions for
     * this device (and any of its children) will fail immediately.
     * this quiesces everyting except pending urbs.
     */
    usb_set_device_state(udev, USB_STATE_NOTATTACHED);

    /* lock the bus list on behalf of HCDs unregistering their root hubs */
    if (!udev->parent)
    {
        //	down(&usb_bus_list_lock);
        usb_lock_device(udev);
    }
    else
    {
        USB_OS_SEMI_PEND(udev->usb_virt_dev_semi, 0, &err);
    }

    DMSG_INFO("[hub]: USB disconnect, address %d\n", udev->devnum);

    /* Free up all the children before we remove this device */
    for (i = 0; i < USB_MAXCHILDREN; i++)
    {
        if (udev->children[i])
            usb_disconnect(&udev->children[i]);
    }

    /* deallocate hcd/hardware state ... nuking all pending urbs and
     * cleaning up all state associated with the current configuration
     * so that the hardware is now fully quiesced.
     */
    //--<1>--会删除device结构
    usb_disable_device(udev, 0);

    /* Free the device number, remove the /proc/bus/usb entry and
     * the sysfs attributes, and delete the parent's children[]
     * (or root_hub) pointer.
     */
    //回收地址
    release_address(udev);

    /* Avoid races with recursively_mark_NOTATTACHED() */
    USB_OS_ENTER_CRITICAL(sr);
    *pdev = NULL;
    USB_OS_EXIT_CRITICAL(sr);

    if (!udev->parent)
    {
        usb_unlock_device(udev);
        //up(&usb_bus_list_lock);
    }
    else
    {
        USB_OS_SEMI_POST(udev->usb_virt_dev_semi);
    }

    usb_host_free_virt_dev(udev);
}


/*
***********************************************************
*
* Description	:
*
* Arguments  	:
*
* Returns    	:
*
* Notes      	:
*				caller has locked the hub device
***********************************************************
*/
static void hub_pre_reset(struct usb_hub *hub)
{
    struct usb_host_virt_dev  *hdev = hub->hdev;
    int i;

    for (i = 0; i < hdev->maxchild; ++i)
    {
        if (hdev->children[i])
            usb_disconnect(&hdev->children[i]);
    }

    hub_quiesce(hub);
}

/*
***********************************************************
*
* Description	:
*	  用来查询hub的status的urb的complete
* Arguments  	:
*
* Returns    	:
*
* Notes      	:
*
***********************************************************
*/
static void hub_status_request(struct urb *urb)
{
    struct usb_hub *hub = (struct usb_hub *)urb->context;
    int status;
    int i;
    unsigned long bits;

    DMSG_HUB_TEST("req_complete: urb = %x, status = %d\n", urb, urb->status);

    switch (urb->status)
    {
    case -ENOENT:		/* synchronous unlink */
    case -ECONNRESET:	/* async unlink */
    case -ESHUTDOWN:	/* hardware going away */
        return;

    default:		/* presumably an error */
        /* Cause a hub reset after 10 consecutive errors */
        if ((++hub->nerrors < 10) || hub->error)
        {
            goto resubmit;
        }
        hub->error = urb->status;
    /* FALL THROUGH */

    //将事件传递给hub thread,让它来处理
    case 0:			/* we got data:  port status changed */
        bits = 0;

        for (i = 0; i < urb->actual_length; ++i)
        {
            bits |= ((unsigned long) ((hub->status_urb_ubff)[i])) << (i * 8);
        }

        hub->event_bits[0] = bits;
        break;
    }

    hub->nerrors = 0;

    //--2--wakeup hub mainthread
    kick_khubd(hub);

resubmit:
    if (hub->quiescing)
    {
        return;
    }

    hub->hub_req_busy = 1;

    //--3--继续压urb
    if ((status = usb_submit_urb (hub->urb, 0)) != 0
            && status != -ENODEV && status != -EPERM)
    {
        DMSG_PANIC( "PANIC : resubmit --> %d\n", status);
    }

    hub->hub_req_busy = 0;

    return ;
}

/* 实时获得hub的状态 */
static void HubStatusThread(void *p_arg)
{
    struct usb_hub *hub = (struct usb_hub *)p_arg;

    if(hub == NULL)
    {
        DMSG_PANIC("ERR: hub == NULL\n");
        KillThreadSelf();
        return;
    }

    USB_OS_SemPost(hub->notify_complete);

    while(1)
    {
        TryToKillThreadSelf("HubStatusThread");

        UsbThreadSleep(hub->HubStatusSemi);

        if(hub->HubRemove == 0)
        {
            hub_status_request(hub->urb);
        }
        else
        {
            DMSG_PANIC("ERR: Hub had remove, can not work\n");
        }
    }
}

static void hub_status_req_complete(struct urb *urb)
{
    struct usb_hub *hub = (struct usb_hub *)urb->context;

    DMSG_HUB_TEST("hub_status_req_complete: urb = %x\n", urb);

    if(hub->HubStatusSemi)
    {
        UsbThreadWakeUp(hub->HubStatusSemi);
    }
}

/*
***********************************************************
*
* Description	:
*
* Arguments  	:
*
* Returns    	:
*
* Notes      	:
*
***********************************************************
*/
static int _hub_config(struct usb_hub *hub, struct usb_endpoint_descriptor *endpoint)
{
    struct usb_host_virt_dev *hdev = hub->hdev;

    u16 hubstatus, hubchange;
    unsigned int pipe;
    int maxp, ret;
    char *message;

    DMSG_HUB_TEST("_hub_config--1--\n");

    hub->status = USB_OS_MALLOC(sizeof(*hub->status), USB_MEM_FILE_TRIGGER , USB_MEM_LINE_TRIGGER );
    if (!hub->status)
    {
        message = "can't kmalloc hub status buffer";
        ret = -ENOMEM;
        goto fail;
    }
    USB_OS_MEMSET(hub->status, 0, sizeof(*hub->status));

    hub->descriptor = USB_OS_MALLOC(sizeof(*hub->descriptor), USB_MEM_FILE_TRIGGER , USB_MEM_LINE_TRIGGER );
    if (!hub->descriptor)
    {
        message = "can't kmalloc hub descriptor";
        ret = -ENOMEM;
        goto fail;
    }
    USB_OS_MEMSET(hub->descriptor, 0, sizeof(*hub->descriptor));

    DMSG_HUB_TEST("_hub_config--2--\n");

    /* Request the entire hub descriptor.
     * hub->descriptor can handle USB_MAXCHILDREN ports,
     * but the hub can/will return fewer bytes here.
     */
    //--<2>--获得hub desc，并解析之
    ret = get_hub_descriptor(hdev, (void *)(hub->descriptor), sizeof(*hub->descriptor));
    if (ret < 0)
    {
        message = "can't read hub descriptor";
        goto fail;
    }
    else if (hub->descriptor->bNbrPorts > USB_MAXCHILDREN)
    {
        message = "hub has too many ports!";
        ret = -ENODEV;
        goto fail;
    }

    DMSG_HUB_TEST("_hub_config--3--\n");

    hdev->maxchild = hub->descriptor->bNbrPorts;
    DMSG_INFO("[hub]: %d port%s detected\n", hdev->maxchild, (hdev->maxchild == 1) ? "" : "s");

    hub->descriptor->wHubCharacteristics = le16_to_cpu(hub->descriptor->wHubCharacteristics);
    if (hub->descriptor->wHubCharacteristics & HUB_CHAR_COMPOUND)
    {
        int	i;
        char portstr [USB_MAXCHILDREN + 1];

        for (i = 0; i < hdev->maxchild; i++)
        {
            portstr[i] = hub->descriptor->DeviceRemovable[((i + 1) / 8)] & (1 << ((i + 1) % 8)) ? 'F' : 'R';
        }

        portstr[hdev->maxchild] = 0;
        DMSG_INFO("[usbh hub]: compound device; port removable status: %s\n", portstr);
    }
    else
    {
        DMSG_HOLD("[usbh hub]: stand alone hub\n");
    }

    DMSG_HUB_TEST("_hub_config--4--\n");

    switch (hub->descriptor->wHubCharacteristics & HUB_CHAR_LPSM)
    {
    case 0x00:
        DMSG_HOLD("[usbh hub]: ganged power switching\n");
        break;

    case 0x01:
        DMSG_HOLD("[usbh hub]: individual port power switching\n");
        break;

    case 0x02:
    case 0x03:
        DMSG_HOLD("[usbh hub]: no power switching (usb 1.0)\n");
        break;
    }

    DMSG_HUB_TEST("_hub_config--4--\n");

    switch (hub->descriptor->wHubCharacteristics & HUB_CHAR_OCPM)
    {
    case 0x00:
        DMSG_HOLD("[usbh hub]: global over-current protection\n");
        break;

    case 0x08:
        DMSG_HOLD("[usbh hub]: individual port over-current protection\n");
        break;

    case 0x10:
    case 0x18:
        DMSG_HOLD("[usbh hub]: no over-current protection\n");
        break;
    }

    DMSG_HUB_TEST("_hub_config--5--\n");

    INIT_LIST_HEAD (&hub->tt.clear_list);

    switch (hdev->descriptor.bDeviceProtocol)
    {
    case 0:
        break;

    case 1:
        hub->tt.hub = hdev;
        break;

    case 2:
        ret = usb_set_interface(hdev, 0, 1);
        if (ret == 0)
        {
            hub->tt.multi = 1;
        }
        else
            DMSG_PANIC("PANIC :Using single TT (err %d)\n", ret);
        hub->tt.hub = hdev;
        break;

    default:
        DMSG_PANIC("[usbh hub]: Unrecognized hub protocol %d\n", hdev->descriptor.bDeviceProtocol);
        break;
    }
    DMSG_HUB_TEST("_hub_config--6--\n");

    switch (hub->descriptor->wHubCharacteristics & HUB_CHAR_TTTT)
    {
    case 0x00:
        if (hdev->descriptor.bDeviceProtocol != 0)
        {
            DMSG_HOLD("[usbh hub]: TT requires at most 8 FS bit times\n");
        }
        break;

    case 0x20:
        DMSG_HOLD("[usbh hub]: TT requires at most 16 FS bit times\n");
        break;

    case 0x40:
        DMSG_HOLD("[usbh hub]: TT requires at most 24 FS bit times\n");
        break;

    case 0x60:
        DMSG_HOLD("[usbh hub]: TT requires at most 32 FS bit times\n");
        break;
    }
    DMSG_HUB_TEST("_hub_config--7--\n");

    /* probe() zeroes hub->indicator[] */
    if (hub->descriptor->wHubCharacteristics & HUB_CHAR_PORTIND)
    {
        hub->has_indicators = 1;
    }

    /* power budgeting mostly matters with bus-powered hubs,
     * and battery-powered root hubs (may provide just 8 mA).
     */
    ret = usb_get_status(hdev, USB_RECIP_DEVICE, 0, &hubstatus);
    if (ret < 0)
    {
        message = "can't get hub status";
        goto fail;
    }

    DMSG_HUB_TEST("_hub_config--8--\n");

    hubstatus = le16_to_cpu(hubstatus);
    if (hdev == hdev->bus->root_hub)
    {
        struct hc_gen_dev  *hcd = hdev->bus->hcpriv;

        hub->power_budget = USB_MIN(500u, hcd->power_budget) / 2;
    }
    else if ((hubstatus & (1 << USB_DEVICE_SELF_POWERED)) == 0)
    {
        hub->power_budget = (501 - hub->descriptor->bHubContrCurrent) / 2;
    }
    if (hub->power_budget)
    {
        DMSG_INFO("[usbh hub]: %dmA bus power budget for children\n", hub->power_budget * 2);
    }
    DMSG_HUB_TEST("_hub_config--9--\n");

    //--<4>--
    ret = hub_hub_status(hub, &hubstatus, &hubchange);
    if (ret < 0)
    {
        message = "can't get hub status";
        goto fail;
    }
    DMSG_HUB_TEST("_hub_config--10--\n");

    /* local power status reports aren't always correct */
    if (hdev->actconfig->desc.bmAttributes & USB_CONFIG_ATT_SELFPOWER)
    {
        DMSG_INFO("[usbh hub]: local power source is %s\n", (hubstatus & HUB_STATUS_LOCAL_POWER) ? "lost (inactive)" : "good");
    }

    if ((hub->descriptor->wHubCharacteristics & HUB_CHAR_OCPM) == 0)
    {
        DMSG_INFO("[usbh hub]: %sover-current condition exists\n",
                  (hubstatus & HUB_STATUS_OVERCURRENT) ? "" : "no ");
    }
    DMSG_HUB_TEST("_hub_config--11--\n");

    //--<5>--设置interrupt ep，用来获得hub port 的status
    pipe = usb_rcvintpipe(hdev, endpoint->bEndpointAddress);
    maxp = usb_maxpacket(hdev, pipe, usb_pipeout(pipe));
    if (maxp > STATUS_URB_BUFF_LEN)
    {
        maxp = STATUS_URB_BUFF_LEN;
    }
    DMSG_HUB_TEST("_hub_config--12--\n");

    hub->urb = usb_alloc_urb(0);
    if (!hub->urb)
    {
        message = "couldn't allocate interrupt urb";
        ret = -ENOMEM;
        goto fail;
    }
    DMSG_HUB_TEST("_hub_config--13--\n");

    //填充该urb
    usb_fill_int_urb(hub->urb,
                     hdev,
                     pipe,
                     hub->status_urb_ubff,
                     maxp,
                     (usb_complete_t)hub_status_req_complete,
                     hub,
                     endpoint->bInterval);

    hub->urb->transfer_dma = hub->buffer_dma;
    hub->urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

    DMSG_HUB_TEST("_hub_config--14--\n");

    /* maybe cycle the hub leds */
    if (hub->has_indicators && blinkenlights)
    {
        hub->indicator [0] = INDICATOR_CYCLE;
    }

    hub_power_on(hub);
    DMSG_HUB_TEST("_hub_config--15--\n");

    //--<6>--发送get status的urb
    hub_activate(hub);

    DMSG_HUB_TEST("_hub_config--16--\n");

    return 0;

fail:
    DMSG_PANIC("PANIC : config failed, %s (err %d)\n", message, ret);

    /* hub_disconnect() frees urb and descriptor */
    return ret;
}

/*
***********************************************************
*
* Description	:
*
* Arguments  	:
*
* Returns    	:
*
* Notes      	:
*
***********************************************************
*/
static void hub_disconnect(struct usb_interface *intf)
{
    struct usb_hub *hub = usb_mod_usb_get_intf_priv_data (intf);
    struct usb_host_virt_dev  *hdev;
    __u8 err = 0;

    if (!hub)
    {
        DMSG_PANIC("ERR: hub_disconnect, input == NULL\n");
        return;
    }

    hdev = hub->hdev;
    if(hdev == NULL)
    {
        DMSG_PANIC("ERR: hub_disconnect: hdev == NULL\n");
        return;
    }

    if (hdev->speed == USB_SPEED_HIGH)
    {
        highspeed_hubs--;
    }

    hub->HubRemove = 1;
    if(hub->hub_req_busy)
    {
        usb_unlink_urb(hub->urb);
        hub->urb->status = -ESHUTDOWN;
    }

    if(hub->HubStatusThrdId)
    {
        UsbKillThread(hub->HubStatusThrdId, hub->HubStatusSemi);
    }

    if(hub->notify_complete)
    {
        USB_OS_SemDel(hub->notify_complete, &err);
        hub->notify_complete = NULL;
    }

    if(hub->HubStatusSemi)
    {
        USB_OS_SemDel(hub->HubStatusSemi, &err);
        hub->HubStatusSemi = NULL;
    }

    usb_mod_usb_set_intf_priv_data (intf, NULL);

    hub_quiesce(hub);
    usb_free_urb(hub->urb);
    hub->urb = NULL;

    if(hub->descriptor)
    {
        USB_OS_FREE((void *)(hub->descriptor));
        hub->descriptor = NULL;
    }
    else
    {
        DMSG_PANIC("ERR: parameter is NULL, can't free\n");
    }

    if(hub->status)
    {
        USB_OS_FREE(hub->status);
        hub->status = NULL;
    }
    else
    {
        DMSG_PANIC("ERR: parameter is NULL, can't free\n");
    }

    /* Free the memory */
    USB_OS_FREE(hub);
    hub = NULL;

    return ;
}

/*
***********************************************************
*						probe函数
*
* Description	:
*
* Arguments  	:
*
* Returns    	:
*
* Notes      	:
*
***********************************************************
*/
static int hub_probe(struct usb_interface *intf, const struct usb_drv_dev_match_table *id)
{
    struct usb_host_virt_interface	*desc 		= NULL;
    struct usb_endpoint_descriptor	*endpoint	= NULL;
    struct usb_host_virt_dev		*hdev 		= NULL;
    struct usb_hub					*hub 		= NULL;
    __s32 ret = -ENODEV;
    __u8  err = 0;

    DMSG_INFO("[hub]: usb hub probe\n");

    desc = intf->cur_altsetting;
    if(desc == NULL)
    {
        DMSG_PANIC("ERR: interface current setting is NULL\n");
        return -EIO;
    }

    hdev = usb_mod_interface_to_usbdev(intf);
    if(hdev == NULL)
    {
        DMSG_PANIC("ERR: hdev is NULL\n");
        return -EIO;
    }

    /* Some hubs have a subclass of 1, which AFAICT according to the */
    /*  specs is not defined, but it works */
    if ((desc->desc.bInterfaceSubClass != 0) &&
            (desc->desc.bInterfaceSubClass != 1))
    {
descriptor_error:
        DMSG_PANIC("PANIC : hub_prob() : bad descriptor, ignoring hub\n");
        return -EIO;
    }

    /* Multiple endpoints? What kind of mutant ninja-hub is this? */
    if (desc->desc.bNumEndpoints != 1)
    {
        DMSG_PANIC("ERR: usb hub endpoint number is not 1\n");
        goto descriptor_error;
    }

    endpoint = &desc->endpoint[0].desc;

    /* Output endpoint? Curiouser and curiouser.. */
    if (!(endpoint->bEndpointAddress & USB_DIR_IN))
    {
        DMSG_PANIC("ERR: usb hub endpoint is not in endpoint\n");
        goto descriptor_error;
    }

    /* If it's not an interrupt endpoint, we'd better punt! */
    if ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) != USB_ENDPOINT_XFER_INT)
    {
        DMSG_PANIC("ERR: it's not an interrupt endpoint\n");
        goto descriptor_error;
    }

    //--<1>--
    hub = USB_OS_MALLOC(sizeof(struct usb_hub), USB_MEM_FILE_TRIGGER , USB_MEM_LINE_TRIGGER );
    if (!hub)
    {
        DMSG_PANIC("PANIC : can't kmalloc hub struct, no mem\n");
        return -ENOMEM;
    }
    USB_OS_MEMSET(hub, 0, sizeof(struct usb_hub));
    hub->hdev = hdev;

    usb_mod_usb_set_intf_priv_data (intf, hub);

    if (hdev->speed == USB_SPEED_HIGH)
    {
        highspeed_hubs++;
    }

    hub->notify_complete = USB_OS_SemCreate(0);
    if(hub->notify_complete == NULL)
    {
        DMSG_PANIC("ERR: USB_OS_SemCreate notify_complete failed\n");
        ret = -ENODEV;
        hub->notify_complete = NULL;
        goto ERR0;
    }

    hub->HubStatusSemi = USB_OS_SemCreate(0);
    if(hub->HubStatusSemi == NULL)
    {
        DMSG_PANIC("ERR: USB_OS_SemCreate notify_complete failed\n");
        ret = -ENODEV;
        hub->HubStatusSemi = NULL;
        goto ERR0;
    }

    hub->HubStatusThrdId = UsbCreateThread((void *)HubStatusThread,
                                           (void *)hub,
                                           0x1000,
                                           HUB_THREAD_PID_PRIO_LEVEL);
    if(hub->HubStatusThrdId == OS_NO_ERR)
    {
        DMSG_PANIC("ERR: create UsbCreateThread failed\n");
        ret = -1;
        hub->HubStatusThrdId = 0;
        goto ERR0;
    }

    USB_OS_SemPend(hub->notify_complete, 0, &err);

    DMSG_HUB_TEST("hub_probe--1--\n");

    //--<2>--
    if (_hub_config(hub, endpoint) >= 0)
    {
        return 0;
    }

ERR0:
    DMSG_PANIC("ERR: hub config failed\n");

    //--<3>--失败，则disconnect
    hub_disconnect(intf);

    return ret;
}

static int hub_ioctl(struct usb_interface *intf, unsigned int code, void *user_data)
{
    /*
    	struct usb_device *hdev = interface_to_usbdev (intf);

    	//assert ifno == 0 (part of hub spec)
    	switch (code) {
    	case USBDEVFS_HUB_PORTINFO: {
    		struct usbdevfs_hub_portinfo *info = user_data;
    		int i;

    		spin_lock_irq(&device_state_lock);
    		if (hdev->devnum <= 0)
    			info->nports = 0;
    		else {
    			info->nports = hdev->maxchild;
    			for (i = 0; i < info->nports; i++) {
    				if (hdev->children[i] == NULL)
    					info->port[i] = 0;
    				else
    					info->port[i] =
    						hdev->children[i]->devnum;
    			}
    		}
    		spin_unlock_irq(&device_state_lock);

    		return info->nports + 1;
    		}

    	default:
    		return -ENOSYS;
    	}
    */
    return 0;
}


static struct usb_drv_dev_match_table hub_id_table [] =
{
    {
        USB_DEVICE_ID_MATCH_DEV_CLASS	//match_flags;

        , 0	//idVendor;
        , 0	//idProduct;
        , 0	//bcdDevice_lo;
        , 0	//bcdDevice_hi;

        //Used for device class matches
        , USB_CLASS_HUB	//bDeviceClass;
        , 0	//bDeviceSubClass;
        , 0	//bDeviceProtocol;

        //Used for interface class matches
        , 0	//bInterfaceClass;
        , 0	//bInterfaceSubClass;
        , 0	//bInterfaceProtocol;

        //not matched against
        , 0	//driver_info;
    },

    {

        USB_DEVICE_ID_MATCH_INT_CLASS	//match_flags;

        , 0	//idVendor;
        , 0	//idProduct;
        , 0	//bcdDevice_lo;
        , 0	//bcdDevice_hi;

        //Used for device class matches
        , 0	//bDeviceClass;
        , 0	//bDeviceSubClass;
        , 0	//bDeviceProtocol;

        //Used for interface class matches
        , USB_CLASS_HUB	//bInterfaceClass;
        , 0	//bInterfaceSubClass;
        , 0	//bInterfaceProtocol;

        //not matched against
        , 0	//driver_info;
    },

    {
        0	//match_flags;

        , 0	//idVendor;
        , 0	//idProduct;
        , 0	//bcdDevice_lo;
        , 0	//bcdDevice_hi;

        //Used for device class matches
        , 0	//bDeviceClass;
        , 0	//bDeviceSubClass;
        , 0	//bDeviceProtocol;

        //Used for interface class matches
        , 0	//bInterfaceClass;
        , 0	//bInterfaceSubClass;
        , 0	//bInterfaceProtocol;

        //not matched against
        , 0	//driver_info;
    }/* Terminating entry */
};

/*
***********************************************************
*
* Description	:
*				初始化hub drv
* Arguments  	:
*	@hub_drv_input	:	要初始化的hub_drv_input
* Returns    	:
*
* Notes      	:
*
***********************************************************
*/
static void usb_gen_hub_func_drv_init(struct usb_host_func_drv *hub_drv_input )
{
    if( !hub_drv_input)
    {
        DMSG_PANIC("PANIC : usb_gen_hub_func_drv_init()  : input == NULL \n");
        return ;
    }

    USB_OS_MEMSET(hub_drv_input, 0, sizeof(struct usb_host_func_drv));

    INIT_LIST_HEAD(&(hub_drv_input->virt_dev_list));
    hub_drv_input->func_drv_name 	= "hub drv";
    hub_drv_input->func_drv_auther 	= "softwiner";
    hub_drv_input->probe 			= hub_probe;
    hub_drv_input->disconnect 		= hub_disconnect;
    hub_drv_input->suspend 			= hub_suspend;
    hub_drv_input->resume 			= hub_resume;
    hub_drv_input->ioctl 			= hub_ioctl;
    hub_drv_input->match_table 		= hub_id_table;
}

/*
***********************************************************
*              usb_gen_hub_func_drv_exit
*
* Description	:
*
* Arguments  	:
*	@hub_drv_input	:	要初始化的hub_drv_input
* Returns    	:
*
* Notes      	:
*
***********************************************************
*/
static void usb_gen_hub_func_drv_exit(struct usb_host_func_drv *hub_drv_input )
{
    if( !hub_drv_input)
    {
        DMSG_PANIC("PANIC : usb_gen_hub_func_drv_init()  : input == NULL \n");
        return ;
    }

    USB_OS_MEMSET(hub_drv_input, 0, sizeof(struct usb_host_func_drv));
    INIT_LIST_HEAD(&(hub_drv_input->virt_dev_list));
}

/*
***********************************************************
*
* Description	:
*				hub context的init
* Arguments  	:
*
* Returns    	:
*
* Notes      	:
*
***********************************************************
*/
static s32 __usb_gen_hub_thread_init(struct hub_thread_context *thread_cont)
{
    if( !thread_cont)
    {
        DMSG_PANIC("ERR: input == NULL \n");
        return -1;
    }
    USB_OS_MEMSET(thread_cont , 0, sizeof(struct hub_thread_context));

    thread_cont->hub_thread_exit_flag 	= USB_HUB_THREAD_EXIT_FLAG_FALSE;
    thread_cont->hub_thread_event 		= USB_OS_SEMI_CREATE(1);
    thread_cont->hub_thread_complete 	= USB_OS_COMPLETE_CREATE();

    if(thread_cont->hub_thread_event == NULL)
    {
        DMSG_PANIC("ERR: can not creat semi \n");
        return -1;
    }
    if(thread_cont->hub_thread_complete == NULL)
    {
        DMSG_PANIC("ERR: can not creat hub_thread_complete \n");
        return -1;
    }

    return 0;
}

/*
***********************************************************
*
* Description	:
*				hub context的销毁
* Arguments  	:
*
* Returns    	:
*
* Notes      	:
*
***********************************************************
*/
static s32 __usb_gen_hub_thread_exit(struct hub_thread_context *thread_cont)
{
    u8 err = 0;

    if( !thread_cont)
    {
        DMSG_PANIC("PANIC : usb_gen_hub_thread_context_exit()  : input == NULL \n");
        return -1;
    }

    //等待hub thread结束，然后再del 其内部的同步semi
    while( USB_OS_THREAD_DELREQ( hub_thread_id ) != USB_OS_THREAD_NOT_EXIST )
    {
        hub_thread_wakeup(&hub_thread_cont);
        USB_OS_TIME_DELAY_10MS(10);
    }

    //del complete
    if(thread_cont->hub_thread_complete )
    {
        USB_OS_COMPLETE_DELETE(thread_cont->hub_thread_complete  , &err);
    }

    thread_cont->hub_thread_complete = NULL;

    //del semi
    if(thread_cont->hub_thread_event )
    {
        USB_OS_SEMI_DELETE(thread_cont->hub_thread_event  , &err);
    }

    thread_cont->hub_thread_event = NULL;

    return 0;
}

__u32 hub_GetHubNo(struct usb_host_virt_dev *udev)
{
    if(udev == NULL)
    {
        DMSG_PANIC("ERR: input error\n");
        return 0;
    }

    DMSG_INFO("hub_port = %d\n", udev->hub_port);

    return udev->hub_port;
}

__u32 hub_GetHubSeries(struct usb_host_virt_dev *udev)
{
    struct usb_host_virt_dev *usb_dev = NULL;
    __u32 cnt = 0;

    if(udev == NULL)
    {
        DMSG_PANIC("ERR: input error\n");
        return 0;
    }

    usb_dev = udev;
    while(usb_dev->parent)
    {
        ++cnt;
        usb_dev = usb_dev->parent;
    }

    /* roothub不能算在其中 */
    if(cnt)
    {
        --cnt;
    }

    DMSG_INFO("cnt = %d\n", cnt);

    return cnt;
}

/*
***********************************************************
*
* Description	:
*				hub的主thread
* Arguments  	:
*
* Returns    	:
*
* Notes      	:
*
***********************************************************
*/
void hub_main_thread(void *para)
{
    do
    {
        hub_events(hub_thread_cont.hub_thread_exit_flag);
        hub_thread_sleep(&hub_thread_cont);
    }
    while(hub_thread_cont.hub_thread_exit_flag != USB_HUB_THREAD_EXIT_FLAG_TRUE );

    while(1)
    {
        if(USB_OS_THREAD_DELREQ(OS_PRIO_SELF) == USB_OS_THREAD_DEL_REQ)
        {
            USB_OS_THREAD_DELETE(OS_PRIO_SELF);
            break;
        }
        USB_OS_TIME_DELAY_10MS(3);
    }
}



/*
***********************************************************
*
* Description	:
*
* Arguments  	:
*
* Returns    	:
*
* Notes      	:
*
***********************************************************
*/
s32 usb_gen_hub_init(void)
{
    u8 ret = 0;

    if(usb_address0_sem == NULL)
    {
        usb_address0_sem = USB_OS_SEMI_CREATE(1);
    }

    hub_event_lock = NULL;
    INIT_LIST_HEAD(&hub_event_list);

    if(__usb_gen_hub_thread_init(&hub_thread_cont) != 0)
    {
        DMSG_PANIC("ERR: __usb_gen_hub_thread_init failed\n");
        return EPDK_FAIL;
    }

    //--<1>--初始化hub drv
    usb_gen_hub_func_drv_init(&hub_driver);

    //--<2>--注册hub驱动
    if(usb_host_func_drv_reg(&hub_driver) != 0)
    {
        DMSG_PANIC("ERR: usb_host_func_drv_reg failed\n");
        goto hub_failed_1;
    }

    //--<3>--开启hub thread
    ret = USB_OS_THREAD_CREATE(hub_main_thread, NULL, 0x8000, HUB_THREAD_PID_PRIO_LEVEL);
    hub_thread_id = ret;
    if(ret == OS_NO_ERR)
    {
        DMSG_PANIC("ERR: create hub main thread failed\n");
        goto hub_failed_2;
    }

    return EPDK_OK;

hub_failed_2:
    usb_host_func_drv_unreg(&hub_driver);

hub_failed_1:
    usb_gen_hub_func_drv_exit(&hub_driver);
    __usb_gen_hub_thread_exit(&hub_thread_cont);

    return EPDK_FAIL;
}

/*
***********************************************************
*
* Description	:
*				exit时候调用，用来回收资源
* Arguments  	:
*
* Returns    	:
*
* Notes      	:
*
***********************************************************
*/
s32 usb_gen_hub_exit(void)
{
    //--<1>--停止hub thread
    hub_thread_cont.hub_thread_exit_flag = USB_HUB_THREAD_EXIT_FLAG_TRUE;
    hub_thread_wakeup(&hub_thread_cont);

    //--<2>--结束hub线程
    __usb_gen_hub_thread_exit(&hub_thread_cont);

    //--<3>--卸载hub drvier
    usb_host_func_drv_unreg(&hub_driver);

    //--<4>--注销hub driver
    usb_gen_hub_func_drv_exit(&hub_driver);

    hub_thread_id = 0;
    if(usb_address0_sem )
    {
        u8 err = 0;
        USB_OS_SEMI_DELETE(usb_address0_sem, &err);
        usb_address0_sem = NULL;
    }

    return EPDK_OK;
}









