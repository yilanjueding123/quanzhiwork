/*
**********************************************************************************************************************
*											        ePOS
*						           the Easy Portable/Player Operation System
*									           fsys sub-system
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : fsys.h
* Version: V1.0
* By     : steven.zgj
**********************************************************************************************************************
*/

#ifndef _FSYS_H_
#define _FSYS_H_

#include "typedef.h"
#include "cfgs.h"

#ifdef EPDK_USED_BY_KRNL
#define __swi(x)
#endif

//**********************************************************************************************************
//* define level 0( system level)
//************************************************
// 常数定义
//#define FSYS_dirnamemax 260

/* File system version */
#define FSYS_VERSION                    0x01000000UL   /* 1.00.0000 */

/* Maximum size of a directory name */
#define FSYS_DIRNAME_MAX                1024

/* maximum size of volume name */
#define MAX_VOLUME_NAME_LEN             512

/* maximum size of part name */
#define MAX_PART_NAME_LEN               MAX_VOLUME_NAME_LEN

/* maximum size of file system name */
#define MAX_FS_NAME_LEN                 16

/* names of supported file system */
#define FSYS_NTFS_NAME                  "NTFS"
#define FSYS_FAT_NAME                   "FAT"
#define FSYS_EXFAT_NAME                 "exFAT"
#define FSYS_CDFS_NAME                  "CDFS"
#define FSYS_UDF_NAME                   "UDF"
/* Global error codes */
#define FSYS_ERR_OK                     (__s16)0x0000
#define FSYS_ERR_EOF                    (__s16)0xfff0
#define FSYS_ERR_DISKFULL               (__s16)0xffe0
#define FSYS_ERR_INVALIDPAR             (__s16)0xffd0
#define FSYS_ERR_WRITEONLY              (__s16)0xffc0
#define FSYS_ERR_READONLY               (__s16)0xffb0
#define FSYS_ERR_READERROR              (__s16)0xffa0
#define FSYS_ERR_WRITEERROR             (__s16)0xff90
#define FSYS_ERR_DISKCHANGED            (__s16)0xff80
#define FSYS_ERR_CLOSE                  (__s16)0xff70

/* 设备占用盘符 */
#define FSYS_MAX_FPARTS                 16          /* 自由分配的分区项个数 */
#define FSYS_MAX_XPARTS                 8          	/* 系统内部固定分配的分区项个数 */
#define	FSYS_MAX_UPARTS					2			/* 方案用户自定义的分区项个数 */
#define FSYS_MAX_PARTS                  (FSYS_MAX_FPARTS + FSYS_MAX_XPARTS + FSYS_MAX_UPARTS)
/* 固定分配的分区项个数 */
#define PART_LETTER_FREE                0xff        /* 根据设备注册先后顺序自由分配盘符 */
#define PART_LETTER_FREESTART           'E'         /* 自由分配盘符的起始盘符 */
#define PART_LETTER_USER           		0xfe        /* 方案用户新增分区根据注册的先后顺利
													 * 在U~W范围内分配盘符(从W到U倒序分配) */
#define PART_LETTER_USERSTART           'V'         /* 方案新增分区的起始盘符V(倒序分配V-U...) */
#define PART_LETTER_DMS                 'B'         /* 设备分区盘符 */
#define PART_LETTER_RAMDISK             'C'         /* RAMDISK盘符 */
#define PART_LETTER_ROOTFS              'D'         /* ROOTFS盘符 */
#define PART_LETTER_UDISK               'E'         /* udisk盘符 */
#define PART_LETTER_SYSDATA             'Z'         /* 系统数据区盘符 */
#define PART_LETTER_SYSBOOT             'Y'         /* 系统启动区盘符 */
#define PART_LETTER_BOOTFS              'X'         /* eGON2启动分区 */
#define PART_LETTER_RESERVEV            'V'         /* 保留未用盘符 */
#define PART_LETTER_RESERVEU            'U'         /* 保留未用盘符 */
#define PART_LETTER_RESERVEA            'A'         /* 保留未用盘符 */

/* GlobalSY constants*/
#define FSYS_SEEK_CUR                   1		//current position
#define FSYS_SEEK_END                   2		//from end of the file
#define FSYS_SEEK_SET                   0		//from begin of the file

