/*
********************************************************************************************************************
*                                              usb host driver
*
*                              (c) Copyright 2007-2010, javen.China
*										All	Rights Reserved
*
* File Name 	: Scsi2.c
*
* Author 		: javen
*
* Version 		: 2.0
*
* Date 			: 2010.03.02
*
* Description 	: scsi command
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

static __s32 ScsiGetSenseData(__mscLun_t *mscLun);


void Scsi2_show_command(__ScsiCmnd_t *ScsiCmnd)
{
#if 0
    char *what = NULL;
    int i;
    __u8 *CB = NULL;

    CB = (__u8 *)ScsiCmnd->cmnd.CommandBlock;
    switch (CB[0])
    {
    case SCSI_TEST_UNIT_READY:
        what = "TEST_UNIT_READY";
        break;
    case SCSI_REQUEST_SENSE:
        what = "REQUEST_SENSE";
        break;
    case SCSI_READ6:
        what = "READ_6";
        break;
    case SCSI_WRITE6:
        what = "WRITE_6";
        break;
    case SCSI_INQUIRY:
        what = "INQUIRY";
        break;
    case SCSI_MODE_SELECT6:
        what = "MODE_SELECT";
        break;
    case SCSI_MODE_SELECT10:
        what = "MODE_SELECT";
        break;
    case SCSI_MODE_SENSE6:
        what = "MODE_SENSE";
        break;
    case SCSI_MODE_SENSE10:
        what = "MODE_SENSE";
        break;
    case SCSI_START_STOP:
        what = "START_STOP";
        break;
    case SCSI_READ_CAPACITY:
        what = "READ_CAPACITY";
        break;
    case SCSI_READ10:
        what = "READ_10";
        break;
    case SCSI_WRITE10:
        what = "WRITE_10";
        break;
    case SCSI_SYNCHRONIZE_CACHE:
        what = "SYNCHRONIZE_CACHE";
        break;

    /* CD */
    case SCSI_CD_READ_TOC:
        what = "READ HEADER";
        break;
    case SCSI_CD_PLAY10:
        what = "PLAY AUDIO (10)";
        break;
    case SCSI_CD_PLAY_MSF:
        what = "PLAY AUDIO MSF";
        break;
    case SCSI_CD_PAUSE_RESUME:
        what = "GET EVENT/STATUS NOTIFICATION";
        break;

    default:
        what = "(unknown command)";
        break;
    }

    DMSG_INFO("\n");
    DMSG_INFO("Command name =  %s (%d bytes)\n", what, ScsiCmnd->cmnd.CBLen);

    for (i = 0; i < ScsiCmnd->cmnd.CBLen && i < MAX_CDB; i++)
    {
        DMSG_INFO(" %x \n", CB[i]);
    }

    DMSG_INFO("allowed   = %d\n", ScsiCmnd->allowed);
    DMSG_INFO("TransLen  = %d\n", ScsiCmnd->DataTransferLength);
    DMSG_INFO("direction = %s\n", (ScsiCmnd->cmnd.data_direction == DATA_FROM_DEVICE)
              ? "DATA_FROM_DEVICE" : "DATA_TO_DEVICE");
    DMSG_INFO("Tag       = %x\n", ScsiCmnd->cmnd.Tag);
    DMSG_INFO("Timeout   = %d\n", ScsiCmnd->cmnd.Timeout);
    DMSG_INFO("dwLun     = %d\n", ScsiCmnd->cmnd.dwLun);
    DMSG_INFO("CB_ADDR   = %x\n", ScsiCmnd->cmnd.CommandBlock);

    DMSG_INFO("\n");
#endif
}

static void ScsiCmndDone(__ScsiCmnd_t *ScsiCmnd)
{
    USB_OS_SemPost(ScsiCmnd->complete);
}

/*
*******************************************************************************
*                     ScsiDoCommand
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
static __s32 ScsiDoCommand(__mscLun_t *mscLun, __ScsiCmnd_t *ScsiCmnd)
{
    __u8 err = 0;
    __u32 ret = 0;

    if(mscLun == NULL || ScsiCmnd == NULL)
    {
        DMSG_PANIC("ERR: ScsiDoCommand: input error, mscLun = %x, ScsiCmd == %x\n", mscLun, ScsiCmnd);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    ScsiCmnd->complete = USB_OS_SemCreate(0);
    if(ScsiCmnd->complete == NULL)
    {
        DMSG_PANIC("ERR: ScsiDoCommand: USB_OS_SemCreatefailed\n");
        return USB_STATUS_UNKOWN_ERROR;
    }

    Scsi2_show_command(ScsiCmnd);

    /* do scsi command */
    ret = mscDevQueueCmnd(mscLun, ScsiCmnd);
    if(ret != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: mscDevQueueCmnd failed\n");
        goto Next;
    }

    /* wait for complete */
    USB_OS_SemPend(ScsiCmnd->complete, 0, &err);

