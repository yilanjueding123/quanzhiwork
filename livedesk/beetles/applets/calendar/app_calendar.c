/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: app_calendar.c
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/05/12 14:26  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#include "beetles_app.h"
#include "app_calendar_i.h"
//////////////////////////////////////////////////////////////////////////
static H_WIN g_calendar_main_hwnd = NULL;

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//检校日期,返回0日期正确,返回非0有修改
//////////////////////////////////////////////////////////////////////////
static __s32 revise_date(__date_t *date)
{
    if (IsInvalidYear(date->year))
    {
        date->year  = DEFAULT_YEAR;
        date->month = DEFAULT_MONTH;
        date->day   = DEFAULT_DAY;
    }
    else if (IsInvalidMonth(date->month))
    {
        date->month = DEFAULT_MONTH;
        date->day   = DEFAULT_DAY;
    }
    else
    {
        __u8  month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (IsLeapYear(date->year))
        {
            month[1] = 29;
        }
        if (OUT_OF_RANGE(date->day, 1, month[date->month - 1]))
        {
            date->day = DEFAULT_DAY;
        }
        else
        {
            return 0;
        }
    }
    return 1;
}

//////////////////////////////////////////////////////////////////////////
//检校时间,返回0时间正确,返回非0有修改
//////////////////////////////////////////////////////////////////////////
static __s32 revise_time(__time_t *time)
{
    __s32 ret = 0;
    if (IsInvalidHour(time->hour))
    {
        time->hour = DEFAULT_HOUR;
        ret |= 1;
    }
    if (IsInvalidMinute(time->minute))
    {
        time->minute = DEFAULT_MINUTE;
        ret |= 2;
    }
    if (IsInvalidSecond(time->second))
    {
        time->second = DEFAULT_SECOND;
        ret |= 4;
    }
    return ret;
}

//////////////////////////////////////////////////////////////////////////
//时间检测线程，时间变化时发消息给主窗口
//////////////////////////////////////////////////////////////////////////
#define CHECK_TIME_THREAD_STKSZ           0x2000//线程栈大小
#define DELAY_CHECK_TIME_THREAD           100

