/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: volume.c
* By      	:
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/
#include "volume.h"
#include "common/common.h"
typedef struct
{
    //输入参数
    H_WIN hparent;
    __s32 scene_id;
    __s32 min_val;
    __s32 max_val;
    __s32 cur_val;

    //内部参数
    H_LYR hlyr;
    H_WIN hfrm;
    __u8  hide_timmer_id;
} movie_volume_scene_t;

/***********************************************************************************************************
	建立图层
************************************************************************************************************/
static H_LYR __volume_8bpp_layer_create(RECT *rect, __s32 pipe)
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

static __s32 __volume_init_ui(void)
{
    movie_volume_uipara_t *ui_para;

    ui_para = movie_volume_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_volume_get_uipara fail...\n");
        return EPDK_FAIL;
    }

    {
        __s32 i;

        for (i = 0 ; i < MOVIE_VOLUME_MAX_ICON_RES_NUM ; i++)
        {
            if (ui_para->uipara_volume_bg.res_id[i])
            {
                ui_para->uipara_volume_bg.res_hdl[i] = dsk_theme_open(ui_para->uipara_volume_bg.res_id[i]);
                if (!ui_para->uipara_volume_bg.res_hdl[i])
                {
                    __msg("dsk_theme_open fail...\n");
                }
            }
        }
    }

    {
        int i;
        int j;

        for (i = 0 ; i < movie_volume_icon_num ; i++)
        {
            for (j = 0 ; j < MOVIE_VOLUME_MAX_ICON_RES_NUM ; j++)
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

static __s32 __volume_deinit_ui(void)
{
    movie_volume_uipara_t *ui_para;

    ui_para = movie_volume_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_volume_get_uipara fail...\n");
        return EPDK_FAIL;
    }

    {
        __s32 i;

        for (i = 0 ; i < MOVIE_VOLUME_MAX_ICON_RES_NUM ; i++)
        {
            if (ui_para->uipara_volume_bg.res_hdl[i])
            {
                dsk_theme_close(ui_para->uipara_volume_bg.res_hdl[i]);
                ui_para->uipara_volume_bg.res_hdl[i] = NULL;
            }
        }
    }

    {
        int i;
        int j;

        for (i = 0 ; i < movie_volume_icon_num ; i++)
        {
            for (j = 0 ; j < MOVIE_VOLUME_MAX_ICON_RES_NUM ; j++)
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

static __s32 __volume_draw_bg(movie_volume_scene_t *scene_para)
{
    movie_volume_uipara_t *ui_para;

    ui_para = movie_volume_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_volume_get_uipara fail...\n");
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

    {
        __s32 i;
        void *pbmp;

        for (i = 0 ; i < MOVIE_VOLUME_MAX_ICON_RES_NUM ; i++)
        {
            if (ui_para->uipara_volume_bg.res_hdl[i])
            {
                pbmp = dsk_theme_hdl2buf(ui_para->uipara_volume_bg.res_hdl[i]);
                if (!pbmp)
                {
                    __msg("pbmp is null...\n");
                    return EPDK_FAIL;
                }
                else
                {
                    GUI_BMP_Draw(pbmp, ui_para->uipara_volume_bg.x, ui_para->uipara_volume_bg.y);
                }
            }
        }
    }

    return EPDK_OK;
}

static __s32 __volume_update_volume_ui(movie_volume_scene_t *scene_para)
{
    movie_volume_uipara_t *ui_para;

    ui_para = movie_volume_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_volume_get_uipara fail...\n");
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
    GUI_SetBkColor(0xF0);
    com_memdev_create(scene_para->hlyr);
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);

    //画音量图标
    {
        void *pbmp;
        __s32 focus;

        if (scene_para->cur_val == scene_para->min_val)
        {
            focus = 1;
        }
        else
        {
            focus = 0;
        }

        if (!ui_para->uipara_icon[movie_volume_icon_lvolume].res_hdl[focus])
        {
            __msg("ui_para->uipara_icon[movie_volume_icon_lvolume].res_hdl[focus] is null...\n");
            com_memdev_delete();
            return EPDK_FAIL;
        }
        pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon[movie_volume_icon_lvolume].res_hdl[focus]);
        if (!pbmp)
        {
            __msg("pbmp is null...\n");
            com_memdev_delete();
            return EPDK_FAIL;
        }

        {
            GUI_RECT gui_rect;
            gui_rect.x0 = ui_para->uipara_icon[movie_volume_icon_lvolume].x;
            gui_rect.y0 = ui_para->uipara_icon[movie_volume_icon_lvolume].y;
            gui_rect.x1 = gui_rect.x0 + ui_para->uipara_icon[movie_volume_icon_lvolume].w - 1;
            gui_rect.y1 = gui_rect.y0 + ui_para->uipara_icon[movie_volume_icon_lvolume].h - 1;
            GUI_ClearRect(gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
        }
        GUI_BMP_Draw(pbmp, ui_para->uipara_icon[movie_volume_icon_lvolume].x, ui_para->uipara_icon[movie_volume_icon_lvolume].y);


        if (!ui_para->uipara_icon[movie_volume_icon_rvolume].res_hdl[focus])
        {
            __msg("ui_para->uipara_icon[movie_volume_icon_rvolume].res_hdl[focus] is null...\n");
            com_memdev_delete();
            return EPDK_FAIL;
        }
        pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon[movie_volume_icon_rvolume].res_hdl[focus]);
        if (!pbmp)
        {
            __msg("pbmp is null...\n");
            com_memdev_delete();
            return EPDK_FAIL;
        }
        GUI_BMP_Draw(pbmp, ui_para->uipara_icon[movie_volume_icon_rvolume].x, ui_para->uipara_icon[movie_volume_icon_rvolume].y);
    }

    //画文字
    {
        char str_val[32] = {0};
        GUI_RECT gui_rect;
        GUI_SetBkColor(0xF0);
        GUI_SetColor(APP_COLOR_WHITE);

        gui_rect.x0 = ui_para->uipara_volume_text.x;
        gui_rect.y0 = ui_para->uipara_volume_text.y;
        gui_rect.x1 = gui_rect.x0 + ui_para->uipara_volume_text.w;
        gui_rect.y1 = gui_rect.y0 + ui_para->uipara_volume_text.h;
        GUI_ClearRectEx(&gui_rect);
        eLIBs_sprintf(str_val, "%d", scene_para->cur_val);
        GUI_DispStringInRect(str_val, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
    }

    //画进度条背景
    {
        void *pbmp;

        if (!ui_para->uipara_icon[movie_volume_icon_prog_bg].res_hdl[0])
        {
            __msg("ui_para->uipara_icon[movie_volume_icon_prog_bg].res_hdl[0] is null...\n");
            com_memdev_delete();
            return EPDK_FAIL;
        }
        pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon[movie_volume_icon_prog_bg].res_hdl[0]);
        if (!pbmp)
        {
            __msg("pbmp is null...\n");
            com_memdev_delete();
            return EPDK_FAIL;
        }
        GUI_BMP_Draw(pbmp, ui_para->uipara_icon[movie_volume_icon_prog_bg].x, ui_para->uipara_icon[movie_volume_icon_prog_bg].y);
    }

    //画进度条
    if(scene_para->cur_val >= scene_para->min_val
            && scene_para->cur_val <= scene_para->max_val)
    {
        void *pbmp;

        if (!ui_para->uipara_icon[movie_volume_icon_prog_cursor].res_hdl[0])
        {
            __msg("ui_para->uipara_icon[movie_volume_icon_prog_cursor].res_hdl[0] is null...\n");
            com_memdev_delete();
            return EPDK_FAIL;
        }
        pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon[movie_volume_icon_prog_cursor].res_hdl[0]);
        if (!pbmp)
        {
            __msg("pbmp is null...\n");
            com_memdev_delete();
            return EPDK_FAIL;
        }

        {
            __s32 i;
            __s32 n;

            if (scene_para->cur_val == scene_para->min_val)
            {
                n = 0;
            }
            else
            {
                n = ui_para->uipara_icon[movie_volume_icon_prog_bg].w * (scene_para->cur_val - scene_para->min_val)
                    / ui_para->uipara_icon[movie_volume_icon_prog_cursor].w / (scene_para->max_val - scene_para->min_val);
            }

            for (i = 0 ; i < n ; i++)
            {
                GUI_BMP_Draw(pbmp, ui_para->uipara_icon[movie_volume_icon_prog_cursor].x + i * ui_para->uipara_icon[movie_volume_icon_prog_cursor].w
                             , ui_para->uipara_icon[movie_volume_icon_prog_cursor].y);
            }
        }
    }

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    com_memdev_delete();

    return EPDK_OK;
}

static __s32 __volume_install_hide_timmer(movie_volume_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    scene_para->hide_timmer_id = 0x16;

    if (!GUI_IsTimerInstalled(scene_para->hfrm, scene_para->hide_timmer_id))
    {
        GUI_SetTimer(scene_para->hfrm, scene_para->hide_timmer_id
                     , 400 * 2, NULL); //2秒
    }
    else
    {
        __msg("timmer already install...\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __volume_reset_hide_timmer(movie_volume_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(scene_para->hfrm, scene_para->hide_timmer_id))
    {
        GUI_ResetTimer(scene_para->hfrm, scene_para->hide_timmer_id
                       , 400 * 2, NULL); //2秒
    }

    return EPDK_OK;
}

static __s32 __volume_uninstall_hide_timmer(movie_volume_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(scene_para->hfrm, scene_para->hide_timmer_id))
    {
        GUI_KillTimer(scene_para->hfrm, scene_para->hide_timmer_id);
    }

    return EPDK_OK;
}

static __s32 __volume_proc(__gui_msg_t *msg)
{
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        movie_volume_scene_t *scene_para;

        __msg("__volume_proc GUI_MSG_CREATE begin\n");

        scene_para = GUI_WinGetAttr(msg->h_deswin);
        if (!scene_para)
        {
            __err("invalid para...\n");
            return EPDK_FAIL;
        }

        scene_para->hfrm = msg->h_deswin;//必须在此初始化，因为窗口创建未返回，否则该值仍然是空

        __volume_init_ui();

        __volume_draw_bg(scene_para);

        __volume_update_volume_ui(scene_para);

        __volume_install_hide_timmer(scene_para);

        __msg("__volume_proc GUI_MSG_CREATE end\n");
        return EPDK_OK;
    }
    case GUI_MSG_DESTROY:
    {
        movie_volume_scene_t *scene_para;

        __msg("__volume_proc GUI_MSG_DESTROY begin\n");

        scene_para = GUI_WinGetAttr(msg->h_deswin);
        if (!scene_para)
        {
            __err("invalid para...\n");
            return EPDK_FAIL;
        }

        __volume_uninstall_hide_timmer(scene_para);
        __volume_deinit_ui();

        __msg("__volume_proc GUI_MSG_DESTROY end\n");
        return EPDK_OK;
    }
    case GUI_MSG_KEY:
    {
        static __s32 last_key = -1;
        movie_volume_scene_t *scene_para;

        __msg("__volume_proc GUI_MSG_KEY begin\n");

        scene_para = GUI_WinGetAttr(msg->h_deswin);
        if (!scene_para)
        {
            __err("invalid para...\n");
            return EPDK_FAIL;
        }

        if (KEY_UP_ACTION == msg->dwAddData2)
        {
            __volume_reset_hide_timmer(scene_para);
            if (GUI_MSG_KEY_LEFT == last_key
                    /*||GUI_MSG_KEY_VDEC == last_key*/)
            {
                if (scene_para->cur_val > scene_para->min_val)
                {
                    scene_para->cur_val--;
                    __volume_update_volume_ui(scene_para);
                    movie_cmd2parent(msg->h_deswin, movie_volume_scene_msg_volumeset, scene_para->cur_val, 0);
                }
            }
            else if (GUI_MSG_KEY_RIGHT == last_key
                     /*||GUI_MSG_KEY_VADD == last_key*/)
            {
                __volume_reset_hide_timmer(scene_para);
                if (scene_para->cur_val < scene_para->max_val)
                {
                    scene_para->cur_val++;
                    __volume_update_volume_ui(scene_para);
                    movie_cmd2parent(msg->h_deswin, movie_volume_scene_msg_volumeset, scene_para->cur_val, 0);
                }
            }
        }
        else
        {
            switch(msg->dwAddData1)
            {
            case GUI_MSG_KEY_LONGLEFT:
                //case GUI_MSG_KEY_LONGVDEC:
            {
                __volume_reset_hide_timmer(scene_para);
                if (scene_para->cur_val > scene_para->min_val)
                {
                    scene_para->cur_val--;
                    __volume_update_volume_ui(scene_para);
                    movie_cmd2parent(msg->h_deswin, movie_volume_scene_msg_volumeset, scene_para->cur_val, 0);
                }
                break;
            }
            case GUI_MSG_KEY_LONGRIGHT:
                //case GUI_MSG_KEY_LONGVADD:
            {
                __volume_reset_hide_timmer(scene_para);
                if (scene_para->cur_val < scene_para->max_val)
                {
                    scene_para->cur_val++;
                    __volume_update_volume_ui(scene_para);
                    movie_cmd2parent(msg->h_deswin, movie_volume_scene_msg_volumeset, scene_para->cur_val, 0);
                }
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

        __msg("__volume_proc GUI_MSG_KEY end\n");

        return EPDK_OK;
    }
    case GUI_MSG_TOUCH:
    {
        __msg("__volume_proc GUI_MSG_TOUCH begin\n");

        __msg("__volume_proc GUI_MSG_TOUCH end\n");
        return EPDK_OK;
    }
    case GUI_MSG_PAINT:
    {
        movie_volume_scene_t *scene_para;

        __msg("__volume_proc GUI_MSG_PAINT begin\n");

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

        __volume_draw_bg(scene_para);

        __volume_update_volume_ui(scene_para);

        __msg("__volume_proc GUI_MSG_PAINT begin\n");

        return EPDK_OK;
    }
    case GUI_MSG_TIMER:
    {
        __s32 ret;
        movie_volume_scene_t *scene_para;

        __msg("__volume_proc GUI_MSG_TIMER begin\n");

        scene_para = GUI_WinGetAttr(msg->h_deswin);

        ret = -1;

        if (scene_para)
        {
            if(LOWORD(msg->dwAddData1) == scene_para->hide_timmer_id)
            {
                movie_cmd2parent(msg->h_deswin, movie_volume_scene_msg_timeout, 0, 0);
                ret = EPDK_OK;
            }
        }
        else
        {
            __err("scene_para is null...\n");
        }

        __msg("__volume_proc GUI_MSG_TIMER end\n");

        if (-1 == ret)//未处理，交给子场景处理
        {
            break;
        }
        else//已处理
        {
            return EPDK_OK;
        }
    }
    case GUI_MSG_CLOSE:
    {
        __msg("__volume_proc GUI_MSG_CLOSE begin\n");
        movie_cmd2parent(msg->h_deswin, movie_volume_scene_msg_timeout, 0, 0);
        __msg("__volume_proc GUI_MSG_CLOSE end\n");
        break;
    }
    default:
    {
        break;
    }
    }

    return GUI_FrmWinDefaultProc(msg);
}

void *movie_volume_scene_create(movie_volume_create_para_t *create_para)
{
    __gui_framewincreate_para_t framewin_para;
    movie_volume_scene_t *sence_para;
    movie_volume_uipara_t *ui_para;
    RECT lyr_rect;

    sence_para = (movie_volume_scene_t *)esMEMS_Balloc(sizeof(movie_volume_scene_t));
    if (!sence_para)
    {
        __msg("mem not enough...\n");
        return NULL;
    }

    eLIBs_memset(sence_para, 0, sizeof(movie_volume_scene_t));

    sence_para->hparent = create_para->hparent;
    sence_para->scene_id = create_para->scene_id;
    sence_para->max_val = create_para->max_val;
    sence_para->min_val = create_para->min_val;
    sence_para->cur_val = create_para->cur_val;

    ui_para = movie_volume_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        esMEMS_Bfree(sence_para, sizeof(movie_volume_scene_t));
        sence_para = NULL;
        __msg("movie_spsc_get_uipara fail...\n");
        return NULL;
    }

    lyr_rect.x		= ui_para->uipara_lyr.x;
    lyr_rect.y		= ui_para->uipara_lyr.y;
    lyr_rect.width	= ui_para->uipara_lyr.w;
    lyr_rect.height = ui_para->uipara_lyr.h;

    sence_para->hlyr = __volume_8bpp_layer_create(&lyr_rect, ui_para->pipe);
    if (NULL == sence_para->hlyr)
    {
        esMEMS_Bfree(sence_para, sizeof(movie_volume_scene_t));
        sence_para = NULL;
        __msg("sence_para->hlyr is null...\n");
        return NULL;
    }

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name			=	"movie_volume_win",
                         framewin_para.dwExStyle		= WS_EX_NONE;
    framewin_para.dwStyle		= WS_NONE | WS_VISIBLE;
    framewin_para.spCaption		= NULL;
    framewin_para.hOwner		= NULL;
    framewin_para.id			= sence_para->scene_id;
    framewin_para.hHosting		=  sence_para->hparent;
    framewin_para.FrameWinProc	= (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__volume_proc);
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
        esMEMS_Bfree(sence_para, sizeof(movie_volume_scene_t));
        sence_para = NULL;
        __msg("GUI_FrmWinCreate fail...\n");
        return NULL;
    }

    if (sence_para->hlyr)
    {
        GUI_LyrWinSetSta(sence_para->hlyr, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSetTop(sence_para->hlyr);
    }
    else
    {
        __err("invalid para...\n");
    }


    return sence_para;
}

__s32 movie_volume_scene_delete(void *handle)
{
    movie_volume_scene_t *scene_para;

    scene_para = (movie_volume_scene_t *)handle;

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

    if (!scene_para->hlyr)
    {
        __err("invalid para..\n");
        return EPDK_FAIL;
    }
    GUI_LyrWinDelete(scene_para->hlyr);

    eLIBs_memset(scene_para, 0, sizeof(movie_volume_scene_t));//防止重复释放
    esMEMS_Bfree(scene_para, sizeof(movie_volume_scene_t));

    return EPDK_OK;
}

__s32 movie_volume_scene_set_focus(void *handle)
{
    movie_volume_scene_t *scene_para;

    scene_para = (movie_volume_scene_t *)handle;

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
