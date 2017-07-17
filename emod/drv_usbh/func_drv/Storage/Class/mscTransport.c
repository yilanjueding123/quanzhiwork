/*
********************************************************************************************************************
*                                              usb host driver
*
*                              (c) Copyright 2007-2010, javen.China
*										All	Rights Reserved
*
* File Name 	: mscTransport.c
*
* Author 		: javen
*
* Version 		: 2.0
*
* Date 			: 2010.03.02
*
* Description 	:
*
* History 		:
*
********************************************************************************************************************
*/
#include  "usb_host_config.h"
#include  "usb_host_base_types.h"
#include  "usb_os_platform.h"
#include  "error.h"

#include  "usb_host_common.h"
#include  "urb.h"
#include  "usb_gen_hub.h"

#include  "usb_msc_i.h"
#include  "mscTransport.h"
#include  "Scsi2.h"

/*
*******************************************************************************
*                     mscUrbCallBack
*
* Description:
*    urb call back函数
*
* Parameters:
*    urb : input. 需要处理的URB
*
* Return value:
*    无
*
* note:
*
*
*******************************************************************************
*/
static void mscUrbCallBack(struct urb *urb)
{
    USB_OS_KERNEL_EVENT *urb_done = (USB_OS_KERNEL_EVENT *)urb->context;

    if(urb_done)
    {
        USB_OS_SemPost(urb_done);
    }
    else
    {
        DMSG_PANIC("ERR: mscUrbCallBack: urb_done == NULL\n");
    }
}

/*
*******************************************************************************
*                     mscTimeOut
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*
*
* note:
*
*
*******************************************************************************
*/
static void mscTimeOut(void *parg)
{
    __mscDev_t *mscDev = (__mscDev_t *)parg;

    if(mscDev == NULL)
    {
        DMSG_PANIC("ERR: mscTimeOut: mscDev == NULL\n");
        return;
    }

    DMSG_PANIC("ERR: mscTimeOut: CurrentUrb = %x\n", mscDev->CurrentUrb);

    if(mscDev->ScsiCmnd)
    {
        DMSG_PANIC("ERR: mscTimeOut, Cmnd = %x, Timeout = %dms, retries = %d, allowed = %d\n",
                   ((__u8 *)(mscDev->ScsiCmnd->cmnd.CommandBlock))[0],
                   mscDev->ScsiCmnd->cmnd.Timeout,
                   mscDev->ScsiCmnd->retries,
                   mscDev->ScsiCmnd->allowed);
    }

    if(mscDev->busy)
    {
        usb_unlink_urb(mscDev->CurrentUrb);
        mscDev->CurrentUrb->status = -ETIMEDOUT;
    }

    return;
}

