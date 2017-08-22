/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           home app sample
*
*						        	(c) Copyright 2006-2009, China
*											 All Rights Reserved
*
* File    : home.c
* By      : lyn
* Version : V1.00
*********************************************************************************************************
*/

#include "dv_i.h"
#include "dv_uipara.h"
#include "dv_frm.h"
#include "dv_common.h"
#include "app_root_scene.h"

#if 1
//#define __here__            eLIBs_printf("@L%d(%s)\n", __LINE__, __FILE__);
#define __mymsg(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
						     eLIBs_printf(__VA_ARGS__)									        )
//#define __inf(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
//								 eLIBs_printf(__VA_ARGS__)											)
#else
#define __inf(...)
#define __mymsg(...)   

#endif

typedef struct   valuet
{
    __u32   single_value;
    __u32   resverd;
} single_t;

#ifdef DV_FRM_SAVE_FREQ
__u8  freq_save_flg = 0;
__u8  freq_save_cnt = 0;
__u8  freq_save_switch = 0;
__s32 freq_save_buff[32][2] = {0};
#endif

typedef struct __auto_search
{
	__u8 channel;
	__u8 search_end;
	__u8 prev_value;
	__u8 current_value;
	__u32 max_value;
	__u8 max_channel;
	__s32 key_save_flg;
	__s32 save_cnt;
	__u8 auto_srch;
}auto_search;

typedef enum
{
	KEY_NONE_ACTION = 0,
	KEY_PRESS_ACTION,
}KEY_FLAG;

static auto_search *spi_auto_srch;
static KEY_FLAG key_flag = KEY_NONE_ACTION;
static __u8 draw_single_flg = 1;
static DV_APP_CONVERT destory_clear_flg = DV_VEDIO_CAMEREA_APP;

//static __u32   dv_single_level = 0;
//static __u32   dv_prevsingle_level = 0xff;
static __u32 next_signal_level, prev_signal_level;
#define SEARCH_TIMER_PERIOD		40
#define TIMER_CAM_PERIOD 10


#ifdef APP_DV_HBAR
static __s32 __app_dv_draw_rec_time_hbar(__gui_msg_t *msg, __u32 time);
static __s32 __app_dv_draw_rec_batt_status(H_LYR hlyr);
extern __u32  channel_freq[16];
#endif
static void __dv_frm_srch_begin(void);
static void __dv_frm_srch_finsh(void);
static void app_dv_search_dialog_create(dv_frmwin_para_t  *dv_frm_ctrl);

#ifdef DV_FRM_SAVE_FREQ
static void dv_frm_save_final_channel(__s32 buff[16][2])
{
	__s32 i,j;
	__s32 tmp = 0;

	for(i = 0; i < 16; i++)
	{
		for(j = i; j < 16; j++)
		{
			if(buff[i][1] < buff[j][1])
			{
				tmp = buff[i][1];
				buff[i][1] = buff[j][1];
				buff[j][1] = tmp;
			}
		}
	}

	for(i = 0; i < 16; i++)
	{
		__mymsg("buff[%d] = %d, value = %d\n", i, buff[i][0], buff[i][1]);
	}

	if(freq_save_cnt > 10)
	{
		freq_save_cnt = 10;
	}
}
static void dv_frm_save_freq(__u32 channel, __u32 value)
{
	__s32 i = 0;
	
	while(freq_save_buff[i][0])
	{
		i++;
		if(i>16)
		{
			break;
		}
	}
	freq_save_cnt = i;
	freq_save_flg = 1;
	freq_save_buff[i][0] = channel;
	freq_save_buff[i][1] = value;
	
	for(i = 0; i < 16; i++)
	{	
		if(freq_save_buff[i][0])
		{
			__mymsg("freq_chanel[%d] = %d, value = %d\n", i, freq_save_buff[i][0], freq_save_buff[i][1]);
		}
	}
}
#endif


static void  __dv_get_time_mark_index(__date_t date, __time_t time, __u8 *mark, __u8 level)
{
    mark[0]  =   date.year / 1000;
    mark[1]  =  (date.year - mark[0] * 1000) / 100;
    mark[2]  =   (date.year - mark[0] * 1000 - mark[1] * 100) / 10;
    mark[3]  =   date.year - mark[0] * 1000 - mark[1] * 100 - mark[2] * 10;
    mark[4]  =   11;
    mark[5]  =   date.month / 10;
    mark[6]  =   date.month - mark[5] * 10;
    mark[7]  =   11;
    mark[8]  =   date.day / 10;
    mark[9]  =   date.day - mark[8] * 10;
    mark[10] =   12;
    mark[11] =  time.hour / 10;
    mark[12] =   time.hour - mark[11] * 10;
    mark[13] =   10;
    mark[14] =   time.minute / 10;
    mark[15] =   time.minute - mark[14] * 10;
    mark[16] =   10;
    mark[17] =   time.second / 10;
    mark[18] =   time.second - mark[17] * 10;

    if(level == 1)
    {
        __u8 i = 0;
        for(i = 0; i <= 18; i++)
        {
            mark[i] += 20;

        }
    }
}

__s32   __dv_get_last_file(char *dir)
{

    __s32			  cnt = 0;
    char			  name[1024];
    char 		  *tmp_sfx;
    ES_DIR		  *tmp_dir;
    ES_DIRENT	  *tmp_dirent;
    __s32  file_index = 0;

    tmp_dir = eLIBs_opendir(dir);
    if(tmp_dir == NULL)
    {
    	
    	__here__;
        return 0;
    }
    eLIBs_memset(&name, 0x00, 1024);
	__here__;
    while(NULL != (tmp_dirent = eLIBs_readdir(tmp_dir)))
    {
        if(tmp_dirent->fatdirattr & FSYS_ATTR_DIRECTORY)
        {
            // 非法文件
            if(tmp_dirent->d_name[0] == '.')
            {
            	__here__;
                continue;
            }
        }
        else
        {
            tmp_sfx = eLIBs_strchrlast((char *)tmp_dirent->d_name, '.');
            if(NULL == tmp_sfx)
            {
                continue;
            }
			__here__;
			if(Cvr_DvGetWorkMode() == WORK_MODE_REC)
			{
	            if(eLIBs_stricmp(tmp_sfx, FILE_NAME_SUFFIX) == 0)
	            {
	                if(0 < eLIBs_strcmp((char *)tmp_dirent->d_name, name))
	                {
	                    cnt++;
	                    eLIBs_strcpy(name, (char *)tmp_dirent->d_name);
	                }
	            }
			}
			else if(Cvr_DvGetWorkMode() == WORK_MODE_CAM)
			{
	            if(eLIBs_stricmp(tmp_sfx, FILE_NAME_IMAGE) == 0)
	            {
	                if(0 < eLIBs_strcmp((char *)tmp_dirent->d_name, name))
	                {
	                    cnt++;
	                    eLIBs_strcpy(name, (char *)tmp_dirent->d_name);
	                }
	            }
			}
        }
    }
    eLIBs_closedir(tmp_dir);
	__here__;
    if(cnt == 0)
    {
    	__here__;
        return 0;//EPDK_FAIL;
    }
    file_index = 1000 * (name[3] - 0x30) + 100 * (name[4] - 0x30) + 10 * (name[5] - 0x30) + name[6] - 0x30;

    return file_index;

}
static __s32   __dv_get_first_file(char *dir, char *file_name, char *invalid_file)
{
    __s32           cnt = 0;
    char            name[1024];
    char           *tmp_sfx;
    ES_DIR         *tmp_dir;
    ES_DIRENT      *tmp_dirent;

    tmp_dir = eLIBs_opendir(dir);
    if(tmp_dir == NULL)
    {
        return EPDK_FAIL;
    }
    eLIBs_memset(&name, 0xFF, 1024);

    while(NULL != (tmp_dirent = eLIBs_readdir(tmp_dir)))
    {
        if(tmp_dirent->fatdirattr & FSYS_ATTR_DIRECTORY)
        {
            // 非法文件
            if(tmp_dirent->d_name[0] == '.')
            {
                continue;
            }
        }
        else
        {
            tmp_sfx = eLIBs_strchrlast((char *)tmp_dirent->d_name, '.');
            if(NULL == tmp_sfx)
            {
                continue;
            }
            if(invalid_file != NULL)
            {
                //　区别当前正在录制文件
                if(0 == eLIBs_strcmp(invalid_file, (char *)tmp_dirent->d_name))
                {
                    continue;
                }
            }
            if(eLIBs_stricmp(tmp_sfx, FILE_NAME_SUFFIX) == 0)
            {
                if(0 < eLIBs_strcmp(name, (char *)tmp_dirent->d_name))
                {
                    cnt++;
                    eLIBs_strcpy(name, (char *)tmp_dirent->d_name);
                }
            }
        }
    }
    eLIBs_closedir(tmp_dir);

    if(cnt == 0)
    {
        return EPDK_FAIL;
    }
    eLIBs_strcpy(file_name, dir);
    eLIBs_strcat(file_name, "\\");
    eLIBs_strcat(file_name, name);

    return EPDK_OK;
}

static void dv_dialog_photo_creat( dv_frmwin_para_t *dv_frm_ctrl, RECT *dialoag_rect, HTHEME bg_res, HTHEME dialog_res)
{
	GUI_RECT gui_rect ;

	if(dv_frm_ctrl->dialoag_lyr_hdl)
	{
		return;
	}

	dv_frm_ctrl->dialoag_lyr_hdl =	dv_layer_create( dialoag_rect, DISP_LAYER_WORK_MODE_NORMAL);
	GUI_LyrWinSetSta(dv_frm_ctrl->dialoag_lyr_hdl, GUI_LYRWIN_STA_ON);
	GUI_LyrWinSetTop(dv_frm_ctrl->dialoag_lyr_hdl);

	if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(dv_frm_ctrl->frmwin_hdl)) == GUI_LYRWIN_STA_SUSPEND)
	{
		esKRNL_TimeDly(1);
		if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(dv_frm_ctrl->frmwin_hdl)) == GUI_LYRWIN_STA_SUSPEND)
			return ;
	}

	GUI_LyrWinSel(dv_frm_ctrl->dialoag_lyr_hdl);

	GUI_SetColor(GUI_DARKRED);

	GUI_BMP_Draw(dsk_theme_hdl2buf(dv_frm_ctrl->uipara->msg_box_bg), 0, 0);

	gui_rect.x0 = 90 - 28;
	gui_rect.y0 = 25 ;
	gui_rect.x1 = dialoag_rect->width - 1;
	gui_rect.y1 = dialoag_rect->height - 1;
	
	GUI_BMP_Draw(dsk_theme_hdl2buf(dv_frm_ctrl->uipara->msg_box_dialog), gui_rect.x0, gui_rect.y0);
	
	GUI_SetTimer(dv_frm_ctrl->frmwin_hdl, TIMER_DIALOAG_TIMEOUT_ID, 30, NULL);
}

