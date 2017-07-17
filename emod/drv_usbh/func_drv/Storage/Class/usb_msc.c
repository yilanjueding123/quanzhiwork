/*
********************************************************************************************************************
*                                              usb host driver
*
*                              (c) Copyright 2007-2010, javen.China
*										All	Rights Reserved
*
* File Name 	: usb_msc.c
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

#include  "usb_utils_find_zero_bit.h"
#include  "usb_list.h"
#include  "list_head_ext.h"

#include  "usb_host_common.h"
#include  "usb_gen_dev_mod.h"
#include  "urb.h"
#include  "usb_core_interface.h"

#include  "usbh_disk_info.h"
#include  "usbh_buff_manager.h"
#include  "usbh_disk_remove_time.h"

#include  "usb_msc_i.h"
#include  "mscTransport.h"
#include  "mscProtocol.h"
#include  "LunMgr.h"
#include  "usb_msc.h"

#define  MSC_DRV_NAME		"Mass Storage Class"
#define  MSC_DRV_AUTHOR		"Host Driver Author"

static struct usb_host_func_drv mscDrv; 	/*  */
static __u32 mscDev_id_array;				/* 记录了mscDev的编号 */

static struct usb_drv_dev_match_table msc_match_table [] =
{
    //--1--CB
    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_RBC
        , USBMSC_INTERFACE_PROTOCOL_CBT
        , 0	//driver_info;
    },
    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_8020
        , USBMSC_INTERFACE_PROTOCOL_CBT
        , 0	//driver_info;
    },
    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_QIC
        , USBMSC_INTERFACE_PROTOCOL_CBT
        , 0	//driver_info;
    },

    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_UFI
        , USBMSC_INTERFACE_PROTOCOL_CBT
        , 0	//driver_info;
    },
    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_8070
        , USBMSC_INTERFACE_PROTOCOL_CBT
        , 0	//driver_info;
    },
    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_SCSI
        , USBMSC_INTERFACE_PROTOCOL_CBT
        , 0	//driver_info;
    },
    //--2--CBI
    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_RBC
        , USBMSC_INTERFACE_PROTOCOL_CBIT
        , 0	//driver_info;
    },
    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_8020
        , USBMSC_INTERFACE_PROTOCOL_CBIT
        , 0	//driver_info;
    },
    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_QIC
        , USBMSC_INTERFACE_PROTOCOL_CBIT
        , 0	//driver_info;
    },

    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_UFI
        , USBMSC_INTERFACE_PROTOCOL_CBIT
        , 0	//driver_info;
    },
    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_8070
        , USBMSC_INTERFACE_PROTOCOL_CBIT
        , 0	//driver_info;
    },
    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_SCSI
        , USBMSC_INTERFACE_PROTOCOL_CBIT
        , 0	//driver_info;
    },
    //--3--BO
    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_RBC
        , USBMSC_INTERFACE_PROTOCOL_BOT
        , 0	//driver_info;
    },
    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_8020
        , USBMSC_INTERFACE_PROTOCOL_BOT
        , 0	//driver_info;
    },
    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_QIC
        , USBMSC_INTERFACE_PROTOCOL_BOT
        , 0	//driver_info;
    },

    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_UFI
        , USBMSC_INTERFACE_PROTOCOL_BOT
        , 0	//driver_info;
    },
    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_8070
        , USBMSC_INTERFACE_PROTOCOL_BOT
        , 0	//driver_info;
    },
    {
        USB_DEVICE_ID_MATCH_INT_INFO	//match_flags;
        , 0, 0, 0, 0     , 0, 0, 0
        //bInterfaceClass;bInterfaceSubClass;bInterfaceProtocol
        , USB_CLASS_MASS_STORAGE
        , USBMSC_SUBCLASS_SCSI
        , USBMSC_INTERFACE_PROTOCOL_BOT
        , 0	//driver_info;
    },
    {
        0
        , 0, 0, 0, 0     , 0, 0, 0
        , 0
        , 0
        , 0

        , 0
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
*******************************************************************************
*                     get_mscDev_id
*
* Description:
*    给新插入的设备分配ID
*
* Parameters:
*    void
*
* Return value:
*    返回分配的ID
*
* note:
*    无
*
*******************************************************************************
*/
static __u32 get_mscDev_id(void)
{
    __u32 bit = 0;

    bit = find_next_zero_bit(&mscDev_id_array, (1 * 32), 0);
    if(bit > 32)
    {
        DMSG_PANIC("ERR: find_next_zero_bit failed\n");
        return 31;
    }

    set_bit(mscDev_id_array, bit);

    return bit;
}

/*
*******************************************************************************
*                     free_mscDev_id
*
* Description:
*    释放分配的ID
*
* Parameters:
*    bit : input.  mscDev的ID号
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
static __s32 free_mscDev_id(__u32 bit)
{
    if(test_bit(mscDev_id_array, bit) == 0)
    {
        DMSG_PANIC("ERR: free_host_id: invalid bit(%d)\n", bit);
        return -1;
    }

    /* 回收该位 */
    clear_bit(mscDev_id_array, bit);

    return 0;
}

