/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           home app sample
*
*						        	(c) Copyright 2006-2009, LYN, China
*											 All Rights Reserved
*
* File    : parrot.c
* By      : lyn
* Version : V1.00
*********************************************************************************************************
*/
#include "epdk.h"
#include "lib_ex.h"

#if  1
#define __here__            eLIBs_printf("@L%d(%s)\n", __LINE__, __FILE__);
//#define __msg(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
//						     eLIBs_printf(__VA_ARGS__)									        )
#else
#define __msg(...)    	
#endif


typedef  struct  tag_PARROT_DATA
{
    __s32       herb_obj;
    __s32       herb_lyr;
    __s32		sensor_index;
} __parrot_data_t;

static  __u8        mid_lotus = 0;
static  __mp        *mod_lotus = NULL;
static  ES_FILE     *parrot_disp = NULL;

__s32   parrot_init(void)
{
    __msg("parrot_init\n");

    //	esDEV_Plugin("\\drv\\di.drv", 0, 0, 1);
    __here__;
    //	esDEV_Plugin("\\drv\\tvd.drv", 0, 0, 1);
    __here__;
RETRY:
    mid_lotus = esMODS_MInstall( "d:\\mod\\lotus\\lotus.mod", 0 );
    if(mid_lotus == NULL)
    {
        __wrn( "install lotus fail\n" );
        goto RETRY;
    }
    __here__;
    mod_lotus = esMODS_MOpen(mid_lotus, 0);
    if(mod_lotus == NULL)
    {
        __wrn( "open herb fail\n" );
        esMODS_MUninstall(mid_lotus);
        return EPDK_FAIL;
    }
    __here__;
    parrot_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "rb");
    if(parrot_disp == NULL)
    {
        __wrn("open display drv fail\n");
        esMODS_MClose(mod_lotus);
        mod_lotus = NULL;
        esMODS_MUninstall(mid_lotus);
        mid_lotus = 0;

        return EPDK_FAIL;
    }
    __here__;
    return EPDK_OK;
}

__s32   parrot_uninit(void)
{

    __msg("parrot_uninit\n");

    if(parrot_disp)
    {
        eLIBs_fclose(parrot_disp);
        parrot_disp = 0;
    }
    if(mod_lotus)
    {
        esMODS_MClose(mod_lotus);
        mod_lotus = 0;
    }
    if(mid_lotus)
    {
        esMODS_MUninstall(mid_lotus);
        mid_lotus = 0;
    }
    //	esDEV_Plugout("\\drv\\tvd.drv", 0 );
    //	esDEV_Plugout("\\drv\\di.drv", 0 );
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       parrot_open
*
* Description: get one parrot hdle
*
* Arguments  : index: sensor index
*
* Returns    : hdle or NULL
*
* Note       :
*********************************************************************************************************
*/
__hdle   parrot_open(__s32 index)
{
    __parrot_data_t     *parrot_data;
    __here__;
    parrot_data = esMEMS_Malloc(0, sizeof(__parrot_data_t));
    if(parrot_data == NULL)
    {
        __wrn("request memery for __parrot_dat_t fail\n");
        return NULL;
    }
    eLIBs_memset(parrot_data, 0, sizeof(__parrot_data_t));
    __here__;
    parrot_data->herb_obj = esMODS_MIoctrl(mod_lotus, LOTUS_CMD_NEW_OBJ, 0, 0);
    if(parrot_data->herb_obj == 0)
    {
        __wrn("open herb.mod err!\n");
        esMEMS_Mfree(0, parrot_data);
        parrot_data = NULL;

        return NULL;
    }
    __here__;
    esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SET_CSI, parrot_data->herb_obj, (void *)index);
    parrot_data->sensor_index = index;

    return (__hdle)parrot_data;
}

