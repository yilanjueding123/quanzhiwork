
#include "av1_uipara.h"
#include "app_av1.h"

//#include "common/common.h"
#if 1
//#define __here__            eLIBs_printf("@L%d(%s)\n", __LINE__, __FILE__);
#define __msg(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
							     eLIBs_printf(__VA_ARGS__)									        )
//#define __wrn(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
//							     eLIBs_printf(__VA_ARGS__)									        )
//#define __inf(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
//							     eLIBs_printf(__VA_ARGS__)									        )
//#define __err(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
//							     eLIBs_printf(__VA_ARGS__)									        )
#else
#define __msg(...)
#endif

#define SCREEN_WIDTH	480
#define SCREEN_HEIGHT	272

#define AV_PAINT_SINGLE0       0x10
#define AV_PAINT_SINGLE1       0x11
#define AV_PAINT_SINGLE2       0x12
#define AV_PAINT_SINGLE3       0x13
#define AV_PAINT_SINGLE4       0x14

typedef struct tag_av1_ctrl
{
    av1_para_t *av1_scene_para;
    __u8 init_osd_off_timmer_id;
	__u8 av1_spi_timmer_id;

} av1_ctrl_t;

typedef struct
{
    H_WIN hfrm;
} av1_man_ctrl_t;

typedef struct __auto_search
{
	__u8 channel;
	__u8 search_end;
	__u8 prev_value;
	__u8 current_value;
	__u32 max_value;
	__u8 max_channel;
}auto_search;

static auto_search *spi_auto_srch;


typedef enum
{
	KEY_NONE_ACTION = 0,
	KEY_PRESS_ACTION,
}KEY_FLAG;

//////////////////////////////////////////////////////////////////////////
//发送命令到主窗口
//////////////////////////////////////////////////////////////////////////

static __u32 av_single_level = 0;
static __u32 av_prevsingle_level = 0xff;
static KEY_FLAG key_flag = KEY_NONE_ACTION;

__s32 av1_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    //char buf[128];

    msg.h_deswin = GUI_WinGetParent(hwin);
    msg.h_srcwin = NULL;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;

    return (GUI_SendMessage(&msg));
}

