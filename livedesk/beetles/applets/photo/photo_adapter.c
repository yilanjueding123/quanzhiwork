/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: photo_adapter.c
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/04/20 14:38  create this file, implements the fundamental interface;
**************************************************************************************************************
*/

#include "epdk.h"

#include "photo_adapter.h"

#define SAVE_REG_IMMEDIATELY    0

//////////////////////////////////////////////////////////////////////////
#if TEST_CODE_PHOTO_SETTING
TempForSaveData_T g_TempSaveData =
{
    4, //__s32 volume;
    PHOTO_SPEEDFLAG_MIDDLE, //__s32 speedflag;
    0, //__s32 ratio;
    4, //__s32 bright;
    0, //__s32 effect;
    PHOTO_BGMUSIC_OFF, //__s32 bgmusic;
    {'e', ':', '\\', '1', '.', 'b', 'i', 'n', 0}, //char  filename[128];
};
#endif
//////////////////////////////////////////////////////////////////////////
static __s32 switchmode[] =
{
    DEFAULT_SWITCH,             //默认无切换效果
    FADE_IN_OUT,                //淡入淡出
    PERSIAN_BLIND_H,            //水平百叶窗
    PERSIAN_BLIND_V,            //垂直百叶窗
    SLID_UP,                    //向上滑动
    SLID_DOWN,                  //向下滑动
    SLID_LEFT,                  //向左滑动
    SLID_RIGHT,	                //向右滑动
    STRETCH_UP,                 //向上展开
    STRETCH_DOWN,               //向下展开
    STRETCH_LEFT,               //向左展开
    STRETCH_RIGHT,              //向右展开
    MOSAIC,                     //马赛克
    ROOM_IN,                    //缩小
    ROOM_OUT,	                //放大
};

static __s32 rotate = 0;
//////////////////////////////////////////////////////////////////////////
////setting
//get data
__u32 _get_sys_bright(void)
{
    reg_system_para_t *para;

    get_reg_pointer(para, reg_system_para_t, REG_APP_SYSTEM);

    if (para)
    {
        return para->backlight;
    }
    else
    {
        __err("dsk_reg_get_para_by_app fail...\n");
        return 0;
    }
}

__u32 _get_sys_volume(void)
{
    reg_system_para_t *para;

    get_reg_pointer(para, reg_system_para_t, REG_APP_SYSTEM);

    if (para)
    {
        return para->volume;
    }
    else
    {
        __err("dsk_reg_get_para_by_app fail...\n");
        return 0;
    }
}

__u32 _get_photo_speedflag(void)
{
    reg_photo_para_t *para;

    get_reg_pointer(para, reg_photo_para_t, REG_APP_PHOTO);

    if (para)
    {
        return para->speedflag;
    }
    else
    {
        __err("dsk_reg_get_para_by_app fail...\n");
        return 0;
    }
}


__u32 _get_photo_ratio(void)
{
    reg_photo_para_t *para;

    get_reg_pointer(para, reg_photo_para_t, REG_APP_PHOTO);

    if (para)
    {
        return para->ratio;
    }
    else
    {
        __err("dsk_reg_get_para_by_app fail...\n");
        return 0;
    }
}

__u32 _get_photo_bgmusic(void)
{
    // 	reg_photo_para_t *para;
    //
    // 	get_reg_pointer(para, reg_photo_para_t, REG_APP_PHOTO);
    //
    // 	if (para)
    // 	{
    // 		return para->bgmusic;
    // 	}
    // 	else
    // 	{
    // 		__err("dsk_reg_get_para_by_app fail...\n");
    // 		return 0;
    // 	}
    return is_app_exist(APP_MUSIC);
}

__u32 _get_photo_effect(void)
{
    reg_photo_para_t *para;

    get_reg_pointer(para, reg_photo_para_t, REG_APP_PHOTO);

    if (para)
    {
        return para->effect;
    }
    else
    {
        __err("dsk_reg_get_para_by_app fail...\n");
        return 0;
    }
}

