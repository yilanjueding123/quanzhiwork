
/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: spsc_prog_ui.h
* By      	: Bayden.chen
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#ifndef __MOVIE_SPSC_PROG_UIPARA_H_
#define  __MOVIE_SPSC_PROG_UIPARA_H_

#include "beetles_app.h"

#define MOVIE_SPSC_PROG_MAX_ICON_RES_NUM 2

typedef struct
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
    __s32 res_id[MOVIE_SPSC_PROG_MAX_ICON_RES_NUM];
    HTHEME res_hdl[MOVIE_SPSC_PROG_MAX_ICON_RES_NUM];
} movie_spsc_prog_rect_t;

typedef enum
{
    movie_spsc_prog_icon_playsta = 0,
    movie_spsc_prog_icon_prog_bg,
    movie_spsc_prog_icon_prog_cursor,
    movie_spsc_prog_icon_num
} movie_spsc_prog_icon_t;

typedef struct
{
    movie_spsc_prog_rect_t uipara_frm;
    movie_spsc_prog_rect_t uipara_icon[movie_spsc_prog_icon_num];
    movie_spsc_prog_rect_t uipara_spsc_prog_cur_time[3];
    movie_spsc_prog_rect_t uipara_spsc_prog_cur_time_dot[2];
    movie_spsc_prog_rect_t uipara_spsc_prog_total_time[3];
    movie_spsc_prog_rect_t uipara_spsc_prog_total_time_dot[2];
} movie_spsc_prog_uipara_t;

movie_spsc_prog_uipara_t *movie_spsc_prog_get_uipara(__s32 rotate);

#endif
