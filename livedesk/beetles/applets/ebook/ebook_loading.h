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

#ifndef __EBOOK_LOADING_H_
#define  __EBOOK_LOADING_H_

#include "beetles_app.h"

typedef	struct tag_ebook_loading_para
{
    H_LYR		loading_layer;
    GUI_FONT 	*font;

} ebook_loading_para_t;

H_WIN ebook_loading_win_create(H_WIN h_parent, ebook_loading_para_t *para);
void  ebook_loading_win_delete(H_WIN loading_win);

#endif
