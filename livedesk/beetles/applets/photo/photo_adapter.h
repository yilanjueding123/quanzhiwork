/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: photo_adapter.h
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/04/20 14:38  create this file, implements the fundamental interface;
**************************************************************************************************************
*/

#ifndef _PHOTO_ADAPTER_H__093AD91A_4E53_49E3_AF29_AB1C6070A320_
#define _PHOTO_ADAPTER_H__093AD91A_4E53_49E3_AF29_AB1C6070A320_
//_093AD91A_4E53_49E3_AF29_AB1C6070A320_
#include "epdk.h"
#include "beetles_app.h"
//#include "../Lib/res/res_decode.h"

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//for debug
#define DEBUG_photo                            __msg

#define DEBUG_SYSMEM()                         NULL//esKSRV_SysInfo()

#if 0 //debug ui

//temp//if(!(0<=(_x) && (_x)<=400 && 0<=(_y) && (_y)<= 240))
//re define GUI_BMP_Draw
#ifdef GUI_BMP_Draw
#define MY_GUI_BMP_DRAW                      (ORANGEMOD_FUNC_ENTRY->ptr_GUI_BMP_Draw         	)
#undef GUI_BMP_Draw
#define GUI_BMP_Draw(_p, _x, _y)             do \
{\
	{__msg("    GUI_BMP_Draw(%x)->(%d, %d)[%d, %d]\n", _p, _x, _y, RES_BMP_DATA_W(_p), RES_BMP_DATA_H(_p));} \
	MY_GUI_BMP_DRAW(_p, _x, _y); \
}while(0)
#endif

//re define GUI_DispStringAt
#ifdef GUI_DispStringAt
#define MY_GUI_DispStringAt                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringAt       	)
#undef GUI_DispStringAt
#define GUI_DispStringAt(_p, _x, _y)         \
do\
{ \
	{__msg("    GUI_DispStringAt(%d, %d)(%s)\n", _x, _y, _p);} \
	MY_GUI_DispStringAt(_p, _x, _y); \
}while(0)
#endif

//re define GUI_DispStringInRect
#ifdef GUI_DispStringInRect
#define MY_GUI_DispStringInRect              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringInRect   	)
#undef GUI_DispStringInRect
#define GUI_DispStringInRect(_p, _prt, _fmt)  \
do \
{ \
	{__msg("    GUI_DispStringInRect(%d, %d, %d, %d)(%s)\n", (_prt)->x0, (_prt)->y0, (_prt)->x1, (_prt)->y1, _p);} \
	MY_GUI_DispStringInRect(_p, _prt, _fmt); \
} while (0)
#endif

#endif

///////////////////adapter/////////////////

//////new for bmp res
typedef load_bmp_res_t                         PHOTO_BMP_RES_T;

//////////////////////////////////////////////////////////////////////////


/////////macro , enum
typedef enum
{
    PHOTO_BGMUSIC_OFF,
    PHOTO_BGMUSIC_ON,
    PHOTO_BGMUSIC_MAX,
} photo_bgmusic_t;

typedef enum
{
    PHOTO_SPEEDFLAG_FAST,
    PHOTO_SPEEDFLAG_MIDDLE,
    PHOTO_SPEEDFLAG_LOW,
    PHOTO_SPEEDFLAG_MANNUAL,
    PHOTO_SPEEDFLAG_MAX,
} photo_speedflag_t;

#define APP_COLOR_CLEAR                        0x0f
//////////////////////////////////////////////////////////////////////////
//////////////////////////////function
////setting
//get data
extern __u32 _get_sys_bright(void);
extern __u32 _get_sys_volume(void);
extern __u32 _get_photo_speedflag(void);
extern __u32 _get_photo_ratio(void);
extern __u32 _get_photo_bgmusic(void);
extern __u32 _get_photo_effect(void);

//set data
extern __u32 _set_sys_bright(__u32 backlight);
extern __u32 _set_sys_volume(__u32 volume);
extern __u32 _set_photo_bgmusic(__u32 bgmusic);
extern __u32 _set_photo_ratio(__u32 ratio);
extern __u32 _set_photo_speedflag(__u32 speedflag);
extern __u32 _set_photo_effect(__u32 effect);

