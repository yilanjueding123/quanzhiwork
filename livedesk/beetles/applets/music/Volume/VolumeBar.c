/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
* File    	: 	VolumeBar
*
* By      	: 	Libaiao
* Func	:
* Version	: 	v1.0
* ============================================================================================================
* 2010-5-25 15:24:52  Libaiao  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "apps.h"
#include "beetles_app.h"
#include "music_player.h"
#include "VolumeBar.h"

//
static H_LYR VolumeBar32bppLayerCreate(RECT *rect, __s32 pipe)
{
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                   		/* size      */
        {0, 0, 0},                                      /* buffer    */
        //{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
        {FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},    /* fmt       */

    };

    __disp_layer_para_t lstlyr =
    {
        //DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
        DISP_LAYER_WORK_MODE_NORMAL,
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
        "APP_MUSIC",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };

    fb.size.width		= rect->width;
    fb.size.height		= rect->height;
    fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888; //PIXEL_MONO_8BPP; //PIXEL_COLOR_ARGB8888;

    lstlyr.src_win.x  		= 0;
    lstlyr.src_win.y  		= 0;
    lstlyr.src_win.width 	= rect->width;
    lstlyr.src_win.height   = rect->height;

    lstlyr.scn_win.x		= rect->x;
    lstlyr.scn_win.y		= rect->y;
    lstlyr.scn_win.width  = rect->width;
    lstlyr.scn_win.height = rect->height;

    lstlyr.pipe = pipe;		//1
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;

    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if( !layer )
    {
        __err("app music layer create error !\n");
    }
    return layer;
}


static __s32 VolumeBarProc(__gui_msg_t *msg)
{

}



__s32 VolumeBarOpenPicture(VolumeBar_t *This)
{

    if(This->Para.UI.BG_bmp_id != 0)
    {
        This->BG_Handle = theme_open(This->Para.UI.BG_bmp_id);
        if(This->BG_Handle != NULL)
        {
            This->BG_bmp = (void *)theme_hdl2buf(This->BG_Handle);
            if(This->BG_bmp == NULL)
            {
                __wrn("open background picture for vloume bar failed\n");
            }
        }
        else
        {
            __wrn("open background picture for vloume bar failed\n");
        }
    }

    if(This->Para.UI.LS_bmp_id != 0)
    {
        This->LS_Handle = theme_open(This->Para.UI.LS_bmp_id);
        if(This->LS_Handle != NULL)
        {
            This->LS_bmp = theme_hdl2buf(This->LS_Handle);
            if(This->LS_bmp == NULL)
            {
                __wrn("open body picture for vloume bar failed\n");
            }
        }
        else
        {
            __wrn("open body picture for vloume bar failed\n");
        }
    }

    if(This->Para.UI.BB_bmp_id != 0)
    {
        This->BB_Handle = theme_open(This->Para.UI.BB_bmp_id);
        if(This->BB_Handle != NULL)
        {
            This->BB_bmp = theme_hdl2buf(This->BB_Handle);
            if(This->BB_bmp == NULL)
            {
                __wrn("open body picture for vloume bar failed\n");
            }
        }
        else
        {
            __wrn("open body picture for vloume bar failed\n");
        }
    }

    if(This->Para.UI.PO_bmp_id != 0)
    {
        This->PO_Handle = theme_open(This->Para.UI.PO_bmp_id);
        if(This->PO_Handle != NULL)
        {
            This->PO_bmp = theme_hdl2buf(This->PO_Handle);
            if(This->PO_bmp == NULL)
            {
                __wrn("open point picture for vloume bar failed\n");
            }
        }
        else
        {
            __wrn("open point picture for vloume bar failed\n");
        }
    }

    if(This->Para.UI.RS_bmp_id != 0)
    {
        This->RS_Handle = theme_open(This->Para.UI.RS_bmp_id);
        if(This->RS_Handle != NULL)
        {
            This->RS_bmp = theme_hdl2buf(This->RS_Handle);
            if(This->RS_bmp == NULL)
            {
                __wrn("open right speaker icon for vloume bar failed\n");
            }
        }
        else
        {
            __wrn("open right speaker icon for vloume bar failed\n");
        }
    }

    if(This->Para.UI.MS_bmp_id != 0)
    {
        This->MS_Handle = theme_open(This->Para.UI.MS_bmp_id);
        if(This->MS_Handle != NULL)
        {
            This->MS_bmp = theme_hdl2buf(This->MS_Handle);
            if(This->MS_bmp == NULL)
            {
                __wrn("open mute speaker icon for vloume bar failed\n");
            }
        }
        else
        {
            __wrn("open mute speaker icon for vloume bar failed\n");
        }
    }

    return EPDK_OK;
}