/********************* 旧part && dev ioctl cmd ******************/
/* I/O coSYmmands */
#define FSYS_CMD_FLUSH_CACHE            1000L
#define FSYS_CMD_CHK_DSKCHANGE          1010L
#define FSYS_CMD_READ_SECTOR            1100L
#define FSYS_CMD_WRITE_SECTOR           1110L
#define FSYS_CMD_FORMAT_MEDIA           2222L
#define FSYS_CMD_FORMAT_AUTO            2333L
#define FSYS_CMD_INC_BUSYCNT            3001L
#define FSYS_CMD_DEC_BUSYCNT            3002L
#define FSYS_CMD_GET_DISKFREE           4000L
#define FSYS_CMD_GET_DEVINFO            4011L
#define FSYS_CMD_FLASH_ERASE_CHIP       9001L
/* part I/O coSYmmands */
#define FSYS_PART_CMD_GET_PARTSIZE      1000L       /* 获取part大小，byte为单位                                     */
#define FSYS_PART_CMD_FLUSH_CACHE       1001L       /* flash cache                                                  */
#define FSYS_PART_CMD_INC_BUSYCNT       1100L       /* 增加busy count                                               */
#define FSYS_PART_CMD_DEC_BUSYCNT       1101L       /* 减少busy count                                               */
#define FSYS_PART_CMD_GET_STATUS        1102L       /* 获取状态                                                     */
#define FSYS_PART_CMD_GET_INFO          1103L       /* 获取info                                                     */
#define FSYS_PART_CMD_FORMAT            1104L       /* 格式化                                                       */
#define FSYS_PART_MODE_CACHE_USED       1002L       /* 启动cache机制                                                */
#define FSYS_PART_MODE_CACHE_UNUSED     1003L       /* 关闭cache机制                                                */
/******************** 旧part && dev ioctl cmd  ******************/

/* part I/O control command */
#define PART_IOC_USR_BASE               0x00000000
#define PART_IOC_SYS_BASE               0x80000000
#define IS_PARTIOCSYS(cmd)              (cmd>=PART_IOC_SYS_BASE)
/* for sys */
#define PART_IOC_SYS_GETNAME            (PART_IOC_SYS_BASE + 0)
#define PART_IOC_SYS_SETFSPRIV          (PART_IOC_SYS_BASE + 1)
#define PART_IOC_SYS_GETFSPRIV          (PART_IOC_SYS_BASE + 2)
//#define PART_IOC_SYS_GETPDPRIV          (PART_IOC_SYS_BASE + 3)
#define PART_IOC_SYS_GETDEV             (PART_IOC_SYS_BASE + 4)
#define PART_IOC_SYS_GETLETTER          (PART_IOC_SYS_BASE + 5)
#define PART_IOC_SYS_GETLASTPART        (PART_IOC_SYS_BASE + 6)

/* for raw part */
#define PART_IOC_USR_GETPARTSIZE        (PART_IOC_USR_BASE+0)
#define PART_IOC_USR_FLUSHCACHE         (PART_IOC_USR_BASE+1)
#define PART_IOC_USR_CACHEUSED          (PART_IOC_USR_BASE+2)
#define PART_IOC_USR_CACHEUNUSED        (PART_IOC_USR_BASE+3)
#define PART_IOC_USR_GETSCTSIZE        	(PART_IOC_USR_BASE+4)
/* for dms part */
#define PART_IOC_USR_GETITEM            (PART_IOC_USR_BASE+5)
#define PART_IOC_USR_GETDEVCLSROOT      (PART_IOC_USR_BASE+6)
/* for cd-rom part */
#define PART_IOC_CDROM_LAST_WRITTEN		(PART_IOC_USR_BASE+7)
#define PART_IOC_CDROM_MULTISESSION		(PART_IOC_USR_BASE+8)

/* fs I/O control command */
#define FS_IOC_USR_BASE                 0x00000000
#define FS_IOC_SYS_BASE                 0x80000000
#define IS_FSIOCSYS(cmd)                (cmd>=FS_IOC_SYS_BASE)
/* for sys */
#define	FS_IOC_SYS_GETFLAGS             (FS_IOC_SYS_BASE+0x80)
#define	FS_IOC_SYS_SETFLAGS             (FS_IOC_SYS_BASE+0x81)
#define	FS_IOC_SYS_GETVERSION           (FS_IOC_SYS_BASE+0x82)
#define	FS_IOC_SYS_SETVERSION           (FS_IOC_SYS_BASE+0x83)
#define	FS_IOC_SYS_GETATTR              (FS_IOC_SYS_BASE+0x84)

