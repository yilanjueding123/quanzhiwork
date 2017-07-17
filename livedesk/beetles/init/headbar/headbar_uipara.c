/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: headbar_com.h
* By      	: Andy.zhang
* Func		: headbar manage
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include "mod_init_i.h"
#include "headbar_uipara.h"

__headbar_uipara_t  headbar_uipara;
GUI_FONT    *FONT16 = NULL;

static void shbar_palette_adjust(void)
{
    ES_FILE  *p_disp;
    __u32	  *palette_buf;
    HTHEME 	  htheme;

    htheme = dsk_theme_open(0);
    palette_buf = dsk_theme_hdl2buf(htheme);
    /*define palette's selfdefine area*/
    *(palette_buf + 0xC0) = 0x00000000;
    *(palette_buf + 0xC1) = 0xC0323232;

    p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if(!p_disp)
    {
        __wrn("open display fail\n");
        return ;
    }
    /* get palette */
    {
        __u32 	arg[3];

        arg[0] = (__u32)palette_buf;
        arg[1] = 0;
        arg[2] = ROBIN_PALETTE_COLOR_COUNT * 4;
        eLIBs_fioctrl( p_disp, DISP_CMD_SET_PALETTE_TBL, 0, (void *)arg );
    }

    eLIBs_fclose(p_disp);

    dsk_theme_close(htheme);

}