Next:
    USB_OS_SemDel(ScsiCmnd->complete, &err);
    ScsiCmnd->complete = NULL;

    switch(ScsiCmnd->Result)
    {
    case USB_STOR_TRANSPORT_GOOD:
        ret = USB_STATUS_SUCCESS;
        break;

    case USB_STOR_TRANSPORT_FAILED:
        ret = ScsiGetSenseData(mscLun);
        break;

    case USB_STOR_TRANSPORT_ERROR:
        ret = USB_STATUS_COMMAND_FAILED;
        break;

    default:
        DMSG_PANIC("ERR: ScsiDoCommand: unkown command result %d\n", ScsiCmnd->Result);
        ret = USB_STATUS_COMMAND_FAILED;
    }

    return ret;
}


/*
*******************************************************************************
*                     ScsiInquery
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
__s32 ScsiInquery(__mscLun_t *mscLun, void *buffer, __u32 buffer_len)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];

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
    CB[0] = SCSI_INQUIRY;					/* scsi command 		*/
    CB[1] = ((mscLun->LunNo & 0x7) << 5);  	/* lun 					*/
    CB[4] = buffer_len;						/* Allocation Length 	*/

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction = DATA_FROM_DEVICE;
    ScsiCmnd.cmnd.Timeout		 = SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun			 = mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen			 = (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_6 : UFI_CDB;
    ScsiCmnd.cmnd.CommandBlock	 = CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= buffer_len;
    ScsiCmnd.buffer	    		= buffer;
    ScsiCmnd.Done				= ScsiCmndDone;

    return ScsiDoCommand(mscLun, &ScsiCmnd);
}

/*
*******************************************************************************
*                     ScsiReadCapacity
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
__s32 ScsiReadCapacity(__mscLun_t *mscLun)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];
    __u8 buffer[8];
    __s32 ret = 0;

    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_READ_CAPACITY;				/* scsi command 		*/
    CB[1] = ((mscLun->LunNo & 0x7) << 5);  	/* lun 					*/

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_FROM_DEVICE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : UFI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= 8;
    ScsiCmnd.buffer	    		= buffer;
    ScsiCmnd.Done				= ScsiCmndDone;

    /* parse read capacity data */
    ret = ScsiDoCommand(mscLun, &ScsiCmnd);
    if(ret == USB_STATUS_SUCCESS)
    {
        mscLun->disk_info.capacity    = get_be32(buffer) + 1;
        mscLun->disk_info.sector_size = get_be32(buffer + 4);
    }
    else
    {
        mscLun->disk_info.capacity    = 0;
        mscLun->disk_info.sector_size = 512;
    }

    return ret;
}

/*
*******************************************************************************
*                     ScsiModeSense6
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
__s32 ScsiModeSense6(__mscLun_t *mscLun,
                     __u32 PageCode,
                     __u32 DBD,
                     void *buffer,
                     __u32 buffer_len,
                     __u32 *ActLen)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];
    __s32 ret = 0;

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
    CB[0] = SCSI_MODE_SENSE6;						/* scsi command 		*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 					*/
    CB[1] = CB[1] | (DBD << 3);  					/* dbd 					*/
    CB[2] = PageCode;								/* Page Code 			*/
    CB[4] = buffer_len;								/* Allocation Length 	*/

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_FROM_DEVICE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_6 : UFI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= buffer_len;
    ScsiCmnd.buffer	    		= buffer;
    ScsiCmnd.Done				= ScsiCmndDone;

    ret = ScsiDoCommand(mscLun, &ScsiCmnd);
    if(ret == USB_ERR_SUCCESS)
    {
        *ActLen = ScsiCmnd.ActualLength;
    }
    else
    {
        *ActLen = 0;
    }

    return ret;
}

