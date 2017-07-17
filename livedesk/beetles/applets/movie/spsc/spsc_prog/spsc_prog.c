/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: spsc_prog.c
* By      	:
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/
#include "spsc_prog.h"
#include "common/common.h"

typedef enum
{
    movie_spsc_prog_pause = 0,
    movie_spsc_prog_play,
    movie_spsc_prog_playsta_num
} movie_spsc_prog_playsta_t;

//定义prog子场景的ui类型
#define MOVIE_SPSC_PROG_UI_TYPE_PROG		0x01
#define MOVIE_SPSC_PROG_UI_TYPE_CUR_TIME	0x02
#define MOVIE_SPSC_PROG_UI_TYPE_TOTAL_TIME	0x04
#define MOVIE_SPSC_PROG_UI_TYPE_ICON		0x08


//prog的所有ui类型
#define MOVIE_SPSC_PROG_UI_TYPE_ALL	(MOVIE_SPSC_PROG_UI_TYPE_PROG\
|MOVIE_SPSC_PROG_UI_TYPE_CUR_TIME\
|MOVIE_SPSC_PROG_UI_TYPE_TOTAL_TIME\
|MOVIE_SPSC_PROG_UI_TYPE_ICON)

//除了图标和总时间以外的其它ui类型
#define MOVIE_SPSC_PROG_UI_TYPE_OTHER		(MOVIE_SPSC_PROG_UI_TYPE_ALL&(~MOVIE_SPSC_PROG_UI_TYPE_ICON))

typedef union
{
    struct
    {
        __s32    hour;
        __s32    minute;
        __s32    second;
    } data1;

    __s32 data2[3];
} spsc_prog_cuckoo_time_t;

static void __time2time( __u32 ms, spsc_prog_cuckoo_time_t *format_time )
{
    __u32  total_minute;
    __u32 second_time;

    second_time = ms / 1000;
    total_minute = second_time / 60;

    format_time->data1.second = second_time % 60;
    format_time->data1.minute = total_minute % 60;
    format_time->data1.hour   = total_minute / 60;
}

typedef struct
{
    //输入参数
    H_WIN hparent;
    __s32 scene_id;
    H_LYR hlyr;

    //内部参数
    H_WIN hfrm;
    movie_spsc_prog_playsta_t playsta;	//图标状态
    __s32 cur_time_index;//当画当前时间时亮选的位置（时、分、秒）
    spsc_prog_cuckoo_time_t cur_time;//输入的设置时间
    __u8 prog_timmer_id;
} movie_spsc_prog_scene_t;

/***********************************************************************************************************
	建立图层
************************************************************************************************************/
static H_LYR __spsc_prog_8bpp_layer_create(RECT *rect, __s32 pipe)
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
        "sub menu layer",
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

    lstlyr.scn_win.x		= rect->x;
    lstlyr.scn_win.y		= rect->y;
    lstlyr.scn_win.width  	= rect->width;
    lstlyr.scn_win.height 	= rect->height;

    lstlyr.pipe = pipe;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;

    layer = GUI_LyrWinCreate(&lyrcreate_info);
    if( !layer )
    {
        __err("app bar layer create error !\n");
    }

    return layer;
}

static __s32 __spsc_prog_init_ui(void)
{
    movie_spsc_prog_uipara_t *ui_para;

    ui_para = movie_spsc_prog_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_prog_get_uipara fail...\n");
        return EPDK_FAIL;
    }

    {
        int i;
        int j;

        for (i = 0 ; i < movie_spsc_prog_icon_num ; i++)
        {
            for (j = 0 ; j < MOVIE_SPSC_PROG_MAX_ICON_RES_NUM ; j++)
            {
                if (ui_para->uipara_icon[i].res_id[j])
                {
                    ui_para->uipara_icon[i].res_hdl[j] = dsk_theme_open(ui_para->uipara_icon[i].res_id[j]);
                    if (!ui_para->uipara_icon[i].res_hdl[j])
                    {
                        __msg("dsk_theme_open fail...\n");
                    }
                }
            }
        }
    }

    return EPDK_OK;
}

