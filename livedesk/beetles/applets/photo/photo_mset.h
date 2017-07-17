/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : photo_mset.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __PHOTO_MSET_H_
#define  __PHOTO_MSET_H_

#include "app_photo_i.h"


typedef struct tag_photo_mset_para
{
    GUI_FONT *mset_font;
    H_WIN      layer;

    __s32	source;
    __s32	bg_music;
    __s32   zoom;
    __s32	rotate;
    __s32	bright;
    __s32   interval;
    __s32   scale;

    H_WIN   h_spsc;
    __s32	focus;
    __s32   first;
} photo_mset_para_t;

typedef enum
{
    ID_PHOTO_MSET_SAVE_BG,
    ID_PHOTO_MSET_BGMUSIC,
    ID_PHOTO_MSET_ZOOM,
    ID_PHOTO_MSET_ROTATE,
    ID_PHOTO_MSET_BRIGHT,
    ID_PHOTO_MSET_SLIDESHOW,
    ID_PHOTO_MSET_INTERVAL,
    ID_PHOTO_MSET_SCALE,
} photo_mset_id_t;

typedef enum
{
    CMD_PHOTO_MSET_SW_ITEM,
    CMD_PHOTO_MSET_UP,
    CMD_PHOTO_MSET_DOWN,
    CMD_PHOTO_MSET_LEFT,
    CMD_PHOTO_MSET_RIGHT,
    CMD_PHOTO_MSET_ENTER,
    CMD_PHOTO_MSET_CLOSE,
} photo_mset_cmd_t;

typedef enum
{
    MSG_PHOTO_MSET_LR_ADJ_SSET  = (GUI_MSG_USER_DEF + 1),
} MSG_PHOTO_MSET_E;


H_WIN photo_mset_win_create(H_WIN h_parent, photo_mset_para_t *para);

#endif
