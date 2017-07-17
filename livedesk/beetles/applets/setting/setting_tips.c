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

#include "setting_tips.h"
#include "setting_para.h"

#define   UDISK_NAME			"f:\\"
#define   TIPS_YES_NO_BMP_WIDTH 37
#define   TIPS_YES_NO_BMP_HEIGHT 18

typedef struct tag_tips_ctrl
{
    setting_tip_para_t *tips_para;

    __s32 focus_txt_color;
    __s32 unfocus_txt_color;
    H_THEME tip_bmp;
    H_THEME tip_f_bmp;

    char ok_txt[128];		//string ok
    char tsize[128];	//total size
    char fsize[128];	//free size
    char svers[128];	//soft version
    char total_size[128];
    char free_size[128];
    char title_txt[128];
    tips_uipara_t *tips_uipara;
} tips_ctrl_t;

/***********************************************************************************************************
	建立图层
************************************************************************************************************/
H_LYR setting_tips_layer_create(RECT *rect)
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
        "setting tips layer",
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

    lstlyr.scn_win.x		= (480 - SET_TIPS_W) / 2;
    lstlyr.scn_win.y		= (272 - SET_TIPS_H) / 2; //240为屏幕高度，60为底端高度
    lstlyr.scn_win.width  	= rect->width;
    lstlyr.scn_win.height 	= rect->height;

    lstlyr.pipe = 1;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;

    layer = GUI_LyrWinCreate(&lyrcreate_info);
    if( !layer )
    {
        __err("tips layer create error !\n");
    }

    return layer;
}
void setting_tips_get_volume_size(__gui_msg_t *msg)
{
    tips_ctrl_t *tips_ctrl;
    char		str[768];
    char		numstr[32];
    __u64		size;
    __u32		gnum;
    __u32		 snum;

    tips_ctrl = (tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    eLIBs_memset(str, 0, 768);
    eLIBs_strcat(str, tips_ctrl->tsize);
    size = eLIBs_GetVolTSpace(UDISK_NAME);
    //size = 0;
    gnum = (size * 100) >> 30;//先乘100，用于取.GB 后面两位小数
    snum = (gnum % 100);		//去小数点后面的数值
    gnum = (gnum / 100);		//取小数点前面的数值
    eLIBs_uint2str_dec(gnum, numstr);
    eLIBs_strcat(str, numstr);
    eLIBs_strcat(str, ".");
    eLIBs_uint2str_dec(snum, numstr);
    if(snum >= 10)
    {
        eLIBs_strcat(str, numstr);
    }
    else
    {
        eLIBs_strcat(str, "0");
        eLIBs_strcat(str, numstr);
    }
    eLIBs_strcat(str, "GB");
    eLIBs_strcpy(tips_ctrl->total_size, (const char *)str);

    eLIBs_memset(str, 0, 768);
    eLIBs_strcat(str, tips_ctrl->fsize);

    size = eLIBs_GetVolFSpace(UDISK_NAME);
    //size = 0;
    gnum = (size * 100) >> 30;
    snum = (gnum % 100);
    gnum = (gnum / 100);
    eLIBs_uint2str_dec(gnum, numstr);
    eLIBs_strcat(str, numstr);
    eLIBs_strcat(str, ".");
    eLIBs_uint2str_dec(snum, numstr);
    if(snum >= 10)
    {
        eLIBs_strcat(str, numstr);
    }
    else
    {
        eLIBs_strcat(str, "0");
        eLIBs_strcat(str, numstr);
    }
    eLIBs_strcat(str, "GB");
    eLIBs_strcpy(tips_ctrl->free_size, (const char *)str);
}

static void setting_tip_init(tips_ctrl_t *tips_ctrl)
{
    __u32 	i;
    SIZE screen_size;

    dsk_display_get_size(&screen_size.width, &screen_size.height);
    tips_ctrl->tips_uipara = setting_get_tips_para(screen_size);


    if(!tips_ctrl->tip_bmp)
    {
        tips_ctrl->tip_bmp = theme_open(ID_SETTING_SET_INFO_BMP);
    }
    if(!tips_ctrl->tip_f_bmp)
    {
        tips_ctrl->tip_f_bmp = theme_open(ID_SETTING_TIP_F_BMP);
    }

    get_menu_text(STRING_SET_OK, tips_ctrl->ok_txt, 128);
    get_menu_text(STRING_SET_COMMON_PRODUCTINFO_TITLE , tips_ctrl->title_txt , 128);
    get_menu_text(STRING_SET_TIPS_TSIZE, tips_ctrl->tsize, 128);
    get_menu_text(STRING_SET_TIPS_FSIZE, tips_ctrl->fsize, 128);
    get_menu_text(STRING_SET_TIPS_SVERS, tips_ctrl->svers, 128);

}

static __s32 setting_tips_paint_item(__gui_msg_t *msg)
{
    tips_ctrl_t *tips_ctrl;
    GUI_RECT gui_rect;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    tips_ctrl = (tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    GUI_LyrWinSel(tips_ctrl->tips_para->layer);
    GUI_SetFont(tips_ctrl->tips_para->msgbox_font);
    GUI_UC_SetEncodeUTF8();

    GUI_SetColor(tips_ctrl->focus_txt_color);
    GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->tip_bmp), tips_ctrl->tips_uipara->tips_bmp_pos.x, tips_ctrl->tips_uipara->tips_bmp_pos.y);
    GUI_DispStringAt(tips_ctrl->total_size, tips_ctrl->tips_uipara->tip_tsize_pos.x, tips_ctrl->tips_uipara->tip_tsize_pos.y);
    GUI_DispStringAt(tips_ctrl->free_size, tips_ctrl->tips_uipara->tip_fsize_pos.x, tips_ctrl->tips_uipara->tip_fsize_pos.y);

    GUI_SetBkColor(BEETLES_BLACK_COLOR);
    gui_rect.x0 = tips_ctrl->tips_uipara->tip_ok_bmp_pos.x ;
    gui_rect.y0 = tips_ctrl->tips_uipara->tip_ok_bmp_pos.y;
    gui_rect.x1 = gui_rect.x0 + TIPS_YES_NO_BMP_WIDTH - 1;
    gui_rect.y1 = gui_rect.y0 + TIPS_YES_NO_BMP_HEIGHT - 1;
    GUI_ClearRectEx(&gui_rect);
    GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->tip_f_bmp), tips_ctrl->tips_uipara->tip_ok_bmp_pos.x, tips_ctrl->tips_uipara->tip_ok_bmp_pos.y);
    GUI_DispStringInRect(tips_ctrl->ok_txt , &gui_rect , GUI_TA_HCENTER | GUI_TA_VCENTER);

    gui_rect.x0 = tips_ctrl->tips_uipara->tips_tiltle_pos.x ;
    gui_rect.y0 = tips_ctrl->tips_uipara->tips_tiltle_pos.y ;
    gui_rect.x1 = gui_rect.x0 + tips_ctrl->tips_uipara->tips_tiltle_pos.width - 1;
    gui_rect.y1 = gui_rect.y0 + tips_ctrl->tips_uipara->tips_tiltle_pos.height - 1 ;
    GUI_DispStringInRect(tips_ctrl->title_txt , &gui_rect , GUI_TA_HCENTER | GUI_TA_VCENTER);

    return EPDK_OK;
}

