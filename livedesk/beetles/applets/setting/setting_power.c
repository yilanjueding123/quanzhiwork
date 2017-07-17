/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: setting_power.c
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "app_setting_i.h"
#include "setting_power.h"
#include "setting_para.h"

#define SET_BMP_BSYSSET_W 131
#define SET_BMP_BSYSSET_H 116

#define SET_BMP_SETICON00_W 28
#define SET_BMP_SETICON00_H 21

#define SET_BMP_BGSEL_W 244
#define SET_BMP_BGSEL_H 24

#define SET_BMP_ARR_W 18
#define SET_BMP_ARR_H 24
typedef enum
{
    BACKLIGHTOFF_NEVER,
    BACKLIGHTOFF_10SECOND,
    BACKLIGHTOFF_30SECOND,
    BACKLIGHTOFF_60SECOND,
    BACKLIGHTOFF_90SECOND,
    BACKLIGHTOFF_MAX,
} backlightoff_t;
typedef enum
{
    POWEROFF_NEVER,
    POWEROFF_10MINUTE,
    POWEROFF_20MINUTE,
    POWEROFF_30MINUTE,
    POWEROFF_60MINUTE,
    POWEROFF_MAX,
} poweroff_t;

typedef struct tag_setting_item_id
{
    __s32 id_string_detail_prompt;
    __s32 id_string_detail_prompt_ex;
    __s32 id_string_title;
    __s32 id_string_content;
    __s32  id_bmp_left;
    __s32  id_bmp_right;
} setting_item_id_t;

typedef struct tag_setting_item_res
{
    char string_title[128];
    OFFSET  string_title_pos;
    char string_content[128];
    RECT  string_content_rect;

    H_THEME h_bmp_focus;
    OFFSET  bmp_focus_pos;
    H_THEME h_bmp_unfocus;
    OFFSET  bmp_unfocus_pos;
    H_THEME h_bmp_left;
    OFFSET  bmp_left_pos;
    H_THEME h_bmp_right;
    OFFSET  bmp_right_pos;

    __s32 content_num;
    __s32 content_nr;
} setting_item_res_t;

typedef struct tag_setting_focus_item_id
{
    __s32  id_string_detail_prompt;
    __s32  id_bmp_focus_item;
} setting_focus_item_id_t;

typedef struct tag_setting_power_attr
{
    H_THEME  h_bmp_big_setting;
    H_THEME  h_bmp_bottom;
    H_THEME  h_bmp_prompt;

    char  string_prompt[128];

    char  string_detail_prompt[128];
    char string_detail_prompt_ex[128];

    setting_item_res_t  res_backlight_level;
    setting_item_res_t res_backlight_off;
    setting_item_res_t res_power_off;
    setting_item_res_t res_screensaver;

    __s32	focus_txt_color;
    __s32	unfocus_txt_color;

    H_WIN	layer;
    GUI_FONT *font;

    __s32 new_focus;
    __s32 old_focus;

    __s32 item_nr;
    setting_power_para_t *power_para;
    set_power_para *power_uipara;
} setting_power_attr_t;
extern void  main_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);

static reg_system_para_t *setting_reg_para;

static __s32 content_backlight_level_id[] = //ITEM1 (背光亮度)
{
    STRING_SET_POWER_BGT_LEVEL1_CON,
    STRING_SET_POWER_BGT_LEVEL2_CON,
    STRING_SET_POWER_BGT_LEVEL3_CON,
    STRING_SET_POWER_BGT_LEVEL4_CON,
    STRING_SET_POWER_BGT_LEVEL5_CON
    //STRING_SET_POWER_BGT_LEVEL6_CON,
    //STRING_SET_POWER_BGT_LEVEL7_CON,
    //STRING_SET_POWER_BGT_LEVEL8_CON,
    //STRING_SET_POWER_BGT_LEVEL9_CON,
    //STRING_SET_POWER_BGT_LEVEL10_CON,
    //STRING_SET_POWER_BGT_LEVEL11_CON,
    //STRING_SET_POWER_BGT_LEVEL12_CON,
    //STRING_SET_POWER_BGT_LEVEL13_CON,
    //STRING_SET_POWER_BGT_LEVEL14_CON,
    //STRING_SET_POWER_BGT_LEVEL15_CON

};