/*
*******************************************************************************
*                     mscDevInit
*
* Description:
*
*
* Parameters:
*    mscDev  ： input. msc设备信息
*    intf    :  input. USB接口
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
static __s32 mscDevInit(__mscDev_t *mscDev, struct usb_interface *intf)
{
    __s32 ret = 0;
    __u8 err = 0;

    if(mscDev == NULL)
    {
        DMSG_PANIC("ERR: invalid argment\n");
        return -1;
    }

    mscDev->pusb_dev 	= usb_mod_interface_to_usbdev(intf);
    if(mscDev->pusb_dev == NULL)
    {
        DMSG_PANIC("ERR: invalid argment\n");
        return -1;
    }

    mscDev->pusb_intf 	= intf;
    mscDev->InterfaceNo = intf->cur_altsetting->desc.bInterfaceNumber;

    /* Store our private data in the interface */
    usb_mod_usb_set_intf_priv_data(intf, (void *)mscDev);

    mscDev->mscDevNo	= get_mscDev_id();
    INIT_LIST_HEAD(&(mscDev->cmd_list));

    /* alloc urb */
    mscDev->CurrentUrb = usb_alloc_urb(0);
    if(mscDev->CurrentUrb == NULL)
    {
        DMSG_PANIC("ERR: USB_OS_SemCreate scan_lock failed\n");
        return USB_ERR_ALLOC_URB_FAILED;
    }

    /* 一把钥匙 */
    mscDev->scan_lock = USB_OS_SemCreate(1);
    if(mscDev->scan_lock == NULL)
    {
        DMSG_PANIC("ERR: USB_OS_SemCreate scan_lock failed\n");
        ret = USB_ERR_CREATE_SIME_FAILED;
        goto error0;
    }

    mscDev->DevLock = USB_OS_SemCreate(1);
    if(mscDev->DevLock == NULL)
    {
        DMSG_PANIC("ERR: USB_OS_SemCreate DevLock failed\n");
        ret = USB_ERR_CREATE_SIME_FAILED;
        goto error1;
    }

    /* 线程同步 */
    mscDev->ThreadSemi = USB_OS_SemCreate(0);
    if(mscDev->ThreadSemi == NULL)
    {
        DMSG_PANIC("ERR: USB_OS_SemCreate dev_lock failed\n");
        ret = USB_ERR_CREATE_SIME_FAILED;
        goto error2;
    }

    mscDev->notify_complete = USB_OS_SemCreate(0);
    if(mscDev->notify_complete == NULL)
    {
        DMSG_PANIC("ERR: USB_OS_SemCreate notify_complete failed\n");
        ret = USB_ERR_CREATE_SIME_FAILED;
        goto error3;
    }

    mscDev->CtrlReq = (struct usb_ctrlrequest *)usb_buffer_alloc(mscDev->pusb_dev,
                      sizeof(struct usb_ctrlrequest),
                      0,
                      NULL);
    if(mscDev->CtrlReq == NULL)
    {
        DMSG_PANIC("ERR: usb_buffer_alloc CtrlReq failed\n");
        ret = USB_ERR_MALLOC_FAILED;
        goto error4;
    }

    mscDev->iobuf = (__u8 *)usb_buffer_alloc(mscDev->pusb_dev, MSC_IOBUF_SIZE, 0, NULL);
    if(mscDev->iobuf == NULL)
    {
        DMSG_PANIC("ERR: usb_buffer_alloc iobuf failed\n");
        ret = USB_ERR_MALLOC_FAILED;
        goto error5;
    }

    mscDev->UrbWait = USB_OS_SemCreate(0);
    if(mscDev->UrbWait == NULL)
    {
        DMSG_PANIC("ERR: USB_OS_SemCreate UrbWait failed\n");
        ret = USB_ERR_CREATE_SIME_FAILED;
        goto error6;
    }

    return USB_ERR_SUCCESS;

error6:
    usb_buffer_free(mscDev->pusb_dev, MSC_IOBUF_SIZE, (void *)(mscDev->iobuf), 0);

error5:
    usb_buffer_free(mscDev->pusb_dev, sizeof(*mscDev->CtrlReq), (void *)(mscDev->CtrlReq), 0);

error4:
    USB_OS_SemDel(mscDev->notify_complete, &err);

error3:
    USB_OS_SemDel(mscDev->ThreadSemi, &err);

error2:
    USB_OS_SemDel(mscDev->DevLock, &err);

error1:
    USB_OS_SemDel(mscDev->scan_lock, &err);

error0:
    usb_free_urb(mscDev->CurrentUrb);

    return ret;
}

