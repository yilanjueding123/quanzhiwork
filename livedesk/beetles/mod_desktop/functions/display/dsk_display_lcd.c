/*
************************************************************************************************************************
*                                                          lcd
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*											       All Rights Reserved
*
* File Name   : dsk_display_lcd.c
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.09.02
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2010.09.02        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_DISPLAY_LCD_C__
#define  __DSK_DISPLAY_LCD_C__


#include "dsk_display_i.h"


/*
****************************************************************************************************
*Name        : dsk_display_lcd_on
*Prototype   : __s32 dsk_display_lcd_on(void)
*Arguments   : void
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : open the LCD display.
*Other       :
****************************************************************************************************
*/
__s32 dsk_display_lcd_on(void)
{
    __disp_output_type_t  	output;
    ES_FILE      		   *p_disp = NULL;

    /* open display driver */
    p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");
    if( p_disp == NULL )
    {
        __err("Error in opening display driver.\n");
        goto error;
    }

    /* get output type and format */

    output = (__disp_output_type_t)eLIBs_fioctrl(p_disp, DISP_CMD_GET_OUTPUT_TYPE, 0, 0);

    if( output == DISP_OUTPUT_TYPE_NONE )
    {
        eLIBs_fioctrl(p_disp, DISP_CMD_LCD_ON, 0, 0);
    }

    /* close display driver */
    eLIBs_fclose(p_disp);
    p_disp = NULL;

    return EPDK_OK;

error:
    if( p_disp != NULL )
    {
        eLIBs_fclose(p_disp);
        p_disp = NULL;
    }
    return EPDK_FAIL;
}

/*
****************************************************************************************************
*Name        : dsk_display_lcd_off
*Prototype   : __s32 dsk_display_lcd_off(void)
*Arguments   : void
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : close the LCD display.
*Other       :
****************************************************************************************************
*/
__s32 dsk_display_lcd_off(void)
{
    __disp_output_type_t  	output;
    ES_FILE      		   *p_disp = NULL;

    /* open display driver */
    p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");
    if( p_disp == NULL )
    {
        __err("Error in opening display driver.\n");
        goto error;
    }

    /* get output type and format */

    output = (__disp_output_type_t)eLIBs_fioctrl(p_disp, DISP_CMD_GET_OUTPUT_TYPE, 0, 0);

    if( output == DISP_OUTPUT_TYPE_LCD )
    {
        eLIBs_fioctrl(p_disp, DISP_CMD_LCD_OFF, 0, 0);	// note: ’‚æ‰”–—” ±
    }

    /* close display driver */
    eLIBs_fclose(p_disp);
    p_disp = NULL;

    return EPDK_OK;

error:
    if( p_disp != NULL )
    {
        eLIBs_fclose(p_disp);
        p_disp = NULL;
    }
    return EPDK_FAIL;
}


/*
****************************************************************************************************
*Name        : dsk_display_set_lcd_bright
*Prototype   : __s32 dsk_display_set_lcd_bright(__LION_BRIGHT_t level)
*Arguments   : level       input. bright level.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : set the bright level of the LCD.
*Other       :
****************************************************************************************************
*/
__s32 dsk_display_set_lcd_bright(__lion_bright_t level)
{
    __disp_output_type_t  	output;
    ES_FILE      		   *p_disp = NULL;
    __disp_lcd_bright_t  	lcd_bright;
    __u32 arg[3];

    /* open display driver */
    p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");
    if( p_disp == NULL )
    {
        __err("Error in opening display driver.\n");
        goto error;
    }

    /* get output type and format */

    output = (__disp_output_type_t)eLIBs_fioctrl(p_disp, DISP_CMD_GET_OUTPUT_TYPE, 0, 0);

    if( output == DISP_OUTPUT_TYPE_LCD )
    {
        switch(level)
        {
        case LION_BRIGHT_LEVEL1:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL0;
            break;

        case LION_BRIGHT_LEVEL2:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL1;
            break;

        case LION_BRIGHT_LEVEL3:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL2;
            break;

        case LION_BRIGHT_LEVEL4:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL3;
            break;

        case LION_BRIGHT_LEVEL5:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL4;
            break;

        case LION_BRIGHT_LEVEL6:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL5;
            break;

        case LION_BRIGHT_LEVEL7:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL6;
            break;

        case LION_BRIGHT_LEVEL8:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL7;
            break;

        case LION_BRIGHT_LEVEL9:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL8;
            break;

        case LION_BRIGHT_LEVEL10:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL9;
            break;

        case LION_BRIGHT_LEVEL11:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL10;
            break;

        case LION_BRIGHT_LEVEL12:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL11;
            break;

        case LION_BRIGHT_LEVEL13:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL12;
            break;

        case LION_BRIGHT_LEVEL14:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL13;
            break;

        case LION_BRIGHT_LEVEL15:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL14;
            break;

        case LION_BRIGHT_LEVEL16:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL15;
            break;

        default:
            lcd_bright = DISP_LCD_BRIGHT_LEVEL11;
            break;
        }

        arg[0] = lcd_bright;
        arg[1] = 0;
        arg[2] = 0;
        eLIBs_fioctrl(p_disp, DISP_CMD_LCD_SET_BRIGHTNESS, 0, (void *)arg);

    }

    /* close display driver */
    eLIBs_fclose(p_disp);
    p_disp = NULL;

    return EPDK_OK;

error:
    if( p_disp != NULL )
    {
        eLIBs_fclose(p_disp);
        p_disp = NULL;
    }
    return EPDK_FAIL;
}


