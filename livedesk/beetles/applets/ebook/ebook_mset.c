/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : ebook_mset.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#include "ebook_mset.h"
#include "ebook_uipara.h"

#define MSET_ITEM_MAX		4
#define MSET_ITEM_START_X   60
#define	MSET_ITEM_BMP_OFFSET  40//菜单中每个item  间图片的间距
#define   MSET_ITEM_TXT_OFFSET   20  //菜单中每个item间字符的间距
enum
{
    PAINT_EBOOK_MSET_ALL,
    PAINT_EBOOK_MSET_ITEM,
};

typedef struct tag_ebook_mset_res_id
{
    __s32 id_str;
    __s32 id_bmp_unfcs;
    __s32 id_bmp_fcs;
} ebook_mset_res_id_t;

typedef struct tag_ebook_mset_res
{
    char str_txt[32];
    HTHEME h_bmp_unfcs;
    HTHEME h_bmp_fcs;
} ebook_mset_res_t;

typedef struct tag_ebook_mset_ctrl
{
    ebook_mset_res_t mset_res[MSET_ITEM_MAX];
    ebook_mset_para_t  *mset_para;
    HTHEME				h_mset_bg;

    __s32 focus_id;
    __s32 old_focus;
    __s32 fcs_color;
    __s32 unfcs_color;
    __s32 lr_adjust_sset;//页数调整标志
} ebook_mset_ctrl_t;

static ebook_mset_res_id_t  mset_id[MSET_ITEM_MAX] =
{
    {STRING_EBOOK_BGMUSIC, ID_EBOOK_UNFCS_BG_MUSIC_BMP,  ID_EBOOK_FCS_BG_MUSIC_BMP},
    {STRING_EBOOK_AUTO, ID_EBOOK_UNFCS_AUTO_BMP,  ID_EBOOK_FCS_AUTO_BMP},
    {STRING_EBOOK_TXTCOLOR, ID_EBOOK_UNFCS_TXTCOLOR_BMP,  ID_EBOOK_FCS_TXTCOLOR_BMP},
    {STRING_EBOOK_GOTO, ID_EBOOK_UNFCS_GOTOPAGE_BMP,  ID_EBOOK_FCS_GOTOPAGE_BMP},
};

static __u8 Timer_msetStaId = 0xfe;


/*
	申请资源
*/
static void ebook_mset_init(ebook_mset_ctrl_t *mset_ctrl)
{
    ebook_uipara_t *ebook_uipara;
    __s32 i;

    ebook_uipara = get_ebook_uipara();
    mset_ctrl->h_mset_bg = dsk_theme_open(ID_EBOOK_BOTTOM_BMP);
    for (i = 0; i < MSET_ITEM_MAX; i++)
    {
        dsk_langres_get_menu_text(mset_id[i].id_str,  mset_ctrl->mset_res[i].str_txt, 32);
        mset_ctrl->mset_res[i].h_bmp_unfcs = dsk_theme_open(mset_id[i].id_bmp_unfcs);
        mset_ctrl->mset_res[i].h_bmp_fcs = dsk_theme_open(mset_id[i].id_bmp_fcs);
    }
}
/*
	释放资源
*/
static void ebook_mset_uninit(ebook_mset_ctrl_t *mset_ctrl)
{
    __s32 i;

    dsk_theme_close(mset_ctrl->h_mset_bg);

    for (i = 0; i < MSET_ITEM_MAX; i++)
    {
        dsk_theme_close(mset_ctrl->mset_res[i].h_bmp_unfcs);
        dsk_theme_close(mset_ctrl->mset_res[i].h_bmp_fcs);
    }
}