/*
*******************************************************************************
*                     mscUsbTransport
*
* Description:
*    发送URB
*
* Parameters:
*    mscDev   :  input. 目标设备
*	 TimeOut  :  input. 超时时间
*
* Return value:
*    返回URB的状态
*
* note:
*
*
*******************************************************************************
*/
static __s32 mscUsbTransport(__mscDev_t *mscDev, __u32 TimeOut)
{
    __s32 ret = 0;
    __u8 err = 0;

    if(mscDev == NULL)
    {
        DMSG_PANIC("ERR: mscUSBTransport: input error, mscDev = %x\n", mscDev);
        return -EINVAL;
    }

    if(mscDev->state != MSC_DEV_ONLINE)
    {
        DMSG_PANIC("ERR: mscUSBTransport: Can't transport for device is not online\n");
        return -EINVAL;
    }

    /* fill URB */
    mscDev->CurrentUrb->context       = mscDev->UrbWait;
    mscDev->CurrentUrb->actual_length = 0;
    mscDev->CurrentUrb->error_count   = 0;
    mscDev->CurrentUrb->status        = 0;

    /* 修正buffer传输方式, 如果不是msc的buff, 就不能使用DMA。 因为buffer, 要palloc出来的 */
    mscDev->CurrentUrb->transfer_flags = URB_ASYNC_UNLINK | URB_NO_SETUP_DMA_MAP;
    if (mscDev->CurrentUrb->transfer_buffer == mscDev->iobuf)
    {
        mscDev->CurrentUrb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
    }
    mscDev->CurrentUrb->transfer_dma = 0;
    mscDev->CurrentUrb->setup_dma    = 0;

    /* create timer */
    if(TimeOut)
    {
        mscDev->TimerHdle = USB_esKRNL_TmrCreate(TimeOut ,
                            USB_TIMER_PERIOD_ONE,
                            (USB_TIMER_CALLBACK)mscTimeOut,
                            (void *)mscDev);
        if(mscDev->TimerHdle == NULL)
        {
            DMSG_PANIC("ERR: create timer failed\n");
            return -EINVAL;
        }
    }

    /* set mscDev busy */
    mscDev->busy = 1;

    /* submit urb */
    ret = usb_submit_urb(mscDev->CurrentUrb, 0);
    if(ret != 0)
    {
        DMSG_PANIC("ERR: submit urb failed. ret = %d\n", ret);

        if(mscDev->TimerHdle)
        {
            USB_esKRNL_TmrDel(mscDev->TimerHdle);
            mscDev->TimerHdle = NULL;
        }

        mscDev->busy = 0;

        return -EINVAL;
    }


    /* start timer */
    if(mscDev->TimerHdle)
    {
        USB_esKRNL_TmrStart(mscDev->TimerHdle);
    }

    /* wait urb done */
    USB_OS_SemPend(mscDev->UrbWait, 0, &err);

    /* urb is done, then set mscDev free */
    mscDev->busy = 0;

    /* kill timer */
    if(mscDev->TimerHdle)
    {
        USB_esKRNL_TmrStop(mscDev->TimerHdle);
        USB_esKRNL_TmrDel(mscDev->TimerHdle);
        mscDev->TimerHdle = NULL;
    }

    return mscDev->CurrentUrb->status;
}

/*
*******************************************************************************
*                     mscCtrlMsg
*
* Description:
*    发送控制请求
*
* Parameters:
*    mscDev			:  input.  目标设备
*    Pipe			:  input.  URB管道
*    Request		:  input.  控制请求
*    RequestType	:  input.  控制请求类型
*    Value			:  input.  值
*    Index			:  input.  索引
*    Buffer			:  input.  缓冲区
*    BufferLen		:  input.  缓冲区大小
*    TimeOut		:  input.  URB超时时间
*
* Return value:
*    返回URB的状态
*
* note:
*
*
*******************************************************************************
*/
static __s32 mscSendCtrlReq(__mscDev_t *mscDev,
                            __s32 Pipe,
                            __u8  Request,
                            __u8  RequestType,
                            __u16 Value,
                            __u16 Index,
                            void *Buffer,
                            __u16 BufferLen,
                            __u32 TimeOut)
{
    if(mscDev == NULL)
    {
        DMSG_PANIC("ERR: USBStorageCtrlMsg : mscDev = %x\n", mscDev);
        return USB_ERR_BAD_ARGUMENTS;
    }

    /* 填充ctrl请求 */
    mscDev->CtrlReq->bRequest     = Request;
    mscDev->CtrlReq->bRequestType = RequestType;
    mscDev->CtrlReq->wValue       = cpu_to_le16(Value);
    mscDev->CtrlReq->wIndex       = cpu_to_le16(Index);
    mscDev->CtrlReq->wLength      = cpu_to_le16(BufferLen);

    /* fill and submit the Urb */
    usb_fill_control_urb(mscDev->CurrentUrb,
                         mscDev->pusb_dev,
                         Pipe,
                         (unsigned char *)mscDev->CtrlReq,
                         Buffer,
                         BufferLen,
                         mscUrbCallBack,
                         NULL);

    /* transport */
    return mscUsbTransport(mscDev, TimeOut);
}

