
#include "app_fm_i.h"

enum
{
    ITEM_AUTOSRH,
#ifdef ADD_ITEM_RECORD
    ITEM_RECORD,//ITEM_FINETUNE,
#endif
    ITEM_CHANNELNU,
    ITEM_MANSRH,
    ITEM_MAX,
};

enum
{
    SRHMODE_IDLE,
    SRHMODE_AUTO,
    SRHMODE_MANUAL,
};

enum
{
    SRHTIPS_YES,
    SRHTIPS_NO,
    SRHTIPS_MAX,
};

enum
{
    FMUISTATE_MAIN,
    FMUISTATE_ITEMS,
    FMUISTATE_TIPS,
    FMUISTATE_VOLUME,
    FMUISTATE_SRHPRG,
};

//main ui item
enum
{
    MAINUI_TITLE = 0x01,//title
    MAINUI_POINTER = 0x02,//freq. pointer
    MAINUI_CHTIPS = 0x04,//channel tips
    MAINUI_SYSTIME = 0x08,//system time&date
    MAINUI_SIGTOWER = 0x10,//signal tower
    MAINUI_ALL = 0x1f,//all ui
};
//main ui action:paint or clear or refresh~
enum
{
    MAINUI_PAINT,
    MAINUI_CLEAR,
    MAINUI_REFRESH,
};

#define FMTIMER_ITEM_ID		(2001)
#define FMTIMER_VOLUME_ID	(2002)
#define FMTIMER_TIME_ID		(2003)

#define ITEM_TIME	(500)	//5s
#define VOLUME_TIME	(300)	//3s
#define TIMEREFRESH_TIME	(100*60) //60s

#define FM_FREQ_MIN	(87000)
#define FM_FREQ_MAX	(108000)

typedef struct tag_item_attr_t
{
    char str_itemsrh[GUI_TITLE_MAX];
    HTHEME bmp_unfocus;
    HTHEME bmp_focus;

} fmplay_itemattr_t;

typedef struct tag_volume_attr_t
{
    HTHEME bmp_volume_bg;
    HTHEME bmp_volume_bar;
    HTHEME bmp_volume_block[3];
    HTHEME bmp_volume_speak[3];

} fmplay_volumeattr_t;

typedef struct tag_fmplay_menu_attr
{
    HTHEME bmp_track[2];//track:mono/stereo
    HTHEME bmp_mainspeaker[2];//speaker:mute or unmute~
    //HTHEME bmp_fmname[2];//name bmp
    char str_fmname[GUI_TITLE_MAX];//name str

    HTHEME bmp_fmscale;//fm scale
    HTHEME	bmp_fmpointer;//fm pointer

    HTHEME bmp_character[4];//character:F\M\H\z
    HTHEME bmp_number[10];//number:0~9
    HTHEME bmp_dot;//dot

    HTHEME bmp_sig;//fm singal

    HTHEME bmp_main_bg;
    HTHEME bmp_menu_bg;
    HTHEME bmp_srh_bg;

    HTHEME bmp_srh_srh;

    fmplay_itemattr_t stItemRes[ITEM_MAX];

    HTHEME bmp_value_top;	//top bg
    HTHEME bmp_value_bottom;	//bottom bg
    HTHEME bmp_value_item;	//item bg

    HTHEME bmp_disk_item;	//disk bg

    HTHEME bmp_search_bg;		//channel search bg
    HTHEME bmp_button[2];	//button select&normal//0:select,1:normal
    char str_channelsrh_title[GUI_TITLE_MAX];
    char str_channelsrh_tips[2][GUI_TITLE_MAX];
    char str_button[2][GUI_TITLE_MAX];//yes and no string

    fmplay_volumeattr_t volumeattr;//volume

    __s32 fmchannelnu;//indicate current channel number
    __u32 fmchannelfreq;//indicate current frequence
    __u32 fmchanneltotal;//indicate total channel

    __s32 fm_volume_value;//record volume value

    __s32 ui_state;//indicate current ui state;

    __s32 search_mode;//indicate search mode : auto or manual~

    __s32 old_focus;
    __s32 new_focus;

    __s32 fmsrhtips_focus;//record channel search tips' cursor focus

    __s32 focus_txt_color;
    __s32 unfocus_txt_color;

    GUI_FONT *font;
    H_LYR	layer;

    __u32	 root_type;
    __u32	 cur_root_type;

} fmplay_menu_attr_t;

typedef struct tag_item_res_t
{
    __s32 res_item_strid;
    __s32 res_item_unfocus_bmpid;
    __s32 res_item_focus_bmpid;

} fmplay_itemares_t;

typedef struct tag_volume_res_t
{
    __s32 res_bg_bmpid;
    __s32 res_bar_bmpid;
    __s32 res_block_bmpid[3];
    __s32 res_speak_bmpid[3];

} fmplay_volume_t;

typedef struct tag_fmplay_res_tab
{
    __s32 res_track_bmpid[2];//track
    __s32 res_mainspeaker_bmpid[2];//speaker:mute or unmute~
    //__s32 res_fmname_bmpid[2];//fm name
    __s32 res_fmname_strid;//fm name

    __s32 res_fmscale_bmpid;//fm scale
    __s32 res_fmpointer_bmpid;//fm pointer

    __s32 res_character_bmpid[4];//character
    __s32 res_number_bmpid[10];//number
    __s32 res_dot_bmpid;//dot

    __s32 res_sig_bmpid;

    __s32 res_main_bg;
    __s32 res_menu_bg;
    __s32 res_srh_bg;

    __s32 res_srh_srh;

    fmplay_itemares_t stItemRes[ITEM_MAX];

    //separator
    //__s32 res_separator_bmpid;

    //value bg
    __s32 res_value_top_bmpid;//top
    __s32 res_value_bottom_bmpid;//bottom
    __s32 res_value_item_bmpid;//item

    __s32 res_value_disk_bg;

    //channel search
    __s32 res_searchbg_bmpid;		//bg
    __s32 res_button_bmpid[2];	//button select&normal
    __s32 res_chsrhtipstitle_strid;	//channel search tips title string
    __s32 res_chsrhtips_strid[2];		//channel search tips string
    __s32 res_button_strid[2];		//button yes&no str

    //volume res
    fmplay_volume_t res_volume;

} fmplay_res_tab_t;

/*resource id tab*/
static fmplay_res_tab_t fmplay_res_tab =
{
    {ID_FM_TRACK1_BMP, ID_FM_TRACK2_BMP}, //track
    {ID_FM_MAINSPEAKER1_BMP, ID_FM_MAINSPEAKER2_BMP}, //speaker:mute or unmute~
    //{ID_FM_CH_FM1_BMP,ID_FM_CH_FM2_BMP},//fm name
    STRING_FM_NAME,//fm name

    ID_FM_MAIN_SCALE_BMP,//fm scale
    ID_FM_MAIN_POINTER_BMP,//fm pointer

    {ID_FM_CH_F_BMP, ID_FM_CH_M_BMP, ID_FM_CH_H_BMP, ID_FM_CH_Z_BMP}, //character:F/M/H/z
    //number 0~9
    {
        ID_FM_NUMBER_0_BMP, ID_FM_NUMBER_1_BMP, ID_FM_NUMBER_2_BMP, ID_FM_NUMBER_3_BMP, ID_FM_NUMBER_4_BMP,
        ID_FM_NUMBER_5_BMP, ID_FM_NUMBER_6_BMP, ID_FM_NUMBER_7_BMP, ID_FM_NUMBER_8_BMP, ID_FM_NUMBER_9_BMP
    },
    //dot
    ID_FM_CH_DOT_BMP,

    ID_FM_BOTM_SIG_BMP,

    ID_FM_MAIN_BG_BMP,//main bg
    ID_FM_MENU_BG_BMP,//menu bg
    ID_FM_SRH_BG_BMP,//search bg

    ID_FM_SRH_SRH_BMP,

    {
        {STRING_FM_AUTOSRH, ID_FM_MENU_AS_BMP, ID_FM_MENU_AS_F_BMP}, //auto search
#ifdef ADD_ITEM_RECORD
        //{STRING_FM_FINETUNE,ID_FM_MENU_FN_BMP,ID_FM_MENU_FN_F_BMP},//fine tuning
        {STRING_FM_RECORD, ID_FM_MENU_RD_BMP, ID_FM_MENU_RD_F_BMP}, //fm record
#endif
        {STRING_FM_CHANNELNU, ID_FM_MENU_CH_BMP, ID_FM_MENU_CH_F_BMP}, //channel number
        {STRING_FM_MANUALSRH, ID_FM_MENU_MS_BMP, ID_FM_MENU_MS_F_BMP}, //manual search
    },

    //value top bmp
    ID_FM_SMENU_UP_BMP,
    //bottom
    ID_FM_SMENU_DW_BMP,
    //value bg: item
    ID_FM_SMENU_BMP,

    //disk
    ID_FM_SMENU_BMP,

    //channel search bg
    ID_FM_TIPS_BG_BMP,
    //button select&normal bg
    {ID_FM_TIPS_BUTTON2_BMP, ID_FM_TIPS_BUTTON1_BMP},
    //channel search tips title str id
    STRING_FM_CHSRH_TITLE,
    //channel search tips str id
    {STRING_FM_CHSRH_TIPS1, STRING_FM_CHSRH_TIPS2},
    //button yes&no str id
    {STRING_FM_TIPS_YES, STRING_FM_TIPS_NO},

    //volume res
    {
        ID_FM_VOLUME_BG_BMP,
        ID_FM_SLIDE_BAR_BMP,
        {ID_FM_SLIDE_BLOCK_L_BMP, ID_FM_SLIDE_BLOCK_BMP, ID_FM_SLIDE_BLOCK_R_BMP},
        {ID_FM_VOLUME_1_BMP, ID_FM_VOLUME_2_BMP, ID_FM_VOLUME_M_BMP},
    },
};

