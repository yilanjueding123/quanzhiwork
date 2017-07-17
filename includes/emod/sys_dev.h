/*
**********************************************************************************************************************
*											        ePOS
*								the Easy Portable/Player Operation System
*									           Krnl sub-system
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : sys_dev.h
* Version: V1.0
* By     : steven.zgj
**********************************************************************************************************************
*/
#ifndef _SYS_DEV_H_
#define _SYS_DEV_H_

#define DEV_DEVTYPE_MAXNUM   32

/* maximum size of device class name */
#define MAX_DEV_CLASS_NAME_LEN    32

/* maximum size of device node name */
#define MAX_DEV_NODE_NAME_LEN   32

/* 设备类名称                                       */
#define DEV_CLASS_NOP           "NOP"
#define DEV_CLASS_DISK          "DISK"              /* 磁盘设备，也就是块设备                                       */
#define DEV_CLASS_USBD          "USBD"              /* u盘设备                                                      */
#define DEV_CLASS_KEY           "KEY"               /* 键盘设备                                                     */
#define DEV_CLASS_MOUSE         "MOUSE"             /* 键盘设备                                                     */
#define DEV_CLASS_FS            "FSDRV"             /* 键盘设备                                                     */
#define DEV_CLASS_PD            "PDDRV"             /* 键盘设备                                                     */
#define DEV_CLASS_DMS			"DMS"				/* 设备管理设备									    */
#define DEV_CLASS_HWSC			"HWSC"				/* 硬件扫描设备类									    */

#define DEV_CLASS_USERDEF       "USERDEF"

/* 设备节点名称 */
#define DEV_NAME_RAMDISK        "RAMDISK"           /* RAMDISK设备 */
#define DEV_NAME_SDCARD         "SDMMC-DISK"        /* SDcard类型 */
#define DEV_NAME_SDCARD0        "SDMMC-DISK:0"      /* 第0个卡槽中SDcard */
#define DEV_NAME_SDCARD1        "SDMMC-DISK:1"      /* 第1个卡槽中SDcard */
#define DEV_NAME_SCSI_DISK_00   "SCSI_DISK_00"      /* USB host第一个盘 */
#define DEV_NAME_USB_CDROM_00   "USB_CDROM_00"
#define DEV_NAME_ROOTFS         "ROOTFS"            /* 用来做系统区的nand分区设备 */
#define DEV_NAME_PROVIDERFS     "PROVIDERFS"        /* 用来做用户区的nand分区设备 */
#define DEV_NAME_SYSDATAFS      "SYSDATAFS"         /* 用来做系统数据区的nand分区设备 */
#define DEV_NAME_SYSBOOTFS      "SYSBOOTFS"         /* 用来做固件修改数据存放的nand分区设备 */
#define DEV_NAME_BOOTFS         "BOOTFS"            /* eGON2.0新增的nand分区设备 */
#define DEV_NAME_USERDISK      	"USERDISK"         	/* 方案用户自定义的nand分区名前缀 */
#define DEV_NAME_HWSC           "hwsc"              /* 用来同外界交互的硬件扫描设备 */
#define DEV_NAME_UDISK      	"UDISK"         	/* nor 方案客户用户盘设置名 */

/* 设备属性 */
#define DEV_NODE_ATTR_BLK       (1<<0)              /* 块设备   */
#define DEV_NODE_ATTR_CHR       (1<<1)              /* 字符设备 */
#define DEV_NODE_ATTR_CTL       (1<<2)              /* 控制设备 */
#define DEV_NODE_ATTR_PART      (1<<3)              /* 设备带分区属性 */
#define DEV_NODE_ATTR_FS        (1<<4)              /* 设备带文件系统属性 */
#define DEV_NODE_ATTR_MOVABLE   (1<<5)              /* 设备有可移动设备属性 */
#define DEV_NODE_ATTR_USBSLOT   (1<<6)              /* 设备有被提交到PC USB插槽属性 */
#define DEV_NODE_ATTR_RD        (1<<7)              /* 设备有可读属性 */
#define DEV_NODE_ATTR_WR        (1<<8)              /* 设备有可写属性 */
#define DEV_NODE_ATTR_SYNMNT    (1<<9)              /* 设备注册时同步分区挂载属性 */

/* 兼容旧dev ioctl */
#define DEV_CMD_GET_INFO            0
#define DEV_CMD_GET_INFO_AUX_SIZE   0
#define DEV_CMD_GET_STATUS          1


