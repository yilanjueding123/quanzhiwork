/*
********************************************************************************************************************
*                                              usb host driver
*
*                              (c) Copyright 2007-2010, javen.China
*										All	Rights Reserved
*
* File Name 	: UsbBlkDev.c
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

#include  "usbh_debug.h"
#include  "usb_gen_hub.h"

#include  "usbh_buff_manager.h"

#include  "usb_msc_i.h"
#include  "Scsi2.h"

#include  "BlkDev.h"
#include  "cd.h"

extern __u32 usbh_get_usbd_port(void);

/*
*******************************************************************************
*                     DiskOpen
*
* Description:
*
*
* Parameters:
*
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
static USB_OS_HANDLE DiskOpen(void *open_arg, __u32 mode)
{
    __UsbBlkDev_t *BlkDev = NULL;

    if(open_arg == NULL)
    {
        DMSG_PANIC("ERR: DiskOpen: input error, open_arg = %x\n", open_arg);
        return NULL;
    }

    BlkDev = (__UsbBlkDev_t *)open_arg;
    if(BlkDev->Magic != USB_BLK_DEV_MAGIC)
    {
        DMSG_PANIC("ERR: DiskOpen: BlkDev Magic(%x) is invalid\n", BlkDev->Magic);
        return NULL;
    }

    BlkDev->used++;

    return (USB_OS_HANDLE)open_arg;
}

/*
*******************************************************************************
*                     DiskClose
*
* Description:
*
*
* Parameters:
*
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
static __s32 DiskClose(USB_OS_HANDLE hDev)
{
    __UsbBlkDev_t *BlkDev = NULL;

    if(hDev == NULL)
    {
        DMSG_PANIC("ERR: DiskClose: input error, hDev = %x\n", hDev);
        return EPDK_FAIL;
    }

    BlkDev = (__UsbBlkDev_t *)hDev;
    if(BlkDev->Magic != USB_BLK_DEV_MAGIC)
    {
        DMSG_PANIC("ERR: DiskClose: BlkDev Magic(%x) is invalid\n", BlkDev->Magic);
        return EPDK_FAIL;
    }

    BlkDev->used--;

    return EPDK_OK;
}

/*
*******************************************************************************
*                     __DiskRead
*
* Description:
*
*
* Parameters:
*
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
static __u32 __DiskRead(void *pBuffer, __u32 blk, __u32 n, USB_OS_HANDLE hDev)
{
    __UsbBlkDev_t *BlkDev = NULL;
    __mscLun_t *mscLun = NULL;
    __u32 cmd_version = 10;   /* 默认为10 */
    __s32 ret = 0;

    if(hDev == NULL)
    {
        DMSG_PANIC("ERR: __DiskRead: input error, hDev = %x\n", hDev);
        return 0;
    }

    if(n == 0)
    {
        DMSG_PANIC("ERR: __DiskRead: the read length can not be zero\n");
        return 0;
    }

    DMSG_MSC_TEST("__DiskRead: Blk = %d, N = %d, LastBlk = %d\n", blk, n, (blk + n));

    BlkDev = (__UsbBlkDev_t *)hDev;
    if(BlkDev->Magic != USB_BLK_DEV_MAGIC)
    {
        DMSG_PANIC("ERR: __DiskRead: BlkDev Magic(%x) is invalid\n", BlkDev->Magic);
        return 0;
    }

    /* 如果没有注册disk设备, 就不能够读数据 */
    if(!BlkDev->is_RegDisk)
    {
        DMSG_PANIC("ERR: __DiskRead: Not reged Disk, can not read\n");
        return 0;
    }

    mscLun = BlkDev->Lun;
    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: __DiskRead: mscLun == NULL\n");
        return 0;
    }

    /* 如果介质不存在, 就不能够从设备读数据 */
    if(!mscLun->MediaPresent)
    {
        DMSG_PANIC("ERR: __DiskRead: media is not present, __DiskRead failed\n");
        return 0;
    }

    if((blk + n) > mscLun->disk_info.capacity)
    {
        DMSG_PANIC("ERR: __DiskRead: block(%x, %x) is adjacence max capacity(%x)\n",
                   blk, n, mscLun->disk_info.capacity);
        return 0;
    }

    set_usbh_disk_status(USB_STORAGE_DEVICE_STATUS_READ);
    /*
    	if(mscLun->mscDev->SubClass == USBMSC_SUBCLASS_SCSI){
    		if(mscLun->disk_info.capacity > 0xffffffff){
    			cmd_version = 16;
    		}else if(mscLun->disk_info.capacity > 0x1fffff){
    			cmd_version = 10;
    		}else{
    			cmd_version = 6;
    		}
    	}else{
    		cmd_version = 10;
    	}
    */

    /* 目前所见的USB设备里面都使用的是10字节命名，而且协议规定的是10/12字节命令 */
    cmd_version = 10;

    UsbLock(mscLun->Lock);
    if(cmd_version == 16)
    {
        ret = ScsiRead(mscLun, 16, blk, n, pBuffer,
                       (n * (mscLun->disk_info.sector_size)));
    }
    else if(cmd_version == 10)
    {
        ret = ScsiRead(mscLun, 10, blk, n, pBuffer,
                       (n * (mscLun->disk_info.sector_size)));
    }
    else
    {
        ret = ScsiRead(mscLun, 6, blk, n, pBuffer,
                       (n * (mscLun->disk_info.sector_size)));
    }
    UsbUnLock(mscLun->Lock);

    set_usbh_disk_status(USB_STORAGE_DEVICE_STATUS_IDLE);

    if(ret != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: ScsiRead%d failed. DevNo = %d, blk = %d, n = %d\n",
                   cmd_version, BlkDev->DevNo, blk, n);
        return 0;
    }

    return n;
}

