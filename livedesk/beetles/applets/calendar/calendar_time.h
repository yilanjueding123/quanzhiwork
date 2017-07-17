/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: calendar_time.h
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/05/12 14:27  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#ifndef __CALENDAR_TIME_H__4B680D80_CC31_4013_A52D_C80DE3F3A3C2__
#define __CALENDAR_TIME_H__4B680D80_CC31_4013_A52D_C80DE3F3A3C2__
//////////////////////////////////////////////////////////////////////////
#include "calendar_adapter.h"

typedef struct tag_calendar_timewnd_res
{
    load_bmp_res_t bmp_bg;
    load_bmp_res_t bmp_num[10];
    load_bmp_res_t bmp_am;
    load_bmp_res_t bmp_pm;
    load_bmp_res_t bmp_colon;
} calendar_time_wnd_res_t;

typedef struct tag_calendar_timewnd_para
{
    H_LYR                  layer;
    calendar_time_wnd_res_t res;
} calendar_time_wnd_para_t;


//////////////////////////////////////////////////////////////////////////
H_WIN  calendar_time_frmwin_create(H_WIN h_parent, H_WIN layer);

#endif //__CALENDAR_TIME_H__4B680D80_CC31_4013_A52D_C80DE3F3A3C2__
//End of this file

