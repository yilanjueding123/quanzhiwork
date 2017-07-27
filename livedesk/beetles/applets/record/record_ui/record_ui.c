/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*
* File    	: record_ui.c
* By      	: Kingvan
* Func		: record view function
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  Kingvan  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include "record_ui.h"

//映射图片
static const res_mapping_info_t record_res_mapping[MENU_MAX] =
{
    {STRING_QUALITY_RC, ID_RECORD_MENU_QUALITY_0_BMP, ID_RECORD_MENU_QUALITY_1_BMP},
    {STRING_RECORD_RC, ID_RECORD_MENU_RECORD_0_BMP, ID_RECORD_MENU_RECORD_1_BMP},
    {STRING_SAVE_RC, ID_RECORD_MENU_SAVE_0_BMP, ID_RECORD_MENU_SAVE_1_BMP},
    {STRING_MUSCI_LIST_RC, ID_RECORD_MENU_LIST_0_BMP, ID_RECORD_MENU_LIST_1_BMP},
    {STRING_TYPE_RC, ID_RECORD_MENU_QUALITY_0_BMP, ID_RECORD_MENU_QUALITY_1_BMP}
};
static const res_mapping_info_t record_res_music_mapping[MUSIC_MENU_MAX] =
{
    {STRING_BACK_RC, ID_RECORD_BACK_1_BMP, ID_RECORD_BACK_0_BMP},
    {STRING_BACKWARD_RC, ID_RECORD_PRE_1_BMP, ID_RECORD_PRE_0_BMP},
    {STRING_PLAY_RC, ID_RECORD_PLAY_PAUSE_1_BMP, ID_RECORD_PLAY_PAUSE_0_BMP},
    {STRING_PAUSE_RC, ID_RECORD_PLAY_START_1_BMP, ID_RECORD_PLAY_START_0_BMP},
    {STRING_FORWARD_RC, ID_RECORD_NEXT_1_BMP, ID_RECORD_NEXT_0_BMP},
    {STRING_DELETE_RC, ID_RECORD_DEL_1_BMP, ID_RECORD_DEL_0_BMP}
};
static const GUI_POINT 			record_res_mapping_bmp_coor[MENU_MAX] =
{
    {12, 209}, {106, 209}, {200, 209}, {294, 209}

};
static const GUI_POINT 			record_res_music_mapping_bmp_coor[MUSIC_MENU_MAX] =
{
    {191, 75}, {231, 75}, {272, 75}, {272, 75}, {313, 75}, {354, 75}
};
static GUI_RECT					record_res_mapping_lang_rect[MENU_MAX] =
{
    {40, 215, 109, 240}, {145, 215, 198, 240}, {239, 215, 293, 240}, {333, 215, 387, 240},

};
static GUI_RECT					record_res_music_mapping_lang_rect[MUSIC_MENU_MAX] =
{
    {253, 102, 324, 120}, {253, 102, 324, 120}, {253, 102, 324, 120}, {253, 102, 324, 120}, {253, 102, 324, 120}, {253, 102, 324, 120}
};