static __s32 ebook_mset_paint(__gui_msg_t *msg, __s8 mode)
{
    ebook_mset_ctrl_t *mset_ctrl;
    ebook_uipara_t *ebook_uipara;
    FB fb;
    GUI_RECT gui_rect;
    __s32  i, x;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    ebook_uipara = get_ebook_uipara();
    mset_ctrl = (ebook_mset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    x = ebook_uipara->sset_bgmusic_lyr.x ;
    switch(mode)
    {
    case PAINT_EBOOK_MSET_ALL:
    {
        GUI_LyrWinSel(mset_ctrl->mset_para->layer);
        GUI_LyrWinGetFB(mset_ctrl->mset_para->layer, &fb);
        GUI_SetFont(mset_ctrl->mset_para->mset_font);
        GUI_UC_SetEncodeUTF8();

        GUI_LyrWinCacheOn();
        GUI_BMP_Draw(dsk_theme_hdl2buf(mset_ctrl->h_mset_bg), 0, 0);

        for (i = 0; i < MSET_ITEM_MAX; i++)
        {
            if (i == mset_ctrl->focus_id)
            {
                GUI_BMP_Draw(dsk_theme_hdl2buf(mset_ctrl->mset_res[i].h_bmp_fcs), x + i * (ebook_uipara->bmp_mset_item.w + MSET_ITEM_BMP_OFFSET) , 1);
                gui_rect.x0 =  x + i * (ebook_uipara->bmp_mset_item.w + MSET_ITEM_BMP_OFFSET) - MSET_ITEM_TXT_OFFSET / 2;
                gui_rect.y0 =  ebook_uipara->bmp_mset_item.h - 10;
                gui_rect.x1 =  gui_rect.x0 + ebook_uipara->bmp_mset_item.w + MSET_ITEM_TXT_OFFSET ;
                gui_rect.y1 =  ebook_uipara->bmp_mset_bg.h;
                GUI_SetColor(mset_ctrl->fcs_color);
                GUI_DispStringInRect(mset_ctrl->mset_res[i].str_txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            }
            else
            {
                GUI_BMP_Draw(dsk_theme_hdl2buf(mset_ctrl->mset_res[i].h_bmp_unfcs), x + i * (ebook_uipara->bmp_mset_item.w + MSET_ITEM_BMP_OFFSET) , 1);
                gui_rect.x0 =  x + i * (ebook_uipara->bmp_mset_item.w + MSET_ITEM_BMP_OFFSET) - MSET_ITEM_TXT_OFFSET / 2;
                gui_rect.y0 =  ebook_uipara->bmp_mset_item.h - 10;
                gui_rect.x1 =  gui_rect.x0 + ebook_uipara->bmp_mset_item.w + MSET_ITEM_TXT_OFFSET ;
                gui_rect.y1 =  ebook_uipara->bmp_mset_bg.h;
                GUI_SetColor(mset_ctrl->unfcs_color);
                GUI_DispStringInRect(mset_ctrl->mset_res[i].str_txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            }
        }
        GUI_LyrWinCacheOff();
    }
    break;
    case PAINT_EBOOK_MSET_ITEM:
    {
        GUI_LyrWinSel(mset_ctrl->mset_para->layer);
        GUI_SetFont(mset_ctrl->mset_para->mset_font);
        GUI_UC_SetEncodeUTF8();

        GUI_LyrWinCacheOn();
        GUI_BMP_Draw(dsk_theme_hdl2buf(mset_ctrl->mset_res[mset_ctrl->focus_id].h_bmp_fcs), x + mset_ctrl->focus_id * (ebook_uipara->bmp_mset_item.w + MSET_ITEM_BMP_OFFSET), 1);
        gui_rect.x0 = x + mset_ctrl->focus_id * (ebook_uipara->bmp_mset_item.w + MSET_ITEM_BMP_OFFSET) - MSET_ITEM_TXT_OFFSET / 2 ;
        gui_rect.y0 = ebook_uipara->bmp_mset_item.h - 10;
        gui_rect.x1 = gui_rect.x0 + ebook_uipara->bmp_mset_item.w + MSET_ITEM_TXT_OFFSET ;
        gui_rect.y1 = ebook_uipara->bmp_mset_bg.h;
        GUI_SetColor(mset_ctrl->fcs_color);
        GUI_DispStringInRect(mset_ctrl->mset_res[mset_ctrl->focus_id].str_txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

        GUI_BMP_Draw(dsk_theme_hdl2buf(mset_ctrl->mset_res[mset_ctrl->old_focus].h_bmp_unfcs), x + mset_ctrl->old_focus * (ebook_uipara->bmp_mset_item.w + MSET_ITEM_BMP_OFFSET), 1);
        gui_rect.x0 = x + mset_ctrl->old_focus * (ebook_uipara->bmp_mset_item.w + MSET_ITEM_BMP_OFFSET) - MSET_ITEM_TXT_OFFSET / 2;
        gui_rect.y0 = ebook_uipara->bmp_mset_item.h - 10 ;
        gui_rect.x1 = gui_rect.x0 + ebook_uipara->bmp_mset_item.w + MSET_ITEM_TXT_OFFSET ;
        gui_rect.y1 = ebook_uipara->bmp_mset_bg.h;
        GUI_SetColor(mset_ctrl->unfcs_color);
        GUI_DispStringInRect(mset_ctrl->mset_res[mset_ctrl->old_focus].str_txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

        GUI_LyrWinCacheOff();
    }
    break;
    }
}


/*
	按键处理
*/
static __s32 ebook_mset_key_proc(__gui_msg_t *msg)
{
    ebook_mset_ctrl_t *mset_ctrl;
    static __s32 last_key = 0xffffffff;

    mset_ctrl = (ebook_mset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    if(KEY_UP_ACTION == msg->dwAddData2)
    {
        if ((last_key == GUI_MSG_KEY_ENTER) || (last_key ==  GUI_MSG_KEY_LONGENTER))
        {
            GUI_ResetTimer(msg->h_deswin, Timer_msetStaId, 700, 0);
            Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_MSET_ENTER, 0, 0);
        }
        else if ((last_key == GUI_MSG_KEY_MENU) || (last_key == GUI_MSG_KEY_LONGMENU) || (last_key == GUI_MSG_KEY_ESCAPE)) //退出菜单显示场景
        {
            Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_MSET_RECORD_LAST_FOCUS, mset_ctrl->focus_id, 0); //记录隐藏菜单前焦点
            Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_MSET_CLOSE, 0, 0);
        }
    }
    else
    {
        switch(msg->dwAddData1)
        {
        case GUI_MSG_KEY_LEFT:
        case GUI_MSG_KEY_LONGLEFT:
        {
            if (mset_ctrl->lr_adjust_sset == 1)//页数调整标志
            {
                GUI_ResetTimer(msg->h_deswin, Timer_msetStaId, 700, 0);
                Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_MSET_DOWN, 0, 0);
            }
            else
            {
                mset_ctrl->old_focus = mset_ctrl->focus_id;
                if (mset_ctrl->focus_id <= 0)
                    mset_ctrl->focus_id = MSET_ITEM_MAX - 1;
                else
                    mset_ctrl->focus_id--;
                GUI_ResetTimer(msg->h_deswin, Timer_msetStaId, 700, 0);
                ebook_mset_paint(msg, PAINT_EBOOK_MSET_ITEM);
                Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_MSET_SW_ITEM, mset_ctrl->focus_id, 0);
            }
        }
        break;
        case GUI_MSG_KEY_RIGHT:
        case GUI_MSG_KEY_LONGRIGHT:
        {
            if (mset_ctrl->lr_adjust_sset == 1)
            {
                GUI_ResetTimer(msg->h_deswin, Timer_msetStaId, 700, 0);
                Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_MSET_UP, 0, 0);
            }
            else
            {
                mset_ctrl->old_focus = mset_ctrl->focus_id;
                if (mset_ctrl->focus_id >= MSET_ITEM_MAX - 1)
                    mset_ctrl->focus_id = 0;
                else
                    mset_ctrl->focus_id++;
                GUI_ResetTimer(msg->h_deswin, Timer_msetStaId, 700, 0);
                ebook_mset_paint(msg, PAINT_EBOOK_MSET_ITEM);
                Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_MSET_SW_ITEM, mset_ctrl->focus_id, 0);
            }
        }
        break;
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LONGUP:
        {
            GUI_ResetTimer(msg->h_deswin, Timer_msetStaId, 700, 0);
            Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_MSET_UP, 0, 0);
        }
        break;
        case GUI_MSG_KEY_DOWN:
        case GUI_MSG_KEY_LONGDOWN:
        {
            GUI_ResetTimer(msg->h_deswin, Timer_msetStaId, 700, 0);
            Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_MSET_DOWN, 0, 0);
        }
        break;

        case GUI_MSG_KEY_VADD:	//短按上下切换
        {
            if (mset_ctrl->lr_adjust_sset == 1)
            {
                GUI_ResetTimer(msg->h_deswin, Timer_msetStaId, 700, 0);
                Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_MSET_RIGHT, 0, 0);
            }
            else
            {
                GUI_ResetTimer(msg->h_deswin, Timer_msetStaId, 700, 0);
                Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_MSET_DOWN, 0, 0);
            }
        }
        break;
        default:
            break;
        }

    }
    if(KEY_UP_ACTION == msg->dwAddData2)
    {
        last_key = 0xffffffff;
    }
    else
    {
        last_key = msg->dwAddData1;
    }
    return EPDK_OK;
}
/*
	回调函数
*/
static __s32 ebook_mset_Proc(__gui_msg_t *msg)
{
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        ebook_mset_ctrl_t *mset_ctrl = NULL;
        ebook_mset_para_t *mset_para = NULL;

        mset_para = (ebook_mset_para_t *)GUI_WinGetAttr(msg->h_deswin);
        mset_ctrl = (ebook_mset_ctrl_t *)My_Balloc(sizeof(ebook_mset_ctrl_t));
        if(!mset_ctrl)
        {

            __msg("memory balloc fail.........\n");
            return EPDK_FAIL;
        }
        eLIBs_memset(mset_ctrl, 0, sizeof(ebook_mset_ctrl_t));
        mset_ctrl->mset_para = mset_para;
        ebook_mset_init(mset_ctrl);
        mset_ctrl->unfcs_color = APP_EBOOK_UNFCS_TXT_COLOR;
        mset_ctrl->fcs_color = APP_EBOOK_FCS_TXT_COLOR;
        mset_ctrl->focus_id = mset_para->focus_id;
        mset_ctrl->lr_adjust_sset = 0;
        GUI_SetTimer(msg->h_deswin, Timer_msetStaId, 700, 0);
        GUI_WinSetAddData(msg->h_deswin, (__u32)mset_ctrl);
    }
    return EPDK_OK ;
    case GUI_MSG_CLOSE:
    {
        GUI_FrmWinDelete(msg->h_deswin);
    }
    break;
    case GUI_MSG_DESTROY:
    {
        ebook_mset_ctrl_t *mset_ctrl;
        mset_ctrl = (ebook_mset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        if(!mset_ctrl)
        {

            __err("mset_ctrl is null ......\n");
            return EPDK_FAIL;
        }

        ebook_mset_uninit(mset_ctrl);

        GUI_KillTimer(msg->h_deswin, Timer_msetStaId);
        if(mset_ctrl->mset_para)
        {
            My_Bfree(mset_ctrl->mset_para, sizeof(ebook_mset_para_t));
        }

        if(mset_ctrl->mset_para)
        {
            My_Bfree(mset_ctrl, sizeof(ebook_mset_ctrl_t));
        }
    }
    break;
    case GUI_MSG_PAINT:
    {
        ebook_mset_paint(msg, PAINT_EBOOK_MSET_ALL);
    }
    break;
    case GUI_MSG_KEY:
        ebook_mset_key_proc(msg);
        break;
    case GUI_MSG_TOUCH:
        break;
    case GUI_MSG_TIMER:
    {
        Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_MSET_CLOSE, 0, 0);
    }
    break;
    case GUI_MSG_COMMAND:
        break;
    case GUI_MSG_WIN_WAKEUP:
        GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
        break;
    case MSG_EBOOK_MSET_LR_ADJ_SSET:
    {
        ebook_mset_ctrl_t *mset_ctrl;
        mset_ctrl = (ebook_mset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        mset_ctrl->lr_adjust_sset  = msg->dwAddData1;
        break;
    }
    default:
        break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN ebook_mset_win_create(H_WIN h_parent, ebook_mset_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    ebook_mset_para_t *mset_para;
    FB fb;

    mset_para = (ebook_mset_para_t *)My_Balloc(sizeof(ebook_mset_para_t));
    if(!mset_para)
    {

        __msg("memory balloc fail.........\n");
        return EPDK_FAIL;
    }
    eLIBs_memset(mset_para, 0, sizeof(ebook_mset_para_t));

    eLIBs_memcpy(mset_para, para, sizeof(ebook_mset_para_t));

    GUI_LyrWinGetFB(para->layer, &fb);

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =	"ebook mset win",
                  framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner	= NULL;
    framewin_para.id         = EBOOK_MSET_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)ebook_mset_Proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr =  (void *)mset_para;
    framewin_para.hLayer = para->layer;

    return (GUI_FrmWinCreate(&framewin_para));
}

__s32 ebook_mset_win_delete(H_WIN mset_win)
{
    GUI_FrmWinDelete(mset_win);
    return EPDK_OK;
}