static void dv_dialog_search_creat( dv_frmwin_para_t *dv_frm_ctrl, RECT *dialoag_rect, HTHEME bg_res, __u32 string_id)
{
	char str[64] ;
	GUI_RECT gui_rect ;

	if(dv_frm_ctrl->dialoag_lyr_hdl)
	{
		return;
	}

	dv_frm_ctrl->dialoag_lyr_hdl =	dv_layer_create( dialoag_rect, DISP_LAYER_WORK_MODE_NORMAL);
	GUI_LyrWinSetSta(dv_frm_ctrl->dialoag_lyr_hdl, GUI_LYRWIN_STA_ON);
	GUI_LyrWinSetTop(dv_frm_ctrl->dialoag_lyr_hdl);

	if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(dv_frm_ctrl->frmwin_hdl)) == GUI_LYRWIN_STA_SUSPEND)
	{
		esKRNL_TimeDly(1);
		if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(dv_frm_ctrl->frmwin_hdl)) == GUI_LYRWIN_STA_SUSPEND)
			return ;
	}

	GUI_LyrWinSel(dv_frm_ctrl->dialoag_lyr_hdl);
	GUI_SetFont(dv_frm_ctrl->font);
	GUI_UC_SetEncodeUTF8();

	GUI_SetColor(GUI_WHITE);

	GUI_BMP_Draw(dsk_theme_hdl2buf(dv_frm_ctrl->uipara->msg_box_bg), 0, 0);

	dsk_langres_get_menu_text(string_id, str, GUI_TITLE_MAX);
	gui_rect.x0 = 0 ;
	gui_rect.y0 = 0 ;
	gui_rect.x1 = dialoag_rect->width - 1;
	gui_rect.y1 = dialoag_rect->height - 1;

	GUI_DispStringInRect(str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
	// creat timer for timer out
	//GUI_SetTimer(dv_frm_ctrl->frmwin_hdl, TIMER_DIALOAG_TIMEOUT_ID, 50, NULL);


}

static void dv_dialog_creat( dv_frmwin_para_t *dv_frm_ctrl, RECT *dialoag_rect, HTHEME bg_res, __u32 string_id)
{
    char str[64] ;
    GUI_RECT gui_rect ;

    if(dv_frm_ctrl->dialoag_lyr_hdl)
    {
        return;
    }

    dv_frm_ctrl->dialoag_lyr_hdl =  dv_layer_create( dialoag_rect, DISP_LAYER_WORK_MODE_NORMAL);
    GUI_LyrWinSetSta(dv_frm_ctrl->dialoag_lyr_hdl, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(dv_frm_ctrl->dialoag_lyr_hdl);

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(dv_frm_ctrl->frmwin_hdl)) == GUI_LYRWIN_STA_SUSPEND)
    {
        esKRNL_TimeDly(1);
        if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(dv_frm_ctrl->frmwin_hdl)) == GUI_LYRWIN_STA_SUSPEND)
            return ;
    }

    GUI_LyrWinSel(dv_frm_ctrl->dialoag_lyr_hdl);
    GUI_SetFont(dv_frm_ctrl->font);
    GUI_UC_SetEncodeUTF8();

    GUI_SetColor(GUI_WHITE);

    GUI_BMP_Draw(dsk_theme_hdl2buf(dv_frm_ctrl->uipara->msg_box_bg), 0, 0);

    dsk_langres_get_menu_text(string_id, str, GUI_TITLE_MAX);
    gui_rect.x0 = 0 ;
    gui_rect.y0 = 0 ;
    gui_rect.x1 = dialoag_rect->width - 1;
    gui_rect.y1 = dialoag_rect->height - 1;

    GUI_DispStringInRect(str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    // creat timer for timer out
    GUI_SetTimer(dv_frm_ctrl->frmwin_hdl, TIMER_DIALOAG_TIMEOUT_ID, 50, NULL);
}


static void dv_dialog_destroy( dv_frmwin_para_t *dv_frm_ctrl)
{

    GUI_KillTimer(dv_frm_ctrl->frmwin_hdl, TIMER_DIALOAG_TIMEOUT_ID);
    if(dv_frm_ctrl->dialoag_lyr_hdl)
    {
        dv_layer_destroy( dv_frm_ctrl->dialoag_lyr_hdl );
        dv_frm_ctrl->dialoag_lyr_hdl = NULL ;
    }

}


static void  __dv_init(dv_frmwin_para_t *dv_frm_ctrl)
{
    __dv_config_t       tmp_config;
    __lotus_overlay_src_init_t  tmp_src;

    //__msg("__dv_init\n");

    eLIBs_memset(&tmp_config, 0 , sizeof(tmp_config) );
    Cvr_DvOpen();
    dv_frm_ctrl->cur_state = DV_ON_REC ;

    if(dv_frm_ctrl->cur_state == DV_ON_REC)
    {
        tmp_config.work_mode    = WORK_MODE_REC;
        tmp_config.rec_quality  = RECORD_QUALITY_1280_720 ;
    }
    else if(dv_frm_ctrl->cur_state == DV_ON_CAM)
    {
        tmp_config.work_mode    = WORK_MODE_CAM;
		tmp_config.cam_quality = CAMERA_QUALITY_200;
    }

    Cvr_DvConfigPara(&tmp_config);
    // 配置时间水印的资源

    //From Kok
    cvr_water_mark_get_overlay_data(&tmp_src, 0);

    //From Kok
    dv_frm_ctrl->user_data->circle_rec_time_interval = 0;//3 * 60; // 关闭循环录像
    dv_frm_ctrl->user_data->record_mute_enable = 0;
    dv_frm_ctrl->user_data->water_mark_enable = 0;//0;//1;


    dv_frm_ctrl->sys_date.year = 2015;
    dv_frm_ctrl->sys_date.month = 1;
    dv_frm_ctrl->sys_date.day = 1;

    Cvr_DvSetOverlaySrc(&tmp_src, 0);
    // 设置普通录像文件名称
    Cvr_DvChangeNamePrefix(FILE_NAME_PRIX);

}

static void  _dv_exit(dv_frmwin_para_t *dv_ctrl)
{
    __msg("_dv_exit\n");
    Cvr_DvClose();
}

#if (CVR_TVOUT_ENABLE == 1)
static void __dv_tv_plugin(dv_frmwin_para_t *dv_frm_ctrl)
{
    RECT        rect;
    ES_FILE    *display;

    dsk_display_off();
    dsk_display_set_format(LION_DISP_TV_NTSC);

    display = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if(display == NULL)
    {
        __wrn("can not open display driver\n");
    }
    switch(dv_frm_ctrl->cur_state)
    {
    case DV_ON_REC:
    case DV_ON_CAM:
    case DV_ON_REC_SET:
    case DV_ON_CAM_SET:
    {
        rect.x      = 30;
        rect.y      = 20;
        rect.width  = eLIBs_fioctrl(display, DISP_CMD_SCN_GET_WIDTH, 0, NULL) - 60;
        rect.height = eLIBs_fioctrl(display, DISP_CMD_SCN_GET_HEIGHT, 0, NULL) - 40;
        Cvr_DvSetShowRect(rect, 0);
        Cvr_DvSetShowRect(rect, 1);
        break;
    }
    default:
        break;
    }

    // need to do  handle UI when turn from lcd->tv

    dsk_display_on(DISP_OUTPUT_TYPE_TV);
    eLIBs_fclose(display);
}

static void __dv_tv_plugout(dv_frmwin_para_t *dv_frm_ctrl)
{
    RECT        rect;
    ES_FILE    *display;

    dsk_display_off();
    dsk_display_set_format(LION_DISP_LCD);
    display = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if(display == NULL)
    {
        __wrn("can not open display driver\n");
    }
    switch(dv_frm_ctrl->cur_state)
    {
    case DV_ON_REC:
    case DV_ON_CAM:
    case DV_ON_REC_SET:
    case DV_ON_CAM_SET:
    {
        rect.x      = 0;
        rect.y      = 0;
        rect.width  = eLIBs_fioctrl(display, DISP_CMD_SCN_GET_WIDTH, 0, NULL);
        rect.height = eLIBs_fioctrl(display, DISP_CMD_SCN_GET_HEIGHT, 0, NULL);
        Cvr_DvSetShowRect(rect, 0);
        Cvr_DvSetShowRect(rect, 1);
        break;
    }
    default:
        break;
    }

    dsk_display_on(DISP_OUTPUT_TYPE_LCD);
    eLIBs_fclose(display);
}
#endif

#if (CVR_HDMI_ENABLE == 1)
static void __dv_hdmi_plugin(dv_frmwin_para_t *dv_frm_ctrl)
{
    __u32       tmp_len;
    RECT        rect;
    ES_FILE    *display;

    dsk_display_off();
    dsk_display_set_format(LION_DISP_HDMI_720P_50HZ);

    display = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if(display == NULL)
    {
        __wrn("can not open display driver\n");
    }
    switch(dv_frm_ctrl->cur_state)
    {
    case DV_ON_REC:
    case DV_ON_CAM:
    case DV_ON_REC_SET:
    case DV_ON_CAM_SET:
    {
        rect.x      = 30;
        rect.y      = 20;
        rect.width  = eLIBs_fioctrl(display, DISP_CMD_SCN_GET_WIDTH, 0, NULL) - 60;
        rect.height = eLIBs_fioctrl(display, DISP_CMD_SCN_GET_HEIGHT, 0, NULL) - 40;
        Cvr_DvSetShowRect(rect, 0);
        Cvr_DvSetShowRect(rect, 1);
        break;
    }
    default:
        break;
    }

    // need to do  handle UI when turn from tv->lcd

    dsk_display_on(DISP_OUTPUT_TYPE_HDMI);
    eLIBs_fclose(display);
}

static void __dv_hdmi_plugout(dv_frmwin_para_t *dv_frm_ctrl)
{
    RECT        rect;
    ES_FILE    *display;

    dsk_display_off();
    dsk_display_set_format(LION_DISP_LCD);

    display = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if(display == NULL)
    {
        __wrn("can not open display driver\n");
    }
    switch(dv_frm_ctrl->cur_state)
    {
    case DV_ON_REC:
    case DV_ON_CAM:
    case DV_ON_REC_SET:
    case DV_ON_CAM_SET:
    {
        rect.x      = 0;
        rect.y      = 0;
        rect.width  = eLIBs_fioctrl(display, DISP_CMD_SCN_GET_WIDTH, 0, NULL);
        rect.height = eLIBs_fioctrl(display, DISP_CMD_SCN_GET_HEIGHT, 0, NULL);
        Cvr_DvSetShowRect(rect, 0);
        Cvr_DvSetShowRect(rect, 1);
        break;
    }
    default:
        break;
    }

    // need to do  handle UI when turn from tv->lcd

    dsk_display_on(DISP_OUTPUT_TYPE_LCD);
    eLIBs_fclose(display);
}
#endif

static __s32 __dv_timer_record_proc(void *p_arg)
{
    dv_frmwin_para_t        	*dv_frm_ctrl;
    dv_frm_ctrl = (dv_frmwin_para_t *)p_arg;


    //From Kok
    //Because there is no rtc,please add it after rtc existed.
    //esTIME_GetTime(&dv_frm_ctrl->sys_time);
    //esTIME_GetDate(&dv_frm_ctrl->sys_date);
    dv_frm_ctrl->sys_date.year  = 2015;
    dv_frm_ctrl->sys_date.day   = 1;
    dv_frm_ctrl->sys_date.day   = 1;

    dv_frm_ctrl->sys_time.hour   = 10;
    dv_frm_ctrl->sys_time.minute = 10;
    dv_frm_ctrl->sys_time.second = 10;


    if(Cvr_DvGetRecState() != RECORD_STOP)
    {
        // 更新时间水印
        if(dv_frm_ctrl->user_data->water_mark_enable == 1)
        {
            dv_frm_ctrl->water_mark_para.total = 19;
            dv_frm_ctrl->water_mark_para.pos.x = 40;
            dv_frm_ctrl->water_mark_para.pos.y = 80;


            __dv_get_time_mark_index(dv_frm_ctrl->sys_date, dv_frm_ctrl->sys_time, dv_frm_ctrl->water_mark_para.IDlist, 0);
            Cvr_DvShowOverlay(&dv_frm_ctrl->water_mark_para, 0);
        }
        else
        {
            Cvr_DvShowOverlay(NULL, 0);
        }

        if(dv_frm_ctrl->recording_flash_flag)
        {
            __gui_msg_t new_msg ;
            new_msg.id			= GUI_MSG_PAINT,
                           new_msg.h_srcwin	= dv_frm_ctrl->frmwin_hdl;
            new_msg.h_deswin	= dv_frm_ctrl->frmwin_hdl;
            new_msg.dwReserved	= DV_PAINT_FLASH_ICON1;
            new_msg.dwAddData2	= 0;
            new_msg.p_arg		= NULL;

            GUI_SendAsyncMessage (&new_msg);
            dv_frm_ctrl->recording_flash_flag = 0 ;
        }
        else
        {
            __gui_msg_t new_msg ;
            new_msg.id			= GUI_MSG_PAINT,
                           new_msg.h_srcwin	= dv_frm_ctrl->frmwin_hdl;
            new_msg.h_deswin	= dv_frm_ctrl->frmwin_hdl;
            new_msg.dwReserved	= DV_PAINT_FLASH_ICON2;
            new_msg.dwAddData2	= 0;
            new_msg.p_arg		= NULL;

            GUI_SendAsyncMessage (&new_msg);
            dv_frm_ctrl->recording_flash_flag = 1 ;

        }

    }

    return EPDK_OK;
}

