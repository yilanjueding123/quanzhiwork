/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           home app sample
*
*						        	(c) Copyright 2006-2009, LYN, China
*											 All Rights Reserved
*
* File    : cvr_show.h
* By      : lyn
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __CVR_SHOW_H__
#define __CVR_SHOW_H__

#include "..\\dv_i.h"

typedef enum tag_SHOW_MODE
{
    SHOW_WINDOW_BESTSHOW = 0x00,        /* 以图片本身的比例缩放至满窗口显示，图片不变形 */
    SHOW_WINDOW_ORIGINAL,               /* 以图片原始大小在窗口内显示，不能溢出窗口     */
    SHOW_WINDOW_FULLSCN,                /* 以窗口的比例缩放图片至满窗口显示，可能会变形 */
    SHOW_WINDOW_
} show_mode_e;

__s32   Cvr_ShowOpen(void);
__s32   Cvr_ShowClose(void);
__s32   Cvr_ShowStatus(__s32 enable);
__s32   Cvr_ShowSetData(void *buf, SIZE buf_size);
__s32   Cvr_ShowSetMode(show_mode_e mode);
__s32   Cvr_ShowGetSize(SIZE *size);

#endif  /* __CVR_SHOW_H__ */

