/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: app_movie.c
* By      	: Bayden.chen
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2011-05-05   Bayden.chen  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "app_movie_i.h"

#if 0

#define __msg(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
							     eLIBs_printf(__VA_ARGS__)									        )
#define __wrn(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
							     eLIBs_printf(__VA_ARGS__)									        )
#define __inf(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
							     eLIBs_printf(__VA_ARGS__)									        )
#define __err(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
							     eLIBs_printf(__VA_ARGS__)									        )
#endif

//#define	__ENABLE_VOLUME_ADJUST_FUNCTION__
#define	TEST_DRAM_PRIORITY_CHANGE		0
#define	PRINTF_SDRAM_CONTROLLER_REG		0
#define	PRINTF_ALL_CCMU_REG		0
typedef struct
{
    //ctrl scene
    void *spsc_scene;//manwin

    void *volume_scene;//frmwin

    void *bright_scene;//frmwin

    void *playsta_scene;//frmwin

    void *sub_show_scene;//frmwin
    __u32 sub_state;
    __u32 sub_pos;
    __u32 sub_color;
    __u32 sub_size;
    __u32 sub_stream;//0代表国语 1代表粤语

    void *prog_scene;//frmwin

    void *sub_set_scene;//frmwin

    H_WIN breakplay_scene;//该值直接就是frmwin句柄
    H_WIN playerror_scene;//该值直接就是frmwin句柄
    H_WIN playend_scene;//该值直接就是frmwin句柄

    H_WIN movie_manager;//movie的manwin

    ES_FILE *hdisp;//显示驱动句柄
    __hdle	 hvedio_lyr;//视频图层

    __u8	 auto_play_timmer_id;

    __bool   exit_movie;//是否退出movie
    __s32    err_cnt;//播放错误的文件个数
    __s32    rr_flag;//是否进行了快退操作，用于快退到尽头时，做判断用

    __s32 stop_cnt ;//连续stop_cnt次检测到stop，才播下一首，防止还没播起来就跳到下一首
    __s32 cur_stop_cnt;
    __s32 cur_time;//记录当前播放时间，用于制造一个视频断点信息
    char  cur_playing_file[RAT_MAX_FULL_PATH_LEN + 1];
    __bool quit_by_fs_part_plug_out;//如果退出movie了，是否拔卡造成的退出
} movie_ctrl_t;

static __s32 __movie_playend_scene_create(movie_ctrl_t *movie_ctrl);


/***********************************************************************************************************
	建立图层
************************************************************************************************************/
static H_LYR movie_8bpp_layer_create(RECT *rect, __s32 pipe)
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

static __bool is_on_tv(void)
{
    __s32 			screen_width;
    __s32 			screen_height;

    /* get lcd size*/
    dsk_display_get_size(&screen_width, &screen_height);

    if((screen_width == 400 ) && ( screen_height == 240)
            || (screen_width == 480 ) && ( screen_height == 272))
    {
        return EPDK_FALSE;
    }

    return EPDK_TRUE;//其余都当作电视
    //return EPDK_TRUE;
}

static __s32 __movie_ctrl_para_init(movie_ctrl_t *movie_ctrl)
{
    if (!movie_ctrl)
    {
        __err("movie_ctrl is null...\n");
        return EPDK_FAIL;
    }

    movie_ctrl->hdisp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r");
    if (!movie_ctrl->hdisp)
    {
        __msg("open disp fail...\n");
    }

    __msg("open disp success...\n");

    {
        __s32 ret;
        __s32 screen_width, screen_height;
        RECT vedio_rect;
        robin_open_arg_t arg;
        arg.reserve_mem_size = 1024 * 1024; //1024*1024;//512*1024;
        ret = robin_open(ROBIN_MODE_VIDEO_MAX, &arg);
        if(EPDK_OK != ret)
        {
            __msg("robin_open fail...\n");
            return EPDK_FAIL;
        }
        __msg("robin_open success...\n");

        //robin_clear_feedback_msgQ();

        dsk_display_get_size(&screen_width, &screen_height);
        vedio_rect.x = 0;
        vedio_rect.y = 0;
        vedio_rect.width = screen_width;
        vedio_rect.height = screen_height;
        __msg("vedio screen=%d %d %d %d", vedio_rect.x, vedio_rect.y, vedio_rect.width, vedio_rect.height);

        movie_ctrl->hvedio_lyr = robin_request_video_layer(&vedio_rect, 0, 0xff);
        if (!movie_ctrl->hvedio_lyr)
        {
            __msg("robin_request_vedio_layer fail...\n");
            return EPDK_FAIL;
        }
        __msg("robin_request_vedio_layer success...\n");
    }

    if (is_on_tv())
    {
        __u32 arg[3] = {0};
        arg[0] = movie_ctrl->hvedio_lyr;
        eLIBs_fioctrl(movie_ctrl->hdisp, DISP_CMD_VIDEO_START, 0, (void *)arg);
    }

    {
        __s32 ret;
        __u32 arg[3] = {0};

        arg[0] = movie_ctrl->hvedio_lyr;
        ret = eLIBs_fioctrl(movie_ctrl->hdisp, DISP_CMD_LAYER_BOTTOM, 0, (void *)arg);
        if (0 != ret)
        {
            __msg("DISP_CMD_LAYER_BOTTOM fail...\n");
        }
        else
        {
            __msg("DISP_CMD_LAYER_BOTTOM success...\n");
        }
    }

    movie_ctrl->auto_play_timmer_id = 0x10;
    movie_ctrl->err_cnt             = 0;
    movie_ctrl->rr_flag             = 0;
    movie_ctrl->stop_cnt            = 2;
    movie_ctrl->cur_stop_cnt        = 0;
    movie_ctrl->cur_time            = 0;
    eLIBs_memset(movie_ctrl->cur_playing_file, 0, sizeof(movie_ctrl->cur_playing_file));
    movie_ctrl->quit_by_fs_part_plug_out    = EPDK_FALSE;

    __msg("__movie_para_init success...\n");
    return EPDK_OK;
}