__s32 VolumeBarClosePicture(VolumeBar_t *This)
{
    if(This->BG_Handle != NULL)
    {
        theme_close(This->BG_Handle);
        This->BG_Handle = NULL;
        This->BB_bmp = NULL;
    }
    if(This->LS_Handle != NULL)
    {
        theme_close(This->LS_Handle);
        This->LS_Handle = NULL;
        This->LS_bmp = NULL;
    }
    if(This->BB_Handle != NULL)
    {
        theme_close(This->BB_Handle);
        This->BB_Handle = NULL;
        This->BB_bmp = NULL;
    }
    if(This->PO_Handle != NULL)
    {
        theme_close(This->PO_Handle);
        This->PO_Handle = NULL;
        This->PO_bmp = NULL;
    }
    if(This->RS_Handle != NULL)
    {
        theme_close(This->RS_Handle);
        This->RS_Handle = NULL;
        This->RS_bmp = NULL;
    }
    if(This->MS_Handle != NULL)
    {
        theme_close(This->MS_Handle);
        This->MS_Handle = NULL;
        This->MS_bmp = NULL;
    }
    return EPDK_OK;
}

__s32 DrawVolumeBarBackground(VolumeBar_t *This)
{
    __s32 X = 0, Y = 0;
    if(This == NULL)
    {
        return EPDK_FAIL;
    }
    //GUI_LyrWinSel(This->VB_Layer);

    X = 0;
    Y = 0;
    if(This->BG_bmp != NULL)
    {
        GUI_BMP_Draw(This->BG_bmp, X, Y);
    }
    return EPDK_OK;
}
__s32 DrawVolumeBarLeftSpeaker(VolumeBar_t *This)
{
    __s32 X = 0, Y = 0;

    if(This == NULL)
    {
        return EPDK_FAIL;
    }

    X = This->Para.UI.LeftSpeaker.x;
    Y = This->Para.UI.LeftSpeaker.y;

    if(This->LS_bmp != NULL)
    {
        GUI_BMP_Draw(This->LS_bmp, X, Y);
    }
    return EPDK_OK;
}

__s32 DrawVolumeBarRightSpeaker(VolumeBar_t *This)
{
    __s32 X = 0, Y = 0;
    if(This == NULL)
    {
        return EPDK_FAIL;
    }

    X = This->Para.UI.RightSpeaker.x;
    Y = This->Para.UI.RightSpeaker.y;

    if(This->RS_bmp != NULL)
    {
        //GUI_LyrWinSel(This->VB_Layer);
        GUI_BMP_Draw(This->RS_bmp, X, Y);
    }
    return EPDK_OK;
}

__s32 DrawVolumeBarBody(VolumeBar_t *This)
{
    __s32 X = 0, Y = 0;
    if(This == NULL)
    {
        return EPDK_FAIL;
    }

    X = This->Para.UI.BarBody.x;
    Y = This->Para.UI.BarBody.y;

    if(This->BB_bmp != NULL)
    {
        GUI_BMP_Draw(This->BB_bmp, X, Y);
    }
    return EPDK_OK;
}

__s32 DrawVolumeBarPoint(void *pic_buf, __s32 X, __s32 Y)
{
    if(pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }
    return EPDK_OK;
}

