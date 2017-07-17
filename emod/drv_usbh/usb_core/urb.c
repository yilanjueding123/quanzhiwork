/*
*******************************************************************************
*                                              usb host module                                           *
*                                                                                                                                          *
*                             Copyright(C), 2006-2008, SoftWinners Co., Ltd.                       *
*								All Rights Reserved
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
*		usb request block的相关操作，包括分配/释放，发送/撤回
* History :
*******************************************************************************
*/
#include "usb_host_config.h"
#include "usb_os_platform.h"
#include "usb_host_common.h"
#include "usb_host_base_types.h"
#include "usb_list.h"

#include "usb_gen_hcd.h"


static u32 urb_sn_now = 0;	//

void usb_kill_urb(struct urb *urb);




//用来打印msc_info结构
void _debug_print_urb(struct urb *urb_input)
{
    //DMSG_INFO(" = %x\n",urb_input-> );

    DMSG_INFO("\n==print_urb=========================\n");
    DMSG_INFO("urb addr			= %x\n", urb_input);
    DMSG_INFO("lock_urb			= %x\n", urb_input->lock_urb );
    DMSG_INFO("wait_urb_timer	= %x\n", urb_input->wait_urb_timer );
    DMSG_INFO("hcpriv			= %x\n", urb_input->hcpriv );
    DMSG_INFO("bandwidth		= %x\n", urb_input->bandwidth );
    DMSG_INFO("use_count		= %x\n", urb_input->use_count );
    DMSG_INFO("reject			= %x\n", urb_input->reject );
    DMSG_INFO("dev				= %x\n", urb_input->dev );
    DMSG_INFO("pipe				= %x\n", urb_input->pipe );
    DMSG_INFO("status			= %x\n", urb_input->status );
    DMSG_INFO("transfer_flags	= %x\n", urb_input->transfer_flags );
    DMSG_INFO("transfer_buffer	= %x\n", urb_input->transfer_buffer );
    DMSG_INFO("transfer_dma		= %x\n", urb_input->transfer_dma );
    DMSG_INFO("transfer_buffer_length = %x\n", urb_input->transfer_buffer_length );
    DMSG_INFO("actual_length	= %x\n", urb_input->actual_length );
    DMSG_INFO("setup_packet		= %x\n", urb_input->setup_packet );
    DMSG_INFO("setup_dma		= %x\n", urb_input->setup_dma );
    DMSG_INFO("start_frame		= %x\n", urb_input->start_frame );
    DMSG_INFO("number_of_packets = %x\n", urb_input->number_of_packets );
    DMSG_INFO("interval			= %x\n", urb_input->interval );
    DMSG_INFO("error_count		= %x\n", urb_input->error_count );
    DMSG_INFO("context			= %x\n", urb_input->context );
    DMSG_INFO("complete			= %x\n", urb_input->complete );
    DMSG_INFO("wrapper_urb		= %x\n", urb_input->wrapper_urb );
    DMSG_INFO("===========================\n\n");
}

/*
***********************************************************
*
* Description	:
*				初始化urb
* Arguments  :
*	@urb		:
* Returns    	:
*				无
* Notes      	:
*
***********************************************************
*/
void usb_init_urb(struct urb *urb)
{
    if (urb == NULL)
    {
        DMSG_PANIC("ERR: urb is NULL\n");
        return;
    }

    USB_OS_MEMSET(urb, 0, sizeof(struct urb));
}

/*
***********************************************************
*
* Description	:
*				分配urb，并初始化urb
* Arguments  :
*	@urb		:
* Returns    	:
*
* Notes      	:
*
***********************************************************
*/
struct urb *usb_alloc_urb(int iso_packets)
{
    struct urb *urb;

    urb = (struct urb *)USB_OS_MALLOC(sizeof(struct urb) + iso_packets * sizeof(struct usb_iso_packet_descriptor),
                                      USB_MEM_FILE_TRIGGER , USB_MEM_LINE_TRIGGER );
    if (!urb)
    {
        DMSG_PANIC("alloc_urb: kmalloc failed");
        return NULL;
    }

    usb_init_urb(urb);

    return urb;
}

