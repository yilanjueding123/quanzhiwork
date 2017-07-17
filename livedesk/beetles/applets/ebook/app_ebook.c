/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : app_ebook.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/
#define EPDK_DEBUG_LEVEL_GATE        4

#include "app_ebook.h"
#include "ebook_loading.h"
#include "ebook_mset.h"
#include "ebook_sset.h"
#include "ebook_tips.h"
#include "ebook_uipara.h"
#include "page_com.h"
#include "mbook\mbook_private.h"
#include "app_ebook_i.h"

#define NAME_EBOOK_BKPOINT "EBOOKINF"
typedef struct tag_ebook_ctrl
{
    GUI_FONT *ebook_font;
    __s32 root_type;

    __ebook_bkpoint_t  book_bkpoint;
    H_DECODE            mbook;
    __decode_config_t   config;				/* monkey core show config*/

    H_LYR				backLyr;			/* background layer */
    H_LYR				pageLyr;			/* slider layer */
    H_LYR				volume_bar_Lyr;			/* slider layer */
    H_LYR				msetlyr;
    H_LYR				ssetlyr;
    H_LYR				tipsLyr;
    H_LYR				loading_lyr;		//加载图层

    H_WIN				h_back;			/* background win component */
    H_WIN				h_page;			/* page win component 	*/
    H_WIN				h_mset;
    H_WIN				h_sset;
    H_WIN				h_tips;
    H_WIN				h_loading;
    __s32				cur_page;
    __s32				total_page;
    __s32				page_sta;
    __s32				auto_time;
    __s32				sset_last_main_id;	//记忆上一次退出时选择的sset的菜单位置
    __s32				sset_last_focus_id;	//
    __s32 				mset_last_focus_id;	//
    __s32				bkpoint_enable;	//是否加载断点提示框
} ebook_ctrl_t;

static __s32 ebook_timeid = 0xff;
static reg_ebook_para_t *reg_ebook_para = 0;

/***********************************************************************************************************
	建立图层
************************************************************************************************************/
H_LYR ebook_tips_layer_create(RECT *rect)
{
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                   		/* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
    };

    __disp_layer_para_t lstlyr =
    {
        DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        0xff,                                           /* prio      */
        {0, 0, 0, 0},                           		/* screen    */
        {0, 0, 0, 0},                               	/* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };

    __layerwincreate_para_t lyrcreate_info =
    {
        "tips layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };

    fb.size.width		= rect->width;
    fb.size.height		= rect->height;

    lstlyr.src_win.x  		= 0;
    lstlyr.src_win.y  		= 0;
    lstlyr.src_win.width 	= rect->width;
    lstlyr.src_win.height 	= rect->height;

    lstlyr.scn_win.x		= (400 - rect->width) / 2;
    lstlyr.scn_win.y		= (240 - 60 - rect->height) / 2; //240为屏幕高度，60为底端高度
    lstlyr.scn_win.width  	= rect->width;
    lstlyr.scn_win.height 	= rect->height;

    lstlyr.pipe = 1;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;

    layer = GUI_LyrWinCreate(&lyrcreate_info);
    if( !layer )
    {
        __err("tips layer create error !\n");
    }

    return layer;
}


/**********************************************************************************************************************
* 图层创建接口
**********************************************************************************************************************/
static H_WIN ebook_layer_create( RECT *rect)
{
    FB	fb =
    {
        {0, 0},
        {0, 0, 0},
        {FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, 0}},
    };
    __disp_layer_para_t para =
    {
        DISP_LAYER_WORK_MODE_PALETTE,
        0,
        0,
        0,

        0,
        0xff,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        DISP_LAYER_OUTPUT_CHN_DE_CH1,
        NULL
    };

    __layerwincreate_para_t create_info =
    {
        "layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        0
    };

    fb.size.width  = rect->width;
    fb.size.height = rect->height;

    para.pipe			= 1;

    para.scn_win.x 		= rect->x;
    para.scn_win.y 		= rect->y;
    para.scn_win.width 	= rect->width;
    para.scn_win.height = rect->height;

    para.src_win.x 		= 0;
    para.src_win.y 		= 0;
    para.src_win.width 	= rect->width;
    para.src_win.height = rect->height;

    para.fb = &fb;
    create_info.lyrpara = &para;
    return(GUI_LyrWinCreate(&create_info));
}

/*********************************************************************************************************************************************************
*
*		向父窗口发信息
*
***********************************************************************************************************************************************************/
__s32 Ebook_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    char buf[128];

    msg.h_deswin = GUI_WinGetParent(hwin);
    msg.h_srcwin = NULL;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;
    return (GUI_SendNotifyMessage(&msg));
    //return (GUI_SendMessage(&msg));
}


static __s32 mbook_load_breakpoint_info(__ebook_bkpoint_t  *book_bkpoint)
{
    __s32 ret;

    ret = MBOOK_Decode_get_filename(book_bkpoint->ebookname);
    if (-1 == ret)
    {
        __msg("get file name fail...\n");
        return -1;
    }

    if (!fifo_db_has_exist(NAME_EBOOK_BKPOINT))
        return -1;

    if (get_item_content_from_fifo_db(NAME_EBOOK_BKPOINT, book_bkpoint->ebookname, book_bkpoint))
        return -1;
    return 0;
}

