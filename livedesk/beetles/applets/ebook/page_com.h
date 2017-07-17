/*
************************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: page_com.h
* By      	: Andy.zhang
* Func		: character show scene
* Version	: v1.0
* ======================================================================================================================
* 2009-11-11 15:43:43  andy.zhang  create this file, implements the fundemental interface;
************************************************************************************************************************
*/

#ifndef __PAGE_COM_H__
#define __PAGE_COM_H__

#include "app_ebook_i.h"

typedef	enum tag_PAGE_NC_MSG
{
    COM_NC_PAGE_NEXT,
    COM_NC_PAGE_PREV,
    COM_NC_PAGE_JUMP,

    COM_NC_PAGE_EXIT,
    COM_NC_SET,

} __page_nc_msg_e;


typedef	enum tag_PAGE_CMD_MSG
{
    COM_CMD_UPDATE_PAGE	= GUI_MSG_,
    COM_CMD_SET_PLAY_STATUS,
    CLOSE_LONG_STRING,
    COM_CMD_STOP_LONG_STRING,
    OPEN_LONG_STRING,
    COM_CMD_START_LONG_STRING,
} __page_cmd_msg_e;

typedef	struct tag_page_create_para
{
    H_WIN						page_layer;			/*场景的父窗口		*/
    H_WIN						volume_bar_layer;			/*场景的父窗口		*/
    GUI_FONT 					*font;

    char			name[128];
    __s32		play_status;
    __s32		total_page;
    __s32		cur_page;
    __s32		volume;
} __page_create_para_t;

typedef enum
{
    CMD_PAGE_SET_VOLUME,
    CMD_PAGE_SET_PREV,
    CMD_PAGE_SET_NEXT,
    CMD_PAGE_SET_PLAY,
    CMD_PAGE_SET_SET,
    CMD_PAGE_SET_CLOSE,
} tag_page_cmd_t;


H_WIN page_frmwin_create(H_WIN hwin, __page_create_para_t *para);
__s32 page_frmwin_delete(H_WIN h_win);


#endif /* __PAGE_COM_H__ */

