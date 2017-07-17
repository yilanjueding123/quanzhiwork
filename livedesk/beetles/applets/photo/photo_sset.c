/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : photo_sub_sset.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#include "photo_sset.h"

//#define MAIN_SET_ITEM_MAX
#define MAX_CUSTOM_INTERVAL                    60
#define MIN_CUSTOM_INTERVAL                    1

//////////////////////////////////////////////////////////////////////////
typedef  struct tag_photo_sset_res_id
{
    __s32  n_item;
    SIZE   res_size[8];
    __s32 id_icon[8];
    __s32 id_string[16];
} photo_sset_res_id_t;

typedef struct tag_photo_sset_res
{
    PHOTO_BMP_RES_T icon[8];//void *icon[8];
    char  string[16][32];
} photo_sset_res_t;

typedef struct tag_photo_sset_ctrl
{
    photo_sset_para_t *sset_para;
    photo_sset_res_t sset_res;

    __s32 main_id;
    __s32  item_nr;
    __s32 focus_id;
    __s32 old_focus;
    __s32 fcs_color;
    __s32 unfcs_color;
    __s32 custom_interval;
    __s32 start_item;
    __s32 end_item;
    __s32 page_num;
} photo_sset_ctrl_t;

static photo_sset_res_id_t sset_red_id[8] =
{
    {
        //0, SetBg
        2,//Item Count
        {
            //Item Size
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_TOP},//bmp bar_top
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_BOTTOM},//bmp bar_bottom
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_SP}, //bmp horizontal_line
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_H},  //bmp  bar
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_H},  //bmp  bar
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_H},  //bmp  bar
            {0, 0}, {0, 0}
        },
        {
            //Item res bmp ID
            ID_PHOTO_BAR_TOP_BMP, ID_PHOTO_BAR_BOTTOM_BMP, ID_PHOTO_HORIZONTAL_LINE_BMP,
            ID_PHOTO_BAR_BMP,  ID_PHOTO_BAR_SEL_N_BMP, ID_PHOTO_BAR_SEL_F_BMP,  0, 0
        },
        {
            //Item res string ID
            STRING_SET_TIPS_YES, STRING_SET_TIPS_NO, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
    },
    {
        //1, Music
        2,//Item Count
        {
            //Item Size
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_TOP},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_BOTTOM},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_SP},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_H},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_H},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_H},
            {0, 0}, {0, 0}
        },
        {
            //Item res bmp ID
            ID_PHOTO_BAR_TOP_BMP, ID_PHOTO_BAR_BOTTOM_BMP, ID_PHOTO_HORIZONTAL_LINE_BMP,
            ID_PHOTO_BAR_BMP,  ID_PHOTO_BAR_SEL_N_BMP, ID_PHOTO_BAR_SEL_F_BMP,  0, 0
        },
        {
            //Item res string ID
            STRING_PHOTO_OFF, STRING_PHOTO_ON, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
    },
    {
        //2, Zoom
        4,//Item Count
        {
            //Item Size
            {SSET_ZOOM_BG_W, SSET_ZOOM_BG_H},
            {20, 12},
            {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}
        },
        {
            //Item res bmp ID
            ID_PHOTO_ZOOMIN_BG_BMP, ID_PHOTO_ZOOMIN_BLOCK_BMP, 0, 0, 0, 0, 0, 0
        },
        {
            //Item res string ID
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
    },
    {
        //3, Rotate
        4,//Item Count
        {
            //Item Size
            {104, 106},
            {104, 106},
            {104, 106},
            {104, 106},
            {0, 0}, {0, 0}, {0, 0}, {0, 0}
        },
        {
            //Item res bmp ID
            ID_PHOTO_ROTATION_0_BMP, ID_PHOTO_ROTATION_90_BMP, ID_PHOTO_ROTATION_180_BMP,
            ID_PHOTO_ROTATION_270_BMP, 0, 0, 0, 0
        },
        {
            //Item res string ID
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
    },
    {
        //4, Bright
        10,//Item Count
        {
            //Item Size
            {18, 118},
            {10, 9},
            {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}
        },
        {
            //Item res bmp ID
            ID_PHOTO_BRIGHT_BG_BMP, ID_PHOTO_BRIGHT_BLOCK_BMP, 0, 0, 0, 0, 0, 0
        },
        {
            //Item res string ID
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
    },
    {
        //5, SlideShow
        16,//Item Count
        {
            //Item Size
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_UP}, //bmp bar_up
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_DOWN}, //bmp bar_down
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_SP},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_H},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_H},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_H},
            {0, 0}, {0, 0}
        },
        {
            //Item res bmp ID
            ID_PHOTO_BAR_UP_BMP, ID_PHOTO_BAR_DOWN_BMP, ID_PHOTO_HORIZONTAL_LINE_BMP,
            ID_PHOTO_BAR_BMP, ID_PHOTO_BAR_SEL_N_BMP, ID_PHOTO_BAR_SEL_F_BMP,  0, 0
        },
        {
            //Item res string ID
            STRING_PHOTO_DEFAULT, STRING_PHOTO_FADE_IN_FADE_OUT, STRING_PHOTO_PERSIAN_BLINDS_H,
            STRING_PHOTO_PERSIAN_BLINDS_V, STRING_PHOTO_SLIDE_UP, STRING_PHOTO_SLIDE_DOWN,
            STRING_PHOTO_SLIDE_LEFT, STRING_PHOTO_SLIDE_RIGHT, STRING_PHOTO_STRETCH_UP,
            STRING_PHOTO_STRETCH_DOWN, STRING_PHOTO_STRETCH_LEFT, STRING_PHOTO_STRETCH_RIGHT,
            STRING_PHOTO_MOSAIC, STRING_PHOTO_ZOOM_OUT, STRING_PHOTO_ZOOM_IN, STRING_PHOTO_RANDOM_SEL
        },
    },
    {
        //6, Interval
        4,//Item Count
        {
            //Item Size
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_TOP},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_BOTTOM},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_SP},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_H},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_H},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_H},
            {20, 20},
            {0, 0}
        },
        {
            //Item res bmp ID
            ID_PHOTO_BAR_TOP_BMP, ID_PHOTO_BAR_BOTTOM_BMP, ID_PHOTO_HORIZONTAL_LINE_BMP,
            ID_PHOTO_BAR_BMP,  ID_PHOTO_BAR_SEL_N_BMP, ID_PHOTO_BAR_SEL_F_BMP,
            ID_PHOTO_GRAY_BLOCK_BMP, 0
        },
        {
            //Item res string ID
            STRING_PHOTO_FAST, STRING_PHOTO_MIDDLE, STRING_PHOTO_SLOW,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
    },
    {
        //7, Scale
        4,//Item Count
        {
            //Item Size
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_TOP},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_BOTTOM},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_SP},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_H},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_H},
            {MENU_LIST_ITEM_W, MENU_LIST_ITEM_H},
            {0, 0}, {0, 0}
        },
        {
            //Item res bmp ID
            ID_PHOTO_BAR_TOP_BMP, ID_PHOTO_BAR_BOTTOM_BMP, ID_PHOTO_HORIZONTAL_LINE_BMP,
            ID_PHOTO_BAR_BMP, ID_PHOTO_BAR_SEL_N_BMP, ID_PHOTO_BAR_SEL_F_BMP,
            0, 0
        },
        {
            //Item res string ID
            STRING_PHOTO_AUTO, STRING_PHOTO_ORIGINAL, STRING_PHOTO_CROP,
            STRING_PHOTO_STRECTCH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
    },
};