static __s32 __movie_ctrl_para_deinit(movie_ctrl_t *movie_ctrl)
{
    if (!movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (is_on_tv())
    {
        __u32 arg[3] = {0};

        arg[0] = movie_ctrl->hvedio_lyr;
        eLIBs_fioctrl(movie_ctrl->hdisp, DISP_CMD_VIDEO_STOP, 0, (void *)arg);
    }

    if (movie_ctrl->hvedio_lyr)
    {
        robin_release_video_layer(movie_ctrl->hvedio_lyr);
        movie_ctrl->hvedio_lyr = NULL;
    }

    if (movie_ctrl->hdisp)
    {
        eLIBs_fclose(movie_ctrl->hdisp);
        movie_ctrl->hdisp = NULL;
    }

    return EPDK_OK;
}

static __s32 __movie_install_auto_play_timmer(movie_ctrl_t *movie_ctrl)
{
    if (!movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (!GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id))
    {
        GUI_SetTimer(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id
                     , 100, NULL);//0.5秒
    }
    else
    {
        __msg("timmer already install...\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __movie_reset_auto_play_timmer(movie_ctrl_t *movie_ctrl)
{
    if (!movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id))
    {
        GUI_ResetTimer(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id
                       , 100, NULL);//0.5秒
    }
    else
    {
        __msg("timmer already install...\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __movie_uninstall_auto_play_timmer(movie_ctrl_t *movie_ctrl)
{
    if (!movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id))
    {
        GUI_KillTimer(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id);
    }

    return EPDK_OK;
}

static __s32 __movie_load_breakpoint_info(__s32 index, __cedar_tag_inf_t *tag_inf)
{
    __s32 ret;
    char filename[RAT_MAX_FULL_PATH_LEN] = {0};

    ret = robin_npl_index2file(index, filename);
    if (-1 == ret)
    {
        __msg("robin_npl_index2file fail...\n");
        return -1;
    }

    __here__;
    if (!fifo_db_has_exist(NAME_MOVIE_BKPOINT))
        return -1;
    __here__;
    if (get_item_content_from_fifo_db(NAME_MOVIE_BKPOINT, filename, tag_inf))
        return -1;

    __here__;
    return 0;
}

static __s32 __movie_make_tag(movie_ctrl_t *movie_ctrl, __cedar_tag_inf_t *tag_inf)
{
    char cur_playing_file[RAT_MAX_FULL_PATH_LEN + 1] = {0};

    if(!movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if(!tag_inf)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    cur_playing_file[0] = 0;
    robin_get_fsm_cur_file(cur_playing_file, sizeof(cur_playing_file));
    __msg("cur_playing_file=%s\n", cur_playing_file);
    __msg("movie_ctrl->cur_playing_file=%s\n", movie_ctrl->cur_playing_file);
    if(0 == eLIBs_strcmp(cur_playing_file, movie_ctrl->cur_playing_file))
    {
        eLIBs_memset(tag_inf, 0, sizeof(__cedar_tag_inf_t));
        eLIBs_strncpy(tag_inf->tag_magic, "VTAG", 4);
        tag_inf->time_offset = movie_ctrl->cur_time;
        __msg("tag_inf->time_offset=%d\n", tag_inf->time_offset);
    }
    else
    {
        __here__;
        movie_ctrl->cur_time = 0;
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __movie_save_breakpoint_info(movie_ctrl_t *movie_ctrl, __s32 index)
{
    __cedar_tag_inf_t tag_inf;
    char filename[RAT_MAX_FULL_PATH_LEN] = {0};
    __s32 ret;
    __cedar_status_t  fsm_sta;

    if(!movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    fsm_sta = robin_get_fsm_status();
    __msg("fsm_sta=%d\n", fsm_sta);

    /*get the tag*/
    ret = robin_get_tag(&tag_inf);
    __msg("robin_get_tag, ret = %d\n", ret);

    if (0 != ret)//如果视频库获取断点信息失败，则自造一个断点信息
    {
        __msg("robin_get_tag fail...\n");
        return -1;
        if(EPDK_TRUE == movie_ctrl->quit_by_fs_part_plug_out)
        {
            ret = __movie_make_tag(movie_ctrl, &tag_inf);
            __msg("__movie_make_tag, ret = %d\n", ret);
            if(0 != ret)
            {
                return -1;
            }
        }
    }

    __here__;

    ret = robin_npl_index2file(index, filename);
    if (-1 == ret)
    {
        __msg("robin_npl_index2file fail...\n");
        return -1;
    }

    __msg("begin save breakpoint, index = %d, filename=%s\n", index, filename);

    if (!fifo_db_has_exist(NAME_MOVIE_BKPOINT))
    {
        ret = create_fifo_db(NAME_MOVIE_BKPOINT, 16, sizeof(__cedar_tag_inf_t));
        if (0 != ret)
        {
            __msg("create_fifo_db fail...\n");
            return -1;
        }
    }

    __here__;

    ret = add_item_to_fifo_db(NAME_MOVIE_BKPOINT, filename, &tag_inf);
    __msg("add_item_to_fifo_db, ret=%d\n", ret);

    return ret;
}


static __s32 __movie_delete_breakpoint_info(movie_ctrl_t *movie_ctrl, __s32 index)
{
    __s32 ret;
    char filename[RAT_MAX_FULL_PATH_LEN] = {0};
    __cedar_tag_inf_t tag_inf;

    if(!movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    ret = robin_npl_index2file(index, filename);
    if (-1 == ret)
    {
        __msg("robin_npl_index2file fail...\n");
        return -1;
    }

    if (!fifo_db_has_exist(NAME_MOVIE_BKPOINT))
        return -1;

    ret = get_item_content_from_fifo_db(NAME_MOVIE_BKPOINT, filename, &tag_inf);
    if(-1 == ret)
    {
        return 0;
    }

    if (delete_item_from_fifo_db(NAME_MOVIE_BKPOINT, filename))
        return -1;

    return 0;
}

static __s32 __movie_play_file(movie_ctrl_t *movie_ctrl, __s32 index, __bool breakplay)
{
    __s32 ret;
    char filename[RAT_MAX_FULL_PATH_LEN] = {0};
    __cedar_tag_inf_t tag_inf;

    ret = robin_npl_index2file(index, filename);
    if (-1 == ret)
    {
        __msg("robin_npl_index2file fail...\n");
        return EPDK_FAIL;
    }

    __msg("breakplay=%d\n", breakplay);
    __msg("filename=%s\n", filename);

    if(EPDK_TRUE == breakplay)
    {
        __msg("EPDK_TRUE == breakplay...\n");
        ret = __movie_load_breakpoint_info(index, &tag_inf);
        if (0 == ret)
        {
            __msg("has breakpoint info, begin breakplay... index = %d filename=%s\n", index, filename);
            robin_set_cmd_stop();
            __msg("tag_inf.time_offset = %d.\n", tag_inf.time_offset);

            eLIBs_printf("after stop11 free_size = %d byte.\n", esMEMS_FreeMemSize());
            eLIBs_printf("after stop11 total_size = %d byte.\n", esMEMS_TotalMemSize());

            //robin_clear_feedback_msgQ();//清空错误信息消息队列
            ret = robin_play_file(filename, &tag_inf);
            if(EPDK_FAIL == ret)
            {
                return EPDK_FAIL;
            }
            robin_set_cmd_play();
        }
        else
        {
            robin_set_cmd_stop();

            eLIBs_printf("after stop22 free_size = %d byte.\n", esMEMS_FreeMemSize());
            eLIBs_printf("after stop22 total_size = %d byte.\n", esMEMS_TotalMemSize());

            //robin_clear_feedback_msgQ();//清空错误信息消息队列
            ret = robin_play_file(filename, NULL);
            if(EPDK_FAIL == ret)
            {
                return EPDK_FAIL;
            }
            robin_set_cmd_play();
            __msg("wrn, no breakpoint info, not breakplay... index = %d filename=%s\n", index, filename);
        }
    }
    else
    {
        robin_set_cmd_stop();

        //eLIBs_printf("after stop33 free_size = %d byte.\n",esMEMS_FreeMemSize());
        //eLIBs_printf("after stop33 total_size = %d byte.\n",esMEMS_TotalMemSize());

        //robin_clear_feedback_msgQ();//清空错误信息消息队列
        ret = robin_play_file(filename, NULL);
        __msg("ret=%d\n", ret);
        if(EPDK_FAIL == ret)
        {
            return EPDK_FAIL;
        }
        robin_set_cmd_play();
        __msg("ret=%d\n", ret);
    }

    return ret;
}

static __s32 __movie_play_next(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;
    __s32 index;
    __s32 total;

    index = robin_npl_get_cur();
    if (-1 == index)
    {
        __msg("robin_npl_get_cur fail...\n");
        return EPDK_FAIL;
    }

    __movie_delete_breakpoint_info(movie_ctrl, index);

    total = robin_npl_get_total_count();
    if(-1 == total)
    {
        __msg("robin_npl_get_total_count fail...\n");
        return EPDK_FAIL;
    }

    index++;
    if(index >= total)
    {
        index = 0;
    }

    robin_npl_set_cur(index);

    ret = __movie_play_file(movie_ctrl, index, EPDK_FALSE);

    return ret;
}

static __s32 __movie_play_prev(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;
    __s32 index;
    __s32 total;

    index = robin_npl_get_cur();
    if (-1 == index)
    {
        __msg("robin_npl_get_cur fail...\n");
        return EPDK_FAIL;
    }

    total = robin_npl_get_total_count();
    if(-1 == total)
    {
        __msg("robin_npl_get_total_count fail...\n");
        return EPDK_FAIL;
    }

    index--;
    if(index < 0)
    {
        index = total - 1;
    }

    if(index < 0)
    {
        __msg("total movie count = 0\n");
        index = 0;
    }

    robin_npl_set_cur(index);

    ret = __movie_play_file(movie_ctrl, index, EPDK_FALSE);

    return ret;
}

static __s32 __movie_cur_file_has_breakpoint(void)
{
    __s32 ret;
    __s32 index;
    __cedar_tag_inf_t tag_inf;

    index = robin_npl_get_cur();
    if (-1 == index)
    {
        __msg("robin_npl_get_cur fail...\n");
        return EPDK_FALSE;
    }

    __msg("before __movie_load_breakpoint_info\n");
    ret = __movie_load_breakpoint_info(index, &tag_inf);
    if (0 == ret)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}

static __s32 __movie_start_play(movie_ctrl_t *movie_ctrl, __bool breakplay)
{
    __s32 ret;
    __s32 index;

    index = robin_npl_get_cur();
    if (-1 == index)
    {
        __msg("robin_npl_get_cur fail...\n");
        return EPDK_FAIL;
    }

    ret = __movie_play_file(movie_ctrl, index, breakplay);

    return ret;
}

static __s32 __movie_auto_play_next(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;
    __s32 index;
    __s32 total;

    index = robin_npl_get_cur();
    if(-1 != index)
    {
        __here__;
        __movie_delete_breakpoint_info(movie_ctrl, index);
        __here__;
    }

    index = robin_npl_get_next();
    if (-1 == index)
    {
        __msg("robin_npl_get_cur fail...\n");
        return EPDK_FAIL;
    }
#if EPDK_PLAY_VIDEO_AUDIO_ENABLE
    {
        rat_media_type_t media_type;
        char	filename[256] ;
        robin_npl_index2file( index, filename);
        media_type = rat_get_file_type( filename );
        DebugPrintf("filename=%s,media_type", filename, media_type);
        if( media_type == RAT_MEDIA_TYPE_AUDIO )
        {
            movie_cmd2parent(movie_ctrl->movie_manager, SWITCH_TO_OTHER_APP, VIDEO_SW_TO_AUDIO, 0 );
        }
    }
    return EPDK_OK;
#endif
    __movie_play_file(movie_ctrl, index, EPDK_FALSE);

    //__msg("ret=%d\n", ret);
    return EPDK_OK;//强制返回ok，cedar_set_play可能返回失败
}


static __s32 __movie_stop_play(movie_ctrl_t *movie_ctrl)
{
    robin_set_cmd_stop();

    return EPDK_OK;
}

static __s32 __movie_pause_play(movie_ctrl_t *movie_ctrl)
{
    robin_set_cmd_pause();

    return EPDK_OK;
}


//flag:0表示退出到explorer，1表示退出到home
static __s32 __movie_exit_movie(movie_ctrl_t *movie_ctrl, __s32 flag)
{
    __msg("before __movie_exit_movie\n");

    if (!movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (movie_ctrl->exit_movie)
    {
        __msg("movie already exit...\n");
        return EPDK_FAIL;
    }

    movie_ctrl->exit_movie = 1;

    if (!movie_ctrl->movie_manager)
    {
        __err("movie_ctrl->movie_manager is null...\n");
        return EPDK_FAIL;
    }

    __movie_uninstall_auto_play_timmer(movie_ctrl);
    //__movie_pause_play(movie_ctrl);视频库修改了，要求获取断点信息的时候只能是play状态，所以不能暂停

    {
        __gui_msg_t msg;

        eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
        msg.id = GUI_MSG_CLOSE;
        msg.dwAddData1 = flag;
        msg.h_deswin = movie_ctrl->movie_manager;
        GUI_SendMessage(&msg);
    }

    __msg("after __movie_exit_movie\n");

    return EPDK_OK;
}

static __s32 __movie_check_for_play_err(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    ret = robin_get_feedback_msg();
    if (ret < 0)
    {
        robin_clear_feedback_msgQ();//清空错误信息消息队列
        __msg("robin_get_feedback_msg < 0, movie play err...\n");
        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
        __movie_playerror_scene_create(movie_ctrl);
        esKRNL_TimeDly(100);
        __movie_playerror_scene_delete(movie_ctrl);

        movie_ctrl->err_cnt++;

        if(movie_ctrl->err_cnt >= robin_npl_get_total_count()
                || RAT_PLAY_MODE_ROTATE_ONE == robin_get_play_mode())
        {
            __movie_exit_movie(movie_ctrl, 0);
        }

        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __movie_check_for_auto_play(movie_ctrl_t *movie_ctrl)
{
    if (!movie_ctrl)
    {
        //__err("invalid para...\n");
        //__log("--jh_dbg1024_20---\n");
        return EPDK_FAIL;
    }

    {
        __cedar_status_t fsm_sta;

        //eLIBs_printf("VE PLL=%x\n",*((volatile __u32*)(0xf1c20000+0x18)));
        //__log("--total_time=%d---",robin_get_total_time());

        fsm_sta = robin_get_fsm_status();
        //__log("fsm_sta= %d\n", fsm_sta);
        if(CEDAR_STAT_PLAY == fsm_sta
                || CEDAR_STAT_PAUSE == fsm_sta)
        {
            movie_ctrl->cur_time = robin_get_cur_time();
            robin_get_fsm_cur_file(movie_ctrl->cur_playing_file, sizeof(movie_ctrl->cur_playing_file));
        }
        else
        {
            movie_ctrl->cur_time = -1;
            eLIBs_strcpy(movie_ctrl->cur_playing_file, "");
        }

        switch(fsm_sta)
        {
        case CEDAR_STAT_STOP:
        {
            __s32 ret;

            movie_ctrl->cur_stop_cnt++;
            if(movie_ctrl->cur_stop_cnt < movie_ctrl->stop_cnt)
            {
                // __log("---jh_dbg1024_11:%d---\n", movie_ctrl->cur_stop_cnt);
                break;
            }
            movie_ctrl->cur_stop_cnt = 0;
            //__log("---jh_dbg1024_12---\n");

            //__msg("CEDAR_STAT_STOP, start play next...\n");
            //  __msg("movie_ctrl->rr_flag=%d\n", movie_ctrl->rr_flag);
            if(1 == movie_ctrl->rr_flag)
            {
                __s32 index;

                index = robin_npl_get_cur();
                //__msg("index=%d\n", index);
                if(-1 != index)
                {
                    __movie_play_file(movie_ctrl, index, EPDK_FALSE);
                    // __log("---jh_dbg1024_-1---\n");
                }
                // __log("---jh_dbg1024_0---\n");
                //movie_ctrl->rr_flag = 0;
            }
            else
            {
                //__log("---jh_dbg1024_1---\n");
                ret = __movie_auto_play_next(movie_ctrl);
                __msg("ret=%d\n", ret);
                if (EPDK_OK != ret)//播放完毕,退出应用
                {
                    //__log("---jh_dbg1024_2---\n");
                    __movie_playend_scene_create(movie_ctrl);
                    esKRNL_TimeDly(100);
                    __movie_exit_movie(movie_ctrl, 0);
                }
                else
                {
                    {
                        //通知子场景更新UI
                        __gui_msg_t msg;
                        //__log("---jh_dbg1024_3---\n");

                        eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
                        __msg("movie_ctrl->spsc_scene=%x\n", movie_ctrl->spsc_scene);
                        if(movie_ctrl->spsc_scene)
                        {
                            msg.h_deswin = movie_spsc_scene_get_hwnd(movie_ctrl->spsc_scene);
                            if (msg.h_deswin)
                            {
                                msg.id = movie_spsc_scene_update_fileinfo;
                                msg.dwAddData1 = 0;
                                GUI_SendMessage(&msg);
                            }
                        }
                    }
                }
            }

            break;
        }
        case CEDAR_STAT_PLAY:
        {
            movie_ctrl->rr_flag = 0;
            //__log("--jh_dbg1024_21---\n");
            //  __msg("movie_ctrl->rr_flag=%d\n", movie_ctrl->rr_flag);

            {
                //存在能正常播放的文件，把该值复位一下
                movie_ctrl->err_cnt = 0;
            }

            break;
        }
        case CEDAR_STAT_RR:
        {
            movie_ctrl->rr_flag = 1;
            //__log("--jh_dbg1024_22---\n");
            //  __msg("movie_ctrl->rr_flag=%d\n", movie_ctrl->rr_flag);
            break;
        }
        default:
        {
            movie_ctrl->rr_flag = 0;
            //__log("--jh_dbg1024_23---\n");
            // __msg("fsm_sta=%d, movie_ctrl->rr_flag=%d\n", fsm_sta, movie_ctrl->rr_flag);
            break;
        }
        }
    }

    return EPDK_OK;
}

static __s32 __movie_spsc_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        movie_spsc_create_para_t create_para;
        eLIBs_memset(&create_para, 0, sizeof(movie_spsc_create_para_t));
        create_para.hparent = movie_ctrl->movie_manager;
        create_para.scene_id = MOVIE_SPSC_ID;
        movie_ctrl->spsc_scene = movie_spsc_scene_create(&create_para);
        if (NULL == movie_ctrl->spsc_scene)
        {
            __msg("movie_spsc_scene_create fail...\n");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_spsc_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->spsc_scene)
        {
            movie_spsc_scene_delete(movie_ctrl->spsc_scene);
            movie_ctrl->spsc_scene = NULL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_volume_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        movie_volume_create_para_t create_para;
        eLIBs_memset(&create_para, 0, sizeof(movie_volume_create_para_t));
        create_para.hparent = movie_ctrl->movie_manager;
        create_para.scene_id = MOVIE_VOLUME_ID;
        create_para.max_val = AUDIO_DEVICE_VOLUME_MAX;
        create_para.min_val = AUDIO_DEVICE_VOLUME_MIN;
        create_para.cur_val = dsk_volume_get();
        movie_ctrl->volume_scene = movie_volume_scene_create(&create_para);
        if (NULL == movie_ctrl->volume_scene)
        {
            __msg("movie_volume_scene_create fail...\n");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_volume_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->volume_scene)
        {
            movie_volume_scene_delete(movie_ctrl->volume_scene);
            movie_ctrl->volume_scene = NULL;
        }
    }

    return EPDK_OK;
}


static __s32 __movie_volume_scene_set_focus(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->volume_scene)
        {
            movie_volume_scene_set_focus(movie_ctrl->volume_scene);
        }
    }

    return EPDK_OK;
}

static __s32 __movie_bright_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        movie_bright_create_para_t create_para;
        eLIBs_memset(&create_para, 0, sizeof(movie_bright_create_para_t));
        create_para.hparent = movie_ctrl->movie_manager;
        create_para.scene_id = MOVIE_BRIGHT_ID;
        create_para.max_val = LION_BRIGHT_LEVEL16;
        create_para.min_val = LION_BRIGHT_LEVEL1;
        create_para.cur_val = dsk_display_get_lcd_bright();
        movie_ctrl->bright_scene = movie_bright_scene_create(&create_para);
        if (NULL == movie_ctrl->bright_scene)
        {
            __msg("movie_bright_scene_create fail...\n");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_bright_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->bright_scene)
        {
            movie_bright_scene_delete(movie_ctrl->bright_scene);
            movie_ctrl->bright_scene = NULL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_bright_scene_set_focus(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->bright_scene)
        {
            movie_bright_scene_set_focus(movie_ctrl->bright_scene);
        }
    }

    return EPDK_OK;
}

static __s32 __movie_playsta_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (NULL == movie_ctrl->playsta_scene)
    {
        movie_playsta_create_para_t create_para;
        eLIBs_memset(&create_para, 0, sizeof(movie_playsta_create_para_t));
        create_para.hparent = movie_ctrl->movie_manager;
        create_para.scene_id = MOVIE_PLAYSTA_ID;
        create_para.playsta = movie_playsta_pause;
        movie_ctrl->playsta_scene = movie_playsta_scene_create(&create_para);
        if (NULL == movie_ctrl->playsta_scene)
        {
            __msg("movie_playsta_scene_create fail...\n");
            return EPDK_FAIL;
        }
        movie_playsta_scene_set_focus(movie_ctrl->playsta_scene);
    }

    return EPDK_OK;
}

static __s32 __movie_playsta_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->playsta_scene)
        {
            movie_playsta_scene_delete(movie_ctrl->playsta_scene);
            movie_ctrl->playsta_scene = NULL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_playsta_scene_set_focus(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->playsta_scene)
        {
            movie_playsta_scene_set_focus(movie_ctrl->playsta_scene);
        }
    }

    return EPDK_OK;
}

static __s32 __movie_sub_show_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        movie_sub_show_create_para_t create_para;
        eLIBs_memset(&create_para, 0, sizeof(movie_sub_show_create_para_t));
        create_para.hparent  = movie_ctrl->movie_manager;
        create_para.scene_id = MOVIE_SUB_SHOW_ID;
        create_para.sub_pos  = movie_ctrl->sub_pos;
        create_para.sub_color = movie_ctrl->sub_color;
        create_para.sub_size = movie_ctrl->sub_size;
        movie_ctrl->sub_show_scene = movie_sub_show_scene_create(&create_para);
        if (NULL == movie_ctrl->sub_show_scene)
        {
            __msg("movie_sub_show_scene_create fail...\n");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_sub_show_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->sub_show_scene)
        {
            movie_sub_show_scene_delete(movie_ctrl->sub_show_scene);
            movie_ctrl->sub_show_scene = NULL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_sub_set_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        movie_sub_set_create_para_t create_para;
        eLIBs_memset(&create_para, 0, sizeof(movie_sub_set_create_para_t));
        create_para.hparent = movie_ctrl->movie_manager;
        create_para.scene_id = MOVIE_SUB_SET_ID;
        create_para.sub_state = movie_ctrl->sub_state;
        create_para.sub_color = movie_sub_set_color2index(movie_ctrl->sub_color);
        create_para.sub_pos =  movie_ctrl->sub_pos;
        create_para.sub_stream =  movie_ctrl->sub_stream;
        movie_ctrl->sub_set_scene = movie_sub_set_scene_create(&create_para);
        if (NULL == movie_ctrl->sub_set_scene)
        {
            __msg("movie_sub_set_scene_create fail...\n");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_sub_set_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->sub_set_scene)
        {
            movie_sub_set_scene_delete(movie_ctrl->sub_set_scene);
            movie_ctrl->sub_set_scene = NULL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_sub_set_scene_set_focus(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->sub_set_scene)
        {
            movie_sub_set_scene_set_focus(movie_ctrl->sub_set_scene);
        }
    }

    return EPDK_OK;
}

static __s32 __movie_prog_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        movie_prog_create_para_t create_para;
        eLIBs_memset(&create_para, 0, sizeof(movie_prog_create_para_t));
        create_para.hparent = movie_ctrl->movie_manager;
        create_para.scene_id = MOVIE_PROG_ID;

        movie_ctrl->prog_scene = movie_prog_scene_create(&create_para);
        if (NULL == movie_ctrl->prog_scene)
        {
            __msg("movie_volume_scene_create fail...\n");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_prog_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->prog_scene)
        {
            movie_prog_scene_delete(movie_ctrl->prog_scene);
            movie_ctrl->prog_scene = NULL;
        }
    }

    return EPDK_OK;
}


static __s32 __movie_prog_scene_set_focus(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->prog_scene)
        {
            movie_prog_scene_set_focus(movie_ctrl->prog_scene);
        }
    }

    return EPDK_OK;
}


static __s32 __movie_breakplay_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }
#if 0
    {
        if (NULL == movie_ctrl->breakplay_scene)
        {
            __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_MOVIE_BREAKPLAY};

            default_dialog(movie_ctrl->breakplay_scene, movie_ctrl->movie_manager, MOVIE_BREAKPLAY_DIALOG_ID, ADLG_YESNO, lang_id);
        }

        //movie_ctrl->breakplay_scene = create_breakplay_dlg();
        if (NULL == movie_ctrl->breakplay_scene)
        {
            __msg("default_dialog fail...\n");
            return EPDK_FAIL;
        }
    }
