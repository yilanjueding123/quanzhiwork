/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: calendar_setting.c
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/05/13 16:33  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#include "beetles_app.h"
#include "calendar_setting.h"
#include "calendar_adapter.h"
#include "app_calendar_i.h"
//////////////////////////////////////////////////////////////////////////

//RTC时间转带时制格式时间
void rtc_time_to_format_time(__time_t *rtc, FORMAT_TIME_T *f_time)
{
    if (&f_time->tm == rtc)//函数用于f_time的内部转换
    {
        Set12HourFormat(f_time->fmt);
    }
    if (Is24HourFormat(f_time->fmt))
    {
        f_time->tm.hour = rtc->hour;
    }
    else
    {
        __s32 tmp = rtc->hour;

        if (hour24_to_hour12(&tmp) == 0)
        {
            SetFormatAm(f_time->fmt);
        }
        else
        {
            SetFormatPm(f_time->fmt);
        }

        f_time->tm.hour = (TIME_FORAMT_T)tmp;
    }

    f_time->tm.minute = rtc->minute;
    f_time->tm.second = rtc->second;
}

//带时制格式时间转RTC时间
void format_time_to_rtc_time(FORMAT_TIME_T *f_time, __time_t *rtc)
{
    rtc->minute = f_time->tm.minute;
    rtc->second = f_time->tm.second;

    if (Is24HourFormat(f_time->fmt))
    {
        rtc->hour = f_time->tm.hour;
    }
    else
    {
        if (IsFormatPm(f_time->fmt))
        {
            if (f_time->tm.hour == 12)//中午12点
            {
                rtc->hour = 12;
            }
            else
            {
                rtc->hour = f_time->tm.hour + 12;
            }
        }
        else
        {
            if (f_time->tm.hour == 12)//凌晨12点
            {
                rtc->hour = 0;
            }
            else
            {
                rtc->hour = f_time->tm.hour;
            }
        }
        if (&f_time->tm == rtc)//函数用于f_time的内部转换
        {
            Set24HourFormat(f_time->fmt);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//加载资源
//////////////////////////////////////////////////////////////////////////
static __s32 calendar_setting_wnd_create_res(calendar_setting_wnd_res_t *res)
{
    create_bmp_res(ID_CALENDAR_SETTIME_BG_BMP, res->bmp_bg);

    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//释放资源
//////////////////////////////////////////////////////////////////////////
static __s32 calendar_setting_wnd_destroy_res(calendar_setting_wnd_res_t *res)
{
    destroy_bmp_res(res->bmp_bg);

    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//更新年份
// para --- 加减的数字( +1 / -1 )
// pos  --- 个位为0, 十位为1, 百位为2，千位为3
//////////////////////////////////////////////////////////////////////////
static void update_year(__date_t *date, __s32 para, __s32 pos)
{
    __s32 year;

    year = date->year % 100;

    if (pos == 2 || pos == 3)
    {
        if (date->year / 100 >= 20)
        {
            date->year = (MIN_YEAR / 100 * 100) + year;
        }
        else
        {
            date->year = (MAX_YEAR / 100 * 100) + year;
        }
    }
    else if (pos == 1)
    {
        date->year += (para * 10);
        if (date->year < MIN_YEAR)
        {
            date->year = (MAX_YEAR / 10 * 10) + date->year % 10;
        }
        else if (date->year > MAX_YEAR)
        {
            date->year = (MIN_YEAR / 10 * 10) + date->year % 10;
        }
    }
    else
    {
        date->year += para;
        if (date->year < MIN_YEAR)
        {
            date->year = MAX_YEAR;
        }
        else if (date->year > MAX_YEAR)
        {
            date->year = MIN_YEAR;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//更新月份
// para --- 加减的数字( +1 / -1 )
// pos  --- 个位为0, 十位为1
//////////////////////////////////////////////////////////////////////////
static void update_month(__date_t *date, __s32 para, __s32 pos)
{
    __u8   temp;

    if (pos == 1)
    {
        //月份的十位改变
        if ((date->month / 10) == 1) //[10, 11, 12]
        {
            //由1变为0
            if (date->month != 10)//(date->month == 11 || date->month == 12)
            {
                date->month -= 10;
            }
            else
            {
                date->month = 1;
            }
        }
        else//由0变为1
        {
            if ((date->month % 10) <= 2)
            {
                date->month += 10;
            }
            else
            {
                date->month = 10;
            }
        }
    }
    else
    {
        //月份个位改变
        temp = date->month;
        temp += para;
        if (12 < temp)
        {
            date->month = 1;
        }
        else if (temp < 1)
        {
            date->month = 12;
        }
        else
        {
            date->month = temp;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//更新天数,
// para --- 加减的数字( +1 / -1 )
// pos  --- 个位为0, 十位为1
//////////////////////////////////////////////////////////////////////////
static void update_day(__date_t *date, __s32 para, __s32 pos)
{
    __u8   temp;
    __s32  tempday;
    __s32  monthday;
    __s32  month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (date->month == 2)
    {
        monthday = get_february_days(date->year);
    }
    else
    {
        monthday = month[date->month - 1];
    }

    tempday = date->day;

    if (pos == 0)
    {
        // +1, -1
        tempday += para;

        if (tempday < 1)
        {
            date->day = monthday;
        }
        else if (tempday > monthday)
        {
            date->day = 1;
        }
        else
        {
            date->day = tempday;
        }
    }
    else
    {
        // +10, -10
        tempday += (para * 10);

        if (tempday > monthday)
        {
            temp = date->day % 10;

            if (temp == 0)
            {
                date->day = 10;
            }
            else
            {
                date->day = temp;
            }
        }
        else if (tempday < 1)
        {
            if ((monthday == 28 && (date->day == 9 || date->day == 10)) //28天的特殊处理
                    || (monthday == 29 && date->day == 10)) //29天的特殊处理
            {
                date->day += 10;
            }
            else if (monthday == 31 && date->day == 1) //31天的特殊处理
            {
                date->day = 31;
            }
            else
            {
                date->day += 20;
            }
        }
        else
        {
            date->day = (__u8)tempday;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//更新小时,
// para --- 加减的数字( +1 / -1 )
// pos  --- 个位为0, 十位为1
//////////////////////////////////////////////////////////////////////////
static void update_hour(__u8 *hour, __s32 para, __s32 pos, TIME_FORAMT_T format)
{
    __s32 temp = *hour;
    if (pos == 0)
    {
        temp += para;
        if (Is24HourFormat(format))
        {
            if (temp < 0)
            {
                *hour = 23;
            }
            else if (temp >= 24)
            {
                *hour = 0;
            }
            else
            {
                *hour = (__u8)temp;
            }
        }
        else
        {
            if (temp < 1)
            {
                *hour = 12;
            }
            else if (temp > 12)
            {
                *hour = 1;
            }
            else
            {
                *hour = (__u8)temp;
            }
        }
    }
    else
    {
        temp += (para * 10);
        if (Is24HourFormat(format))
        {
            if (temp < 0)
            {
                if (*hour % 10 < 4)
                {
                    *hour += 20;
                }
                else
                {
                    *hour += 10;
                }
            }
            else if (temp >= 24)
            {
                *hour %= 10;
            }
            else
            {
                *hour = (__u8)temp;
            }
        }
        else
        {
            if (temp == 0)// *hour is 10
            {
                *hour = 1;
            }
            else if (temp < 1)
            {
                if (*hour % 10 <= 2)//*hour is 1 or 2
                {
                    *hour += 10;
                }
                else //*hour is 3...9
                {
                    *hour = 10;
                }
            }
            else if (temp > 12)
            {
                if (*hour == 10)
                {
                    *hour = 1;
                }
                else if (*hour == 12 || *hour == 11)
                {
                    *hour -= 10;
                }
                else
                {
                    *hour = 10;
                }
            }
            else
            {
                *hour = (__u8)temp;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//更新minute,
// para --- 加减的数字( +1 / -1 )
// pos  --- 个位为0, 十位为1
//////////////////////////////////////////////////////////////////////////
static void update_minute(__u8 *minute, __s32 para, __s32 pos)
{
    __s32 temp = *minute;

    if (pos == 0)
    {
        temp += para;
        if (temp < 0)
        {
            *minute = 59;
        }
        else if (temp > 59)
        {
            *minute = 0;
        }
        else
        {
            *minute = (__u8)temp;
        }
    }
    else
    {
        temp += (para * 10);

        if (temp < 0)
        {
            *minute += 50;
        }
        else if (temp > 59)
        {
            *minute -= 50;
        }
        else
        {
            *minute = (__u8)temp;
        }
    }
}


//初始化数据
static __s32 setting_data_get_now(calendar_setting_data_t *set_data)
{
    esTIME_GetTime(&set_data->f_time.tm);
    esTIME_GetDate(&set_data->date);

    set_data->f_time.tm.second = 0;//TODO:check if need to set second

    set_data->f_time.fmt = (TIME_FORAMT_T)get_time_format();

    if (Is24HourFormat(set_data->f_time.fmt))
    {
    }
    else
    {
        rtc_time_to_format_time(&set_data->f_time.tm, &set_data->f_time);
    }

    set_data->op_ui.line = 0;
    set_data->op_ui.pos  = 0;

    eLIBs_memcpy(&set_data->old_time, &set_data->f_time, sizeof(set_data->f_time));
    eLIBs_memcpy(&set_data->old_date, &set_data->date, sizeof(set_data->date));

    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//更新设置数据
// para ---  -1表示左按键操作，+1表示右按键操作
//           SET_OP_PARA_INVALIDATION 用于取系统时间
//////////////////////////////////////////////////////////////////////////
static __s32 update_setting_data(calendar_setting_data_t *set_data, __s32 para)
{
    ASSERT(set_data);
    ASSERT(0 <= set_data->op_ui.line && set_data->op_ui.line < CLD_SET_LINE_MAX);
    ASSERT(0 <= set_data->op_ui.pos);

    switch (set_data->op_ui.line)
    {
    case CLD_SET_LINE_DATE:
    {
        ASSERT(set_data->op_ui.pos < 8);

        if (set_data->op_ui.pos <= 3)//年份
        {
            update_year(&set_data->date, para, 3 - set_data->op_ui.pos);
        }
        else if (set_data->op_ui.pos <= 5)//月份
        {
            update_month(&set_data->date, para, 5 - set_data->op_ui.pos);
        }
        else //if (set_data->op_ui.pos <= 7)//天
        {
            update_day(&set_data->date, para, 7 - set_data->op_ui.pos);
        }
    }
    break;
    case CLD_SET_LINE_FORMAT:
        if (Is24HourFormat(set_data->f_time.fmt))
        {
            //Set12HourFormat(set_data->f_time.fmt);
            rtc_time_to_format_time(&set_data->f_time.tm, &set_data->f_time);
        }
        else
        {
            format_time_to_rtc_time(&set_data->f_time, &set_data->f_time.tm);
        }
        break;
    case CLD_SET_LINE_TIME:
    {
        ASSERT((set_data->op_ui.pos < 4 && Is24HourFormat(set_data->f_time.fmt)) || (set_data->op_ui.pos < 5));

        if (set_data->op_ui.pos < 2)//小时
        {
            update_hour(&set_data->f_time.tm.hour, para,
                        1 - set_data->op_ui.pos, set_data->f_time.fmt);
        }
        else if (set_data->op_ui.pos < 4)//分
        {
            update_minute(&set_data->f_time.tm.minute, para, 3 - set_data->op_ui.pos);
        }
        else
        {
            //update_am_pm
            ASSERT(Is12HourFormat(set_data->f_time.fmt));
            if (IsFormatPm(set_data->f_time.fmt))
            {
                SetFormatAm(set_data->f_time.fmt);
            }
            else
            {
                SetFormatPm(set_data->f_time.fmt);
            }
        }
    }
    break;
    }

    return EPDK_OK;
}
//////////////////////////////////////////////////////////////////////////
//设置数据到系统
//////////////////////////////////////////////////////////////////////////
static __s32 set_data_to_system(calendar_setting_data_t *set_data)
{
    __s32 ret = 0;
    if (set_data->f_time.fmt != set_data->old_time.fmt)
    {
        set_time_format(set_data->f_time.fmt);
        ret |= 0x1;
    }

    if (eLIBs_memcmp(&set_data->f_time, &set_data->old_time, sizeof(FORMAT_TIME_T)) != EPDK_YES)
    {
        __time_t time;

        format_time_to_rtc_time(&set_data->f_time, &time);
        time.second = 0;
        esTIME_SetTime(&time);

        ret |= 0x2;
    }

    if (eLIBs_memcmp(&set_data->date, &set_data->old_date, sizeof(__date_t)) != EPDK_YES)
    {
        esTIME_SetDate(&set_data->date);

        ret |= 0x4;
    }

    DEBUG_CLD("set_data_to_system() = %x\n", ret);
    DEBUG_CLD("old(%d-%d-%d), new(%d-%d-%d)\n",
              set_data->old_date.year, set_data->old_date.month, set_data->old_date.day,
              set_data->date.year, set_data->date.month, set_data->date.day);
    DEBUG_CLD("old(%d:%d:%d)[%d], new(%d:%d:%d)[%d]\n",
              set_data->old_time.tm.hour, set_data->old_time.tm.minute, set_data->old_time.tm.second, set_data->old_time.fmt,
              set_data->f_time.tm.hour, set_data->f_time.tm.minute, set_data->f_time.tm.second, set_data->f_time.fmt);

    return ret;
}

//////////////////////////////////////////////////////////////////////////
//更新设置数据
//////////////////////////////////////////////////////////////////////////
static __s32 update_setting_data_op_ui(calendar_setting_data_t *set_data/*, __s32 para*/)
{
    if (set_data->op_ui.line == CLD_SET_LINE_DATE)
    {
        set_data->op_ui.pos++;
        if (set_data->op_ui.pos >= 8)
        {
            set_data->op_ui.line = CLD_SET_LINE_FORMAT;
            set_data->op_ui.pos = 0;
        }
    }
    else if (set_data->op_ui.line == CLD_SET_LINE_FORMAT)
    {
        set_data->op_ui.line = CLD_SET_LINE_TIME;
        set_data->op_ui.pos = 0;
    }
    else if (set_data->op_ui.line == CLD_SET_LINE_TIME)
    {
        set_data->op_ui.pos++;
        if (((Is24HourFormat(set_data->f_time.fmt) && (set_data->op_ui.pos >= 4)))
                || (set_data->op_ui.pos >= 5))
        {
            set_data->op_ui.line = CLD_SET_LINE_DATE;
            set_data->op_ui.pos = 0;
        }
    }
    else
    {
        ASSERT(0);
    }
    return EPDK_OK;
}

//窗口按键处理
static __s32 calendar_setting_wnd_key_proc(__gui_msg_t *msg)
{
    //////////////////////////////////////////////////////////////////////////
    //TODO:to check
    if (msg->dwAddData2 != KEY_UP_ACTION)
    {
        DEBUG_CLD("key do nothing\n");
        return EPDK_OK;
    }
    //////////////////////////////////////////////////////////////////////////

    switch (msg->dwAddData1)
    {
    case VK_CLD_SETTING_SUB:
    {
        calendar_setting_wnd_para_t *wnd_para = (calendar_setting_wnd_para_t *)GUI_WinGetAttr(msg->h_deswin);
        update_setting_data(&wnd_para->data, -1);
        GUI_WinUpdate(msg->h_deswin, EPDK_TRUE);
    }
    break;
    case VK_CLD_SETTING_ADD:
    {
        calendar_setting_wnd_para_t *wnd_para = (calendar_setting_wnd_para_t *)GUI_WinGetAttr(msg->h_deswin);
        update_setting_data(&wnd_para->data, 1);
        GUI_WinUpdate(msg->h_deswin, EPDK_TRUE);
    }
    break;
    case VK_CLD_SETTING_LOOPMENU:
    {
        calendar_setting_wnd_para_t *wnd_para = (calendar_setting_wnd_para_t *)GUI_WinGetAttr(msg->h_deswin);
        update_setting_data_op_ui(&wnd_para->data);
        GUI_WinUpdate(msg->h_deswin, EPDK_TRUE);
    }
    break;
    case VK_CLD_SETTING_ENTER:
    {
        calendar_setting_wnd_para_t *wnd_para = (calendar_setting_wnd_para_t *)GUI_WinGetAttr(msg->h_deswin);
        set_data_to_system(&wnd_para->data);
    }
    //		break;
    case VK_ESCAPE:
    case VK_CLD_POP_SETTING:
        return CLD_SendCommand(msg->h_deswin, CLD_CMD_HIDE_SETTING_WND, 0, msg->h_deswin);
    default:
        break;
    }
    return EPDK_OK;
}

static __s32 on_calendar_setting_wnd_command(__gui_msg_t *msg)
{
    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//draw window
//////////////////////////////////////////////////////////////////////////
static __s32 draw_setting_focus(H_WIN hwnd, calendar_setting_data_t *data,
                                char *str, CLD_UI_SETTING_PARA_T *ui)
{
    __s32 x = 1, y = 1, w = 0;
    GUI_RECT rt  = {0, 0, 0, 0};
    char *p = str;

    switch (data->op_ui.line)
    {
    case CLD_SET_LINE_DATE:
    {
        // 			GUI_GetTextExtend(&rt, p, eLIBs_strlen(p));
        // 			w = rt.x1 - rt.x0;
        if (data->op_ui.pos < 4)
        {
            p += data->op_ui.pos;
        }
        else if (data->op_ui.pos < 6)
        {
            p += data->op_ui.pos + 3;
        }
        else//if(data->op_ui.pos < 8)
        {
            p += data->op_ui.pos + (3 + 3);
        }
        w = GUI_GetCharDistX((__u16)(*p)) - 1;
        GUI_GetTextExtend(&rt, p, eLIBs_strlen(p));
        x = (ui->date_tip.x + ui->size.line.width) - (rt.x1 - rt.x0);
        y = ui->date_tip.y;
    }
    break;
    case CLD_SET_LINE_FORMAT:
    {
        GUI_GetTextExtend(&rt, p, eLIBs_strlen(p));
        w = (rt.x1 - rt.x0);
        x = ui->format_tip.x + ui->size.line.width - w;
        y = ui->format_tip.y;
    }
    break;
    case CLD_SET_LINE_TIME:
    {
        if (data->op_ui.pos < 2)
        {
            p += data->op_ui.pos;
        }
        else if (data->op_ui.pos < 4)
        {
            p += data->op_ui.pos + 3;
        }
        else//if(data->op_ui.pos < 5)
        {
            ASSERT(Is12HourFormat(data->f_time.fmt));
            p += data->op_ui.pos + (3 + 2);
        }
        GUI_GetTextExtend(&rt, p, eLIBs_strlen(p));
        x = (ui->time_tip.x + ui->size.line.width) - (rt.x1 - rt.x0);
        y = ui->time_tip.y;
        if (data->op_ui.pos < 4)
        {
            w = GUI_GetCharDistX((__u16)(*p)) - 1;
        }
        else
        {
            w = rt.x1 - rt.x0;
        }
    }
    break;
    default:
        ASSERT(0);
    }

    GUI_InvertRect(x - 1, y, x + w, y + GUI_GetFontSizeY());

    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//draw window
//////////////////////////////////////////////////////////////////////////
static __s32 draw_setting(H_WIN hwnd, calendar_setting_data_t *data,
                          calendar_setting_wnd_res_t *res, CLD_UI_SETTING_PARA_T *ui)
{
    GUI_RECT rt;
    char str[256] = {0};
    char str2[32] = {0};

    //擦背景
    GUI_BMP_RES_Draw(res->bmp_bg, 0, 0);

    GUI_UC_SetEncodeUTF8();

    //日期
    if (data->op_ui.line == CLD_SET_LINE_DATE)
    {
        GUI_SetColor(ui->colour.focus_line);
    }
    else
    {
        GUI_SetColor(ui->colour.txt);
    }
    get_lang_res(STRING_CALENDAR_DATE, str);
    GUI_DispStringAt(str, ui->date_tip.x, ui->date_tip.y);

    app_sprintf(str, "%04d . %02d . %02d", data->date.year, data->date.month, data->date.day);
    rt.x0 = ui->date_tip.x;
    rt.y0 = ui->date_tip.y;
    rt.x1 = rt.x0 + ui->size.line.width;
    rt.y1 = rt.y0 + ui->size.line.height;
    GUI_DispStringInRect(str, &rt, GUI_TA_RIGHT);
    if (data->op_ui.line == CLD_SET_LINE_DATE)
    {
        draw_setting_focus(hwnd, data, str, ui);
    }

    //时制
    if (data->op_ui.line == CLD_SET_LINE_FORMAT)
    {
        GUI_SetColor(ui->colour.focus_line);
    }
    else
    {
        GUI_SetColor(ui->colour.txt);
    }
    get_lang_res(STRING_CALENDAR_MODE, str);
    GUI_DispStringAt(str, ui->format_tip.x, ui->format_tip.y);
    if (Is24HourFormat(data->f_time.fmt))
    {
        get_lang_res(STRING_CALENDAR_24HOUR, str);
    }
    else
    {
        get_lang_res(STRING_CALENDAR_12HOUR, str);
    }
    rt.x0 = ui->format_tip.x;
    rt.y0 = ui->format_tip.y;
    rt.x1 = rt.x0 + ui->size.line.width;
    rt.y1 = rt.y0 + ui->size.line.height;
    GUI_DispStringInRect(str, &rt, GUI_TA_RIGHT);
    if (data->op_ui.line == CLD_SET_LINE_FORMAT)
    {
        draw_setting_focus(hwnd, data, str, ui);
    }

    //时间
    if (data->op_ui.line == CLD_SET_LINE_TIME)
    {
        GUI_SetColor(ui->colour.focus_line);
    }
    else
    {
        GUI_SetColor(ui->colour.txt);
    }
    get_lang_res(STRING_CALENDAR_TIME, str);
    GUI_DispStringAt(str, ui->time_tip.x, ui->time_tip.y);

    if (Is24HourFormat(data->f_time.fmt))
    {
        app_sprintf(str, "%02d : %02d", data->f_time.tm.hour, data->f_time.tm.minute);
    }
    else
    {
        if (IsFormatPm(data->f_time.fmt))
        {
            get_lang_res(STRING_CLD_PM, str2);
        }
        else
        {
            get_lang_res(STRING_CLD_AM, str2);
        }
        app_sprintf(str, "%02d : %02d  %s", data->f_time.tm.hour, data->f_time.tm.minute, str2);
    }
    rt.x0 = ui->time_tip.x;
    rt.y0 = ui->time_tip.y;
    rt.x1 = rt.x0 + ui->size.line.width;
    rt.y1 = rt.y0 + ui->size.line.height;
    GUI_DispStringInRect(str, &rt, GUI_TA_RIGHT);
    if (data->op_ui.line == CLD_SET_LINE_TIME)
    {
        draw_setting_focus(hwnd, data, str, ui);
    }


    //GUI_GetTextExtend
    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//窗口 paint
//////////////////////////////////////////////////////////////////////////
static __s32 on_calendar_setting_wnd_paint(__gui_msg_t *msg)
{
    calendar_setting_wnd_para_t *wnd_para;
    CLD_UI_SETTING_PARA_T       *ui_setting;
    GUI_MEMDEV_Handle	      draw_mem;
    H_LYR layer;

    layer = GUI_WinGetLyrWin(msg->h_deswin);
    if (GUI_LyrWinGetSta(layer) != GUI_LYRWIN_STA_ON)
    {
        return EPDK_FAIL;
    }

    wnd_para = (calendar_setting_wnd_para_t *)GUI_WinGetAttr(msg->h_deswin);

    ui_setting = &(get_calendar_uipara()->ui_setting);

    GUI_LyrWinSel(layer);
    GUI_SetFont(SWFFont);

    draw_mem = GUI_MEMDEV_Create(0, 0, ui_setting->rt.width, ui_setting->rt.height);
    GUI_MEMDEV_Select(draw_mem);

    draw_setting(msg->h_deswin, &wnd_para->data, &wnd_para->res, ui_setting);

    GUI_MEMDEV_CopyToLCD_Ex(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;

    return EPDK_OK;
}

//Timer处理
static __s32 on_calendar_setting_wnd_timer(__gui_msg_t *msg)
{
    return EPDK_OK;
}

//创建窗口处理
static __s32 on_calendar_setting_wnd_create(__gui_msg_t *msg)
{
    calendar_setting_wnd_para_t *wnd_para = (calendar_setting_wnd_para_t *)GUI_WinGetAttr(msg->h_deswin);

    calendar_setting_wnd_create_res(&wnd_para->res);


    setting_data_get_now(&wnd_para->data);

    _app_show_focus_wnd(msg->h_deswin, wnd_para->layer);

    return EPDK_OK;
}

//销毁窗口处理
static __s32 on_calendar_setting_wnd_destroy(__gui_msg_t *msg)
{
    calendar_setting_wnd_para_t *wnd_para = GUI_WinGetAttr(msg->h_deswin);

    calendar_setting_wnd_destroy_res(&wnd_para->res);

    BFreeType(wnd_para, calendar_setting_wnd_para_t);
    return EPDK_OK;
}

//窗口回调
static __s32 calendar_setting_wnd_Proc(__gui_msg_t *msg)
{
    DEBUG_CBMSGEx(calendar_setting_wnd_Proc);
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
        on_calendar_setting_wnd_create(msg);
        return EPDK_OK;
    case GUI_MSG_CLOSE:
        GUI_FrmWinDelete(msg->h_deswin);
        return EPDK_OK;
    case GUI_MSG_DESTROY:
        on_calendar_setting_wnd_destroy(msg);
        return EPDK_OK;
    case GUI_MSG_PAINT:
        on_calendar_setting_wnd_paint(msg);
        return EPDK_OK;
    case MSG_CALENDAR_SETTING_UPDATE:
    {
        calendar_setting_wnd_para_t *wnd_para = (calendar_setting_wnd_para_t *)GUI_WinGetAttr(msg->h_deswin);
        ASSERT(wnd_para);
        setting_data_get_now(&wnd_para->data);
    }
    return EPDK_OK;
    case GUI_MSG_KEY:
        calendar_setting_wnd_key_proc(msg);
        break;
    case GUI_MSG_TOUCH:
        break;
    case GUI_MSG_TIMER:
        on_calendar_setting_wnd_timer(msg);
        return EPDK_OK;
    case GUI_MSG_COMMAND:
        on_calendar_setting_wnd_command(msg);
        return EPDK_OK;
    case GUI_MSG_WIN_WAKEUP:
        GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
        return EPDK_OK;
    default:
        break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

//创建窗口
H_WIN calendar_setting_frmwin_create(H_WIN h_parent, H_WIN layer)
{
    __gui_framewincreate_para_t framewin_para;
    calendar_setting_wnd_para_t *wnd_para;
    FB fb;

    BallocType(wnd_para, calendar_setting_wnd_para_t);
    ASSERT(wnd_para != NULL);
    ZeroTypeMem(wnd_para, calendar_setting_wnd_para_t);

    wnd_para->layer = layer;

    GUI_LyrWinGetFB(wnd_para->layer, &fb);

    ZeroTypeMem(&framewin_para, __gui_framewincreate_para_t);

    framewin_para.name          = CALENDAR_TIME_WND_NAME;
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = CALENDAR_SETTINT_WND_ID;
    framewin_para.hHosting      = h_parent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)calendar_setting_wnd_Proc);
    framewin_para.rect.x        = 0;
    framewin_para.rect.y        = 0;
    framewin_para.rect.width    = fb.size.width;
    framewin_para.rect.height   = fb.size.height;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr          = (void *)wnd_para;
    framewin_para.hLayer        = wnd_para->layer;

    return (GUI_FrmWinCreate(&framewin_para));
}

//窗口销毁
__s32 calendar_setting_frmwin_delete(H_WIN hwnd)
{
    __s32 ret;
    ret = GUI_FrmWinDelete(hwnd);
    return ret;
}



