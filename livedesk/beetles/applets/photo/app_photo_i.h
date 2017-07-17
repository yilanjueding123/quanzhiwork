/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*											All	Rights Reserved
*
* File    : app_photo_i.h
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/
#ifndef __APP_PHOTO_I_H_
#define  __APP_PHOTO_I_H_

#include "beetles_app.h"  //add for compile
//#include "app_root_i.h"
#include "app_photo.h"
#include "photo_uipara.h"
#include "photo_spsc.h"
#include "photo_mset.h"
#include "photo_sset.h"

#include "photo_adapter.h" //add for compile

#define DELAY_HIDE_MENU                     2500
// #define KEY_VALUE_ENABLE                	"1"
// #define KEY_VALUE_DISABLE               	"0"
// #define SETTING_REGNODE_MODULE          	"\\settings\\module"
// #define SET_DEFAULT_SETTING_ENABLE      	"default_setting"
// #define MODULE_NAME_ANOLE					"ANOLE"
//
//
// #define ANOLE_REG_BASE   					"\\anole\\base"
//
// #define ANOLE_NODE_SETTING      			"setting"
// #define ANOLE_KEY_PALYORDRE  				"play_order"
// #define ANOLE_KEY_TIMESLOT  				"time_slot"
// #define ANOLE_KEY_SWITCHMODE				"switch_mode"

typedef struct tag_photo_ctrl
{
    H_WIN    h_photo_spsc;
    H_WIN    h_photo_mset;
    H_WIN    h_photo_sset;
    H_WIN    h_dialog;

    H_WIN    lyr_photo_spsc;
    H_WIN    lyr_photo_mset;
    H_WIN    lyr_photo_sset;

    __s32    root_type;
    GUI_FONT *photo_font;

    PHOTO_BMP_RES_T err_pic;//void     *err_pic;

    __s32	menu_focus_id;
    __s32	menu_page_first;
} photo_ctrl_t;


#define PHOTO_SPSC_ID			(APP_PHOTO_ID+1)
#define PHOTO_MSET_ID			(APP_PHOTO_ID+2)
#define PHOTO_SSET_ID			(APP_PHOTO_ID+3)

__s32  _photo_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);

#define photo_cmd2parent(_h, _id, _d1, _d2)  {__msg("[ %x -> %x ]cmd2parent(%d, %d, %d)\n", _h, GUI_WinGetParent(_h), _id, _d1, _d2); _photo_cmd2parent(_h, _id, _d1, _d2);__here__;}

#endif