static void mbook_save_breakpoint_info(__gui_msg_t *msg)
{
    __s32 ret;
    HRATNPL	h_rat_npl;
    __s32  npl_index;
    ebook_ctrl_t         *ebook_ctrl;

    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    ebook_ctrl->book_bkpoint.page_no = ebook_ctrl->cur_page;	//获取当前页码

    ret = MBOOK_Decode_get_filename(ebook_ctrl->book_bkpoint.ebookname);	//获取电子书的书面
    if(ret != 0 )
    {
        __msg("get file name fail ........\n");
    }
    if (!fifo_db_has_exist(NAME_EBOOK_BKPOINT))
    {
        ret = create_fifo_db(NAME_EBOOK_BKPOINT, 16, sizeof(__ebook_bkpoint_t));	//不存在，创建数据库
        if (0 != ret)
        {
            __msg("create_fifo_db fail...\n");
        }
    }
    ret = add_item_to_fifo_db(NAME_EBOOK_BKPOINT, ebook_ctrl->book_bkpoint.ebookname, &ebook_ctrl->book_bkpoint);

    if(ret != 0)
    {

        __msg("add_item_to_fifo_db fail...\n");
    }
}

void ebook_set_auto_time(__gui_msg_t *msg)
{
    __u32 auto_play_speed;
    ebook_ctrl_t         *ebook_ctrl;

    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if(reg_ebook_para)
    {
        auto_play_speed = reg_ebook_para->switch_time;
        if(auto_play_speed == 10)
        {
            GUI_SetTimer(msg->h_deswin, ebook_timeid, 10 * 100 , NULL);
            ebook_ctrl->auto_time = 10 * 100;
        }
        else if(auto_play_speed == 20)
        {
            GUI_SetTimer(msg->h_deswin, ebook_timeid, 20 * 100 , NULL);
            ebook_ctrl->auto_time = 20 * 100;
        }
        else if(auto_play_speed == 30)
        {
            GUI_SetTimer(msg->h_deswin, ebook_timeid, 30 * 100 , NULL);
            ebook_ctrl->auto_time = 30 * 100;
        }
        else
        {
            ebook_ctrl->auto_time = 0;
        }

    }
}