/* for fat fs */
#define FS_IOC_USR_DEBUG_FATCHUNK       (FS_IOC_SYS_BASE+0x100)
#define FSYS_SECTOR_SIZE                512
#define FSYS_PART_MEDIACHANGED          0x0001

/* for minfs fs */
#define MINFS_IOC_GET_UNCOMPRESS_FILE_SIZE  (FS_IOC_SYS_BASE+0x200)
#define MINFS_IOC_GET_UNCOMPRESS_FILE_DATA  (FS_IOC_SYS_BASE+0x201)

#define DEVFS_NODE_TYPE                 1
#define DEVFS_CLASS_TYPE                2

/* flags for esFSYS_statfs (get file system status) operation */
#define FSYS_KSTATUS_TYPE       0x00000001  /* get fs magic type            */
#define FSYS_KSTATUS_BSIZE      0x00000002  /* get fs block size            */
#define FSYS_KSTATUS_BLOCKS     0x00000004  /* get fs total blocks number   */
#define FSYS_KSTATUS_BFREE      0x00000008  /* get fs free blocks number(for NTFS it is very slowly) */
#define FSYS_KSTATUS_FILES      0x00000010  /* get fs total files number    */
#define FSYS_KSTATUS_NAMELEN    0x00000100  /* get fs name length           */
#define FSYS_KSTATUS_ATTR       0x00000200  /* get fs attribute(eg:READONLY and so on) */
#define FSYS_KSTATUS_MEDIAATTR  0x00000400  /* get fs media attribute       */
#define FSYS_KSTATUS_VOLLETTER  0x00000800  /* get fs volume letter         */
#define FSYS_KSTATUS_VOLNAME    0x00001000  /* get fs volume name           */
#define FSYS_KSTATUS_FSNAME     0x00002000  /* get fs type name             */
#define FSYS_KSTATUS_ALL        0x0000FFFF  /* get fs all kernal status information*/


//************************************************
// 变量定义
/* GLOBAL VARIABLES */


//************************************************
// 函数定义
/* system call table */
typedef struct
{
    __pSWI_t esFSYS_opendir         ;
    __pSWI_t esFSYS_closedir        ;
    __pSWI_t esFSYS_readdir         ;
    __pSWI_t esFSYS_rewinddir       ;
    __pSWI_t esFSYS_mkdir           ;
    __pSWI_t esFSYS_rmdir           ;

    __pSWI_t esFSYS_remove          ;
    __pSWI_t esFSYS_rename          ;
    __pSWI_t esFSYS_fopen           ;
    __pSWI_t esFSYS_fclose          ;
    __pSWI_t esFSYS_fread           ;
    __pSWI_t esFSYS_fwrite          ;
    __pSWI_t esFSYS_fseek           ;
    __pSWI_t esFSYS_ftell           ;
    __pSWI_t esFSYS_fseekex         ;
    __pSWI_t esFSYS_ftellex         ;
    __pSWI_t esFSYS_fioctrl         ;
    __pSWI_t esFSYS_ferror          ;
    __pSWI_t esFSYS_ferrclr         ;

    __pSWI_t esFSYS_premove         ;
    __pSWI_t esFSYS_prename         ;
    __pSWI_t esFSYS_popen           ;
    __pSWI_t esFSYS_pclose          ;
    __pSWI_t esFSYS_pread           ;
    __pSWI_t esFSYS_pwrite          ;
    __pSWI_t esFSYS_pioctrl         ;
    __pSWI_t esFSYS_perror          ;
    __pSWI_t esFSYS_perrclr         ;

    __pSWI_t esFSYS_mntparts        ;
    __pSWI_t esFSYS_umntparts       ;
    __pSWI_t esFSYS_fsreg           ;
    __pSWI_t esFSYS_fsunreg         ;
    __pSWI_t esFSYS_pdreg           ;
    __pSWI_t esFSYS_pdunreg         ;
    __pSWI_t esFSYS_partfslck       ;
    __pSWI_t esFSYS_partfsunlck     ;
    __pSWI_t esFSYS_mntfs           ;
    __pSWI_t esFSYS_umntfs          ;
    __pSWI_t esFSYS_format          ;
    __pSWI_t esFSYS_statfs          ;
    __pSWI_t esFSYS_fsdbg           ;
    __pSWI_t esFSYS_fsync           ;
    __pSWI_t esFSYS_fstat           ;
    __pSWI_t esFSYS_statpt          ;
    __pSWI_t esFSYS_setfs           ;
    __pSWI_t esFSYS_getfscharset    ;

    /* main for POSIX interface support */
    __pSWI_t esFSYS_file2fd    		;
    __pSWI_t esFSYS_fd2file    		;
    __pSWI_t esFSYS_open    		;

    /* main for update media file information performance */
    __pSWI_t esFSYS_querypartupdateflag;
    __pSWI_t esFSYS_clearpartupdateflag;
    __pSWI_t esFSYS_ftruncate;

} SWIHandler_FSYS_t;
#ifndef SIM_PC_WIN
#define SYSCALL_FSYS(x,y) __swi(SWINO(SWINO_FSYS, SWIHandler_FSYS_t, y))x y
#else
#define SYSCALL_FSYS(x,y) x y
#endif

