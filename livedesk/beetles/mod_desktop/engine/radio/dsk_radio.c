/*
**************************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           LARK app sample
*
*						        	(c) Copyright 2006-2007, TERRY, China
*											 All Rights Reserved
*
* File    : dsk_radio.c
* By      : terry
* Version : V1.00
* time    : 2009-12-01
**************************************************************************************************************
*/

#include "dsk_radio_i.h"

dsk_radio_t *h_radio = NULL;

/*
**********************************************************************************************************************
*                                               dsk_radio_open
*
* author:        terry.zeng
*
* date:             2009-12-01
*
* Description:      dsk_radio 打开函数(初始化FM播放功能)
*
* parameters:
*
* return:           if success return h_radio handle
*                   if fail return NULL
* modify history:
**********************************************************************************************************************
*/
__s32 dsk_radio_open(void)
{
    if(h_radio)
    {
        return EPDK_OK;
    }

    h_radio = (dsk_radio_t *)esMEMS_Malloc(0, sizeof(dsk_radio_t));
    if(h_radio == NULL)
    {
        __log("Error: malloc h_radio failed!\n");
        return EPDK_FAIL;
    }
    eLIBs_memset(h_radio, 0, sizeof(dsk_radio_t));

    /*加载FM驱动*/
    esDEV_Plugin("\\drv\\fm.drv", 0, 0, 1);
    /*打开FM驱动*/
    h_radio->fm_drv = eLIBs_fopen("b:\\USER\\FM", "r+");

    if(h_radio->fm_drv == NULL)
    {
        __log(" Error: open fm driver failed!\n ");
        return EPDK_FAIL;
    }
    h_radio->drv_status = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_INIT, 0, 0);

    return EPDK_OK;
}

__bool dsk_radio_is_open(void)
{
    if(h_radio)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}

__pCBK_t dsk_radio_set_cb( dsk_radio_event_e event, __pCBK_t cb, void *ctx )
{
    __pCBK_t  cb_old;

    switch( event )
    {
    case DSK_RADIO_EVENT_SEARCH_SUCCESS:
    {
        cb_old = cb_search_success;
        cb_search_success = cb;
        cb_ctx_search_success = ctx;
        break;
    }
    case DSK_RADIO_EVENT_SEARCH_FAIL:
    {
        cb_old = cb_search_fail;
        cb_search_fail = cb;
        cb_ctx_search_fail = ctx;
    }
    case DSK_RADIO_EVENT_SEARCH_OVER:
    {
        cb_old = cb_search_over;
        cb_search_over = cb;
        cb_ctx_search_over = ctx;
        break;
    }
    default :
    {
        cb_old = NULL;
    }
    }

    return cb_old;
}

__pCBK_t dsk_radio_get_cb( dsk_radio_event_e event )
{
    __pCBK_t   cb;

    switch( event )
    {
    case DSK_RADIO_EVENT_SEARCH_SUCCESS:
    {
        cb = cb_search_success;
        break;
    }
    default :
    {
        cb = NULL;
    }
    }

    return cb;
}

__s32 dsk_radio_mute(void)
{
    if(h_radio == NULL)
    {
        __err("radio not open!\n");
        return EPDK_FAIL;
    }

    return eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_MUTE, DRV_FM_VOICE_OFF, 0);
}

__s32 dsk_radio_unmute(void)
{
    if(h_radio == NULL)
    {
        __err("radio not open!\n");
        return EPDK_FAIL;
    }

    return eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_MUTE, DRV_FM_VOICE_ON, 0);
}

__s32 dsk_radio_set_band(dsk_radio_band_mode_t band_mode)
{
    __s32 result = 0;

    if(h_radio == NULL)
    {
        __err("radio not open!\n");
        return EPDK_FAIL;
    }

    if(band_mode == DSK_RADIO_BAND_JAPAN && h_radio->cur_band != DSK_RADIO_BAND_JAPAN)
    {
        result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_AREA_SET, DVR_FM_AREA_JAPAN, &h_radio->freq_range);
        h_radio->cur_band = DSK_RADIO_BAND_JAPAN;
    }
    else if(band_mode == DSK_RADIO_BAND_SCHOOL && h_radio->cur_band != DSK_RADIO_BAND_SCHOOL)
    {
        result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_AREA_SET, DRV_FM_AREA_CHN_SCHOOL, &h_radio->freq_range);
        h_radio->cur_band = DSK_RADIO_BAND_SCHOOL;
    }
    else
    {

        result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_AREA_SET, DRV_FM_AREA_CHN_US_EUP, &h_radio->freq_range);

        h_radio->cur_band = DSK_RADIO_BAND_US_EUP;

    }

    return result;
}

