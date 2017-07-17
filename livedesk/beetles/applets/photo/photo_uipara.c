/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : photo_uipara.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#include "photo_uipara.h"

//////////////////////////////////////////////////////////////////////////
static photo_uipara_t photo_uipara =
{
    {
        //spsc_layer
        0, 0, LCD_WIDTH, LCD_HEIGHT,
    },
    {
        //sset_main_layer
        0, SSET_MAIN_LAYER_Y, SSET_MAIN_LAYER_W, SSET_MAIN_LAYER_H,
    },
    {
        //sset_origin_layer
        4, (MENU_LIST_BOTTOM - SSET_ORIGIN_LAYER_H), SSET_ORIGIN_LAYER_W, SSET_ORIGIN_LAYER_H,
    },
    {
        //sset_bg_muisc_layer
        54, (MENU_LIST_BOTTOM - SSET_BG_MUSIC_LAYER_H), SSET_BG_MUSIC_LAYER_W, SSET_BG_MUSIC_LAYER_H,
    },
    {
        //sset_zoom_layer
        366, 60, SSET_ZOOM_LAYER_W, SSET_ZOOM_LAYER_H,
    },
    {
        //sset_rotate_layer
        316, 12, SSET_ROTATE_LAYER_W, SSET_ROTATE_LAYER_H,
    },
    {
        //sset_brightness_layer
        316, 50, SSET_BRIGHTNESS_LAYER_W, SSET_BRIGHTNESS_LAYER_H,
    },
    {
        //sset_slideshow_layer
        252, (MENU_LIST_BOTTOM - SSET_SLIDESHOW_LAYER_H), SSET_SLIDESHOW_LAYER_W, SSET_SLIDESHOW_LAYER_H,
    },
    {
        //sset_interval_layer
        302, (MENU_LIST_BOTTOM - SSET_INTERVAL_LAYER_H), SSET_INTERVAL_LAYER_W, SSET_INTERVAL_LAYER_H,
    },
    {
        //sset_scale_layer
        LCD_WIDTH - SSET_SCALE_LAYER_W - 1, (MENU_LIST_BOTTOM - SSET_SCALE_LAYER_H), SSET_SCALE_LAYER_W, SSET_SCALE_LAYER_H,
    },
    {
        //bmp_big_pause
        0, 0, BMP_BIG_PAUSE_W, BMP_BIG_PAUSE_H,
    },
    {
        //bmp_play_status
        0, 0, BMP_BIG_PAUSE_W, BMP_BIG_PAUSE_H,
    },
    {
        //bmp_set_bg
        0, 0, 400, 59,
    },
    {
        //bmp_set_item
        0, 0, MENU_BTN_W, MENU_BTN_H,
    },
    {
        //bmp_set_bar_top
        0, 0, MENU_LIST_BTN_W, 3,
    },
    {
        //bmp_set_bar_bottom
        0, 0, MENU_LIST_BTN_W, 3,
    },
    {
        //bmp_set_bar
        0, 0, MENU_LIST_BTN_W, MENU_LIST_BTN_H,
    },
    {
        //bmp_set_bar_up
        0, 0, MENU_LIST_BTN_W, 5,
    },
    {
        //bmp_set_bar_down
        0, 0, MENU_LIST_BTN_W, 5,
    },
    {
        //bmp_set_line
        0, 0, 0, 0
    },
    {
        //bmp_set_block
        0, 0, 30, 30,//???
    },
    {
        //bmp_set_vertical_line
        0, 0, 1, 86,//???
    },
    {
        //bmp_set_horizontal_line
        0, 0, 230, 1,//???
    },
    {
        //bmp_zoom_bar
        0, 0, 22, 104,
    },
    {
        //bmp_zoom_block
        0, 0, 10, 1,
    },
    {
        //bmp_rotate
        0, 0, 66, 66,
    },
    {
        //bmp_bright_bar
        0, 0, 12, 108,
    },
    {
        //bmp_bright_block
        0, 0, 11, 9,
    },
    {
        //bmp_volume
        0, 0, 15, 13,
    },
    {
        //bmp_slide_bg
        10, 10, 356, 24,//???
    },
    {
        //bmp_slide_block
        0, 0, 5, 4,
    },
    {
        //bmp_slide_bar
        0, 0, 255, 4,
    }
};