static __s32 content_backlight_off_id[] = //ITEM2 (关闭背光)
{
    STRING_SET_POWER_BGT_NEVER_CON,
    STRING_SET_POWER_BGT_LAST10S_CON,
    STRING_SET_POWER_BGT_LAST30S_CON,
    STRING_SET_POWER_BGT_LAST60S_CON,
    STRING_SET_POWER_BGT_LAST90S_CON
};

static __s32 content_power_off_id[] = //ITEM3 (关闭电源)
{
    STRING_SET_POWER_OFF_NEVER_CON,
    STRING_SET_POWER_OFF_LAST10M_CON,		//10 分
    STRING_SET_POWER_OFF_LAST20M_CON,		// 20 分
    STRING_SET_POWER_OFF_LAST30M_CON,		// 30 分
    STRING_SET_POWER_OFF_LAST1H_CON			// 1 小时
};

static setting_focus_item_id_t setting_focus_item_res_id[] =
{
    { STRING_SET_POWER_BGT_EX, ID_SETTING_SET_ITEM_F_BMP},	// 0
    { STRING_SET_POWER_BGT_OFF_EX, ID_SETTING_SET_ITEM_F_BMP},	// 1
    { STRING_SET_POWER_OFF_EX, ID_SETTING_SET_ITEM_F_BMP},		// 2
};

static setting_item_id_t setting_power_res_id[] =
{
    {STRING_SET_POWER_BGT_EX, STRING_SET_POWER_BGT_DE, STRING_SET_POWER_BGT_TITLE, STRING_SET_POWER_BGT_LEVEL1_CON, ID_SETTING_SET_BUTT_LF_BMP, ID_SETTING_SET_BUTT_RF_BMP},	// 0
    {STRING_SET_POWER_BGT_OFF_EX, STRING_SET_POWER_BGT_OFF_DE, STRING_SET_POWER_BGT_OFF_TITLE, STRING_SET_POWER_BGT_NEVER_CON, ID_SETTING_SET_BUTT_LF_BMP, ID_SETTING_SET_BUTT_RF_BMP},	// 1
    {STRING_SET_POWER_OFF_EX, STRING_SET_POWER_OFF_DE, STRING_SET_POWER_OFF_TITLE, STRING_SET_POWER_OFF_NEVER_CON, ID_SETTING_SET_BUTT_LF_BMP, ID_SETTING_SET_BUTT_RF_BMP},		// 2
};