#endif
    return EPDK_OK;
}

static __s32 __movie_breakplay_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->breakplay_scene)
        {
            app_dialog_destroy(movie_ctrl->breakplay_scene);
            movie_ctrl->breakplay_scene = NULL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_playerror_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (NULL == movie_ctrl->playerror_scene)
        {
            __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_MOVIE_FILE_NOT_SUPPORT};
			//default_dialog(movie_ctrl->playerror_scene, movie_ctrl->movie_manager, MOVIE_PLAYERROR_DIALOG_ID, ADLG_YESNO, lang_id);
            default_dialog(movie_ctrl->playerror_scene, movie_ctrl->movie_manager, MOVIE_PLAYERROR_DIALOG_ID, ADLG_OK, lang_id);
        }

        //movie_ctrl->breakplay_scene = create_breakplay_dlg();
        if (NULL == movie_ctrl->playerror_scene)
        {
            __msg("default_dialog fail...\n");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_playerror_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->playerror_scene)
        {
            app_dialog_destroy(movie_ctrl->playerror_scene);
            movie_ctrl->playerror_scene = NULL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_playend_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (NULL == movie_ctrl->playend_scene)
        {
            __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_MOVIE_PLAY_END};

            default_dialog(movie_ctrl->playend_scene, movie_ctrl->movie_manager, MOVIE_PLAYEND_DIALOG_ID, ADLG_OK, lang_id);
        }

        if (NULL == movie_ctrl->playend_scene)
        {
            __msg("default_dialog fail...\n");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_playend_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->playend_scene)
        {
            app_dialog_destroy(movie_ctrl->playend_scene);
            movie_ctrl->playend_scene = NULL;
        }
    }

    return EPDK_OK;
}

