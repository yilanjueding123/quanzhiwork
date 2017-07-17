/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           home app sample
*
*						        	(c) Copyright 2006-2009, China
*											 All Rights Reserved
*
* File    : dv_uipara.c
* By      : lincoln
* Version : V1.00
*********************************************************************************************************
*/
#include "dv_i.h"
#include "dv_uipara.h"

#if  0
#define __msg(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
						     eLIBs_printf(__VA_ARGS__)									        )
#else
#define __msg(...)   
#endif


static dv_uipara_para_t dv_para ;

void Dv_Uipara_Init( void )
{
    ES_FILE *pfile ;
	__s32 i;

    pfile = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if( !pfile )
    {
        __err("open display fail..........\n");
    }
    dv_para.scn_w = eLIBs_fioctrl( pfile, DISP_CMD_SCN_GET_WIDTH, 0, 0 );
    dv_para.scn_h = eLIBs_fioctrl( pfile, DISP_CMD_SCN_GET_HEIGHT, 0, 0 );
    eLIBs_fclose(pfile);
    //__log("dv_para.scn_w=%d,dv_para.scn_h=%d\n", dv_para.scn_w, dv_para.scn_h);

    dv_para.rec_setting_item_num = 2 ;
    dv_para.image_setting_item_num = 3;

    dv_para.work_mode_icon_pos.x = 2;//450;//10 ;
    dv_para.work_mode_icon_pos.y = 2; // 2;//0 ;

    dv_para.quantity_text_pos.x = 35 ;
    dv_para.quantity_text_pos.y = 2 ;

    dv_para.single0_handle = dsk_theme_open(ID_DV_RF_SIGNAL_0_BMP);
    dv_para.single1_handle = dsk_theme_open(ID_DV_RF_SIGNAL_1_BMP);
    dv_para.single2_handle = dsk_theme_open(ID_DV_RF_SIGNAL_2_BMP);
    dv_para.single3_handle = dsk_theme_open(ID_DV_RF_SIGNAL_3_BMP);
    dv_para.single4_handle = dsk_theme_open(ID_DV_RF_SIGNAL_FULL_BMP);

    dv_para.msg_box_bg = dsk_theme_open(ID_DV_MSG_BACK_BMP);
    dv_para.msg_box_size.width = 220 ;
    dv_para.msg_box_size.height = 120 ;

    dv_para.no_sd_id = STRING_NO_SD ;
    dv_para.disk_full_id = STRING_DISK_FULL ;

    dv_para.rec_quantity_strid[0] = STRING_REC_MP1 ;
    dv_para.rec_quantity_strid[1] = STRING_REC_MP1 ;
    dv_para.image_quantity_strid[0] = STRING_CAM_MP1;
    dv_para.image_quantity_strid[1] = STRING_CAM_MP2 ;
    dv_para.image_quantity_strid[2] = STRING_CAM_MP3;
		
}

void Dv_Uipara_UnInit( void )
{
	__s32 i;
		
    if(dv_para.single0_handle)
    {
        dsk_theme_close(dv_para.single0_handle);
        dv_para.single0_handle = NULL ;
    }
    if(dv_para.single1_handle)
    {
        dsk_theme_close(dv_para.single1_handle);
        dv_para.single1_handle = NULL ;
    }
    if(dv_para.single2_handle)
    {
        dsk_theme_close(dv_para.single2_handle);
        dv_para.single2_handle = NULL ;
    }
    if(dv_para.single3_handle)
    {
        dsk_theme_close(dv_para.single3_handle);
        dv_para.single3_handle = NULL ;
    }
    if(dv_para.single4_handle)
    {
        dsk_theme_close(dv_para.single4_handle);
        dv_para.single4_handle = NULL ;
    }

    if(dv_para.msg_box_bg)
    {
        dsk_theme_close(dv_para.msg_box_bg);
        dv_para.msg_box_bg = NULL ;
    }
    if(dv_para.hbar_back_handle)
    {
        dsk_theme_close(dv_para.hbar_back_handle);
        dv_para.hbar_back_handle = NULL ;
    }
	
}

dv_uipara_para_t *Dv_GetUipara( void )
{
    return &dv_para;
}