static void __dv_delete_first_file_thread(void *p_arg)
{
    __s32               ret;
    __s64               tmp_size;
    char                cur_file[1024];
    char                file_path[1024];
    dv_frmwin_para_t        	*dv_frm_ctrl;
    dv_frm_ctrl = (dv_frmwin_para_t *)p_arg;

    while (1)
    {
        if(esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            esKRNL_TDel(EXEC_prioself);
            break;
        }
        //　正在录像，且当前录像模式为循环覆盖（user_data->circle_rec_time_interval = 0 时录像不覆盖）
        if((dv_frm_ctrl->rec_status != 0) && (dv_frm_ctrl->user_data->circle_rec_time_interval > 0))
        {
            tmp_size = eLIBs_GetVolFSpace(USER_DISK) - DISK_FULL_LEVEL;
            if(tmp_size <= (long long)0)    // 磁盘满
            {
                // 获取当前正在录制文件名
                Cvr_DvGetFileName(cur_file, 0);
			#ifdef TWO_FOLDER 
				ret = __dv_get_first_file(FILE_VIDEO_PATH, file_path, cur_file);
			#else
                ret = __dv_get_first_file(FILE_PATH, file_path, cur_file);
			#endif
                if(ret == EPDK_OK)
                {
                    // 删除文件失败
                    if(EPDK_FAIL == eLIBs_remove(file_path))
                    {
                        __log("remove file fail : %s, ret = %d \n", file_path, ret);
                    }
                }
            }
            esKRNL_TimeDly(100);
        }
        else
        {
            esKRNL_TimeDly(100);
        }
    }
}

static __s32 __dv_on_start_record( __gui_msg_t *msg)
{
    __s64               tmp_size;
    char                file_path[1024];
    dv_frmwin_para_t        	*dv_frm_ctrl;

	__mymsg("__dv_on_start_record\n");
    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);
    // 循环录像打开
    if(dv_frm_ctrl->user_data->circle_rec_time_interval > 0)
    {
        tmp_size = eLIBs_GetVolFSpace(USER_DISK) - DISK_FULL_LEVEL;
        if(tmp_size <= (long long)0)    // 磁盘满
        {
            // 获取最早录制的文件（非当前录制文件）
        #ifdef TWO_FOLDER
			__dv_get_first_file(FILE_VIDEO_PATH, file_path, NULL);
		#else
            __dv_get_first_file(FILE_PATH, file_path, NULL);
		#endif
            eLIBs_remove(file_path);
        }
        // 开始录像
        Cvr_DvStartRecord();
        dv_frm_ctrl->rec_status = 1;

    }
    else	// 未打开循环录像
    {
        // 磁盘满检测
        tmp_size = eLIBs_GetVolFSpace(USER_DISK) - DISK_FULL_LEVEL;
        if(tmp_size <= (long long)0)
        {
            // 磁盘满对话框,need to do			
			RECT rect ;
            __mymsg("dsk_full\n");
			rect.x = ( dv_frm_ctrl->uipara->scn_w - dv_frm_ctrl->uipara->msg_box_size.width ) >> 1 ;
			rect.y = ( dv_frm_ctrl->uipara->scn_h - dv_frm_ctrl->uipara->msg_box_size.height ) >> 1 ;
			rect.width = dv_frm_ctrl->uipara->msg_box_size.width ;
			rect.height = dv_frm_ctrl->uipara->msg_box_size.height ;
			dv_dialog_creat(dv_frm_ctrl, &rect, dv_frm_ctrl->uipara->msg_box_bg , dv_frm_ctrl->uipara->disk_full_id);
        }
        else
        {
        	__mymsg("Cvr_DvStartRecord\n");
            // 开始录像
            Cvr_DvStartRecord();
            dv_frm_ctrl->rec_status = 1;
        }
    }
    return EPDK_OK;

}

static __s32 __dv_on_stop_record(__gui_msg_t *msg)
{
    dv_frmwin_para_t        	*dv_frm_ctrl;

    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);
    // 清除时间水印
    if(dv_frm_ctrl->user_data->water_mark_enable == 1)
    {
        dv_frm_ctrl->water_mark_para.total = 0;
        dv_frm_ctrl->water_mark_para.pos.x = 40;
        dv_frm_ctrl->water_mark_para.pos.y = 80;
        Cvr_DvShowOverlay(&dv_frm_ctrl->water_mark_para, 0);
    }
    dv_frm_ctrl->rec_status = 0;
    Cvr_DvStopRecord();
    eLIBs_memset((void *)(&dv_frm_ctrl->cur_rec_time), 0, sizeof((dv_frm_ctrl->cur_rec_time)));

    // need to do : draw recording time to zero

    return EPDK_OK;
}

static __s32 __dv_on_recording(__gui_msg_t *msg)
{
    __s64               tmp_size;
    dv_frmwin_para_t        	*dv_frm_ctrl;

    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);
    // 循环录像打开
    if(dv_frm_ctrl->user_data->circle_rec_time_interval > 0)
    {

        // 循环录制
        if(Cvr_DvGetCurRecTime() >= dv_frm_ctrl->user_data->circle_rec_time_interval)
        {
            dv_frm_ctrl->rec_status = 0;
            Cvr_DvHaltRecord();
            eLIBs_memset((void *)(&dv_frm_ctrl->cur_rec_time), 0, sizeof((dv_frm_ctrl->cur_rec_time)));
            // 开始录像
            Cvr_DvStartRecord();
            dv_frm_ctrl->rec_status = 1;

        }
    }
    // 未打开循环录像，则采用10分钟循环录像，磁盘满即停止录像。
    else
    {
        tmp_size = eLIBs_GetVolFSpace(USER_DISK) - DISK_FULL_LEVEL;
        if(tmp_size <= (long long)0)    // 磁盘满
        {
            dv_frm_ctrl->rec_status = 0;
            Cvr_DvStopRecord();
            eLIBs_memset((void *)(&dv_frm_ctrl->cur_rec_time), 0, sizeof((dv_frm_ctrl->cur_rec_time)));

            // 更新场景显示图标
            // need to do :  disk full dialoag ;
        }
        else
        {
#ifdef  LOOP_VIDEO_ONOFF
			 Cvr_DvStartRecord();
             dv_frm_ctrl->rec_status = 1;
#else
			if(Cvr_DvGetCurRecTime() >= 10 * 60)  // 循环录制
            {
                dv_frm_ctrl->rec_status = 0;
                Cvr_DvHaltRecord();
                eLIBs_memset((void *)(&dv_frm_ctrl->cur_rec_time), 0, sizeof((dv_frm_ctrl->cur_rec_time)));
                // 开始录像
                Cvr_DvStartRecord();
                dv_frm_ctrl->rec_status = 1;
            }
#endif           
        }
    }
    return EPDK_OK;

}



static __s32 __dv_frm_on_create(__gui_msg_t *msg)
{
    dv_frmwin_para_t        	*dv_frm_ctrl;
	__s32 i = 0, j = 0;
    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);

    __msg("__dv_frm_on_create\n");
    __mymsg("dv_frm_ctrl->switch_frm = %d\n", dv_frm_ctrl->switch_frm);

    // 初始化水印图标资源
    cvr_water_mark_res_init();

    dv_frm_ctrl->cur_state    = DV_ON_REC;
    dv_frm_ctrl->rec_status   = 0;
    dv_frm_ctrl->frmwin_hdl = msg->h_deswin ;
    dv_frm_ctrl->user_data = dsk_reg_get_para_by_app(REG_APP_DV);

    // 打开DV模块
    __dv_init(dv_frm_ctrl);
	
    // 创建时间显示TIMER
    GUI_SetTimer(msg->h_deswin, TIMER_STATUS_ID, TIMER_STATUS_TIMEROUT_ID, NULL);
	
    GUI_SetTimer(msg->h_deswin, TIMER_DV_SRCH_ID, SEARCH_TIMER_PERIOD, NULL);
		
    dv_frm_ctrl->os_timer_hdl = esKRNL_TmrCreate(500, OS_TMR_OPT_PRIO_HIGH | OS_TMR_OPT_PERIODIC, (OS_TMR_CALLBACK)__dv_timer_record_proc, (void *)dv_frm_ctrl);
    if(dv_frm_ctrl->os_timer_hdl)
    {
        esKRNL_TmrStart(dv_frm_ctrl->os_timer_hdl);
    }
    dv_frm_ctrl->delete_file_tsk = esKRNL_TCreate(__dv_delete_first_file_thread, dv_frm_ctrl, 0x2000, KRNL_priolevel3);
	destory_clear_flg = DV_VEDIO_CAMEREA_APP;
#ifdef DV_FRM_SAVE_FREQ
#ifdef APP_DV_SUPOORT_BREAK
	if(dv_frm_ctrl->switch_frm == DV_SRCH_APP)
	{
		for(i = 0; i<2; i++)
		{
			for(j = 0; j<32; j++)
			{	
				freq_save_buff[j][i] = 0;
			}
		}
		
		spi_auto_srch->channel = 0;
		spi_auto_srch->max_value = 0;
		spi_auto_srch->max_channel = 0;
		spi_auto_srch->save_cnt = 0;
		freq_save_flg = 0;
		freq_save_switch = freq_save_cnt = 0;
		key_flag = KEY_PRESS_ACTION;
		app_dv_search_dialog_create(dv_frm_ctrl);
	}
	else
	{
		key_flag = KEY_NONE_ACTION;
	}
#else
	for(i = 0; i<2; i++)
	{
		for(j = 0; j<32; j++)
		{	
			freq_save_buff[j][i] = 0;
		}
	}
	key_flag = KEY_NONE_ACTION;
#endif	
		
	next_signal_level = prev_signal_level = 0;
#endif
	
    return EPDK_OK;
}

static void dv_save_value(void)
{
	reg_dv_para_t * dv_para;
	__msg("dv_save_value, spi_auto_srch->max_channel = %d\n", spi_auto_srch->max_channel);
	dv_para = (reg_dv_para_t *)dsk_reg_get_para_by_app(REG_APP_DV);

	dv_para->channel = spi_auto_srch->max_channel;
}

static __s32 __dv_frm_on_destroy(__gui_msg_t *msg)
{
    dv_frmwin_para_t        	*dv_frm_ctrl;
    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);

    __msg("__dv_frm_on_destroy\n");	
	
	
    // 判断是否处于录像中，若是则停止．
    if(dv_frm_ctrl->rec_status != 0)
    {
        dv_frm_ctrl->rec_status = 0;
        // 停止录制
        Cvr_DvStopRecord();
    }
    if(dv_frm_ctrl->delete_file_tsk)
    {
        while(esKRNL_TDelReq(dv_frm_ctrl->delete_file_tsk) != OS_TASK_NOT_EXIST)
        {
            esKRNL_TimeDly(1);
        }
        dv_frm_ctrl->delete_file_tsk = NULL;
    }
    // 删除线程
    if(dv_frm_ctrl->os_timer_hdl != NULL)
    {
        esKRNL_TmrStop(dv_frm_ctrl->os_timer_hdl, OS_TMR_OPT_PRIO_HIGH | OS_TMR_OPT_PERIODIC, dv_frm_ctrl);
        esKRNL_TmrDel(dv_frm_ctrl->os_timer_hdl);
        dv_frm_ctrl->os_timer_hdl = NULL;
    }