//allocate res
static void fmplay_menu_init_res(fmplay_menu_attr_t *fmplay_menu_attr)
{
    __u32 	i;

    for(i = 0; i < 2; i++)
    {
        fmplay_menu_attr->bmp_track[i] = dsk_theme_open(fmplay_res_tab.res_track_bmpid[i]);
        //fmbtm_menu_attr->bmp_fmname[i] = dsk_theme_open(fmbtm_res_tab.res_fmname_bmpid[i]);
        fmplay_menu_attr->bmp_mainspeaker[i] = dsk_theme_open(fmplay_res_tab.res_mainspeaker_bmpid[i]);
    }

    fmplay_menu_attr->bmp_fmscale = dsk_theme_open(fmplay_res_tab.res_fmscale_bmpid);
    fmplay_menu_attr->bmp_fmpointer = dsk_theme_open(fmplay_res_tab.res_fmpointer_bmpid);

    dsk_langres_get_menu_text(fmplay_res_tab.res_fmname_strid, fmplay_menu_attr->str_fmname, GUI_TITLE_MAX);

    //character
    for(i = 0; i < 4; i++)
    {
        fmplay_menu_attr->bmp_character[i] = dsk_theme_open(fmplay_res_tab.res_character_bmpid[i]);
    }

    //number
    for(i = 0; i < 10; i++)
    {
        fmplay_menu_attr->bmp_number[i] = dsk_theme_open(fmplay_res_tab.res_number_bmpid[i]);
    }
    //dot
    fmplay_menu_attr->bmp_dot = dsk_theme_open(fmplay_res_tab.res_dot_bmpid);

    //singal
    fmplay_menu_attr->bmp_sig = dsk_theme_open(fmplay_res_tab.res_sig_bmpid);

    //main bg
    fmplay_menu_attr->bmp_main_bg = dsk_theme_open(fmplay_res_tab.res_main_bg);
    //menu bg
    fmplay_menu_attr->bmp_menu_bg = dsk_theme_open(fmplay_res_tab.res_menu_bg);
    //srh bg
    fmplay_menu_attr->bmp_srh_bg = dsk_theme_open(fmplay_res_tab.res_srh_bg);

    fmplay_menu_attr->bmp_srh_srh = dsk_theme_open(fmplay_res_tab.res_srh_srh);

    for(i = 0; i < ITEM_MAX; i++)
    {
        dsk_langres_get_menu_text(fmplay_res_tab.stItemRes[i].res_item_strid, fmplay_menu_attr->stItemRes[i].str_itemsrh, GUI_TITLE_MAX);
        fmplay_menu_attr->stItemRes[i].bmp_unfocus = dsk_theme_open(fmplay_res_tab.stItemRes[i].res_item_unfocus_bmpid);
        fmplay_menu_attr->stItemRes[i].bmp_focus = dsk_theme_open(fmplay_res_tab.stItemRes[i].res_item_focus_bmpid);
    }

    //value res
    fmplay_menu_attr->bmp_value_top = dsk_theme_open(fmplay_res_tab.res_value_top_bmpid);
    fmplay_menu_attr->bmp_value_bottom = dsk_theme_open(fmplay_res_tab.res_value_bottom_bmpid);
    fmplay_menu_attr->bmp_value_item = dsk_theme_open(fmplay_res_tab.res_value_item_bmpid);
    //disk
    fmplay_menu_attr->bmp_disk_item = dsk_theme_open(fmplay_res_tab.res_value_item_bmpid);

    //search bg
    fmplay_menu_attr->bmp_search_bg = dsk_theme_open(fmplay_res_tab.res_searchbg_bmpid);
    //button
    for(i = 0; i < 2; i++)
    {
        fmplay_menu_attr->bmp_button[i] = dsk_theme_open(fmplay_res_tab.res_button_bmpid[i]);
    }
    //search tips str
    dsk_langres_get_menu_text(fmplay_res_tab.res_chsrhtipstitle_strid, fmplay_menu_attr->str_channelsrh_title, GUI_TITLE_MAX);
    for(i = 0; i < 2; i++)
    {
        dsk_langres_get_menu_text(fmplay_res_tab.res_chsrhtips_strid[i], fmplay_menu_attr->str_channelsrh_tips[i], GUI_TITLE_MAX);
        dsk_langres_get_menu_text(fmplay_res_tab.res_button_strid[i], fmplay_menu_attr->str_button[i], GUI_TITLE_MAX);
    }

    //volume
    fmplay_menu_attr->volumeattr.bmp_volume_bg = dsk_theme_open(fmplay_res_tab.res_volume.res_bg_bmpid);
    fmplay_menu_attr->volumeattr.bmp_volume_bar = dsk_theme_open(fmplay_res_tab.res_volume.res_bar_bmpid);
    for(i = 0; i < 3; i++)
    {
        fmplay_menu_attr->volumeattr.bmp_volume_block[i] = dsk_theme_open(fmplay_res_tab.res_volume.res_block_bmpid[i]);
    }
    for(i = 0; i < 3; i++)
    {
        fmplay_menu_attr->volumeattr.bmp_volume_speak[i] = dsk_theme_open(fmplay_res_tab.res_volume.res_speak_bmpid[i]);
    }

    return ;
}

//free res
static void fmplay_menu_uninit_res(fmplay_menu_attr_t *fmplay_menu_attr)
{
    __u32 	i;

    for(i = 0; i < 2; i++)
    {
        dsk_theme_close(fmplay_menu_attr->bmp_track[i]);
        //dsk_theme_close(fmbtm_menu_attr->bmp_fmname[i]);
        dsk_theme_close(fmplay_menu_attr->bmp_mainspeaker[i]);
    }

    dsk_theme_close(fmplay_menu_attr->bmp_fmscale);
    dsk_theme_close(fmplay_menu_attr->bmp_fmpointer);

    //character
    for(i = 0; i < 4; i++)
    {
        dsk_theme_close(fmplay_menu_attr->bmp_character[i]);
    }

    //number
    for(i = 0; i < 10; i++)
    {
        dsk_theme_close(fmplay_menu_attr->bmp_number[i]);
    }
    //dot
    dsk_theme_close(fmplay_menu_attr->bmp_dot);

    //signal
    dsk_theme_close(fmplay_menu_attr->bmp_sig);

    //main bg
    dsk_theme_close(fmplay_menu_attr->bmp_main_bg);
    //menu bg
    dsk_theme_close(fmplay_menu_attr->bmp_menu_bg);
    //srh bg
    dsk_theme_close(fmplay_menu_attr->bmp_srh_bg);

    dsk_theme_close(fmplay_menu_attr->bmp_srh_srh);

    for(i = 0; i < ITEM_MAX; i++)
    {
        dsk_theme_close(fmplay_menu_attr->stItemRes[i].bmp_unfocus);
        dsk_theme_close(fmplay_menu_attr->stItemRes[i].bmp_focus);
    }

    //value res
    dsk_theme_close(fmplay_menu_attr->bmp_value_top);
    dsk_theme_close(fmplay_menu_attr->bmp_value_bottom);
    dsk_theme_close(fmplay_menu_attr->bmp_value_item);
    //disk
    dsk_theme_close(fmplay_menu_attr->bmp_disk_item);

    //search bg
    dsk_theme_close(fmplay_menu_attr->bmp_search_bg);
    //button
    for(i = 0; i < 2; i++)
    {
        dsk_theme_close(fmplay_menu_attr->bmp_button[i]);
    }

    //volume
    dsk_theme_close(fmplay_menu_attr->volumeattr.bmp_volume_bg);
    dsk_theme_close(fmplay_menu_attr->volumeattr.bmp_volume_bar);
    for(i = 0; i < 3; i++)
    {
        dsk_theme_close(fmplay_menu_attr->volumeattr.bmp_volume_block[i]);
    }
    for(i = 0; i < 3; i++)
    {
        dsk_theme_close(fmplay_menu_attr->volumeattr.bmp_volume_speak[i]);
    }

}

//function to paint&clear channel tips region (channel number and channel frequence)
static __s32 mainui_channeltips_paint(__gui_msg_t *msg, __u8 ui_act)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    __s32 i;
    __s16 x, y;
    GUI_RECT stGuiRect;
    char ucStringBuf[20] = {0};
    __u32 uCurFreq = 0;
    void *pbmp;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;

    //clear first
    GUI_SetBkColor(0x00);
    stGuiRect.x0 = CHANNELTIPS2_STARTX;
    stGuiRect.y0 = CHANNELTIPS1_STARTY;
    stGuiRect.x1 = 400;
    stGuiRect.y1 = stGuiRect.y0 + 40;
    GUI_ClearRect(stGuiRect.x0, stGuiRect.y0, stGuiRect.x1, stGuiRect.y1);

    if(MAINUI_CLEAR != ui_act)
    {
        //channel num
        x = CHANNELTIPS1_STARTX;
        y = CHANNELTIPS1_STARTY;
        //draw "FM"
        for(i = 0; i < 2; i++)
        {
            pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_character[i]);
            if(NULL != pbmp)
            {
                GUI_BMP_Draw(pbmp, x, y);
            }
            x += BMP_CHARACTER_WIDTH;
        }
        /*suppose fmplay_menu_attr->fmchannelnu is double-digit number*/
        pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_number[fmplay_menu_attr->fmchannelnu / 10]);
        if(NULL != pbmp)
        {
            GUI_BMP_Draw(pbmp, x, y);
        }
        x += BMP_NUMBER_WIDTH;
        pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_number[fmplay_menu_attr->fmchannelnu % 10]);
        if(NULL != pbmp)
        {
            GUI_BMP_Draw(pbmp, x, y);
        }
        //channel frequence
        x = CHANNELTIPS2_STARTX;
        y = CHANNELTIPS2_STARTY;
        /*//convert freq. value to string~
        changefreqvaluetostring(fmplay_menu_attr->fmchannelfreq,ucStringBuf);*/
        uCurFreq = fmplay_menu_attr->fmchannelfreq;
        sprintf(ucStringBuf, "%d.%dMHz", uCurFreq / (1000), uCurFreq % (1000) / 100);
        for(i = 0; i < eLIBs_strlen(ucStringBuf); i++)
        {
            if((ucStringBuf[i] >= '0') && (ucStringBuf[i] <= '9'))
            {
                pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_number[ucStringBuf[i] - '0']);
                if(NULL != pbmp)
                {
                    GUI_BMP_Draw(pbmp, x, y);
                }
                x += BMP_NUMBER_WIDTH;
            }
            else if(ucStringBuf[i] == '.')
            {
                pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_dot);
                if(NULL != pbmp)
                {
                    GUI_BMP_Draw(pbmp, x, y);
                }
                x += BMP_DOT_WIDTH;
            }
            else if(ucStringBuf[i] == 'M')
            {
                pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_character[1]);
                if(NULL != pbmp)
                {
                    GUI_BMP_Draw(pbmp, x, y);
                }
                x += BMP_CHARACTER_WIDTH;
            }
            else if(ucStringBuf[i] == 'H')
            {
                pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_character[2]);
                if(NULL != pbmp)
                {
                    GUI_BMP_Draw(pbmp, x, y);
                }
                x += BMP_CHARACTER_WIDTH;
            }
            else if(ucStringBuf[i] == 'z')
            {
                pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_character[3]);
                if(NULL != pbmp)
                {
                    GUI_BMP_Draw(pbmp, x, y);
                }
                x += BMP_CHARACTER_WIDTH;
            }
            else//in case of mistake
            {
                x += BMP_NUMBER_WIDTH;
            }
        }
    }

    return EPDK_OK;
}

static void time_to_char(char *buff, __time_t time)
{
    buff[0] = time.hour / 10 + '0';
    buff[1] = time.hour % 10 + '0';

    buff[2] = ':';

    buff[3] = time.minute / 10 + '0';
    buff[4] = time.minute % 10 + '0';
    buff[5] = '\0';
    /*
    	buff[5] = ':';

    	buff[6] = time.second/10 + '0';
    	buff[7] = time.second%10 + '0';

    	buff[8] = '\0';
    */
}

static void date_to_char(char *buff, __date_t date)
{
    buff[0] = date.year / 1000 + '0';
    buff[1] = (date.year % 1000) / 100 + '0';
    buff[2] = ((date.year % 1000) % 100) / 10 + '0';
    buff[3] = (date.year % 1000) % 100 % 10 + '0';

    buff[4] = '/';
    buff[5] = date.month / 10 + '0';
    buff[6] = date.month % 10 + '0';

    buff[7] = '/';
    buff[8] = date.day / 10 + '0';
    buff[9] = date.day % 10 + '0';
    buff[10] = '\0';
    /*
    	buff[0] = date.month/10 + '0';
    	buff[1] = date.month%10 + '0';

    	buff[2] = '-';
    	buff[3] = date.day/10 + '0';
    	buff[4] = date.day%10 + '0';
    	buff[5]= '\0';
    */
}


