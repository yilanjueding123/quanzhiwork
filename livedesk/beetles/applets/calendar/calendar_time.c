/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: calendar_time.c
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/05/12 14:27  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#include "beetles_app.h"
#include "calendar_time.h"
#include "calendar_adapter.h"
#include "app_calendar_i.h"
//////////////////////////////////////////////////////////////////////////

//
#define mc_make_str_3_(_str1, _str2, _str3)  _str1 ## _str2 ## _str3
#define res_id_num_(_id)                     mc_make_str_3_(ID_CALENDAR_TIME_NUM_, _id, _BMP)
static __s32 calendar_time_wnd_create_res(calendar_time_wnd_res_t *res)
{
    __s32 i;
    __s32 num_id[10] =
    {
        res_id_num_(0), res_id_num_(1), res_id_num_(2), res_id_num_(3), res_id_num_(4),
        res_id_num_(5), res_id_num_(6), res_id_num_(7), res_id_num_(8), res_id_num_(9)
    };

    create_bmp_res(ID_CALENDAR_TIME_AM_BMP, res->bmp_am);
    create_bmp_res(ID_CALENDAR_TIME_PM_BMP, res->bmp_pm);
    create_bmp_res(ID_CALENDAR_TIME_BG_BMP, res->bmp_bg);
    create_bmp_res(ID_CALENDAR_TIME_COLON_BMP, res->bmp_colon);

    for (i = 10; i--;)
    {
        create_bmp_res(num_id[i], res->bmp_num[i]);
    }

    return EPDK_OK;
}

static __s32 calendar_time_wnd_destroy_res(calendar_time_wnd_res_t *res)
{
    __s32 i;

    destroy_bmp_res(res->bmp_am);
    destroy_bmp_res(res->bmp_pm);
    destroy_bmp_res(res->bmp_bg);
    destroy_bmp_res(res->bmp_colon);

    for (i = 10; i--;)
    {
        destroy_bmp_res(res->bmp_num[i]);
    }

    return EPDK_OK;
}

static __s32 switch_date_wnd(H_WIN hwnd)
{
    calendar_time_wnd_para_t *para = (calendar_time_wnd_para_t *)GUI_WinGetAttr(hwnd);
    app_hide_layer(para->layer);
    return CLD_SendCommand(hwnd, CLD_CMD_SWITCH_DATE_WND, hwnd, 0);
}

//窗口按键处理
static __s32 calendar_time_wnd_key_proc(__gui_msg_t *msg)
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
        switch_date_wnd(msg->h_deswin);
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

static __s32 on_calendar_time_wnd_command(__gui_msg_t *msg)
{
    return EPDK_OK;
}

static __s32 draw_time(H_WIN hwnd, __time_t *time, calendar_time_wnd_res_t *res, CLD_UI_TIME_PARA_T *ui_time)
{
    __s32 format;
    __s32 hour;
    __s32 x, y;
    load_bmp_res_t *bmp_num;
    __s32 isPM = 0;

    //DEBUG_CLD(" draw time( %d : %d : %d )\n", time->hour, time->minute, time->second);

    bmp_num = res->bmp_num;

    //isPM = 0;
    format = get_time_format();//TODO: get reg setting
    //时
    hour = time->hour;
    if ( ! Is24HourFormat(format))
    {
        isPM = hour24_to_hour12(&hour);
    }

    //draw back
    GUI_BMP_RES_Draw(res->bmp_bg, 0, 0);

    y = ui_time->pos_first_num.y;
    x = ui_time->pos_first_num.x;

    GUI_BMP_RES_Draw(bmp_num[hour / 10], x, y);
    x += ui_time->bmp_size.num.width;
    GUI_BMP_RES_Draw(bmp_num[hour % 10], x, y);
    x += ui_time->bmp_size.num.width;

    GUI_BMP_RES_Draw(res->bmp_colon, x, y);
    x += ui_time->bmp_size.colon.width;

    //分
    GUI_BMP_RES_Draw(bmp_num[time->minute / 10], x, y);
    x += ui_time->bmp_size.num.width;
    GUI_BMP_RES_Draw(bmp_num[time->minute % 10], x, y);
    x += ui_time->bmp_size.num.width;

    GUI_BMP_RES_Draw(res->bmp_colon, x, y);
    x += ui_time->bmp_size.colon.width;

    //秒
    GUI_BMP_RES_Draw(bmp_num[time->second / 10], x, y);
    x += ui_time->bmp_size.num.width;
    GUI_BMP_RES_Draw(bmp_num[time->second % 10], x, y);
    x += ui_time->bmp_size.num.width;

    //format
    if (format)
    {
        if (isPM)
        {
            GUI_BMP_RES_Draw(res->bmp_pm, x, y);
        }
        else
        {
            GUI_BMP_RES_Draw(res->bmp_am, x, y);
        }
    }

    return EPDK_OK;
}