/**********************************************************************************************************************
* 图层创建接口
**********************************************************************************************************************/
static H_WIN av1_layer_create( RECT *rect)
{
    FB	fb =
    {
        {0, 0},
        {0, 0, 0},
        {FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, 0}},
    };
    __disp_layer_para_t para =
    {
        //DISP_LAYER_WORK_MODE_PALETTE,
        DISP_LAYER_WORK_MODE_NORMAL,
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
        "av1 layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
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


static __s32 av1_single_paint(__gui_msg_t *msg)
{

    av1_ctrl_t *av1_ctrl;
    static __u8 display_nosingle = 0;
    static __u8 prev_display_nosingle = 0;
    GUI_RECT gui_rect;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;
    av1_ctrl = (av1_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    GUI_LyrWinSel(av1_ctrl->av1_scene_para->layer);
    GUI_SetFont(av1_ctrl->av1_scene_para->font);
    GUI_UC_SetEncodeUTF8();
    //GUI_SetColor(TEXT_COLOR);

    display_nosingle = 0;

    if(AV_PAINT_SINGLE0 == msg->dwReserved)
    {
        reg_system_para_t *av_para;
        av_para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
        if(av_para)
        {
            if(av_para->language == EPDK_LANGUAGE_ENM_CHINESES)
        	{
    	        GUI_BMP_Draw(dsk_theme_hdl2buf(av1_ctrl->av1_scene_para->no_single1_handle), (480 - 96) / 2, (272 - 32) / 2);
        	}
            else
        	{
      	    	GUI_BMP_Draw(dsk_theme_hdl2buf(av1_ctrl->av1_scene_para->no_single0_handle), (480 - 96) / 2, (272 - 32) / 2);
        	}
            display_nosingle = 1;
        }

        GUI_BMP_Draw(dsk_theme_hdl2buf(av1_ctrl->av1_scene_para->single0_handle), 2, 2);
    }
    else if(AV_PAINT_SINGLE1 == msg->dwReserved)
    {

        GUI_BMP_Draw(dsk_theme_hdl2buf(av1_ctrl->av1_scene_para->single1_handle), 2, 2);
    }
    else if(AV_PAINT_SINGLE2 == msg->dwReserved)
    {

        GUI_BMP_Draw(dsk_theme_hdl2buf(av1_ctrl->av1_scene_para->single2_handle), 2, 2);
    }
    else if(AV_PAINT_SINGLE3 == msg->dwReserved)
    {

        GUI_BMP_Draw(dsk_theme_hdl2buf(av1_ctrl->av1_scene_para->single3_handle), 2, 2);
    }
    else if(AV_PAINT_SINGLE4 == msg->dwReserved)
    {

        GUI_BMP_Draw(dsk_theme_hdl2buf(av1_ctrl->av1_scene_para->single4_handle), 2, 2);
    }
    if(prev_display_nosingle)
    {
        if(display_nosingle == 0)
        {
            gui_rect.x0 = (480 - 96) / 2 ;
            gui_rect.y0 = (272 - 32) / 2;
            gui_rect.x1 = gui_rect.x0 + 96;
            gui_rect.y1 = gui_rect.y0 + 32;
            GUI_ClearRectEx(&gui_rect);
        }
    }
    prev_display_nosingle = display_nosingle;
    return EPDK_OK;

}


static __s32 _dv_single_paint(__gui_msg_t *msg)
{
    //	__gui_msg_t * msg;

    if(av_single_level == KPAD_SINGLE0)
	{
	    msg->dwReserved = AV_PAINT_SINGLE0 ;
	}
    else if(av_single_level == KPAD_SINGLE1)
    {
        msg->dwReserved = AV_PAINT_SINGLE1 ;
	}
    else if(av_single_level == KPAD_SINGLE2)
	{
	    msg->dwReserved = AV_PAINT_SINGLE2 ;
	}
    else if(av_single_level == KPAD_SINGLE3)
	{
	    msg->dwReserved = AV_PAINT_SINGLE3 ;
	}
    else
	{
	    msg->dwReserved = AV_PAINT_SINGLE4 ;
	}
    // __dv_frm_on_paint( msg);
    av1_single_paint(msg);
    return EPDK_OK;
}

static __s32 av1_loading_paint(H_WIN h_win, __s32 index)
{
    av1_ctrl_t *av1_ctrl;
    av1_uipara_t *av1_uipara;
    GUI_RECT gui_rect;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(h_win)) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    if(index < 0 || index > 1)
    {
        __wrn("invalid para...\n");
        return EPDK_FAIL;
    }

    av1_ctrl = (av1_ctrl_t *)GUI_WinGetAddData(h_win);
    av1_uipara = (av1_uipara_t *)app_av1_get_uipara();

    GUI_LyrWinSel(av1_ctrl->av1_scene_para->layer);
    GUI_SetFont(av1_ctrl->av1_scene_para->font);
    GUI_UC_SetEncodeUTF8();


    return EPDK_OK;
}

__u32 saturation_av1 = 128;
static __s32  av1_win_key_proc(__gui_msg_t *msg)
{
    static __s32 last_key = -1;
    __s32  bok, ret;

    bok = -1;

    //  eLIBs_printf("msg->dwAddData1 =%x;msg->dwAddData2 = %x\n",msg->dwAddData1,msg->dwAddData2);

    if(((KEY_DOWN_ACTION == msg->dwAddData2) || (KEY_REPEAT_ACTION == msg->dwAddData2)) && (GUI_MSG_KEY_LONGENTER == msg->dwAddData1))
    {
        av1_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0 ); //直接退出
    }
    else if( //(KEY_UP_ACTION == msg->dwAddData2)&&
        (GUI_MSG_KEY_ESCAPE == msg->dwAddData1)
        )
    {
        av1_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0 ); //直接退出
    }
    else if(KEY_UP_ACTION == msg->dwAddData2)
    {
        if(GUI_MSG_KEY_MENU == last_key
	        || GUI_MSG_KEY_NUM6 == last_key
	        || GUI_MSG_KEY_ESCAPE == last_key
          )
        {
            //linein_cmd2parent(msg->h_deswin,SWITCH_TO_MMENU,0,1 );//进入后台模式
            av1_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0 ); //直接退出
        }
        else if(GUI_MSG_KEY_NUM1 == last_key)
        {
            __here__;
            //dsk_video_in_dectection();
            //dsk_video_tcon_test();
        }
    }

    else
    {
        last_key = msg->dwAddData1;

        switch(last_key)
        {
            eLIBs_printf("last_key =%x\n", last_key);
        case  GUI_MSG_KEY_ESCAPE://return
        {
            //__app_videoin_stop();
            av1_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
            break;
        }
        case  GUI_MSG_KEY_MENU:
        case GUI_MSG_KEY_NUM6:
        {
            //	eLIBs_printf("MENU!\n");
            av1_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
            break;
        }

        case GUI_MSG_KEY_LEFT://left
        {
            eLIBs_printf("LEFT!\n");
            //if(saturation_av1>0)
            //	saturation_av1--;
            //eLIBs_printf("saturation_av1=%d\n",saturation_av1);
            //com_video_set_saturation(saturation_av1);
            break;
        }

        case GUI_MSG_KEY_RIGHT://right
        {
            eLIBs_printf("right!\n");
            break;
        }

        case GUI_MSG_KEY_VADD://volume add
        {
#if 0
            __volume_reset_hide_timmer(scene_para);
            if (scene_para->cur_val < scene_para->max_val)
            {
                eLIBs_printf("vadd\n");
                scene_para->cur_val++;
                videoin_cmd2parent(msg->h_deswin, videoin_volume_scene_msg_volumeset, scene_para->cur_val, 0);
            }
#endif
            break;
        }


        case GUI_MSG_KEY_VDEC://void decs
        {
            eLIBs_printf("vdec!\n");
            break;
        }


        case  GUI_MSG_KEY_ENTER://enter
        {
            eLIBs_printf("ENTER!\n");
			spi_auto_srch->channel = 0;
			spi_auto_srch->max_channel = 0;
			spi_auto_srch->max_value = 0;
			key_flag = KEY_PRESS_ACTION;
            break;
        }

        case GUI_MSG_KEY_UP://up
        {
            eLIBs_printf("up!\n");
            break;
        }

        case GUI_MSG_KEY_DOWN://down
        {
            eLIBs_printf("down!\n");
            break;
        }

        default:
        {
            ret = EPDK_FALSE;
            break;
        }
        }
    }

    return ret;
}


