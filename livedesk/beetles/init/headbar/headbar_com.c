/*
**************************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									            desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											  All Rights Reserved
*
* File    	: headbar_com.c
* By      	: Andy.zhang
* Func		: app bar view
* Version	: v1.0
* ============================================================================================================
* 2009-9-24 15:12:06  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "mod_init_i.h"
#include "headbar_uipara.h"
#include "headbar_com.h"
#include "..\\live_init.h"

/**********************************************************************************************************************/
#define ID_HEADBAR_COM		1002
//#define HB_BACKCOLOR			0xBF171717
#define HB_BACKCOLOR_LCD  			0x0//0xB23F4C60
#define HB_BACKCOLOR_TV				0xB206396A

#define MAX_VOL_LEVEL			6
#define TBAR_VOL_TIMER_ID		1000

typedef enum
{
    HB_PAINT_INIT,
    HB_PAINT_TIME,
    HB_PAINT_DATA,
    HB_PAINT_VOL,
    HB_PAINT_BRIGHT,
    HB_PAINT_VOLUME,
    HB_PAINT_MUSCI_BG,
    HB_PAINT_TITLE,
    HB_PAINT_TF,
    HB_PAINT_
} headbar_paint_e;

typedef struct
{
    __s32 		vol_level;			// 电压级别，供电池显示
    __bool      charge_sta;         //充电状态
    __date_t 	date;				// 日期
    __time_t	time;				// 时间
    __bool		music_bg;			// 标识音乐是否后台

    __s32		volume;				// 标识当前音量大小
    __s32		bright;				// 标识当前亮度

} headbar_data_t;

typedef struct
{
    H_WIN	hbar;						// bar窗口

    RECT 	music_rect;					// 音乐后台坐标
    GUI_RECT 	date_rect;					// 日期坐标
    GUI_RECT 	time_rect;					// 时间坐标
    RECT	volume_rect;				// 音量坐标
    RECT	volume_data_rect;			// 音量数据坐标
    RECT 	bright_rect;				// 亮度坐标
    RECT	bright_data_rect;			// 亮度数据坐标
    RECT 	vol_rect;					// 电池坐标
    RECT 	home_rect;					// home坐标
    RECT	title_rect;					// title坐标

    HTHEME	vol_htheme[MAX_VOL_LEVEL];
    HTHEME	vol_htheme_charge[MAX_VOL_LEVEL];
    HTHEME	music_htheme;
    HTHEME	bright_htheme;
    HTHEME	volume_htheme;
    HTHEME  tf_y_htheme;
    HTHEME  tf_n_htheme;

    __u32 	vol_id[MAX_VOL_LEVEL];		// 电池资源
    __u32 	vol_id_charge[MAX_VOL_LEVEL];// 充电电池资源
    /*__u8   *vol_bmp[MAX_VOL_LEVEL];		// 电池图片
    __u8   *music_bmp;					// 音乐后台播放图片
    __u8   *bright_bmp;					// 背光强度图片
    __u8   *volume_bmp;		*/			// 音量图片
    void   *vol_bmp[MAX_VOL_LEVEL];		// 电池图片
    void   *vol_bmp_charge[MAX_VOL_LEVEL];//充电电池图片
    void   *music_bmp;					// 音乐后台播放图片
    void   *bright_bmp;					// 背光强度图片
    void   *volume_bmp;					// 音量图片
    void   *tf_y_bmp;
    void   *tf_n_bmp;

    char    date_buf[32];				// 日期
    char 	time_buf[32];				// 时间
    char 	volume_buf[32];				// 音量
    char	bright_buf[32];				// 亮度
    char	title_buf[32];				// 应用程序title

    headbar_data_t 	data;				// 状态数据
    GUI_FONT 		*font;
} headbar_para_t;

typedef struct
{
    headbar_para_t	para;
    __u8			refresh_tid;
} headbar_ctl_t;
/**********************************************************************************************************************/
static __s32 cb_headbar_framewin(__gui_msg_t 	*p_msg);
static void  init_headbar_para(__gui_msg_t *p_msg, headbar_para_t 	*para);
static void  deinit_headbar_para(headbar_para_t *para);

static __s32 power_level_2_display(power_level_e level);
static void  time_to_char(char *buff, __time_t time);
static void  date_to_char(char *buff, __date_t date);
static __s32 bright_level_2_display(__lion_bright_t	bright);
/**********************************************************************************************************************/
static __u8 card_current_state = 0;
static __u8 card_prev_state = 0x33;
static __u8 display_tf_flag = 0;

static __bool is_music_background()
{
    return (dsk_wkm_is_open());
}


static void reset_headbar_data(headbar_para_t *para)
{
    power_level_e	 	level;
    __bool              charge_state;
    __lion_bright_t		bright;

    esTIME_GetTime(&(para->data.time));						// 时间
    eLIBs_memset( para->time_buf, 0, 32);
    time_to_char( para->time_buf, para->data.time );

    esTIME_GetDate(&(para->data.date));						// 日期
    eLIBs_memset( para->date_buf, 0, 32);
    date_to_char( para->date_buf, para->data.date );

    dsk_power_get_voltage_level(&level);
    para->data.vol_level = power_level_2_display(level);	//电池级别

    para->data.charge_sta = dsk_get_charge_state();         //充电状态

    para->data.volume = dsk_volume_get();			    	//音量
    eLIBs_memset(para->volume_buf, 0, 32);
    eLIBs_int2str_dec(para->data.volume, para->volume_buf);

    bright = dsk_display_get_lcd_bright();
    para->data.bright = bright_level_2_display(bright);		//背光亮度
    eLIBs_memset(para->bright_buf, 0, 32);
    eLIBs_int2str_dec(para->data.bright, para->bright_buf);

    para->data.music_bg = is_music_background();			//音乐后台
}

