/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: live_init.c
* By      	: Andy.zhang
* Func		: Live main res init
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "mod_init_i.h"
#include "live_init.h"

#define LANG_DIR			BEETLES_APP_ROOT"apps\\lang.bin"
#define THEME_DIR			BEETLES_APP_ROOT"apps\\theme.bin"

/**************************************************************************************************************/
GUI_FONT    *SWFFont;		// 主界面字体

__s32 		LCD_WIDTH;
__s32 		LCD_HEIGHT;

/**************************************************************************************************************/

/**************************************************************************************************************/

/**
 * 初始化 系统资源
 */
__s32 live_init(void)
{
    __msg("~~~~~~~~~~~~~~file system char set~~~~~~~~~~~\n");
    dsk_set_fs_charset(EPDK_CHARSET_ENM_UTF8);
    __here__;
    dsk_set_isn_charset(EPDK_CHARSET_ENM_GBK);
    __here__;
    //create font
    SWFFont 	= GUI_SFT_CreateFont(16, BEETLES_APP_ROOT"res\\fonts\\font16.sft" );
    if(!SWFFont)
    {
        __msg("GUI_SFT_CreateFont fail, set default font....\n");
        SWFFont = GUI_GetDefaultFont();
    }
    __here__;
#ifndef  BOOT_FROM_SDMMC	   //卡量产	
    //initialize walkman/keytone
    dsk_wkm_init(DSK_WKM_MODE_AUDIO_MIN);
    dsk_keytone_init(BEETLES_APP_ROOT"res\\sounds\\chord.wav");
#endif
    //initialize system settings
    //dsk_gamma_on();
    //dsk_display_enhance_on();
#ifndef  BOOT_FROM_SDMMC	   //卡量产	

    //init reg
    {
        __s32 ret;

        ret = dsk_reg_init_para();
        if(EPDK_FAIL == ret)
        {
            __msg("dsk_reg_init_para fail...\n");
        }
    }
#endif
    __here__;
    //initialize lang/theme
    dsk_langres_init(LANG_DIR);
    dsk_theme_init(THEME_DIR);
    //dsk_orchid_load_last_npl();
    __here__;
#ifndef  BOOT_FROM_SDMMC	   //卡量产
    {
        reg_system_para_t *para;
        para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
        if(para)
        {
            //dsk_set_gamma(lion_reg_gamma_get());
            dsk_display_set_lcd_bright(para->backlight);
            __msg("para->backlight=%d\n", para->backlight);

            dsk_volume_set(para->volume);
            __msg("para->volume=%d\n", para->volume);

            g_set_close_scn_time(para->backlightoff);
            __msg("para->backlightoff=%d\n", para->backlightoff);

            dsk_set_auto_off_time(para->poweroff);
            __msg("para->poweroff=%d\n", para->poweroff);

            dsk_langres_set_type(para->language);
            __msg("para->language=0x%x\n", para->language);
            para->output = LION_DISP_LCD;

			dsk_set_contrast(para->contrast);
			dsk_set_bright(para->bright);
			dsk_set_saturation(para->saturation);
			dsk_set_hue(para->hue);

            if(1 == para->keytone)
            {
                dsk_keytone_set_state(SET_KEYTONE_ON);
            }
            else
            {
                dsk_keytone_set_state(SET_KEYTONE_OFF);
            }

            if(1 == para->time_set)
            {
                esTIME_SetTime(&para->time);
                esTIME_SetDate(&para->date);
                para->time_set = 0;
            }
        }
        else
        {
            dsk_display_set_lcd_bright(LION_BRIGHT_LEVEL12);
            dsk_volume_set(20);
            dsk_set_auto_off_time(0);
            g_set_close_scn_time(0);
            dsk_langres_set_type(EPDK_LANGUAGE_ENM_CHINESES);
            dsk_keytone_set_state(SET_KEYTONE_OFF);
        }

        __here__;
        //dsk_reg_flush();
        __here__;
    }
#endif
    return EPDK_OK;
}


/**
 * 释放 系统资源
 */
__s32 live_exit(void)
{
    /* 释放字体 */
    GUI_SetFont(GUI_GetDefaultFont());
    if(SWFFont && (GUI_GetDefaultFont() != SWFFont))
    {
        GUI_SFT_ReleaseFont(SWFFont);
        SWFFont = NULL;
    }

    //save system settings
    dsk_reg_flush();
    dsk_reg_deinit_para();

    //exit walkman/keytone
    dsk_keytone_exit();
    dsk_wkm_exit();
    //dsk_orchid_save_npl();

    //释放语言图片资源
    dsk_theme_exit();
    dsk_langres_exit();

    return EPDK_OK;
}

void init_screen_size(void)
{
    dsk_display_get_size(&LCD_WIDTH, &LCD_HEIGHT);
}