static __s32  av1_win_touch_proc(__gui_msg_t *msg)
{
    __s32 x, y;
    static __bool down_point_in_rect = EPDK_FALSE;
    static __bool up_point_in_rect = EPDK_FALSE;
    av1_uipara_t *av1_uipara;

    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);



    av1_uipara = (av1_uipara_t *)app_av1_get_uipara();
    if(NULL == av1_uipara)
    {
        __wrn("av1_uipara is null...\n");
        return EPDK_FAIL;
    }

    switch(msg->dwAddData1)
    {
    case GUI_MSG_TOUCH_DOWN:
    {
        if(GUI_WinGetCaptureWin() != msg->h_deswin)
        {
            GUI_WinSetCaptureWin(msg->h_deswin);
        }

        break;
    }
    case GUI_MSG_TOUCH_UP:
    {
        if(GUI_WinGetCaptureWin() == msg->h_deswin)
        {
            GUI_WinReleaseCapture();
        }

        break;
    }
    default:
    {
        break;
    }
    }

    if(GUI_MSG_TOUCH_UP == msg->dwAddData1
            && EPDK_TRUE == up_point_in_rect
            && EPDK_TRUE == down_point_in_rect)
    {
        av1_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 1 ); //进入后台模式
    }

    return EPDK_OK;
}
#define AV1_OSD_TIMEOUT		25//130		// 5S
#define AV1_SPI_TIMEOUT		20
static av1_para_t *av1_initpara;
typedef struct   valuet
{
    __u32   single_value;
    __u32   resverd;
} single_t;

