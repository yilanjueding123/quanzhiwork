/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : app_ebook_i.h
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __APP_EBOOK_I_H__
#define  __APP_EBOOK_I_H__

#include "beetles_app.h"
#include "common/common.h"

#define app_sprintf                            eLIBs_sprintf

#define  EBOOK_PAGE_ID		(APP_EBOOK_ID+1)
#define  EBOOK_MSET_ID		(APP_EBOOK_ID+2)
#define  EBOOK_SSET_ID		(APP_EBOOK_ID+3)
#define  EBOOK_TIPS_ID		(APP_EBOOK_ID+4)
#define  EBOOK_LOADING_ID		(APP_EBOOK_ID+5)

#define  APP_EBOOK_FCS_TXT_COLOR	APP_COLOR_YELLOW
#define  APP_EBOOK_UNFCS_TXT_COLOR	APP_COLOR_WHITE

__s32 Ebook_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);

#endif

