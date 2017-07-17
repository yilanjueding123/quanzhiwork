/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : photo_mset.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#include "photo_mset.h"

enum
{
    PAINT_PHOTO_MSET_ALL,
    PAINT_PHOTO_MSET_ITEM,
};

typedef struct tag_photo_mset_res_id
{
    __s32 id_str;
    __s32 id_bmp_unfcs;
    __s32 id_bmp_fcs;
} photo_mset_res_id_t;

typedef struct tag_photo_mset_res
{
    char str_txt[32];
    PHOTO_BMP_RES_T  bmp_unfcs;//void *bmp_unfcs;
    PHOTO_BMP_RES_T  bmp_fcs;  //void *bmp_fcs;
} photo_mset_res_t;

typedef struct tag_photo_mset_ctrl
{
    photo_mset_res_t mset_res[MSET_ITEM_MAX];
    photo_mset_para_t  *mset_para;
    PHOTO_BMP_RES_T     mset_bg; //void				*mset_bg;
    PHOTO_BMP_RES_T     mset_line; //void				*mset_line;

    __s32 page_first_id;
    __s32 focus_id;
    __s32 old_focus;
    __s32 fcs_color;
    __s32 unfcs_color;
    __s32 lr_adjust_sset;
} photo_mset_ctrl_t;

static photo_mset_res_id_t mset_id[MSET_ITEM_MAX] =
{
    {STRING_PHOTO_SET_BG, ID_PHOTO_BG_SET_N_BMP,  ID_PHOTO_BG_SET_F_BMP},
    {STRING_PHOTO_BG_MUSIC, ID_PHOTO_BG_MUSIC_N_BMP,  ID_PHOTO_BG_MUSIC_F_BMP},
    {STRING_PHOTO_ZOOM, ID_PHOTO_ZOOM_N_BMP,  ID_PHOTO_ZOOM_F_BMP},
    {STRING_PHOTO_ROTATE, ID_PHOTO_ROTATION_N_BMP,  ID_PHOTO_ROTATION_F_BMP},
    {STRING_PHOTO_BRIGHTNESS, ID_PHOTO_BRIGHT_N_BMP,  ID_PHOTO_BRIGHT_F_BMP},
    {STRING_PHOTO_SLIDESHOW, ID_PHOTO_SLIDESHOW_N_BMP,  ID_PHOTO_SLIDESHOW_F_BMP},
    {STRING_PHOTO_INTERVAL, ID_PHOTO_INTERVAL_N_BMP,  ID_PHOTO_INTERVAL_F_BMP},
    {STRING_PHOTO_SCALE, ID_PHOTO_SCALE_N_BMP,  ID_PHOTO_SCALE_F_BMP},
};

static __u8 Timer_msetStaId = 0xfe;
#define DELAY_MSETSTAID             DELAY_HIDE_MENU


/*
	申请资源
*/
static void photo_mset_init(photo_mset_ctrl_t *mset_ctrl)
{
    __s32 i;
    create_bmp_res(ID_PHOTO_SSET_BG_BMP, mset_ctrl->mset_bg);
    create_bmp_res(ID_PHOTO_SSET_BG_SP_BMP, mset_ctrl->mset_line);

    for (i = 0; i < MSET_ITEM_MAX; i++)
    {
        get_lang_res(mset_id[i].id_str,  mset_ctrl->mset_res[i].str_txt);
        create_bmp_res(mset_id[i].id_bmp_unfcs, mset_ctrl->mset_res[i].bmp_unfcs);
        create_bmp_res(mset_id[i].id_bmp_fcs, mset_ctrl->mset_res[i].bmp_fcs);
    }
}
/*
	释放资源
*/
static void photo_mset_uninit(photo_mset_ctrl_t *mset_ctrl)
{
    // 	photo_uipara_t *photo_uipara;
    __s32 i;
    DEBUG_photo("------------->>>>photo_mset_uninit\n");
    //
    // 	photo_uipara = get_photo_uipara();

    destroy_bmp_res(mset_ctrl->mset_bg);

    destroy_bmp_res(mset_ctrl->mset_line);
    //
    for (i = 0; i < MSET_ITEM_MAX; i++)
    {
        destroy_bmp_res(mset_ctrl->mset_res[i].bmp_unfcs);
        destroy_bmp_res(mset_ctrl->mset_res[i].bmp_fcs);
    }
}