static void init_headbar_para(__gui_msg_t *p_msg, headbar_para_t *para)
{
    int 				i;

    para->hbar   =  p_msg->h_deswin;

    para->music_rect.x 		= headbar_uipara.music.x;
    para->music_rect.y 		= headbar_uipara.music.y;
    para->music_rect.width  = headbar_uipara.music.w;
    para->music_rect.height = headbar_uipara.music.h;

    para->date_rect.x0 		= headbar_uipara.date.x;
    para->date_rect.y0 		= headbar_uipara.date.y;
    para->date_rect.x1  	= para->date_rect.x0 + headbar_uipara.date.w - 1;
    para->date_rect.y1  	= para->date_rect.y0 + headbar_uipara.date.h - 1;

    para->time_rect.x0 		= headbar_uipara.time.x;
    para->time_rect.y0 		= headbar_uipara.time.y;
    para->time_rect.x1  	= para->time_rect.x0 + headbar_uipara.time.w - 1;
    para->time_rect.y1   	= para->time_rect.y0 + headbar_uipara.time.h - 1;

    para->volume_rect.x 	= headbar_uipara.volume_icon.x;
    para->volume_rect.y 	= headbar_uipara.volume_icon.y;
    para->volume_rect.width = headbar_uipara.volume_icon.w;
    para->volume_rect.height = headbar_uipara.volume_icon.h;

    para->volume_data_rect.x 		= headbar_uipara.volume_txt.x;
    para->volume_data_rect.y 		= headbar_uipara.volume_txt.y;
    para->volume_data_rect.width	= headbar_uipara.volume_txt.w;
    para->volume_data_rect.height 	= headbar_uipara.volume_txt.h;

    para->bright_rect.x 			= headbar_uipara.bright_icon.x;
    para->bright_rect.y 			= headbar_uipara.bright_icon.y;
    para->bright_rect.width  		= headbar_uipara.bright_icon.w;
    para->bright_rect.height 		= headbar_uipara.bright_icon.h;

    para->bright_data_rect.x 		= headbar_uipara.bright_txt.x;
    para->bright_data_rect.y 		= headbar_uipara.bright_txt.y;
    para->bright_data_rect.width  	= headbar_uipara.bright_txt.w;
    para->bright_data_rect.height 	= headbar_uipara.bright_txt.h;

    para->vol_rect.x 				= headbar_uipara.battery.x;
    para->vol_rect.y 				= headbar_uipara.battery.y;
    para->vol_rect.width  	    	= headbar_uipara.battery.w;
    para->vol_rect.height    		= headbar_uipara.battery.h;

    para->title_rect.x				= headbar_uipara.title.x;
    para->title_rect.y				= headbar_uipara.title.y;
    para->title_rect.width			= headbar_uipara.title.w;
    para->title_rect.height			= headbar_uipara.title.h;

    para->vol_id[0] = headbar_uipara.res_id.vol_id0;
    para->vol_id[1] = headbar_uipara.res_id.vol_id1;
    para->vol_id[2] = headbar_uipara.res_id.vol_id2;
    para->vol_id[3] = headbar_uipara.res_id.vol_id3;
    para->vol_id[4] = headbar_uipara.res_id.vol_id4;
    para->vol_id[5] = headbar_uipara.res_id.vol_id5;

    para->vol_id_charge[0] = headbar_uipara.res_id.charge_vol_id0;
    para->vol_id_charge[1] = headbar_uipara.res_id.charge_vol_id1;
    para->vol_id_charge[2] = headbar_uipara.res_id.charge_vol_id2;
    para->vol_id_charge[3] = headbar_uipara.res_id.charge_vol_id3;
    para->vol_id_charge[4] = headbar_uipara.res_id.charge_vol_id4;
    para->vol_id_charge[5] = headbar_uipara.res_id.charge_vol_id5;

    for( i = 0; i < MAX_VOL_LEVEL; i++ )
    {
        para->vol_htheme[i] = dsk_theme_open(para->vol_id[i]);
        para->vol_bmp[i]	= dsk_theme_hdl2buf(para->vol_htheme[i]);

        para->vol_htheme_charge[i] = dsk_theme_open(para->vol_id_charge[i]);
        para->vol_bmp_charge[i]	= dsk_theme_hdl2buf(para->vol_htheme_charge[i]);
    }
    para->music_htheme		= dsk_theme_open(headbar_uipara.res_id.music_id);
    para->bright_htheme		= dsk_theme_open(headbar_uipara.res_id.bright_id);
    para->volume_htheme		= dsk_theme_open(headbar_uipara.res_id.volume_id);

    para->tf_n_htheme = dsk_theme_open(headbar_uipara.res_id.tf_n_id);
    para->tf_y_htheme = dsk_theme_open(headbar_uipara.res_id.tf_y_id);

    para->tf_y_bmp = dsk_theme_hdl2buf(para->tf_y_htheme);

    para->tf_n_bmp = dsk_theme_hdl2buf(para->tf_n_htheme);

    para->music_bmp			= dsk_theme_hdl2buf(para->music_htheme);
    para->bright_bmp		= dsk_theme_hdl2buf(para->bright_htheme);
    para->volume_bmp		= dsk_theme_hdl2buf(para->volume_htheme);

    para->font				= headbar_uipara.font;

    reset_headbar_data(para);
}


