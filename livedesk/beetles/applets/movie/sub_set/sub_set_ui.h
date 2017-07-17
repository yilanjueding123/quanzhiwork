
/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: movie_ctrl_ui.h
* By      	: Bayden.chen
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#ifndef __MOVIE_SUB_SET_UIPARA_H_
#define  __MOVIE_SUB_SET_UIPARA_H_

#include "beetles_app.h"

#define MOVIE_SUB_SET_MAX_ICON_RES_NUM 2

typedef struct
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
    __s32 res_id[MOVIE_SUB_SET_MAX_ICON_RES_NUM];
    HTHEME res_hdl[MOVIE_SUB_SET_MAX_ICON_RES_NUM];
    __s32 lang_id;
} movie_sub_set_rect_t;

typedef struct
{
    __s32 res_id;
    HTHEME res_hdl;
} movie_sub_set_res2hdl_t;

typedef enum
{
    movie_sub_set_res_id = 0,
    movie_sub_set_hdl
} movie_sub_set_res2hdl_e;

typedef enum
{
    movie_sub_set_item_bmp_l = 0,
    movie_sub_set_item_bmp_r,
    movie_sub_set_item_bmp_num
} movie_sub_set_item_bmp_e;

typedef enum
{
    movie_sub_set_item_static_val = 0,
    movie_sub_set_item_static_num
} moive_sub_set_item_static_e;

typedef struct
{
    movie_sub_set_rect_t item_bmp[movie_sub_set_item_bmp_num];
    movie_sub_set_rect_t item_static_text[movie_sub_set_item_static_num];
} movie_sub_set_item_t;

//定义场景所有元素
typedef struct
{
    __s32                   pipe;
    movie_sub_set_rect_t	uipara_lyr;
    movie_sub_set_rect_t	uipara_frm;
    __s32				    focus_item_index;
    movie_sub_set_rect_t    uipara_bg;
    movie_sub_set_item_t    item[movie_sub_set_item_num];
    movie_sub_set_rect_t    static_text[movie_sub_set_static_icon_num];
    __s32                   item_text_langid[movie_sub_set_item_num][MOVIE_SUB_MAX_ITEM_TEXT_NR];

} movie_sub_set_uipara_t;

movie_sub_set_uipara_t *movie_sub_set_get_uipara(__s32 rotate);

#endif