/*
*******************************************************************************
*                     __DiskWrite
*
* Description:
*
*
* Parameters:
*
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
static __u32 __DiskWrite(const void *pBuffer, __u32 blk, __u32 n, USB_OS_HANDLE hDev)
{
    __UsbBlkDev_t *BlkDev = NULL;
    __mscLun_t *mscLun = NULL;
    __u32 cmd_version = 10;   /* 默认为10 */
    __s32 ret = 0;

    if(hDev == NULL)
    {
        DMSG_PANIC("ERR: __DiskWrite: input error, hDev = %x\n", hDev);
        return 0;
    }

    if(n == 0)
    {
        DMSG_PANIC("ERR: __DiskWrite: the write length can not be zero\n");
        return 0;
    }

    DMSG_MSC_TEST("__DiskWrite: Blk = %d, N = %d, LastBlk = %d\n", blk, n, (blk + n));


    BlkDev = (__UsbBlkDev_t *)hDev;
    if(BlkDev->Magic != USB_BLK_DEV_MAGIC)
    {
        DMSG_PANIC("ERR: __DiskWrite: BlkDev Magic(%x) is invalid\n", BlkDev->Magic);
        return 0;
    }

    /* 如果没有注册disk设备, 就不能够读数据 */
    if(!BlkDev->is_RegDisk)
    {
        DMSG_PANIC("ERR: __DiskWrite: Not reged Disk, can not read\n");
        return 0;
    }

    mscLun = BlkDev->Lun;
    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: __DiskWrite: mscLun == NULL\n");
        return 0;
    }

    /* 如果介质不存在, 就不能够往设备写数据 */
    if(!mscLun->MediaPresent)
    {
        DMSG_PANIC("ERR: __DiskWrite: media is not present, __DiskWrite failed\n");
        return 0;
    }

    if(mscLun->WriteProtect)
    {
        DMSG_PANIC("ERR: __DiskWrite: device is protect, can't write\n");
        return 0;
    }

    if((blk + n) > mscLun->disk_info.capacity)
    {
        DMSG_PANIC("ERR: __DiskWrite: block(%x, %x) is adjacence max capacity(%x)\n",
                   blk, n, mscLun->disk_info.capacity);
        return 0;
    }

    set_usbh_disk_status(USB_STORAGE_DEVICE_STATUS_WRITE);

    /*
    	if(mscLun->mscDev->SubClass == USBMSC_SUBCLASS_SCSI){
    		if(mscLun->disk_info.capacity > 0xffffffff){
    			cmd_version = 16;
    		}else if(mscLun->disk_info.capacity > 0x1fffff){
    			cmd_version = 10;
    		}else{
    			cmd_version = 6;
    		}
    	}else{
    		cmd_version = 10;
    	}
    */

    /* 目前所见的USB设备里面都使用的是10字节命名，而且协议规定的是10/12字节命令 */
    cmd_version = 10;

    UsbLock(mscLun->Lock);
    if(cmd_version == 16)
    {
        ret = ScsiWrite(mscLun, 16, blk, n, (void *)pBuffer,
                        (n * (mscLun->disk_info.sector_size)));
    }
    else if(cmd_version == 10)
    {
        ret = ScsiWrite(mscLun, 10, blk, n, (void *)pBuffer,
                        (n * (mscLun->disk_info.sector_size)));
    }
    else
    {
        ret = ScsiWrite(mscLun, 6, blk, n, (void *)pBuffer,
                        (n * (mscLun->disk_info.sector_size)));
    }
    UsbUnLock(mscLun->Lock);

    set_usbh_disk_status(USB_STORAGE_DEVICE_STATUS_IDLE);

    if(ret != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: ScsiWrite%d failed. DevNo = %d, blk = %d, n = %d\n",
                   cmd_version, BlkDev->DevNo, blk, n);
        return 0;
    }

    return n;
}

#if 1
/*
*******************************************************************************
*                     DiskRead
*
* Description:
*    设备读
*
* Parameters:
*    pBuffer	:  output. 装载读回来的数据
*    blk		:  input.  起始扇区
*    n			:  input.  扇区个数
*    hDev   	:  input.  设备
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
static __u32 DiskRead(void *pBuffer, __u32 blk, __u32 n, USB_OS_HANDLE hDev)
{
    __UsbBlkDev_t *BlkDev = NULL;
    __mscLun_t *mscLun = NULL;
    __u32 cnt = 0;

    if(hDev == NULL)
    {
        DMSG_PANIC("ERR: DiskRead: input error, hDev = %x\n", hDev);
        return 0;
    }

    DMSG_MSC_TEST("DiskRead: Blk = %d, N = %d, LastBlk = %d\n", blk, n, (blk + n));

    BlkDev = (__UsbBlkDev_t *)hDev;
    if(BlkDev->Magic != USB_BLK_DEV_MAGIC)
    {
        DMSG_PANIC("ERR: DiskRead: BlkDev Magic(%x) is invalid\n", BlkDev->Magic);
        return 0;
    }

    mscLun = BlkDev->Lun;
    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: DiskRead: mscLun == NULL\n");
        return 0;
    }

    if((blk + n) > mscLun->disk_info.capacity)
    {
        DMSG_PANIC("ERR: DiskRead: block(%x, %x) is adjacence max capacity(%x)\n",
                   blk, n, mscLun->disk_info.capacity);
        return 0;
    }

    /* 1、lba0 不预读
       2、本次读写长度超过了最大容量，不预读
    */
    if((blk != 0)
            && ((blk + (USBH_TEMP_BUFFER_MAX_LEN / mscLun->disk_info.sector_size)) < mscLun->disk_info.capacity))
    {
        cnt = usbh_msc_special_read((void *)pBuffer,
                                    blk,
                                    n,
                                    hDev,
                                    BlkDev->DevNo,
                                    mscLun->disk_info.sector_size,
                                    (void *)__DiskRead);
    }
    else
    {
        DMSG_INFO("DiskRead: block(%x, %x) is adjacence max capacity(%x), can't use special write\n",
                  blk, n, mscLun->disk_info.capacity);
        cnt = __DiskRead(pBuffer, blk, n, hDev);
    }

    if(cnt != n)
    {
        DMSG_PANIC("ERR: DiskRead failed, want(%d) != real(%d)\n", n, cnt);
        return 0;
    }

    DMSG_MSC_TEST("read end\n");

    return n;
}