/*
*********************************************************************************************************
*                                       parrot_close
*
* Description: get one parrot hdle
*
* Arguments  : hdle: parrot hdle
*
* Returns    : hdle or NULL
*
* Note       :
*********************************************************************************************************
*/
__s32   parrot_close(__hdle hdle)
{
    __parrot_data_t     *parrot_data;
    __lotus_status_t     parrot_status;

    if(hdle == NULL)
    {
        return EPDK_OK;
    }
    parrot_data = (__parrot_data_t *)hdle;
    if(parrot_data->herb_obj)
    {
        parrot_status = (__lotus_status_t)esMODS_MIoctrl(mod_lotus, LOTUS_CMD_GET_STATUS, parrot_data->herb_obj, NULL);
        if((parrot_status == LOTUS_STAT_RECORD) || (parrot_status == LOTUS_STAT_PAUSE))
        {
            esMODS_MIoctrl(mod_lotus, LOTUS_CMD_STOP_RECORD, 0, NULL);
        }
        esMODS_MIoctrl(mod_lotus, LOTUS_CMD_STOP, parrot_data->herb_obj, NULL);
        esMODS_MIoctrl(mod_lotus, LOTUS_CMD_DEL_OBJ, parrot_data->herb_obj, NULL);
    }

    esMEMS_Mfree(0, parrot_data);
    parrot_data = NULL;

    return EPDK_OK;
}

__s32   parrot_get_csi_mode(__hdle hdle, __csi_mode_all_t *mode)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return (esMODS_MIoctrl(mod_lotus, LOTUS_CMD_GET_OUT_MODE_ALL, parrot_data->herb_obj, (void *)mode));
}

__s32   parrot_set_mode(__hdle hdle, __lotus_record_mode_t mode)
{
    __parrot_data_t     *parrot_data;
    __lotus_status_t     parrot_status;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    parrot_status = (__lotus_status_t)esMODS_MIoctrl(mod_lotus, LOTUS_CMD_GET_STATUS, parrot_data->herb_obj, NULL);
    if((parrot_status == LOTUS_STAT_RECORD) || (parrot_status == LOTUS_STAT_PAUSE))
    {
        // 非录像停止状态设置模式无效
        return EPDK_FAIL;
    }
    esMODS_MIoctrl(mod_lotus, LOTUS_CMD_STOP, parrot_data->herb_obj, NULL);
    return esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SET_MODE, parrot_data->herb_obj, (void *)mode);
}

__s32   parrot_set_media_file(__hdle hdle, __lotus_media_file_inf *info)
{
    __parrot_data_t     *parrot_data;
    __lotus_status_t     parrot_status;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    parrot_status = (__lotus_status_t)esMODS_MIoctrl(mod_lotus, LOTUS_CMD_GET_STATUS, parrot_data->herb_obj, NULL);

	__here__;

//    __msg("uAbsFmt = %d\n", info->audio_inf.uAbsFmt);
//    __msg("uVolume = %d\n", info->audio_inf.uVolume);
//    __msg("uChn = %d\n", info->audio_inf.uChn);
//    __msg("uSampWidth = %d\n", info->audio_inf.uSampWidth);
//    __msg("uAudioBps = %d\n", info->audio_inf.uAudioBps);
//    __msg("uAudioSampRate = %d\n", info->audio_inf.uAudioSampRate);

    if((parrot_status == LOTUS_STAT_IDLE) || (parrot_status == LOTUS_STAT_STANDBY))
    {
        return esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SET_MEDIAFILE, parrot_data->herb_obj, info);
    }
    else
    {
        return EPDK_FAIL;
    }
}

__s32   parrot_set_record_thumb(__hdle hdle, FB *fb)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return esMODS_MIoctrl(mod_lotus, LOTUS_CMD_NEED_PREVIEW_PIC_FOR_RECORD, parrot_data->herb_obj, fb);
}

__s32   parrot_set_preview_mode(__hdle hdle, __csi_mode_t *csi_mode)
{
    __parrot_data_t     *parrot_data;
    __lotus_status_t     parrot_status;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    parrot_status = (__lotus_status_t)esMODS_MIoctrl(mod_lotus, LOTUS_CMD_GET_STATUS, parrot_data->herb_obj, NULL);
    if((parrot_status == LOTUS_STAT_IDLE) || (parrot_status == LOTUS_STAT_STANDBY))
    {
        esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SET_PREVIEW_OUT_MODE, parrot_data->herb_obj, csi_mode);

        esMODS_MIoctrl(mod_lotus, LOTUS_CMD_START, parrot_data->herb_obj, NULL);

        return EPDK_OK;
    }
    else
    {
        return EPDK_FAIL;
    }
}

