/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	:sub_menu.h
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __SUB_MENU_H_
#define __SUB_MENU_H_

#include "app_home_i.h"

#define MAX_SUB_ITEM		4

typedef enum
{
    MSG_SUB_OP_UP = GUI_MSG_USER_DEF,
    MSG_SUB_OP_DOWN,
    MSG_SUB_OP_ENTER,
    MSG_SUB_OP_LIST,
} sub_msg_t;

#define SMENU_APP_SWITCH	0

typedef struct tag_smenu_uipara_pos
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
    __s32 reserve;
} smenu_uipara_pos_t;

typedef struct tag_sub_menu
{
    H_WIN		layer;
    GUI_FONT 	*smenu_font;
    __s32			root_type;
    __s8	 		sub_menu_id;
    __s32           focus_item;
} smenu_para_t;

H_WIN sub_menu_win_create(H_WIN h_parent, smenu_para_t *para);

void sub_menu_init_res(H_WIN hwin);
void sub_menu_uninit_res(H_WIN hwin);

#endif
