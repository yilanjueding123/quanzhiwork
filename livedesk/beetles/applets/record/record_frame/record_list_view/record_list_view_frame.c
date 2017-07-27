#include "record_list_view_frame.h"

static __s32 _rec_list_view_frm_play(__gui_msg_t *msg);
static __s32 _rec_list_view_frm_pre(__gui_msg_t *msg);
static __s32 _rec_list_view_frm_next(__gui_msg_t *msg);
static void _rec_list_view_frm_sendSelf(__gui_msg_t *msg, __u32 id)
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

static void _rec_list_view_frm_sendParent(__gui_msg_t *msg, __u32 id)
{
    __gui_msg_t sendMsg;

    sendMsg.h_deswin = GUI_WinGetParent(msg->h_deswin);;
    sendMsg.h_srcwin = NULL;
    sendMsg.id = id;
    sendMsg.dwAddData1 = 0;
    sendMsg.dwAddData2 = 0;
    sendMsg.dwReserved = 0;

    GUI_SendMessage(&sendMsg);
}
static __s32 _rec_list_view_frm_on_create(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( plistviewFrm_ctl == NULL )
        return EPDK_FAIL;

    plistviewFrm_ctl->plistDoc->musicTimeID = 0xff;
    GUI_SetTimer(msg->h_deswin, plistviewFrm_ctl->plistDoc->musicTimeID, REC_LIST_TIMER_TIMES, NULL);

    record_list_play(plistviewFrm_ctl->plistDoc);

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_close(__gui_msg_t *msg)
{
    GUI_FrmWinDelete(msg->h_deswin);

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_destory(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( plistviewFrm_ctl == NULL )
        return EPDK_FAIL;

    record_list_MusciUninit( plistviewFrm_ctl->plistDoc );
    GUI_KillTimer(msg->h_deswin, plistviewFrm_ctl->plistDoc->musicTimeID);

    esMEMS_Bfree(plistviewFrm_ctl, sizeof(rec_list_view_frame_t));

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_menu_key(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( plistviewFrm_ctl == NULL )
        return EPDK_FAIL;

    if (KEY_UP_ACTION != msg->dwAddData2)
    {
        switch( msg->dwAddData1 )
        {
        case GUI_MSG_KEY_ENTER:
            if( plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_BACKWARD )
            {
                record_list_backward( plistviewFrm_ctl->plistDoc );
            }
            else if( plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_FORWARD )
            {
                record_list_forward( plistviewFrm_ctl->plistDoc );
            }
            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_PRESS;
            break;
        case GUI_MSG_KEY_LONGLEFT:
        case GUI_MSG_KEY_LEFT:
            if( plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_FORWARD )
            {
                plistviewFrm_ctl->plistView->curMenu = plistviewFrm_ctl->plistView->playMenu;
            }
            else if( plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PAUSE ||
                     plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PLAY )
            {
                plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_BACKWARD;
            }
            else
            {
                plistviewFrm_ctl->plistView->curMenu = (MUSIC_LIST_MENU)((plistviewFrm_ctl->plistView->curMenu + MUSIC_MENU_MAX - 1) % MUSIC_MENU_MAX);
            }
            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
            break;
        case GUI_MSG_KEY_LONGRIGHT:
        case GUI_MSG_KEY_RIGHT:
            if( plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_BACKWARD )
            {
                plistviewFrm_ctl->plistView->curMenu = plistviewFrm_ctl->plistView->playMenu;
            }
            else if( plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PAUSE ||
                     plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PLAY )
            {
                plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_FORWARD;
            }
            else
            {
                plistviewFrm_ctl->plistView->curMenu = (MUSIC_LIST_MENU)((plistviewFrm_ctl->plistView->curMenu + 1) % MUSIC_MENU_MAX);
            }
            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
            break;
        }
    }
    else
    {
        switch( msg->dwAddData1 )
        {
        case GUI_MSG_KEY_MENU:
            plistviewFrm_ctl->plistView->curPart = RECORD_MUSIC_LIST;
            break;
        case GUI_MSG_KEY_LONGVADD:
            plistviewFrm_ctl->plistView->oldPart = plistviewFrm_ctl->plistView->curPart;
            plistviewFrm_ctl->plistView->curPart = RECORD_MUSIC_VOLUME;
            break;
        case GUI_MSG_KEY_ENTER:
            if( plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_BACK )
            {
                _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_BACK);
            }
            else if( plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PLAY )
            {
                plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_PAUSE;
                plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PAUSE;
                _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_CONTINUE);
            }
            else if( plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PAUSE )
            {
                plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_PLAY;
                plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PLAY;
                _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_PAUSE);
            }
            else if( plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_DEL )
            {
                plistviewFrm_ctl->plistView->delState = RECORD_DEL_CAL;
                plistviewFrm_ctl->plistView->curPart  = RECORD_MUSIC_DELETE;
                _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_PAUSE);
            }
            else if( plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_BACKWARD ||
                     plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_FORWARD )
            {
                _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_CONTINUE);
            }
            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
            break;
        case GUI_MSG_KEY_LEFT:
            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
            break;
        case GUI_MSG_KEY_RIGHT:
            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
            break;
        }
    }

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_list_key(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( plistviewFrm_ctl == NULL )
        return EPDK_FAIL;

    if (KEY_UP_ACTION != msg->dwAddData2)
    {
        switch( msg->dwAddData1 )
        {
        case GUI_MSG_KEY_ENTER:
            if( plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PLAY ||
                    plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PAUSE )
                plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_PAUSE;
            plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PAUSE;
            _rec_list_view_frm_play(msg);
            //_rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_PLAY);
            break;
        case GUI_MSG_KEY_LONGLEFT:
        case GUI_MSG_KEY_LEFT:
            _rec_list_view_frm_pre(msg);
            //_rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_PRE);
            break;
        case GUI_MSG_KEY_LONGRIGHT:
        case GUI_MSG_KEY_RIGHT:
            _rec_list_view_frm_next(msg);
            //_rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_NEXT);
            break;
        }
    }
    else
    {
        switch( msg->dwAddData1 )
        {
        case GUI_MSG_KEY_MENU:
            _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_BACK);
            break;
        case GUI_MSG_KEY_LONGMENU:
            plistviewFrm_ctl->plistView->curPart = RECORD_MUSIC_MENU;
            break;
        case GUI_MSG_KEY_LONGVADD:
            plistviewFrm_ctl->plistView->oldPart = plistviewFrm_ctl->plistView->curPart;
            plistviewFrm_ctl->plistView->curPart = RECORD_MUSIC_VOLUME;
            break;
        }
    }

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_delete_key(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( plistviewFrm_ctl == NULL )
        return EPDK_FAIL;

    if (KEY_UP_ACTION != msg->dwAddData2)
    {
        switch( msg->dwAddData1 )
        {
        case GUI_MSG_KEY_LONGRIGHT:
        case GUI_MSG_KEY_LONGLEFT:
        case GUI_MSG_KEY_RIGHT:
        case GUI_MSG_KEY_LEFT:
            if( plistviewFrm_ctl->plistView->delState == RECORD_DEL_OK )
                plistviewFrm_ctl->plistView->delState = RECORD_DEL_CAL;
            else
                plistviewFrm_ctl->plistView->delState = RECORD_DEL_OK;
            break;
        }
    }
    else
    {
        switch( msg->dwAddData1 )
        {
        case GUI_MSG_KEY_MENU:
            plistviewFrm_ctl->plistView->delState = RECORD_DEL_NONE;
            plistviewFrm_ctl->plistView->curPart = RECORD_MUSIC_MENU;

            if( plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PLAY ||
                    plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PAUSE )
                plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_PAUSE;
            plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PAUSE;
            _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_PLAY);
            break;
        case GUI_MSG_KEY_ENTER:
            if( plistviewFrm_ctl->plistView->delState == RECORD_DEL_OK )
                _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_DELETE);
            plistviewFrm_ctl->plistView->delState = RECORD_DEL_NONE;
            plistviewFrm_ctl->plistView->curPart = RECORD_MUSIC_MENU;

            if( plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PLAY ||
                    plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PAUSE )
                plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_PAUSE;
            plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PAUSE;
            _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_PLAY);
            break;
        }
    }

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_volume_key(__gui_msg_t *msg)
{
    __s32 vol = 0;
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( plistviewFrm_ctl == NULL )
        return EPDK_FAIL;

    if (KEY_UP_ACTION != msg->dwAddData2)
    {
        switch( msg->dwAddData1 )
        {
        case GUI_MSG_KEY_LONGLEFT: //volume down
        case GUI_MSG_KEY_LEFT:
            plistviewFrm_ctl->plistView->timeCnt = 0;
            vol = dsk_volume_get();
            if(vol != 0)
                dsk_volume_set(vol - 1);
            break;
        case GUI_MSG_KEY_LONGRIGHT://volume up
        case GUI_MSG_KEY_RIGHT:
            plistviewFrm_ctl->plistView->timeCnt = 0;
            vol = dsk_volume_get();
            if( vol <= 30 )
                dsk_volume_set(vol + 1);
            break;
        }
    }
    else
    {
        switch( msg->dwAddData1 )
        {
        case GUI_MSG_KEY_MENU:
        case GUI_MSG_KEY_VADD:
            plistviewFrm_ctl->plistView->timeCnt = 0;
            plistviewFrm_ctl->plistView->curPart = plistviewFrm_ctl->plistView->oldPart;
            break;
        }
    }
    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_key(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( plistviewFrm_ctl == NULL )
        return EPDK_FAIL;

    if( plistviewFrm_ctl->plistView->curPart == RECORD_MUSIC_LIST )
    {
        _rec_list_view_frm_on_list_key(msg);
    }
    else if( plistviewFrm_ctl->plistView->curPart == RECORD_MUSIC_DELETE )
    {
        _rec_list_view_frm_on_delete_key(msg);
    }
    else if( plistviewFrm_ctl->plistView->curPart == RECORD_MUSIC_VOLUME )
    {
        _rec_list_view_frm_on_volume_key(msg);
    }
    else
    {
        _rec_list_view_frm_on_menu_key(msg);
    }

    record_list_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);
    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_command(__gui_msg_t *msg)
{
    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_paint(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( plistviewFrm_ctl == NULL )
        return EPDK_FAIL;

    GUI_SetFont(plistviewFrm_ctl->sys_font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(0);

    //record_list_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_back(__gui_msg_t *msg)
{
    //prec_list_view_frame_t plistviewFrm_ctl;
    //plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    _rec_list_view_frm_sendParent(msg, RECORD_TO_VIEW_FRAME);

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_pre(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( plistviewFrm_ctl == NULL )
        return EPDK_FAIL;

    record_list_pre(plistviewFrm_ctl->plistDoc);

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_play(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( plistviewFrm_ctl == NULL )
        return EPDK_FAIL;

    record_list_play(plistviewFrm_ctl->plistDoc);

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_continue(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( plistviewFrm_ctl == NULL )
        return EPDK_FAIL;

    record_list_continue(plistviewFrm_ctl->plistDoc);

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_pause(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( plistviewFrm_ctl == NULL )
        return EPDK_FAIL;

    record_list_pause(plistviewFrm_ctl->plistDoc);

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_next(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( plistviewFrm_ctl == NULL )
        return EPDK_FAIL;

    record_list_next(plistviewFrm_ctl->plistDoc);

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_del(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    if( plistviewFrm_ctl == NULL )
        return EPDK_FAIL;

    if( record_list_delete( plistviewFrm_ctl->plistDoc ) == EPDK_FAIL )
        _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_BACK);

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_manager_proc(__gui_msg_t *msg)
{
    __msg("_rec_list_view_frm_manager_proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
          , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
        return _rec_list_view_frm_on_create(msg);
    case GUI_MSG_CLOSE:
        return _rec_list_view_frm_on_close(msg);
    case GUI_MSG_DESTROY:
        return _rec_list_view_frm_on_destory(msg);
    case GUI_MSG_PAINT:
        return _rec_list_view_frm_on_paint(msg);
    case GUI_MSG_KEY:
        return _rec_list_view_frm_on_key(msg);
    case GUI_MSG_COMMAND:
        return _rec_list_view_frm_on_command(msg);
    case GUI_MSG_TIMER:
    {
        prec_list_view_frame_t plistviewFrm_ctl;
        plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
        if( plistviewFrm_ctl == NULL )
            return EPDK_FAIL;

        if( plistviewFrm_ctl->plistView->curPart == RECORD_MUSIC_VOLUME	)
        {
            plistviewFrm_ctl->plistView->timeCnt++;
            if( plistviewFrm_ctl->plistView->timeCnt > VOLUME_EXIST_TIMES )
            {
                plistviewFrm_ctl->plistView->timeCnt = 0;
                plistviewFrm_ctl->plistView->curPart = plistviewFrm_ctl->plistView->oldPart;
            }
        }

        if( record_list_timer( plistviewFrm_ctl->plistDoc ) == EPDK_FAIL )
        {
            /*if( plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PLAY ||
            	plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PAUSE )
            	plistviewFrm_ctl->plistView->curMenu  = MUSIC_MENU_PLAY;
            plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PLAY;*/
            record_list_playNext( plistviewFrm_ctl->plistDoc );
        }

        record_list_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);
    }
    return EPDK_OK;
    case RECORD_LIST_VIEW_FRAME_BACK:
    {
        _rec_list_view_frm_back(msg);
    }
    return EPDK_OK;
    case RECORD_LIST_VIEW_FRAME_PRE:
    {
        _rec_list_view_frm_pre(msg);
    }
    break;
    case RECORD_LIST_VIEW_FRAME_NEXT:
    {
        _rec_list_view_frm_next(msg);
    }
    break;
    case RECORD_LIST_VIEW_FRAME_PLAY:
    {
        _rec_list_view_frm_play(msg);
    }
    break;
    case RECORD_LIST_VIEW_FRAME_CONTINUE:
    {
        _rec_list_view_frm_continue(msg);
    }
    break;
    case RECORD_LIST_VIEW_FRAME_PAUSE:
    {
        _rec_list_view_frm_pause(msg);
    }
    break;
    case RECORD_LIST_VIEW_FRAME_DELETE:
    {
        _rec_list_view_frm_del(msg);
    }
    break;
    case GUI_MSG_TOUCH:
        break;
    case GUI_MSG_WIN_WAKEUP:
        GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
        return EPDK_OK;
    }
    return GUI_FrmWinDefaultProc(msg);
}

H_WIN rec_list_view_frm_create(H_WIN h_parent, precord_ctrl_t para)
{
    __gui_framewincreate_para_t framewin_para;
    prec_list_view_frame_t		plistviewFrm_ctl;
    FB fb;

    GUI_LyrWinGetFB(para->layer, &fb);

    plistviewFrm_ctl = (prec_list_view_frame_t)esMEMS_Balloc(sizeof(rec_list_view_frame_t));
    if( plistviewFrm_ctl == NULL )
    {
        __msg("esMEMS_Balloc fail\n");
        return NULL;
    }
    eLIBs_memset(plistviewFrm_ctl, 0, sizeof(rec_list_view_frame_t));

    plistviewFrm_ctl->sys_font = para->pfont;
    plistviewFrm_ctl->layer	   = para->layer;
    plistviewFrm_ctl->plistView = &para->rec_listView;

    plistviewFrm_ctl->plistDoc = &para->rec_listDoc;
    plistviewFrm_ctl->plistView->precord_list_doc = &para->rec_listDoc;

    plistviewFrm_ctl->plistDoc->curRootType	= para->root_type;			//get from app_record
    if( record_list_MusicInit( plistviewFrm_ctl->plistDoc ) == EPDK_FAIL )//´íÎó·µ»Ø
    {
        esMEMS_Bfree(plistviewFrm_ctl, sizeof(rec_list_view_frame_t));
        return NULL;
    }

    plistviewFrm_ctl->plistView->curMenu 	= MUSIC_MENU_BACK;
    plistviewFrm_ctl->plistView->curMenuSt 	= FOCUSE_OVER;
    plistviewFrm_ctl->plistView->playMenu 	= MUSIC_MENU_PAUSE;
    plistviewFrm_ctl->plistView->curPart 	= RECORD_MUSIC_LIST;
    plistviewFrm_ctl->plistView->delState   = RECORD_DEL_NONE;
    plistviewFrm_ctl->plistView->timeCnt	= 0;

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name 			= "record_list_view_frm",
                        framewin_para.dwExStyle 	= WS_EX_NONE;
    framewin_para.dwStyle 		= WS_NONE | WS_VISIBLE;
    framewin_para.spCaption 	= NULL;
    framewin_para.hOwner		= NULL;
    framewin_para.id         	= APP_RECORD_LIST_VIEW_ID;
    framewin_para.hHosting 		= h_parent;
    framewin_para.FrameWinProc 	= (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_rec_list_view_frm_manager_proc);
    framewin_para.rect.x 		= 0;
    framewin_para.rect.y 		= 0;
    framewin_para.rect.width	= fb.size.width;
    framewin_para.rect.height 	= fb.size.height;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red 	= 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue 	= 0;
    framewin_para.attr 			= (void *)plistviewFrm_ctl;
    framewin_para.hLayer 		= para->layer;

    GUI_LyrWinSetSta(para->layer, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(para->layer);

    return (GUI_FrmWinCreate(&framewin_para));
}






