//set data
__u32 _set_sys_bright(__u32 backlight)
{
    reg_system_para_t *para;

    get_reg_pointer(para, reg_system_para_t, REG_APP_SYSTEM);

    para->backlight = backlight;

    //dsk_display_set_lcd_bright(__lion_bright_t);
#if SAVE_REG_IMMEDIATELY
    return update_to_reg();
#else
    return para->backlight;
#endif
}

__u32 _set_sys_volume(__u32 volume)
{
    reg_system_para_t *para;

    get_reg_pointer(para, reg_system_para_t, REG_APP_SYSTEM);

    para->volume = volume;

#if SAVE_REG_IMMEDIATELY
    return update_to_reg();
#else
    return para->volume;
#endif
}

__u32 _set_photo_bgmusic(__u32 bgmusic)
{
    reg_photo_para_t *para;

    get_reg_pointer(para, reg_photo_para_t, REG_APP_PHOTO);

    para->bgmusic = bgmusic;

#if SAVE_REG_IMMEDIATELY
    return update_to_reg();
#else
    return para->bgmusic;
#endif
}

__u32 _set_photo_ratio(__u32 ratio)
{
    reg_photo_para_t *para;

    get_reg_pointer(para, reg_photo_para_t, REG_APP_PHOTO);

    para->ratio = ratio;

#if SAVE_REG_IMMEDIATELY
    return update_to_reg();
#else
    return para->ratio;
#endif
}

__u32 _set_photo_speedflag(__u32 speedflag)
{
    reg_photo_para_t *para;

    get_reg_pointer(para, reg_photo_para_t, REG_APP_PHOTO);

    para->speedflag = speedflag;

#if SAVE_REG_IMMEDIATELY
    return update_to_reg();
#else
    return para->speedflag;
#endif
}

__u32 _set_photo_effect(__u32 effect)
{
    reg_photo_para_t *para;

    get_reg_pointer(para, reg_photo_para_t, REG_APP_PHOTO);

    para->effect = effect;

#if SAVE_REG_IMMEDIATELY
    return update_to_reg();
#else
    return para->effect;
#endif
}

__s32 photo_save_cur_play_info( rat_root_t root_type)
{
    __s32 index;
    char filename[MAX_FILE_NAME_LEN] = {0};

    index = anole_npl_get_cur();
    anole_npl_index2file(index, filename);
    DEBUG_photo("save file pos(%d, %s)\n", index, filename);

    return dsk_reg_save_cur_play_info(REG_APP_PHOTO, index, filename, root_type);
}

/////////////////////////////加载初始化数据
#define ANOLE_RandomMode_ID  BEETLES_TBL_SIZE(switchmode)
#define IsRandomMode(_id)   ((_id) >= ANOLE_RandomMode_ID)
__s32 PHOTO_LoadData(__anole_para_t *anole_para)
{
    __s32  val;

    anole_para->PlayOrder  = ANOLE_PLAY_NEXT;

    val = get_photo_speedflag();
    if (val == PHOTO_SPEEDFLAG_FAST)
        anole_para->interval = ANOLE_FAST;
    else if (val == PHOTO_SPEEDFLAG_MIDDLE)
        anole_para->interval = ANOLE_MIDDLE;
    else if (val == PHOTO_SPEEDFLAG_LOW)
        anole_para->interval = ANOLE_SLOW;
    else
        anole_para->interval = ANOLE_CUSTOM;

    anole_para->switchmodearray = switchmode;
    anole_para->switchnum = BEETLES_TBL_SIZE(switchmode);

    val = get_photo_effect();

    if (IsRandomMode(val))
    {
        anole_para->SwitchMode = ANOLE_FLAG_RANDOM_MODE;
    }
    else
    {
        anole_para->SwitchMode = val;
    }

    DEBUG_photo("anole_para: PlayOrder = %d, interval = %d, SwitchMode = %d\n",
                anole_para->PlayOrder, anole_para->interval, anole_para->SwitchMode);

    return val;
}
/////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
static __hdle g_pMoveStr = NULL;