__s32   mbook_on_create(__gui_msg_t *msg)
{
    __s32               tmp, time;
    __s32               tmp_point;
    char                path[1024];
    ebook_ctrl_t         *ebook_ctrl;
    ebook_uipara_t *ebook_uipara;
    __page_create_para_t page_para;
    power_state_e state;
    RECT rect;
    __s32 ret;
    __u8    err;

    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();
    if(ebook_ctrl == NULL)
    {
        __wrn(" CShowScene malloc error \n");
        return EPDK_FAIL;
    }

    /*加载loading 提示框*/
    {
        ebook_loading_para_t ebook_loading_para;
        rect.x = ebook_uipara->loading_lyr.x;
        rect.y = ebook_uipara->loading_lyr.y;
        rect.width = ebook_uipara->loading_lyr.w;
        rect.height = ebook_uipara->loading_lyr.h;
        ebook_ctrl->loading_lyr = ebook_tips_layer_create(&rect);
        ebook_loading_para.font = ebook_ctrl->ebook_font;
        ebook_loading_para.loading_layer = ebook_ctrl->loading_lyr;
        ebook_ctrl->h_loading = ebook_loading_win_create(msg->h_deswin, &ebook_loading_para);
    }
    /* 创建 mbook core 句柄 */
    err = 0;
    ebook_ctrl->mbook = MBOOK_Decode_Init(NULL, &err);
    if(ebook_ctrl->mbook == NULL)
    {
        __wrn(" init mbook error \n");
        goto EBOOK_ERROR_1;
    }

    MBOOK_Decode_GetFilePath(ebook_ctrl->mbook, path);
    // 设置默认编码
    MBOOK_Decode_SetDefaultCharset(ebook_ctrl->mbook, EPDK_CHARSET_ENM_GBK);

    if(reg_ebook_para)
    {
        if( (reg_ebook_para->text_colour == APP_COLOR_WHITE) || (reg_ebook_para->text_colour == APP_COLOR_BLACK)
                || (reg_ebook_para->text_colour == APP_COLOR_YELLOW) )
            ebook_ctrl->config.font_color    =  reg_ebook_para->text_colour;
        else
            ebook_ctrl->config.font_color    = APP_COLOR_WHITE;
    }
    else
    {
        ebook_ctrl->config.font_color    = APP_COLOR_WHITE;
    }
    ebook_ctrl->config.show_rotate   = 0;
    ebook_ctrl->config.font_size     = 16;
    ebook_ctrl->config.char_font     = ebook_ctrl->ebook_font;
    ebook_ctrl->config.row_space     = 0;
    ebook_ctrl->config.col_space     = 0;
    ebook_ctrl->config.border_width  = 10;
    ebook_ctrl->config.scroll_width  =  0;

    ebook_ctrl->config.bottom_width  = 0;
    ebook_ctrl->config.show_width    = 400;
    ebook_ctrl->config.show_height   = 210;

    MBOOK_Decode_Config(ebook_ctrl->mbook, &ebook_ctrl->config);


    // 打开页面显示
    ebook_ctrl->total_page = MBOOK_Decode_GetTotalPage(ebook_ctrl->mbook);
    if(ebook_ctrl->book_bkpoint.page_no == 0)
    {
        ebook_ctrl->cur_page = MBOOK_Decode_ShowPage(ebook_ctrl->mbook, 0); //不存在断点信息或者存在断点信息但从头开始阅读
    }
    else
    {
        ebook_ctrl->cur_page = MBOOK_Show_LastPage(ebook_ctrl->mbook, ebook_ctrl->book_bkpoint.page_no);//从上一次阅读的地方，接着往下阅读
    }
    //ebook_set_auto_time(msg);	//设置自动播放时间(通过读取注册表)

    //这句话主要是因为客户的特定需要，要求每次开机后应该默认为手动播放
    ebook_ctrl->auto_time = 0;
    if( reg_ebook_para )
    {
        reg_ebook_para->switch_time = 0;
    }
    rect.x = ebook_uipara->page_lyr.x;
    rect.y = ebook_uipara->page_lyr.y;
    rect.width = ebook_uipara->page_lyr.w;
    rect.height = ebook_uipara->page_lyr.h;
    ebook_ctrl->pageLyr = ebook_layer_create(&rect);//创建page 图层

    rect.x = ebook_uipara->volume_bar_lyr.x;
    rect.y = ebook_uipara->volume_bar_lyr.y;
    rect.width = ebook_uipara->volume_bar_lyr.w;
    rect.height = ebook_uipara->volume_bar_lyr.h;
    ebook_ctrl->volume_bar_Lyr = ebook_layer_create(&rect);	//创建volume bar 图层

    page_para.font = ebook_ctrl->ebook_font;
    page_para.page_layer = ebook_ctrl->pageLyr;
    page_para.volume_bar_layer = ebook_ctrl->volume_bar_Lyr;
    if (ebook_ctrl->auto_time != 0)
        page_para.play_status = 1;
    else
        page_para.play_status = 0;

    page_para.volume = dsk_volume_get();
    page_para.cur_page = ebook_ctrl->cur_page;
    page_para.total_page = ebook_ctrl->total_page;
    eLIBs_strcpy(page_para.name, (void *)(eLIBs_strchrlast(path, '\\') + 1));	//获取当前阅读的电子书的书名

    if(ebook_ctrl->loading_lyr)
    {
        GUI_LyrWinDelete(ebook_ctrl->loading_lyr);	//删除loading 窗口
        ebook_ctrl->loading_lyr = NULL;
    }

    ebook_ctrl->h_page = page_frmwin_create(msg->h_deswin, &page_para);
    GUI_LyrWinSetSta(ebook_ctrl->pageLyr, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(ebook_ctrl->pageLyr);
    GUI_WinSetFocusChild(ebook_ctrl->h_page);

    return EPDK_OK;

EBOOK_ERROR_2:
    if(ebook_ctrl->mbook)
    {
        MBOOK_Decode_Uninit(ebook_ctrl->mbook);
        ebook_ctrl->mbook = NULL;
    }
EBOOK_ERROR_1:
    {
        __msg("*******err = %d*******\n", err);

        if(1 != err)//如果拔卡则不退到浏览器
        {
            __gui_msg_t setmsg;
            setmsg.id = GUI_MSG_CLOSE;
            setmsg.h_deswin = msg->h_deswin;
            GUI_SendMessage(&setmsg);
        }
    }
    return EPDK_FAIL;
}

__s32   mbook_on_delete(__gui_msg_t *msg)
{
    char            context[100];
    __s32           offset, ret, ebook_num;
    char		ebook_file[1024];
    ebook_ctrl_t     *ebook_ctrl;

    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr( msg->h_deswin);

    mbook_save_breakpoint_info(msg);	//保存断点信息
    // 关闭mbook//
    __here__;

    if(ebook_ctrl->mbook)
    {
        MBOOK_Decode_Uninit(ebook_ctrl->mbook);
        ebook_ctrl->mbook = NULL;
    }
    __here__;

    if(ebook_ctrl->loading_lyr)
    {
        GUI_LyrWinDelete(ebook_ctrl->loading_lyr);
        ebook_ctrl->loading_lyr = NULL;
    }
    __here__;


    if(ebook_ctrl->tipsLyr)
    {
        GUI_LyrWinDelete(ebook_ctrl->tipsLyr);
        ebook_ctrl->tipsLyr = NULL;
    }
    __here__;

    if(ebook_ctrl->volume_bar_Lyr)
    {

        GUI_LyrWinDelete(ebook_ctrl->volume_bar_Lyr);
        ebook_ctrl->volume_bar_Lyr = 0;
    }

    __here__;
    if (ebook_ctrl->msetlyr)
    {
        GUI_LyrWinDelete(ebook_ctrl->msetlyr);
        ebook_ctrl->msetlyr = NULL;
    }
    __here__;

    if (ebook_ctrl->ssetlyr)
    {
        GUI_LyrWinDelete(ebook_ctrl->ssetlyr);
        ebook_ctrl->ssetlyr = NULL;
    }

    __msg("ebook_ctrl->pageLyr=%x\n", ebook_ctrl->pageLyr);

    if(ebook_ctrl->pageLyr)
    {
        __msg("before GUI_LyrWinDelete\n");
        GUI_LyrWinDelete(ebook_ctrl->pageLyr);
        __here__;
        ebook_ctrl->pageLyr = NULL;
    }

    __here__;
    // 释放主窗口控制结构 //
    My_Bfree(ebook_ctrl, sizeof(ebook_ctrl_t));

    __here__;
    ebook_ctrl = NULL;

    // 保存控制句柄//
    GUI_WinSetAttr(msg->h_deswin, NULL);

    // 恢复自动关屏功能 //
    //dsk_display_enable_close_scn();

    return EPDK_OK;
}

/*
	sset create
*/
static __s32 sset_win_create(__gui_msg_t *msg, __s32 mset_id)
{
    ebook_ctrl_t *ebook_ctrl;
    ebook_uipara_t *ebook_uipara;
    ebook_sset_para_t sset_para;
    RECT rect;

    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();

    if (ebook_ctrl->ssetlyr)
        GUI_LyrWinDelete(ebook_ctrl->ssetlyr);
    ebook_ctrl->ssetlyr = 0;

    eLIBs_memset(&sset_para, 0, sizeof(ebook_sset_para_t));

    switch(mset_id)
    {
    case ID_EBOOK_MSET_BG_MUSIC:
    {
        __u32	ebook_bg_flag;

        ebook_bg_flag = is_app_exist(APP_MUSIC);
        rect.x = ebook_uipara->sset_bgmusic_lyr.x;
        rect.y = ebook_uipara->sset_bgmusic_lyr.y;
        rect.width = ebook_uipara->sset_bgmusic_lyr.w;
        rect.height = ebook_uipara->sset_bgmusic_lyr.h;
        sset_para.value = ebook_bg_flag;
        break;
    }
    case ID_EBOOK_MSET_AUTO:
    {
        __u32	ebook_auto_time;
        __u32   ebook_auto_play_focus_id;

        if(reg_ebook_para)
            ebook_auto_time = reg_ebook_para->switch_time;
        else
            ebook_auto_time = 0;
        switch(ebook_auto_time)
        {
        case 0:
            ebook_auto_play_focus_id = AUTO_TIME_OFF;
            break;
        case 10:
            ebook_auto_play_focus_id = AUTO_TIME_FAST;
            break;
        case 20:
            ebook_auto_play_focus_id = AUTO_TIME_MIDDLE;
            break;
        case 30:
            ebook_auto_play_focus_id = AUTO_TIME_SLOW;
            break;
        default:
            ebook_auto_play_focus_id = AUTO_TIME_OFF;
            break;

        }

        //ebook_auto_play_focus_id = AUTO_TIME_OFF;
        rect.x = ebook_uipara->sset_play_lyr.x;
        rect.y = ebook_uipara->sset_play_lyr.y;
        rect.width = ebook_uipara->sset_play_lyr.w;
        rect.height = ebook_uipara->sset_play_lyr.h;
        sset_para.value = ebook_auto_play_focus_id;
        break;
    }
    case ID_EBOOK_MSET_TXT_COLOR:
    {

        __u32	ebook_text_color_index;
        __u32   ebook_text_color;

        if(reg_ebook_para)
            ebook_text_color = reg_ebook_para->text_colour;
        else
            ebook_text_color = APP_EBOOK_FCS_TXT_COLOR;

        switch(ebook_text_color)
        {
        case APP_COLOR_BLACK:
            ebook_text_color_index = 0;
            break;
        case APP_COLOR_WHITE:
            ebook_text_color_index = 1;
            break;
        case APP_COLOR_YELLOW:
            ebook_text_color_index = 2;
            break;
        default:
            ebook_text_color_index = 1;
            break;

        }
        rect.x = ebook_uipara->sset_txtcolor_lyr.x;
        rect.y = ebook_uipara->sset_txtcolor_lyr.y;
        rect.width = ebook_uipara->sset_txtcolor_lyr.w;
        rect.height = ebook_uipara->sset_txtcolor_lyr.h;
        sset_para.value = ebook_text_color_index;
        break;
    }
    case ID_EBOOK_MSET_GOTO_PAGE:
    {
        rect.x = ebook_uipara->sset_goto_lyr.x;
        rect.y = ebook_uipara->sset_goto_lyr.y;
        rect.width = ebook_uipara->sset_goto_lyr.w;
        rect.height = ebook_uipara->sset_goto_lyr.h;
        sset_para.value = ebook_ctrl->cur_page;
        sset_para.total_page = ebook_ctrl->total_page;
        break;
    }
    default:
        return EPDK_FAIL;
    }

    ebook_ctrl->ssetlyr = ebook_layer_create(&rect);
    sset_para.layer = ebook_ctrl->ssetlyr;
    sset_para.sset_font = ebook_ctrl->ebook_font;
    sset_para.main_id = mset_id;

    ebook_ctrl->h_sset = ebook_sset_win_create(msg->h_deswin, &sset_para);

    GUI_LyrWinCacheOn();
    GUI_LyrWinSetSta(ebook_ctrl->ssetlyr, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(ebook_ctrl->ssetlyr);
    GUI_LyrWinCacheOff();

    return EPDK_OK;
}
/*

*/
static __s32 page_info_proc(__gui_msg_t *msg)
{
    __gui_msg_t setmsg;
    ebook_ctrl_t     *ebook_ctrl;
    ebook_uipara_t  *ebook_uipara;

    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr( msg->h_deswin);

    switch(HIWORD(msg->dwAddData1))
    {
    case CMD_PAGE_SET_VOLUME:
    {
        reg_system_para_t *para;
        para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);	//获取当前音量
        if(para)
            para->volume = msg->dwAddData2;			//设置当前音量
        dsk_volume_on();
        dsk_volume_set(msg->dwAddData2);
        break;
    }
    case CMD_PAGE_SET_NEXT:
    {
        ebook_ctrl->cur_page = MBOOK_Decode_ShowNext(ebook_ctrl->mbook);
        setmsg.id = COM_CMD_UPDATE_PAGE;
        setmsg.h_deswin = ebook_ctrl->h_page;
        setmsg.dwAddData1 = ebook_ctrl->cur_page;
        GUI_SendMessage(&setmsg);
        break;
    }
    case CMD_PAGE_SET_PREV:
    {
        ebook_ctrl->cur_page = MBOOK_Decode_ShowPrev(ebook_ctrl->mbook);
        setmsg.id = COM_CMD_UPDATE_PAGE;
        setmsg.h_deswin = ebook_ctrl->h_page;
        setmsg.dwAddData1 = ebook_ctrl->cur_page;
        GUI_SendMessage(&setmsg);
        break;
    }
    case CMD_PAGE_SET_CLOSE:
    {
        Ebook_cmd2parent(msg->h_deswin, SWITCH_TO_EXPLORER, 0, 0);
        break;
    }
    case CMD_PAGE_SET_PLAY:
    {
        if (msg->dwAddData2 == 0)	//电子书播放状态, 1 表示正在播放， 0 不是不是正常播放状态
        {
            if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE)
            {
                GUI_KillTimer(msg->h_deswin, ebook_timeid);
            }

            if(reg_ebook_para)
            {
                reg_ebook_para->switch_time = 0; 		//注册表记录
            }

            ebook_ctrl->auto_time = 0 ; 	//停止自动播放
        }
        else
        {
            __u32 ebook_auto_time;

            switch(ebook_ctrl->auto_time )
            {
            case 0:
                ebook_ctrl->auto_time  = 10 * 100; //开始不是自动播放模式的话，按下enter键后，默认的自动播放速度为10s
                ebook_auto_time = 10;
                break;
            case 1000:
                ebook_ctrl->auto_time  = 10 * 100;
                ebook_auto_time = 10;
                break;
            case 2000:
                ebook_ctrl->auto_time  = 20 * 100;
                ebook_auto_time = 20;
                break;
            case 3000:
                ebook_ctrl->auto_time  = 30 * 100;
                ebook_auto_time = 30;
                break;
            default:
                ebook_ctrl->auto_time = 10 * 100;
                ebook_auto_time = 10;
                break;
            }

            if(reg_ebook_para)
            {
                reg_ebook_para->switch_time = ebook_auto_time;
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE)
                GUI_ResetTimer(msg->h_deswin, ebook_timeid, ebook_ctrl->auto_time, 0);
            else
                GUI_SetTimer(msg->h_deswin, ebook_timeid, ebook_ctrl->auto_time, 0);
        }
        break;
    }
    case CMD_PAGE_SET_SET:
    {
        ebook_mset_para_t mset_para;
        RECT rect;

        ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();

        GUI_KillTimer(msg->h_deswin,	 ebook_timeid);

        /*停止长字符串 滚动*/
        {
            __gui_msg_t mymsg;
            __s32	ret ;
            mymsg.id = COM_CMD_STOP_LONG_STRING;
            mymsg.h_deswin = ebook_ctrl->h_page;
            ret = GUI_SendMessage(&mymsg);
            if( EPDK_FAIL == ret )
            {
                __msg(" send stop long string message fail\n");

            }
            else
            {

                __msg(" send stop long string message ok\n");

            }
        }

        if (ebook_ctrl->pageLyr)
            //GUI_LyrWinSetSta(ebook_ctrl->pageLyr, GUI_LYRWIN_STA_SUSPEND);
            GUI_LyrWinSetSta(ebook_ctrl->pageLyr, GUI_LYRWIN_STA_SLEEP);
        rect.x = ebook_uipara->mset_lyr.x;
        rect.y = ebook_uipara->mset_lyr.y;
        rect.width = ebook_uipara->mset_lyr.w;
        rect.height = ebook_uipara->mset_lyr.h;
        ebook_ctrl->msetlyr = ebook_layer_create(&rect);

        mset_para.layer = ebook_ctrl->msetlyr;
        mset_para.mset_font = ebook_ctrl->ebook_font;
        mset_para.focus_id = ebook_ctrl->mset_last_focus_id;
        ebook_ctrl->h_mset = ebook_mset_win_create(msg->h_deswin, &mset_para);
        GUI_WinSetFocusChild(ebook_ctrl->h_mset);
        GUI_LyrWinSetSta(ebook_ctrl->msetlyr, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSetTop(ebook_ctrl->msetlyr);
        sset_win_create(msg, ebook_ctrl->sset_last_main_id);
        break;
    }
    default:
        break;
    }
}
/*

*/
static __s32 ebook_mset_proc(__gui_msg_t *msg)
{
    __gui_msg_t setmsg;
    ebook_ctrl_t     *ebook_ctrl;;

    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr( msg->h_deswin);

    switch(HIWORD(msg->dwAddData1))
    {
    case CMD_EBOOK_MSET_SW_ITEM:
    {
        sset_win_create(msg, msg->dwAddData2);
        break;
    }
    case CMD_EBOOK_MSET_UP:
    {
        setmsg.id = MSG_EBOOK_SSET_UP;
        setmsg.h_deswin = ebook_ctrl->h_sset;
        GUI_SendMessage(&setmsg);
        break;
    }
    case CMD_EBOOK_MSET_DOWN:
    {
        setmsg.id = MSG_EBOOK_SSET_DOWN;
        setmsg.h_deswin = ebook_ctrl->h_sset;
        GUI_SendMessage(&setmsg);
        break;
    }

    case CMD_EBOOK_MSET_LEFT:
    {
        setmsg.id = MSG_EBOOK_SSET_LEFT;
        setmsg.h_deswin = ebook_ctrl->h_sset;
        GUI_SendMessage(&setmsg);
        break;
    }
    case CMD_EBOOK_MSET_RIGHT:
    {
        setmsg.id = MSG_EBOOK_SSET_RIGHT;
        setmsg.h_deswin = ebook_ctrl->h_sset;
        GUI_SendMessage(&setmsg);
        break;
    }
    case CMD_EBOOK_MSET_ENTER:
    {
        setmsg.id = MSG_EBOOK_SSET_ENTER;
        setmsg.h_deswin = ebook_ctrl->h_sset;
        GUI_SendMessage(&setmsg);
        break;
    }
    case CMD_EBOOK_MSET_RECORD_LAST_FOCUS:
    {
        ebook_ctrl->mset_last_focus_id = msg->dwAddData2;
        break;
    }
    case CMD_EBOOK_MSET_CLOSE:
    {
        if (ebook_ctrl->ssetlyr)
            GUI_LyrWinDelete(ebook_ctrl->ssetlyr);
        ebook_ctrl->ssetlyr = 0;

        if (ebook_ctrl->msetlyr)
            GUI_LyrWinDelete(ebook_ctrl->msetlyr);
        ebook_ctrl->msetlyr = 0;

        GUI_LyrWinSetSta(ebook_ctrl->pageLyr, GUI_LYRWIN_STA_ON);//关闭菜单场景后，把page图层打开
        GUI_LyrWinSetTop(ebook_ctrl->pageLyr);
        GUI_WinSetFocusChild(ebook_ctrl->h_page);

        /*开始长字符串 滚动*/
        {
            __gui_msg_t mymsg;
            __s32 ret ;
            mymsg.id = COM_CMD_START_LONG_STRING;
            mymsg.h_deswin = ebook_ctrl->h_page;
            ret = GUI_SendMessage( &mymsg )  ;
            if( EPDK_FAIL == ret )
            {

                __msg("send long string start message fai\n");
            }
            else
            {

                __msg("send long string start messageok\n");
            }
        }
        break;
    }
    default:
        break;
    }

    return EPDK_OK ;
}

static __s32 ebook_sset_proc(__gui_msg_t *msg)
{
    __gui_msg_t setmsg;
    ebook_ctrl_t     *ebook_ctrl;;

    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr( msg->h_deswin);

    switch(HIWORD(msg->dwAddData1))
    {
    case CMD_EBOOK_SSET_BGMUSIC:
    {
        __msg("msg->dwAddData2 = %d\n " , msg->dwAddData2 );
        if(reg_ebook_para)
        {
            reg_ebook_para->bg_music_switch = msg->dwAddData2;
        }
        if(msg->dwAddData2)
        {
            __msg(" send message to root to  open bg music \n");
            Ebook_cmd2parent(msg->h_deswin, OPEN_BG_MUSIC, 0, 0);//打开背景音乐
        }
        else
        {
            __msg(" send message to root to  close bg music \n");
            Ebook_cmd2parent(msg->h_deswin, CLOSE_BG_MUSIC, 0, 0);//关闭背景音乐
            __here__ ;
        }
        __here__;
        /*更新页数*/
        {
            __gui_msg_t setmsg;
            setmsg.id = COM_CMD_UPDATE_PAGE;
            setmsg.h_deswin = ebook_ctrl->h_page;
            setmsg.dwAddData1 = ebook_ctrl->cur_page;
            GUI_SendMessage(&setmsg);
        }
    }
    break;
    case CMD_EBOOK_SSET_AUTO:
    {
        __u32 ebook_auto_time;

        switch (msg->dwAddData2)
        {
        case AUTO_TIME_FAST:
            ebook_ctrl->auto_time = 10 * 100;
            ebook_auto_time = 10;
            break;
        case AUTO_TIME_MIDDLE:
            ebook_ctrl->auto_time = 20 * 100;
            ebook_auto_time = 20;
            break;
        case AUTO_TIME_SLOW:
            ebook_ctrl->auto_time = 30 * 100;
            ebook_auto_time = 30;
            break;
        default:
        {
            if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE)
                GUI_KillTimer(msg->h_deswin,  ebook_timeid );

            ebook_auto_time = 0;
            setmsg.id = COM_CMD_SET_PLAY_STATUS;	//设置暂停，播放状态
            setmsg.h_deswin = ebook_ctrl->h_page;
            setmsg.dwAddData1 = 0;		//不自动播放
            GUI_SendMessage(&setmsg);


            /*更新页数*/
            {
                __gui_msg_t setmsg;
                setmsg.id = COM_CMD_UPDATE_PAGE;
                setmsg.h_deswin = ebook_ctrl->h_page;
                setmsg.dwAddData1 = ebook_ctrl->cur_page;
                GUI_SendMessage(&setmsg);
            }
            return EPDK_OK;
        }
        }

        if(reg_ebook_para)
        {
            reg_ebook_para->switch_time = ebook_auto_time;
        }

        if(GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE)
            GUI_ResetTimer(msg->h_deswin, ebook_timeid, ebook_ctrl->auto_time , NULL);
        else
            GUI_SetTimer(msg->h_deswin, ebook_timeid, ebook_ctrl->auto_time , NULL);

        setmsg.id = COM_CMD_SET_PLAY_STATUS;	//设置暂停，播放状态
        setmsg.h_deswin = ebook_ctrl->h_page;
        setmsg.dwAddData1 = 1;		//自动播放
        GUI_SendMessage(&setmsg);


        /*更新页数*/
        {
            __gui_msg_t setmsg;
            setmsg.id = COM_CMD_UPDATE_PAGE;
            setmsg.h_deswin = ebook_ctrl->h_page;
            setmsg.dwAddData1 = ebook_ctrl->cur_page;
            GUI_SendMessage(&setmsg);
        }
    }
    break;
    case CMD_EBOOK_SSET_TXTCOLOR:
    {
        __s32 color;

        switch(msg->dwAddData2)
        {
        case BOOK_COLOR_BLACK:
            color = APP_COLOR_BLACK;
            break;
        case BOOK_COLOR_WHITE:
            color = APP_COLOR_WHITE;
            break;
        case BOOK_COLOR_YELLOW:
            color = APP_COLOR_YELLOW;
            break;
        default:
            color = APP_COLOR_WHITE;
            break;
        }
        if(reg_ebook_para)
        {
            reg_ebook_para->text_colour = color;
        }
        MBOOK_Decode_SetFontColor(ebook_ctrl->mbook, color);

        /*更新页数*/
        {
            __gui_msg_t setmsg;
            setmsg.id = COM_CMD_UPDATE_PAGE;
            setmsg.h_deswin = ebook_ctrl->h_page;
            setmsg.dwAddData1 = ebook_ctrl->cur_page;
            GUI_SendMessage(&setmsg);
        }
    }
    break;
    case CMD_EBOOK_SSET_GOTO:
    {
        ebook_ctrl->cur_page = msg->dwAddData2;
        MBOOK_Decode_SeekPage(ebook_ctrl->mbook, ebook_ctrl->cur_page);

        /*更新页数*/
        {
            __gui_msg_t setmsg;
            setmsg.id = COM_CMD_UPDATE_PAGE;
            setmsg.h_deswin = ebook_ctrl->h_page;
            setmsg.dwAddData1 = ebook_ctrl->cur_page;
            GUI_SendMessage(&setmsg);
        }
    }
    break;
    case CMD_EBOOK_SSET_LR_CTRL:
    {
        setmsg.id = MSG_EBOOK_MSET_LR_ADJ_SSET;
        setmsg.h_deswin = ebook_ctrl->h_mset;
        setmsg.dwAddData1 = msg->dwAddData2;
        GUI_SendMessage(&setmsg);
    }
    break;
    case CMD_EBOOK_SSET_UPDATA_PAGE:
    {
        setmsg.id = COM_CMD_UPDATE_PAGE;
        setmsg.h_deswin = ebook_ctrl->h_page;
        setmsg.dwAddData1 = msg->dwAddData2;
        GUI_SendMessage(&setmsg);
    }
    break;
    case MSG_EBOOK_SSET_RECORD_LAST_FOCUS_ID:
    {
        ebook_ctrl->sset_last_main_id = msg->dwAddData2;
        ebook_ctrl->sset_last_focus_id = msg->dwReserved;
        break;
    }
    default:
        break;
    }
    __here__ ;
    return EPDK_OK;
}

