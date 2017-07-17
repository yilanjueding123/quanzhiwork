/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           home app sample
*
*						        	(c) Copyright 2006-2009, China
*											 All Rights Reserved
*
* File    : cvr_core.c
* By      : lyn
* Version : V1.00
*********************************************************************************************************
*/

#include "cvr_core.h"
//#include "elibs_stdio.h"

typedef struct tag_DV_CORE
{
    ES_FILE                        *h_pdisp;              // 显示驱动句柄
    __s32							show_layer;			  // 预览图层
    RECT                            show_rect;            // 显示区域

    __lotus_media_file_inf          media_info;        // 媒体文件信息
    __work_mode_e                   work_mode;            // LOTUS工作模式
    __record_state_e                record_sta;           // 录像的状态

    __u32                           rec_cur_mode_index;      // 当前录像模式
    __csi_mode_all_t                csi_mode_rec;      // 录像支持模式
    __record_quality_e              rec_quality_mode;  // 录像质量
    SIZE                            rec_size;          // 录像的分辨率
    SIZE                            rec_source_size;
    __u32                           video_bps;         // 录像的码率

    __u32                           cam_cur_mode_index;      // 当前拍照模式
    __csi_mode_all_t                csi_mode_cam;      // 拍照支持模式
    __camera_quality_e              cam_quality_mode;  // 拍照质量
    SIZE                            cam_size;          // 拍照的分辨率

    __s32                           record_mute;       // 录像静音功能标志

    char                            prefix_name[16];      // 文件名前缀

    __hdle						    dv_obj;            //dv操作句柄

} __dv_core_t;

static  __dv_core_t    *dv_core = NULL;

static void __dv_release_fb(FB *fb)
{
    if(fb != NULL)
    {
        if(fb->addr[0])
        {
            esMEMS_Bfree(fb->addr[0], fb->size.width * fb->size.width * 4);
            fb->addr[0] = NULL;
        }
    }
}

static void __dv_get_record_mode(__csi_mode_all_t *csi_src, __csi_mode_all_t *csi_des)
{
    __u32           i, ii;
    __csi_mode_t   *csi_mode;

    ii = 0;
    for(i = 0; i < csi_src->number; i++)
    {
        csi_mode = &csi_src->csi_mode[i];
        if((csi_mode->color_format == PIXEL_YUV420) && (csi_mode->store_mode == YUV_MOD_UV_NON_MB_COMBINED))
        {
            // YUV420 + 宏块平面模式
            csi_des->csi_mode[ii] = *csi_mode;
            ii++;
        }
    }
    csi_des->number = ii;
}

static void __dv_get_camera_mode(__csi_mode_all_t *csi_src, __csi_mode_all_t *csi_des)
{
    __u32           i, ii;
    __csi_mode_t   *csi_mode;

    ii = 0;
    for(i = 0; i < csi_src->number; i++)
    {
        csi_mode = &csi_src->csi_mode[i];
        if((csi_mode->color_format == PIXEL_YUV420) && (csi_mode->store_mode == YUV_MOD_UV_NON_MB_COMBINED))
        {
            // YUV422 + 宏块平面模式
            csi_des->csi_mode[ii] = *csi_mode;
            ii++;
        }
    }
    csi_des->number = ii;
}

__u32 year  = 2000;
__u32 month = 1;
__u32 day   = 0;

__u32   record_file_index = 0;

