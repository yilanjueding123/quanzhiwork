
#include "app_fm_i.h"

typedef struct tag_fmmod_ctrl
{
    H_WIN h_frm_fmbottom;
    H_WIN h_frm_fmtop;

    H_WIN h_lyr_fmbottom;
    H_WIN h_lyr_fmtop;


    GUI_FONT *fmmod_font;
    __bool	bIsRecord;
    //__u32	 root_type;
} fmmode_ctrl_t;

/***********************************************************************************************************
	½¨Á¢Í¼²ã
************************************************************************************************************/
static H_LYR fmmod_layer_palette_create(RECT *rect)
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
        "fm ui layer",
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

    lstlyr.pipe = 1;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;

    layer = GUI_LyrWinCreate(&lyrcreate_info);
    if( !layer )
    {
        __err("app bar layer create error !\n");
    }

    return layer;
}

__s32 cb_srhch_success(void *arg_p)
{
    fmmode_ctrl_t	*fmmod_ctrl;
    fmmod_ctrl = (fmmode_ctrl_t *)arg_p;

    NOTIFY_MSG(CMD_AUTOSRH_FINDCH, 0, fmmod_ctrl->h_frm_fmtop, 0, 0);

    return 0;
}

__s32 cb_srhch_fail(void *arg_p)
{
    fmmode_ctrl_t	*fmmod_ctrl;
    fmmod_ctrl = (fmmode_ctrl_t *)arg_p;

    NOTIFY_MSG(CMD_AUTOSRH_FINDCHFAIL, 0, fmmod_ctrl->h_frm_fmtop, 0, 0);

    return 0;
}

__s32 cb_srhch_over(void *arg_p)
{
    fmmode_ctrl_t	*fmmod_ctrl;
    fmmod_ctrl = (fmmode_ctrl_t *)arg_p;

    NOTIFY_MSG(CMD_AUTOSRH_OVER, 0, fmmod_ctrl->h_frm_fmtop, 0, 0);

    return 0;
}