static __s32 __spsc_prog_deinit_ui(void)
{
    movie_spsc_prog_uipara_t *ui_para;

    ui_para = movie_spsc_prog_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_prog_get_uipara fail...\n");
        return EPDK_FAIL;
    }

    {
        int i;
        int j;

        for (i = 0 ; i < movie_spsc_prog_icon_num ; i++)
        {
            for (j = 0 ; j < MOVIE_SPSC_PROG_MAX_ICON_RES_NUM ; j++)
            {
                if (ui_para->uipara_icon[i].res_hdl[j])
                {
                    dsk_theme_close(ui_para->uipara_icon[i].res_hdl[j]);
                    ui_para->uipara_icon[i].res_hdl[j] = NULL;
                }
            }
        }
    }

    return EPDK_OK;
}

static __s32 __spsc_prog_init_internal_para(movie_spsc_prog_scene_t *scene_para)
{
    if (NULL == scene_para)
    {
        __msg("invalid para...\n");
        return EPDK_FAIL;
    }

    scene_para->cur_time_index = -1;
    scene_para->prog_timmer_id = 0x14;
    eLIBs_memset(&scene_para->cur_time, 0, sizeof(spsc_prog_cuckoo_time_t));

    return EPDK_OK;
}
//jh_time
static __s32 __spsc_prog_update_spsc_prog_ui(movie_spsc_prog_scene_t *scene_para
        , __u32 ui_type)
{
    movie_spsc_prog_uipara_t *ui_para;

    ui_para = movie_spsc_prog_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_prog_get_uipara fail...\n");
        return EPDK_FAIL;
    }

    if (!scene_para)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (!scene_para->hlyr)
    {
        __msg("scene_para->hlyr is null..., do not paint\n");
        return EPDK_FAIL;
    }
    if (GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr))
    {
        __msg("GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
        return EPDK_FAIL;
    }
    GUI_LyrWinSel(scene_para->hlyr);

    com_memdev_create(scene_para->hlyr);

    //画图标
    __here__;
    if(ui_type & MOVIE_SPSC_PROG_UI_TYPE_ICON)
    {
        void *pbmp;
        __s32 focus;
        GUI_RECT gui_rect1;
        __here__;
        if (!ui_para->uipara_icon[movie_spsc_prog_icon_playsta].res_hdl[scene_para->playsta])
        {
            __msg("ui_para->uipara_icon[movie_spsc_prog_icon_playsta].res_hdl[scene_para->playsta] is null...\n");
            com_memdev_delete();
            return EPDK_FAIL;
        }
        pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon[movie_spsc_prog_icon_playsta].res_hdl[scene_para->playsta]);
        if (!pbmp)
        {
            __msg("pbmp is null...\n");
            com_memdev_delete();
            return EPDK_FAIL;
        }

        __log("------jh_dbg20170224-------17\n");
        gui_rect1.x0 = ui_para->uipara_icon[movie_spsc_prog_icon_playsta].x;
        gui_rect1.y0 = ui_para->uipara_icon[movie_spsc_prog_icon_playsta].y;
        gui_rect1.x1 = gui_rect1.x0 + ui_para->uipara_icon[movie_spsc_prog_icon_playsta].w;
        gui_rect1.y1 = gui_rect1.y0 + ui_para->uipara_icon[movie_spsc_prog_icon_playsta].h;
        GUI_ClearRectEx(&gui_rect1);
        GUI_BMP_Draw(pbmp, ui_para->uipara_icon[movie_spsc_prog_icon_playsta].x, ui_para->uipara_icon[movie_spsc_prog_icon_playsta].y);
    }

    //当前时间
    __here__;
    if(ui_type & MOVIE_SPSC_PROG_UI_TYPE_CUR_TIME)
    {
        char str_val[32] = {0};
        GUI_RECT gui_rect;
        spsc_prog_cuckoo_time_t format_time;

        __u32 t;

        __here__;
        if (-1 == scene_para->cur_time_index) //普通更新，从robin获取时间
        {
            t = robin_get_cur_time();
            __time2time(t, &format_time);
        }
        else if(scene_para->cur_time_index >= 0 || scene_para->cur_time_index < 3)//设置时间，从外部获取时间
        {
            eLIBs_memcpy(&format_time, &scene_para->cur_time, sizeof(spsc_prog_cuckoo_time_t));
        }
        else
        {
            __err("invalid para...\n");
            com_memdev_delete();
            return EPDK_FAIL;
        }

        if (SWFFont)
        {
            GUI_SetFont(SWFFont);
        }
        else
        {
            GUI_SetFont(GUI_GetDefaultFont());
        }

        GUI_SetColor(APP_COLOR_WHITE);
        GUI_UC_SetEncodeUTF8();

        {
            __s32 i;

            __here__;
            for (i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_spsc_prog_cur_time) ; i++)
            {
                if (i == scene_para->cur_time_index)
                {
                    GUI_SetBkColor(0);//亮选该时间位置
                }
                else
                {
                    GUI_SetBkColor(0);
                }
                gui_rect.x0 = ui_para->uipara_spsc_prog_cur_time[i].x;
                gui_rect.y0 = ui_para->uipara_spsc_prog_cur_time[i].y;
                gui_rect.x1 = gui_rect.x0 + ui_para->uipara_spsc_prog_cur_time[i].w;
                gui_rect.y1 = gui_rect.y0 + ui_para->uipara_spsc_prog_cur_time[i].h;
                GUI_ClearRectEx(&gui_rect);
                eLIBs_sprintf(str_val, "%.2d", format_time.data2[i]);
                GUI_DispStringInRect(str_val, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            }
        }

        {
            __s32 i;

            __here__;
            for (i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_spsc_prog_cur_time_dot) ; i++)
            {
                gui_rect.x0 = ui_para->uipara_spsc_prog_cur_time_dot[i].x;
                gui_rect.y0 = ui_para->uipara_spsc_prog_cur_time_dot[i].y;
                gui_rect.x1 = gui_rect.x0 + ui_para->uipara_spsc_prog_cur_time_dot[i].w;
                gui_rect.y1 = gui_rect.y0 + ui_para->uipara_spsc_prog_cur_time_dot[i].h;
                eLIBs_sprintf(str_val, ":");
                GUI_DispStringInRect(str_val, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            }
        }
    }

    //总时间
    __here__;
    if(ui_type & MOVIE_SPSC_PROG_UI_TYPE_TOTAL_TIME)
    {
        char str_val[32] = {0};
        GUI_RECT gui_rect;
        spsc_prog_cuckoo_time_t format_time;

        __u32 t;

        __here__;
        t = robin_get_total_time();

        __time2time(t, &format_time);


        if (SWFFont)
        {
            GUI_SetFont(SWFFont);
        }
        else
        {
            GUI_SetFont(GUI_GetDefaultFont());
        }

        GUI_SetColor(APP_COLOR_WHITE);
        GUI_SetBkColor(0x00);
        GUI_UC_SetEncodeUTF8();

        {
            __s32 i;
            __here__;

            for (i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_spsc_prog_total_time) ; i++)
            {
                gui_rect.x0 = ui_para->uipara_spsc_prog_total_time[i].x;
                gui_rect.y0 = ui_para->uipara_spsc_prog_total_time[i].y;
                gui_rect.x1 = gui_rect.x0 + ui_para->uipara_spsc_prog_total_time[i].w;
                gui_rect.y1 = gui_rect.y0 + ui_para->uipara_spsc_prog_total_time[i].h;
                GUI_ClearRectEx(&gui_rect);
                eLIBs_sprintf(str_val, "%.2d", format_time.data2[i]);
                GUI_DispStringInRect(str_val, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            }
        }

        {
            __s32 i;

            __here__;
            for (i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_spsc_prog_total_time_dot) ; i++)
            {
                gui_rect.x0 = ui_para->uipara_spsc_prog_total_time_dot[i].x;
                gui_rect.y0 = ui_para->uipara_spsc_prog_total_time_dot[i].y;
                gui_rect.x1 = gui_rect.x0 + ui_para->uipara_spsc_prog_total_time_dot[i].w;
                gui_rect.y1 = gui_rect.y0 + ui_para->uipara_spsc_prog_total_time_dot[i].h;
                eLIBs_sprintf(str_val, ":");
                GUI_DispStringInRect(str_val, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            }
        }
    }

    __here__;
    if(ui_type & MOVIE_SPSC_PROG_UI_TYPE_PROG)
    {
        //画进度条背景
        __here__;
        {
            void *pbmp;

            if (!ui_para->uipara_icon[movie_spsc_prog_icon_prog_bg].res_hdl[0])
            {
                __msg("ui_para->uipara_icon[movie_spsc_prog_icon_prog_bg].res_hdl[0] is null...\n");
                com_memdev_delete();
                return EPDK_FAIL;
            }
            pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon[movie_spsc_prog_icon_prog_bg].res_hdl[0]);
            if (!pbmp)
            {
                __msg("pbmp is null...\n");
                com_memdev_delete();
                return EPDK_FAIL;
            }
            GUI_BMP_Draw(pbmp, ui_para->uipara_icon[movie_spsc_prog_icon_prog_bg].x, ui_para->uipara_icon[movie_spsc_prog_icon_prog_bg].y);
        }

        //画进度条
        __here__;
        {
            void *pbmp;

            __u32 cur_time;
            __u32 total_time;

            cur_time = robin_get_cur_time();
            total_time = robin_get_total_time();
            if(cur_time > total_time)
            {
                cur_time = total_time;
            }

            if (!ui_para->uipara_icon[movie_spsc_prog_icon_prog_cursor].res_hdl[0])
            {
                __msg("ui_para->uipara_icon[movie_spsc_prog_icon_prog_cursor].res_hdl[0] is null...\n");
                com_memdev_delete();
                return EPDK_FAIL;
            }
            pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon[movie_spsc_prog_icon_prog_cursor].res_hdl[0]);
            if (!pbmp)
            {
                __msg("pbmp is null...\n");
                com_memdev_delete();
                return EPDK_FAIL;
            }

            {
                __s32 i;
                __s32 n;

                __here__;
                __msg("cur_time=%d total_time=%d\n", cur_time, total_time);
                if (cur_time <= 0 || total_time <= 0)
                {
                    n = 0;
                }
                else
                {
                    __here__;
                    n = (ui_para->uipara_icon[movie_spsc_prog_icon_prog_bg].w * cur_time)
                        / (ui_para->uipara_icon[movie_spsc_prog_icon_prog_cursor].w * total_time);
                    __here__;
                }

                __msg("n= %d \n", n);
                for (i = 0 ; i < n ; i++)
                {
                    __here__;
                    GUI_BMP_Draw(pbmp, ui_para->uipara_icon[movie_spsc_prog_icon_prog_cursor].x + i * ui_para->uipara_icon[movie_spsc_prog_icon_prog_cursor].w
                                 , ui_para->uipara_icon[movie_spsc_prog_icon_prog_cursor].y);
                    __here__;
                }
            }
        }
    }
    __here__;

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    com_memdev_delete();

    return EPDK_OK;
}