// creat filename :normally  the file is aw_yyyymmdd_hhmmss.mov
static void __dv_create_file_name(char *path, char *suffix, char *prefix)
{
    __date_t date;
    __time_t time;
    __u32    data;
    char	 str[32];

#if 0///ckk
    if(day++ < 30)
    {
    }
    else
    {
        if(month++ < 12)
        {
            day = 1;
        }
        else
        {
            if(year++ < 2200)
            {
                day   = 1;
                month = 1;
            }

        }
    }

    date.day   = day;
    date.month = month;
    date.year  = year;

    time.hour    = 10;
    time.minute  = 10;
    time.second  = 10;

    __msg("day   = %d\n", date.day);
    __msg("month = %d\n", date.month);
    __msg("year  = %d\n", date.year);

    __msg("hour   = %d\n", time.hour);
    __msg("minute = %d\n", time.minute);
    __msg("second = %d\n", time.second);
#endif

    // 创建录制目录
    eLIBs_strcpy(path, FILE_PATH);
    eLIBs_strcat(path, "\\");
    eLIBs_strcat(path, prefix);
#if 1//ckk
    if(record_file_index < 9999)
        record_file_index++;
    else
        record_file_index = 0;
#if 0
    str[0] = record_file_index / 1000 + 0x30;
    str[1] = (record_file_index % 1000) / 100 + 0x30;
    str[2] = (record_file_index % 100) / 10 + 0x30;
    str[3] = record_file_index % 10 + 0x30;
    eLIBs_strcat(path, str);
#endif
    if(record_file_index < 10)
        eLIBs_strcat(path, "000");
    else if(record_file_index < 100)
        eLIBs_strcat(path, "00");
    else if(record_file_index < 1000)
        eLIBs_strcat(path, "0");
    eLIBs_int2str_dec(record_file_index, str);
    eLIBs_strcat(path, str);


#endif
#if 0//ckk
    data = date.year * 10000 + date.month * 100 + date.day;
    eLIBs_int2str_dec(data, str);
    eLIBs_strcat(path, str);
    eLIBs_strcat(path, "_");

    if(time.hour < 10)
    {
        eLIBs_strcat(path, "0");
        eLIBs_int2str_dec(time.hour, str);
        eLIBs_strcat(path, str);
    }
    else
    {
        eLIBs_int2str_dec(time.hour, str);
        eLIBs_strcat(path, str);
    }
    if(time.minute < 10)
    {
        eLIBs_strcat(path, "0");
        eLIBs_int2str_dec(time.minute, str);
        eLIBs_strcat(path, str);
    }
    else
    {
        eLIBs_int2str_dec(time.minute, str);
        eLIBs_strcat(path, str);
    }
    if(time.second < 10)
    {
        eLIBs_strcat(path, "0");
        eLIBs_int2str_dec(time.second, str);
        eLIBs_strcat(path, str);
    }
    else
    {
        eLIBs_int2str_dec(time.second, str);
        eLIBs_strcat(path, str);
    }
#endif
    eLIBs_strcat(path, suffix);
	__msg("file path:%s\n", path);
}

static void __dv_get_cur_record_mode_index(__u32 sensorID)
{
    __s32       i;
    __csi_mode_all_t   *p_mode;


    dv_core->rec_cur_mode_index = 0;
    p_mode = &dv_core->csi_mode_rec;
    for(i = 0; i < p_mode->number; i++)
    {
        if(p_mode->csi_mode[i].size.width == dv_core->rec_size.width
                && p_mode->csi_mode[i].size.height == dv_core->rec_size.height)
        {
            dv_core->rec_cur_mode_index = i;
        }
    }
}

static void __dv_get_cur_cam_mode_index(__u32 sensorID)
{
    dv_core->cam_cur_mode_index = 0;
}

static void __dv_get_record_size(__u32 sensorID)
{

    switch(dv_core->rec_quality_mode)
    {
    case RECORD_QUALITY_640_480 :
    {

        __msg("__dv_get_record_size RECORD_QUALITY_640_480 \n");


        dv_core->rec_source_size.width = 690 ;
        dv_core->rec_source_size.height = 480 ;
        dv_core->rec_size.width = 690;
        dv_core->rec_size.height = 480;
        dv_core->video_bps = 6000000;
    }
    break;
    case RECORD_QUALITY_1280_720:
    {
        dv_core->rec_size.width = 1280;
        dv_core->rec_size.height = 720;
        dv_core->video_bps = 10000000;
    }
    break;
    case RECORD_QUALITY_1920_1080:
    {
        dv_core->rec_size.width = 1920;
        dv_core->rec_size.height = 1080;
        dv_core->video_bps = 10000000;
    }
    break;
    default:
    {
        dv_core->rec_source_size.width = 720;
        dv_core->rec_source_size.height = 480;
        dv_core->rec_size.width = 720;
        dv_core->rec_size.height = 480;
        dv_core->video_bps = 2000000;
    }
    break;
    }
}

