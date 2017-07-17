/*
************************************************************************************************************************
*                                                          lcd
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*											       All Rights Reserved
*
* File Name   : dsk_display_lcd.h
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
#ifndef  __DSK_DISPLAY_LCD_H__
#define  __DSK_DISPLAY_LCD_H__

#ifdef __cplusplus
extern "C" {
#endif


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
extern __s32 dsk_display_lcd_on(void);

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
extern __s32 dsk_display_lcd_off(void);

/*
****************************************************************************************************
*Name        : dsk_display_set_lcd_bright
*Prototype   : __s32 dsk_display_set_lcd_bright(__dsk_bright_t level)
*Arguments   : level       input. bright level.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : set the bright level of the LCD.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_display_set_lcd_bright(__lion_bright_t level);

/*
****************************************************************************************************
*Name        : dsk_display_get_lcd_bright
*Prototype   : __dsk_bright_t dsk_display_get_lcd_bright( void )
*Arguments   : void
*Return      : the bright level of the LCD.
*Description : get the bright level of the LCD.
*Other       :
****************************************************************************************************
*/
extern __lion_bright_t dsk_display_get_lcd_bright( void );



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_DISPLAY_LCD_H__

/* end of dsk_display_lcd.h */