#ifdef	APP_DV_HBAR	
	if(dv_frm_ctrl->subset)
	{
		GUI_LyrWinDelete(dv_frm_ctrl->subset);
		dv_frm_ctrl->subset = NULL;
	}
#endif

    GUI_KillTimer(msg->h_deswin, TIMER_STATUS_ID);
    if( GUI_IsTimerInstalled(msg->h_deswin, TIMER_DIALOAG_TIMEOUT_ID) )
    {
        GUI_KillTimer(msg->h_deswin, TIMER_DIALOAG_TIMEOUT_ID);
    }

    if( GUI_IsTimerInstalled(msg->h_deswin, TIMER_DV_SRCH_ID) )
	{
  		GUI_KillTimer(msg->h_deswin, TIMER_DV_SRCH_ID);
	}
		
	if( GUI_IsTimerInstalled(msg->h_deswin, TIMER_DV_SRCH_OVER_ID) )
	{
  		GUI_KillTimer(msg->h_deswin, TIMER_DV_SRCH_OVER_ID);
	}
	
#ifdef	APP_DV_HBAR	
	if(dv_frm_ctrl->subset)
	{
		GUI_LyrWinDelete(dv_frm_ctrl->subset);
		dv_frm_ctrl->subset = NULL;
	}
#endif
    _dv_exit(dv_frm_ctrl);

    cvr_water_mark_res_uninit();
    esMEMS_Mfree(0, dv_frm_ctrl);
	
    return EPDK_OK;
}

static __s32 __dv_frm_on_paint(__gui_msg_t *msg)
{
    dv_frmwin_para_t        	*dv_frm_ctrl;
    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    GUI_LyrWinSel(dv_frm_ctrl->frm_lyr_hdl);
    GUI_SetFont(dv_frm_ctrl->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetColor(TEXT_COLOR);

    if( DV_CLEAR_MENUBAR_BG == msg->dwReserved )
    {
        GUI_ClearRect(dv_frm_ctrl->uipara->menu_bar_bg_rect.x, dv_frm_ctrl->uipara->menu_bar_bg_rect.y, \
                      dv_frm_ctrl->uipara->menu_bar_bg_rect.x + dv_frm_ctrl->uipara->menu_bar_bg_rect.width - 1 , \
                      dv_frm_ctrl->uipara->menu_bar_bg_rect.y + dv_frm_ctrl->uipara->menu_bar_bg_rect.height - 1 );
    }
    else if(DV_PAINT_SINGLE0 == msg->dwReserved)
    {
        GUI_BMP_Draw(dsk_theme_hdl2buf(dv_frm_ctrl->uipara->single0_handle), dv_frm_ctrl->uipara->work_mode_icon_pos.x, dv_frm_ctrl->uipara->work_mode_icon_pos.y);
    }
    else if(DV_PAINT_SINGLE1 == msg->dwReserved)
    {
        GUI_BMP_Draw(dsk_theme_hdl2buf(dv_frm_ctrl->uipara->single1_handle), dv_frm_ctrl->uipara->work_mode_icon_pos.x, dv_frm_ctrl->uipara->work_mode_icon_pos.y);
    }
    else if(DV_PAINT_SINGLE2 == msg->dwReserved)
    {
        GUI_BMP_Draw(dsk_theme_hdl2buf(dv_frm_ctrl->uipara->single2_handle), dv_frm_ctrl->uipara->work_mode_icon_pos.x, dv_frm_ctrl->uipara->work_mode_icon_pos.y);
    }
    else if(DV_PAINT_SINGLE3 == msg->dwReserved)
    {

        GUI_BMP_Draw(dsk_theme_hdl2buf(dv_frm_ctrl->uipara->single3_handle), dv_frm_ctrl->uipara->work_mode_icon_pos.x, dv_frm_ctrl->uipara->work_mode_icon_pos.y);
    }
    else if(DV_PAINT_SINGLE4 == msg->dwReserved)
    {
        GUI_BMP_Draw(dsk_theme_hdl2buf(dv_frm_ctrl->uipara->single4_handle), dv_frm_ctrl->uipara->work_mode_icon_pos.x, dv_frm_ctrl->uipara->work_mode_icon_pos.y);
    }
    return EPDK_OK;
}

static __u32 dv_set_srch_signal_value(__u32 channel)
{
	ES_FILE *ktemp;
	__u32 channel_temp = channel;
	__u32 ret = 0;
	__dv_frm_srch_begin();
	spi_auto_srch->max_channel = channel;
	ktemp = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "w");
	if(!ktemp)
	{
	    __msg("open_key_fail\n");
		return EPDK_OK;
	}
	eLIBs_fioctrl(ktemp, DRV_KEY_CMD_AUTO_SEARCH_SPI, NULL, &channel_temp);
	esKRNL_TimeDly(35);
	eLIBs_fioctrl(ktemp, DRV_KEY_CMD_SET_FIRST_DEBOUNCE_TIME, NULL, &ret);
	eLIBs_fclose( ktemp );
	__dv_frm_srch_finsh();
	return ret;
}


static __s32 __dv_frm_on_key_proc(__gui_msg_t *msg)
{
    dv_frmwin_para_t        	*dv_frm_ctrl;
    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);
	
#ifdef APP_DV_SUPOORT_BREAK
	if(dv_frm_ctrl->switch_frm == DV_SRCH_APP)
	{
		__mymsg("dv_frm_ctrl->switch_frm = %d\n", dv_frm_ctrl->switch_frm);
		return EPDK_OK;
	}
#endif	

	if(((msg->dwAddData1 == GUI_MSG_KEY_LONGLEFT)||(msg->dwAddData1 == GUI_MSG_KEY_LONGRIGHT))\
		/*&&(msg->dwAddData2 == KEY_REPEAT_ACTION)*/)
	{
		__s32 i,j;
		if(RECORD_STOP != Cvr_DvGetRecState())		//录像过程中禁止自动搜台
	    {
	    	return EPDK_OK;
		}

		if(msg->dwAddData2 == KEY_REPEAT_ACTION)
		{
			__mymsg("Show dialog of Search\n");
			app_dv_search_dialog_create(dv_frm_ctrl);
		}
		else if(msg->dwAddData2 == KEY_UP_ACTION)
		{
			__mymsg("Search of KEY_UP_ACTION\n");
#ifdef DV_FRM_SAVE_FREQ
			for(i = 0; i<2; i++)
			{
				for(j = 0; j<32; j++)
				{	
					freq_save_buff[j][i] = 0;
				}
			}
#endif
			
			spi_auto_srch->channel = 0;
			spi_auto_srch->max_value = 0;
			spi_auto_srch->max_channel = 0;
			spi_auto_srch->save_cnt = 0;
			key_flag = KEY_PRESS_ACTION;
			destory_clear_flg = DV_SRCH_APP;
			
		}
		return EPDK_OK;
	}
	
	if(msg->dwAddData1 == GUI_MSG_KEY_MENU)       // 弹出设置菜单
    {
        if(msg->dwAddData2 == KEY_UP_ACTION)
        {
        	if(Cvr_DvGetWorkMode() == DV_ON_REC )
        	{
                if(RECORD_STOP != Cvr_DvGetRecState())
                {    
					__dv_on_stop_record(msg);
					esKRNL_TimeDly(5);
                }
        	}
			Dv_cmd2parent(msg->h_deswin, ID_OP_DV_TO_HOME, 0, 0);
        }

		return EPDK_OK;
    }
	if((msg->dwAddData1 == GUI_MSG_KEY_RIGHT/*GUI_MSG_KEY_LEFT*/)&&(msg->dwAddData2 == KEY_UP_ACTION))
	{
		ES_FILE *ktemp;
		single_t key_value;
		
		__dv_frm_srch_begin();
		ktemp = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "w");
		if(!ktemp)
    	{
			return EPDK_OK;
    	}
		if(RECORD_STOP != Cvr_DvGetRecState())		//录像过程中禁止搜台
	    {
	    	return EPDK_OK;
		}
		//spi_auto_srch->key_save_flg = 1;
		spi_auto_srch->save_cnt = 0;
#ifdef	 DV_FRM_SAVE_FREQ
		if(freq_save_flg)
		{
			
			if(freq_save_switch <= 0)
			{
				freq_save_switch = freq_save_cnt;
			}
			else
			{
				freq_save_switch -= 1;
			}
			
			spi_auto_srch->max_channel = freq_save_buff[freq_save_switch][0]-1;
			__mymsg("spi_auto_srch->max_channel = %d, freq_save_switch = %d\n", spi_auto_srch->max_channel, freq_save_switch);
		}
		else
#endif
		{
			spi_auto_srch->max_channel += 1;
		}
		if((spi_auto_srch->max_channel > 15)||(spi_auto_srch->max_channel < 0))
		{
			spi_auto_srch->max_channel = 0;
		}
		eLIBs_fioctrl(ktemp, DRV_KEY_CMD_AUTO_SEARCH_SPI, NULL, &spi_auto_srch->max_channel);
		esKRNL_TimeDly(35);
		eLIBs_fioctrl(ktemp, DRV_KEY_CMD_SET_FIRST_DEBOUNCE_TIME, NULL, &key_value);
		eLIBs_fclose( ktemp );
		
#ifdef	APP_DV_HBAR
		__app_dv_draw_signal_level(dv_frm_ctrl->subset, key_value.single_value);
		__app_dv_draw_freq_hbar(dv_frm_ctrl->subset, channel_freq[spi_auto_srch->max_channel]);
#endif
		spi_auto_srch->key_save_flg = 1;
		__dv_frm_srch_finsh();
		dv_save_value();
		return EPDK_OK;
	}
	else if((msg->dwAddData1 == GUI_MSG_KEY_LEFT/*GUI_MSG_KEY_RIGHT*/)&&(msg->dwAddData2 == KEY_UP_ACTION))
	{
		ES_FILE *ktemp;
		single_t key_value;
		
		__dv_frm_srch_begin();
		ktemp = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "w");
		if(!ktemp)
    	{
			return EPDK_OK;
    	}
		if(RECORD_STOP != Cvr_DvGetRecState())
	    {
	    	return EPDK_OK;
		}
		//spi_auto_srch->key_save_flg = 1;
		spi_auto_srch->save_cnt = 0;
#ifdef	 DV_FRM_SAVE_FREQ
		if(freq_save_flg)
		{
			
			if(freq_save_switch < freq_save_cnt)
			{
				freq_save_switch += 1;
			}
			else
			{
				freq_save_switch = 0;
			}
			
			spi_auto_srch->max_channel = freq_save_buff[freq_save_switch][0]-1;
			__mymsg("spi_auto_srch->max_channel = %d, freq_save_switch = %d\n", spi_auto_srch->max_channel, freq_save_switch);
		}
		else
#endif
		{
			spi_auto_srch->max_channel -= 1;
		}
		if((spi_auto_srch->max_channel > 15)||(spi_auto_srch->max_channel < 0))
		{
			spi_auto_srch->max_channel = 15;
		}
		eLIBs_fioctrl(ktemp, DRV_KEY_CMD_AUTO_SEARCH_SPI, NULL, &spi_auto_srch->max_channel);
		esKRNL_TimeDly(35);
		eLIBs_fioctrl(ktemp, DRV_KEY_CMD_SET_FIRST_DEBOUNCE_TIME, NULL, &key_value);
		eLIBs_fclose( ktemp );