static void __dv_get_camera_size(__u32 sensorID)
{
    switch(dv_core->cam_quality_mode)
    {
    case CAMERA_QUALITY_100:
    {
        dv_core->cam_size.width = 1280/2;
      	//dv_core->cam_size.height = 960;
        dv_core->cam_size.height = 720/2;
    }
    break;
    case CAMERA_QUALITY_200:
    {
        dv_core->cam_size.width = 1600;
        dv_core->cam_size.height = 1200;
    }
    break;
    case CAMERA_QUALITY_300:
    {
        dv_core->cam_size.width = 2048;
        dv_core->cam_size.height = 1536;
    }
    break;
    case CAMERA_QUALITY_500:
    {
        dv_core->cam_size.width = 2560;
        dv_core->cam_size.height = 1920;
    }
    break;
    case CAMERA_QUALITY_800:
    {
        dv_core->cam_size.width = 3264;
        dv_core->cam_size.height = 2488;
    }
    break;
    default:
    {
        dv_core->cam_size.width = 1600;
        dv_core->cam_size.height = 1200;
    }
    break;
    }
}

static void __dv_record_media_info(__u32 sensorID)
{
    eLIBs_memset(&dv_core->media_info, 0, sizeof(__lotus_media_file_inf));

    __dv_create_file_name(dv_core->media_info.file_path, FILE_NAME_SUFFIX, dv_core->prefix_name);
    __msg("new file is : %s \n", dv_core->media_info.file_path);

    dv_core->media_info.uFileFmt                 = SW_VIDEO_FORMAT_AVI;
    dv_core->media_info.audio_inf.uAbsFmt   	 = SW_CODEC_TYPE_PCM;//SW_CODEC_TYPE_MPEG_AAC_LC;
    dv_core->media_info.audio_inf.uVolume        = MAX_VOLUME_VAL;//
    dv_core->media_info.audio_inf.uChn           = LOTUS_AUDIO_CHN_MONO;//
    dv_core->media_info.audio_inf.uSampWidth     = LOTUS_AUDIO_SAMP_WIDTH_16;
    dv_core->media_info.audio_inf.uAudioBps      = 80000;
    dv_core->media_info.audio_inf.uAudioSampRate = 22050;	/* 8000 48k 24k(22.05k在录像和音乐同时工作时，声音无变形)	*/
    dv_core->media_info.audio_inf.uAudioSource   = LOTUS_AUDIO_SRC_MIC;
    dv_core->media_info.audio_inf.frame_style    = 1;

    dv_core->media_info.video_inf.uVbsFmt        = SW_CODEC_TYPE_MJPEG;

    dv_core->media_info.video_inf.srcWidth 	     = dv_core->rec_source_size.width;
    dv_core->media_info.video_inf.srcHeight 	 = dv_core->rec_source_size.height;
    dv_core->media_info.video_inf.nWidth         = dv_core->rec_size.width;
    dv_core->media_info.video_inf.nHeight        = dv_core->rec_size.height;
    dv_core->media_info.video_inf.nVideoBps      = dv_core->video_bps;

    __msg("srcWidth  = %d\n", dv_core->media_info.video_inf.srcWidth);
    __msg("srcHeight = %d\n", dv_core->media_info.video_inf.srcHeight);


    dv_core->media_info.video_inf.uVideoFrmRate  = (dv_core->csi_mode_rec.csi_mode + dv_core->rec_cur_mode_index)->frame_rate;
    dv_core->media_info.video_inf.color_format   = (dv_core->csi_mode_rec.csi_mode + dv_core->rec_cur_mode_index)->color_format;
    dv_core->media_info.video_inf.color_space    = (dv_core->csi_mode_rec.csi_mode + dv_core->rec_cur_mode_index)->color_space;
    dv_core->media_info.video_inf.qp_max         = 35; // 20
    dv_core->media_info.video_inf.qp_min         = 20;  // 1
    dv_core->media_info.video_inf.maxKeyInterval = 29; // 10  29
    dv_core->media_info.video_inf.jpgenc_quality = 50;
    dv_core->media_info.video_inf.videoTimes     = 0;//设置视频时间; 注释不设置,不做文件碎片处理.
}


