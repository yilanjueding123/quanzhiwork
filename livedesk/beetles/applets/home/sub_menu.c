/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	:sub_menu.c
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "sub_menu.h"
#include "app_home_i.h"

enum
{
    SMENU_PAINT_ALL,
    SMENU_PAINT_ITEM,
};

typedef struct tag_res_id_t
{
    __s32 lang_id;
    __s32 unfocus_bmp_id;
    __s32 focus_bmp_id;
} sm_res_id_t;

typedef struct tag_res_tab_t
{
    __s32 res_bgsell_id;		//线
    __s32 res_bgselt_id;		//顶
    __s32 res_bgselu_l_id;	//底左
    __s32 res_bgselu_r_id;	//底右
    //sm_res_id_t  res_fm_id[2];
    sm_res_id_t  res_media_id[ID_MEDIA_END - ID_MEDIA_START][MAX_SUB_ITEM]; //video photo music ebook
    sm_res_id_t  res_others_id[MAX_SUB_ITEM];
} sm_res_tab_t;

typedef struct
{
    sm_res_id_t item_res_id[MAX_SUB_ITEM];
    char	item_str[MAX_SUB_ITEM][128];
    HTHEME focus_bmp_item[MAX_SUB_ITEM];
    HTHEME unfocus_bmp_item[MAX_SUB_ITEM];
    HTHEME bmp_bgsell;
    HTHEME bmp_bgselt;
    HTHEME bmp_bgselu_l;
    HTHEME bmp_bgselu_r;

    __s32 focus_item;
    __s32 item_nr;

    __s32 focus_txt_color;
    __s32 unfocus_txt_color;

    __u8	sub_menu_id;
    GUI_FONT *font;
    H_LYR	layer;
    __s32	root_type;
    __bool  res_init;
} smenu_attr_t;

/*
	资源id号的表
*/
static sm_res_tab_t uipara_400_240 =
{
    ID_HOME_BGSELL_BMP,
    ID_HOME_BGSELT_BMP,
    ID_HOME_BGSELU_L_BMP,
    ID_HOME_BGSELU_R_BMP,

    {
        //  {	//record
        //		{STRING_HOME_NULL_DISK, ID_HOME_NULL_LOSE_BMP, ID_HOME_NULL_FOCUS_BMP},
        //		{STRING_HOME_USB, ID_HOME_RECORD_LOSE_BMP, ID_HOME_RECORD_FOCUS_BMP},
        //		{STRING_HOME_SD, ID_HOME_RECORD_LOSE_BMP, ID_HOME_RECORD_FOCUS_BMP},
        //	},
        {
            //video
            {STRING_HOME_NULL_DISK, ID_HOME_NULL_LOSE_BMP, ID_HOME_NULL_FOCUS_BMP},
            {STRING_HOME_USB, ID_HOME_VIDEO_LOSE_BMP, ID_HOME_VIDEO_FOCUS_BMP},
            {STRING_HOME_SD, ID_HOME_VIDEO_LOSE_BMP, ID_HOME_VIDEO_FOCUS_BMP},
        },
        {
            //photo
            {STRING_HOME_NULL_DISK, ID_HOME_NULL_LOSE_BMP, ID_HOME_NULL_FOCUS_BMP},
            {STRING_HOME_USB, ID_HOME_PHOTO_LOSE_BMP, ID_HOME_PHOTO_FOCUS_BMP},
            {STRING_HOME_SD, ID_HOME_PHOTO_LOSE_BMP, ID_HOME_PHOTO_FOCUS_BMP},
        },
        {
            //music
            {STRING_HOME_NULL_DISK, ID_HOME_NULL_LOSE_BMP, ID_HOME_NULL_FOCUS_BMP},
            {STRING_HOME_USB, ID_HOME_MUSIC_LOSE_BMP, ID_HOME_MUSIC_FOCUS_BMP},
            {STRING_HOME_SD, ID_HOME_MUSIC_LOSE_BMP, ID_HOME_MUSIC_FOCUS_BMP},
        },
        ///	{	//ebook
        //		{STRING_HOME_NULL_DISK, ID_HOME_NULL_LOSE_BMP, ID_HOME_NULL_FOCUS_BMP},
        ////		{STRING_HOME_USB, ID_HOME_EBOOK_LOSE_BMP, ID_HOME_EBOOK_FOCUS_BMP},
        //		{STRING_HOME_SD, ID_HOME_EBOOK_LOSE_BMP, ID_HOME_EBOOK_FOCUS_BMP},
        //	},
    },
    {
        //others
        {STRING_HOME_SET_COMMON, ID_HOME_SET_COMMON_LOSE_BMP, ID_HOME_SET_COMMON_FOCUS_BMP},
        {STRING_HOME_SET_POWER, ID_HOME_SET_POWER_LOSE_BMP, ID_HOME_SET_POWER_FOCUS_BMP},
#if BEETLES_RTC_EXIST
        {STRING_HOME_CALENDAR, ID_HOME_OTHERS_CALENDER_LOSE_BMP, ID_HOME_OTHERS_CALENDER_FOCUS_BMP},
#endif
        {STRING_HOME_EXPLORER, ID_HOME_OTHERS_EXPLORER_LOSE_BMP, ID_HOME_OTHERS_EXPLORER_FOCUS_BMP},
    }
};

