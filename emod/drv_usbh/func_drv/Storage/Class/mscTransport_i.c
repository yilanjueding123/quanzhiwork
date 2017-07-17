/*
********************************************************************************************************************
*                                              usb host driver
*
*                              (c) Copyright 2007-2010, javen.China
*										All	Rights Reserved
*
* File Name 	: mscTransport_i.c
*
* Author 		: javen
*
* Version 		: 2.0
*
* Date 			: 2010.7.12
*
* Description 	:
*     USB Mass Storage Class传输容错辅助处理部分。
*
*     这部分主要是为了解决USB光驱中光盘突然弹出时，USB容错时间过长。其原因是Lun中media已经不存在，但是lun本身还在，
* 虽不可读/写数据，但是可以操作Lun。因此现在增加了在读写操作超时后，先去test_unit_ready，判断Lun的状态后，
* 再决定下一步的操作。
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
*                     mscTransportRequestSense
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
static __s32 mscTransportRequestSense(__mscLun_t *mscLun, void *buffer, __u32 buffer_len)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];
    __u32 ret = 0;

    if(mscLun == NULL || buffer == NULL)
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x, buffer = %x\n", mscLun, buffer);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    if(buffer_len == 0)
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, buffer_len == %d\n", buffer_len);
        return USB_STATUS_BUFFER_TOO_SMALL;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_REQUEST_SENSE;				/* scsi command 		*/
    CB[1] = ((mscLun->LunNo & 0x7) << 5);  	/* lun 					*/
    CB[4] = buffer_len;						/* Allocation Length 	*/

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_FROM_DEVICE;
    ScsiCmnd.cmnd.Timeout		= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun			= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen			= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_6 : UFI_CDB;
    ScsiCmnd.cmnd.CommandBlock	= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= buffer_len;
    ScsiCmnd.buffer	    		= buffer;

    ret = mscBoTransport(mscLun->mscDev, &ScsiCmnd);

    return ret;
}

/*
*******************************************************************************
*                     mscTransportGetSenseData
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
static __s32 mscTransportGetSenseData(__mscLun_t *mscLun)
{
    __SenseData_t SenseData;
    __s32 ret = 0;

    USB_OS_MEMSET(&SenseData, 0, sizeof(__SenseData_t));
    ret = mscTransportRequestSense(mscLun, (void *)&SenseData, sizeof(__SenseData_t));
    if(ret != USB_STOR_TRANSPORT_GOOD)
    {
        DMSG_PANIC("ERR: mscTransportGetSenseData failed\n");
        return USB_ERR_BAD_ARGUMENTS;
    }

    DMSG_PANIC("mscReason:SenseKey = %x, AdditionalSenseCode = %x\n",
               SenseData.SenseKey, SenseData.AdditionalSenseCode);

    switch(SenseData.SenseKey)
    {
    case SENSE_NONE :
        ret = USB_ERR_SUCCESS;
        break;

    case SENSE_NOT_READY :
        switch(SenseData.AdditionalSenseCode)
        {
        case ASC_MEDIUM_NOT_PRESENT :
            DMSG_PANIC("ERR: media is not present\n");
            mscLun->MediaPresent = 0;
            ret = USB_ERR_MEDIA_NOT_PRESENT;
            break;

        default:
            DMSG_PANIC("ERR: unkown ASC(%d)\n", SenseData.AdditionalSenseCode);
            ret = USB_ERR_UNKOWN_ERROR;
            break;
        }
        break;

    case SENSE_ILLEGAL_REQUEST :
        ret = USB_ERR_NOT_SUPPORT_COMMAND;
        break;

    case SENSE_UNIT_ATTENTION :
        switch(SenseData.AdditionalSenseCode)
        {
        case ASC_MEDIUM_NOT_PRESENT :
            DMSG_PANIC("ERR: media is not present\n");
            mscLun->MediaPresent = 0;
            ret = USB_ERR_MEDIA_NOT_PRESENT;
            break;

        default:
            DMSG_PANIC("ERR: unkown ASC(%d)\n", SenseData.AdditionalSenseCode);
            ret = USB_ERR_UNKOWN_ERROR;
        }
        break;

    default:
        ret = USB_ERR_UNKOWN_ERROR;
    }

    return ret;
}

/*
*******************************************************************************
*                     mscGetDataTransportReason
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
__s32 mscGetDataTransportReason(__mscLun_t *mscLun, __ScsiCmnd_t *ScsiCmnd)
{
    __s32 ret = USB_ERR_COMMAND_NEED_RETRY;
    __u8  Command = 0;
    __s32 status = 0;

    if(mscLun == NULL || ScsiCmnd == NULL)
    {
        DMSG_PANIC("ERR: input error\n");
        return USB_ERR_COMMAND_NEED_RETRY;
    }

    status = mscTransportGetSenseData(mscLun);
    if(status == USB_ERR_MEDIA_NOT_PRESENT || status == USB_ERR_NOT_SUPPORT_COMMAND)
    {
        Command = ((__u8 *)(ScsiCmnd->cmnd.CommandBlock))[0];
        if(Command == SCSI_READ6 || Command == SCSI_READ10 || Command == SCSI_READ16
                || Command == SCSI_WRITE6 || Command == SCSI_WRITE10 || Command == SCSI_WRITE16)
        {
            DMSG_PANIC("ERR: Command(%x) execute failed, for media is not present\n", Command);
            ret = USB_ERR_COMMAND_EXECUTE_FAILED;
        }
    }

    return ret;
}