__s32   parrot_start_preview(__hdle hdle)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return esMODS_MIoctrl(mod_lotus, LOTUS_CMD_ENABLE_PLAYBACK, parrot_data->herb_obj, (void *)1);
}

__s32   parrot_stop_preview(__hdle hdle)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    esMODS_MIoctrl(mod_lotus, LOTUS_CMD_ENABLE_PLAYBACK, parrot_data->herb_obj, (void *)0);
    return EPDK_OK;
}

__s32   parrot_start_record(__hdle hdle)
{
    __parrot_data_t     *parrot_data;
    __lotus_status_t     parrot_status;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    parrot_status = (__lotus_status_t)esMODS_MIoctrl(mod_lotus, LOTUS_CMD_GET_STATUS, parrot_data->herb_obj, NULL);
    if(parrot_status == LOTUS_STAT_STANDBY)
    {
        return (esMODS_MIoctrl(mod_lotus, LOTUS_CMD_START_RECORD, parrot_data->herb_obj, NULL));
    }
    else
    {
        return EPDK_FAIL;
    }
}

__s32   parrot_halt_record(__hdle hdle)
{
    __parrot_data_t     *parrot_data;
    __lotus_status_t     parrot_status;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    parrot_status = (__lotus_status_t)esMODS_MIoctrl(mod_lotus, LOTUS_CMD_GET_STATUS, parrot_data->herb_obj, NULL);
    if(parrot_status == LOTUS_STAT_RECORD)
    {
        return (esMODS_MIoctrl(mod_lotus, LOTUS_CMD_HOLD_ON, parrot_data->herb_obj, NULL));
    }
    else
    {
        return EPDK_FAIL;
    }
}

__s32   parrot_stop_record(__hdle hdle)
{
    __parrot_data_t     *parrot_data;
    __lotus_status_t     parrot_status;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    parrot_status = (__lotus_status_t)esMODS_MIoctrl(mod_lotus, LOTUS_CMD_GET_STATUS, parrot_data->herb_obj, NULL);
    if(parrot_status == LOTUS_STAT_RECORD)
    {
        return (esMODS_MIoctrl(mod_lotus, LOTUS_CMD_STOP_RECORD, parrot_data->herb_obj, NULL));
    }
    else
    {
        return EPDK_FAIL;
    }
}

__s32   parrot_get_record_time(__hdle hdle)
{
    __parrot_data_t     *parrot_data;
    __lotus_status_t     parrot_status;

    if(hdle == NULL)
    {
        return 0;
    }
    parrot_data = (__parrot_data_t *)hdle;

    parrot_status = (__lotus_status_t)esMODS_MIoctrl(mod_lotus, LOTUS_CMD_GET_STATUS, parrot_data->herb_obj, NULL);
    if(parrot_status == LOTUS_STAT_RECORD)
    {
        return (esMODS_MIoctrl(mod_lotus, LOTUS_CMD_GET_CUR_TIME, parrot_data->herb_obj, NULL));
    }
    else
    {
        return 0;
    }
}

__s32   parrot_take_picture(__hdle hdle, __u32 cnt, FB *fb)
{
    __parrot_data_t     *parrot_data;
    __lotus_status_t     parrot_status;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    parrot_status = (__lotus_status_t)esMODS_MIoctrl(mod_lotus, LOTUS_CMD_GET_STATUS, parrot_data->herb_obj, NULL);
    if(parrot_status == LOTUS_STAT_STANDBY)
    {
    	__here__;
        return (esMODS_MIoctrl(mod_lotus, LOTUS_CMD_TAKE_IMAGE, parrot_data->herb_obj, fb));
    }
    else
    {
    	__here__;
        return EPDK_FAIL;
    }
}

__s32   parrot_set_md_gate(__hdle hdle, __s32 enable)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    if(enable)
    {
        return (esMODS_MIoctrl(mod_lotus, LOTUS_CMD_MD_ONOFF, parrot_data->herb_obj, (void *)1));
    }

    else
    {
        return (esMODS_MIoctrl(mod_lotus, LOTUS_CMD_MD_ONOFF, parrot_data->herb_obj, (void *)0));
    }
}