static __s32 fmplay_win_create(__gui_msg_t *msg)
{
    fmmode_ctrl_t	*fmmod_ctrl;
    RECT rect;
    fmplay_frm_para_t frmwin_para;

    fmmod_ctrl = (fmmode_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    rect.x = 0;
    rect.y = 0;
    rect.width = 400;
    rect.height = 240;

    dsk_radio_set_cb(DSK_RADIO_EVENT_SEARCH_SUCCESS, esKRNL_GetCallBack(cb_srhch_success), (void *)fmmod_ctrl);
    dsk_radio_set_cb(DSK_RADIO_EVENT_SEARCH_FAIL, esKRNL_GetCallBack(cb_srhch_fail), (void *)fmmod_ctrl);
    dsk_radio_set_cb(DSK_RADIO_EVENT_SEARCH_OVER, esKRNL_GetCallBack(cb_srhch_over), (void *)fmmod_ctrl);

    fmmod_ctrl->h_lyr_fmtop = fmmod_layer_palette_create(&rect);

    frmwin_para.fmplay_frm_font = fmmod_ctrl->fmmod_font;
    frmwin_para.layer = fmmod_ctrl->h_lyr_fmtop;
    //frmwin_para.root_type = fmmod_ctrl->root_type;

    fmmod_ctrl->h_frm_fmtop = fmplay_frmwin_create(msg->h_deswin, &frmwin_para);
    GUI_LyrWinSetSta(fmmod_ctrl->h_lyr_fmtop, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(fmmod_ctrl->h_lyr_fmtop);

    GUI_WinSetFocusChild(fmmod_ctrl->h_frm_fmtop);

    return EPDK_OK;
}


static __s32 fmmod_win_create(__gui_msg_t *msg)
{
    /*
    	fmmode_ctrl_t	*fmmod_ctrl;
    	RECT rect;
    	fmbtm_frm_para_t frmwin_para;

    	fmmod_ctrl = (fmmode_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    */
    com_set_palette_by_id(ID_FM_WKM_BMP);

    g_disable_standby();

    if(!dsk_radio_is_open())
    {
        reg_fm_para_t *para;
        para = (reg_fm_para_t *)dsk_reg_get_para_by_app(REG_APP_FM);

        dsk_radio_open();

        dsk_radio_rcv_open();
        dsk_radio_set_band(DSK_RADIO_BAND_US_EUP);

        dsk_radio_set_threshold(DSK_RADIO_THRESHOLD_MID);
        dsk_radio_set_audio_mode(DSK_RADIO_AUDIO_STEREO);

        {
            __s32 i, num = 0;
            for(i = 0; i < 50; i++)
            {
                __msg("para->FM_channel[%d]:%d\n", i, para->FM_channel[i]);
                if(0 != para->FM_channel[i])
                {
                    num++;
                }
            }
            __msg("num:%d\n", num);
            __msg("para->channel_id:%d\n", para->channel_id);
        }

        dsk_radio_rcv_get_search_result(para);
        dsk_radio_rcv_set_play_list_type(PLAY_LIST_SEARCH_RESULT);
    }

    fmplay_win_create(msg);

    return EPDK_OK;
}

void  fm_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;

    msg.h_deswin = GUI_WinGetParent(hwin);
    msg.h_srcwin = NULL;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;

    GUI_SendNotifyMessage(&msg);
}

void  fm_cmd2win(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;

    msg.h_deswin = hwin;
    msg.h_srcwin = NULL;
    msg.id = id;
    msg.dwAddData1 = data1;
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;

    GUI_SendNotifyMessage(&msg);
}


static __s32 _app_fmmod_Proc(__gui_msg_t *msg)
{
    switch(msg->id)
    {
    case DSK_MSG_FS_PART_PLUGOUT:
    {
        fmmode_ctrl_t *fmmod_ctrl;
        fmmod_ctrl = (fmmode_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

        fm_cmd2win(fmmod_ctrl->h_frm_fmtop, DSK_MSG_FS_PART_PLUGOUT, 0, 0);
    }
    return EPDK_OK;
    case DSK_MSG_FS_PART_PLUGIN:
    {
        fmmode_ctrl_t *fmmod_ctrl;
        fmmod_ctrl = (fmmode_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

        fm_cmd2win(fmmod_ctrl->h_frm_fmtop, DSK_MSG_FS_PART_PLUGIN, 0, 0);
    }
    return EPDK_OK;
    case GUI_MSG_CREATE:
    {
        fmmod_win_create(msg);
    }
    return EPDK_OK;
    case GUI_MSG_CLOSE:
    {
        GUI_ManWinDelete(msg->h_deswin);

    }
    return EPDK_OK;
    case GUI_MSG_DESTROY:
    {
        fmmode_ctrl_t *fmmod_ctrl;

        fmmod_ctrl = (fmmode_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

        g_enable_standby();
        if( !fmmod_ctrl->bIsRecord )
            dsk_radio_close();

        GUI_LyrWinDelete(fmmod_ctrl->h_lyr_fmbottom);
        GUI_LyrWinDelete(fmmod_ctrl->h_lyr_fmtop);

        esMEMS_Bfree(fmmod_ctrl, sizeof(fmmode_ctrl_t));
    }
    return EPDK_OK;
    case GUI_MSG_KEY:
        break;
    case GUI_MSG_TOUCH:
        break;
    case GUI_MSG_COMMAND:
    {
        fmmode_ctrl_t *fmmod_ctrl;

        fmmod_ctrl = (fmmode_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

        __msg("------%s,%d-,msg->dwAddData1:%x---\n", __FUNCTION__, __LINE__, msg->dwAddData1);

        switch(LOWORD(msg->dwAddData1))
        {
        case FM_TOP_FRMID:
        {
            switch(HIWORD(msg->dwAddData1))
            {
            case COMMAND_SWITCH_RECORD:
            {
                fmmod_ctrl->bIsRecord = EPDK_TRUE;
                GUI_LyrWinSetSta(fmmod_ctrl->h_lyr_fmtop, GUI_LYRWIN_STA_SUSPEND);
                fm_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, MAKELONG(FM_SW_TO_RECORD, msg->dwAddData2), 0);
            }
            break;
            case COMMAND_SWITCH_MMENU:
            {
                fmmod_ctrl->bIsRecord = EPDK_FALSE;
                GUI_LyrWinSetSta(fmmod_ctrl->h_lyr_fmtop, GUI_LYRWIN_STA_SUSPEND);
                fm_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
            }
            break;
            case COMMAND_AUTOSRH_START:
            {
                dsk_radio_set_automanual_mode(DSK_RADIO_SEARCH_AUTO);
                dsk_radio_rcv_set_play_list_type(PLAY_LIST_SEARCH_RESULT);
                dsk_radio_rcv_search_all(msg->dwAddData1, msg->dwReserved);
            }
            break;
            case COMMAND_MANUALSRH_START:
            {
                dsk_radio_set_automanual_mode(DSK_RADIO_SEARCH_MANUAL);
                dsk_radio_rcv_set_play_list_type(PLAY_LIST_SEARCH_RESULT);

                dsk_radio_rcv_search_all(msg->dwAddData2, msg->dwReserved);
            }
            break;
            default:
                break;
            }
        }
        break;
        default:
            break;
        }
    }
    return EPDK_OK;
    default:
        break;
    }

    return GUI_ManWinDefaultProc(msg);
}

/*******************************************************************************************************
*********************************************************************************************************/
__s32 app_fm_create(root_para_t *para)
{
    __gui_manwincreate_para_t create_info;
    fmmode_ctrl_t *fmmod_ctrl;

    __inf("****************************************************************************************\n");
    __inf("********  enter fm app  **************\n");
    __inf("****************************************************************************************\n");

    fmmod_ctrl = (fmmode_ctrl_t *)esMEMS_Balloc(sizeof(fmmode_ctrl_t));
    if( fmmod_ctrl == NULL )
    {
        __msg("esMEMS_Balloc() fail!\n");
        return NULL;
    }
    eLIBs_memset(fmmod_ctrl, 0, sizeof(fmmode_ctrl_t));

    fmmod_ctrl->fmmod_font = para->font;
    //fmmod_ctrl->root_type  = para->root_type;

    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));

    create_info.name            = APP_FM;
    create_info.hParent         = para->h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_app_fmmod_Proc);
    create_info.attr            = (void *)fmmod_ctrl;
    create_info.id				= APP_FM_ID;
    create_info.hHosting        = NULL;

    return(GUI_ManWinCreate(&create_info));
}