/*
*******************************************************************************
*                     ScsiModeSense10
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
__s32 ScsiModeSense10(__mscLun_t *mscLun,
                      __u32 PageCode,
                      __u32 DBD,
                      void *buffer,
                      __u32 buffer_len,
                      __u32 *ActLen)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];
    __s32 ret = 0;

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
    CB[0] = SCSI_MODE_SENSE10;						/* scsi command 		*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 					*/
    CB[1] = CB[1] | (DBD << 3);  					/* dbd 					*/
    CB[2] = PageCode;								/* Page Code 			*/
    put_be16(&CB[7], buffer_len);					/* Allocation Length 	*/

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_FROM_DEVICE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : UFI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= buffer_len;
    ScsiCmnd.buffer	    		= buffer;
    ScsiCmnd.Done				= ScsiCmndDone;

    ret = ScsiDoCommand(mscLun, &ScsiCmnd);
    if(ret == USB_ERR_SUCCESS)
    {
        *ActLen = ScsiCmnd.ActualLength;
    }
    else
    {
        *ActLen = 0;
    }

    return ret;
}

/*
*******************************************************************************
*                     ScsiTestUnitReady
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
__s32 ScsiTestUnitReady(__mscLun_t *mscLun)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];
    __s32 ret = 0;

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_TEST_UNIT_READY;					/* scsi command 		*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 					*/

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_NONE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_6 : UFI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= 0;
    ScsiCmnd.buffer	    		= NULL;
    ScsiCmnd.Done				= ScsiCmndDone;

    ret = ScsiDoCommand(mscLun, &ScsiCmnd);
    /*
    	if((ret == USB_STATUS_SUCCESS && !(mscLun->MediaPresent))){
    		DMSG_INFO("mscLun Media not Present, but test unit ready command is successful\n");
    		mscLun->MediaPresent = 1;
    	}else if((ret == USB_STATUS_MEDIA_NOT_PRESENT)
    	         && (ret == USB_STATUS_UNIT_NOT_READY)
    	         && (ret == USB_STATUS_UNRECOGNIZED_MEDIA)
    	         && (ret == USB_STATUS_MEDIA_CHANGE)){
    		mscLun->MediaPresent = 0;
    	}
    */
    return ret;
}

/*
*******************************************************************************
*                     ScsiRequestSense
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
__s32 ScsiRequestSense(__mscLun_t *mscLun, void *buffer, __u32 buffer_len)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];
    __u8 err = 0;
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
    ScsiCmnd.Done				= ScsiCmndDone;

    /* do command */
    ScsiCmnd.complete = USB_OS_SemCreate(0);
    if(ScsiCmnd.complete == NULL)
    {
        DMSG_PANIC("ERR: ScsiDoCommand: USB_OS_SemCreatefailed\n");
        return USB_STATUS_UNKOWN_ERROR;
    }

    ret = mscDevQueueCmnd(mscLun, &ScsiCmnd);
    if(ret != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: mscDevQueueCmnd failed\n");
        goto Next;
    }

    /* wait for complete */
    USB_OS_SemPend(ScsiCmnd.complete, 0, &err);

Next:
    USB_OS_SemDel(ScsiCmnd.complete, &err);
    ScsiCmnd.complete = NULL;

    switch(ScsiCmnd.Result)
    {
    case USB_STOR_TRANSPORT_GOOD:
        ret = USB_STATUS_SUCCESS;
        break;

    case USB_STOR_TRANSPORT_FAILED:
        ret = USB_STATUS_UNKOWN_ERROR;
        break;

    case USB_STOR_TRANSPORT_ERROR:
        ret = USB_STATUS_COMMAND_FAILED;
        break;

    default:
        DMSG_PANIC("ERR: ScsiDoCommand: unkown command result %d\n", ScsiCmnd.Result);
        ret = USB_STATUS_COMMAND_FAILED;
    }

    return ret;
}

