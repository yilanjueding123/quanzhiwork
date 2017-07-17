/*
*******************************************************************************
*                                            sun project ---- usb host module
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
* Date : 2008.05.xx
*
* Description :
*
* History :
********************************************************************************************************************
*/



#include "usb_host_config.h"

#include "usb_host_base_types.h"
#include "usb_os_platform.h"

#include "usb_host_common.h"

#include "usb_list.h"
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

#include "usb_gen_hub.h"
#include "usb_core_interface.h"
#include "usb_gen_hcd_rh.h"



static USB_OS_KERNEL_EVENT *usb_bus_list_lock;	//用来保护usb virt bus的，虽然目前用不到
//但是当我们支持多个物理hcd的时候必须用





/**
 * usb_release_bandwidth - reverses effect of usb_claim_bandwidth()
 * @dev: source/target of request
 * @urb: request (urb->dev == dev)
 * @isoc: true iff the request is isochronous
 *
 * This records that previously allocated bandwidth has been released.
 * Bandwidth is released when endpoints are removed from the host controller's
 * periodic schedule.
 */
/*
********************************************************************************
*                     usb_release_bandwidth
* Description:
*     释放带宽
* Arguments:
*     hcd  		: input.
*     hep  		: input.
*     urb  		: input.
*     mem_flags : input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
void usb_release_bandwidth (struct usb_host_virt_dev *dev, struct urb *urb, int isoc)
{
    dev->bus->bandwidth_allocated -= urb->bandwidth;
    if (isoc)
        dev->bus->bandwidth_isoc_reqs--;
    else
        dev->bus->bandwidth_int_reqs--;

    urb->bandwidth = 0;
}

/*
********************************************************************************
*                     usb_release_bandwidth
* Description:
*     把urb 从list中摘出来, 但是没有callback
* Arguments:
*     hcd  		: input.
*     hep  		: input.
*     urb  		: input.
*     mem_flags : input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
static void urb_unlink (struct urb *urb)
{
    __cpu_sr sr = 0;

    /* Release any periodic transfer bandwidth */
    if (urb->bandwidth)
    {
        usb_release_bandwidth (urb->dev, urb, usb_pipeisoc (urb->pipe));
    }

    /* clear all state linking urb to this dev (and hcd) */
    USB_OS_ENTER_CRITICAL(sr);
    if(urb->wrapper_urb)
    {
        list_del_init (urb->wrapper_urb);
        _list_head_free(urb->wrapper_urb);
        urb->wrapper_urb = NULL;
    }
    else
    {
        DMSG_PANIC("PANIC : urb_unlink() urb->wrapper_urb == NULL\n");
    }
    USB_OS_EXIT_CRITICAL(sr);

    return;
}

/*-------------------------------------------------------------------------*/

/**
 * usb_hcd_giveback_urb - return URB from HCD to device driver
 * @hcd: host controller returning the URB
 * @urb: urb being returned to the USB device driver.
 * @regs: pt_regs, passed down to the URB completion handler
 * Context: in_interrupt()
 *
 * This hands the URB from HCD to its USB device driver, using its
 * completion function.  The HCD has freed all per-urb resources
 * (and is done using urb->hcpriv).  It also released all HCD locks;
 * the device driver won't cause problems if it frees, modifies,
 * or resubmits this URB.
 */
/*
********************************************************************************
*                     usb_hcd_giveback_urb
* Description:
*     urb callback函数, 这里只是把urb从list里面摘出来,并且callback, 后面的事交给
* device driver处理
* Arguments:
*     hcd  		: input. host controller
*     urb  		: input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
void usb_hcd_giveback_urb (struct hc_gen_dev *hcd, struct urb *urb)
{
    //int at_root_hub;

    //at_root_hub = (urb->dev == hcd->self.root_hub);
    urb_unlink (urb);

    /* lower level hcd code should use *_dma exclusively
    if (hcd->self.controller->dma_mask && !at_root_hub) {
    	if (usb_pipecontrol (urb->pipe)
    		&& !(urb->transfer_flags & URB_NO_SETUP_DMA_MAP))
    		dma_unmap_single (hcd->self.controller, urb->setup_dma,
    				sizeof (struct usb_ctrlrequest),
    				DMA_TO_DEVICE);
    	if (urb->transfer_buffer_length != 0
    		&& !(urb->transfer_flags & URB_NO_TRANSFER_DMA_MAP))
    		dma_unmap_single (hcd->self.controller,
    				urb->transfer_dma,
    				urb->transfer_buffer_length,
    				usb_pipein (urb->pipe)
    				    ? DMA_FROM_DEVICE
    				    : DMA_TO_DEVICE);
    }
    */

    urb->complete(urb);	//如果是rh 的status urb ,这个时候的use_count = 2
    //见hub_status_req_complete()
    usb_atomic_dec32 ((&urb->use_count) );
    if (urb->reject)
    {
        DMSG_PANIC("PANIC : usb_hcd_giveback_urb: submissions will fail\n");
        //wake_up (&usb_kill_urb_queue);
    }

    /*不能在这里就将urb了解了 ，
    //这么做非常不明智，所以由urb的申请者去处理它的归宿:)
    	//当没有人使用的时候才
    	if(urb->use_count == 0){
    		usb_free_urb (urb);
    	}else{
    		DMSG_PANIC("=====usb_hcd_giveback_urb()...urb->use_count = %d != 0,maybe roothub status urb\n",urb->use_count);
    	}
    */
    return;
}