#ifdef	APP_DV_HBAR
		__app_dv_draw_signal_level(dv_frm_ctrl->subset, key_value.single_value);
		__app_dv_draw_freq_hbar(dv_frm_ctrl->subset, channel_freq[spi_auto_srch->max_channel]);
#endif
		spi_auto_srch->key_save_flg = 1;
		__dv_frm_srch_finsh();
		dv_save_value();
		return EPDK_OK;
	}
	
	if(msg->dwAddData1 == GUI_MSG_KEY_SEETING)        // 切换模式(进入拍照模式)
    {    
        if(msg->dwAddData2 == KEY_UP_ACTION)
        {
        	if(dv_frm_ctrl->cur_state == DV_ON_REC )
        	{
	            if(RECORD_STOP != Cvr_DvGetRecState())
	            {
	                //__msg("Switch to photograph \n");
	                // 切换至拍照状态
	                __dv_on_stop_record(msg);
					esKRNL_TimeDly(5);
					
	            }
				
				Cvr_DvSetWorkMode(WORK_MODE_CAM);
				dv_frm_ctrl->cur_state = DV_ON_CAM ;
				//	UI 部分更新到从录像模式切换到拍照状态
				__dv_frm_on_paint( msg);
				
        	}
			else if(dv_frm_ctrl->cur_state == DV_ON_CAM )
			{
                // 切换至录像预览状态
                Cvr_DvSetWorkMode(WORK_MODE_REC);
                dv_frm_ctrl->cur_state = DV_ON_REC;
                // UI 部分更新到从拍照状态切换到录像模式
                __dv_frm_on_paint( msg);
			}
#ifdef TWO_FOLDER
			record_file_index=__dv_get_last_file(FILE_VIDEO_PATH);
			photo_file_index=__dv_get_last_file(FILE_PHOTO_PATH);
#else
			record_file_index=__dv_get_last_file(FILE_PATH);
			photo_file_index=__dv_get_last_file(FILE_PATH);
#endif			
        }
#ifdef	APP_DV_HBAR
		__app_dv_draw_rec_time_hbar(msg, 0);
		__app_dv_draw_mode_hbar(msg);
#endif
		spi_auto_srch->key_save_flg = 1;
		spi_auto_srch->save_cnt = 0;

		return EPDK_OK;
    }
	
    if(dv_frm_ctrl->cur_state == DV_ON_REC)
    {
#ifdef	APP_DV_HBAR
		__app_dv_draw_mode_hbar(msg);
#endif
        if(msg->dwAddData1 == GUI_MSG_KEY_ENTER)        // 录像的开始、停止
        {	
            if(msg->dwAddData2 == KEY_UP_ACTION)
            {
                ES_DIR			*p_file;

                // 磁盘空间检测
                p_file = eLIBs_opendir(USER_DISK);
                if(!p_file)
                {
                    RECT rect ;

                    rect.x = ( dv_frm_ctrl->uipara->scn_w - dv_frm_ctrl->uipara->msg_box_size.width ) >> 1 ;
                    rect.y = ( dv_frm_ctrl->uipara->scn_h - dv_frm_ctrl->uipara->msg_box_size.height ) >> 1 ;
                    rect.width  = dv_frm_ctrl->uipara->msg_box_size.width ;
                    rect.height = dv_frm_ctrl->uipara->msg_box_size.height ;

                    dv_dialog_creat(dv_frm_ctrl, &rect, dv_frm_ctrl->uipara->msg_box_bg , dv_frm_ctrl->uipara->no_sd_id);
                    return EPDK_OK;
                }
                // 开始录像
                if(RECORD_STOP == Cvr_DvGetRecState())
                {
                    __mymsg("To start recording \n");
                    __dv_on_start_record(msg);
#ifdef APP_DV_HBAR				
					__app_dv_draw_rec_status(msg);
#endif
					
                }
                // 停止录像
                else if(RECORD_START == Cvr_DvGetRecState())
                {
                    __msg("To stop recording \n");
                    __dv_on_stop_record(msg);
#ifdef APP_DV_HBAR				
					__app_dv_draw_rec_status(msg);
					__app_dv_draw_rec_time_hbar(msg, 0);
#endif
                    __dv_frm_on_paint(msg);
                }
            }
        }
        return EPDK_OK;
    }
    else if(dv_frm_ctrl->cur_state == DV_ON_CAM)
    {    
#ifdef	APP_DV_HBAR
		__app_dv_draw_mode_hbar(msg);
#endif    
        if(msg->dwAddData1 == GUI_MSG_KEY_ENTER)        // 开始拍照
        {
            if(msg->dwAddData2 == KEY_UP_ACTION)
            {
                ES_DIR         *p_file;
                __u64			tmp_size ;

                // 磁盘空间检测
                p_file = eLIBs_opendir(USER_DISK);
                if(!p_file)
                {
                    RECT rect ;
                    //  弹出没有tf 卡对话框
                    rect.x = ( dv_frm_ctrl->uipara->scn_w - dv_frm_ctrl->uipara->msg_box_size.width ) >> 1 ;
                    rect.y = ( dv_frm_ctrl->uipara->scn_h - dv_frm_ctrl->uipara->msg_box_size.height ) >> 1 ;
                    rect.width = dv_frm_ctrl->uipara->msg_box_size.width ;
                    rect.height = dv_frm_ctrl->uipara->msg_box_size.height ;
                    dv_dialog_creat(dv_frm_ctrl, &rect, dv_frm_ctrl->uipara->msg_box_bg , dv_frm_ctrl->uipara->no_sd_id);

                    return EPDK_OK;
                }
                // 磁盘空间已满，无法进行拍照
                tmp_size = eLIBs_GetVolFSpace(USER_DISK) - DISK_FULL_LEVEL;
                if(tmp_size <= (long long)0)    // 停止当前录制，提示磁盘空间满
                {
                    // need to do :弹出磁盘满对话框
                    RECT rect ;
                    rect.x = ( dv_frm_ctrl->uipara->scn_w - dv_frm_ctrl->uipara->msg_box_size.width ) >> 1 ;
                    rect.y = ( dv_frm_ctrl->uipara->scn_h - dv_frm_ctrl->uipara->msg_box_size.height ) >> 1 ;
                    rect.width = dv_frm_ctrl->uipara->msg_box_size.width ;
                    rect.height = dv_frm_ctrl->uipara->msg_box_size.height ;
                    dv_dialog_creat(dv_frm_ctrl, &rect, dv_frm_ctrl->uipara->msg_box_bg , dv_frm_ctrl->uipara->disk_full_id);
                    return EPDK_OK;
                }

                // 设置时间水印
                if(dv_frm_ctrl->user_data->water_mark_enable == 1)
                {
                    dv_frm_ctrl->water_mark_para.total = 19;
                    dv_frm_ctrl->water_mark_para.pos.x = 40;
                    dv_frm_ctrl->water_mark_para.pos.y = 80;
                    __dv_get_time_mark_index(dv_frm_ctrl->sys_date, dv_frm_ctrl->sys_time, dv_frm_ctrl->water_mark_para.IDlist, 0);
                    Cvr_DvShowOverlay(&dv_frm_ctrl->water_mark_para, 0);
                }

                if( DISP_OUTPUT_TYPE_LCD == dsk_display_get_output_type())
                {
                    // 开始拍照
					RECT rect ;
					
					Cvr_DvTakeImage(1);
#ifdef	APP_DV_HBAR
					//__app_dv_draw_cam_play_pause(msg, 0);
					
					// need to do :弹出拍照对话框
					rect.x = ( dv_frm_ctrl->uipara->scn_w - dv_frm_ctrl->uipara->msg_box_size.width ) >> 1 ;
					rect.y = ( dv_frm_ctrl->uipara->scn_h - dv_frm_ctrl->uipara->msg_box_size.height ) >> 1 ;
					rect.width = dv_frm_ctrl->uipara->msg_box_size.width ;
					rect.height = dv_frm_ctrl->uipara->msg_box_size.height ;
					
					dv_dialog_photo_creat(dv_frm_ctrl, &rect, dv_frm_ctrl->uipara->msg_box_bg , dv_frm_ctrl->uipara->msg_box_dialog);
#endif					
                }
                else
                {
                    __wrn("only can take picture in lcd output mode ");
                }
                // 清除时间水印
                if(dv_frm_ctrl->user_data->water_mark_enable == 1)
                {
                    dv_frm_ctrl->water_mark_para.total = 0;
                    dv_frm_ctrl->water_mark_para.pos.x = 40;
                    dv_frm_ctrl->water_mark_para.pos.y = 80;

                    Cvr_DvShowOverlay(&dv_frm_ctrl->water_mark_para, 0);
                }
            }
        }
        return EPDK_OK;
    }

    return EPDK_OK;
}

extern __u32  channel_freq[16];
static void __dv_frm_srch_begin(void)
{
	ES_FILE *ktemp;
	ktemp = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "w");
	if(!ktemp)
	{
		return;
	}
	spi_auto_srch->auto_srch = 1;
	eLIBs_fioctrl(ktemp, DRV_KEY_CMD_AUTO_SEARCH_SPI_BEGIN, NULL, NULL);
	eLIBs_fclose( ktemp );
}


static void __dv_frm_srch_finsh(void)
{
	ES_FILE *ktemp;
	ktemp = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "w");
	if(!ktemp)
	{
		return;
	}
	spi_auto_srch->auto_srch = 0;
	eLIBs_fioctrl(ktemp, DRV_KEY_CMD_AUTO_SEARCH_SPI_FINSH, NULL, NULL);
	eLIBs_fclose( ktemp );
}

static void dly_nop(void)
{
	__s32 i = 0;
	for(i = 0; i<20000; i++);
}

static void app_dv_search_dialog_create(dv_frmwin_para_t  *dv_frm_ctrl)
{
	
	RECT rect ;
	__mymsg("app_dv_search_dialog_create\n");
	rect.x = ( dv_frm_ctrl->uipara->scn_w - dv_frm_ctrl->uipara->msg_box_size.width ) >> 1 ;
	rect.y = ( dv_frm_ctrl->uipara->scn_h - dv_frm_ctrl->uipara->msg_box_size.height ) >> 1 ;
	rect.width = dv_frm_ctrl->uipara->msg_box_size.width ;
	rect.height = dv_frm_ctrl->uipara->msg_box_size.height ;

	dv_dialog_search_creat(dv_frm_ctrl, &rect, dv_frm_ctrl->uipara->msg_box_bg , dv_frm_ctrl->uipara->search_id);
}
static __u8 clear_redcnt=0;
static __s32 __dv_frm_on_timer_proc(__gui_msg_t *msg)
{
    dv_frmwin_para_t        	*dv_frm_ctrl;
    static __u8 flag = 0;

    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);



/////////////////////////////////////////////////////////////////////////////

	if(draw_single_flg)
	{
		if(clear_redcnt)
		{
		  	clear_redcnt--;
			if(!clear_redcnt)
			{
			 	__dv_frm_srch_finsh();
			}
		}
	}