/*
*******************************************************************************
*                     mscDevFree
*
* Description:
*
*
* Parameters:
*    mscDev  ： input. msc设备信息
*
* Return value:
*    无
*
* note:
*    无
*
*******************************************************************************
*/
static void mscDevFree(__mscDev_t *mscDev)
{
    __u8 err = 0;

    if(mscDev->ThreadSemi)
    {
        USB_OS_SemDel(mscDev->ThreadSemi, &err);
    }

    if(mscDev->scan_lock)
    {
        USB_OS_SemDel(mscDev->scan_lock, &err);
    }

    if(mscDev->notify_complete)
    {
        USB_OS_SemDel(mscDev->notify_complete, &err);
    }

    if(mscDev->UrbWait)
    {
        USB_OS_SemDel(mscDev->UrbWait, &err);
    }

    if(mscDev->CtrlReq)
    {
        usb_buffer_free(mscDev->pusb_dev, sizeof(*mscDev->CtrlReq), (void *)(mscDev->CtrlReq), 0);
    }

    if(mscDev->iobuf)
    {
        usb_buffer_free(mscDev->pusb_dev, MSC_IOBUF_SIZE, (void *)(mscDev->iobuf), 0);
    }

    usb_free_urb(mscDev->CurrentUrb);

    /* Remove our private data from the interface */
    usb_mod_usb_set_intf_priv_data(mscDev->pusb_intf, NULL);

    free_mscDev_id(mscDev->mscDevNo);
}

/*
*******************************************************************************
*                     SetMscDevState
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*    无
*
* note:
*    无
*
*******************************************************************************
*/
static void SetMscDevState(__mscDev_t *mscDev, mscDev_state_t state)
{
    __cpu_sr cup_sr	= 0;

    USB_OS_ENTER_CRITICAL(cup_sr);
    mscDev->state = state;
    USB_OS_EXIT_CRITICAL(cup_sr);
}

/*
*******************************************************************************
*                     mscDevOnline
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*    真/假
*
* note:
*    无
*
*******************************************************************************
*/
__u32 mscDevOnline(__mscDev_t *mscDev)
{
    return (mscDev->state == MSC_DEV_ONLINE);
}

/*
*******************************************************************************
*                     GetDeviceInfo
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*    无
*
* note:
*    无
*
*******************************************************************************
*/
static void GetDeviceInfo(__mscDev_t *mscDev, __u32 index)
{
    struct usb_interface_descriptor *idesc = &(mscDev->pusb_intf->cur_altsetting->desc);

    mscDev->SubClass = idesc->bInterfaceSubClass ;
    mscDev->Protocol = idesc->bInterfaceProtocol ;

    return;
}