static void __dv_camera_media_info(__u32 sensorID)
{
    eLIBs_memset(&dv_core->media_info, 0, sizeof(__lotus_media_file_inf));

    __dv_create_file_name(dv_core->media_info.file_path, ".jpg", dv_core->prefix_name);
    __msg("new file is : %s \n", dv_core->media_info.file_path);

    dv_core->media_info.uFileFmt                 = SW_PIC_FORMAT_JPG;
    dv_core->media_info.audio_inf.uAbsFmt        = SW_CODEC_TYPE_NONE;
    dv_core->media_info.audio_inf.uVolume        = MAX_VOLUME_VAL;
    dv_core->media_info.audio_inf.uChn           = LOTUS_AUDIO_CHN_MONO;
    dv_core->media_info.audio_inf.uSampWidth     = LOTUS_AUDIO_SAMP_WIDTH_16;
    dv_core->media_info.audio_inf.uAudioBps      = 80000;
    dv_core->media_info.audio_inf.uAudioSampRate = 22050;
    dv_core->media_info.audio_inf.uAudioSource   = LOTUS_AUDIO_SRC_MIC;
    dv_core->media_info.audio_inf.frame_style    = 1;

    dv_core->media_info.video_inf.uVbsFmt        = SW_CODEC_TYPE_JPEG;


    dv_core->media_info.video_inf.nWidth         = dv_core->cam_size.width;
    dv_core->media_info.video_inf.nHeight        = dv_core->cam_size.height;
    dv_core->media_info.video_inf.nVideoBps      = 2000000;

	__here__;
//    __msg("dv_core->cam_cur_mode_index    = %d\n", dv_core->cam_cur_mode_index);
//    __msg("dv_core->cam_cur_mode_index    = %d\n", dv_core->cam_cur_mode_index);


    dv_core->media_info.video_inf.uVideoFrmRate  = (dv_core->csi_mode_cam.csi_mode + dv_core->cam_cur_mode_index)->frame_rate;
    dv_core->media_info.video_inf.color_format   = (dv_core->csi_mode_cam.csi_mode + dv_core->cam_cur_mode_index)->color_format;
    dv_core->media_info.video_inf.color_space    = (dv_core->csi_mode_cam.csi_mode + dv_core->cam_cur_mode_index)->color_space;
    dv_core->media_info.video_inf.qp_max         = 35;
    dv_core->media_info.video_inf.qp_min         = 20;
    dv_core->media_info.video_inf.maxKeyInterval = 29;
    dv_core->media_info.video_inf.jpgenc_quality = 85;
	__here__;
//    __msg("uVideoFrmRate = %d\n", dv_core->media_info.video_inf.uVideoFrmRate);
//    __msg("color_format  = %d\n", dv_core->media_info.video_inf.color_format);
//    __msg("color_space   = %d\n", dv_core->media_info.video_inf.color_space);


}

static void __dv_rec_reset_preview(void)
{
    parrot_stop_preview(dv_core->dv_obj);
    __dv_record_media_info(0);
    parrot_set_media_file(dv_core->dv_obj, &dv_core->media_info);

    parrot_set_preview_mode(dv_core->dv_obj, dv_core->csi_mode_rec.csi_mode + dv_core->rec_cur_mode_index);
    parrot_start_preview(dv_core->dv_obj);
    return ;
}