__s32 dsk_radio_set_threshold(dsk_radio_threshold_mode_t threshold)
{
    __s32 result = 0;

    if(h_radio == NULL)
    {
        __err("radio not open!\n");
        return EPDK_FAIL;
    }

    if(threshold == DSK_RADIO_THRESHOLD_HIGH)
    {
        result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_SLEVEL_SET, DRV_FM_SLEVEL_HIGH, 0);
    }
    else if(threshold == DSK_RADIO_THRESHOLD_LOW)
    {
        result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_SLEVEL_SET, DRV_FM_SLEVEL_LOW, 0);
    }
    else
    {
        result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_SLEVEL_SET, DRV_FM_SLEVEL_NORMAL, 0);
    }

    return result;
}

__s32 dsk_radio_set_audio_mode(dsk_radio_audio_mode_t audio_mode)
{
    __s32 result = 0;

    if(h_radio == NULL)
    {
        __err("radio not open!\n");
        return EPDK_FAIL;
    }

    if(audio_mode == DSK_RADIO_AUDIO_MONO)
    {
        result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_STEREO_SET, DRV_FM_MONO, 0);
        h_radio->audio_mode = DSK_RADIO_AUDIO_MONO;
    }
    else
    {
        result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_STEREO_SET, DRV_FM_STEREO, 0);
        h_radio->audio_mode = DSK_RADIO_AUDIO_STEREO;
    }

    return result;
}

__s32 dsk_radio_get_band(__s32 *cur_band)
{
    if(h_radio == NULL)
    {
        __err("radio not open!\n");
        return EPDK_FAIL;
    }

    *cur_band = h_radio->cur_band;

    return EPDK_OK;
}

__s32 dsk_radio_get_audio_mode(__s32 *audio_mode)
{
    if(h_radio == NULL)
    {
        __err("radio not open!\n");
        return EPDK_FAIL;
    }

    *audio_mode = h_radio->audio_mode;

    return EPDK_OK;
}

__s32 dsk_radio_get_min_freq(__s32 *min_freq)
{
    if(h_radio == NULL)
    {
        __err("radio not open!\n");
        return EPDK_FAIL;
    }

    *min_freq = h_radio->freq_range.fm_area_min_freq;

    return EPDK_OK;
}

