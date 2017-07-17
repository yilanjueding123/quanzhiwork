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

#ifndef __PHOTO_SSET_H_
#define  __PHOTO_SSET_H_

#include "app_photo_i.h"

typedef enum
{
    MSG_PHOTO_SSET_ENTER = (GUI_MSG_USER_DEF + 1),
    MSG_PHOTO_SSET_UP,
    MSG_PHOTO_SSET_DOWN,
    MSG_PHOTO_SSET_LEFT,
    MSG_PHOTO_SSET_RIGHT,
} photo_sset_msg_t;

typedef enum
{
    CMD_PHOTO_SSET_SOURCE,
    CMD_PHOTO_SSET_BGMUSIC,
    CMD_PHOTO_SSET_ZOOM,
    CMD_PHOTO_SSET_ROTATE,
    CMD_PHOTO_SSET_BRIGHT,
    CMD_PHOTO_SSET_SLIDESHOW,
    CMD_PHOTO_SSET_INTERVAL,
    CMD_PHOTO_SSET_SCALE,
    CMD_PHOTO_SSET_LR_CTRL,
    CMD_PHOTO_SSET_MOVE_UP,
    CMD_PHOTO_SSET_MOVE_DOWN,
    CMD_PHOTO_SSET_MOVE_LEFT,
    CMD_PHOTO_SSET_MOVE_RIGHT,
} photo_Sset_cmd_t;

typedef struct tag_photo_sset_para
{
    GUI_FONT *sset_font;
    H_WIN      layer;
    __s32	  main_id;
    __s32	  value;
    __s32	interval_time;
} photo_sset_para_t;

H_WIN photo_sset_win_create(H_WIN h_parent, photo_sset_para_t *para);

#endif