static __s32 __spsc_prog_install_prog_timmer(movie_spsc_prog_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (!GUI_IsTimerInstalled(scene_para->hfrm, scene_para->prog_timmer_id))
    {
        GUI_SetTimer(scene_para->hfrm, scene_para->prog_timmer_id
                     , 400 * 0.25, NULL); //0.25秒
    }
    else
    {
        __msg("timmer already install...\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __spsc_prog_uninstall_prog_timmer(movie_spsc_prog_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(scene_para->hfrm, scene_para->prog_timmer_id))
    {
        GUI_KillTimer(scene_para->hfrm, scene_para->prog_timmer_id);
    }

    return EPDK_OK;
}

static __s32 __spsc_prog_proc(__gui_msg_t *msg)
{
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        movie_spsc_prog_scene_t *scene_para;

        __msg("__spsc_prog_proc GUI_MSG_CREATE begin\n");

        scene_para = GUI_WinGetAttr(msg->h_deswin);
        if (!scene_para)
        {
            __err("invalid para...\n");
            return EPDK_FAIL;
        }

        scene_para->hfrm = msg->h_deswin;//必须在此初始化，因为窗口创建未返回，否则该值仍然是空

        __spsc_prog_init_internal_para(scene_para);

        __spsc_prog_init_ui();

        __spsc_prog_update_spsc_prog_ui(scene_para, MOVIE_SPSC_PROG_UI_TYPE_ALL);

        __spsc_prog_install_prog_timmer(scene_para);

        __msg("__spsc_prog_proc GUI_MSG_CREATE end\n");
        return EPDK_OK;
    }
    case GUI_MSG_DESTROY:
    {
        movie_spsc_prog_scene_t *scene_para;

        __msg("__spsc_prog_proc GUI_MSG_DESTROY begin\n");

        scene_para = GUI_WinGetAttr(msg->h_deswin);
        if (!scene_para)
        {
            __err("invalid para...\n");
            return EPDK_FAIL;
        }

        __msg("before __spsc_prog_uninstall_prog_timmer\n");
        __spsc_prog_uninstall_prog_timmer(scene_para);
        __msg("after __spsc_prog_uninstall_prog_timmer\n");

        __msg("before __spsc_prog_deinit_ui\n");
        __spsc_prog_deinit_ui();
        __msg("after __spsc_prog_deinit_ui\n");

        __msg("__spsc_prog_proc GUI_MSG_DESTROY end\n");
        return EPDK_OK;
    }
    case GUI_MSG_KEY:
    {
        static __s32 last_key = -1;
        movie_spsc_prog_scene_t *scene_para;
        ///////jh1
        //return EPDK_OK;

        __msg("__spsc_prog_proc GUI_MSG_KEY begin\n");
        __log("--------log_7--------\n");
        scene_para = GUI_WinGetAttr(msg->h_deswin);
        if (!scene_para)
        {
            __err("invalid para...\n");
            return EPDK_FAIL;
        }

        if (KEY_UP_ACTION == msg->dwAddData2)
        {
            if (GUI_MSG_KEY_UP == last_key
                    || GUI_MSG_KEY_VADD == last_key)
            {
                scene_para->cur_time_index++;
                if (scene_para->cur_time_index >= 3)
                {
                    scene_para->cur_time_index = 0;
                }
                __spsc_prog_update_spsc_prog_ui(scene_para, MOVIE_SPSC_PROG_UI_TYPE_CUR_TIME);
            }
            else if (GUI_MSG_KEY_VDEC == last_key)
            {
                scene_para->cur_time_index--;
                if (scene_para->cur_time_index < 0)
                {
                    scene_para->cur_time_index = 2;
                }
                __spsc_prog_update_spsc_prog_ui(scene_para, MOVIE_SPSC_PROG_UI_TYPE_CUR_TIME);
            }
            else if (GUI_MSG_KEY_LEFT == last_key)
            {
                if (scene_para->cur_time_index < 0 || scene_para->cur_time_index >= 3)
                {
                    __err("invalid para...\n");
                    return EPDK_OK;
                }
                scene_para->cur_time.data2[scene_para->cur_time_index]--;
                if (scene_para->cur_time.data2[scene_para->cur_time_index] < 0)
                {
                    scene_para->cur_time.data2[scene_para->cur_time_index] = 59;
                }
                __msg("before __spsc_prog_update_spsc_prog_ui\n");
                __spsc_prog_update_spsc_prog_ui(scene_para, MOVIE_SPSC_PROG_UI_TYPE_CUR_TIME);
                __msg("after __spsc_prog_update_spsc_prog_ui\n");
            }
            else if (GUI_MSG_KEY_RIGHT == last_key)
            {
                if (scene_para->cur_time_index < 0 || scene_para->cur_time_index >= 3)
                {
                    __err("invalid para...\n");
                    return EPDK_OK;
                }
                scene_para->cur_time.data2[scene_para->cur_time_index]++;
                if (scene_para->cur_time.data2[scene_para->cur_time_index] > 59)
                {
                    scene_para->cur_time.data2[scene_para->cur_time_index] = 0;
                }
                __msg("before __spsc_prog_update_spsc_prog_ui\n");
                __spsc_prog_update_spsc_prog_ui(scene_para, MOVIE_SPSC_PROG_UI_TYPE_CUR_TIME);
                __msg("after __spsc_prog_update_spsc_prog_ui\n");
            }
            else if (GUI_MSG_KEY_ENTER == last_key)
            {
                __s32 i;
                __u32 t;

                t = 0;
                __log("------jh_dbg20170224-------1\n");
                for (i = 0 ; i < BEETLES_TBL_SIZE(scene_para->cur_time.data2) ; i++)
                {
                    t *= 60;
                    t += scene_para->cur_time.data2[i];
                }

                t *= 1000;//ms

                __here__;
                scene_para->cur_time_index = -1;//设置当前选中时间的位置为无效
                __spsc_prog_update_spsc_prog_ui(scene_para, MOVIE_SPSC_PROG_UI_TYPE_CUR_TIME);//更新当前时间
                __here__;

                movie_cmd2parent(msg->h_deswin, movie_spsc_prog_scene_msg_timeset, (__s32)t, 0);
                __here__;
                __spsc_prog_install_prog_timmer(scene_para);//允许更新时间和进度条
                __here__;
            }
        }
        else
        {
            switch(msg->dwAddData1)
            {
            case GUI_MSG_KEY_LONGLEFT:
            {
#if 0
                if (scene_para->cur_time_index < 0 || scene_para->cur_time_index >= 3)
                {
                    __err("invalid para...\n");
                    return EPDK_OK;
                }
                scene_para->cur_time.data2[scene_para->cur_time_index]--;
                if (scene_para->cur_time.data2[scene_para->cur_time_index] < 0)
                {
                    scene_para->cur_time.data2[scene_para->cur_time_index] = 59;
                }

                __msg("before __spsc_prog_update_spsc_prog_ui\n");
                __spsc_prog_update_spsc_prog_ui(scene_para, MOVIE_SPSC_PROG_UI_TYPE_CUR_TIME);
                __msg("after __spsc_prog_update_spsc_prog_ui\n");
#endif
                break;
            }
            case GUI_MSG_KEY_LONGRIGHT:
            {
#if 0
                if (scene_para->cur_time_index < 0 || scene_para->cur_time_index >= 3)
                {
                    __err("invalid para...\n");
                    return EPDK_OK;
                }
                scene_para->cur_time.data2[scene_para->cur_time_index]++;
                if (scene_para->cur_time.data2[scene_para->cur_time_index] > 59)
                {
                    scene_para->cur_time.data2[scene_para->cur_time_index] = 0;
                }
                __msg("before __spsc_prog_update_spsc_prog_ui\n");
                __spsc_prog_update_spsc_prog_ui(scene_para, MOVIE_SPSC_PROG_UI_TYPE_CUR_TIME);
                __msg("after __spsc_prog_update_spsc_prog_ui\n");
#endif
                break;
            }
            default:
            {
                break;
            }
            }
        }

        if (KEY_UP_ACTION == msg->dwAddData2)
        {
            last_key = -1;
        }
        else
        {
            last_key = msg->dwAddData1;
        }

        __msg("__spsc_prog_proc GUI_MSG_KEY end\n");

        return EPDK_OK;
    }
    case GUI_MSG_TOUCH:
    {
        __msg("__spsc_prog_proc GUI_MSG_TOUCH begin\n");

        __msg("__spsc_prog_proc GUI_MSG_TOUCH end\n");
        return EPDK_OK;
    }
    case GUI_MSG_PAINT:
    {
        movie_spsc_prog_scene_t *scene_para;

        __msg("__spsc_prog_proc GUI_MSG_PAINT begin\n");

        scene_para = GUI_WinGetAttr(msg->h_deswin);
        if (!scene_para)
        {
            __err("invalid para...\n");
            return EPDK_FAIL;
        }

        if (GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr))
        {
            __msg("GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
            return EPDK_OK;
        }

        __spsc_prog_update_spsc_prog_ui(scene_para, MOVIE_SPSC_PROG_UI_TYPE_ALL);

        __msg("__spsc_prog_proc GUI_MSG_PAINT begin\n");

        return EPDK_OK;
    }
    case GUI_MSG_TIMER:
    {
        __s32 ret;
        movie_spsc_prog_scene_t *scene_para;

        __msg("__spsc_prog_proc GUI_MSG_TIMER begin\n");

        scene_para = GUI_WinGetAttr(msg->h_deswin);

        ret = -1;

        if (scene_para)
        {
            if(LOWORD(msg->dwAddData1) == scene_para->prog_timmer_id)
            {
                //	__log("-------------tap\n");
                //__msg("before __spsc_prog_update_spsc_prog_ui\n");
                __spsc_prog_update_spsc_prog_ui(scene_para, MOVIE_SPSC_PROG_UI_TYPE_ALL & 0x07);
                //__msg("after __spsc_prog_update_spsc_prog_ui\n");
                ret = EPDK_OK;
            }
        }
        else
        {
            __err("scene_para is null...\n");
        }

        __msg("__spsc_prog_proc GUI_MSG_TIMER end\n");

        if (-1 == ret)//未处理，交给子场景处理
        {
            break;
        }
        else//已处理
        {
            return EPDK_OK;
        }
    }
    case GUI_MSG_SET_FOCUS:
    {
        movie_spsc_prog_scene_t *scene_para;

        __msg("__spsc_prog_proc GUI_MSG_SET_FOCUS begin\n");

        scene_para = GUI_WinGetAttr(msg->h_deswin);
        if (!scene_para)
        {
            __err("invalid para...\n");
            return EPDK_FAIL;
        }

        __spsc_prog_uninstall_prog_timmer(scene_para);//停止更新时间和进度条

        //初始化设置时间
        {
            __u32 t;

            t = robin_get_cur_time();
            __time2time(t, &scene_para->cur_time);
        }

        scene_para->cur_time_index = 2;//初始化当前选中时间的位置
        __spsc_prog_update_spsc_prog_ui(scene_para, MOVIE_SPSC_PROG_UI_TYPE_CUR_TIME);//画时间

        __msg("__spsc_prog_proc GUI_MSG_SET_FOCUS end\n");

        return EPDK_OK;
    }
    case GUI_MSG_SET_UNFOCUS:
    {
        __msg("__spsc_prog_proc GUI_MSG_SET_UNFOCUS begin\n");

        __msg("__spsc_prog_proc GUI_MSG_SET_UNFOCUS end\n");
        return EPDK_OK;
    }
    case GUI_MSG_CLOSE:
    {
        __msg("__spsc_prog_proc GUI_MSG_CLOSE begin\n");

        __msg("__spsc_prog_proc GUI_MSG_CLOSE end\n");
        break;
    }
    case movie_spsc_prog_scene_set_sta:
    {
        __s32 sta;
        movie_spsc_prog_scene_t *scene_para;

        __msg("__spsc_prog_proc movie_spsc_prog_scene_set_sta begin\n");

        scene_para = GUI_WinGetAttr(msg->h_deswin);
        if (!scene_para)
        {
            __err("invalid para...\n");
            return EPDK_FAIL;
        }

        sta = msg->dwAddData1;

        if(0 == sta)
        {
            scene_para->playsta = 0;
        }
        else
        {
            scene_para->playsta = 1;
        }

        __log("------jh_dbg20170224-------16:%d\n", scene_para->playsta);

        __spsc_prog_update_spsc_prog_ui(scene_para, MOVIE_SPSC_PROG_UI_TYPE_ICON);
        __msg("__spsc_prog_proc movie_spsc_prog_scene_set_sta end\n");

        return EPDK_OK;
    }
    default:
    {
        break;
    }
    }

    return GUI_FrmWinDefaultProc(msg);
}

