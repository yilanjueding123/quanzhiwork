/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : photo_sset.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __EBOOK_SSET_H_
#define  __EBOOK_SSET_H_

#include "app_ebook_i.h"

typedef enum
{
    MSG_EBOOK_SSET_ENTER = (GUI_MSG_USER_DEF + 1),
    MSG_EBOOK_SSET_UP,
    MSG_EBOOK_SSET_DOWN,
    MSG_EBOOK_SSET_LEFT,
    MSG_EBOOK_SSET_RIGHT,
} ebook_sset_msg_t;

typedef enum
{
    CMD_EBOOK_SSET_BGMUSIC,
    CMD_EBOOK_SSET_AUTO,
    CMD_EBOOK_SSET_TXTCOLOR,
    CMD_EBOOK_SSET_GOTO,
    CMD_EBOOK_SSET_LR_CTRL,
    CMD_EBOOK_SSET_UPDATA_PAGE,
    MSG_EBOOK_SSET_RECORD_LAST_FOCUS_ID,
} ebook_Sset_cmd_t;

typedef struct tag_ebook_sset_para
{
    GUI_FONT *sset_font;
    H_WIN      layer;
    __s32	  main_id;
    __s32	  value;
    __s32	  total_page;
} ebook_sset_para_t;

H_WIN ebook_sset_win_create(H_WIN h_parent, ebook_sset_para_t *para);

#endif