/////////////////////////////////////////////////////////////////////////////
    if(msg->dwAddData1 == TIMER_STATUS_ID)    // 录像时，更新当前已录制时间;回放时，更新当前播放时间和进度
    {
        // 录像状态检测
        if(dv_frm_ctrl->rec_status != 0)
        {
            __dv_on_recording(msg);
#ifdef	APP_DV_HBAR			
			__app_dv_draw_rec_time_hbar(msg, Cvr_DvGetCurRecTime()*1000);
#endif
        }
		
		if((spi_auto_srch->save_cnt++ > 10)&&(spi_auto_srch->key_save_flg))
		{	
			if(spi_auto_srch->auto_srch)
			{
				return EPDK_OK;
			}
			dsk_reg_flush();
			spi_auto_srch->save_cnt = 0;
			spi_auto_srch->key_save_flg = 0;
		}

		if(draw_single_flg)
		{
			__s32 i;
			ES_FILE *ktemp;
			single_t key_value;
			
			ktemp = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "w");
			if(!ktemp)
	    	{
				return EPDK_OK;
	    	}
			__dv_frm_srch_begin();
			clear_redcnt=15;
			for(i = 0; i<5; i++)
			{
				dly_nop();			
			}
			eLIBs_fioctrl(ktemp, DRV_KEY_CMD_SET_FIRST_DEBOUNCE_TIME, NULL, &key_value);
			__mymsg("key_value.single_value = %d\n", key_value.single_value);
#ifdef	APP_DV_HBAR
			__app_dv_draw_signal_level(dv_frm_ctrl->subset, key_value.single_value);
#endif
			//dly_nop();
			//dly_nop();
			//dly_nop();
			//__dv_frm_srch_finsh();
			eLIBs_fclose(ktemp);
		}

    }
    else if(msg->dwAddData1 == TIMER_DIALOAG_TIMEOUT_ID)
    {
        dv_dialog_destroy(dv_frm_ctrl);
    }
	else if((msg->dwAddData1 == TIMER_DV_SRCH_ID)&&(key_flag == KEY_PRESS_ACTION))
	{
		ES_FILE *ktemp, *ktemp1;
		single_t key_value;

		if(KEY_PRESS_ACTION == key_flag)
		{
			draw_single_flg = 0;
			__dv_frm_srch_begin();
			if(spi_auto_srch->channel)
			{
				//__dv_frm_srch_begin();
				ktemp1 = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "w");
				if(!ktemp1)
		    	{
					return EPDK_OK;
		    	}
				eLIBs_fioctrl(ktemp1, DRV_KEY_CMD_SET_FIRST_DEBOUNCE_TIME, NULL, &key_value);
//#ifdef	APP_DV_HBAR
//				__app_dv_draw_signal_level(dv_frm_ctrl->subset, key_value.single_value);
//#endif
				__mymsg("spi_auto_srch->channel = %d,key_value.single_value = %d\n",spi_auto_srch->channel - 1, key_value.single_value);
#ifdef DV_FRM_SAVE_FREQ
				if(key_value.single_value >= MAX_FREQ_SAVE_VALUE)
				{
					dv_frm_save_freq(spi_auto_srch->channel, key_value.single_value);
				}
#endif

				if(spi_auto_srch->max_value < key_value.single_value)
				{
					spi_auto_srch->max_value = key_value.single_value; 
					spi_auto_srch->max_channel = spi_auto_srch->channel - 1;
				}
				eLIBs_fclose( ktemp1 );
			}
			ktemp = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "w");
			if(!ktemp)
	    	{
				return EPDK_OK;
	    	}
			eLIBs_fioctrl(ktemp, DRV_KEY_CMD_AUTO_SEARCH_SPI, NULL, &spi_auto_srch->channel);
#ifdef	APP_DV_HBAR	
			__app_dv_draw_freq_hbar(dv_frm_ctrl->subset, channel_freq[spi_auto_srch->channel]);			
#endif
			eLIBs_fclose( ktemp );
		}
//		draw_single_flg = 0;
		//if(++spi_auto_srch->channel >31)
		if(++spi_auto_srch->channel > 15)
		{
			spi_auto_srch->search_end = 1;
			spi_auto_srch->channel = 0;
			//key_flag = KEY_NONE_ACTION;
						
			if(GUI_IsTimerInstalled(msg->h_deswin, TIMER_DV_SRCH_OVER_ID))
	        {
	            GUI_ResetTimer(msg->h_deswin,TIMER_DV_SRCH_OVER_ID,SEARCH_TIMER_PERIOD,NULL);
	        }
			else
			{
				GUI_SetTimer(msg->h_deswin, TIMER_DV_SRCH_OVER_ID, SEARCH_TIMER_PERIOD, NULL);
			}
		}
					
    }
	else if((LOWORD(msg->dwAddData1) == TIMER_DV_SRCH_OVER_ID)&&(spi_auto_srch->search_end))
	{
		ES_FILE *ktemp;
		single_t key_value;
		__s32 i = 0;
		
		ktemp = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "w");
		if(!ktemp)
    	{
			return EPDK_OK;
    	}
		eLIBs_fioctrl(ktemp, DRV_KEY_CMD_SET_FIRST_DEBOUNCE_TIME, NULL, &key_value);
		
#ifdef DV_FRM_SAVE_FREQ
		if(key_value.single_value >= MAX_FREQ_SAVE_VALUE)
		{
			dv_frm_save_freq(16, key_value.single_value);
		}
#endif
		
		if(spi_auto_srch->max_value < key_value.single_value)
		{
			spi_auto_srch->max_value = key_value.single_value; 
			spi_auto_srch->max_channel = 15;
		}
		__mymsg("spi_auto_srch->max_value = %d,spi_auto_srch->max_channel = %d\n",spi_auto_srch->max_value, spi_auto_srch->max_channel);

#ifdef	APP_DV_HBAR
		__app_dv_draw_signal_level(dv_frm_ctrl->subset, spi_auto_srch->max_value);
#endif
	
		for(i = 0; i<25; i++)
		{
			dly_nop();
		}
		eLIBs_fioctrl(ktemp, DRV_KEY_CMD_AUTO_SEARCH_SPI, NULL, &spi_auto_srch->max_channel);
		dly_nop();
		dly_nop();
		
#ifdef	APP_DV_HBAR	
		__app_dv_draw_freq_hbar(dv_frm_ctrl->subset, channel_freq[spi_auto_srch->max_channel]);			
#endif
		dv_save_value();
		spi_auto_srch->search_end = 0;
		eLIBs_fclose( ktemp );
		spi_auto_srch->key_save_flg = 1;
		key_flag = KEY_NONE_ACTION;
		__dv_frm_srch_finsh();
		draw_single_flg = 1;
		dv_dialog_destroy(dv_frm_ctrl);
#ifdef APP_DV_SUPOORT_BREAK		
		if(dv_frm_ctrl->switch_frm == DV_SRCH_APP)
		{
			dv_frm_ctrl->switch_frm = DV_VEDIO_CAMEREA_APP;
			__mymsg("dv_frm_ctrl->switch_frm = %d\n", dv_frm_ctrl->switch_frm);
		}
#endif		
		if( GUI_IsTimerInstalled(msg->h_deswin, TIMER_DV_SRCH_OVER_ID) )
		{
			GUI_KillTimer(msg->h_deswin, TIMER_DV_SRCH_OVER_ID);
		}
	}
    return EPDK_OK;
}

static __s32 __dv_frm_set_record_quality_proc(__gui_msg_t *msg)
{
    dv_frmwin_para_t        	*dv_frm_ctrl;

    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if( dv_frm_ctrl->cur_state == DV_ON_REC_SET)
    {
        switch(msg->dwAddData1)
        {
        case 0:
            dv_frm_ctrl->user_data->rec_quality = 0;
            Cvr_DvSetRecQuality(RECORD_QUALITY_640_480);
            break ;
        case 1:
            dv_frm_ctrl->user_data->rec_quality = 1;
            Cvr_DvSetRecQuality(RECORD_QUALITY_1280_720);
            break ;
        }
    }
    else if( dv_frm_ctrl->cur_state == DV_ON_CAM_SET)
    {
        switch(msg->dwAddData1)
        {
        case 0:
            dv_frm_ctrl->user_data->cam_quality = 0;
            Cvr_DvSetRecQuality(CAMERA_QUALITY_100);
            break ;
        case 1:
            dv_frm_ctrl->user_data->cam_quality = 1;
            Cvr_DvSetRecQuality(CAMERA_QUALITY_200);
            break ;
        case 2:
            dv_frm_ctrl->user_data->cam_quality = 2;
            Cvr_DvSetRecQuality(CAMERA_QUALITY_300);
            break ;
        }
    }
    // updata quality setting
    __dv_frm_on_paint(msg);
    return EPDK_OK;
}

#ifdef	APP_DV_HBAR

#ifdef DV_FREQ_UPPER_ARRANGEMENT
__u32  channel_freq[32]=
{
	5645,
	5665,
	5685,
	5705,
	5725,
	5733,
	5740,
	5745,
	5752,
	5760,
	5765,
	5771,
	5780,
	5785,
	5790,
	5800,
	5805,
	5809,
	5820,
	5825,
	5828,
	5840,
	5845,
	5847,
	5860,
	5865,
	5866,
	5880,
	5885,
	5905,
	5925,
	5945,
};

#elif 0
__u32  channel_freq[32]=
{
	5945,		// 1	5945
	5925,		// 2	5925
	5905,		// 3 	5905
	5885,		// 4	5885
	5865,		// 5	5865

	5845,		// 6 	5845
	5825,		// 7	5825
	5805,		// 8	5805
	5785,		// 9	5785
	5765,		// 10	5765

	5745,		// 11	5745
	5725,		// 12	5725
	5705,		// 13	5705
	5685,		// 14	5685
	5665,		// 15	5665

	5645,		// 16  	5645
	5625,		// 17	5625
	5605,		// 18	5605
	5585,		// 19 	5585
	5565,		// 20	5565

	5545,		// 21	5545
	5525,		// 22	5525
	5505,		// 23 	5505
	5485,		// 24	5485
	5465,		// 25 	5465

	5445,		// 26	5445
	5425,		// 27	5425
	5405,		// 28 	5405
	5385,		// 29	5385
	5365,		// 30	5365
	5345,		// 31	5345
	5325,		// 32	5325
};
/*
{
   //0x0000,
   //chanl    A
   5740,				   //  1			   5740
   5760,				   //  2			   5760
   5780,				   //  3			   5780
   5800,				   //  4			   5800
   5820,				   //  5			   5820
   5840,				   //  6			   5840
   5860,				   //  7			   5860
   5880,				   //  8			   5880

   //chanl   B
   5733,				   //   9			   5733
   5752,				   //  10			   5752
   5771,				   //  11			   5771
   5790,				   //  12			   5790
   5809,				   //  13			   5809
   5828,				   //  14			   5828
   5847,				   //  15			   5847
   5866,				   //  16			   5866
   
   //chanl	 C
   5705,				   //  17			   5705
   5685,				   //  18			   5685 
   5665,				   //  19			   5665
   5645,				   //  20			   5645
   5885,				   //  21			   5885
   5905,				   //  22			   5905
   5925,				   //  23			   5925
   5945,				   //  24			   5945

   //chanl	 D
   5865,				   // 25				5865
   5845,				   // 26			    5845
   5825,				   // 27			    5825
   5805,				   // 28			    5805
   5785,				   // 29			    5785
   5765,				   // 30			    5765
   5745,				   // 31			    5745
   5725,				   // 32 				5725

};
*/
#else
__u32  channel_freq[16]=
{
    5657,
   	5676,
    5695,
    5714,
    5733,
    5752,
    5771,
    5790,
    5809,
    5828,
    5847,
    5866,
    5885,
    5904,
    5923,
    5942
    
};
#endif