//普通图片
static const __s32				record_res_bmp[MAX_RECORD_BMP_ITEM] =
{
    //数字图片
    ID_RECORD_0_BMP, ID_RECORD_1_BMP, ID_RECORD_2_BMP, ID_RECORD_3_BMP, ID_RECORD_4_BMP,
    ID_RECORD_5_BMP, ID_RECORD_6_BMP, ID_RECORD_7_BMP, ID_RECORD_8_BMP, ID_RECORD_9_BMP, ID_RECORD_10_BMP,
    //音乐播放进度
    ID_RECORD_66_BMP, ID_RECORD_66_LEFT_BMP, ID_RECORD_66_RIGHT_BMP, ID_RECORD_66_MID_BMP,
    //录音动画
    ID_RECORD_ANI_BG_BMP, ID_RECORD_ANI_0_BMP, ID_RECORD_ANI_1_BMP, ID_RECORD_ANI_2_BMP, ID_RECORD_ANI_3_BMP, ID_RECORD_ANI_4_BMP,
    //底部和顶部bar
    ID_RECORD_BAR_BOTTOM_BMP, ID_RECORD_BAR_TOP_BMP,
    //电池状态
    ID_RECORD_BATTERY_0_BMP, ID_RECORD_BATTERY_1_BMP, ID_RECORD_BATTERY_2_BMP, ID_RECORD_BATTERY_3_BMP, ID_RECORD_BATTERY_4_BMP, ID_RECORD_BATTERY_5_BMP,
    ID_RECORD_BATTERY_ADAPTER_BMP, ID_RECORD_BATTERY_ADD_BMP, ID_RECORD_BATTERY_NO_BMP,
    //音量标记
    ID_RECORD_VOL_OFF_BMP, ID_RECORD_VOL_ON_BMP,
    //删除对话框
    ID_RECORD_DEL_BOX_BMP, ID_RECORD_DEL_SEL_0_BMP, ID_RECORD_DEL_SEL_1_BMP,
    //质量对话框
    ID_RECORD_QUALITY_BK_BMP, ID_RECORD_QUALITY_FLAG_BMP,
    //录音格式
    ID_RECORD_FORMAT_BK_BMP,
    //音乐列表卷轴
    ID_RECORD_MUSIC_PROGRESS_BG_BMP, ID_RECORD_MUSIC_PROGRESS_DOWN_BMP,
    ID_RECORD_MUSIC_PROGRESS_FLAG_BMP, ID_RECORD_MUSIC_PROGRESS_UP_BMP, ID_RECORD_MUSIC_PROGRESS_FOCUS_BMP,
    //乐谱
    ID_RECORD_MUSIC_PARTICLE_BMP, ID_RECORD_MUSIC_BOUY_BMP,
    //录音停止, 录音开始, 录音暂停
    ID_RECORD_MENU_RECORD_2_BMP, ID_RECORD_MENU_RECORD_0_BMP, ID_RECORD_MENU_RECORD_1_BMP,
    //音量调节
    ID_RECORD_VOL_BAR_BG_BMP, ID_RECORD_VOL_BAR_BODY_BMP, ID_RECORD_VOL_LEFT_BMP,
    ID_RECORD_VOL_MID_BMP, ID_RECORD_VOL_RIGHT_BMP, ID_RECORD_VOL_SPEAKER_RIGHT_BMP

};
static const GUI_POINT 			record_res_bmp_coor[MAX_RECORD_BMP_ITEM] =
{
    //数字图片
    {122, 56}, {122, 56}, {122, 56}, {122, 56}, {122, 56},
    {122, 56}, {122, 56}, {122, 56}, {122, 56}, {122, 56}, {122, 56},
    //音乐播放进度
    {201, 185}, {201, 185}, {201, 185}, {201, 185},
    //录音动画
    {19, 148}, {19, 148}, {19, 148}, {19, 148}, {19, 148}, {19, 148},
    //底部和顶部bar
    {0, 205}, {0, 0},
    //电池状态
    {348, 4}, {348, 4}, {348, 4}, {348, 4}, {348, 4}, {348, 4},
    {348, 4}, {348, 4}, {348, 4},
    //音量标记
    {280, 4}, {280, 4},
    //删除对话框
    {101, 54}, {158, 124}, {158, 124},
    //质量对话框
    {5, 120}, {5, 183},
    //录音格式
    {5, 147},
    //音乐列表卷轴
    {163, 33}, {161, 230},
    {163, 33}, {161, 22}, {0, 33},
    //乐谱
    {0, 0}, {0, 0},
    //录音停止, 录音开始, 录音暂停
    {106, 209}, {106, 209}, {106, 209},
    //音量调节
    {0, 219}, {76, 228}, {76, 228},
    {76, 228}, {76, 228}, {335, 222}
};
static const __s32				record_res_lang[MAX_RECORD_LANG_ITEM] =
{
    //质量
    STRING_POOR_QUALITY_RC,
    STRING_NORMAL_QUALITY_RC,
    STRING_GOOD_QUALITY_RC,
    //录音机,播放机
    STRING_RECORDER_RC,
    STRING_RECORD_PLAYER_RC,
    //可录音时间
    STRING_RECORD_TIME_RC,
    //暂停,播放中..
    STRING_PAUSE_RC,
    STRING_PLAYING_RC,
    //停止
    STRING_STOP_RC,
    //删除对话框
    STRING_DELETE_RC,
    STRING_MSG4_RC,
    STRING_YES_RC,
    STRING_NO_RC,
    //录音停止, 录音开始, 录音暂停
    STRING_STOP_RC, STRING_RECORD_RC, STRING_PAUSE_RC,
    //无录音文件
    STRING_MSG3_RC

};
static GUI_RECT					record_res_lang_rect[MAX_RECORD_LANG_ITEM] =
{
    //质量
    {15, 181 - 4, 120, 198 - 4},
    {15, 154 - 4, 120, 172 - 4},
    {15, 130 - 4, 120, 148 - 4},
    //录音机,播放机
    {0, 0, 104, 18},
    {0, 0, 104, 18},
    //可录音时间
    {0, 23, 90, 48},
    //暂停,播放中..
    {255, 189, 325, 208},
    {255, 189, 325, 208},
    //停止
    {255, 189, 325, 205},
    //删除对话框
    {110, 58, 161, 76},
    {145, 80, 288, 112},
    {159, 124, 196, 141},
    {230, 123, 266, 142},
    //录音停止, 录音开始, 录音暂停
    {145, 215, 198, 240}, {145, 215, 198, 240}, {145, 215, 198, 240},
    //无录音文件
    {170, 169, 390, 203}
};

