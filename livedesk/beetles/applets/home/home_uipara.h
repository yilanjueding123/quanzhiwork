/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	:main_uipara.h
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __MAIN_UIPARA_H_
#define __MAIN_UIPARA_H_

#include "app_home_i.h"

typedef struct tag_main_uipara_pos
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
    __s32 reserve;
} main_uipara_pos_t;

#define MAX_SUB_ITEM		4

typedef struct tag_main_uipara_t
{
    main_uipara_pos_t lyr_main_menu;
    main_uipara_pos_t lyr_sub_item;

    //main menu
    __s32 item_width;
    __s32 total_item_num;
    __s32 item_per_screen;
    __s32 max_main_bmp_width;
    __s32 max_main_bmp_height;

    //sub menu
    __s32 bmp_item_width;
    __s32 bmp_item_height;
    __s32 bmp_line_width;
    __s32 bmp_line_height;
    __s32 bmp_top_width;
    __s32 bmp_top_height;
    __s32 bmp_bottom_width;
    __s32 bmp_bottom_height;
} home_uipara_t;

home_uipara_t *home_get_ui_para(__s32 rotate);


#endif

