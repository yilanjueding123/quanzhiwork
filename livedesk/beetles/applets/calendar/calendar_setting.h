/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: calendar_setting.h
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/05/13 16:32  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#ifndef __CALENDAR_SETTING_H__FF741D68_0BBC_4EC7_A895_0FA3F3B1AAC2__
#define __CALENDAR_SETTING_H__FF741D68_0BBC_4EC7_A895_0FA3F3B1AAC2__

//资源
typedef struct tag_calendar_setting_res
{
    load_bmp_res_t bmp_bg;
} calendar_setting_wnd_res_t;

enum
{
    CLD_SET_LINE_DATE = 0,
    CLD_SET_LINE_FORMAT,
    CLD_SET_LINE_TIME,
    CLD_SET_LINE_MAX,
};
#define SET_OP_PARA_INVALIDATION       10000

//用户操作时，ui的相关数据
typedef struct tag_calendar_set_op_ui
{
    __s32    line;       //选中行的序号
    __s32    pos;        //选中行的焦点序号
} calendar_set_op_ui_t;

typedef __s8 TIME_FORAMT_T;
typedef struct tag_format_time_t
{
    __time_t      tm;  //用于带时制的时间, 当format为0时, 为24时制[00:00 - 23:59],
    //当format为非0时, 为12时制[12:00am - 11:59am, 12:00pm - 11:59pm],

    TIME_FORAMT_T fmt; //时制(注意:字节对齐问题, __time_t 为24字节, 故设计为__s8)

} FORMAT_TIME_T;       //(如 __time_t大小 改变, 为保持本结构体4字节对齐, 可加保留字节或改变fmt大小)

#define Is24HourFormat(_format)        ((_format) == 0)
#define Set24HourFormat(_format)       ((_format) =  0)
#define Is12HourFormat(_format)        ((_format) &  0x3)
#define Set12HourFormat(_format)       ((_format) =  0x3)//此宏不单独使用，后续代码应该可使_format值为正确的am，pm值
#define SetFormatAm(_format)           ((_format) =  0x1)
#define SetFormatPm(_format)           ((_format) =  0x2)
#define IsFormatPm(_format)            ((_format) == 0x2)

// #define CSTR_AM                         "AM"
// #define CSTR_PM                         "PM"


//设置用的数据
typedef struct tag_calendar_setting_data
{
    __date_t                date;
    FORMAT_TIME_T           f_time;
    calendar_set_op_ui_t    op_ui;

    //以下为设置前备份的数据，和新设置的数据比较，如没变化则不设置到系统
    __date_t                old_date;
    FORMAT_TIME_T           old_time;
} calendar_setting_data_t;

//窗口数据
typedef struct tag_calendar_setting_para
{
    H_LYR                      layer;
    calendar_setting_wnd_res_t res;
    calendar_setting_data_t    data;
} calendar_setting_wnd_para_t;

//////////////////////////////////////////////////////////////////////////

//RTC时间转带时制格式时间
/*使用方式示例:
{
	__time_t rtc;
	FORMAT_TIME_T f_time;

	esTIME_GetTime(&rtc);
	f_time->fmt = get_time_format();//取系统时制,返回0为24时制,返回1为12时制
	//Set12HourFormat(f_time->fmt)  //手动设置为12时制
	//Set24HourFormat(f_time->fmt); //手动设置为24时制
	rtc_time_to_format_time(&rtc, &f_time); //此调用会使f_time->fmt为正确的am,pm值
}
{//另一种使用方式
	FORMAT_TIME_T f_time;
	esTIME_GetTime(&f_time.tm);
	rtc_time_to_format_time(&f_time.tm, f_time);//此次调用实现了f_time的内部转换(24时制转12时制)
}
*/
extern void rtc_time_to_format_time(__time_t *rtc, FORMAT_TIME_T *f_time);

//带时制格式时间转RTC时间
/*注意：
{
	//FORMAT_TIME_T f_time;
	format_time_to_rtc_time(&f_time, &f_time.tm);//此次调用实现了f_time的内部转换，可能会修改f_time.fmt
}
*/
extern void format_time_to_rtc_time(FORMAT_TIME_T *f_time, __time_t *rtc);

//创建时间设置窗口
extern H_WIN calendar_setting_frmwin_create(H_WIN h_parent, H_WIN layer);

#endif //__CALENDAR_SETTING_H__FF741D68_0BBC_4EC7_A895_0FA3F3B1AAC2__
//End of this file