/*
	加载资源
*/
__s32 init_record_res( precord_ui_t pui )
{
    __u32 	i;

    com_set_palette_by_id(ID_RECORD_WKM_BMP);
    //预先加载映射图片
    for( i = 0; i < MENU_MAX; i++ )
    {
        pui->mapping_bmp[i].focus = NULL;
        pui->mapping_bmp[i].unfocus = NULL;
        pui->mapping_lang[i][0] = '\0';
    }
    for( i = 0; i < MUSIC_MENU_MAX; i++ )
    {
        pui->music_mapping_bmp[i].focus = NULL;
        pui->music_mapping_bmp[i].unfocus = NULL;
        pui->music_mapping_lang[i][0] = '\0';
    }
    //初始化一般图片
    for( i = 0; i < MAX_RECORD_BMP_ITEM; i++ )
    {
        pui->bmp[i] = NULL;
    }
    //初始化一般语言
    for( i = 0; i < MAX_RECORD_LANG_ITEM; i++ )
    {
        pui->lang[i][0] = '\0';
    }
    return EPDK_OK;
}

/*
	卸载资源
*/
__s32 uninit_record_res( precord_ui_t pui )
{
    __u32 	i;
    //释放映射图片
    for( i = 0; i < MENU_MAX; i++ )
    {
        if( pui->mapping_bmp[i].focus )
        {
            dsk_theme_close( pui->mapping_bmp[i].focus );
            pui->mapping_bmp[i].focus = NULL;
        }
        if( pui->mapping_bmp[i].unfocus )
        {
            dsk_theme_close( pui->mapping_bmp[i].unfocus );
            pui->mapping_bmp[i].unfocus = NULL;
        }
        pui->mapping_lang[i][0] = '\0';
    }
    for( i = 0; i < MUSIC_MENU_MAX; i++ )
    {
        if( pui->music_mapping_bmp[i].focus )
        {
            dsk_theme_close( pui->music_mapping_bmp[i].focus );
            pui->music_mapping_bmp[i].focus = NULL;
        }
        if( pui->music_mapping_bmp[i].unfocus )
        {
            dsk_theme_close( pui->music_mapping_bmp[i].unfocus );
            pui->music_mapping_bmp[i].unfocus = NULL;
        }
        pui->music_mapping_lang[i][0] = '\0';
    }
    //释放一般图片
    for( i = 0; i < MAX_RECORD_BMP_ITEM; i++ )
    {
        if( pui->bmp[i] )
        {
            dsk_theme_close( pui->bmp[i] );
            pui->bmp[i] = NULL;
        }
    }
    //释放一般语言
    for( i = 0; i < MAX_RECORD_LANG_ITEM; i++ )
    {
        pui->lang[i][0] = '\0';
    }

    return EPDK_OK;
}

/*
 *	绘画映射图片
 */