static H_LYR dv_32bpp_layer_create(RECT *rect, __s32 pipe)
{
	H_LYR layer = NULL;
	FB	fb =
	{
		{0, 0}, 										/* size 	 */
		{0, 0, 0},										/* buffer	 */
		{FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},	 /* fmt 	  */
	};
	__disp_layer_para_t lstlyr =
	{
		DISP_LAYER_WORK_MODE_NORMAL,					/* mode 	 */
		0,												/* ck_mode	 */
		0,												/* alpha_en  */
		0,												/* alpha_val */
		1,												/* pipe 	 */
		0xff,											/* prio 	 */
		{0, 0, 0, 0},									/* screen	 */
		{0, 0, 0, 0},									/* source	 */
		DISP_LAYER_OUTPUT_CHN_DE_CH1,					/* channel	 */
		NULL											/* fb		 */
	};
	__layerwincreate_para_t lyrcreate_info =
	{
		"subset menu layer",
		NULL,
		GUI_LYRWIN_STA_SUSPEND,
		GUI_LYRWIN_NORMAL
	};
	fb.size.width		= rect->width;
	fb.size.height		= rect->height;
	lstlyr.src_win.x		= 0;
	lstlyr.src_win.y		= 0;
	lstlyr.src_win.width	= rect->width;
	lstlyr.src_win.height	= rect->height;
	lstlyr.scn_win.x		= rect->x;
	lstlyr.scn_win.y		= rect->y;
	lstlyr.scn_win.width	= rect->width;
	lstlyr.scn_win.height	= rect->height;
	lstlyr.pipe = pipe;
	lstlyr.fb = &fb;
	lyrcreate_info.lyrpara = &lstlyr;
	layer = GUI_LyrWinCreate(&lyrcreate_info);
	if( !layer )
	{
		__err("GUI_LyrWinCreate fail !\n");
	}
	return layer;
}

static __s32 power_level_2_display(power_level_e level)
{
    int ret = 0;

    switch(level)
    {
    case DSK_POWER_LEVEL_0:
        ret = 0;
        break;
    case DSK_POWER_LEVEL_1:
        ret = 1;
        break;
    case DSK_POWER_LEVEL_2:
        ret = 2;
        break;
    case DSK_POWER_LEVEL_3:
        ret = 3;
        break;
    case DSK_POWER_LEVEL_4:
        ret = 4;
        break;

    case DSK_POWER_LEVEL_5:
        ret = 5;
        break;

    default:
        ret = 0;
        break;
    }

    return ret;
}

static __u32 dv_signal_level_handle(__u32 level)
{
	__u32	ret = 0;
	switch(level/100)
	{
		case DV_LEVEL_NOSIGNAL:
			ret = DV_LEVEL_NOSIGNAL;
			break;
		case DV_LEVEL_SIGNAL_1:
			ret = DV_LEVEL_SIGNAL_1;
			break;
		case DV_LEVEL_SIGNAL_2:
			ret = DV_LEVEL_SIGNAL_2;
			break;
		case DV_LEVEL_SIGNAL_3:
			ret = DV_LEVEL_SIGNAL_3;
			break;
		case DV_LEVEL_SIGNAL_4:
			ret = DV_LEVEL_SIGNAL_4;
			break;
		case DV_LEVEL_SIGNAL_5:
		default:
			ret = DV_LEVEL_SIGNAL_5;
			break;
	}
	return ret - 2;
}

static __s32 check_disk(void)
{
    __s32 i;
    __s32 ret;
    __s32 cnt;
	__s32 card_status;
    char diskname[RAT_MAX_PARTITION][4];

    __mymsg("check_disk\n");
    cnt = 0;
    card_status = RAT_UNKNOWN;

    ret = rat_get_partition_name(RAT_USB_DISK, diskname);
    if(EPDK_OK == ret)
    {
        i = 0;
        //while (i < RAT_MAX_PARTITION)
        {
            if (diskname[i][0])
            {
                __msg("SD : diskname[%d]=%s\n", i, diskname[i]);
                card_status += RAT_USB;
                cnt++;
            }
            i++;
        }
    }

    ret = rat_get_partition_name(RAT_SD_CARD, diskname);
    if(EPDK_OK == ret)
    {
        i = 0;
        //while (i < RAT_MAX_PARTITION)
        {
            if (diskname[i][0])
            {
                __msg("SD : diskname[%d]=%s\n", i, diskname[i]);
                card_status += RAT_TF << 8;
                cnt++;
            }
            i++;
        }
    }
    if(card_status & 0xff00)
    {	
        return 0;
    }
    else
    {
        return -1;	
    }
}

static __s32 __app_dv_draw_card_status(H_LYR hlyr)
{
	GUI_RECT gui_rect;
	__s32 ret;
	dv_sub_res dv_subse_ui;

	if(NULL == hlyr)
	{
		__err("invalid para\n");
		return EPDK_FAIL;
	}

	ret = check_disk();
	
	dv_subse_ui = dv_get_sub_res();
	
	gui_rect.x0 = 375;
    gui_rect.y0 = 0;
    gui_rect.x1 = 414;
    gui_rect.y1 = 20;
	
	GUI_LyrWinSel(hlyr);
	GUI_SetBkColor(0);
	GUI_ClearRectEx(&gui_rect);
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);

	if(ret == 0)
	{
		GUI_BMP_RES_Draw(dv_subse_ui->sd_card_status[1], gui_rect.x0, gui_rect.y0+5);
	}
	else
	{
		GUI_BMP_RES_Draw(dv_subse_ui->sd_card_status[0], gui_rect.x0, gui_rect.y0+5);
	}

	return EPDK_OK;
}

static __s32 __app_dv_draw_signal_level(H_LYR hlyr, __u32 level)
{
	GUI_RECT gui_rect;
	dv_sub_res dv_subse_ui;

	if(NULL == hlyr)
	{
		__err("invalid para\n");
		return EPDK_FAIL;
	}
	
    dv_subse_ui = dv_get_sub_res();
	/*
	gui_rect.x0 = 400;
	gui_rect.y0 = 0;
	gui_rect.x1 = 460;
	gui_rect.y1 = 20;
	*/

	gui_rect.x0 = 415;
    gui_rect.y0 = 0;
    gui_rect.x1 = 440;
    gui_rect.y1 = 20;

	next_signal_level = dv_signal_level_handle(level);

	if(next_signal_level == prev_signal_level)
	{
		return EPDK_OK;
	}
	GUI_LyrWinSel(hlyr);
	GUI_SetBkColor(0);
	GUI_ClearRectEx(&gui_rect);
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	/*
	if((next_signal_level == 1) || (next_signal_level == 2))
	{
		next_signal_level = 2;
	}
	*/
	GUI_BMP_RES_Draw(dv_subse_ui->bmp_subset_singal[next_signal_level], gui_rect.x0, gui_rect.y0+5);
	
	prev_signal_level = next_signal_level;
	return EPDK_OK;
}


static __s32 __app_dv_draw_rec_batt_status(H_LYR hlyr)
{
	power_level_e level;
	GUI_RECT gui_rect;
	dv_headbar_data_t dv_batt_ctrl;
	dv_sub_res  dv_subse_ui;

	if(NULL == hlyr)
	{
		__err("invalid para\n");
		return EPDK_FAIL;
	}
	
    dv_subse_ui = dv_get_sub_res();

	gui_rect.x0 = 445;
	gui_rect.y0 = 0;
	gui_rect.x1 = 480;
	gui_rect.y1 = 20;
	
	dsk_power_get_voltage_level(&level);
	dv_batt_ctrl.vol_level = power_level_2_display(level);  	//电量级别
	dv_batt_ctrl.charge_sta = dsk_get_charge_state();

	eLIBs_printf("dv_batt_ctrl.vol_level:%d\n", dv_batt_ctrl.vol_level);
	GUI_LyrWinSel(hlyr);
	GUI_SetBkColor(0);
	GUI_ClearRectEx(&gui_rect);
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	
	GUI_BMP_RES_Draw(dv_subse_ui->bmp_subset_vol[dv_batt_ctrl.vol_level], gui_rect.x0, gui_rect.y0+8);

	
	return EPDK_OK;
}
/*
static __s32 __app_dv_draw_cam_play_pause(__gui_msg_t *msg, __s32 flag)
{
	static __s32 old_flg, new_flg;
	GUI_RECT gui_rect;
	dv_frmwin_para_t        	*dv_frm_ctrl;
	dv_sub_res  dv_subse_ui;
    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);

	new_flg = flag;
	if(NULL == dv_frm_ctrl->subset)
	{
		__err("invalid para\n");
		return EPDK_FAIL;
	}
	
	if(Cvr_DvGetWorkMode() == WORK_MODE_REC)
	{
		__msg("WORK_MODE_CAM\n");
		return EPDK_OK;
	}
	if(new_flg == old_flg)
	{
		return EPDK_OK;
	}
	dv_subse_ui = dv_get_sub_res();
	
	gui_rect.x0 = 220;
    gui_rect.y0 = 0;
    gui_rect.x1 = 300;
    gui_rect.y1 = 50;

	GUI_LyrWinSel(dv_frm_ctrl->subset);
	GUI_SetBkColor(0);
	GUI_ClearRectEx(&gui_rect);
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);

	if(Cvr_DvGetWorkMode() == WORK_MODE_CAM)
	{
		old_flg = new_flg;
		GUI_BMP_RES_Draw(dv_subse_ui->cam_play_pause[flag], gui_rect.x0+20, gui_rect.y0+5);
	}
	else
	{
		return EPDK_OK;
		//GUI_DispStringInRect(time_text, &gui_rect, GUI_TA_VCENTER|GUI_TA_HCENTER);
	}
}
*/
static __s32 __app_dv_draw_rec_time_hbar(__gui_msg_t *msg, __u32 time)
{
	char time_text[32];
	GUI_RECT gui_rect;
	dv_frmwin_para_t        	*dv_frm_ctrl;
    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);
	
	if(NULL == dv_frm_ctrl->subset)
	{
		__err("invalid para\n");
		return EPDK_FAIL;
	}
	
	gui_rect.x0 = 280;
    gui_rect.y0 = 3;
    gui_rect.x1 = 375;
    gui_rect.y1 = 23;
	
	eLIBs_memset(time_text, 0 ,sizeof(time_text));	
	
	time2str_by_format_hour_show(time, time_text, TIME_AUTO_HMS);
	GUI_LyrWinSel(dv_frm_ctrl->subset);
	GUI_SetBkColor(0);
	GUI_ClearRectEx(&gui_rect);
	GUI_SetFont(SWFFont);
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	GUI_SetColor(GUI_WHITE);
	GUI_UC_SetEncodeUTF8();
	if(Cvr_DvGetWorkMode() == WORK_MODE_CAM)
	{
		__msg("WORK_MODE_CAM\n");
		return EPDK_OK;
		//GUI_DispStringInRect(NULL, &gui_rect, GUI_TA_VCENTER|GUI_TA_HCENTER);
	}
	else
	{
		GUI_DispStringInRect(time_text, &gui_rect, GUI_TA_VCENTER|GUI_TA_HCENTER);
	}

	return EPDK_OK;
}


static __s32 __app_dv_draw_freq_hbar(H_LYR hlyr, __u32 channel)
{
	char text[6] = {0};
	GUI_RECT gui_rect;
	
	//__msg("__app_dv_draw_hbar\n");
	if(NULL == hlyr)
	{
		__err("invalid para\n");
		return EPDK_FAIL;
	}

	gui_rect.x0 = 0;
    gui_rect.y0 = 0;
    gui_rect.x1 = 120;
    gui_rect.y1 = 30;
	
	//dsk_langres_get_menu_text(STRING_REC, text, sizeof(text));

	text[0] = channel/1000 + '0';
	channel %= 1000;
	text[1] = '.';
	text[2] = channel /100 + '0';
	channel %= 100;
	text[3] = channel/10 + '0';
	channel %= 10;
	text[4] = channel + '0';
	text[5] = 'G';

	//__msg("text: %s\n", text);
	GUI_LyrWinSel(hlyr);
	GUI_SetBkColor(0);
	GUI_ClearRectEx(&gui_rect);
	GUI_SetFont(SWFFont);
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	GUI_SetColor(GUI_WHITE);
	GUI_UC_SetEncodeUTF8();
	GUI_DispStringInRect(text, &gui_rect, GUI_TA_VCENTER|GUI_TA_HCENTER);

	return EPDK_OK;
}
static __s32 __app_dv_draw_rec_status(__gui_msg_t *msg)
{
	dv_sub_res	dv_subse_ui;
	GUI_RECT gui_rect;
	dv_frmwin_para_t 		   *dv_frm_ctrl;
	dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);

	if(NULL == dv_frm_ctrl->subset)
	{
	   __err("invalid para\n");
	   return EPDK_FAIL;
	}

	dv_subse_ui = dv_get_sub_res();

	gui_rect.x0 = 255;
    gui_rect.y0 = 3;
    gui_rect.x1 = 280;
    gui_rect.y1 = 23;

	GUI_LyrWinSel(dv_frm_ctrl->subset);
	GUI_SetBkColor(0);
	GUI_ClearRectEx(&gui_rect);
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	if(dv_frm_ctrl->rec_status != 0)
	{
		GUI_BMP_RES_Draw(dv_subse_ui->rec_status_bmp, gui_rect.x0+5, gui_rect.y0 + 6);
	}
	
	return EPDK_OK;
}


