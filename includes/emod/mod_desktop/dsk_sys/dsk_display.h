/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: dsk_display.h
* By      	: Andy.zhang
* Func		: desk display control
* Version	: v1.0
* ============================================================================================================
* 2009-7-8 10:02:17  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __DSK_DISPLAY_H__
#define __DSK_DISPLAY_H__


typedef enum tag_DSK_OUTPUT_TYPE
{
    DSK_OUTPUT_LCD,
    DSK_OUTPUT_TV,
    DSK_OUTPUT_HDMI,
    DSK_OUTPUT_VGA,
    DSK_OUTPUT_
} __dsk_output_type_t;

typedef enum tag_DSK_DISP_TVFORMAT
{
    DSK_DISP_TVFORMAT_NTSC = 0,
    DSK_DISP_TVFORMAT_480I,
    DSK_DISP_TVFORMAT_480P,
    DSK_DISP_TVFORMAT_720P,
    DSK_DISP_TVFORMAT_HDMI,
    DSK_DISP_TVFORMAT_XGA,
    DSK_DISP_TVFORMAT_
} __dsk_disp_tvformat_t;

typedef enum tag_DSK_BRIGHT
{
    DSK_BRIGHT_LEVEL1 = 0,
    DSK_BRIGHT_LEVEL2,
    DSK_BRIGHT_LEVEL3,
    DSK_BRIGHT_LEVEL4,
    DSK_BRIGHT_LEVEL5,
    DSK_BRIGHT_LEVEL6,
    DSK_BRIGHT_LEVEL7,
    DSK_BRIGHT_LEVEL8,
    DSK_BRIGHT_LEVEL9,
    DSK_BRIGHT_LEVEL10,
    DSK_BRIGHT_LEVEL11,
    DSK_BRIGHT_LEVEL12,
    DSK_BRIGHT_LEVEL13,
    DSK_BRIGHT_LEVEL14,
    DSK_BRIGHT_LEVEL15,
    DSK_BRIGHT_LEVEL16,
    DSK_BRIGHT_ = -1
} __dsk_bright_t;


// 1. 调节屏幕背光亮度
__s32 			dsk_display_set_lcd_bright(__dsk_bright_t level);
__dsk_bright_t 	dsk_display_get_lcd_bright(void);

// 2. 输出类型
__dsk_output_type_t dsk_display_get_output_type(void);

// 3. 输出分辨率
__s32 dsk_display_get_size(__s32 *p_width, __s32 *p_height);

// 4. 输出切换

// 本机/电视机 切换
//__s32 dsk_display_lcd_switch_tv_format(__dsk_disp_tvformat_t mode);
//__s32 dsk_display_tv_format_switch_lcd(void);
// 电视机输出格式切换
//__s32 dsk_display_tv_switch_format(__dsk_disp_tvformat_t mode);

// 5. 设置背景色
__s32 dsk_display_set_backcolor(__s32 color);
__s32 dsk_display_get_backcolor(void);

// 6.设置自动关屏时间 单位 : 1ms
__s32 dsk_display_set_close_scn_time(__u32 time);

// 禁止/ 使能自动关屏
__s32 dsk_display_enable_close_scn(void);
__s32 dsk_display_disable_close_scn(void);

__s32 dsk_display_switch_device(void);
#endif /* __DSK_DISPLAY_H__ */








