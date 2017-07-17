/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : photo_uipara.h
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef  __PHOTO_UIPARA_H_
#define   __PHOTO_UIPARA_H_

#include "beetles_app.h" //#include "app_photo_i.h"

//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////
#define LCD_WIDTH                  400
#define LCD_HEIGHT                 240

#define SSET_MAIN_LAYER_W          LCD_WIDTH
#define SSET_MAIN_LAYER_H          59
#define SSET_MAIN_LAYER_Y          (LCD_HEIGHT - SSET_MAIN_LAYER_H)

#define MENU_LIST_BTN_W            118//88
#define MENU_LIST_BTN_H            24
#define MENU_LIST_BTN_TOP          3
#define MENU_LIST_BTN_BOTTOM       3
#define MENU_LIST_BTN_PREV         5
#define MENU_LIST_BTN_NEXT         5
#define MENU_LIST_BOTTOM           176

#define SSET_LIST_ITEM_SP          3
#define SSET_LIST_ITEM_SP_H        (MENU_LIST_BTN_H + SSET_LIST_ITEM_SP)
#define SSET_LIST_LAYER_H_(_n)     (MENU_LIST_BTN_TOP + SSET_LIST_ITEM_SP_H * (_n) - SSET_LIST_ITEM_SP + MENU_LIST_BTN_BOTTOM)
#define SSET_LIST_LAYER_H_EX(_n)   (MENU_LIST_BTN_PREV + SSET_LIST_ITEM_SP_H * (_n) - SSET_LIST_ITEM_SP + MENU_LIST_BTN_NEXT)

#define SSET_ORIGIN_LAYER_H        SSET_LIST_LAYER_H_(2)
#define SSET_ORIGIN_LAYER_W        MENU_LIST_BTN_W

#define SSET_BG_MUSIC_LAYER_H      SSET_LIST_LAYER_H_(2)
#define SSET_BG_MUSIC_LAYER_W      MENU_LIST_BTN_W

#define SSET_ZOOM_LAYER_H          104
#define SSET_ZOOM_LAYER_W          22

#define SSET_ROTATE_LAYER_H        66
#define SSET_ROTATE_LAYER_W        66

#define SSET_BRIGHTNESS_LAYER_H    118
#define SSET_BRIGHTNESS_LAYER_W    18

#define SSET_SLIDESHOW_LAYER_H     SSET_LIST_LAYER_H_EX(6)
#define SSET_SLIDESHOW_LAYER_W     MENU_LIST_BTN_W

#define SSET_INTERVAL_LAYER_H      SSET_LIST_LAYER_H_(4)
#define SSET_INTERVAL_LAYER_W      MENU_LIST_BTN_W

#define SSET_SCALE_LAYER_H         SSET_LIST_LAYER_H_(4)
#define SSET_SCALE_LAYER_W         MENU_LIST_BTN_W

#define BMP_BIG_PAUSE_W            81
#define BMP_BIG_PAUSE_H            36

#define MSET_ITEM_MAX		       8
#define MSET_ITEM_PER_PAGE         5
#define MENU_BTN_W                 43
#define MENU_BTN_H                 41
#define PHOTO_FONT_H               16
#define PHOTO_FONT_BYTE_W          (PHOTO_FONT_H / 2)
#define MENU_BTN_SP_W              (2)
#define MENU_BTN_SP_X_(_i)         (((_i) + 1) * (LCD_WIDTH / MSET_ITEM_PER_PAGE) - (MENU_BTN_SP_W / 2))
/////////////////////////////////////
#define SSET_ZOOM_BG_X                  0//366
#define SSET_ZOOM_BG_Y                  0//60
#define SSET_ZOOM_BG_W                  22
#define SSET_ZOOM_BG_H                  104