/*
	申请资源
*/
static void _setting_power_res_init(setting_power_attr_t *power_attr)
{
    __u32 	i;
    setting_item_res_t  *p_item_res;

    power_attr->h_bmp_big_setting = theme_open(ID_SETTING_SET_BSYSSET_BMP);
    power_attr->h_bmp_prompt = theme_open(ID_SETTING_SET_SETICON00_BMP);
    power_attr->h_bmp_bottom = theme_open(ID_SETTING_SET_BOTTOM_BMP);

    get_menu_text(STRING_SET_PROMPT_POWER_MANAGER, power_attr->string_prompt, 128);
    get_menu_text(setting_power_res_id[power_attr->new_focus].id_string_detail_prompt, power_attr->string_detail_prompt, 128);
    get_menu_text(setting_power_res_id[power_attr->new_focus].id_string_detail_prompt_ex, power_attr->string_detail_prompt_ex, 128);

    power_attr->item_nr = sizeof(setting_power_res_id) / sizeof(setting_power_res_id[0]);

    for (i = 0; i < power_attr->item_nr; i++)
    {
        switch (i)
        {
        case 0:
            p_item_res = &power_attr->res_backlight_level;
            if(setting_reg_para)
            {
                if( (setting_reg_para->backlight > 15) || (setting_reg_para->backlight < 0))
                {
                    p_item_res->content_num = 10;
                }
                else
                {
                    p_item_res->content_num = setting_reg_para->backlight;
                }
            }
            else
            {
                p_item_res->content_num = 10;
            }
            get_menu_text(content_backlight_level_id[p_item_res->content_num], p_item_res->string_content, 128);
            p_item_res->content_nr = sizeof(content_backlight_level_id) / sizeof(__s32);
            break;
        case 1:
        {
            p_item_res = &power_attr->res_backlight_off;
            if(setting_reg_para)
            {
                switch(setting_reg_para->backlightoff)
                {
                case 0:
                    p_item_res->content_num = 0;
                    break;
                case 10:
                    p_item_res->content_num = 1;
                    break;
                case 30:
                    p_item_res->content_num = 2;
                    break;
                case 60:
                    p_item_res->content_num = 3;
                    break;
                case 90:
                    p_item_res->content_num = 4;
                    break;
                default:
                    p_item_res->content_num = 0;
                    break;
                }
            }
            else
            {
                p_item_res->content_num = 0;
            }

            get_menu_text(content_backlight_off_id[p_item_res->content_num], p_item_res->string_content, 128);
            p_item_res->content_nr = sizeof(content_backlight_off_id) / sizeof(__s32);
        }
        break;
        case 2:
        {
            p_item_res = &power_attr->res_power_off;
            if(setting_reg_para)
            {
                switch(setting_reg_para->poweroff)
                {
                case 0:
                    p_item_res->content_num = 0;
                    break;
                case 10*60:
                    p_item_res->content_num = 1;
                    break;
                case 20*60:
                    p_item_res->content_num = 2;
                    break;
                case 30*60:
                    p_item_res->content_num = 3;
                    break;
                case 60*60:
                    p_item_res->content_num = 4;
                    break;
                default:
                    p_item_res->content_num = 0;
                    break;
                }
            }
            else
            {
                p_item_res->content_num = 0;
            }

            get_menu_text(content_power_off_id[p_item_res->content_num], p_item_res->string_content, 128);
            p_item_res->content_nr = sizeof(content_power_off_id) / sizeof(__s32);
        }
        break;
        default:
            break;
        }

        p_item_res->string_content_rect = power_attr->power_uipara->item[i].item_string_content_rect;
        p_item_res->string_title_pos = power_attr->power_uipara->item[i].item_string_title_pos;

        p_item_res->bmp_left_pos = power_attr->power_uipara->item[i].item_bmp_left_pos;
        p_item_res->bmp_right_pos = power_attr->power_uipara->item[i].item_bmp_right_pos;
        p_item_res->bmp_focus_pos = power_attr->power_uipara->item[i].item_bmp_focus_pos;

        p_item_res->h_bmp_left = theme_open(setting_power_res_id[i].id_bmp_left);
        p_item_res->h_bmp_right = theme_open(setting_power_res_id[i].id_bmp_right);

        get_menu_text(setting_power_res_id[i].id_string_title, p_item_res->string_title, 128);

    }



}

static void _setting_power_res_uninit(setting_power_attr_t *power_attr)
{
    __u32 	i;
    setting_item_res_t  *p_item_res;

    theme_close(power_attr->h_bmp_big_setting);
    theme_close(power_attr->h_bmp_prompt);
    theme_close(power_attr->h_bmp_bottom);
    for (i = 0; i < power_attr->item_nr; i++)
    {
        switch (i)
        {
        case 0:
            p_item_res = &power_attr->res_backlight_level;
            break;
        case 1:
            p_item_res = &power_attr->res_backlight_off;
            break;
        case 2:
            p_item_res = &power_attr->res_power_off;
            break;
        default:
            break;
        }

        theme_close(p_item_res->h_bmp_focus);
        theme_close(p_item_res->h_bmp_left);
        theme_close(p_item_res->h_bmp_right);
    }
}

static void _setting_power_focus_res_reset(setting_power_attr_t *power_attr)
{
    setting_item_res_t  *p_item_res;

    switch (power_attr->new_focus)
    {
    case 0:
        p_item_res = &power_attr->res_backlight_level;
        break;
    case 1:
        p_item_res = &power_attr->res_backlight_off;
        break;
    case 2:
        p_item_res = &power_attr->res_power_off;
        break;
    default:
        break;
    }

    get_menu_text(setting_focus_item_res_id[power_attr->new_focus].id_string_detail_prompt, power_attr->string_detail_prompt, 128);
    get_menu_text(setting_power_res_id[power_attr->new_focus].id_string_detail_prompt_ex, power_attr->string_detail_prompt_ex, 128);

    if(p_item_res->h_bmp_focus)
    {
        theme_close(p_item_res->h_bmp_focus);
        p_item_res->h_bmp_focus = theme_open(setting_focus_item_res_id[power_attr->new_focus].id_bmp_focus_item);
    }
    else
    {

        p_item_res->h_bmp_focus = theme_open(setting_focus_item_res_id[power_attr->new_focus].id_bmp_focus_item);
    }

}