/*
**********************************************************************************************************************
*                                               Cvr_DvOpen
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/

#define __msg  eLIBs_printf

__s32   Cvr_DvOpen(void)
{
    __s32                       i;
    __csi_mode_all_t            csi_mode_all;


    __msg("Cvr_DvOpen\n");
    if(EPDK_FAIL == parrot_init())
    {
        parrot_uninit();
        return EPDK_FAIL;
    }
	__msg("parrot init ok\n");
    dv_core = esMEMS_Malloc(0, sizeof(__dv_core_t));
    if(dv_core == NULL)
    {
        __wrn("request memery for __dv_core_t fail\n");
        return NULL;
    }
    eLIBs_memset(dv_core, 0, sizeof(__dv_core_t));

	__msg("dv_core->dv_obj, parrot_open\n");
    dv_core->dv_obj = parrot_open(0);
    // 前后镜头打开失败
    if(dv_core->dv_obj == NULL)
    {
        __msg("init parrot fail\n");
        esMEMS_Mfree(0, dv_core);
        dv_core = NULL;
        parrot_uninit();

        return EPDK_FAIL;
    }
    // 创建预览图层
    dv_core->h_pdisp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if(dv_core->h_pdisp == NULL)
    {
        __wrn("can not open display driver\n");
        parrot_close(dv_core->dv_obj);
        esMEMS_Mfree(0, dv_core);
        dv_core = NULL;

        return EPDK_FAIL;
    }
    dv_core->show_rect.x      = 0;
    dv_core->show_rect.y      = 0;
    dv_core->show_rect.width  = eLIBs_fioctrl(dv_core->h_pdisp, DISP_CMD_SCN_GET_WIDTH, 0, NULL);
    dv_core->show_rect.height = eLIBs_fioctrl(dv_core->h_pdisp, DISP_CMD_SCN_GET_HEIGHT , 0, NULL);
    __log(" dv_core->show_rect.width=%d, dv_core->show_rect.height=%d\n", dv_core->show_rect.width, dv_core->show_rect.height);
    //dv_core->show_rect.width  = 480;
    //dv_core->show_rect.height = 272;

    dv_core->show_layer = parrot_request_preview_layer(dv_core->show_rect, 0, 0xff);

    if(dv_core->show_layer == NULL)
    {
        __wrn("show layer = %x \n", dv_core->show_layer);
    }
    parrot_set_show_layer(dv_core->dv_obj, dv_core->show_layer);

    parrot_set_show_mode(dv_core->dv_obj, LOTUS_VID_WIN_FULLSCN);

    parrot_set_show_rect(dv_core->dv_obj, dv_core->show_rect);

    // 得到sensor的模式
    parrot_get_csi_mode(dv_core->dv_obj, &csi_mode_all);
    for(i = 0; i < csi_mode_all.number; i++)
    {
        __msg("i = %d, width = %d, height = %d \n", i, csi_mode_all.csi_mode[i].size.width, csi_mode_all.csi_mode[i].size.height);
        __msg("color_format = %d, store_mode = %d \n", csi_mode_all.csi_mode[i].color_format, csi_mode_all.csi_mode[i].store_mode);
    }
    // 获取录像对应的模式
    __dv_get_record_mode(&csi_mode_all, &dv_core->csi_mode_rec);
    for(i = 0; i < dv_core->csi_mode_rec.number; i++)
    {
        __msg("\n");
        __msg("i = %d, width = %d, height = %d \n", i, dv_core->csi_mode_rec.csi_mode[i].size.width, dv_core->csi_mode_rec.csi_mode[i].size.height);
        __msg("color_format = %d, store_mode = %d \n", dv_core->csi_mode_rec.csi_mode[i].color_format, dv_core->csi_mode_rec.csi_mode[i].store_mode);
        __msg("\n");
    }
    // 获取拍照对应的模式
    __dv_get_camera_mode(&csi_mode_all, &dv_core->csi_mode_cam);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               Cvr_DvConfigPara
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
/*

if the folder DCM not ,creat it
*/
static void set_folder_path(void)
{
    ES_DIR *dir;
    __s32 ret;


    dir = g_opendir(FILE_PATH);
    if( dir == NULL )
    {
        ret = g_mkdir(FILE_PATH);

        if(!ret)
    	{
	        __msg("--jh_dbg_0810_fail--\n");
    	}
    }
    else
    {
        __msg("--jh_dbg_0810_1--\n");
        g_closedir(dir);
        dir = NULL;
    }
}