static void fm_check_disk(__gui_msg_t *msg)
{
    __s32 i;
    __s32 ret;
    __s32 cnt;
    char diskname[RAT_MAX_PARTITION][4];
    fmplay_menu_attr_t *fmplay_menu_attr;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return;

    __msg("**********check_disk**********\n");
    cnt = 0;
    fmplay_menu_attr->root_type = RAT_UNKNOWN;
    fmplay_menu_attr->cur_root_type = RAT_UNKNOWN;

    ret = rat_get_partition_name(RAT_USB_DISK, diskname);
    if(EPDK_OK == ret)
    {
        i = 0;
        //while (i < RAT_MAX_PARTITION)
        {
            if (diskname[i][0])
            {
                __msg("SD : diskname[%d]=%s\n", i, diskname[i]);
                fmplay_menu_attr->root_type |= RAT_USB;
                fmplay_menu_attr->cur_root_type = RAT_USB;
                cnt++;
            }
            i++;
        }
    }

    ret = rat_get_partition_name(RAT_SD_CARD, diskname);
    if(EPDK_OK == ret)
    {
        i = 0;
        //while (i < RAT_MAX_PARTITION)
        {
            if (diskname[i][0])
            {
                __msg("SD : diskname[%d]=%s\n", i, diskname[i]);
                fmplay_menu_attr->root_type |= (RAT_TF << 8);
                fmplay_menu_attr->cur_root_type = RAT_TF;
                cnt++;
            }
            i++;
        }
    }

}

//function to paint/clear/refresh systime: system time&date
static __s32 mainui_systime_paint(__gui_msg_t *msg, __u8 ui_act)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    __s16 x, y;
    __date_t date;
    __time_t time;
    char ucTimeBuf[32] = {0};
    char ucDateBuf[32] = {0};
    //static __date_t date_last;
    void *pbmp;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;

#define SYSMAINBG_STARTX	(0)
#define SYSMAINBG_STARTY	(200)
#define SYSTIME_STARTX	(240)
#define SYSTIME_STARTY	(210)
#define SYSDATE_STARTX	(300)
#define SYSDATE_STARTY	(SYSTIME_STARTY)

    //draw main bg (always)
    x = SYSMAINBG_STARTX;
    y = SYSMAINBG_STARTY;
    pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_menu_bg);
    if(NULL != pbmp)
    {
        GUI_BMP_Draw(pbmp, x, y);
    }

#ifdef BEETLES_RTC_EXIST
    //draw system time&date
    esTIME_GetTime(&time);//system date
    esTIME_GetDate(&date);//system time
    /*
    	if(MAINUI_REFRESH == ui_act)
    	{
    		x = SYSTIME_STARTX;
    		y = SYSTIME_STARTY;
    		//sprintf(ucTimeBuf,"%d:%02d",time.hour,time.minute);
    		time_to_char(ucTimeBuf,time);
    		GUI_SetColor(fmplay_menu_attr->unfocus_txt_color);
    		GUI_DispStringAt(ucTimeBuf,x,y);

    		if((date.day != date_last.day)||(date.month != date_last.month)||(date.year != date_last.year))
    		{
    			//record new date
    			date_last.year = date.year;
    			date_last.month = date.month;
    			date_last.day = date.day;

    			x = SYSDATE_STARTX;
    			y = SYSDATE_STARTY;
    			//sprintf(ucDateBuf,"%4d/%02d/%02d",date.year,date.month,date.day);
    			date_to_char(ucDateBuf,date);
    			GUI_DispStringAt(ucDateBuf,x,y);
    		}
    	}
    	else
    */
    {
        x = SYSTIME_STARTX;
        y = SYSTIME_STARTY;
        //sprintf(ucTimeBuf,"%d:%02d",time.hour,time.minute);
        time_to_char(ucTimeBuf, time);
        GUI_SetColor(fmplay_menu_attr->unfocus_txt_color);
        GUI_DispStringAt(ucTimeBuf, x, y);
        x = SYSDATE_STARTX;
        y = SYSDATE_STARTY;
        //sprintf(ucDateBuf,"%4d/%02d/%02d",date.year,date.month,date.day);
        date_to_char(ucDateBuf, date);
        GUI_DispStringAt(ucDateBuf, x, y);
    }
#endif
    return EPDK_OK;
}

//function to paint&clear title region (track/fm name/voluem icon&num)
static __s32 mainui_title_paint(__gui_msg_t *msg, __u8 ui_act)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    __s16 x, y;
    void *pbmp;
    GUI_RECT stGuiRect;
    //char ucStrBuf[10]={0};

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;

    if(MAINUI_PAINT == ui_act)
    {
        //draw main bg
        x = 0;
        y = 0;
        pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_main_bg);
        if(NULL != pbmp)
        {
            GUI_BMP_Draw(pbmp, x, y);
        }
        //draw track: mono or stereo
        x = TRACK_STARTX;
        y = TRACK_STARTY;
        pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_track[0]);
        if(NULL != pbmp)
        {
            GUI_BMP_Draw(pbmp, x, y);
        }
        //draw fm name
        /*
        if(0)//system language is english
        {
        	idx = 0;
        }
        else
        {
        	idx = 1;
        }
        pbmp = dsk_theme_hdl2buf(fmbtm_menu_attr->bmp_fmname[idx]);
        if(NULL == pbmp)
        {
        	__msg("dsk_theme_hdl2buf fail\n");
        	return EPDK_FAIL;
        }

        hbmp = bmp_open(pbmp);
        if(NULL == hbmp)
        {
        	__msg("open bmp fail\n");
        	return EPDK_FAIL;
        }
        bmp_width = bmp_get_width(hbmp);
        bmp_height = bmp_get_height(hbmp);
        bmp_close(hbmp);
        hbmp = 0;

        bmp_x = (400 - bmp_width)/2;
        bmp_y = FMNAME_STARTY;
        GUI_BMP_Draw(pbmp,bmp_x, bmp_y);*/
        stGuiRect.x0 = 0;
        stGuiRect.y0 = 0;
        stGuiRect.x1 = stGuiRect.x0 + 400;
        stGuiRect.y1 = stGuiRect.y0 + 40;
        GUI_SetColor(fmplay_menu_attr->unfocus_txt_color);
        GUI_DispStringInRect(fmplay_menu_attr->str_fmname, &stGuiRect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        //draw volume
        x = VOLUME_STARTX;
        y = VOLUME_STARTY;

        if(fmplay_menu_attr->fm_volume_value == 0)
        {
            pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_mainspeaker[0]);
        }
        else
        {
            pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_mainspeaker[1]);
        }

        if(NULL != pbmp)
        {
            GUI_BMP_Draw(pbmp, x, y);
        }
        /*
        x += 18;
        sprintf(ucStrBuf,"%d",fmplay_menu_attr->fm_volume_value);
        stGuiRect.x0 = x-4;
        stGuiRect.y0 = y;
        stGuiRect.x1 = stGuiRect.x0+17;
        stGuiRect.y1 = stGuiRect.y0+16;
        GUI_SetColor(fmplay_menu_attr->unfocus_txt_color);
        GUI_DispStringInRect(ucStrBuf, &stGuiRect, GUI_TA_HCENTER|GUI_TA_VCENTER);
        */
    }
    else
    {
        /*
        GUI_SetBkColor(0x00);
        stGuiRect.x0 = 0;
        stGuiRect.y0 = 0;
        stGuiRect.x1 = 400;
        stGuiRect.y1 = stGuiRect.y0 + 40;
        GUI_ClearRect(stGuiRect.x0,stGuiRect.y0,stGuiRect.x1,stGuiRect.y1);
        */
        //draw main bg
        pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_main_bg);
        if(NULL != pbmp)
        {
            GUI_BMP_Draw(pbmp, 0, 0);
        }
    }

    return EPDK_OK;
}

//function to paint signal tower~
static __s32 mainui_sigtower_paint(__gui_msg_t *msg, __u8 ui_act)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    __s16 x, y;
    void *pbmp;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;

    ui_act = ui_act;//reserved, for future used~

    //draw ui~
    //draw signal tower
    x = SIGNALTOWER_STARTX;
    y = SIGNALTOWER_STARTY;
    pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_sig);
    if(NULL == pbmp)
    {
        __msg("dsk_theme_hdl2buf fail\n");
        return EPDK_FAIL;
    }
    GUI_BMP_Draw(pbmp, x, y);

    return EPDK_OK;
}

//function to draw scale and pointer~
static __s32 mainui_scale_paint(__gui_msg_t *msg, __u8 ui_act)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    __s16 x, y;
    void *pbmp;
    GUI_RECT stGuiRect;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;

    //clear first~
    GUI_SetBkColor(0x00);
    stGuiRect.x0 = SCALE_STARTX;
    stGuiRect.y0 = SCALE_STARTY;
    stGuiRect.x1 = 400;
    stGuiRect.y1 = stGuiRect.y0 + 50;
    GUI_ClearRect(stGuiRect.x0, stGuiRect.y0, stGuiRect.x1, stGuiRect.y1);
    //draw scale
    x = SCALE_STARTX;
    y = SCALE_STARTY;

    pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_fmscale);
    if(NULL != pbmp)
    {
        GUI_BMP_Draw(pbmp, x, y);
    }
    //draw freq. str
    GUI_SetColor(fmplay_menu_attr->unfocus_txt_color);
    GUI_DispStringAt("87.0MHz", FREQSTR1_STARTX, FREQSTR1_STARTY);
    GUI_DispStringAt("108.0MHz", FREQSTR2_STARTX, FREQSTR2_STARTY);
    //draw freq. pointer~
    x = (fmplay_menu_attr->fmchannelfreq - 87000) * 400 / (108000 - 87000);
    y = SCALE_STARTY;

    pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_fmpointer);
    if(NULL != pbmp)
    {
        GUI_BMP_Draw(pbmp, x, y);
    }

    return EPDK_OK;
}


static __s32 fmplay_mainui_paint(__gui_msg_t *msg, __u8 ui_item, __u8 ui_act)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    GUI_MEMDEV_Handle	draw_mem;
    FB  fb;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;

    GUI_LyrWinGetFB(fmplay_menu_attr->layer, &fb);

    GUI_LyrWinSel(fmplay_menu_attr->layer);
    GUI_SetFont(fmplay_menu_attr->font);
    GUI_UC_SetEncodeUTF8();

    draw_mem = GUI_MEMDEV_Create(0, 0, fb.size.width,  fb.size.height);
    GUI_MEMDEV_Select(draw_mem);

    if(ui_item & (MAINUI_TITLE | MAINUI_SYSTIME))
    {
        fmplay_menu_attr->ui_state = FMUISTATE_MAIN;
    }

    //title(track/fm name/voluem icon&num)
    if(ui_item & MAINUI_TITLE)
    {
        mainui_title_paint(msg, ui_act);
    }

    //signal tower
    if(ui_item & MAINUI_SIGTOWER)
    {
        mainui_sigtower_paint(msg, ui_act);
    }

    //draw scale: scale and pointer~
    if(ui_item & MAINUI_POINTER)
    {
        mainui_scale_paint(msg, ui_act);
    }

    //draw channel tips: channel num and frequence~
    if(ui_item & MAINUI_CHTIPS)
    {
        mainui_channeltips_paint(msg, ui_act);
    }

    //draw system time&date
    if(ui_item & MAINUI_SYSTIME)
    {
        mainui_systime_paint(msg, ui_act);
    }

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select( NULL );
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;

    return EPDK_OK;
}

