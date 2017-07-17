
/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: bright_ui.h
* By      	: Bayden.chen
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#ifndef __MOVIE_BRIGHT_UIPARA_H_
#define  __MOVIE_BRIGHT_UIPARA_H_

#include "beetles_app.h"

#define MOVIE_BRIGHT_MAX_ICON_RES_NUM 2

typedef struct
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
    __s32 res_id[MOVIE_BRIGHT_MAX_ICON_RES_NUM];
    HTHEME res_hdl[MOVIE_BRIGHT_MAX_ICON_RES_NUM];
} movie_bright_rect_t;

typedef enum
{
    movie_bright_icon_lbright = 0,
    movie_bright_icon_rbright,
    movie_bright_icon_prog_bg,
    movie_bright_icon_prog_cursor,
    movie_bright_icon_num
} movie_bright_icon_t;

typedef struct
{
    __s32 pipe;
    movie_bright_rect_t uipara_lyr;
    movie_bright_rect_t uipara_frm;
    movie_bright_rect_t uipara_bright_bg;
    movie_bright_rect_t uipara_icon[movie_bright_icon_num];
    movie_bright_rect_t uipara_bright_text;
} movie_bright_uipara_t;

movie_bright_uipara_t *movie_bright_get_uipara(__s32 rotate);

#endif