/*
*******************************************************************************
*                     mscClearHalt
*
* Description:
*    清除ep不良状态
*
* Parameters:
*    mscDev  	:  input. 目标设备
*    Pipe		:  input. urb的pipe
*
* Return value:
*    返回URB的执行结果
*
* note:
*
*
*******************************************************************************
*/
static __s32 mscClearHalt(__mscDev_t *mscDev, __u32 Pipe)
{
    __s32 Result = 0;
    __u32 endp = usb_pipeendpoint(Pipe);

    if (usb_pipein (Pipe))
    {
        endp |= USB_DIR_IN;
    }

    Result = mscSendCtrlReq(mscDev,
                            mscDev->CtrlOut,
                            USB_REQ_CLEAR_FEATURE,
                            USB_RECIP_ENDPOINT,
                            USB_ENDPOINT_HALT,
                            (__u16)endp,
                            NULL,
                            0,
                            USB_STOR_CTRL_MSG_TIME);

    /* reset the endpoint toggle */
    if (Result >= 0)
    {
        usb_settoggle(mscDev->pusb_dev, usb_pipeendpoint(Pipe), usb_pipeout(Pipe), 0);
    }

    if(Result == 0)
    {
        Result = USB_ERR_SUCCESS;
    }
    else
    {
        Result = USB_ERR_UNKOWN_ERROR;
    }

    return Result;
}

/*
*******************************************************************************
*                     AnalyseBlukUrbState
*
* Description:
*    分析Bulk Urb的状态
*
* Parameters:
*    mscDev  	:  input. 目标设备
*    UrbState  	:  input. URB的执行状态
*    Pipe		:  input. urb的pipe
*    WantLen	:  input. 原本想传输的数据长度
*    ActLen  	:  input. 实际传输的数据长度
*
* Return value:
*    返回USB传输的结果
*
* note:
*
*
*******************************************************************************
*/
static __s32 AnalyseBulkUrbState(__mscDev_t *mscDev,
                                 __s32 UrbState,
                                 __s32 Pipe,
                                 __u32 WantLen,
                                 __u32 ActLen)
{
    switch(UrbState)
    {
    case 0:		/* no error code; did we send all the data? */
        if (WantLen != ActLen)
        {
            DMSG_PANIC("Wrn: short transfer, urb_state(%d), want_len(%d), real_len(%d)\n",
                       UrbState, WantLen, ActLen);
            return USB_STOR_XFER_SHORT;
        }

        return USB_STOR_XFER_GOOD;

    case -EPIPE:	/* stalled */
        DMSG_PANIC("ERR: ep stalled, need clear feature\n");

        /* for control endpoints, (used by CB[I]) a stall indicates a failed command */
        if(usb_pipecontrol(Pipe))
        {
            DMSG_PANIC("stall on control pipe, urb_state(%d), want_len(%d), real_len(%d)\n",
                       UrbState, WantLen, ActLen);
            return USB_STOR_XFER_STALLED;
        }

        /* for other sorts of endpoint, clear the stall */
        if (mscClearHalt(mscDev, Pipe) != USB_ERR_SUCCESS)
        {
            DMSG_PANIC("ERR: mscClearHalt ep failed, urb_state(%d), want_len(%d), real_len(%d)\n",
                       UrbState, WantLen, ActLen);
            return USB_STOR_XFER_ERROR;
        }

        return USB_STOR_XFER_STALLED;

    /* timeout or excessively long NAK */
    case -ETIMEDOUT:
        DMSG_PANIC("ERR: timeout or NAK, urb_state(%d), want_len(%d), real_len(%d)\n",
                   UrbState, WantLen, ActLen);
        return USB_STOR_XFER_TIME_OUT;

    /* babble - the device tried to send more than we wanted to read */
    case -EOVERFLOW:
        DMSG_PANIC("ERR: babble, data overflow, urb_state(%d), want_len(%d), real_len(%d)\n",
                   UrbState, WantLen, ActLen);
        return USB_STOR_XFER_LONG;

    /* the transfer was cancelled by abort, disconnect, or timeout */
    case -ECONNRESET:
        DMSG_PANIC("ERR: transfer cancelled, urb_state(%d), want_len(%d), real_len(%d)\n",
                   UrbState, WantLen, ActLen);
        return USB_STOR_XFER_ERROR;

    /* short scatter-gather read transfer */
    case -EREMOTEIO:
        DMSG_PANIC("ERR: short read transfer, urb_state(%d), want_len(%d), real_len(%d)\n",
                   UrbState, WantLen, ActLen);
        return USB_STOR_XFER_SHORT;

    /* abort or disconnect in progress */
    case -EIO:
        DMSG_PANIC("ERR: abort or disconnect in progress, urb_state(%d), want_len(%d), real_len(%d)\n",
                   UrbState, WantLen, ActLen);
        return USB_STOR_XFER_ERROR;

    default:
        DMSG_PANIC("ERR: unkown urb state, urb_state(%d), want_len(%d), real_len(%d)\n",
                   UrbState, WantLen, ActLen);
        return USB_STOR_XFER_ERROR;
    }
}