/*
***********************************************************
*
* Description	:
*				释放urb
* Arguments  :
*	@urb		:
* Returns    	:
*
* Notes      	:
*
***********************************************************
*/
void usb_free_urb(struct urb *urb)
{
    if (urb)
    {
        USB_OS_FREE(urb);
        urb = NULL;
    }
    else
    {
        DMSG_PANIC("ERR: usb_free_urb: urb = NULL\n");
    }
}

/*
*************************************************************************************
*                     usb_submit_urb
*
* Description:
*     发送urb
*
* Arguments:
*     urb        :  input.  待提交的urb
*     mem_flags  :  input.
*
* Returns:
*     返回命令的执行结果
* note:
*
*************************************************************************************
*/
int usb_submit_urb(struct urb *urb, unsigned mem_flags)
{
    int	pipe, temp, max;
    struct usb_host_virt_dev *dev;
    //struct usb_operations *op;
    int	is_out;

    //--<1>--check input
    if (!urb)
    {
        DMSG_PANIC("ERR: usb_submit_urb: invalid argment\n");
        return -EINVAL;
    }

    if (urb->hcpriv || !urb->complete)
    {
        DMSG_PANIC("ERR: usb_submit_urb: urb->hcpriv = 0x%x ,urb->complete = 0x%x input error\n ",
                   urb->hcpriv , urb->complete);
        return -EINVAL;
    }

    //--<2>--check dev
    dev = urb->dev;
    if (!dev || (dev->state < USB_STATE_DEFAULT) || (!dev->bus) || (dev->devnum < 0))
    {
        DMSG_PANIC("ERR: usb_submit_urb: dev had ben destroied\n");
        return -ENODEV;
    }

    if (dev->state == USB_STATE_SUSPENDED)
    {
        DMSG_PANIC("ERR: usb_submit_urb: dev had ben destroied\n");
        return -EHOSTUNREACH;
    }

    //--<3>--构造URB
    urb_sn_now ++;
    urb->urb_sn        = urb_sn_now;
    urb->status        = -EINPROGRESS;
    urb->actual_length = 0;
    urb->bandwidth     = 0;

    /* Lots of sanity checks, so HCDs can rely on clean data
     * and don't need to duplicate tests
     */
    pipe   = urb->pipe;
    temp   = usb_pipetype(pipe);
    is_out = usb_pipeout(pipe);

    if (!usb_pipecontrol (pipe) && dev->state < USB_STATE_CONFIGURED)
    {
        DMSG_PANIC("ERR: usb_submit_urb: USB_STATE_CONFIGURED\n");
        return -ENODEV;
    }

    /* FIXME there should be a sharable lock protecting us against
     * config/altsetting changes and disconnects, kicking in here.
     * (here == before maxpacket, and eventually endpoint type,
     * checks get made.)
     */
    max = usb_maxpacket(dev, pipe, is_out);
    if (max <= 0)
    {
        DMSG_PANIC("bogus endpoint ep%d%s  (bad maxpacket %d)\n",
                   usb_pipeendpoint (pipe), is_out ? "out" : "in",
                   max);
        return -EMSGSIZE;
    }

#if	0
    /* periodic transfers limit size per frame/uframe,
     * but drivers only control those sizes for ISO.
     * while we're checking, initialize return status.
     */
    if (temp == PIPE_ISOCHRONOUS)
    {
        int	n, len;

        /* "high bandwidth" mode, 1-3 packets/uframe? */
        if (dev->speed == USB_SPEED_HIGH)
        {
            int	mult = 1 + ((max >> 11) & 0x03);
            max &= 0x07ff;
            max *= mult;
        }

        if (urb->number_of_packets <= 0)
            return -EINVAL;
        for (n = 0; n < urb->number_of_packets; n++)
        {
            len = urb->iso_frame_desc [n].length;
            if (len < 0 || len > max)
                return -EMSGSIZE;
            urb->iso_frame_desc [n].status = -EXDEV;
            urb->iso_frame_desc [n].actual_length = 0;
        }
    }
#endif

    /* the I/O buffer must be mapped/unmapped, except when length=0 */
    if (urb->transfer_buffer_length < 0)
    {
        DMSG_PANIC("ERR: urb->transfer_buffer_length must not be negative\n");
        return -EMSGSIZE;
    }

#ifdef DEBUG
    /* stuff that drivers shouldn't do, but which shouldn't
     * cause problems in HCDs if they get it wrong.
     */
    {
        unsigned int	orig_flags = urb->transfer_flags;
        unsigned int	allowed;

        /* enforce simple/standard policy */
        allowed = URB_ASYNC_UNLINK;	// affects later unlinks
        allowed |= (URB_NO_TRANSFER_DMA_MAP | URB_NO_SETUP_DMA_MAP);
        allowed |= URB_NO_INTERRUPT;
        switch (temp)
        {
        case PIPE_BULK:
            if (is_out)
                allowed |= URB_ZERO_PACKET;
        /* FALLTHROUGH */
        case PIPE_CONTROL:
            allowed |= URB_NO_FSBR;	/* only affects UHCI */
        /* FALLTHROUGH */
        default:			/* all non-iso endpoints */
            if (!is_out)
                allowed |= URB_SHORT_NOT_OK;
            break;
        case PIPE_ISOCHRONOUS:
            allowed |= URB_ISO_ASAP;
            break;
        }
        urb->transfer_flags &= allowed;

        /* fail if submitter gave bogus flags */
        if (urb->transfer_flags != orig_flags)
        {
            DMSG_PANIC  ("PANIC : BOGUS urb flags, %x --> %x",
                         orig_flags, urb->transfer_flags);
            return -EINVAL;
        }
    }
#endif

    /*
     * Force periodic transfer intervals to be legal values that are
     * a power of two (so HCDs don't need to).
     *
     * FIXME want bus->{intr,iso}_sched_horizon values here.  Each HC
     * supports different values... this uses EHCI/UHCI defaults (and
     * EHCI can use smaller non-default values).
     */
    switch (temp)
    {
    case PIPE_ISOCHRONOUS:
    case PIPE_INTERRUPT:
        /* too small? */
        if (urb->interval <= 0)
        {
            DMSG_PANIC("ERR: urb->interval ERROR\n");
            return -EINVAL;
        }

        /* too big? */
        switch (dev->speed)
        {
        case USB_SPEED_HIGH:	/* units are microframes */
            // NOTE usb handles 2^15
            if (urb->interval > (1024 * 8))
                urb->interval = 1024 * 8;
            temp = 1024 * 8;
            break;

        case USB_SPEED_FULL:	/* units are frames/msec */
        case USB_SPEED_LOW:
            if (temp == PIPE_INTERRUPT)
            {
                if (urb->interval > 255)
                    return -EINVAL;
                // NOTE ohci only handles up to 32
                temp = 128;
            }
            else
            {
                if (urb->interval > 1024)
                    urb->interval = 1024;
                // NOTE usb and ohci handle up to 2^15
                temp = 1024;
            }
            break;

        default:
            return -EINVAL;
        }

        /* power of two? */
        while (temp > urb->interval)
            temp >>= 1;
        urb->interval = temp;
    }

    return hcd_ops_submit_urb(urb, mem_flags);
}