static __s32 _av1_proc(__gui_msg_t *msg)
{
    ES_FILE *ktemp;
    single_t key_value;

    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        av1_ctrl_t *av1_ctrl;
        av1_para_t *av1_para;
		
        __wrn("_av1_proc\n");
        av1_ctrl = (av1_ctrl_t *)My_Balloc(sizeof(av1_ctrl_t));
        if(!av1_ctrl)
        {
            __msg("memory balloc fail.........\n");
            return EPDK_FAIL;
        }
		
        eLIBs_memset(av1_ctrl, 0, sizeof(av1_ctrl_t));

        av1_para = (av1_para_t *)GUI_WinGetAttr(msg->h_deswin);

        av1_ctrl->av1_scene_para = av1_para;
        GUI_WinSetAddData(msg->h_deswin, (__u32)av1_ctrl);

        //GUI_LyrWinCacheOn();
        GUI_LyrWinSetSta(av1_ctrl->av1_scene_para->layer, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSetTop(av1_ctrl->av1_scene_para->layer);
        //GUI_LyrWinCacheOff();
        av1_ctrl->init_osd_off_timmer_id = 0x18;
        GUI_SetTimer(msg->h_deswin, av1_ctrl->init_osd_off_timmer_id, AV1_OSD_TIMEOUT, 0);
	
        av1_ctrl->av1_spi_timmer_id = 0x30;
        GUI_SetTimer(msg->h_deswin, av1_ctrl->av1_spi_timmer_id, AV1_SPI_TIMEOUT, 0);
    }
    return EPDK_OK;
    case GUI_MSG_CLOSE:
        return EPDK_OK;
    case GUI_MSG_DESTROY:
    {
        av1_ctrl_t *av1_ctrl;
        av1_ctrl = (av1_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        if(!av1_ctrl)
        {

            __wrn("av1_ctrl is null.......\n");
            return EPDK_FAIL;
        }

        if(GUI_IsTimerInstalled(msg->h_deswin, av1_ctrl->init_osd_off_timmer_id))
        {
            GUI_KillTimer(msg->h_deswin, av1_ctrl->init_osd_off_timmer_id);
        }
		
        if(GUI_IsTimerInstalled(msg->h_deswin, av1_ctrl->av1_spi_timmer_id))
        {
            GUI_KillTimer(msg->h_deswin, av1_ctrl->av1_spi_timmer_id);
        }
		
        if(av1_ctrl->av1_scene_para)
        {
            My_Bfree(av1_ctrl->av1_scene_para, sizeof(av1_para_t));
        }

        if(av1_ctrl)
        {
            My_Bfree(av1_ctrl, sizeof(av1_ctrl_t));
        }
        if(av1_initpara->single0_handle)
        {
            dsk_theme_close(av1_initpara->single0_handle);
            av1_initpara->single0_handle = NULL ;
        }
        if(av1_initpara->single1_handle)
        {
            dsk_theme_close(av1_initpara->single1_handle);
            av1_initpara->single1_handle = NULL ;
        }
        if(av1_initpara->single2_handle)
        {
            dsk_theme_close(av1_initpara->single2_handle);
            av1_initpara->single2_handle = NULL ;
        }
        if(av1_initpara->single3_handle)
        {
            dsk_theme_close(av1_initpara->single3_handle);
            av1_initpara->single3_handle = NULL ;
        }
        if(av1_initpara->single4_handle)
        {
            dsk_theme_close(av1_initpara->single4_handle);
            av1_initpara->single4_handle = NULL ;
        }

        if(av1_initpara->no_single0_handle)
        {
            dsk_theme_close(av1_initpara->no_single0_handle);

            av1_initpara->no_single0_handle = NULL;
        }
        if(av1_initpara->no_single1_handle)
        {
            dsk_theme_close(av1_initpara->no_single1_handle);

            av1_initpara->no_single1_handle = NULL;
        }
    }

    return EPDK_OK;
    case GUI_MSG_PAINT:
        //return av1_loading_paint(msg->h_deswin, 0);
        return EPDK_OK;
    case GUI_MSG_KEY:
        return av1_win_key_proc(msg);
    case GUI_MSG_TOUCH:
        return av1_win_touch_proc(msg);
    case GUI_MSG_COMMAND:
        return EPDK_OK;
    case GUI_MSG_TIMER:
	{
		ES_FILE *ktemp;
		av1_ctrl_t *av1_ctrl;
        av1_ctrl = (av1_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

		if((LOWORD(msg->dwAddData1) == av1_ctrl->av1_spi_timmer_id)&&(key_flag == KEY_PRESS_ACTION))
		{
			ES_FILE *ktemp1;
			
			ktemp1 = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "w");
			if(!ktemp)
	    	{
		        __msg("open_key_fail\n");
				return EPDK_OK;
	    	}
			
			if(KEY_PRESS_ACTION == key_flag)
			{
				if(spi_auto_srch->channel)
				{
					eLIBs_fioctrl(ktemp1, DRV_KEY_CMD_SET_FIRST_DEBOUNCE_TIME, NULL, &key_value);
					__msg("value=%d\n", key_value.single_value);
					if(spi_auto_srch->max_value < key_value.single_value)
					{
						spi_auto_srch->max_value = key_value.single_value; 
						spi_auto_srch->max_channel = spi_auto_srch->channel - 1;
						__msg("spi_auto_srch->max_value = %d\n", spi_auto_srch->max_value);
						__msg("spi_auto_srch->max_channel = %d\n", spi_auto_srch->max_channel);
					}
				}
				eLIBs_fioctrl(ktemp1, DRV_KEY_CMD_AUTO_SEARCH_SPI, NULL, &spi_auto_srch->channel);
			}
			eLIBs_fclose( ktemp1 );
			if(++spi_auto_srch->channel >31)
			{
				spi_auto_srch->search_end = 1;
				spi_auto_srch->channel = 0;
				key_flag = KEY_NONE_ACTION;
			}
		}
		if((LOWORD(msg->dwAddData1) == av1_ctrl->init_osd_off_timmer_id)&&(spi_auto_srch->search_end))
		{
			ktemp = eLIBs_fopen("b:\\INPUT\\MATRIX_KEY", "w");
			if(!ktemp)
	    	{
		        __msg("open_key_fail\n");
				return EPDK_OK;
	    	}
			eLIBs_fioctrl(ktemp, DRV_KEY_CMD_SET_FIRST_DEBOUNCE_TIME, NULL, &key_value);
			if(spi_auto_srch->max_value < key_value.single_value)
			{
				spi_auto_srch->max_value = key_value.single_value; 
				spi_auto_srch->max_channel = 31;
			}
			__msg("spi_auto_srch->max_value = %d\n", spi_auto_srch->max_value);
			__msg("spi_auto_srch->max_channel = %d\n", spi_auto_srch->max_channel);
			eLIBs_fioctrl(ktemp, DRV_KEY_CMD_AUTO_SEARCH_SPI, NULL, &spi_auto_srch->max_channel);
			spi_auto_srch->search_end = 0;
			eLIBs_fclose( ktemp );
		}
				
        av_single_level = key_value.single_value;

        if(av_prevsingle_level != av_single_level)
        {
        	//__msg("value=%d\n", key_value.single_value);
            av_prevsingle_level = av_single_level;
            _dv_single_paint(msg);
        }
        //__msg("value=%d\n", key_value.single_value);

        return EPDK_OK;
    }
    default:
        break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

/*
	建立窗口
*/

static H_WIN av1_win_create(H_WIN h_parent, av1_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    //av1_para_t *av1_scene_para;
    FB fb;

    __inf("****************************************************************************************\n");
    __inf("********  enter video in application  **************\n");
    __inf("****************************************************************************************\n");

    GUI_LyrWinGetFB(para->layer, &fb);
    av1_initpara = (av1_para_t *)My_Balloc(sizeof(av1_para_t));
    if(!av1_initpara)
    {
        __err("memory balloc fail.........\n");
        return EPDK_FAIL;
    }
    av1_initpara->font = para->font;
    av1_initpara->layer = para->layer;

    av1_initpara->single0_handle = dsk_theme_open(ID_DV_RF_SIGNAL_0_BMP);
    av1_initpara->single1_handle = dsk_theme_open(ID_DV_RF_SIGNAL_1_BMP);
    av1_initpara->single2_handle = dsk_theme_open(ID_DV_RF_SIGNAL_2_BMP);
    av1_initpara->single3_handle = dsk_theme_open(ID_DV_RF_SIGNAL_3_BMP);
    av1_initpara->single4_handle = dsk_theme_open(ID_DV_RF_SIGNAL_FULL_BMP);
    av1_initpara->no_single0_handle = dsk_theme_open(ID_DV_NOSINGLE0_BMP);
    av1_initpara->no_single1_handle = dsk_theme_open(ID_DV_NOSINGLE1_BMP);
    __here__;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));

    framewin_para.name 			= "app_av1";
    framewin_para.dwExStyle 	= WS_EX_NONE;
    framewin_para.dwStyle 		= WS_NONE | WS_VISIBLE;
    framewin_para.spCaption 	= NULL;
    framewin_para.hOwner		= NULL;
    framewin_para.hHosting 		= h_parent;
    framewin_para.id        	= APP_AV1_ID;
    framewin_para.FrameWinProc 	= (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_av1_proc);
    framewin_para.rect.x 		= 0;
    framewin_para.rect.y 		= 0;
    framewin_para.rect.width	= fb.size.width;
    framewin_para.rect.height 	= fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red 	= 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue 	= 0;
    framewin_para.attr 			=  (void *)av1_initpara;
    framewin_para.hLayer 		= para->layer;

    return (GUI_FrmWinCreate(&framewin_para));
}