/*
*******************************************************************************
*                     DiskWrite
*
* Description:
*    设备写
*
* Parameters:
*    pBuffer	:  input. 要写的数据
*    blk		:  input. 起始扇区
*    n			:  input. 扇区个数
*    hDev   	:  input. 设备
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
static __u32 DiskWrite(const void *pBuffer, __u32 blk, __u32 n, USB_OS_HANDLE hDev)
{
    __UsbBlkDev_t *BlkDev = NULL;
    __mscLun_t *mscLun = NULL;
    __u32 cnt = 0;

    if(hDev == NULL)
    {
        DMSG_PANIC("ERR: DiskWrite: input error, hDev = %x\n", hDev);
        return 0;
    }

    DMSG_MSC_TEST("DiskWrite: Blk = %d, N = %d, LastBlk = %d\n", blk, n, (blk + n));

    BlkDev = (__UsbBlkDev_t *)hDev;
    if(BlkDev->Magic != USB_BLK_DEV_MAGIC)
    {
        DMSG_PANIC("ERR: DiskWrite: BlkDev Magic(%x) is invalid\n", BlkDev->Magic);
        return 0;
    }

    mscLun = BlkDev->Lun;
    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: __DiskWrite: mscLun == NULL\n");
        return 0;
    }

    if((blk + n) > mscLun->disk_info.capacity)
    {
        DMSG_PANIC("ERR: DiskWrite: block(%x, %x) is adjacence max capacity(%x)\n",
                   blk, n, mscLun->disk_info.capacity);
        return 0;
    }

    /* 1、lba0 不预读
       2、本次读写长度超过了最大容量，不预读
    */
    if((blk != 0)
            && ((blk + (USBH_TEMP_BUFFER_MAX_LEN / mscLun->disk_info.sector_size)) < mscLun->disk_info.capacity))
    {
        cnt = usbh_msc_special_write((void *)pBuffer,
                                     blk,
                                     n,
                                     hDev,
                                     BlkDev->DevNo,
                                     mscLun->disk_info.sector_size,
                                     (void *)__DiskWrite);
    }
    else
    {
        DMSG_INFO("DiskWrite: block(%x, %x) is adjacence max capacity(%x), can't use special write\n",
                  blk, n, mscLun->disk_info.capacity);
        set_usbh_temp_buff_invalid_by_dev(BlkDev->DevNo);
        cnt = __DiskWrite(pBuffer, blk, n, hDev);
    }

    if(cnt != n)
    {
        DMSG_PANIC("ERR: DiskWrite failed, want(%d) != real(%d)\n", n, cnt);
        return 0;
    }

    DMSG_MSC_TEST("write end\n");

    return n;
}

#else

static __u32 DiskRead(void *pBuffer, __u32 blk, __u32 n, USB_OS_HANDLE hDev)
{
    return __DiskRead(pBuffer, blk, n, hDev);
}

static __u32 DiskWrite(const void *pBuffer, __u32 blk, __u32 n, USB_OS_HANDLE hDev)
{
    return __DiskWrite(pBuffer, blk, n, hDev);
}

#endif

