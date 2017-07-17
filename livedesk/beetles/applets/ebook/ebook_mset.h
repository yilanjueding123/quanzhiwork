/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : ebook_mset.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __EBOOK_MSET_H_
#define  __EBOOK_MSET_H_

#include "app_ebook_i.h"


typedef struct tag_ebook_mset_para
{
    GUI_FONT *mset_font;
    H_WIN      layer;
    __s32	   focus_id;
} ebook_mset_para_t;

typedef enum
{
    ID_EBOOK_MSET_BG_MUSIC,
    ID_EBOOK_MSET_AUTO,
    ID_EBOOK_MSET_TXT_COLOR,
    ID_EBOOK_MSET_GOTO_PAGE,
} ebook_mset_id_t;

typedef enum
{
    CMD_EBOOK_MSET_SW_ITEM,
    CMD_EBOOK_MSET_UP,
    CMD_EBOOK_MSET_DOWN,
    CMD_EBOOK_MSET_LEFT,
    CMD_EBOOK_MSET_RIGHT,
    CMD_EBOOK_MSET_ENTER,
    CMD_EBOOK_MSET_CLOSE,
    CMD_EBOOK_MSET_RECORD_LAST_FOCUS,
} ebook_mset_cmd_t;

typedef enum
{
    MSG_EBOOK_MSET_LR_ADJ_SSET  = (GUI_MSG_USER_DEF + 1),
};


H_WIN ebook_mset_win_create(H_WIN h_parent, ebook_mset_para_t *para);

#endif