static __s32 fmplay_searchprogress_paint(__gui_msg_t *msg, __u32 cursearch_freq)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    GUI_MEMDEV_Handle	draw_mem;
    FB  fb;
    __s32 i, max;
    __s16 x, y;
    void *pbmp;
    GUI_RECT stGuiRect;
    char ucStrBuf[20] = {0};
    __u32 uCurFreq = 0;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;

    if (GUI_LyrWinGetSta(fmplay_menu_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    GUI_LyrWinGetFB(fmplay_menu_attr->layer, &fb);

    GUI_LyrWinSel(fmplay_menu_attr->layer);
    GUI_SetFont(fmplay_menu_attr->font);
    GUI_UC_SetEncodeUTF8();

    draw_mem = GUI_MEMDEV_Create(0, 0, fb.size.width,  fb.size.height);
    GUI_MEMDEV_Select(draw_mem);

    fmplay_menu_attr->ui_state = FMUISTATE_SRHPRG;

    //clear first~
    GUI_SetBkColor(0x00);
    stGuiRect.x0 = FMMODE_MENUUI_STARTX;
    stGuiRect.y0 = FMMODE_MENUUI_STARTY;
    stGuiRect.x1 = stGuiRect.x0 + FMMODE_MENUUI_WIDTH;
    stGuiRect.y1 = stGuiRect.y0 + FMMODE_MENUUI_HEIGHT;
    GUI_ClearRect(stGuiRect.x0, stGuiRect.y0, stGuiRect.x1, stGuiRect.y1);

#define FMSRHPRGBG_WIDTH	(400)
#define FMSRHPRGBG_HEIGHT	(30)
#define FMSRHPRGBG_STARTX	(0)
#define FMSRHPRGBG_STARTY	(240-FMSRHPRGBG_HEIGHT)

    //draw bg
    x = FMSRHPRGBG_STARTX;
    y = FMSRHPRGBG_STARTY;
    pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_srh_bg);
    if(NULL != pbmp)
    {
        GUI_BMP_Draw(pbmp, x, y);
    }

    //draw scan
    x = 38;
    y = 240 - 26;
    pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_srh_srh);
    if(NULL != pbmp)
    {
        GUI_BMP_Draw(pbmp, x, y);
    }

    //draw bar
    x = FMSRHPRGBG_STARTX + (FMSRHPRGBG_WIDTH - BMP_FMVOLUEMBAR_WIDTH) / 2;
    y = FMSRHPRGBG_STARTY + (FMSRHPRGBG_HEIGHT - BMP_FMVOLUEMBAR_HEIGHT) / 2;
    pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->volumeattr.bmp_volume_bar);
    if(NULL != pbmp)
    {
        GUI_BMP_Draw(pbmp, x, y);
    }
    //draw block
    max = (cursearch_freq - FM_FREQ_MIN) * 51 / (FM_FREQ_MAX - FM_FREQ_MIN);
    if(max > 0)
    {
        for(i = 1; i <= max; i++)
        {
            if(i == 1)
            {
                pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->volumeattr.bmp_volume_block[0]);
            }
            else if(i == 51)
            {
                pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->volumeattr.bmp_volume_block[2]);
            }
            else
            {
                pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->volumeattr.bmp_volume_block[1]);
            }
            if(NULL != pbmp)
            {
                GUI_BMP_Draw(pbmp, x, y);
            }
            x += BMP_FMVOLUEMBLOCK_WIDTH;
        }
    }

    //draw freq~
    x = 336;//FMVOLUMEBG_STARTX+6;
    y = 217;//FMVOLUMEBG_STARTY+4;
    //draw string
    /*//convert freq. value to string~
    //changefreqvaluetostring(fmplay_menu_attr->fmchannelfreq,ucStrBuf);*/
    uCurFreq = cursearch_freq;
    sprintf(ucStrBuf, "%d.%dMHz", uCurFreq / (1000), uCurFreq % (1000) / 100);
    GUI_SetColor(fmplay_menu_attr->unfocus_txt_color);
    GUI_DispStringAt(ucStrBuf, x, y);

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select( NULL );
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;

    return EPDK_OK;
}

/*
static __s32 fmplay_volume_clear(__gui_msg_t *msg)
{
	fmplay_menu_attr_t *fmplay_menu_attr;
	GUI_RECT stGuiRect;

	fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
	if (fmplay_menu_attr==NULL)
		return EPDK_FAIL;

	GUI_SetBkColor(0x00);
	stGuiRect.x0 = FMVOLUMEBG_STARTX;
	stGuiRect.y0 = FMVOLUMEBG_STARTY;
	stGuiRect.x1 = stGuiRect.x0 + BMP_FMVOLUEMBG_WIDTH;
	stGuiRect.y1 = stGuiRect.y0 + BMP_FMVOLUEMBG_HEIGHT;
	GUI_ClearRect(stGuiRect.x0,stGuiRect.y0,stGuiRect.x1,stGuiRect.y1);

	return EPDK_OK;

}
*/

static __s32 fmplay_volume_paint(__gui_msg_t *msg, __u8 paint_mode)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    GUI_MEMDEV_Handle	draw_mem;
    FB  fb;
    __s32 i, j, max;
    __s16 x, y;
    void *pbmp;
    char vol[5] = {0};
    //GUI_RECT stGuiRect;
    //char ucStrBuf[10]={0};

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;

    if (GUI_LyrWinGetSta(fmplay_menu_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    GUI_LyrWinGetFB(fmplay_menu_attr->layer, &fb);

    GUI_LyrWinSel(fmplay_menu_attr->layer);
    GUI_SetFont(fmplay_menu_attr->font);
    GUI_UC_SetEncodeUTF8();

    draw_mem = GUI_MEMDEV_Create(0, 0, fb.size.width,  fb.size.height);
    GUI_MEMDEV_Select(draw_mem);

    fmplay_menu_attr->ui_state = FMUISTATE_VOLUME;

    //draw bg
    x = FMVOLUMEBG_STARTX;
    y = FMVOLUMEBG_STARTY;

    pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->volumeattr.bmp_volume_bg);
    if(NULL != pbmp)
    {
        GUI_BMP_Draw(pbmp, x, y);
    }

    //draw bar
    x = FMVOLUMEBG_STARTX + (BMP_FMVOLUEMBG_WIDTH - BMP_FMVOLUEMBAR_WIDTH) / 2 - 12;
    y = FMVOLUMEBG_STARTY + (BMP_FMVOLUEMBG_HEIGHT - BMP_FMVOLUEMBAR_HEIGHT) / 2;
    pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->volumeattr.bmp_volume_bar);
    if(NULL != pbmp)
    {
        GUI_BMP_Draw(pbmp, x, y);
    }
    //draw block
    for(i = 1; i <= fmplay_menu_attr->fm_volume_value; i++)
    {
        if((i <= 21)) //2 blocks
        {
            max = 2;
        }
        else//1 blocks
        {
            max = 1;
        }
        for(j = 0; j < max; j++)
        {
            if((i == 1) && (j == 0))
            {
                pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->volumeattr.bmp_volume_block[0]);
            }
            else if((i == 30) && (j == max - 1))
            {
                pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->volumeattr.bmp_volume_block[2]);
            }
            else
            {
                pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->volumeattr.bmp_volume_block[1]);
            }
            if(NULL != pbmp)
            {
                GUI_BMP_Draw(pbmp, x, y);
            }
            x += BMP_FMVOLUEMBLOCK_WIDTH;
        }
    }

    //draw speak
    x = 40 - 3; //FMVOLUMEBG_STARTX+6;
    y = FMVOLUMEBG_STARTY + 4;
    if(fmplay_menu_attr->fm_volume_value == 0)
    {
        pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->volumeattr.bmp_volume_speak[2]);
        if(NULL != pbmp)
        {
            GUI_BMP_Draw(pbmp, x, y);
        }
        x = 346 - 18; //FMVOLUMEBG_STARTX+46+BMP_FMVOLUEMBAR_WIDTH+26;
        pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->volumeattr.bmp_volume_speak[2]);
        if(NULL != pbmp)
        {
            GUI_BMP_Draw(pbmp, x, y);
        }
    }
    else
    {
        pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->volumeattr.bmp_volume_speak[0]);
        if(NULL != pbmp)
        {
            GUI_BMP_Draw(pbmp, x, y);
        }
        x = 346 - 18; //FMVOLUMEBG_STARTX+46+BMP_FMVOLUEMBAR_WIDTH+26;
        pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->volumeattr.bmp_volume_speak[1]);
        if(NULL != pbmp)
        {
            GUI_BMP_Draw(pbmp, x, y);
        }
    }
    eLIBs_int2str_dec(fmplay_menu_attr->fm_volume_value, vol);
    GUI_SetColor(APP_COLOR_WHITE);
    GUI_DispStringAt( vol, 346, y );
    /*
    	//draw volume number
    	if(fmplay_menu_attr->fm_volume_value>0)
    	{
    		sprintf(ucStrBuf,"%d",fmplay_menu_attr->fm_volume_value);
    		stGuiRect.x0 = 350+10;//FMVOLUMEBG_STARTX+46+BMP_FMVOLUEMBAR_WIDTH+26+BMP_FMVOLUMESPEAK_WIDTH+10;
    		stGuiRect.x1 = stGuiRect.x0+32;
    		stGuiRect.y0 = 14;
    		stGuiRect.y1 = stGuiRect.y0+13;
    		GUI_SetColor(fmplay_menu_attr->unfocus_txt_color);
    		GUI_DispStringInRect(ucStrBuf, &stGuiRect, GUI_TA_HCENTER|GUI_TA_VCENTER);
    	}
    */
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select( NULL );
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;

    return EPDK_OK;

}

static __s32 fmplay_srhtips_clear(__gui_msg_t *msg)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    GUI_RECT stGuiRect;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;

    fmplay_menu_attr->ui_state = FMUISTATE_MAIN;

    GUI_SetBkColor(0x00);
    stGuiRect.x0 = SRHTIPFRAME_STARTX;
    stGuiRect.y0 = SRHTIPFRAME_STARTY;
    stGuiRect.x1 = stGuiRect.x0 + BMP_SRHBG_WIDTH;
    stGuiRect.y1 = stGuiRect.y0 + BMP_SRHBG_HEIGHT;
    GUI_ClearRect(stGuiRect.x0, stGuiRect.y0, stGuiRect.x1, stGuiRect.y1);

    return EPDK_OK;
}


//paint channel search tips frame
static __s32 fmplay_srhtips_paint(__gui_msg_t *msg, __u8 paint_mode)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    GUI_MEMDEV_Handle	draw_mem;
    FB  fb;
    __s32 i;
    __s16 x, y;
    GUI_RECT stGuiRect;
    void *pbmp;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;

    GUI_LyrWinGetFB(fmplay_menu_attr->layer, &fb);

    GUI_LyrWinSel(fmplay_menu_attr->layer);
    GUI_SetFont(fmplay_menu_attr->font);
    GUI_UC_SetEncodeUTF8();

    draw_mem = GUI_MEMDEV_Create(0, 0, fb.size.width,  fb.size.height);
    GUI_MEMDEV_Select(draw_mem);

    fmplay_menu_attr->ui_state = FMUISTATE_TIPS;

    if(MENU_PAINT_ALL == paint_mode)
    {
        fmplay_menu_attr->fmsrhtips_focus = SRHTIPS_NO;

        x = SRHTIPFRAME_STARTX;
        y = SRHTIPFRAME_STARTY;
        //draw bg
        pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_search_bg);
        if(NULL != pbmp)
        {
            GUI_BMP_Draw(pbmp, x, y);
        }
        //draw tip string
        //title
        stGuiRect.x0 = SRHTIPFRAME_STARTX;
        stGuiRect.y0 = SRHTIPFRAME_STARTY;
        stGuiRect.x1 = stGuiRect.x0 + BMP_SRHBG_WIDTH;
        stGuiRect.y1 = stGuiRect.y0 + 22;
        GUI_SetColor(fmplay_menu_attr->unfocus_txt_color);
        GUI_DispStringInRect(fmplay_menu_attr->str_channelsrh_title, &stGuiRect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        //tips
        x = SRHTIPFRAME_STARTX + 46;
        y = SRHTIPFRAME_STARTY + 32;
        for(i = 0; i < 2; i++)
        {
            y += 20 * i;
            GUI_DispStringAt(fmplay_menu_attr->str_channelsrh_tips[i], x, y);
        }
    }
    //draw button
    x = SRHTIPFRAME_STARTX + 66;
    y = SRHTIPFRAME_STARTY + BMP_SRHBG_HEIGHT - BMP_BUTTON_HEIGHT - 8;
    stGuiRect.y0 = y;
    stGuiRect.y1 = stGuiRect.y0 + BMP_BUTTON_HEIGHT;
    for(i = 0; i < SRHTIPS_MAX; i++)
    {
        x += (BMP_BUTTON_WIDTH + 20) * i;
        if(fmplay_menu_attr->fmsrhtips_focus == i)
        {
            pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_button[0]);
            if(NULL != pbmp)
            {
                GUI_BMP_Draw(pbmp, x, y);
            }
        }
        else
        {
            pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_button[1]);
            if(NULL != pbmp)
            {
                GUI_BMP_Draw(pbmp, x, y);
            }
        }
        stGuiRect.x0 = x;
        stGuiRect.x1 = stGuiRect.x0 + BMP_BUTTON_WIDTH;
        GUI_SetColor(APP_COLOR_BLACK);//black color
        GUI_DispStringInRect(fmplay_menu_attr->str_button[i], &stGuiRect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select( NULL );
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;

    return EPDK_OK;
}