/*
*******************************************************************************
*                     ScsiGetSenseData
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
static __s32 ScsiGetSenseData(__mscLun_t *mscLun)
{
    __SenseData_t SenseData;
    __s32 ret = 0;

    USB_OS_MEMSET(&SenseData, 0, sizeof(__SenseData_t));
    ret = ScsiRequestSense(mscLun, (void *)&SenseData, sizeof(__SenseData_t));
    if(ret != USB_STATUS_SUCCESS)
    {
        DMSG_PANIC("ERR: ScsiRequestSense failed\n");
        return USB_STATUS_UNKOWN_ERROR;
    }

    DMSG_MSC_TEST("SenseKey = %x, AdditionalSenseCode = %x\n", SenseData.SenseKey, SenseData.AdditionalSenseCode);

    switch(SenseData.SenseKey)
    {
    case SENSE_NONE :
        ret = USB_STATUS_SUCCESS;
        break;

    case SENSE_NOT_READY :
        switch(SenseData.AdditionalSenseCode)
        {
        case ASC_LUN:
            if (0x02 == SenseData.AdditionalSenseCodeQualifier)   // Initialization Required
            {
                ScsiStartStopUnit(mscLun, 0, 1);
            }

            if (0x01 == SenseData.AdditionalSenseCodeQualifier)   /* not ready to ready */
            {
                ret = USB_STATUS_NOT_READY_TO_READY_TRANSITION;
            }
            else
            {
                ret = USB_STATUS_UNIT_NOT_READY;
            }
            break;

        case ASC_MEDIUM_NOT_PRESENT :
            DMSG_MSC_TEST("ERR: media is not present\n");
            mscLun->MediaPresent = 0;
            ret = USB_STATUS_MEDIA_NOT_PRESENT;
            break;

        default:
            DMSG_MSC_TEST("ERR: unkown ASC(%d)\n", SenseData.AdditionalSenseCode);
            ret = USB_STATUS_UNIT_NOT_READY;
            break;
        }
        break;

    case SENSE_MEDIUM_ERROR :
        ret = USB_STATUS_UNRECOGNIZED_MEDIA;
        break;

    case SENSE_HARDWARE_ERROR :
        ret = USB_STATUS_UNRECOGNIZED_MEDIA;
        break;

    case SENSE_ILLEGAL_REQUEST :
        ret = USB_STATUS_INVALID_FIELD_IN_COMMAND;
        break;

    case SENSE_UNIT_ATTENTION :
        switch(SenseData.AdditionalSenseCode)
        {
        case ASC_MEDIA_CHANGED :
            DMSG_MSC_TEST("ERR: media not ready to ready transition\n");
            ret = USB_STATUS_NOT_READY_TO_READY_TRANSITION;
            break;

        /* 见到sandisk的U盘，竟然在SENSE_UNIT_ATTENTION时, 说ASC_MEDIUM_NOT_PRESENT。
           为了处理这样的U盘, 这里也检查ASC_MEDIUM_NOT_PRESENT */
        case ASC_MEDIUM_NOT_PRESENT :
            DMSG_MSC_TEST("ERR: media is not present\n");
            mscLun->MediaPresent = 0;
            ret = USB_STATUS_MEDIA_NOT_PRESENT;
            break;

        default:
            DMSG_MSC_TEST("ERR: unkown ASC(%d)\n", SenseData.AdditionalSenseCode);
            ret = USB_STATUS_UNKOWN_SENSE;
        }
        break;

    default:
        ret = USB_STATUS_UNKOWN_SENSE;
    }

    return ret;
}

/*
*******************************************************************************
*                     ScsiStartStopUnit
*
* Description:
*    开始 / 停止 设备工作
*
* Parameters:
*    mscLun  : input.  命令发向的设备
*	 LoEj    : input.  加载/弹出 媒体介质
* 	 Start	 : input.  开始 / 停止 设备工作
*
* Return value:
*
*
* note:
*
*
*******************************************************************************
*/
__s32 ScsiStartStopUnit(__mscLun_t *mscLun, __u32 LoEj, __u32 Start)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_START_STOP;						/* scsi command 		*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 					*/
    CB[4] = Start;
    CB[4] = CB[4] | (LoEj << 1);

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_NONE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_6 : UFI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= 0;
    ScsiCmnd.buffer	    		= NULL;
    ScsiCmnd.Done				= ScsiCmndDone;

    return ScsiDoCommand(mscLun, &ScsiCmnd);
}

/*
*******************************************************************************
*                     SynchronizeCache
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
__s32 ScsiSynchronizeCache(__mscLun_t *mscLun)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_SYNCHRONIZE_CACHE;					/* scsi command 		*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 					*/

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_NONE;
    ScsiCmnd.cmnd.Timeout			= SCSI_MEDIA_OP_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : UFI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= 0;
    ScsiCmnd.buffer	    		= NULL;
    ScsiCmnd.Done				= ScsiCmndDone;

    return ScsiDoCommand(mscLun, &ScsiCmnd);
}

