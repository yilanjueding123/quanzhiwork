/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: dsk_tmain.h
* By      	: Andy.zhang
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __DSK_MAIN_H__
#define __DSK_MAIN_H__


/* desktop 和 touchmain 之间公用数据结构 */
typedef enum tag_desktop_scene
{
    DESKTOP_SCENE_CMD_SET_HBAR_LYRSTATE,	// headbar
    DESKTOP_SCENE_CMD_GET_HBAR_LYRSTATE,
    DESKTOP_SCENE_CMD_SET_HBAR_FORMAT,
    DESKTOP_SCENE_CMD_GET_HBAR_SCN_POS,
    DESKTOP_SCENE_CMD_SET_HBAR_SCN_POS,
    DESKTOP_SCENE_CMD_SET_TITLE,
    DESKTOP_SCENE_CMD_SET_VOLUME,
    DESKTOP_SCENE_CMD_SET_BRIGHT,
    DESKTOP_SCENE_CMD_
} dsk_scmd_t;


#endif /* __DSK_MAIN_H__ */