static void photo_sset_init(photo_sset_ctrl_t *sset_ctrl)
{
    __s32 i;
    sset_ctrl->item_nr = sset_red_id[sset_ctrl->main_id].n_item;

    for (i = 0; i < sset_ctrl->item_nr; i++)
    {
        if (sset_red_id[sset_ctrl->main_id].id_string[i] == 0)
            continue;
        get_lang_res(sset_red_id[sset_ctrl->main_id].id_string[i],  sset_ctrl->sset_res.string[i]);
    }

    for (i = 0; i < 8; i++)
    {
        if (sset_red_id[sset_ctrl->main_id].id_icon[i] == 0)
            continue;
        create_bmp_res(sset_red_id[sset_ctrl->main_id].id_icon[i],  sset_ctrl->sset_res.icon[i]);
    }
}

static void photo_sset_uninit(photo_sset_ctrl_t *sset_ctrl)
{
    //remove for new res interface

    __s32 i;

    for (i = 0; i < 8; i++)
    {
        if (sset_red_id[sset_ctrl->main_id].id_icon[i] == 0)
            continue;

        destroy_bmp_res(sset_ctrl->sset_res.icon[i]);
    }

}
/*
	显示列表
*/
#define INTERVAL_TIME_STR_FORMAT     "%d%d"
static void photo_sset_paint_list(/*H_LYR layer, */photo_sset_ctrl_t *sset_ctrl)
{
#if _USE_MEMDEV_Handle_
    GUI_MEMDEV_Handle	draw_mem;
#endif
    photo_sset_res_t *sset_res;
    FB  fb;
    __s32 x, y, i;
    char buf[32];
    char long_str[32] = {0};
    RECT long_str_rt;

    DEBUG_photo("photo_sset_paint_list(%d, %d)\n", sset_ctrl->item_nr, sset_ctrl->focus_id);

    sset_res = &sset_ctrl->sset_res;

    GUI_LyrWinGetFB(sset_ctrl->sset_para->layer, &fb);

    GUI_LyrWinSel(sset_ctrl->sset_para->layer);
    GUI_SetFont(SWFFont);
    GUI_UC_SetEncodeUTF8();
#if _USE_MEMDEV_Handle_
    draw_mem = GUI_MEMDEV_Create(0, 0, fb.size.width, fb.size.height);
    GUI_MEMDEV_Select(draw_mem);
#endif
    GUI_SetBkColor(APP_COLOR_CLEAR);
    GUI_Clear();
    x = 0;
    y = 0;
    GUI_BMP_RES_Draw(sset_res->icon[ID_ICON_TOP], x, y);

    y = sset_red_id[sset_ctrl->main_id].res_size[0].height;

    for (i = 0; i < sset_ctrl->item_nr; i++)
    {
        //Draw item back
        if ((i == sset_ctrl->sset_para->value) && (i == sset_ctrl->focus_id))
            GUI_BMP_RES_Draw(sset_res->icon[ID_ICON_SEL_F], x,  y);
        else if (i == sset_ctrl->sset_para->value)
            GUI_BMP_RES_Draw(sset_res->icon[ID_ICON_SEL_N], x,  y);
        else
            GUI_BMP_RES_Draw(sset_res->icon[ID_ICON_BAR], x,  y);


        //draw string
        GUI_OpenAlphaBlend();
        if (i == sset_ctrl->focus_id)
        {
            GUI_SetColor(sset_ctrl->fcs_color);

            long_str_rt.x = x + MENU_LIST_STR_X_OFFSET;
            long_str_rt.y = y + MENU_LIST_STR_Y_OFFSET;
            long_str_rt.width  =  MENU_LIST_ITEM_W - long_str_rt.x;
            long_str_rt.height =  LONG_STR_H;

            if ((sset_ctrl->main_id == ID_PHOTO_MSET_INTERVAL) && (i == 3))
            {
                if (1 <= sset_ctrl->custom_interval && sset_ctrl->custom_interval <= 2)
                {
                    GUI_RECT rt_focus;
                    if (sset_ctrl->custom_interval == 1)
                    {
                        rt_focus.x0 = x + MENU_LIST_STR_X_OFFSET;
                    }
                    else
                    {
                        rt_focus.x0 = x + MENU_LIST_STR_X_OFFSET + PHOTO_FONT_BYTE_W;
                    }
                    rt_focus.x1 = rt_focus.x0 + PHOTO_FONT_BYTE_W;
                    rt_focus.y0 = y + 5;
                    rt_focus.y1 = rt_focus.y0 + PHOTO_FONT_H;
                    GUI_SetColor(COLOUR_TO_8BPP_INDEX_(1));
                    GUI_FillRect(rt_focus.x0, rt_focus.y0, rt_focus.x1, rt_focus.y1);
                    DEBUG_photo("rt_focus(%d, %d, %d, %d)\n", rt_focus.x0, rt_focus.y0, rt_focus.x1, rt_focus.y1);
                }
                app_sprintf(buf, INTERVAL_TIME_STR_FORMAT, sset_ctrl->sset_para->interval_time / 10,
                            sset_ctrl->sset_para->interval_time % 10);
                GUI_SetColor(sset_ctrl->fcs_color);
                GUI_DispStringAt(buf, x + MENU_LIST_STR_X_OFFSET, y + MENU_LIST_STR_Y_OFFSET);
                eLIBs_strcpy(long_str, buf);
            }
            else
            {
                GUI_DispStringAt(sset_res->string[i], x + MENU_LIST_STR_X_OFFSET, y + MENU_LIST_STR_Y_OFFSET);
                eLIBs_strcpy(long_str, sset_res->string[i]);
            }
        }
        else
        {
            GUI_SetColor(sset_ctrl->unfcs_color);
            if ((sset_ctrl->main_id == ID_PHOTO_MSET_INTERVAL) && (i == 3))
            {
                app_sprintf(buf, INTERVAL_TIME_STR_FORMAT, sset_ctrl->sset_para->interval_time / 10, sset_ctrl->sset_para->interval_time % 10);
                GUI_DispStringAt(buf, x + MENU_LIST_STR_X_OFFSET, y + MENU_LIST_STR_Y_OFFSET);
            }
            else
            {
                GUI_DispStringAt(sset_res->string[i], x + MENU_LIST_STR_X_OFFSET, y + MENU_LIST_STR_Y_OFFSET);
            }
        }
        GUI_CloseAlphaBlend();

        y += sset_red_id[sset_ctrl->main_id].res_size[3].height;
        //draw space line
        if (i != sset_ctrl->item_nr - 1)
        {
            GUI_BMP_RES_Draw(sset_res->icon[ID_ICON_SPLINE],  x,  y);
        }

        y += sset_red_id[sset_ctrl->main_id].res_size[2].height;
    }

    //draw bottom
    GUI_BMP_RES_Draw(sset_res->icon[ID_ICON_BOTTOM], x, y);

    GUI_OpenAlphaBlend();
    PhotoLongStrShow(sset_ctrl->sset_para->layer, &long_str_rt, long_str, sset_ctrl->fcs_color);
    GUI_CloseAlphaBlend();

#if _USE_MEMDEV_Handle_
    GUI_MEMDEV_CopyToLCD_Ex(draw_mem);
    GUI_MEMDEV_Select( NULL );
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
#endif
}
/*
	显示放大
*/
static __s32 photo_sset_paint_zoom(photo_sset_ctrl_t *sset_ctrl)
{
    photo_sset_res_t *sset_res;
    __s32 x, y;

    sset_res = &sset_ctrl->sset_res;

    GUI_LyrWinSel(sset_ctrl->sset_para->layer);

    x = SSET_ZOOM_BG_X;
    y = SSET_ZOOM_BG_Y;
    GUI_BMP_RES_Draw(sset_res->icon[0], x, y);
    x = x + 12;
    y = (sset_ctrl->item_nr - sset_ctrl->sset_para->value - 1) * 18 + 18 + y + 7;

    GUI_BMP_RES_Draw(sset_res->icon[1], x, y);
    return 0;
}