static __s32 setting_tips_paint_all(__gui_msg_t *msg)
{
    tips_ctrl_t *tips_ctrl;
    GUI_RECT gui_rect;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    tips_ctrl = (tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    if( !tips_ctrl )
    {
        __msg( "tips_ctrl is null \n ");
        return EPDK_FAIL ;
    }
    GUI_LyrWinSel(tips_ctrl->tips_para->layer);
    GUI_SetFont(tips_ctrl->tips_para->msgbox_font);
    GUI_UC_SetEncodeUTF8();

    setting_tips_get_volume_size( msg);

    GUI_SetColor(tips_ctrl->focus_txt_color);
    GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->tip_bmp), tips_ctrl->tips_uipara->tips_bmp_pos.x, tips_ctrl->tips_uipara->tips_bmp_pos.y);
    GUI_DispStringAt(tips_ctrl->total_size, tips_ctrl->tips_uipara->tip_tsize_pos.x, tips_ctrl->tips_uipara->tip_tsize_pos.y);
    GUI_DispStringAt(tips_ctrl->free_size, tips_ctrl->tips_uipara->tip_fsize_pos.x, tips_ctrl->tips_uipara->tip_fsize_pos.y);

#if 1
    GUI_SetBkColor(BEETLES_BLACK_COLOR);
    gui_rect.x0 = tips_ctrl->tips_uipara->tip_ok_bmp_pos.x ;
    gui_rect.y0 = tips_ctrl->tips_uipara->tip_ok_bmp_pos.y;
    gui_rect.x1 = gui_rect.x0 + TIPS_YES_NO_BMP_WIDTH - 1;
    gui_rect.y1 = gui_rect.y0 + TIPS_YES_NO_BMP_HEIGHT - 1;
    GUI_ClearRectEx(&gui_rect);
    GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->tip_f_bmp), tips_ctrl->tips_uipara->tip_ok_bmp_pos.x, tips_ctrl->tips_uipara->tip_ok_bmp_pos.y);
    GUI_DispStringInRect(tips_ctrl->ok_txt , &gui_rect , GUI_TA_HCENTER | GUI_TA_VCENTER);

    gui_rect.x0 = tips_ctrl->tips_uipara->tips_tiltle_pos.x ;
    gui_rect.y0 = tips_ctrl->tips_uipara->tips_tiltle_pos.y ;
    gui_rect.x1 = gui_rect.x0 + tips_ctrl->tips_uipara->tips_tiltle_pos.width - 1;
    gui_rect.y1 = gui_rect.y0 + tips_ctrl->tips_uipara->tips_tiltle_pos.height - 1 ;
    GUI_DispStringInRect(tips_ctrl->title_txt , &gui_rect , GUI_TA_HCENTER | GUI_TA_VCENTER);