/*
*******************************************************************************
*                     mscSendBlukReq
*
* Description:
*    发送bulk请求
*
* Parameters:
*    mscDev  	:  input. 目标设备
*    Pipe		:  input. urb的pipe
*    Buffer		:  input. 缓冲区
*    BufferLen	:  input. 缓冲区大小
*    ActLen  	:  input. 实际传输的数据长度
*    TimeOut    :  input. URB超时时间
*
* Return value:
*    返回USB传输的结果
*
* note:
*
*
*******************************************************************************
*/
static __s32 mscSendBulkReq(__mscDev_t *mscDev,
                            __s32 Pipe,
                            void *Buffer,
                            __u32 BufferLen,
                            __u32 *ActLen,
                            __u32 TimeOut)
{
    /* fill urb */
    usb_fill_bulk_urb(mscDev->CurrentUrb,
                      mscDev->pusb_dev,
                      Pipe,
                      Buffer,
                      BufferLen,
                      mscUrbCallBack,
                      NULL);

    /* submit urb */
    mscUsbTransport(mscDev, TimeOut);
    if(ActLen)
    {
        *ActLen = (__u32)(mscDev->CurrentUrb->actual_length);
    }

    return AnalyseBulkUrbState(mscDev,
                               mscDev->CurrentUrb->status,
                               Pipe,
                               BufferLen,
                               (__u32)(mscDev->CurrentUrb->actual_length));
}

/*
*******************************************************************************
*                     GetMaxLun
*
* Description:
*    获得最大Lun个数，如果从设备获得失败，那么就默认设备只有1个Lun
*
* Parameters:
*    mscDev  :  input. 目标设备
*
* Return value:
*    Lun 个数
*
* note:
*
*
*******************************************************************************
*/
__u32 mscGetMaxLun(__mscDev_t *mscDev)
{
    __s32 ret = 0;
    __u32 MaxLun = 0;

    if(mscDev == NULL)
    {
        DMSG_PANIC("ERR: GetMaxLun: input error, mscDev = %x\n", mscDev);
        return 0;
    }

    UsbLock(mscDev->DevLock);

    /* issue the command */
    ret = mscSendCtrlReq(mscDev,
                         mscDev->CtrlIn,
                         USB_BULK_GET_MAX_LUN,
                         USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
                         0,
                         mscDev->InterfaceNo,
                         mscDev->iobuf,
                         1,
                         USB_STOR_CTRL_MSG_TIME);
    if (ret == 0)
    {
        DMSG_INFO("[msc]: GetMaxLUN successful, max lun is %d\n", mscDev->iobuf[0]);
        MaxLun = mscDev->iobuf[0];
    }
    else
    {
        DMSG_INFO("[msc]: GetMaxLUN failed, max lun is zero\n");
        MaxLun = 0;

        if (ret == -EPIPE)
        {
            mscClearHalt(mscDev, mscDev->BulkIn);
            mscClearHalt(mscDev, mscDev->BulkOut);
        }
    }

    UsbUnLock(mscDev->DevLock);

    return MaxLun;
}

