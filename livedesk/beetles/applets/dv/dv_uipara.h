/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           home app sample
*
*						        	(c) Copyright 2006-2009, China
*											 All Rights Reserved
*
* File    : dv_uipara.h
* By      : lincoln
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __DV_UIPARA_H__
#define __DV_UIPARA_H__

//typedef struct __dv_sub_res
//{
//	load_bmp_res_t 	bmp_subset_singal[6];
//	//load_bmp_res_t  bmp_subset_charge[6];
//	load_bmp_res_t  bmp_subset_vol[6];
//	load_bmp_res_t  cam_play_pause[2];
//	
//} *dv_sub_res;


typedef struct dv_uipara_para
{
    __s32 				scn_w, 	scn_h ;

    // menu bar
    RECT        menu_bar_bg_rect;
    HTHEME		menu_bar_bg_handle ;
    //record setting backgroud
    RECT        rec_setting_bk_rect;
    HTHEME		rec_setting_bk_handle ;
    //picture setting backgroud
    RECT        image_setting_bk_rect;
    HTHEME		image_setting_bk_handle ;
    // focus item
    SIZE        select_item_size;
    HTHEME		select_focus_item_handle ;
    HTHEME		select_unfocus_item_handle ;

    // option item
    SIZE		opt_item_size;
    HTHEME		opt_item_handle ;

    __u32		rec_setting_item_num;
    __u32		image_setting_item_num;
    HTHEME		mp_fcs_handle;

    OFFSET		work_mode_icon_pos;
    HTHEME		hbar_back_handle;
    RECT        hbar_bg_rect;
    OFFSET		quantity_text_pos;
    HTHEME      single4_handle;
    HTHEME		single3_handle;
    HTHEME		single2_handle;
    HTHEME		single1_handle;
    HTHEME		single0_handle;

    HTHEME		msg_box_bg ;
    SIZE		msg_box_size ;
    __u32		no_sd_id ;
    __u32		disk_full_id ;

    __u32  		rec_quantity_strid[2] ;
    __u32  		image_quantity_strid[3] ;
//	dv_sub_res  sub_dv_res;
} dv_uipara_para_t;

void Dv_Uipara_Init( void );
void Dv_Uipara_UnInit( void );
void DV_Uipara_Subset_UnInit(void);

dv_uipara_para_t *Dv_GetUipara( void );


#endif /* __DV_UIPARA_H__ */