/*
*******************************************************************************
*                     GetTransport
*
* Description:
*    获得msc设备的传输方式
*
* Parameters:
*    mscDev  ： input. msc设备信息
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
static __s32 GetTransport(__mscDev_t *mscDev)
{
    switch(mscDev->Protocol)
    {
    case USBMSC_INTERFACE_PROTOCOL_BOT:
        mscDev->transport_name  = "BulkOnly";
        mscDev->ResetRecovery 	= mscResetRecovery;
        mscDev->Transport 		= mscBoTransport;
        mscDev->StopTransport 	= mscBoStopTransport;
        break;

    default:
        DMSG_PANIC("ERR: not support transport Protocol(%x)\n", mscDev->Protocol);
        mscDev->transport_name  = "unkown";
        mscDev->ResetRecovery 	= NULL;
        mscDev->Transport 		= NULL;
        mscDev->StopTransport 	= NULL;

        return USB_ERR_UNKOWN_ERROR;
    }

    return USB_ERR_SUCCESS;
}

/*
*******************************************************************************
*                     GetTransport
*
* Description:
*    获得msc设备的传输方式
*
* Parameters:
*    mscDev  ： input. msc设备信息
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
static __s32 GetProtocol(__mscDev_t *mscDev)
{
    switch(mscDev->SubClass)
    {
    case USBMSC_SUBCLASS_RBC:
        mscDev->Protocol_name	= "Reduced Block Commands (RBC)";
        mscDev->ProtocolHandler = mscScsiCommand;
        break;

    case USBMSC_SUBCLASS_8020:
        mscDev->Protocol_name	= "8020i";
        mscDev->ProtocolHandler = mscATAPICommand;
        break;

    case USBMSC_SUBCLASS_QIC:
        mscDev->Protocol_name	= "QIC-157";
        mscDev->ProtocolHandler = mscQIC157Command;
        break;

    case USBMSC_SUBCLASS_UFI:
        mscDev->Protocol_name	= "Uniform Floppy Interface (UFI)";
        mscDev->ProtocolHandler = mscUFICommand;
        break;

    case USBMSC_SUBCLASS_8070:
        mscDev->Protocol_name	= "8070i";
        mscDev->ProtocolHandler = mscQIC157Command;
        break;

    case USBMSC_SUBCLASS_SCSI:
        mscDev->Protocol_name	= "Reduced Block Commands (RBC)";
        mscDev->ProtocolHandler = mscScsiCommand;
        break;

    default:
        DMSG_PANIC("ERR: not support transport SubClass(%x)\n", mscDev->SubClass);
        return USB_ERR_UNKOWN_ERROR;
    }

    return USB_ERR_SUCCESS;
}

/*
*******************************************************************************
*                     CreatePipes
*
* Description:
*    创建传输管道
*
* Parameters:
*    mscDev  ： input. msc设备信息
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
static __s32 CreatePipes(__mscDev_t *mscDev)
{
    struct usb_host_virt_interface *altsetting = mscDev->pusb_intf->cur_altsetting;
    struct usb_endpoint_descriptor *ep 		= NULL;
    struct usb_endpoint_descriptor *ep_in 	= NULL;
    struct usb_endpoint_descriptor *ep_out 	= NULL;
    struct usb_endpoint_descriptor *ep_int 	= NULL;
    int i = 0;

    /*
     * Find the endpoints we need.
     * We are expecting a minimum of 2 endpoints - in and out (bulk).
     * An optional interrupt is OK (necessary for CBI protocol).
     * We will ignore any others.
     */
    for (i = 0; i < altsetting->desc.bNumEndpoints; i++)
    {
        ep = &altsetting->endpoint[i].desc;

        /* Is it a BULK endpoint? */
        if ((ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
                == USB_ENDPOINT_XFER_BULK)
        {
            /* BULK in or out? */
            if (ep->bEndpointAddress & USB_DIR_IN)
                ep_in = ep;
            else
                ep_out = ep;
        }

        /* Is it an interrupt endpoint? */
        else if ((ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
                 == USB_ENDPOINT_XFER_INT)
        {
            ep_int = ep;
        }
    }

    if (!ep_in || !ep_out || (mscDev->Protocol == USBMSC_INTERFACE_PROTOCOL_CBIT && !ep_int))
    {
        DMSG_PANIC("Endpoint sanity check failed! Rejecting dev.\n");
        return USB_ERR_UNKOWN_ERROR;
    }

    /* Calculate and store the pipe values */
    mscDev->CtrlOut = usb_sndctrlpipe(mscDev->pusb_dev, 0);
    mscDev->CtrlIn  = usb_rcvctrlpipe(mscDev->pusb_dev, 0);
    mscDev->BulkOut = usb_sndbulkpipe(mscDev->pusb_dev,
                                      ep_out->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK);
    mscDev->BulkIn  = usb_rcvbulkpipe(mscDev->pusb_dev,
                                      ep_in->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK);
    if(ep_int)
    {
        mscDev->IntIn = usb_rcvintpipe(mscDev->pusb_dev,
                                       ep_int->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK);
        mscDev->EpInterval = ep_int->bInterval;
    }

    return USB_ERR_SUCCESS;
}

/*
*******************************************************************************
*                     mscDevAdd
*
* Description:
*
*
* Parameters:
*    mscDev  ： input. msc设备信息
*
* Return value:
*    无
*
* note:
*    无
*
*******************************************************************************
*/
static void mscDevAdd(__mscDev_t *mscDev)
{
    __u32 i = 0;
    __s32 ret = 0;

    if(mscDev == NULL)
    {
        DMSG_PANIC("ERR: mscDevAdd input error\n");
        return;
    }

    __wrn("mscDev->MaxLun=%d\n", mscDev->MaxLun);
    /* probe lun */
    for(i = 0; i < mscDev->MaxLun; i++)
    {
        __mscLun_t *mscLun = NULL;

        mscLun = mscLun_alloc();
        if(mscLun == NULL)
        {
            DMSG_PANIC("ERR: MscLun_alloc failed\n");
            return;
        }

        mscLun->mscDev 		 = mscDev;
        mscLun->LunNo 		 = i;
        mscLun->DiskSubClass = mscDev->SubClass;
        mscLun->RemoveAble   = 1;    /* 默认就是可移动设备 */

        mscDev->Lun[i] 	= mscLun;

        __wrn("begin mscLunAdd\n");
        ret = mscLunAdd(mscLun);
        __wrn("end mscLunAdd\n");
        if(ret != USB_ERR_SUCCESS)
        {
            DMSG_PANIC("ERR: probe Lun(%d) failed\n", i);
            continue;
        }

        /* 如果设备是非移动设备, 就delay */
        if(mscLun->RemoveAble == 0)
        {
            mscDev->SuspendTime = 10000; /* 10s */
        }
        else
        {
            mscDev->SuspendTime = 0;
        }

        usbh_disk_save_time(mscDev->SuspendTime);
    }

    return;
}

/*
*******************************************************************************
*                     mscDevDel
*
* Description:
*
*
* Parameters:
*    mscDev  ： input. msc设备信息
*
* Return value:
*    无
*
* note:
*    无
*
*******************************************************************************
*/
static void mscDevDel(__mscDev_t *mscDev)
{
    __u32 i = 0;

    if(mscDev == NULL)
    {
        DMSG_PANIC("ERR: mscDevDel input error\n");
        return;
    }

    /* remove lun */
    for(i = 0; i < mscDev->MaxLun; i++)
    {
        __mscLun_t *mscLun = NULL;

        mscLun = mscDev->Lun[i];
        if(mscLun == NULL)
        {
            DMSG_PANIC("ERR: mscLun == NULL\n");
            return;
        }

        mscLunDel(mscLun);

        mscLun_free(mscLun);

        usbh_disk_del_time(mscDev->SuspendTime);
    }

    return;
}

/*
*******************************************************************************
*                     MediaChangeThread
*
* Description:
*    介质检测线程
*
* Parameters:
*    p_arg  ： input. 即mscDev, msc设备信息
*
* Return value:
*    无
*
* note:
*    无
*
*******************************************************************************
*/
static void MediaChangeThread(void *p_arg)
{
    __mscDev_t *mscDev = (__mscDev_t *)p_arg;

    USB_OS_SemPost(mscDev->notify_complete);

    while(1)
    {
        __u32 i = 0;

        //--<1>--杀死线程
        TryToKillThreadSelf("MediaChangeThread");

        for(i = 0; i < mscDev->MaxLun; i++)
        {
            __mscLun_t *mscLun = NULL;

            mscLun = mscDev->Lun[i];
            if(mscLun == NULL)
            {
                DMSG_PANIC("ERR: MediaChangeThread: mscLun is NULL\n");
                continue;
            }

            if(mscLun->MediaChange)
            {
                mscLun->MediaChange(mscLun);
            }
        }

        /* delay 1.25s */
        USB_OS_DELAY_x_MS(1250);
    }
}

/*
*******************************************************************************
*                     mscDevScanThread
*
* Description:
*    mscDev扫描
*
* Parameters:
*    p_arg  ： input. 即mscDev, msc设备信息
*
* Return value:
*    无
*
* note:
*    无
*
*******************************************************************************
*/
static void mscDevScanThread(void *p_arg)
{
    __mscDev_t *mscDev = (__mscDev_t *)p_arg;

    mscDevAdd(mscDev);
    USB_OS_SemPost(mscDev->notify_complete);

    KillThreadSelf();
}


/*
*******************************************************************************
*                     QueueCmnd
*
* Description:
*    发送命令到Lun
*
* Parameters:
*    mscLun  	： input. 命令对应的Lun
*    scsi_cmnd 	:  input. 命令
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
__s32 mscDevQueueCmnd(__mscLun_t *mscLun, __ScsiCmnd_t *ScsiCmnd)
{
    __mscDev_t *mscDev = NULL;
    __cpu_sr sr = 0 ;

    if(ScsiCmnd == NULL)
    {
        DMSG_PANIC("ERR: invalid argment\n");
        return USB_ERR_BAD_ARGUMENTS;
    }

    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: MscTransport: input error, mscLun = %x, ScsiCmnd = %x\n", mscLun, ScsiCmnd);
        ScsiCmnd->Result = USB_STOR_TRANSPORT_ERROR;
        return USB_ERR_BAD_ARGUMENTS;
    }

    DMSG_MSC_TEST("mscDevQueueCmnd: cmnd = %x\n", ((__u8 *)(ScsiCmnd->cmnd.CommandBlock))[0]);

    mscDev = mscLun->mscDev;
    if(mscDev == NULL)
    {
        DMSG_PANIC("ERR: MscTransport: input error, mscDev == NULL\n");
        ScsiCmnd->Result = USB_STOR_TRANSPORT_ERROR;
        return USB_ERR_BAD_ARGUMENTS;
    }

    /* if mscDev is Busy, then all cmd to cmd_list */
    if(!mscDevOnline(mscDev))
    {
        DMSG_PANIC("ERR: mscDevice is %s\n",
                   (mscDev->state == MSC_DEV_OFFLINE) ? "offline" : "died");
        ScsiCmnd->Result = USB_STOR_TRANSPORT_ERROR;
        return USB_ERR_IO_DEVICE_OFFLINE;
    }

    /* add command to cmd_list */
    USB_OS_ENTER_CRITICAL(sr);
    list_head_malloc_and_add((void *)ScsiCmnd, &(mscDev->cmd_list));
    USB_OS_EXIT_CRITICAL(sr);

    /* wakeup mscDev main thread */
    UsbThreadWakeUp(mscDev->ThreadSemi);

    return USB_ERR_SUCCESS;
}