static __s32 __app_dv_draw_mode_hbar(__gui_msg_t *msg)
{
    //char text[128];
    dv_sub_res  dv_subse_ui;
	GUI_RECT gui_rect;
	dv_frmwin_para_t        	*dv_frm_ctrl;
    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);
	
    if(NULL == dv_frm_ctrl->subset)
    {
        __err("invalid para\n");
        return EPDK_FAIL;
    }
	
	dv_subse_ui = dv_get_sub_res();
	
	gui_rect.x0 = 226;
    gui_rect.y0 = 3;
    gui_rect.x1 = 255;
    gui_rect.y1 = 30;

	GUI_LyrWinSel(dv_frm_ctrl->subset);
	GUI_SetBkColor(0);
	GUI_ClearRectEx(&gui_rect);
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	
	if(dv_frm_ctrl->cur_state == DV_ON_REC)
	{
		GUI_BMP_RES_Draw(dv_subse_ui->rec_cam_bmp[0], gui_rect.x0, gui_rect.y0 + 5);
	}
	else if(dv_frm_ctrl->cur_state == DV_ON_CAM)
	{
		GUI_BMP_RES_Draw(dv_subse_ui->rec_cam_bmp[1], gui_rect.x0, gui_rect.y0 + 3);
	}
	
    return EPDK_OK;
}

static __s32 __app_dv_draw_hbar_on_plugout(__gui_msg_t *msg)
{
	GUI_RECT gui_rect;

	dv_frmwin_para_t        	*dv_frm_ctrl;
    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);
	
	if(NULL == dv_frm_ctrl->subset)
	{
		__err("invalid para\n");
		return EPDK_FAIL;
	}
	gui_rect.x0 = 130;
	gui_rect.y0 = 0;
	gui_rect.x1 = 300;
	gui_rect.y1 = 60;
	GUI_ClearRectEx(&gui_rect);
	__app_dv_draw_mode_hbar(msg);
	if(dv_frm_ctrl->cur_state == DV_ON_REC)
	{
		__app_dv_draw_rec_time_hbar(msg, 0);
	}
	__app_dv_draw_card_status(dv_frm_ctrl->subset);

	return EPDK_OK;
}


void app_dv_subscene_create(__gui_msg_t *msg)
{
	__s32 width;
    __s32 height;
	RECT rect;
	__u32 channel;
	__u32 temp;
	reg_dv_para_t * dv_para;
	
	dv_frmwin_para_t        	*dv_frm_ctrl;
    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);
	
	dsk_display_get_size(&width, &height);

	rect.x = 0;
    rect.y = 0;
    rect.width = width;
    rect.height = 60;

	dv_frm_ctrl->subset = dv_32bpp_layer_create(&rect, 1);
	if(NULL == dv_frm_ctrl->subset)
    {
    	__err("create dv_32bpp_layer failed\n");
        return; //EPDK_FAIL;
    }

	if(dsk_reg_get_app_restore_flag(REG_APP_DV))
    {
        __msg("load default dv...\n");
        dv_para = (reg_dv_para_t *)dsk_reg_get_default_para_by_app(REG_APP_DV);
    }
    else
    {
        __msg("load current dv...\n");
        dv_para = (reg_dv_para_t *)dsk_reg_get_para_by_app(REG_APP_DV);
    }
	if(dv_para)
    {
        channel = channel_freq[dv_para->channel];
        __msg("dv_para->channel = %d\n", channel);
    }
    else
    {
        channel = channel_freq[0];
    }
	
	temp = dv_set_srch_signal_value(dv_para->channel);
	GUI_LyrWinSetSta(dv_frm_ctrl->subset, GUI_LYR_STA_ON);
    GUI_LyrWinSetTop(dv_frm_ctrl->subset);
	__app_dv_draw_mode_hbar(msg);
	__app_dv_draw_freq_hbar(dv_frm_ctrl->subset, channel);
	__app_dv_draw_rec_time_hbar(msg, 0);
	__app_dv_draw_card_status(dv_frm_ctrl->subset);
	__app_dv_draw_rec_batt_status(dv_frm_ctrl->subset);
	__app_dv_draw_signal_level(dv_frm_ctrl->subset, temp);
}
#endif


static __s32 __dv_frm_main_proc(__gui_msg_t *msg)
{
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        __dv_frm_on_create(msg);
#ifdef	APP_DV_HBAR
		app_dv_subscene_create(msg);
#endif
    }
    break;
    case GUI_MSG_DESTROY:
    {	
        __dv_frm_on_destroy(msg);
    }
    break;
    case GUI_MSG_PAINT:
    {
        __dv_frm_on_paint(msg);
    }
    break;

    case GUI_MSG_KEY:
    {
        __dv_frm_on_key_proc(msg);
    }
    break;
	case DSK_MSG_VOLTAGE_CHANGE:
	{
		dv_frmwin_para_t        	*dv_frm_ctrl;

	    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);
		__mymsg("DSK_MSG_VOLTAGE_CHANGE\n");
		
#ifdef	APP_DV_HBAR
		__app_dv_draw_rec_batt_status(dv_frm_ctrl->subset);
#endif
		break;
	}
    case GUI_MSG_COMMAND:
    {
//        __msg("gui_msg_command\n");
    }
    break;
    case GUI_MSG_TIMER:
    {
        __dv_frm_on_timer_proc(msg);
    }
    break;

    case DSK_MSG_TVDAC_PLUGIN:		// tvdac plug in
    {
        dv_frmwin_para_t        	*dv_frm_ctrl;
        dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);
//        __msg("TVDAC plug in \n");

#if (CVR_TVOUT_ENABLE == 1)
        __dv_tv_plugin(dv_frm_ctrl);
#endif
    }
    break;
    case DSK_MSG_TVDAC_PLUGOUT:		// tvdac plug out
    {
        dv_frmwin_para_t        	*dv_frm_ctrl;
        dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);

//        __msg("TVDAC plug out \n");
#if (CVR_TVOUT_ENABLE == 1)
        __dv_tv_plugout(dv_frm_ctrl);
#endif
    }
    break;
    case DSK_MSG_HDMI_PLUGIN:		// hdmi plug in
    {
        dv_frmwin_para_t        	*dv_frm_ctrl;
        dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);

//        __msg("HDMI plug in \n");
#if (CVR_HDMI_ENABLE == 1)
        __dv_hdmi_plugin(dv_frm_ctrl);
#endif
    }
    break;
    case DSK_MSG_HDMI_PLUGOUT:		// hdmi plug out
    {
        dv_frmwin_para_t        	*dv_frm_ctrl;
        dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);
#if (CVR_HDMI_ENABLE == 1)
        __dv_hdmi_plugout(dv_frm_ctrl);
#endif
    }
    break;
    case DSK_MSG_FS_PART_PLUGIN:	// 文件系统分区插入
	{		
		
		dv_frmwin_para_t        	*dv_frm_ctrl;

	    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);
		
		__mymsg("File system plug in \n");
#ifdef TWO_FOLDER
		record_file_index=__dv_get_last_file(FILE_VIDEO_PATH);
		photo_file_index=__dv_get_last_file(FILE_PHOTO_PATH);
#else
		record_file_index=__dv_get_last_file(FILE_PATH);
		photo_file_index=__dv_get_last_file(FILE_PATH);
#endif			
		
#ifdef	APP_DV_HBAR
		__app_dv_draw_card_status(dv_frm_ctrl->subset);
#endif
		
    	break;
	}
    case DSK_MSG_FS_PART_PLUGOUT:	// 文件系统分区拔出
    {
		
//		dv_frmwin_para_t        	*dv_frm_ctrl;

//	    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);
		
        __mymsg("File system plug out \n");
        // 停止当前录制
        if(RECORD_START == Cvr_DvGetRecState())
		{
			__dv_on_stop_record(msg);
		}
#ifdef	APP_DV_HBAR
		__app_dv_draw_hbar_on_plugout(msg);
		//__app_dv_draw_card_status(dv_frm_ctrl->subset);
#endif
		//check_disk();
    }
    break;
    case CMD_DV_FRM_SET_RECORD_RESOLUTION:
    {

        __dv_frm_set_record_quality_proc(msg);
    }
    break ;
    case CMD_DV_FRM_SETTING_MENU_EXIT:
    {
        __dv_frm_set_record_quality_proc(msg);
    }
    break ;


    default:
        break;
    }
    return GUI_FrmWinDefaultProc(msg);
}



__s32 dv_frm_create(dv_frmwin_para_t *para)
{
    RECT rect ;
    __gui_framewincreate_para_t framewin_para;
    dv_frmwin_para_t *dv_frmwin_ctrl ;

    __msg("dv_frm_create\n");

    dv_frmwin_ctrl = (dv_frmwin_para_t *)esMEMS_Malloc(0, sizeof(dv_frmwin_para_t));
    if(dv_frmwin_ctrl == NULL)
    {
        __wrn("malloc error \n");
        return EPDK_FAIL;
    }
    eLIBs_memcpy(dv_frmwin_ctrl, para, sizeof(dv_frmwin_para_t));

    dv_frmwin_ctrl->uipara = Dv_GetUipara();
    rect.x = 0 ;
    rect.y = 0 ;
    rect.width = dv_frmwin_ctrl->uipara->scn_w ;
    rect.height = dv_frmwin_ctrl->uipara->scn_h ;
//    dv_prevsingle_level = 0xff;
//    dv_single_level = 0;

    dv_frmwin_ctrl->frm_lyr_hdl = dv_layer_create(&rect, DISP_LAYER_WORK_MODE_NORMAL);
    GUI_LyrWinSetSta(dv_frmwin_ctrl->frm_lyr_hdl, GUI_LYRWIN_STA_ON);
	
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
	
    framewin_para.name =	"dv frame win",
                  framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner	= NULL;
    framewin_para.id         = DV_FRM_WIN_ID;
    framewin_para.hHosting = para->h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__dv_frm_main_proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = rect.width;
    framewin_para.rect.height = rect.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr =  (void *)dv_frmwin_ctrl;
    framewin_para.hLayer = dv_frmwin_ctrl->frm_lyr_hdl;

    return(GUI_FrmWinCreate(&framewin_para));

}

__s32 dv_frm_destroy( H_WIN h_win )
{
	__s32 i,j;
    dv_frmwin_para_t        	*dv_frm_ctrl;

    __msg("dv_frm_destroy\n");

    dv_frm_ctrl = (dv_frmwin_para_t *)GUI_WinGetAttr(h_win);
	if(destory_clear_flg == DV_SRCH_APP)
	{
		for(i = 0; i<2; i++)
		{
			for(j = 0; j<32; j++)
			{	
				freq_save_buff[j][i] = 0;
			}
		}
		freq_save_flg = 0;
		freq_save_switch = freq_save_cnt = 0;
	}
    // for frame win ,there is no need to call  GUI_FrmWinDelete to destroy frame win ,because the function of  GUI_LyrWinDelete will do that
    GUI_LyrWinDelete(dv_frm_ctrl->frm_lyr_hdl);
}