static void _setting_power_content_res_reset(setting_power_attr_t *power_attr)
{
    __u32 	i, *content_id;
    setting_item_res_t  *p_item_res;

    switch (power_attr->new_focus)
    {
    case 0:
        p_item_res = &power_attr->res_backlight_level;
        content_id = content_backlight_level_id;
        break;
    case 1:
        p_item_res = &power_attr->res_backlight_off;
        content_id = content_backlight_off_id;
        break;
    case 2:
        p_item_res = &power_attr->res_power_off;
        content_id = content_power_off_id;
        break;
    default:
        break;
    }

    get_menu_text(content_id[p_item_res->content_num], p_item_res->string_content, 128);

}

static __s32 setting_power_content_paint(__gui_msg_t *msg)
{
    setting_power_attr_t *power_attr;
    setting_item_res_t  *p_item_res;
    RECT rect;
    GUI_RECT gui_rect;

    power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (power_attr == NULL)
        return EPDK_FAIL;

    if (GUI_LyrWinGetSta(power_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    GUI_LyrWinSel(power_attr->layer);
    GUI_SetFont(power_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_LyrWinCacheOn();
    GUI_OpenAlphaBlend();
    GUI_LyrWinGetScnWindow(power_attr->layer, &rect);

    _setting_power_content_res_reset(power_attr);

    switch (power_attr->new_focus)
    {
    case 0:
        p_item_res = &power_attr->res_backlight_level;
        break;
    case 1:
        p_item_res = &power_attr->res_backlight_off;
        break;
    case 2:
        p_item_res = &power_attr->res_power_off;
        break;
    default:
        break;
    }

    GUI_SetBkColor(0x00);
    GUI_ClearRect(p_item_res->string_content_rect.x, p_item_res->string_content_rect.y, p_item_res->string_content_rect.x + p_item_res->string_content_rect.width, p_item_res->string_content_rect.y + p_item_res->string_content_rect.height);

    gui_rect.x0 = SET_CONT_START_X;
    gui_rect.y0 = SET_CONT_START_Y + SET_CONT_H * power_attr->new_focus;
    gui_rect.x1 = gui_rect.x0 + SET_CONT_W;
    gui_rect.y1 = gui_rect.y0 + SET_CONT_H;

    GUI_BMP_Draw(theme_hdl2buf(p_item_res->h_bmp_focus), p_item_res->bmp_focus_pos.x, p_item_res->bmp_focus_pos.y);
    GUI_BMP_Draw(theme_hdl2buf(p_item_res->h_bmp_left), p_item_res->bmp_left_pos.x, p_item_res->bmp_left_pos.y);
    GUI_BMP_Draw(theme_hdl2buf(p_item_res->h_bmp_right), p_item_res->bmp_right_pos.x, p_item_res->bmp_right_pos.y);
    GUI_SetColor(power_attr->focus_txt_color);
    GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x, p_item_res->string_title_pos.y);
    GUI_DispStringInRect(p_item_res->string_content, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    GUI_CloseAlphaBlend();
    GUI_LyrWinCacheOff();

    return EPDK_OK;
}

static __s32 setting_power_item_paint(__gui_msg_t *msg)
{
    setting_power_attr_t *power_attr;
    setting_item_res_t  *p_item_res;
    __s32 i;
    RECT rect;
    GUI_RECT gui_rect;

    power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (power_attr == NULL)
        return EPDK_FAIL;

    if (GUI_LyrWinGetSta(power_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    GUI_LyrWinSel(power_attr->layer);
    GUI_SetFont(power_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_LyrWinCacheOn();
    GUI_OpenAlphaBlend();
    GUI_LyrWinGetScnWindow(power_attr->layer, &rect);

    _setting_power_focus_res_reset(power_attr);

    GUI_SetColor(power_attr->unfocus_txt_color);
    GUI_ClearRect(power_attr->power_uipara->string_detail_prompt_rect.x, power_attr->power_uipara->string_detail_prompt_rect.y, power_attr->power_uipara->string_detail_prompt_rect.x + power_attr->power_uipara->string_detail_prompt_rect.width - 1, power_attr->power_uipara->string_detail_prompt_rect.y + power_attr->power_uipara->string_detail_prompt_rect.height - 1);
    GUI_DispStringAt(power_attr->string_detail_prompt, power_attr->power_uipara->string_detail_prompt_rect.x, power_attr->power_uipara->string_detail_prompt_rect.y);
    gui_rect.x0 = power_attr->power_uipara->string_detail_prompt_rect_ex.x;
    gui_rect.y0 = power_attr->power_uipara->string_detail_prompt_rect_ex.y;
    gui_rect.x1 = gui_rect.x0 + power_attr->power_uipara->string_detail_prompt_rect_ex.width;
    gui_rect.y1 = gui_rect.y0 + power_attr->power_uipara->string_detail_prompt_rect_ex.height;
    GUI_ClearRectEx(&gui_rect);
    GUI_DispStringInRect(power_attr->string_detail_prompt_ex, &gui_rect, GUI_TA_LEFT);


    for(i = 0; i < power_attr->item_nr; i++)
    {
        switch (i)
        {
        case 0:
            p_item_res = &power_attr->res_backlight_level;
            break;
        case 1:
            p_item_res = &power_attr->res_backlight_off;
            break;
        case 2:
            p_item_res = &power_attr->res_power_off;
            break;
        default:
            break;
        }

        if(i == power_attr->new_focus)
        {
            setting_power_content_paint(msg);
        }
        else if(i == power_attr->old_focus)
        {
            gui_rect.x0 = SET_ITEM_START_X;
            gui_rect.y0 = SET_ITEM_START_Y + SET_ITEM_H * i;
            gui_rect.x1 = gui_rect.x0 + SET_ITEM_W;
            gui_rect.y1 = gui_rect.y0 + SET_ITEM_H;
            GUI_ClearRectEx(&gui_rect);

            GUI_BMP_Draw(theme_hdl2buf(p_item_res->h_bmp_unfocus), p_item_res->bmp_unfocus_pos.x, p_item_res->bmp_unfocus_pos.y);
            GUI_SetColor(power_attr->unfocus_txt_color);
            GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x, p_item_res->string_title_pos.y);
            gui_rect.x0 = SET_CONT_START_X;
            gui_rect.y0 = SET_CONT_START_Y + SET_CONT_H * i;
            gui_rect.x1 = gui_rect.x0 + SET_CONT_W;
            gui_rect.y1 = gui_rect.y0 + SET_CONT_H;
            GUI_DispStringInRect(p_item_res->string_content, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

        }
    }
    GUI_CloseAlphaBlend();
    GUI_LyrWinCacheOff();

    return EPDK_OK;
}

static __s32 setting_power_paint(__gui_msg_t *msg)
{
    setting_power_attr_t *power_attr;
    setting_item_res_t  *p_item_res;
    __s32 i;
    RECT rect;
    GUI_RECT gui_rect;

    power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (power_attr == NULL)
        return EPDK_FAIL;

    if (GUI_LyrWinGetSta(power_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    GUI_LyrWinSel(power_attr->layer);
    GUI_SetFont(power_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);

    GUI_LyrWinCacheOn();
    GUI_OpenAlphaBlend();
    GUI_LyrWinGetScnWindow(power_attr->layer, &rect);

    GUI_BMP_Draw(theme_hdl2buf(power_attr->h_bmp_big_setting), power_attr->power_uipara->bmp_big_setting_pos.x, power_attr->power_uipara->bmp_big_setting_pos.y);

    GUI_SetBkColor(0x00);
    GUI_ClearRect(power_attr->power_uipara->toolbar_rect.x, power_attr->power_uipara->toolbar_rect.y, power_attr->power_uipara->toolbar_rect.x + power_attr->power_uipara->toolbar_rect.width, power_attr->power_uipara->toolbar_rect.y + power_attr->power_uipara->toolbar_rect.height);
    GUI_BMP_Draw(theme_hdl2buf(power_attr->h_bmp_bottom), power_attr->power_uipara->toolbar_rect.x, power_attr->power_uipara->toolbar_rect.y);

    GUI_BMP_Draw(theme_hdl2buf(power_attr->h_bmp_prompt), power_attr->power_uipara->bmp_prompt_rect.x, power_attr->power_uipara->bmp_prompt_rect.y);
    GUI_SetColor(power_attr->unfocus_txt_color);
    GUI_DispStringAt(power_attr->string_prompt, power_attr->power_uipara->string_prompt_pos.x , power_attr->power_uipara->string_prompt_pos.y);

    gui_rect.x0 = power_attr->power_uipara->string_detail_prompt_rect.x;
    gui_rect.y0 = power_attr->power_uipara->string_detail_prompt_rect.y;
    gui_rect.x1 = gui_rect.x0 + power_attr->power_uipara->string_detail_prompt_rect.width;
    gui_rect.y1 = gui_rect.y0 + power_attr->power_uipara->string_detail_prompt_rect.height;
    GUI_ClearRectEx(&gui_rect);
    GUI_DispStringInRect(power_attr->string_detail_prompt, &gui_rect, GUI_TA_LEFT);

    gui_rect.x0 = power_attr->power_uipara->string_detail_prompt_rect_ex.x;
    gui_rect.y0 = power_attr->power_uipara->string_detail_prompt_rect_ex.y;
    gui_rect.x1 = gui_rect.x0 + power_attr->power_uipara->string_detail_prompt_rect_ex.width;
    gui_rect.y1 = gui_rect.y0 + power_attr->power_uipara->string_detail_prompt_rect_ex.height;
    GUI_ClearRectEx(&gui_rect);
    GUI_DispStringInRect(power_attr->string_detail_prompt_ex, &gui_rect, GUI_TA_LEFT);

    GUI_SetBkColor(0x00);

    for(i = 0; i < power_attr->item_nr; i++)
    {
        switch (i)
        {
        case 0:
            p_item_res = &power_attr->res_backlight_level;
            break;
        case 1:
            p_item_res = &power_attr->res_backlight_off;
            break;
        case 2:
            p_item_res = &power_attr->res_power_off;
            break;
        default:
            break;
        }

        if(i == power_attr->new_focus)
        {
            setting_power_content_paint(msg);
        }
        else
        {
            gui_rect.x0 = SET_ITEM_START_X;
            gui_rect.y0 = SET_ITEM_START_Y + SET_ITEM_H * i;
            gui_rect.x1 = gui_rect.x0 + SET_ITEM_W;
            gui_rect.y1 = gui_rect.y0 + SET_ITEM_H;
            GUI_ClearRectEx(&gui_rect);
            GUI_SetColor(power_attr->unfocus_txt_color);

            GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x, p_item_res->string_title_pos.y);
            gui_rect.x0 = SET_CONT_START_X;
            gui_rect.y0 = SET_CONT_START_Y + SET_CONT_H * i;
            gui_rect.x1 = gui_rect.x0 + SET_CONT_W;
            gui_rect.y1 = gui_rect.y0 + SET_CONT_H;
            GUI_DispStringInRect(p_item_res->string_content, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
    }

    GUI_CloseAlphaBlend();
    GUI_LyrWinCacheOff();
    return EPDK_OK;
}

static __s32  setting_power_key_proc(__gui_msg_t *msg)
{
    setting_power_attr_t *power_attr;
    static __s32 last_key = -1;

    power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if(KEY_UP_ACTION == msg->dwAddData2)
    {

        if(last_key == GUI_MSG_KEY_ENTER)
        {

            main_cmd2parent(msg->h_deswin, ID_OP_ENTER, power_attr->new_focus, 0);
        }
    }
    else
    {
        switch(msg->dwAddData1)
        {
        case GUI_MSG_KEY_LEFT:	//作up键使用
        case GUI_MSG_KEY_LONGLEFT:
        {
            main_cmd2parent(msg->h_deswin, ID_OP_UP, power_attr->new_focus, 0);
        }
        break;
        case GUI_MSG_KEY_RIGHT:	//作down键使用
        case GUI_MSG_KEY_LONGRIGHT:
        {
            main_cmd2parent(msg->h_deswin, ID_OP_DOWN, power_attr->new_focus, 0);
        }
        break;
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LONGUP:
        {
            main_cmd2parent(msg->h_deswin, ID_OP_UP, power_attr->new_focus, 0);
        }
        break;
        case GUI_MSG_KEY_LONGDOWN:
        case GUI_MSG_KEY_DOWN:
        {
            main_cmd2parent(msg->h_deswin, ID_OP_DOWN, power_attr->new_focus, 0);
        }
        break;

        case GUI_MSG_KEY_ESCAPE:
        case GUI_MSG_KEY_MENU:
        case GUI_MSG_KEY_VDEC:
        {
            main_cmd2parent(msg->h_deswin, ID_OP_EXIT, power_attr->new_focus, 0);
        }
        break;
        default :
            break;
        }
    }

    if(KEY_UP_ACTION == msg->dwAddData2)
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
static __s32 _setting_power_Proc(__gui_msg_t *msg)
{
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        setting_power_attr_t *power_attr;
        setting_power_para_t *power_para;
        RECT rect;
        SIZE screen_size;

        setting_reg_para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

        power_para = (setting_power_para_t *)GUI_WinGetAttr(msg->h_deswin);

        power_attr = (setting_power_attr_t *)My_Balloc(sizeof(setting_power_attr_t));
        if(!power_attr)
        {

            __msg("memory balloc fail.........\n");
            return EPDK_FAIL;
        }
        eLIBs_memset(power_attr, 0, sizeof(setting_power_attr_t));

        power_attr->power_para = power_para;
        power_attr->new_focus = 0;
        power_attr->old_focus = 0;

        dsk_display_get_size(&screen_size.width, &screen_size.height);
        power_attr->power_uipara = setting_get_power_para(screen_size );
        _setting_power_res_init(power_attr);
        _setting_power_focus_res_reset(power_attr);
        _setting_power_content_res_reset(power_attr);

        power_attr->focus_txt_color = APP_COLOR_YELLOW;
        power_attr->unfocus_txt_color = APP_COLOR_WHITE;
        power_attr->font = power_para->font;
        power_attr->layer = power_para->layer;

        GUI_WinSetAddData(msg->h_deswin, (__u32)power_attr);
        GUI_LyrWinSetSta(power_attr->layer, GUI_LYRWIN_STA_ON);
    }
    return EPDK_OK;
    case GUI_MSG_CLOSE:
    {
        GUI_FrmWinDelete(msg->h_deswin);
    }
    return EPDK_OK;
    case GUI_MSG_DESTROY:
    {
        setting_power_attr_t *power_attr;
        setting_power_para_t *power_para;
        power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);
        if(!power_attr)
        {

            __err("power_attr is null ......\n");
            return EPDK_FAIL;
        }

        _setting_power_res_uninit(power_attr);

        //dsk_reg_flush();

        power_para = power_attr->power_para;
        if(!power_para)
        {

            __err("power_para is null ......\n");
            return EPDK_FAIL;
        }

        if(power_para)
        {

            My_Bfree(power_para, sizeof(setting_power_para_t));
        }
        if(power_attr)
        {

            My_Bfree(power_attr, sizeof(setting_power_attr_t));
        }
    }
    return EPDK_OK;
    case GUI_MSG_PAINT:
        setting_power_paint(msg);
        return EPDK_OK;
    case GUI_MSG_KEY:
        setting_power_key_proc(msg);
        return EPDK_OK;
    case GUI_MSG_TOUCH:
        break;
    case MSG_OP_UP:
    {
        setting_power_attr_t *power_attr;
        power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);

        power_attr->old_focus = power_attr->new_focus;
        if (power_attr->new_focus <= 0)
            power_attr->new_focus = power_attr->item_nr - 1;
        else
            power_attr->new_focus--;
        setting_power_item_paint( msg);
    }
    return EPDK_OK;
    case MSG_OP_DOWN:
    {
        setting_power_attr_t *power_attr;
        power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);

        power_attr->old_focus = power_attr->new_focus;
        if (power_attr->new_focus >= power_attr->item_nr - 1)
            power_attr->new_focus = 0;
        else
            power_attr->new_focus++;
        setting_power_item_paint( msg);
    }
    return EPDK_OK;
    case MSG_OP_LEFT:
    {
        setting_power_attr_t *power_attr;
        power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);

        power_attr->old_focus = power_attr->new_focus;
        if (power_attr->new_focus <= 0)
            power_attr->new_focus = power_attr->item_nr - 1;
        else
            power_attr->new_focus--;
        setting_power_item_paint( msg);
    }
    return EPDK_OK;
    case MSG_OP_RIGHT:
    {
        setting_power_attr_t *power_attr;
        power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);

        power_attr->old_focus = power_attr->new_focus;
        if (power_attr->new_focus >= power_attr->item_nr - 1)
            power_attr->new_focus = 0;
        else
            power_attr->new_focus++;
        setting_power_item_paint( msg);
    }
    return EPDK_OK;

    case MSG_OP_ENTER:
    {
        setting_power_attr_t *power_attr;
        setting_item_res_t  *p_item_res;

        power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);

        switch (power_attr->new_focus)
        {
        case 0:
            p_item_res = &power_attr->res_backlight_level;
            break;
        case 1:
            p_item_res = &power_attr->res_backlight_off;
            break;
        case 2:
            p_item_res = &power_attr->res_power_off;
            break;
        default:
            break;
        }

        if (p_item_res->content_num >= p_item_res->content_nr - 1)
            p_item_res->content_num = 0;
        else
            p_item_res->content_num++;
        setting_power_content_paint( msg);
        switch (power_attr->new_focus)
        {
        case 0:
        {
            if(setting_reg_para)
            {
                setting_reg_para->backlight = p_item_res->content_num;
            }
            dsk_display_set_lcd_bright((__lion_bright_t)(p_item_res->content_num));
            break;
        }
        case 1:
        {
            __u32  close_scn_time ;
            switch(p_item_res->content_num)
            {
            case BACKLIGHTOFF_NEVER:
                g_set_close_scn_time(0);
                close_scn_time = 0;
                break;
            case BACKLIGHTOFF_10SECOND:
                g_set_close_scn_time(10);//10s
                close_scn_time = 10;
                break;
            case BACKLIGHTOFF_30SECOND:
                g_set_close_scn_time(30);// 30s
                close_scn_time = 30;
                break;
            case BACKLIGHTOFF_60SECOND:
                g_set_close_scn_time(60);
                close_scn_time = 60;
                break;
            case BACKLIGHTOFF_90SECOND:
                g_set_close_scn_time(90);
                close_scn_time = 90;
                break;
            default:
                g_set_close_scn_time(0);
                close_scn_time = 0;
                break;
            }
            if(setting_reg_para)
            {
                setting_reg_para->backlightoff = close_scn_time;
            }
        }
        break;
        case 2:
        {
            __u32  auto_off_time ;
            switch(p_item_res->content_num)
            {
            case POWEROFF_NEVER:
                dsk_set_auto_off_time(0);
                auto_off_time = 0;
                break;
            case POWEROFF_10MINUTE:
                dsk_set_auto_off_time(10 * 60);
                auto_off_time = 10 * 60;
                break;
            case POWEROFF_20MINUTE:
                dsk_set_auto_off_time(20 * 60);
                auto_off_time = 20 * 60;
                break;
            case POWEROFF_30MINUTE:
                dsk_set_auto_off_time(30 * 60);
                auto_off_time = 30 * 60;
                break;
            case POWEROFF_60MINUTE:
                dsk_set_auto_off_time(60 * 60);
                auto_off_time = 60 * 60;
                break;
            default:
                dsk_set_auto_off_time(0);
                auto_off_time = 0;
                break;
            }
            if(setting_reg_para)
            {
                setting_reg_para->poweroff = auto_off_time;
            }
        }
        break;
        default:
            break;
        }
    }
    return EPDK_OK;

    case MSG_OP_EXIT:
    {
        __gui_msg_t my_msg;
        my_msg.id = GUI_MSG_CLOSE;
        my_msg.h_srcwin = msg->h_deswin;
        my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
        my_msg.dwAddData1 = 0;
        my_msg.dwAddData2 = 0;
        GUI_SendMessage(&my_msg);
    }
    return EPDK_OK;

    case GUI_MSG_COMMAND:
        break;

    default:
        break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN setting_power_win_create(H_WIN h_parent, setting_power_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    setting_power_para_t *power_para;
    FB fb;

    GUI_LyrWinGetFB(para->layer, &fb);

    power_para = (setting_power_para_t *)My_Balloc(sizeof(setting_power_para_t));
    if(!power_para)
    {

        __msg("memory balloc fail.........\n");
        return EPDK_FAIL;
    }
    eLIBs_memset((void *)power_para, 0, sizeof(setting_power_para_t));
    power_para->font = para->font;
    power_para->layer = para->layer;
    power_para->focus_id = para->focus_id;

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =	"power setting win",
                  framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner	= NULL;
    framewin_para.id         = POWER_MANAGE_MENU_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_setting_power_Proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr =  (void *)power_para;
    framewin_para.hLayer = para->layer;

    return (GUI_FrmWinCreate(&framewin_para));
}


__s32 setting_power_win_delete(H_WIN list_win)
{
    GUI_FrmWinDelete(list_win);
    return EPDK_OK;
}