/*
*******************************************************************************
*                     mscCmndDone
*
* Description:
*    决策命令的去向，重发还是完成
*
* Parameters:
*    mscLun  		： input. 命令对应的Lun
*    scsi_cmnd 		:  input. 命令
* 	 TransStatus 	:  input. 传输状态
*
* Return value:
*    无
*
* note:
*    无
*
*******************************************************************************
*/
static void mscCmndDone(__mscDev_t *mscDev, __ScsiCmnd_t *ScsiCmnd, __s32 TransStatus)
{
    __cpu_sr sr = 0 ;

    if(mscDev->state != MSC_DEV_OFFLINE)
    {
        /* need retry? */
        if(TransStatus == USB_STOR_TRANSPORT_ERROR && (ScsiCmnd->retries < ScsiCmnd->allowed))
        {
            if(mscGetDataTransportReason(ScsiCmnd->sc_lun, ScsiCmnd) == USB_ERR_COMMAND_NEED_RETRY)
            {
                ScsiCmnd->retries++;
                DMSG_PANIC("ERR: do command(%x) failed, need retry, retries = %d, ScsiCmnd->allowed = %d\n",
                           ((__u8 *)(ScsiCmnd->cmnd.CommandBlock))[0], ScsiCmnd->retries, ScsiCmnd->allowed);
                return ;
            }
            else
            {
                DMSG_PANIC("ERR: mscCmndDone: media is not present\n");
            }
        }

        DMSG_MSC_TEST("mscCmndDone: cmnd = %x\n", ((__u8 *)(ScsiCmnd->cmnd.CommandBlock))[0]);

        /* retries = allowed = 0 ? */
        if(ScsiCmnd->retries > ScsiCmnd->allowed)
        {
            DMSG_PANIC("ERR: ScsiCmnd(%x) retry %d times, maybe device is died\n",
                       ((__u8 *)(ScsiCmnd->cmnd.CommandBlock))[0], ScsiCmnd->retries);

            /* 即使命令执行多次失败也不能认为设备坏了, 因为设备可能不支持这个命令 */
        }
    }
    else
    {
        DMSG_PANIC("ERR: mscCmndDone: device is offline\n");
    }

    /* Done command */
    USB_OS_ENTER_CRITICAL(sr);
    list_head_unlink_and_del(mscDev->cmd_list.next);
    ScsiCmnd->Result = TransStatus;
    USB_OS_EXIT_CRITICAL(sr);

    ScsiCmnd->Done(ScsiCmnd);

    return ;
}

