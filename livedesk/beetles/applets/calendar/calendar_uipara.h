/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: calendar_uipara.h
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/05/13 14:32  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#ifndef __CALENDAR_UIPARA_H__1C41C96B_8695_4CC3_AA1E_4B651F6A0725__
#define __CALENDAR_UIPARA_H__1C41C96B_8695_4CC3_AA1E_4B651F6A0725__

#include "beetles_app.h"

//////////////////////////////////////////////////////////////////////////
//数字时钟的图片资源(宽高)大小
typedef struct tag_CLD_UI_SIZE_TIME
{
    __rectsz_t num;     //数字图片大小
    __rectsz_t colon;   //冒号图片大小
    __rectsz_t format;  //am, pm图片大小
} CLD_UI_SIZE_TIME_T;

//数字时钟ui参数
typedef struct tag_CLD_UI_TIME_PARA
{
    RECT               rt;            //ui界面区域(图层区域)
    CLD_UI_SIZE_TIME_T bmp_size;      //图片资源(宽高)大小
    __pos_t            pos_first_num; //数字时钟第一位数字的坐标
    __pos_t            pos_format;    //am, pm图片的坐标
} CLD_UI_TIME_PARA_T;

//////////////////////////////////////////////////////////////////////////
//日期ui的(宽高)大小参数
typedef struct tag_CLD_UI_SIZE_DATE
{
    __rectsz_t week_sp;   //星期间的间隔
    __rectsz_t date_sp;   //日期间的间隔
    __rectsz_t chs_dt_sp; //公历 / 农历 间的间隔
    __rectsz_t date;      //日期块(含公历 / 农历)的大小
    //	__rectsz_t reserve;   //prompt;
} CLD_UI_SIZE_DATE_T;

//日期ui的颜色参数
typedef struct tag_CLD_UI_COLOUR_DATE
{
    GUI_COLOR  txt;       //一般文字颜色
    GUI_COLOR  date_f;    //date focus colour
    //	GUI_COLOR  holiday;   //holiday colour
    //	GUI_COLOR  week;      //星期颜色
    //	GUI_COLOR  date_n;    //date normal colour
    //	GUI_COLOR  dateinfo;  //日期信息颜色
    //	GUI_COLOR  date_s;    //date select colour
} CLD_UI_COLOUR_DATE_T;

//日期ui参数
typedef struct tag_CLD_UI_DATE_PARA
{
    RECT                 rt;          //ui界面区域(图层区域)
    CLD_UI_COLOUR_DATE_T colour;      //颜色参数
    CLD_UI_SIZE_DATE_T   size;        //日期ui的(宽高)大小参数
    __pos_t              week_first;  //星期开始的坐标
    __pos_t              date_first;  //日期开始的坐标
    __pos_t              prompt;      //日期信息的坐标
} CLD_UI_DATE_PARA_T;


typedef struct tag_CLD_UI_COLOUR_SETTING
{
    GUI_COLOR  txt;                      //一般文字颜色
    GUI_COLOR  focus_line;               //focus line text colour
    //	GUI_COLOR  focus;                    //focus colour
} CLD_UI_COLOUR_SETTING_T;

typedef struct tag_CLD_UI_SIZE_SETTING
{
    __rectsz_t    line;                  //一行文字宽高
} CLD_UI_SIZE_SETTING_T;

//设置界面参数
typedef struct tag_CLD_UI_SETTING_PARA
{
    RECT                    rt;          //ui界面区域(图层区域)
    CLD_UI_COLOUR_SETTING_T colour;      //颜色参数
    CLD_UI_SIZE_SETTING_T   size;        //日期ui的(宽高)大小参数
    __pos_t                 date_tip;    //"日期"的坐标
    __pos_t                 format_tip;  //"模式"的坐标
    __pos_t                 time_tip;    //"时间"的坐标
} CLD_UI_SETTING_PARA_T;

//////////////////////////////////////////////////////////////////////////
//calendar ui para
typedef struct tag_calendar_uipara
{
    //	RECT                    rt_lcd;         //LCD区域
    CLD_UI_TIME_PARA_T      ui_time;        //数字时钟ui参数
    CLD_UI_DATE_PARA_T      ui_date;        //日期ui参数
    CLD_UI_SETTING_PARA_T   ui_setting;     //设置界面
} calendar_uipara_t;


//获取ui数据
calendar_uipara_t *get_calendar_uipara(void);

#endif //__CALENDAR_UIPARA_H__1C41C96B_8695_4CC3_AA1E_4B651F6A0725__
//End of this file

