/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: calendar_date.c
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/05/12 21:35  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#include "beetles_app.h"
#include "calendar_date.h"
#include "calendar_adapter.h"
#include "app_calendar_i.h"
//////////////////////////////////////////////////////////////////////////

//
const __u32 week_str_res_id[7] =
{
    STRING_CLD_SUNDAY, STRING_CLD_MONDAY, STRING_CLD_TUESDAY,
    STRING_CLD_WEDNESDAY, STRING_CLD_THURSDAY, STRING_CLD_FRIDAY,
    STRING_CLD_SATURDAY
};
const __u32 week_str_res_id_ex[7] =
{
    STRING_CALENDAR_SUNDAY, STRING_CALENDAR_MONDAY, STRING_CALENDAR_TUESDAY,
    STRING_CALENDAR_WEDNESDAY, STRING_CALENDAR_THURSDAY, STRING_CALENDAR_FRIDAY,
    STRING_CALENDAR_SATURDAY
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//加载资源
//////////////////////////////////////////////////////////////////////////
static __s32 calendar_date_wnd_create_res(calendar_date_wnd_res_t *res)
{
    create_bmp_res(ID_CALENDAR_TIME_BG_BMP, res->bmp_bg);

    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//释放资源
//////////////////////////////////////////////////////////////////////////
static __s32 calendar_date_wnd_destroy_res(calendar_date_wnd_res_t *res)
{
    destroy_bmp_res(res->bmp_bg);

    return EPDK_OK;
}

static __s32 switch_time_wnd(H_WIN hwnd)
{
    calendar_date_wnd_para_t *para = (calendar_date_wnd_para_t *)GUI_WinGetAttr(hwnd);
    app_hide_layer(para->layer);
    return CLD_SendCommand(hwnd, CLD_CMD_SWITCH_TIME_WND, hwnd, 0);
}

//////////////////////////////////////////////////////////////////////////
//窗口按键处理
//////////////////////////////////////////////////////////////////////////
static __s32 calendar_date_wnd_key_proc(__gui_msg_t *msg)
{
    //msg->dwAddData2 == KEY_REPEAT_ACTION
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
    case VK_CLD_SWITCH_TIME_DATE:
        switch_time_wnd(msg->h_deswin);
        break;
    case VK_ESCAPE:
        return CLD_SendCommand(msg->h_deswin, CLD_CMD_CLOSE_WND, 0, msg->h_deswin);
    case VK_CLD_POP_SETTING:
        return CLD_SendCommand(msg->h_deswin, CLD_CMD_SETTING_POP_WND, 0, msg->h_deswin);
    default:
        break;
    }
    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
static __s32 on_calendar_date_wnd_command(__gui_msg_t *msg)
{
    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//画日历
//////////////////////////////////////////////////////////////////////////
#define DRAW_LONG_TIP             0
static __s32 draw_date(H_WIN hwnd, __date_t *now, calendar_date_wnd_res_t *res, CLD_UI_DATE_PARA_T *ui)
{
    __s32 i;
    __s32 x, y, x_end;
    __s32 weekday;
    __u16 monthday[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; //monthday[0] is nullity
    char str[256] = {0};
#if DRAW_LONG_TIP
    char str_week[16] = {0};
    char str_chs_date_tip[8 * 4] = {0};//距离立春还有01天
#else
    char str_chs_date[10 * 4] = {0};//壬午(马)年腊月初一
    GUI_RECT rt;
#endif


    DEBUG_CLD(" draw date( %d - %d - %d )\n", now->year, now->month, now->day);

    GUI_SetColor(ui->colour.txt);
    GUI_SetFont(SWFFont);

    monthday[2] = get_february_days(now->year);
    weekday = get_week_day(now->year, now->month, 1);

    //draw bg
    x = 0;
    y = 0;
    GUI_BMP_RES_Draw(res->bmp_bg, x, y);

    //draw week name
    GUI_UC_SetEncodeUTF8();
    x = ui->week_first.x;
    y = ui->week_first.y;
    for (i = 0; i < 7; i++)
    {
        get_lang_res(week_str_res_id[i], str);
        GUI_DispStringAt(str, x, y);
        x += ui->size.week_sp.width;
        //y += ui->size.week_sp.height;
    }

    //draw date
    GUI_UC_SetEncodeGBK(); //todo:make to resource (农历)
    x = ui->date_first.x + ui->size.date_sp.width * weekday;
    y = ui->date_first.y;
    x_end =  ui->date_first.x + ui->size.date_sp.width * 7;
    for (i = 1; i <= monthday[now->month]; i++)
    {
        if (i == now->day)
        {
            GUI_SetColor(ui->colour.date_f);
        }
        else
        {
            GUI_SetColor(ui->colour.txt);
        }

        //公历
        app_sprintf(str, "%d", i);
        GUI_DispStringAt(str, x, y);
        //农历
        Cld_GetChinaDayStrEx(now->year, now->month, i, str);
        GUI_DispStringAt(str, x, y + ui->size.chs_dt_sp.height);

        x += ui->size.date_sp.width;
        if (x >= x_end)
        {
            //到下一行
            x = ui->date_first.x;
            y += ui->size.date_sp.height;
        }
    }

    GUI_SetColor(ui->colour.txt);
    //TODO:to check char code page
    //画日期信息
    Cld_GetChinaCalendarStrEx(now->year, now->month, now->day, str_chs_date);
    //Cld_GetChinaCalendarStr(now->year, now->month, now->day, str_chs_date);
#if DRAW_LONG_TIP
    weekday = get_week_day(now->year, now->month, now->day);
    get_lang_res(week_str_res_id_ex[weekday], str_week);
    Cld_GetJieQiStrEx(now->year, now->month, now->day, str_chs_date_tip);
    app_sprintf(str, "%d.%d.%d %s %s %s", now->year, now->month, now->day,
                str_week, str_chs_date, str_chs_date_tip);
    GUI_DispStringAt(str, ui->prompt.x, ui->prompt.y);
#else
    //rt.x0 = ui->prompt.x;
    rt.x0 = ui->date_first.x + ui->size.date_sp.width * (31 % 7 - 1);
    rt.y0 = ui->prompt.y;
    rt.x1 = x_end;
    rt.y1 = ui->rt.height;
    app_sprintf(str, "%d.%d.%d %s", now->year, now->month, now->day, str_chs_date);
    GUI_DispStringInRect(str, &rt, GUI_TA_LEFT);
#endif

    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//窗口 paint
//////////////////////////////////////////////////////////////////////////
static __s32 on_calendar_date_wnd_paint(__gui_msg_t *msg)
{
    __date_t now;
    calendar_date_wnd_res_t  *res;
    CLD_UI_DATE_PARA_T       *ui_date;
    GUI_MEMDEV_Handle	      draw_mem;
    H_LYR layer;

    layer = GUI_WinGetLyrWin(msg->h_deswin);
    if (GUI_LyrWinGetSta(layer) != GUI_LYRWIN_STA_ON)
    {
        return EPDK_FAIL;
    }

    res = &((calendar_date_wnd_para_t *)GUI_WinGetAttr(msg->h_deswin))->res;

    ui_date = &(get_calendar_uipara()->ui_date);

    esTIME_GetDate(&now);

    GUI_LyrWinSel(layer);

    draw_mem = GUI_MEMDEV_Create(0, 0, ui_date->rt.width, ui_date->rt.height);
    GUI_MEMDEV_Select(draw_mem);

    draw_date(msg->h_deswin, &now, res, ui_date);

    GUI_MEMDEV_CopyToLCD_Ex(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;

    return EPDK_OK;
}

//Timer处理
static __s32 on_calendar_date_wnd_timer(__gui_msg_t *msg)
{
    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//创建窗口处理
//////////////////////////////////////////////////////////////////////////
static __s32 on_calendar_date_wnd_create(__gui_msg_t *msg)
{
    //GUI_WinSetAddData(msg->h_deswin, (__u32)spsc_ctrl);
    calendar_date_wnd_para_t *wnd_para;

    wnd_para = (calendar_date_wnd_para_t *)GUI_WinGetAttr(msg->h_deswin);
    ASSERT(wnd_para);

    calendar_date_wnd_create_res(&wnd_para->res);

    _app_show_focus_wnd(msg->h_deswin, wnd_para->layer);

    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//销毁窗口处理
//////////////////////////////////////////////////////////////////////////
static __s32 on_calendar_date_wnd_destroy(__gui_msg_t *msg)
{
    calendar_date_wnd_para_t *wnd_para;

    wnd_para = (calendar_date_wnd_para_t *)GUI_WinGetAttr(msg->h_deswin);
    ASSERT(wnd_para);

    calendar_date_wnd_destroy_res(&wnd_para->res);

    BFreeType(wnd_para, calendar_date_wnd_para_t);
    //GUI_WinSetAttr(msg->h_deswin, NULL);

    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//窗口回调
//////////////////////////////////////////////////////////////////////////
static __s32 calendar_date_wnd_Proc(__gui_msg_t *msg)
{
    DEBUG_CBMSGEx(calendar_date_wnd_Proc);

    switch(msg->id)
    {
    case GUI_MSG_CREATE:
        on_calendar_date_wnd_create(msg);
        return EPDK_OK;
    case GUI_MSG_CLOSE:
        GUI_FrmWinDelete(msg->h_deswin);
        return EPDK_OK;
    case GUI_MSG_DESTROY:
        on_calendar_date_wnd_destroy(msg);
        return EPDK_OK;
    case GUI_MSG_PAINT:
        on_calendar_date_wnd_paint(msg);
        return EPDK_OK;
    case MSG_CALENDAR_DATE_UPDATE:
        GUI_WinUpdate(msg->h_deswin, EPDK_TRUE);
        return EPDK_OK;
    case GUI_MSG_KEY:
        calendar_date_wnd_key_proc(msg);
        break;
    case GUI_MSG_TOUCH:
        break;
    case GUI_MSG_TIMER:
        on_calendar_date_wnd_timer(msg);
        return EPDK_OK;
    case GUI_MSG_COMMAND:
        on_calendar_date_wnd_command(msg);
        return EPDK_OK;
    case GUI_MSG_WIN_WAKEUP:
        GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
        return EPDK_OK;
    default:
        break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

//////////////////////////////////////////////////////////////////////////
//创建窗口
//////////////////////////////////////////////////////////////////////////
H_WIN calendar_date_frmwin_create(H_WIN h_parent, H_LYR layer)
{
    __gui_framewincreate_para_t framewin_para;
    calendar_date_wnd_para_t *wnd_para;
    FB fb;

    BallocType(wnd_para, calendar_date_wnd_para_t);
    ASSERT(wnd_para != NULL);
    ZeroTypeMem(wnd_para, calendar_date_wnd_para_t);

    wnd_para->layer = layer;

    GUI_LyrWinGetFB(wnd_para->layer, &fb);

    ZeroTypeMem(&framewin_para, __gui_framewincreate_para_t);

    framewin_para.name          = CALENDAR_TIME_WND_NAME;
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = CALENDAR_DATE_WND_ID;
    framewin_para.hHosting      = h_parent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)calendar_date_wnd_Proc);
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
__s32 calendar_date_frmwin_delete(H_WIN hwnd)
{
    __s32 ret;
    ret = GUI_FrmWinDelete(hwnd);
    return ret;
}