/*
*******************************************************************************
*                     mscMainTread
*
* Description:
*    主线程，完成命令的传输
*
* Parameters:
*    p_arg  ： input. 即mscDev, msc设备信息
*
* Return value:
*    无
*
* note:
*    无
*
*******************************************************************************
*/
static void mscMainTread(void *p_arg)
{
    __mscDev_t *mscDev = (__mscDev_t *)p_arg;
    __ScsiCmnd_t *ScsiCmnd = NULL;
    __s32 TransStatus = 0;
    __cpu_sr sr = 0 ;

    USB_OS_SemPost(mscDev->notify_complete);

    while(1)
    {
        //--<1>--杀死线程
        TryToKillThreadSelf("mscMainTread");

        /* sleep */
        UsbThreadSleep(mscDev->ThreadSemi);

        /* cmd_list is empty?
         * mscDevQueueCmnd把cmd加到cmd_list，刚好遇到mscMainTread处理完命令，这样
         * for循环检测到cmd_list不空就会去执行下一条命令。执行完毕后cmd_list空了。
         * 此时UsbThreadWakeUp唤醒mscMainTread，但是cmd_list已经空了。
         */
        if(list_empty(&(mscDev->cmd_list)))
        {
            DMSG_PANIC("Wrn: mscDev cmd_list is empty\n");
            continue;
        }

        for( ; !list_empty(&(mscDev->cmd_list)); )
        {
            /* Get command */
            ScsiCmnd = (__ScsiCmnd_t *)mscDev->cmd_list.next->data;
            if(ScsiCmnd == NULL)
            {
                DMSG_PANIC("Wrn: mscDev cmd_list is not empty, but ScsiCmnd is NULL\n");

                USB_OS_ENTER_CRITICAL(sr);
                list_head_unlink_and_del(mscDev->cmd_list.next);
                USB_OS_EXIT_CRITICAL(sr);

                continue;
            }

            /* transport */
            if(mscDev->ProtocolHandler)
            {
                TransStatus = mscDev->ProtocolHandler(mscDev, ScsiCmnd);
            }
            else
            {
                DMSG_PANIC("ERR: mscDev ProtocolHandler is NULL\n");
                ScsiCmnd->Result = USB_STOR_TRANSPORT_ERROR;
                TransStatus = USB_STOR_TRANSPORT_GOOD;
            }

            /* done cmnd */
            mscCmndDone(mscDev, ScsiCmnd, TransStatus);
        }
    }
}