void  av1_win_delete(H_WIN av1_win)
{
    if(av1_win != NULL)
    {
        GUI_LyrWinDelete(GUI_WinGetLyrWin(av1_win));
    }
}

static H_WIN av1_frame_create(H_WIN hparent)
{

    av1_para_t av1_scene_para;
    av1_uipara_t *av1_uipara;
    RECT rect;
    H_WIN h_wnd;

    gscene_bgd_set_state(BGD_STATUS_HIDE);
    rect.x = 0;
    rect.y = 0;
    rect.width = SCREEN_WIDTH;
    rect.height = SCREEN_HEIGHT;

    //Esh_Msg("in  gscene_bgd_set_state(BGD_STATUS_SHOW)");
    av1_scene_para.layer = av1_layer_create(&rect);
    eLIBs_printf("\n   av1_win_create beginning!\n");
    av1_scene_para.font = SWFFont;
    h_wnd = av1_win_create(hparent, &av1_scene_para);
	
    return h_wnd;
}

#if 1
#define PIOC_MEM_BASE   (0xf1c20800)

#define PIO_REG_CFG(n, i)               ((volatile unsigned int *)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x00))
#define PIO_REG_DLEVEL(n, i)            ((volatile unsigned int *)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x14))
#define PIO_REG_PULL(n, i)              ((volatile unsigned int *)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x1C))
#define PIO_REG_DATA(n) 	            ((volatile unsigned int *)( PIOC_MEM_BASE + ((n)-1)*0x24 + 0x10))

