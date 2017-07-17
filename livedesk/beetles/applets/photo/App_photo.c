/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : app_photo.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/
#include "beetles_app.h"
#include "app_photo_i.h"


#define ShowErrorPic(_res)   do \
{ \
	__msg("Show default ErrorPic\n"); \
	if (IsValidBMPRes(_res)) \
	{ \
		ANOLE_ShowErrorPic((__u8*)ResHandle2Data(_res) + 54, 67, 67); \
	} \
}while(0)

#define PHOTO_SAVE_BG_DIALOG_ID       0xfedc
#define PHOTO_BG_OK_DIALOG_ID         0xedcb


/*********************************************************************************************************************************************************
*
*		向父窗口发信息
*
***********************************************************************************************************************************************************/
__s32  _photo_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;

    msg.h_deswin = GUI_WinGetParent(hwin);
    msg.h_srcwin = NULL;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;

    //	DEBUG_photo(" [ %x --> %x ] photo_cmd2parent(%d, %d, %d)\n", hwin, msg.h_deswin, id, data1, data2);

    return (GUI_SendMessage(&msg));
    //return (GUI_SendNotifyMessage(&msg));
}



/*
**********************************************************************************************************************
*                                               _Uninit
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static __s32 photo_uninit(photo_ctrl_t *photo_ctrl)
{
    __here__;
    anole_set_cb(ANOLE_EVENT_SHOW_PIC_FAIL, NULL, (void *)NULL);
    __here__;
    anole_set_cb(ANOLE_EVENT_SHOW_PIC_NEXT, NULL, (void *)NULL);
    __here__;
    ANOLE_Exit();
    __inf("photo uninit success!\n");

    destroy_bmp_res(photo_ctrl->err_pic);

    return EPDK_TRUE;
}

__s32 __cb_show_next_pic(void *arg_p)
{
    H_WIN hwin;
    photo_ctrl_t *photo_ctrl;
    __gui_msg_t  setmsg = {0};
    static __s32       OldIndex = -1, OldNum = -1;

    hwin = (H_WIN)arg_p;
    photo_ctrl = (photo_ctrl_t *)GUI_WinGetAttr(hwin);
    //ZeroTypeMem(&setmsg, __gui_msg_t);

    __here__;

    if (photo_ctrl->lyr_photo_spsc && photo_ctrl->h_photo_spsc && ANOLE_GetSta() == ANOLE_MODE_PLAY)
    {
        setmsg.h_deswin   = photo_ctrl->h_photo_spsc;
        setmsg.id         = MSG_PHOTO_SPSC_SET_NO;
        __here__;
        setmsg.dwAddData1 = ANOLE_GetCurPicIndex();
        __here__;
        setmsg.dwAddData2 = ANOLE_GetPicNum();
        __here__;
        if (OldIndex != setmsg.dwAddData1 || OldNum != setmsg.dwAddData2)
        {
            __here__;
            photo_spsc_set_no(&setmsg);//GUI_SendNotifyMessage(&setmsg);
            __here__;
            OldIndex = setmsg.dwAddData1;
            OldNum   = setmsg.dwAddData2;
        }
    }

    __here__;
    //DEBUG_photo("cur_id = %d, total = %d, sta = %d\n", ANOLE_GetCurPicIndex(), ANOLE_GetPicNum(), ANOLE_GetSta());

    return 0;
}


__s32 __cb_show_pic_fail(void *arg_p)
{
    photo_ctrl_t   *photo_ctrl;

    if (arg_p)
    {
        photo_ctrl = (photo_ctrl_t *)GUI_WinGetAttr((H_WIN)arg_p);
        if (photo_ctrl)
        {
            __here__;
            ShowErrorPic(photo_ctrl->err_pic);
            __here__;
        }
    }
    //	__u8 *buf = (__u8 *)arg_p;

    __msg("__cb_show_pic_fail...\n");
    return 0;
}

static __s32 notify_bg_music(H_WIN hwnd, H_WIN focus, __u32 is_open)
{
    if (is_open == PHOTO_BGMUSIC_ON)
    {
        photo_cmd2parent(hwnd, OPEN_BG_MUSIC, PHOTO_BGMUSIC_ON, 0);
    }
    else
    {
        photo_cmd2parent(hwnd, CLOSE_BG_MUSIC, PHOTO_BGMUSIC_OFF, 0);
    }

    if (focus)
    {
        GUI_WinSetFocusChild(focus);
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               _Init2
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static __s32 photo_init(__gui_msg_t *msg)
{
    __anole_para_t 	anole_para;
    __s32			ret;
    photo_ctrl_t   *photo_ctrl;

    photo_ctrl = (photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    //	notify_bg_music(msg->h_deswin, msg->h_deswin, get_photo_bgmusic());


    PHOTO_LoadData(&anole_para);

    create_bmp_res(ID_PHOTO_ERROR_BMP, photo_ctrl->err_pic );
    anole_set_cb(ANOLE_EVENT_SHOW_PIC_FAIL, esKRNL_GetCallBack(__cb_show_pic_fail), (void *)(msg->h_deswin));
    /*initial anole core*/
    ret = ANOLE_Init(&anole_para);
    if (ret == -1)
    {
        __inf("anole_init failed!\n");
        return EPDK_FAIL;
    }
    else if (ret == 1)
    {
        __inf("anole_init failed! ret = 1\n");
        ShowErrorPic(photo_ctrl->err_pic);
        return EPDK_FAIL;
    }

    anole_set_cb(ANOLE_EVENT_SHOW_PIC_NEXT, esKRNL_GetCallBack(__cb_show_next_pic), (void *)(msg->h_deswin));


    ANOLE_SetShowMode(get_photo_ratio());

    //ANOLE_PlayCurFile();

    return EPDK_TRUE;
}
/***********************************************************************************************************
	建立图层
************************************************************************************************************/
#define photo_layer_palette_create(_lyr, _prt)      app_com_layer_create_default(_lyr, _prt, "photo  layer")
// static H_LYR photo_layer_palette_create(RECT *rect)
// {
// 	H_LYR layer = NULL;
// 	FB  fb =
// 	{
// 	    {0, 0},                                   		/* size      */
// 	    {0, 0, 0},                                      /* buffer    */
// 	    {FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
// 	};
//
// 	__disp_layer_para_t lstlyr =
// 	{
// 	    DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
// 	    0,                                              /* ck_mode   */
// 	    0,                                              /* alpha_en  */
// 	    0,                                              /* alpha_val */
// 	    1,                                              /* pipe      */
// 	    0xff,                                           /* prio      */
// 	    {0, 0, 0, 0},                           		/* screen    */
// 	    {0, 0, 0, 0},                               	/* source    */
// 	    DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
// 	    NULL                                            /* fb        */
// 	};
//
// 	__layerwincreate_para_t lyrcreate_info =
// 	{
// 	    "photo  layer",
// 	    NULL,
// 	    GUI_LYRWIN_STA_SUSPEND,
// 	    GUI_LYRWIN_NORMAL
// 	};
//
// 	fb.size.width		= rect->width;
// 	fb.size.height		= rect->height;
//
// 	lstlyr.src_win.x  		= 0;
// 	lstlyr.src_win.y  		= 0;
// 	lstlyr.src_win.width 	= rect->width;
// 	lstlyr.src_win.height 	= rect->height;
//
// 	lstlyr.scn_win.x		= rect->x;
// 	lstlyr.scn_win.y		= rect->y;
// 	lstlyr.scn_win.width  	= rect->width;
// 	lstlyr.scn_win.height 	= rect->height;
//
// 	lstlyr.pipe = 1;
// 	lstlyr.fb = &fb;
// 	lyrcreate_info.lyrpara = &lstlyr;
//
// 	layer = GUI_LyrWinCreate(&lyrcreate_info);
// 	if( !layer )
// 	{
// 		__err("app bar layer create error !\n");
// 	}
//
// 	return layer;
// }

