#ifndef __RAT_PARTITION_H
#define __RAT_PARTITION_H

#define RAT_CD_ROM		"ExternalDisk2"
#define RAT_USB_DISK	"ExternalDisk1"
#define RAT_SD_CARD		"ExternalDisk0"
#define RAT_LOCAL_DISK  "LocalDisk"

#define RAT_MAX_PARTITION		11

/*!
* \brief
*     分区对应的设备
*/
typedef enum
{
    RAT_UNKNOWN = 0,      //未知
    RAT_LOCAL,          //本地磁盘
    RAT_TF,             //TF卡
    RAT_USB,            //USB设备
    RAT_CD,             //CD-ROM
    RAT_MEDIA           //媒体库
} rat_root_t;

/*!
* \brief
*     分区属性
*/
typedef enum
{
    RAT_READONLY = 0x01,    //只读
    RAT_READWRITE           //可读写
} rat_pati_attr_t;

/*
**********************************************************************************************************************
*
*
* author:
*
* date:
*
* Description:       获得盘符信息
*
* parameters:       disk_type: RAT_SD_CARD RAT_USB_DISK RAT_LOCAL_DISK
					disk_name: 返回的盘符名
*
* return:           0 	get part success
					-1	part not exist
					-2	part not formatted
* modify history:
**********************************************************************************************************************
*/
__s32 rat_get_partition_name(char *disk_type, char disk_name[RAT_MAX_PARTITION][4]);

#endif//__RAT_PARTITION_H
