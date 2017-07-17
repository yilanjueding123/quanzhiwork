/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               file disk dirver
*
*                                    (c) Copyright 2006-2010, Sunny China
*                                             All Rights Reserved
*
* File    : drv_filedisk.h
* By      : Sunny
* Version : v1.0
* Date    : 2010-9-7
* Descript: dirver for mount a file image as one partition.
* Update  : date                auther      ver     notes
*           2010-9-7 14:28:24   Sunny       1.0     dirver for mount a file image.
*********************************************************************************************************
*/

#ifndef __DRV_FILE_DISK_H__
#define __DRV_FILE_DISK_H__

/* file disk control commnads */
#define DRV_FDISK_CMD_MOUNT     1   /* mount an image   */
#define DRV_FDISK_CMD_UNMOUNT   2   /* unmount an image */

/*  */
#define DRV_FDISK_CMD_GET_VALID_FMT_TABLE       3   /* get valid image file format table */
#define DRV_FDISK_CMD_FREE_VALID_FMT_TABLE      4   /* free get table */

#endif  /* __DRV_FILE_DISK_H__ */