/*
********************************************************************************
*                     usb_bus_init
* Description:
*     内部函数--只是初始化usb_bus结构
* Arguments:
*     hcd  		: input.
*     hep  		: input.
*     urb  		: input.
*     mem_flags : input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
static void usb_bus_init (struct usb_virt_bus *bus)
{
    USB_OS_MEMSET(&bus->devmap, 0, sizeof(struct usb_devmap));

    bus->devnum_next 			= 1;
    bus->root_hub 				= NULL;
    bus->hcpriv 				= NULL;
    bus->busnum 				= -1;
    bus->bandwidth_allocated 	= 0;
    bus->bandwidth_int_reqs  	= 0;
    bus->bandwidth_isoc_reqs 	= 0;

    //	INIT_LIST_HEAD (&bus->bus_list);
}


/* this makes the hcd giveback() the urb more quickly, by kicking it
 * off hardware queues (which may take a while) and returning it as
 * soon as practical.  we've already set up the urb's return status,
 * but we can't know if the callback completed already.
 */
/*
********************************************************************************
*                     unlink1
* Description:
*     快速删除urb, 并且callback
* Arguments:
*     hcd  		: input. host controller device
*     urb  		: input. 请求被删除的urb
* Return value:
*     void
* note:
*
*********************************************************************************
*/
static int unlink1 (struct hc_gen_dev *hcd, struct urb *urb)
{
    int		value;

    //--<1>--roothub的urb就让roothub自己处理
    if (urb->dev == hcd->self.root_hub)
        value = rh_urb_dequeue (hcd, urb);
    else
    {
        //--<2>--其他的urb就dequeue
        /* The only reason an HCD might fail this call is if
         * it has not yet fully queued the urb to begin with.
         * Such failures should be harmless. */
        value = hcd->driver->urb_dequeue (hcd, urb);
    }

    if (value != 0)
    {
        DMSG_PANIC( "dequeue failed, %p --> %d\n", urb, value);
    }

    return value;
}


/*
********************************************************************************
*                     usb_hc_died
* Description:
*
* Arguments:
*     hcd  		: input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
void usb_hc_died (struct hc_gen_dev *hcd)
{
    __cpu_sr sr = 0;
    int ret = 0;

    DMSG_PANIC( "PANIC : hc_gen_dev died; cleaning up\n");

    USB_OS_ENTER_CRITICAL(sr);
    if (hcd->rh_registered)
    {
        hcd->poll_rh = 0;

        ret = USB_esKRNL_TmrStop(hcd->rh_timer);
        if( ret != USB_OS_EPDK_TRUE)
        {
            DMSG_PANIC("PANIC : usb_hc_died, timer stop error code = %x\n", USB_esKRNL_TmrError(hcd->rh_timer));
            return ;
        }

        ret = USB_esKRNL_TmrDel(hcd->rh_timer);
        if( ret != USB_OS_EPDK_TRUE)
        {
            DMSG_PANIC("PANIC : usb_hc_died, timer delete error code = %x\n", USB_esKRNL_TmrError(hcd->rh_timer));
            return ;
        }

        /* make khubd clean up old urbs and devices */
        usb_set_device_state (hcd->self.root_hub, USB_STATE_NOTATTACHED);

        DMSG_PANIC("PANIC : need to modify,\n");
        //		usb_kick_khubd (hcd->self.root_hub);
        //		kick_khubd
    }

    USB_OS_EXIT_CRITICAL(sr);
}

/**
 * register_root_hub - called by usb_add_hcd() to register a root hub
 * @usb_dev: the usb root hub device to be registered.
 * @hcd: host controller for this root hub
 *
 * This function registers the root hub with the USB subsystem.  It sets up
 * the device properly in the device tree and stores the root_hub pointer
 * in the bus structure, then calls usb_new_device() to register the usb
 * device.  It also assigns the root hub's USB address (always 1).
 */