/*
***********************************************************
*
* Description	:
*				撤回urb
* Arguments  :
*	@urb		:
* Returns    	:
*
* Notes      	:
*
***********************************************************
*/
int usb_unlink_urb(struct urb *urb)
{
    if (!urb)
    {
        DMSG_PANIC("ERR: usb_unlink_urb: input = NULL\n");
        return -EINVAL;
    }

    if (!(urb->transfer_flags & URB_ASYNC_UNLINK))
    {
        DMSG_PANIC("ERR: usb_unlink_urb: URB_ASYNC_UNLINK\n");
        usb_kill_urb(urb);
        return 0;
    }

    return hcd_ops_unlink_urb(urb, -ECONNRESET);
}

/*
***********************************************************
*
* Description	:
*
* Arguments  :
*	@urb		:
* Returns    	:
*
* Notes      	:
*
***********************************************************
*/
/* 和usb_unlink_urb()的区别 */
void usb_kill_urb(struct urb *urb)
{
    if(urb == NULL)
    {
        DMSG_PANIC("ERR: usb_kill_urb: input error\n");
        return;
    }

    USB_OS_ENTER_CRITICAL(urb->lock_urb);
    ++urb->reject;
    USB_OS_EXIT_CRITICAL(urb->lock_urb);

    hcd_ops_unlink_urb(urb, -ENOENT);

    USB_OS_ENTER_CRITICAL(urb->lock_urb);
    --urb->reject;
    USB_OS_EXIT_CRITICAL(urb->lock_urb);
}

