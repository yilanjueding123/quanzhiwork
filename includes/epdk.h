/*
************************************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Developent Kits
*									           ePDK interface
*
*                                   (c) Copyright 2007-2008, steven.zgj.china
*                                            All Rights Reserved
*
* File    : epdk.h
* By      : Steven.ZGJ
* Version : V0.1
*
*	若需要使用ePDK做开发，只需要将此头文件添加到自己的代码中，同时要将eslibc.o + epdk.o文件也添加到工程文件中。
*做完以上动作，你就可以调用ePDK公开的所有系统调用和驱动
*
************************************************************************************************************************
*/

#ifndef EPDK_H
#define EPDK_H


#include    "cfgs.h"        /* 系统配置     */
#include    "typedef.h"     /* 类型定义     */

#if(EPDK_OS == EPDK_OS_EPOS)
#include    "configs.h"
#include    "dbgs.h"
#include    "ebios.h"       /* bios调用     */
#include	"epos.h"        /* 系统调用     */
#include    "emod.h"        /* module调用   */
#include    "elibs.h"       /* 标准库      */
#include    "cplus.h"       /* c++库        */

#elif((EPDK_OS == EPDK_OS_WIN2K) || (EPDK_OS == EPDK_OS_WINXP))

#else
#error please select os platform!
#endif
//#define HBAR_SHOW_BT

//#define DV_FREQ_UPPER_ARRANGEMENT

#define EPDK_AUDIO_READY 1
#define EPDK_SPI_READY 1
#define EPDK_AUTO_PLAY_MOVIE_ENABLE 	0
#define EPDK_PLAY_VIDEO_AUDIO_ENABLE 	0
#define EPDK_LOAD_FROM_SD 0
#define	EPDK_ROOTFS_PATH						"f:\\rootfs\\"
#define	EPDK_TRANS_KEY_LONGLEFT2LONGVADD		1
#define EPDK_USE_ONLY_ONE_SD 1//多媒体卡和启动卡是否同一个
#define	EPDK_USE_EBOOK_TEST_TVIN				0
#define EPDK_PLAY_AUDIO_ENABLE			1
/**********************************************************************************************************************/
#endif		//#define EPDK_H