static void deinit_headbar_para(headbar_para_t *para)
{
    int i;

    for( i = 0; i < MAX_VOL_LEVEL; i++)
    {
        dsk_theme_close(para->vol_htheme[i]);
        dsk_theme_close(para->vol_htheme_charge[i]);
    }
    dsk_theme_close(para->tf_n_htheme);
    dsk_theme_close(para->tf_y_htheme);
    dsk_theme_close(para->music_htheme);
    dsk_theme_close(para->bright_htheme);
    dsk_theme_close(para->volume_htheme);
}


static void time_to_char(char *buff, __time_t time)
{
    buff[0] = time.hour / 10 + '0';
    buff[1] = time.hour % 10 + '0';

    buff[2] = ':';

    buff[3] = time.minute / 10 + '0';
    buff[4] = time.minute % 10 + '0';
    buff[5] = '\0';
    //	buff[5] = ':';
    //
    //	buff[6] = time.second/10 + '0';
    //	buff[7] = time.second%10 + '0';
    //
    //	buff[8] = '\0';
}

static void date_to_char(char *buff, __date_t date)
{
    //	buff[0] = date.year/1000 + '0';
    //	buff[1] = (date.year%1000)/100 + '0';
    //	buff[2] = ((date.year%1000)%100)/10 + '0';
    //	buff[3] = (date.year%1000)%100%10 + '0';
    //
    //	buff[4] = '-';
    //	buff[5] = date.month/10 + '0';
    //	buff[6] = date.month%10 + '0';
    //
    //	buff[7] = '-';
    //	buff[8] = date.day/10 + '0';
    //	buff[9] = date.day%10 + '0';

    buff[0] = date.month / 10 + '0';
    buff[1] = date.month % 10 + '0';

    buff[2] = '-';
    buff[3] = date.day / 10 + '0';
    buff[4] = date.day % 10 + '0';
    buff[5] = '\0';
}

static __s32 bright_level_2_display(__lion_bright_t	bright)
{
    int ret = 0;

    switch( bright )
    {
    case LION_BRIGHT_LEVEL1:
        ret = 1;
        break;
    case LION_BRIGHT_LEVEL2:
        ret = 2;
        break;
    case LION_BRIGHT_LEVEL3:
        ret = 3;
        break;
    case LION_BRIGHT_LEVEL4:
        ret = 4;
        break;
    case LION_BRIGHT_LEVEL5:
        ret = 5;
        break;
    case LION_BRIGHT_LEVEL6:
        ret = 6;
        break;
    case LION_BRIGHT_LEVEL7:
        ret = 7;
        break;
    case LION_BRIGHT_LEVEL8:
        ret = 8;
        break;
    case LION_BRIGHT_LEVEL9:
        ret = 9;
        break;
    case LION_BRIGHT_LEVEL10:
        ret = 10;
        break;
    case LION_BRIGHT_LEVEL11:
        ret = 11;
        break;
    case LION_BRIGHT_LEVEL12:
        ret = 12;
        break;
    case LION_BRIGHT_LEVEL13:
        ret = 13;
        break;
    case LION_BRIGHT_LEVEL14:
        ret = 14;
        break;
    case LION_BRIGHT_LEVEL15:
        ret = 15;
        break;
    case LION_BRIGHT_LEVEL16:
        ret = 16;
        break;

    case LION_BRIGHT_:
        ret = 0;
        break;
    }

    return ret;
}

static __s32 power_level_2_display(power_level_e level)
{
    int ret = 0;

    switch(level)
    {
    case DSK_POWER_LEVEL_0:
        ret = 0;
        break;
    case DSK_POWER_LEVEL_1:
        ret = 1;
        break;
    case DSK_POWER_LEVEL_2:
        ret = 2;
        break;
    case DSK_POWER_LEVEL_3:
        ret = 3;
        break;
    case DSK_POWER_LEVEL_4:
        ret = 4;
        break;

    case DSK_POWER_LEVEL_5:
        ret = 5;
        break;

    default:
        ret = 0;
        break;
    }

    return ret;
}

