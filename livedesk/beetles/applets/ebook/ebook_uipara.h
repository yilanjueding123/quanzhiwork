/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : ebook_uipara.ch
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __EBOOK_UIPARA_H__
#define  __EBOOK_UIPARA_H__

#include "app_ebook_i.h"

typedef struct tag_ebook_res
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
} ebook_res_t;


typedef struct tag_ebook_uipara
{
    ebook_res_t  volume_bar_lyr;
    ebook_res_t  tips_lyr;
    ebook_res_t loading_lyr;
    ebook_res_t  page_lyr;
    ebook_res_t  mset_lyr;
    ebook_res_t  sset_bgmusic_lyr;
    ebook_res_t  sset_play_lyr;
    ebook_res_t  sset_txtcolor_lyr;
    ebook_res_t  sset_goto_lyr;

    ebook_res_t  bmp_mset_bg;
    ebook_res_t  bmp_mset_item;
    ebook_res_t  bmp_mset_line;

    ebook_res_t  bmp_sset_top;
    ebook_res_t  bmp_sset_btm;
    ebook_res_t  bmp_sset_bar;
    ebook_res_t  bmp_sset_line;

    ebook_res_t  bmp_play;
    ebook_res_t  bmp_battery;

    ebook_res_t bmp_slide_bar;
    ebook_res_t bmp_slide_block_l;
    ebook_res_t bmp_slide_block_r;
    ebook_res_t bmp_slide_block;
    ebook_res_t bmp_volume_l;
    ebook_res_t bmp_volume_r;

    ebook_res_t page_no_pos;
    ebook_res_t page_name_pos;
    ebook_res_t page_volume_value_pos;

    ebook_res_t	tips_txt_pos;
    ebook_res_t	tip_no_bmp_pos;
    ebook_res_t	tip_yes_bmp_pos;

    ebook_res_t	loading_bmp_pos;
    ebook_res_t	loading_txt_pos;
} ebook_uipara_t;

ebook_uipara_t *get_ebook_uipara(void);

#endif