void headbar_init_uipara(__hbar_format_t format)
{
    int width;
    int height;
    int screen_type;

    dsk_display_get_size(&width, &height);

    if( (width == 400) && (height == 240) )
        screen_type = SCREEN_TYPE_400_240;
    else if( (width == 480) && (height == 272) )
        screen_type = SCREEN_TYPE_400_240;
    else if( (width == 720) && (height == 480) )
        screen_type = SCREEN_TYPE_720_480;
    else if( (width == 720) && (height == 576) )
        screen_type = SCREEN_TYPE_720_576;
    else if( ( width == 800 ) && ( height == 480 ) )
        screen_type = SCREEN_TYPE_800_480;
    else if( (width == 1024) && (height == 768) )
        screen_type = SCREEN_TYPE_1024_768;
    else if ( (width == 1280) && (height == 720) )
        screen_type = SCREEN_TYPE_1280_720;
    else if( (width == 1920) && (height == 1080) )
        screen_type = SCREEN_TYPE_1920_1080;
    else
        screen_type = SCREEN_TYPE_LAST;

    switch( screen_type )
    {
    case SCREEN_TYPE_400_240:
    {
        //layer
        headbar_uipara.hbar_lyr.fb_w  = 480;//400;
        headbar_uipara.hbar_lyr.fb_h  = 32;

        headbar_uipara.hbar_lyr.src_x = 0;
        headbar_uipara.hbar_lyr.src_y = 0;
        headbar_uipara.hbar_lyr.src_w = 480; // 400;
        headbar_uipara.hbar_lyr.src_h = 32;

        headbar_uipara.hbar_lyr.scn_x = 0;
        headbar_uipara.hbar_lyr.scn_y = 0;
        headbar_uipara.hbar_lyr.scn_w = 480;//400;
        headbar_uipara.hbar_lyr.scn_h = 32;

        headbar_uipara.hbar_lyr.pipe  = 1;
        //
        headbar_uipara.hbar_frmwin.x = 0;
        headbar_uipara.hbar_frmwin.y = 0;
        headbar_uipara.hbar_frmwin.w = 480;//400;
        headbar_uipara.hbar_frmwin.h = 32;

        headbar_uipara.title.x	= 80;
        headbar_uipara.title.y	= 0;
        headbar_uipara.title.w	= 180;
        headbar_uipara.title.h	= 32;

        headbar_uipara.music.x = 0;
        headbar_uipara.music.y = 0;
        headbar_uipara.music.w = 24;
        headbar_uipara.music.h = 32;

        headbar_uipara.date.x = 260;
        headbar_uipara.date.y = 16;
        headbar_uipara.date.w = 140;
        headbar_uipara.date.h = 16;

        headbar_uipara.time.x = 260;
        headbar_uipara.time.y = 0;
        headbar_uipara.time.w = 140;
        headbar_uipara.time.h = 16;

        headbar_uipara.volume_icon.x 	= 0;
        headbar_uipara.volume_icon.y 	= 0;
        headbar_uipara.volume_icon.w 	= 24;
        headbar_uipara.volume_icon.h 	= 32;

        headbar_uipara.volume_txt.x 	= headbar_uipara.volume_icon.x + headbar_uipara.volume_icon.w + 1;
        headbar_uipara.volume_txt.y 	= 0;
        headbar_uipara.volume_txt.w 	= 32;
        headbar_uipara.volume_txt.h 	= 32;

        headbar_uipara.bright_icon.x 	= 0;
        headbar_uipara.bright_icon.y 	= 0;
        headbar_uipara.bright_icon.w 	= 24;
        headbar_uipara.bright_icon.h 	= 32;

        headbar_uipara.bright_txt.x 	= headbar_uipara.bright_icon.x + headbar_uipara.bright_icon.w + 1;
        headbar_uipara.bright_txt.y 	= 0;
        headbar_uipara.bright_txt.w 	= 32;
        headbar_uipara.bright_txt.h 	= 32;

        headbar_uipara.battery.x 		= 440;//350;//80;
        headbar_uipara.battery.y 		= (32 - 12) / 2;
        headbar_uipara.battery.w 		= 30;
        headbar_uipara.battery.h 		= 12;

        headbar_uipara.res_id.vol_id0	= ID_INIT_ENERGY_0_BMP;
        headbar_uipara.res_id.vol_id1	= ID_INIT_ENERGY_1_BMP;
        headbar_uipara.res_id.vol_id2	= ID_INIT_ENERGY_2_BMP;
        headbar_uipara.res_id.vol_id3	= ID_INIT_ENERGY_3_BMP;
        headbar_uipara.res_id.vol_id4	= ID_INIT_ENERGY_4_BMP;
        headbar_uipara.res_id.vol_id5 	= ID_INIT_ENERGY_5_BMP;


        headbar_uipara.res_id.tf_n_id =  ID_INIT_SD_N_BMP;
        headbar_uipara.res_id.tf_y_id =  ID_INIT_SD_Y_BMP;

        headbar_uipara.res_id.charge_vol_id0	= ID_INIT_CHARGE_ENERGY_0_BMP;
        headbar_uipara.res_id.charge_vol_id1	= ID_INIT_CHARGE_ENERGY_1_BMP;
        headbar_uipara.res_id.charge_vol_id2	= ID_INIT_CHARGE_ENERGY_2_BMP;
        headbar_uipara.res_id.charge_vol_id3	= ID_INIT_CHARGE_ENERGY_3_BMP;
        headbar_uipara.res_id.charge_vol_id4	= ID_INIT_CHARGE_ENERGY_4_BMP;
        headbar_uipara.res_id.charge_vol_id5 	= ID_INIT_CHARGE_ENERGY_5_BMP;
        headbar_uipara.res_id.no_vol_id	        = ID_INIT_NO_VOL_BMP;
        headbar_uipara.res_id.usb_connect_vol_id = ID_INIT_USB_CONNECT_VOL_BMP;

        headbar_uipara.res_id.music_id 	= 0;
        headbar_uipara.res_id.bright_id	= 0;
        headbar_uipara.res_id.volume_id	= 0;

        headbar_uipara.font 			= FONT16;
        break;
    }
    case SCREEN_TYPE_720_480:
    {
        //layer
        headbar_uipara.hbar_lyr.fb_w  = 720;
        headbar_uipara.hbar_lyr.fb_h  = 32;

        headbar_uipara.hbar_lyr.src_x = 0;
        headbar_uipara.hbar_lyr.src_y = 0;
        headbar_uipara.hbar_lyr.src_w = 720;
        headbar_uipara.hbar_lyr.src_h = 32;

        headbar_uipara.hbar_lyr.scn_x = 0;
        headbar_uipara.hbar_lyr.scn_y = 0;
        headbar_uipara.hbar_lyr.scn_w = 720;
        headbar_uipara.hbar_lyr.scn_h = 32;

        headbar_uipara.hbar_lyr.pipe  = 1;
        //
        headbar_uipara.hbar_frmwin.x = 0;
        headbar_uipara.hbar_frmwin.y = 0;
        headbar_uipara.hbar_frmwin.w = 720;
        headbar_uipara.hbar_frmwin.h = 32;

        headbar_uipara.title.x	= 80;
        headbar_uipara.title.y	= 0;
        headbar_uipara.title.w	= 180;
        headbar_uipara.title.h	= 32;

        headbar_uipara.music.x = 0;
        headbar_uipara.music.y = 0;
        headbar_uipara.music.w = 24;
        headbar_uipara.music.h = 32;

        headbar_uipara.date.x = 260;
        headbar_uipara.date.y = 16;
        headbar_uipara.date.w = 140;
        headbar_uipara.date.h = 16;

        headbar_uipara.time.x = 260;
        headbar_uipara.time.y = 0;
        headbar_uipara.time.w = 140;
        headbar_uipara.time.h = 16;

        headbar_uipara.volume_icon.x 	= 0;
        headbar_uipara.volume_icon.y 	= 0;
        headbar_uipara.volume_icon.w 	= 24;
        headbar_uipara.volume_icon.h 	= 32;

        headbar_uipara.volume_txt.x 	= headbar_uipara.volume_icon.x + headbar_uipara.volume_icon.w + 1;
        headbar_uipara.volume_txt.y 	= 0;
        headbar_uipara.volume_txt.w 	= 32;
        headbar_uipara.volume_txt.h 	= 32;

        headbar_uipara.bright_icon.x 	= 0;
        headbar_uipara.bright_icon.y 	= 0;
        headbar_uipara.bright_icon.w 	= 24;
        headbar_uipara.bright_icon.h 	= 32;

        headbar_uipara.bright_txt.x 	= headbar_uipara.bright_icon.x + headbar_uipara.bright_icon.w + 1;
        headbar_uipara.bright_txt.y 	= 0;
        headbar_uipara.bright_txt.w 	= 32;
        headbar_uipara.bright_txt.h 	= 32;

        headbar_uipara.battery.x 		= 80;
        headbar_uipara.battery.y 		= (32 - 12) / 2;
        headbar_uipara.battery.w 		= 30;
        headbar_uipara.battery.h 		= 12;

        headbar_uipara.res_id.vol_id0	= ID_INIT_ENERGY_0_BMP;
        headbar_uipara.res_id.vol_id1	= ID_INIT_ENERGY_1_BMP;
        headbar_uipara.res_id.vol_id2	= ID_INIT_ENERGY_2_BMP;
        headbar_uipara.res_id.vol_id3	= ID_INIT_ENERGY_3_BMP;
        headbar_uipara.res_id.vol_id4	= ID_INIT_ENERGY_4_BMP;
        headbar_uipara.res_id.vol_id5 	= ID_INIT_ENERGY_5_BMP;

        headbar_uipara.res_id.charge_vol_id0	= ID_INIT_CHARGE_ENERGY_0_BMP;
        headbar_uipara.res_id.charge_vol_id1	= ID_INIT_CHARGE_ENERGY_1_BMP;
        headbar_uipara.res_id.charge_vol_id2	= ID_INIT_CHARGE_ENERGY_2_BMP;
        headbar_uipara.res_id.charge_vol_id3	= ID_INIT_CHARGE_ENERGY_3_BMP;
        headbar_uipara.res_id.charge_vol_id4	= ID_INIT_CHARGE_ENERGY_4_BMP;
        headbar_uipara.res_id.charge_vol_id5 	= ID_INIT_CHARGE_ENERGY_5_BMP;
        headbar_uipara.res_id.no_vol_id	        = ID_INIT_NO_VOL_BMP;
        headbar_uipara.res_id.usb_connect_vol_id = ID_INIT_USB_CONNECT_VOL_BMP;

        headbar_uipara.res_id.music_id 	= 0;
        headbar_uipara.res_id.bright_id	= 0;
        headbar_uipara.res_id.volume_id	= 0;

        headbar_uipara.font 			= FONT16;
        break;
    }

    case SCREEN_TYPE_720_576:
    {
        //layer
        headbar_uipara.hbar_lyr.fb_w  = 720;
        headbar_uipara.hbar_lyr.fb_h  = 32;

        headbar_uipara.hbar_lyr.src_x = 0;
        headbar_uipara.hbar_lyr.src_y = 0;
        headbar_uipara.hbar_lyr.src_w = 720;
        headbar_uipara.hbar_lyr.src_h = 32;

        headbar_uipara.hbar_lyr.scn_x = 0;
        headbar_uipara.hbar_lyr.scn_y = 0;
        headbar_uipara.hbar_lyr.scn_w = 720;
        headbar_uipara.hbar_lyr.scn_h = 32;

        headbar_uipara.hbar_lyr.pipe  = 1;
        //
        headbar_uipara.hbar_frmwin.x = 0;
        headbar_uipara.hbar_frmwin.y = 0;
        headbar_uipara.hbar_frmwin.w = 720;
        headbar_uipara.hbar_frmwin.h = 32;

        headbar_uipara.title.x	= 80;
        headbar_uipara.title.y	= 0;
        headbar_uipara.title.w	= 180;
        headbar_uipara.title.h	= 32;

        headbar_uipara.music.x = 0;
        headbar_uipara.music.y = 0;
        headbar_uipara.music.w = 24;
        headbar_uipara.music.h = 32;

        headbar_uipara.date.x = 260;
        headbar_uipara.date.y = 16;
        headbar_uipara.date.w = 140;
        headbar_uipara.date.h = 16;

        headbar_uipara.time.x = 260;
        headbar_uipara.time.y = 0;
        headbar_uipara.time.w = 140;
        headbar_uipara.time.h = 16;

        headbar_uipara.volume_icon.x 	= 0;
        headbar_uipara.volume_icon.y 	= 0;
        headbar_uipara.volume_icon.w 	= 24;
        headbar_uipara.volume_icon.h 	= 32;

        headbar_uipara.volume_txt.x 	= headbar_uipara.volume_icon.x + headbar_uipara.volume_icon.w + 1;
        headbar_uipara.volume_txt.y 	= 0;
        headbar_uipara.volume_txt.w 	= 32;
        headbar_uipara.volume_txt.h 	= 32;

        headbar_uipara.bright_icon.x 	= 0;
        headbar_uipara.bright_icon.y 	= 0;
        headbar_uipara.bright_icon.w 	= 24;
        headbar_uipara.bright_icon.h 	= 32;

        headbar_uipara.bright_txt.x 	= headbar_uipara.bright_icon.x + headbar_uipara.bright_icon.w + 1;
        headbar_uipara.bright_txt.y 	= 0;
        headbar_uipara.bright_txt.w 	= 32;
        headbar_uipara.bright_txt.h 	= 32;

        headbar_uipara.battery.x 		= 80;
        headbar_uipara.battery.y 		= (32 - 12) / 2;
        headbar_uipara.battery.w 		= 30;
        headbar_uipara.battery.h 		= 12;

        headbar_uipara.res_id.vol_id0	= ID_INIT_ENERGY_0_BMP;
        headbar_uipara.res_id.vol_id1	= ID_INIT_ENERGY_1_BMP;
        headbar_uipara.res_id.vol_id2	= ID_INIT_ENERGY_2_BMP;
        headbar_uipara.res_id.vol_id3	= ID_INIT_ENERGY_3_BMP;
        headbar_uipara.res_id.vol_id4	= ID_INIT_ENERGY_4_BMP;
        headbar_uipara.res_id.vol_id5 	= ID_INIT_ENERGY_5_BMP;



        headbar_uipara.res_id.charge_vol_id0	= ID_INIT_CHARGE_ENERGY_0_BMP;
        headbar_uipara.res_id.charge_vol_id1	= ID_INIT_CHARGE_ENERGY_1_BMP;
        headbar_uipara.res_id.charge_vol_id2	= ID_INIT_CHARGE_ENERGY_2_BMP;
        headbar_uipara.res_id.charge_vol_id3	= ID_INIT_CHARGE_ENERGY_3_BMP;
        headbar_uipara.res_id.charge_vol_id4	= ID_INIT_CHARGE_ENERGY_4_BMP;
        headbar_uipara.res_id.charge_vol_id5 	= ID_INIT_CHARGE_ENERGY_5_BMP;
        headbar_uipara.res_id.no_vol_id	        = ID_INIT_NO_VOL_BMP;
        headbar_uipara.res_id.usb_connect_vol_id = ID_INIT_USB_CONNECT_VOL_BMP;

        headbar_uipara.res_id.music_id 	= 0;
        headbar_uipara.res_id.bright_id	= 0;
        headbar_uipara.res_id.volume_id	= 0;

        headbar_uipara.font 			= FONT16;
        break;
    }

    case SCREEN_TYPE_LAST:
    case SCREEN_TYPE_800_480:
    {
        /* hbar layer */
        headbar_uipara.hbar_lyr.fb_w  = 800;
        headbar_uipara.hbar_lyr.fb_h  = 30;

        headbar_uipara.hbar_lyr.src_x = 0;
        headbar_uipara.hbar_lyr.src_y = 0;
        headbar_uipara.hbar_lyr.src_w = 800;
        headbar_uipara.hbar_lyr.src_h = 30;

        headbar_uipara.hbar_lyr.scn_x = 0;
        headbar_uipara.hbar_lyr.scn_y = 0;
        headbar_uipara.hbar_lyr.scn_w = 800;
        headbar_uipara.hbar_lyr.scn_h = 30;

        headbar_uipara.hbar_lyr.pipe  = 1;
        //
        headbar_uipara.hbar_frmwin.x = 0;
        headbar_uipara.hbar_frmwin.y = 0;
        headbar_uipara.hbar_frmwin.w = 800;
        headbar_uipara.hbar_frmwin.h = 30;

        headbar_uipara.title.x	= 31;
        headbar_uipara.title.y	= 0;
        headbar_uipara.title.w	= 190;
        headbar_uipara.title.h	= 30;

        headbar_uipara.music.x = 138;
        headbar_uipara.music.y = 6;
        headbar_uipara.music.w = 24;
        headbar_uipara.music.h = 24;

        headbar_uipara.date.x = 228;
        headbar_uipara.date.y = 0;
        headbar_uipara.date.w = 100;
        headbar_uipara.date.h = 30;

        headbar_uipara.time.x = 379;
        headbar_uipara.time.y = 0;
        headbar_uipara.time.w = 100;
        headbar_uipara.time.h = 30;

        headbar_uipara.volume_icon.x 	= 540;
        headbar_uipara.volume_icon.y 	= 6;
        headbar_uipara.volume_icon.w 	= 24;
        headbar_uipara.volume_icon.h 	= 24;

        headbar_uipara.volume_txt.x 	= headbar_uipara.volume_icon.x + headbar_uipara.volume_icon.w + 1;
        headbar_uipara.volume_txt.y 	= 0;
        headbar_uipara.volume_txt.w 	= 32;
        headbar_uipara.volume_txt.h 	= 30;

        headbar_uipara.bright_icon.x 	= 652;
        headbar_uipara.bright_icon.y 	= 4;
        headbar_uipara.bright_icon.w 	= 24;
        headbar_uipara.bright_icon.h 	= 30;

        headbar_uipara.bright_txt.x 	= headbar_uipara.bright_icon.x + headbar_uipara.bright_icon.w + 1;
        headbar_uipara.bright_txt.y 	= 0;
        headbar_uipara.bright_txt.w 	= 32;
        headbar_uipara.bright_txt.h 	= 30;

        headbar_uipara.battery.x 		= 752;
        headbar_uipara.battery.y 		= 6;
        headbar_uipara.battery.w 		= 50;
        headbar_uipara.battery.h 		= 24;

        headbar_uipara.res_id.vol_id0	= 0;
        headbar_uipara.res_id.vol_id1	= 0;
        headbar_uipara.res_id.vol_id2	= 0;
        headbar_uipara.res_id.vol_id3	= 0;
        headbar_uipara.res_id.vol_id4	= 0;
        headbar_uipara.res_id.vol_id5 	= 0;

        headbar_uipara.res_id.music_id 	= 0;
        headbar_uipara.res_id.bright_id	= 0;
        headbar_uipara.res_id.volume_id	= 0;

        headbar_uipara.font 			    = FONT16;
        break;
    }

    case SCREEN_TYPE_1280_720:
    {
        /* hbar layer */
        headbar_uipara.hbar_lyr.fb_w  = 1280;
        headbar_uipara.hbar_lyr.fb_h  = 40;

        headbar_uipara.hbar_lyr.src_x = 0;
        headbar_uipara.hbar_lyr.src_y = 0;
        headbar_uipara.hbar_lyr.src_w = 1280;
        headbar_uipara.hbar_lyr.src_h = 40;

        headbar_uipara.hbar_lyr.scn_x = 0;
        headbar_uipara.hbar_lyr.scn_y = 28;
        headbar_uipara.hbar_lyr.scn_w = 1280;
        headbar_uipara.hbar_lyr.scn_h = 40;

        headbar_uipara.hbar_lyr.pipe  = 0;
        //
        headbar_uipara.hbar_frmwin.x = 0;
        headbar_uipara.hbar_frmwin.y = 0;
        headbar_uipara.hbar_frmwin.w = 1280;
        headbar_uipara.hbar_frmwin.h = 40;

        headbar_uipara.title.x	= 113;
        headbar_uipara.title.y	= 2;
        headbar_uipara.title.w	= 190;
        headbar_uipara.title.h	= 33;

        headbar_uipara.music.x = 277;
        headbar_uipara.music.y = 6;
        headbar_uipara.music.w = 29;
        headbar_uipara.music.h = 30;

        headbar_uipara.date.x = 437;
        headbar_uipara.date.y = 2;
        headbar_uipara.date.w = 100;
        headbar_uipara.date.h = 30;

        headbar_uipara.time.x = 640;
        headbar_uipara.time.y = 2;
        headbar_uipara.time.w = 100;
        headbar_uipara.time.h = 33;

        headbar_uipara.volume_icon.x 	= 864;
        headbar_uipara.volume_icon.y 	= 4;
        headbar_uipara.volume_icon.w 	= 50;
        headbar_uipara.volume_icon.h 	= 33;

        headbar_uipara.volume_txt.x 	= headbar_uipara.volume_icon.x + headbar_uipara.volume_icon.w + 1;
        headbar_uipara.volume_txt.y 	= 0;
        headbar_uipara.volume_txt.w 	= 32;
        headbar_uipara.volume_txt.h 	= 33;

        headbar_uipara.bright_icon.x 	= 1014;
        headbar_uipara.bright_icon.y 	= 2;
        headbar_uipara.bright_icon.w 	= 36;
        headbar_uipara.bright_icon.h 	= 36;

        headbar_uipara.bright_txt.x 	= headbar_uipara.bright_icon.x + headbar_uipara.bright_icon.w + 1;
        headbar_uipara.bright_txt.y 	= 0;
        headbar_uipara.bright_txt.w 	= 32;
        headbar_uipara.bright_txt.h 	= 33;

        headbar_uipara.battery.x 		= 1165;
        headbar_uipara.battery.y 		= 6;
        headbar_uipara.battery.w 		= 62;
        headbar_uipara.battery.h 		= 28;

        headbar_uipara.res_id.vol_id0	= 0;
        headbar_uipara.res_id.vol_id1	= 0;
        headbar_uipara.res_id.vol_id2	= 0;
        headbar_uipara.res_id.vol_id3	= 0;
        headbar_uipara.res_id.vol_id4	= 0;
        headbar_uipara.res_id.vol_id5 	= 0;

        headbar_uipara.res_id.music_id 	= 0;
        headbar_uipara.res_id.bright_id	= 0;
        headbar_uipara.res_id.volume_id	= 0;

        headbar_uipara.font 				= FONT16;
        break;
    }

    case SCREEN_TYPE_1920_1080:
    {
        /* hbar layer */
        headbar_uipara.hbar_lyr.fb_w  = 1920;
        headbar_uipara.hbar_lyr.fb_h  = 40;

        headbar_uipara.hbar_lyr.src_x = 0;
        headbar_uipara.hbar_lyr.src_y = 0;
        headbar_uipara.hbar_lyr.src_w = 1920;
        headbar_uipara.hbar_lyr.src_h = 40;

        headbar_uipara.hbar_lyr.scn_x = 0;
        headbar_uipara.hbar_lyr.scn_y = 48;
        headbar_uipara.hbar_lyr.scn_w = 1920;
        headbar_uipara.hbar_lyr.scn_h = 40;

        headbar_uipara.hbar_lyr.pipe  = 1;
        //
        headbar_uipara.hbar_frmwin.x = 0;
        headbar_uipara.hbar_frmwin.y = 0;
        headbar_uipara.hbar_frmwin.w = 1920;
        headbar_uipara.hbar_frmwin.h = 40;

        headbar_uipara.title.x	= 170;
        headbar_uipara.title.y	= 2;
        headbar_uipara.title.w	= 190;
        headbar_uipara.title.h	= 33;

        headbar_uipara.music.x = 416;
        headbar_uipara.music.y = 6;
        headbar_uipara.music.w = 29;
        headbar_uipara.music.h = 30;

        headbar_uipara.date.x = 656;
        headbar_uipara.date.y = 2;
        headbar_uipara.date.w = 100;
        headbar_uipara.date.h = 30;

        headbar_uipara.time.x = 960;
        headbar_uipara.time.y = 2;
        headbar_uipara.time.w = 100;
        headbar_uipara.time.h = 33;

        headbar_uipara.volume_icon.x 	= 1296;
        headbar_uipara.volume_icon.y 	= 4;
        headbar_uipara.volume_icon.w 	= 50;
        headbar_uipara.volume_icon.h 	= 33;

        headbar_uipara.volume_txt.x 	= headbar_uipara.volume_icon.x + headbar_uipara.volume_icon.w + 1;
        headbar_uipara.volume_txt.y 	= 0;
        headbar_uipara.volume_txt.w 	= 32;
        headbar_uipara.volume_txt.h 	= 33;

        headbar_uipara.bright_icon.x 	= 1521;
        headbar_uipara.bright_icon.y 	= 2;
        headbar_uipara.bright_icon.w 	= 36;
        headbar_uipara.bright_icon.h 	= 36;

        headbar_uipara.bright_txt.x 	= headbar_uipara.bright_icon.x + headbar_uipara.bright_icon.w + 1;
        headbar_uipara.bright_txt.y 	= 0;
        headbar_uipara.bright_txt.w 	= 32;
        headbar_uipara.bright_txt.h 	= 33;

        headbar_uipara.battery.x 		= 1747;
        headbar_uipara.battery.y 		= 6;
        headbar_uipara.battery.w 		= 62;
        headbar_uipara.battery.h 		= 28;

        if( format == HBAR_FOARMAT_8BPP )
        {
            headbar_uipara.res_id.vol_id0	= 0;
            headbar_uipara.res_id.vol_id1	= 0;
            headbar_uipara.res_id.vol_id2	= 0;
            headbar_uipara.res_id.vol_id3	= 0;
            headbar_uipara.res_id.vol_id4	= 0;
            headbar_uipara.res_id.vol_id5 	= 0;

            headbar_uipara.res_id.music_id 	= 0;
            headbar_uipara.res_id.bright_id	= 0;
            headbar_uipara.res_id.volume_id	= 0;

            shbar_palette_adjust();
        }
        else
        {
            headbar_uipara.res_id.vol_id0	= 0;
            headbar_uipara.res_id.vol_id1	= 0;
            headbar_uipara.res_id.vol_id2	= 0;
            headbar_uipara.res_id.vol_id3	= 0;
            headbar_uipara.res_id.vol_id4	= 0;
            headbar_uipara.res_id.vol_id5 	= 0;

            headbar_uipara.res_id.music_id 	= 0;
            headbar_uipara.res_id.bright_id	= 0;
            headbar_uipara.res_id.volume_id	= 0;
        }

        headbar_uipara.font 				= FONT16;
        break;
    }

    default:
        __err(" unknown screen type \n");
        break;
    }
}