/*
*******************************************************************************
*                     DiskIoctl
*
* Description:
*
*
* Parameters:
*
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
static __s32 DiskIoctl(USB_OS_HANDLE hDev, __u32 Cmd, __s32 Aux, void *pBuffer)
{
    __UsbBlkDev_t *BlkDev = NULL;
    __mscLun_t *mscLun = NULL;
    __u32 ret = 0;

    if(hDev == NULL)
    {
        DMSG_PANIC("ERR: DiskIoctl: input error, hDev = %x\n", hDev);
        return NULL;
    }

    BlkDev = (__UsbBlkDev_t *)hDev;
    if(BlkDev->Magic != USB_BLK_DEV_MAGIC)
    {
        DMSG_PANIC("ERR: DiskIoctl: BlkDev Magic(%x) is invalid\n", BlkDev->Magic);
        return EPDK_FAIL;
    }

    mscLun = BlkDev->Lun;
    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: DiskIoctl: mscLun == NULL\n");
        return EPDK_FAIL;
    }

    /* 由于块设备不支持光驱设备, 因此这里就只能做个临时版本 */
    if(mscLun->DeviceType == SCSI_DEVICE_CDROM)
    {
        ret = CDIOCtrl(BlkDev, Cmd, Aux, pBuffer);
        if(ret == EPDK_OK)
        {
            DMSG_PANIC("[CD_ROM]: Cmd(%x) is USB CD_ROM command\n");
            return EPDK_OK;
        }
    }

    switch (Cmd)
    {
    case DEV_CMD_GET_INFO:
    {
        if (!pBuffer)
        {
            DMSG_PANIC("ERR : usb storage disk ,pBuffer == NULL\n");
            return EPDK_FAIL;
        }

        ((__dev_blkinfo_t *)pBuffer)->hiddennum	= 0;
        ((__dev_blkinfo_t *)pBuffer)->headnum  	= 0;
        ((__dev_blkinfo_t *)pBuffer)->secnum 	= 0;
        ((__dev_blkinfo_t *)pBuffer)->partsize 	= mscLun->disk_info.capacity;
        ((__dev_blkinfo_t *)pBuffer)->secsize 	= mscLun->disk_info.sector_size;

        DMSG_INFO("[usb storage]: DEV_CMD_GET_INFO, capacity = %dM, sector = %d\n",
                  ((mscLun->disk_info.capacity) >> 11), mscLun->disk_info.sector_size);
    }
    break;

    case DEV_CMD_GET_STATUS:
        return EPDK_OK;

    case DEV_IOC_USR_FLUSH_CACHE:
        return EPDK_OK;

    case DEV_CDROM_LAST_WRITTEN:
        return EPDK_FAIL;

    case DEV_CDROM_MULTISESSION:
        return EPDK_FAIL;

    default:
        DMSG_PANIC("WARN : DiskIoctl ,cmd = %x ,not support now\n", Cmd);
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

/*
*******************************************************************************
*                     UsbBlkDevAllocInit
*
* Description:
*
*
* Parameters:
*
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
__UsbBlkDev_t *UsbBlkDevAllocInit(__mscLun_t *mscLun)
{
    __UsbBlkDev_t *BlkDev = NULL;
    __u8 temp_buff[32];
    __u32 temp = 0;

    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: UsbBlkDevAllocInit: input error\n");
        return NULL;
    }

    //--<1>--create a block device
    BlkDev = (__UsbBlkDev_t *)USB_OS_MALLOC(sizeof(__UsbBlkDev_t), USB_MEM_FILE_TRIGGER, USB_MEM_LINE_TRIGGER);
    if(BlkDev == NULL)
    {
        DMSG_PANIC("ERR: UsbBlkDevAllocInit: USB_OS_MALLOC failed\n");
        return NULL;
    }

    USB_OS_MEMSET(BlkDev, 0, sizeof(__UsbBlkDev_t));
    BlkDev->Lun = mscLun;

    //--<2>--create lock

    //--<3>--create DevNo
    BlkDev->DevNo = (mscLun->LunNo * 1) + (mscLun->mscDev->mscDevNo * 10);

    //--<4>--create sub device name
    if(mscLun->DeviceType == SCSI_DEVICE_CDROM)
    {
        USB_OS_MEMCPY((void *)BlkDev->DevName, USB_CDROM_DEV_NAME, USB_OS_STRLEN(USB_CDROM_DEV_NAME));
        USB_OS_STRCAT((char *)BlkDev->DevName, "_");
    }
    else
    {
        USB_OS_MEMCPY((void *)BlkDev->DevName, USB_STORAGE_DEV_NAME, USB_OS_STRLEN(USB_STORAGE_DEV_NAME));
        USB_OS_STRCAT((char *)BlkDev->DevName, "_");
    }

    /* usb controler number */
    temp = usbh_get_usbd_port();
    USB_OS_MEMSET(temp_buff, 0, 32);
    USB_OS_uint2str(temp, (char *)temp_buff);
    USB_OS_STRCAT((char *)BlkDev->DevName, (const char *)temp_buff);

    /* mscDevNo */
    USB_OS_MEMSET(temp_buff, 0, 32);
    USB_OS_uint2str(mscLun->mscDev->mscDevNo, (char *)temp_buff);
    USB_OS_STRCAT((char *)BlkDev->DevName, (const char *)temp_buff);

    /* LunNo */
    USB_OS_MEMSET(temp_buff, 0, 32);
    USB_OS_uint2str(mscLun->LunNo, (char *)temp_buff);
    USB_OS_STRCAT((char *)BlkDev->DevName, (const char *)temp_buff);

    /* init device operation function */
    BlkDev->DiskOp.Open  = DiskOpen;
    BlkDev->DiskOp.Close = DiskClose;
    BlkDev->DiskOp.Read  = DiskRead;
    BlkDev->DiskOp.Write = DiskWrite;
    BlkDev->DiskOp.Ioctl = DiskIoctl;

    return BlkDev;
}

