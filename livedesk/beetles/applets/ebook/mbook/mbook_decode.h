/*
**************************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           FM app sample
*
*						        	(c) Copyright 2006-2007, ANDY, China
*											 All Rights Reserved
*
* File    : mbook_decode.h
* By      : lyn
* Version : V1.00
* time    : 2009-8-14
**************************************************************************************************************
*/
#ifndef __MBOOK_DECODE_H__
#define __MBOOK_DECODE_H__

#define     DECODE_MBOOK_NUM     3

typedef     void   *H_DECODE;

typedef struct tag_DECODE_CONFIG
{
    GUI_FONT  *char_font;
    __u32       font_size;              // 字体大小
    __u32       border_width;           // 两边留白宽度
    __u32       bottom_width;           // 底边留白宽度
    __u32	      scroll_width;			//滑动条的宽度
    __u32       show_width;             // 显示的宽度
    __u32       show_height;            // 显示的高度
    __u32       row_space;              // 行间距
    __u32       col_space;              // 列间距
    __u32       font_color;             // 字体的颜色
    __u32       back_color;             // 背景的颜色
    __u32       show_rotate;            // 横竖屏显示
} __decode_config_t;


H_DECODE   MBOOK_Decode_Init(char *filename, __u8 *err);

__s32   MBOOK_Decode_Config(H_DECODE hdle, __decode_config_t *config);

__s32   MBOOK_Decode_ShowPage(H_DECODE hdle, __u32 offset);

__s32   MBOOK_Decode_ShowNext(H_DECODE hdle);

__s32   MBOOK_Decode_ShowPrev(H_DECODE hdle);

__s32   MBOOK_Decode_SeekPage(H_DECODE hdle, __u32 page_no);

__s32   MBOOK_Decode_SeekOffset(H_DECODE hdle, __u32 offset);

__s32   MBOOK_Decode_MovePage(H_DECODE hdle, __s32 x, __s32 y);

__s32   MBOOK_Decode_RotatePage(H_DECODE hdle);

__s32   MBOOK_Decode_GetRotate(H_DECODE hdle);

__s32   MBOOK_Decode_GetTotalPage(H_DECODE hdle);

__s32   MBOOK_Decode_GetCurrentPage(H_DECODE hdle);

__s32   MBOOK_Decode_GetPageOffset(H_DECODE hdle, __u32 page_no);

__s32   MBOOK_Decode_GetFileName(H_DECODE hdle, char *name);

__s64   MBOOK_Decode_GetFileSize(H_DECODE hdle);

__s32   MBOOK_Decode_GetFilePath(H_DECODE hdle, char *path);

__s32   MBOOK_Decode_GetFileContext(H_DECODE hdle, char *context);

__s32   MBOOK_Decode_SetRowSpace(H_DECODE hdle, __u32 space);

__s32   MBOOK_Decode_SetColSpace(H_DECODE hdle, __u32 space);

__s32   MBOOK_Decode_SetBorderWidth(H_DECODE hdle, __u32 width);

__s32   MBOOK_Decode_SetBackColor(H_DECODE hdle, __u32 color);

__s32   MBOOK_Decode_SetFontColor(H_DECODE hdle, __u32 color);

__s32   MBOOK_Decode_SetFontSize(H_DECODE hdle, __u32 size);

__s32   MBOOK_Decode_SetDefaultCharset(H_DECODE hdle, __epdk_charset_enm_e charset);

__s32   MBOOK_Decode_Uninit(H_DECODE hdle);


#endif /* __MBOOK_DECODE_H__ */