//判断movie manwin是否有指定的子场景之一
static __s32 __movie_has_sub_scene(movie_ctrl_t *movie_ctrl, __u32 sub_id)
{
    H_WIN child;
    char  winname[256];
    __u32 child_id;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FALSE;
    }

    if (!movie_ctrl->movie_manager)
    {
        __err("invalid para...\n");
        return EPDK_FALSE;
    }

    child = GUI_WinGetFirstChild(movie_ctrl->movie_manager);

    while( child != NULL )
    {
        child_id = GUI_WinGetItemId(child);
        if ((child_id & sub_id))
        {
            return EPDK_TRUE;
        }

        child = GUI_WinGetNextBro(child);
    }

    return EPDK_FALSE;
}

//删除movie的子场景
static __s32 __movie_delete_subscene_by_id(movie_ctrl_t *movie_ctrl, __u32 sub_id)
{
    H_WIN child, next_child;
    char  winname[256];
    __u32 child_id;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (!movie_ctrl->movie_manager)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    child = GUI_WinGetFirstChild(movie_ctrl->movie_manager);

    while( child != NULL )
    {
        eLIBs_memset(winname, 0, 256);
        GUI_WinGetName(child, winname);

        next_child = GUI_WinGetNextBro(child);

        child_id = GUI_WinGetItemId(child);
        if ((child_id & sub_id))//需要删除
        {
            __msg("begin delete movie sub scene, winname = %s \n", winname);

            switch(child_id)
            {
            case MOVIE_SPSC_ID:
            {
                __movie_spsc_scene_delete(movie_ctrl);
                break;
            }
            case MOVIE_VOLUME_ID:
            {
                __movie_volume_scene_delete(movie_ctrl);
                break;
            }
            case MOVIE_BRIGHT_ID:
            {
                __movie_bright_scene_delete(movie_ctrl);
                break;
            }
            case MOVIE_PLAYSTA_ID:
            {
                __movie_playsta_scene_delete(movie_ctrl);
                break;
            }
            case MOVIE_SUB_SHOW_ID:
            {
                __movie_sub_show_scene_delete(movie_ctrl);
                break;
            }
            case MOVIE_SUB_SET_ID:
            {
                __movie_sub_set_scene_delete(movie_ctrl);
                break;
            }
            case MOVIE_PROG_ID:
            {
                __movie_prog_scene_delete(movie_ctrl);
                break;
            }
            case MOVIE_BREAKPLAY_DIALOG_ID:
            {
                __movie_breakplay_scene_delete(movie_ctrl);
                break;
            }
            case MOVIE_PLAYERROR_DIALOG_ID:
            {
                __movie_playerror_scene_delete(movie_ctrl);
                break;
            }
            case MOVIE_PLAYEND_DIALOG_ID:
            {
                __movie_playend_scene_delete(movie_ctrl);
                break;
            }
            default:
            {
                __err("unknown child id, child_id=%d\n", child_id);
                return EPDK_FAIL;
            }
            }
        }

        child = next_child;
    }

    return EPDK_OK;
}

static __s32 __movie_delete_scene_berore_switch_tv(movie_ctrl_t *movie_ctrl)
{
    //删除场景和部分资源
    __here__;
    __movie_uninstall_auto_play_timmer(movie_ctrl);

    {
        //save breakpoint
        __s32 index;

        index = robin_npl_get_cur();
        __here__;
        if(-1 != index)
        {
            __here__;
            __movie_save_breakpoint_info(movie_ctrl, index);
        }
    }

    __here__;
    __movie_stop_play(movie_ctrl);

    __here__;
    __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL);

    __here__;
    __movie_ctrl_para_deinit(movie_ctrl);

    robin_close();

    return EPDK_OK;
}

static __s32 __movie_create_scene_after_switch_tv(movie_ctrl_t *movie_ctrl)
{
    //创建部分场景和资源
    __movie_ctrl_para_init(movie_ctrl);

    __app_movie_reg_para_init(movie_ctrl);

    __movie_start_play(movie_ctrl, EPDK_TRUE);

    if(1 == movie_ctrl->sub_state)
    {
        __movie_sub_show_scene_create(movie_ctrl);
    }

    __movie_install_auto_play_timmer(movie_ctrl);

    return EPDK_OK;
}

static __s32 __movie_switch_output(movie_ctrl_t *movie_ctrl)
{
    //切换输出
    {
        reg_system_para_t *para;

        para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
        if (para)
        {
            __msg("para->pal_ntsc = %d\n", para->pal_ntsc);
            if (0 == para->pal_ntsc)
            {
                if (para->output == LION_DISP_LCD)
                {
                    g_display_switch_output(LION_DISP_TV_NTSC);
                }
                else
                {
                    g_display_switch_output(LION_DISP_LCD);
                }
            }
            else
            {
                if (para->output == LION_DISP_LCD)
                {
                    g_display_switch_output(LION_DISP_TV_PAL);
                }
                else
                {
                    g_display_switch_output(LION_DISP_LCD);
                }
            }
        }
    }

    return EPDK_OK;
}

static __s32 __movie_switch_to_tv_or_lcd(movie_ctrl_t *movie_ctrl)
{
    __movie_delete_scene_berore_switch_tv(movie_ctrl);
    __movie_switch_output(movie_ctrl);
    __movie_create_scene_after_switch_tv(movie_ctrl);
    return EPDK_OK;
}