/*
*******************************************************************************
*                     UsbBlkDevFree
*
* Description:
*
*
* Parameters:
*
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
__s32 UsbBlkDevFree(__UsbBlkDev_t *BlkDev)
{
    if(BlkDev == NULL)
    {
        DMSG_PANIC("ERR: UsbBlkDevFree: input error\n");
        return -1;
    }

    BlkDev->DiskOp.Open  = NULL;
    BlkDev->DiskOp.Close = NULL;
    BlkDev->DiskOp.Read  = NULL;
    BlkDev->DiskOp.Write = NULL;
    BlkDev->DiskOp.Ioctl = NULL;

    USB_OS_FREE(BlkDev);

    return 0;
}

/*
*******************************************************************************
*                     PrintDevStatus
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
static void PrintDevStatus(__u8 *FunName, __s32 status)
{
    DMSG_INFO("FunName: %s,", FunName);
    switch(status)
    {
    case  USB_STATUS_SUCCESS:
        DMSG_INFO("USB_STATUS_SUCCESS\n");
        break;

    case  USB_STATUS_DEVICE_DISCONNECTED:
        DMSG_INFO("USB_STATUS_DEVICE_DISCONNECTED\n");
        break;

    case USB_STATUS_IO_TIMEOUT:
        DMSG_INFO("USB_STATUS_IO_TIMEOUT\n");
        break;

    case USB_STATUS_IO_DEVICE_ERROR:
        DMSG_INFO("USB_STATUS_IO_DEVICE_ERROR\n");
        break;

    case USB_STATUS_DEVICE_BUSY:
        DMSG_INFO("USB_STATUS_DEVICE_BUSY\n");
        break;

    case USB_STATUS_BUFFER_TOO_SMALL:
        DMSG_INFO("USB_STATUS_BUFFER_TOO_SMALL\n");
        break;

    case USB_STATUS_INVALID_COMMAND:
        DMSG_INFO("USB_STATUS_INVALID_COMMAND\n");
        break;

    case USB_STATUS_INVALID_FIELD_IN_COMMAND:
        DMSG_INFO("USB_STATUS_INVALID_FIELD_IN_COMMAND\n");
        break;

    case USB_STATUS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE:
        DMSG_INFO("USB_STATUS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE\n");
        break;

    case USB_STATUS_MEDIA_NOT_PRESENT:
        DMSG_INFO("USB_STATUS_MEDIA_NOT_PRESENT\n");
        break;

    case USB_STATUS_NOT_READY_TO_READY_TRANSITION:
        DMSG_INFO("USB_STATUS_NOT_READY_TO_READY_TRANSITION\n");
        break;

    case USB_STATUS_UNRECOGNIZED_MEDIA:
        DMSG_INFO("USB_STATUS_UNRECOGNIZED_MEDIA\n");
        break;

    case USB_STATUS_UNKOWN_ERROR:
        DMSG_INFO("USB_STATUS_UNKOWN_ERROR\n");
        break;

    default:
        DMSG_INFO("unkonw status %d\n", status);
    }

    return ;
}

static void PrintDiskInfo(__UsbBlkDev_t *BlkDev)
{
    __mscLun_t *mscLun = NULL;

    if(BlkDev == NULL)
    {
        DMSG_PANIC("ERR: PrintDiskInfo: input error, BlkDev = %x\n", BlkDev);
        return ;
    }

    mscLun = BlkDev->Lun;
    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: UnitReady: mscLun == NULL\n");
        return ;
    }

    DMSG_INFO("-----------------Disk Information-----------------\n");
    DMSG_INFO("WriteProtect = %d\n", mscLun->WriteProtect);
    DMSG_INFO("MediaPresent = %d\n", mscLun->MediaPresent);
    DMSG_INFO("WCE          = %d\n", mscLun->WCE);
    DMSG_INFO("RCD          = %d\n", mscLun->RCD);
    DMSG_INFO("capacity     = %dM, sector number = %d\n",
              (mscLun->disk_info.capacity * 512) / (1024 * 1024),
              mscLun->disk_info.capacity);
    DMSG_INFO("sector_size  = %d\n", mscLun->disk_info.sector_size);
    DMSG_INFO("DevNo        = %d\n", BlkDev->DevNo);
    DMSG_INFO("ClassName    = %s\n", BlkDev->ClassName);
    DMSG_INFO("DevName      = %s\n", BlkDev->DevName);
    DMSG_INFO("--------------------------------------------------\n");

    return ;
}


/*
*******************************************************************************
*                     WaitForDeviceReady
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
static void WaitForDeviceReady(__UsbBlkDev_t *BlkDev)
{
    __mscLun_t *mscLun = NULL;
    __u32 Repeat = 10;
    __s32 ret = USB_STATUS_SUCCESS;

    if(BlkDev == NULL)
    {
        DMSG_PANIC("ERR: UnitReady: input error\n");
        return ;
    }

    mscLun = BlkDev->Lun;
    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: UnitReady: mscLun == NULL\n");
        return ;
    }

    UsbLock(mscLun->Lock);

    /* Unit Ready? */
    ret = ScsiTestUnitReady(mscLun);
    if(ret == USB_STATUS_SUCCESS)
    {
        goto end;
    }
    else if( mscLun->DeviceType == SCSI_DEVICE_CDROM)
    {
        PrintDevStatus("ScsiTestUnitReady", ret);
        ScsiStartStopUnit(mscLun, 0, 1);
    }

    /* wait for unit ready */
    do
    {
        ret = ScsiTestUnitReady(mscLun);
        if(ret == USB_STATUS_INVALID_COMMAND)
        {
            DMSG_PANIC("ERR: Device Not Support ScsiTestUnitReady command\n");
            PrintDevStatus("ScsiTestUnitReady", ret);
            break;
        }

        /* 判断media的状态 */
        if(ret != USB_STATUS_SUCCESS)
        {
            mscLun->MediaPresent = 0;
        }
        else if((ret == USB_STATUS_SUCCESS && !(mscLun->MediaPresent)))
        {
            DMSG_INFO("Media not Present, but ScsiTestUnitReady command is successful,"
                      " Repeat = %d\n", Repeat);
            mscLun->MediaPresent = 1;
        }

        --Repeat;

        if (USB_STATUS_SUCCESS != ret && Repeat != 0)
        {
            USB_OS_DELAY_x_MS(1000);
        }
    }
    while(Repeat && !(mscLun->MediaPresent));