__s32 PhotoLongStrShow(H_LYR hLayer, RECT *pRt, char *String, GUI_COLOR TxtClr)
{
    __show_info_move_t  str_move;

    __msg("PhotoLongStrShow(%x, [%d, %d, %d, %d], %s, %x)\n",
          hLayer, pRt->x, pRt->y, pRt->width, pRt->height, String, TxtClr);

    str_move.fontColor       = TxtClr;
    str_move.hlayer          = hLayer;
    str_move.string          = String;
    str_move.region.x        = pRt->x;
    str_move.region.y        = pRt->y;
    str_move.region.width    = pRt->width;
    str_move.region.height   = pRt->height;
    str_move.encode_id       = EPDK_CHARSET_ENM_UTF8;
    ASSERT(SWFFont);
    str_move.pFont           = SWFFont;
    str_move.bkColor         = APP_COLOR_CLEAR;
    str_move.align           = GUI_TA_LEFT | GUI_TA_TOP;
    str_move.bmp             = NULL;
    str_move.alpha_en        = 1;

    if (g_pMoveStr)
    {
        GUI_LongStringStop(g_pMoveStr);
        GUI_LongStringReset(g_pMoveStr, &str_move);
    }
    else
    {
        g_pMoveStr = GUI_LongStringCreate(&str_move);
        if (g_pMoveStr == NULL)
        {
            __wrn("create move handle failed!\n");
        }
    }
    GUI_LongStringStart(g_pMoveStr);

    return EPDK_OK;
}