static __s32 ebook_tips_proc(__gui_msg_t *msg)
{
    ebook_ctrl_t     *ebook_ctrl;

    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr( msg->h_deswin);

    switch(HIWORD(msg->dwAddData1))
    {
    case TIPS_YES:
    {
        H_LYR tips_win_lyr;
        tips_win_lyr = GUI_WinGetLyrWin(ebook_ctrl->h_tips);
        GUI_LyrWinDelete(tips_win_lyr);
        ebook_ctrl->tipsLyr = NULL;
        mbook_on_create(msg);
        break;
    }
    case TIPS_NO:
    {
        H_LYR tips_win_lyr;
        tips_win_lyr = GUI_WinGetLyrWin(ebook_ctrl->h_tips);
        GUI_LyrWinDelete(tips_win_lyr);
        ebook_ctrl->tipsLyr = NULL;
        ebook_ctrl->book_bkpoint.page_no = 0;
        mbook_on_create(msg);
        break;
    }
    default:
        break;
    }
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               _MainProc
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

static __s32 _app_ebook_Proc(__gui_msg_t  *msg)
{
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        ebook_ctrl_t *ebook_ctrl;
        __msg("******ebook create begin*******\n");

        ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
        reg_ebook_para = dsk_reg_get_para_by_app(REG_APP_EBOOK);
        com_set_palette_by_id(ID_EBOOK_WKM_BMP);	//设置调色板
        {
            //背景色，文字编辑框底色
            __u32 color[] = {BEETLES_BLACK_COLOR, BEETLES_GRAY_COLOR};
            com_set_palette_color_by_index(0xF0, color, BEETLES_TBL_SIZE(color));
        }
        /*加载断点信息*/
        {
            ebook_tip_para_t tips_para;
            __s32 ret;
            RECT rect;

            __here__;
            ret = mbook_load_breakpoint_info(&ebook_ctrl->book_bkpoint);	//获取断点信息

            __here__;
            if(ret != 0)	//不存在断点信息
            {

                __msg("mbook_load_breakpoint_info fail or there is no breakpoint info........\n");
                ebook_ctrl->book_bkpoint.page_no = 0;
                mbook_on_create(msg);
                __here__;
            }
            else
            {

                __msg("mbook_load_breakpoint_info succeed\n");
                if(ebook_ctrl->book_bkpoint.page_no > 1)
                {
                    ebook_uipara_t *ebook_uipara;

                    ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();
                    rect.x = ebook_uipara->tips_lyr.x;
                    rect.y = ebook_uipara->tips_lyr.y;
                    rect.width = ebook_uipara->tips_lyr.w;
                    rect.height = ebook_uipara->tips_lyr.h;
                    __here__;
                    ebook_ctrl->tipsLyr = ebook_tips_layer_create(&rect);//创建tips图层

                    __here__;
                    tips_para.font = ebook_ctrl->ebook_font;
                    tips_para.layer = ebook_ctrl->tipsLyr;

                    __here__;
                    ebook_ctrl->h_tips = ebook_tips_win_create(msg->h_deswin, &tips_para);

                    __here__;
                    GUI_WinSetFocusChild(ebook_ctrl->h_tips);
                }
                else
                {

                    ebook_ctrl->book_bkpoint.page_no = 0;

                    __here__;
                    mbook_on_create(msg);
                    __here__;
                }
            }
        }

        __msg("*********ebook create end************\n");
        return 0;
    }
    case DSK_MSG_FS_PART_PLUGOUT:
    {
        __bool 	to_exist;//是否退出
        HRATNPL	h_rat_npl;
        __s32	npl_index;
        char 	file_path[768] = {0};
        char 	diskname[2] = {0};
        ebook_ctrl_t *ebook_ctrl;
        ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
        if (!ebook_ctrl)
        {
            __msg("ebook_ctrl is null...\n");
            return EPDK_OK;
        }

        diskname[0] = (char)(msg->dwAddData2);
        diskname[1] = 0;

        to_exist = 1;//默认需要退出
        h_rat_npl = rat_npl_open(RAT_MEDIA_TYPE_EBOOK);
        npl_index = rat_npl_get_cur(h_rat_npl);
        rat_npl_index2file(h_rat_npl, npl_index, file_path);
        rat_npl_close(h_rat_npl);
        if(0 != eLIBs_strnicmp(file_path, diskname, 1))
        {
            to_exist = 0;//拔的卡不是当前播放文件所在卡，不需要退出
        }

        __msg("******to_exist=%d******\n", to_exist);
        if(1 == to_exist)//需要退出
        {
            __here__;
            Ebook_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
        }
        __here__;

        return EPDK_OK;
    }
    case DSK_MSG_HOME:
    case DSK_MSG_KILL:
    case GUI_MSG_CLOSE:
    {
        Ebook_cmd2parent(msg->h_deswin, SWITCH_TO_EXPLORER, 0, 0);
        break;
    }
    case GUI_MSG_DESTROY:
    {
        ebook_ctrl_t *ebook_ctrl;
        ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

        __msg("*****ebook destroy begin********\n");

        if(!ebook_ctrl)
        {

            __err("ebook_ctrl is null ......\n");
            return EPDK_FAIL;
        }

        __here__;
        if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE)
        {
            GUI_KillTimer(msg->h_deswin, ebook_timeid);
        }
        __here__;

        {
            HRATNPL		h_rat_npl;
            __s32		npl_index;
            char 		file_path[768] = {0};
            char 		*p_filename;

            h_rat_npl = rat_npl_open(RAT_MEDIA_TYPE_EBOOK);
            npl_index = rat_npl_get_cur(h_rat_npl);
            rat_npl_index2file(h_rat_npl, npl_index, file_path);
            rat_npl_close(h_rat_npl);
            p_filename = file_path;//eLIBs_strchrlast(file_path,'\\') + 1;
            dsk_reg_save_cur_play_info(REG_APP_EBOOK, npl_index, p_filename, ebook_ctrl->root_type);
        }

        __msg("**********before mbook_on_delete********\n");
        mbook_on_delete(msg);
        __here__;
        //dsk_reg_flush();
        __msg("*****ebook destroy end********\n");
        return EPDK_OK;
    }
    case GUI_MSG_COMMAND:
    {
        if(LOWORD(msg->dwAddData1) == EBOOK_PAGE_ID)
        {
            page_info_proc(msg);
        }
        else if(LOWORD(msg->dwAddData1) == EBOOK_MSET_ID)
        {
            ebook_mset_proc(msg);
        }
        else if(LOWORD(msg->dwAddData1) == EBOOK_SSET_ID)
        {
            ebook_sset_proc(msg);
        }
        else if(LOWORD(msg->dwAddData1) == EBOOK_TIPS_ID)
        {
            ebook_tips_proc(msg);
        }
        return EPDK_OK;
    }
    case GUI_MSG_KEY:
        break;
    case GUI_MSG_TIMER:
    {
        ebook_ctrl_t     *ebook_ctrl;
        __gui_msg_t		setmsg;

        ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr( msg->h_deswin);
        ebook_ctrl->cur_page = MBOOK_Decode_ShowNext(ebook_ctrl->mbook);
        setmsg.id = COM_CMD_UPDATE_PAGE;
        setmsg.h_deswin = ebook_ctrl->h_page;
        setmsg.dwAddData1 = ebook_ctrl->cur_page;
        GUI_SendMessage(&setmsg);
        break;
    }
    default:
        break;
    }
    return 	GUI_ManWinDefaultProc(msg);

}