static sm_res_tab_t uipara_480_272 =
{
    ID_HOME_BGSELL_BMP,
    ID_HOME_BGSELT_BMP,
    ID_HOME_BGSELU_L_BMP,
    ID_HOME_BGSELU_R_BMP,

    {
        // {	//record
        //		{STRING_HOME_NULL_DISK, ID_HOME_NULL_LOSE_BMP, ID_HOME_NULL_FOCUS_BMP},
        //		{STRING_HOME_USB, ID_HOME_RECORD_LOSE_BMP, ID_HOME_RECORD_FOCUS_BMP},
        //		{STRING_HOME_SD, ID_HOME_RECORD_LOSE_BMP, ID_HOME_RECORD_FOCUS_BMP},
        //	},
        {
            //video
            {STRING_HOME_NULL_DISK, ID_HOME_NULL_LOSE_BMP, ID_HOME_NULL_FOCUS_BMP},
            {STRING_HOME_USB, ID_HOME_VIDEO_LOSE_BMP, ID_HOME_VIDEO_FOCUS_BMP},
            {STRING_HOME_SD, ID_HOME_VIDEO_LOSE_BMP, ID_HOME_VIDEO_FOCUS_BMP},
        },
        {
            //photo
            {STRING_HOME_NULL_DISK, ID_HOME_NULL_LOSE_BMP, ID_HOME_NULL_FOCUS_BMP},
            {STRING_HOME_USB, ID_HOME_PHOTO_LOSE_BMP, ID_HOME_PHOTO_FOCUS_BMP},
            {STRING_HOME_SD, ID_HOME_PHOTO_LOSE_BMP, ID_HOME_PHOTO_FOCUS_BMP},
        },
        {
            //dv
            {STRING_HOME_NULL_DISK, ID_HOME_NULL_LOSE_BMP, ID_HOME_NULL_FOCUS_BMP},
            {STRING_HOME_USB, ID_HOME_MUSIC_LOSE_BMP, ID_HOME_MUSIC_FOCUS_BMP},
            {STRING_HOME_SD, ID_HOME_MUSIC_LOSE_BMP, ID_HOME_MUSIC_FOCUS_BMP},
        },
        //	{	//ebook
        //		{STRING_HOME_NULL_DISK, ID_HOME_NULL_LOSE_BMP, ID_HOME_NULL_FOCUS_BMP},
        //		{STRING_HOME_USB, ID_HOME_EBOOK_LOSE_BMP, ID_HOME_EBOOK_FOCUS_BMP},
        //		{STRING_HOME_SD, ID_HOME_EBOOK_LOSE_BMP, ID_HOME_EBOOK_FOCUS_BMP},
        //	},
    },
    {
        //others
        {STRING_HOME_SET_COMMON, ID_HOME_SET_COMMON_LOSE_BMP, ID_HOME_SET_COMMON_FOCUS_BMP},
        {STRING_HOME_SET_POWER, ID_HOME_SET_POWER_LOSE_BMP, ID_HOME_SET_POWER_FOCUS_BMP},
#if BEETLES_RTC_EXIST
        {STRING_HOME_CALENDAR, ID_HOME_OTHERS_CALENDER_LOSE_BMP, ID_HOME_OTHERS_CALENDER_FOCUS_BMP},
#endif
        {STRING_HOME_EXPLORER, ID_HOME_OTHERS_EXPLORER_LOSE_BMP, ID_HOME_OTHERS_EXPLORER_FOCUS_BMP},
    }
};