static __s32 fmplay_disk_clear(__gui_msg_t *msg, __u8 ucIdx)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    GUI_RECT stGuiRect;
    __s16 x, y;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;

    /*if( ITEM_RECORD == fmplay_menu_attr->new_focus )
    {
    	return EPDK_OK;
    }*/

    GUI_SetBkColor(0x00);

    GUI_ClearRect(100, 75, 186, 199);

    return EPDK_OK;
}

static __s32 fmplay_disk_paint(__gui_msg_t *msg, __u8 paint_mode)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    char ucStrBuf[20] = {0};
    void *pbmp;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;

    fmplay_disk_clear(msg, ITEM_RECORD);
    if( ITEM_RECORD != fmplay_menu_attr->new_focus )
    {
        return EPDK_OK;
    }

    //draw disk;
    pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_disk_item);
    if(NULL != pbmp)
    {
        GUI_BMP_Draw(pbmp, 100, 165);
        GUI_BMP_Draw(pbmp, 100, 172);
        if( fmplay_menu_attr->root_type == (RAT_USB | (RAT_TF << 8)) )
        {
            GUI_BMP_Draw(pbmp, 100, 145);
            if( fmplay_menu_attr->cur_root_type == RAT_USB )
            {
                GUI_SetColor(APP_COLOR_YELLOW);
                GUI_DispStringAt( "USB", 129, 150 );
                GUI_SetColor(APP_COLOR_WHITE);
                GUI_DispStringAt( "SD", 129, 178 );
            }
            else
            {
                GUI_SetColor(APP_COLOR_WHITE);
                GUI_DispStringAt( "USB", 129, 150 );
                GUI_SetColor(APP_COLOR_YELLOW);
                GUI_DispStringAt( "SD", 129, 178 );
            }
        }
        else if( fmplay_menu_attr->root_type == RAT_USB )
        {
            GUI_SetColor(APP_COLOR_YELLOW);
            GUI_DispStringAt( "USB", 129, 172 );
        }
        else if( fmplay_menu_attr->root_type == (RAT_TF << 8) )
        {
            GUI_SetColor(APP_COLOR_YELLOW);
            GUI_DispStringAt( "SD", 129, 172 );
        }
        else if( fmplay_menu_attr->root_type == RAT_UNKNOWN )
        {
            GUI_SetColor(APP_COLOR_YELLOW);
            GUI_DispStringAt( "No disk", 117, 172 );
        }
        __inf("\n\n\n          disk:  %d          \n\n\n", fmplay_menu_attr->root_type);
    }
    return EPDK_OK;
}

static __s32 fmplay_submenu_paint(__gui_msg_t *msg, __u8 paint_mode)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    __s16 x, y;
    GUI_RECT stGuiRect;
    char ucStrBuf[20] = {0};
    void *pbmp;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;

    if((ITEM_AUTOSRH == fmplay_menu_attr->new_focus)
            || (ITEM_MANSRH == fmplay_menu_attr->new_focus)
#ifdef ADD_ITEM_RECORD
            /*||(ITEM_FINETUNE == fmplay_menu_attr->new_focus)*/
            || (ITEM_RECORD == fmplay_menu_attr->new_focus)
#endif
      )
    {
        return EPDK_OK;
    }

    x = ITEMS_STARTX + (BMP_ITEMBG_WIDTH + BMP_SEPARATOR_WIDTH) * fmplay_menu_attr->new_focus;
    //x -= (BMP_VALUESIDE_WIDTH-BMP_ITEMBG_WIDTH)/2;
    y = (FMMODE_MENUUI_STARTY - (BMP_VALUEITEM_HEIGHT + BMP_VALUESIDE_HEIGHT * 2) - 4);

    //draw item top
    pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_value_top);
    if(NULL != pbmp)
    {
        GUI_BMP_Draw(pbmp, x, y);
    }
    //draw item bg
    pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_value_item);
    if(NULL != pbmp)
    {
        GUI_BMP_Draw(pbmp, x, y + BMP_VALUESIDE_HEIGHT);
    }
    //draw bottom
    pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_value_bottom);
    if(NULL != pbmp)
    {
        GUI_BMP_Draw(pbmp, x, y + BMP_VALUESIDE_HEIGHT + BMP_VALUEITEM_HEIGHT);
    }

    if(ITEM_CHANNELNU == fmplay_menu_attr->new_focus)
    {
        //draw string
        stGuiRect.x0 = x;
        stGuiRect.x1 = stGuiRect.x0 + BMP_VALUEITEM_WIDTH;
        stGuiRect.y0 = y + BMP_VALUESIDE_HEIGHT;
        stGuiRect.y1 = stGuiRect.y0 + BMP_VALUEITEM_HEIGHT;
        GUI_SetColor(fmplay_menu_attr->focus_txt_color);
        sprintf(ucStrBuf, "FM%d", fmplay_menu_attr->fmchannelnu);
        GUI_DispStringInRect(ucStrBuf, &stGuiRect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }
    /*
    else if(ITEM_FINETUNE == fmplay_menu_attr->new_focus)
    {
    	__u32 uCurFreq = 0;
    	//draw string
    	stGuiRect.x0 = x;
    	stGuiRect.x1 = stGuiRect.x0+BMP_VALUEITEM_WIDTH;
    	stGuiRect.y0 = y+BMP_VALUESIDE_HEIGHT;
    	stGuiRect.y1 = stGuiRect.y0+BMP_VALUEITEM_HEIGHT;
    	//convert freq. value to string~
    	//changefreqvaluetostring(fmplay_menu_attr->fmchannelfreq,ucStrBuf);
    	uCurFreq = fmplay_menu_attr->fmchannelfreq;
    	sprintf(ucStrBuf,"%d.%dMHz",uCurFreq/(1000),uCurFreq%(1000)/100);
    	GUI_SetColor(fmplay_menu_attr->focus_txt_color);
    	GUI_DispStringInRect(ucStrBuf, &stGuiRect, GUI_TA_HCENTER|GUI_TA_VCENTER);
    }*/

    return EPDK_OK;
}

static __s32 fmplay_submenu_clear(__gui_msg_t *msg, __u8 ucIdx)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    GUI_RECT stGuiRect;
    __s16 x, y;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;

    if((ITEM_AUTOSRH == ucIdx)
            || (ITEM_MANSRH == ucIdx))
    {
        return EPDK_OK;
    }

    GUI_SetBkColor(0x00);

    x = ITEMS_STARTX + (BMP_ITEMBG_WIDTH + BMP_SEPARATOR_WIDTH) * ucIdx;
    //x -= (BMP_VALUESIDE_WIDTH-BMP_ITEMBG_WIDTH)/2;
    y = (FMMODE_MENUUI_STARTY - (BMP_VALUEITEM_HEIGHT + BMP_VALUESIDE_HEIGHT * 2) - 4);

    stGuiRect.x0 = x;
    stGuiRect.y0 = y;
    stGuiRect.x1 = stGuiRect.x0 + BMP_VALUEITEM_WIDTH;
    stGuiRect.y1 = stGuiRect.y0 + BMP_VALUESIDE_HEIGHT * 2 + BMP_VALUEITEM_HEIGHT;
    GUI_ClearRect(stGuiRect.x0, stGuiRect.y0, stGuiRect.x1, stGuiRect.y1);

    return EPDK_OK;
}

static __s32 fmplay_item_paint(__gui_msg_t *msg, __u8 ucIdx)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    __s16 x, y;
    void *pbmp;
    GUI_RECT stGuiRect;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;

    y = ITEMS_STARTY + 3;
    x = ITEMS_STARTX + (BMP_ITEMBG_WIDTH + BMP_SEPARATOR_WIDTH) * ucIdx;

    stGuiRect.x0 = x + BMP_ITEMBG_WIDTH;
    stGuiRect.y0 = y;
    stGuiRect.x1 = stGuiRect.x0 + BMP_SEPARATOR_WIDTH;
    stGuiRect.y1 = stGuiRect.y0 + BMP_ITEMBG_HEIGHT;

    if(fmplay_menu_attr->new_focus == ucIdx)
    {
        pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->stItemRes[ucIdx].bmp_focus);
        if(NULL != pbmp)
        {
            GUI_BMP_Draw(pbmp, x, y);
        }
        GUI_SetColor(fmplay_menu_attr->focus_txt_color);
        GUI_DispStringInRect(fmplay_menu_attr->stItemRes[ucIdx].str_itemsrh, &stGuiRect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }
    else
    {
        pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->stItemRes[ucIdx].bmp_unfocus);
        if(NULL != pbmp)
        {
            GUI_BMP_Draw(pbmp, x, y);
        }
        GUI_SetColor(fmplay_menu_attr->unfocus_txt_color);
        GUI_DispStringInRect(fmplay_menu_attr->stItemRes[ucIdx].str_itemsrh, &stGuiRect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }

    return EPDK_OK;
}

static __s32 fmplay_menu_clear(__gui_msg_t *msg)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    __s16 x, y;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;
    //clear submenu rect
    fmplay_submenu_clear(msg, fmplay_menu_attr->new_focus);
    fmplay_disk_clear(msg, fmplay_menu_attr->new_focus);
    //clear bg rect
    GUI_SetBkColor(0x00);
    x = FMMODE_MENUUI_STARTX;
    y = FMMODE_MENUUI_STARTY;
    GUI_ClearRect(x, y, x + FMMODE_MENUUI_WIDTH, y + FMMODE_MENUUI_HEIGHT);

    return EPDK_OK;
}