/* 目录操作 */
SYSCALL_FSYS(__hdle, esFSYS_opendir     )(const char * /*dirname*/);
SYSCALL_FSYS(__s32,  esFSYS_closedir    )(__hdle /*hDir*/);
SYSCALL_FSYS(__hdle, esFSYS_readdir     )(__hdle /*hDir*/);
SYSCALL_FSYS(void,   esFSYS_rewinddir   )(__hdle /*hDir*/);
SYSCALL_FSYS(__s32,  esFSYS_mkdir       )(const char * /*pdirname*/);
SYSCALL_FSYS(__s32,  esFSYS_rmdir       )(const char * /*pdirname*/);
/* 文件操作 */
SYSCALL_FSYS(__s32,  esFSYS_remove      )(const char * /*filename*/);
SYSCALL_FSYS(__s32,  esFSYS_rename      )(const char * /*filename*/, const char * /*newname*/);
SYSCALL_FSYS(__hdle, esFSYS_fopen       )(const char * /*filename*/, const char * /*pMode*/);
SYSCALL_FSYS(__s32,  esFSYS_fclose      )(__hdle /*hFile*/);
SYSCALL_FSYS(__u32,  esFSYS_fread       )(void * /*pdata*/,
        __u32 /*regfile: blksize, chrdevfile: 1,        blkdevfile: sector pos*/,
        __u32 /*regfile: blkcnt,  chrdevfile: byte cnt, blkdevfile: sector cnt*/,
        __hdle /*hFile*/);
SYSCALL_FSYS(__u32,  esFSYS_fwrite      )(const void * /*pdata*/,
        __u32 /*regfile: blksize, chrdev: 1,        blkdev: sector pos*/,
        __u32 /*regfile: blkcnt,  chrdev: byte cnt, blkdev: sector cnt*/,
        __hdle /*hFile*/);
SYSCALL_FSYS(__s32,  esFSYS_fseek       )(__hdle /*hFile*/, __s32 /*offset*/, __s32 /*whence*/);
SYSCALL_FSYS(__s32,  esFSYS_ftell       )(__hdle /*hFile*/);
SYSCALL_FSYS(__s32,  esFSYS_fseekex     )(__hdle /*hFile*/, __s32 /*l_offset*/, __s32 /* h_offset*/, __s32 /*whence*/);
SYSCALL_FSYS(__s32,  esFSYS_ftellex     )(__hdle /*hFile*/, __s32 * /*l_offset*/, __s32 * /* h_offset*/);
SYSCALL_FSYS(__s32,  esFSYS_fioctrl     )(__hdle /*hFile*/, __s32 /*Cmd*/, __s32 /*Aux*/, void * /*pBuffer*/);
SYSCALL_FSYS(__s32,  esFSYS_ferror      )(__hdle /*hFile*/);
SYSCALL_FSYS(void,   esFSYS_ferrclr     )(__hdle /*hFile*/);
/* 分区操作 */
SYSCALL_FSYS(__s32,  esFSYS_premove     )(const char * /*partname*/);
SYSCALL_FSYS(__s32,  esFSYS_prename     )(const char * /*oldname*/, const char * /*newname*/);
SYSCALL_FSYS(__hdle, esFSYS_popen       )(const char * /*partname*/, const char * /*pMode*/);
SYSCALL_FSYS(__s32,  esFSYS_pclose      )(__hdle /*hPart*/);
SYSCALL_FSYS(__u32,  esFSYS_pread       )(void * /*pdata*/,
        __u32 /*chrpart: 1,         blkpart: sector pos*/,
        __u32 /*chrpart: bytes cnt, blkpart: sector cnt*/,
        __hdle /*pPart*/);