//窗口 paint
static __s32 on_calendar_time_wnd_paint(__gui_msg_t *msg)
{
    __time_t now;
    calendar_time_wnd_res_t  *res;
    CLD_UI_TIME_PARA_T       *ui_time;
    GUI_MEMDEV_Handle	      draw_mem;
    H_LYR layer;

    layer = GUI_WinGetLyrWin(msg->h_deswin);
    if (GUI_LyrWinGetSta(layer) != GUI_LYRWIN_STA_ON)
    {
        return EPDK_FAIL;
    }

    res = &((calendar_time_wnd_para_t *)GUI_WinGetAttr(msg->h_deswin))->res;

    ui_time = &(get_calendar_uipara()->ui_time);

    GUI_LyrWinSel(layer);
    GUI_SetFont(SWFFont);
    draw_mem = GUI_MEMDEV_Create(0, 0, ui_time->rt.width, ui_time->rt.height);
    GUI_MEMDEV_Select(draw_mem);

    esTIME_GetTime(&now);
    draw_time(msg->h_deswin, &now, res, ui_time);

    GUI_MEMDEV_CopyToLCD_Ex(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;

    return EPDK_OK;
}

//Timer处理
static __s32 on_calendar_time_wnd_timer(__gui_msg_t *msg)
{
    return EPDK_OK;
}

//窗口创建后处理
static __s32 on_calendar_time_wnd_create(__gui_msg_t *msg)
{
    calendar_time_wnd_para_t *wnd_para;

    wnd_para = (calendar_time_wnd_para_t *)GUI_WinGetAttr(msg->h_deswin);
    ASSERT(wnd_para);

    calendar_time_wnd_create_res(&wnd_para->res);

    _app_show_focus_wnd(msg->h_deswin, wnd_para->layer);

    return EPDK_OK;
}

//销毁窗口处理
static __s32 on_calendar_time_wnd_destroy(__gui_msg_t *msg)
{
    calendar_time_wnd_para_t *wnd_para;

    wnd_para = (calendar_time_wnd_para_t *)GUI_WinGetAttr(msg->h_deswin);
    ASSERT(wnd_para);

    calendar_time_wnd_destroy_res(&wnd_para->res);

    BFreeType(wnd_para, calendar_time_wnd_para_t);
    //GUI_WinSetAttr(msg->h_deswin, NULL);

    return EPDK_OK;
}

//窗口回调
static __s32 calendar_time_wnd_Proc(__gui_msg_t *msg)
{
    if (msg->id != MSG_CALENDAR_TIME_UPDATE)
    {
        DEBUG_CBMSGEx(calendar_time_wnd_Proc);
    }

    switch(msg->id)
    {
    case GUI_MSG_CREATE:
        on_calendar_time_wnd_create(msg);
        return EPDK_OK;
    case GUI_MSG_CLOSE:
        GUI_FrmWinDelete(msg->h_deswin);
        return EPDK_OK;
    case GUI_MSG_DESTROY:
        on_calendar_time_wnd_destroy(msg);
        return EPDK_OK;
    case GUI_MSG_PAINT:
        on_calendar_time_wnd_paint(msg);
        return EPDK_OK;
    case MSG_CALENDAR_TIME_UPDATE:
        on_calendar_time_wnd_paint(msg);//TODO:To check
        return EPDK_OK;
    case GUI_MSG_KEY:
        calendar_time_wnd_key_proc(msg);
        break;
    case GUI_MSG_TOUCH:
        break;
    case GUI_MSG_TIMER:
        on_calendar_time_wnd_timer(msg);
        return EPDK_OK;
    case GUI_MSG_COMMAND:
        on_calendar_time_wnd_command(msg);
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
H_WIN calendar_time_frmwin_create(H_WIN h_parent, H_WIN layer)
{
    __gui_framewincreate_para_t framewin_para;
    calendar_time_wnd_para_t *wnd_para;
    FB fb;

    BallocType(wnd_para, calendar_time_wnd_para_t);
    ASSERT(wnd_para != NULL);
    ZeroTypeMem(wnd_para, calendar_time_wnd_para_t);

    wnd_para->layer = layer;

    GUI_LyrWinGetFB(wnd_para->layer, &fb);

    ZeroTypeMem(&framewin_para, __gui_framewincreate_para_t);

    framewin_para.name          = CALENDAR_TIME_WND_NAME;
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = CALENDAR_TIME_WND_ID;
    framewin_para.hHosting      = h_parent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)calendar_time_wnd_Proc);
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
__s32 calendar_time_frmwin_delete(H_WIN hwnd)
{
    __s32 ret;
    ret = GUI_FrmWinDelete(hwnd);
    return ret;
}