/*
*******************************************************************************
*                     mscBoReset
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*
*
* note:
*
*
*******************************************************************************
*/
#if 0
static __s32 mscBoReset(__mscDev_t *mscDev)
{
    __s32 ret = 0;

    if(mscDev == NULL)
    {
        DMSG_PANIC("ERR: mscBoReset: input error, mscDev = %x\n", mscDev);
        return USB_ERR_BAD_ARGUMENTS;
    }

    /* send reset request */
    ret = mscSendCtrlReq(mscDev,
                         mscDev->CtrlOut,
                         USB_BULK_RESET_REQUEST,
                         USB_TYPE_CLASS | USB_RECIP_INTERFACE,
                         0,
                         mscDev->InterfaceNo,
                         NULL,
                         0,
                         USB_STOR_CTRL_MSG_TIME);
    if(ret != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: Soft reset failed 1\n");
        return ret;
    }

    /* clear ep status */
    ret = mscClearHalt(mscDev, mscDev->BulkIn);
    if(ret != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: Soft reset failed 2\n");
        return ret;
    }

    ret = mscClearHalt(mscDev, mscDev->BulkOut);
    if(ret != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: Soft reset failed 3\n");
        return ret;
    }

    return ret;
}
#endif

/*
*******************************************************************************
*                     mscPortReset
*
* Description:
*    reset 设备
*
* Parameters:
*    mscDev  :  input. 目标设备
*
* Return value:
*    返回成功或者失败
*
* note:
*
*
*******************************************************************************
*/
static __s32 mscPortReset(__mscDev_t *mscDev)
{
    __s32 ret = 0;

    if(mscDev == NULL)
    {
        DMSG_PANIC("ERR: mscPortReset: input error, mscDev = %x\n", mscDev);
        return USB_ERR_BAD_ARGUMENTS;
    }

    /* device online? */
    if(mscDev->state == MSC_DEV_OFFLINE)
    {
        DMSG_PANIC("ERR: mscPortReset: device is offline\n");
        return USB_ERR_IO_DEVICE_OFFLINE;
    }

    /* reset a multi-interface device must be wariness */
    if(mscDev->pusb_dev->actconfig->desc.bNumInterfaces != 1)
    {
        DMSG_PANIC("ERR: Refusing to reset a multi-interface device\n");
        return USB_ERR_IO_DEVICE_BUSY;
    }


    /* reset device */
    ret = usb_reset_device(mscDev->pusb_dev);
    if(ret != 0)
    {
        DMSG_PANIC("ERR: reset device failed\n");
        return USB_ERR_RESET_POERT_FAILED;
    }

    return USB_ERR_SUCCESS;
}

__s32 mscResetRecovery(__mscDev_t *mscDev)
{
    return mscPortReset(mscDev);
}