/*
********************************************************************************
*                     _register_root_hub
* Description:
*     注册root hub dev, root hub's USB address (always 1).
* Arguments:
*     hcd      : input.
*     usb_dev  : input.
* Return value:
*     void
* note:
*     void
*********************************************************************************
*/
static int _register_root_hub (struct usb_host_virt_dev *usb_dev,
                               struct hc_gen_dev  *hcd)
{
    __cpu_sr sr = 0;
    u8 err = 0;
    const int devnum = 1;
    int retval = 0;


    //--<1>--初始化virt_dev
    usb_dev->devnum = devnum;
    usb_dev->bus->devnum_next = devnum + 1;
    USB_OS_MEMSET(&usb_dev->bus->devmap.devicemap, 0, sizeof (struct usb_devmap));
    usb_atomic_set_bit (devnum, usb_dev->bus->devmap.devicemap);
    usb_set_device_state(usb_dev, USB_STATE_ADDRESS);

    USB_OS_SEMI_PEND(usb_bus_list_lock, 0, &err);
    usb_dev->bus->root_hub = usb_dev;

    usb_dev->ep0.desc.wMaxPacketSize = 64;

    //--<2>--获得rh设备描述符
    retval = usb_get_device_descriptor(usb_dev, USB_DT_DEVICE_SIZE);
    if (retval != sizeof usb_dev->descriptor)
    {
        usb_dev->bus->root_hub = NULL;
        USB_OS_SEMI_POST (usb_bus_list_lock);
        DMSG_INFO( "can't read %s device descriptor %d\n", usb_dev->bus->bus_name, retval);
        return (retval < 0) ? retval : -EMSGSIZE;
    }

    //--<3>--创建新设备
    usb_lock_device (usb_dev);
    retval = usb_new_device (usb_dev);
    usb_unlock_device(usb_dev);
    if (retval)
    {
        usb_dev->bus->root_hub = NULL;
        DMSG_PANIC ( "PANIC : can't register root hub for %s, %d\n",
                     usb_dev->bus->bus_name, retval);
    }
    USB_OS_SEMI_POST (usb_bus_list_lock);

    if (retval == 0)
    {
        USB_OS_ENTER_CRITICAL(sr);
        hcd->rh_registered = 1;
        USB_OS_EXIT_CRITICAL(sr);

        /* Did the HC die before the root hub was registered? */
        if (hcd->state == HC_GEN_DEV_STATE_HALT)
            usb_hc_died (hcd);	/* This time clean up */
    }

    return retval;
}

/*
********************************************************************************
*                     usb_create_hc_gen_dev
* Description:
*     创建，并初始化usb_hcd
* Arguments:
*     driver 	: input.
*     bus_name	: input.
* Return value:
*     void
* note:
*     void
*********************************************************************************
*/
struct hc_gen_dev *usb_create_hc_gen_dev (const struct hc_driver *driver, const char *bus_name)
{
    struct hc_gen_dev *hcd;
    u8 ret = 0;

    //--<1>--初始化host controller device
    hcd = USB_OS_MALLOC( sizeof(*hcd) + driver->hcd_priv_size, USB_MEM_FILE_TRIGGER , USB_MEM_LINE_TRIGGER );
    if (!hcd)
    {
        DMSG_PANIC("usb_create_hc_gen_dev alloc failed\n");
        return NULL;
    }

    USB_OS_MEMSET(hcd, 0, sizeof(struct hc_gen_dev) + driver->hcd_priv_size);

    usb_bus_init(&hcd->self);
    hcd->self.hcpriv 		= hcd;
    hcd->self.bus_name 		= bus_name;
    hcd->self.point_gen_hcd	= hcd;

    //*****************************
    //创建root hub poll的timer
    //*****************************

    //--<2>--create and start timer
    hcd->rh_timer = USB_esKRNL_TmrCreate(USB_HOST_RH_TIMEROUT,
                                         USB_TIMER_PERIOD_MORE,
                                         (USB_TIMER_CALLBACK)rh_timer_func,
                                         (void *)hcd);
    if( hcd->rh_timer == NULL)
    {
        DMSG_PANIC("PANIC : create timer fail\n");
        return NULL;
    }

    ret = USB_esKRNL_TmrStart(hcd->rh_timer);
    if(ret ==  USB_OS_EPDK_FALSE)
    {
        DMSG_PANIC("PANIC : start timer fail,err = 0x%x\n", hcd->rh_timer->err);
        USB_esKRNL_TmrDel(hcd->rh_timer);

        return NULL;
    }

    //--<3>--匹配hcd设备的驱动
    hcd->driver 		= driver;
    hcd->product_desc 	= (driver->product_desc) ? driver->product_desc : "USB Host Controller";