static __s32 get_gpio_status(__u32 port, __u32 port_num)
{
    volatile  __u32    *tmp_addr;
    __u32               tmp_data;
    __s32               ret;
    __u32               tmp1;
    __u32               tmp2;

    //设置为输入
    tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
    tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x07 << tmp1);
    tmp_data |=  (0x00 << tmp1);
    *tmp_addr = tmp_data;

    //设置为下拉
    tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
    tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x03 << tmp2);
    tmp_data |=  (0x02 << tmp2);
    *tmp_addr = tmp_data;


    tmp_addr = PIO_REG_DATA(port);
    tmp_data = *tmp_addr;

    __wrn("tmp_data=0x%x\n", tmp_data);
    ret = (tmp_data & (1 << port_num)) >> port_num;

    return ret;
}

static void pull_down_gpio(__u32 port, __u32 port_num)
{
    volatile  __u32    *tmp_addr;
    __u32               tmp_data;
    __u32               tmp1;
    __u32               tmp2;


    //设置为输出
    tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
    tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x07 << tmp1);
    tmp_data |=  (0x01 << tmp1);
    *tmp_addr = tmp_data;

    //设置为下拉
    tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
    tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x03 << tmp2);
    tmp_data |=  (0x02 << tmp2);
    *tmp_addr = tmp_data;

    //输出低电平
    tmp_addr = PIO_REG_DATA(port);
    tmp_data = *tmp_addr;
    tmp_data &= ~(1 << port_num);
    //tmp_data |=  (1 << port_num);
    *tmp_addr = tmp_data;

}

