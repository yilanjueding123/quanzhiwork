/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: app_calendar_i.h
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/05/12 14:27  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#ifndef __APP_CALENDAR_I_H__BD3E1064_942F_4A13_82DB_F93E5B4196FD__
#define __APP_CALENDAR_I_H__BD3E1064_942F_4A13_82DB_F93E5B4196FD__

#include "beetles_app.h"
//#include "../Lib/res/res_decode.h"
#include "calendar_uipara.h"
#include "calendar_time.h"
#include "calendar_date.h"
#include "calendar_setting.h"
#include "china_cld.h"

#define OUT_OF_RANGE(_d, _min, _max)   ((_d) < (_min) || (_max) < (_d))

#define IsLeapYear(_y)                 (((_y) % 100) ? (((_y) % 4) ? 0 : 1) : (((_y) % 400) ? 0 : 1))
#define IsLargeMonth(_m)               ((_m) == 1 || (_m) == 3 || (_m) == 5 || (_m) == 7 || (_m) == 8 || (_m) == 10 || (_m) == 12)
#define get_february_days(_y)          (((_y) % 100) ? (((_y) % 4) ? 28 : 29) : (((_y) % 400) ? 28 : 29))//(28 + IsLeapYear(year))

#define DEFAULT_SECOND                 0
#define DEFAULT_MINUTE                 0
#define DEFAULT_HOUR                   0
#define DEFAULT_DAY                    1
#define DEFAULT_MONTH                  1
#define DEFAULT_YEAR                   2011

#define MAX_YEAR                       2099
#define MIN_YEAR                       1900

#define IsInvalidSecond(_s)            OUT_OF_RANGE(_s, 0, 59)
#define IsInvalidMinute(_m)            OUT_OF_RANGE(_m, 0, 59)
#define IsInvalidHour(_h)              OUT_OF_RANGE(_h, 0, 23)
#define IsInvalidYear(_y)              OUT_OF_RANGE(_y, MIN_YEAR, MAX_YEAR)
#define IsInvalidMonth(_m)             OUT_OF_RANGE(_m, 1, 12)
#define IsInvalidDay(_y, _m, _d)       (((_m) == 2) ? \
	(IsLeapYear(_y) ? OUT_OF_RANGE(_d, 1, 29) : OUT_OF_RANGE(_d, 1, 28)) \
	: (IsLargeMonth(_m) ? OUT_OF_RANGE(_d, 1, 31) : OUT_OF_RANGE(_d, 1, 30)))

#define compare_time(t1, t2)    ((t1).second != (t2).second || (t1).minute != (t2).minute || (t1).hour != (t2).hour)
#define compare_date(d1, d2)    ((d1).day != (d2).day || (d1).month != (d2).month || (d1).year != (d2).year)
#define set_time_(t1, t2)       { (t1).second = (t2).second; (t1).minute = (t2).minute; (t1).hour = (t2).hour; }
#define set_date_(d1, d2)       { (d1).day = (d2).day; (d1).month = (d2).month; (d1).year = (d2).year; }

//////////////////////////////////////////////////////////////////////////
//adapter

//////////////////////////////////////////////////////////////////////////

//TODO:to move to beetles_app.h

#define CALENDAR_TIME_WND_ID           (APP_CALENDAR_ID         + 1)
#define CALENDAR_SETTINT_WND_ID        (CALENDAR_TIME_WND_ID    + 1)
#define CALENDAR_DATE_WND_ID           (CALENDAR_SETTINT_WND_ID + 1)

//自定义消息
#define MSG_CALENDAR_TIME_UPDATE       (GUI_MSG_USER_DEF + 1)	// 时间更新
#define MSG_CALENDAR_DATE_UPDATE       (GUI_MSG_USER_DEF + 2)	// 日期更新
#define MSG_CALENDAR_SETTING_UPDATE    (GUI_MSG_USER_DEF + 3)	// 设置数据取现在的时间/日期

//命令消息ID
enum
{
    CLD_CMD_START_ID = 0x0100,
    CLD_CMD_SWITCH_TIME_WND,
    CLD_CMD_SWITCH_DATE_WND,
    CLD_CMD_SETTING_POP_WND,
    CLD_CMD_HIDE_SETTING_WND,
    CLD_CMD_CLOSE_WND,
    CLD_CMD_END_ID
};
//#define CLD_CMD_SWITCH_LAYER_ON         0x0100
//////////////////////////////////////////////////////////////////////////

//主窗口参数
typedef struct tag_calendar_data
{
    H_WIN h_time_frm;
    H_WIN h_date_frm;
    H_WIN h_setting_frm;

    H_LYR lyr_time;
    H_LYR lyr_date;
    H_WIN lyr_setting;

    __s32 update_thread_id;

    __u32 root_type;
    GUI_FONT *font;

} calendar_data_t;

// typedef struct tag_calendar_date_time
// {
// 	__date_t date;
// 	__time_t time;
// }calendar_date_time_t;

//发送命令到主窗口
__s32  CLD_SendCommand(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);

#endif //__APP_CALENDAR_I_H__BD3E1064_942F_4A13_82DB_F93E5B4196FD__
//End of this file

