/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: main_uipara.c
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include "home_uipara.h"
#include "main_menu.h"
#include "sub_menu.h"

static home_uipara_t uipara_400_240 =
{
    {
        0, 171, 400, 240 - 171
    },
    {
        0, 40, 118, 123
    },
    400 / 5, //一屏5个
    7,
    5,//一屏5个
    45,//最大宽度
    45,//最大高度

    118,//item width
    24,//item height
    118,//line width
    3,//line height
    118,//top width
    2,//top height
    118,//bottom width
    6//bottom height
};
//x,y,w,h
static home_uipara_t uipara_480_272 =
{
    {
        //0, 185, 480, 272-185
        0, 108, 480, 272 - 108
        // 0,129,480,272-129
    },
    {
        0, 50, 118, 50
        //  0, 56, 118, 123
        // 0, 0, 118, 123
    },
    480 / 4, //一屏5个
    4,//总共7个图标
    4,//一屏5个
    56,// 45,//最大图标宽度
    56,// 45,//最大图标高度

    118,//item width
    24,//item height
    118,//line width
    3,//line height
    118,//top width
    2,//top height
    118,//bottom width
    6//bottom height
};

home_uipara_t *home_get_ui_para(__s32 rotate)
{
    __s32           screen_width;
    __s32 			screen_height;

    /* get lcd size*/
    dsk_display_get_size(&screen_width, &screen_height);
    __msg("screen_width=%d,screen_height=%d\n", screen_width, screen_height);
    switch(rotate)
    {
    case GUI_SCNDIR_NORMAL:
    case GUI_SCNDIR_ROTATE180:
    {
        if((screen_width == 400 ) && ( screen_height == 240))
            return &uipara_400_240;
        else if((screen_width == 480 ) && ( screen_height == 272))
            return &uipara_480_272;

    }
    case GUI_SCNDIR_ROTATE90:
    case GUI_SCNDIR_ROTATE270:

        break;
    }

    return &uipara_400_240;
}