/*
	spsc create
*/
static __s32 spsc_win_create(__gui_msg_t *msg)
{
    photo_ctrl_t *photo_ctrl;
    photo_uipara_t *photo_uipara;
    photo_spsc_para_t spsc_para;
    RECT rect;

    photo_ctrl = (photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    photo_uipara = (photo_uipara_t *)get_photo_uipara();

    photo_ctrl->menu_focus_id   = 0;
    photo_ctrl->menu_page_first = 0;

    rect.x = photo_uipara->spsc_layer.x;
    rect.y = photo_uipara->spsc_layer.y;
    rect.width = photo_uipara->spsc_layer.w;
    rect.height = photo_uipara->spsc_layer.h;
    photo_layer_palette_create(photo_ctrl->lyr_photo_spsc, &rect);

    eLIBs_memset(&spsc_para, 0, sizeof(photo_spsc_para_t));
    spsc_para.layer = photo_ctrl->lyr_photo_spsc;
    spsc_para.spsc_font = photo_ctrl->photo_font;

    spsc_para.cur_no = ANOLE_GetCurPicIndex();
    spsc_para.total_no = ANOLE_GetPicNum();

    update_pic_info(spsc_para.cur_no, &spsc_para);


    if (ANOLE_GetSta() == ANOLE_MODE_PLAY)
        spsc_para.play_status = PHOTO_STAT_PLAY;
    else
        spsc_para.play_status = PHOTO_STAT_PAUSE;

    spsc_para.volume = get_sys_volume();

    photo_ctrl->h_photo_spsc = photo_spsc_win_create(msg->h_deswin, &spsc_para);

    GUI_WinSetFocusChild(photo_ctrl->h_photo_spsc );

    return 0;
}
/*
	子设置菜单的建立
*/
static __s32 sset_win_create(__gui_msg_t *msg, __s32 mset_id)
{
    photo_ctrl_t *photo_ctrl;
    photo_uipara_t *photo_uipara;
    photo_sset_para_t sset_para;
    RECT rect;

    photo_ctrl = (photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    photo_uipara = (photo_uipara_t *)get_photo_uipara();

    app_com_layer_destroy(photo_ctrl->lyr_photo_sset);

    eLIBs_memset(&sset_para, 0, sizeof(photo_sset_para_t));

    switch(mset_id)
    {
    case ID_PHOTO_MSET_SAVE_BG:
        rect.x = photo_uipara->sset_origin_layer.x;
        rect.y = photo_uipara->sset_origin_layer.y;
        rect.width = photo_uipara->sset_origin_layer.w;
        rect.height = photo_uipara->sset_origin_layer.h;
        break;
    case ID_PHOTO_MSET_BGMUSIC:
        rect.x = photo_uipara->sset_bg_muisc_layer.x;
        rect.y = photo_uipara->sset_bg_muisc_layer.y;
        rect.width = photo_uipara->sset_bg_muisc_layer.w;
        rect.height = photo_uipara->sset_bg_muisc_layer.h;
        sset_para.value = get_photo_bgmusic();;
        break;
    case ID_PHOTO_MSET_ZOOM:
        rect.x = photo_uipara->sset_zoom_layer.x;
        rect.y = photo_uipara->sset_zoom_layer.y;
        rect.width = photo_uipara->sset_zoom_layer.w;
        rect.height = photo_uipara->sset_zoom_layer.h;
        sset_para.value = ANOLE_GetZoomIndex();;
        break;
    case ID_PHOTO_MSET_ROTATE:
        rect.x = photo_uipara->sset_rotate_layer.x;
        rect.y = photo_uipara->sset_rotate_layer.y;
        rect.width = photo_uipara->sset_rotate_layer.w;
        rect.height = photo_uipara->sset_rotate_layer.h;
        sset_para.value = ANOLE_GetRotateId();
        break;
    case ID_PHOTO_MSET_BRIGHT:
        rect.x = photo_uipara->sset_brightness_layer.x;
        rect.y = photo_uipara->sset_brightness_layer.y;
        rect.width = photo_uipara->sset_brightness_layer.w;
        rect.height = photo_uipara->sset_brightness_layer.h;
        sset_para.value = (get_sys_bright()) * 11 / 15;
        break;
    case ID_PHOTO_MSET_SLIDESHOW:
        rect.x = photo_uipara->sset_slideshow_layer.x;
        rect.y = photo_uipara->sset_slideshow_layer.y;
        rect.width = photo_uipara->sset_slideshow_layer.w;
        rect.height = photo_uipara->sset_slideshow_layer.h;
        sset_para.value = ANOLE_GetSwitchMode();
        __msg("sset_para.value = %d \n", sset_para.value);
        break;
    case ID_PHOTO_MSET_INTERVAL:
    {
        __s32 temp;
        rect.x = photo_uipara->sset_interval_layer.x;
        rect.y = photo_uipara->sset_interval_layer.y;
        rect.width = photo_uipara->sset_interval_layer.w;
        rect.height = photo_uipara->sset_interval_layer.h;
        temp = get_photo_speedflag();
        if (temp == PHOTO_SPEEDFLAG_FAST)
        {
            sset_para.value = PHOTO_SPEEDFLAG_FAST;
            sset_para.interval_time = ANOLE_FAST;
        }
        else  if (temp == PHOTO_SPEEDFLAG_MIDDLE)
        {
            sset_para.value = PHOTO_SPEEDFLAG_MIDDLE;
            sset_para.interval_time = ANOLE_MIDDLE;
        }
        else if  (temp == PHOTO_SPEEDFLAG_LOW)
        {
            sset_para.value = PHOTO_SPEEDFLAG_LOW;
            sset_para.interval_time = ANOLE_SLOW;
        }
        else
        {
            sset_para.value = PHOTO_SPEEDFLAG_MANNUAL;
            sset_para.interval_time = ANOLE_GetPlayInterval();
        }
    }
    break;
    case ID_PHOTO_MSET_SCALE:
        rect.x = photo_uipara->sset_scale_layer.x;
        rect.y = photo_uipara->sset_scale_layer.y;
        rect.width = photo_uipara->sset_scale_layer.w;
        rect.height = photo_uipara->sset_scale_layer.h;
        sset_para.value = get_photo_ratio();
        break;
    default:
        return EPDK_FAIL;
    }

    photo_layer_palette_create(photo_ctrl->lyr_photo_sset, &rect);
    sset_para.layer = photo_ctrl->lyr_photo_sset;
    sset_para.sset_font = photo_ctrl->photo_font;
    sset_para.main_id = mset_id;

    photo_ctrl->h_photo_sset = photo_sset_win_create(msg->h_deswin, &sset_para);

    GUI_LyrWinCacheOn();
    GUI_LyrWinSetSta(photo_ctrl->lyr_photo_sset, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(photo_ctrl->lyr_photo_sset);
    GUI_LyrWinCacheOff();

    return EPDK_OK;
}
/*
	mset win creat
*/
static __s32 mset_win_create(__gui_msg_t *msg)
{
    photo_ctrl_t *photo_ctrl;
    photo_uipara_t *photo_uipara;
    photo_mset_para_t mset_para;
    RECT rect;

    photo_ctrl = (photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    photo_uipara = (photo_uipara_t *)get_photo_uipara();

    rect.x = photo_uipara->sset_main_layer.x;
    rect.y = photo_uipara->sset_main_layer.y;
    rect.width = photo_uipara->sset_main_layer.w;
    rect.height = photo_uipara->sset_main_layer.h;
    photo_layer_palette_create(photo_ctrl->lyr_photo_mset, &rect);

    eLIBs_memset(&mset_para, 0, sizeof(photo_mset_para_t));
    mset_para.h_spsc = msg->h_deswin;
    mset_para.focus = photo_ctrl->menu_focus_id;
    mset_para.first = photo_ctrl->menu_page_first;
    mset_para.layer = photo_ctrl->lyr_photo_mset;
    mset_para.mset_font = photo_ctrl->photo_font;
    mset_para.bright = get_sys_bright();
    mset_para.bg_music = 0 ;
    mset_para.source = 0;
    mset_para.interval = ANOLE_GetPlayInterval();
    //	mset_para.slide_switch = ANOLE_GetSwitchMode();
    mset_para.scale = 0;
    mset_para.zoom = 0;
    mset_para.rotate = 0;

    photo_ctrl->h_photo_mset = photo_mset_win_create(msg->h_deswin, &mset_para);

    GUI_LyrWinCacheOn();
    GUI_LyrWinSetSta(photo_ctrl->lyr_photo_mset, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(photo_ctrl->lyr_photo_mset);
    GUI_LyrWinCacheOff();

    GUI_WinSetFocusChild(photo_ctrl->h_photo_mset );

    return 0;
}

/************************************************************************/
// 通知桌面退出应用
/************************************************************************/
static __s32 notify_to_close_photo(H_WIN hwnd, __s32 swith_wnd_cmd)
{
    __gui_msg_t msg_send;

    msg_send.h_deswin   = GUI_WinGetParent(hwnd);
    msg_send.h_srcwin   = hwnd;
    msg_send.id         = GUI_MSG_COMMAND;
    msg_send.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwnd), swith_wnd_cmd);
    msg_send.dwAddData2 = 0;
    msg_send.dwReserved = 0;
    return GUI_SendNotifyMessage(&msg_send);
}

/*
**********************************************************************************************************************
*                                               photo_spsc_proc
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static __s32 photo_spsc_cmd_proc(__gui_msg_t  *msg)
{
    __s32 ret;

    DEBUG_CBMSGEx(photo_spsc_cmd_proc);

    switch(HIWORD(msg->dwAddData1))
    {
    case CMD_PHOTO_SPSC_GET_INFO:
    {
        photo_spsc_para_t *spsc_para;

        spsc_para = (photo_spsc_para_t *)msg->dwAddData2;
        spsc_para->cur_no = ANOLE_GetCurPicIndex();
        spsc_para->total_no = ANOLE_GetPicNum();
        update_pic_info(spsc_para->cur_no, spsc_para);

        if (ANOLE_GetSta() == ANOLE_MODE_PLAY)
            spsc_para->play_status = PHOTO_STAT_PLAY;
        else
            spsc_para->play_status = PHOTO_STAT_PAUSE;
        break;
    }
    case CMD_PHOTO_SPSC_PLAY:
    {
        ANOLE_Play();
        break;
    }
    case CMD_PHOTO_SPSC_PAUSE:
    {
        ANOLE_BackNormal();
        break;
    }
    case CMD_PHOTO_SPSC_PLAYFILE:
    {
        ret = ANOLE_ShowPicByIndex(msg->dwAddData2);
        if (ret == EPDK_FAIL)
        {
            photo_ctrl_t *photo_ctrl;
            photo_ctrl = (photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
            ShowErrorPic(photo_ctrl->err_pic);
            __msg("----need to Show error\n");
        }
        __here__;
        break;
    }
    case CMD_PHOTO_SPSC_VOLUME:
    {
        set_sys_volume(msg->dwAddData2);
        dsk_volume_set(msg->dwAddData2);
        break;
    }
    case CMD_PHOTO_SPSC_SET:
    {
        photo_ctrl_t *photo_ctrl;
        photo_spsc_para_t *spsc_para;
        spsc_para = (photo_spsc_para_t *)msg->dwAddData2;

        ANOLE_BackNormal();
        if (spsc_para)
        {
            spsc_para->play_status = PHOTO_STAT_PAUSE;
        }
        mset_win_create(msg);
        photo_ctrl = (photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
        ASSERT(0 <= photo_ctrl->menu_focus_id && photo_ctrl->menu_focus_id < MSET_ITEM_MAX);
        sset_win_create(msg, photo_ctrl->menu_focus_id);
        break;
    }
    case CMD_PHOTO_SPSC_CLOSE:
    {
        //photo_cmd2parent(msg->h_deswin, SWITCH_TO_EXPLORER, 0, 0);
        notify_to_close_photo(msg->h_deswin, SWITCH_TO_EXPLORER);
        break;
    }
    default:
        break;
    }
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               photo_sset_proc
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static __s32 photo_mset_cmd_proc(__gui_msg_t  *msg)
{
    __gui_msg_t setmsg;
    photo_ctrl_t *photo_ctrl;

    DEBUG_CBMSGEx(photo_mset_cmd_proc);

    photo_ctrl = (photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    switch(HIWORD(msg->dwAddData1))
    {
    case CMD_PHOTO_MSET_SW_ITEM:
    {
        sset_win_create(msg, msg->dwAddData2);
        break;
    }
    case CMD_PHOTO_MSET_UP:
    {
        setmsg.id = MSG_PHOTO_SSET_UP;
        setmsg.h_deswin = photo_ctrl->h_photo_sset;
        GUI_SendMessage(&setmsg);
        break;
    }
    case CMD_PHOTO_MSET_DOWN:
    {
        setmsg.id = MSG_PHOTO_SSET_DOWN;
        setmsg.h_deswin = photo_ctrl->h_photo_sset;
        GUI_SendMessage(&setmsg);
        break;
    }
    case CMD_PHOTO_MSET_LEFT:
    {
        setmsg.id = MSG_PHOTO_SSET_LEFT;
        setmsg.h_deswin = photo_ctrl->h_photo_sset;
        GUI_SendMessage(&setmsg);
        break;
    }
    case CMD_PHOTO_MSET_RIGHT:
    {
        setmsg.id = MSG_PHOTO_SSET_RIGHT;
        setmsg.h_deswin = photo_ctrl->h_photo_sset;
        GUI_SendMessage(&setmsg);
        break;
    }
    case CMD_PHOTO_MSET_ENTER:
    {
        setmsg.id = MSG_PHOTO_SSET_ENTER;
        setmsg.h_deswin = photo_ctrl->h_photo_sset;
        GUI_SendMessage(&setmsg);
        break;
    }
    case CMD_PHOTO_MSET_CLOSE:
    {
        if (photo_ctrl->h_dialog)
        {
            app_dialog_destroy(photo_ctrl->h_dialog);
            photo_ctrl->h_dialog = NULL;
        }
        app_com_layer_destroy(photo_ctrl->lyr_photo_sset);
        app_com_layer_destroy(photo_ctrl->lyr_photo_mset);
        GUI_WinSetFocusChild(photo_ctrl->h_photo_spsc);
        break;
    }
    default:
        break;
    }
    return EPDK_OK;
}

#define DEF_SAVE_BG_FILE_NAME_PATH          ""
#define DEF_SAVE_BG_FILE_NAME_EXT           ".bin"
static char *get_bg_name(char file_name[])
{
    char *pName;
    __anole_pic_info_t info;

    ZeroMemory(&info, sizeof(info));
    ANOLE_GetPicInfo(ANOLE_GetCurPicIndex(), &info);

    eLIBs_strcpy(file_name, info.name);

    pName = eLIBs_strchr(file_name, ':');
    ASSERT(pName);
    eLIBs_strcpy(pName + 1, DEF_SAVE_BG_FILE_NAME_PATH);

    pName = eLIBs_strchrlast(info.name, '\\');
    ASSERT(pName);
    eLIBs_strcat(file_name, pName);

    pName = eLIBs_strchrlast(file_name, '.');
    eLIBs_strcpy(pName, DEF_SAVE_BG_FILE_NAME_EXT);

    return file_name;
}

static __s32 photo_save_bg(void)
{
    FB fb;
    char name[MAX_FILE_NAME_LEN] = {0};

    DEBUG_photo("..........set bg file\n");

    get_bg_name(name);
    DEBUG_photo("save file bg is %s\n", name);

    eLIBs_memset(&fb, 0x0, sizeof(fb));
    fb.fmt.type = FB_TYPE_YUV;
    anole_set_bg_pic(&fb);
    gscene_bgd_save_fb(&fb, name);

    return EPDK_OK;
}

static __s32 photo_sset_cmd_proc(__gui_msg_t  *msg)
{
    __gui_msg_t setmsg;
    photo_ctrl_t *photo_ctrl;

    DEBUG_CBMSGEx(photo_sset_cmd_proc);

    photo_ctrl = (photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    switch(HIWORD(msg->dwAddData1))
    {
    case CMD_PHOTO_SSET_SOURCE:
    {
        //__s32 bmp[] = {ID_DIALOG_MSGBOX_BMP, ID_DIALOG_MSGBOX_BTN_F_BMP, ID_DIALOG_MSGBOX_BTN_N_BMP, 0};
        __s32 str[] = {STRING_HOME_PHOTO, STRING_CONFIRM_BG_SET};
        __here__;
        default_dialog(photo_ctrl->h_dialog, msg->h_deswin, PHOTO_SAVE_BG_DIALOG_ID, ADLG_YESNO, str);
        __here__;
        //			return EPDK_OK;
    }
    break;
    case CMD_PHOTO_SSET_BGMUSIC:
    {
        // 			if (msg->dwAddData2 == PHOTO_BGMUSIC_ON)
        // 			{
        // 				photo_cmd2parent(msg->h_deswin, OPEN_BG_MUSIC, msg->dwAddData2, 0);
        // 			}
        // 			else
        // 			{
        // 				photo_cmd2parent(msg->h_deswin, CLOSE_BG_MUSIC, msg->dwAddData2, 0);
        // 			}
        notify_bg_music(msg->h_deswin, photo_ctrl->h_photo_mset, msg->dwAddData2);
        set_photo_bgmusic(msg->dwAddData2);
        //__wait__;
        break;
    }
    case CMD_PHOTO_SSET_ZOOM:
    {
        ANOLE_Zoom(msg->dwAddData2);
        break;
    }
    case CMD_PHOTO_SSET_MOVE_UP:
    {
        ANOLE_ScaleMove(ANOLE_MOVE_UP);
        break;
    }
    case CMD_PHOTO_SSET_MOVE_DOWN:
    {
        ANOLE_ScaleMove(ANOLE_MOVE_DOWN);
        break;
    }
    case CMD_PHOTO_SSET_MOVE_LEFT:
    {
        ANOLE_ScaleMove(ANOLE_MOVE_LEFT);
        break;
    }
    case CMD_PHOTO_SSET_MOVE_RIGHT:
    {
        ANOLE_ScaleMove(ANOLE_MOVE_RIGHT);
        break;
    }
    case CMD_PHOTO_SSET_ROTATE:
    {
        ANOLE_RotateRight();
        break;
    }
    case CMD_PHOTO_SSET_BRIGHT:
    {
        __s32 bright;
        bright = (msg->dwAddData2 * 15) / 11;
        set_sys_bright(bright);
        dsk_display_set_lcd_bright((__lion_bright_t)bright);
        break;
    }
    case CMD_PHOTO_SSET_SLIDESHOW:
    {
        ANOLE_SetSwitchMode(msg->dwAddData2);
        set_photo_effect(msg->dwAddData2);
        break;
    }
    case CMD_PHOTO_SSET_INTERVAL:
    {
        if (msg->dwAddData2 == 0)
            ANOLE_SetPlayInterval(ANOLE_FAST);
        else if (msg->dwAddData2 == 1)
            ANOLE_SetPlayInterval(ANOLE_MIDDLE);
        else if(msg->dwAddData2 == 2)
            ANOLE_SetPlayInterval(ANOLE_SLOW);
        else
            ANOLE_SetPlayInterval(msg->dwReserved);		//自定义
        set_photo_speedflag(msg->dwAddData2);
        break;
    }
    case CMD_PHOTO_SSET_SCALE:
    {
        ANOLE_SetShowMode(msg->dwAddData2);
        set_photo_ratio(msg->dwAddData2);
        break;
    }
    case CMD_PHOTO_SSET_LR_CTRL:
    {
        setmsg.id = MSG_PHOTO_MSET_LR_ADJ_SSET;
        setmsg.h_deswin = photo_ctrl->h_photo_mset;
        setmsg.dwAddData1 = msg->dwAddData2;
        GUI_SendMessage(&setmsg);
        break;
    }
    default:
        break;
    }
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               _MainProc
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

static __s32 _app_photo_Proc(__gui_msg_t  *msg)
{
    DEBUG_CBMSGEx(_app_photo_Proc);
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        gscene_bgd_set_state(BGD_STATUS_HIDE);
        g_disable_close_scn();

        com_set_palette_by_id(ID_PHOTO_COLOURTABLE_BMP);
        {
            //背景色，文字编辑底色
            __u32 color[] = {COLOUR_BG_8BPP, COLOUR_EDIT_BG_8BPP};
            com_set_palette_color_by_index(COLOUR_TO_8BPP_INDEX_0, color, BEETLES_TBL_SIZE(color));
        }


        photo_init(msg);
        spsc_win_create(msg);
        return 0;
    }
    case DSK_MSG_HOME:
    case DSK_MSG_KILL:
    case GUI_MSG_CLOSE:
    {
        GUI_ManWinDelete(msg->h_deswin);
        break;
    }
    case GUI_MSG_DESTROY:
    {
        photo_ctrl_t *photo_ctrl;

        __msg("********photo GUI_MSG_DESTROY begin*********\n");

        photo_ctrl = (photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

        ANOLE_BackNormal();//设置为非播放状态，让自动播放线程不延时直接退出
        if (photo_ctrl->h_dialog)
        {
            app_dialog_destroy(photo_ctrl->h_dialog);
            photo_ctrl->h_dialog = NULL;
        }

        LongStrDestroy(); //TODO:to remove

        //update_to_reg();
        photo_save_cur_play_info((rat_root_t)photo_ctrl->root_type);

        photo_uninit(photo_ctrl);

        app_com_layer_destroy(photo_ctrl->lyr_photo_sset);
        app_com_layer_destroy(photo_ctrl->lyr_photo_mset);
        app_com_layer_destroy(photo_ctrl->lyr_photo_spsc);

        BFreeType(photo_ctrl, photo_ctrl_t);

        g_enable_close_scn();

        //gscene_bgd_set_state(BGD_STATUS_SHOW);

        EndOfDebugAPPMEM();

        __msg("********photo GUI_MSG_DESTROY end*********\n");
        return 0;
    }
    case GUI_MSG_COMMAND:
    {
        if (LOWORD(msg->dwAddData1) == PHOTO_SPSC_ID)
        {
            return (photo_spsc_cmd_proc(msg));
        }
        else if (LOWORD(msg->dwAddData1) == PHOTO_MSET_ID)
        {
            return (photo_mset_cmd_proc(msg));
        }
        else if (LOWORD(msg->dwAddData1) == PHOTO_SSET_ID)
        {
            return (photo_sset_cmd_proc(msg));
        }
        else if (LOWORD(msg->dwAddData1) == PHOTO_SAVE_BG_DIALOG_ID)
        {
            __here__;
            switch(HIWORD(msg->dwAddData1))
            {
            case ADLG_CMD_EXIT:
            {
                photo_ctrl_t *photo_ctrl;
                photo_ctrl = (photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
                DEBUG_photo("dialog ret = %d \n", msg->dwAddData2);
                app_dialog_destroy(msg->h_srcwin);
                photo_ctrl->h_dialog = NULL;
                if (msg->dwAddData2 == ADLG_IDYES)
                {
                    __s32 str[] = {STRING_HOME_PHOTO, STRING_PHOTO_BG_SAVED};
                    photo_save_bg();
                    default_dialog(photo_ctrl->h_dialog, msg->h_deswin, PHOTO_BG_OK_DIALOG_ID, ADLG_OK, str);
                    return EPDK_OK;
                }
                if (photo_ctrl->h_photo_mset)
                {
                    GUI_WinSetFocusChild(photo_ctrl->h_photo_mset);
                }
            }
            return EPDK_OK;
            case ADLG_CMD_SET_UI:
            {
                //set dialog ui para
                app_dialog_set_ui(msg->h_srcwin, ADLG_UI_OK, 0, 0);
            }
            break;
            }
            return EPDK_OK;
        }
        else if (LOWORD(msg->dwAddData1) == PHOTO_BG_OK_DIALOG_ID)
        {
            if (HIWORD(msg->dwAddData1) == ADLG_CMD_EXIT)
            {
                photo_ctrl_t *photo_ctrl;
                photo_ctrl = (photo_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
                app_dialog_destroy(msg->h_srcwin);
                photo_ctrl->h_dialog = NULL;
                if (photo_ctrl->h_photo_mset)
                {
                    GUI_WinSetFocusChild(photo_ctrl->h_photo_mset);
                }
            }
            return EPDK_OK;
        }
        break;
    }
    case GUI_MSG_KEY:
    {
        //////////////////////////////////////////////////////////////////////////
        {
            //消除down和up按键不对应的消息
            static __u32 lastkey = VK_NULL;

            if (msg->dwAddData2 != KEY_UP_ACTION)
            {
                lastkey = msg->dwAddData1;
            }
            else //if (msg->dwAddData2 == KEY_UP_ACTION)
            {
                if (lastkey != msg->dwAddData1)
                {
                    lastkey = VK_NULL;
                    DEBUG_photo("key up, but no key down, do nothing\n");
                    return EPDK_OK;
                }
            }
        }
#if 1
        {
            //for old develope board
            if (msg->dwAddData1 == GUI_MSG_KEY_VDEC)
            {
                DEBUG_photo("\n _db_ GUI_MSG_KEY_VDEC -> GUI_MSG_KEY_MENU  ____ \n");
                msg->dwAddData1 = GUI_MSG_KEY_MENU;
            }
            else if (msg->dwAddData1 == GUI_MSG_KEY_LONGVDEC)
            {
                DEBUG_photo("\n _db_ GUI_MSG_KEY_LONGVDEC -> GUI_MSG_KEY_LONGMENU ____ \n");
                msg->dwAddData1 = GUI_MSG_KEY_LONGMENU;
            }
        }
#endif
        //trans key
        if (msg->dwAddData2 == KEY_REPEAT_ACTION)
        {
            if (msg->dwAddData1 == GUI_MSG_KEY_ENTER || msg->dwAddData1 == GUI_MSG_KEY_LONGENTER
                    || msg->dwAddData1 == GUI_MSG_KEY_ESCAPE || msg->dwAddData1 == GUI_MSG_KEY_LONGESCAPE)
            {
                return EPDK_OK;
            }
        }
        else
        {
            if (msg->dwAddData1 == GUI_MSG_KEY_MENU)
            {
                DEBUG_photo("\n _app_ GUI_MSG_KEY_MENU -> GUI_MSG_KEY_ESCAPE ____ \n");
                msg->dwAddData1 = GUI_MSG_KEY_ESCAPE;
            }
            else if (msg->dwAddData1 == GUI_MSG_KEY_LONGMENU)
            {
                DEBUG_photo("\n _app_ GUI_MSG_KEY_LONGMENU -> GUI_MSG_KEY_MENU ____ \n");
                msg->dwAddData1 = GUI_MSG_KEY_MENU;
            }
            else if (msg->dwAddData1 == GUI_MSG_KEY_VADD)
            {
                DEBUG_photo("\n _app_ GUI_MSG_KEY_VADD -> GUI_MSG_KEY_UP ____ \n");
                msg->dwAddData1 = GUI_MSG_KEY_UP;
            }
            else if (msg->dwAddData1 == GUI_MSG_KEY_LONGVADD)
            {
                DEBUG_photo("\n _app_ GUI_MSG_KEY_LONGVADD -> ? ____ \n");
            }
        }

        //////////////////////////////////////////////////////////////////////////
        break;
    }
    case DSK_MSG_FS_PART_PLUGOUT:
    {
        __bool to_exist;//是否退出
        __s32 ret;
        char diskname[2] = {0};
        char cur_play_file[RAT_MAX_FULL_PATH_LEN] = {0};

        __msg("************DSK_MSG_FS_PART_PLUGOUT*******\n");

        diskname[0] = (char)(msg->dwAddData2);
        diskname[1] = 0;

        to_exist = 1;//默认需要退出

        __msg("diskname=%s\n", diskname);
        ret = anole_npl_get_cur();
        __msg("ret=%d\n", ret);
        if(-1 != ret)
        {
            ret = anole_npl_index2file(ret, cur_play_file);
            __msg("ret = %d, cur_play_file=%s\n", ret, cur_play_file);
            if(EPDK_OK == ret)
            {
                __here__;
                if(0 != eLIBs_strnicmp(cur_play_file, diskname, 1))
                {
                    __here__;
                    to_exist = 0;//拔的卡不是当前播放文件所在卡，不需要退出
                }
            }
        }

        __msg("to_exist=%d\n", to_exist);
        if(1 == to_exist)
        {
            //退到桌面
            notify_to_close_photo(msg->h_deswin, SWITCH_TO_MMENU);
        }

        return EPDK_OK;
    }
    default:
        break;
    }
    return 	GUI_ManWinDefaultProc(msg);

}

H_WIN app_photo_create(root_para_t *para)
{
    __gui_manwincreate_para_t create_info;
    photo_ctrl_t *photo_ctrl;

    StartDebugAPPMEM();
    __inf("****************************************************************************************\n");
    __inf("********  enter photo app  **************\n");
    __inf("****************************************************************************************\n");

    BallocType(photo_ctrl, photo_ctrl_t);
    eLIBs_memset(photo_ctrl, 0, sizeof(photo_ctrl_t));

    photo_ctrl->photo_font = para->font;
    photo_ctrl->root_type = para->root_type;

    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));

    create_info.name            = APP_PHOTO;
    create_info.hParent         = para->h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_app_photo_Proc);
    create_info.attr            = (void *)photo_ctrl;
    create_info.id              = APP_PHOTO_ID;
    create_info.hHosting        = NULL;

    return(GUI_ManWinCreate(&create_info));
}