photo_uipara_t *get_photo_uipara(void)
{
    /*
    photo_uipara.spsc_layer.x = 0;
    photo_uipara.spsc_layer.y = 0;
    photo_uipara.spsc_layer.w = 800;
    photo_uipara.spsc_layer.h = 600;

    photo_uipara.sset_main_layer.x = 0;
    photo_uipara.sset_main_layer.y = 514;
    photo_uipara.sset_main_layer.w = 800;
    photo_uipara.sset_main_layer.h = 86;

    photo_uipara.sset_origin_layer.x = 5;
    photo_uipara.sset_origin_layer.y = 400;
    photo_uipara.sset_origin_layer.w = 230;
    photo_uipara.sset_origin_layer.h = 106;

    photo_uipara.sset_bg_muisc_layer.x = 100;
    photo_uipara.sset_bg_muisc_layer.y = 400;
    photo_uipara.sset_bg_muisc_layer.w = 230;
    photo_uipara.sset_bg_muisc_layer.h = 106;

    photo_uipara.sset_zoom_layer.x = 600;
    photo_uipara.sset_zoom_layer.y = 50;
    photo_uipara.sset_zoom_layer.w = 200;
    photo_uipara.sset_zoom_layer.h = 394;

    photo_uipara.sset_rotate_layer.x = 650;
    photo_uipara.sset_rotate_layer.y = 50;
    photo_uipara.sset_rotate_layer.w = 104;
    photo_uipara.sset_rotate_layer.h = 106;

    photo_uipara.sset_brightness_layer.x = 700;
    photo_uipara.sset_brightness_layer.y = 150;
    photo_uipara.sset_brightness_layer.w = 30;
    photo_uipara.sset_brightness_layer.h = 276;

    photo_uipara.sset_slideshow_layer.x = 430;
    photo_uipara.sset_slideshow_layer.y = 160;
    photo_uipara.sset_slideshow_layer.w = 230;
    photo_uipara.sset_slideshow_layer.h = 346;

    photo_uipara.sset_interval_layer.x = 540;
    photo_uipara.sset_interval_layer.y = 300;
    photo_uipara.sset_interval_layer.w = 230;
    photo_uipara.sset_interval_layer.h = 197;

    photo_uipara.sset_scale_layer.x = 550;
    photo_uipara.sset_scale_layer.y = 300;
    photo_uipara.sset_scale_layer.w = 230;
    photo_uipara.sset_scale_layer.h = 197;

    photo_uipara.bmp_big_pause.x = 0;
    photo_uipara.bmp_big_pause.y = 0;
    photo_uipara.bmp_big_pause.w = 52;
    photo_uipara.bmp_big_pause.h = 52;

    photo_uipara.bmp_play_status.x = 0;
    photo_uipara.bmp_play_status.y = 0;
    photo_uipara.bmp_play_status.w = 96;
    photo_uipara.bmp_play_status.h = 42;

    photo_uipara.bmp_set_bg.x = 0;
    photo_uipara.bmp_set_bg.y = 0;
    photo_uipara.bmp_set_bg.w = 800;
    photo_uipara.bmp_set_bg.h = 86;

    photo_uipara.bmp_set_item.x = 0;
    photo_uipara.bmp_set_item.y = 0;
    photo_uipara.bmp_set_item.w = 90;
    photo_uipara.bmp_set_item.h = 86;

    photo_uipara.bmp_set_bar_top.x = 0;
    photo_uipara.bmp_set_bar_top.y = 0;
    photo_uipara.bmp_set_bar_top.w = 230;
    photo_uipara.bmp_set_bar_top.h = 8;

    photo_uipara.bmp_set_bar_bottom.x = 0;
    photo_uipara.bmp_set_bar_bottom.y = 0;
    photo_uipara.bmp_set_bar_bottom.w = 230;
    photo_uipara.bmp_set_bar_bottom.h = 8;

    photo_uipara.bmp_set_bar.x = 0;
    photo_uipara.bmp_set_bar.y = 0;
    photo_uipara.bmp_set_bar.w = 230;
    photo_uipara.bmp_set_bar.h = 44;

    photo_uipara.bmp_set_bar_up.x = 0;
    photo_uipara.bmp_set_bar_up.y = 0;
    photo_uipara.bmp_set_bar_up.w = 230;
    photo_uipara.bmp_set_bar_up.h = 14;

    photo_uipara.bmp_set_bar_down.x = 0;
    photo_uipara.bmp_set_bar_down.y = 0;
    photo_uipara.bmp_set_bar_down.w = 230;
    photo_uipara.bmp_set_bar_down.h = 14;


    photo_uipara.bmp_set_block.x = 0;
    photo_uipara.bmp_set_block.y = 0;
    photo_uipara.bmp_set_block.w = 30;
    photo_uipara.bmp_set_block.h = 30;

    photo_uipara.bmp_set_vertical_line.x = 0;
    photo_uipara.bmp_set_vertical_line.y = 0;
    photo_uipara.bmp_set_vertical_line.w = 1;
    photo_uipara.bmp_set_vertical_line.h = 86;

    photo_uipara.bmp_set_horizontal_line.x = 0;
    photo_uipara.bmp_set_horizontal_line.y = 0;
    photo_uipara.bmp_set_horizontal_line.w = 230;
    photo_uipara.bmp_set_horizontal_line.h = 1;

    photo_uipara.bmp_zoom_bar.x = 0;
    photo_uipara.bmp_zoom_bar.y = 0;
    photo_uipara.bmp_zoom_bar.w = 46;
    photo_uipara.bmp_zoom_bar.h = 276;

    photo_uipara.bmp_zoom_block.x = 0;
    photo_uipara.bmp_zoom_block.y = 0;
    photo_uipara.bmp_zoom_block.w = 20;
    photo_uipara.bmp_zoom_block.h = 12;

    photo_uipara.bmp_rotate.x = 0;
    photo_uipara.bmp_rotate.y = 0;
    photo_uipara.bmp_rotate.w = 104;
    photo_uipara.bmp_rotate.h = 106;

    photo_uipara.bmp_bright_bar.x = 0;
    photo_uipara.bmp_bright_bar.y = 0;
    photo_uipara.bmp_bright_bar.w = 30;
    photo_uipara.bmp_bright_bar.h = 276;

    photo_uipara.bmp_bright_block.x = 0;
    photo_uipara.bmp_bright_block.y = 0;
    photo_uipara.bmp_bright_block.w = 20;
    photo_uipara.bmp_bright_block.h = 20;

    photo_uipara.bmp_volume.x = 0;
    photo_uipara.bmp_volume.y = 0;
    photo_uipara.bmp_volume.w = 28;
    photo_uipara.bmp_volume.h = 30;

    photo_uipara.bmp_slide_bg.x = 10;
    photo_uipara.bmp_slide_bg.y = 10;
    photo_uipara.bmp_slide_bg.w = 762;
    photo_uipara.bmp_slide_bg.h = 38;

    photo_uipara.bmp_slide_bar.x = 0;
    photo_uipara.bmp_slide_bar.y = 0;
    photo_uipara.bmp_slide_bar.w = 472;
    photo_uipara.bmp_slide_bar.h = 8;

    photo_uipara.bmp_slide_block.x = 0;
    photo_uipara.bmp_slide_block.y = 0;
    photo_uipara.bmp_slide_block.w = 4;
    photo_uipara.bmp_slide_block.h = 8;*/

    return (&photo_uipara);
}

