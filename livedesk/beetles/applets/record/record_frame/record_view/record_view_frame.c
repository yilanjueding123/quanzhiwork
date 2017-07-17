#include "record_view_frame.h"

//#define TEST_CONTROL

#ifdef TEST_CONTROL
H_WIN	g_controlWin = 0;
#endif

static void rec_view_frm_sendSelf(__gui_msg_t *msg, __u32 id)
{
    __gui_msg_t sendMsg;

    sendMsg.h_deswin = msg->h_deswin;
    sendMsg.h_srcwin = NULL;
    sendMsg.id = id;
    sendMsg.dwAddData1 = 0;
    sendMsg.dwAddData2 = 0;
    sendMsg.dwReserved = 0;

    GUI_SendMessage(&sendMsg);
}

static void rec_view_frm_sendParent(__gui_msg_t *msg, __u32 id)
{
    __gui_msg_t sendMsg;

    sendMsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
    sendMsg.h_srcwin = NULL;
    sendMsg.id = id;
    sendMsg.dwAddData1 = 0;
    sendMsg.dwAddData2 = 0;
    sendMsg.dwReserved = 0;

    GUI_SendMessage(&sendMsg);
}

static __s32 rec_view_frm_on_create(__gui_msg_t *msg)
{
    prec_view_frame_t pviewFrm_ctl;
    pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( pviewFrm_ctl == NULL)
        return EPDK_FAIL;

    pviewFrm_ctl->recTimeID 	= 0xFF;
    GUI_SetTimer(msg->h_deswin, pviewFrm_ctl->recTimeID, REC_TIMER_TIMES, NULL);

    return EPDK_OK;
}
static __s32 _rec_view_frm_on_close(__gui_msg_t *msg)
{
    prec_view_frame_t pviewFrm_ctl;
    pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( pviewFrm_ctl == NULL)
        return EPDK_FAIL;

    if( pviewFrm_ctl->precordDoc->recState != RECORD_STOP )
    {
        rec_view_frm_sendSelf(msg, RECORD_VIEW_FRAME_STOPRECORD);
    }
    GUI_FrmWinDelete(msg->h_deswin);

    return EPDK_OK;
}
static __s32 _rec_view_frm_on_destory(__gui_msg_t *msg)
{
    prec_view_frame_t pviewFrm_ctl;
    pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( pviewFrm_ctl == NULL)
        return EPDK_FAIL;

    record_view_exit(pviewFrm_ctl->pview, pviewFrm_ctl->layer);

    GUI_KillTimer(msg->h_deswin, pviewFrm_ctl->recTimeID);

    record_unInit(pviewFrm_ctl->precordDoc);

    esMEMS_Bfree(pviewFrm_ctl, sizeof(rec_view_frame_t));

    return EPDK_OK;
}
static __s32 _rec_view_frm_on_key(__gui_msg_t *msg)
{
    prec_view_frame_t pviewFrm_ctl;
    pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( pviewFrm_ctl == NULL)
        return EPDK_FAIL;

    if (KEY_UP_ACTION != msg->dwAddData2)
    {
        switch( msg->dwAddData1 )
        {
        case GUI_MSG_KEY_ENTER:
            if( pviewFrm_ctl->pview->curMenu == MENU_RECORD )
            {
                if( pviewFrm_ctl->precordDoc->recState == RECORD_STOP )
                    rec_view_frm_sendSelf(msg, RECORD_VIEW_FRAME_STARTRECORD);
                else if( pviewFrm_ctl->precordDoc->recState == RECORD_START )
                    rec_view_frm_sendSelf(msg, RECORD_VIEW_FRAME_PAUSERECORD);
                else
                    rec_view_frm_sendSelf(msg, RECORD_VIEW_FRAME_STARTRECORD);
            }
            else if( pviewFrm_ctl->pview->curMenu == MENU_MUSIC_LIST )
            {
                if( record_get_files(pviewFrm_ctl->precordDoc) != 0)
                    rec_view_frm_sendParent(msg, RECORD_TO_LIST_VIEW_FRAME);
                else
                    pviewFrm_ctl->precordDoc->timeCnt = 0;
            }
            else if( pviewFrm_ctl->pview->curMenu == MENU_QUALITY )
            {
                //pviewFrm_ctl->pview->curMenuSt = FOCUSE_OVER;
                //pviewFrm_ctl->pview->curMenu   = MENU_RECORD;
                pviewFrm_ctl->precordDoc->recQuality = pviewFrm_ctl->precordDoc->recQualityTemp;
                record_updateFreeTime( pviewFrm_ctl->precordDoc );
                break;
            }
            else if( pviewFrm_ctl->pview->curMenu == MENU_FORMAT )
            {
                //pviewFrm_ctl->pview->curMenuSt = FOCUSE_OVER;
                //pviewFrm_ctl->pview->curMenu   = MENU_RECORD;
                pviewFrm_ctl->precordDoc->audioType = pviewFrm_ctl->precordDoc->audioTypeTemp;
                break;
            }
            else if( pviewFrm_ctl->pview->curMenu == MENU_SAVE )
            {
                rec_view_frm_sendSelf(msg, RECORD_VIEW_FRAME_STOPRECORD);
            }
            pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
            break;
        case GUI_MSG_KEY_VADD:
            if( pviewFrm_ctl->pview->curMenu == MENU_QUALITY &&
                    pviewFrm_ctl->pview->curMenuSt == FOCUSE_PRESS )
            {
                pviewFrm_ctl->precordDoc->recQualityTemp = (QUALITY_STATE)((pviewFrm_ctl->precordDoc->recQualityTemp + 1) % QUALITY_STATE_MAX);
            }
            else if( pviewFrm_ctl->pview->curMenu == MENU_FORMAT &&
                     pviewFrm_ctl->pview->curMenuSt == FOCUSE_PRESS )
            {
                pviewFrm_ctl->precordDoc->audioTypeTemp = (RECORD_AUDIO_TYPE)((pviewFrm_ctl->precordDoc->audioTypeTemp + 1) % RECORD_AUDIO_TYPE_MAX);
            }
            break;
        case GUI_MSG_KEY_LONGLEFT:
        case GUI_MSG_KEY_LEFT:
#ifdef TEST_CONTROL
#endif
            pviewFrm_ctl->pview->curMenu = (RECORD_MENU)((pviewFrm_ctl->pview->curMenu + MENU_MAX - 1) % MENU_MAX);
            pviewFrm_ctl->pview->curMenuSt = FOCUSE_OVER;
            if( pviewFrm_ctl->pview->curMenu == MENU_QUALITY )
            {
                pviewFrm_ctl->precordDoc->recQualityTemp = pviewFrm_ctl->precordDoc->recQuality;
                pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
                pviewFrm_ctl->pview->menuOsffset = 0;
            }
            else if( pviewFrm_ctl->pview->curMenu == MENU_FORMAT )
            {
                pviewFrm_ctl->precordDoc->audioTypeTemp = pviewFrm_ctl->precordDoc->audioType;
                pviewFrm_ctl->pview->menuOsffset = -1;
                pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
            }
            break;
        case GUI_MSG_KEY_LONGRIGHT:
        case GUI_MSG_KEY_RIGHT:
            pviewFrm_ctl->pview->curMenu = (RECORD_MENU)((pviewFrm_ctl->pview->curMenu + 1) % MENU_MAX);
            pviewFrm_ctl->pview->curMenuSt = FOCUSE_OVER;
            if( pviewFrm_ctl->pview->curMenu == MENU_QUALITY )
            {
                pviewFrm_ctl->precordDoc->recQualityTemp = pviewFrm_ctl->precordDoc->recQuality;
                pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
                pviewFrm_ctl->pview->menuOsffset = 0;
            }
            else if( pviewFrm_ctl->pview->curMenu == MENU_FORMAT )
            {
                pviewFrm_ctl->precordDoc->audioTypeTemp = pviewFrm_ctl->precordDoc->audioType;
                pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
                pviewFrm_ctl->pview->menuOsffset = -1;
            }
            break;
        }
    }
    else
    {
        switch( msg->dwAddData1 )
        {
        case GUI_MSG_KEY_MENU:
            rec_view_frm_sendParent(msg, RECORD_TO_HOME);
            break;
        case GUI_MSG_KEY_ENTER:
            if( pviewFrm_ctl->pview->curMenu == MENU_MUSIC_LIST )
            {
                pviewFrm_ctl->pview->curMenuSt = FOCUSE_OVER;
            }
            if( pviewFrm_ctl->pview->curMenu == MENU_SAVE )
            {
                pviewFrm_ctl->pview->curMenuSt = FOCUSE_OVER;
            }
            break;
        }
    }
    record_view_show(pviewFrm_ctl->pview, pviewFrm_ctl->layer);
    return EPDK_OK;
}
static __s32 _rec_view_frm_on_command(__gui_msg_t *msg)
{
    return EPDK_OK;
}
static __s32 _rec_view_frm_on_paint(__gui_msg_t *msg)
{
    prec_view_frame_t pviewFrm_ctl;
    pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( pviewFrm_ctl == NULL)
        return EPDK_FAIL;

    GUI_SetFont(pviewFrm_ctl->sys_font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(0);

    //record_view_show(pviewFrm_ctl->pview, pviewFrm_ctl->layer);

    return EPDK_OK;
}
static __s32 _rec_view_frm_manager_proc(__gui_msg_t *msg)
{
    __msg("_rec_view_frm_manager_proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
          , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
        return rec_view_frm_on_create(msg);
    case GUI_MSG_CLOSE:
        return _rec_view_frm_on_close(msg);
    case GUI_MSG_DESTROY:
        return _rec_view_frm_on_destory(msg);
    case GUI_MSG_PAINT:
        return _rec_view_frm_on_paint(msg);
    case GUI_MSG_KEY:
        return _rec_view_frm_on_key(msg);
    case GUI_MSG_COMMAND:
        return _rec_view_frm_on_command(msg);
    case RECORD_VIEW_FRAME_PAUSERECORD:
    {
        prec_view_frame_t pviewFrm_ctl;
        pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
        if( pviewFrm_ctl == NULL)
            break;

        record_pause(pviewFrm_ctl->precordDoc);
    }
    break;
    case RECORD_VIEW_FRAME_STARTRECORD:		//¿ªÊ¼Â¼Òô
    {
        prec_view_frame_t pviewFrm_ctl;
        pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
        if( pviewFrm_ctl == NULL)
            break;

        if( pviewFrm_ctl->precordDoc->recState == RECORD_STOP )
        {
            record_start(pviewFrm_ctl->precordDoc);
        }
        else if( pviewFrm_ctl->precordDoc->recState == RECORD_PAUSE )
        {
            record_continue(pviewFrm_ctl->precordDoc);
        }
    }
    break;
    case RECORD_VIEW_FRAME_STOPRECORD:		//½áÊøÂ¼Òô
    {
        prec_view_frame_t pviewFrm_ctl;
        pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
        if( pviewFrm_ctl == NULL)
            break;

        if( pviewFrm_ctl->precordDoc->recState != RECORD_STOP )
        {
            record_stop(pviewFrm_ctl->precordDoc);
        }
    }
    break;
    case GUI_MSG_TIMER:
    {
        prec_view_frame_t pviewFrm_ctl;
        pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
        if( pviewFrm_ctl == NULL)
            break;

        pviewFrm_ctl->precordDoc->timeCnt++;
        record_updateCurTime(pviewFrm_ctl->precordDoc);

        record_view_show(pviewFrm_ctl->pview, pviewFrm_ctl->layer);
    }
    return EPDK_OK;
    case GUI_MSG_TOUCH:
        break;
    case GUI_MSG_WIN_WAKEUP:
        GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
        return EPDK_OK;

    }
    return GUI_FrmWinDefaultProc(msg);
}

H_WIN rec_view_frm_create(H_WIN h_parent, precord_ctrl_t para)
{
    __gui_framewincreate_para_t framewin_para;
    prec_view_frame_t			pviewFrm_ctl;
    FB fb;

    GUI_LyrWinGetFB(para->layer, &fb);

    pviewFrm_ctl = (prec_view_frame_t)esMEMS_Balloc(sizeof(rec_view_frame_t));
    if( pviewFrm_ctl == NULL )
    {
        __msg("esMEMS_Balloc fail\n");
        return NULL;
    }
    eLIBs_memset(pviewFrm_ctl, 0, sizeof(rec_view_frame_t));

    pviewFrm_ctl->sys_font 		= para->pfont;
    pviewFrm_ctl->layer	  		= para->layer;
    pviewFrm_ctl->pview	  		= &para->rec_view;

    pviewFrm_ctl->precordDoc    	= &para->rec_Doc;
    pviewFrm_ctl->pview->precord_doc = &para->rec_Doc;

    pviewFrm_ctl->pview->curMenu		= MENU_RECORD;
    pviewFrm_ctl->pview->curMenuSt		= FOCUSE_OVER;
    pviewFrm_ctl->pview->menuOsffset	= 0;

    pviewFrm_ctl->precordDoc->timeCnt   = NO_FILES_TIMES;

    pviewFrm_ctl->precordDoc->curRootType	= para->root_type;	//get from app_record
    if( para->bIsFMRecord )										//get from app_record
        pviewFrm_ctl->precordDoc->recordType = RECORD_FM;
    else
        pviewFrm_ctl->precordDoc->recordType = RECORD_MIC;

    if( record_init(pviewFrm_ctl->precordDoc) == EPDK_FAIL )
    {
        __inf("rec_view_frm_create() create fail!\n");
        esMEMS_Bfree(pviewFrm_ctl, sizeof(rec_view_frame_t));
        return NULL;
    }

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name 			= "record_view_frm",
                        framewin_para.dwExStyle 	= WS_EX_NONE;
    framewin_para.dwStyle 		= WS_NONE | WS_VISIBLE;
    framewin_para.spCaption 	= NULL;
    framewin_para.hOwner		= NULL;
    framewin_para.id         	= APP_RECORD_VIEW_ID;
    framewin_para.hHosting 		= h_parent;
    framewin_para.FrameWinProc 	= (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_rec_view_frm_manager_proc);
    framewin_para.rect.x 		= 0;
    framewin_para.rect.y 		= 0;
    framewin_para.rect.width	= fb.size.width;
    framewin_para.rect.height 	= fb.size.height;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red 	= 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue 	= 0;
    framewin_para.attr 			= (void *)pviewFrm_ctl;
    framewin_para.hLayer 		= para->layer;


    GUI_LyrWinSetSta(para->layer, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(para->layer);

    return (GUI_FrmWinCreate(&framewin_para));
}


