#define MENU_LIST_ITEM_W                MENU_LIST_BTN_W//88
#define MENU_LIST_ITEM_H                24
#define MENU_LIST_ITEM_TOP              2
#define MENU_LIST_ITEM_BOTTOM           MENU_LIST_ITEM_TOP
#define MENU_LIST_STR_X_OFFSET          30//20
#define MENU_LIST_STR_Y_OFFSET          4
#define MENU_LIST_ITEM_SP               3
#define MENU_LIST_ITEM_UP               5
#define MENU_LIST_ITEM_DOWN             MENU_LIST_ITEM_UP

#define LONG_STR_H                      18

//ID_MENU_LIST_ITEM_
#define ID_ICON_TOP                     0
#define ID_ICON_BOTTOM                  1
#define ID_ICON_SPLINE                  2
#define ID_ICON_BAR                     3
#define ID_ICON_SEL_N                   4 //select normal
#define ID_ICON_SEL_F                   5 //select focus
#define ID_ICON_BG                      6

#define LIST_SLIDESHOW_ITEM_PAGE_COUNT  6

//bright
#define BRINGT_STR_X_OFFSET             0
#define BRINGT_STR_Y_OFFSET             0
#define BRINGT_STR_Y1_OFFSET            12

#define BRINGT_BLOCK_H                  9
#define BRINGT_BLOCK_BOTTOM             103
#define BRINGT_BLOCK_Y_OFFSET           BRINGT_BLOCK_BOTTOM//(BRINGT_BLOCK_BOTTOM - BRINGT_BLOCK_H)
#define BRINGT_BLOCK_X_OFFSET           4
/////////////////////////////////////
#define COLOUR_BG_8BPP                  BEETLES_BLACK_COLOR
#define COLOUR_EDIT_BG_8BPP             BEETLES_GRAY_COLOR

#define COLOUR_TO_8BPP_INDEX_0          0xF0
#define COLOUR_TO_8BPP_INDEX_(_i)       (COLOUR_TO_8BPP_INDEX_0 + (_i))
//////////////////////////////////////////////////////////////////////////
#define _USE_MEMDEV_Handle_             1//temp for debug
//////////////////////////////////////////////////////////////////////////
typedef struct tag_photo_rect
{
    __s16 x;
    __s16 y;
    __s16 w;
    __s16 h;
} photo_rect_t;

typedef struct tag_photo_uipara
{
    photo_rect_t 	spsc_layer;
    photo_rect_t 	sset_main_layer;

    photo_rect_t	sset_origin_layer;
    photo_rect_t	sset_bg_muisc_layer;
    photo_rect_t	sset_zoom_layer;
    photo_rect_t	sset_rotate_layer;
    photo_rect_t	sset_brightness_layer;
    photo_rect_t	sset_slideshow_layer;
    photo_rect_t	sset_interval_layer;
    photo_rect_t	sset_scale_layer;

    photo_rect_t	bmp_big_pause;
    photo_rect_t	bmp_play_status;

    photo_rect_t  bmp_set_bg;
    photo_rect_t  bmp_set_item;
    photo_rect_t  bmp_set_bar_top;
    photo_rect_t  bmp_set_bar_bottom;
    photo_rect_t  bmp_set_bar;
    photo_rect_t  bmp_set_bar_up;
    photo_rect_t  bmp_set_bar_down;
    photo_rect_t  bmp_set_line;
    photo_rect_t  bmp_set_block;
    photo_rect_t  bmp_set_vertical_line;
    photo_rect_t  bmp_set_horizontal_line;

    photo_rect_t  bmp_zoom_bar;
    photo_rect_t  bmp_zoom_block;
    photo_rect_t  bmp_rotate;
    photo_rect_t  bmp_bright_bar;
    photo_rect_t  bmp_bright_block;

    photo_rect_t  bmp_volume;
    photo_rect_t  bmp_slide_bg;
    photo_rect_t  bmp_slide_block;
    photo_rect_t  bmp_slide_bar;
} photo_uipara_t;

photo_uipara_t *get_photo_uipara(void);

#endif

