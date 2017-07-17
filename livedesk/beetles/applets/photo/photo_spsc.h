/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : photo_spsc.h
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/
#ifndef __PHOTO_SPSC_H__
#define  __PHOTO_SPSC_H__

#include "app_photo_i.h"

typedef struct tag_photo_spsc_para
{
    H_WIN layer;
    GUI_FONT *spsc_font;

    __s32 cur_no;
    __s32 total_no;
    __s32 volume;
    __s32 play_status;
    __s32 isShowVolume;

    char  name[128];
    ES_TIMESTAMP time;
    __s32 file_size;
    char  place[1024];
    __s32 width;
    __s32 height;
    __s32 resolution;
    __s32 camera_model;
    __s32 exposurebais;
    __s32 flash;
    __s32 focus_length;
    __s32 color_space;

} photo_spsc_para_t;

typedef enum
{
    CMD_PHOTO_SPSC_GET_INFO,
    CMD_PHOTO_SPSC_PLAY,
    CMD_PHOTO_SPSC_PAUSE,
    CMD_PHOTO_SPSC_PLAYFILE,
    CMD_PHOTO_SPSC_VOLUME,
    CMD_PHOTO_SPSC_CLOSE,
    CMD_PHOTO_SPSC_SET,
} photo_spsc_cmd_t;

typedef enum
{
    PHOTO_STAT_PLAY,
    PHOTO_STAT_PAUSE,
    PHOTO_STAT_NEXT,
    PHOTO_STAT_PREV,
} photo_play_status_t;

typedef enum
{
    MSG_PHOTO_SPSC_SET_NO = (GUI_MSG_USER_DEF + 1),
} photo_spsc_msg_t;

H_WIN photo_spsc_win_create(H_WIN h_parent, photo_spsc_para_t *para);
__s32 photo_spsc_win_delete(H_WIN spsc_win);

__s32 update_pic_info(__s32 index, photo_spsc_para_t *spsc_para);
__s32 photo_spsc_set_no(__gui_msg_t *msg);

#endif