/* user defined device ioctrol cmd: 0x00000000~0x7FFFFFFF */
/* system defined device ioctrol cmd: 0x80000000~0xFFFFFFFF */
#define DEV_IOC_USR_BASE				0x00000000
#define DEV_IOC_SYS_BASE				0x80000000
#define IS_DEVIOCSYS(cmd)				(cmd>=DEV_IOC_SYS_BASE)
/* sys */
#define DEV_IOC_SYS_GET_DEVNAME		(DEV_IOC_SYS_BASE + 0)
#define DEV_IOC_SYS_GET_CLSNAME		(DEV_IOC_SYS_BASE + 1)
#define DEV_IOC_SYS_GET_ATTRIB		(DEV_IOC_SYS_BASE + 2)
#define DEV_IOC_SYS_GET_LETTER		(DEV_IOC_SYS_BASE + 3)
#define DEV_IOC_SYS_GET_OPENARGS    (DEV_IOC_SYS_BASE + 4)

/* dms */
#define DEV_IOC_USR_GET_DEVITEM		(DEV_IOC_USR_BASE + 102)
#define DEV_IOC_USR_GET_DEVROOT		(DEV_IOC_USR_BASE + 103)
/* blk */
#define DEV_IOC_USR_GET_GEO			(DEV_IOC_USR_BASE + 104)
#define DEV_IOC_USR_FLUSH_CACHE		(DEV_IOC_USR_BASE + 105)
#define DEV_IOC_USR_PHY_READ        (DEV_IOC_USR_BASE + 106)
#define DEV_IOC_USR_PHY_WRITE       (DEV_IOC_USR_BASE + 107)
#define DEV_IOC_USR_PHY_ERASE       (DEV_IOC_USR_BASE + 108)
#define DEV_IOC_USR_LOGIC_WRITE     (DEV_IOC_USR_BASE + 109)
#define DEV_IOC_USR_LOGIC_READ      (DEV_IOC_USR_BASE + 110)
#define DEV_IOC_USR_GET_HW_INFO     (DEV_IOC_USR_BASE + 111)
#define DEV_IOC_USR_BLK_ERASE       (DEV_IOC_USR_BASE + 112)

/* cd-rom */
#define DEV_CDROM_LAST_WRITTEN		(DEV_IOC_USR_BASE + 120)  /* get last block written on disc */
#define DEV_CDROM_MULTISESSION		(DEV_IOC_USR_BASE + 121)  /* Obtain the start-of-last-session
 															   * address of multi session disks,
 															   * address type is logical block.*/

#define DRV_CMD_PLUGIN				0xffff0000
#define DRV_CMD_PLUGOUT				0xffff0001
#define DRV_CMD_GET_STATUS			0xffff0002

typedef enum __DRV_STA
{
    DRV_STA_FREE = 0,
    DRV_STA_BUSY
} __drv_sta_t;


typedef enum __DEV_ERR
{
    DEV_NO_ERR = 0,
    DEV_ERR,

    DEV_HANDLE_ERR,                                 /* handle error, maybe handle equ. null, or handle not a opened */
    /* handle                                                       */

    DEV_CLK_NOT_EN,                                 /* clock not enable                                             */

    DEV_CLK_SCLK_ERR,
    DEV_CLK_DIV_ERR,
    DEV_CLK_HANDLE_FREE,

} __dev_err_t;


/* 设备操作函数集 */
typedef struct  __DEV_DEVOP
{
    __hdle  (*Open  )(void *open_arg, __u32 mode);
    __s32   (*Close )(__hdle hDev);
    __u32   (*Read  )(void * /*pBuffer*/,
                      __u32 /*chrdev: 1,        blkdev: sector pos*/,
                      __u32 /*chrdev: byte cnt, blkdev: sector cnt*/,
                      __hdle/*hDev*/);
    __u32   (*Write )(const void * /*pBuffer*/,
                      __u32 /*chrdev: 1,        blkdev: sector pos*/,
                      __u32 /*chrdev: byte cnt, blkdev: sector cnt*/,
                      __hdle/*hDev*/);
    __s32   (*Ioctl )(__hdle hDev, __u32 Cmd, __s32 Aux, void *pBuffer);
} __dev_devop_t;