    return hcd;
}

/*
********************************************************************************
*                     usb_add_hc_gen_dev
* Description:
*     添加一个新设备到总线上, 分配buff,并向usb总线注册rh
* Arguments:
*     hcd  		: input.
*     irqnum  	: input.
*     irqflags  : input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
s32 usb_add_hc_gen_dev(struct hc_gen_dev *hcd, u32  irqnum, u32 irqflags)
{
    int retval = 0;
    struct usb_host_virt_dev  *rh_dev = NULL;

    DMSG_INFO("[usbh core]: add gen_dev %s\n", hcd->product_desc);

    //call drv reset
    if (hcd->driver->reset && (retval = hcd->driver->reset(hcd)) < 0)
    {
        DMSG_PANIC("PANIC : usb_add_hc_gen_dev() :  can't reset\n");
        return retval;
    }

    //--<1>--为rh 创建一个usb_device结构
    if ((rh_dev = usb_host_alloc_virt_dev(NULL, &hcd->self, 0)) == NULL)
    {
        DMSG_PANIC("PANIC : usb_add_hc_gen_dev() : unable to allocate root hub\n");
        retval = -ENOMEM;
        return retval;
    }

    //--<2>--此hcd设备支持的速度
    rh_dev->speed = (hcd->driver->flags & HC_DRIVER_FLAG_HCD_USB2) ? USB_SPEED_HIGH : USB_SPEED_FULL;

    //--<3>--start这个设备
    if ((retval = hcd->driver->start(hcd)) < 0)
    {
        DMSG_PANIC("PANIC : usb_add_hc_gen_dev() :startup error %d\n", retval);
        goto err_hcd_driver_start;
    }

    /* hcd->driver->start() reported can_wakeup, probably with
     * assistance from board's boot firmware.
     * NOTE:  normal devices won't enable wakeup by default.
     */
    if (hcd->can_wakeup)
    {
        DMSG_INFO("supports USB remote wakeup\n");
    }

    hcd->remote_wakeup = hcd->can_wakeup;

    //--<4>--注册root hub, 即添加一个usb_host_virt_dev
    if ((retval = _register_root_hub (rh_dev, hcd)) != 0)
    {
        DMSG_PANIC("PANIC : can not register root hub :(\n");
        goto err_register_root_hub;
    }

    //--<5>--
    if (hcd->uses_new_polling && hcd->poll_rh)
    {
        usb_hcd_poll_rh_status(hcd);
    }

    return retval;

err_register_root_hub:
    hcd->driver->stop(hcd);

err_hcd_driver_start:
    usb_host_free_virt_dev(rh_dev);


    return retval;
}

/**
 * usb_remove_hcd - shutdown processing for generic HCDs
 * @hcd: the usb_hcd structure to remove
 * Context: !in_interrupt()
 *
 * Disconnects the root hub, then reverses the effects of usb_add_hcd(),
 * invoking the HCD's stop() method.
 */
/*
********************************************************************************
*                     usb_remove_hc_gen_dev
* Description:
*     删除hcd设备
* Arguments:
*     hcd  		: input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
void usb_remove_hc_gen_dev(struct hc_gen_dev *hcd)
{
    __cpu_sr sr = 0;
    s32 ret = 0;

    if(hcd == NULL)
    {
        DMSG_PANIC("ERR: invalid argment\n");
        return ;
    }

    if (HC_GEN_DEV_IS_RUNNING (hcd->state))
    {
        hcd->state = HC_GEN_DEV_STATE_QUIESCING;
    }

    USB_OS_ENTER_CRITICAL(sr);
    hcd->rh_registered = 0;
    USB_OS_EXIT_CRITICAL(sr);

    //--<1>--断开这个设备
    usb_disconnect(&hcd->self.root_hub);

    hcd->poll_rh = 0;

    //--<2>--停止并且删除timer
    ret = USB_esKRNL_TmrStop(hcd->rh_timer);
    if( ret != USB_OS_EPDK_TRUE)
    {
        DMSG_PANIC("PANIC : usb_hc_died, timer stop error code = %x\n", USB_esKRNL_TmrError(hcd->rh_timer));
        return ;
    }

    ret = USB_esKRNL_TmrDel(hcd->rh_timer);
    if(ret != USB_OS_EPDK_TRUE)
    {
        DMSG_PANIC("PANIC : usb_hc_died, timer del error code = %x\n", USB_esKRNL_TmrError(hcd->rh_timer));
        return ;
    }

    //--<3>--停止hcd设备
    hcd->driver->stop(hcd);
    hcd->state = HC_GEN_DEV_STATE_HALT;

    //--<4>--释放这个设备所有的资源
    if(hcd->self.root_hub)
    {
        usb_host_free_virt_dev(hcd->self.root_hub);
        hcd->self.root_hub = NULL;
    }

    //free self
    USB_OS_FREE(hcd);
    hcd = NULL;

    return ;
}

/*
********************************************************************************
*                     usb_remove_hc_gen_dev
* Description:
*     hcd的抽象ops
* Arguments:
*     hcd  		: input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
int hcd_ops_get_frame_number (struct usb_host_virt_dev *udev)
{
    struct hc_gen_dev	*hcd = (struct hc_gen_dev *)udev->bus->hcpriv;
    if (!HC_GEN_DEV_IS_RUNNING (hcd->state))
        return -ESHUTDOWN;

    return hcd->driver->get_frame_number (hcd);
}

/* may be called in any context with a valid urb->dev usecount
 * caller surrenders "ownership" of urb
 * expects usb_submit_urb() to have sanity checked and conditioned all
 * inputs in the urb
 */