__s32 app_ebook_create(root_para_t *para)
{
    __gui_manwincreate_para_t create_info;
    ebook_ctrl_t *ebook_ctrl;

    __inf("****************************************************************************************\n");
    __inf("********  enter ebook app  **************\n");
    __inf("****************************************************************************************\n");

#if EPDK_USE_EBOOK_TEST_TVIN
    gscene_bgd_set_state(BGD_STATUS_HIDE);
    esEXEC_PCreate(EPDK_ROOTFS_PATH"\\test_app.axf", NULL, 0, NULL);
    while(1)
    {
        esKRNL_TimeDly(100);
    }
#endif
    //__wait__;

    ebook_ctrl = (ebook_ctrl_t *)My_Balloc(sizeof(ebook_ctrl_t));
    if(!ebook_ctrl)
    {

        __msg("memory balloc fail.........\n");
        return EPDK_FAIL;
    }
    eLIBs_memset(ebook_ctrl, 0, sizeof(ebook_ctrl_t));

    ebook_ctrl->ebook_font = para->font;
    ebook_ctrl->root_type = para->root_type;

    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));

    create_info.name            = APP_EBOOK;
    create_info.hParent         = para->h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_app_ebook_Proc);
    create_info.attr            = (void *)ebook_ctrl;
    create_info.id			= APP_EBOOK_ID;
    create_info.hHosting        = NULL;

    return(GUI_ManWinCreate(&create_info));
}