#endif
    return EPDK_OK;
}

static __s32  setting_tips_key_proc(__gui_msg_t *msg)
{
    static __s32 last_key = -1;
    tips_ctrl_t *tips_ctrl;

    tips_ctrl = (tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        if (GUI_MSG_KEY_ENTER == last_key)
        {
            main_cmd2parent(msg->h_deswin, ID_OP_ENTER, 0, 0);
        }
    }
    else
    {
        switch( msg->dwAddData1 )
        {

        case GUI_MSG_KEY_LEFT:		//映射为up
        case GUI_MSG_KEY_LONGLEFT:
        {
            main_cmd2parent(msg->h_deswin, ID_OP_UP, 0, 0);
        }
        break;
        case GUI_MSG_KEY_RIGHT: 	//映射为down
        case GUI_MSG_KEY_LONGRIGHT:
        {
            main_cmd2parent(msg->h_deswin, ID_OP_DOWN, 0, 0);
        }
        break;
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LONGUP:
            main_cmd2parent(msg->h_deswin, ID_OP_UP, 0, 0);
            break;
        case GUI_MSG_KEY_DOWN:
        case GUI_MSG_KEY_LONGDOWN:
            main_cmd2parent(msg->h_deswin, ID_OP_DOWN, 0, 0);
            break;
        case GUI_MSG_KEY_ESCAPE:
        case GUI_MSG_KEY_MENU:
            main_cmd2parent(msg->h_deswin, ID_OP_EXIT, 0, 0);
            break;
        default:
            break;

        }
    }

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        last_key = -1;
    }
    else
    {
        last_key = msg->dwAddData1;
    }

    return EPDK_OK;
}