__s32 DrawVolumeBarMuteSpeaker(VolumeBar_t *This)
{
    __s32 X = 0, Y = 0;
    if(This == NULL)
    {
        return EPDK_FAIL;
    }

    X = This->Para.UI.LeftSpeaker.x;
    Y = This->Para.UI.LeftSpeaker.y;

    if(This->MS_bmp != NULL)
    {
        GUI_BMP_Draw(This->MS_bmp, X, Y);			//两边都画静音icon
    }

    X = This->Para.UI.RightSpeaker.x;
    Y = This->Para.UI.RightSpeaker.y;
    if(This->MS_bmp != NULL)
    {
        GUI_BMP_Draw(This->MS_bmp, X, Y);
    }
    return EPDK_OK;
}

__s32 DrawVolumeBarVolNumber(VolumeBar_t *This, __s32 vol_value)
{
    __s32 X = 0, Y = 0;
    GUI_RECT vol_rect;
    char vol_text[3];

    if(This == NULL)
    {
        return EPDK_FAIL;
    }

    eLIBs_memset(vol_text, 0, sizeof(vol_text));

    if(vol_value < 100)
    {
        uint2str(vol_value, vol_text);
    }
    //else
    {
        //uint2str(0, vol_text);
    }

    //GUI_LyrWinSel(This->VB_Layer);

    vol_rect.x0 = This->Para.UI.VolNumber.x;
    vol_rect.y0 = This->Para.UI.VolNumber.y;
    vol_rect.x1 = vol_rect.x0 + This->Para.UI.VolNumber.width;
    vol_rect.y1 = vol_rect.y0 + This->Para.UI.VolNumber.height;
    GUI_ClearRect(vol_rect.x0, vol_rect.y0, vol_rect.x1, vol_rect.y1);

    GUI_DispStringInRect(vol_text, &vol_rect, GUI_TA_VCENTER);

    return EPDK_OK;
}


//根据音量值显示process bar
__s32 DrawVolumeBarByValue(VolumeBar_t *This, __s32 vol_value)
{
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    __s32 BodyWidth = 0, BodyHeight = 0, PointWidth = 0, PointHeight = 0;
    __u32 total_pieces = 0;
    __u32 average = 0;
    __u32 point = 0, i = 0;

    if(This == NULL)
    {
        return EPDK_FAIL;
    }
    DrawVolumeBarBody(This);

    if(This->Para.CtrlVar.vol_max <= 0)
    {

        return EPDK_OK;
    }
    if(vol_value > This->Para.CtrlVar.vol_max)
    {
        __msg("volume is %d\n", vol_value);
        vol_value = This->Para.CtrlVar.vol_max;
    }

    if(This->PO_bmp == NULL)
    {
        return EPDK_FAIL;
    }


    switch(This->Para.CtrlVar.ViewAlign)
    {
    case GUI_TA_HORIZONTAL:
        BodyWidth = This->Para.UI.BarBody.width;
        PointWidth = This->Para.UI.Point.width;
        total_pieces = BodyWidth / PointWidth;
        if(This->Para.CtrlVar.vol_max >= total_pieces)
        {
            average = This->Para.CtrlVar.vol_max / total_pieces;		//获得平均每份代表的值
            point = vol_value / average;								//获得当前显示的进度点
        }
        else
        {
            point = vol_value * total_pieces / This->Para.CtrlVar.vol_max;
        }
        X = This->Para.UI.BarBody.x;
        Y = This->Para.UI.BarBody.y;
        for(i = 0; i < point; i++)
        {
            GUI_BMP_Draw(This->PO_bmp, X, Y);
            X += PointWidth;
        }
        break;
    case GUI_TA_VERTICAL:
        BodyHeight = This->Para.UI.BarBody.width;
        PointHeight = This->Para.UI.Point.height;
        total_pieces = BodyHeight / PointHeight;

        if(This->Para.CtrlVar.vol_max >= total_pieces)
        {
            average = This->Para.CtrlVar.vol_max / total_pieces;		//获得平均每份代表的值
            point = vol_value / average;								//获得当前显示的进度点
        }
        else
        {
            point = vol_value * total_pieces / This->Para.CtrlVar.vol_max;
        }
        X = This->Para.UI.BarBody.x;
        Y = This->Para.UI.BarBody.y;
        for(i = 0; i < point; i++)
        {
            GUI_BMP_Draw(This->PO_bmp, X, Y);
            Y += PointHeight;
        }
        break;
    default:
        break;
    }
    return EPDK_OK;
}
extern GUI_COLOR GUI_GetBkColor(void);
__s32 DrawVolumeBar(VolumeBar_t *This, __s32 vol_value)
{

    __u32 Color = GUI_BLACK;
    //__u32 DrawMode = GUI_DRAWMODE_NORMAL;

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    //Color = GUI_GetBkColor();
    Color = 0x99000000;
    GUI_SetBkColor(0x99000000);
    GUI_SetColor(GUI_WHITE);

    DrawVolumeBarBackground(This);

    //GUI_OpenAlphaBlend();
    {

        DrawVolumeBarVolNumber(This, vol_value);
        //GUI_OpenAlphaBlend();
        DrawVolumeBarRightSpeaker(This);
        DrawVolumeBarRightSpeaker(This);
        //GUI_CloseAlphaBlend();
        DrawVolumeBarLeftSpeaker(This);
    }

    if(vol_value == 0)
    {
        DrawVolumeBarMuteSpeaker(This);
        DrawVolumeBarBody(This);
    }
    else
    {
        DrawVolumeBarByValue(This, vol_value);
    }
    //GUI_CloseAlphaBlend();
}

