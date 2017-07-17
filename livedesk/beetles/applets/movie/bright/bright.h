/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: bright.h
* By      	: Bayden.chen
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-06  create this file
**************************************************************************************************************
*/

#ifndef __MOVIE_BRIGHT_H__
#define  __MOVIE_BRIGHT_H__

#include "beetles_app.h"
#include "bright_ui.h"

//发往父窗口的消息
typedef enum
{
    movie_bright_scene_msg_timeout = 0,
    movie_bright_scene_msg_brightset,
    movie_bright_scene_msg_num
} movie_bright_scene_msg_t;

typedef struct
{
    H_WIN hparent;
    __s32 scene_id;
    __s32 min_val;
    __s32 max_val;
    __s32 cur_val;
} movie_bright_create_para_t;

void *movie_bright_scene_create(movie_bright_create_para_t *create_para);
__s32 movie_bright_scene_delete(void *handle);
__s32 movie_bright_scene_set_focus(void *handle);

#endif