/*
****************************************************************************************************
*Name        : dsk_display_get_lcd_bright
*Prototype   : __LION_BRIGHT_t dsk_display_get_lcd_bright( void )
*Arguments   : void
*Return      : the bright level of the LCD.
*Description : get the bright level of the LCD.
*Other       :
****************************************************************************************************
*/
__lion_bright_t dsk_display_get_lcd_bright( void )
{
    __disp_output_type_t  	output;
    ES_FILE      		   *p_disp = NULL;
    __lion_bright_t			level;
    __disp_lcd_bright_t  	lcd_bright;

    /* open display driver */
    p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");
    if( p_disp == NULL )
    {
        __err("Error in opening display driver.\n");
        goto error;
    }

    /* get output type and format */
    level = LION_BRIGHT_LEVEL1;

    output = (__disp_output_type_t)eLIBs_fioctrl(p_disp, DISP_CMD_GET_OUTPUT_TYPE, 0, 0);

    if( output == DISP_OUTPUT_TYPE_LCD )
    {
        lcd_bright = (__disp_lcd_bright_t)eLIBs_fioctrl(p_disp, DISP_CMD_LCD_GET_BRIGHTNESS, 0, 0);
        switch(lcd_bright)
        {
        case DISP_LCD_BRIGHT_LEVEL0:
            level = LION_BRIGHT_LEVEL1;
            break;

        case DISP_LCD_BRIGHT_LEVEL1:
            level = LION_BRIGHT_LEVEL2;
            break;

        case DISP_LCD_BRIGHT_LEVEL2:
            level = LION_BRIGHT_LEVEL3;
            break;

        case DISP_LCD_BRIGHT_LEVEL3:
            level = LION_BRIGHT_LEVEL4;
            break;

        case DISP_LCD_BRIGHT_LEVEL4:
            level = LION_BRIGHT_LEVEL5;
            break;

        case DISP_LCD_BRIGHT_LEVEL5:
            level = LION_BRIGHT_LEVEL6;
            break;

        case DISP_LCD_BRIGHT_LEVEL6:
            level = LION_BRIGHT_LEVEL7;
            break;

        case DISP_LCD_BRIGHT_LEVEL7:
            level = LION_BRIGHT_LEVEL8;
            break;

        case DISP_LCD_BRIGHT_LEVEL8:
            level = LION_BRIGHT_LEVEL9;
            break;

        case DISP_LCD_BRIGHT_LEVEL9:
            level = LION_BRIGHT_LEVEL10;
            break;

        case DISP_LCD_BRIGHT_LEVEL10:
            level = LION_BRIGHT_LEVEL11;
            break;

        case DISP_LCD_BRIGHT_LEVEL11:
            level = LION_BRIGHT_LEVEL12;
            break;

        case DISP_LCD_BRIGHT_LEVEL12:
            level = LION_BRIGHT_LEVEL13;
            break;

        case DISP_LCD_BRIGHT_LEVEL13:
            level = LION_BRIGHT_LEVEL14;
            break;

        case DISP_LCD_BRIGHT_LEVEL14:
            level = LION_BRIGHT_LEVEL15;
            break;

        case DISP_LCD_BRIGHT_LEVEL15:
            level = LION_BRIGHT_LEVEL16;
            break;

        default:
            level = LION_BRIGHT_;
            break;
        }

    }

    /* close display driver */
    eLIBs_fclose(p_disp);
    p_disp = NULL;

    return level;

error:
    if( p_disp != NULL )
    {
        eLIBs_fclose(p_disp);
        p_disp = NULL;
    }
    return LION_BRIGHT_;
}




#endif     //  ifndef __DSK_DISPLAY_LCD_C__

/* end of dsk_display_lcd.c */