__s32 VolumeBarOnAdd(VolumeBar_t *This)
{
    __s32 vol = 0;

    vol = dsk_volume_get();
    __msg("volume is %d\n", vol);
    dsk_volume_inc();
    vol = dsk_volume_get();
    __msg("volume is %d\n", vol);


    GUI_LyrWinSetTop(This->VB_Layer);
    GUI_LyrWinSel(This->VB_Layer);

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(0x99000000);
    GUI_SetColor(GUI_WHITE);

    DrawVolumeBarVolNumber(This, vol);
    if(vol == 1)
    {
        DrawVolumeBarLeftSpeaker(This);
        DrawVolumeBarRightSpeaker(This);
    }
    if(vol == 0)
    {
        DrawVolumeBarMuteSpeaker(This);
        DrawVolumeBarBody(This);
    }
    DrawVolumeBarByValue(This, vol);

    if(This->Para.Event.OnVolAdd != NULL)
    {
        if(This->Para.CtrlVar.UserLayer != NULL)
        {
            GUI_LyrWinSel(This->Para.CtrlVar.UserLayer);
        }
        This->Para.Event.OnVolAdd(This->Para.CtrlVar.attr);
    }
    return EPDK_OK;
}

__s32 VolumeBarOnDec(VolumeBar_t *This)
{
    __s32 vol = 0;
    dsk_volume_dec();
    vol = dsk_volume_get();
    GUI_LyrWinSetTop(This->VB_Layer);
    GUI_LyrWinSel(This->VB_Layer);

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(0x99000000);
    GUI_SetColor(GUI_WHITE);

    DrawVolumeBarVolNumber(This, vol);
    if(vol == 1)
    {
        DrawVolumeBarLeftSpeaker(This);
        DrawVolumeBarRightSpeaker(This);
    }
    if(vol == 0)
    {
        DrawVolumeBarMuteSpeaker(This);
        DrawVolumeBarBody(This);
    }

    DrawVolumeBarByValue(This, vol);
    if(This->Para.Event.OnVolDec != NULL)
    {
        if(This->Para.CtrlVar.UserLayer != NULL)
        {
            GUI_LyrWinSel(This->Para.CtrlVar.UserLayer);
        }
        This->Para.Event.OnVolDec(This->Para.CtrlVar.attr);
    }
    return EPDK_OK;
}