static void headbar_refresh_thread(void *arg)
{
    headbar_para_t *para = (headbar_para_t *)arg;
    headbar_data_t	new_data;
    __lion_bright_t	bright;
    H_WIN			lyr;
    __gui_msg_t 	msg;

    while(1)
    {
        if(esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
        {
            esKRNL_TDel(OS_PRIO_SELF);
            return;
        }

        lyr = GUI_WinGetLyrWin(para->hbar);
        if( GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(lyr) )
        {
            esTIME_GetTime(&new_data.time);							//时间
            esTIME_GetDate(&new_data.date);							//日期

            bright = dsk_display_get_lcd_bright();

            new_data.bright 	= bright_level_2_display(bright);		//背光亮度
            new_data.volume   	= dsk_volume_get();				//音量
            new_data.music_bg 	= is_music_background();				//音乐后台

            if( (new_data.date.year != para->data.date.year)
                    || (new_data.date.month != para->data.date.month)
                    || (new_data.date.day != para->data.date.day)
              )
            {
#if 0//jh_dbg_0720
                //更新显示日期
                eLIBs_memset( para->date_buf, 0, 32);
                para->data.date = new_data.date;
                date_to_char( para->date_buf, para->data.date );

                msg.id = HBAR_DATA_PAINT;
                msg.h_srcwin = NULL;
                msg.h_deswin = para->hbar;
                msg.dwAddData1 = 0;
                msg.dwAddData2 = 0;
                msg.dwReserved = 0;
                msg.p_arg	   = NULL;

                GUI_SendNotifyMessage(&msg);
#endif
            }

            if( (new_data.time.hour != para->data.time.hour)
                    || (new_data.time.minute != para->data.time.minute)
              )
            {
#if 0
                //更新显示时间
                eLIBs_memset( para->time_buf, 0, 32);
                para->data.time = new_data.time;
                time_to_char( para->time_buf, para->data.time);

                msg.id 			= HBAR_TIME_PAINT;
                msg.h_srcwin 	= NULL;
                msg.h_deswin 	= para->hbar;
                msg.dwAddData1 	= 0;
                msg.dwAddData2 	= 0;
                msg.dwReserved 	= 0;
                msg.p_arg	  	= NULL;

                GUI_SendNotifyMessage(&msg);
#endif
            }

            if( new_data.bright != para->data.bright )
            {
                // 更新亮度
                msg.id = HBAR_BRIGHT_PAINT;
                msg.h_srcwin = NULL;
                msg.h_deswin = para->hbar;
                msg.dwAddData1 = (__u32)bright;
                msg.dwAddData2 = 0;
                msg.dwReserved = 0;
                msg.p_arg	   = NULL;

                GUI_SendNotifyMessage(&msg);
            }

            if( new_data.volume != para->data.volume )
            {
                // 更新音量
                msg.id 			= HBAR_VOLUME_PAINT;
                msg.h_srcwin 	= NULL;
                msg.h_deswin 	= para->hbar;
                msg.dwAddData1 	= new_data.volume;
                msg.dwAddData2 	= 0;
                msg.dwReserved 	= 0;
                msg.p_arg	   	= NULL;

                GUI_SendNotifyMessage(&msg);
            }

            if( new_data.music_bg != para->data.music_bg )
            {
                para->data.music_bg = new_data.music_bg;

                msg.id = HBAR_MUSIC_BG_PAINT;
                msg.h_srcwin = NULL;
                msg.h_deswin = para->hbar;
                msg.dwAddData1 = 0;
                msg.dwAddData2 = 0;
                msg.dwReserved = 0;
                msg.p_arg	   = NULL;

                GUI_SendNotifyMessage(&msg);
            }
        }

        esKRNL_TimeDly(10);		// 10 * 5ms = 50ms
    }
}

static __s32 headbar_on_paint_argb(__gui_msg_t *msg, headbar_paint_e mode)
{
    headbar_para_t 	*p_ctrl;
    RECT           	fbrect;
    H_LYR		   	layer;
    GUI_FONT	   	*font;
    int 			screen_width;
    int 			screen_height;
    GUI_COLOR 		backcolor;

    dsk_display_get_size(&screen_width, &screen_height);
    if( ((screen_width == 400 ) && ( screen_height == 240)) || ((screen_width == 480 ) && ( screen_height == 272)))
    {
        backcolor = HB_BACKCOLOR_LCD;
    }
    else
    {
        backcolor = HB_BACKCOLOR_TV;
    }

    p_ctrl 	= &(((headbar_ctl_t *)GUI_WinGetAttr(msg->h_deswin))->para);
    GUI_WinGetClientFBRect(msg->h_deswin, &fbrect);
    font 	= p_ctrl->font;

    layer  	= (H_LYR)GUI_WinGetLyrWin(msg->h_deswin);
    //__here__;

    if( GUI_LYRWIN_STA_SUSPEND == GUI_LyrWinGetSta(layer) )
    {
        return EPDK_OK;
    }

    //__here__;
    GUI_LyrWinSel(layer);
    switch(mode)
    {
    case HB_PAINT_INIT:			/* 全部更新 */
    {
        GUI_SetBkColor(backcolor);
        GUI_ClearRect(fbrect.x, fbrect.y, fbrect.x + fbrect.width - 1, fbrect.y + fbrect.height);

        /*GUI_SetBkColor(0xBF4B4B4B);
        GUI_ClearRect(fbrect.x, fbrect.y + fbrect.height -3, fbrect.x + fbrect.width -1, fbrect.y + fbrect.height -2);
        GUI_SetBkColor(0xBF000000);
        GUI_ClearRect(fbrect.x, fbrect.y + fbrect.height -2, fbrect.x + fbrect.width -1, fbrect.y + fbrect.height -1);*/
        //GUI_OpenAlphaBlend();
        /* 画电池图标 */
        {
            __bool charge_sta;

            GUI_SetBkColor(backcolor);

            GUI_ClearRect(
                p_ctrl->vol_rect.x,
                p_ctrl->vol_rect.y,
                p_ctrl->vol_rect.x + p_ctrl->vol_rect.width - 1,
                p_ctrl->vol_rect.y + p_ctrl->vol_rect.height - 1
            );

            /* 画电池图标 */
            {
                power_level_e level;
                dsk_power_get_voltage_level(&level);
                p_ctrl->data.vol_level = power_level_2_display(level);  	//电量级别
                p_ctrl->data.charge_sta = dsk_get_charge_state();
                __msg("p_ctrl->data.vol_level=%d\n", p_ctrl->data.vol_level);
                __msg("p_ctrl->data.charge_sta=%d\n", p_ctrl->data.charge_sta);
                GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                if(EPDK_TRUE == p_ctrl->data.charge_sta)
                {
                    GUI_BMP_Draw(p_ctrl->vol_bmp_charge[p_ctrl->data.vol_level], p_ctrl->vol_rect.x, p_ctrl->vol_rect.y);
                }
                else
                {
                    GUI_BMP_Draw(p_ctrl->vol_bmp[p_ctrl->data.vol_level], p_ctrl->vol_rect.x, p_ctrl->vol_rect.y);
                }
            }
        }

        /* 音量*/
        //			GUI_BMP_Draw_Trans(p_ctrl->volume_bmp, p_ctrl->volume_rect.x, p_ctrl->volume_rect.y, 0, 0);

        /* 亮度*/
        //			GUI_BMP_Draw_Trans(p_ctrl->bright_bmp, p_ctrl->bright_rect.x, p_ctrl->bright_rect.y, 0, 0);

        /* 后台播放 */
        //			if( p_ctrl->data.music_bg )
        //				GUI_BMP_Draw_Trans(p_ctrl->music_bmp, p_ctrl->music_rect.x, p_ctrl->music_rect.y, 0, 0);

        GUI_OpenAlphaBlend();
        GUI_SetFont(font);
        GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
        GUI_SetColor(GUI_WHITE);

#if BEETLES_RTC_EXIST
        /* 写日期 */
        GUI_DispStringInRect(p_ctrl->date_buf, &p_ctrl->date_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);

        /* 写时间 */
        GUI_DispStringInRect(p_ctrl->time_buf, &p_ctrl->time_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
#endif

        //			GUI_DispStringAt(p_ctrl->volume_buf, p_ctrl->volume_data_rect.x, p_ctrl->volume_data_rect.y);
        //			GUI_DispStringAt( p_ctrl->bright_buf ,p_ctrl->bright_data_rect.x, p_ctrl->bright_data_rect.y);
        //			GUI_DispStringAt( p_ctrl->title_buf, p_ctrl->title_rect.x, p_ctrl->title_rect.y);

        GUI_LyrWinFlushFB(layer);
        GUI_CloseAlphaBlend();
    }
    return EPDK_OK;

    case HB_PAINT_TIME:		/* 更新当前时间 */
    {
#if BEETLES_RTC_EXIST
        GUI_SetBkColor(backcolor);
        GUI_ClearRectEx(&p_ctrl->time_rect);

        GUI_OpenAlphaBlend();
        GUI_SetFont(font);
        GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
        GUI_SetColor(GUI_WHITE);
        /* 写时间 */
        GUI_DispStringInRect(p_ctrl->time_buf, &p_ctrl->time_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_LyrWinFlushFB(layer);
        GUI_CloseAlphaBlend();
#endif
    }
    return EPDK_OK;

    case HB_PAINT_DATA:		/* 更新当前日期 */
    {
#if BEETLES_RTC_EXIST
        GUI_SetBkColor(backcolor);
        GUI_ClearRectEx(&p_ctrl->date_rect);

        GUI_OpenAlphaBlend();
        GUI_SetFont(font);
        GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
        GUI_SetColor(GUI_WHITE);
        /* 写日期 */
        GUI_DispStringInRect(p_ctrl->date_buf, &p_ctrl->date_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_LyrWinFlushFB(layer);
        GUI_CloseAlphaBlend();
#endif
    }
    return EPDK_OK;
    case HB_PAINT_TF:
        __log("----jh_dbg_1019_13----\n");
        GUI_SetBkColor(backcolor);
        GUI_ClearRect(350, 0, 32, 32);

        if(card_current_state == 0x55)
            GUI_BMP_Draw(p_ctrl->tf_y_bmp, 350, 0);
        else if(card_current_state == 0x56)
            GUI_BMP_Draw(p_ctrl->tf_n_bmp, 350, 0);

        GUI_LyrWinFlushFB(layer);
        return EPDK_OK;
    case HB_PAINT_VOL:			/* 更新电量 */
    {
        __bool charge_sta;
        //__here__;

        /* 画电池图标 */
        {
            __s32 vol_level;
            __bool charge_sta;
            power_level_e level;
            dsk_power_get_voltage_level(&level);
            vol_level = power_level_2_display(level);  	//电量级别
            charge_sta = dsk_get_charge_state();
            // __msg("vol_level=%d\n", vol_level);
            // __msg("charge_sta=%d\n", charge_sta);
            if(p_ctrl->data.vol_level != vol_level
                    || p_ctrl->data.charge_sta != charge_sta)
            {
                GUI_SetBkColor(backcolor);

                GUI_ClearRect(
                    p_ctrl->vol_rect.x,
                    p_ctrl->vol_rect.y,
                    p_ctrl->vol_rect.x + p_ctrl->vol_rect.width - 1,
                    p_ctrl->vol_rect.y + p_ctrl->vol_rect.height - 1
                );
                p_ctrl->data.vol_level = vol_level;
                p_ctrl->data.charge_sta = charge_sta;
                if(EPDK_TRUE == p_ctrl->data.charge_sta)
                {
                    GUI_BMP_Draw(p_ctrl->vol_bmp_charge[p_ctrl->data.vol_level], p_ctrl->vol_rect.x, p_ctrl->vol_rect.y);
                }
                else
                {
                    GUI_BMP_Draw(p_ctrl->vol_bmp[p_ctrl->data.vol_level], p_ctrl->vol_rect.x, p_ctrl->vol_rect.y);
                }
                GUI_LyrWinFlushFB(layer);
            }

        }
    }
    return EPDK_OK;

    case HB_PAINT_BRIGHT:		/* 更新亮度 */
    {
        //			GUI_SetBkColor(backcolor);
        //			GUI_ClearRect(
        //						  p_ctrl->bright_data_rect.x,
        //						  p_ctrl->bright_data_rect.y,
        //						  p_ctrl->bright_data_rect.x + p_ctrl->bright_data_rect.width -1,
        //						  p_ctrl->bright_data_rect.y + p_ctrl->bright_data_rect.height -1
        //						  );
        //
        //			GUI_OpenAlphaBlend();
        //			GUI_SetFont(font);
        //			GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
        //			GUI_SetColor(GUI_WHITE);
        //			GUI_DispStringAt( p_ctrl->bright_buf ,p_ctrl->bright_data_rect.x, p_ctrl->bright_data_rect.y);
        //			GUI_LyrWinFlushFB(layer);
        //			GUI_CloseAlphaBlend();
    }
    return EPDK_OK;

    case HB_PAINT_VOLUME:		/*  更新音量 */
    {
        //			GUI_SetBkColor(backcolor);
        //			GUI_ClearRect(
        //						  p_ctrl->volume_data_rect.x,
        //						  p_ctrl->volume_data_rect.y,
        //						  p_ctrl->volume_data_rect.x + p_ctrl->volume_data_rect.width -1,
        //						  p_ctrl->volume_data_rect.y + p_ctrl->volume_data_rect.height -1
        //						  );
        //			GUI_OpenAlphaBlend();
        //			GUI_SetFont(font);
        //			GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
        //			GUI_SetColor(GUI_WHITE);
        //			GUI_DispStringAt( p_ctrl->volume_buf ,p_ctrl->volume_data_rect.x, p_ctrl->volume_data_rect.y);
        //			GUI_LyrWinFlushFB(layer);
        //			GUI_CloseAlphaBlend();

    }
    return EPDK_OK;

    case HB_PAINT_MUSCI_BG:		/* 更新后台播放状态 */
    {
        //			GUI_SetBkColor(backcolor);
        //			GUI_ClearRect(
        //						  p_ctrl->music_rect.x,
        //						  p_ctrl->music_rect.y,
        //						  p_ctrl->music_rect.x + p_ctrl->music_rect.width -1,
        //						  p_ctrl->music_rect.y + p_ctrl->music_rect.height -1
        //						  );
        //			if( p_ctrl->data.music_bg )
        //			{
        //				/* 画后台播放图标 */
        //				GUI_BMP_Draw(p_ctrl->music_bmp, p_ctrl->music_rect.x, p_ctrl->music_rect.y);
        //			}
    }
    return EPDK_OK;

    case HB_PAINT_TITLE:
    {
        //			GUI_SetBkColor(backcolor);
        //			GUI_ClearRect(
        //						  p_ctrl->title_rect.x,
        //						  p_ctrl->title_rect.y,
        //						  p_ctrl->title_rect.x + p_ctrl->title_rect.width -1,
        //						  p_ctrl->title_rect.y + p_ctrl->title_rect.height -1
        //						  );
        //
        //			GUI_OpenAlphaBlend();
        //			GUI_SetFont(font);
        //			GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
        //			GUI_SetColor(GUI_WHITE);
        //			GUI_UC_SetEncodeUTF8();
        //			GUI_DispStringAt( p_ctrl->title_buf, p_ctrl->title_rect.x, p_ctrl->title_rect.y);
        //			GUI_LyrWinFlushFB(layer);
        //			GUI_CloseAlphaBlend();
    }
    return EPDK_OK;

    default:
        break;
    }

    return EPDK_OK;
}

static __s32 headbar_on_paint_8bpp(__gui_msg_t *msg, headbar_paint_e mode)
{

    return EPDK_OK;
}

static __s32 headbar_on_paint(__gui_msg_t *msg, headbar_paint_e mode)
{
    H_LYR		   layer;
    FB             fb;

    layer  	= (H_LYR)GUI_WinGetLyrWin(msg->h_deswin);
    GUI_LyrWinGetFB(layer, &fb);

    if( fb.fmt.fmt.rgb.pixelfmt == PIXEL_MONO_8BPP )
    {
        //__here__;
        headbar_on_paint_8bpp(msg, mode);
    }
    else
    {
        //__here__;
        headbar_on_paint_argb(msg, mode);
    }

    return EPDK_OK;
}

/*static __bool pt_in_rect(RECT *rect, __s32 x, __s32 y)
{
	if( ( x > rect->x)&&( y>rect->y )&&( x<(rect->x + rect->width) )&&( y<(rect->y + rect->height) ) )
	{
		return EPDK_TRUE;
	}
	else
	{
		return EPDK_FALSE;
	}
}*/

static __s32 cb_headbar_framewin(__gui_msg_t *p_msg)
{
    switch( p_msg->id )
    {
    case GUI_MSG_CREATE:
    {
        headbar_ctl_t *p_ctrl;
        __here__
        p_ctrl = (headbar_ctl_t *)esMEMS_Malloc(0, sizeof(headbar_ctl_t));		// 分配内存单元
        if( !p_ctrl )
        {
            __err("headbar_para_t malloc error \n");
        }
        eLIBs_memset(p_ctrl, 0, sizeof(headbar_ctl_t));
        GUI_WinSetAttr(p_msg->h_deswin, p_ctrl);
        //__wait__;
        init_headbar_para(p_msg, &(p_ctrl->para));

        headbar_on_paint(p_msg, HB_PAINT_INIT);

        GUI_SetTimer(p_msg->h_deswin, TBAR_VOL_TIMER_ID, 100, NULL);
        p_ctrl->refresh_tid =  esKRNL_TCreate( headbar_refresh_thread, &(p_ctrl->para), 0x800, KRNL_priolevel5);
    }
    return EPDK_OK;

    case GUI_MSG_DESTROY:
    {
        headbar_ctl_t *p_ctrl;
        RECT 	fbrect;
        H_LYR	layer;

        p_ctrl = (headbar_ctl_t *)GUI_WinGetAttr(p_msg->h_deswin);

        GUI_KillTimer(p_msg->h_deswin, TBAR_VOL_TIMER_ID);

        while( esKRNL_TDelReq(p_ctrl->refresh_tid) != OS_TASK_NOT_EXIST)
        {
            esKRNL_TimeDly(2);
        }

        layer  = (H_LYR)GUI_WinGetLyrWin(p_msg->h_deswin);

        if( GUI_LYRWIN_STA_SUSPEND != GUI_LyrWinGetSta(layer) )
        {
            GUI_LyrWinSel(layer);
            GUI_SetBkColor(0);
            GUI_WinGetClientFBRect(p_msg->h_deswin, &fbrect);
            GUI_ClearRect(fbrect.x, fbrect.y, fbrect.x + fbrect.width - 1, fbrect.y + fbrect.height - 1);
        }

        deinit_headbar_para( &(p_ctrl->para) );

        esMEMS_Mfree(0, p_ctrl);

        return EPDK_OK;
    }

    case GUI_MSG_CLOSE:
        GUI_FrmWinDelete(p_msg->h_deswin);
        return EPDK_OK;

    case GUI_MSG_TIMER:
    {

        H_LYR lyr_jh;
        lyr_jh = GUI_WinGetLyrWin(p_msg->h_deswin);
        if(GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(lyr_jh))
        {

            card_current_state = tf_cardstate_get();
            if(card_current_state != card_prev_state)
            {
                display_tf_flag = 0;
                card_prev_state = card_current_state;
            }
            //if(card_current_state != card_prev_state)
            //{
            //card_prev_state=card_current_state;
            // if(card_current_state>=0x55)
            if(display_tf_flag == 0)
            {

                __gui_msg_t 	msgjh;

                msgjh.id 			= HBAR_TF_PAINT;
                msgjh.h_srcwin 		= NULL;
                msgjh.h_deswin 		= p_msg->h_deswin;
                msgjh.dwAddData1 	= 0;
                msgjh.dwAddData2 	= 0;
                msgjh.dwReserved 	= 0;
                msgjh.p_arg	   		= NULL;
                display_tf_flag = 1;
                GUI_SendMessage(&msgjh);

                __log("----jh_dbg_1019_11----\n");
            }

            //}
        }
        else
            display_tf_flag = 0;

        if( p_msg->dwAddData1 == TBAR_VOL_TIMER_ID )
        {
            H_LYR lyr;

            lyr = GUI_WinGetLyrWin(p_msg->h_deswin);

            //                __here__;
            if(GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(lyr))
            {
                __bool charge_sta;
                power_level_e	level;
                __s32 			vol_level;
                headbar_ctl_t  *p_ctrl;
                __gui_msg_t 	msgex;

                //                  __here__;

                p_ctrl = (headbar_ctl_t *)GUI_WinGetAttr(p_msg->h_deswin);

                //dsk_power_get_voltage_level(&level);
                //vol_level = power_level_2_display(level);  	//电量级别
                //charge_sta = dsk_get_charge_state();
                //__msg("vol_level=%d\n", vol_level);
                // __msg("charge_sta=%d\n", charge_sta);
                //if( vol_level != p_ctrl->para.data.vol_level
                //    || charge_sta != p_ctrl->para.data.charge_sta)
                {
                    // 更新电池图标

                    msgex.id 			= HBAR_VOL_PAINT;
                    msgex.h_srcwin 		= NULL;
                    msgex.h_deswin 		= p_msg->h_deswin;
                    msgex.dwAddData1 	= 0;
                    msgex.dwAddData2 	= 0;
                    msgex.dwReserved 	= 0;
                    msgex.p_arg	   		= NULL;

                    GUI_SendMessage(&msgex);
                }

                //p_ctrl->para.data.vol_level = vol_level;
                //p_ctrl->para.data.charge_sta = charge_sta;
            }

        }

    }
    return EPDK_OK;

    case GUI_MSG_PAINT:
    {
        __here__
    }
    return EPDK_OK;

    case GUI_MSG_WIN_WAKEUP:
    {
        headbar_ctl_t *p_ctrl;

        p_ctrl = (headbar_ctl_t *)GUI_WinGetAttr(p_msg->h_deswin);
        reset_headbar_data(&(p_ctrl->para));
        headbar_on_paint(p_msg, HB_PAINT_INIT);
    }
    return EPDK_OK;

    case GUI_MSG_TOUCH:
    {
        /*headbar_ctl_t *p_ctrl;

        p_ctrl = (headbar_ctl_t *)GUI_WinGetAttr(p_msg->h_deswin);

        switch( p_msg->dwAddData1 )
        {
        	case GUI_MSG_TOUCH_UP:
        	{
        		int	 	x,y;
        		__bool  ret1;
        		__bool  ret2;
        		__bool  ret3;

        		x = LOSWORD(p_msg->dwAddData2);
        		y = HISWORD(p_msg->dwAddData2);

        		ret1 = pt_in_rect( &(p_ctrl->para.volume_rect) , x, y);
        		ret2 = pt_in_rect( &(p_ctrl->para.volume_data_rect) , x, y);
        		ret3 = pt_in_rect( &(p_ctrl->para.home_rect) , x, y);

        		if( (ret1 == EPDK_TRUE )||( ret2 == EPDK_TRUE ) )
        		{
        			__gui_notify_msg_t	notify_msg;

                    notify_msg.hWnd     = p_msg->h_deswin;
                    notify_msg.id       = GUI_WinGetItemId(p_msg->h_deswin);
                    notify_msg.msgcode  = HBAR_ADJUST_VOLUME;
                    notify_msg.dwAddData= 0;

                    GUI_NotifyParent (&notify_msg);
        		}

        		if( ret3 == EPDK_TRUE )
        		{
        			activity_return_home(NULL);
        		}

        		return 0;
        	}

        	default:
        		break;
        }*/
    }
    return EPDK_OK;

    case HBAR_TITLE_PAINT:
    {
        //			headbar_ctl_t *p_ctrl;
        //
        //			p_ctrl = (headbar_ctl_t *)GUI_WinGetAttr(p_msg->h_deswin);
        //
        //			eLIBs_memset( p_ctrl->para.title_buf, 0, 32);
        //			eLIBs_strncpy( p_ctrl->para.title_buf, (char *)p_msg->dwAddData1 ,32);
        //			headbar_on_paint(p_msg, HB_PAINT_TITLE);
    }
    return EPDK_OK;

    case HBAR_TIME_PAINT:
    {
        headbar_on_paint(p_msg, HB_PAINT_TIME);
    }
    return EPDK_OK;

    case HBAR_DATA_PAINT:
    {
        headbar_on_paint(p_msg, HB_PAINT_DATA);
    }
    return EPDK_OK;
    case HBAR_TF_PAINT:
    {
        __log("----jh_dbg_1019_12----\n");
        headbar_on_paint(p_msg, HB_PAINT_TF);
    }
    return EPDK_OK;
    case HBAR_VOL_PAINT:
    {
        // __here__;
        headbar_on_paint(p_msg, HB_PAINT_VOL);
    }
    return EPDK_OK;

    case HBAR_BRIGHT_PAINT:
    {
        //		headbar_ctl_t  *p_ctrl;
        //		headbar_para_t *para;
        //
        //		p_ctrl = (headbar_ctl_t *)GUI_WinGetAttr(p_msg->h_deswin);
        //		para   = &(p_ctrl->para);
        //
        //		para->data.bright = bright_level_2_display( (__lion_bright_t)p_msg->dwAddData1 );
        //   	eLIBs_memset(para->bright_buf, 0, 32);
        //		eLIBs_int2str_dec(para->data.bright, para->bright_buf);
        //
        //		headbar_on_paint(p_msg, HB_PAINT_BRIGHT);
    }
    return EPDK_OK;

    case HBAR_VOLUME_PAINT:
    {
        //		headbar_ctl_t  *p_ctrl;
        //		headbar_para_t *para;
        //
        //		p_ctrl = (headbar_ctl_t *)GUI_WinGetAttr(p_msg->h_deswin);
        //		para   = &(p_ctrl->para);
        //
        //		para->data.volume = p_msg->dwAddData1;
        //
        //		eLIBs_memset(para->volume_buf, 0, 32);
        //		eLIBs_int2str_dec(para->data.volume, para->volume_buf);
        //
        //		headbar_on_paint(p_msg, HB_PAINT_VOLUME);
    }
    return EPDK_OK;

    case HBAR_MUSIC_BG_PAINT:
    {
        //		headbar_on_paint(p_msg, HB_PAINT_MUSCI_BG);
    }
    return EPDK_OK;

    default:
        break;
    }

    return GUI_FrmWinDefaultProc(p_msg);
}

H_WIN headbar_frm_create(H_WIN parent, H_LYR layer)
{
    H_WIN 							h_win;
    //headbar_ctl_t 					*p_ctrl;
    __gui_framewincreate_para_t 	para;

    eLIBs_memset(&para, 0, sizeof(__gui_framewincreate_para_t));

    para.dwStyle          = WS_VISIBLE;
    para.dwExStyle        = 0;
    para.name			  = NULL;
    para.hOwner			  = 0;
    para.hHosting         = parent;
    para.FrameWinProc     = cb_headbar_framewin;
    para.id				  = ID_HEADBAR_COM;
    para.hLayer           = layer;

    para.rect.x			  = headbar_uipara.hbar_frmwin.x;
    para.rect.y			  = headbar_uipara.hbar_frmwin.y;
    para.rect.width		  = headbar_uipara.hbar_frmwin.w;
    para.rect.height	  = headbar_uipara.hbar_frmwin.h;

    h_win = GUI_FrmWinCreate(&para);
    if( !h_win )
    {
        __err("headbar frmwin create error ! \n");
    }

    return h_win;
}

__s32 headbar_frm_delete(H_WIN frmwin)
{
    if( frmwin )
    {
        GUI_FrmWinDelete(frmwin);
    }

    return EPDK_FAIL;
}