static __s32 __spsc_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
    switch(msg_id)
    {
    case spsc_scene_msg_timeout:
    {
        __s32 ret;

        __msg("spsc_scene_msg_timeout\n");

        //__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL&(~MOVIE_SUB_SHOW_ID));

        {
            //如果处于暂停状态，则创建播放状态子场景
            __cedar_status_t  status;

            status = robin_get_fsm_status();
            if(CEDAR_STAT_PAUSE == status)
            {
                __msg("spsc time out, cedar in pause status, create play/pause scene...\n");
                //jh_0224
#if 0
                __movie_playsta_scene_create(movie_ctrl);
#endif
            }
        }

        return EPDK_OK;
    }
    case spsc_scene_msg_brightset:
    {
        if(is_on_tv())
        {
            __msg("on tv, not adjust bright scene...\n");
            return EPDK_OK;
        }
        __msg("before spsc_scene_msg_brightset\n");

        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));

        __movie_bright_scene_create(movie_ctrl);
        __msg("__movie_bright_scene_create\n");

        __movie_bright_scene_set_focus(movie_ctrl);
        __msg("__movie_bright_scene_set_focus\n");

        __msg("after spsc_scene_msg_brightset\n");
        return EPDK_OK;
    }
    case spsc_scene_msg_volumeset:
    {
        __msg("before spsc_scene_msg_volumeset\n");

        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));

        __movie_volume_scene_create(movie_ctrl);
        __msg("__movie_volume_scene_create\n");

        __movie_volume_scene_set_focus(movie_ctrl);
        __msg("__movie_volume_scene_set_focus\n");

        __msg("after spsc_scene_msg_volumeset\n");

        return EPDK_OK;
    }
    case spsc_scene_msg_tvout:
    {
        __msg("before __movie_switch_to_tv_or_lcd\n");
        __movie_switch_to_tv_or_lcd(movie_ctrl);
        __msg("after __movie_switch_to_tv_or_lcd\n");

        return EPDK_OK;
    }
    case spsc_scene_msg_rr:
    {
        movie_ctrl->rr_flag = 1;

        return EPDK_OK;
    }
    default:
    {
        break;
    }
    }

    return EPDK_FAIL;
}

static __s32 __volume_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
    switch(msg_id)
    {
    case movie_volume_scene_msg_timeout:
    {
        __msg("movie_volume_scene_msg_timeout\n");
        if (movie_ctrl->volume_scene)
        {
            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
        }
        return EPDK_OK;
    }
    case movie_volume_scene_msg_volumeset:
    {
        __msg("movie_volume_scene_msg_volumeset, dwAddData2=%d\n", dwAddData2);
        dsk_volume_set(dwAddData2);
        return EPDK_OK;
    }
    default:
    {
        break;
    }
    }

    return EPDK_FAIL;
}

static __s32 __bright_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
    switch(msg_id)
    {
    case movie_bright_scene_msg_timeout:
    {
        __msg("movie_bright_scene_msg_timeout\n");
        if (movie_ctrl->bright_scene)
        {
            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
        }
        return EPDK_OK;
    }
    case movie_bright_scene_msg_brightset:
    {
        __msg("movie_bright_scene_msg_brightset, dwAddData2=%d\n", dwAddData2);
        dsk_display_set_lcd_bright(dwAddData2);
        return EPDK_OK;
    }
    default:
    {
        break;
    }
    }

    return EPDK_FAIL;
}

static __s32 __playsta_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
    switch(msg_id)
    {
    case movie_playsta_scene_msg_timeout:
    {
        __s32 ret;

        __msg("movie_playsta_scene_msg_timeout\n");
        ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PLAYSTA_ID);

        if (EPDK_TRUE == ret)
        {
            __here__;
            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
        }
        return EPDK_OK;
    }
    case movie_playsta_scene_msg_clicked:
    {
        __msg("movie_playsta_scene_msg_playstaset\n");
        if (movie_playsta_play == dwAddData2)
        {
            robin_set_cmd_play();
        }
        else if(movie_playsta_pause == dwAddData2)
        {
            robin_set_cmd_pause();
        }
        else
        {
            __err("dwAddData2 invalid...\n");
        }
        return EPDK_OK;
    }
    default:
    {
        break;
    }
    }

    return EPDK_FAIL;
}

static __s32 __sub_set_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
    reg_movie_para_t *para;

    para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);

    switch(msg_id)
    {
    case sub_set_scene_msg_timeout:
    {
        __s32 ret;

        __msg("sub_set_scene_msg_timeout\n");
        ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SET_ID);

        if (EPDK_TRUE == ret)
        {
            __here__;
            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
        }
        return EPDK_OK;
    }
    case sub_set_scene_msg_sub_state:
    {
        __msg("sub_set_scene_msg_sub_state\n");
        if (0 == dwAddData2)
        {
            __s32 ret;

            ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SHOW_ID);
            if(EPDK_TRUE == ret)
            {
                __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SHOW_ID);
            }
        }
        else if(1 == dwAddData2)
        {
            __s32 ret;

            ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SHOW_ID);
            if(EPDK_FALSE == ret)
            {
                __movie_sub_show_scene_create(movie_ctrl);
            }
            else
            {
                __msg("sub_show scene alread on...\n");
            }
        }
        else
        {
            __err("dwAddData2 invalid...\n");
        }

        movie_ctrl->sub_state = dwAddData2;
        if(para)
        {
            para->sub_state = dwAddData2;
        }

        if(movie_ctrl->sub_set_scene)
        {
            movie_sub_set_scene_set_lyr_top(movie_ctrl->sub_set_scene);
        }
        return EPDK_OK;
    }
    case sub_set_scene_msg_sub_color:
    {
        __msg("sub_set_scene_msg_sub_color, dwAddData2=%x\n", dwAddData2);
        movie_ctrl->sub_color = dwAddData2;
        if(1 == movie_ctrl->sub_state)//如果存在字幕场景，则需删除后重键，使得其颜色变化
        {
            __s32 ret;

            ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SHOW_ID);
            if(EPDK_TRUE == ret)
            {
                __movie_sub_show_scene_delete(movie_ctrl);
                __movie_sub_show_scene_create(movie_ctrl);
            }
        }
        if(para)
        {
            para->sub_color = dwAddData2;
        }

        if(movie_ctrl->sub_set_scene)
        {
            movie_sub_set_scene_set_lyr_top(movie_ctrl->sub_set_scene);
        }

        return EPDK_OK;
    }
    case sub_set_scene_msg_sub_pos:
    {
        __msg("sub_set_scene_msg_sub_pos, dwAddData2=%d\n", dwAddData2);
        if (movie_ctrl->sub_pos == dwAddData2)
        {
            __msg("sub pos not need change...\n");
            return EPDK_OK;
        }

        movie_ctrl->sub_pos = dwAddData2;

        if(1 == movie_ctrl->sub_state)//如果存在字幕场景，则需删除后重键，使得其颜色变化
        {
            if(dwAddData2 >= 0 || dwAddData2 < 2)
            {
                __s32 ret;

                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SHOW_ID);
                if(EPDK_TRUE == ret)
                {
                    __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SHOW_ID);
                }

                __movie_sub_show_scene_create(movie_ctrl);
            }
            else
            {
                __err("dwAddData2 invalid...\n");
            }
        }

        if(para)
        {
            para->sub_pos = dwAddData2;
        }

        if(movie_ctrl->sub_set_scene)
        {
            movie_sub_set_scene_set_lyr_top(movie_ctrl->sub_set_scene);
        }

        return EPDK_OK;
    }
    case sub_set_scene_msg_sub_stream:
    {
        eLIBs_printf("sub_set_scene_msg_sub_stream, dwAddData2=%d\n", dwAddData2);
        if (movie_ctrl->sub_stream == dwAddData2)
        {
            __msg("sub stream not need change...\n");
            return EPDK_OK;
        }

        movie_ctrl->sub_stream = dwAddData2;

        robin_select_subtitle(dwAddData2);

        if(para)
        {
            para->sub_stream = dwAddData2;
        }
        return EPDK_OK;
    }
    default:
    {
        break;
    }
    }

    return EPDK_FAIL;
}
// 快进快退停止后从这里返回
static __s32 __prog_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{

    switch(msg_id)
    {
    case movie_prog_scene_msg_timeout:
    {
        __s32 ret;

        __msg("movie_prog_scene_msg_timeout\n");
        ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID);

        if (EPDK_TRUE == ret)
        {
            __here__;
            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
            //快进快退图标消失后，重新打开时间
            __movie_spsc_scene_create(movie_ctrl);
        }
        return EPDK_OK;
    }
    default:
    {
        break;
    }
    }

    return EPDK_FAIL;
}

static __s32 __movie_process_after_start_play(movie_ctrl_t *movie_ctrl)
{

    {
        //创建字幕

        if(1 == movie_ctrl->sub_state)
        {
            __movie_sub_show_scene_create(movie_ctrl);
        }
    }

    __movie_install_auto_play_timmer(movie_ctrl);

    //__movie_spsc_scene_create(movie_ctrl);//刚进入视频时，不显示控制菜单

    return EPDK_OK;
}

static __s32 __breakplay_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
    switch(msg_id)
    {
    case ADLG_CMD_EXIT:
    {
        if(ADLG_IDYES == dwAddData2)//断点播放
        {
            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL);

            __movie_start_play(movie_ctrl, EPDK_TRUE);

            __movie_process_after_start_play(movie_ctrl);

            return EPDK_OK;
        }
        else//非断点播放
        {

            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL);

            __movie_start_play(movie_ctrl, EPDK_FALSE);

            __movie_process_after_start_play(movie_ctrl);

            return EPDK_OK;
        }
    }
    default:
    {
        break;
    }
    }

    return EPDK_FAIL;
}

