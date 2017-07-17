/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           home app sample
*
*						        	(c) Copyright 2006-2009, China
*											 All Rights Reserved
*
* File    : home.c
* By      : lyn
* Version : V1.00
*********************************************************************************************************
*/

#include "dv_i.h"
#include "dv_uipara.h"
#include "dv_mset.h"
#include "dv_common.h"
static __s32 __dv_mset_on_create(__gui_msg_t *msg)
{
    return EPDK_OK ;
}

static __s32 __dv_mset_on_destroy(__gui_msg_t *msg)
{
    dv_mset_para_t *dv_mset_ctrl ;
    dv_mset_ctrl = (dv_mset_para_t *)GUI_WinGetAttr(msg->h_deswin);

    esMEMS_Mfree(0, dv_mset_ctrl) ;

    return EPDK_OK ;
}



static __s32 __dv_mset_on_paint(__gui_msg_t *msg)
{
    dv_mset_para_t *dv_mset_ctrl ;
    __u32	i ;
    char	str[32] ;
    GUI_RECT gui_rect ;

    dv_mset_ctrl = (dv_mset_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    GUI_LyrWinSel(dv_mset_ctrl->lyr_hdl);
    GUI_SetFont(dv_mset_ctrl->font);
    GUI_SetColor(dv_mset_ctrl->txt_color);
    GUI_UC_SetEncodeUTF8();

    switch(dv_mset_ctrl->dv_menu_id)
    {
    case DV_MENU_ID_REC_SET:
    {
        //GUI_BMP_Draw(dsk_theme_hdl2buf(dv_mset_ctrl->uipara->rec_setting_bk_handle),\
        //dv_mset_ctrl->uipara->rec_setting_bk_rect.x, dv_mset_ctrl->uipara->rec_setting_bk_rect.y);

        for( i = 0; i < dv_mset_ctrl->uipara->rec_setting_item_num; i++)
        {
            if( (i == dv_mset_ctrl->tmp_focus_item) && ( i == dv_mset_ctrl->focus_item_index))
            {
                // draw select focus item
                //GUI_BMP_Draw(dsk_theme_hdl2buf(dv_mset_ctrl->uipara->select_focus_item_handle), 5 ,dv_mset_ctrl->uipara->select_item_size.height*(i+1));
                // draw opt item
                //GUI_BMP_Draw(dsk_theme_hdl2buf(dv_mset_ctrl->uipara->opt_item_handle), 8, \
                dv_mset_ctrl->uipara->select_item_size.height*(i + 1) \
                + ( dv_mset_ctrl->uipara->select_item_size.height - dv_mset_ctrl->uipara->opt_item_size.height) / 2);
            }
            else if( i == dv_mset_ctrl->tmp_focus_item)
            {
                // draw select focus item
                //GUI_BMP_Draw(dsk_theme_hdl2buf(dv_mset_ctrl->uipara->select_focus_item_handle), 5 ,dv_mset_ctrl->uipara->select_item_size.height*(i+1));
            }
            else if( i == dv_mset_ctrl->focus_item_index)
            {
                // draw opt item
                //GUI_BMP_Draw(dsk_theme_hdl2buf(dv_mset_ctrl->uipara->opt_item_handle), 8, \
                dv_mset_ctrl->uipara->select_item_size.height*(i + 1) \
                + ( dv_mset_ctrl->uipara->select_item_size.height - dv_mset_ctrl->uipara->opt_item_size.height) / 2);
            }
            else
            {
                // draw select focus item
                //GUI_BMP_Draw(dsk_theme_hdl2buf(dv_mset_ctrl->uipara->select_unfocus_item_handle), 5 ,dv_mset_ctrl->uipara->select_item_size.height*(i+1));
            }
            dsk_langres_get_menu_text(dv_mset_ctrl->uipara->rec_quantity_strid[i], str, sizeof(str));
            gui_rect.x0 =  5 + dv_mset_ctrl->uipara->opt_item_size.width;
            gui_rect.y0 =  dv_mset_ctrl->uipara->select_item_size.height * (i + 1) ;
            gui_rect.x1 =  gui_rect.x0 + 20 ;
            gui_rect.y1 =  gui_rect.y0 + dv_mset_ctrl->uipara->select_item_size.height ;
            GUI_DispStringInRect(str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

        }
    }
    break ;
    case DV_MENU_ID_IMAGE_SET:
    {
        //GUI_BMP_Draw(dsk_theme_hdl2buf(dv_mset_ctrl->uipara->image_setting_bk_handle),\
        //dv_mset_ctrl->uipara->image_setting_bk_rect.x, dv_mset_ctrl->uipara->image_setting_bk_rect.y);

        for( i = 0; i < dv_mset_ctrl->uipara->rec_setting_item_num; i++)
        {
            if( (i == dv_mset_ctrl->tmp_focus_item) && ( i == dv_mset_ctrl->focus_item_index))
            {
                // draw select focus item
                //GUI_BMP_Draw(dsk_theme_hdl2buf(dv_mset_ctrl->uipara->select_focus_item_handle),5 , dv_mset_ctrl->uipara->select_item_size.height*(i+1));
                // draw opt item
                //GUI_BMP_Draw(dsk_theme_hdl2buf(dv_mset_ctrl->uipara->opt_item_handle),8, \
                //	dv_mset_ctrl->uipara->select_item_size.height*(i+1) \
                //	+ ( dv_mset_ctrl->uipara->select_item_size.height - dv_mset_ctrl->uipara->opt_item_size.height)/2);
            }
            else if( i == dv_mset_ctrl->tmp_focus_item)
            {
                // draw select focus item
                //GUI_BMP_Draw(dsk_theme_hdl2buf(dv_mset_ctrl->uipara->select_focus_item_handle),5 , dv_mset_ctrl->uipara->select_item_size.height*(i+1));
            }
            else if( i == dv_mset_ctrl->focus_item_index)
            {
                // draw opt item
                //	GUI_BMP_Draw(dsk_theme_hdl2buf(dv_mset_ctrl->uipara->opt_item_handle),8, \
                //	dv_mset_ctrl->uipara->select_item_size.height*(i+1) \
                //	+ ( dv_mset_ctrl->uipara->select_item_size.height - dv_mset_ctrl->uipara->opt_item_size.height)/2);
            }
            else
            {
                // draw select unfocus item
                //GUI_BMP_Draw(dsk_theme_hdl2buf(dv_mset_ctrl->uipara->select_unfocus_item_handle),5 ,dv_mset_ctrl->uipara->select_item_size.height*(i+1));
            }
            dsk_langres_get_menu_text(dv_mset_ctrl->uipara->image_quantity_strid[i], str, sizeof(str));
            gui_rect.x0 =  5 + dv_mset_ctrl->uipara->opt_item_size.width;
            gui_rect.y0 =  dv_mset_ctrl->uipara->select_item_size.height * (i + 1) ;
            gui_rect.x1 =  gui_rect.x0 + 20 ;
            gui_rect.y1 =  gui_rect.y0 + dv_mset_ctrl->uipara->select_item_size.height ;
            GUI_DispStringInRect(str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

        }
    }
    break ;
    }
    return EPDK_OK;
}


static __s32 __dv_mset_on_key_proc(__gui_msg_t *msg)
{

    dv_mset_para_t *dv_mset_ctrl ;

    dv_mset_ctrl = (dv_mset_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if( DV_MENU_ID_REC_SET == dv_mset_ctrl->dv_menu_id )
    {
        if(msg->dwAddData1 == GUI_MSG_KEY_ENTER)        // 选项确定键
        {
            if(msg->dwAddData2 == KEY_UP_ACTION)
            {
                dv_mset_ctrl->focus_item_index = dv_mset_ctrl->tmp_focus_item ;
                __dv_mset_on_paint(msg);
            }
        }
        else if(msg->dwAddData1 == GUI_MSG_KEY_MENU)        // 退出按键
        {
            if(msg->dwAddData2 == KEY_UP_ACTION)
            {
                Dv_cmd2parent(msg->h_deswin, CMD_DV_MSET_EXIT, 0, 0);
            }
        }
        else if(msg->dwAddData1 == GUI_MSG_KEY_UP)
        {
            if(msg->dwAddData2 == KEY_UP_ACTION)
            {
                dv_mset_ctrl->tmp_focus_item = (dv_mset_ctrl->tmp_focus_item + dv_mset_ctrl->uipara->rec_setting_item_num - 1) % dv_mset_ctrl->uipara->rec_setting_item_num;
                __dv_mset_on_paint(msg);
            }
        }
        else if(msg->dwAddData1 == GUI_MSG_KEY_DOWN)
        {
            if(msg->dwAddData2 == KEY_UP_ACTION)
            {
                dv_mset_ctrl->tmp_focus_item = (dv_mset_ctrl->tmp_focus_item + 1) % dv_mset_ctrl->uipara->rec_setting_item_num;
                __dv_mset_on_paint(msg);
            }
        }

        return EPDK_OK;
    }
    else if( DV_MENU_ID_IMAGE_SET == dv_mset_ctrl->dv_menu_id )
    {
        if(msg->dwAddData1 == GUI_MSG_KEY_ENTER)        // 选项确定键
        {
            if(msg->dwAddData2 == KEY_UP_ACTION)
            {
                dv_mset_ctrl->focus_item_index = dv_mset_ctrl->tmp_focus_item ;
                __dv_mset_on_paint(msg);
            }
        }
        else if(msg->dwAddData1 == GUI_MSG_KEY_MENU)        // 退出按键
        {
            if(msg->dwAddData2 == KEY_UP_ACTION)
            {
                Dv_cmd2parent(msg->h_deswin, CMD_DV_MSET_EXIT, 0, 0);
            }
        }
        else if(msg->dwAddData1 == GUI_MSG_KEY_UP)
        {
            if(msg->dwAddData2 == KEY_UP_ACTION)
            {
                dv_mset_ctrl->tmp_focus_item = (dv_mset_ctrl->tmp_focus_item + dv_mset_ctrl->uipara->image_setting_item_num - 1) % dv_mset_ctrl->uipara->image_setting_item_num;
                __dv_mset_on_paint(msg);
            }
        }
        else if(msg->dwAddData1 == GUI_MSG_KEY_DOWN)
        {
            if(msg->dwAddData2 == KEY_UP_ACTION)
            {
                dv_mset_ctrl->tmp_focus_item = (dv_mset_ctrl->tmp_focus_item + 1) % dv_mset_ctrl->uipara->image_setting_item_num;
                __dv_mset_on_paint(msg);
            }
        }
        return EPDK_OK;
    }

}


static __s32 __dv_mset_main_proc(__gui_msg_t *msg)
{
    dv_mset_para_t *dv_mset_ctrl ;

    dv_mset_ctrl = (dv_mset_para_t *)GUI_WinGetAttr(msg->h_deswin);

    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        __dv_mset_on_create(msg);
    }
    return EPDK_OK;
    case GUI_MSG_DESTROY:
    {
        __dv_mset_on_destroy(msg);
    }
    return EPDK_OK;
    case GUI_MSG_PAINT:
    {
        __dv_mset_on_paint(msg);
    }
    return EPDK_OK;
    case GUI_MSG_KEY:
    {
        __dv_mset_on_key_proc(msg);
    }
    return EPDK_OK;
    default:
        break;
    }
    return GUI_FrmWinDefaultProc(msg);
}


