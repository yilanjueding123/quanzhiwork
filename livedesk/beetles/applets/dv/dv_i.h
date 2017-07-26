/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           home app sample
*
*						        	(c) Copyright 2006-2009, China
*											 All Rights Reserved
*
* File    : home_i.h
* By      : lyn
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __DV_I_H__
#define __DV_I_H__


#include "beetles_app.h"
#include "core\\cvr_core.h"
#include "core\\cvr_show.h"
#include "core\\cvr_watermark.h"

#define     USER_DISK           "F:"                        // 文件存储盘
#define     DISK_FULL_LEVEL     (1024 * 1024 * 100 * 2)      // 100M空间(预留空间，当磁盘检测发现不足50M空间时即开始删除文件)
#define     FILE_NAME_PRIX       "AW_"                       // 文件前缀
#define     FILE_NAME_SUFFIX		".avi"
#define     FILE_NAME_IMAGE			".jpg"
#define     CVR_HDMI_ENABLE         0                       // HDMI支持开关
#define     CVR_TVOUT_ENABLE        0                       // TVOUT支持开关

#define 	DV_FRM_WIN_ID				(APP_DV_ID+100)
#define 	DV_MENU_WIN_ID			(APP_DV_ID+110)
#define 	DV_DIALOAG_WIN_ID			(APP_DV_ID+120)

#define TIMER_STATUS_TIMEROUT_ID      100
typedef enum dv_TIMER
{
    TIMER_STATUS_ID = 0x23,//0x100,            // 状态检测TIMER
    TIMER_DIALOAG_TIMEOUT_ID ,
    TIMER_DV_SRCH_ID = 0x21,
    TIMER_DV_SRCH_OVER_ID = 0x22,
    TIMER_DV_BATT_CHCK_ID = 0X25,
} dv_timer_e;

// 录像分辨率, vga， 或者HD(720P)
typedef enum dv_record_resuliton
{
    dv_record_resuliton_VGA,
    dv_record_resuliton_HD,
} dv_record_resuliton_e;

// 拍照分辨率1M,2M,3M
typedef enum dv_image_resuliton
{
    dv_image_resuliton_1M,
    dv_image_resuliton_2M,
    dv_image_resuliton_3M,
} dv_image_resuliton_e;


typedef enum dv_PAINT
{
    DV_PAINT_FLASH_ICON1 = 100,
    DV_PAINT_FLASH_ICON2,
    DV_PAINT_MENUBAR_BG,
    DV_CLEAR_MENUBAR_BG,
    DV_PAINT_SINGLE0,
    DV_PAINT_SINGLE1,
    DV_PAINT_SINGLE2,
    DV_PAINT_SINGLE3,
    DV_PAINT_SINGLE4,
} dv_paint_e;

typedef enum dv_STATE
{
    DV_ON_REC,                        // 录像场景
    DV_ON_CAM,                        // 拍照场景
    DV_ON_REC_SET,                    // 录像设置场景
    DV_ON_CAM_SET,                    // 拍照设置场景
} dv_state_e;


#endif /* __DV_I_H__ */