static sm_res_tab_t *sub_get_ui_para(__s32 rotate)
{
    __s32           screen_width;
    __s32 			screen_height;

    /* get lcd size*/
    __log("jh_dbg_0\n");
    dsk_display_get_size(&screen_width, &screen_height);
    switch(rotate)
    {
    case GUI_SCNDIR_NORMAL:
    case GUI_SCNDIR_ROTATE180:
    {
        if((screen_width == 400 ) && ( screen_height == 240))
            return &uipara_400_240;
        else if((screen_width == 480 ) && ( screen_height == 272))
            return &uipara_480_272;

    }
    case GUI_SCNDIR_ROTATE90:
    case GUI_SCNDIR_ROTATE270:

        break;
    }

    return &uipara_400_240;
}


static void smenu_init_res(smenu_attr_t *smenu_attr)
{
    __u32 	i;

    sm_res_tab_t *sub_ui_para;

    if(EPDK_TRUE == smenu_attr->res_init)
    {
        return ;
    }

    sub_ui_para = sub_get_ui_para(GUI_GetScnDir());
    if(!sub_ui_para)
    {
        __err("sub_ui_para is null...\n");
        return ;
    }

    smenu_attr->bmp_bgsell = dsk_theme_open(sub_ui_para->res_bgsell_id);

    smenu_attr->bmp_bgselt = dsk_theme_open(sub_ui_para->res_bgselt_id);

    smenu_attr->bmp_bgselu_l = dsk_theme_open(sub_ui_para->res_bgselu_l_id);

    smenu_attr->bmp_bgselu_r = dsk_theme_open(sub_ui_para->res_bgselu_r_id);

    switch(smenu_attr->sub_menu_id)
    {
    //case ID_HOME_FM:
    //	smenu_attr->item_nr = 0;
    //eLIBs_memcpy(item_res_id, sm_res_tab.res_fm_id, sizeof(sm_res_id_t)*smenu_attr->item_nr);
    //break;
    //  case ID_HOME_RECORD:
    case ID_HOME_MOVIE:
    case ID_HOME_PHOTO:
    case ID_HOME_DV:
        //case ID_HOME_EBOOK:
    {
        if (smenu_attr->root_type == 0)
        {
            __log("jh_dbg_1\n");
            smenu_attr->item_nr = 1;
            smenu_attr->item_res_id[0] = sub_ui_para->res_media_id[smenu_attr->sub_menu_id - ID_MEDIA_START][0];
        }
        else
        {
            if((smenu_attr->root_type & 0xff)//usb
                    && (smenu_attr->root_type & 0xff00)) //TF
            {
                __log("jh_dbg_2\n");
                smenu_attr->item_nr = 2;
                smenu_attr->item_res_id[0] = sub_ui_para->res_media_id[smenu_attr->sub_menu_id - ID_MEDIA_START][1];
                smenu_attr->item_res_id[1] = sub_ui_para->res_media_id[smenu_attr->sub_menu_id - ID_MEDIA_START][2];
            }
            else if  ((smenu_attr->root_type & 0xff))//USB
            {
                __log("jh_dbg_3\n");
                smenu_attr->item_nr = 1;
                smenu_attr->item_res_id[0] = sub_ui_para->res_media_id[smenu_attr->sub_menu_id - ID_MEDIA_START][1];
            }
            else if((smenu_attr->root_type & 0xff00))//TF
            {
                __log("jh_dbg_4\n");
                smenu_attr->item_nr = 1;
                smenu_attr->item_res_id[0] = sub_ui_para->res_media_id[smenu_attr->sub_menu_id - ID_MEDIA_START][2];
            }
            else
            {
                __err("root type err....\n");
            }
        }
    }
    break;
    case ID_HOME_OTHERS:
#if BEETLES_RTC_EXIST
        smenu_attr->item_nr = 4;
#else
        smenu_attr->item_nr = 3;
#endif
        eLIBs_memcpy(smenu_attr->item_res_id, sub_ui_para->res_others_id, sizeof(sm_res_id_t)*smenu_attr->item_nr);
        break;

    default:
        smenu_attr->item_nr = 0;
        __err("invalid para...\n");
        break;
    }

    for(i = 0; i < smenu_attr->item_nr; i++)
    {
        smenu_attr->unfocus_bmp_item[i] = dsk_theme_open(smenu_attr->item_res_id[i].unfocus_bmp_id);
        smenu_attr->focus_bmp_item[i] = dsk_theme_open(smenu_attr->item_res_id[i].focus_bmp_id);
    }

    if(smenu_attr->focus_item < 0)
    {
        smenu_attr->focus_item = 0;
    }
    if(smenu_attr->focus_item > smenu_attr->item_nr - 1)
    {
        smenu_attr->focus_item = 0;//smenu_attr->item_nr-1;
    }

    smenu_attr->res_init = EPDK_TRUE;

    return ;
}

