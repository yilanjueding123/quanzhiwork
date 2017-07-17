/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: dsk_keytone.h
* By      	: Andy.zhang
* Func		: desk key tone control
* Version	: v1.0
* ============================================================================================================
* 2009-7-8 10:02:17  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __DSK_KEYTONE_H__
#define __DSK_KEYTONE_H__


typedef enum tag_SET_KEYTON
{
    SET_KEYTONE_ON = 1,			// 打开按键音
    SET_KEYTONE_OFF,			// 关闭按键音
} __set_keytone_t;


__s32 dsk_keytone_on(void);			//按键音响

__s32 dsk_keytone_set_state(__set_keytone_t state);		// 禁止/打开 按键音
__s32 dsk_keytone_get_state(__set_keytone_t *p_state);	// 查询是否允许按键音

#endif /* __DSK_KEYTONE_H__ */