static __s32 __movie_save_last_playing_file(__gui_msg_t *msg)
{
    __s32 index;
    char file[RAT_MAX_FULL_PATH_LEN] = {0};
    root_para_t *root_para;

    root_para = (root_para_t *)GUI_WinGetAttr(msg->h_deswin);
    index = robin_npl_get_cur();
    if(-1 != index)
    {
        __s32 ret;

        ret = robin_npl_index2file(index, file);
        if(EPDK_OK == ret)
        {
            ret = dsk_reg_save_cur_play_info(REG_APP_MOVIE, index,
                                             file, root_para->root_type);
            if(EPDK_OK == ret)
            {
                __msg("dsk_reg_save_cur_play_info success,index=%d, file=%s, root=%d\n",
                      index, file, root_para->root_type);
                return EPDK_OK;
            }
            else
            {
                __msg("dsk_reg_save_cur_play_info fail...\n");
                return EPDK_FAIL;
            }
        }
        else
        {
            __msg("robin_npl_index2file fail...\n");
            return EPDK_FAIL;
        }
    }

    return EPDK_FAIL;
}

static void __app_movie_set_play_mode(void)
{
    reg_movie_para_t *para;

    if(dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
    {
        __msg("load default setting...\n");
        para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
    }
    else
    {
        __msg("load current setting...\n");
        para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
    }

    if(para)
    {
        robin_set_play_mode(para->rotate_mode);
        __msg("para->rotate_mode=%d\n", para->rotate_mode);
    }
    else
    {
        robin_set_play_mode(RAT_PLAY_MODE_ROTATE_ALL);
    }
}

static void __app_movie_set_zoom_mode(void)
{
    reg_movie_para_t *para;

    if(dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
    {
        __msg("load default setting...\n");
        para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
    }
    else
    {
        __msg("load current setting...\n");
        para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
    }

    if(para)
    {
        robin_set_zoom(para->zoom_mode);
        __msg("para->zoom_mode=%d\n", para->zoom_mode);
    }
}

static __u32 __app_movie_get_sub_show_state(void)
{
    reg_movie_para_t *para;

    if(dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
    {
        __msg("load default setting...\n");
        para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
    }
    else
    {
        __msg("load current setting...\n");
        para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
    }

    if(para)
    {
        __msg("para->sub_state=%d\n", para->sub_state);
        return para->sub_state;
    }
    else
    {
        return 1;
    }
}

static __u32 __app_movie_get_sub_stream_index(void)
{
    reg_movie_para_t *para;

    if(dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
    {
        __msg("load default setting...\n");
        para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
    }
    else
    {
        __msg("load current setting...\n");
        para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
    }

    if(para)
    {
        __msg("para->sub_stream=%d\n", para->sub_stream);
        return para->sub_stream;
    }
    else
    {
        return 1;
    }
}

static __u32 __app_movie_get_sub_show_pos(void)
{
    reg_movie_para_t *para;

    if(dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
    {
        __msg("load default setting...\n");
        para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
    }
    else
    {
        __msg("load current setting...\n");
        para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
    }

    if(para)
    {
        __msg("para->sub_pos=%d\n", para->sub_pos);
        return para->sub_pos;
    }
    else
    {
        return 0;
    }
}

static __u32 __app_movie_get_sub_show_color(void)
{
    reg_movie_para_t *para;

    if(dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
    {
        __msg("load default setting...\n");
        para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
    }
    else
    {
        __msg("load current setting...\n");
        para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
    }

    if(para)
    {
        __msg("para->sub_color=%x\n", para->sub_color);
        return para->sub_color;
    }
    else
    {
        return GUI_WHITE;
    }
}

static __s32 __app_movie_reg_para_init(movie_ctrl_t *movie_ctrl)
{
    __app_movie_set_play_mode();
    __app_movie_set_zoom_mode();
    movie_ctrl->sub_pos   = __app_movie_get_sub_show_pos();
    movie_ctrl->sub_color = __app_movie_get_sub_show_color();
    movie_ctrl->sub_state = __app_movie_get_sub_show_state();
    movie_ctrl->sub_stream = __app_movie_get_sub_stream_index();
    movie_ctrl->sub_size  = 16;

    dsk_reg_set_app_restore_flag(REG_APP_MOVIE, 0);

    return EPDK_OK;
}

static void dsk_display_set_FLICKER()
{
    ES_FILE *fp;

    fp = eLIBs_fopen("b:\\disp\\display", "r+");
    if(fp)
    {
        __u32 args[3] = {0};

        args[0] = 1;
        __msg("DISP_CMD_SET_DE_FLICKER, args[0]=%d\n", args[0]);
        eLIBs_fioctrl(fp, DISP_CMD_SET_DE_FLICKER, 0, args);
        eLIBs_fclose(fp);
    }
    else
    {
        __msg("open disp drv fail...\n");
    }
}

static __s32 __app_movie_proc(__gui_msg_t *msg)
{
    __msg("__app_movie_proc: msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
          , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);

    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        __s32 ret;
        movie_ctrl_t *movie_ctrl;
        //by wanggang
        esMEM_RequestDramUsrMode(DRAM_USER_MODE_VIDEO);

        g_disable_close_scn();
        //__movie_set_title();
        //gscene_hbar_set_state(HBAR_ST_SHOW);
        gscene_hbar_set_state(HBAR_ST_HIDE);
        gscene_bgd_set_state(BGD_STATUS_HIDE);

        __msg("__app_movie_proc: GUI_MSG_CREATE begin\n");
        movie_ctrl = esMEMS_Balloc(sizeof(movie_ctrl_t));
        if (!movie_ctrl)
        {
            __msg("mem not enough...\n");
            return EPDK_FAIL;
        }
        __movie_robin_sem_init();

        eLIBs_memset(movie_ctrl, 0, sizeof(movie_ctrl_t));

        GUI_WinSetAddData(msg->h_deswin, (__u32)movie_ctrl);

        movie_ctrl->movie_manager = msg->h_deswin;

        __movie_ctrl_para_init(movie_ctrl);

        __app_movie_reg_para_init(movie_ctrl);

        com_set_palette_by_id(ID_MOVIE_PAL_BMP);//设置调色板

        {
            //修改调色板
            //背景色，灰白色，字幕边框颜色
            __u32 color[] = {BEETLES_BLACK_COLOR, BEETLES_GRAY_COLOR, 0xC0323232};
            com_set_palette_color_by_index(0xF0, color, BEETLES_TBL_SIZE(color));
        }

        __msg("before __movie_cur_file_has_breakpoint\n");
        ret = __movie_cur_file_has_breakpoint();
        __msg("__movie_cur_file_has_breakpoint=%d\n", ret);

        if(1 == MOVIE_ENABLE_BREAKPLAY && EPDK_TRUE == ret)//弹框让用户选择是否断点播放
        {
#if 0
            //弹框
            ret = __movie_breakplay_scene_create(movie_ctrl);
            if(EPDK_FAIL == ret)
            {
                __msg("__movie_breakplay_scene_create fail, exit movie...\n");
                __movie_exit_movie(movie_ctrl, 0);
                return EPDK_OK;
            }
#endif
        }
        else//不弹框，默认有断点则断点播放，无断点则从头播放
        {

            __movie_start_play(movie_ctrl, ret);

            __movie_process_after_start_play(movie_ctrl);

        }
        //////////////////////////////////////////////////////////////////////////////////////

        __movie_spsc_scene_create(movie_ctrl);

        __msg("__app_movie_proc: GUI_MSG_CREATE end\n");

        //__sub_set_scene_cmd_proc(movie_ctrl,sub_set_scene_msg_sub_stream, 1);

        return EPDK_OK;
    }
    case GUI_MSG_KEY:
    {
        __s32 bok;// 按键消息是否已处理完毕，-1代表往下传，否则直接拦截
        __s32 ret;
        static __s32 last_key = -1;
        movie_ctrl_t *movie_ctrl;

        __msg("__app_movie_proc: GUI_MSG_KEY begin\n");

        movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        if (!movie_ctrl)
        {
            __err("movie_ctrl is null...\n");
            return EPDK_OK;
        }
        __log("------jh_dbg20170224-------15\n");

        {
            //如果有断点播放对话框的子场景，则让按键消息全部往下传
            ret = __movie_has_sub_scene(movie_ctrl, MOVIE_BREAKPLAY_DIALOG_ID);
            if(EPDK_TRUE == ret)
            {
                break;
            }
        }

        bok = -1;
        __log("------jh_dbg20170224-------6\n");

        if (KEY_UP_ACTION == msg->dwAddData2)
        {
#if 0
            if (GUI_MSG_KEY_LONGENTER == last_key
                    || GUI_MSG_KEY_LONGMENU == last_key)
            {
                __s32 ret;

                __here__;

                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SPSC_ID);
                if (EPDK_FALSE == ret)
                {
                    __here__;
                    __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));

                    __here__;
                    __movie_spsc_scene_create(movie_ctrl);
                    __here__;
                }
                else
                {
                    __here__;
                    //__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL&(~MOVIE_SUB_SHOW_ID));
                    __here__;
                }

                bok = EPDK_OK;
            }
            else
#endif
                if ((GUI_MSG_KEY_ENTER == last_key) || (GUI_MSG_KEY_LONGENTER == last_key	))
                {
                    /*
                    //如果没有弹出菜单，则按enter弹出菜单
                    __s32 ret;

                    ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SPSC_ID);
                    if (EPDK_FALSE == ret)
                    {
                    	__here__;
                    	__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL&(~MOVIE_SUB_SHOW_ID));

                    	__here__;
                    	__movie_spsc_scene_create(movie_ctrl);
                    	__here__;
                    	ret = EPDK_OK;
                    }
                    */

                    //如果没有弹出菜单，则按enter暂停或开始播放
                    __s32 ret;
                    __log("------jh_dbg20170224-------4\n");
                    ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
                    if(EPDK_FALSE == ret)//如果没有除字幕以外的子场景，则处理暂停、继续播放
                    {
                        __cedar_status_t fsm_sta;
                        __log("------jh_dbg20170224-------7\n");
                        fsm_sta = robin_get_fsm_status();
                        if (CEDAR_STAT_PLAY == fsm_sta)
                        {
                            __msg("before robin_set_cmd_pause\n");
                            robin_set_cmd_pause();
                            __msg("after robin_set_cmd_pause\n");

                            __movie_playsta_scene_create(movie_ctrl);
                            __log("------jh_dbg20170224-------8\n");
                            bok = EPDK_OK;
                        }
                        else if(CEDAR_STAT_PAUSE == fsm_sta)
                        {
                            __msg("before robin_set_cmd_play\n");
                            robin_set_cmd_play();
                            __msg("after robin_set_cmd_play\n");
                            __log("------jh_dbg20170224-------9\n");
                            bok = EPDK_OK;
                        }
                    }
                }
                else if(GUI_MSG_KEY_VADD == last_key)
                {
                    ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SPSC_ID);
                    if (EPDK_FALSE == ret)//如果没有spsc场景则创建字幕设置场景(VADD按键有冲突，只能这么做)
                    {
                        ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SET_ID);
                        if (EPDK_FALSE == ret)
                        {
                            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
                            __movie_sub_set_scene_create(movie_ctrl);
                            __movie_sub_set_scene_set_focus(movie_ctrl);
                        }
                        else
                        {
                            //__movie_sub_set_scene_delete(movie_ctrl);
                        }
                        bok = EPDK_OK;
                    }
                }
                else if (GUI_MSG_KEY_LONGVADD == last_key)
                {
                    __s32 ret;

                    ret = __movie_has_sub_scene(movie_ctrl, MOVIE_VOLUME_ID);
                    if (EPDK_FALSE == ret)
                    {
                        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));

                        __movie_volume_scene_create(movie_ctrl);
                        __msg("__movie_volume_scene_create\n");

                        __movie_volume_scene_set_focus(movie_ctrl);
                        __msg("__movie_volume_scene_set_focus\n");
                        bok = EPDK_OK;
                    }
                }
                else if (GUI_MSG_KEY_ESCAPE == last_key
                         || GUI_MSG_KEY_MENU == last_key
                        )
                {
                    __s32 ret;

                    ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_PLAYSTA_ID));
                    if(EPDK_TRUE == ret)//如果有除字幕和playpause以外的子场景，则不退出movie，删除所有子场景
                    {
                        __cedar_status_t fsm_sta;

                        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
                        __movie_exit_movie(movie_ctrl, 0);
#if 0
                        fsm_sta = robin_get_fsm_status();
                        if (CEDAR_STAT_PAUSE == fsm_sta)
                        {
                            ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PLAYSTA_ID);
                            if(EPDK_FALSE == ret)
                            {
                                __movie_playsta_scene_create(movie_ctrl);
                            }
                        }