end:
    UsbUnLock(mscLun->Lock);

    return ;
}

/*
*******************************************************************************
*                     ReadCapacity
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
static void ReadCapacity(__UsbBlkDev_t *BlkDev)
{
    __mscLun_t *mscLun = NULL;
    __s32 ret = 0;

    if(BlkDev == NULL)
    {
        DMSG_PANIC("ERR: UnitReady: input error\n");
        return ;
    }

    mscLun = BlkDev->Lun;
    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: UnitReady: mscLun == NULL\n");
        return ;
    }

    if(mscLun->MediaPresent)
    {
        UsbLock(mscLun->Lock);
        ret = ScsiReadCapacity(mscLun);
        if(ret != USB_STATUS_SUCCESS)
        {
            DMSG_PANIC("ERR: ScsiReadCapacity failed\n");
            PrintDevStatus("ScsiReadCapacity", ret);
        }
        UsbUnLock(mscLun->Lock);
    }
    else
    {
        DMSG_PANIC("ERR: media is not present, ReadCapacity failed\n");
    }

    return ;
}

/*
*******************************************************************************
*                     ReadProtectFlag
*
* Description:
*    获得设备写保护标志位
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
static void ReadProtectFlag(__UsbBlkDev_t *BlkDev)
{
    __mscLun_t *mscLun = NULL;
    __BlockDevSpecPara_t *DevSpecPara = NULL;
    __u32 cmd_version = 6;   /* 默认使用mode sense 6 */
    __s32 ret = 0;
    __u32 ActLen = 0;
    __u8  buffer[SCSI_MODE_SENSE_MAX_DATA_LEN];

    if(BlkDev == NULL)
    {
        DMSG_PANIC("ERR: UnitReady: input error\n");
        return ;
    }

    mscLun = BlkDev->Lun;
    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: UnitReady: mscLun == NULL\n");
        return ;
    }

    if((mscLun->mscDev->SubClass != USBMSC_SUBCLASS_SCSI) || (mscLun->DeviceType == SCSI_DEVICE_CDROM))
    {
        cmd_version = 10;
    }

    USB_OS_MEMSET(buffer, 0, SCSI_MODE_SENSE_MAX_DATA_LEN);

    UsbLock(mscLun->Lock);

    /* send mode sense command */
    if(cmd_version == 6)
    {
        ret = ScsiModeSense6(mscLun,
                             MODE_PAGE_ALL_PAGES,
                             ((mscLun->DeviceType == SCSI_DEVICE_CDROM) ? 1 : 0),
                             (void *)buffer,
                             SCSI_MODE_SENSE_MAX_DATA_LEN,
                             &ActLen);
        if(ret == USB_STATUS_INVALID_COMMAND)
        {
            cmd_version = 10;
        }
        else if(ret != USB_STATUS_SUCCESS)
        {
            DMSG_PANIC("ERR: ScsiModeSense6 failed\n");
            PrintDevStatus("ScsiModeSense6", ret);
            cmd_version = 0;
        }
    }

    if(cmd_version == 10)
    {
        ret = ScsiModeSense10(mscLun,
                              MODE_PAGE_ALL_PAGES,
                              ((mscLun->DeviceType == SCSI_DEVICE_CDROM) ? 1 : 0),
                              (void *)buffer,
                              SCSI_MODE_SENSE_MAX_DATA_LEN,
                              &ActLen);
        if(ret != USB_STATUS_SUCCESS)
        {
            DMSG_PANIC("ERR: ScsiModeSense10 failed\n");
            PrintDevStatus("ScsiModeSense10", ret);
            cmd_version = 0;
        }
    }

    /* parse mode sense data */
    if(ActLen)
    {
        if(cmd_version == 6)
        {
            DevSpecPara = (__BlockDevSpecPara_t *) & (buffer[2]);
            mscLun->WriteProtect = DevSpecPara->WP;
        }
        else if(cmd_version == 10)
        {
            DevSpecPara = (__BlockDevSpecPara_t *) & (buffer[3]);
            mscLun->WriteProtect = DevSpecPara->WP;
        }
        else
        {
            mscLun->WriteProtect = 0;
        }
    }
    else   /* if deivce not have 0x3f page, then no mode sense data */
    {
        DMSG_PANIC("ReadProtectFlag: no mode sense data\n");
        mscLun->WriteProtect = 0;
    }

    UsbUnLock(mscLun->Lock);

    return ;
}