#ifdef SIM_PC_WIN
#define __packed
#endif
typedef __packed struct __DEV_BLKINFO
{
    __u32   hiddennum;
    __u32   headnum;
    __u32   secnum;
    __u32   partsize;
    __u32	secsize;
} __dev_blkinfo_t;

/* disk info            */
typedef __packed struct __DEV_DSKINFO
{
    char                name[16];
    __u32               n;
    __dev_blkinfo_t    *info;
} __dev_dskinfo_t;

typedef enum __DEV_HOTPLUG_MSG_TARGET
{
    DEV_MSG_TARGET_HOTPLUG_USBMONITOR,
} __dev_msgtarget_t;

//SYSTEM_PAGE_SIZE 此处块为系统页大小 512B
typedef struct __BLK_DEV_RW_ATTR
{
    __u32       blk;        //块设备读写的起始块号
    __s32       cnt;        //块设备读写的块数
    void        *buf;       //存储块设备读写数据的缓冲区
    __u32       reserved;   //保留

} __blk_dev_rw_attr_t;


//************************************************
// 函数定义
/* system call table */
typedef struct
{
    __pSWI_t esDEV_Plugin               ;
    __pSWI_t esDEV_Plugout              ;

    __pSWI_t esDEV_DevReg               ;
    __pSWI_t esDEV_DevUnreg             ;

    __pSWI_t esDEV_Open                 ;
    __pSWI_t esDEV_Close                ;
    __pSWI_t esDEV_Read                 ;
    __pSWI_t esDEV_Write                ;
    __pSWI_t esDEV_Ioctl                ;

    __pSWI_t esDEV_Lock                 ;
    __pSWI_t esDEV_Unlock               ;

    __pSWI_t esDEV_Insmod               ;
    __pSWI_t esDEV_Unimod               ;
} SWIHandler_DEV_t;
#ifndef SIM_PC_WIN
#define SYSCALL_DEV(x,y) __swi(SWINO(SWINO_DEV, SWIHandler_DEV_t, y))x y
#else
#define SYSCALL_DEV(x,y) x y
#endif

SYSCALL_DEV(__s32 , esDEV_Plugin     )(char * /* plgmsg */, __u32 /*devno*/, void * /*p_arg*/, __u8/*prio*/);

SYSCALL_DEV(__s32 , esDEV_Plugout    )(char * /*device*/, __u32 /*devno*/);

SYSCALL_DEV(__hdle, esDEV_DevReg     )(const char * /*class*/, const char * /*name*/ ,
                                       __dev_devop_t * /*drv*/  , void * /*p_arg*/);
SYSCALL_DEV(__s32 , esDEV_DevUnreg   )(__hdle/*dev*/);

SYSCALL_DEV(__hdle, esDEV_Open       )(__hdle /*hNode*/, __u32/*Mode*/);
SYSCALL_DEV(__s32 , esDEV_Close      )(__hdle /*hDev*/);
SYSCALL_DEV(__u32 , esDEV_Read       )(void * /*pdata*/,
                                       __u32 /*chrdev: 1,        blkdev: sector pos*/,
                                       __u32 /*chrdev: byte cnt, blkdev: sector cnt*/,
                                       __hdle/*hDev*/);
SYSCALL_DEV(__u32 , esDEV_Write      )(const void * /*pdata*/,
                                       __u32 /*chrdev: 1,        blkdev: sector pos*/,
                                       __u32 /*chrdev: byte cnt, blkdev: sector cnt*/,
                                       __hdle/*hDev*/);
SYSCALL_DEV(__s32 , esDEV_Ioctl      )(__hdle /*hDev*/,   __u32/*cmd*/, __s32/*aux*/, void * /*pbuffer*/);

SYSCALL_DEV(__s32,  esDEV_Lock       )(__hdle hNode);
SYSCALL_DEV(__s32,  esDEV_Unlock     )(__hdle hNode);

SYSCALL_DEV(__s32 , esDEV_Insmod     )(char * /*modfile*/, __u32 /*devno*/, void * /*p_arg*/);
SYSCALL_DEV(__s32 , esDEV_Unimod     )(char * /*modfile*/, __u32 /*devno*/);

//************************************************
// 变量定义
extern SWIHandler_DEV_t SWIHandler_DEV;
//************************************************
// 函数定义
__s32 DEV_Init (void);
__s32 DEV_Exit (void);
__s32 HWSC_Init(void);
__s32 HWSC_Exit(void);

#endif //#ifndef _SYS_DEV_H_
