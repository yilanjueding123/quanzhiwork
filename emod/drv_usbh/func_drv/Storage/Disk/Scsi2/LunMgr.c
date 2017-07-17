/*
********************************************************************************************************************
*                                              usb host driver
*
*                              (c) Copyright 2007-2010, javen.China
*										All	Rights Reserved
*
* File Name 	: LunMgr.c
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

#include  "usb_msc_i.h"
#include  "Scsi2.h"
#include  "LunMgr.h"
#include  "LunMgr_i.h"

static void print_inquiry(__InquiryData_t *InquiryData)
{
    DMSG_INFO("\n");
    DMSG_INFO("------------------device infomation----------------------\n");
    DMSG_INFO("Device_Type          = %d\n", InquiryData->Device_Type);
    DMSG_INFO("Removable_Media      = %d\n", InquiryData->Removable_Media);
    DMSG_INFO("ANSI_Version         = %d\n", InquiryData->ANSI_Version);
    DMSG_INFO("ECMA_Version         = %d\n", InquiryData->ECMA_Version);
    DMSG_INFO("ISO_Version          = %d\n", InquiryData->ISO_Version);
    DMSG_INFO("Data_Format          = %d\n", InquiryData->Data_Format);
    DMSG_INFO("Additional_Length    = %d\n", InquiryData->Additional_Length);
    DMSG_INFO("Vendor_Info          = %s\n", InquiryData->Vendor_Info);
    DMSG_INFO("Product_Info         = %s\n", InquiryData->Product_Info);
    DMSG_INFO("Product_Revision     = %s\n", InquiryData->Product_Revision);
    DMSG_INFO("--------------------------------------------------------\n");
    DMSG_INFO("\n");
}

/*
*******************************************************************************
*                     mscLun_alloc
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
__mscLun_t *mscLun_alloc(void)
{
    __mscLun_t *mscLun = NULL;

    mscLun = USB_OS_MALLOC(sizeof(__mscLun_t), USB_MEM_FILE_TRIGGER, USB_MEM_LINE_TRIGGER);
    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: USB_OS_MALLOC failed\n");
        return NULL;
    }

    USB_OS_MEMSET(mscLun, 0, sizeof(__mscLun_t));

    //--<2>--create lock
    mscLun->Lock = USB_OS_SemCreate(1);
    if(mscLun->Lock == NULL)
    {
        DMSG_PANIC("ERR: UsbBlkDevAllocInit: create lock failed\n");
        USB_OS_FREE(mscLun);
        return NULL;
    }

    return mscLun;
}

/*
*******************************************************************************
*                     mscLun_free
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
void mscLun_free(__mscLun_t *mscLun)
{
    __u8 err = 0;

    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: input error\n");
        return;
    }

    if(mscLun->Lock)
    {
        USB_OS_SemDel(mscLun->Lock, &err);
        mscLun->Lock = NULL;
    }
    else
    {
        DMSG_PANIC("ERR: BlkDev Lock had already delete\n");
    }

    USB_OS_FREE(mscLun);
}


/*
*******************************************************************************
*                     mscLunAdd
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
__s32 mscLunAdd(__mscLun_t *mscLun)
{
    __mscDev_t 		*mscDev 		= NULL;
    __InquiryData_t *InquiryData 	= NULL;
    __s32 ret = 0;
    __u32 i   = 0;

    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: MscLunAdd: input error\n");
        return USB_ERR_BAD_ARGUMENTS;
    }

    mscDev = mscLun->mscDev;
    if(mscDev == NULL)
    {
        DMSG_PANIC("ERR: MscLunAdd: mscDev == NULL\n");
        return -1;
    }

    /* 获得LUN信息 */
    for(i = 0; i < 3; i++)
    {
        UsbLock(mscLun->Lock);
        ret = ScsiInquery(mscLun, mscLun->Inquiry, SCSI_INQUERY_LEN);
        UsbUnLock(mscLun->Lock);
        if(ret == USB_STATUS_SUCCESS)
        {
            break;
        }
    }

    if(i >= 3)
    {
        DMSG_PANIC("ERR: MscLunAdd: ScsiInquery failed\n");
        return USB_ERR_COMMAND_EXECUTE_FAILED;
    }

    /* 识别LUN */
    InquiryData = (__InquiryData_t *)mscLun->Inquiry;
    print_inquiry(InquiryData);

    mscLun->DeviceType 	= InquiryData->Device_Type;
    mscLun->ScsiLevel 	= InquiryData->ANSI_Version;
    mscLun->Vendor		= (__u8 *)InquiryData->Vendor_Info;
    mscLun->Product		= (__u8 *)InquiryData->Product_Info;
    mscLun->Revision	= (__u8 *)InquiryData->Product_Revision;

    mscLun->RemoveAble	= InquiryData->Removable_Media;
    switch (mscLun->DeviceType)
    {
    case SCSI_DEVICE_DIRECT_ACCESS:
        mscLun->Probe  = DiskProbe;
        mscLun->Remove = DiskRemove;
        break;

    case SCSI_DEVICE_CDROM:
        mscLun->Probe  = CDProbe;
        mscLun->Remove = CDRemove;
        break;

    default:
        DMSG_PANIC("[Lun]: MscLunAdd: unknown device type %d\n", mscLun->DeviceType);
        return USB_ERR_UNKOWN_DEVICE;
    }

    /* LUN探测 */
    if(mscLun->Probe)
    {
        ret = mscLun->Probe(mscLun);
        if(ret != 0)
        {
            DMSG_PANIC("[Lun]: MscLunAdd: Lun(%d) Probe fialed\n", mscLun->LunNo);
            return USB_ERR_DEVICE_PROBE_FAILED;
        }
    }

    return USB_ERR_SUCCESS;
}

/*
*******************************************************************************
*                     mscLunDel
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
__s32 mscLunDel(__mscLun_t *mscLun)
{
    __s32 ret = 0;

    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: MscLunDel: input error\n");
        return USB_ERR_BAD_ARGUMENTS;
    }

    if(mscLun->Remove)
    {
        ret = mscLun->Remove(mscLun);
        if(ret != 0)
        {
            DMSG_PANIC("[Lun]: MscLunAdd: Lun(%d) Remove fialed\n", mscLun->LunNo);
            return USB_ERR_DEVICE_REMOVE_FAILED;
        }
    }

    return USB_ERR_SUCCESS;
}