void draw_record_mapping( precord_ui_t pui, __s32 mappingIndex, FOCUSE_STATE focus, __s16 mappOffset )
{
    void *pbmp;
    GUI_COLOR mapping_txt_color;

    if( focus >= FOCUSE_STATE_MAX || mappingIndex >= MENU_MAX )
        return;

    if( focus == FOCUSE_NORMAL || focus == FOCUSE_OVER )
    {
        if( pui->mapping_bmp[mappingIndex].unfocus == NULL )
            pui->mapping_bmp[mappingIndex].unfocus = dsk_theme_open(record_res_mapping[mappingIndex].unfocus);
        if( focus == FOCUSE_NORMAL )
            mapping_txt_color = APP_COLOR_WHITE;
        else
            mapping_txt_color = APP_COLOR_YELLOW;

        pbmp = dsk_theme_hdl2buf(pui->mapping_bmp[mappingIndex].unfocus);

    }
    else
    {
        if( pui->mapping_bmp[mappingIndex].focus == NULL )
            pui->mapping_bmp[mappingIndex].focus = dsk_theme_open(record_res_mapping[mappingIndex].focus);
        mapping_txt_color = APP_COLOR_YELLOW;

        pbmp = dsk_theme_hdl2buf(pui->mapping_bmp[mappingIndex].focus);
    }
    GUI_BMP_Draw( pbmp, record_res_mapping_bmp_coor[mappingIndex + mappOffset].x, record_res_mapping_bmp_coor[mappingIndex + mappOffset].y );

    if( pui->mapping_lang[mappingIndex][0] == '\0' )
        dsk_langres_get_menu_text(record_res_mapping[mappingIndex].lang, pui->mapping_lang[mappingIndex], GUI_NAME_MAX);
    GUI_SetColor(mapping_txt_color);
    GUI_DispStringInRect(pui->mapping_lang[mappingIndex], &record_res_mapping_lang_rect[mappingIndex + mappOffset], GUI_TA_HCENTER | GUI_TA_VCENTER);

}
void draw_record_music_mapping( precord_ui_t pui, __s32 mappingIndex, FOCUSE_STATE focus )
{
    void *pbmp;

    if( focus >= FOCUSE_STATE_MAX || mappingIndex >= MUSIC_MENU_MAX )
        return;

    if( focus == FOCUSE_NORMAL )
    {
        if( pui->music_mapping_bmp[mappingIndex].unfocus == NULL )
            pui->music_mapping_bmp[mappingIndex].unfocus = dsk_theme_open(record_res_music_mapping[mappingIndex].unfocus);
        pbmp = dsk_theme_hdl2buf(pui->music_mapping_bmp[mappingIndex].unfocus);
    }
    else if( focus == FOCUSE_OVER || focus == FOCUSE_UP )
    {
        if( pui->music_mapping_bmp[mappingIndex].focus == NULL )
            pui->music_mapping_bmp[mappingIndex].focus = dsk_theme_open(record_res_music_mapping[mappingIndex].focus);
        pbmp = dsk_theme_hdl2buf(pui->music_mapping_bmp[mappingIndex].focus);

        if( pui->music_mapping_lang[mappingIndex][0] == '\0' )
            dsk_langres_get_menu_text(record_res_music_mapping[mappingIndex].lang, pui->music_mapping_lang[mappingIndex], GUI_NAME_MAX);
        GUI_SetColor(APP_COLOR_WHITE);
        GUI_DispStringInRect(pui->music_mapping_lang[mappingIndex], &record_res_music_mapping_lang_rect[mappingIndex], GUI_TA_HCENTER | GUI_TA_VCENTER);
    }
    else
    {
        if( pui->music_mapping_bmp[mappingIndex].unfocus == NULL )
            pui->music_mapping_bmp[mappingIndex].unfocus = dsk_theme_open(record_res_music_mapping[mappingIndex].unfocus);
        pbmp = dsk_theme_hdl2buf(pui->music_mapping_bmp[mappingIndex].unfocus);

        if( pui->music_mapping_lang[mappingIndex][0] == '\0' )
            dsk_langres_get_menu_text(record_res_music_mapping[mappingIndex].lang, pui->music_mapping_lang[mappingIndex], GUI_NAME_MAX);
        GUI_SetColor(APP_COLOR_WHITE);
        GUI_DispStringInRect(pui->music_mapping_lang[mappingIndex], &record_res_music_mapping_lang_rect[mappingIndex], GUI_TA_HCENTER | GUI_TA_VCENTER);
    }
    GUI_BMP_Draw( pbmp, record_res_music_mapping_bmp_coor[mappingIndex].x, record_res_music_mapping_bmp_coor[mappingIndex].y );

}
/*
 * 清除映射图片
 */
