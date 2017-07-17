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

#include "usb_os_platform.h"
#include "usb_host_base_types.h"
#include "usb_list.h"

#include "usb_host_common.h"
#include "usb_core_base.h"
#include "usb_gen_hub.h"
#include "usb_msg.h"
#include "usb_gen_hub.h"
#include "usb_virt_bus.h"

#include "urb.h"


/* 被urb的消费者调用，如hcd的finish函数通知core该urb被执行完毕 */
static void _urb_done_complete_callback(struct urb *urb)
{
    USB_OS_COMPLETE_POST((USB_OS_KERNEL_EVENT *)(urb->context));
}

/* urb timeout 处理函数, 其实就是在queue的过程中出现了问题，
   通过unqueue来并行的做一下，别的地方都是复用的。 */
static void _urb_timeout_process(s32 data)
{
    struct urb *urb = (struct urb *)data;

    if(urb == NULL )
    {
        DMSG_PANIC("PANIC : _urb_timeout_process() urb = %x \n", urb);
        return ;
    }

    DMSG_INFO("ERR: _urb_timeout_process: urb time out %x\n", urb);

    usb_unlink_urb(urb);

    return;
}

// Starts urb and waits for completion or timeout
// note that this call is NOT interruptible, while
// many device driver i/o requests should be interruptible
//timeout 	:	!= 0时表示有timeout，单位为ms

/* 	内部函数--发送urb，并等待其完成，或timeout
   	本函数要求可以重入，所以semi和timer得时时分配
	虽然epos的semi带有timeout 功能，
	但是为了提高本代码的可移植性
	将不利用该特性，
	但是将来为了优化，可以考虑该特性
*/
static int usb_start_wait_urb(struct urb *urb, int timeout_ms, int *actual_length)
{
    __u8  err 		= 0;
    __u32 ret 		= 0;
    __s32 status 	= 0;
    USB_OS_KERNEL_EVENT 		*wait_urb_complete_semi = NULL;
    USB_OS_KERNEL_SOFT_TIMER 	*wait_urb_timer = NULL;

    /* check input */
    if(urb == NULL)
    {
        DMSG_PANIC("ERR: usb_start_wait_urb: input error\n");
        return -1;
    }

    /* initialize urb */
    wait_urb_complete_semi = USB_OS_COMPLETE_CREATE();
    if(wait_urb_complete_semi == NULL)
    {
        DMSG_PANIC("ERR: create complete semi false\n");
        return -1;
    }

    urb->context		= (void *)wait_urb_complete_semi;
    urb->transfer_flags |= URB_ASYNC_UNLINK;
    urb->actual_length 	= 0;

    /* submit urb */
    status = usb_submit_urb(urb, 0);
    if(status == 0)
    {
        if(timeout_ms > 0)
        {
            wait_urb_timer = USB_esKRNL_TmrCreate(timeout_ms ,
                                                  USB_TIMER_PERIOD_ONE,
                                                  (USB_TIMER_CALLBACK)_urb_timeout_process,
                                                  urb);
            if(wait_urb_timer == NULL)
            {
                DMSG_PANIC("PANIC : create timer for urb false\n");
                return -1;
            }

            urb->wait_urb_timer = wait_urb_timer ;
            ret = USB_esKRNL_TmrStart(wait_urb_timer);
            if( ret ==  USB_OS_EPDK_FALSE )
            {
                USB_esKRNL_TmrDel(wait_urb_timer);
                DMSG_PANIC("PANIC : usb_start_wait_urb,start timer fail\n");

                return -1;
            }
        }

        USB_OS_COMPLETE_PEND(wait_urb_complete_semi, 0, &err);

        status = urb->status;

        /* note:  HCDs return ETIMEDOUT for other reasons too */
        if (status == -ECONNRESET)
        {
            DMSG_PANIC("ERR: timed out on ep%d%s len=%d/%d\n",
                       usb_pipeendpoint(urb->pipe),
                       usb_pipein(urb->pipe) ? "in" : "out",
                       urb->actual_length,
                       urb->transfer_buffer_length);
            if (urb->actual_length > 0)
            {
                status = 0;
            }
            else
            {
                status = -ETIMEDOUT;
            }
        }

        //stop timer
        if (wait_urb_timer !=  0)
        {
            ret = USB_esKRNL_TmrStop(wait_urb_timer);
            if(ret != USB_OS_EPDK_TRUE)
            {
                DMSG_PANIC("PANIC : usb_start_wait_urb, timer stop error code = %x\n", USB_esKRNL_TmrError(wait_urb_timer));
                return -1;
            }

            ret = USB_esKRNL_TmrDel(wait_urb_timer);
            if( ret != USB_OS_EPDK_TRUE)
            {
                DMSG_PANIC("PANIC : usb_start_wait_urb, timer del error code = %x\n", USB_esKRNL_TmrError(wait_urb_timer));
                return -1;
            }
        }

        urb->wait_urb_timer = NULL;
    }

    if (actual_length)
    {
        *actual_length = urb->actual_length;
    }

    //free complete
    if(wait_urb_complete_semi)
    {
        USB_OS_COMPLETE_DELETE(wait_urb_complete_semi, &err);
    }

    //free urb
    usb_free_urb(urb);

    return status;
}