/*
*******************************************************************************
*                     ReadCacheType
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
static void ReadCacheType(__UsbBlkDev_t *BlkDev)
{
    __mscLun_t *mscLun = NULL;
    __u32 cmd_version = 6;   /* 默认使用mode sense 6 */
    __s32 ret = 0;
    __u8  buffer[SCSI_MODE_SENSE_MAX_DATA_LEN];
    __u32 ActLen = 0;
    __u32 CachepageValid = 0;
    __u32 CachepageAddr = 0;

    if(BlkDev == NULL)
    {
        DMSG_PANIC("ERR: UnitReady: input error\n");
        return ;
    }

    mscLun = BlkDev->Lun;
    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: UnitReady: mscLun == NULL\n");
        return ;
    }

    if((mscLun->mscDev->SubClass != USBMSC_SUBCLASS_SCSI) || (mscLun->DeviceType == SCSI_DEVICE_CDROM))
    {
        cmd_version = 10;
    }

    USB_OS_MEMSET(buffer, 0, SCSI_MODE_SENSE_MAX_DATA_LEN);

    UsbLock(mscLun->Lock);

    /* send mode sense command */
    if(cmd_version == 6)
    {
        ret = ScsiModeSense6(mscLun,
                             MODE_PAGE_WCACHING_PAGE,
                             ((mscLun->DeviceType == SCSI_DEVICE_CDROM) ? 1 : 0),
                             (void *)buffer,
                             SCSI_MODE_SENSE_MAX_DATA_LEN,
                             &ActLen);
        if(ret == USB_STATUS_INVALID_COMMAND)
        {
            cmd_version = 10;
        }
        else if(ret != USB_STATUS_SUCCESS)
        {
            DMSG_PANIC("ERR: ScsiModeSense6 failed\n");
            PrintDevStatus("ScsiModeSense6", ret);
            cmd_version = 0;
        }
    }

    if(cmd_version == 10)
    {
        ret = ScsiModeSense10(mscLun,
                              MODE_PAGE_WCACHING_PAGE,
                              ((mscLun->DeviceType == SCSI_DEVICE_CDROM) ? 1 : 0),
                              (void *)buffer,
                              SCSI_MODE_SENSE_MAX_DATA_LEN,
                              &ActLen);
        if(ret != USB_STATUS_SUCCESS)
        {
            DMSG_PANIC("ERR: ScsiModeSense10 failed\n");
            PrintDevStatus("ScsiModeSense10", ret);
            cmd_version = 0;
        }
    }

    /* parse mode sense data */
    if(ActLen)
    {
        if(cmd_version == 6)
        {
            CachepageAddr  = 4;
            CachepageValid = 1;
        }
        else if(cmd_version == 10)
        {
            CachepageAddr  = 8;
            CachepageValid = 1;
        }
        else
        {
            CachepageAddr  = 0;
            CachepageValid = 0;
        }

        if(CachepageValid)
        {
            mscLun->WCE = (buffer[CachepageAddr + 2] & 0x04) ? 1 : 0;	/* bit2 */
            mscLun->RCD = (buffer[CachepageAddr + 2] & 0x01) ? 1 : 0;	/* bit0 */
        }
        else
        {
            mscLun->WCE = 0;
            mscLun->RCD = 0;
        }
    }
    else   /* if deivce not have cahe page, then no mode sense data */
    {
        DMSG_PANIC("ReadCacheType: no mode sense data\n");
        mscLun->WCE = 0;
        mscLun->RCD = 0;
    }

    UsbUnLock(mscLun->Lock);

    return ;
}


/*
*******************************************************************************
*                     GetDiskInfo
*
* Description:
*
*
* Parameters:
*
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
void GetDiskInfo(__UsbBlkDev_t *BlkDev)
{
    __mscLun_t *mscLun = NULL;

    if(BlkDev == NULL)
    {
        DMSG_PANIC("ERR: GetDiskInfo: input error\n");
        return ;
    }

    mscLun = BlkDev->Lun;
    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: GetDiskInfo: mscLun == NULL\n");
        return ;
    }

    mscLun->MediaPresent          = 1;
    mscLun->WriteProtect          = 0;
    mscLun->WCE                   = 0;
    mscLun->RCD                   = 0;
    mscLun->disk_info.capacity    = 0;
    mscLun->disk_info.sector_size = 0;

    /* check unit is ready? */
    WaitForDeviceReady(BlkDev);
    if(!mscLun->MediaPresent)
    {
        DMSG_PANIC("ERR: can not get disk info, for unit is not ready\n");

        mscLun->MediaPresent          = 0;
        mscLun->WriteProtect          = 0;
        mscLun->WCE                   = 0;
        mscLun->RCD                   = 0;
        mscLun->disk_info.capacity    = 0;
        mscLun->disk_info.sector_size = 0;

        return;
    }

    /* read capacity */
    ReadCapacity(BlkDev);

    /* Lun是否写保护? */
    ReadProtectFlag(BlkDev);

    /* Lun是否带cache? */
    ReadCacheType(BlkDev);

    PrintDiskInfo(BlkDev);

    return ;
}

/*
*******************************************************************************
*                     WaitForDeviceReady
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
void ShutDown(__UsbBlkDev_t *BlkDev)
{
    __mscLun_t *mscLun = NULL;

    if(BlkDev == NULL)
    {
        DMSG_PANIC("ERR: GetDiskInfo: input error\n");
        return ;
    }

    mscLun = BlkDev->Lun;
    if(mscLun == NULL)
    {
        DMSG_PANIC("ERR: GetDiskInfo: mscLun == NULL\n");
        return ;
    }

    /* sync cache */
    if(mscLun->WCE)
    {
        ScsiSynchronizeCache(mscLun);
    }

    return ;
}