/*
	回调
*/
static __s32 _setting_tips_proc(__gui_msg_t *msg)
{
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        tips_ctrl_t *tips_ctrl;
        setting_tip_para_t *tips_para;

        tips_ctrl = (tips_ctrl_t *)My_Balloc(sizeof(tips_ctrl_t));
        if(!tips_ctrl)
        {

            __err("memory balloc fail........\n");
            return EPDK_FAIL;
        }
        eLIBs_memset(tips_ctrl, 0, sizeof(tips_ctrl_t));

        tips_para = (setting_tip_para_t *)GUI_WinGetAttr(msg->h_deswin);

        tips_ctrl->focus_txt_color = APP_COLOR_YELLOW;
        tips_ctrl->unfocus_txt_color = APP_COLOR_WHITE;
        tips_ctrl->tips_para = tips_para;
        GUI_WinSetAddData(msg->h_deswin, (__u32)tips_ctrl);
        setting_tip_init(tips_ctrl);

        GUI_LyrWinCacheOn();
        GUI_LyrWinSetSta(tips_ctrl->tips_para->layer, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSetTop(tips_ctrl->tips_para->layer);
        GUI_LyrWinCacheOff();
    }
    return EPDK_OK;
    case DSK_MSG_FS_PART_PLUGIN:
    case DSK_MSG_FS_PART_PLUGOUT:
    {

        setting_tips_get_volume_size(msg);
        setting_tips_paint_item(msg);	//重新获取磁盘的容量
        __here__;
    }
    return EPDK_OK;
    case GUI_MSG_CLOSE:
    {
        tips_ctrl_t *tips_ctrl;

        tips_ctrl = (tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        if( !tips_ctrl )
        {
            __msg( "tips_ctrl is null \n");
            return EPDK_FAIL ;
        }

        main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_GENERAL, 0, 0);
    }
    return EPDK_OK;
    case GUI_MSG_DESTROY:
    {
        tips_ctrl_t *tips_ctrl = NULL ;
        tips_ctrl = (tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        if(!tips_ctrl)
        {

            __err("tips_ctrl is null........\n");
            return EPDK_FAIL;
        }
        __msg("tips win destroy\n");

        __msg("tips win close theme\n");
        theme_close(tips_ctrl->tip_bmp);
        theme_close(tips_ctrl->tip_f_bmp);

        if(tips_ctrl->tips_para)
        {
            __msg("tips win free tips_para.....\n");
            My_Bfree(tips_ctrl->tips_para, sizeof(setting_tip_para_t));
            tips_ctrl->tips_para = NULL ;
        }
        if(tips_ctrl)
        {
            __msg("tips win free tips_ctrl.....\n");
            My_Bfree(tips_ctrl, sizeof(tips_ctrl_t));
            tips_ctrl = NULL ;
        }

    }
    return EPDK_OK;
    case GUI_MSG_PAINT:
        setting_tips_paint_all( msg);
        return EPDK_OK;
    case GUI_MSG_KEY:
        setting_tips_key_proc(msg);
        return EPDK_OK;
    case GUI_MSG_TOUCH:
        break;
    case GUI_MSG_TIMER:
        break;
    case MSG_OP_UP:
    {

    }
    return EPDK_OK;
    case MSG_OP_DOWN:
    {

    }
    return EPDK_OK;
    case MSG_OP_LEFT:
    {

    }
    return EPDK_OK;
    case MSG_OP_RIGHT:
    {
    }
    return EPDK_OK;

    case MSG_OP_ENTER:
    {
        tips_ctrl_t *tips_ctrl;
        tips_ctrl = (tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

        {
            __gui_msg_t mymsg;

            mymsg.id = GUI_MSG_CLOSE;
            mymsg.h_deswin = msg->h_deswin;
            mymsg.h_srcwin = msg->h_srcwin;
            mymsg.dwAddData1 = mymsg.dwAddData1;
            mymsg.dwAddData2 = mymsg.dwAddData2;
            GUI_SendMessage(&mymsg);
        }
    }
    return EPDK_OK;

    case MSG_OP_EXIT:
    {

        __gui_msg_t mymsg;
        mymsg.id = GUI_MSG_CLOSE;
        mymsg.h_deswin = msg->h_deswin;
        mymsg.h_srcwin = msg->h_srcwin;
        mymsg.dwAddData1 = msg->dwAddData1;
        mymsg.dwAddData2 = msg->dwAddData2;
        GUI_SendMessage(&mymsg);
    }
    return EPDK_OK;
    default:
        break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

/*
	建立窗口
*/
H_WIN setting_tips_win_create(H_WIN h_parent, setting_tip_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    setting_tip_para_t *tips_para;
    FB fb;

    GUI_LyrWinGetFB(para->layer, &fb);

    tips_para = (setting_tip_para_t *)My_Balloc(sizeof(setting_tip_para_t));
    if(!tips_para)
    {

        __err("memory balloc fail........\n");
        return EPDK_FAIL;
    }
    tips_para->msgbox_font = para->msgbox_font;
    tips_para->layer = para->layer;

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));

    framewin_para.name =	"tips_win";
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner	= NULL;
    framewin_para.hHosting = h_parent;
    framewin_para.id         = GENERAL_TIPS_ID;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_setting_tips_proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width =  fb.size.width;
    framewin_para.rect.height =  fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr =  (void *)tips_para;
    framewin_para.hLayer = para->layer;

    return (GUI_FrmWinCreate(&framewin_para));
}

void  setting_tips_win_delete(H_WIN tips_win)
{
    GUI_FrmWinDelete(tips_win);
}