__s32   parrot_set_md_para(__hdle hdle, __lotus_motion_dectect_para_t *para)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SET_MD_PARA, parrot_data->herb_obj, para);
}

__s32   parrot_get_md_result(__hdle hdle)
{
    __s32				ret, result;
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;
    ret = esMODS_MIoctrl(mod_lotus, LOTUS_CMD_GET_MD_DETECT, parrot_data->herb_obj, (void *)&result);
    if(ret == EPDK_OK)
    {
        return result;
    }
    return -1;
}


__s32   parrot_request_preview_layer(RECT rect, __s32 pipe, __s32 prio)
{
    __s32               hlayer;

    __u32                   arg[3];
    __disp_fb_t             image_fb_para;
    __disp_layer_info_t     image_layer_info;

    image_fb_para.size.height      	= 0;                    // DONT'T CARE
    image_fb_para.size.width       	= 0;                    // DONT'T CARE
    image_fb_para.addr[0]          	= NULL;
    image_fb_para.format         	= DISP_FORMAT_RGB565;   // DONT'T CARE
    image_fb_para.seq     			= DISP_SEQ_ARGB;  	    // DONT'T CARE
    image_fb_para.mode      		= 0;                    // DONT'T CARE
    image_fb_para.br_swap       	= 0;                    // DONT'T CARE
    image_fb_para.cs_mode 			= NULL;

    image_layer_info.mode			= DISP_LAYER_WORK_MODE_NORMAL;
    image_layer_info.pipe			= pipe;
    image_layer_info.prio			= prio;
    image_layer_info.alpha_en		= 0;
    image_layer_info.alpha_val		= 255;
    image_layer_info.ck_enable		= 0;
    image_layer_info.src_win.x      = 0;
    image_layer_info.src_win.y      = 0;
    image_layer_info.src_win.width  = rect.width ;
    image_layer_info.src_win.height = rect.height;
    image_layer_info.scn_win.x      = rect.x     ;
    image_layer_info.scn_win.y      = rect.y     ;
    image_layer_info.scn_win.width  = rect.width ;
    image_layer_info.scn_win.height = rect.height;
    image_layer_info.fb				= image_fb_para;


    __msg("image_layer_info.scn_win.width  = %d\n", image_layer_info.scn_win.width);
    __msg("image_layer_info.scn_win.height = %d\n", image_layer_info.scn_win.height);

    arg[0] = DISP_LAYER_WORK_MODE_NORMAL;
    arg[1] = 0;
    arg[2] = 0;
    hlayer = eLIBs_fioctrl(parrot_disp, DISP_CMD_LAYER_REQUEST, 0, (void *)arg);
    if(hlayer == NULL)
    {
        __wrn("Error in applying for the preview layer.\n");
        return NULL;
    }

    arg[0] = hlayer;
    arg[1] = (__u32)&image_layer_info;
    arg[2] = 0;
    eLIBs_fioctrl(parrot_disp, DISP_CMD_LAYER_SET_PARA, 0, (void *)arg);

    arg[0] = hlayer;
    arg[1] = 0;
    arg[2] = 0;
    eLIBs_fioctrl(parrot_disp, DISP_CMD_LAYER_BOTTOM, 0, (void *)arg);

    return hlayer;
}

__s32   parrot_release_preview_layer(__s32  hlayer)
{
    __u32       arg[3];

    if(hlayer)
    {
        arg[0] = hlayer;
        arg[1] = 0;
        arg[2] = 0;
        eLIBs_fioctrl(parrot_disp, DISP_CMD_LAYER_RELEASE, 0, (void *)arg);
        hlayer = NULL;
    }

    return EPDK_OK;
}

__s32   parrot_set_show_layer(__hdle hdle, __s32 hlayer)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;
    parrot_data->herb_lyr = hlayer;

    if(EPDK_FAIL == esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SET_VID_LAYERHDL, parrot_data->herb_obj, (void *)hlayer))
    {
        return EPDK_FAIL;
    }
    esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SWITCH_VID_SHOW, parrot_data->herb_obj, (void *)1);

    return EPDK_OK;
}