//returns
//	<0	:	失败
//	>=0	:	返回的长度
int _usb_internal_ctrll_msg(struct usb_host_virt_dev *usb_dev,
                            u32 pipe,
                            struct usb_ctrlrequest *cmd,
                            void *data,
                            s32 len,
                            s32 timeout)
{
    struct urb *urb;
    int retv;
    int length;

    //--<1>--malloc urb
    urb = usb_alloc_urb(0);
    if (!urb)
    {
        DMSG_PANIC("ERR: usb_alloc_urb failed\n");
        return -ENOMEM;
    }

    //--<2>--填充urb
    usb_fill_control_urb(urb,
                         usb_dev,
                         pipe,
                         (unsigned char *)cmd,
                         data,
                         len,
                         _urb_done_complete_callback,
                         NULL);

    //--<3>--发送并等待urb完成或timeout
    retv = usb_start_wait_urb(urb, timeout, &length);
    if (retv < 0)
    {
        return retv;
    }
    else
    {
        return length;
    }
}



int _usb_internal_bulk_msg( struct usb_host_virt_dev *usb_dev,
                            unsigned int pipe,
                            void *data,
                            int len,
                            int *actual_length,
                            int timeout)
{
    struct urb *urb = NULL;

    //--<1>--分配一个urb
    urb = usb_alloc_urb(0);
    if (!urb)
    {
        return -ENOMEM;
    }

    //--<2>--填充一个urb
    usb_fill_bulk_urb(urb,
                      usb_dev,
                      pipe,
                      data,
                      len,
                      _urb_done_complete_callback,
                      NULL);

    //--<3>--发送urb并等待其完成或timeout
    return usb_start_wait_urb(urb, timeout, actual_length);
}


/**
 * usb_get_string - gets a string descriptor
 * @dev: the device whose string descriptor is being retrieved
 * @langid: code for language chosen (from string descriptor zero)
 * @index: the number of the descriptor
 * @buf: where to put the string
 * @size: how big is "buf"?
 * Context: !in_interrupt ()
 *
 * Retrieves a string, encoded using UTF-16LE (Unicode, 16 bits per character,
 * in little-endian byte order).
 * The usb_string() function will often be a convenient way to turn
 * these strings into kernel-printable form.
 *
 * Strings may be referenced in device, configuration, interface, or other
 * descriptors, and could also be used in vendor-specific ways.
 *
 * This call is synchronous, and may not be used in an interrupt context.
 *
 * Returns the number of bytes received on success, or else the status code
 * returned by the underlying usb_control_msg() call.
 */
int usb_get_string( struct usb_host_virt_dev  *dev,
                    u16 langid,
                    u8  index,
                    void *buf,
                    s32 size)
{
    int i;
    int result;

    for (i = 0; i < 3; ++i)
    {
        /* retry on length 0 or stall; some devices are flakey */
        result = usb_control_msg(dev,
                                 usb_rcvctrlpipe(dev, 0),
                                 USB_REQ_GET_DESCRIPTOR,
                                 USB_DIR_IN,
                                 (USB_DT_STRING << 8) + index,
                                 langid,
                                 buf,
                                 size,
                                 USB_CTRL_GET_TIMEOUT);
        if (!(result == 0 || result == -EPIPE))
        {
            break;
        }
    }

    return result;
}