/*
	显示旋转
*/
static __s32 photo_sset_paint_rotate(photo_sset_ctrl_t *sset_ctrl)
{
    photo_sset_res_t *sset_res;

    sset_res = &sset_ctrl->sset_res;
    GUI_LyrWinSel(sset_ctrl->sset_para->layer);
    GUI_BMP_RES_Draw(sset_res->icon[sset_ctrl->sset_para->value], 0, 0);
    return 0;
}
/*
	显示亮度
*/
static __s32 photo_sset_paint_bright(photo_sset_ctrl_t *sset_ctrl)
{
#if _USE_MEMDEV_Handle_
    GUI_MEMDEV_Handle	draw_mem;
#endif
    photo_sset_res_t *sset_res;
    FB fb;
    char buf[32];
    GUI_RECT gui_rect;

    __s32 x, y, i;
    __s32 block_h = BRINGT_BLOCK_H;

    GUI_LyrWinGetFB(sset_ctrl->sset_para->layer, &fb);
    GUI_LyrWinSel(sset_ctrl->sset_para->layer);
    GUI_SetFont(SWFFont);
    GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
#if _USE_MEMDEV_Handle_
    draw_mem = GUI_MEMDEV_Create(0, 0, fb.size.width, fb.size.height);
    GUI_MEMDEV_Select(draw_mem);
#endif
    GUI_SetBkColor(APP_COLOR_CLEAR);
    GUI_Clear();

    sset_res = &sset_ctrl->sset_res;
    GUI_BMP_RES_Draw(sset_res->icon[0], 0, 0);
    gui_rect.x0 = 0;
    gui_rect.y0 = 0;
    gui_rect.x1 = sset_red_id[sset_ctrl->main_id].res_size[0].width;
    gui_rect.y1 = BRINGT_STR_Y1_OFFSET;
    app_sprintf(buf, "%d", sset_ctrl->focus_id);
    GUI_OpenAlphaBlend();
    GUI_DispStringInRect(buf, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    GUI_CloseAlphaBlend();

    x = BRINGT_BLOCK_X_OFFSET;
    y = BRINGT_BLOCK_Y_OFFSET;

    for (i = 0; i < sset_ctrl->focus_id; i++)
    {
        y -= (block_h + 1);
        GUI_BMP_RES_Draw(sset_res->icon[1], x, y);
    }

#if _USE_MEMDEV_Handle_
    GUI_MEMDEV_CopyToLCD_Ex(draw_mem);
    GUI_MEMDEV_Select( NULL );
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
#endif
    return 0;
}
/*
	disp slide show
*/
static __s32 photo_sset_paint_slideshow(photo_sset_ctrl_t *sset_ctrl)
{
#if _USE_MEMDEV_Handle_
    GUI_MEMDEV_Handle	draw_mem;
#endif
    photo_sset_res_t *sset_res;
    FB fb;
    char long_str[16 * 3] = {0};
    __s32 x, y, i;
    RECT long_str_rt;

    sset_res = &sset_ctrl->sset_res;
    GUI_LyrWinGetFB(sset_ctrl->sset_para->layer, &fb);
    GUI_LyrWinSel(sset_ctrl->sset_para->layer);
    GUI_UC_SetEncodeUTF8();
    GUI_SetFont(SWFFont);
#if _USE_MEMDEV_Handle_
    draw_mem = GUI_MEMDEV_Create(0, 0, fb.size.width, fb.size.height);
    GUI_MEMDEV_Select(draw_mem);
#endif
    GUI_SetBkColor(APP_COLOR_CLEAR);
    GUI_Clear();

    x = 0;
    y = 0;
    GUI_BMP_RES_Draw(sset_res->icon[0], 0, 0);
    x = 0;
    y = y + sset_red_id[sset_ctrl->main_id].res_size[0].height;

    for (i = sset_ctrl->start_item; i < sset_ctrl->end_item; i++)
    {
        //Draw item back
        y = sset_red_id[sset_ctrl->main_id].res_size[0].height
            + (i - sset_ctrl->start_item) * (sset_red_id[sset_ctrl->main_id].res_size[3].height
                                             + sset_red_id[sset_ctrl->main_id].res_size[2].height);

        if ((i == sset_ctrl->sset_para->value) && (i == sset_ctrl->focus_id))
            GUI_BMP_RES_Draw(sset_res->icon[ID_ICON_SEL_F], x,  y);
        else if (i == sset_ctrl->sset_para->value)
            GUI_BMP_RES_Draw(sset_res->icon[ID_ICON_SEL_N], x,  y);
        else
            GUI_BMP_RES_Draw(sset_res->icon[ID_ICON_BAR], x,  y);

        GUI_OpenAlphaBlend();

        //Draw string
        if (i == sset_ctrl->focus_id)
        {
            GUI_SetColor(sset_ctrl->fcs_color);
            GUI_DispStringAt(sset_res->string[i], x + MENU_LIST_STR_X_OFFSET, y + MENU_LIST_STR_Y_OFFSET);

            long_str_rt.x = x + MENU_LIST_STR_X_OFFSET;
            long_str_rt.y = y + MENU_LIST_STR_Y_OFFSET;
            long_str_rt.width  =  MENU_LIST_ITEM_W - long_str_rt.x;
            long_str_rt.height =  LONG_STR_H;

            eLIBs_strcpy(long_str, sset_res->string[i]);
        }
        else
        {
            GUI_SetColor(sset_ctrl->unfcs_color);
            GUI_DispStringAt(sset_res->string[i], x + MENU_LIST_STR_X_OFFSET, y + MENU_LIST_STR_Y_OFFSET);
        }
        GUI_CloseAlphaBlend();

        //Draw space line
        if (i == sset_ctrl->end_item - 1)
            GUI_BMP_RES_Draw(sset_res->icon[ID_ICON_SPLINE],  x,  y + sset_red_id[sset_ctrl->main_id].res_size[3].height);
    }

    GUI_BMP_RES_Draw(sset_res->icon[ID_ICON_BOTTOM], x, y + (sset_red_id[sset_ctrl->main_id].res_size[3].height));

    GUI_OpenAlphaBlend();
    PhotoLongStrShow(sset_ctrl->sset_para->layer, &long_str_rt, long_str, sset_ctrl->fcs_color);
    GUI_CloseAlphaBlend();

#if _USE_MEMDEV_Handle_
    GUI_MEMDEV_CopyToLCD_Ex(draw_mem);
    GUI_MEMDEV_Select( NULL );
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
#endif
    return EPDK_OK;
}
/*
	显示
*/
static __s32  photo_sset_paint(__gui_msg_t *msg)
{
    photo_sset_ctrl_t *sset_ctrl;
    H_LYR layer;

    layer = GUI_WinGetLyrWin(msg->h_deswin);
    if (GUI_LyrWinGetSta(layer) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    sset_ctrl = (photo_sset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    DEBUG_photo("    photo_sset_paint(%d)\n", sset_ctrl->main_id);

    switch(sset_ctrl->main_id)
    {
    //		case ID_PHOTO_MSET_SAVE_BG:
    case ID_PHOTO_MSET_BGMUSIC:
    case ID_PHOTO_MSET_INTERVAL:
    case ID_PHOTO_MSET_SCALE:
    {
        photo_sset_paint_list(/*layer, */sset_ctrl);
        break;
    }
    case ID_PHOTO_MSET_ZOOM:
    {
        photo_sset_paint_zoom(sset_ctrl);
        break;
    }
    case ID_PHOTO_MSET_ROTATE:
    {
        photo_sset_paint_rotate(sset_ctrl);
        break;
    }
    case ID_PHOTO_MSET_BRIGHT:
    {
        photo_sset_paint_bright(sset_ctrl);
        break;
    }
    case ID_PHOTO_MSET_SLIDESHOW:
    {
        photo_sset_paint_slideshow(sset_ctrl);
        break;
    }
    default:
        break;
    }

    return EPDK_OK;
}
/*
	op enter
*/
static __s32 photo_sset_op_enter(__gui_msg_t *msg)
{
    photo_sset_ctrl_t *sset_ctrl;

    sset_ctrl = (photo_sset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if ((sset_ctrl->main_id == ID_PHOTO_MSET_INTERVAL ) && (sset_ctrl->focus_id == 3) && (sset_ctrl->custom_interval == 0))
    {
        sset_ctrl->custom_interval = 1;
        sset_ctrl->sset_para->value = sset_ctrl->focus_id;
        photo_cmd2parent(msg->h_deswin, CMD_PHOTO_SSET_LR_CTRL, 1, 0);
        photo_sset_paint(msg);
        return EPDK_OK;
    }
    else if (sset_ctrl->main_id == ID_PHOTO_MSET_ZOOM || sset_ctrl->main_id == ID_PHOTO_MSET_ROTATE
             || sset_ctrl->main_id == ID_PHOTO_MSET_BRIGHT)
    {
        if (sset_ctrl->focus_id >= sset_ctrl->item_nr - 1)
            sset_ctrl->focus_id = 0;
        else
            sset_ctrl->focus_id++;

        if (sset_ctrl->main_id == ID_PHOTO_MSET_ZOOM)
        {
            if (sset_ctrl->focus_id == 0)
            {
                photo_cmd2parent(msg->h_deswin, CMD_PHOTO_SSET_LR_CTRL, 0, 0);
            }
            else
            {
                photo_cmd2parent(msg->h_deswin, CMD_PHOTO_SSET_LR_CTRL, 1, 0);
            }
        }
    }
    DEBUG_photo("=========focus = %d============\n", sset_ctrl->focus_id);
    sset_ctrl->sset_para->value = sset_ctrl->focus_id;
    if ((sset_ctrl->main_id == ID_PHOTO_MSET_INTERVAL ) && (sset_ctrl->focus_id == 3) && (sset_ctrl->custom_interval))
    {
        sset_ctrl->custom_interval  = 0;
        photo_cmd2parent(msg->h_deswin, CMD_PHOTO_SSET_LR_CTRL, 0, 0);
        photo_cmd2parent(msg->h_deswin, sset_ctrl->main_id, sset_ctrl->focus_id, sset_ctrl->sset_para->interval_time);
    }
    else
        photo_cmd2parent(msg->h_deswin, sset_ctrl->main_id, sset_ctrl->focus_id, 0);
    photo_sset_paint(msg);
    return EPDK_OK;
}

//设置自定义播放速度焦点
static void photo_sset_custom_interval_focus(photo_sset_ctrl_t *sset_ctrl)
{
    if (sset_ctrl->focus_id == 3 && sset_ctrl->custom_interval)
    {
        if (sset_ctrl->custom_interval == 1)
        {
            sset_ctrl->custom_interval = 2;
        }
        else
        {
            sset_ctrl->custom_interval = 1;
        }
    }
}

/*
	自定义播放速度
*/
static void photo_sset_custom_interval(photo_sset_ctrl_t *sset_ctrl, __u8 flag)
{
    __s32 tmp_time;

    tmp_time = sset_ctrl->sset_para->interval_time;

    if (flag)
    {
        if (sset_ctrl->custom_interval == 1)
        {
            tmp_time += 10;
            if (tmp_time >= MAX_CUSTOM_INTERVAL)
            {
                tmp_time = sset_ctrl->sset_para->interval_time % 10;
            }
        }
        else
        {
            tmp_time++;
            if (tmp_time >= MAX_CUSTOM_INTERVAL)
            {
                tmp_time = MIN_CUSTOM_INTERVAL;
            }
        }
    }
    else
    {
        if (sset_ctrl->custom_interval == 1)
        {
            tmp_time -= 10;
            if (tmp_time < MIN_CUSTOM_INTERVAL)
            {
                tmp_time = (MAX_CUSTOM_INTERVAL - 1) / 10 * 10 + sset_ctrl->sset_para->interval_time % 10;
            }
        }
        else
        {
            tmp_time--;
            if (tmp_time < MIN_CUSTOM_INTERVAL)
            {
                tmp_time = MAX_CUSTOM_INTERVAL - 1;
            }
        }
    }
    sset_ctrl->sset_para->interval_time = tmp_time;
    __msg("time  = %d\n", sset_ctrl->sset_para->interval_time);
}
/*
	op up
*/
static __s32 photo_sset_op_up(__gui_msg_t *msg)
{
    photo_sset_ctrl_t *sset_ctrl;

    sset_ctrl = (photo_sset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (sset_ctrl->main_id != ID_PHOTO_MSET_ZOOM && sset_ctrl->main_id != ID_PHOTO_MSET_ROTATE)
    {
        if (sset_ctrl->main_id ==  ID_PHOTO_MSET_INTERVAL &&  sset_ctrl->focus_id == 3 && sset_ctrl->custom_interval)
        {
            photo_sset_custom_interval_focus(sset_ctrl);//photo_sset_custom_interval(sset_ctrl, 1);
        }
        else if (sset_ctrl->main_id ==  ID_PHOTO_MSET_BRIGHT)
        {
            if (sset_ctrl->focus_id < sset_ctrl->item_nr - 1)
            {
                sset_ctrl->focus_id ++;
            }
            else
            {
                sset_ctrl->focus_id = 0;
            }
        }
        else
        {
            if (sset_ctrl->focus_id <= 0)
            {
                sset_ctrl->focus_id = sset_ctrl->item_nr - 1;
                if (sset_ctrl->main_id == ID_PHOTO_MSET_SLIDESHOW)
                {
                    sset_ctrl->end_item = sset_ctrl->item_nr;
                    sset_ctrl->start_item = sset_ctrl->end_item - sset_ctrl->page_num;
                }
            }
            else
            {
                sset_ctrl->focus_id--;
                if (sset_ctrl->main_id == ID_PHOTO_MSET_SLIDESHOW)
                {
                    if (sset_ctrl->focus_id < sset_ctrl->start_item)
                    {
                        sset_ctrl->start_item = sset_ctrl->focus_id;
                        sset_ctrl->end_item = sset_ctrl->start_item + sset_ctrl->page_num;
                    }
                }
            }
        }

        if (sset_ctrl->main_id == ID_PHOTO_MSET_BRIGHT)
        {
            sset_ctrl->sset_para->value = sset_ctrl->focus_id;
            photo_cmd2parent(msg->h_deswin, sset_ctrl->main_id, sset_ctrl->focus_id, 0);
        }
        photo_sset_paint(msg);
    }
    else if (sset_ctrl->main_id == ID_PHOTO_MSET_ZOOM)
    {
        photo_cmd2parent(msg->h_deswin, CMD_PHOTO_SSET_MOVE_UP, 0, 0);
    }
    DEBUG_photo("=************ focus = %d============\n", sset_ctrl->focus_id);
    return 0;
}

/*
	op down
*/
static __s32 photo_sset_op_down(__gui_msg_t *msg)
{
    photo_sset_ctrl_t *sset_ctrl;

    sset_ctrl = (photo_sset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (sset_ctrl->main_id != ID_PHOTO_MSET_ZOOM && sset_ctrl->main_id != ID_PHOTO_MSET_ROTATE)
    {
        if (sset_ctrl->main_id ==  ID_PHOTO_MSET_INTERVAL &&  sset_ctrl->focus_id == 3 && sset_ctrl->custom_interval)
        {
            photo_sset_custom_interval_focus(sset_ctrl);//photo_sset_custom_interval(sset_ctrl, 0);
        }
        else if (sset_ctrl->main_id ==  ID_PHOTO_MSET_BRIGHT)
        {
            if (sset_ctrl->focus_id > 1)
                sset_ctrl->focus_id --;
        }
        else
        {
            if (sset_ctrl->focus_id >= sset_ctrl->item_nr - 1)
            {
                sset_ctrl->focus_id = 0;
                if (sset_ctrl->main_id == ID_PHOTO_MSET_SLIDESHOW)
                {
                    sset_ctrl->start_item = sset_ctrl->focus_id;
                    sset_ctrl->end_item = sset_ctrl->start_item + sset_ctrl->page_num;
                }
            }
            else
            {
                sset_ctrl->focus_id++;
                if (sset_ctrl->main_id == ID_PHOTO_MSET_SLIDESHOW)
                {
                    if (sset_ctrl->focus_id >= sset_ctrl->end_item)
                    {
                        sset_ctrl->end_item = sset_ctrl->focus_id + 1;
                        sset_ctrl->start_item = sset_ctrl->end_item - sset_ctrl->page_num;
                    }
                }
            }
        }
        photo_sset_paint(msg);
        if (sset_ctrl->main_id == ID_PHOTO_MSET_BRIGHT)
        {
            sset_ctrl->sset_para->value = sset_ctrl->focus_id;
            photo_cmd2parent(msg->h_deswin, sset_ctrl->main_id, sset_ctrl->focus_id, 0);
        }
    }
    else if (sset_ctrl->main_id == ID_PHOTO_MSET_ZOOM)
    {
        photo_cmd2parent(msg->h_deswin, CMD_PHOTO_SSET_MOVE_DOWN, 0, 0);
    }
    return 0;
}
/*
	操作左
*/
static __s32 photo_sset_op_left(__gui_msg_t *msg)
{
    photo_sset_ctrl_t *sset_ctrl;

    sset_ctrl = (photo_sset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    if (sset_ctrl->main_id == ID_PHOTO_MSET_INTERVAL)
    {
        photo_sset_custom_interval(sset_ctrl, 0);//photo_sset_custom_interval_focus(sset_ctrl);
        photo_sset_paint(msg);
    }
    else if (sset_ctrl->main_id == ID_PHOTO_MSET_ZOOM)
    {
        photo_cmd2parent(msg->h_deswin, CMD_PHOTO_SSET_MOVE_LEFT, 0, 0);
    }
    return 0;
}
/*
	操作右
*/
static __s32 photo_sset_op_right(__gui_msg_t *msg)
{
    photo_sset_ctrl_t *sset_ctrl;

    sset_ctrl = (photo_sset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    if (sset_ctrl->main_id == ID_PHOTO_MSET_INTERVAL)
    {
        photo_sset_custom_interval(sset_ctrl, 1);//photo_sset_custom_interval_focus(sset_ctrl);
        photo_sset_paint(msg);
    }
    else if (sset_ctrl->main_id == ID_PHOTO_MSET_ZOOM)
    {
        photo_cmd2parent(msg->h_deswin, CMD_PHOTO_SSET_MOVE_RIGHT, 0, 0);
    }
    return 0;
}
/*
	回调函数
*/
static __s32 photo_sset_Proc(__gui_msg_t *msg)
{
    DEBUG_CBMSGEx(photo_sset_Proc);
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        photo_sset_ctrl_t *sset_ctrl;
        photo_sset_para_t *sset_para;

        sset_para = (photo_sset_para_t *)GUI_WinGetAttr(msg->h_deswin);
        BallocType(sset_ctrl, photo_sset_ctrl_t);
        eLIBs_memset(sset_ctrl, 0, sizeof(photo_sset_ctrl_t));
        sset_ctrl->sset_para = sset_para;
        sset_ctrl->main_id = sset_para->main_id;
        photo_sset_init(sset_ctrl);
        sset_ctrl->unfcs_color = APP_COLOR_WHITE;
        sset_ctrl->fcs_color = APP_COLOR_YELLOW;
        if (sset_ctrl->main_id == ID_PHOTO_MSET_ZOOM || sset_ctrl->main_id == ID_PHOTO_MSET_ROTATE
                || sset_ctrl->main_id == ID_PHOTO_MSET_BRIGHT
                || sset_ctrl->main_id == ID_PHOTO_MSET_BGMUSIC)
            sset_ctrl->focus_id = sset_ctrl->sset_para->value;
        else
            sset_ctrl->focus_id = 0;

        sset_ctrl->page_num = LIST_SLIDESHOW_ITEM_PAGE_COUNT;
        sset_ctrl->start_item = 0;
        sset_ctrl->end_item  = sset_ctrl->start_item + sset_ctrl->page_num;

        if ((sset_ctrl->main_id == ID_PHOTO_MSET_ZOOM) && (sset_ctrl->focus_id))
        {
            photo_cmd2parent(msg->h_deswin, CMD_PHOTO_SSET_LR_CTRL, 0, 0);
        }

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
        photo_sset_ctrl_t *sset_ctrl;
        sset_ctrl = (photo_sset_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        photo_sset_uninit(sset_ctrl);

        BFreeType(sset_ctrl->sset_para, photo_sset_para_t);
        BFreeType(sset_ctrl, photo_sset_ctrl_t);
    }
    break;
    case GUI_MSG_PAINT:
    {
        photo_sset_paint(msg);
    }
    break;
    case GUI_MSG_KEY:
        break;
    case GUI_MSG_COMMAND:
        break;
    case GUI_MSG_WIN_WAKEUP:
        GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
        break;
    case MSG_PHOTO_SSET_ENTER:
    {
        photo_sset_op_enter(msg);
        break;
    }
    case MSG_PHOTO_SSET_UP:
    {
        photo_sset_op_up(msg);
        break;
    }
    case MSG_PHOTO_SSET_DOWN:
    {
        photo_sset_op_down(msg);
        break;
    }
    case MSG_PHOTO_SSET_LEFT:
    {
        photo_sset_op_left(msg);
        break;
    }
    case MSG_PHOTO_SSET_RIGHT:
    {
        photo_sset_op_right(msg);
        break;
    }
    default:
        break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN photo_sset_win_create(H_WIN h_parent, photo_sset_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    photo_sset_para_t *sset_para;
    FB fb;

    BallocType(sset_para, photo_sset_para_t);
    eLIBs_memset(sset_para, 0, sizeof(photo_sset_para_t));

    eLIBs_memcpy(sset_para, para, sizeof(photo_sset_para_t));

    GUI_LyrWinGetFB(para->layer, &fb);

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =	"photo set win",
                  framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner	= NULL;
    framewin_para.id         = PHOTO_SSET_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)photo_sset_Proc);
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

__s32 photo_sset_win_delete(H_WIN sset_win)
{
    GUI_FrmWinDelete(sset_win);
    return EPDK_OK;
}



