/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lion
* File    : apps.h
*
* By      : andy.zhang
* Version : v1.0
* Date    : 2008-10-4 11:28:56
**********************************************************************************************************************
*/
#ifndef __APPS_H__
#define __APPS_H__

#define BEETLES_APP_ROOT   "d:\\"//"f:\\rootfs\\"//程序和资源的所在盘符.如果非调试，则定义为"d:\\"
#define BEETLES_RTC_EXIST	0//fk关闭日期显示
#define BEETLES_BLACK_COLOR	0x99000000
#define BEETLES_GRAY_COLOR	0x99A0A0A0
#include "epdk.h"
#include "mod_orange.h"
#include "mod_orange\orange_api_entry.h"
//#include "mod_flash.h"
#include "applets/app_load_para.h"
#include "applets/tv_visible_area.h"

#include "elibs\lib_ex.h"

#include "mod_desktop\desktop.h"

#include "init\init.h"

#include "libapps\libapps_i.h"

#include "res\lang.h"
#include "res\theme.h"

//#define DV_VIDEO_PHOTO_DIR


#endif