/*
*******************************************************************************
*                     ScsiRead
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
__s32 ScsiRead(__mscLun_t *mscLun,
               __u32 Version,
               __u32 StartSector,
               __u32 NumSectors,
               void *buffer,
               __u32 buffer_len)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    if(Version == 16)
    {
        DMSG_PANIC("ERR: read16 not support\n");

        ScsiCmnd.cmnd.CBLen = (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_16 : UFI_CDB;

        return USB_STATUS_INVALID_COMMAND;
    }
    else if(Version == 10)
    {
        CB[0] = SCSI_READ10;							/* scsi command 		*/
        CB[1] = ((mscLun->LunNo & 0x7) << 5);  			/* lun 					*/
        put_be32(&CB[2], StartSector);					/* LOGICAL BLOCK ADDRESS */
        put_be16(&CB[7], NumSectors);					/* TRANSFER LENGTH 		*/

        ScsiCmnd.cmnd.CBLen = (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : UFI_CDB;
    }
    else
    {
        CB[0] = SCSI_READ6;								/* scsi command 		*/
        CB[1] = ((mscLun->LunNo & 0x7) << 5);  			/* lun 					*/
        CB[1] = (__u8)(StartSector >> 16) & 0x1f;		/* LOGICAL BLOCK ADDRESS */
        put_be16(&CB[2], (__u16)StartSector);
        CB[4] = NumSectors;								/* TRANSFER LENGTH 		*/

        ScsiCmnd.cmnd.CBLen = (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_6 : UFI_CDB;
    }

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_FROM_DEVICE;
    ScsiCmnd.cmnd.Timeout			= SCSI_MEDIA_OP_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= buffer_len;
    ScsiCmnd.buffer	    		= buffer;
    ScsiCmnd.Done				= ScsiCmndDone;

    return ScsiDoCommand(mscLun, &ScsiCmnd);
}

/*
*******************************************************************************
*                     ScsiWrite
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
__s32 ScsiWrite(__mscLun_t *mscLun,
                __u32 Version,
                __u32 StartSector,
                __u32 NumSectors,
                void *buffer,
                __u32 buffer_len)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    if(Version == 16)
    {
        DMSG_PANIC("ERR: read16 not support\n");

        ScsiCmnd.cmnd.CBLen = (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_16 : UFI_CDB;

        return USB_STATUS_INVALID_COMMAND;
    }
    else if(Version == 10)
    {
        CB[0] = SCSI_WRITE10;							/* scsi command 		*/
        CB[1] = ((mscLun->LunNo & 0x7) << 5);  			/* lun 					*/
        put_be32(&CB[2], StartSector);					/* LOGICAL BLOCK ADDRESS */
        put_be16(&CB[7], NumSectors);					/* TRANSFER LENGTH 		*/

        ScsiCmnd.cmnd.CBLen = (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : UFI_CDB;
    }
    else
    {
        CB[0] = SCSI_WRITE6;							/* scsi command 		*/
        CB[1] = ((mscLun->LunNo & 0x7) << 5);  			/* lun 					*/
        CB[1] = (__u8)(StartSector >> 16) & 0x1f;		/* LOGICAL BLOCK ADDRESS */
        put_be16(&CB[2], (__u16)StartSector);
        CB[4] = NumSectors;								/* TRANSFER LENGTH 		*/

        ScsiCmnd.cmnd.CBLen = (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_6 : UFI_CDB;
    }

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_TO_DEVICE;
    ScsiCmnd.cmnd.Timeout			= SCSI_MEDIA_OP_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= buffer_len;
    ScsiCmnd.buffer	   			= buffer;
    ScsiCmnd.Done				= ScsiCmndDone;

    return ScsiDoCommand(mscLun, &ScsiCmnd);
}

/*
*******************************************************************************
*                     ScsiPreventAllowMedium
*
* Description:
*     禁止/允许介质移动
*
* Parameters:
*    mscLun 	:  input. 目标设备
*    Persistent	:  input. 禁止介质数据传输
*    Prevent   	:  input. 禁止介质移动
*
*
* Return value:
*    返回命令的执行结果
*
* note:
*    Persistent只能在设备枚举的时候设置, 在设备操作阶段不要使用。
*
*******************************************************************************
*/
__s32 ScsiPreventAllowMedium(__mscLun_t *mscLun, __u32 Persistent, __u32 Prevent)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_DVD_PREVENT_ALLOW_MEDIUM_REMOVAL;	/* scsi command 		*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 					*/
    CB[4] = CB[4] | (Persistent << 1);				/* persistent 			*/
    CB[4] = CB[4] | (Prevent << 0);					/* prevent 				*/

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_NONE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : UFI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= 0;
    ScsiCmnd.buffer	    		= NULL;
    ScsiCmnd.Done				= ScsiCmndDone;

    return ScsiDoCommand(mscLun, &ScsiCmnd);
}

/*
*******************************************************************************
*                     ScsiCDPauseResume
*
* Description:
*    停止/恢复播放CD media
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
__s32 ScsiCDPauseResume(__mscLun_t *mscLun, __u32 Resume)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_CD_PAUSE_RESUME;					/* scsi command 		*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 					*/
    CB[8] = Resume & 0x01;							/* Resume bit 			*/

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_NONE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : ATAPI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= 0;
    ScsiCmnd.buffer	    		= NULL;
    ScsiCmnd.Done				= ScsiCmndDone;

    return ScsiDoCommand(mscLun, &ScsiCmnd);
}