#define _U	0x01	/* upper */
#define _L	0x02	/* lower */
#define _D	0x04	/* digit */
#define _C	0x08	/* cntrl */
#define _P	0x10	/* punct */
#define _S	0x20	/* white space (space/lf/tab) */
#define _X	0x40	/* hex digit */
#define _SP	0x80	/* hard space (0x20) */


unsigned char _ctype[] =
{
    _C, _C, _C, _C, _C, _C, _C, _C,					/* 0-7 */
    _C, _C | _S, _C | _S, _C | _S, _C | _S, _C | _S, _C, _C,		/* 8-15 */
    _C, _C, _C, _C, _C, _C, _C, _C,					/* 16-23 */
    _C, _C, _C, _C, _C, _C, _C, _C,					/* 24-31 */
    _S | _SP, _P, _P, _P, _P, _P, _P, _P,				/* 32-39 */
    _P, _P, _P, _P, _P, _P, _P, _P,					/* 40-47 */
    _D, _D, _D, _D, _D, _D, _D, _D,					/* 48-55 */
    _D, _D, _P, _P, _P, _P, _P, _P,					/* 56-63 */
    _P, _U | _X, _U | _X, _U | _X, _U | _X, _U | _X, _U | _X, _U,	/* 64-71 */
    _U, _U, _U, _U, _U, _U, _U, _U,					/* 72-79 */
    _U, _U, _U, _U, _U, _U, _U, _U,					/* 80-87 */
    _U, _U, _U, _P, _P, _P, _P, _P,					/* 88-95 */
    _P, _L | _X, _L | _X, _L | _X, _L | _X, _L | _X, _L | _X, _L,	/* 96-103 */
    _L, _L, _L, _L, _L, _L, _L, _L,					/* 104-111 */
    _L, _L, _L, _L, _L, _L, _L, _L,					/* 112-119 */
    _L, _L, _L, _P, _P, _P, _P, _C,					/* 120-127 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,			/* 128-143 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,			/* 144-159 */
    _S | _SP, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, /* 160-175 */
    _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, /* 176-191 */
    _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, /* 192-207 */
    _U, _U, _U, _U, _U, _U, _U, _P, _U, _U, _U, _U, _U, _U, _U, _L, /* 208-223 */
    _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, /* 224-239 */
    _L, _L, _L, _L, _L, _L, _L, _P, _L, _L, _L, _L, _L, _L, _L, _L /* 240-255 */
};


extern unsigned char _ctype[];

#define __ismask(x) (_ctype[(int)(unsigned char)(x)])

#define my_isprint(c)	((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)

static void usb_try_string_workarounds(u8  *buf, s32 *length)
{
    s32 newlength, oldlength = *length;

    for (newlength = 2; newlength + 1 < oldlength; newlength += 2)
    {
        if (!my_isprint(buf[newlength]) || buf[newlength + 1])
        {
            break;
        }
    }

    if (newlength > 2)
    {
        buf[0] = newlength;
        *length = newlength;
    }
}




int _usb_string_sub(struct usb_host_virt_dev  *dev,  u32 langid, u32 index, u8 *buf)
{
    int rc;

    /* Try to read the string descriptor by asking for the maximum
     * possible number of bytes */
    rc = usb_get_string(dev, langid, index, buf, 255);

    /* If that failed try to read the descriptor length, then
     * ask for just that many bytes */
    if(rc < 2)
    {
        rc = usb_get_string(dev, langid, index, buf, 2);
        if (rc == 2)
        {
            rc = usb_get_string(dev, langid, index, buf, buf[0]);
        }
    }

    if(rc >= 2)
    {
        if (!buf[0] && !buf[1])
        {
            usb_try_string_workarounds(buf, &rc);
        }

        /* There might be extra junk at the end of the descriptor */
        if (buf[0] < rc)
        {
            rc = buf[0];
        }

        rc = rc - (rc & 1); /* force a multiple of two */
    }

    if (rc < 2)
    {
        rc = (rc < 0 ? rc : -EINVAL);
    }

    return rc;
}