void clean_record_mapping( precord_ui_t pui, __s32 mappingIndex )
{
    void *pbmp;
    __s16 x0, y0, x1, y1;

    if( pui->mapping_bmp[mappingIndex].focus )
    {
        pbmp = dsk_theme_hdl2buf(pui->mapping_bmp[mappingIndex].focus);
        x0 = record_res_mapping_bmp_coor[mappingIndex].x;
        y0 = record_res_mapping_bmp_coor[mappingIndex].y;
        x1 = x0 + GUI_BMP_GetXSize(pbmp);
        y1 = y0 + GUI_BMP_GetYSize(pbmp);

        GUI_ClearRect(x0, y0, x1, y1);
        dsk_theme_close( pui->mapping_bmp[mappingIndex].focus );
        pui->mapping_bmp[mappingIndex].focus = NULL;
    }

    if( pui->mapping_bmp[mappingIndex].unfocus )
    {
        pbmp = dsk_theme_hdl2buf(pui->mapping_bmp[mappingIndex].unfocus);
        x0 = record_res_mapping_bmp_coor[mappingIndex].x;
        y0 = record_res_mapping_bmp_coor[mappingIndex].y;
        x1 = x0 + GUI_BMP_GetXSize(pbmp);
        y1 = y0 + GUI_BMP_GetYSize(pbmp);

        GUI_ClearRect(x0, y0, x1, y1);
        dsk_theme_close( pui->mapping_bmp[mappingIndex].unfocus );
        pui->mapping_bmp[mappingIndex].unfocus = NULL;
    }
}
void clean_record_music_mapping( precord_ui_t pui, __s32 mappingIndex )
{
    void *pbmp;
    __s16 x0, y0, x1, y1;

    if( pui->music_mapping_bmp[mappingIndex].focus )
    {
        pbmp = dsk_theme_hdl2buf(pui->music_mapping_bmp[mappingIndex].focus);
        x0 = record_res_music_mapping_bmp_coor[mappingIndex].x;
        y0 = record_res_music_mapping_bmp_coor[mappingIndex].y;
        x1 = x0 + GUI_BMP_GetXSize(pbmp);
        y1 = y0 + GUI_BMP_GetYSize(pbmp);

        GUI_ClearRect(x0, y0, x1, y1);
        dsk_theme_close( pui->music_mapping_bmp[mappingIndex].focus );
        pui->music_mapping_bmp[mappingIndex].focus = NULL;
    }
    if( pui->music_mapping_bmp[mappingIndex].unfocus )
    {
        pbmp = dsk_theme_hdl2buf(pui->music_mapping_bmp[mappingIndex].unfocus);
        x0 = record_res_music_mapping_bmp_coor[mappingIndex].x;
        y0 = record_res_music_mapping_bmp_coor[mappingIndex].y;
        x1 = x0 + GUI_BMP_GetXSize(pbmp);
        y1 = y0 + GUI_BMP_GetYSize(pbmp);

        GUI_ClearRect(x0, y0, x1, y1);
        dsk_theme_close( pui->music_mapping_bmp[mappingIndex].unfocus );
        pui->music_mapping_bmp[mappingIndex].unfocus = NULL;
    }
}

/*
 *	绘画普通图片和字串
 */
