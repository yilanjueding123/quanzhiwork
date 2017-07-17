/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: msgbox.h
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __MSGBOX_H_
#define  __MSGBOX_H_

#include "app_home_i.h"

#define CMD_MSGBOX_CLOSE 		0

typedef struct tag_main_msgbox_para
{
    GUI_FONT *msgbox_font;
    H_LYR	  layer;
} main_msgbox_para_t;

H_WIN msgbox_win_create(H_WIN h_parent, main_msgbox_para_t *para);

#endif