__s32 VolumeBarOnKeyProc(__gui_msg_t *msg)
{
    VolumeBar_t *This = NULL;
    This = GUI_WinGetAttr(msg->h_deswin);

    GUI_ResetTimer(This->H_Framewin, This->TimerID, This->Para.CtrlVar.AutoExitTime * C_VB_AUTO_EXIT_TIME, NULL);

    if((KEY_DOWN_ACTION == msg->dwAddData2) || (KEY_REPEAT_ACTION == msg->dwAddData2))
    {
        if((msg->dwAddData1 == This->Para.Event.VolAddKey)
                || (msg->dwAddData1 == This->Para.Event.VolAddRepeatKey))
        {
            VolumeBarOnAdd(This);			//音量加
        }
        if((msg->dwAddData1 == This->Para.Event.VolDecKey)
                || (msg->dwAddData1 == This->Para.Event.VolDecRepeatKey))
        {
            VolumeBarOnDec(This);			//音量减
        }
        if(msg->dwAddData1 == This->Para.Event.VolExitKey)
        {
            //VolumeBarDelete(This);			//退出
        }
    }
    else if(KEY_UP_ACTION == msg->dwAddData2)
    {
        if(msg->dwAddData1 == This->Para.Event.VolExitKey)
        {
            VolumeBarDelete(This);			//退出
        }
    }

    return EPDK_OK;
}

__s32 VolumeBarOnTimer(__gui_msg_t *msg)
{
    VolumeBar_t *This = NULL;
    This = GUI_WinGetAttr(msg->h_deswin);
    VolumeBarDelete(This);
    return EPDK_OK;
}
__s32 VolumeBarOnPaint(__gui_msg_t *msg)
{
    VolumeBar_t *This = NULL;
    This = GUI_WinGetAttr(msg->h_deswin);
    if (GUI_LyrWinGetSta(This->VB_Layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }
    VolumeBarShow(This);
    return EPDK_OK;
}
__s32 VolumeBarOnCreate(__gui_msg_t *msg)
{
    VolumeBar_t *This = NULL;
    This = GUI_WinGetAttr(msg->h_deswin);
    This->H_Framewin = msg->h_deswin;

    VolumeBarOpenPicture(This);

    VolumeBarTimerCreate(This);

    //app_print_memory_usage("music volume bar create finished", NULL);
    return EPDK_OK;
}

__s32 VolumeBarOnClose(__gui_msg_t *msg)
{

    return GUI_FrmWinDelete(msg->h_deswin);			//send destroy message
}

__s32 VolumeBarOnDestroy(__gui_msg_t *msg)
{
    VolumeBar_t *this = NULL;
    //app_print_memory_usage("delete music volume bar ", NULL);

    this = GUI_WinGetAttr(msg->h_deswin);
    if(this != NULL)
    {
        VolumeBarTimerDelete(this);
        VolumeBarClosePicture(this);
        esMEMS_Bfree(this, sizeof(VolumeBar_t));
    }
    //app_print_memory_usage("delete music volume bar finished", NULL);
    return EPDK_OK;
}


__s32 VolumeBarCB(__gui_msg_t *msg)
{
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        VolumeBarOnCreate(msg);
        break;
    }
    case GUI_MSG_PAINT:
    {
        VolumeBarOnPaint(msg);
        break;
    }
    case GUI_MSG_KEY:
    {
        VolumeBarOnKeyProc(msg);
        break;
    }
    case GUI_MSG_TOUCH:
    {
        break;
    }
    case GUI_MSG_TIMER:
    {
        //VolumeBarDelete();
        VolumeBarOnTimer(msg);
        break;
    }
    case GUI_MSG_CLOSE:
    {
        VolumeBarOnClose(msg);
        break;
    }
    case GUI_MSG_DESTROY:
    {
        VolumeBarOnDestroy(msg);
        break;
    }
    default:
    {
        break;
    }
    }
    return GUI_FrmWinDefaultProc(msg);
}

__s32 VolumeBarTimerCreate(VolumeBar_t *This)
{

    if(This->Para.CtrlVar.AutoExitTime == 0)
    {
        return EPDK_FAIL;
    }
    GUI_SetTimer(This->H_Framewin, This->TimerID, This->Para.CtrlVar.AutoExitTime * C_VB_AUTO_EXIT_TIME, NULL);
    //以秒钟为单位，GUITimer单位为100ms
    return EPDK_OK;
}


__s32 VolumeBarTimerDelete(VolumeBar_t *This)
{

    GUI_KillTimer(This->H_Framewin, This->TimerID);
    return EPDK_OK;
}