void LongStrDestroy(void)
{
    if (g_pMoveStr)
    {
        GUI_LongStringDelete(g_pMoveStr);
        g_pMoveStr = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//for anole
#if 1//for atv anole
//for anole / ANOLE
__s32 ANOLE_Init(__anole_para_t *para)
{
    __s32 ret;
    DEBUG_CallFuncHere(anole_init);
    ret = anole_init();

    if (ret == 0)
    {
        //init ok
        anole_set_switch_mode_array_for_play(para->switchmodearray, para->switchnum);
        anole_set_switch_mode(para->SwitchMode);
        anole_set_play_order(para->PlayOrder);
        if (para->interval > 15)//for debug
        {
            para->interval = 15;
        }
        anole_set_play_interval(para->interval);
        //anole_set_show_mode(0);//temp
        ANOLE_PlayCurFile();
    }
    return ret;
}

__bool ANOLE_ShowErrorPic(__u8 *buffer, __s32 width, __s32 height)
{
    DEBUG_CallFuncHere(anole_show_pic_from_buffer);
    return anole_show_pic_from_buffer(buffer, width, height);
}

__u32 ANOLE_GetCurPicIndex(void)
{
    __u32 ret = anole_npl_get_cur();
    __msg(" Call %s() = %d\n", Symbol2Str(ANOLE_GetCurPicIndex), ret);
    return ret;
}

__u32 ANOLE_GetPicNum(void)
{
    __u32 ret = anole_npl_get_total_count();
    __msg(" Call %s() = %d\n", Symbol2Str(ANOLE_GetPicNum), ret);
    return ret;
}


__s32 ANOLE_GetPicInfo(__u32 index, __anole_pic_info_t *info)
{
    __s32 ret;
    DEBUG_CallFuncHere(anole_get_pic_info);
    if (info && info->name == NULL)
    {
        static char Buf[128];
        info->name = Buf;
        anole_npl_index2file(index, Buf);
    }
    ret = anole_get_pic_info(info);
    //////////////////////////////////////////////////////////////////////////
    {
        __msg("info(%s, %d, %d, %d) = %d\n", info->name, info->file_len, info->size.width, info->size.height, ret);
    }
    //////////////////////////////////////////////////////////////////////////
    return ret;
}

__anole_sta_t ANOLE_GetSta(void)
{
    //	DEBUG_CallFuncHere(ANOLE_GetSta);
    return anole_get_sta();
}

__bool ANOLE_Play(void)
{
    DEBUG_CallFuncHere(anole_auto_play);
    ANOLE_ResetRotate();
    return anole_auto_play();
}

__s32 ANOLE_PlayCurFile(void)
{
    __s32 ret;
    char Buf[FSYS_DIRNAME_MAX];
    ZeroMemory(Buf, sizeof(Buf));

    ret = anole_npl_get_cur();
    if(-1 == ret)
    {
        __msg("anole_npl_get_cur fail...\n");
        return EPDK_FAIL;
    }

    ret = anole_npl_index2file(ret, Buf);
    if (/*Buf[0] != 0 && */0 == ret)
    {
        anole_play_file(Buf);
        __msg("anole_play_file file=%s\n", Buf);
        ANOLE_ResetRotate();
    }
    else
    {
        __msg("ANOLE_PlayCurFile fail\n");
    }
    return ret;
}

__bool ANOLE_BackNormal(void)
{
    DEBUG_CallFuncHere(anole_backnormal);
    return anole_backnormal();
}

__bool ANOLE_SetShowMode(__s32 index)
{
    __bool ret;
    //DEBUG_CallFuncHere(ANOLE_SetShowMode);
    DEBUG_photo("ANOLE_SetShowMode(%d)\n", index);
    ret = anole_set_show_mode(index);
    //ret = 0;
    __here__;
    return ret;
}

__bool ANOLE_ShowPicByIndex(__u32 index)
{
    DEBUG_CallFuncHere(ANOLE_ShowPicByIndex);
    ANOLE_ResetRotate();
    return anole_play_file_by_index(index);
    // 	__s32 ret;
    // 	char Buf[MAX_FILE_NAME_LEN];
    //
    // 	ZeroMemory(Buf, sizeof(Buf));
    //
    // 	DEBUG_CallFuncHere(ANOLE_ShowPicByIndex);
    //
    // 	ret = anole_npl_index2file(index, Buf);
    // 	if (/*Buf[0] != 0 && */0 == ret)
    // 	{
    // 		__msg("anole_play_file file[%d] =%s\n", index, Buf);
    // 		ret = anole_play_file(Buf);
    // 		if (ret == EPDK_OK)
    // 		{
    // 			anole_npl_playing_seek(index);
    // 		}
    // 		else
    // 		{
    // 			__wrn("anole_play_file fail\n");
    // 		}
    // 	}
    // 	else
    // 	{
    // 		__msg("ANOLE_ShowPicByIndex fail\n");
    // 	}
    // 	return ret;
}

__bool ANOLE_Zoom(__s32 index)
{
    DEBUG_CallFuncHere(ANOLE_Zoom);

    DEBUG_photo("anole_zoom_set(%d)\n", index);

    return anole_zoom_set(index);;
}

__bool ANOLE_RotateRight(void)
{
    rotate = (rotate + 90) % 360;
    DEBUG_CallFuncHere(anole_rotate_right);
    return anole_rotate_right();
}

__s32 ANOLE_GetRotateId(void)
{
    return (rotate / 90);
}

void ANOLE_ResetRotate(void)
{
    rotate = 0;
}

__s32 ANOLE_SetSwitchMode(__u32 Id)
{
    if (IsRandomMode(Id))
    {
        Id = ANOLE_FLAG_RANDOM_MODE;
    }
    DEBUG_photo("anole_set_switch_mode (%d)\n", Id);

    return anole_set_switch_mode(Id);
}

__s32 ANOLE_SetPlayInterval(__u32 sec)
{
    DEBUG_CallFuncHere(anole_set_play_interval);
    return anole_set_play_interval(sec);
}

__u32 ANOLE_GetPlayInterval(void)
{
    DEBUG_CallFuncHere(anole_get_play_interval);
    return anole_get_play_interval();
}

__s32 ANOLE_GetSwitchMode(void)
{
    __s32 ret;

    ret = anole_get_switch_mode();

    if (ret == ANOLE_FLAG_RANDOM_MODE)
    {
        ret = ANOLE_RandomMode_ID;
    }

    DEBUG_photo("ANOLE_GetSwitchMode = %d\n", ret);

    return ret;
}

__s32 ANOLE_GetZoomIndex(void)
{
    DEBUG_CallFuncHere(ANOLE_GetZoomIndex);
    return anole_zoom_get();
}

void ANOLE_Exit(void)
{
    DEBUG_CallFuncHere(anole_exit);

    anole_exit();
}

__bool ANOLE_ScaleMove(__anole_diretion_t way)
{
    DEBUG_CallFuncHere(ANOLE_ScaleMove);
    return anole_scale_move(way);
}

#endif
//end of the file






