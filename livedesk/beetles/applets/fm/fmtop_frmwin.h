
#ifndef __FMPLAY_FRMWIN_H_
#define __FMPLAY_FRMWIN_H_

#include "app_fm_i.h"

typedef struct tag_fmplay_frm_para
{
    GUI_FONT *fmplay_frm_font;
    H_LYR layer;

    //__u32	 root_type;

} fmplay_frm_para_t;

#define ADD_ITEM_RECORD//pls. close this define when there is not fm record function~

//main bg
#define BMP_MAINBG_WIDTH	(400)
#define BMP_MAINBG_HEIGHT	(40)
//menu bg
#define BMP_MENUBG_WIDTH	(400)
#define BMP_MENUBG_HEIGHT	(43)
//signal
#define SIGNALTOWER_STARTX	(34)//23
#define SIGNALTOWER_STARTY	(100)//92

//separator
#define BMP_SEPARATOR_WIDTH	(60)
#define BMP_SEPARATOR_HEIGHT	(38)//(36)
//item bg
#define BMP_ITEMBG_WIDTH	(36)
#define BMP_ITEMBG_HEIGHT	(36)
//value bg
#define BMP_VALUESIDE_WIDTH		(86)
#define BMP_VALUESIDE_HEIGHT	(7)
#define BMP_VALUEITEM_WIDTH		(86)
#define BMP_VALUEITEM_HEIGHT	(27)
//search bg
#define BMP_SRHBG_WIDTH		(191)
#define BMP_SRHBG_HEIGHT	(101)
//button
#define BMP_BUTTON_WIDTH	(37)
#define BMP_BUTTON_HEIGHT	(18)

//track
#define BMP_TRACK_WIDTH		(66)
#define BMP_TRACK_HEIGHT	(32)
//volume
#define BMP_VOLUME_WIDTH	(66)
#define BMP_VOLUME_HEIGHT	(32)
//character
#define BMP_CHARACTER_WIDTH	(15)//(28)
#define BMP_CHARACTER_HEIGHT (20)//(30)
//number
#define BMP_NUMBER_WIDTH	(12)//(20)
#define BMP_NUMBER_HEIGHT 	(20)//(30)
//dot
#define BMP_DOT_WIDTH	(12)
#define BMP_DOT_HEIGHT	(20)

//volume
#define BMP_FMVOLUEMBG_WIDTH	(356)
#define BMP_FMVOLUEMBG_HEIGHT	(24)
#define BMP_FMVOLUEMBAR_WIDTH	(252)
#define BMP_FMVOLUEMBAR_HEIGHT (4)
#define BMP_FMVOLUEMBLOCK_WIDTH	(5)
#define BMP_FMVOLUEMBLOCK_HEIGHT	(4)
#define BMP_FMVOLUMESPEAK_WIDTH	(15)
#define BMP_FMVOLUMESPEAK_HEIGHT	(13)

//track
#define TRACK_STARTX	(33)
#define TRACK_STARTY	(13)
//fm name
//#define FMNAME_STARTY	(9)
//volume
#define VOLUME_STARTX	(346)
#define VOLUME_STARTY	(13)
//scale
#define SCALE_STARTX	(0)
#define SCALE_STARTY	(40)
//freq. str
#define FREQSTR1_STARTX	(3)
#define FREQSTR1_STARTY	(67)
#define FREQSTR2_STARTX	(328)
#define FREQSTR2_STARTY	(67)
//speaker
//#define SPEAKERL_STARTX	(54)
//#define SPEAKERL_STARTY	(127)
//#define SPEAKERR_STARTX	(224)
//#define SPEAKERR_STARTY	(99)

//bg rect
#define FMMODE_MENUUI_STARTX		(0)
#define FMMODE_MENUUI_STARTY		(200)//240-40
#define FMMODE_MENUUI_WIDTH			(400)
#define FMMODE_MENUUI_HEIGHT		(40)
//item
#ifdef ADD_ITEM_RECORD
#define ITEMS_STARTX	(4)//(11)
#else
#define ITEMS_STARTX	(56)
#endif
#define ITEMS_STARTY	(FMMODE_MENUUI_STARTY)

//channel search tip frame
#define SRHTIPFRAME_STARTX	(105)//(123)
#define SRHTIPFRAME_STARTY	(80)//(80)

//channel tips
#define CHANNELTIPS1_STARTX	(326)//(646)
#define CHANNELTIPS1_STARTY	(96)//(166)
#define CHANNELTIPS2_STARTX	(302)
#define CHANNELTIPS2_STARTY	(117)

//volume
#define FMVOLUMEBG_STARTX		((400-BMP_FMVOLUEMBG_WIDTH)/2)
#define FMVOLUMEBG_STARTY		(10)


H_WIN fmplay_frmwin_create(H_WIN h_parent, fmplay_frm_para_t *para);
extern   void sprintf(char *buffer, const char *str, ...);


#endif