void draw_record_bmp( precord_ui_t pui, __s32 index )
{
    void *pbmp;

    if( pui->bmp[index] == NULL )
        pui->bmp[index] = dsk_theme_open(record_res_bmp[index]);

    pbmp = dsk_theme_hdl2buf(pui->bmp[index]);

    GUI_BMP_Draw( pbmp, record_res_bmp_coor[index].x, record_res_bmp_coor[index].y );
}
void draw_record_bmp_ext( precord_ui_t pui, __s32 index, __s16 offsetX, __s16 offsetY )
{
    void *pbmp;

    if( pui->bmp[index] == NULL )
        pui->bmp[index] = dsk_theme_open(record_res_bmp[index]);

    pbmp = dsk_theme_hdl2buf(pui->bmp[index]);

    GUI_BMP_Draw( pbmp, record_res_bmp_coor[index].x + offsetX, record_res_bmp_coor[index].y + offsetY );
}
void draw_record_bmp_coor( precord_ui_t pui, __s32 index, __s16 X, __s16 Y )
{
    void *pbmp;

    if( pui->bmp[index] == NULL )
        pui->bmp[index] = dsk_theme_open(record_res_bmp[index]);

    pbmp = dsk_theme_hdl2buf(pui->bmp[index]);

    GUI_BMP_Draw( pbmp, X, Y );
}
void draw_record_bmp_cut( precord_ui_t pui, __s32 index, __s16 xCoor, __s16 yCoor, __s16 height )
{
    void *pbmp;
    __s16 x0, y0, x1, y1;

    if( pui->bmp[index] == NULL )
        pui->bmp[index] = dsk_theme_open(record_res_bmp[index]);

    pbmp = dsk_theme_hdl2buf(pui->bmp[index]);

    x0 = xCoor;
    y0 = yCoor;
    x1 = x0 + GUI_BMP_GetXSize(pbmp) - 1;
    y1 = y0 + (GUI_BMP_GetYSize(pbmp) - height) - 1;

    GUI_BMP_Draw( pbmp, xCoor, yCoor );
    GUI_ClearRect(x0, y0, x1, y1);
}
void draw_record_lang( precord_ui_t pui, __s32 index, __s32 fnt_color )
{
    GUI_SetColor(fnt_color);

    if( pui->lang[index][0] == '\0' )
        dsk_langres_get_menu_text(record_res_lang[index], pui->lang[index], RECORD_NAME_MAX);
    GUI_DispStringInRect(pui->lang[index], &record_res_lang_rect[index], GUI_TA_HCENTER | GUI_TA_VCENTER);
}
void draw_record_lang_wrap( precord_ui_t pui, __s32 index, __s32 fnt_color )
{
    GUI_SetColor(fnt_color);

    if( pui->lang[index][0] == '\0' )
        dsk_langres_get_menu_text(record_res_lang[index], pui->lang[index], RECORD_NAME_MAX);
    GUI_DispStringInRectWrap(pui->lang[index], &record_res_lang_rect[index], GUI_TA_HCENTER | GUI_TA_VCENTER, GUI_WRAPMODE_WORD);
}

void draw_record_lang_ext( precord_ui_t pui, __s32 index, __s32 fnt_color, GUI_RECT *rect )
{
    GUI_SetColor(fnt_color);

    if( pui->lang[index][0] == '\0' )
        dsk_langres_get_menu_text(record_res_lang[index], pui->lang[index], RECORD_NAME_MAX);
    GUI_DispStringInRect(pui->lang[index], rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
}
void draw_record_str( precord_ui_t pui, char *str, __s32 fnt_color, GUI_RECT *rect )
{
    GUI_SetColor(fnt_color);

    GUI_DispStringInRect(str, rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
}
/*
 *	清除普通图片和字串
 */
void clean_record_bmp( precord_ui_t pui, __s32 index )
{
    void *pbmp;
    __s16 x0, y0, x1, y1;

    if(pui->bmp[index] == NULL)
        return;

    pbmp = dsk_theme_hdl2buf(pui->bmp[index]);
    x0 = record_res_bmp_coor[index].x;
    y0 = record_res_bmp_coor[index].y;
    x1 = x0 + GUI_BMP_GetXSize(pbmp);
    y1 = y0 + GUI_BMP_GetYSize(pbmp);

    GUI_ClearRect(x0, y0, x1, y1);

    dsk_theme_close( pui->bmp[index] );
    pui->bmp[index] = NULL;
}
void clean_record_lang( precord_ui_t pui, __s32 index )
{
    GUI_ClearRectEx(&record_res_lang_rect[index]);
}
void clean_record_str( precord_ui_t pui, const GUI_RECT *rect )
{
    GUI_ClearRectEx(rect);
}

__s32 flush_record_res( precord_ui_t pui )
{
    __u32 	i;
    //flush映射图片
    for( i = 0; i < MENU_MAX; i++ )
    {
        clean_record_mapping(pui, i);
    }
    for( i = 0; i < MUSIC_MENU_MAX; i++ )
    {
        clean_record_music_mapping(pui, i);
    }
    //flush一般图片
    for( i = 0; i < MAX_RECORD_BMP_ITEM; i++ )
    {
        clean_record_bmp(pui, i);
    }
    //flush一般字符
    for( i = 0; i < MAX_RECORD_LANG_ITEM; i++ )
    {
        clean_record_lang(pui, i);
    }
    return EPDK_OK;
}



