void *movie_spsc_prog_scene_create(movie_spsc_prog_create_para_t *create_para)
{
    __gui_framewincreate_para_t framewin_para;
    movie_spsc_prog_scene_t *sence_para;
    movie_spsc_prog_uipara_t *ui_para;

    sence_para = (movie_spsc_prog_scene_t *)esMEMS_Balloc(sizeof(movie_spsc_prog_scene_t));
    if (!sence_para)
    {
        __msg("mem not enough...\n");
        return NULL;
    }

    eLIBs_memset(sence_para, 0, sizeof(movie_spsc_prog_scene_t));

    sence_para->hlyr = create_para->hlyr;
    sence_para->hparent = create_para->hparent;
    sence_para->scene_id = create_para->scene_id;

    {
        __cedar_status_t cedar_status;

        cedar_status = robin_get_fsm_status();
        if(CEDAR_STAT_PAUSE == cedar_status)
        {
            sence_para->playsta = movie_spsc_prog_pause;
        }
        else
        {
            sence_para->playsta = movie_spsc_prog_play;
        }
    }

    ui_para = movie_spsc_prog_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        esMEMS_Bfree(sence_para, sizeof(movie_spsc_prog_scene_t));
        sence_para = NULL;
        __msg("movie_spsc_get_uipara fail...\n");
        return NULL;
    }

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name			=	"movie_spsc_prog_win",
                         framewin_para.dwExStyle		= WS_EX_NONE;
    framewin_para.dwStyle		= WS_NONE | WS_VISIBLE;
    framewin_para.spCaption		= NULL;
    framewin_para.hOwner		= NULL;
    framewin_para.id			= sence_para->scene_id;
    framewin_para.hHosting		=  sence_para->hparent;
    framewin_para.FrameWinProc	= (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__spsc_prog_proc);
    framewin_para.rect.x		= ui_para->uipara_frm.x;
    framewin_para.rect.y		= ui_para->uipara_frm.y;
    framewin_para.rect.width	= ui_para->uipara_frm.w;
    framewin_para.rect.height	= ui_para->uipara_frm.h;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red	= 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue	= 0;
    framewin_para.attr			= (void *)sence_para;
    framewin_para.hLayer		= sence_para->hlyr;

    sence_para->hfrm = GUI_FrmWinCreate(&framewin_para);
    if (NULL == sence_para->hfrm)
    {
        esMEMS_Bfree(sence_para, sizeof(movie_spsc_prog_scene_t));
        sence_para = NULL;
        __msg("GUI_FrmWinCreate fail...\n");
        return NULL;
    }

    return sence_para;
}