//初试化参数
VolumeBar_t *VolumeBarCreate(VolumeBarPara_t *Para)
{
    __gui_framewincreate_para_t  framewin_para;

    VolumeBar_t *this = NULL;

    //app_print_memory_usage("music volume bar create", NULL);

    this = (VolumeBar_t *)esMEMS_Balloc(sizeof(VolumeBar_t));
    eLIBs_memset((void *)this, 0, sizeof(VolumeBar_t));

    eLIBs_memcpy((void *)(&(this->Para)), (void *)Para, sizeof(VolumeBarPara_t));

    this->VB_Layer = VolumeBar32bppLayerCreate(&(Para->UI.Background), Para->CtrlVar.Pipe);
    if(this->VB_Layer == NULL)
    {
        esMEMS_Bfree(this, sizeof(VolumeBar_t));
        __wrn("Create layer for Volume Bar failed!\n");
        return NULL;
    }


    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name			= MUSIC_VOLBAR_WIN,
                         framewin_para.dwExStyle		= WS_EX_NONE;
    framewin_para.dwStyle		= WS_NONE | WS_VISIBLE;
    framewin_para.spCaption		= NULL;
    framewin_para.hOwner		= NULL;
    framewin_para.id			= C_VOLUMEBAR_FRAMEWIN_ID;
    framewin_para.hHosting		=  Para->CtrlVar.hParent;

    framewin_para.FrameWinProc	= (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)VolumeBarCB);
    framewin_para.rect.x		= 0;
    framewin_para.rect.y		= 0;
    framewin_para.rect.width	= Para->UI.Background.width;
    framewin_para.rect.height	= Para->UI.Background.height;

    framewin_para.BkColor.alpha =  0x99;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;

    framewin_para.attr			= (void *)this;
    framewin_para.hLayer		= this->VB_Layer;

    this->H_Framewin = GUI_FrmWinCreate(&framewin_para);
    if (this->H_Framewin == NULL)
    {
        esMEMS_Bfree(this, sizeof(VolumeBar_t));
        __msg("GUI_FrmWinCreate fail...\n");
        return NULL;
    }

    GUI_LyrWinSetSta(this->VB_Layer, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(this->VB_Layer);

    GUI_WinSetFocusChild(this->H_Framewin);
    return this;
}

__s32 VolumeBarDelete(VolumeBar_t *This)
{
    music_player_ctrl_t *music_play_ctrl ;
    if(This == NULL)
    {
        __wrn("Delete Volume Bar failed!\n");
        return EPDK_FAIL;
    }

    music_play_ctrl = (music_player_ctrl_t *)(This->Para.CtrlVar.attr);
    if(music_play_ctrl && music_play_ctrl->hVolumeBar)
    {
        __msg("*************VolumeBarDelete****************\n");
        if(This->VB_Layer != NULL)
        {
            GUI_LyrWinDelete(This->VB_Layer);		//删除图层时，会自动删除framewin -> on close->on destroy
            This->VB_Layer = NULL;

        }
        music_play_ctrl->hVolumeBar = NULL;
    }

    return EPDK_OK;
}

__s32 VolumeBarSetFocus(VolumeBar_t *This)
{
    if(This == NULL)
    {
        return EPDK_FAIL;
    }
    GUI_WinSetFocusChild(This->H_Framewin);
    VolumeBarShow(This);
    return EPDK_OK;
}


__s32 VolumeBarShow(VolumeBar_t *This)
{
    __s32 vol_value = 0;
    if(This == NULL)
    {
        return EPDK_OK;
    }
    GUI_LyrWinSetTop(This->VB_Layer);
    GUI_LyrWinSel(This->VB_Layer);

    //GUI_LyrWinSetTop(This->Para.CtrlVar.UserLayer);
    //GUI_LyrWinSel(This->Para.CtrlVar.UserLayer);
    vol_value = dsk_volume_get();
    __msg("volume value is %d\n", vol_value);
    DrawVolumeBar(This, vol_value);


    return EPDK_OK;
}

__s32 VolumeBarNotifyParentDestory(VolumeBarPara_t *Para)
{

    return EPDK_OK;
}