/*
********************************************************************************
*                     hcd_ops_submit_urb
* Description:
*     提交urb, hcd的抽象ops。其实就是为了区分发往的目的地:
*                         1，rh，则走特殊通道
*                         2，物理device,走正规渠道
* Arguments:
*     urb  		: input.
*     mem_flags : input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
int hcd_ops_submit_urb (struct urb *urb, unsigned mem_flags)
{
    struct hc_gen_dev *hcd = NULL;
    struct usb_host_virt_endpoint *ep = NULL;
    __cpu_sr cpu_sr = 0;
    int	status = 0;

    if(!urb || !urb->dev || !urb->dev->bus || !urb->dev->bus->hcpriv)
    {
        DMSG_PANIC("PANIC : hcd_ops_submit_urb() : input NULL\n");
        return -ENODEV;
    }

    //--<1>--通过urb找到hcd
    hcd = urb->dev->bus->hcpriv;

    /*
     * Atomically queue the urb,  first to our records, then to the HCD.
     * Access to urb->status is controlled by urb->lock ... changes on
     * i/o completion (normal or fault) or unlinking.
     */

    // FIXME:  verify that quiescing hc works right (RH cleans up)

    //--<2>--通过urb找到ep
    USB_OS_ENTER_CRITICAL(cpu_sr);
    ep = (usb_pipein(urb->pipe) ? urb->dev->ep_in : urb->dev->ep_out)[usb_pipeendpoint(urb->pipe)];
    if (!ep)
    {
        DMSG_PANIC("ERR: ep = NULL\n");
        status = -ENOENT;
    }
    else if(urb->reject)
    {
        DMSG_PANIC("ERR: urb->reject must be zero\n");
        status = -EPERM;
    }
    else switch (hcd->state)
        {
        case HC_GEN_DEV_STATE_RUNNING:
        case HC_GEN_DEV_STATE_RESUMING:
        {
            //--<3>--添加到list, wrapper_urb
            struct list_head *list = _list_head_malloc_init();

            if(list)
            {
                list->data = urb;
                urb->wrapper_urb = list;
                list_add_tail(list, &(ep->urb_list));
            }

            status = 0;
        }
        break;

        default:
            DMSG_PANIC("ERR: unkown hcd->state(0x%x)\n", hcd->state);
            status = -ESHUTDOWN;
            break;
        }
    USB_OS_EXIT_CRITICAL(cpu_sr);

    if (status)
    {
        DMSG_PANIC("ERR: status failed\n");
        urb->wrapper_urb = NULL;
        //usbmon_urb_submit_error(&hcd->self, urb, status);
        return status;
    }

    /* increment urb's reference count as part of giving it to the HCD
     * (which now controls it).  HCD guarantees that it either returns
     * an error or calls giveback(), but not both.
     */

    usb_atomic_inc32 (&urb->use_count);

    //--<4>--目标设备为rh
    if (urb->dev == hcd->self.root_hub)
    {
        /* NOTE:  requirement on hub callers (usbfs and the hub
         * driver, for now) that URBs' urb->transfer_buffer be
         * valid and usb_buffer_{sync,unmap}() not be needed, since
         * they could clobber root hub response data.
         */
        status = rh_urb_enqueue (hcd, urb);
        goto done;
    }

    /* lower level hcd code should use *_dma exclusively,
     * unless it uses pio or talks to another transport.
     */