/*
*******************************************************************************
*                     UsbBlkDevReg
*
* Description:
*
*
* Parameters:
*
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
__s32 UsbBlkDevReg(__UsbBlkDev_t *BlkDev, __u8 *ClassName, __u32 RegDisk)
{
    if(BlkDev == NULL || ClassName == NULL)
    {
        DMSG_PANIC("ERR: UsbBlkDevInit: input error\n");
        return -1;
    }

    //更新标志位
    BlkDev->Magic = USB_BLK_DEV_MAGIC;
    if(RegDisk)
    {
        BlkDev->is_RegDisk = 1;
    }

    USB_OS_STRNCPY((char *)BlkDev->ClassName,
                   (const char *)ClassName,
                   USB_OS_STRLEN((const char *)ClassName));

    /* save deivce info */
    USB_OS_MEMSET(&BlkDev->device_info, 0, sizeof(usbh_disk_device_info_t));
    USB_OS_STRCPY((char *)BlkDev->device_info.Classname, (char *)BlkDev->ClassName);
    USB_OS_STRCPY((char *)BlkDev->device_info.DeviceName, (char *)BlkDev->DevName);
    USB_OS_STRCPY((char *)BlkDev->device_info.DeivceInfo.Vender, (char *)BlkDev->Lun->Vendor);
    USB_OS_STRCPY((char *)BlkDev->device_info.DeivceInfo.Product, (char *)BlkDev->Lun->Product);
    USB_OS_STRCPY((char *)BlkDev->device_info.DeivceInfo.Serial, (char *)BlkDev->Lun->Revision);
    BlkDev->device_info.DeivceInfo.HubPortNo = (hub_GetHubNo(BlkDev->Lun->mscDev->pusb_dev) & 0x0f)
            | (hub_GetHubSeries(BlkDev->Lun->mscDev->pusb_dev) & 0x0f) << 8;
    if((BlkDev->Lun->DeviceType == SCSI_DEVICE_DIRECT_ACCESS) && (BlkDev->Lun->RemoveAble == 0))
    {
        BlkDev->device_info.DeivceInfo.DeviceType = USB_STORAGE_DEVICE_TYPE_HDD;
    }
    else if((BlkDev->Lun->DeviceType == SCSI_DEVICE_DIRECT_ACCESS) && BlkDev->Lun->RemoveAble)
    {
        BlkDev->device_info.DeivceInfo.DeviceType = USB_STORAGE_DEVICE_TYPE_UDISK;
    }
    else if(BlkDev->Lun->DeviceType == SCSI_DEVICE_CDROM)
    {
        BlkDev->device_info.DeivceInfo.DeviceType = USB_STORAGE_DEVICE_TYPE_CDROM;
    }
    else
    {
        BlkDev->device_info.DeivceInfo.DeviceType = USB_STORAGE_DEVICE_TYPE_UNKOWN;
    }

    DMSG_INFO("Classname  = %s\n", BlkDev->device_info.Classname);
    DMSG_INFO("DeviceName = %s\n", BlkDev->device_info.DeviceName);
    DMSG_INFO("Vender     = %s\n", BlkDev->device_info.DeivceInfo.Vender);
    DMSG_INFO("Product    = %s\n", BlkDev->device_info.DeivceInfo.Product);
    DMSG_INFO("Serial     = %s\n", BlkDev->device_info.DeivceInfo.Serial);
    DMSG_INFO("HubPortNo  = %d\n", BlkDev->device_info.DeivceInfo.HubPortNo);
    DMSG_INFO("DeviceType = %d\n", BlkDev->device_info.DeivceInfo.DeviceType);

    usbh_disk_SaveDeviceInfo(&BlkDev->device_info);

    /* reg disk */
    BlkDev->DevRegHdle = USB_OS_DEV_REG((const char *)ClassName, (const char *)BlkDev->DevName, &(BlkDev->DiskOp), (void *)BlkDev);
    if(BlkDev->DevRegHdle == NULL)
    {
        DMSG_PANIC("ERR: Block device register failed.\n");
        BlkDev->Magic = 0;
        BlkDev->is_RegDisk = 0;
        return USB_ERR_UNKOWN_ERROR;
    }

    DMSG_INFO("\n..............................................................................\n");
    DMSG_INFO("[USB Disk]: Register new device, class = [%s], dev = [%s]\n",
              BlkDev->ClassName, BlkDev->DevName);
    DMSG_INFO("..............................................................................\n\n");

    return USB_ERR_SUCCESS;
}

/*
*******************************************************************************
*                     UsbBlkDevUnReg
*
* Description:
*
*
* Parameters:
*
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
__s32 UsbBlkDevUnReg(__UsbBlkDev_t *BlkDev)
{
    if(BlkDev == NULL)
    {
        DMSG_PANIC("ERR: UsbBlkDevUnReg: input error\n");
        return USB_ERR_BAD_ARGUMENTS;
    }

    if(BlkDev->DevRegHdle)
    {
        DMSG_INFO("\n..............................................................................\n");
        DMSG_INFO("[USB Disk]: UnRegister old device, class = [%s], dev = [%s]\n",
                  BlkDev->ClassName, BlkDev->DevName);
        DMSG_INFO("..............................................................................\n\n");

        usbh_disk_FreeDeviceInfo(&BlkDev->device_info);
        USB_OS_DEV_UNREG(BlkDev->DevRegHdle);

        BlkDev->DevRegHdle = NULL;
        BlkDev->is_RegDisk = 0;
        USB_OS_MEMSET(BlkDev->ClassName, 0, USB_BULK_DISK_MAX_NAME_LEN);
    }
    else
    {
        DMSG_PANIC("ERR: UsbBlkDevUnReg: DevRegHdle = NULL\n");
        return USB_ERR_BAD_ARGUMENTS;
    }

    set_usbh_temp_buff_invalid_by_dev(BlkDev->DevNo);
    BlkDev->Magic = 0;

    return USB_ERR_SUCCESS;
}


