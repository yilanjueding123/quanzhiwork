/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: live_init.h
* By      	: Andy.zhang
* Func		: Live main res init
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __LIVE_INIT_H__
#define __LIVE_INIT_H__


#define THEM_BIN_PATH	BEETLES_APP_ROOT"apps\\theme.bin"

extern GUI_FONT	*SWFFont;
extern GUI_FONT *ScnLockFont;
extern __s32 LCD_WIDTH;
extern __s32 LCD_HEIGHT;

__s32 live_init(void);
__s32 live_exit(void);

void  init_screen_size(void);

#endif /* __LIVE_INIT_H__ */