static __s32 fmplay_menu_paint(__gui_msg_t *msg, __u8 paint_mode)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    GUI_MEMDEV_Handle	draw_mem;
    FB  fb;
    __s32 i;
    __s16 x, y;
    //__s32 bmp_width;
    //__s32 bmp_height;
    void *pbmp;
    //HBMP hbmp;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (fmplay_menu_attr == NULL)
        return EPDK_FAIL;

    if (GUI_LyrWinGetSta(fmplay_menu_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    GUI_LyrWinGetFB(fmplay_menu_attr->layer, &fb);

    GUI_LyrWinSel(fmplay_menu_attr->layer);
    GUI_SetFont(fmplay_menu_attr->font);
    GUI_UC_SetEncodeUTF8();

    draw_mem = GUI_MEMDEV_Create(0, 0, fb.size.width,  fb.size.height);
    GUI_MEMDEV_Select(draw_mem);

    fmplay_menu_attr->ui_state = FMUISTATE_ITEMS;

    if(MENU_PAINT_ALL == paint_mode)
    {
        //draw bg rect
        /*
        GUI_SetBkColor(0x10);
        x = FMMODE_TIPUI_STARTX;
        y = FMMODE_TIPUI_STARTY;
        GUI_ClearRect(x,y,x+FMMODE_TIPUI_WIDTH,y+FMMODE_TIPUI_HEIGHT);
        */
        x = FMMODE_MENUUI_STARTX;
        y = FMMODE_MENUUI_STARTY;
        pbmp = dsk_theme_hdl2buf(fmplay_menu_attr->bmp_menu_bg);
        if(NULL != pbmp)
        {
            GUI_BMP_Draw(pbmp, x, y);
        }

        //draw items
        //y = ITEMS_STARTY;
        for(i = 0; i < ITEM_MAX; i++)
        {
            //x = ITEMS_STARTX + (BMP_ITEMBG_WIDTH+BMP_SEPARATOR_WIDTH)*i;
            //draw item
            fmplay_item_paint(msg, i);
            /*
            //draw separator
            if(i<ITEM_MAX-1)
            {
            	x += BMP_ITEMBG_WIDTH;
            	GUI_BMP_Draw(fmplay_menu_attr->bmp_separator, x, y);
            }
            */
        }

        //draw subitem
        fmplay_submenu_paint(msg, MENU_PAINT_ALL);
        fmplay_disk_paint(msg, MENU_PAINT_ALL);

    }
    else if(MENU_PAINT_ITEMSW == paint_mode)
    {
        fmplay_submenu_clear(msg, fmplay_menu_attr->old_focus);
        fmplay_disk_clear(msg, fmplay_menu_attr->new_focus);
        fmplay_item_paint(msg, fmplay_menu_attr->old_focus);
        fmplay_item_paint(msg, fmplay_menu_attr->new_focus);
        fmplay_submenu_paint(msg, MENU_PAINT_ALL);
        fmplay_disk_paint(msg, MENU_PAINT_ALL);
    }
    else if(MENU_PAINT_VALUESW == paint_mode)
    {
        fmplay_submenu_paint(msg, MENU_PAINT_VALUESW);
        fmplay_disk_paint(msg, MENU_PAINT_ALL);
    }

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select( NULL );
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;

    return EPDK_OK;
}

static __s32 fmplay_changechannelno(__gui_msg_t *msg, __u8 ucChMod)
{
    //__u8 ucSendFlag = 0;
    fmplay_menu_attr_t *fmplay_menu_attr;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if(dsk_radio_rcv_get_search_result_vn() != fmplay_menu_attr->fmchanneltotal)
    {
        __msg("somewhere error, pls. check~");
    }

    if(fmplay_menu_attr->fmchanneltotal <= 1)
    {
        return EPDK_OK;
    }

    if(0 == ucChMod)//down channel number
    {
        if(fmplay_menu_attr->fmchannelnu > 1)
        {
            fmplay_menu_attr->fmchannelnu--;
        }
        else
        {
            fmplay_menu_attr->fmchannelnu = fmplay_menu_attr->fmchanneltotal;
        }
    }
    else//up channel number
    {
        if(fmplay_menu_attr->fmchannelnu < fmplay_menu_attr->fmchanneltotal)
        {
            fmplay_menu_attr->fmchannelnu++;
        }
        else
        {
            fmplay_menu_attr->fmchannelnu = 1;
        }
    }

    //if(ucSendFlag == 1)
    {
        dsk_radio_chan_t chan_info;
        reg_fm_para_t *para;

        para = (reg_fm_para_t *)dsk_reg_get_para_by_app(REG_APP_FM);

        dsk_radio_rcv_get_search_result_chaninfo(fmplay_menu_attr->fmchannelnu - 1, &chan_info);

        fmplay_menu_attr->fmchannelfreq = chan_info.freq;
        dsk_radio_rcv_set_freq_play(chan_info.freq);

        para->channel_id = fmplay_menu_attr->fmchannelnu;

        dsk_radio_rcv_get_cur_freq(&fmplay_menu_attr->fmchannelfreq);//just for mistake~

        //refresh ui:
        fmplay_mainui_paint(msg, MAINUI_POINTER | MAINUI_CHTIPS, MAINUI_PAINT);
        if(fmplay_menu_attr->ui_state == FMUISTATE_ITEMS)
        {
            fmplay_menu_paint(msg, MENU_PAINT_VALUESW); //value sw
        }
    }

    return EPDK_OK;
}

static __s32 fmplay_finetune(__gui_msg_t *msg, __u8 ucChMod)
{
    __u32 uFreq = 0;
    //__u8 ucSendFlag = 0;
    fmplay_menu_attr_t *fmplay_menu_attr;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    GUI_ResetTimer(msg->h_deswin, FMTIMER_ITEM_ID, ITEM_TIME, NULL);

    if(0 == ucChMod)
    {
        //ucSendFlag = dsk_radio_rcv_pre_freq_play();
        uFreq = fmplay_menu_attr->fmchannelfreq - LARK_SEARCH_STEP_US;
        if(uFreq < FM_FREQ_MIN)
        {
            uFreq = FM_FREQ_MAX;
        }
    }
    else
    {
        //ucSendFlag = dsk_radio_rcv_next_freq_play();
        uFreq = fmplay_menu_attr->fmchannelfreq + LARK_SEARCH_STEP_US;
        if(uFreq > FM_FREQ_MAX)
        {
            uFreq = FM_FREQ_MIN;
        }
    }

    //if(ucSendFlag == EPDK_OK)
    {
        fmplay_menu_attr->fmchannelfreq = uFreq;
        dsk_radio_rcv_set_freq_play(uFreq);
        //refresh ui(value sw \ channeltips and pointer)
        fmplay_mainui_paint(msg, MAINUI_POINTER | MAINUI_CHTIPS, MAINUI_PAINT);
        if(fmplay_menu_attr->ui_state == FMUISTATE_ITEMS)
        {
            fmplay_menu_paint(msg, MENU_PAINT_VALUESW); //value sw
        }
    }

    return EPDK_OK;
}

static __s32 fmplay_volume_set(__gui_msg_t *msg, __u8 ucChMod)
{
    fmplay_menu_attr_t *fmplay_menu_attr;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    GUI_ResetTimer(msg->h_deswin, FMTIMER_VOLUME_ID, VOLUME_TIME, NULL);

    if(0 == ucChMod)
    {
        if(fmplay_menu_attr->fm_volume_value > 0)
        {
            fmplay_menu_attr->fm_volume_value--;
            fmplay_volume_paint(msg, MENU_PAINT_VALUESW);
            dsk_volume_set(fmplay_menu_attr->fm_volume_value);
        }
    }
    else
    {
        if(fmplay_menu_attr->fm_volume_value < 30)
        {
            fmplay_menu_attr->fm_volume_value++;
            fmplay_volume_paint(msg, MENU_PAINT_VALUESW);
            dsk_volume_set(fmplay_menu_attr->fm_volume_value);
        }
    }

    return EPDK_OK;
}

static __s32 fmplay_items_switch(__gui_msg_t *msg, __u8 ucChMod)
{
    fmplay_menu_attr_t *fmplay_menu_attr;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    GUI_ResetTimer(msg->h_deswin, FMTIMER_ITEM_ID, ITEM_TIME, NULL);

    fmplay_menu_attr->old_focus = fmplay_menu_attr->new_focus;

    if(0 == ucChMod)
    {
        if(fmplay_menu_attr->new_focus <= 0)
        {
            fmplay_menu_attr->new_focus = ITEM_MAX - 1;
        }
        else
        {
            fmplay_menu_attr->new_focus--;
        }
    }
    else
    {
        if(fmplay_menu_attr->new_focus >= ITEM_MAX - 1)
        {
            fmplay_menu_attr->new_focus = 0;
        }
        else
        {
            fmplay_menu_attr->new_focus++;
        }
    }

    fmplay_menu_paint(msg, MENU_PAINT_ITEMSW);

    return EPDK_OK;
}


static void fmplay_menu_create(__gui_msg_t *msg)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    fmplay_frm_para_t *frmwin_para;

    frmwin_para = (fmplay_frm_para_t *)GUI_WinGetAttr(msg->h_deswin);

    fmplay_menu_attr = (fmplay_menu_attr_t *)esMEMS_Balloc(sizeof(fmplay_menu_attr_t));
    if( fmplay_menu_attr == NULL )
    {
        __msg("esMEMS_Balloc() fail!\n");
        return;
    }
    eLIBs_memset((void *)fmplay_menu_attr, 0, sizeof(fmplay_menu_attr_t));

    fmplay_menu_attr->layer = GUI_WinGetLyrWin(msg->h_deswin);
    //fmplay_menu_attr->root_type = frmwin_para->root_type;
    fmplay_menu_attr->font = frmwin_para->fmplay_frm_font;
    fmplay_menu_attr->new_focus = 0;
    fmplay_menu_attr->old_focus = 0;
    fmplay_menu_attr->focus_txt_color = APP_COLOR_YELLOW;
    fmplay_menu_attr->unfocus_txt_color = APP_COLOR_WHITE;
    fmplay_menu_attr->fm_volume_value = dsk_volume_get();

    fmplay_menu_attr->ui_state = FMUISTATE_MAIN;//ui state

    fmplay_menu_attr->search_mode = SRHMODE_IDLE;

    fmplay_menu_attr->fmsrhtips_focus = SRHTIPS_NO;

    fmplay_menu_init_res(fmplay_menu_attr);

    if(dsk_radio_is_open())
    {
        dsk_radio_chan_t chan_info;

        reg_fm_para_t *para;
        para = (reg_fm_para_t *)dsk_reg_get_para_by_app(REG_APP_FM);
        __msg("dsk_radio_rcv_get_search_result_vn():%x\n", dsk_radio_rcv_get_search_result_vn());
        if(dsk_radio_rcv_get_search_result_vn() == 0) //no channel info
        {
            fmplay_menu_attr->fmchannelnu = 1;//channel number
            fmplay_menu_attr->fmchannelfreq = 87000;
            fmplay_menu_attr->fmchanneltotal = 1;
            //add for reg data not all zero~
            eLIBs_memset((void *)para, 0, sizeof(reg_fm_para_t));
            para->channel_id = 1;
            para->FM_channel[0] = fmplay_menu_attr->fmchannelfreq;
            //dsk_radio_rcv_save_search_result();
            dsk_radio_rcv_get_search_result(para);
        }
        else
        {
            fmplay_menu_attr->fmchanneltotal = dsk_radio_rcv_get_search_result_vn();
            if(para->channel_id > fmplay_menu_attr->fmchanneltotal)
            {
                dsk_radio_rcv_get_search_result_chaninfo(0, &chan_info);
                fmplay_menu_attr->fmchannelnu = 1;
            }
            else
            {
                dsk_radio_rcv_get_search_result_chaninfo(para->channel_id - 1, &chan_info);
                fmplay_menu_attr->fmchannelnu = para->channel_id;
            }
            dsk_radio_rcv_set_freq_play(chan_info.freq);
            fmplay_menu_attr->fmchannelfreq = chan_info.freq;
        }
    }

    GUI_SetTimer(msg->h_deswin, FMTIMER_TIME_ID, TIMEREFRESH_TIME, NULL);

    GUI_WinSetAddData(msg->h_deswin, (__u32)fmplay_menu_attr);
    GUI_InvalidateRect(msg->h_deswin, 0, EPDK_TRUE);

    fm_check_disk(msg);
}

