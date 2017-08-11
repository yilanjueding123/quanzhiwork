/*
************************************************************************************************************************
*                                                        CUCKOO
*                                                   the Audio Player
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   :ResDecode.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2008.11.08
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2008.11.08       1.1.0        build the file
*
*/

#ifndef __COM_PALETTE_H_
#define __COM_PALETTE_H_

#include "beetles_app.h"
#define PALETTE_TEXT_COLOUR_INDEX_(_i)  (0xC0 + (_i) * 0x10)
#define APP_COLOR_WHITE                 PALETTE_TEXT_COLOUR_INDEX_(0)
#define APP_COLOR_YELLOW                PALETTE_TEXT_COLOUR_INDEX_(1)
#define APP_COLOR_BLACK                 PALETTE_TEXT_COLOUR_INDEX_(2)
#define APP_COLOR_GREEN                 PALETTE_TEXT_COLOUR_INDEX_(3)
#define APP_COLOR_DARKGRAY				PALETTE_TEXT_COLOUR_INDEX_(4)
#define APP_COLOR_X1					PALETTE_TEXT_COLOUR_INDEX_(5)
#define APP_COLOR_X2					PALETTE_TEXT_COLOUR_INDEX_(6)
#define APP_COLOR_X3					PALETTE_TEXT_COLOUR_INDEX_(7)
#define APP_COLOR_X4					PALETTE_TEXT_COLOUR_INDEX_(8)
#define APP_COLOR_X5					PALETTE_TEXT_COLOUR_INDEX_(9)

__s32  com_set_palette_text_color(__u32 *palette, __u32 text_color_index,
                                  __u32 gui_color);

__s32 com_set_palette(void *palette_buf);

__s32 com_set_palette_by_id(__s32 ID);

__s32 com_set_palette_color_by_index( __u32 palette_index, const __u32 *buf, __u32 nr );

//´´½¨Í¼²ã, rect is screen region
H_LYR com_layer_create(RECT *rect, __s32 pipe, __pixel_rgbfmt_t rgb_fmt, __gui_lyrwin_sta_t initsta, char *lyr_name);
//Ïú»ÙÍ¼²ã
#define com_layer_destroy(_layer)   if (_layer){GUI_LyrWinDelete(_layer); _layer = NULL;}

/////////////////////////DEBUG_APP_MEM////////////////////////////////////
#ifdef DEBUG_APP_MEM

#define app_com_layer_create(_layer, _rect, _pipe, _rgb_fmt, _initsta, lyr_name)  do \
{ \
	_layer = com_layer_create(_rect, _pipe, _rgb_fmt, _initsta, lyr_name); \
	AddDebugAPPMEM(_layer, __LINE__, _T(__FILE__)); \
} while (0)

#define app_com_layer_destroy(_layer)   if (_layer){SubDebugAPPMEM(_layer); GUI_LyrWinDelete(_layer); _layer = NULL;}

#else//no debug

#define app_com_layer_create(_layer, _rect, _pipe, _rgb_fmt, _initsta, lyr_name)  do \
{ \
	_layer = com_layer_create(_rect, _pipe, _rgb_fmt, _initsta, lyr_name); \
} while (0)

#define app_com_layer_destroy(_layer)   com_layer_destroy(_layer)

#endif //DEBUG_APP_MEM

#define app_com_layer_create_default(_layer, _prt, _name)  \
	app_com_layer_create(_layer, _prt, 1, PIXEL_MONO_8BPP, GUI_LYRWIN_STA_SUSPEND, _name)
//////////////////////////////////////////////////////////////////////////

#endif//__COM_PALETTE_H_
