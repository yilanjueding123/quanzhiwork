/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: calendar_date.h
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/05/12 20:12  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#ifndef __CALENDAR_DATE_H__209AE875_3B45_4852_9395_EA9B17D7FB9A__
#define __CALENDAR_DATE_H__209AE875_3B45_4852_9395_EA9B17D7FB9A__

//窗口资源
typedef struct tag_calendar_date_res
{
    load_bmp_res_t bmp_bg;
} calendar_date_wnd_res_t;


//窗口数据
typedef struct tag_calendar_date_para
{
    H_WIN                  layer;
    calendar_date_wnd_res_t res;
} calendar_date_wnd_para_t;


//////////////////////////////////////////////////////////////////////////
H_WIN calendar_date_frmwin_create(H_WIN h_parent, H_LYR layer);

#endif //__CALENDAR_DATE_H__209AE875_3B45_4852_9395_EA9B17D7FB9A__
//End of this file