__s32   Cvr_DvConfigPara(__dv_config_t *para)
{
    //char                file_path[256];

    dv_core->work_mode           = para->work_mode;
    dv_core->record_mute      = para->record_mute;
    dv_core->rec_quality_mode = para->rec_quality;
    dv_core->cam_quality_mode = para->cam_quality;

    // 录像的CSI_MODE设置
    __dv_get_record_size(1);
    __dv_get_cur_record_mode_index(0);
    // 拍照的CSI_MODE设置
    __dv_get_camera_size(1);
    __dv_get_cur_cam_mode_index(0);

    parrot_stop_preview(dv_core->dv_obj);

	__msg("dv_core->work_mode = %d\n", dv_core->work_mode);
    if(dv_core->work_mode == WORK_MODE_REC)
    {
        // 设置录像文件信息
        parrot_set_mode(dv_core->dv_obj, LOTUS_RECORD_MODE_CAMCORDER);
        __msg("----jh_dbg_0716_32----\n");
        set_folder_path();
        //ckk
        record_file_index=__dv_get_last_file(FILE_PATH);

        __dv_record_media_info(0);
        // not record this step
        if(record_file_index > 0)
            record_file_index--;
        parrot_set_media_file(dv_core->dv_obj, &dv_core->media_info);

        parrot_set_preview_mode(dv_core->dv_obj, dv_core->csi_mode_rec.csi_mode + dv_core->rec_cur_mode_index);
        parrot_start_preview(dv_core->dv_obj);

        dv_core->record_sta   = RECORD_STOP;
    }
    else if(dv_core->work_mode == WORK_MODE_CAM)
    {
        // 设置拍照文件信息
        parrot_set_mode(dv_core->dv_obj, LOTUS_RECORD_MODE_IMAGE);
		set_folder_path();
				//ckk
		record_file_index=__dv_get_last_file(FILE_PATH);

        __dv_camera_media_info(0);
		if(record_file_index > 0)
            record_file_index--;
        parrot_set_media_file(dv_core->dv_obj, &dv_core->media_info);

        parrot_set_preview_mode(dv_core->dv_obj, dv_core->csi_mode_cam.csi_mode + dv_core->cam_cur_mode_index);
        parrot_start_preview(dv_core->dv_obj);
    }
    else
    {
        __wrn("input work mode is error! \n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               Cvr_DvSetWorkMode
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvSetWorkMode(__work_mode_e mode)
{
    if(dv_core->work_mode == mode)
    {
        __wrn("input work mode is now work mode! \n");
        return EPDK_OK;
    }

    if(mode == WORK_MODE_REC)
    {
        // 进入录像模式
        parrot_set_mode(dv_core->dv_obj, LOTUS_RECORD_MODE_CAMCORDER);

        parrot_stop_preview(dv_core->dv_obj);

        __dv_record_media_info(0);
        parrot_set_media_file(dv_core->dv_obj, &dv_core->media_info);

        parrot_set_preview_mode(dv_core->dv_obj, dv_core->csi_mode_rec.csi_mode + dv_core->rec_cur_mode_index);
        parrot_start_preview(dv_core->dv_obj);
        dv_core->work_mode    = WORK_MODE_REC;
        dv_core->record_sta   = RECORD_STOP;
    }
    else if(mode == WORK_MODE_CAM)
    {
        // 进入拍照模式

        parrot_set_mode(dv_core->dv_obj, LOTUS_RECORD_MODE_IMAGE);

        parrot_stop_preview(dv_core->dv_obj);

        __dv_camera_media_info(0);
        parrot_set_media_file(dv_core->dv_obj, &dv_core->media_info);

        parrot_set_preview_mode(dv_core->dv_obj, dv_core->csi_mode_cam.csi_mode + dv_core->cam_cur_mode_index);
        parrot_start_preview(dv_core->dv_obj);

        dv_core->work_mode    = WORK_MODE_CAM;
    }
    else
    {
        __wrn("input work mode is error! \n");
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               Cvr_DvStartRecord
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvStartRecord(void)
{
	//__msg("Cvr_DvStartRecord, dv_core->work_mode = %d\n", dv_core->work_mode);
    // 非录像模式下调用此接口无效。
    if((dv_core->work_mode != WORK_MODE_REC) || (dv_core->record_sta == RECORD_START))
    {
    	//__msg("work mode error\n");
        return EPDK_FAIL;
    }
    // 设置录像文件信息
    __dv_record_media_info(0);

    parrot_set_media_file(dv_core->dv_obj, &dv_core->media_info);
	
    // 开始录像
    parrot_start_record(dv_core->dv_obj);
    dv_core->record_sta = RECORD_START;

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               DV_FuncStopRecord
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvStopRecord(void)
{
    // 非录像模式下调用此接口无效
    if((dv_core->work_mode != WORK_MODE_REC) || (dv_core->record_sta == RECORD_STOP))
    {
        return EPDK_FAIL;
    }

    // 停止录像
    parrot_stop_record(dv_core->dv_obj);

    dv_core->record_sta = RECORD_STOP;

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               Cvr_DvHaltRecord
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvHaltRecord(void)
{
    // 非录像模式下调用此接口无效
    if((dv_core->work_mode != WORK_MODE_REC) || (dv_core->record_sta == RECORD_HALT))
    {
        return EPDK_FAIL;
    }

    // 停止录像
    parrot_halt_record(dv_core->dv_obj);


    dv_core->record_sta = RECORD_HALT;

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               Cvr_DvGetRecState
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__record_state_e    Cvr_DvGetRecState(void)
{
    // 非录像模式下调用此接口无效
    if(dv_core->work_mode != WORK_MODE_REC)
    {
        return RECORD_STOP;
    }

    return dv_core->record_sta;
}

/*
**********************************************************************************************************************
*                                               Cvr_DvTakeImage
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvTakeImage(__u32 cnt)
{
    // 非拍照模式下调用此接口无效
    if(dv_core->work_mode != WORK_MODE_CAM)
    {
        return EPDK_FAIL;
    }
	__here__;
    // 设置拍照文件信息
    __dv_camera_media_info(0);
	__here__;
    parrot_set_media_file(dv_core->dv_obj, &dv_core->media_info);
    // 开始拍照，并取得缩略图。
    __here__;
    parrot_take_picture(dv_core->dv_obj, cnt, NULL);
	__here__;
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               Cvr_DvGetWorkMode
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__work_mode_e   Cvr_DvGetWorkMode(void)
{
    return dv_core->work_mode;
}

/*
**********************************************************************************************************************
*                                               Cvr_DvGetCurRecTime
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvGetCurRecTime(void)
{
    // 非录像模式下调用此接口无效。
    if(dv_core->work_mode != WORK_MODE_REC)
    {
        return 0;
    }

    return (parrot_get_record_time(dv_core->dv_obj) / 1000);
}


/*
**********************************************************************************************************************
*                                               Cvr_DvGetFileName
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvGetFileName(char *name, __u32 sensorID)
{
    char       *p;

    p = eLIBs_strchrlast(dv_core->media_info.file_path, '\\');
    eLIBs_strcpy(name, p + 1);

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               Cvr_DvSetRecQuality
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvSetRecQuality(__record_quality_e mode)
{
    // 正在录制中，调用此接口无效
    if(dv_core->record_sta != RECORD_STOP)
    {
        return EPDK_FAIL;
    }
    dv_core->rec_quality_mode = mode;
    __dv_get_record_size(0);
    __dv_get_cur_record_mode_index(0);

    __dv_rec_reset_preview();

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               Cvr_DvSetCamQuality
*
* Description: 注意这里只是仅仅记录一下拍照的质量，而不会根据拍照质量直接修改显示
*			预览，设置的拍照质量仅仅是按下拍照按键进行拍照时有用，预览仍然使用
* Arguments  :先前录像模式的预览
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvSetCamQuality(__camera_quality_e mode)
{
    dv_core->cam_quality_mode = mode;
    // 获取拍照对应的分辨率。
    __dv_get_camera_size(0);

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               Cvr_DvSetColorEffect
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvSetColorEffect(__csi_color_effect_t effect)
{
    parrot_set_ce(dv_core->dv_obj, (__csi_color_effect_t)effect);

    return EPDK_OK;

}

/*
**********************************************************************************************************************
*                                               Cvr_DvSetAWB
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvSetAWB(__csi_awb_t awb)
{

    parrot_set_awb(dv_core->dv_obj, (__csi_awb_t)awb);

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               Cvr_DvSetAE
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvSetAE(__csi_ae_t ae)
{
    parrot_set_ae(dv_core->dv_obj, (__csi_ae_t)ae);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               Cvr_DvSetBAND
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvSetBAND(__csi_band_t band)
{
    parrot_set_band(dv_core->dv_obj, (__csi_band_t)band);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               Cvr_SetMDSwitch
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_SetMDSwitch(__s32 state)
{

    parrot_set_md_gate(dv_core->dv_obj, state);

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               Cvr_GetRecordBps
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_GetRecordBps(void)
{
    return dv_core->media_info.video_inf.nVideoBps;
}

/*
**********************************************************************************************************************
*                                               Cvr_GetPhotoSize
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_GetPhotoSize(void)
{
    __u32       photo_size;

    switch(dv_core->cam_quality_mode)
    {
    case CAMERA_QUALITY_100:
        photo_size = 300;
        break;
    case CAMERA_QUALITY_200:
        photo_size = 600;
        break;
    case CAMERA_QUALITY_300:
        photo_size = 1000;
        break;
    case CAMERA_QUALITY_500:
        photo_size = 1500;
        break;
    case CAMERA_QUALITY_800:
        photo_size = 2000;
        break;
    default:
        photo_size = 2000;
    }


    return photo_size;
}

/*
**********************************************************************************************************************
*                                               Cvr_DvSetOverlaySrc
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvSetOverlaySrc(__lotus_overlay_src_init_t *src, __u32 sensorID)
{
    parrot_add_overlay_src(dv_core->dv_obj, src);

    return EPDK_OK;

}

/*
**********************************************************************************************************************
*                                               Cvr_DvShowOverlay
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvShowOverlay(__lotus_overlay_dis_par_t *para, __u32 sensorID)
{
    parrot_show_overlay_blk(dv_core->dv_obj, para);
    return EPDK_OK;

}

/*
**********************************************************************************************************************
*                                               Cvr_DvRecordMute
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvRecordMute(__u32 enable, __u32 sensorID)
{
    dv_core->record_mute = !enable;
    parrot_set_record_mute(dv_core->dv_obj, dv_core->record_mute);

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               Cvr_DvSetShowRect
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvSetShowRect(RECT rect, __u32 sensorID)
{
    parrot_set_show_rect(dv_core->dv_obj, rect);

    return EPDK_OK;

}

/*
**********************************************************************************************************************
*                                               Cvr_DvChangeNamePrefix
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
void    Cvr_DvChangeNamePrefix(char *prix_name)
{
    eLIBs_strncpy(dv_core->prefix_name, prix_name, MAX_PREFIX - 1);
}

/*
**********************************************************************************************************************
*                                               Cvr_DvClose
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   Cvr_DvClose(void)
{
    if(dv_core)
    {

        __msg("Cvr_DvClose\n");

        parrot_stop_preview(dv_core->dv_obj);
        // 卸载parrot
        if(dv_core->dv_obj != NULL)
        {
            parrot_close(dv_core->dv_obj);
            dv_core->dv_obj = NULL;
        }

        if(dv_core->show_layer)
        {
            parrot_release_preview_layer(dv_core->show_layer);
            dv_core->show_layer = NULL;
        }

        if(dv_core->h_pdisp)
        {
            eLIBs_fclose(dv_core->h_pdisp);
            dv_core->h_pdisp = NULL;
        }
        parrot_uninit();

        esMEMS_Mfree(0, dv_core);
        dv_core = NULL;
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               Cvr_DVStopPreview
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
void   Cvr_DVStopPreview(void)
{
    parrot_stop_preview(dv_core->dv_obj);
}

void   Cvr_DVStartPreview(void)
{
    parrot_start_preview(dv_core->dv_obj);
}


/*
**********************************************************************************************************************
*                                               Cvr_DVGetPreviewLayer
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/

__s32   Cvr_DVGetPreviewLayer(void)
{
    if(dv_core->show_layer)
    {
        return dv_core->show_layer;
    }

    return NULL;
}