static __s32  fmplay_menu_key_proc(__gui_msg_t *msg)
{
    fmplay_menu_attr_t *fmplay_menu_attr;
    static __s32 last_key = -1;

    fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    __inf("--------%s,%d---,msg->dwAddData1:%x,msg->dwAddData2:%x-----\n", __FUNCTION__, __LINE__, msg->dwAddData1, msg->dwAddData2);

    if((fmplay_menu_attr->search_mode != SRHMODE_IDLE)//key not response in search mode except 'ESC'~
            && (msg->dwAddData1 != GUI_MSG_KEY_MENU))
    {
        return EPDK_OK;
    }

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        __msg("last_key:%d\n", last_key);
        switch(last_key)
        {
        //add by Kingvan
        case GUI_MSG_KEY_LONGLEFT:
        {
            if(fmplay_menu_attr->ui_state == FMUISTATE_MAIN)
            {
                GUI_KillTimer(msg->h_deswin, FMTIMER_ITEM_ID);

                fmplay_menu_attr->search_mode = SRHMODE_MANUAL;
                dsk_radio_set_manual_way(1);
                fm_cmd2parent(msg->h_deswin, COMMAND_MANUALSRH_START, fmplay_menu_attr->fmchannelfreq, fmplay_menu_attr->fmchannelnu);
            }
        }
        break;
        case GUI_MSG_KEY_LEFT:
        {
            //fmplay_finetune(msg, 1);
            //break;
            if(fmplay_menu_attr->ui_state == FMUISTATE_MAIN)
            {
                fmplay_changechannelno(msg, 0);
            }
            else if(fmplay_menu_attr->ui_state == FMUISTATE_VOLUME)
            {
                fmplay_volume_set(msg, 0);
            }
            else if(fmplay_menu_attr->ui_state == FMUISTATE_TIPS)
            {
                if(fmplay_menu_attr->fmsrhtips_focus == SRHTIPS_NO)
                {
                    fmplay_menu_attr->fmsrhtips_focus = SRHTIPS_YES;
                }
                else if(fmplay_menu_attr->fmsrhtips_focus == SRHTIPS_YES)
                {
                    fmplay_menu_attr->fmsrhtips_focus = SRHTIPS_NO;
                }
                fmplay_srhtips_paint(msg, MENU_PAINT_VALUESW);
            }
            else if(fmplay_menu_attr->ui_state == FMUISTATE_ITEMS)
            {
                fmplay_items_switch(msg, 0);
            }
        }
        break;
        //add by Kingvan
        case GUI_MSG_KEY_LONGRIGHT:
        {
            if(fmplay_menu_attr->ui_state == FMUISTATE_MAIN)
            {
                GUI_KillTimer(msg->h_deswin, FMTIMER_ITEM_ID);

                fmplay_menu_attr->search_mode = SRHMODE_MANUAL;
                dsk_radio_set_manual_way(0);
                fm_cmd2parent(msg->h_deswin, COMMAND_MANUALSRH_START, fmplay_menu_attr->fmchannelfreq, fmplay_menu_attr->fmchannelnu);
            }
        }
        break;
        case GUI_MSG_KEY_RIGHT:
            //case GUI_MSG_KEY_LONGRIGHT:
        {
            //fmplay_finetune(msg, 0);
            //break;
            if(fmplay_menu_attr->ui_state == FMUISTATE_MAIN)
            {
                fmplay_changechannelno(msg, 1);
            }
            else if(fmplay_menu_attr->ui_state == FMUISTATE_VOLUME)
            {
                fmplay_volume_set(msg, 1);
            }
            if(fmplay_menu_attr->ui_state == FMUISTATE_TIPS)
            {
                if(fmplay_menu_attr->fmsrhtips_focus == SRHTIPS_YES)
                {
                    fmplay_menu_attr->fmsrhtips_focus = SRHTIPS_NO;
                }
                else if(fmplay_menu_attr->fmsrhtips_focus == SRHTIPS_NO)
                {
                    fmplay_menu_attr->fmsrhtips_focus = SRHTIPS_YES;
                }
                fmplay_srhtips_paint(msg, MENU_PAINT_VALUESW);
            }
            else if(fmplay_menu_attr->ui_state == FMUISTATE_ITEMS)
            {
                fmplay_items_switch(msg, 1);
            }
        }
        break;
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LONGUP:
        {
            if(fmplay_menu_attr->ui_state == FMUISTATE_ITEMS)
            {
                GUI_ResetTimer(msg->h_deswin, FMTIMER_ITEM_ID, ITEM_TIME, NULL);

                if(fmplay_menu_attr->new_focus == ITEM_CHANNELNU)
                {
                    fmplay_changechannelno(msg, 0);
                }
                /*
                else if(fmplay_menu_attr->new_focus == ITEM_FINETUNE)
                {
                	fmplay_finetune(msg,0);
                }*/
            }
        }
        break;
        case GUI_MSG_KEY_LONGDOWN:
        case GUI_MSG_KEY_DOWN:
        {
            if(fmplay_menu_attr->ui_state == FMUISTATE_ITEMS)
            {
                GUI_ResetTimer(msg->h_deswin, FMTIMER_ITEM_ID, ITEM_TIME, NULL);

                if(fmplay_menu_attr->new_focus == ITEM_CHANNELNU)
                {
                    fmplay_changechannelno(msg, 1);
                }
                /*
                else if(fmplay_menu_attr->new_focus == ITEM_FINETUNE)
                {
                	fmplay_finetune(msg,1);
                }*/
            }
        }
        break;
        case GUI_MSG_KEY_ENTER:
        {
            if(fmplay_menu_attr->ui_state == FMUISTATE_ITEMS)
            {
                if(fmplay_menu_attr->new_focus == ITEM_AUTOSRH)
                {
                    fmplay_menu_clear(msg);
                    GUI_KillTimer(msg->h_deswin, FMTIMER_ITEM_ID);
                    fmplay_mainui_paint(msg, MAINUI_SYSTIME, MAINUI_PAINT);
                    fmplay_srhtips_paint(msg, MENU_PAINT_ALL);
                }
                else if(fmplay_menu_attr->new_focus == ITEM_MANSRH)
                {
                    //switch to channel manual search
                    fmplay_menu_clear(msg);
                    GUI_KillTimer(msg->h_deswin, FMTIMER_ITEM_ID);

                    fmplay_menu_attr->search_mode = SRHMODE_MANUAL;
                    dsk_radio_set_manual_way(0);
                    fm_cmd2parent(msg->h_deswin, COMMAND_MANUALSRH_START, fmplay_menu_attr->fmchannelfreq, fmplay_menu_attr->fmchannelnu);

                    fmplay_mainui_paint(msg, MAINUI_SYSTIME, MAINUI_PAINT);
                }
                /*
                else if(fmplay_menu_attr->new_focus == ITEM_CHANNELNU)
                {
                	GUI_ResetTimer(msg->h_deswin,FMTIMER_ITEM_ID, ITEM_TIME, NULL);
                	fmplay_changechannelno(msg,1);//reserved
                }
                else if(fmplay_menu_attr->new_focus == ITEM_FINETUNE)
                {
                	fmplay_finetune(msg,1);
                }*/
#ifdef ADD_ITEM_RECORD
                else if(fmplay_menu_attr->new_focus == ITEM_RECORD)
                {
                    //GUI_ResetTimer(msg->h_deswin,FMTIMER_ITEM_ID, ITEM_TIME, NULL);
                    //reserved~(for fm record function~)
                    if( fmplay_menu_attr->root_type != RAT_UNKNOWN )
                    {
                        fm_cmd2parent(msg->h_deswin, COMMAND_SWITCH_RECORD, fmplay_menu_attr->cur_root_type, 0);
                    }
                    else
                    {
                        ;//fm_cmd2parent(msg->h_deswin,COMMAND_SWITCH_MMENU,0,0);
                    }
                }
#endif
            }
            else if(fmplay_menu_attr->ui_state == FMUISTATE_TIPS)
            {
                if(fmplay_menu_attr->fmsrhtips_focus == SRHTIPS_NO)
                {
                    fmplay_srhtips_clear(msg);
                    fmplay_menu_paint(msg, MENU_PAINT_ALL);
                    GUI_SetTimer(msg->h_deswin, FMTIMER_ITEM_ID, ITEM_TIME, NULL);
                }
                else//if(fmplay_menu_attr->fmsrhtips_focus == SRHTIPS_YES)
                {
                    //switch to channel auto search
                    fmplay_srhtips_clear(msg);
                    fmplay_menu_attr->search_mode = SRHMODE_AUTO;
                    fm_cmd2parent(msg->h_deswin, COMMAND_AUTOSRH_START, 0, 0);

                    fmplay_searchprogress_paint(msg, FM_FREQ_MIN);
                }
            }
        }
        break;
        case GUI_MSG_KEY_LONGVADD:
        {
            if(fmplay_menu_attr->ui_state == FMUISTATE_MAIN)
            {
                fmplay_volume_paint(msg, MENU_PAINT_ALL);
                GUI_SetTimer(msg->h_deswin, FMTIMER_VOLUME_ID, VOLUME_TIME, NULL);
            }
            else if(fmplay_menu_attr->ui_state == FMUISTATE_VOLUME)
            {
                //fmplay_volume_clear(msg);
                GUI_KillTimer(msg->h_deswin, FMTIMER_VOLUME_ID);
                fmplay_mainui_paint(msg, MAINUI_TITLE, MAINUI_PAINT);
            }
            break;
        }
        case GUI_MSG_KEY_VADD:
        {
            if(fmplay_menu_attr->ui_state == FMUISTATE_ITEMS)
            {
                GUI_ResetTimer(msg->h_deswin, FMTIMER_ITEM_ID, ITEM_TIME, NULL);

                if(fmplay_menu_attr->new_focus == ITEM_CHANNELNU)
                {
                    fmplay_changechannelno(msg, 1);
                }
                else if(fmplay_menu_attr->new_focus == ITEM_RECORD)
                {
                    if( fmplay_menu_attr->root_type == (RAT_USB | (RAT_TF << 8)) )
                    {
                        if(fmplay_menu_attr->cur_root_type == RAT_TF)
                            fmplay_menu_attr->cur_root_type = RAT_USB;
                        else
                            fmplay_menu_attr->cur_root_type = RAT_TF;
                        fmplay_disk_paint(msg, 0);
                    }
                }
                /*
                else if(fmplay_menu_attr->new_focus == ITEM_FINETUNE)
                {
                	fmplay_finetune(msg,1);
                }*/
            }
            break;
        }
        case GUI_MSG_KEY_MENU:
        {
            if( fmplay_menu_attr->ui_state == FMUISTATE_TIPS )
            {
                fmplay_srhtips_clear(msg);
                fmplay_menu_paint(msg, MENU_PAINT_ALL);
                GUI_SetTimer(msg->h_deswin, FMTIMER_ITEM_ID, ITEM_TIME, NULL);
            }
            else if( fmplay_menu_attr->ui_state == FMUISTATE_SRHPRG )
            {
                break;
            }
            else if(fmplay_menu_attr->ui_state == FMUISTATE_VOLUME)
            {
                //fmplay_volume_clear(msg);
                GUI_KillTimer(msg->h_deswin, FMTIMER_VOLUME_ID);
                fmplay_mainui_paint(msg, MAINUI_TITLE, MAINUI_PAINT);
            }
            else if(fmplay_menu_attr->ui_state == FMUISTATE_ITEMS)
            {
                fmplay_menu_clear(msg);
                GUI_KillTimer(msg->h_deswin, FMTIMER_ITEM_ID);
                fmplay_mainui_paint(msg, MAINUI_SYSTIME, MAINUI_PAINT);
            }
            else
            {
                fm_cmd2parent(msg->h_deswin, COMMAND_SWITCH_MMENU, 0, 0);
            }
            break;
        }
        case GUI_MSG_KEY_LONGMENU:
            __here__;
            if(fmplay_menu_attr->ui_state == FMUISTATE_VOLUME)
            {
                //fmplay_volume_clear(msg);
                GUI_KillTimer(msg->h_deswin, FMTIMER_VOLUME_ID);
                fmplay_mainui_paint(msg, MAINUI_TITLE, MAINUI_PAINT);
            }
            if(fmplay_menu_attr->ui_state != FMUISTATE_ITEMS)
            {
                fmplay_menu_paint(msg, MENU_PAINT_ALL);
                GUI_SetTimer(msg->h_deswin, FMTIMER_ITEM_ID, ITEM_TIME, NULL);
            }
            else
            {
                fmplay_menu_clear(msg);
                GUI_KillTimer(msg->h_deswin, FMTIMER_ITEM_ID);
                fmplay_mainui_paint(msg, MAINUI_SYSTIME, MAINUI_PAINT);
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch(msg->dwAddData1)
        {
        case GUI_MSG_KEY_LONGLEFT:
        {
            if(fmplay_menu_attr->ui_state == FMUISTATE_VOLUME)
            {
                if(fmplay_menu_attr->fm_volume_value > 0)
                {
                    fmplay_menu_attr->fm_volume_value--;
                    fmplay_volume_paint(msg, MENU_PAINT_VALUESW);
                    dsk_volume_set(fmplay_menu_attr->fm_volume_value);
                }
                GUI_ResetTimer(msg->h_deswin, FMTIMER_VOLUME_ID, VOLUME_TIME, NULL);
            }
            break;
        }
        case GUI_MSG_KEY_LONGRIGHT:
        {
            if(fmplay_menu_attr->ui_state == FMUISTATE_VOLUME)
            {
                if(fmplay_menu_attr->fm_volume_value < 30)
                {
                    fmplay_menu_attr->fm_volume_value++;
                    fmplay_volume_paint(msg, MENU_PAINT_VALUESW);
                    dsk_volume_set(fmplay_menu_attr->fm_volume_value);
                }
                GUI_ResetTimer(msg->h_deswin, FMTIMER_VOLUME_ID, VOLUME_TIME, NULL);
            }
            break;
        }
        default:
            break;
        }
    }

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        last_key = -1;
    }
    else
    {
        last_key = msg->dwAddData1;
    }

    return EPDK_OK;
}