/*
*******************************************************************************
*                     mscBoTransport
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*
*
* note:
*
*
*******************************************************************************
*/
__s32 mscBoTransport(__mscDev_t *mscDev, __ScsiCmnd_t *ScsiCmnd)
{
    __CBW_t *CBW  = NULL;
    __CSW_t *CSW  = NULL;
    __u32 ActLen  = 0;
    __u32 TimeOut = 0;
    __u32 CSWRepeat = 0;
    __s32 Pipe    = 0;
    __s32 ret     = 0;

    if(mscDev == NULL || ScsiCmnd == NULL)
    {
        DMSG_PANIC("ERR: mscBoTransport: input error, mscDev = %x,ScsiCmnd = %x\n", mscDev, ScsiCmnd);
        return USB_STOR_TRANSPORT_ERROR;
    }

    /* is scsi command valid */
    if(ScsiCmnd->DataTransferLength)
    {
        switch(ScsiCmnd->cmnd.data_direction)
        {
        case DATA_FROM_DEVICE:
            Pipe = mscDev->BulkIn;
            break;

        case DATA_TO_DEVICE:
            Pipe = mscDev->BulkOut;
            break;

        default:
            Pipe = 0;
            DMSG_PANIC("ERR: data phase do not kwon data direction\n");
            return USB_STOR_TRANSPORT_ERROR;
        }
    }

    UsbLock(mscDev->DevLock);

    mscDev->ScsiCmnd = ScsiCmnd;

    //----------------------------------------------------
    //  Command Block Transport  (CBW)
    //----------------------------------------------------
    /* build the active CBW */
    CBW = (__CBW_t *)mscDev->iobuf;
    USB_OS_MEMSET(CBW, 0, sizeof(__CBW_t));

    mscDev->Tag++;
    TimeOut = USB_STOR_CBW_CSW_TIME;

    CBW->dCBWSignature 			= CBW_SIGNATURE;
    CBW->dCBWTag				= mscDev->Tag;
    CBW->dCBWDataTransferLength	= ScsiCmnd->DataTransferLength;
    CBW->bmCBWFlags				= (ScsiCmnd->cmnd.data_direction == DATA_FROM_DEVICE)
                                  ? CBW_FLAGS_DATA_IN : CBW_FLAGS_DATA_OUT;
    CBW->bCBWLUN				= ScsiCmnd->cmnd.dwLun;
    CBW->bCBWCBLength			= ScsiCmnd->cmnd.CBLen;
    USB_OS_MEMCPY((void *)CBW->CBWCB, ScsiCmnd->cmnd.CommandBlock, CBW->bCBWCBLength);

    /* Command Block Transport */
    ret = mscSendBulkReq(mscDev,
                         mscDev->BulkOut,
                         CBW,
                         sizeof(__CBW_t),
                         NULL,
                         TimeOut);
    if(ret != USB_STOR_XFER_GOOD)
    {
        DMSG_PANIC("ERR: mscBoTransport: Command Block Transport failed, reset\n");
        mscDev->ResetRecovery(mscDev);
        ret = USB_STOR_TRANSPORT_ERROR;
        goto TransportDone;
    }

    //----------------------------------------------------
    //  Data Transport
    //----------------------------------------------------
    if(ScsiCmnd->DataTransferLength)
    {
        TimeOut = ScsiCmnd->cmnd.Timeout;
        ret = mscSendBulkReq(mscDev,
                             Pipe,
                             ScsiCmnd->buffer,
                             ScsiCmnd->DataTransferLength,
                             &ActLen,
                             TimeOut);
        /* 有些设备在read/write命令的数据传输过程中，会出现ep stall。
           出现这种情况，仅仅clear feature是不够的，还得需要命令重传。
          */
        if(ret == USB_STOR_XFER_STALLED)
        {
            __u8  Command = 0;

            Command = ((__u8 *)(ScsiCmnd->cmnd.CommandBlock))[0];
            if(Command == SCSI_READ6 || Command == SCSI_READ10 || Command == SCSI_READ16
                    || Command == SCSI_WRITE6 || Command == SCSI_WRITE10 || Command == SCSI_WRITE16)
            {
                DMSG_PANIC("ERR: Command(%x) execute failed, for ep stall, need retry\n", Command);
                mscDev->ResetRecovery(mscDev);
                ret = USB_STOR_TRANSPORT_ERROR;
                goto TransportDone;
            }
        }

        if(ret == USB_STOR_XFER_ERROR)
        {
            DMSG_PANIC("ERR: mscBoTransport: Data Transport failed, reset\n");
            mscDev->ResetRecovery(mscDev);
            ret = USB_STOR_TRANSPORT_ERROR;
            goto TransportDone;
        }

        ScsiCmnd->ActualLength = ActLen;
    }

    //----------------------------------------------------
    //  Command Status Transport (CSW)
    //----------------------------------------------------
RETRY_CSW:
    if(ScsiCmnd->DataTransferLength)
    {
        TimeOut = USB_STOR_CBW_CSW_TIME;
    }
    else
    {
        TimeOut = ScsiCmnd->cmnd.Timeout;
    }

    CSW = (__CSW_t *)mscDev->iobuf;
    USB_OS_MEMSET(CSW, 0, sizeof(__CSW_t));

    ret = mscSendBulkReq(mscDev,
                         mscDev->BulkIn,
                         CSW,
                         sizeof(__CSW_t),
                         &ActLen,
                         TimeOut);
    if(ret == USB_STOR_XFER_SHORT && ActLen == 0 && ++CSWRepeat < MAX_BOT_CSW_REPEAT)
    {
        DMSG_PANIC("ERR: Received 0-length CSW, must Receive the CSW again\n");
        goto RETRY_CSW;
    }

    if(ret == USB_STOR_XFER_STALLED && ++CSWRepeat < MAX_BOT_CSW_REPEAT)
    {
        DMSG_PANIC("ERR: EP stall, must Receive the CSW again\n");
        goto RETRY_CSW;
    }

    if(ret != USB_STOR_XFER_GOOD)
    {
        DMSG_PANIC("ERR: unkown error happen during receive cmd(%x)'s CSW, must reset\n",
                   ((__u8 *)(ScsiCmnd->cmnd.CommandBlock))[0]);
        mscDev->ResetRecovery(mscDev);
        ret = USB_STOR_TRANSPORT_ERROR;
        goto TransportDone;
    }

    /* check CSW size */
    if(sizeof(__CSW_t) != ActLen)
    {
        DMSG_PANIC("ERR: Invalid Csw size: (%d, %d), must reset\n", sizeof(__CSW_t), ActLen);
        mscDev->ResetRecovery(mscDev);
        ret = USB_STOR_TRANSPORT_ERROR;
        goto TransportDone;
    }

    /* check CSW Signature */
    if(CSW->dCSWSignature != CSW_SIGNATURE)
    {
        DMSG_PANIC("ERR: Invalid Csw Signature: %x, must reset\n", CSW->dCSWSignature);
        mscDev->ResetRecovery(mscDev);
        ret = USB_STOR_TRANSPORT_ERROR;
        goto TransportDone;
    }

    /* check CSW Tags */
    if(CSW->dCSWTag != mscDev->Tag)
    {
        DMSG_PANIC("ERR: Invalid Csw Tags: (%x, %x), must reset\n", mscDev->Tag, CSW->dCSWTag);
        mscDev->ResetRecovery(mscDev);
        ret = USB_STOR_TRANSPORT_ERROR;
        goto TransportDone;
    }

    /* check CSW Status, command failed, need sense */
    if(CSW->bCSWStatus == 1)
    {
        DMSG_MSC_TEST("WRN: command(%x) failed, need sense\n", ((__u8 *)(ScsiCmnd->cmnd.CommandBlock))[0]);
        ret = USB_STOR_TRANSPORT_FAILED;
        goto TransportDone;
    }

    /* check CSW Status, Phase Error */
    if(CSW->bCSWStatus == 2)
    {
        DMSG_PANIC("ERR: command Phase Error, must reset\n");
        ret = USB_STOR_TRANSPORT_ERROR;
        goto TransportDone;
    }

    mscDev->ScsiCmnd = NULL;
    UsbUnLock(mscDev->DevLock);

    return USB_STOR_TRANSPORT_GOOD;

TransportDone:
    mscDev->ScsiCmnd = NULL;
    UsbUnLock(mscDev->DevLock);

    return ret;
}


/*
*******************************************************************************
*                     mscBoStopTransport
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*
*
* note:
*
*
*******************************************************************************
*/
__s32 mscBoStopTransport(__mscDev_t *mscDev)
{
    if(mscDev == NULL)
    {
        DMSG_PANIC("ERR: mscBoStopTransport: input error, mscDev = %x\n", mscDev);
        return USB_ERR_BAD_ARGUMENTS;
    }

    if(mscDev->TimerHdle)
    {
        USB_esKRNL_TmrStop(mscDev->TimerHdle);
        USB_esKRNL_TmrDel(mscDev->TimerHdle);
        mscDev->TimerHdle = NULL;
    }

    if(mscDev->busy)
    {
        usb_unlink_urb(mscDev->CurrentUrb);
        mscDev->CurrentUrb->status = -ESHUTDOWN;
    }

    return USB_ERR_SUCCESS;
}



