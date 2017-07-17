/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : ebook_sset.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#include "ebook_sset.h"
#include "ebook_mset.h"

#define MAIN_SET_ITEM_MAX
#define PAGE_RECT_X 10
#define PAGE_RECT_W 20
#define PAGE_RECT_H 20
#define SSET_TEXT_X 30
#define SSET_TEXT_Y 5

typedef  struct tag_ebook_sset_res_id
{
    __s32  n_item;
    SIZE   res_size[8];
    __s32 id_icon[8];
    __s32 id_string[16];
} ebook_sset_res_id_t;

typedef struct tag_ebook_sset_res
{
    HTHEME h_icon[8];
    char  string[4][32];
} ebook_sset_res_t;

typedef struct tag_ebook_sset_ctrl
{
    ebook_sset_para_t *sset_para;
    ebook_sset_res_t sset_res;

    __s32 main_id;
    __s32  item_nr;
    __s32 focus_id;
    __s32 old_focus;
    __s32 fcs_color;
    __s32 unfcs_color;
    __s32 goto_adj;		//表示页数跳转时的页数的设置位数，从左到右递增
    __s32  page_max;
} ebook_sset_ctrl_t;

static ebook_sset_res_id_t sset_red_id[] =
{
    {
        // 0
        2,
        {{118, 2}, {118, 2}, {88, 1}, {118, 24}, {118, 24}, {118, 24}, {0, 0}, {0, 0}},
        {ID_EBOOK_BAR_TOP_BMP, ID_EBOOK_BAR_BOTTOM_BMP, NULL, ID_EBOOK_BAR_BMP,  ID_EBOOK_UNFCS_SEL_BMP, ID_EBOOK_FCS_SEL_BMP,  0, 0},
        {STRING_EBOOK_OFF, STRING_EBOOK_ON, 0, 0},
    },
    {
        // 1
        4,
        {{118, 2}, {118, 2}, {88, 1}, {118, 24}, {118, 24}, {118, 24}, {0, 0}, {0, 0}},
        {ID_EBOOK_BAR_TOP_BMP, ID_EBOOK_BAR_BOTTOM_BMP, NULL, ID_EBOOK_BAR_BMP,  ID_EBOOK_UNFCS_SEL_BMP, ID_EBOOK_FCS_SEL_BMP,  0, 0},
        {STRING_EBOOK_FAST, STRING_EBOOK_MIDDLE, STRING_EBOOK_SLOW, STRING_EBOOK_OFF},
    },
    {
        // 1
        3,
        {{118, 2}, {118, 2}, {88, 1}, {118, 24}, {118, 24}, {118, 24}, {20, 20}, {0, 0}},
        {ID_EBOOK_BAR_TOP_BMP, ID_EBOOK_BAR_BOTTOM_BMP, NULL, ID_EBOOK_BAR_BMP,  ID_EBOOK_UNFCS_SEL_BMP, ID_EBOOK_FCS_SEL_BMP,  0, 0},
        {STRING_EBOOK_BLACK, STRING_EBOOK_WHITE,  STRING_EBOOK_YELLOW, 0},
    },
    {
        // 1
        1,
        {{118, 2}, {118, 2},  {118, 24}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {ID_EBOOK_BAR_TOP_BMP, ID_EBOOK_BAR_BOTTOM_BMP, ID_EBOOK_BAR_BMP, ID_EBOOK_GRAY_BLOCK_BMP,  0, 0,  0, 0},
        {0, 0, 0, 0},
    },

};

static void ebook_sset_init(ebook_sset_ctrl_t *sset_ctrl)
{
    __s32 i;

    sset_ctrl->item_nr = sset_red_id[sset_ctrl->main_id].n_item;

    for (i = 0; i < 15; i++)
    {
        if (sset_red_id[sset_ctrl->main_id].id_string[i] == 0)
            continue;
        get_menu_text(sset_red_id[sset_ctrl->main_id].id_string[i],  sset_ctrl->sset_res.string[i], 32);
    }

    for (i = 0; i < 8; i++)
    {
        if (sset_red_id[sset_ctrl->main_id].id_icon[i] == 0)
            continue;

        sset_ctrl->sset_res.h_icon[i] =  theme_open(sset_red_id[sset_ctrl->main_id].id_icon[i]);
    }

}

static void ebook_sset_uninit(ebook_sset_ctrl_t *sset_ctrl)
{
    __s32 i;

    for (i = 0; i < 8; i++)
    {
        if (sset_red_id[sset_ctrl->main_id].id_icon[i] == 0)
            continue;
        theme_close(sset_ctrl->sset_res.h_icon[i]);
    }
}
/*
	显示列表
*/
static void ebook_sset_paint_list(ebook_sset_ctrl_t *sset_ctrl)
{
    ebook_sset_res_t *sset_res;
    __s32 x, y, i, x0;
    __s32 tmp_time;
    FB  fb;
    char buf[32];
    GUI_RECT gui_rect;

    sset_res = &sset_ctrl->sset_res;
    GUI_LyrWinGetFB(sset_ctrl->sset_para->layer, &fb);

    GUI_LyrWinSel(sset_ctrl->sset_para->layer);
    GUI_SetFont(sset_ctrl->sset_para->sset_font);
    GUI_UC_SetEncodeUTF8();

    GUI_LyrWinCacheOn();
    x = 0;
    y = 0;
    GUI_BMP_Draw(dsk_theme_hdl2buf(sset_res->h_icon[0]), x, y);	//菜单的topbar

    for (i = 0; i < sset_ctrl->item_nr; i++)
    {
        if (sset_ctrl ->main_id == ID_EBOOK_MSET_GOTO_PAGE)
        {
            y = i * (sset_red_id[sset_ctrl->main_id].res_size[2].height + 1) + sset_red_id[sset_ctrl->main_id].res_size[0].height;
            GUI_BMP_Draw(dsk_theme_hdl2buf(sset_res->h_icon[2]), x,  y);

            GUI_SetColor(sset_ctrl->fcs_color);
            GUI_OpenAlphaBlend();

            if (sset_ctrl->goto_adj)
            {
                if (sset_ctrl->sset_para->total_page / 10000)
                    app_sprintf(buf, "%d  %d  %d %d %d", (sset_ctrl->sset_para->value % 100000) / 10000, (sset_ctrl->sset_para->value % 10000) / 1000, (sset_ctrl->sset_para->value % 1000) / 100, (sset_ctrl->sset_para->value % 100) / 10, (sset_ctrl->sset_para->value % 10));
                else if (sset_ctrl->sset_para->total_page / 1000)
                    app_sprintf(buf, "%d  %d %d %d",  (sset_ctrl->sset_para->value % 10000) / 1000, (sset_ctrl->sset_para->value % 1000) / 100, (sset_ctrl->sset_para->value % 100) / 10, (sset_ctrl->sset_para->value % 10));
                else if (sset_ctrl->sset_para->total_page / 100)
                    app_sprintf(buf, "%d  %d %d", (sset_ctrl->sset_para->value % 1000) / 100, (sset_ctrl->sset_para->value % 100) / 10, (sset_ctrl->sset_para->value % 10));
                else if (sset_ctrl->sset_para->total_page / 10)
                    app_sprintf(buf, "%d  %d",  (sset_ctrl->sset_para->value % 100) / 10, (sset_ctrl->sset_para->value % 10));
                else
                    app_sprintf(buf, "%d",   (sset_ctrl->sset_para->value % 10));

                gui_rect.x0 = x + PAGE_RECT_X + (sset_ctrl->goto_adj - 1) * PAGE_RECT_W;
                gui_rect.y0 = y;
                gui_rect.x1 = gui_rect.x0 + PAGE_RECT_W;
                gui_rect.y1 = gui_rect.y0 + PAGE_RECT_H;
                GUI_SetBkColor(0xF0);
                GUI_ClearRectEx(&gui_rect);

                GUI_SetColor(0xF1);
                GUI_DrawRect(gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);

                GUI_SetColor(sset_ctrl->fcs_color);
                GUI_DispStringAt(buf, x + PAGE_RECT_X + 5, y + 3);

            }
            else
            {
                app_sprintf(buf, "%d", sset_ctrl->sset_para->value);
                GUI_DispStringAt(buf, x + 20, y + 5);
            }

            GUI_CloseAlphaBlend();
        }
        else
        {

            y = i * (sset_red_id[sset_ctrl->main_id].res_size[3].height + 1) + sset_red_id[sset_ctrl->main_id].res_size[0].height;
            if ((i == sset_ctrl->sset_para->value) && (i == sset_ctrl->focus_id))
                GUI_BMP_Draw(dsk_theme_hdl2buf(sset_res->h_icon[5]), x,  y);//绘制选中图标
            else if (i == sset_ctrl->sset_para->value)
                GUI_BMP_Draw(dsk_theme_hdl2buf(sset_res->h_icon[4]), x,  y);
            else
                GUI_BMP_Draw(dsk_theme_hdl2buf(sset_res->h_icon[3]), x,  y);

            if (i == sset_ctrl->focus_id)
            {
                GUI_SetColor(sset_ctrl->fcs_color);
                GUI_DispStringAt(sset_res->string[i], x + SSET_TEXT_X, y + SSET_TEXT_Y);
            }
            else
            {
                GUI_SetColor(sset_ctrl->unfcs_color);
                GUI_DispStringAt(sset_res->string[i], x + SSET_TEXT_X, y + SSET_TEXT_Y);
            }

            if (i != sset_ctrl->item_nr - 1)	//中间的横线
            {
                GUI_SetColor(0xF1);
                y = (i + 1) * (sset_red_id[sset_ctrl->main_id].res_size[3].height + 1) + sset_red_id[sset_ctrl->main_id].res_size[0].height;
                GUI_DrawLine(x, y - 1, x + sset_red_id[sset_ctrl->main_id].res_size[0].width - 1, y - 1);
            }
        }
    }
    /*draw menu down bar*/
    if (sset_ctrl ->main_id == ID_EBOOK_MSET_GOTO_PAGE)
        GUI_BMP_Draw(dsk_theme_hdl2buf(sset_res->h_icon[1]), x, y + (sset_red_id[sset_ctrl->main_id].res_size[2].height));
    else
        GUI_BMP_Draw(dsk_theme_hdl2buf(sset_res->h_icon[1]), x, y + (sset_red_id[sset_ctrl->main_id].res_size[3].height));

    GUI_LyrWinCacheOff();

}


/*
	显示
*/
static __s32  ebook_sset_paint(__gui_msg_t *msg)
{
    ebook_sset_ctrl_t *sset_ctrl;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    sset_ctrl = (ebook_sset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    ebook_sset_paint_list(sset_ctrl);

    return EPDK_OK;
}
/*
	op enter
*/
static __s32 ebook_sset_op_enter(__gui_msg_t *msg)
{
    ebook_sset_ctrl_t *sset_ctrl;

    sset_ctrl = (ebook_sset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (sset_ctrl->main_id == ID_EBOOK_MSET_GOTO_PAGE)
    {
        if (sset_ctrl->goto_adj)	// 原来是在调整页数状态,按下enter键后，确定跳到哪一页
        {
            sset_ctrl->goto_adj = 0;
            Ebook_cmd2parent(msg->h_deswin, sset_ctrl->main_id, sset_ctrl->sset_para->value, 0);	//电子书跳到哪一页
            Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_SSET_UPDATA_PAGE, sset_ctrl->sset_para->value, 0);//更新显示页数
            Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_SSET_LR_CTRL, 0, 0);//将页数在左右调整的状态置为0，表示不是在调整页数
        }
        else
        {
            sset_ctrl->goto_adj = 1;		//在goto_page菜单，按下enter键时，原本不是调整页数的话，就进入页数调整状态
            if (sset_ctrl->sset_para->total_page / 10000)
                sset_ctrl->page_max = 5;
            else if (sset_ctrl->sset_para->total_page / 1000)
                sset_ctrl->page_max = 4;
            else if (sset_ctrl->sset_para->total_page / 100)
                sset_ctrl->page_max = 3;
            else if (sset_ctrl->sset_para->total_page / 10)
                sset_ctrl->page_max = 2;
            else
                sset_ctrl->page_max = 1;
            Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_SSET_LR_CTRL, 1, 0);//将页数在左右调整的状态置为1，表示是在调整页数
        }
        ebook_sset_paint(msg);
    }
    else
    {
        sset_ctrl->sset_para->value = sset_ctrl->focus_id;
        Ebook_cmd2parent(msg->h_deswin, sset_ctrl->main_id, sset_ctrl->focus_id, 0);
        ebook_sset_paint(msg);
    }
    return EPDK_OK;
}
/*
	op up
*/
static __s32 ebook_sset_op_up(__gui_msg_t *msg)
{
    ebook_sset_ctrl_t *sset_ctrl;
    __s32 i, tmp1, tmp2, tmp3, tvalue;

    sset_ctrl = (ebook_sset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (sset_ctrl->main_id == ID_EBOOK_MSET_GOTO_PAGE)
    {
        tmp3 = sset_ctrl->sset_para->value;			//当前的页数
        if (sset_ctrl->goto_adj)
        {
            tmp1 = sset_ctrl->page_max - sset_ctrl->goto_adj;
            tmp2 = 1;
            for(i = 0; i < tmp1; i++)
                tmp2 *= 10;
            tvalue = ( tmp3 % (tmp2 * 10)) / tmp2;	//求出goto_adj所在位的值，1为最高位，2为次高位，类推
            tmp3  -= tvalue * tmp2;
            tvalue++;

            if(((tvalue * tmp2 + tmp3) > sset_ctrl->sset_para->total_page) || (tvalue > 9)) //页数增加时超出总的页数或是所增加的位数大于9时
            {
                if(sset_ctrl->page_max ==  sset_ctrl->goto_adj)		//当调整的页数为各位时
                {
                    if(tmp3 < 10)	//当前页数小于10页时，调整页数时大于9的话，变为1
                        tvalue = 1;
                    else
                        tvalue = 0;
                }
                else
                    tvalue = 0;
            }
            sset_ctrl->sset_para->value = tmp3 + tvalue * tmp2;
        }
    }
    else
    {
        if (sset_ctrl->focus_id <= 0)
            sset_ctrl->focus_id = sset_ctrl->item_nr - 1;
        else
            sset_ctrl->focus_id--;
    }
    ebook_sset_paint(msg);
}

/*
	op down
*/
static __s32 ebook_sset_op_down(__gui_msg_t *msg)
{
    ebook_sset_ctrl_t *sset_ctrl;
    __s32 i, tmp1, tmp2, tmp3, tvalue;

    sset_ctrl = (ebook_sset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if(sset_ctrl->main_id == ID_EBOOK_MSET_GOTO_PAGE)
    {

        tmp3 = sset_ctrl->sset_para->value;
        if(sset_ctrl->goto_adj)
        {
            tmp1 = sset_ctrl->page_max - sset_ctrl->goto_adj;
            tmp2 = 1;
            for(i = 0; i < tmp1; i++)
                tmp2 *= 10;
            tvalue = ( tmp3 % (tmp2 * 10)) / tmp2;
            tmp3  -= tvalue * tmp2;
            tvalue--;

            if( ((tvalue < 0) && (sset_ctrl->page_max !=  sset_ctrl->goto_adj)) ||
                    ((tvalue < 1) && (sset_ctrl->page_max ==  sset_ctrl->goto_adj)))
            {
                for (i = 1; i < 10; i++)
                {
                    if((i * tmp2 + tmp3) > sset_ctrl->sset_para->total_page)
                        break;
                }

                tvalue = i - 1;
            }
            sset_ctrl->sset_para->value = tmp3 + tvalue * tmp2;
        }

        ebook_sset_paint(msg);
        Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_SSET_UPDATA_PAGE, sset_ctrl->sset_para->value, 0); //更新页数
    }
    else
    {
        if (sset_ctrl->focus_id >= sset_ctrl->item_nr - 1)
            sset_ctrl->focus_id = 0;
        else
            sset_ctrl->focus_id++;

        ebook_sset_paint(msg);
        Ebook_cmd2parent(msg->h_deswin, CMD_EBOOK_SSET_UPDATA_PAGE, sset_ctrl->sset_para->value, 0);
    }
}
/*
	操作左
*/
static __s32 ebook_sset_op_left(__gui_msg_t *msg)
{
    ebook_sset_ctrl_t *sset_ctrl;

    sset_ctrl = (ebook_sset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (sset_ctrl->main_id == ID_EBOOK_MSET_GOTO_PAGE)
    {
        if (sset_ctrl->goto_adj)
        {
            sset_ctrl->goto_adj--;
            if (sset_ctrl->goto_adj < 1)
                sset_ctrl->goto_adj = sset_ctrl->page_max;
        }
        ebook_sset_paint(msg);
    }

}
/*
	操作右
*/
static __s32 ebook_sset_op_right(__gui_msg_t *msg)
{
    ebook_sset_ctrl_t *sset_ctrl;

    sset_ctrl = (ebook_sset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (sset_ctrl->main_id == ID_EBOOK_MSET_GOTO_PAGE)
    {
        if (sset_ctrl->goto_adj)
        {
            sset_ctrl->goto_adj++;
            if (sset_ctrl->goto_adj > sset_ctrl->page_max)
                sset_ctrl->goto_adj = 1;
        }
        ebook_sset_paint(msg);
    }

}
/*
	回调函数
*/
static __s32 ebook_sset_Proc(__gui_msg_t *msg)
{
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        ebook_sset_ctrl_t *sset_ctrl;
        ebook_sset_para_t *sset_para;

        sset_para = (ebook_sset_para_t *)GUI_WinGetAttr(msg->h_deswin);
        sset_ctrl = (ebook_sset_ctrl_t *)My_Balloc(sizeof(ebook_sset_ctrl_t));
        if(!sset_ctrl)
        {

            __msg("memory balloc fail.........\n");
            return EPDK_FAIL;
        }
        eLIBs_memset(sset_ctrl, 0, sizeof(ebook_sset_ctrl_t));
        sset_ctrl->sset_para = sset_para;
        sset_ctrl->main_id = sset_para->main_id;
        sset_ctrl->focus_id = sset_para->value;
        ebook_sset_init(sset_ctrl);
        sset_ctrl->unfcs_color = APP_EBOOK_UNFCS_TXT_COLOR;
        sset_ctrl->fcs_color = APP_EBOOK_FCS_TXT_COLOR;

        GUI_WinSetAddData(msg->h_deswin, (__u32)sset_ctrl);
    }
    break;
    case GUI_MSG_CLOSE:
    {
        GUI_FrmWinDelete(msg->h_deswin);
    }
    break;
    case GUI_MSG_DESTROY:
    {
        ebook_sset_ctrl_t *sset_ctrl;
        sset_ctrl = (ebook_sset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        if(!sset_ctrl)
        {

            __err("sset_ctrl is null ......\n");
            return EPDK_FAIL;
        }
        ebook_sset_uninit(sset_ctrl);
        Ebook_cmd2parent(msg->h_deswin, MSG_EBOOK_SSET_RECORD_LAST_FOCUS_ID, sset_ctrl->main_id, sset_ctrl->focus_id); //退出前记录当前 焦点位置

        My_Bfree(sset_ctrl->sset_para, sizeof(ebook_sset_para_t));
        My_Bfree(sset_ctrl, sizeof(ebook_sset_ctrl_t));
    }
    break;
    case GUI_MSG_PAINT:
    {
        ebook_sset_paint(msg);
    }
    break;
    case GUI_MSG_KEY:
        break;
    case GUI_MSG_COMMAND:
        break;
    case GUI_MSG_WIN_WAKEUP:
        GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
        break;
    case MSG_EBOOK_SSET_ENTER:
    {
        ebook_sset_op_enter(msg);
        break;
    }
    case MSG_EBOOK_SSET_UP:
    {
        ebook_sset_op_up(msg);
        break;
    }
    case MSG_EBOOK_SSET_DOWN:
    {
        ebook_sset_op_down(msg);
        break;
    }
    case MSG_EBOOK_SSET_LEFT:
    {
        ebook_sset_op_left(msg);
        break;
    }
    case MSG_EBOOK_SSET_RIGHT:
    {
        ebook_sset_op_right(msg);
        break;
    }
    default:
        break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN ebook_sset_win_create(H_WIN h_parent, ebook_sset_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    ebook_sset_para_t *sset_para;
    FB fb;

    sset_para = (ebook_sset_para_t *)My_Balloc(sizeof(ebook_sset_para_t));
    if(!sset_para)
    {

        __msg("memory balloc fail.........\n");
        return EPDK_FAIL;
    }
    eLIBs_memset(sset_para, 0, sizeof(ebook_sset_para_t));

    eLIBs_memcpy(sset_para, para, sizeof(ebook_sset_para_t));

    GUI_LyrWinGetFB(para->layer, &fb);

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =	"ebook set win",
                  framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner	= NULL;
    framewin_para.id         = EBOOK_SSET_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)ebook_sset_Proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr =  (void *)sset_para;
    framewin_para.hLayer = para->layer;

    return (GUI_FrmWinCreate(&framewin_para));
}

__s32 ebook_sset_win_delete(H_WIN sset_win)
{
    GUI_FrmWinDelete(sset_win);
    return EPDK_OK;
}