/*
*******************************************************************************
*                     ScsiCDPlayAudio10
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
__s32 ScsiCDPlayAudio10(__mscLun_t *mscLun, __u32 Lba, __u32 Lenght)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_CD_PLAY_AUDIO10;					/* scsi command 				*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 							*/
    put_be32(&CB[2], Lba);							/* Start LOGICAL BLOCK ADDRESS 	*/
    put_be16(&CB[7], Lenght);						/* TRANSFER LENGTH 				*/

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_NONE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : ATAPI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= 0;
    ScsiCmnd.buffer	    		= NULL;
    ScsiCmnd.Done				= ScsiCmndDone;

    return ScsiDoCommand(mscLun, &ScsiCmnd);
}

/*
*******************************************************************************
*                     ScsiCDPlayAudioMSF
*
* Description:
*    播放指定mfs的音乐光碟
*
* Parameters:
*    mscLun			:  input. Logic unit
*	 MSF 			:  input. 多扇区
*
* Return value:
*    返回命令的执行情况
*
* note:
*
*
*******************************************************************************
*/
__s32 ScsiCDPlayAudioMSF(__mscLun_t *mscLun, CDROM_PlayAudioMsf_t *MSF)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_CD_PLAY_AUDIO_MSF;					/* scsi command 				*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 							*/
    CB[3] = MSF->StartingM;
    CB[4] = MSF->StartingS;
    CB[5] = MSF->StartingF;
    CB[6] = MSF->EndingM;
    CB[7] = MSF->EndingS;
    CB[8] = MSF->EndingF;

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_NONE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : ATAPI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= 0;
    ScsiCmnd.buffer	    		= NULL;
    ScsiCmnd.Done				= ScsiCmndDone;

    return ScsiDoCommand(mscLun, &ScsiCmnd);
}

/*
*******************************************************************************
*                     ScsiCDReadToc
*
* Description:
*    读取介质内容的分配表信息
*
* Parameters:
*    mscLun			:  input. Logic unit
*	 MSF 			:  input. 多扇区
*	 Format			:  input. 数据格式化模式
*    StartingNumber	:  input. 起始扇区
*	 Buffer 		:  input. 缓冲区。存放命令执行的结果
*	 BufferLen		:  input. 缓冲区大小
*
* Return value:
*    返回命令的执行情况
*
* note:
*
*
*******************************************************************************
*/
__s32 ScsiCDReadToc(__mscLun_t *mscLun,
                    __u32 MSF,
                    __u32 Format,
                    __u32 StartingNumber,
                    void *Buffer,
                    __u32 BufferLen)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_CD_READ_TOC;						/* scsi command 					*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 								*/
    CB[1] = CB[1] | ((MSF ? 0 : 1) << 1);			/* is Msf?	 						*/
    CB[2] = CB[2] | (Format & 0x07);				/* Read TOC Data Format 			*/
    CB[6] = StartingNumber;							/* Starting Track/Session Number 	*/
    put_be16(&CB[7], BufferLen);					/* TRANSFER LENGTH 					*/

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_FROM_DEVICE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : ATAPI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= BufferLen;
    ScsiCmnd.buffer	    		= Buffer;
    ScsiCmnd.Done				= ScsiCmndDone;

    return ScsiDoCommand(mscLun, &ScsiCmnd);
}

/*
*******************************************************************************
*                     ScsiCDStopPlayScan
*
* Description:
*    中止扫描或是播放
*
* Parameters:
*    mscLun			:  input. Logic unit
*
* Return value:
*    返回命令的执行情况
*
* note:
*
*
*******************************************************************************
*/
__s32 ScsiCDStopPlayScan(__mscLun_t *mscLun)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_CD_STOP_PLAY_SCAN;					/* scsi command 				*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 							*/

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_NONE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : ATAPI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= 0;
    ScsiCmnd.buffer	    		= NULL;
    ScsiCmnd.Done				= ScsiCmndDone;

    return ScsiDoCommand(mscLun, &ScsiCmnd);
}