#if	0
    if (hcd->self.controller->dma_mask)
    {
        if (usb_pipecontrol (urb->pipe)
                && !(urb->transfer_flags & URB_NO_SETUP_DMA_MAP))
            urb->setup_dma = dma_map_single (
                                 hcd->self.controller,
                                 urb->setup_packet,
                                 sizeof (struct usb_ctrlrequest),
                                 DMA_TO_DEVICE);
        if (urb->transfer_buffer_length != 0
                && !(urb->transfer_flags & URB_NO_TRANSFER_DMA_MAP))
            urb->transfer_dma = dma_map_single (
                                    hcd->self.controller,
                                    urb->transfer_buffer,
                                    urb->transfer_buffer_length,
                                    usb_pipein (urb->pipe)
                                    ? DMA_FROM_DEVICE
                                    : DMA_TO_DEVICE);
    }
#endif


    //--<5>--物理外设
    status = hcd->driver->urb_enqueue (hcd, ep, urb, mem_flags);

done:
    if (status)
    {
        DMSG_PANIC("PANIC : [ gen hcd ] hcd_ops_submit_urb()...2...call real hcd fail\n");
        urb_unlink (urb);
        usb_atomic_dec32(&urb->use_count);

#if	0
        if (urb->reject)
            wake_up (&usb_kill_urb_queue);
#endif
        //这个做法不合理，不能释放，这个urb是别人的。
        //usb_free_urb (urb);

    }

    return status;
}


/*
 * called in any context
 *
 * caller guarantees urb won't be recycled till both unlink()
 * and the urb's completion function return
 */
/*
********************************************************************************
*                     hcd_ops_unlink_urb
* Description:
*     hcd的抽象ops
* Arguments:
*     urb  		: input.
*     status    : input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
int hcd_ops_unlink_urb (struct urb *urb, int status)
{
    struct usb_host_virt_endpoint	*ep = NULL;
    struct hc_gen_dev			    *hcd = NULL;

    s32	 retval = 0;

    if (!urb)
    {
        DMSG_PANIC("ERR: urb is NULL\n");
        return -EINVAL;
    }

    if (!urb->dev)
    {
        DMSG_PANIC("ERR: urb dev is invalid\n");
        return -ENODEV;
    }

    if ( !urb->dev->bus)
    {
        DMSG_PANIC("ERR: urb bus is invalid\n");
        return -ENODEV;
    }

    //--<1>--通过urb找到ep
    ep = (usb_pipein(urb->pipe) ? urb->dev->ep_in : urb->dev->ep_out)[usb_pipeendpoint(urb->pipe)];
    if (!ep)
    {
        DMSG_PANIC("ERR: ep is invalid\n");
        return -ENODEV;
    }

    /*
     * we contend for urb->status with the hcd core,
     * which changes it while returning the urb.
     *
     * Caller guaranteed that the urb pointer hasn't been freed, and
     * that it was submitted.  But as a rule it can't know whether or
     * not it's already been unlinked ... so we respect the reversed
     * lock sequence needed for the usb_hcd_giveback_urb() code paths
     * (urb lock, then hcd_data_lock) in case some other CPU is now
     * unlinking it.
     */
    USB_OS_ENTER_CRITICAL(urb->lock_urb);

    //--<1>--通过urb找到hcd
    hcd = urb->dev->bus->hcpriv;
    if (hcd == NULL)
    {
        DMSG_PANIC("ERR: hcd is invalid\n");
        retval = -ENODEV;
        goto done;
    }

    /* running ~= hc unlink handshake works (irq, timer, etc)
     * halted ~= no unlink handshake is needed
     * suspended, resuming == should never happen
     */
    if (!HC_GEN_DEV_IS_RUNNING (hcd->state) && hcd->state != HC_GEN_DEV_STATE_HALT)
    {
        DMSG_PANIC("PANIC : hcd_ops_unlink_urb() fail\n");
    }
    /*
    	// insist the urb is still queued
    	list_for_each(tmp, &ep->urb_list) {
    		if (tmp == &urb->urb_list)
    			break;
    	}
    	if (tmp != &urb->urb_list) {
    		retval = -EIDRM;
    		goto done;
    	}
    */
    if(!(urb->wrapper_urb))
    {
        DMSG_PANIC("PANIC : hcd_ops_unlink_urb() urb->wrapper_urb == NULL,not in ep->urb_list\n");
        goto done;
    }

    /* Any status except -EINPROGRESS means something already started to
     * unlink this URB from the hardware.  So there's no more work to do.
     */
    if (urb->status != -EINPROGRESS)
    {
        retval = -EBUSY;
        goto done;
    }

    /* IRQ setup can easily be broken so that USB controllers
     * never get completion IRQs ... maybe even the ones we need to
     * finish unlinking the initial failed usb_set_address()
     * or device descriptor fetch.
     */
    if (!hcd->saw_irq && hcd->self.root_hub != urb->dev)
    {
        DMSG_PANIC("WRN: Unlink after no-IRQ?  "
                   "Controller is probably using the wrong IRQ."
                   "\n");
        DMSG_PANIC("WRN: saw_irq = %d, root_hub = %x, dev = %x\n",
                   hcd->saw_irq, hcd->self.root_hub, urb->dev);
        hcd->saw_irq = 1;
    }

    urb->status = status;


    USB_OS_EXIT_CRITICAL(urb->lock_urb);

    retval = unlink1 (hcd, urb);
    if (retval == 0)
    {
        DMSG_PANIC("wrn: urb is dispose, but hcd_ops_unlink_urb happen\n");
        retval = -EINPROGRESS;
    }

    return retval;