__s32 movie_spsc_prog_scene_delete(void *handle)
{
    movie_spsc_prog_scene_t *scene_para;

    scene_para = (movie_spsc_prog_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..\n");
        return EPDK_FAIL;
    }

    if (!scene_para->hfrm)
    {
        __err("invalid para..\n");
        return EPDK_FAIL;
    }
    GUI_FrmWinDelete(scene_para->hfrm);

    eLIBs_memset(scene_para, 0, sizeof(movie_spsc_prog_scene_t));//防止重复释放
    esMEMS_Bfree(scene_para, sizeof(movie_spsc_prog_scene_t));

    return EPDK_OK;
}

__s32 movie_spsc_prog_scene_set_focus(void *handle)
{
    movie_spsc_prog_scene_t *scene_para;

    scene_para = (movie_spsc_prog_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..\n");
        return EPDK_FAIL;
    }

    if (!scene_para->hfrm)
    {
        __err("invalid para..\n");
        return EPDK_FAIL;
    }

    GUI_WinSetFocusChild(scene_para->hfrm);

    return EPDK_OK;
}

H_WIN movie_spsc_prog_scene_get_hwnd(void *handle)
{
    movie_spsc_prog_scene_t *scene_para;

    scene_para = (movie_spsc_prog_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..\n");
        return EPDK_FAIL;
    }

    return scene_para->hfrm;
}