#define TEST_CODE_PHOTO_SETTING 0
#if TEST_CODE_PHOTO_SETTING
// typedef struct tagTempForSaveData//temp code for test
// {
// 	__s32 volume;
// 	__s32 speedflag;
// 	__s32 ratio;
// 	__s32 bright;
// 	__s32 effect;
// 	__s32 bgmusic;
// 	char  filename[128];
// }TempForSaveData_T;
//
// extern TempForSaveData_T g_TempSaveData;
//
// //get data
// #define get_photo_speedflag()                  (g_TempSaveData.speedflag)//PHOTO_SPEEDFLAG_MIDDLE
// #define get_photo_ratio()                      (g_TempSaveData.ratio) //0 //PHOTO_SPEEDFLAG_FAST=0
// #define get_sys_bright()                       (g_TempSaveData.bright) //4
// #define get_sys_volume()                       (g_TempSaveData.volume) //lion_get_volume
// #define get_photo_bgmusic()                    (g_TempSaveData.bgmusic) //PHOTO_BGMUSIC_OFF
// #define get_photo_effect()                     (g_TempSaveData.effect) //0
//
// //set data
// #define set_sys_volume(_d)                     (g_TempSaveData.volume = (_d)) //lion_set_volume
// #define set_photo_bgmusic(_d)                  (g_TempSaveData.bgmusic = (_d)) //__msg("set_photo_bgmusic(%d)\n", _d)
// #define set_photo_ratio(_d)                    (g_TempSaveData.ratio = (_d)) //__msg("set_photo_ratio(%d)\n", _d)
// #define set_photo_speedflag(_d)                (g_TempSaveData.speedflag = (_d)) //__msg("set_photo_speedflag(%d)\n", _d)
// #define set_photo_effect(_d)                   (g_TempSaveData.effect = (_d)) //__msg("set_photo_effect(%d)\n", _d)
// #define set_sys_bright(_d)                     (g_TempSaveData.bright = (_d)) //__msg("set_sys_bright(%d)\n", _d)

#else

//get data
#define get_sys_bright                         _get_sys_bright
#define get_sys_volume                         _get_sys_volume
#define get_photo_speedflag                    _get_photo_speedflag
#define get_photo_ratio                        _get_photo_ratio
#define get_photo_bgmusic                      _get_photo_bgmusic
#define get_photo_effect                       _get_photo_effect

//set data
#define set_sys_volume                         _set_sys_volume
#define set_sys_bright                         _set_sys_bright
#define set_photo_bgmusic                      _set_photo_bgmusic
#define set_photo_ratio                        _set_photo_ratio
#define set_photo_speedflag                    _set_photo_speedflag
#define set_photo_effect                       _set_photo_effect

#endif //TEST_CODE_PHOTO_SETTING

__s32 photo_save_cur_play_info( rat_root_t root_type);
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
__s32 PhotoLongStrShow(H_LYR hLayer, RECT *pRt, char *String, GUI_COLOR TxtClr);
void LongStrDestroy(void);

/////////////for anole
__s32 ANOLE_Init(__anole_para_t *para);
__bool ANOLE_ShowErrorPic(__u8 *buffer, __s32 width, __s32 height);
__u32 ANOLE_GetCurPicIndex(void);
__u32 ANOLE_GetPicNum(void);
__s32 ANOLE_GetPicInfo(__u32 index, __anole_pic_info_t *info);
__anole_sta_t ANOLE_GetSta(void);
__bool ANOLE_Play(void);
__s32 ANOLE_PlayCurFile(void);
__bool ANOLE_BackNormal(void);
__bool ANOLE_SetShowMode(__s32 index);
__bool ANOLE_ShowPicByIndex(__u32 index);
__bool ANOLE_Zoom(__s32 index);
__bool ANOLE_RotateRight(void);
__s32 ANOLE_SetSwitchMode(__u32 Id);
__s32 ANOLE_SetPlayInterval(__u32 sec);
__u32 ANOLE_GetPlayInterval(void);
__s32 ANOLE_GetSwitchMode(void);
__s32 ANOLE_GetZoomIndex(void);
void ANOLE_Exit(void);
__bool ANOLE_ScaleMove(__anole_diretion_t way);

///////////
__s32 ANOLE_GetRotateId(void);
void ANOLE_ResetRotate(void);


#endif
//end of the file