static __s32 _fmplay_frmwin_Proc(__gui_msg_t *msg)
{
    __inf("--------%s,%d---,msg->id:%x-----\n", __FUNCTION__, __LINE__, msg->id);

    switch(msg->id)
    {
    case DSK_MSG_FS_PART_PLUGOUT:
    {
        fm_check_disk(msg);
        if( GUI_IsTimerInstalled(msg->h_deswin, FMTIMER_ITEM_ID) )
        {
            fmplay_srhtips_clear(msg);
            fmplay_menu_paint(msg, MENU_PAINT_ALL);
            GUI_ResetTimer(msg->h_deswin, FMTIMER_ITEM_ID, ITEM_TIME, NULL);
        }
    }
    return EPDK_OK;
    case DSK_MSG_FS_PART_PLUGIN:
    {
        fm_check_disk(msg);
        if( GUI_IsTimerInstalled(msg->h_deswin, FMTIMER_ITEM_ID) )
        {
            fmplay_srhtips_clear(msg);
            fmplay_menu_paint(msg, MENU_PAINT_ALL);
            GUI_ResetTimer(msg->h_deswin, FMTIMER_ITEM_ID, ITEM_TIME, NULL);
        }
    }
    return EPDK_OK;
    case GUI_MSG_CREATE:
    {
        fmplay_menu_create(msg);
    }
    return EPDK_OK;
    case GUI_MSG_CLOSE:
    {
        GUI_FrmWinDelete(msg->h_deswin);
    }
    return EPDK_OK;
    case GUI_MSG_DESTROY:
    {
        fmplay_menu_attr_t *fmplay_menu_attr;
        fmplay_frm_para_t *frmwin_para;

        frmwin_para = (fmplay_frm_para_t *)GUI_WinGetAttr(msg->h_deswin);
        fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

        GUI_KillTimer(msg->h_deswin, FMTIMER_ITEM_ID);
        GUI_KillTimer(msg->h_deswin, FMTIMER_VOLUME_ID);
        GUI_KillTimer(msg->h_deswin, FMTIMER_TIME_ID);

        fmplay_menu_uninit_res(fmplay_menu_attr);

        //dsk_reg_flush();//保存注册表的数据

        esMEMS_Bfree(fmplay_menu_attr, sizeof(fmplay_menu_attr_t));
        esMEMS_Bfree(frmwin_para, sizeof(fmplay_frm_para_t));
    }
    return EPDK_OK;
    case GUI_MSG_PAINT:
        __here__;
        //fmplay_menu_paint(msg,MENU_PAINT_ALL);
        fmplay_mainui_paint(msg, MAINUI_ALL, MAINUI_PAINT);
        return EPDK_OK;
    case GUI_MSG_KEY:
        __here__;
        fmplay_menu_key_proc(msg);
        break;
    case GUI_MSG_TIMER:
    {
        fmplay_menu_attr_t *fmplay_menu_attr;
        fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

        __here__;
        __inf("msg->dwAddData1:%d\n", msg->dwAddData1);
        if(FMTIMER_VOLUME_ID == msg->dwAddData1)
        {
            if(fmplay_menu_attr->ui_state == FMUISTATE_VOLUME)
            {
                //fmplay_volume_clear(msg);
                fmplay_mainui_paint(msg, MAINUI_TITLE, MAINUI_PAINT);
            }
            GUI_KillTimer(msg->h_deswin, FMTIMER_VOLUME_ID);
        }
        else if(FMTIMER_ITEM_ID == msg->dwAddData1)
        {
            if(fmplay_menu_attr->ui_state == FMUISTATE_ITEMS)
            {
                fmplay_menu_clear(msg);
                fmplay_mainui_paint(msg, MAINUI_SYSTIME, MAINUI_PAINT);
            }
            GUI_KillTimer(msg->h_deswin, FMTIMER_ITEM_ID);
        }
        else if(FMTIMER_TIME_ID == msg->dwAddData1)
        {
            if((fmplay_menu_attr->ui_state == FMUISTATE_MAIN)
                    /*||(fmplay_menu_attr->ui_state == FMUISTATE_VOLUME)*/)//should be marked
            {
                fmplay_mainui_paint(msg, MAINUI_SYSTIME, MAINUI_REFRESH);
            }
            GUI_ResetTimer(msg->h_deswin, FMTIMER_TIME_ID, TIMEREFRESH_TIME, NULL);
        }
        return EPDK_OK;
    }
    case GUI_MSG_TOUCH:
        break;
    case GUI_MSG_COMMAND:
        break;
    case CMD_AUTOSRH_FINDCH:
    {
        //自动搜索状态下搜索到一个频点:
        dsk_radio_chan_t chan_info;
        //reg_fm_para_t* para;
        fmplay_menu_attr_t *fmplay_menu_attr;

        __here__;
        fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

        //para = (reg_fm_para_t*)dsk_reg_get_para_by_app(REG_APP_FM);

        if(fmplay_menu_attr->search_mode == SRHMODE_MANUAL)
        {
            __here__;
            dsk_radio_rcv_get_cur_freq(&fmplay_menu_attr->fmchannelfreq);

            fmplay_mainui_paint(msg, MAINUI_POINTER | MAINUI_CHTIPS, MAINUI_PAINT);
        }
        else
        {
            if(dsk_radio_rcv_get_search_result_vn() != 0)
            {
                fmplay_menu_attr->fmchanneltotal = dsk_radio_rcv_get_search_result_vn();
                fmplay_menu_attr->fmchannelnu = fmplay_menu_attr->fmchanneltotal;

                dsk_radio_rcv_get_search_result_chaninfo(fmplay_menu_attr->fmchannelnu - 1, &chan_info);
                fmplay_menu_attr->fmchannelfreq = chan_info.freq;
            }

            fmplay_mainui_paint(msg, MAINUI_POINTER | MAINUI_CHTIPS, MAINUI_PAINT);
            fmplay_searchprogress_paint(msg, fmplay_menu_attr->fmchannelfreq);
        }
        return EPDK_OK;
    }
    case CMD_AUTOSRH_FINDCHFAIL:
    {
        fmplay_menu_attr_t *fmplay_menu_attr;
        fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

        if(fmplay_menu_attr->search_mode == SRHMODE_MANUAL)
        {
            __here__;
            dsk_radio_rcv_get_cur_freq(&fmplay_menu_attr->fmchannelfreq);

            fmplay_mainui_paint(msg, MAINUI_POINTER | MAINUI_CHTIPS, MAINUI_PAINT);
        }
        else
        {
            __u32 search_freq = 0;
            __here__;
            dsk_radio_rcv_get_cur_freq(&search_freq);
            fmplay_searchprogress_paint(msg, search_freq);
        }
        return EPDK_OK;
    }
    case CMD_AUTOSRH_OVER:
    {
        //自动搜索完成:
        dsk_radio_chan_t chan_info;
        //reg_fm_para_t* para;
        fmplay_menu_attr_t *fmplay_menu_attr;

        fmplay_menu_attr = (fmplay_menu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
        __here__;
        //para = (reg_fm_para_t*)dsk_reg_get_para_by_app(REG_APP_FM);

        if(fmplay_menu_attr->search_mode == SRHMODE_MANUAL)
        {
            dsk_radio_rcv_get_search_result_chaninfo(fmplay_menu_attr->fmchannelnu - 1, &chan_info);
            fmplay_menu_attr->fmchannelfreq = chan_info.freq;
        }
        else
        {
            if(dsk_radio_rcv_get_search_result_vn() == 0) //no channel info
            {
                fmplay_menu_attr->fmchannelnu = 1;//channel number
                fmplay_menu_attr->fmchannelfreq = 87000;
                fmplay_menu_attr->fmchanneltotal = 1;
            }
            else
            {
                fmplay_menu_attr->fmchanneltotal = dsk_radio_rcv_get_search_result_vn();

                dsk_radio_rcv_get_search_result_chaninfo(0, &chan_info);
                fmplay_menu_attr->fmchannelnu = 1;
                fmplay_menu_attr->fmchannelfreq = chan_info.freq;
            }
        }

        fmplay_menu_attr->search_mode = SRHMODE_IDLE;
        fmplay_mainui_paint(msg, MAINUI_POINTER | MAINUI_CHTIPS | MAINUI_SYSTIME, MAINUI_PAINT);

        dsk_radio_rcv_save_search_result();

        return EPDK_OK;
    }
    case GUI_MSG_WIN_WAKEUP:
        GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
        return EPDK_OK;
    default:
        break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN fmplay_frmwin_create(H_WIN h_parent, fmplay_frm_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    fmplay_frm_para_t *frmwin_para;
    FB fb;

    __msg("dsk_volume_get()=%d\n", dsk_volume_get());

    GUI_LyrWinGetFB(para->layer, &fb);

    frmwin_para = (fmplay_frm_para_t *)esMEMS_Balloc(sizeof(fmplay_frm_para_t));
    if( frmwin_para == NULL )
    {
        __msg("esMEMS_Balloc() fail!\n");
        return NULL;
    }
    eLIBs_memset((void *)frmwin_para, 0, sizeof(fmplay_frm_para_t));
    frmwin_para->fmplay_frm_font = para->fmplay_frm_font;
    frmwin_para->layer = para->layer;

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =	"fm top win";
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner	= NULL;
    framewin_para.id         = FM_TOP_FRMID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_fmplay_frmwin_Proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr =  (void *)frmwin_para;
    framewin_para.hLayer = para->layer;

    return (GUI_FrmWinCreate(&framewin_para));
}