SYSCALL_FSYS(__u32,  esFSYS_pwrite      )(const void * /*pdata*/,
        __u32 /*chrpart: 1,         blkpart: sector pos*/,
        __u32 /*chrpart: bytes cnt, blkpart: sector cnt*/,
        __hdle /*pPart*/);
SYSCALL_FSYS(__s32,  esFSYS_pioctrl     )(__hdle /*pPart*/, __u32 /*cmd*/, __s32 /*aux*/, void * /*pbuffer*/);
SYSCALL_FSYS(__s32,  esFSYS_perror      )(__hdle /*hFile*/);
SYSCALL_FSYS(void,   esFSYS_perrclr     )(__hdle /*hFile*/);
/* 其它操作 */
SYSCALL_FSYS(__s32,  esFSYS_mntparts    )(__hdle /*hNode*/);
SYSCALL_FSYS(__s32,  esFSYS_umntparts   )(__hdle /*hNode*/, __u32 /*mode*/);
SYSCALL_FSYS(__s32,  esFSYS_fsreg       )(__hdle /*hFS*/);
SYSCALL_FSYS(__s32,  esFSYS_fsunreg     )(__hdle /*hFS*/);
SYSCALL_FSYS(__s32,  esFSYS_pdreg       )(__hdle /*hPD*/);
SYSCALL_FSYS(__s32,  esFSYS_pdunreg     )(__hdle /*hPD*/);

SYSCALL_FSYS(__s32,  esFSYS_partfslck   )(char * );
SYSCALL_FSYS(__s32,  esFSYS_partfsunlck )(char * );
SYSCALL_FSYS(__s32,  esFSYS_mntfs       )(__hdle /*hPart*/);
SYSCALL_FSYS(__s32,  esFSYS_umntfs      )(__hdle /*hPart*/, __s32 /*force*/);

SYSCALL_FSYS(__s32,  esFSYS_format      )(const char * /*partname*/, const char * /*fsname*/, __hdle /*fmtpara*/);
SYSCALL_FSYS(__s32,  esFSYS_statfs      )(const char * /*pathname*/, __hdle /*buf*/, __u32 /*flags*/);
SYSCALL_FSYS(__s32,  esFSYS_fsdbg       )(const char * /*cmd*/, const char * /*para*/);
SYSCALL_FSYS(__s32,  esFSYS_fsync       )(__hdle /*hFile*/);
SYSCALL_FSYS(__s32,  esFSYS_fstat       )(__hdle /*hFile*/, void * /*stat_buf*/);
SYSCALL_FSYS(__s32,  esFSYS_statpt      )(const char * /*PartName*/, __hdle /*stat buf*/);
SYSCALL_FSYS(__s32,  esFSYS_setfs       )(char * /*partname*/, __u32 /*cmd*/, __s32 /*aux*/, char * /*para*/);
SYSCALL_FSYS(__s32,  esFSYS_getfscharset)(const char * /*partname*/, __s32 * /*pCharset*/);
SYSCALL_FSYS(__s32,  esFSYS_file2fd)(__hdle	/* hFile */);
SYSCALL_FSYS(__hdle, esFSYS_fd2file)(__s32 /*fd*/);
SYSCALL_FSYS(__s32,  esFSYS_open)(const char * /*name*/, int /*flag*/, int /*prems*/);
SYSCALL_FSYS(__s32,  esFSYS_querypartupdateflag)(const char *path, __bool *flag);
SYSCALL_FSYS(__s32,  esFSYS_clearpartupdateflag)(const char *path);
SYSCALL_FSYS(__s32,  esFSYS_ftruncate)(__hdle  filehandle, __u32 length);


//**********************************************************************************************************

//**********************************************************************************************************
//* define level 1( system level)
//************************************************
// 常数定义

//************************************************
// 数据类型定义

//************************************************
// 变量定义
extern SWIHandler_FSYS_t SWIHandler_FSYS;
//************************************************
// 函数定义
__u32 FSYS_Init (void);
__u32 FSYS_Exit (void);

//**********************************************************************************************************

#endif  /* _FSYS_H_ */