static __s32 photo_mset_paint(__gui_msg_t *msg, __s8 mode)
{
#define BMP_Y_OFFSET  6
#define BTN_SP        ((LCD_WIDTH / MSET_ITEM_PER_PAGE) - MENU_BTN_W)
#define BMP_X_OFFSET  (BTN_SP / 2)
#define STR_X_OFFSET  0
    photo_mset_ctrl_t *mset_ctrl;
    photo_uipara_t *photo_uipara;
    GUI_RECT gui_rect;
    __s32  i, k;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    photo_uipara = get_photo_uipara();
    mset_ctrl = (photo_mset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    switch(mode)
    {
    case PAINT_PHOTO_MSET_ALL:
    {
#if _USE_MEMDEV_Handle_
        GUI_MEMDEV_Handle	      draw_mem;
#endif

        GUI_LyrWinSel(mset_ctrl->mset_para->layer);
        GUI_UC_SetEncodeUTF8();
        GUI_SetFont(SWFFont);

#if _USE_MEMDEV_Handle_
        draw_mem = GUI_MEMDEV_Create(0, 0, photo_uipara->sset_main_layer.w, photo_uipara->sset_main_layer.h);
        GUI_MEMDEV_Select(draw_mem);
#endif

        GUI_BMP_RES_Draw(mset_ctrl->mset_bg, 0, 0);
        k = mset_ctrl->page_first_id;
        ASSERT(k + MSET_ITEM_PER_PAGE <= MSET_ITEM_MAX);

        gui_rect.y0 = photo_uipara->bmp_set_item.h - BMP_Y_OFFSET;//photo_uipara->bmp_set_item.h - STR_Y_OFFSET;
        gui_rect.y1 = photo_uipara->sset_main_layer.h;//photo_uipara->bmp_set_item.h + STR_Y_OFFSET;
        gui_rect.x0 = STR_X_OFFSET;//i * (photo_uipara->bmp_set_item.w + BTN_SP);
        //gui_rect.x0 + (photo_uipara->bmp_set_item.w + BTN_SP);
        for (i = 0; i < MSET_ITEM_PER_PAGE; i++)
        {
            gui_rect.x1 = gui_rect.x0 + photo_uipara->bmp_set_item.w + BTN_SP;

            if (k == mset_ctrl->focus_id)
            {
                GUI_BMP_RES_Draw(mset_ctrl->mset_res[k].bmp_fcs, gui_rect.x0 + BMP_X_OFFSET, BMP_Y_OFFSET);
                GUI_SetColor(mset_ctrl->fcs_color);
            }
            else
            {
                GUI_BMP_RES_Draw(mset_ctrl->mset_res[k].bmp_unfcs, gui_rect.x0 + BMP_X_OFFSET, BMP_Y_OFFSET);
                GUI_SetColor(mset_ctrl->unfcs_color);
            }
            GUI_DispStringInRect(mset_ctrl->mset_res[k].str_txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            k++;

            gui_rect.x0 += (photo_uipara->bmp_set_item.w + BTN_SP);

            if (i < MSET_ITEM_PER_PAGE - 1)
            {
                GUI_BMP_RES_Draw(mset_ctrl->mset_line, MENU_BTN_SP_X_(i), BMP_Y_OFFSET);
            }
        }

#if _USE_MEMDEV_Handle_
        GUI_MEMDEV_CopyToLCD_Ex(draw_mem);
        GUI_MEMDEV_Select(NULL);
        GUI_MEMDEV_Delete(draw_mem);
        draw_mem = NULL;
#endif
    }
    break;
        // 	case PAINT_PHOTO_MSET_ITEM:
        // 		{
        // 			GUI_LyrWinSel(mset_ctrl->mset_para->layer);
        // 			GUI_UC_SetEncodeUTF8();
        //
        // 			gui_rect.y0 = photo_uipara->bmp_set_item.h - STR_Y_OFFSET;
        // 			gui_rect.y1 = photo_uipara->bmp_set_item.h + STR_Y_OFFSET;
        //
        // 			gui_rect.x0 = (mset_ctrl->focus_id - mset_ctrl->page_first_id) * (photo_uipara->bmp_set_item.w + BTN_SP);
        // 			gui_rect.x1 = gui_rect.x0 + (photo_uipara->bmp_set_item.w + BTN_SP);
        //
        // 			GUI_BMP_RES_Draw(mset_ctrl->mset_res[mset_ctrl->focus_id].bmp_fcs, gui_rect.x0 + BMP_X_OFFSET, 0);
        //
        // 			GUI_SetColor(mset_ctrl->fcs_color);
        // 			GUI_DispStringInRect(mset_ctrl->mset_res[mset_ctrl->focus_id].str_txt, &gui_rect, GUI_TA_HCENTER|GUI_TA_VCENTER);
        //
        // 			k = mset_ctrl->old_focus - mset_ctrl->page_first_id;
        // 			if (0 <= k && k < MSET_ITEM_PER_PAGE)
        // 			{
        // 				gui_rect.x0 = (mset_ctrl->old_focus - mset_ctrl->page_first_id) * (photo_uipara->bmp_set_item.w + BTN_SP);
        // 				gui_rect.x1 = gui_rect.x0 + (photo_uipara->bmp_set_item.w + BTN_SP);
        //
        // 				GUI_BMP_RES_Draw(mset_ctrl->mset_res[mset_ctrl->old_focus].bmp_unfcs, gui_rect.x0 + BMP_X_OFFSET, 0);
        //
        // 				GUI_SetColor(mset_ctrl->unfcs_color);
        // 				GUI_DispStringInRect(mset_ctrl->mset_res[mset_ctrl->old_focus].str_txt, &gui_rect, GUI_TA_HCENTER|GUI_TA_VCENTER);
        // 			}
        // 		}
        // 		break;
    }

    return 0;
}

/*
	按键处理
*/
static __s32 photo_mset_key_proc(__gui_msg_t *msg)
{
    photo_mset_ctrl_t *mset_ctrl;

    DEBUG_CBMSGEx(photo_mset_key_proc);

    {
        if (msg->dwAddData2 != KEY_UP_ACTION)
        {
            if (msg->dwAddData1 != GUI_MSG_KEY_LONGLEFT && msg->dwAddData1 != GUI_MSG_KEY_LONGRIGHT)
            {
                if (ANOLE_GetSta() == ANOLE_MODE_ZOOM)
                {
                    if (msg->dwAddData1 == GUI_MSG_KEY_LONGMENU || msg->dwAddData1 == VK_POP_VOLUME)
                    {
                        //向下传做处理(放大时上下长按)
                    }
                    else
                    {
                        return EPDK_OK;
                    }
                }
                else
                {
                    return EPDK_OK;
                }
            }
        }
    }

    mset_ctrl = (photo_mset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    {
        //增加放大时的处理
        if (mset_ctrl && mset_ctrl->lr_adjust_sset == 1)
        {
            if (msg->dwAddData1 == GUI_MSG_KEY_ESCAPE      //退出用作向下
                    || msg->dwAddData1 == GUI_MSG_KEY_LONGMENU)
            {
                if (ANOLE_GetSta() == ANOLE_MODE_ZOOM)
                {
                    msg->dwAddData1 = GUI_MSG_KEY_DOWN;
                    __msg("\nkey : GUI_MSG_KEY_ESCAPE -> GUI_MSG_KEY_DOWN for ANOLE ZOOM\n");
                }
            }
            else if (msg->dwAddData1 == VK_POP_VOLUME)
            {
                if (ANOLE_GetSta() == ANOLE_MODE_ZOOM)
                {
                    msg->dwAddData1 = GUI_MSG_KEY_UP;
                    __msg("\nkey : VK_POP_VOLUME -> GUI_MSG_KEY_UP for ANOLE ZOOM\n");
                }
            }
        }
    }

    switch(msg->dwAddData1)
    {
    case GUI_MSG_KEY_LEFT:
    case GUI_MSG_KEY_LONGLEFT:
    {
        if (mset_ctrl->lr_adjust_sset == 1)
        {
            GUI_ResetTimer(msg->h_deswin, Timer_msetStaId, DELAY_MSETSTAID, 0);
            photo_cmd2parent(msg->h_deswin, CMD_PHOTO_MSET_LEFT, 0, 0);
        }
        else
        {
            mset_ctrl->old_focus = mset_ctrl->focus_id;
            if (mset_ctrl->focus_id <= 0)
            {
                mset_ctrl->focus_id = MSET_ITEM_MAX - 1;
                mset_ctrl->page_first_id = MSET_ITEM_MAX - MSET_ITEM_PER_PAGE;
            }
            else
            {
                mset_ctrl->focus_id--;
                if (mset_ctrl->focus_id < mset_ctrl->page_first_id)
                {
                    ASSERT(mset_ctrl->focus_id >= mset_ctrl->page_first_id - 1);
                    mset_ctrl->page_first_id--;
                }
            }
            GUI_ResetTimer(msg->h_deswin, Timer_msetStaId, DELAY_MSETSTAID, 0);
            photo_mset_paint(msg, PAINT_PHOTO_MSET_ALL);//photo_mset_paint(msg, PAINT_PHOTO_MSET_ITEM);
            photo_cmd2parent(msg->h_deswin, CMD_PHOTO_MSET_SW_ITEM, mset_ctrl->focus_id, 0);
        }
        break;
    }
    case GUI_MSG_KEY_RIGHT:
    case GUI_MSG_KEY_LONGRIGHT:
    {
        if (mset_ctrl->lr_adjust_sset == 1)
        {
            GUI_ResetTimer(msg->h_deswin, Timer_msetStaId, DELAY_MSETSTAID, 0);
            photo_cmd2parent(msg->h_deswin, CMD_PHOTO_MSET_RIGHT, 0, 0);
        }
        else
        {
            mset_ctrl->old_focus = mset_ctrl->focus_id;
            if (mset_ctrl->focus_id >= MSET_ITEM_MAX - 1)
            {
                mset_ctrl->focus_id = 0;
                mset_ctrl->page_first_id = 0;
            }
            else
            {
                mset_ctrl->focus_id++;
                if (mset_ctrl->focus_id - mset_ctrl->page_first_id >= MSET_ITEM_PER_PAGE)
                {
                    //mset_ctrl->page_first_id = mset_ctrl->focus_id - (MSET_ITEM_PER_PAGE - 1);
                    mset_ctrl->page_first_id++;
                }
            }
            GUI_ResetTimer(msg->h_deswin, Timer_msetStaId, DELAY_MSETSTAID, 0);
            photo_mset_paint(msg, PAINT_PHOTO_MSET_ALL);//photo_mset_paint(msg, PAINT_PHOTO_MSET_ITEM);
            photo_cmd2parent(msg->h_deswin, CMD_PHOTO_MSET_SW_ITEM, mset_ctrl->focus_id, 0);
        }
        break;
    }
    case GUI_MSG_KEY_UP:
    case GUI_MSG_KEY_LONGUP:
        GUI_ResetTimer(msg->h_deswin, Timer_msetStaId, DELAY_MSETSTAID, 0);
        photo_cmd2parent(msg->h_deswin, CMD_PHOTO_MSET_UP, 0, 0);
        break;
    case GUI_MSG_KEY_DOWN:
    case GUI_MSG_KEY_LONGDOWN:
        GUI_ResetTimer(msg->h_deswin, Timer_msetStaId, DELAY_MSETSTAID, 0);
        photo_cmd2parent(msg->h_deswin, CMD_PHOTO_MSET_DOWN, 0, 0);
        break;
    case GUI_MSG_KEY_ENTER:
    case GUI_MSG_KEY_LONGENTER:
    {
        GUI_ResetTimer(msg->h_deswin, Timer_msetStaId, DELAY_MSETSTAID, 0);
        photo_cmd2parent(msg->h_deswin, CMD_PHOTO_MSET_ENTER, 0, 0);
        break;
    }
    case GUI_MSG_KEY_ESCAPE:
    {
        photo_cmd2parent(msg->h_deswin, CMD_PHOTO_MSET_CLOSE, 0, 0);
        break;
    }
    case GUI_MSG_KEY_RISE:
    {
        break;
    }
    default:
        break;
    }

    return EPDK_OK;
}
/*
	回调函数
*/
static __s32 photo_mset_Proc(__gui_msg_t *msg)
{
    DEBUG_CBMSGEx(photo_mset_Proc);
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        photo_mset_ctrl_t *mset_ctrl;
        photo_mset_para_t *mset_para;

        mset_para = (photo_mset_para_t *)GUI_WinGetAttr(msg->h_deswin);
        BallocType(mset_ctrl, photo_mset_ctrl_t);
        ZeroTypeMem(mset_ctrl, photo_mset_ctrl_t);
        mset_ctrl->mset_para = mset_para;
        photo_mset_init(mset_ctrl);
        mset_ctrl->unfcs_color = APP_COLOR_WHITE;
        mset_ctrl->fcs_color = APP_COLOR_YELLOW;
        mset_ctrl->focus_id = mset_para->focus;
        mset_ctrl->page_first_id = mset_para->first;
        mset_ctrl->lr_adjust_sset = 0;
        GUI_SetTimer(msg->h_deswin, Timer_msetStaId, DELAY_MSETSTAID, 0);
        GUI_WinSetAddData(msg->h_deswin, (__u32)mset_ctrl);
    }
    return EPDK_OK;
    case GUI_MSG_CLOSE:
    {
        GUI_FrmWinDelete(msg->h_deswin);
    }
    return EPDK_OK;
    case GUI_MSG_DESTROY:
    {
        photo_ctrl_t      *photo_ctrl;
        photo_mset_ctrl_t *mset_ctrl;
        mset_ctrl = (photo_mset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        photo_mset_uninit(mset_ctrl);

        GUI_KillTimer(msg->h_deswin, Timer_msetStaId);

        photo_ctrl = (photo_ctrl_t *)GUI_WinGetAttr(mset_ctrl->mset_para->h_spsc);
        photo_ctrl->menu_page_first = mset_ctrl->page_first_id;
        photo_ctrl->menu_focus_id   = mset_ctrl->focus_id;

        BFreeType(mset_ctrl->mset_para, photo_mset_para_t);
        BFreeType(mset_ctrl, photo_mset_ctrl_t);
    }
    return EPDK_OK;
    case GUI_MSG_PAINT:
    {
        photo_mset_paint(msg, PAINT_PHOTO_MSET_ALL);
    }
    return EPDK_OK;
    case GUI_MSG_KEY:
        photo_mset_key_proc(msg);
        break;
    case GUI_MSG_TOUCH:
        break;
    case GUI_MSG_TIMER:
    {
        photo_cmd2parent(msg->h_deswin, CMD_PHOTO_MSET_CLOSE, 0, 0);
    }
    return EPDK_OK;
    case GUI_MSG_COMMAND:
        break;
    case GUI_MSG_WIN_WAKEUP:
        GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
        return EPDK_OK;
    case MSG_PHOTO_MSET_LR_ADJ_SSET:
    {
        photo_mset_ctrl_t *mset_ctrl;
        mset_ctrl = (photo_mset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        mset_ctrl->lr_adjust_sset  = msg->dwAddData1;
        break;
    }
    default:
        break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN photo_mset_win_create(H_WIN h_parent, photo_mset_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    photo_mset_para_t *mset_para;
    FB fb;

    BallocType(mset_para, photo_mset_para_t);
    eLIBs_memset(mset_para, 0, sizeof(photo_mset_para_t));

    eLIBs_memcpy(mset_para, para, sizeof(photo_mset_para_t));

    GUI_LyrWinGetFB(para->layer, &fb);

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =	"photo mset win",
                  framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner	= NULL;
    framewin_para.id         = PHOTO_MSET_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)photo_mset_Proc);
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

__s32 photo_mset_win_delete(H_WIN mset_win)
{
    GUI_FrmWinDelete(mset_win);
    return EPDK_OK;
}