/*
*******************************************************************************
*                     ScsiCDReadDiskInfo
*
* Description:
*    读取光碟的信息
*
* Parameters:
*    mscLun			:  input. Logic unit
*	 Buffer 		:  input. 缓冲区。存放disk的信息
*	 BufferLen		:  input. 本次打算获得数据的长度
*
* Return value:
*    返回命令的执行情况
*
* note:
*
*
*******************************************************************************
*/
__s32 ScsiCDReadDiscInfo(__mscLun_t *mscLun,
                         __u32 DataType,
                         void *Buffer,
                         __u32 BufferLen,
                         __u32 *ActLen)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];
    __s32 ret = 0;

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_DVD_READ_DISC_INFORMATION;			/* scsi command 				*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 							*/
    CB[1] = CB[1] | (DataType & 0x07);
    put_be16(&CB[7], BufferLen);					/* TRANSFER LENGTH 				*/

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_FROM_DEVICE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : ATAPI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= BufferLen;
    ScsiCmnd.buffer	    		= Buffer;
    ScsiCmnd.Done				= ScsiCmndDone;

    ret = ScsiDoCommand(mscLun, &ScsiCmnd);
    if(ret == USB_ERR_SUCCESS)
    {
        *ActLen = ScsiCmnd.ActualLength;
    }
    else
    {
        *ActLen = 0;
    }

    return ret;
}

/*
*******************************************************************************
*                     ScsiCDReadTrackInfo
*
* Description:
*    读取光碟轨道的信息
*
* Parameters:
*    mscLun				:  input. Logic unit
*    Open				:  input.
*    Address_or_Number 	:  input.
*    LogicalBlockAddress:  input.
*	 Buffer 			:  input. 缓冲区。存放disk的信息
*	 BufferLen			:  input. 本次打算获得数据的长度
*    ActLen				:  input. 实际取得的长度
*
* Return value:
*    返回命令的执行情况
*
* note:
*
*
*******************************************************************************
*/
__s32 ScsiCDReadTrackInfo(__mscLun_t *mscLun,
                          __u32 Open,
                          __u32 Address_or_Number,
                          __u32 LogicalBlockAddress,
                          void *Buffer,
                          __u32 BufferLen,
                          __u32 *ActLen)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];
    __s32 ret = 0;

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_DVD_READ_TRACK_INFORMATION;		/* scsi command 				*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 							*/
    CB[1] = CB[1] | (Open ? 1 : 0);
    CB[1] = CB[1] | (Address_or_Number & 0x03);
    put_be16(&CB[7], BufferLen);					/* TRANSFER LENGTH 				*/

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_FROM_DEVICE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : ATAPI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= BufferLen;
    ScsiCmnd.buffer	    		= Buffer;
    ScsiCmnd.Done				= ScsiCmndDone;

    ret = ScsiDoCommand(mscLun, &ScsiCmnd);
    if(ret == USB_ERR_SUCCESS)
    {
        *ActLen = ScsiCmnd.ActualLength;
    }
    else
    {
        *ActLen = 0;
    }

    return ret;
}

/*
*******************************************************************************
*                     ScsiDvdGetEventStatus
*
* Description:
*    获得DVD的事件状态。
*
* Parameters:
*    mscLun			:  input. Logic unit
*	 Immed			:  input. 是否立即返回命令的执行结果
*	 ClassRequest	:  input. 类请求
*	 Buffer 		:  input. 存放DVD的事件结果
*	 BufferLen		:  input. 本次打算获得数据的长度
*
* Return value:
*    返回命令的执行情况
*
* note:
*
*
*******************************************************************************
*/
__s32 ScsiDvdGetEventStatus(__mscLun_t *mscLun,
                            __u32 Immed,
                            __u32 ClassRequest,
                            void *Buffer,
                            __u32 BufferLen)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_DVD_GET_EVENT_STATUS_NOTIFICATION;	/* scsi command 				*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 							*/
    CB[1] = CB[1] | (Immed ? 1 : 0);
    CB[4] = ClassRequest;							/* Notification Class Request 	*/
    put_be16(&CB[7], BufferLen);					/* TRANSFER LENGTH 				*/

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_FROM_DEVICE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : ATAPI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= BufferLen;
    ScsiCmnd.buffer	    		= Buffer;
    ScsiCmnd.Done				= ScsiCmndDone;

    return ScsiDoCommand(mscLun, &ScsiCmnd);
}