#endif
                    }
                    else//退出movie
                    {
                        __log("---jh_dbg1019_7----\n");
                        __movie_exit_movie(movie_ctrl, 0);
                    }

                    bok = EPDK_OK;
                }
                else if(GUI_MSG_KEY_LONGLEFT == last_key
                        || GUI_MSG_KEY_LONGRIGHT == last_key)
                {
                    __here__;
                    ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID)
                                                & (~MOVIE_PROG_ID));
                    __msg("ret=%d\n", ret);
                    if(EPDK_FALSE == ret)//如果没有除字幕和进度以外的子场景，则恢复快退快进后的正常播放
                    {
                        __u32 cur_time;

                        cur_time = 0;
                        ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID);
                        __msg("ret=%d\n", ret);
                        if(EPDK_TRUE == ret)
                        {
                            movie_prog_scene_update_prog(movie_ctrl->prog_scene);
                        }

                        {
                            __cedar_status_t fsm_sta;

                            fsm_sta = robin_get_fsm_status();
                            if (CEDAR_STAT_RR == fsm_sta
                                    || CEDAR_STAT_FF == fsm_sta)
                            {
                                robin_set_cmd_play();
                            }
                            else
                            {
                                __msg("fsm not in ff/rr status, do not play\n");
                            }
                        }
                        //__movie_spsc_scene_create(movie_ctrl);

                        bok = EPDK_OK;
                    }
                }
                else if(GUI_MSG_KEY_LEFT == last_key)
                {
                    ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_PLAYSTA_ID));
                    if(EPDK_FALSE == ret)//如果没有除字幕和playpause以外的子场景，则处理上一个下一个
                    {
                        __cedar_status_t fsm_sta;

                        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));

                        fsm_sta = robin_get_fsm_status();
                        if (CEDAR_STAT_PLAY == fsm_sta
                                || CEDAR_STAT_PAUSE == fsm_sta
                                || CEDAR_STAT_STOP == fsm_sta)
                        {
                            __msg("before __movie_play_prev\n");
                            __movie_play_prev(movie_ctrl);
                            __msg("after __movie_play_prev\n");
#if 0
                            ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SPSC_ID);
                            if(ret == EPDK_OK)
                            {
                                __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SPSC_ID);

                            }

                            __movie_spsc_scene_create(movie_ctrl);
#endif
                            bok = EPDK_OK;
                        }
                        else
                        {
                            __msg("fsm in ff/rr status, do not play prev\n");
                        }
                    }
                }
                else if(GUI_MSG_KEY_RIGHT == last_key)
                {
                    ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_PLAYSTA_ID));
                    if(EPDK_FALSE == ret)//如果没有除字幕和playpause以外的子场景，则处理上一个下一个
                    {
                        __cedar_status_t fsm_sta;

                        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));

                        fsm_sta = robin_get_fsm_status();
                        if (CEDAR_STAT_PLAY == fsm_sta
                                || CEDAR_STAT_PAUSE == fsm_sta
                                || CEDAR_STAT_STOP == fsm_sta)
                        {
                            __msg("before __movie_play_prev\n");
                            __movie_play_next(movie_ctrl);
                            __msg("after __movie_play_prev\n");
#if 0
                            ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SPSC_ID);
                            if(ret == EPDK_OK)
                            {
                                __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SPSC_ID);

                            }

                            __movie_spsc_scene_create(movie_ctrl);
#endif
                            bok = EPDK_OK;
                        }
                        else
                        {
                            __msg("fsm in ff/rr status, do not play next\n");
                        }
                    }
                }
        }
        else
        {
            switch(msg->dwAddData1)
            {
#ifdef __ENABLE_VOLUME_ADJUST_FUNCTION__
            case GUI_MSG_KEY_LONGLEFT:
            {

            }
            case GUI_MSG_KEY_LONGRIGHT:
            {

            }
#else

            case GUI_MSG_KEY_LONGLEFT:
            {
                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID)
                                            & (~MOVIE_PROG_ID));
                if(ret == EPDK_TRUE)
                {
                    __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_PROG_ID));
                }
                //  if(EPDK_FALSE == ret)//如果没有除字幕和进度以外的子场景，则快退快进
                {
                    __cedar_status_t fsm_sta;

                    fsm_sta = robin_get_fsm_status();
                    if (CEDAR_STAT_PLAY == fsm_sta)
                    {
                        __msg("before robin_set_cmd_rr\n");
                        robin_set_cmd_rr();
                        movie_ctrl->rr_flag = 1;
                        __msg("after robin_set_cmd_rr\n");

                        ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID);
                        if(EPDK_FALSE == ret)
                        {
                            __movie_prog_scene_create(movie_ctrl);
                            __movie_prog_scene_set_focus(movie_ctrl);
                        }
                    }
                    else
                    {
                        __msg("fsm not in play status, do not rr\n");
                    }

                    bok = EPDK_FAIL;//消息往下传给prog scene
                }
                break;
            }
            case GUI_MSG_KEY_LONGRIGHT:
            {
                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID)
                                            & (~MOVIE_PROG_ID));
                if(ret == EPDK_TRUE)
                {
                    __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_PROG_ID));
                }
                //   if(EPDK_FALSE == ret)//如果没有除字幕和进度以外的子场景，则快退快进
                {
                    __cedar_status_t fsm_sta;

                    fsm_sta = robin_get_fsm_status();
                    if (CEDAR_STAT_PLAY == fsm_sta)
                    {
                        __msg("before robin_set_cmd_ff\n");
                        robin_set_cmd_ff();
                        __msg("after robin_set_cmd_ff\n");

                        ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID);
                        if(EPDK_FALSE == ret)
                        {
                            __movie_prog_scene_create(movie_ctrl);
                            __movie_prog_scene_set_focus(movie_ctrl);
                        }
                    }
                    else
                    {
                        __msg("fsm not in play status, do not ff\n");
                    }

                    bok = EPDK_FAIL;//消息往下传给prog scene
                }
                break;
            }