__s32   parrot_set_show_switch(__hdle hdle, __s32 enable)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    if(enable == 0)
    {
        esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SWITCH_VID_SHOW, parrot_data->herb_obj, (void *)0);
    }
    else
    {
        esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SWITCH_VID_SHOW, parrot_data->herb_obj, (void *)1);
    }

    return EPDK_OK;
}

__s32   parrot_set_show_mode(__hdle hdle, __lotus_vid_win_ratio_mode_t mode)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SET_VID_SHOW_MODE, parrot_data->herb_obj, (void *)mode);
}

__s32   parrot_set_show_rect(__hdle hdle, RECT rect)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SET_VID_WINDOW, parrot_data->herb_obj, (void *) & (rect));
}

__s32   parrot_set_ce(__hdle hdle, __csi_color_effect_t ce)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SET_COLOR_EFFECT, parrot_data->herb_obj, (void *)ce);
}

__s32   parrot_set_awb(__hdle hdle, __csi_awb_t awb)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SET_AWB, parrot_data->herb_obj, (void *)awb);
}

__s32   parrot_set_ae(__hdle hdle, __csi_ae_t ae)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SET_AE, parrot_data->herb_obj, (void *)ae);
}

__s32   parrot_set_band(__hdle hdle, __csi_band_t band)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SET_BAND, parrot_data->herb_obj, (void *)band);
}

__csi_color_effect_t   parrot_get_ce(__hdle hdle)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return (__csi_color_effect_t)0;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return (__csi_color_effect_t)esMODS_MIoctrl(mod_lotus, LOTUS_CMD_GET_COLOR_EFFECT, parrot_data->herb_obj, NULL);
}

__csi_awb_t   parrot_get_awb(__hdle hdle)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return (__csi_awb_t)0;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return (__csi_awb_t)esMODS_MIoctrl(mod_lotus, LOTUS_CMD_GET_AWB, parrot_data->herb_obj, NULL);
}

__csi_ae_t   parrot_get_ae(__hdle hdle)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return (__csi_ae_t)0;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return (__csi_ae_t)esMODS_MIoctrl(mod_lotus, LOTUS_CMD_GET_AE, parrot_data->herb_obj, NULL);
}

__csi_band_t   parrot_get_band(__hdle hdle)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return (__csi_band_t)0;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return (__csi_band_t)esMODS_MIoctrl(mod_lotus, LOTUS_CMD_GET_BAND, parrot_data->herb_obj, NULL);
}

__s32   parrot_add_overlay_src(__hdle hdle, __lotus_overlay_src_init_t *src)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return esMODS_MIoctrl(mod_lotus, LOTUS_CMD_ADD_OVERLAY_SRC, parrot_data->herb_obj, (void *)src);
}

__s32   parrot_show_overlay_blk(__hdle hdle, __lotus_overlay_dis_par_t *para)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SHOW_OVERLAY_BLK, parrot_data->herb_obj, (void *)para);
}

/*
*********************************************************************************************************
*                                   parrot_set_record_mute
*
* Description: recording mute switch.
*
* Arguments  :  hdle: lotus hdle
*               state: mute switch(0->off, 1->on)
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__s32	parrot_set_record_mute(__hdle hdle, __s32 state)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return esMODS_MIoctrl(mod_lotus, LOTUS_CMD_MUTE, parrot_data->herb_obj, (void *)state);
}

/*
*********************************************************************************************************
*                                   parrot_set_output_mode
*
* Description: set file storage type.
*
* Arguments  :  hdle: lotus hdle
*               type: storage type(0->SD, 1->memery)
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__s32   parrot_set_output_mode(__hdle hdle, __lotus_muxer_type_t type)
{
    __parrot_data_t     *parrot_data;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    parrot_data = (__parrot_data_t *)hdle;

    return esMODS_MIoctrl(mod_lotus, LOTUS_CMD_SET_MUXER_OUT, parrot_data->herb_obj, (void *)type);
}