__s32 dsk_radio_get_max_freq(__s32 *max_freq)
{
    if(h_radio == NULL)
    {
        __err("radio not open!\n");
        return EPDK_FAIL;
    }

    *max_freq = h_radio->freq_range.fm_area_max_freq;

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               dsk_radio_control
*
* author:        terry.zeng
*
* date:             2009-12-01
*
* Description:      dsk_radio功能操作接口,参数1一般为输入参数，参数2为输出参数
*
* parameters:
*
* return:           if success return 0
*                   if fail return -1
* modify history:
**********************************************************************************************************************
*/
/*__s32 dsk_radio_control(__s32 cmd,__s32 param1,__s32 param2)
{
	__s32 result = 0;

	if(h_radio == NULL)
	{
		__err("radio not open!\n");
		return EPDK_FAIL;
	}

	switch(cmd)
	{
		case DSK_RADIO_MUTE:
		{
			result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_MUTE, DRV_FM_VOICE_OFF, 0);
			break;
		}
		case DSK_RADIO_UNMUTE:
		{
			result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_MUTE, DRV_FM_VOICE_ON, 0);
			break;
		}
		case DSK_RADIO_SET_BAND:
		{
			if(param1 == DSK_RADIO_BAND_JAPAN && h_radio->cur_band != DSK_RADIO_BAND_JAPAN)
            {
                result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_AREA_SET, DVR_FM_AREA_JAPAN, &h_radio->freq_range);
                h_radio->cur_band = DSK_RADIO_BAND_JAPAN;
            }
            else if(param1 == DSK_RADIO_BAND_SCHOOL && h_radio->cur_band != DSK_RADIO_BAND_SCHOOL)
            {
                result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_AREA_SET, DRV_FM_AREA_CHN_SCHOOL, &h_radio->freq_range);
                h_radio->cur_band = DSK_RADIO_BAND_SCHOOL;
            }
            else
            {

                result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_AREA_SET, DRV_FM_AREA_CHN_US_EUP, &h_radio->freq_range);

                h_radio->cur_band = DSK_RADIO_BAND_US_EUP;

            }
			break;
		}
		case DSK_RADIO_SET_HTRESHOLD:
		{
			if(param1 == DSK_RADIO_THRESHOLD_HIGH)
            {
                result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_SLEVEL_SET, DRV_FM_SLEVEL_HIGH, 0);
            }
            else if(param1 == DSK_RADIO_THRESHOLD_LOW)
            {
                result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_SLEVEL_SET, DRV_FM_SLEVEL_LOW, 0);
            }
            else
            {
                result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_SLEVEL_SET, DRV_FM_SLEVEL_NORMAL, 0);
            }
			break;
		}
		case DSK_RADIO_SET_AUDIO_MODE:
		{
			if(param1 == DSK_RADIO_AUDIO_MONO)
            {
                result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_STEREO_SET, DRV_FM_MONO, 0);
                h_radio->audio_mode = DSK_RADIO_AUDIO_MONO;
            }
            else
            {
                result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_STEREO_SET, DRV_FM_STEREO, 0);
                h_radio->audio_mode = DSK_RADIO_AUDIO_STEREO;

            }
			break;
		}
		case DSK_RADIO_GET_BAND:
		{
			__s32 *band = (__s32 *)param2;
    		*band = h_radio->cur_band;
			break;
		}
		case DSK_RADIO_GET_FREQ:
		{
			__u32 freq;

//			__u32 *ret = (__u32 *)param2;

			if(h_radio->drv_status == FM_DRV_STATUS_SEND)
			{
            	dsk_radio_send_get_cur_freq(&freq);
            }
            else
            {

            	dsk_radio_rcv_get_cur_freq(&freq);

            }
            *((__u32 *)param2) = freq;
			break;
		}
		case DSK_RADIO_GET_AUDIO_MODE:
		{
			__u32 *track = (__u32 *)param2;
            *track = h_radio->audio_mode;

			break;
		}
		case DSK_RADIO_GET_MIN_FREQ:
		{
			__s32 *min_freq = (__s32 *)param2;
            *min_freq = h_radio->freq_range.fm_area_min_freq;
			break;
		}
		case DSK_RADIO_GET_MAX_FREQ:
		{
			__s32 *max_freq = (__s32 *)param2;
            *max_freq = h_radio->freq_range.fm_area_max_freq;
			break;
		}
		case DSK_RADIO_REC_OPEN:
		{
			if(h_radio->drv_status == FM_DRV_STATUS_SEND)
			{
				dsk_radio_send_close();
				h_radio->rcv_handle = dsk_radio_rcv_open();
				if(h_radio->rcv_handle == NULL)
				{
					result = -1;
				}
			}
			else
			{

				h_radio->rcv_handle = dsk_radio_rcv_open();
				if(h_radio->rcv_handle == NULL)
				{
					result = -1;
				}
			}
			h_radio->drv_status = FM_DRV_STATUS_RECEIVE;
			break;
		}
		case DSK_RADIO_REC_SET_FREQ_PLAY:
		{

			result = dsk_radio_rcv_set_freq_play(param1);
			if(result > 0)
			{
				h_radio->cur_freq = result;
				result = 0;
			}

			break;
		}
		case DSK_RADIO_REC_NEXT_FREQ_PLAY:
		{
			result = dsk_radio_rcv_next_freq_play();
			break;
		}
		case DSK_RADIO_REC_PRE_FREQ_PLAY:
		{
			result = dsk_radio_rcv_pre_freq_play();
			break;
		}
		case DSK_RADIO_REC_SET_AUTO_SEARCH_MODE:
		{
			result = dsk_radio_rcv_set_search_mode(param1);
			break;
		}
		case DSK_RADIO_REC_AUTO_SEARCH:
		{
			result = dsk_radio_rcv_auto_search();
			if(result > 0)
			{
				h_radio->cur_freq = result;
				result = 0;
			}
			break;
		}
		case DSK_RADIO_REC_AUTO_SEARCH_ALL:
		{
			result = dsk_radio_rcv_search_all();
			break;
		}

		case DSK_RADIO_REC_SET_PLAY_LIST_TYPE:
		{
			result = dsk_radio_rcv_set_play_list_type( (__play_list_type_e)param1);
			break;
		}

		case DSK_RADIO_REC_GET_PLAY_LIST_TYPE:
		{
			result = dsk_radio_rcv_get_play_list_type( (__s32 *)param1);
			break;
		}
		case DSK_RADIO_REC_GET_SEARCH_RESULT_VN:
		{
			result = dsk_radio_rcv_get_search_result_vn( (__s32 *)param1);
			break;
		}
		case DSK_RADIO_REC_GET_FAVORITE_VN:
		{
			result = dsk_radio_rcv_get_favorite_vn( (__s32 *)param1);
			break;
		}
		case DSK_RADIO_REC_GET_SEARCH_RESULT_CHAN:
		{
			result = dsk_radio_rcv_get_search_result_chaninfo( (__s32 )param1, (dsk_radio_chan_t *)param2);
			break;
		}
		case DSK_RADIO_REC_GET_FAVORITE_CHAN:
		{
			result = dsk_radio_rcv_get_favorite_chaninfo( (__s32 )param1, (dsk_radio_chan_t *)param2);
			break;
		}
		case DSK_RADIO_REC_GET_FAVORITE:
		{
			result = dsk_radio_rcv_get_favorite((char *)param1);
			break;
		}
		case DSK_RADIO_REC_SAVE_FAVORITE:
		{
			result = dsk_radio_rcv_save_favorite((char *)param1);
			break;
		}
		case DSK_RADIO_REC_GET_SEARCH_RESULT:
		{
			result = dsk_radio_rcv_get_search_result((char *)param1);
			break;
		}
		case DSK_RADIO_REC_SAVE_SEARCH_RESULT:
		{
			result = dsk_radio_rcv_save_search_result((char *)param1);
			break;
		}
		case DSK_RADIO_REC_ADD_CHAN_TO_FAVORITE:
		{
			result = dsk_radio_rcv_add_chan_to_favorite( (__s32 )param1, (dsk_radio_chan_t *)param2);
			break;
		}
		case DSK_RADIO_REC_DEL_CHAN_FROM_FAVORITE:
		{
			result = dsk_radio_rcv_del_chan_from_favorite( (__s32 )param1);
			break;
		}
		case DSK_RADIO_REC_CLOSE:
		{
			result = dsk_radio_rcv_close();
			h_radio->rcv_handle = NULL;
			break;
		}
		case DSK_RADIO_SEND_OPEN:
		{
			h_radio->drv_status = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_QUERY_STATUS, 0, 0);
			if(h_radio->drv_status == FM_DRV_STATUS_RECEIVE)
			{
				dsk_radio_rcv_close();
				h_radio->send_handle = dsk_radio_send_open();
				if(h_radio->send_handle == NULL)
				{
					result = -1;
				}
			}
			else
			{
				h_radio->send_handle = dsk_radio_send_open();
				if(h_radio->send_handle == NULL)
				{
					result = -1;
				}
			}
			h_radio->drv_status = FM_DRV_STATUS_SEND;
			break;
		}
		case DSK_RADIO_SEND_SET_FREQ_PLAY:
		{
			result = dsk_radio_send_set_freq_play(param1);
			break;
		}
		case DSK_RADIO_SEND_SET_GAIN:
		{
			result = dsk_radio_send_set_gain(param1);
			break;
		}
		case DSK_RADIO_SEND_GET_GAIN:
		{
			__u32 *gain = (__u32 *)param2;
			result = dsk_radio_send_get_gain(gain);
			break;
		}
		case DSK_RADIO_SEND_CLOSE:
		{
			result = dsk_radio_send_close();
			h_radio->send_handle = NULL;
			break;
		}
		default:
			break;
	}
	return result;
}*/

/*
**********************************************************************************************************************
*                                               dsk_radio_close
*
* author:        terry.zeng
*
* date:             2009-12-01
*
* Description:       关闭dsk_radio，释放相关资源
*
* parameters:
*
* return:           if success return 0
*                   if fail return -1
* modify history:
**********************************************************************************************************************
*/
__s32 dsk_radio_close(void)
{
    __s32 result = 0;

    if(h_radio == NULL)
    {
        __msg("radio not open!\n");
        return EPDK_FAIL;
    }

    if(h_radio->fm_drv != NULL)
    {
        //if(h_radio->send_handle != NULL)
        {
            dsk_radio_send_close();
            //h_radio->send_handle = NULL;
        }
        //if(h_radio->rcv_handle != NULL)
        {
            dsk_radio_rcv_close();
            //h_radio->rcv_handle = NULL;
        }
        /*退出FM驱动*/
        result = eLIBs_fioctrl(h_radio->fm_drv, DRV_FM_CMD_EXIT, 0, 0);
        /*关闭FM驱动*/
        result = eLIBs_fclose(h_radio->fm_drv);
    }
    /*卸载FM驱动*/
    esDEV_Plugout("\\drv\\fm.drv", 0);
    /*free fm_engine,so the memory can be used for other*/
    esMEMS_Mfree(0, h_radio);
    h_radio = NULL;

    return result;
}