/*
*******************************************************************************
*                     ScsiDvdGetEventStatus
*
* Description:
*    装载或是卸载光碟
*
* Parameters:
*    mscLun	:  input. Logic unit
*	 Immed	:  input. 是否立即返回命令的执行结果
*	 Start	:  input. media就绪或者停止
*	 LoUnlo :  input. 装载或是卸载
*	 Slot	:  input.
*
* Return value:
*    返回命令的执行情况
*
* note:
*
*
*******************************************************************************
*/
__s32 ScsiDvdLoad(__mscLun_t *mscLun,
                  __u32 Immed,
                  __u32 Start,
                  __u32 LoUnlo,
                  __u32 Slot)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_DVD_LOAD_UNLOAD;					/* scsi command 				*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 							*/
    CB[1] = CB[1] | (Immed ? 1 : 0);
    CB[4] = CB[4] | (Start ? 1 : 0);
    CB[4] = CB[4] | ((LoUnlo ? 1 : 0) << 1);
    CB[8] = Slot;

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_FROM_DEVICE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : ATAPI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= 0;
    ScsiCmnd.buffer	    		= NULL;
    ScsiCmnd.Done				= ScsiCmndDone;

    return ScsiDoCommand(mscLun, &ScsiCmnd);
}

/*
*******************************************************************************
*                     ScsiSetSpeed
*
* Description:
*    设置光驱的读写速度
*
* Parameters:
*    mscLun		:  input. Logic unit
*	 ReadSpeed	:  input. 读速度. Kbytes/second
*	 WriteSpeed	:  input. 写速度. Kbytes/second
*
* Return value:
*    返回命令的执行情况
*
* note:
*
*
*******************************************************************************
*/
__s32 ScsiCDSetSpeed(__mscLun_t *mscLun, __u32 ReadSpeed, __u32 WriteSpeed)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_CD_SET_SPEED;						/* scsi command 				*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 							*/
    put_be16(&CB[2], ReadSpeed);
    put_be16(&CB[4], WriteSpeed);

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_NONE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : ATAPI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= 0;
    ScsiCmnd.buffer	    		= NULL;
    ScsiCmnd.Done				= ScsiCmndDone;

    return ScsiDoCommand(mscLun, &ScsiCmnd);
}

/*
*******************************************************************************
*                     ScsiGetConfiguration
*
* Description:
*    设置光驱的读写速度
*
* Parameters:
*    mscLun		:  input. Logic unit
*	 ReadSpeed	:  input. 读速度. Kbytes/second
*	 WriteSpeed	:  input. 写速度. Kbytes/second
*
* Return value:
*    返回命令的执行情况
*
* note:
*
*
*******************************************************************************
*/
__s32 ScsiDVDGetConfiguration(__mscLun_t *mscLun,
                              __u32 RT,
                              __u32 StartingFeatureNumber,
                              void *Buffer,
                              __u32 BufferLen)
{
    __ScsiCmnd_t ScsiCmnd;
    __u8 CB[MAX_CDB];

    if(mscLun == NULL )
    {
        DMSG_PANIC("ERR: ScsiInquery: input error, mscLun = %x\n", mscLun);
        return USB_STATUS_BAD_ARGUMENTS;
    }

    USB_OS_MEMSET((void *)&ScsiCmnd, 0, sizeof(__ScsiCmnd_t));
    USB_OS_MEMSET(CB, 0, MAX_CDB);

    /* create command block */
    CB[0] = SCSI_DVD_GET_EVENT_STATUS_NOTIFICATION;	/* scsi command 				*/
    CB[1] = CB[1] | ((mscLun->LunNo & 0x7) << 5);  	/* lun 							*/
    CB[1] = CB[1] | (RT & 0x03);					/* Request type 				*/
    put_be16(&CB[2], StartingFeatureNumber);		/* Starting Feature Number 		*/
    put_be16(&CB[7], BufferLen);					/* TRANSFER LENGTH 				*/

    /* initialize transport command */
    ScsiCmnd.cmnd.data_direction	= DATA_FROM_DEVICE;
    ScsiCmnd.cmnd.Timeout			= SCSI_COMMAND_TIMEOUT;
    ScsiCmnd.cmnd.dwLun				= mscLun->LunNo;
    ScsiCmnd.cmnd.CBLen				= (USBMSC_SUBCLASS_SCSI == mscLun->DiskSubClass) ? SCSI_CDB_10 : ATAPI_CDB;
    ScsiCmnd.cmnd.CommandBlock		= CB;

    /* initialize scsi_cnmd */
    ScsiCmnd.sc_lun     		= mscLun;
    ScsiCmnd.allowed 			= SCSI_CMD_RETRY;
    ScsiCmnd.DataTransferLength	= BufferLen;
    ScsiCmnd.buffer	    		= Buffer;
    ScsiCmnd.Done				= ScsiCmndDone;

    return ScsiDoCommand(mscLun, &ScsiCmnd);
}



