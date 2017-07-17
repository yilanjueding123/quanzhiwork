/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : app_ebook.h
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __APP_EBOOK_H__
#define  __APP_EBOOK_H__

#include "app_ebook_i.h"

typedef enum tag_AUTO_TIME
{
    AUTO_TIME_FAST = 0,
    AUTO_TIME_MIDDLE,
    AUTO_TIME_SLOW,
    AUTO_TIME_OFF,
} __auto_time_e;

typedef enum tag_BG_MUSIC
{
    BG_MUSIC_OFF = 0,
    BG_MUSIC_ON,
} __bg_music_e;

typedef enum tag_BOOK_COLOR
{
    BOOK_COLOR_BLACK = 0,
    BOOK_COLOR_WHITE,
    BOOK_COLOR_YELLOW,
    BOOK_COLOR_
} __book_color_e;

typedef struct __ebook_bkpoint_inf
{
    __u32   page_no;                /* 上次退出电子书的页码        */
    char    ebookname[RAT_MAX_FULL_PATH_LEN + 1]; /* 上次退出电子书时的电子书的名字  */

} __ebook_bkpoint_t;

__s32 app_ebook_create(root_para_t *para);

#endif