/*
*******************************************************************************
*                     mscDevProbe
*
* Description:
*    创建传输管道
*
* Parameters:
*    intf  		： input. USB接口信息
*    table_item :  input. 匹配表
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
static __s32 mscDevProbe(struct usb_interface *intf, const struct usb_drv_dev_match_table *table_item)
{
    const s32 match_table_index = table_item - msc_match_table;
    __mscDev_t *mscDev = NULL;
    __s32 ret = 0;
    __u8 err  = 0;
    __u8 mscDev_Scan_Id = 0;

    DMSG_INFO("mscDevProbe begin\n");

    if(intf == NULL || table_item == NULL)
    {
        DMSG_PANIC("ERR: mscDevProbe: input error\n");
        return -1;
    }

    //----------------------------------------------------------------
    //   初始化mscDev
    //----------------------------------------------------------------

    /* 初始化一个mscDev */
    mscDev = (__mscDev_t *)USB_OS_MALLOC(sizeof(__mscDev_t), USB_MEM_FILE_TRIGGER, USB_MEM_LINE_TRIGGER );
    if(mscDev == NULL)
    {
        DMSG_PANIC("ERR: USB_OS_MALLOC failed\n");
        ret = -1;
        goto error0;
    }

    USB_OS_MEMSET(mscDev, 0, sizeof(__mscDev_t));

    ret = mscDevInit(mscDev, intf);
    if(ret != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: mscDevInit failed\n");
        ret = -1;
        goto error1;
    }

    GetDeviceInfo(mscDev, match_table_index);

    /* 获得SubClass */
    ret = GetProtocol(mscDev);
    if(ret != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: GetProtocol failed\n");
        ret = -1;
        goto error2;
    }

    /* 获得Transport */
    ret = GetTransport(mscDev);
    if(ret != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: GetTransport failed\n");
        ret = -1;
        goto error3;
    }

    /* 创建传输管道 */
    ret = CreatePipes(mscDev);
    if(ret != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: CreatePipes failed\n");
        ret = -1;
        goto error4;
    }

    //----------------------------------------------------------------
    //   识别mscDev
    //----------------------------------------------------------------

    SetMscDevState(mscDev, MSC_DEV_ONLINE);

    /* 获得MaxLun */
    mscDev->MaxLun = mscGetMaxLun(mscDev) + 1;
    if(mscDev->MaxLun > MSC_MAX_LUN)
    {
        DMSG_PANIC("ERR: MaxLun = %d, mscDrv only support %d Lun\n", mscDev->MaxLun, MSC_MAX_LUN);
        mscDev->MaxLun = MSC_MAX_LUN;
    }

    /* msc main thread */
    mscDev->MainThreadId = UsbCreateThread((void *)mscMainTread,
                                           (void *)mscDev,
                                           0x4000,
                                           THREAD_LEVEL_MSC_MAIN);
    if(mscDev->MainThreadId == OS_NO_ERR)
    {
        DMSG_PANIC("ERR: create MainThreadId failed\n");
        ret = -1;
        goto error5;
    }

    USB_OS_SemPend(mscDev->notify_complete, 0, &err);

    /* mscDev scan thread */
    mscDev_Scan_Id = UsbCreateThread((void *)mscDevScanThread,
                                     (void *)mscDev,
                                     0x4000,
                                     THREAD_LEVEL_MSC_SCAN);
    if(mscDev_Scan_Id == OS_NO_ERR)
    {
        DMSG_PANIC("ERR: create mscDevScanThread failed\n");
        ret = -1;
        goto error6;
    }

    USB_OS_SemPend(mscDev->notify_complete, 0, &err);

    __wrn("mscDevScanThread end...\n");

    /* media change thread */
    mscDev->MediaChangeId = UsbCreateThread((void *)MediaChangeThread,
                                            (void *)mscDev,
                                            0x4000,
                                            THREAD_LEVEL_MSC_MEDIA_CHANGE);
    if(mscDev->MediaChangeId == OS_NO_ERR)
    {
        DMSG_PANIC("ERR: create MediaChangeId failed\n");
        ret = -1;
        goto error7;
    }

    USB_OS_SemPend(mscDev->notify_complete, 0, &err);

    return 0;

error7:
    UsbKillThread(mscDev_Scan_Id, NULL);

error6:
    UsbKillThread(mscDev->MainThreadId, NULL);

error5:
error4:
error3:
error2:
    mscDevFree(mscDev);
error1:
    USB_OS_FREE(mscDev);

error0:

    DMSG_PANIC("ERR: USB Mass Storage Driver probe failed\n");

    return ret;
}


