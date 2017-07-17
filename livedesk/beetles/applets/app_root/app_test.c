#include "beetles_app.h"
void test_rat_partition(char *root)
{
    __s32 ret;
    __s32 i;
    HRAT hrat;
    char diskname[RAT_MAX_PARTITION][4];

    __msg("**********test rat partition %s:**********\n", root);
    ret = rat_get_partition_name(root, diskname);
    i = 0;
    while (i < RAT_MAX_PARTITION)
    {
        if (diskname[i][0])
        {
            __msg("diskname[%d]=%s\n", i, diskname[i]);
        }
        i++;
    }
}

/***********************************************************************************************************
	建立图层
************************************************************************************************************/
static H_LYR test_32bpp_layer_create(RECT *rect, __s32 pipe)
{
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                   		/* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},    /* fmt       */
    };

    __disp_layer_para_t lstlyr =
    {
        DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
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
        GUI_LYRWIN_STA_ON,
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


void test_miniature(void)
{
    rat_miniature_para_t in_para;
    rat_pic_info_t out_para;
    char *pbuf;
    __s32 ret;
    RECT rect;
    static H_LYR hlayer = 0;
    HRAT hrat;
    rat_entry_t entry;
    char file[RAT_MAX_FULL_PATH_LEN];

    hrat = rat_open("f:", RAT_MEDIA_TYPE_PIC, 100);
    if (NULL == hrat)
    {
        return ;
    }
    __here__;

    gscene_bgd_set_state(BGD_STATUS_HIDE);
    __here__;

    rat_move_cursor_to_first(hrat);

    __here__;
    ret = rat_get_cur_item_info(hrat, &entry);
    if (EPDK_OK == ret)
    {
        eLIBs_strcpy(file, entry.Path);
    }
    else
    {
        return ;
    }

    __msg("file=%s\n", file);

    pbuf = esMEMS_Balloc(24 * 24 * 4);
    if (NULL == pbuf)
    {
        __msg("esMEMS_Balloc fail...\n");
        return ;
    }
    __here__;
    rect.x = 0;
    rect.y = 0;
    rect.width = 100;
    rect.height = 100;
    if (NULL == hlayer)
    {
        hlayer = test_32bpp_layer_create(&rect, 0);
    }

    if (NULL == hlayer)
    {
        return ;
    }
    __here__;

    rat_start_miniature_decode();
    __here__;

    eLIBs_strcpy(in_para.file, file);
    in_para.format = PIXEL_COLOR_ARGB8888;
    in_para.width = 100;
    in_para.height = 100;
    in_para.mode = 2;

    __here__;

    eLIBs_memset(&out_para, 0, sizeof(out_para));
    out_para.miniature.buf = pbuf;

    __here__;
    ret = rat_get_pic_info(&in_para, &out_para);
    __here__;
    rat_stop_miniature_decode();
    __here__;

    if (EPDK_OK == ret)
    {
        FB fb;

        __here__;

        eLIBs_memset(&fb, 0, sizeof(FB));
        GUI_LyrWinSetSta(hlayer, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSetTop(hlayer);
        gscene_bgd_set_state(BGD_STATUS_HIDE);
        GUI_LyrWinSel(hlayer);
        fb.size.width = 100;
        fb.size.height = 100;
        fb.addr[0] = out_para.miniature.buf;
        fb.fmt.type = FB_TYPE_RGB;
        fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
        fb.fmt.fmt.rgb.br_swap = 0;
        fb.fmt.fmt.rgb.pixseq = (__rgb_seq_t)0;
        GUI_BitString_DrawEx(&fb, 0, 0);
    }
    __here__;
}

HRAT test_rat(const char *pDir, rat_media_type_t media_type, __u32 OnceCnt)
{
    __s32 i;
    __s32 ret;
    __u32 cnt;
    HRAT hrat;
    rat_entry_t entry;
    hrat = rat_open(pDir, media_type, OnceCnt);
    if (NULL == hrat)
    {
        return NULL;
    }

    cnt = rat_get_cur_scan_cnt(hrat);
    __msg("rat_get_cur_scan_cnt=%d\n", cnt);

    while (!rat_is_cursor_end(hrat))
    {
        ret = rat_get_cur_item_info(hrat, &entry);
        if (EPDK_OK == ret)
        {
            __msg("file name:%s\n", entry.Path);
        }
        rat_move_cursor_forward(hrat, 1);
    }

    cnt = rat_get_cur_scan_cnt(hrat);
    __msg("rat_get_cur_scan_cnt=%d\n", cnt);

    for (i = 0 ; i < 10 ; i++)
    {
        ret = rat_get_item_info_by_index(hrat, i, &entry);
        if (EPDK_OK == ret)
        {
            __msg("index %d, file name:%s\n", i, entry.Path);
        }
        else
        {
            __msg("index %d, file name:%s\n", i, "NULL");
        }
    }

    //	rat_close(hrat);//测试，不关闭

    return hrat;
}

void test_npl(const char *pDir, rat_media_type_t media_type, __u32 OnceCnt
              , rat_play_mode_e play_mode, __bool direction)
{
    HRAT hrat;
    HRATNPL hnpl;
    rat_entry_t entry;
    __s32 index;
    char  file_name[RAT_MAX_FULL_PATH_LEN];
    __s32 file_index;
    char file[RAT_MAX_FULL_PATH_LEN];
    __s32 cnt;

    hrat = rat_open(pDir, media_type, OnceCnt);
    if (NULL == hrat)
    {
        return ;
    }
    // 	test_rat("e:", RAT_MEDIA_TYPE_PIC, 10);
    // 	test_rat("e:", RAT_MEDIA_TYPE_AUDIO, 10);
    // 	test_rat("e:", RAT_MEDIA_TYPE_VIDEO, 10);
    // 	test_rat("e:", RAT_MEDIA_TYPE_EBOOK, 10);

    __msg("**********************************\n");
    __msg("*******media type: %d play mode %d", media_type, play_mode);

    rat_move_cursor_to_first(hrat);
    rat_move_cursor_forward(hrat, 1);
    rat_move_cursor_forward(hrat, 1);
    rat_get_cur_item_info(hrat, &entry);

    rat_set_file_for_play(hrat, entry.Path);

    hnpl = rat_npl_open(media_type);

    rat_npl_set_play_mode(hnpl, play_mode);
    index = rat_npl_get_cur(hnpl);
    __msg("rat_npl_get_cur=%d\n", index);
    rat_npl_index2file(hnpl, index, file);
    __msg("index = %d file=%s\n", index, file);

    cnt = 0;
    if (direction)
    {
        while (index = rat_npl_get_next(hnpl), -1 != index)
        {
            rat_npl_index2file(hnpl, index, file);
            __msg("index = %d file=%s\n", index, file);
            cnt ++;
            if (cnt > 150)
            {
                __msg("*************test end***************\n");
                break;
            }
        }
    }
    else
    {
        while (index = rat_npl_get_prev(hnpl), -1 != index)
        {
            rat_npl_index2file(hnpl, index, file);
            __msg("index = %d file=%s\n", index, file);
            cnt ++;
            if (cnt > 150)
            {
                __msg("*************test end***************\n");
                break;
            }
        }
    }

    //rat_npl_close(hnpl);//测试，不关闭
}

void test_rat_interface(void)
{
    __msg("****************************\n");
    __msg("**********TEST test_rat_partition**********\n");
    __msg("****************************\n");
    test_rat_partition(RAT_LOCAL_DISK);
    test_rat_partition(RAT_USB_DISK);
    test_rat_partition(RAT_SD_CARD);

    //__wait__;

    __msg("****************************\n");
    __msg("**********TEST test_miniature**********\n");
    __msg("****************************\n");
    test_miniature();

    //__wait__;

    __msg("****************************\n");
    __msg("**********TEST test_npl direction = 1**********\n");
    __msg("****************************\n");
    test_npl("e:", RAT_MEDIA_TYPE_PIC, 10, RAT_PLAY_MODE_ROTATE_ONE, 1);
    test_npl("e:", RAT_MEDIA_TYPE_PIC, 10, RAT_PLAY_MODE_ONLY_ONCE, 1);
    test_npl("e:", RAT_MEDIA_TYPE_PIC, 10, RAT_PLAY_MODE_ROTATE_ALL, 1);
    test_npl("e:", RAT_MEDIA_TYPE_PIC, 10, RAT_PLAY_MODE_SEQUENCE, 1);
    test_npl("e:", RAT_MEDIA_TYPE_PIC, 10, RAT_PLAY_MODE_RANDOM, 1);


    //	__wait__;

    __msg("****************************\n");
    __msg("**********TEST test_npl direction = 0**********\n");
    __msg("****************************\n");

    test_npl("e:", RAT_MEDIA_TYPE_PIC, 10, RAT_PLAY_MODE_ROTATE_ONE, 0);
    test_npl("e:", RAT_MEDIA_TYPE_PIC, 10, RAT_PLAY_MODE_ONLY_ONCE, 0);
    test_npl("e:", RAT_MEDIA_TYPE_PIC, 10, RAT_PLAY_MODE_ROTATE_ALL, 0);
    test_npl("e:", RAT_MEDIA_TYPE_PIC, 10, RAT_PLAY_MODE_SEQUENCE, 0);
    test_npl("e:", RAT_MEDIA_TYPE_PIC, 10, RAT_PLAY_MODE_RANDOM, 0);

    //__wait__;
    {
        HRAT hrat;

        __msg("****************************\n");
        __msg("**********TEST test_rat**********\n");
        __msg("****************************\n");
        test_rat("e:", RAT_MEDIA_TYPE_PIC, 10);
        test_rat("e:", RAT_MEDIA_TYPE_AUDIO, 10);
        test_rat("e:", RAT_MEDIA_TYPE_VIDEO, 10);
        test_rat("e:", RAT_MEDIA_TYPE_EBOOK, 10);
        test_rat("e:", RAT_MEDIA_TYPE_PIC, 10);
        hrat = test_rat("e:", RAT_MEDIA_TYPE_PIC, 10);
        rat_close(hrat);
        hrat = test_rat("e:", RAT_MEDIA_TYPE_PIC, 10);
        rat_close(hrat);
        rat_close(hrat);
        rat_close(hrat);
        rat_close(hrat);
    }
}

void test_playmusic(const char *pDir)
{
    HRAT hrat;
    HRATNPL hnpl;
    rat_entry_t entry;
    __s32 index;
    char  file_name[RAT_MAX_FULL_PATH_LEN];
    __s32 file_index;
    char file[RAT_MAX_FULL_PATH_LEN];
    __s32 cnt;

    hrat = rat_open(pDir, RAT_MEDIA_TYPE_AUDIO, 100);
    if (NULL == hrat)
    {
        return ;
    }

    rat_move_cursor_to_first(hrat);

    rat_get_cur_item_info(hrat, &entry);

    rat_set_file_for_play(hrat, entry.Path);

    hnpl = rat_npl_open(RAT_MEDIA_TYPE_AUDIO);

    rat_npl_set_play_mode(hnpl, RAT_PLAY_MODE_ROTATE_ALL);
    index = rat_npl_get_cur(hnpl);
    __msg("rat_npl_get_cur=%d\n", index);
    rat_npl_index2file(hnpl, index, file);
    __msg("index = %d file=%s\n", index, file);

    robin_open(ROBIN_MODE_AUDIO_MIN, NULL);
    robin_set_cmd_play_file(file, NULL);

    //rat_npl_close(hnpl);//测试，不关闭
}

void test_bgd_index(void)
{
    static __s32 index = 0;
    index++;
    if(index >= 2)
    {
        index = 0;
    }
    //gscene_bgd_set_flag();
    gscene_bgd_set_default_bg_index(index);

    {
        reg_init_para_t *para;

        para = (reg_init_para_t *)dsk_reg_get_default_para_by_app(REG_APP_INIT);
        if(para)
        {
            __msg("para->default_bgd_index=%d\n", para->default_bgd_index);

        }
        else
        {
            __msg("dsk_reg_get_default_para_by_app fail...\n");

        }
    }

    __here__;
    esKSRV_SysInfo();
    __here__;
}

void test_bgd_show(void)
{
    bgd_status_t sta;

    sta = gscene_bgd_get_state();
    if(BGD_STATUS_SHOW == sta)
    {
        gscene_bgd_set_state(BGD_STATUS_HIDE);
    }
    else
    {
        gscene_bgd_set_state(BGD_STATUS_SHOW);
        __here__;
        esMEMS_Info();
        __here__;
    }
}

__s32 test_mem_alloc(void)
{
    __s32 i;
    void *pbuf;
    __s32 size;
    __s32 num;

    num = 20000;
    for(i = 0 ; i < num ; i++)
    {
        size = eLIBs_ramdom(1024) + 1;
        pbuf = (void *)esMEMS_Balloc( 1024);
        __msg(" size = %d, pbuf=%x\n", size, pbuf);
        if(!pbuf)
        {
            break;
        }
    }

    if(i < num)
    {
        eLIBs_printf("*************test fail*********************\n");
        eLIBs_printf("i=%d, size = %d, pbuf=%x\n", i, size, pbuf);
    }
    else
    {
        eLIBs_printf("test  success, i=%d\n", i);
    }

    return 0;
}