static void pull_up_gpio(__u32 port, __u32 port_num)
{
    volatile  __u32    *tmp_addr;
    __u32               tmp_data;
    __u32               tmp1;
    __u32               tmp2;


    //设置为输出
    tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
    tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x07 << tmp1);
    tmp_data |=  (0x01 << tmp1);
    *tmp_addr = tmp_data;

    //设置为上拉
    tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
    tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x03 << tmp2);
    tmp_data |=  (0x01 << tmp2);
    *tmp_addr = tmp_data;

    //输出高电平
    tmp_addr = PIO_REG_DATA(port);
    tmp_data = *tmp_addr;
    tmp_data &= ~(1 << port_num);
    tmp_data |=  (1 << port_num);
    *tmp_addr = tmp_data;

}

static __s32 __lcd_bl_open(void)
{

    __msg("__lcd_bl_open ok\n");
}
#endif


typedef struct
{

    __u32 micPreAmpGainCtl    : 3;//B2-B0

    __u32 micPreAmpEnBit      : 1;//B3
    __u32	FMtoLRMirerGain		: 3;//B6-B4
    __u32 paspreedselect 		: 1;//b7
    __u32 adcInputSrcSel      : 6;//b13-b8. adc input source
    __u32 CosSlopTimeForAntipop: 2; //b15-b14
    __u32 adcInputGainCtl     : 3;//b18-16, adc input gain
    __u32 reserved4           : 2;//b20-19
    __u32	LineintoLRMirerGain	: 3;//b23-21
    __u32 MictoLRMirerGain	: 3; //b26-24
    __u32 reserved5           : 4;//b30-b27
    __u32 adcEnBit            : 1;//b31
} AdcAnaCtl;


//value rang is 0-7
//0db -14db 2 db/step
static __s32 _audio_set_linein_gain(__s32 value)
{
    volatile AdcAnaCtl *pAdcAnaCtl = (volatile AdcAnaCtl *)(0x24 + 0Xf1C23C00);
    if(value > 7 || value < 0)
        return -1;
    pAdcAnaCtl->LineintoLRMirerGain	= value;
    return 0;

}

static __s32 __app_av1_start(void) // Need Modify the addr
{
    reg_system_para_t *para;

#if 1
    ES_FILE *paudio;
    __here__;
    paudio = eLIBs_fopen("b:\\AUDIO\\FM", "r+");
    if(paudio)
    {
        __here__;
        eLIBs_fioctrl(paudio, AUDIO_DEV_CMD_START, 0xff, 0);
        eLIBs_fclose(paudio);

        //*(volatile unsigned long*)(0xf1c23c00 + 0x10) |= (0x5<<23);
        //__wrn("*(volatile unsigned long*)(0xf1c23c00 + 0x10)=0x%x\n", *(volatile unsigned long*)(0xf1c23c00 + 0x10));
    }
    else
    {
        __msg("audio dev open fail\n");
    }
    __here__;

    _audio_set_linein_gain(3);
    __here__;
#endif

    //dsk_display_lcd_off();
    //dsk_video_open();
#if 1
    com_video_in_open(1, 0, 0);
#else
    para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
    if(para->pal_ntsc == 0)
        com_video_in_open(1, 0, 1, 0);
    else
        com_video_in_open(1, 0, 1, 1);
#endif

    //__lcd_bl_open();
    //eLIBs_printf("start av1 [f1c23c20]=0x%x\n",*(volatile unsigned long*)0xf1c23c20);

    return EPDK_OK;
}

static __s32 __app_av1_stop(void) // Need Modify the addr
{
#if 0
    ES_FILE *paudio;

    _audio_set_linein_gain(0);
    paudio = eLIBs_fopen("b:\\AUDIO\\FM", "r+");
    if(paudio)
    {
        __inf("\n              VIDEO IN STOP succeed!\n");
        eLIBs_fioctrl(paudio, AUDIO_DEV_CMD_STOP, 0xff, 0);
        eLIBs_fclose(paudio);

        //*(volatile unsigned long*)(0xf1c23c00 + 0x10) |= (0x3<<23);
        //__wrn("*(volatile unsigned long*)(0xf1c23c00 + 0x10)=0x%x\n", *(volatile unsigned long*)(0xf1c23c00 + 0x10));
    }
#endif
    //dsk_video_close();
    com_video_in_close();
    //esKRNL_TimeDly(10);

    //dsk_display_lcd_on();
    //eLIBs_printf("\n   stop here!\n");
    //gscene_bgd_refresh();//wqx 20141010

    //eLIBs_printf("exit av1 [f1c23c20]=0x%x\n",*(volatile unsigned long*)0xf1c23c20);
    return EPDK_OK;
}