static void check_time_thread(void *arg)
{
    __date_t        now_date, old_date;
    __time_t        now_time, old_time;
    __gui_msg_t 	msg;
    calendar_data_t *main_data = (calendar_data_t *)GUI_WinGetAttr((H_WIN)arg);

    ZeroTypeMem(&msg, __gui_msg_t);

    esTIME_GetTime(&old_time);
    esTIME_GetDate(&old_date);
    if (revise_date(&old_date))
    {
        esTIME_SetDate(&old_date);
    }
    if (revise_time(&old_time))
    {
        esTIME_SetTime(&old_time);
    }

    while (1)
    {
        esKRNL_TimeDly(DELAY_CHECK_TIME_THREAD);

        if(esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            esKRNL_TDel(EXEC_prioself);
            DEBUG_CLD("****EXIT check_time_thread*****\n");
            return;
        }

        if (main_data->lyr_time && GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(main_data->lyr_time))
        {
            esTIME_GetTime(&now_time);

            if (compare_time(old_time, now_time))
            {
                set_time_(old_time, now_time);

                msg.id         = MSG_CALENDAR_TIME_UPDATE;
                msg.h_deswin   = main_data->h_time_frm;
                GUI_SendNotifyMessage(&msg);
            }
        }

        if (main_data->h_date_frm && main_data->lyr_date && GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(main_data->lyr_date))
        {
            esTIME_GetDate(&now_date);

            if (compare_date(old_date, now_date))
            {
                set_date_(old_date, now_date);

                msg.id         = MSG_CALENDAR_DATE_UPDATE;
                msg.h_deswin   = main_data->h_date_frm;
                GUI_SendNotifyMessage(&msg);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//发送命令到主窗口
//////////////////////////////////////////////////////////////////////////
__s32  CLD_SendCommand(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;

    msg.h_deswin   = g_calendar_main_hwnd;
    msg.h_srcwin   = hwin;
    msg.id         = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;

    DEBUG_CLD(" CLD_SendCommand(%x, %d, %d, %d)\n", hwin, id, data1, data2);

    return (GUI_SendMessage(&msg));
}


/***********************************************************************************************************
	建立图层
************************************************************************************************************/
#define cld_layer_palette_create(_layer, _prt)   app_com_layer_create_default(_layer, _prt, "calendar layer")
// static H_LYR cld_layer_palette_create(RECT *rect)
// {
// 	H_LYR layer = NULL;
// 	FB  fb =
// 	{
// 	    {0, 0},                                   		/* size      */
// 	    {0, 0, 0},                                      /* buffer    */
// 	    {FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
// 	};
//
// 	__disp_layer_para_t lstlyr =
// 	{
// 	    DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
// 	    0,                                              /* ck_mode   */
// 	    0,                                              /* alpha_en  */
// 	    0,                                              /* alpha_val */
// 	    1,                                              /* pipe      */
// 	    0xff,                                           /* prio      */
// 	    {0, 0, 0, 0},                           		/* screen    */
// 	    {0, 0, 0, 0},                               	/* source    */
// 	    DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
// 	    NULL                                            /* fb        */
// 	};
//
// 	__layerwincreate_para_t lyrcreate_info =
// 	{
// 	    "calendar  layer",
// 	    NULL,
// 	    GUI_LYRWIN_STA_SUSPEND,
// 	    GUI_LYRWIN_NORMAL
// 	};
//
// 	fb.size.width		= rect->width;
// 	fb.size.height		= rect->height;
//
// 	// framebuffer source window
// 	lstlyr.src_win.x  		= 0;
// 	lstlyr.src_win.y  		= 0;
// 	lstlyr.src_win.width 	= rect->width;
// 	lstlyr.src_win.height 	= rect->height;
//
// 	//screen region
// 	lstlyr.scn_win.x		= rect->x;
// 	lstlyr.scn_win.y		= rect->y;
// 	lstlyr.scn_win.width  	= rect->width;
// 	lstlyr.scn_win.height 	= rect->height;
//
// 	lstlyr.pipe = 1;
// 	lstlyr.fb = &fb;
// 	lyrcreate_info.lyrpara = &lstlyr;
//
// 	layer = GUI_LyrWinCreate(&lyrcreate_info);
// 	if( !layer )
// 	{
// 		__err("app bar layer create error !\n");
// 	}
//
// 	return layer;
// }

static __s32 on_command_setting_pop_wnd(H_WIN hmainwnd)
{
    calendar_data_t *main_data  = (calendar_data_t *)GUI_WinGetAttr(hmainwnd);

    ASSERT(main_data);
    ASSERT(main_data->lyr_setting);
    if (main_data->h_setting_frm == NULL)//TODO: h_setting_frm is window ?
    {
        main_data->h_setting_frm = calendar_setting_frmwin_create(hmainwnd, main_data->lyr_setting);
    }
    else
    {
        SEND_MSG(MSG_CALENDAR_SETTING_UPDATE, hmainwnd, main_data->h_setting_frm, 0, 0);
        _app_show_focus_wnd(main_data->h_setting_frm, main_data->lyr_setting);
    }

    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//命令消息处理
//////////////////////////////////////////////////////////////////////////
static __s32 on_calendar_command(__gui_msg_t *msg)
{
    H_WIN hwnd    = msg->h_deswin;
    __s32 cmd_id  = HIWORD(msg->dwAddData1);
    //	__s32 item_id = LOWORD(msg->dwAddData1);
    calendar_data_t *main_data  = (calendar_data_t *)GUI_WinGetAttr(hwnd);

    switch (cmd_id)
    {
    case CLD_CMD_SWITCH_TIME_WND:
    {
        ASSERT(main_data->lyr_time);
        _app_show_focus_wnd(main_data->h_time_frm, main_data->lyr_time);
    }
    break;
    case CLD_CMD_SWITCH_DATE_WND:
    {
        ASSERT(main_data->lyr_date);
        if (main_data->h_date_frm == NULL)//TODO: h_date_frm is window ?
        {
            main_data->h_date_frm = calendar_date_frmwin_create(hwnd, main_data->lyr_date);
        }
        else
        {
            _app_show_focus_wnd(main_data->h_date_frm, main_data->lyr_date);
        }
    }
    break;
    case CLD_CMD_SETTING_POP_WND:
        on_command_setting_pop_wnd(hwnd);
        break;
    case CLD_CMD_HIDE_SETTING_WND:
    {
        //TODO:
        if (main_data->lyr_setting)
        {
            app_hide_layer(main_data->lyr_setting);

            //TODO: make a function check last wnd
            if (GUI_LyrWinGetSta(main_data->lyr_time) == GUI_LYRWIN_STA_ON)
            {
                GUI_WinSetFocusChild(main_data->h_time_frm);
            }
            else if (GUI_LyrWinGetSta(main_data->lyr_date) == GUI_LYRWIN_STA_ON)
            {
                GUI_WinSetFocusChild(main_data->h_date_frm);
            }
        }
    }
    break;
    case CLD_CMD_CLOSE_WND:
    {
        __gui_msg_t msg_send;
        msg_send.h_deswin   = GUI_WinGetParent(hwnd);
        msg_send.h_srcwin   = hwnd;
        msg_send.id         = GUI_MSG_COMMAND;
        msg_send.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwnd), SWITCH_TO_MMENU);
        msg_send.dwAddData2 = 0;
        msg_send.dwReserved = 0;

        if( main_data->update_thread_id != 0)
        {
            while( esKRNL_TDelReq(main_data->update_thread_id) != OS_TASK_NOT_EXIST )
                esKRNL_TimeDly(5);

            main_data->update_thread_id = 0;
        }

        app_com_layer_destroy(main_data->lyr_date);
        app_com_layer_destroy(main_data->lyr_time);
        return (GUI_SendNotifyMessage(&msg_send));
    }
    default:
        break;
    }
    return EPDK_OK;
}


//////////////////////////////////////////////////////////////////////////
//创建时间窗口
//////////////////////////////////////////////////////////////////////////
static __s32 time_wnd_create(H_WIN hwnd, calendar_uipara_t *uipara)
{
    calendar_data_t *main_data = (calendar_data_t *)GUI_WinGetAttr(hwnd);

    cld_layer_palette_create(main_data->lyr_time, &uipara->ui_time.rt);

    main_data->h_time_frm = calendar_time_frmwin_create(hwnd, main_data->lyr_time);

    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//主窗口初始化
//////////////////////////////////////////////////////////////////////////
static __s32 calendar_init(__gui_msg_t *msg)
{
    __s32 ret;
    calendar_uipara_t *uipara = get_calendar_uipara();
    calendar_data_t   *main_data   = (calendar_data_t *)GUI_WinGetAttr(msg->h_deswin);

    ret = time_wnd_create(msg->h_deswin, uipara);

    main_data->update_thread_id = esKRNL_TCreate(check_time_thread, (void *)msg->h_deswin,
                                  CHECK_TIME_THREAD_STKSZ, KRNL_priolevel5);

    cld_layer_palette_create(main_data->lyr_date, &uipara->ui_date.rt);
    cld_layer_palette_create(main_data->lyr_setting, &uipara->ui_setting.rt);

    return ret;
}


//主消息处理函数
static __s32 _app_calendar_Proc(__gui_msg_t *msg)
{
    DEBUG_CBMSGEx(_app_calendar_Proc);
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        g_calendar_main_hwnd = msg->h_deswin;
        com_set_palette_by_id(ID_CALENDAR_COLOURTABLE_BMP);
        calendar_init(msg);
        gscene_bgd_set_state(BGD_STATUS_HIDE);
        return EPDK_OK;
    }
    case DSK_MSG_HOME:
    case DSK_MSG_KILL:
    case GUI_MSG_CLOSE:
    {
        GUI_ManWinDelete(msg->h_deswin);
    }
    return EPDK_OK;
    case GUI_MSG_DESTROY:
    {
        calendar_data_t   *main_data   = (calendar_data_t *)GUI_WinGetAttr(msg->h_deswin);

        g_calendar_main_hwnd = NULL;

        if( main_data->update_thread_id != 0)
        {
            while( esKRNL_TDelReq(main_data->update_thread_id) != OS_TASK_NOT_EXIST )
            {
                esKRNL_TimeDly(5);
            }

            main_data->update_thread_id = 0;
        }

        app_com_layer_destroy(main_data->lyr_date);
        app_com_layer_destroy(main_data->lyr_time);
        app_com_layer_destroy(main_data->lyr_setting);

        BFreeType(main_data, calendar_data_t);
        return EPDK_OK;
    }
    case GUI_MSG_COMMAND:
    {
        on_calendar_command(msg);
    }
    return EPDK_OK;
    case GUI_MSG_KEY:
    {
#if 1
        {
            //for develope board
            if (msg->dwAddData1 == GUI_MSG_KEY_VDEC)
            {
                DEBUG_CLD("\n _db_ GUI_MSG_KEY_VDEC -> GUI_MSG_KEY_MENU  ____ \n");
                msg->dwAddData1 = GUI_MSG_KEY_MENU;
            }
            else if (msg->dwAddData1 == GUI_MSG_KEY_LONGVDEC)
            {
                DEBUG_CLD("\n _db_ GUI_MSG_KEY_LONGVDEC -> GUI_MSG_KEY_LONGMENU ____ \n");
                msg->dwAddData1 = GUI_MSG_KEY_LONGMENU;
            }
        }
#endif
    }
    break;
    default:
        break;
    }
    return 	GUI_ManWinDefaultProc(msg);
}


__s32 app_calendar_create(root_para_t *para)
{
    __gui_manwincreate_para_t create_info;
    calendar_data_t *main_data;

    __inf("****************************************************************************************\n");
    __inf("********  enter calendar app  **************\n");
    __inf("****************************************************************************************\n");


    BallocType(main_data, calendar_data_t);
    if (main_data == NULL)
    {
        return EPDK_FAIL;
    }
    ZeroTypeMem(main_data, calendar_data_t);

    main_data->font      = para->font;
    main_data->root_type = para->root_type;

    ZeroTypeMem(&create_info, __gui_manwincreate_para_t);

    create_info.name            = APP_CALENDAR;
    create_info.hParent         = para->h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_app_calendar_Proc);
    create_info.attr            = (void *)main_data;
    create_info.id				= APP_CALENDAR_ID;
    create_info.hHosting        = NULL;

    return(GUI_ManWinCreate(&create_info));
}