done:
    USB_OS_EXIT_CRITICAL(urb->lock_urb);

    return retval;
}
/* sometimes alloc/free could use kmalloc with SLAB_DMA, for
 * better sharing and to leverage mm/slab.c intelligence.
 */
/*
********************************************************************************
*                     hcd_ops_buffer_alloc
* Description:
*     申请连续的物理内存
* Arguments:
*     hcd  		: input.
*     mem_flags : input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
void *hcd_ops_buffer_alloc (struct usb_virt_bus *bus,
                            u32	size,
                            u32	mem_flags,
                            u32	*dma)
{

    void *addr = 0;
    int real_want_page  = (size + USB_OS_PAGE_MALLOC_PAGE_SIZE - 1) / USB_OS_PAGE_MALLOC_PAGE_SIZE;

    addr = USB_OS_PAGE_MALLOC(real_want_page);

    if(dma)
    {
        *dma = 0;
    }

    return addr;
}

/*
********************************************************************************
*                     hcd_ops_buffer_free
* Description:
*     释放申请的物理内存
* Arguments:
*     hcd  		: input.
*     mem_flags : input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
void hcd_ops_buffer_free (struct usb_virt_bus *bus,
                          u32	size,
                          u32 *addr,
                          u32	 *dma)

{
    u32 real_free_pages  = (size + USB_OS_PAGE_MALLOC_PAGE_SIZE - 1) / USB_OS_PAGE_MALLOC_PAGE_SIZE;

    USB_OS_PAGE_FREE(addr , real_free_pages);

    if(dma)
    {
        *dma = 0;
    }

}

/*-------------------------------------------------------------------------*/

/* disables the endpoint: cancels any pending urbs, then synchronizes with
 * the hcd to make sure all endpoint state is gone from hardware. use for
 * set_configuration, set_interface, driver removal, physical disconnect.
 *
 * example:  a qh stored in ep->hcpriv, holding state related to endpoint
 * type, maxpacket size, toggle, halt status, and scheduling.
 */
/*
********************************************************************************
*                     hcd_ops_endpoint_disable
* Description:
*     hcd的抽象ops
* Arguments:
*     hcd  		: input.
*     mem_flags : input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
void hcd_ops_endpoint_disable (struct usb_host_virt_dev *udev, struct usb_host_virt_endpoint *ep)
{
    struct hc_gen_dev *hcd 			= NULL;
    struct urb *urb 				= NULL;
    struct list_head *list_start 	= NULL;
    struct list_head *list_now 	= NULL;

    __cpu_sr cpu_sr = 0;

    if( udev == NULL || (udev->bus == NULL) || ep == NULL)
    {
        DMSG_PANIC("PANIC : hcd_ops_endpoint_disable() :input :udev = %x ,bus = %x ,ep  == %x \n",
                   udev , udev ? (udev->bus) : (0) , ep);
        return ;
    }

    hcd = udev->bus->hcpriv;
    if (!HC_GEN_DEV_IS_RUNNING (hcd->state) && hcd->state != HC_GEN_DEV_STATE_HALT &&
            udev->state != USB_STATE_NOTATTACHED)
    {
        DMSG_PANIC("PANIC : hcd_ops_endpoint_disable() : error state\n");
        return ;
    }


    /* FIXME move most of this into message.c as part of its
     * endpoint disable logic
     */

    /* ep is already gone from udev->ep_{in,out}[]; no more submits */
    USB_OS_ENTER_CRITICAL(cpu_sr);

