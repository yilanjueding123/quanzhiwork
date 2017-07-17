
/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: volume_ui.h
* By      	: Bayden.chen
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#ifndef __MOVIE_VOLUME_UIPARA_H_
#define  __MOVIE_VOLUME_UIPARA_H_

#include "beetles_app.h"

#define MOVIE_VOLUME_MAX_ICON_RES_NUM 2

typedef struct
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
    __s32 res_id[MOVIE_VOLUME_MAX_ICON_RES_NUM];
    HTHEME res_hdl[MOVIE_VOLUME_MAX_ICON_RES_NUM];
} movie_volume_rect_t;

typedef enum
{
    movie_volume_icon_lvolume = 0,
    movie_volume_icon_rvolume,
    movie_volume_icon_prog_bg,
    movie_volume_icon_prog_cursor,
    movie_volume_icon_num
} movie_volume_icon_t;

typedef struct
{
    __s32 pipe;
    movie_volume_rect_t uipara_lyr;
    movie_volume_rect_t uipara_frm;
    movie_volume_rect_t uipara_volume_bg;
    movie_volume_rect_t uipara_icon[movie_volume_icon_num];
    movie_volume_rect_t uipara_volume_text;
} movie_volume_uipara_t;

movie_volume_uipara_t *movie_volume_get_uipara(__s32 rotate);

#endif
