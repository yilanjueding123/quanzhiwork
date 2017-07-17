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

#include "ebook_loading.h"
#include "ebook_uipara.h"
#include "app_ebook_i.h"

#define LOADING_BMP_NUM	10

typedef struct tag_loading_ctrl
{
    ebook_loading_para_t *loading_para;
    HTHEME h_bmp_loading;
    __s32 focus_txt_color;
    char  loading_tips_text[128];
    __u8  loading_bmp_cnt;
    __u8  loading_task;
    __u8  loading_txt_init_flag;
} ebook_loading_ctrl_t;

static __s32 loading_bmp_id[LOADING_BMP_NUM] =
{
    ID_EBOOK_BUFFER1_BMP,
    ID_EBOOK_BUFFER2_BMP,
    ID_EBOOK_BUFFER3_BMP,
    ID_EBOOK_BUFFER4_BMP,
    ID_EBOOK_BUFFER5_BMP,
    ID_EBOOK_BUFFER6_BMP,
    ID_EBOOK_BUFFER7_BMP,
    ID_EBOOK_BUFFER8_BMP,
    ID_EBOOK_BUFFER9_BMP,
    ID_EBOOK_BUFFER10_BMP,
};

static __s32 loading_paint(H_WIN h_win)
{
    ebook_loading_ctrl_t *loading_ctrl;
    GUI_RECT gui_rect;
    ebook_uipara_t *ebook_uipara;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(h_win)) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    loading_ctrl = (ebook_loading_ctrl_t *)GUI_WinGetAddData(h_win);
    ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();

    GUI_LyrWinSel(loading_ctrl->loading_para->loading_layer);
    GUI_SetFont(loading_ctrl->loading_para->font);
    GUI_UC_SetEncodeUTF8();

    if(loading_ctrl->h_bmp_loading)
    {

        dsk_theme_close(loading_ctrl->h_bmp_loading);
        if(loading_ctrl->loading_bmp_cnt >= LOADING_BMP_NUM)
            loading_ctrl->loading_bmp_cnt = 0;
        loading_ctrl->h_bmp_loading = dsk_theme_open(loading_bmp_id[loading_ctrl->loading_bmp_cnt]);
        loading_ctrl->loading_bmp_cnt++;
    }
    else
    {
        loading_ctrl->h_bmp_loading = dsk_theme_open(loading_bmp_id[loading_ctrl->loading_bmp_cnt]);
    }
    GUI_BMP_Draw(theme_hdl2buf(loading_ctrl->h_bmp_loading), ebook_uipara->loading_bmp_pos.x, ebook_uipara->loading_bmp_pos.y);


    if(loading_ctrl->loading_txt_init_flag == 0)
    {
        GUI_SetColor(APP_COLOR_YELLOW);
        get_menu_text(STRING_EBOOK_LOADING, loading_ctrl->loading_tips_text, 128);
        gui_rect.x0 = ebook_uipara->loading_txt_pos.x;
        gui_rect.y0 = ebook_uipara->loading_txt_pos.y;
        gui_rect.x1 = gui_rect.x0 + ebook_uipara->loading_txt_pos.w;
        gui_rect.y1 = gui_rect.y0 + ebook_uipara->loading_txt_pos.h;
        GUI_DispStringInRectWrap(loading_ctrl->loading_tips_text, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER, GUI_WRAPMODE_WORD);
        loading_ctrl->loading_txt_init_flag = 1;
    }

    return EPDK_OK;
}

static void __show_task(void *p_arg)
{
    H_WIN		hwin;
    hwin = (H_WIN)p_arg;

    while(1)
    {
        if(esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            goto EXIT_DEC_TASK;
        }
        loading_paint(hwin);
        esKRNL_TimeDly(10);
    }

EXIT_DEC_TASK:
    esKRNL_TDel(EXEC_prioself);

    return;
}

static __s32 _ebook_loading_proc(__gui_msg_t *msg)
{
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        ebook_loading_ctrl_t *loading_ctrl;
        ebook_loading_para_t *loading_para;

        loading_ctrl = (ebook_loading_ctrl_t *)My_Balloc(sizeof(ebook_loading_ctrl_t));
        if(!loading_ctrl)
        {

            __msg("memory balloc fail.........\n");
            return EPDK_FAIL;
        }
        eLIBs_memset(loading_ctrl, 0, sizeof(ebook_loading_ctrl_t));

        loading_para = (ebook_loading_para_t *)GUI_WinGetAttr(msg->h_deswin);

        loading_ctrl->loading_para = loading_para;
        GUI_WinSetAddData(msg->h_deswin, (__u32)loading_ctrl);

        GUI_LyrWinCacheOn();
        GUI_LyrWinSetSta(loading_ctrl->loading_para->loading_layer, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSetTop(loading_ctrl->loading_para->loading_layer);
        GUI_LyrWinCacheOff();

        loading_ctrl->loading_task = esKRNL_TCreate(__show_task, (void *)(msg->h_deswin), 0x1000, KRNL_priolevel3);
    }
    break;
    case GUI_MSG_CLOSE:
        break;
    case GUI_MSG_DESTROY:
    {
        ebook_loading_ctrl_t *loading_ctrl;
        loading_ctrl = (ebook_loading_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        if(!loading_ctrl)
        {

            __err("loading_ctrl is null.......\n");
            return EPDK_FAIL;
        }
        if(loading_ctrl->h_bmp_loading)
        {
            dsk_theme_close(loading_ctrl->h_bmp_loading);
            loading_ctrl->h_bmp_loading = 0;
        }

        if(loading_ctrl->loading_task != 0)
        {
            while( esKRNL_TDelReq(loading_ctrl->loading_task) != OS_TASK_NOT_EXIST )
                esKRNL_TimeDly(5);
            loading_ctrl->loading_task = 0;
        }

        if(loading_ctrl->loading_para)
        {
            My_Bfree(loading_ctrl->loading_para, sizeof(ebook_loading_para_t));
        }

        if(loading_ctrl)
        {
            My_Bfree(loading_ctrl, sizeof(ebook_loading_ctrl_t));
        }
    }
    break;
    case GUI_MSG_PAINT:
        break;
    case GUI_MSG_KEY:
        break;
    case GUI_MSG_TOUCH:
        break;
    case GUI_MSG_TIMER:
        break;
    default:
        break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

/*
	½¨Á¢´°¿Ú
*/
H_WIN ebook_loading_win_create(H_WIN h_parent, ebook_loading_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    ebook_loading_para_t *loading_para;
    FB fb;

    GUI_LyrWinGetFB(para->loading_layer, &fb);
    loading_para = (ebook_loading_para_t *)My_Balloc(sizeof(ebook_loading_para_t));
    if(!loading_para)
    {

        __msg("memory balloc fail.........\n");
        return EPDK_FAIL;
    }
    loading_para->font = para->font;
    loading_para->loading_layer = para->loading_layer;

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));

    framewin_para.name =	"ebook_loading_win";
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner	= NULL;
    framewin_para.hHosting = h_parent;
    framewin_para.id         = EBOOK_LOADING_ID;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_ebook_loading_proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width =  fb.size.width;
    framewin_para.rect.height =  fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr =  (void *)loading_para;
    framewin_para.hLayer = para->loading_layer;

    return (GUI_FrmWinCreate(&framewin_para));
}

void  ebook_loading_win_delete(H_WIN loading_win)
{
    GUI_FrmWinDelete(loading_win);
}

