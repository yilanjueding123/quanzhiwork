/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: msgbox.c
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "msgbox.h"
#include "app_home_i.h"

#define MSGBOX_BG_W		322
#define MSGBOX_BG_H		156

static __u8	msgbox_time = 0xff;

typedef struct tag_msgbox_ctrl
{
    main_msgbox_para_t *msgbox_para;

    HTHEME msgbox_bg;
    char msgbox_title[128];
    char msgbox_Content[128];
} msgbox_ctrl_t;

static void msgbox_init(msgbox_ctrl_t *msgbox_ctrl)
{
    __u32 	i;

    __err("not finish yet.....");
    //msgbox_ctrl->msgbox_bg = dsk_theme_open(ID_HOME_BMP_COM_BGINFO1_BMP);

    dsk_langres_get_menu_text(STRING_HOME_CUE, msgbox_ctrl->msgbox_title, GUI_TITLE_MAX);
    dsk_langres_get_menu_text(STRING_HOME_INSERT_DISK, msgbox_ctrl->msgbox_Content, GUI_TITLE_MAX);
}

static __s32 msgbox_paint(__gui_msg_t *msg)
{
    msgbox_ctrl_t *msgbox_ctrl;
    GUI_RECT gui_rect;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    msgbox_ctrl = (msgbox_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    GUI_LyrWinSel(msgbox_ctrl->msgbox_para->layer);
    GUI_SetFont(msgbox_ctrl->msgbox_para->msgbox_font);

    GUI_BMP_Draw(dsk_theme_hdl2buf(msgbox_ctrl->msgbox_bg), 0, 0);

    GUI_OpenAlphaBlend();

    gui_rect.x0 = 0;
    gui_rect.y0 = 0;
    gui_rect.x1 = MSGBOX_BG_W;
    gui_rect.y1 = 40;
    GUI_DispStringInRect(msgbox_ctrl->msgbox_title, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

    gui_rect.x0 = 50;
    gui_rect.y0 = 40;
    gui_rect.x1 = MSGBOX_BG_W;
    gui_rect.y1 = MSGBOX_BG_H ;
    GUI_DispStringInRect(msgbox_ctrl->msgbox_Content, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

    GUI_CloseAlphaBlend();

    return EPDK_OK;
}

/*
	回调
*/
static __s32 _msgbox_proc(__gui_msg_t *msg)
{
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        msgbox_ctrl_t *msgbox_ctrl;
        main_msgbox_para_t *msgbox_para;

        msgbox_ctrl = (msgbox_ctrl_t *)esMEMS_Balloc(sizeof(msgbox_ctrl_t));
        eLIBs_memset(msgbox_ctrl, 0, sizeof(msgbox_ctrl_t));

        msgbox_para = (main_msgbox_para_t *)GUI_WinGetAttr(msg->h_deswin);

        GUI_SetTimer(msg->h_deswin, msgbox_time, 700, 0);

        msgbox_init(msgbox_ctrl);
        msgbox_ctrl->msgbox_para = msgbox_para;
        GUI_WinSetAddData(msg->h_deswin, (__u32)msgbox_ctrl);
    }
    break;
    case GUI_MSG_CLOSE:
    {
        GUI_FrmWinDelete(msg->h_deswin);
    }
    break;
    case GUI_MSG_DESTROY:
    {
        msgbox_ctrl_t *msgbox_ctrl;
        msgbox_ctrl = (msgbox_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

        GUI_KillTimer(msg->h_deswin, msgbox_time);

        dsk_theme_close(msgbox_ctrl->msgbox_bg);

        esMEMS_Bfree(msgbox_ctrl->msgbox_para, sizeof(main_msgbox_para_t));
        esMEMS_Bfree(msgbox_ctrl, sizeof(msgbox_ctrl_t));
    }
    break;
    case GUI_MSG_PAINT:
        msgbox_paint( msg);
        break;
    case GUI_MSG_KEY:
        break;
    case GUI_MSG_TOUCH:
        break;
    case GUI_MSG_TIMER:
        main_cmd2parent(msg->h_deswin, CMD_MSGBOX_CLOSE, 0, 0);
        break;
    default:
        break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

/*
	建立窗口
*/
H_WIN msgbox_win_create(H_WIN h_parent, main_msgbox_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    main_msgbox_para_t *msgbox_para;
    FB fb;

    __err("not finish yet...\n");

    GUI_LyrWinGetFB(para->layer, &fb);

    msgbox_para = (main_msgbox_para_t *)esMEMS_Balloc(sizeof(main_msgbox_para_t));
    msgbox_para->msgbox_font = para->msgbox_font;
    msgbox_para->layer = para->layer;

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));

    framewin_para.name =	"msgbox_win",
                  framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner	= NULL;
    framewin_para.hHosting = h_parent;
    framewin_para.id         = MSGBOX_ID;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_msgbox_proc);
    framewin_para.rect.x = 0;// 0;
    framewin_para.rect.y = 0;//0;
    framewin_para.rect.width =  fb.size.width; //movie_layout.spsc_scnwin.width; //800
    framewin_para.rect.height =  fb.size.height;//movie_layout.spsc_scnwin.height;//300
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr =  (void *)msgbox_para;
    framewin_para.hLayer = para->layer;

    return (GUI_FrmWinCreate(&framewin_para));
}

void  msgbox_win_delete(H_WIN msgbox_win)
{
    GUI_FrmWinDelete(msgbox_win);
}