/*
*******************************************************************************
*                     mscDevSuspend
*
* Description:
*    挂起设备
*
* Parameters:
*    intf  ： input. USB接口信息
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
static __s32 mscDevSuspend(struct usb_interface *intf)
{
    __mscDev_t *mscDev = NULL;

    if(intf == NULL)
    {
        DMSG_PANIC("ERR: mscDevRemove: input error\n");
        return -1;
    }

    mscDev = (__mscDev_t *)usb_mod_usb_get_intf_priv_data(intf);
    if(mscDev == NULL)
    {
        DMSG_PANIC("ERR: mscDev = NULL\n");
        return -1;
    }

    /* suspend interface */

    /* delay for suspend successful */
    DMSG_INFO("mscDevSuspend: SuspendTime = %dms\n", mscDev->SuspendTime);
    //	USB_OS_DELAY_x_MS(mscDev->SuspendTime);

    return 0;
}


/*
*******************************************************************************
*                     mscDevRemove
*
* Description:
*    msc设备移除
*
* Parameters:
*    intf  ： input. USB接口信息
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
static void mscDevRemove(struct usb_interface *intf)
{
    __mscDev_t *mscDev = NULL;

    if(intf == NULL)
    {
        DMSG_PANIC("ERR: mscDevRemove: input error\n");
        return ;
    }

    mscDev = (__mscDev_t *)usb_mod_usb_get_intf_priv_data(intf);
    if(mscDev == NULL)
    {
        DMSG_PANIC("ERR: mscDev = NULL\n");
        return ;
    }

    SetMscDevState(mscDev, MSC_DEV_OFFLINE);
    DMSG_INFO("mscDevRemove: SetMscDevState MSC_DEV_OFFLINE\n");

    /* 停止当前传输 */
    mscDev->StopTransport(mscDev);

    /* kill media change thrad */
    UsbKillThread(mscDev->MediaChangeId, NULL);

    /* 断开mscDev上面的所有的Lun */
    mscDevDel(mscDev);

    /* kill main thrad */
    UsbKillThread(mscDev->MainThreadId, mscDev->ThreadSemi);

    /* 释放mscDev资源 */
    mscDevFree(mscDev);

    USB_OS_FREE(mscDev);

    DMSG_INFO("mscDevRemove complete\n");

    return ;
}

/*
*******************************************************************************
*                     mscDrv_init
*
* Description:
*    msc驱动初始化
*
* Parameters:
*    drv  ： input. msc驱动
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
static __s32 mscDrvInit(struct usb_host_func_drv *drv)
{
    if(drv == NULL)
    {
        DMSG_PANIC("ERR: mscDrv_init: input error\n");
        return -1;
    }

    INIT_LIST_HEAD(&(drv->virt_dev_list));
    drv->func_drv_name 		= MSC_DRV_NAME;
    drv->func_drv_auther 	= MSC_DRV_AUTHOR;
    drv->probe 				= mscDevProbe;
    drv->disconnect 		= mscDevRemove;
    drv->suspend			= mscDevSuspend;
    drv->resume				= NULL;
    drv->match_table 		= msc_match_table;

    return 0;
}

/*
*******************************************************************************
*                     scsi_bus_drv_reg
*
* Description:
*    往scsi总线注册驱动
*
* Parameters:
*    drv : input.  驱动
*
* Return value:
*    EPDK_OK / EPDK_FAIL
*
* note:
*    无
*
*******************************************************************************
*/
__s32 mscInit(void)
{
    __s32 ret = 0;

    USB_OS_MEMSET(&mscDrv, 0, sizeof(struct usb_host_func_drv));
    mscDev_id_array	 = 0;

    init_usbh_buff_manager();
    usbh_disk_time_init();

    /* 根据客户的需求，要求反映disk的状态, 这里就是管理disk的状态 */
    usbh_disk_info_reg();

    if(mscDrvInit(&mscDrv) != 0)
    {
        DMSG_PANIC("ERR: mscDrv_init failed\n");
        return -1;
    }

    ret = usb_host_func_drv_reg(&mscDrv);
    if(ret != 0)
    {
        DMSG_PANIC("ERR: mscInit: Reg driver %s failed\n", mscDrv.func_drv_name);
        return -1;
    }

    return 0;
}

/*
*******************************************************************************
*                     scsi_bus_drv_reg
*
* Description:
*    往scsi总线注册驱动
*
* Parameters:
*    drv : input.  驱动
*
* Return value:
*    EPDK_OK / EPDK_FAIL
*
* note:
*    无
*
*******************************************************************************
*/
__s32 mscExit(void)
{
    __s32 ret = 0;

    ret = usb_host_func_drv_unreg(&mscDrv );
    if(ret != 0)
    {
        DMSG_PANIC("ERR: mscExit: UnReg driver %s failed\n", mscDrv.func_drv_name);
        return -1;
    }

    usbh_disk_info_unreg();
    exit_usbh_buff_manager();
    usbh_disk_time_exit();

    return 0;
}