__s32 dv_mset_create(dv_mset_para_t *para)
{
    RECT rect ;
    __gui_framewincreate_para_t framewin_para;
    dv_mset_para_t *dv_mset_ctrl ;

    dv_mset_ctrl = (dv_mset_para_t *)esMEMS_Malloc(0, sizeof(dv_mset_para_t));
    if(dv_mset_ctrl == NULL)
    {
        __wrn("malloc error \n");
        return EPDK_FAIL;
    }
    eLIBs_memcpy(dv_mset_ctrl, para, sizeof(dv_mset_para_t));

    dv_mset_ctrl->uipara = Dv_GetUipara( );
    if( DV_MENU_ID_REC_SET == para->dv_menu_id )
    {
        rect.x = dv_mset_ctrl->uipara->rec_setting_bk_rect.x ;
        rect.y = dv_mset_ctrl->uipara->rec_setting_bk_rect.y ;
        rect.width = dv_mset_ctrl->uipara->rec_setting_bk_rect.width;
        rect.height = dv_mset_ctrl->uipara->rec_setting_bk_rect.height ;
    }
    else if( DV_MENU_ID_IMAGE_SET == para->dv_menu_id )
    {
        rect.x = dv_mset_ctrl->uipara->image_setting_bk_rect.x ;
        rect.y = dv_mset_ctrl->uipara->image_setting_bk_rect.y ;
        rect.width = dv_mset_ctrl->uipara->image_setting_bk_rect.width;
        rect.height = dv_mset_ctrl->uipara->image_setting_bk_rect.height ;
    }
    else
    {
        __err(" menu setting id is invalid menu id = %d\n", para->dv_menu_id);
    }

    dv_mset_ctrl->lyr_hdl = dv_layer_create(&rect, DISP_LAYER_WORK_MODE_NORMAL);
    GUI_LyrWinSetSta(dv_mset_ctrl->lyr_hdl, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(dv_mset_ctrl->lyr_hdl);
    dv_mset_ctrl->txt_color = TEXT_COLOR ;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =	"dv menu setting win",
                  framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner	= NULL;
    framewin_para.id         = DV_MENU_WIN_ID;
    framewin_para.hHosting = para->h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__dv_mset_main_proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = rect.width;
    framewin_para.rect.height = rect.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr =  (void *)dv_mset_ctrl;
    framewin_para.hLayer = dv_mset_ctrl->lyr_hdl;
    return(GUI_FrmWinCreate(&framewin_para));

}

__s32 dv_mset_destroy( H_WIN h_win )
{
    dv_mset_para_t *dv_mset_ctrl ;

    dv_mset_ctrl = (dv_mset_para_t *)GUI_WinGetAttr(h_win);
    // for frame win ,there is no need to call  GUI_FrmWinDelete to destroy frame win ,because the function of  GUI_LyrWinDelete will do that
    GUI_LyrWinDelete(dv_mset_ctrl->lyr_hdl);
}