rescan:
    list_start = & (ep->urb_list);

    list_now = list_start->next;

    while(list_now != list_start )
    {
        int	tmp = 0;

        urb = (struct urb *)(list_now->data);
        if(!urb)
        {
            DMSG_PANIC("PANIC : hcd_ops_endpoint_disable() : urb = NULL\n");
            list_now = list_now->next;
            continue;
        }

        /* another cpu may be in hcd, spinning on hcd_data_lock
         * to giveback() this urb.  the races here should be
         * small, but a full fix needs a new "can't submit"
         * urb state.
         * FIXME urb->reject should allow that...
         */
        if (urb->status != -EINPROGRESS)
        {
            list_now = list_now->next;
            continue;
        }

        tmp = urb->status;
        if (tmp == -EINPROGRESS)
        {
            urb->status = -ESHUTDOWN;
        }

        /* kick hcd unless it's already returning this */
        if (tmp == -EINPROGRESS)
        {
            tmp = urb->pipe;
            unlink1 (hcd, urb);
        }

        /* list contents may have changed */
        list_now = list_now->next;

        goto rescan;
    }
    USB_OS_EXIT_CRITICAL(cpu_sr);

    /* synchronize with the hardware, so old configuration state
     * clears out immediately (and will be freed).
     */

    if (hcd->driver->endpoint_disable)
    {
        hcd->driver->endpoint_disable (hcd, ep);
    }
}

/*
void hcd_ops_endpoint_disable (struct usb_host_virt_dev *udev, struct usb_host_virt_endpoint *ep)
{
	struct hc_gen_dev		*hcd;
	struct urb		*urb;
	struct list_head * list_start = NULL;
	struct list_head * list_now = NULL;

	u32 flags;

	if( udev == NULL || (udev->bus == NULL)|| ep == NULL){
		DMSG_PANIC("PANIC : hcd_ops_endpoint_disable() :input :udev = %x ,bus = %x ,ep  == %x \n",
					 udev ,udev ? (udev->bus) : (0) , ep);
		return ;
	}
	hcd = udev->bus->hcpriv;
	if (!HC_GEN_DEV_IS_RUNNING (hcd->state) && hcd->state != HC_GEN_DEV_STATE_HALT &&
			udev->state != USB_STATE_NOTATTACHED){
		DMSG_PANIC("PANIC : hcd_ops_endpoint_disable() : error state\n");
		return ;
	}


	// FIXME move most of this into message.c as part of its
	//endpoint disable logic


	// ep is already gone from udev->ep_{in,out}[]; no more submits //
rescan:
	USB_OS_ENTER_CRITICAL(flags);

	list_start = & (ep->urb_list);

	list_now = list_start->next;

	while(list_now != list_start ){
		int	tmp;
		urb = (struct urb *)(list_now->data);
		if(!urb){
			DMSG_PANIC("PANIC : hcd_ops_endpoint_disable() : urb = NULL\n");
		}
		// another cpu may be in hcd, spinning on hcd_data_lock
		// to giveback() this urb.  the races here should be
		// small, but a full fix needs a new "can't submit"
		// urb state.
		// FIXME urb->reject should allow that...

		if (urb->status != -EINPROGRESS){
			continue;
		}

		USB_OS_ENTER_CRITICAL (urb->lock_urb);
		tmp = urb->status;
		if (tmp == -EINPROGRESS){
			urb->status = -ESHUTDOWN;
		}
		USB_OS_EXIT_CRITICAL (urb->lock_urb);
		// kick hcd unless it's already returning this//
		if (tmp == -EINPROGRESS) {
			tmp = urb->pipe;
			unlink1 (hcd, urb);
			DMSG_INFO ("shutdown urb %p pipe %x ep%d%s\n",
				urb, tmp, usb_pipeendpoint (tmp),
				(tmp & USB_DIR_IN) ? "in" : "out");
		}

		//list contents may have changed //
		goto rescan;
	}
	USB_OS_EXIT_CRITICAL(flags);

	//synchronize with the hardware, so old configuration state
	// clears out immediately (and will be freed).
	//
//	might_sleep ();
	if (hcd->driver->endpoint_disable){
		hcd->driver->endpoint_disable (hcd, ep);
	}
}
*/

#ifdef	CONFIG_USB_SUSPEND

static int hcd_ops_hub_suspend (struct usb_bus *bus)
{
    struct usb_hcd		*hcd;

    hcd = container_of (bus, struct usb_hcd, self);
    if (hcd->driver->hub_suspend)
    {
        return hcd->driver->hub_suspend (hcd);
    }

    return 0;
}

static int hcd_ops_hub_resume (struct usb_bus *bus)
{
    struct usb_hcd		*hcd;

    hcd = container_of (bus, struct usb_hcd, self);
    if (hcd->driver->hub_resume)
        return hcd->driver->hub_resume (hcd);
    return 0;
}

#endif



void usb_gen_hcd_init(void)
{
    usb_bus_list_lock = USB_OS_SEMI_CREATE(1);
}

void usb_gen_hcd_exit(void)
{
    u8 err = 0;
    if(usb_bus_list_lock == NULL)
    {
        DMSG_PANIC("PANIC : usb_bus_list_lock == NULL\n");
        return;
    }
    USB_OS_SEMI_DELETE(usb_bus_list_lock, &err);
    usb_bus_list_lock = NULL;
}