static __s32 __app_av1_proc(__gui_msg_t *msg)
{
    __s32 ret;

    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        root_para_t *para;
        av1_man_ctrl_t *man_ctrl;

        para = (root_para_t *)GUI_WinGetAttr(msg->h_deswin);
        if(NULL == para)
        {
            __err("invalid para...\n");
            return EPDK_FAIL;
        }
		
        __wrn("para->data=%d\n", para->data);

        man_ctrl = esMEMS_Balloc(sizeof(av1_man_ctrl_t));
        if(NULL == man_ctrl)
        {
            __wrn("mem not enough...\n");
            return EPDK_FAIL;
        }
        eLIBs_memset(man_ctrl, 0, sizeof(av1_man_ctrl_t));
		
        eLIBs_memset(spi_auto_srch, 0, sizeof(auto_search));

        GUI_WinSetAddData(msg->h_deswin, (__u32)man_ctrl);
#if 1//1107
        gscene_bgd_set_state(BGD_STATUS_HIDE);
#endif
        __app_av1_start();
        __wrn("para->data=%d\n", para->data);

        man_ctrl->hfrm = av1_frame_create(msg->h_deswin);
        GUI_WinSetFocusChild(man_ctrl->hfrm);


        //dsk_amplifier_onoff(1);
        //dsk_shdn_onoff(1);   //打开外接输入

        dsk_set_auto_off_time(0);
        //不允许关屏
        g_disable_close_scn();

        //gscene_hbar_set_music_state(BG_AUX);

        g_disable_standby();
#ifdef HBAR_SHOW_BT
        gscene_hbar_set_state(HBAR_ST_SHOW);
#else
        gscene_hbar_set_state(HBAR_ST_HIDE);
#endif

        return EPDK_OK;
    }
    case GUI_MSG_DESTROY:
    {
        av1_man_ctrl_t *man_ctrl;

        __wrn("__app_av1_proc: GUI_MSG_DESTROY begin\n");

        //		dsk_shdn_onoff(0);//  关闭音频输入
        man_ctrl = (av1_man_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        if(NULL == man_ctrl)
        {
            return EPDK_FAIL;
        }

        g_enable_standby();

        //dsk_amplifier_onoff(0);//关闭功放

        {
            reg_system_para_t *para;
            para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
            if(para)
            {
                dsk_set_auto_off_time(para->poweroff);
                __wrn("para->poweroff=%d\n", para->poweroff);
            }
        }

        ret =  __app_av1_stop();
        if(EPDK_FAIL == ret)
        {
            eLIBs_printf("  __app_av1_stop false!");
        }

        if(man_ctrl->hfrm)
        {
            av1_win_delete(man_ctrl->hfrm);
            man_ctrl->hfrm = NULL;
        }

        g_enable_close_scn();
        __wrn("__app_av1_proc: GUI_MSG_DESTROY end\n");
        return EPDK_OK;
    }
    case DSK_MSG_HOME:
    case DSK_MSG_KILL:
    case GUI_MSG_CLOSE:
    {
        av1_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 1 ); //进入后台模式
        GUI_ManWinDelete(msg->h_deswin);
        return EPDK_OK;
    }

    default:
    {
        __msg("__app_video_proc: default\n");

        break;
    }
    }

    return GUI_ManWinDefaultProc(msg);
}


H_WIN app_av1_create(root_para_t *para)
{
    __gui_manwincreate_para_t create_info;
    H_WIN hManWin;
#ifdef AV1_AUTO_STANDBY
    extern __u32 av_no_signal_cnt;
    av_no_signal_cnt = 0;
#endif
    av_prevsingle_level = 0xff;
    av_single_level = 0;


    __inf("****************************************************************************************\n");
    __inf("********  enter av1 app  **************\n");
    __inf("****************************************************************************************\n");

    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    create_info.name            = "app_av1";
    create_info.hParent         = para->h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__app_av1_proc);
    create_info.attr            = (void *)para;
    create_info.id				= APP_AV1_ID;
    create_info.hHosting        = NULL;

    __wrn("para->data=%d\n", para->data);
    hManWin = GUI_ManWinCreate(&create_info);
    __here__;
    return hManWin;
}