#endif
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
        __msg("__app_movie_proc: GUI_MSG_KEY end\n");

        if (EPDK_OK == bok)//如果消息已处理，则返回，否则交给子场景处理
        {
            return EPDK_OK;
        }
        else
        {
            break;
        }
    }
    case GUI_MSG_CLOSE:
    {
        esMEM_ReleaseDramUsrMode(DRAM_USER_MODE_VIDEO);

        if(1 == msg->dwAddData1)//退出到home
        {
            movie_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);//退出movie
        }
        else//退出到explorer
        {
            // __log("--jh_dbg1019_8---\n");
            movie_cmd2parent(msg->h_deswin, SWITCH_TO_EXPLORER, 0, 0);//退出movie
        }

        return EPDK_OK;
    }
    case GUI_MSG_TOUCH:
    {
        __s32 ret;

        movie_ctrl_t *movie_ctrl;

        __msg("__app_movie_proc: GUI_MSG_TOUCH begin\n");

        ret = -1;

        movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        if (!movie_ctrl)
        {
            __err("movie_ctrl is null...\n");
            return EPDK_OK;
        }

        {
            //如果有断点播放对话框的子场景，则让触摸消息全部往下传(避免下面的对话框没有set capture)
            ret = __movie_has_sub_scene(movie_ctrl, MOVIE_BREAKPLAY_DIALOG_ID);
            if(EPDK_TRUE == ret)
            {
                break;
            }
        }

        __msg("__app_movie_proc: GUI_MSG_TOUCH end\n");
        break;
    }
    case GUI_MSG_TIMER:
    {
        movie_ctrl_t *movie_ctrl;

        __s32 ret;

        ret = -1;
#if TEST_DRAM_PRIORITY_CHANGE
        {
            static __u32  dram_mode = 0 ;
            dram_mode  = (dram_mode + 1) % (DRAM_USER_MODE_PICTURE + 1);
            esMEM_RequestDramUsrMode(dram_mode);
        }
#endif

#if PRINTF_SDRAM_CONTROLLER_REG
        {
            volatile __u32	addr ;
            volatile __u32 reg_vule ;
            for(addr = 0xf1c01000 ; addr <= (0xf1c01000 + 0x140) ; addr = addr + 4 )
            {
                reg_vule = *(volatile __u32 *)addr ;
                eLIBs_printf("sdram reg[%x]=%x\n", addr, reg_vule);
            }
        }
#endif

#if PRINTF_ALL_CCMU_REG
        {
            volatile __u32	addr ;
            volatile __u32 reg_vule ;
            for(addr = 0xf1c20000 ; addr <= (0xf1c20000 + 0x2d0) ; addr = addr + 4 )
            {
                reg_vule = *(volatile __u32 *)addr ;
                eLIBs_printf("sdram reg[%x]=%x\n", addr, reg_vule);
            }
        }
#endif
        //__log("---jhdbg1024_4----\n");
        movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        if (movie_ctrl)
        {

            //__log("---jhdbg1024_5----\n");
            if(LOWORD(msg->dwAddData1) == movie_ctrl->auto_play_timmer_id)
            {
                __s32 play_sta;
                //__msg("before __movie_check_for_play_err\n");
                play_sta = __movie_check_for_play_err(movie_ctrl);
                //__msg("after __movie_check_for_play_err\n");

                if(EPDK_OK == play_sta)
                {
                    //__msg("before __movie_check_for_auto_play\n");
                    __log("---jhdbg1024_6----\n");
                    __movie_check_for_auto_play(movie_ctrl);
                    //__msg("aftr __movie_check_for_auto_play\n");
                }
                else//如果播放错误，则重新开始检测
                {
                    // __log("---jhdbg1024_7----\n");
                    __movie_reset_auto_play_timmer(movie_ctrl);
                    movie_ctrl->cur_stop_cnt = 0;
                }
                ret = EPDK_OK;
            }
        }
        else
        {
            //__log("---jhdbg1024_8----\n");
            //__err("movie_ctrl is null...\n");
            return EPDK_OK;
        }

        if (-1 == ret)//未处理，交给子场景处理
        {
            break;
        }
        else//已处理
        {
            return EPDK_OK;
        }
    }
    case GUI_MSG_DESTROY:
    {
        movie_ctrl_t *movie_ctrl;

        __msg("__app_movie_proc: GUI_MSG_DESTROY begin\n");

        //by wanggang
        esMEM_ReleaseDramUsrMode(DRAM_USER_MODE_VIDEO);

        movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

        if (movie_ctrl)
        {
            __here__;
            __movie_uninstall_auto_play_timmer(movie_ctrl);

            {
                //save breakpoint
                __s32 index;

                index = robin_npl_get_cur();
                __here__;
                if(-1 != index)
                {
                    __here__;
                    __movie_save_breakpoint_info(movie_ctrl, index);
                }
            }

            {
                //save last playing file
                __s32 ret;

                ret = __movie_save_last_playing_file(msg);
                if(EPDK_OK == ret)
                {
                    __msg("__movie_save_last_playing_file success...\n");
                }
                else
                {
                    __msg("__movie_save_last_playing_file fail...\n");
                }
            }


            __here__;
            __movie_stop_play(movie_ctrl);

            __here__;
            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL);

            __here__;
            __movie_ctrl_para_deinit(movie_ctrl);

            eLIBs_memset(movie_ctrl, 0, sizeof(movie_ctrl_t));

            esMEMS_Bfree(movie_ctrl, sizeof(movie_ctrl_t));

            __here__;
        }

        //112350//g_display_switch_output(LION_DISP_LCD);//movie 退出时恢复到小机显示

        g_enable_close_scn();

        robin_close();

        __movie_robin_sem_deinit();
        //dsk_reg_flush();

        dsk_display_set_FLICKER();

        __msg("__app_movie_proc: GUI_MSG_DESTROY end\n");
        return EPDK_OK;
    }
    case GUI_MSG_COMMAND:
    {
        __s32 ret;
        movie_ctrl_t *movie_ctrl;

        movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

        __msg("__app_movie_proc: GUI_MSG_COMMAND begin\n");
        if (movie_ctrl)
        {
            switch(LOWORD(msg->dwAddData1))
            {
            case MOVIE_SPSC_ID:
            {
                ret = __spsc_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
                break;
            }
            case MOVIE_VOLUME_ID:
            {
                ret = __volume_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
                break;
            }
            case MOVIE_BRIGHT_ID:
            {
                ret = __bright_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
                break;
            }
            case MOVIE_PLAYSTA_ID:
            {
                ret = __playsta_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
                break;
            }
            case MOVIE_SUB_SHOW_ID:
            {
                break;
            }
            case MOVIE_SUB_SET_ID:
            {
                ret = __sub_set_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
                break;
            }
            case MOVIE_PROG_ID:
            {
                ret = __prog_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
                break;
            }
            case MOVIE_BREAKPLAY_DIALOG_ID:
            {
                ret = __breakplay_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
                break;
            }
            default:
            {
                break;
            }
            }
        }
        else
        {
            __msg("movie_ctrl is null...\n");
        }

        __msg("__app_movie_proc: GUI_MSG_COMMAND end\n");

        return EPDK_OK;
    }
    case DSK_MSG_TVDAC_PLUGOUT:
    case DSK_MSG_HDMI_PLUGOUT:
    {
        movie_ctrl_t *movie_ctrl;

        __msg("************DSK_MSG_TVDAC_PLUGOUT/DSK_MSG_HDMI_PLUGOUT*******\n");

        movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        if (!movie_ctrl)
        {
            __err("movie_ctrl is null...\n");
            return EPDK_OK;
        }

        __msg("before __movie_switch_to_tv_or_lcd\n");
        __movie_switch_to_tv_or_lcd(movie_ctrl);
        __msg("after __movie_switch_to_tv_or_lcd\n");

        return EPDK_OK;
    }
    case DSK_MSG_FS_PART_PLUGOUT:
    {
        __bool to_exist;//用于记录是否退出
        __s32 ret;
        char diskname[2] = {0};
        char cur_play_file[RAT_MAX_FULL_PATH_LEN] = {0};
        movie_ctrl_t *movie_ctrl;

        __msg("************DSK_MSG_FS_PART_PLUGOUT*******\n");

        movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        if (!movie_ctrl)
        {
            __msg("movie_ctrl is null...\n");
            return EPDK_OK;
        }

        diskname[0] = (char)(msg->dwAddData2);
        diskname[1] = 0;

        to_exist = 1;//默认需要退出

        ret = robin_npl_get_cur();
        if(-1 != ret)
        {
            ret = robin_npl_index2file(ret, cur_play_file);
            if(EPDK_OK == ret)
            {
                if(0 != eLIBs_strnicmp(cur_play_file, diskname, 1))
                {
                    to_exist = 0;//拔的卡不是当前播放文件所在卡，不需要退出
                }
            }
        }

        if(1 == to_exist)//需要退出
        {
            movie_ctrl->quit_by_fs_part_plug_out = EPDK_TRUE;
            __movie_exit_movie(movie_ctrl, 1);
        }

        return EPDK_OK;
    }
    case DSK_MSG_HOME:
    {
        movie_ctrl_t *movie_ctrl;

        __msg("************DSK_MSG_HOME*******\n");

        movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

        if (!movie_ctrl)
        {
            __msg("movie_ctrl is null...\n");
            return EPDK_OK;
        }

        __movie_exit_movie(movie_ctrl, 1);

        return EPDK_OK;
    }
    default:
    {
        __msg("__app_movie_proc: default\n");

        break;
    }
    }

    return GUI_ManWinDefaultProc(msg);
}

H_WIN app_movie_create(root_para_t *para)
{
    __gui_manwincreate_para_t create_info;
    H_WIN hManWin;

    __inf("****************************************************************************************\n");
    __inf("********  enter movie app  **************\n");
    __inf("****************************************************************************************\n");

    // esKSRV_SysInfo();

    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));

    create_info.name            = APP_MOVIE;
    create_info.hParent         = para->h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__app_movie_proc);
    create_info.attr            = (void *)para;
    create_info.id				= APP_MOVIE_ID;
    create_info.hHosting        = NULL;

    hManWin = GUI_ManWinCreate(&create_info);
    __msg("app_movie_create, hManWin=%x\n", hManWin);
    return hManWin;
}
