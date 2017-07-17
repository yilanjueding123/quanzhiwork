/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: volume.h
* By      	: Bayden.chen
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-06  create this file
**************************************************************************************************************
*/

#ifndef __MOVIE_VOLUME_H__
#define  __MOVIE_VOLUME_H__

#include "beetles_app.h"
#include "volume_ui.h"

//发往父窗口的消息
typedef enum
{
    movie_volume_scene_msg_timeout = 0,
    movie_volume_scene_msg_volumeset,
    movie_volume_scene_msg_num
} movie_volume_scene_msg_t;

typedef struct
{
    H_WIN hparent;
    __s32 scene_id;
    __s32 min_val;
    __s32 max_val;
    __s32 cur_val;
} movie_volume_create_para_t;

void *movie_volume_scene_create(movie_volume_create_para_t *create_para);
__s32 movie_volume_scene_delete(void *handle);
__s32 movie_volume_scene_set_focus(void *handle);

#endif