/*
	释放资源
*/
static void smenu_uninit_res(smenu_attr_t *smenu_attr)
{
    __s32 i;

    if(EPDK_FALSE == smenu_attr->res_init)
    {
        return ;
    }

    dsk_theme_close(smenu_attr->bmp_bgsell);
    dsk_theme_close(smenu_attr->bmp_bgselt);
    dsk_theme_close(smenu_attr->bmp_bgselu_l);
    dsk_theme_close(smenu_attr->bmp_bgselu_r);
    smenu_attr->bmp_bgsell = NULL;
    smenu_attr->bmp_bgselt = NULL;
    smenu_attr->bmp_bgselu_l = NULL;
    smenu_attr->bmp_bgselu_r = NULL;

    for(i = 0; i < smenu_attr->item_nr; i++)
    {
        dsk_theme_close(smenu_attr->unfocus_bmp_item[i]);
        dsk_theme_close(smenu_attr->focus_bmp_item[i]);
        smenu_attr->unfocus_bmp_item[i] = NULL;
        smenu_attr->focus_bmp_item[i] = NULL;
    }

    smenu_attr->res_init = EPDK_FALSE;

}

/*
	paint
*/
static __s32 sub_menu_paint(__gui_msg_t *msg)
{
    smenu_attr_t *smenu_attr;
    __s32 i;
    __s16 x, y;
    RECT rect;
    GUI_RECT gui_rect;
    home_uipara_t *ui_para;
    return;
    ui_para = home_get_ui_para(GUI_GetScnDir());
    if(!ui_para)
    {
        __err("ui_para is null...\n");
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) != GUI_LYRWIN_STA_ON)
    {
        __msg("layer not on, do not paint...\n");
        return EPDK_FAIL;
    }

    smenu_attr = (smenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (smenu_attr == NULL)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if(EPDK_FALSE == smenu_attr->res_init)
    {
        smenu_init_res(smenu_attr);
    }

    {
        __u32 color[] = {0x99000000, 0x66808080, 0x00000000};
        com_set_palette_color_by_index(0xF0, color, BEETLES_TBL_SIZE(color));
    }

    GUI_LyrWinSel(smenu_attr->layer);

    com_memdev_create(smenu_attr->layer);

    //先清背景
    GUI_LyrWinGetScnWindow(smenu_attr->layer, &rect);
    gui_rect.x0 = 0;
    gui_rect.y0 = 0;
    gui_rect.x1 = gui_rect.x0 + rect.width;
    gui_rect.y1 = gui_rect.y0 + rect.height;

    //    0, 185, 480, 272-185
    //0,114,480,272-114

    // 0, 56, 118, 123
    //	  118,//item width
    //    24,//item height
    //    118,//line width
    //    3,//line height
    //    118,//top width
    //    2,//top height
    //    118,//bottom width
    //    6//bottom height
    __log("jh:%d,%d\n", rect.width, rect.height);
    GUI_SetColor(0xF2);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_FillRectEx(&gui_rect);

    if(smenu_attr->item_nr <= 0)
    {
        com_memdev_delete();
        __msg("smenu_attr->item_nr <= 0, not need paint...\n");
        return EPDK_OK;
    }

    //GUI_SetFontMode(GUI_FONTMODE_8BPP256);
    GUI_SetFont(smenu_attr->font);
    GUI_UC_SetEncodeUTF8();

    GUI_SetBkColor(0xF0);

    x = 0;
    y = rect.height - (ui_para->bmp_top_height + ui_para->bmp_item_height * smenu_attr->item_nr + ui_para->bmp_line_height * (smenu_attr->item_nr - 1) + ui_para->bmp_bottom_height);
    //      123           2                           24
    //3
    //6
    //这个是画上面那个亮条
    GUI_BMP_Draw(dsk_theme_hdl2buf(smenu_attr->bmp_bgselt), 0, y);

    __log("jh1%d:%d\n", y, smenu_attr->item_nr);
    for(i = 0; i < smenu_attr->item_nr; i++)
    {
        if(i == smenu_attr->focus_item)
        {
            GUI_BMP_Draw(dsk_theme_hdl2buf(smenu_attr->focus_bmp_item[i]), x, y + ui_para->bmp_top_height + ui_para->bmp_item_height * i + ui_para->bmp_line_height * i);
            GUI_SetColor(smenu_attr->focus_txt_color);
        }
        else
        {
            GUI_BMP_Draw(dsk_theme_hdl2buf(smenu_attr->unfocus_bmp_item[i]), x, y + ui_para->bmp_top_height + ui_para->bmp_item_height * i + ui_para->bmp_line_height * i);
            GUI_SetColor(smenu_attr->unfocus_txt_color);
        }

        {
            GUI_RECT gui_rect;

            gui_rect.x0 = x + 30;
            gui_rect.y0 = y + ui_para->bmp_top_height + ui_para->bmp_item_height * i + ui_para->bmp_line_height * i;
            gui_rect.x1 = gui_rect.x0 + ui_para->bmp_item_width - 30;
            gui_rect.y1 = gui_rect.y0 + ui_para->bmp_item_height;

            //GUI_ClearRectEx(&gui_rect);
            dsk_langres_get_menu_text(smenu_attr->item_res_id[i].lang_id, smenu_attr->item_str[i], GUI_TITLE_MAX);
            GUI_DispStringInRect(smenu_attr->item_str[i], &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        if (smenu_attr->item_nr > 1 && i < (smenu_attr->item_nr - 1))
        {
            GUI_BMP_Draw(dsk_theme_hdl2buf(smenu_attr->bmp_bgsell), x, y + ui_para->bmp_top_height + ui_para->bmp_item_height * (i + 1) + ui_para->bmp_line_height * i );
        }
    }

    if (smenu_attr->sub_menu_id != (ID_MAX_NUM - 1))
        GUI_BMP_Draw(dsk_theme_hdl2buf(smenu_attr->bmp_bgselu_r), x, y + ui_para->bmp_top_height + ui_para->bmp_item_height * smenu_attr->item_nr + ui_para->bmp_line_height * (smenu_attr->item_nr - 1) );
    else
        GUI_BMP_Draw(dsk_theme_hdl2buf(smenu_attr->bmp_bgselu_l), x, y + ui_para->bmp_top_height + ui_para->bmp_item_height * smenu_attr->item_nr + ui_para->bmp_line_height * (smenu_attr->item_nr - 1) );

    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    com_memdev_delete();

    return EPDK_OK;
}

/*

*/
static void sub_menu_create(__gui_msg_t *msg)
{
    smenu_attr_t *smenu_attr;
    smenu_para_t *smenu_para;

    smenu_para = (smenu_para_t *)GUI_WinGetAttr(msg->h_deswin);

    smenu_attr = (smenu_attr_t *)esMEMS_Balloc(sizeof(smenu_attr_t));
    if(!smenu_attr)
    {
        return ;
    }
    eLIBs_memset((void *)smenu_attr, 0, sizeof(smenu_attr_t));

    smenu_attr->sub_menu_id = smenu_para->sub_menu_id;

    smenu_attr->layer = smenu_para->layer;
    smenu_attr->font = smenu_para->smenu_font;
    smenu_attr->root_type = smenu_para->root_type;
    smenu_attr->focus_item = smenu_para->focus_item;
    smenu_attr->focus_txt_color = APP_COLOR_YELLOW;
    smenu_attr->unfocus_txt_color = APP_COLOR_WHITE;
    smenu_attr->res_init = EPDK_FALSE;

    smenu_init_res(smenu_attr);

    GUI_WinSetAddData(msg->h_deswin, (__u32)smenu_attr);
    //GUI_InvalidateRect(msg->h_deswin, 0, EPDK_TRUE);
}

/*
	回调
*/
static __s32 _sub_menu_Proc(__gui_msg_t *msg)
{
    __msg("_sub_menu_Proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
          , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        sub_menu_create(msg);
    }
    return EPDK_OK;
    case GUI_MSG_CLOSE:
    {
        GUI_FrmWinDelete(msg->h_deswin);
    }
    return EPDK_OK;
    case GUI_MSG_DESTROY:
    {
        smenu_attr_t *smenu_attr;
        smenu_para_t *smenu_para;

        smenu_para = (smenu_para_t *)GUI_WinGetAttr(msg->h_deswin);
        smenu_attr = (smenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
        smenu_uninit_res(smenu_attr);

        esMEMS_Bfree(smenu_attr, sizeof(smenu_attr_t));
        esMEMS_Bfree(smenu_para, sizeof(smenu_para_t));
    }
    return EPDK_OK;
    case GUI_MSG_PAINT:

        sub_menu_paint( msg);
        return EPDK_OK;
    case GUI_MSG_KEY:
        break;
    case GUI_MSG_TOUCH:
        break;
    case GUI_MSG_COMMAND:
        return EPDK_OK;
    case MSG_SUB_OP_UP:
    {
        smenu_attr_t *smenu_attr;
        smenu_attr = (smenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

        if (smenu_attr->focus_item <= 0)
            smenu_attr->focus_item = smenu_attr->item_nr - 1;
        else
            smenu_attr->focus_item--;
        sub_menu_paint( msg);
    }
    return EPDK_OK;
    case MSG_SUB_OP_DOWN:
    {
        smenu_attr_t *smenu_attr;
        smenu_attr = (smenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

        if (smenu_attr->focus_item >= smenu_attr->item_nr - 1)
            smenu_attr->focus_item = 0;
        else
            smenu_attr->focus_item++;
        sub_menu_paint( msg);
    }
    return EPDK_OK;
    case MSG_SUB_OP_ENTER:
    {
        smenu_attr_t *smenu_attr;

        smenu_attr = (smenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
        main_cmd2parent(msg->h_deswin, SMENU_APP_SWITCH, smenu_attr->focus_item, 0);
        __log("---jh_fk_6------%d", smenu_attr->focus_item);
    }
    return EPDK_OK;
    case MSG_SUB_OP_LIST:
        main_cmd2parent(msg->h_deswin, SMENU_APP_SWITCH, 4, 0);
        return EPDK_OK;
    default:
        break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

/*
	建立窗口
*/
H_WIN sub_menu_win_create(H_WIN h_parent, smenu_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    smenu_para_t *smenu_para;
    FB fb;

    GUI_LyrWinGetFB(para->layer, &fb);

    smenu_para = (smenu_para_t *)esMEMS_Balloc(sizeof(smenu_para_t));
    if(!smenu_para)
    {
        return NULL;
    }
    smenu_para->smenu_font = para->smenu_font;
    smenu_para->layer = para->layer;
    smenu_para->sub_menu_id = para->sub_menu_id;
    smenu_para->root_type = para->root_type;
    smenu_para->focus_item = para->focus_item;

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));

    framewin_para.name =	"sub_menu_win",
                  framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner	= NULL;
    framewin_para.hHosting = h_parent;
    framewin_para.id         = SUB_MENU_ID;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_sub_menu_Proc);
    framewin_para.rect.x = 0;// 0;
    framewin_para.rect.y = 0;//0;
    framewin_para.rect.width =  fb.size.width; //movie_layout.spsc_scnwin.width; //800
    framewin_para.rect.height =  fb.size.height;//movie_layout.spsc_scnwin.height;//300
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr =  (void *)smenu_para;
    framewin_para.hLayer = para->layer;

    return (GUI_FrmWinCreate(&framewin_para));
}

void  sub_menu_win_delete(H_WIN smenu_win)
{
    GUI_FrmWinDelete(smenu_win);
}

void sub_menu_init_res(H_WIN hwin)
{
    smenu_attr_t *smenu_attr = (smenu_attr_t *)GUI_WinGetAddData(hwin);
    if(smenu_attr)
    {
        smenu_init_res(smenu_attr);
    }
}

void sub_menu_uninit_res(H_WIN hwin)
{
    smenu_attr_t *smenu_attr = (smenu_attr_t *)GUI_WinGetAddData(hwin);
    if(smenu_attr)
    {
        smenu_uninit_res(smenu_attr);
    }
}


