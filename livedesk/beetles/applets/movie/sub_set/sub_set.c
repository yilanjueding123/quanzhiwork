/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: sub_set.c
* By      	:
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/
#include "sub_set.h"
#include "sub_set_ui.h"
#include "common/common.h"
typedef struct
{
    // 输入参数
    H_WIN hparent;
    __s32 scene_id;
    __u32 sub_state;
    __u32 sub_pos;
    __u32 sub_color;
    __u32 sub_stream;

    //内部参数
    H_LYR hlyr;
    H_WIN hfrm;
    __u8  hide_timmer_id;
} movie_sub_set_scene_t;

static __u32 sub_color_tbl[MOVIE_SUB_COLOR_NR] =
{
    0xffffffff,
    0xff0000ff,
    0xffff0000,
};

/***********************************************************************************************************
	建立图层
************************************************************************************************************/
static H_LYR movie_sub_set_8bpp_layer_create(RECT *rect, __s32 pipe)
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
        __err("GUI_LyrWinCreate fail... !\n");
    }

    return layer;
}


static __s32 __movie_sub_set_install_hide_timmer(movie_sub_set_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    scene_para->hide_timmer_id = 0x15;

    if (!GUI_IsTimerInstalled(scene_para->hfrm, scene_para->hide_timmer_id))
    {
        GUI_SetTimer(scene_para->hfrm, scene_para->hide_timmer_id
                     , 400 * 3, NULL);
    }
    else
    {
        __msg("timmer already install...\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __movie_sub_set_reset_hide_timmer(movie_sub_set_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(scene_para->hfrm, scene_para->hide_timmer_id))
    {
        GUI_ResetTimer(scene_para->hfrm, scene_para->hide_timmer_id
                       , 400 * 3, NULL);
    }
    else
    {
        __msg("timmer already install...\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __movie_sub_set_uninstall_hide_timmer(movie_sub_set_scene_t *scene_para)
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

static __s32 __movie_sub_set_init_ui(void)
{
    movie_sub_set_uipara_t *ui_para;

    ui_para = movie_sub_set_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...\n");
        return EPDK_FAIL;
    }

    {
        int j;

        for (j = 0 ; j < MOVIE_SUB_SET_MAX_ICON_RES_NUM ; j++)
        {
            if (ui_para->uipara_bg.res_id[j])
            {
                ui_para->uipara_bg.res_hdl[j] = dsk_theme_open(ui_para->uipara_bg.res_id[j]);
                if (!ui_para->uipara_bg.res_hdl[j])
                {
                    __msg("dsk_theme_open fail...\n");
                }
                else//先占住内存
                {
                    //  dsk_theme_hdl2buf(ui_para->uipara_bg.res_hdl[j]);
                }
            }
        }
    }

    {
        int i;
        int j;
        int k;

        for (i = 0 ; i < movie_sub_set_item_num ; i++)
        {
            for (j = 0 ; j < movie_sub_set_item_bmp_num ; j++)
            {
                for(k = 0 ; k < MOVIE_SUB_SET_MAX_ICON_RES_NUM ; k++)
                    if (ui_para->item[i].item_bmp[j].res_id[k])
                    {
                        ui_para->item[i].item_bmp[j].res_hdl[k] = dsk_theme_open(ui_para->item[i].item_bmp[j].res_id[k]);
                        if (!ui_para->item[i].item_bmp[j].res_hdl[k])
                        {
                            __msg("dsk_theme_open fail...\n");
                        }
                        else//先占住内存
                        {
                            //  dsk_theme_hdl2buf(ui_para->uipara_icon[i].res_hdl[j]);
                        }
                    }
            }

        }
    }

    return EPDK_OK;
}

static __s32 __movie_sub_set_deinit_ui(void)
{
    movie_sub_set_uipara_t *ui_para;

    ui_para = movie_sub_set_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...\n");
        return EPDK_FAIL;
    }

    {
        int j;

        for (j = 0 ; j < MOVIE_SUB_SET_MAX_ICON_RES_NUM ; j++)
        {
            if (ui_para->uipara_bg.res_hdl[j])
            {
                dsk_theme_close(ui_para->uipara_bg.res_hdl[j]);
                ui_para->uipara_bg.res_hdl[j] = NULL;
            }
        }
    }

    {
        int i;
        int j;
        int k;

        for (i = 0 ; i < movie_sub_set_item_num ; i++)
        {
            for (j = 0 ; j < movie_sub_set_item_bmp_num ; j++)
            {
                for(k = 0 ; k < MOVIE_SUB_SET_MAX_ICON_RES_NUM ; k++)
                    if (ui_para->item[i].item_bmp[j].res_hdl[k])
                    {
                        dsk_theme_close(ui_para->item[i].item_bmp[j].res_hdl[k]);
                        ui_para->item[i].item_bmp[j].res_hdl[k] = NULL;
                    }
            }

        }
    }

    return EPDK_OK;
}

static __s32 __movie_sub_set_draw_item(movie_sub_set_scene_t *scene_para, __s32 index, __s32 focus)
{
    movie_sub_set_uipara_t *ui_para;

    ui_para = movie_sub_set_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...\n");
        return EPDK_FAIL;
    }

    if(index < 0 || index >= movie_sub_set_item_num)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (focus < 0 || focus > 1)
    {
        __err("invalid para...\n");
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

    {
        int i;
        void *pbmp;
        GUI_RECT gui_rect;

        GUI_SetBkColor(0xF0);
        GUI_SetDrawMode(GUI_DRAWMODE_TRANS);

        for(i = 0 ; i < movie_sub_set_item_bmp_num ; i++)
        {
            gui_rect.x0 = ui_para->item[index].item_bmp[i].x;
            gui_rect.y0 = ui_para->item[index].item_bmp[i].y;
            gui_rect.x1 = gui_rect.x0 + ui_para->item[index].item_bmp[i].w;
            gui_rect.y1 = gui_rect.y0 + ui_para->item[index].item_bmp[i].h;

            GUI_ClearRectEx(&gui_rect);

            if (!ui_para->item[index].item_bmp[i].res_hdl[focus])
            {
                __msg("res_hdl is null...\n");
                continue;
            }

            pbmp = dsk_theme_hdl2buf(ui_para->item[index].item_bmp[i].res_hdl[focus]);
            if (!pbmp)
            {
                __msg("pbmp is null...\n");
                return EPDK_FAIL;
            }

            //draw icon
            {
                __s32 ret;

                ret = GUI_BMP_Draw(pbmp, ui_para->item[index].item_bmp[i].x, ui_para->item[index].item_bmp[i].y);
                if (0 != ret)
                {
                    __msg("GUI_BMP_Draw fail...\n");
                    continue;
                }
            }
        }
    }

    //draw text
    {
        __s32 lang_id;
        __s32 ret;
        char str[32] = {0};
        GUI_RECT gui_rect;
        GUI_SetFont(SWFFont);
        GUI_SetColor(APP_COLOR_WHITE);
        GUI_SetBkColor(0xF0);
        GUI_UC_SetEncodeUTF8();

        lang_id = 0;
        switch(index)
        {
        case movie_sub_set_item_state:
        {
            lang_id = ui_para->item_text_langid[index][scene_para->sub_state];
            break;
        }
        case movie_sub_set_item_color:
        {
            lang_id = ui_para->item_text_langid[index][scene_para->sub_color];
            break;
        }
        case movie_sub_set_item_pos:
        {
            lang_id = ui_para->item_text_langid[index][scene_para->sub_pos];
            break;
        }
        case movie_sub_set_item_stream:
        {
            __msg("scene_para->sub_stream=%d\n", scene_para->sub_stream);
            lang_id = ui_para->item_text_langid[index][scene_para->sub_stream];
            break;
        }
        default:
        {
            __err("unknown item type...\n");
            break;
        }
        }
        if (lang_id)
        {
            __msg("lang_id=%d\n", lang_id);
            ret = dsk_langres_get_menu_text(lang_id, str, sizeof(str));
            if(EPDK_FAIL == ret)
            {
                __msg("dsk_langres_get_menu_text fail..., lang_id=%d\n", lang_id);
                return EPDK_FAIL;
            }
            __msg("str=%s\n", str);

            gui_rect.x0 = ui_para->item[index].item_static_text[movie_sub_set_item_static_val].x;
            gui_rect.y0 = ui_para->item[index].item_static_text[movie_sub_set_item_static_val].y;
            gui_rect.x1 = gui_rect.x0 + ui_para->item[index].item_static_text[movie_sub_set_item_static_val].w;
            gui_rect.y1 = gui_rect.y0 + ui_para->item[index].item_static_text[movie_sub_set_item_static_val].h;
            GUI_ClearRectEx(&gui_rect);
            GUI_DispStringInRect(str, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
        }
    }

    return EPDK_OK;
}

static __s32 __movie_sub_set_bg_draw(movie_sub_set_scene_t *scene_para)
{
    movie_sub_set_uipara_t *ui_para;

    ui_para = movie_sub_set_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...\n");
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

    {
        __s32 j;
        void *pbmp;

        for (j = 0 ; j < MOVIE_SUB_SET_MAX_ICON_RES_NUM ; j++)
        {
            if (ui_para->uipara_bg.res_hdl[j])
            {
                pbmp = dsk_theme_hdl2buf(ui_para->uipara_bg.res_hdl[j]);
                if (!pbmp)
                {
                    __msg("dsk_theme_hdl2buf fail...\n");
                    continue;
                }

                GUI_BMP_Draw(pbmp, ui_para->uipara_bg.x, ui_para->uipara_bg.y);
            }
        }
    }

    return EPDK_OK;
}

static __s32 __movie_sub_set_static_icon_draw(movie_sub_set_scene_t *scene_para, movie_sub_set_static_icon_t type)
{
    movie_sub_set_uipara_t *ui_para;

    ui_para = movie_sub_set_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...\n");
        return EPDK_FAIL;
    }

    if (type < 0 || type >= movie_sub_set_static_icon_num)
    {
        __err("invalid para...\n");
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

    {
        __s32 ret;
        __s32 index;
        char str[32] = {0};
        GUI_RECT gui_rect;

        if(ui_para->static_text[type].lang_id)
        {
            ret = dsk_langres_get_menu_text(ui_para->static_text[type].lang_id, str, sizeof(str));
            if(EPDK_FAIL == ret)
            {
                __msg("dsk_langres_get_menu_text fail...\n");
                return EPDK_FAIL;
            }

            gui_rect.x0 = ui_para->static_text[type].x;
            gui_rect.y0 = ui_para->static_text[type].y;
            gui_rect.x1 = gui_rect.x0 + ui_para->static_text[type].w;
            gui_rect.y1 = gui_rect.y0 + ui_para->static_text[type].h;
            GUI_SetFont(SWFFont);
            GUI_SetColor(APP_COLOR_WHITE);
            GUI_SetBkColor(0xF0);
            GUI_UC_SetEncodeUTF8();
            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            GUI_ClearRectEx(&gui_rect);
            GUI_DispStringInRect(str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
    }

    return EPDK_OK;
}

static __s32 __movie_sub_set_static_icon_draw_all(movie_sub_set_scene_t *scene_para)
{
    __s32 ret;

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

    ret = EPDK_OK;

    {
        __s32 i;

        for (i = 0 ; i < movie_sub_set_static_icon_num ; i++)
        {
            if(EPDK_FAIL == __movie_sub_set_static_icon_draw(scene_para, i))
            {
                __msg("__movie_sub_set_static_icon_draw fail, i= %d\n", i);
                ret = EPDK_FAIL;
            }
        }
    }

    return ret;
}

static __s32 __movie_sub_set_item_draw_all(movie_sub_set_scene_t *scene_para)
{
    movie_sub_set_uipara_t *ui_para;

    ui_para = movie_sub_set_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...\n");
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

    {
        int i;

        for (i = 0 ; i < movie_sub_set_item_num ; i++)
        {
            if (ui_para->focus_item_index == i)
            {
                __movie_sub_set_draw_item(scene_para, i, 1);
            }
            else
            {
                __movie_sub_set_draw_item(scene_para, i, 0);
            }
        }
    }

    return EPDK_OK;
}

static __s32 __movie_sub_set_enter_key_proc(movie_sub_set_scene_t *scene_para, __gui_msg_t *msg, movie_sub_set_item_e item_id)
{

    movie_sub_set_uipara_t *ui_para;
    ui_para = movie_sub_set_get_uipara(GUI_GetScnDir());
    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...\n");
        return EPDK_FAIL;
    }

    switch(item_id)
    {
    case movie_sub_set_item_state:
    {
        if(1 == scene_para->sub_state)
        {
            scene_para->sub_state = 0;
        }
        else
        {
            scene_para->sub_state = 1;
        }
        com_memdev_create(scene_para->hlyr);
        __movie_sub_set_draw_item(scene_para, ui_para->focus_item_index, 1);
        com_memdev_delete();
        movie_cmd2parent(msg->h_deswin, sub_set_scene_msg_sub_state, scene_para->sub_state, 0);
        return EPDK_OK;
    }
    case movie_sub_set_item_color:
    {
        __s32 sub_color_nr;

        sub_color_nr = sizeof(sub_color_tbl) / sizeof(sub_color_tbl[0]);
        if(scene_para->sub_color < 0 || scene_para->sub_color >= sub_color_nr )
        {
            scene_para->sub_color = 0;
        }

        scene_para->sub_color++;

        if(scene_para->sub_color >= sub_color_nr )
        {
            scene_para->sub_color = 0;
        }
        com_memdev_create(scene_para->hlyr);
        __movie_sub_set_draw_item(scene_para, ui_para->focus_item_index, 1);
        com_memdev_delete();
        movie_cmd2parent(msg->h_deswin, sub_set_scene_msg_sub_color, sub_color_tbl[scene_para->sub_color], 0);
        return EPDK_OK;
    }
    case movie_sub_set_item_pos:
    {
        scene_para->sub_pos++;
        if(scene_para->sub_pos > 2)
        {
            scene_para->sub_pos = 0;
        }
        com_memdev_create(scene_para->hlyr);
        __movie_sub_set_draw_item(scene_para, ui_para->focus_item_index, 1);
        com_memdev_delete();
        movie_cmd2parent(msg->h_deswin, sub_set_scene_msg_sub_pos, scene_para->sub_pos, 0);
        return EPDK_OK;
    }
    case movie_sub_set_item_stream:
    {
        __s32 ret;
        __subtitle_profile_t 	subtitle_para;

        ret = robin_get_subtitle_list(&subtitle_para);
        if(EPDK_OK != ret)
        {
            __msg("robin_get_subtitle_list fail...\n");
            return EPDK_OK;
        }

        if (0 == subtitle_para.nSubtitleStrmNum)
        {
            __msg("0 == subtitle_para.nSubtitleStrmNum\n");
            return EPDK_OK;
        }

        scene_para->sub_stream++;
        if(scene_para->sub_stream >= subtitle_para.nSubtitleStrmNum)
        {
            scene_para->sub_stream = 0;
        }
        if(scene_para->sub_stream < 0)
        {
            scene_para->sub_stream = 0;
        }

        __msg("scene_para->sub_stream=%d\n", scene_para->sub_stream);
        com_memdev_create(scene_para->hlyr);
        __movie_sub_set_draw_item(scene_para, ui_para->focus_item_index, 1);
        com_memdev_delete();
        movie_cmd2parent(msg->h_deswin, sub_set_scene_msg_sub_stream, scene_para->sub_stream, 0);

        return EPDK_OK;
    }
    default:
    {
        //__msg("unknown msg from sub_set scene.....\n");
        break;
    }
    }

    __msg("unknown message...\n");

    return EPDK_FAIL;
}

static __s32 __movie_sub_set_proc(__gui_msg_t *msg)
{
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        movie_sub_set_scene_t *scene_para;

        __msg("__movie_sub_set_proc GUI_MSG_CREATE begin\n");

        scene_para = GUI_WinGetAttr(msg->h_deswin);
        if (!scene_para)
        {
            __err("invalid para...\n");
            return EPDK_FAIL;
        }

        scene_para->hfrm = msg->h_deswin;//必须在此初始化，因为窗口创建未返回，否则该值仍然是空

        __movie_sub_set_init_ui();

        __movie_sub_set_install_hide_timmer(scene_para);

        __msg("__movie_sub_set_proc GUI_MSG_CREATE end\n");
        return EPDK_OK;
    }
    case GUI_MSG_DESTROY:
    {
        movie_sub_set_scene_t *scene_para;

        __msg("__movie_sub_set_proc GUI_MSG_DESTROY begin\n");

        scene_para = GUI_WinGetAttr(msg->h_deswin);
        if (!scene_para)
        {
            __err("invalid para...\n");
            return EPDK_FAIL;
        }

        __movie_sub_set_uninstall_hide_timmer(scene_para);

        __movie_sub_set_deinit_ui();

        __msg("__movie_sub_set_proc GUI_MSG_DESTROY end\n");
        return EPDK_OK;
    }
    case GUI_MSG_KEY:
    {
        static __s32 last_key = -1;
        movie_sub_set_scene_t *scene_para;
        movie_sub_set_uipara_t *ui_para;

        __msg("__movie_sub_set_proc GUI_MSG_KEY begin\n");

        scene_para = GUI_WinGetAttr(msg->h_deswin);
        if (!scene_para)
        {
            __err("invalid para...\n");
            return EPDK_FAIL;
        }

        ui_para = movie_sub_set_get_uipara(GUI_GetScnDir());
        if (NULL == ui_para)
        {
            __msg("movie_spsc_get_uipara fail...\n");
            return EPDK_FAIL;
        }

        __movie_sub_set_reset_hide_timmer(scene_para);

        if (KEY_UP_ACTION == msg->dwAddData2)
        {
            if (GUI_MSG_KEY_LEFT == last_key)
            {
                __msg("draw unfocus icon..\n");
                com_memdev_create(scene_para->hlyr);
                __movie_sub_set_draw_item(scene_para, ui_para->focus_item_index, 0);

                ui_para->focus_item_index--;
                if (ui_para->focus_item_index < 0)
                {
                    ui_para->focus_item_index = movie_sub_set_item_num - 1;
                }
                __msg("draw focus icon..\n");
                __movie_sub_set_draw_item(scene_para, ui_para->focus_item_index, 1);
                com_memdev_delete();
            }
            else if (GUI_MSG_KEY_RIGHT == last_key)
            {
                __msg("draw unfocus icon..\n");
                com_memdev_create(scene_para->hlyr);
                __movie_sub_set_draw_item(scene_para, ui_para->focus_item_index, 0);

                ui_para->focus_item_index++;
                if (ui_para->focus_item_index >= movie_sub_set_item_num)
                {
                    ui_para->focus_item_index = 0;
                }
                __msg("draw focus icon..\n");
                __movie_sub_set_draw_item(scene_para, ui_para->focus_item_index, 1);
                com_memdev_delete();
            }
            else if (GUI_MSG_KEY_ENTER == last_key)
            {
                __movie_sub_set_enter_key_proc(scene_para, msg, ui_para->focus_item_index);
            }
        }
        else
        {
            switch(msg->dwAddData1)
            {
            case GUI_MSG_KEY_LONGLEFT:
            {
                __msg("draw unfocus icon..\n");
                com_memdev_create(scene_para->hlyr);
                __movie_sub_set_draw_item(scene_para, ui_para->focus_item_index, 0);

                ui_para->focus_item_index--;
                if (ui_para->focus_item_index < 0)
                {
                    ui_para->focus_item_index = movie_sub_set_item_num - 1;
                }
                __msg("draw focus icon..\n");
                __movie_sub_set_draw_item(scene_para, ui_para->focus_item_index, 1);
                com_memdev_delete();

                break;
            }
            case GUI_MSG_KEY_LONGRIGHT:
            {
                __msg("draw unfocus icon..\n");
                com_memdev_create(scene_para->hlyr);
                __movie_sub_set_draw_item(scene_para, ui_para->focus_item_index, 0);

                ui_para->focus_item_index++;
                if (ui_para->focus_item_index >= movie_sub_set_item_num)
                {
                    ui_para->focus_item_index = 0;
                }
                __msg("draw focus icon..\n");
                __movie_sub_set_draw_item(scene_para, ui_para->focus_item_index, 1);
                GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                com_memdev_delete();

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

        __msg("__movie_sub_set_proc GUI_MSG_KEY end\n");
        return EPDK_OK;
    }
    case GUI_MSG_TOUCH:
    {
        __msg("__movie_sub_set_proc GUI_MSG_TOUCH begin\n");

        __msg("__movie_sub_set_proc GUI_MSG_TOUCH end\n");
        return EPDK_OK;
    }
    case GUI_MSG_TIMER:
    {
        movie_sub_set_scene_t *sence_para;
        __s32 ret;

        __msg("__movie_sub_set_proc GUI_MSG_TIMER begin\n");

        ret = -1;

        sence_para = GUI_WinGetAttr(msg->h_deswin);
        if (sence_para)
        {
            if(LOWORD(msg->dwAddData1) == sence_para->hide_timmer_id)
            {
                movie_cmd2parent(msg->h_deswin, sub_set_scene_msg_timeout, 0, 0);
                ret = EPDK_OK;
            }
        }
        else
        {
            __msg("sence_para is null...\n");
        }

        __msg("__movie_sub_set_proc GUI_MSG_TIMER end\n");

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
        __msg("__movie_sub_set_proc GUI_MSG_CLOSE begin\n");
        movie_cmd2parent(msg->h_deswin, sub_set_scene_msg_timeout, 0, 0);
        __msg("__movie_sub_set_proc GUI_MSG_CLOSE end\n");
        return EPDK_OK;
    }
    case GUI_MSG_PAINT:
    {
        movie_sub_set_scene_t *scene_para;

        scene_para = GUI_WinGetAttr(msg->h_deswin);
        if (!scene_para)
        {
            __err("invalid para...\n");
            return EPDK_FAIL;
        }

        if(!scene_para->hlyr)
        {
            return EPDK_FAIL;
        }

        if (GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr))
        {
            __msg("GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
            return EPDK_OK;
        }

        GUI_LyrWinSel(scene_para->hlyr);

        com_memdev_create(scene_para->hlyr);

        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);

        __msg("before __movie_sub_set_bg_draw\n");
        __movie_sub_set_bg_draw(scene_para);
        __msg("after __movie_sub_set_bg_draw\n");

        __msg("before __movie_sub_set_item_draw_all\n");
        __movie_sub_set_item_draw_all(scene_para);
        __msg("after __movie_sub_set_item_draw_all\n");

        __msg("before __movie_sub_set_static_icon_draw_all\n");
        __movie_sub_set_static_icon_draw_all(scene_para);
        __msg("after __movie_sub_set_static_icon_draw_all\n");

        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
        com_memdev_delete();

        return EPDK_OK;
    }
    case GUI_MSG_WIN_WAKEUP:
    {
        GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
        return EPDK_OK;
    }
    default:
    {
        break;
    }
    }

    return GUI_FrmWinDefaultProc(msg);
}

void *movie_sub_set_scene_create(movie_sub_set_create_para_t *create_para)
{
    __gui_framewincreate_para_t framewin_para;
    movie_sub_set_scene_t *sence_para;
    movie_sub_set_uipara_t *ui_para;
    RECT lyr_rect;

    sence_para = (movie_sub_set_scene_t *)esMEMS_Balloc(sizeof(movie_sub_set_scene_t));
    if (!sence_para)
    {
        __msg("mem not enough...\n");
        return NULL;
    }

    eLIBs_memset(sence_para, 0, sizeof(movie_sub_set_scene_t));

    sence_para->hparent = create_para->hparent;
    sence_para->scene_id = create_para->scene_id;
    sence_para->sub_state = create_para->sub_state;
    sence_para->sub_color = create_para->sub_color;
    sence_para->sub_pos = create_para->sub_pos;
    sence_para->sub_stream = create_para->sub_stream;

    ui_para = movie_sub_set_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        esMEMS_Bfree(sence_para, sizeof(movie_sub_set_scene_t));
        sence_para = NULL;
        __msg("movie_spsc_get_uipara fail...\n");
        return NULL;
    }

    lyr_rect.x		= ui_para->uipara_lyr.x;
    lyr_rect.y		= ui_para->uipara_lyr.y;
    lyr_rect.width	= ui_para->uipara_lyr.w;
    lyr_rect.height = ui_para->uipara_lyr.h;

    sence_para->hlyr = movie_sub_set_8bpp_layer_create(&lyr_rect, ui_para->pipe);
    if (NULL == sence_para->hlyr)
    {
        esMEMS_Bfree(sence_para, sizeof(movie_sub_set_scene_t));
        sence_para = NULL;
        __msg("sence_para->hlyr is null...\n");
        return NULL;
    }

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name			=	"movie_sub_set_win",
                         framewin_para.dwExStyle		= WS_EX_NONE;
    framewin_para.dwStyle		= WS_NONE | WS_VISIBLE;
    framewin_para.spCaption		= NULL;
    framewin_para.hOwner		= NULL;
    framewin_para.id			= sence_para->scene_id;
    framewin_para.hHosting		=  sence_para->hparent;
    framewin_para.FrameWinProc	= (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__movie_sub_set_proc);
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
        esMEMS_Bfree(sence_para, sizeof(movie_sub_set_scene_t));
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

__s32 movie_sub_set_scene_set_focus(void *handle)
{
    movie_sub_set_scene_t *scene_para;

    scene_para = (movie_sub_set_scene_t *)handle;

    __here__;
    if (NULL == handle)
    {
        __err("invalid para..\n");
        return EPDK_FAIL;
    }

    __here__;
    if (!scene_para->hfrm)
    {
        __err("invalid para..\n");
        return EPDK_FAIL;
    }

    __here__;
    GUI_WinSetFocusChild(scene_para->hfrm);

    __here__;
    return EPDK_OK;
}

__s32 movie_sub_set_scene_delete(void *handle)
{
    movie_sub_set_scene_t *scene_para;

    scene_para = (movie_sub_set_scene_t *)handle;

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

    eLIBs_memset(scene_para, 0, sizeof(movie_sub_set_scene_t));

    esMEMS_Bfree(scene_para, sizeof(movie_sub_set_scene_t));

    return EPDK_OK;
}

H_WIN movie_sub_set_scene_get_hwnd(void *handle)
{
    movie_sub_set_scene_t *scene_para;

    scene_para = (movie_sub_set_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..\n");
        return NULL;
    }

    return scene_para->hfrm;
}

//根据颜色获得其在表里的下标
__s32 movie_sub_set_color2index(__u32 color)
{
    __s32 i;
    __s32 sub_color_nr;

    sub_color_nr = sizeof(sub_color_tbl) / sizeof(sub_color_tbl[0]);
    for(i = 0 ; i < sub_color_nr ; i++)
    {
        if((__u32)color == (__u32)(sub_color_tbl[i]))
        {
            return i;
        }
    }

    return 0;
}

__s32 movie_sub_set_scene_set_lyr_top(void *handle)
{
    movie_sub_set_scene_t *scene_para;

    scene_para = (movie_sub_set_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..\n");
        return EPDK_FAIL;
    }

    __msg("scene_para->hlyr=%d\n", scene_para->hlyr);
    if (scene_para->hlyr)
    {
        GUI_LyrWinSetTop(scene_para->hlyr);
    }

    __msg("scene_para->hfrm=%d\n", scene_para->hfrm);
    if(scene_para->hfrm)
    {
        __gui_msg_t msg;

        eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

        msg.h_deswin = scene_para->hfrm;
        msg.id = GUI_MSG_PAINT;
        GUI_SendMessage(&msg);
    }

    return EPDK_OK;
}


