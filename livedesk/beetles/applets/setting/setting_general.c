/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: setting_general.c
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "Setting_general.h"
#include "setting_para.h"


#if 0
#define __msg(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
						     eLIBs_printf(__VA_ARGS__)									        )
#else
#define __msg(...)    	
#endif


#define   UDISK_NAME			"f:\\"
#define     USER_DISK           "F:"


#define SET_BMP_BSYSSET_W 131
#define SET_BMP_BSYSSET_H 116

#define SET_BMP_SETICON00_W 28
#define SET_BMP_SETICON00_H 21

#define SET_BMP_BGSEL_W 244
#define SET_BMP_BGSEL_H 24

#define SET_BMP_ARR_W 18
#define SET_BMP_ARR_H 24

#define MAIN_LIST_LAYER           0
#define POP_LIST_LAYER        1
#define POP_INF_LIST_LAYER   2
//#define THIRD_LIST_LAYER         3
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
    OFFSET string_title_pos;

    char string_content[128];
    RECT  string_content_rect;

    H_THEME  h_bmp_focus;
    OFFSET   bmp_focus_pos;

    H_THEME  h_bmp_unfocus;
    OFFSET   bmp_unfocus_pos;

    H_THEME  h_bmp_left;
    OFFSET   bmp_left_pos;

    H_THEME  h_bmp_right;
    OFFSET   bmp_right_pos;

    __s32 content_num;
    __s32 content_nr;
} setting_item_res_t;

typedef struct tag_setting_focus_item_id
{
    __s32  id_string_detail_prompt;
    __s32  id_bmp_focus_item;
} setting_focus_item_id_t;

typedef struct tag_setting_general_attr
{
    char string_prompt[8];
    char string_detail_prompt[8];
    char string_detail_prompt_ex[8];
    H_THEME 	h_bmp_big_setting, h_bmp_prompt, h_bmp_bottom, h_bmp_unselect, h_bmp_jh_right, h_bmp_jh_focus, h_bmp_jh_point, h_bmp_jh_point_1;
	H_THEME		h_backlight_select_bmp, h_backlight_unselect_bmp;
	H_THEME     h_bmp_inf, h_bmp_format;
	H_THEME		h_proinf_bmp;

    setting_item_res_t res_language;

    setting_item_res_t  res_jh_display;
    setting_item_res_t  res_jh_language;
    setting_item_res_t  res_jh_senior;

    setting_item_res_t  res_jh_bglight;
    setting_item_res_t  res_jh_bgtime;

    setting_item_res_t  res_jh_lanchn;
    setting_item_res_t  res_jh_lanen;
#ifdef SUPPORT_KOREAN_LANGUAGE
	setting_item_res_t	res_jh_lankr;
#endif

    setting_item_res_t  res_jh_inf;
    setting_item_res_t  res_jh_cardformat;
    setting_item_res_t  res_jh_defaultset;


    setting_item_res_t res_jh_verson;

    setting_item_res_t res_jh_totalsize;
    setting_item_res_t res_jh_freesize;

    __s32	focus_txt_color;
    __s32	unfocus_txt_color;

    H_WIN	layer;
    GUI_FONT *font;

    __s32 new_focus;
    __s32 old_focus;

    __s32 pop_new_focus;
    __s32 pop_old_focus;


    __s32 pop_inf_new_focus;
    __s32 pop_inf_old_focus;



    __s32 item_nr;
    setting_general_para_t *general_para;
    set_general_para *general_uipara;
} setting_general_attr_t;
__u8  current_language_set;
__u32		total_size;
__u32       free_size;

extern void  main_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);

static reg_system_para_t *setting_reg_para;
typedef enum
{
    BACKLIGHTOFF_NEVER,
    BACKLIGHTOFF_5SECOND,
    BACKLIGHTOFF_10SECOND,
    BACKLIGHTOFF_20SECOND,
    BACKLIGHTOFF_30SECOND,
    BACKLIGHTOFF_60SECOND,
    BACKLIGHTOFF_300SECOND,
    BACKLIGHTOFF_MAX,
} backlightoff_t;

static __s32 content_backlight_level_id[] = //ITEM1 (背光亮度)
{
    STRING_SET_POWER_BGT_LEVEL1_CON,
    STRING_SET_POWER_BGT_LEVEL2_CON,
    STRING_SET_POWER_BGT_LEVEL3_CON,
    STRING_SET_POWER_BGT_LEVEL4_CON,
    STRING_SET_POWER_BGT_LEVEL5_CON
};
static __s32 content_backlight_off_id[] = //ITEM2 (关闭背光)
{
    STRING_SET_POWER_BGT_NEVER_CON,
    STRING_SET_POWER_BGT_LAST5S_CON,
    STRING_SET_POWER_BGT_LAST10S_CON,
    STRING_SET_POWER_BGT_LAST20S_CON,
    STRING_SET_POWER_BGT_LAST30S_CON,
    STRING_SET_POWER_BGT_LAST60S_CON,
    STRING_SET_POWER_BGT_LAST5M_CON
};

static __s32 content_lang_id[] = //ITEM1 ( 设置语言)
{
    STRING_SET_COMMON_LANGUAGE_CHINESE_CON,
    STRING_SET_COMMON_LANGUAGE_ENGLISH_CON,
#ifdef SUPPORT_KOREAN_LANGUAGE
	STRING_SET_COMMON_LANGUAGE_KEREAN_CON
#endif
};

static __s32 content_style_id[] = //ITEM2 (设置主题风格)
{
    STRING_SET_COMMON_TOPIC_STYLE1_CON,
    STRING_SET_COMMON_TOPIC_STYLE2_CON,
    STRING_SET_COMMON_TOPIC_STYLE3_CON
};

static __s32 content_output_id[] = //输出类型
{
    STRING_SET_OUTPUT_NTSC_CON,
    STRING_SET_OUTPUT_PAL_CON
};

static __s32 content_keytone_id[] = //按键音
{
    STRING_SET_KEYTONE_ON_CON,
    STRING_SET_KEYTONE_OFF_CON
};
static __s32 one_list_str_title[] =
{
    STRING_SET_COMMON_DISPLAY_TITLE,
    STRING_SET_COMMON_LANGUAGE_TITLE,
#ifdef  SET_ITEM_SENSOR   
    STRING_SET_COMMON_SENIOR_TITLE,
#else
	STRING_SET_COMMON_PRODUCTINFO_TITLE,
#endif
#ifdef SET_ITEM_PRO_INF
	STRING_SET_COMMON_CARDFOMART_TITLE,
#endif
#ifdef SET_ITEM_CARD_INF
    STRING_SET_COMMON_FACTORY_DEFAULT_TITLE,
#endif
};
static __s32 pop_list_strdisplay_title[] =
{

    STRING_SET_POWER_BGT_TITLE,
    STRING_SET_POWER_BGT_OFF_TITLE
};

static __s32 pop_list_strlanguage_title[] =
{
    STRING_SET_COMMON_LANGUAGE_CHINESE_CON,
    STRING_SET_COMMON_LANGUAGE_ENGLISH_CON,
#ifdef SUPPORT_KOREAN_LANGUAGE
	STRING_SET_COMMON_LANGUAGE_KEREAN_CON
#endif
    
};
static __s32 pop_list_strsenior_title[] =
{
    STRING_SET_COMMON_PRODUCTINFO_TITLE,
    STRING_SET_COMMON_CARDFOMART_TITLE,
    STRING_SET_COMMON_FACTORY_DEFAULT_TITLE,
};
static __s32 pop_inf_title[] =
{

    STRING_SET_TIPS_SVERS,
    STRING_SET_TIPS_CARDT,
    STRING_SET_TIPS_CARDF

};
u8 current_list_layer = MAIN_LIST_LAYER;
#define SET_PAGE_NUM    5
u8 current_page = 0;
u8 prev_page = 0;

static void setting_clear_screen(void)
{
	GUI_ClearRect(0, 0, 480, 232);
}

static void setting_restore_default(__gui_msg_t *msg)
{
    dsk_reg_set_app_restore_all();

    if(setting_reg_para)
    {
        dsk_langres_set_type(setting_reg_para->language);			//语言恢复出厂设置
        //gscene_bgd_set_default_bg_index(setting_reg_para->style);	//恢复默认背景图
        gscene_bgd_refresh();
        dsk_keytone_set_state((__set_keytone_t)(setting_reg_para->keytone));//desktop模块里面1 表示on,2 表示off
        dsk_keytone_on();
        dsk_volume_set(setting_reg_para->volume);	//音量恢复出厂设置
        dsk_display_set_lcd_bright((__lion_bright_t)(setting_reg_para->backlight));	//背光亮度
        g_set_close_scn_time(setting_reg_para->backlightoff);	//自动关屏时间
        dsk_set_auto_off_time(setting_reg_para->poweroff);		//自动关机时间

        if(BEETLES_RTC_EXIST)
        {

            __date_t factory_date;
            __time_t factory_time;
            factory_date = setting_reg_para->date;
            factory_time = setting_reg_para->time;
            esTIME_SetDate(&factory_date);
            esTIME_SetTime(&factory_time);
        }
    }
    //setting_general_paint(msg);
    setting_general_default_paint(msg);
    //setting_pop_general_paint(msg);
}
/*
	申请资源
*/
static void get_tf_size(void)
{
    total_size = eLIBs_GetVolTSpace(UDISK_NAME) >> 20;
    __msg("TF_TOTAL_SIZE=%dM\n", (int)total_size);

    free_size = eLIBs_GetVolFSpace(UDISK_NAME) >> 20;
    __msg("TF_FREE_SIZE=%dM\n", (int)free_size);

}
static void _setting_pop_general_res_init(setting_general_attr_t *general_attr)
{
    __u32	i;
    __bool	restore_flag;
    setting_item_res_t	*p_item_res;

    //general_attr->h_bmp_unselect=theme_open(ID_SETTING_SET_ITEM_BMP);
    __msg("tatol: %d, general_attr->new_focus: %d\n", general_attr->item_nr,general_attr->new_focus);
    if(general_attr->new_focus == 0)
    {
        general_attr->item_nr = 2;
    }
    else if(general_attr->new_focus == 1)			//language
    {
#ifdef SUPPORT_KOREAN_LANGUAGE
		general_attr->item_nr = 3;	
#else
        general_attr->item_nr = 2;
#endif
    }
    else if(general_attr->new_focus == 2)
    {
#ifdef    SET_ITEM_SENSOR
        general_attr->item_nr = 3;
#else
		general_attr->item_nr = 1;
#endif

    }
//#ifdef SET_ITEM_PRO_INF
//	else if(general_attr->new_focus == 3)
//	{
//		__msg("Contain one item for card format\n");
//		general_attr->item_nr = 1;
//	}
//#endif
//#ifdef SET_ITEM_CARD_INF
//	else if(general_attr->new_focus == 4)
//	{
//		__msg("Contain one item for factory default\n");
//		general_attr->item_nr = 1;
//	}
//#endif
	
    for(i = 0; i < general_attr->item_nr; i++)
    {
        if(general_attr->new_focus == 0)
        {
            switch (i)
            {
            case 0:
                p_item_res = &general_attr->res_jh_bglight;
                break;
            case 1:
                p_item_res = &general_attr->res_jh_bgtime;
                break;
            default:
                break;
            }
        }
        else if(general_attr->new_focus == 1)
        {
            switch (i)
            {
            case 0:
                p_item_res = &general_attr->res_jh_lanchn;
                break;
            case 1:
                p_item_res = &general_attr->res_jh_lanen;
                break;
#ifdef SUPPORT_KOREAN_LANGUAGE
			case 2:
				p_item_res = &general_attr->res_jh_lankr;
				break;
			
#endif
            default:
                break;
            }


        }
        else if(general_attr->new_focus == 2)
        {
            switch (i)
            {
#ifdef    SET_ITEM_SENSOR            
            case 0:
                p_item_res = &general_attr->res_jh_inf;
                break;
            case 1:
                p_item_res = &general_attr->res_jh_cardformat;
                break;
            case	2:
                p_item_res = &general_attr->res_jh_defaultset;
                break;
#else
	
			case 0:
				p_item_res = &general_attr->res_jh_inf;
				break;
#endif
            default:
                break;
            }
        }
//#ifdef SET_ITEM_PRO_INF
//		else if(general_attr->new_focus == 3)
//		{
//			switch (i)
//            {
//            case 0:
//                p_item_res = &general_attr->res_jh_cardformat;
//                break;
//            default:
//                break;
//            }
//		}
//#endif
//#ifdef SET_ITEM_CARD_INF
//		else if(general_attr->new_focus == 4)
//		{
//			switch (i)
//            {
//            case 0:
//                p_item_res = &general_attr->res_jh_defaultset;
//                break;
//            default:
//                break;
//            }
//		}		
//#endif

        p_item_res->string_title_pos = general_attr->general_uipara->item[i].item_string_title_pos;
#ifdef SET_ITEM_RIGHT_BMP
		p_item_res->bmp_right_pos = general_attr->general_uipara->item[i].item_bmp_right_pos;
#endif
        p_item_res->bmp_focus_pos = general_attr->general_uipara->item[i].item_bmp_focus_pos;
    }

}
static void _setting_general_res_init(setting_general_attr_t *general_attr)
{
    __u32 	i;
    __bool  restore_flag;
    setting_item_res_t  *p_item_res;
	__msg("_setting_general_res_init\n");
    general_attr->item_nr = SET_PAGE_NUM;
    for(i = 0; i < SET_PAGE_NUM; i++)
    {
        switch(i)
        {
        case 0:
            p_item_res = &general_attr->res_jh_display; 		//显示设置
            break;
        case 1:
            p_item_res = &general_attr->res_jh_language; 		//语音支持
            break;
#ifdef SET_ITEM_SENSOR
		case 2:
            p_item_res = &general_attr->res_jh_senior; 		//高级
            break;
#else			
		case 2:
		{
			p_item_res = &general_attr->res_jh_defaultset; //高级
			break;
		}			
#endif			
#ifdef SET_ITEM_PRO_INF
		case 3:
		{
			p_item_res = &general_attr->res_jh_inf; 		//产品信息
			break;
		}
#endif		
#ifdef SET_ITEM_CARD_INF
		case 4:
		{
			p_item_res = &general_attr->res_jh_cardformat; 			//卡信息
			break;
		}
#endif		
#ifdef SET_ITEM_FACT_DEF
		case 5:
		{
			p_item_res = &general_attr->res_jh_defaultset; 			//出厂设置
			break;
		}
#endif		
        default:
            break;
        }

        p_item_res->string_title_pos = general_attr->general_uipara->item[i].item_string_title_pos;
#ifdef SET_ITEM_RIGHT_BMP
		p_item_res->bmp_right_pos = general_attr->general_uipara->item[i].item_bmp_right_pos;
#endif		
        p_item_res->bmp_focus_pos = general_attr->general_uipara->item[i].item_bmp_focus_pos;
    }


}
static void _setting_general_popinf_init(setting_general_attr_t *general_attr)
{
    __u32	i;
    setting_item_res_t	*p_item_res;
    for(i = 0; i < 3; i++)
    {
        switch(i)
        {
        case 0:
            p_item_res = &general_attr->res_jh_verson; //
            get_menu_text(STRING_SET_VER_NUM, p_item_res->string_content, 128); ///
            break;
        case 1:
            p_item_res = &general_attr->res_jh_totalsize; //
            eLIBs_memset(p_item_res->string_content, 0, 16);
            eLIBs_uint2str_dec(total_size, p_item_res->string_content);
            eLIBs_strcat(p_item_res->string_content, "MB");
            break;
        case 2:
            p_item_res = &general_attr->res_jh_freesize; //
            eLIBs_memset(p_item_res->string_content, 0, 16);
            eLIBs_uint2str_dec(free_size, p_item_res->string_content);
            eLIBs_strcat(p_item_res->string_content, "MB");
            break;
        default:
            break;
        }

        p_item_res->string_title_pos = general_attr->general_uipara->item[i].item_string_title_pos;
#ifdef SET_ITEM_RIGHT_BMP
        p_item_res->bmp_right_pos = general_attr->general_uipara->item[i].item_bmp_right_pos;
#endif
        p_item_res->bmp_focus_pos = general_attr->general_uipara->item[i].item_bmp_focus_pos;
    }
}
static void _setting_general_pop_res_uninit(setting_general_attr_t *general_attr)
{
#if 0
    __u32  i;
    setting_item_res_t  *p_item_res;

    // theme_close(general_attr->h_bmp_unselect);
    for(i = 0; i < general_attr->item_nr; i++)
    {
        if(general_attr->new_focus == 0)
        {
            switch (i)
            {
            case 0:
                p_item_res = &general_attr->res_jh_bglight;
                break;
            case 1:
                p_item_res = &general_attr->res_jh_bgtime;
                break;
            default:
                break;
            }
        }
        else if(general_attr->new_focus == 1)
        {
            switch (i)
            {
            case 0:
                p_item_res = &general_attr->res_jh_lanchn;
                break;
            case 1:
                p_item_res = &general_attr->res_jh_lanen;
                break;
            default:
                break;
            }


        }
        else if(general_attr->new_focus == 2)
        {
            switch (i)
            {
            case 0:
                p_item_res = &general_attr->res_jh_inf;
                break;
            case 1:
                p_item_res = &general_attr->res_jh_cardformat;
                break;
            case	2:
                p_item_res = &general_attr->res_jh_defaultset;
                break;
            default:
                break;
            }


        }
        //  theme_close(p_item_res->h_bmp_focus);
        //theme_close(p_item_res->h_bmp_right);
    }
#endif
}

static void _setting_general_res_uninit(setting_general_attr_t *general_attr)
{
#if 0
    __u32 	i;
    setting_item_res_t  *p_item_res;

    //theme_close(general_attr->h_bmp_big_setting);
    //theme_close(general_attr->h_bmp_prompt);
    //theme_close(general_attr->h_bmp_bottom);
    //theme_close(general_attr->h_bmp_unselect);
    for (i = 0; i < SET_PAGE_NUM; i++)
    {
        switch (i)
        {
        case 0:
            p_item_res = &general_attr->res_jh_display; //显示设置
            break;
        case 1:
            p_item_res = &general_attr->res_jh_language; //语音支持
            break;
        case 2:
            p_item_res = &general_attr->res_jh_senior; //高级
            break;
#ifdef SET_ITEM_PRO_INF
		case 3:
		{
			p_item_res = &general_attr->res_jh_inf; 		//产品信息
			break;
		}
#endif		
#ifdef SET_ITEM_CARD_INF
		case 4:
		{
			p_item_res = &general_attr->res_jh_cardformat; 			//卡信息
			break;
		}
#endif		
#ifdef SET_ITEM_FACT_DEF
		case 5:
		{
			p_item_res = &general_attr->res_jh_defaultset; 			//出厂设置
			break;
		}
#endif				
        default:
            break;
        }
        //theme_close(p_item_res->h_bmp_focus);
        //theme_close(p_item_res->h_bmp_left);
        //theme_close(p_item_res->h_bmp_right);
    }
#endif
}
static void _setting_general_focus_pop_res_reset(setting_general_attr_t *general_attr)
{
#if 0
    setting_item_res_t	*p_item_res;
    if(general_attr->new_focus == 0)
    {
        switch (general_attr->pop_new_focus)
        {
        case 0:
            p_item_res = &general_attr->res_jh_bglight;
            break;
        case 1:
            p_item_res = &general_attr->res_jh_bgtime;
            break;
        default:
            break;
        }
    }
    else if(general_attr->new_focus == 1)
    {
        switch (general_attr->pop_new_focus)
        {
        case 0:
            p_item_res = &general_attr->res_jh_lanchn;
            break;
        case 1:
            p_item_res = &general_attr->res_jh_lanen;
            break;
        default:
            break;
        }


    }
    else if(general_attr->new_focus == 2)
    {
        switch (general_attr->pop_new_focus)
        {
        case 0:
            p_item_res = &general_attr->res_jh_inf;
            break;
        case 1:
            p_item_res = &general_attr->res_jh_cardformat;
            break;
        case	2:
            p_item_res = &general_attr->res_jh_defaultset;
            break;
        default:
            break;
        }


    }
#if 0
    if(p_item_res->h_bmp_focus)
    {
        theme_close(p_item_res->h_bmp_focus);
        p_item_res->h_bmp_focus = theme_open(ID_SETTING_SET_ITEM_F_BMP);
    }
    else
    {

        p_item_res->h_bmp_focus = theme_open(ID_SETTING_SET_ITEM_F_BMP);
    }
#endif
#endif
}
static void _setting_general_focus_res_reset(setting_general_attr_t *general_attr)
{
#if 0
    setting_item_res_t  *p_item_res;

    switch (general_attr->new_focus)
    {
    case 0:
        p_item_res = &general_attr->res_jh_display; //显示设置
        break;
    case 1:
        p_item_res = &general_attr->res_jh_language; //语音支持
        break;
    case 2:
        p_item_res = &general_attr->res_jh_senior; //高级
        break;
    default:
        break;
    }
#endif
    //	get_menu_text(setting_focus_item_res_id[general_attr->new_focus].id_string_detail_prompt, general_attr->string_detail_prompt, 128);
    //	get_menu_text(setting_general_res_id[general_attr->new_focus].id_string_detail_prompt_ex, general_attr->string_detail_prompt_ex, 128);
#if 0
    if(p_item_res->h_bmp_focus)
    {
        theme_close(p_item_res->h_bmp_focus);
        p_item_res->h_bmp_focus = theme_open(ID_SETTING_SET_ITEM_F_BMP);
    }
    else
    {

        p_item_res->h_bmp_focus = theme_open(ID_SETTING_SET_ITEM_F_BMP);
    }
#endif
}
static void _setting_general_pop_content_res_reset(setting_general_attr_t *general_attr)
{
    __u32 	 *content_id;
    setting_item_res_t  *p_item_res;
    if(general_attr->new_focus == 0)
    {
        switch (general_attr->pop_new_focus)
        {
        case 0:
            p_item_res = &general_attr->res_jh_bglight;
            content_id = content_backlight_level_id;
            get_menu_text(content_id[p_item_res->content_num], p_item_res->string_content, 128); ///
            p_item_res->content_nr = sizeof(content_backlight_level_id) / sizeof(__s32);
            break;
        case 1:
            p_item_res = &general_attr->res_jh_bgtime;
            content_id = content_backlight_off_id;
            get_menu_text(content_id[p_item_res->content_num], p_item_res->string_content, 128); ///
            p_item_res->content_nr = sizeof(content_backlight_off_id) / sizeof(__s32);
            break;
        default:
            break;
        }
    }
    else if(general_attr->new_focus == 1)
    {
        switch (general_attr->pop_new_focus)
        {
        case 0:
            p_item_res = &general_attr->res_jh_lanchn;
            break;
        case 1:
            p_item_res = &general_attr->res_jh_lanen;
            break;
#ifdef SUPPORT_KOREAN_LANGUAGE
		case 2:
			p_item_res = &general_attr->res_jh_lankr;
			break;

#endif
        default:
            break;
        }
    }
    else if(general_attr->new_focus == 2)
    {
        switch (general_attr->pop_new_focus)
        {
#ifdef SET_ITEM_SENSOR        
        case 0:
            p_item_res = &general_attr->res_jh_inf;
            break;
        case 1:
            p_item_res = &general_attr->res_jh_cardformat;
            break;
        case 2:
            p_item_res = &general_attr->res_jh_defaultset;
            break;
#else
		case 0:
			p_item_res = &general_attr->res_jh_inf;
			break;
#endif			
        default:
            break;
        }
    }
//#ifdef SET_ITEM_PRO_INF
//	else if(general_attr->new_focus == 3)
//	{
//		switch (general_attr->pop_new_focus)
//        {
//		case 0:
//			p_item_res = &general_attr->res_jh_cardformat;
//			break;
//        default:
//            break;
//        }
//	}
//#endif
//#ifdef SET_ITEM_CARD_INF
//	else if(general_attr->new_focus == 4)
//	{
//		switch (general_attr->pop_new_focus)
//        {
//		case 0:
//			p_item_res = &general_attr->res_jh_defaultset;
//			break;
//        default:
//            break;
//        }
//	}
//#endif

}

static void _setting_general_content_res_reset(setting_general_attr_t *general_attr)
{

    __u32 	 *content_id;
    setting_item_res_t  *p_item_res;

    switch (general_attr->new_focus)
    {
    case 0:
        p_item_res = &general_attr->res_jh_display; //显示设置
        break;
    case 1:
        p_item_res = &general_attr->res_jh_language; //语音支持
        content_id = content_lang_id;
        get_menu_text(content_id[p_item_res->content_num], p_item_res->string_content, 128); ///
        break;
#ifdef SET_ITEM_SENSOR		
    case 2:
        p_item_res = &general_attr->res_jh_senior; //高级
        break;
#else			
	case 2:
	{
		p_item_res = &general_attr->res_jh_defaultset; //高级
		break;
	}		
#endif	
#ifdef SET_ITEM_PRO_INF
	case 3:
	{
		p_item_res = &general_attr->res_jh_inf; 		//产品信息
		break;
	}
#endif		
#ifdef SET_ITEM_CARD_INF
	case 4:
	{
		p_item_res = &general_attr->res_jh_cardformat;			//卡信息
		break;
	}
#endif		
#ifdef SET_ITEM_FACT_DEF
	case 5:
	{
		p_item_res = &general_attr->res_jh_defaultset;			//出厂设置
		break;
	}
#endif	

    default:
        break;
    }

}
static __s32 setting_generl_popinf_content_paint(__gui_msg_t *msg)
{
    setting_general_attr_t *general_attr;
    setting_item_res_t	*p_item_res;
    RECT rect;
    GUI_RECT gui_rect;

    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (general_attr == NULL)
        return EPDK_FAIL;

    if (GUI_LyrWinGetSta(general_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    GUI_LyrWinSel(general_attr->layer);
    GUI_SetFont(general_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);

    GUI_LyrWinCacheOn();
    GUI_OpenAlphaBlend();
    GUI_LyrWinGetScnWindow(general_attr->layer, &rect);
    switch (general_attr->pop_inf_new_focus)
    {
    case 0:
        p_item_res = &general_attr->res_jh_verson;
        break;
    case 1:
        p_item_res = &general_attr->res_jh_totalsize;
        break;
    case 2:
        p_item_res = &general_attr->res_jh_freesize;
        break;
    default:
        break;
    }
    GUI_SetBkColor(0x00);
    GUI_SetColor(general_attr->focus_txt_color);
#ifdef SET_ITEM_SENSOR	
    GUI_BMP_Draw(theme_hdl2buf(/*p_item_res->h_bmp_focus*/general_attr->h_bmp_jh_focus), p_item_res->bmp_focus_pos.x, p_item_res->bmp_focus_pos.y);
#endif
#ifdef SET_ITEM_RIGHT_BMP
	GUI_BMP_Draw(theme_hdl2buf(/*p_item_res->h_bmp_right*/general_attr->h_bmp_jh_right), p_item_res->bmp_right_pos.x, p_item_res->bmp_right_pos.y);
#endif
    GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x - 40, p_item_res->string_title_pos.y);

    GUI_DispStringAt(p_item_res->string_content, p_item_res->string_title_pos.x + 70, p_item_res->string_title_pos.y);


    GUI_CloseAlphaBlend();
    GUI_LyrWinCacheOff();
    return EPDK_OK;

}
static __s32 setting_general_pop_content_paint(__gui_msg_t *msg)
{
    setting_general_attr_t *general_attr;
    setting_item_res_t  *p_item_res;
    RECT rect;
    GUI_RECT gui_rect;

	__msg("setting_general_pop_content_paint\n");
    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (general_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(general_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    { 	
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(general_attr->layer);
    GUI_SetFont(general_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);

    GUI_LyrWinCacheOn();
    GUI_OpenAlphaBlend();
    GUI_LyrWinGetScnWindow(general_attr->layer, &rect);
    _setting_general_pop_content_res_reset(general_attr);
    if(general_attr->new_focus == 0)
    {
        switch (general_attr->pop_new_focus)
        {
        case 0:
            p_item_res = &general_attr->res_jh_bglight;
            break;
        case 1:
            p_item_res = &general_attr->res_jh_bgtime;
            break;
        default:
            break;
        }
    }
    else if(general_attr->new_focus == 1)
    {
        switch (general_attr->pop_new_focus)
        {
        case 0:
            p_item_res = &general_attr->res_jh_lanchn;
            break;
        case 1:
            p_item_res = &general_attr->res_jh_lanen;
            break;
#ifdef SUPPORT_KOREAN_LANGUAGE			
		case 2:
			p_item_res = &general_attr->res_jh_lankr;
			break;
#endif			
			
        default:
            break;
        }


    }
    else if(general_attr->new_focus == 2)
    {
        switch (general_attr->pop_new_focus)
        {
#ifdef SET_ITEM_SENSOR        	
        case 0:
            p_item_res = &general_attr->res_jh_inf;
            break;
        case 1:
            p_item_res = &general_attr->res_jh_cardformat;
            break;
        case 2:
            p_item_res = &general_attr->res_jh_defaultset;
            break;
#else
		case 0:
		{
			p_item_res = &general_attr->res_jh_inf;
			break;
        }
#endif
        default:
            break;
        }
    }
#ifdef SET_ITEM_PRO_INF
	else if(general_attr->new_focus == 3)
	{
        switch (general_attr->pop_new_focus)
        {
		case 0:
		{
			p_item_res = &general_attr->res_jh_cardformat;
			break;
        }
        default:
            break;
        }
    }	
#endif
#ifdef SET_ITEM_CARD_INF
	else if(general_attr->new_focus == 4)
	{
        switch (general_attr->pop_new_focus)
        {
		case 0:
		{
			p_item_res = &general_attr->res_jh_defaultset;
			break;
        }
        default:
            break;
        }
    }	

#endif

    GUI_SetBkColor(0x00);
    GUI_SetColor(general_attr->focus_txt_color);
	if(general_attr->new_focus == 0)
    {
    	__msg("draw: general_attr->h_backlight_select_bmp:%d\n", general_attr->pop_new_focus);
		GUI_BMP_Draw(theme_hdl2buf(general_attr->h_backlight_select_bmp), 10, 2 + general_attr->pop_new_focus*60);
	}
#ifndef	SET_ITEM_SENSOR
	else if(2 == general_attr->new_focus)
	{
		GUI_BMP_Draw(theme_hdl2buf(general_attr->h_proinf_bmp), 10, 2);
	}
#endif
	else
	{
		__msg("draw: general_attr->h_bmp_jh_focus\n");
		GUI_BMP_Draw(theme_hdl2buf(general_attr->h_bmp_jh_focus), p_item_res->bmp_focus_pos.x, p_item_res->bmp_focus_pos.y);
	}
	
#ifdef SET_ITEM_RIGHT_BMP
	GUI_BMP_Draw(theme_hdl2buf(general_attr->h_bmp_jh_right), p_item_res->bmp_right_pos.x, p_item_res->bmp_right_pos.y);
#endif

    if(general_attr->new_focus == 0)
    { 	
    	__msg("p_item_res->string_title: %s, %d\n", p_item_res->string_title, p_item_res->string_title_pos.x);
        GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x, p_item_res->string_title_pos.y + 10);
    }
    else if(general_attr->new_focus == 1)
    {
    
#ifdef SET_ITEM_RIGHT_BMP
		if(current_language_set == general_attr->pop_new_focus)
		{
			GUI_BMP_Draw(theme_hdl2buf(general_attr->h_bmp_jh_point), p_item_res->string_title_pos.x, p_item_res->bmp_right_pos.y);

		}
#endif
    	__msg("p_item_res->string_title: %s\n", p_item_res->string_title);
        GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x, p_item_res->string_title_pos.y);
		
	}
#ifndef SET_ITEM_SENSOR		
	else if(general_attr->new_focus == 2)
	{
		get_tf_size();
		_setting_general_popinf_init(general_attr);
		seting_pop_information_paint(msg);
	}
#endif
//#ifdef SET_ITEM_PRO_INF
//	else if(general_attr->new_focus == 3)
//	{
//		ES_DIR			*p_file;
//        p_file = eLIBs_opendir(USER_DISK);
//        if(!p_file)//没有卡
//        {
//			__msg("ID_SWITCH_TO_TIPS: TIPS_NOCARD_NOTE\n");
//            main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_NOCARD_NOTE, 256);
//        }
//        else
//        {
//        	__msg("ID_SWITCH_TO_TIPS: TIPS_FORMAT_CARD\n");
//            main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_FORMAT_CARD, 192);
//        }
//	}	
//#endif
//#ifdef SET_ITEM_CARD_INF
//	else if(general_attr->new_focus == 4)
//	{
//		__msg("ID_SWITCH_TO_TIPS: TIPS_FACTORY_DEFAULT\n");
//		
//		main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_FACTORY_DEFAULT, 64);
//	}	
//#endif

    else
    {
    	__msg("p_item_res->string_title: %s\n", p_item_res->string_title);
        GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x, p_item_res->string_title_pos.y);
    }

    GUI_CloseAlphaBlend();
    GUI_LyrWinCacheOff();
    return EPDK_OK;
}

static __s32 setting_general_content_paint(__gui_msg_t *msg)
{
    setting_general_attr_t *general_attr;
    setting_item_res_t  *p_item_res;
    RECT rect;
    GUI_RECT gui_rect;
	
	__msg("setting_general_content_paint\n");
	
    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (general_attr == NULL)
        return EPDK_FAIL;

    if (GUI_LyrWinGetSta(general_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    GUI_LyrWinSel(general_attr->layer);
    GUI_SetFont(general_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);

    GUI_LyrWinCacheOn();
    GUI_OpenAlphaBlend();
    GUI_LyrWinGetScnWindow(general_attr->layer, &rect);

    _setting_general_content_res_reset(general_attr);

    switch (general_attr->new_focus)
    {
    case 0:
        p_item_res = &general_attr->res_jh_display; //显示设置
        break;
    case 1:
        p_item_res = &general_attr->res_jh_language; //语音支持
        break;
#ifdef SET_ITEM_SENSOR		
		case 2:
		{
			p_item_res = &general_attr->res_jh_senior; //高级
			break;
		}
#else			
		case 2:
		{
			p_item_res = &general_attr->res_jh_defaultset; //高级
			break;
		}		
#endif	
#ifdef SET_ITEM_PRO_INF
		case 3:
		{
			p_item_res = &general_attr->res_jh_inf; 		//产品信息
			break;
		}
#endif		
#ifdef SET_ITEM_CARD_INF
		case 4:
		{
			p_item_res = &general_attr->res_jh_cardformat;			//卡信息
			break;
		}
#endif		
#ifdef SET_ITEM_FACT_DEF
		case 5:
		{
			p_item_res = &general_attr->res_jh_defaultset;			//出厂设置
			break;
		}
#endif	
    default:
        break;
    }


    GUI_SetBkColor(0x00);
    GUI_SetColor(general_attr->focus_txt_color);
    //菜单里面的选择条
    GUI_BMP_Draw(theme_hdl2buf(general_attr->h_bmp_jh_focus), p_item_res->bmp_focus_pos.x, p_item_res->bmp_focus_pos.y);
#ifdef SET_ITEM_RIGHT_BMP
	GUI_BMP_Draw(theme_hdl2buf(general_attr->h_bmp_jh_right), p_item_res->bmp_right_pos.x, p_item_res->bmp_right_pos.y);
#endif

//    if(general_attr->new_focus == 1)
//    {
//        GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x, p_item_res->string_title_pos.y - 12);
//    }
//    else
//    {
    GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x, p_item_res->string_title_pos.y);
//    }
#if 0
    GUI_DispStringInRect(p_item_res->string_content, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
#endif
//    if(general_attr->new_focus == 1)
//    {
//    	__msg("p_item_res->string_content:%s\n", p_item_res->string_content);
//        GUI_DispStringAt(p_item_res->string_content, p_item_res->string_title_pos.x, p_item_res->string_title_pos.y + 8);
//    }

    GUI_CloseAlphaBlend();
    GUI_LyrWinCacheOff();
    return EPDK_OK;
}
static __s32 setting_general_popinf_item_paint(__gui_msg_t *msg)
{

    setting_general_attr_t *general_attr;
    setting_item_res_t  *p_item_res;
    __s32 i;
    RECT rect;
    GUI_RECT gui_rect;
	__msg("setting_general_popinf_item_paint\n");
    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (general_attr == NULL)
        return EPDK_FAIL;

    if (GUI_LyrWinGetSta(general_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    GUI_LyrWinSel(general_attr->layer);
    GUI_SetFont(general_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_LyrWinCacheOn();
    GUI_OpenAlphaBlend();
    GUI_LyrWinGetScnWindow(general_attr->layer, &rect);
    for(i = 0; i < 3; i++)
    {
        switch(i)
        {

        case 0:
            p_item_res = &general_attr->res_jh_verson;
            break;
        case 1:
            p_item_res = &general_attr->res_jh_totalsize;
            break;
        case 2:
            p_item_res = &general_attr->res_jh_freesize;
            break;
        default:
            break;


        }
        if(i == general_attr->pop_inf_new_focus)
        {
            setting_generl_popinf_content_paint(msg);
        }
        else if(i == general_attr->pop_inf_old_focus)
        {
            gui_rect.y0 = SET_ITEM_START_Y + SET_ITEM_H * (i % SET_PAGE_NUM);
#ifdef SET_ITEM_SENSOR			
            GUI_BMP_Draw(theme_hdl2buf(general_attr->h_bmp_unselect), 10, gui_rect.y0);
#endif
            GUI_SetColor(general_attr->unfocus_txt_color);
            GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x, p_item_res->string_title_pos.y);

            GUI_DispStringAt(p_item_res->string_content, p_item_res->string_title_pos.x + 50, p_item_res->string_title_pos.y);
        }
    }
    GUI_CloseAlphaBlend();
    GUI_LyrWinCacheOff();
    return EPDK_OK;

}

static __s32 setting_general_pop_item_paint(__gui_msg_t *msg)
{

    setting_general_attr_t *general_attr;
    setting_item_res_t  *p_item_res;
    __s32 i;
    RECT rect;
    GUI_RECT gui_rect;
	__msg("setting_general_pop_item_paint\n");
    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (general_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(general_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(general_attr->layer);
    GUI_SetFont(general_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_LyrWinCacheOn();
    GUI_OpenAlphaBlend();
    GUI_LyrWinGetScnWindow(general_attr->layer, &rect);
	
    for(i = 0; i < general_attr->item_nr; i++)
    {
        if(general_attr->new_focus == 0)
        {
            switch (i)
            {
            case 0:
                p_item_res = &general_attr->res_jh_bglight;
                break;
            case 1:
                p_item_res = &general_attr->res_jh_bgtime;
                break;
            default:
                break;
            }
        }
        else if(general_attr->new_focus == 1)
        {
            switch (i)
            {
            case 0:
                p_item_res = &general_attr->res_jh_lanchn;
                break;
            case 1:
                p_item_res = &general_attr->res_jh_lanen;
                break;
#ifdef SUPPORT_KOREAN_LANGUAGE
			case 2:
                p_item_res = &general_attr->res_jh_lankr;
                break;		
#endif
            default:
                break;
            }


        }
        else if(general_attr->new_focus == 2)
        {
            switch (i)
            {
            case 0:
                p_item_res = &general_attr->res_jh_inf;
                break;
            case 1:
                p_item_res = &general_attr->res_jh_cardformat;
                break;
            case	2:
                p_item_res = &general_attr->res_jh_defaultset;
                break;
            default:
                break;
            }


        }
        if(i == general_attr->pop_new_focus)
        {
        	__msg("general_attr->pop_new_focus\n");
            setting_general_pop_content_paint(msg);
        }
        else if(i == general_attr->pop_old_focus)
        {
            gui_rect.x0 = SET_ITEM_START_X;
            gui_rect.y0 = SET_ITEM_START_Y + SET_ITEM_H * (i % SET_PAGE_NUM);
            gui_rect.x1 = gui_rect.x0 + SET_ITEM_W;
            gui_rect.y1 = gui_rect.y0 + SET_ITEM_H;
#if 0
            GUI_ClearRectEx(&gui_rect);
#endif
			__msg("general_attr->pop_old_focus\n");
			if(general_attr->new_focus == 0)
			{
				__msg("draw: general_attr->h_backlight_unselect_bmp: %d\n", i);
				GUI_ClearRect(10, 2 + 60*i, 480, 62);
				GUI_BMP_Draw(theme_hdl2buf(general_attr->h_backlight_unselect_bmp), 10, 2 + 60*i);
			}
			else
			{
				GUI_ClearRectEx(&gui_rect);
				GUI_BMP_Draw(theme_hdl2buf(general_attr->h_bmp_unselect), 10, gui_rect.y0);
			}

            GUI_SetColor(general_attr->unfocus_txt_color);
            if(general_attr->new_focus == 1)
            {
#ifdef SET_ITEM_RIGHT_BMP
				if(i == current_language_set)
				{
					GUI_BMP_Draw(theme_hdl2buf(general_attr->h_bmp_jh_point_1), p_item_res->string_title_pos.x, p_item_res->bmp_right_pos.y);
				}
#endif

                GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x, p_item_res->string_title_pos.y);
            }
            else
            {
            	__msg("%s,%d\n", p_item_res->string_title, p_item_res->string_title_pos.y);
                GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x, p_item_res->string_title_pos.y + 10);
            }

        }
    }
    GUI_CloseAlphaBlend();
    GUI_LyrWinCacheOff();
    return EPDK_OK;

}
static __s32 setting_general_item_paint(__gui_msg_t *msg)
{
    setting_general_attr_t *general_attr;
    setting_item_res_t  *p_item_res;
    __s32 i;
    RECT rect;
    GUI_RECT gui_rect;
	__msg("setting_general_item_paint\n");
    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (general_attr == NULL)
        return EPDK_FAIL;

    if (GUI_LyrWinGetSta(general_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    GUI_LyrWinSel(general_attr->layer);
    GUI_SetFont(general_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_LyrWinCacheOn();
    GUI_OpenAlphaBlend();
    GUI_LyrWinGetScnWindow(general_attr->layer, &rect);

    for(i = 0; i < general_attr->item_nr; i++)
    {
        switch (i)
        {
        case 0:
            p_item_res = &general_attr->res_jh_display; //显示设置
            break;
        case 1:
            p_item_res = &general_attr->res_jh_language; //语音支持
            break;
			
#ifdef SET_ITEM_SENSOR		
		case 2:
			p_item_res = &general_attr->res_jh_senior; //高级
			break;
#else			
		case 2:
		{
			__msg("general_attr->res_jh_defaultset\n");
			p_item_res = &general_attr->res_jh_defaultset; //高级
			break;
		}			
#endif	
#ifdef SET_ITEM_PRO_INF
		case 3:
		{
			__msg("general_attr->res_jh_inf\n");
			p_item_res = &general_attr->res_jh_inf; 		//产品信息
			break;
		}
#endif		
#ifdef SET_ITEM_CARD_INF
		case 4:
		{
			__msg("general_attr->res_jh_cardformat\n");
			p_item_res = &general_attr->res_jh_cardformat;			//卡信息
			break;
		}
#endif		
#ifdef SET_ITEM_FACT_DEF
		case 5:
		{
			p_item_res = &general_attr->res_jh_defaultset;			//出厂设置
			break;
		}
#endif	
        default:
            break;
        }

        if(i == general_attr->new_focus)
        {
        	__msg("general_attr->new_focus:%d\n", general_attr->new_focus);
            setting_general_content_paint( msg);
        }
        else if(i == general_attr->old_focus)
        {

            gui_rect.x0 = SET_ITEM_START_X;
            gui_rect.y0 = SET_ITEM_START_Y + SET_ITEM_H * (i % SET_PAGE_NUM);
            gui_rect.x1 = gui_rect.x0 + SET_ITEM_W;
            gui_rect.y1 = gui_rect.y0 + SET_ITEM_H;
			__msg("general_attr->old_focus:%d\n", general_attr->old_focus);
			GUI_ClearRect(gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
            GUI_BMP_Draw(theme_hdl2buf(general_attr->h_bmp_unselect), 10, gui_rect.y0);

            GUI_SetColor(general_attr->unfocus_txt_color);
            GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x, p_item_res->string_title_pos.y);
        }
    }
    GUI_CloseAlphaBlend();
    GUI_LyrWinCacheOff();
    return EPDK_OK;
}
static __s32 seting_pop_information_paint(__gui_msg_t *msg)
{

    setting_general_attr_t *general_attr;
    setting_item_res_t	*p_item_res;
    __s32 i;
    GUI_RECT gui_rect;
    RECT rect;

    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (general_attr == NULL)
        return EPDK_FAIL;

    if (GUI_LyrWinGetSta(general_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    GUI_LyrWinSel(general_attr->layer);
    GUI_SetFont(general_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);

    GUI_LyrWinCacheOn();
    GUI_OpenAlphaBlend();
//    gui_rect.x0 = SET_ITEM_START_X;
//    gui_rect.y0 = SET_ITEM_START_Y + SET_ITEM_H * 2;
//    gui_rect.x1 = gui_rect.x0 + SET_ITEM_W;
//    gui_rect.y1 = gui_rect.y0 + SET_ITEM_H;

//    GUI_ClearRectEx(&gui_rect);


    GUI_LyrWinGetScnWindow(general_attr->layer, &rect);


    for(i = 0; i < 3; i++)
    {
        switch(i)
        {

        case 0:
            p_item_res = &general_attr->res_jh_verson;
            break;
        case 1:
            p_item_res = &general_attr->res_jh_totalsize;
            break;
        case 2:
            p_item_res = &general_attr->res_jh_freesize;
            break;
        default:
            break;


        }
        get_menu_text(pop_inf_title[i], p_item_res->string_title, 128);

        if(general_attr->pop_inf_new_focus == i)
        {
            setting_generl_popinf_content_paint(msg);
        }
        else
        {
            gui_rect.y0 = SET_ITEM_START_Y + SET_ITEM_H * i;
#ifdef SET_ITEM_SENSOR			
            GUI_BMP_Draw(theme_hdl2buf(general_attr->h_bmp_unselect), 10, gui_rect.y0);
#endif
            GUI_SetColor(general_attr->unfocus_txt_color);
            GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x - 40, p_item_res->string_title_pos.y);

            GUI_DispStringAt(p_item_res->string_content, p_item_res->string_title_pos.x + 70, p_item_res->string_title_pos.y);
        }
    }

    GUI_CloseAlphaBlend();
    GUI_LyrWinCacheOff();

    return EPDK_OK;

}
static __s32 setting_pop_general_paint(__gui_msg_t *msg)
{
    setting_general_attr_t *general_attr;
    setting_item_res_t	*p_item_res;
    __s32 i;
    GUI_RECT gui_rect;
    RECT rect;

    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (general_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(general_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }
	
	__msg("setting_pop_general_paint, general_attr->item_nr = %d\n", general_attr->item_nr);
	
    GUI_LyrWinSel(general_attr->layer);
    GUI_SetFont(general_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);

    GUI_LyrWinCacheOn();
    GUI_OpenAlphaBlend();
	
	setting_clear_screen();
	
    for(i = 0; i < general_attr->item_nr; i++)
    {
        if(general_attr->new_focus == 0)
        {
            switch (i)
            {
            case 0:
                p_item_res = &general_attr->res_jh_bglight;
                if(setting_reg_para)
                {
                    if( (setting_reg_para->backlight > 15) || (setting_reg_para->backlight < 0))
                    {
                        p_item_res->content_num = 3;
                    }
                    else
                    {
                        p_item_res->content_num = 4 - (setting_reg_para->backlight - 2) / 3 ; //setting_reg_para->backlight/3+2;
                    }
                }
                else
                {
                    p_item_res->content_num = 3;
                }
                break;
            case 1:
                p_item_res = &general_attr->res_jh_bgtime;
                if(setting_reg_para)
                {
                    switch(setting_reg_para->backlightoff)
                    {
                    case 0:
                        p_item_res->content_num = 0;
                        break;
                    case 5:
                        p_item_res->content_num = 1;
                        break;
                    case 10:
                        p_item_res->content_num = 2;
                        break;
                    case 20:
                        p_item_res->content_num = 3;
                        break;
                    case 30:
                        p_item_res->content_num = 4;
                        break;
                    case 60:
                        p_item_res->content_num = 5;
                        break;
                    case 300:
                        p_item_res->content_num = 6;
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

                break;
            default:
                break;
            }

            get_menu_text(pop_list_strdisplay_title[i], p_item_res->string_title, 128);
        }
        else if(general_attr->new_focus == 1)
        {
            switch (i)
            {
            case 0:
                p_item_res = &general_attr->res_jh_lanchn;
                break;
            case 1:
                p_item_res = &general_attr->res_jh_lanen;
                break;
#ifdef SUPPORT_KOREAN_LANGUAGE
			case 2:
				p_item_res = &general_attr->res_jh_lankr;
				break;
#endif				
            default:
                break;
            }
            get_menu_text(pop_list_strlanguage_title[i], p_item_res->string_title, 128);

            if(setting_reg_para->language == EPDK_LANGUAGE_ENM_CHINESES)
            {
                general_attr->pop_new_focus = 0;
                current_language_set = 0;
            }
#ifdef SUPPORT_KOREAN_LANGUAGE
			else if(setting_reg_para->language == EPDK_LANGUAGE_ENM_ENGLISH)
			{
				general_attr->pop_new_focus = 1;
				current_language_set = 1;
			}	
            else
            {
                general_attr->pop_new_focus = 2;
                current_language_set = 2;
            }
#else			
            else
            {
                general_attr->pop_new_focus = 1;
                current_language_set = 1;
            }
#endif

        }
        else if(general_attr->new_focus == 2)
        {
            switch (i)
            {
#ifdef SET_ITEM_SENSOR            
            case 0:
                p_item_res = &general_attr->res_jh_inf;
                break;
            case 1:
                p_item_res = &general_attr->res_jh_cardformat;
                break;
            case	2:
                p_item_res = &general_attr->res_jh_defaultset;
                break;
#else			
			case 0:
			{
				p_item_res = &general_attr->res_jh_inf;
				break;
			}
#endif				
            default:
                break;
            }

            get_menu_text(pop_list_strsenior_title[i], p_item_res->string_title, 128);
        }
#ifdef  SET_ITEM_PRO_INF
		else if(general_attr->new_focus == 3)
		{
            switch (i)
            {
			case 0:
			{
				p_item_res = &general_attr->res_jh_cardformat;
				break;
			}
            default:
                break;
            }

            get_menu_text(pop_list_strsenior_title[i], p_item_res->string_title, 128);
        }	
#endif
#ifdef  SET_ITEM_CARD_INF
		else if(general_attr->new_focus == 4)
		{
            switch (i)
            {
			case 0:
			{
				p_item_res = &general_attr->res_jh_defaultset;
				break;
			}
            default:
                break;
            }

            get_menu_text(pop_list_strsenior_title[i], p_item_res->string_title, 128);
        }	
#endif

    }
    GUI_LyrWinGetScnWindow(general_attr->layer, &rect);
    for(i = 0; i < general_attr->item_nr; i++)
    {
        if(general_attr->new_focus == 0)
        {
            switch (i)
            {
            case 0:
                p_item_res = &general_attr->res_jh_bglight;
                break;
            case 1:
                p_item_res = &general_attr->res_jh_bgtime;
                break;
            default:
                break;
            }

        }
        else if(general_attr->new_focus == 1)
        {
            switch (i)
            {
            case 0:
                p_item_res = &general_attr->res_jh_lanchn;
                break;
            case 1:
                p_item_res = &general_attr->res_jh_lanen;
                break;
#ifdef SUPPORT_KOREAN_LANGUAGE
			case 2:
				p_item_res = &general_attr->res_jh_lankr;
				break;	
#endif
            default:
                break;
            }


        }
        else if(general_attr->new_focus == 2)
        {
            switch (i)
            {
#ifdef SET_ITEM_SENSOR            
            case 0:
                p_item_res = &general_attr->res_jh_inf;
                break;
            case 1:
                p_item_res = &general_attr->res_jh_cardformat;
                break;
            case	2:
                p_item_res = &general_attr->res_jh_defaultset;
                break;
#else
			case 0:
			{
                p_item_res = &general_attr->res_jh_inf;
                break;
			}
#endif				
            default:
                break;
            }

        }
#ifdef SET_ITEM_PRO_INF
		else if(general_attr->new_focus == 3)
        {
            switch (i)
            {
			case 0:
			{
                p_item_res = &general_attr->res_jh_cardformat;
                break;
			}
            default:
                break;
            }

        }
#endif
#ifdef SET_ITEM_CARD_INF
		else if(general_attr->new_focus == 4)
        {
            switch (i)
            {
			case 0:
			{
                p_item_res = &general_attr->res_jh_defaultset;
                break;
			}
            default:
                break;
            }

        }
#endif

        if(i == general_attr->pop_new_focus)
        {
        	__msg("general_attr->pop_new_focus = %d\n", general_attr->pop_new_focus);
            setting_general_pop_content_paint(msg);
        }
        else
        {
            gui_rect.y0 = SET_ITEM_START_Y + SET_ITEM_H * i;
			if(general_attr->new_focus == 0)
			{
				GUI_BMP_Draw(theme_hdl2buf(general_attr->h_backlight_unselect_bmp), 10, 2 + i*60);
			}
			else
			{
				GUI_BMP_Draw(theme_hdl2buf(general_attr->h_bmp_unselect), 10, gui_rect.y0);
			}

			__msg("general_attr->pop_new_focus = %d\n", general_attr->pop_new_focus);
            GUI_SetColor(general_attr->unfocus_txt_color);
            if(general_attr->new_focus == 1)
            {
#ifdef SET_ITEM_RIGHT_BMP
				if(i == current_language_set)
				{
					GUI_BMP_Draw(theme_hdl2buf(general_attr->h_bmp_jh_point_1), p_item_res->string_title_pos.x, p_item_res->bmp_right_pos.y);
				}
#endif
				__msg("p_item_res->string_title:%s\n", p_item_res->string_title);
                GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x , p_item_res->string_title_pos.y);
            }
            else
            {
				__msg("p_item_res->string_title:%s\n", p_item_res->string_title);
                GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x, p_item_res->string_title_pos.y + 10);
            }
        }
    }
    GUI_CloseAlphaBlend();
    GUI_LyrWinCacheOff();

    return EPDK_OK;
}

static __s32 setting_general_default_paint(__gui_msg_t *msg)
{
	setting_general_attr_t *general_attr;
	setting_item_res_t	*p_item_res;
	__s32 i;
	GUI_RECT gui_rect;
	RECT rect;

	__msg("setting_general_default_paint\n");
	general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
	if (general_attr == NULL)
		return EPDK_FAIL;

	if (GUI_LyrWinGetSta(general_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
		return EPDK_FAIL;

	GUI_LyrWinSel(general_attr->layer);
	GUI_SetFont(general_attr->font);
	GUI_UC_SetEncodeUTF8();
	GUI_SetDrawMode(GUI_DRAWMODE_TRANS);

	GUI_LyrWinCacheOn();
	GUI_OpenAlphaBlend();

	//setting_clear_screen();

	for (i = 0; i < SET_PAGE_NUM; i++)
	{
		switch (i)
		{
		case 0:
			p_item_res = &general_attr->res_jh_display; //显示设置
			break;
		case 1:
			p_item_res = &general_attr->res_jh_language; //语音支持
			if(setting_reg_para)
			{
				if(setting_reg_para->language == EPDK_LANGUAGE_ENM_CHINESES)
					p_item_res->content_num = 0;
				else if(setting_reg_para->language == EPDK_LANGUAGE_ENM_ENGLISH)
					p_item_res->content_num = 1;
#ifdef SUPPORT_KOREAN_LANGUAGE
				else if(setting_reg_para->language == EPDK_LANGUAGE_ENM_CHINESET)
				{
					p_item_res->content_num = 2;
				}
#endif
				else
					p_item_res->content_num = 0;
			}
			else
			{
				p_item_res->content_num = 0;
			}

			p_item_res->content_nr = sizeof(content_lang_id) / sizeof(__s32);
			get_menu_text(content_lang_id[p_item_res->content_num], p_item_res->string_content, 128);
			break;
#ifdef SET_ITEM_SENSOR		
		case 2:
			p_item_res = &general_attr->res_jh_senior; //高级
			break;
#else			
		case 2:
		{
			p_item_res = &general_attr->res_jh_defaultset; //高级
			break;
		}			
#endif	
#ifdef SET_ITEM_PRO_INF
			case 3:
			{
				p_item_res = &general_attr->res_jh_inf; 		//产品信息
				break;
			}
#endif		
#ifdef SET_ITEM_CARD_INF
			case 4:
			{
				p_item_res = &general_attr->res_jh_cardformat;			//卡信息
				break;
			}
#endif		
#ifdef SET_ITEM_FACT_DEF
			case 5:
			{
				p_item_res = &general_attr->res_jh_defaultset;			//出厂设置
				break;
			}
#endif	
		default:
			break;
		}
		get_menu_text(one_list_str_title[i], p_item_res->string_title, 128);
		__msg("p_item_res->string_title = %s\n", p_item_res->string_title);
	}

	GUI_LyrWinGetScnWindow(general_attr->layer, &rect);
	//设置大图标下面那串文字
	for(i = 0; i < SET_PAGE_NUM; i++)
	{
		switch (i + SET_PAGE_NUM * current_page)
		{
		case 0:
			p_item_res = &general_attr->res_jh_display; //显示设置
			break;
		case 1:
			p_item_res = &general_attr->res_jh_language; //语音支持
			break;
#ifdef SET_ITEM_SENSOR		
		case 2:
			p_item_res = &general_attr->res_jh_senior; //高级
			break;
#else			
		case 2:
		{
			p_item_res = &general_attr->res_jh_defaultset; //高级
			break;
		}					
#endif	
#ifdef SET_ITEM_PRO_INF
		case 3:
		{
			p_item_res = &general_attr->res_jh_inf; 		//产品信息
			break;
		}
#endif		
#ifdef SET_ITEM_CARD_INF
		case 4:
		{
			p_item_res = &general_attr->res_jh_cardformat;			//卡信息
			break;
		}
#endif		
#ifdef SET_ITEM_FACT_DEF
		case 5:
		{
			p_item_res = &general_attr->res_jh_defaultset;			//出厂设置
			break;
		}
#endif	
		default:
			break;
		}

		if(i == general_attr->new_focus)
		{
			__msg("general_attr->new_focus:%d\n", general_attr->new_focus);
			setting_general_content_paint(msg);
		}
		else
		{
			gui_rect.x0 = SET_ITEM_START_X;
			gui_rect.y0 = SET_ITEM_START_Y + SET_ITEM_H * i;
			gui_rect.x1 = gui_rect.x0 + SET_ITEM_W;
			gui_rect.y1 = gui_rect.y0 + SET_ITEM_H;
			
			//__msg("gui_rect.x0;%d, gui_rect.y0:%d, gui_rect.x1:%d, gui_rect.y1:%d\n", gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
			GUI_BMP_Draw(theme_hdl2buf(general_attr->h_bmp_unselect), 10, gui_rect.y0);

			GUI_SetColor(general_attr->unfocus_txt_color);
			__msg("i = %d, p_item_res->string_title: %s\n",i, p_item_res->string_title);
			GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x, p_item_res->string_title_pos.y);
		}

	}
	GUI_CloseAlphaBlend();
	GUI_LyrWinCacheOff();
	prev_page = current_page;
	return EPDK_OK;
}


static __s32 setting_general_paint(__gui_msg_t *msg)
{
    setting_general_attr_t *general_attr;
    setting_item_res_t  *p_item_res;
    __s32 i;
    GUI_RECT gui_rect;
    RECT rect;

	__msg("setting_general_paint\n");
    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (general_attr == NULL)
        return EPDK_FAIL;

    if (GUI_LyrWinGetSta(general_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;

    GUI_LyrWinSel(general_attr->layer);
    GUI_SetFont(general_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);

    GUI_LyrWinCacheOn();
    GUI_OpenAlphaBlend();

//    gui_rect.x0 = SET_ITEM_START_X;
//    gui_rect.y0 = SET_ITEM_START_Y + SET_ITEM_H * 2;
//    gui_rect.x1 = gui_rect.x0 + SET_ITEM_W;
//    gui_rect.y1 = gui_rect.y0 + SET_ITEM_H;
//	__msg("gui_rect.x0;%d, gui_rect.y0:%d, gui_rect.x1:%d, gui_rect.y1:%d\n", gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
//    GUI_ClearRectEx(&gui_rect);
	setting_clear_screen();
    if(general_attr->new_focus > 2)
    {
        current_page = 1;
    }
    else
    {
        current_page = 0;
    }

    for (i = 0; i < SET_PAGE_NUM; i++)
    {
        switch (i)
        {
        case 0:
            p_item_res = &general_attr->res_jh_display; //显示设置
            break;
        case 1:
            p_item_res = &general_attr->res_jh_language; //语音支持
            if(setting_reg_para)
            {
                if(setting_reg_para->language == EPDK_LANGUAGE_ENM_CHINESES)
                    p_item_res->content_num = 0;
                else if(setting_reg_para->language == EPDK_LANGUAGE_ENM_ENGLISH)
                    p_item_res->content_num = 1;
#ifdef SUPPORT_KOREAN_LANGUAGE
				else if(setting_reg_para->language == EPDK_LANGUAGE_ENM_CHINESET)
				{
					p_item_res->content_num = 2;
				}
#endif
                else
                    p_item_res->content_num = 0;
            }
            else
            {
                p_item_res->content_num = 0;
            }

            p_item_res->content_nr = sizeof(content_lang_id) / sizeof(__s32);
            get_menu_text(content_lang_id[p_item_res->content_num], p_item_res->string_content, 128);
            break;
#ifdef SET_ITEM_SENSOR		
		case 2:
			p_item_res = &general_attr->res_jh_senior; //高级
			break;
#else			
		case 2:
		{
			p_item_res = &general_attr->res_jh_defaultset; //高级
			break;
		}			
#endif	
#ifdef SET_ITEM_PRO_INF
			case 3:
			{
				p_item_res = &general_attr->res_jh_inf; 		//产品信息
				break;
			}
#endif		
#ifdef SET_ITEM_CARD_INF
			case 4:
			{
				p_item_res = &general_attr->res_jh_cardformat;			//卡信息
				break;
			}
#endif		
#ifdef SET_ITEM_FACT_DEF
			case 5:
			{
				p_item_res = &general_attr->res_jh_defaultset;			//出厂设置
				break;
			}
#endif	
        default:
            break;
        }
        get_menu_text(one_list_str_title[i], p_item_res->string_title, 128);
		__msg("p_item_res->string_title = %s\n", p_item_res->string_title);
    }

    GUI_LyrWinGetScnWindow(general_attr->layer, &rect);
    //设置大图标下面那串文字
    for(i = 0; i < SET_PAGE_NUM; i++)
    {
        switch (i + SET_PAGE_NUM * current_page)
        {
        case 0:
            p_item_res = &general_attr->res_jh_display; //显示设置
            break;
        case 1:
            p_item_res = &general_attr->res_jh_language; //语音支持
            break;
#ifdef SET_ITEM_SENSOR		
		case 2:
			p_item_res = &general_attr->res_jh_senior; //高级
			break;
#else			
		case 2:
		{
			p_item_res = &general_attr->res_jh_defaultset; //高级
			break;
		}					
#endif	
#ifdef SET_ITEM_PRO_INF
		case 3:
		{
			p_item_res = &general_attr->res_jh_inf; 		//产品信息
			break;
		}
#endif		
#ifdef SET_ITEM_CARD_INF
		case 4:
		{
			p_item_res = &general_attr->res_jh_cardformat;			//卡信息
			break;
		}
#endif		
#ifdef SET_ITEM_FACT_DEF
		case 5:
		{
			p_item_res = &general_attr->res_jh_defaultset;			//出厂设置
			break;
		}
#endif	
        default:
            break;
        }

        if(i == general_attr->new_focus)
        {
        	__msg("general_attr->new_focus:%d\n", general_attr->new_focus);
            setting_general_content_paint(msg);
        }
        else
        {
            gui_rect.x0 = SET_ITEM_START_X;
            gui_rect.y0 = SET_ITEM_START_Y + SET_ITEM_H * i;
            gui_rect.x1 = gui_rect.x0 + SET_ITEM_W;
            gui_rect.y1 = gui_rect.y0 + SET_ITEM_H;
			
			//__msg("gui_rect.x0;%d, gui_rect.y0:%d, gui_rect.x1:%d, gui_rect.y1:%d\n", gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
            GUI_BMP_Draw(theme_hdl2buf(general_attr->h_bmp_unselect), 10, gui_rect.y0);

            GUI_SetColor(general_attr->unfocus_txt_color);
			__msg("i = %d, p_item_res->string_title: %s\n",i, p_item_res->string_title);
            GUI_DispStringAt(p_item_res->string_title, p_item_res->string_title_pos.x, p_item_res->string_title_pos.y);
        }

    }
    GUI_CloseAlphaBlend();
    GUI_LyrWinCacheOff();
    prev_page = current_page;
    return EPDK_OK;
}

static __s32  setting_general_key_proc(__gui_msg_t *msg)
{
    setting_general_attr_t *general_attr;
    static __s32 last_key = -1;
    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        if (GUI_MSG_KEY_ENTER == last_key)
        {
        	__msg("GUI_MSG_KEY_ENTER\n");
            main_cmd2parent(msg->h_deswin, ID_OP_ENTER, general_attr->new_focus, 0);
        }
    }
    else
    {
        switch( msg->dwAddData1 )
        {

        case GUI_MSG_KEY_LEFT:		//映射为up
        case GUI_MSG_KEY_LONGLEFT:
        {
			__msg("GUI_MSG_KEY_LEFT\n");
            main_cmd2parent(msg->h_deswin, ID_OP_UP, general_attr->new_focus, 0);
        }
        break;
        case GUI_MSG_KEY_RIGHT:		//映射为down
        case GUI_MSG_KEY_LONGRIGHT:
        {
			__msg("GUI_MSG_KEY_RIGHT\n");
            main_cmd2parent(msg->h_deswin, ID_OP_DOWN, general_attr->new_focus, 0);
        }
        break;
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LONGUP:
		{
			__msg("GUI_MSG_KEY_UP\n");
            main_cmd2parent(msg->h_deswin, ID_OP_UP, general_attr->new_focus, 0);
            break;
        }
        case GUI_MSG_KEY_DOWN:
        case GUI_MSG_KEY_LONGDOWN:
		{
			__msg("GUI_MSG_KEY_DOWN\n");
            main_cmd2parent(msg->h_deswin, ID_OP_DOWN, general_attr->new_focus, 0);
            break;
        }
        case GUI_MSG_KEY_ESCAPE:
        case GUI_MSG_KEY_MENU:
		{
			__msg("GUI_MSG_KEY_ESCAPE\n");
			main_cmd2parent(msg->h_deswin, ID_OP_EXIT, general_attr->new_focus, 0);
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

/*
	回调
*/

static __s32 _setting_general_Proc(__gui_msg_t *msg)
{
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        setting_general_attr_t *general_attr;
        setting_general_para_t *general_para;
        SIZE screen_size;

        setting_reg_para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
        general_para = (setting_general_para_t *)GUI_WinGetAttr(msg->h_deswin);
        general_attr = (setting_general_attr_t *)My_Balloc(sizeof(setting_general_attr_t));
        if(!general_attr)
        {

            __msg("memory balloc fail.........\n");
            return EPDK_FAIL;
        }
		__msg("_setting_general_Proc, GUI_MSG_CREATE\n");
        eLIBs_memset(general_attr, 0, sizeof(setting_general_attr_t));

        dsk_display_get_size(&screen_size.width, &screen_size.height);
		__msg("screen_size.width:%d, screen_size.height:%d\n", screen_size.width, screen_size.height);
        //在这里把相关坐标导入
        general_attr->general_uipara = setting_get_general_para(screen_size);

        general_attr->new_focus = general_para->focus_id;
        general_attr->old_focus = general_para->focus_id;
        general_attr->general_para = general_para;
        general_attr->h_bmp_unselect = theme_open(ID_SETTING_SET_ITEM_BMP);
        general_attr->h_bmp_jh_right = theme_open(ID_SETTING_SET_BUTT_RF_BMP);
        general_attr->h_bmp_jh_focus = theme_open(ID_SETTING_SET_ITEM_F_BMP);
		
        general_attr->h_backlight_select_bmp = theme_open(ID_SETTING_SET_BACKLIGHT_SL_BMP);
        general_attr->h_backlight_unselect_bmp = theme_open(ID_SETTING_SET_BACKLIGHT_UNSL_BMP);
        general_attr->h_proinf_bmp = theme_open(ID_SETTING_SET_PRODUCT_INFO_BG_BMP);
		

        general_attr->h_bmp_jh_point = theme_open(ID_SETTING_SET_OPT_POINT_BMP);
        general_attr->h_bmp_jh_point_1 = theme_open(ID_SETTING_SET_OPT_POINT_1_BMP);
        _setting_general_res_init(general_attr);
        //_setting_general_focus_res_reset(general_attr);
        _setting_general_content_res_reset(general_attr);

        general_attr->focus_txt_color = APP_COLOR_WHITE;
        general_attr->unfocus_txt_color = APP_COLOR_DARKGRAY;//APP_COLOR_BLACK; 
        general_attr->font = general_para->font;
        general_attr->layer = general_para->layer;

        GUI_WinSetAddData(msg->h_deswin, (__u32)general_attr);
        GUI_LyrWinSetSta(general_attr->layer, GUI_LYRWIN_STA_ON);
    }
    return EPDK_OK;
    case GUI_MSG_CLOSE:
    {
        GUI_FrmWinDelete(msg->h_deswin);
    }
    return EPDK_OK;
    case GUI_MSG_DESTROY:
    {
        setting_general_attr_t *general_attr;
        setting_general_para_t *general_para;
        general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
        if(!general_attr)
        {

            __err("general_attr is null ......\n");
            return EPDK_FAIL;
        }
        _setting_general_res_uninit(general_attr);
        theme_close(general_attr->h_bmp_unselect);
        theme_close(general_attr->h_bmp_jh_right);
        theme_close(general_attr->h_bmp_jh_focus);
        theme_close(general_attr->h_bmp_jh_point);
        theme_close(general_attr->h_bmp_jh_point_1);
        theme_close(general_attr->h_bmp_inf);
        theme_close(general_attr->h_backlight_select_bmp);
        theme_close(general_attr->h_backlight_unselect_bmp);
        theme_close(general_attr->h_proinf_bmp);
		
		

        //dsk_reg_flush();	//写进文件里面

        general_para = general_attr->general_para;
        if(general_para)
        {

            My_Bfree(general_para, sizeof(setting_general_para_t));
        }
        if(general_attr)
        {

            My_Bfree(general_attr, sizeof(setting_general_attr_t));
        }

    }
    return EPDK_OK;
    case GUI_MSG_PAINT:
        setting_general_paint(msg);
        return EPDK_OK;
    case GUI_MSG_KEY:
        setting_general_key_proc(msg);
        return EPDK_OK;
    case GUI_MSG_TOUCH:
        return EPDK_OK;
#if 0
    case MSG_OP_UP:
    {
        setting_general_attr_t *general_attr;
        general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
        general_attr->old_focus = general_attr->new_focus;
        if (general_attr->new_focus <= 0)
            general_attr->new_focus = general_attr->item_nr - 1;
        else
            general_attr->new_focus--;
        //if((general_attr->new_focus)%SET_PAGE_NUM ==2 )
        //  setting_general_paint(msg);
        //else
        setting_general_item_paint( msg);

    }
    return EPDK_OK;
    case MSG_OP_DOWN:
    {
        setting_general_attr_t *general_attr;
        general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

        general_attr->old_focus = general_attr->new_focus;
        if (general_attr->new_focus >= general_attr->item_nr - 1)
            general_attr->new_focus = 0;
        else
            general_attr->new_focus++;
        //if((general_attr->new_focus)%SET_PAGE_NUM ==0 )
        // setting_general_paint(msg);
        //else
        setting_general_item_paint( msg);

    }
    return EPDK_OK;
#endif
    case MSG_OP_UP:
    case MSG_OP_LEFT:
    {
        setting_general_attr_t *general_attr;
        general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
        if(current_list_layer == MAIN_LIST_LAYER)
        {
            general_attr->old_focus = general_attr->new_focus;
            if (general_attr->new_focus <= 0)
                general_attr->new_focus = general_attr->item_nr - 1;
            else
                general_attr->new_focus--;
            setting_general_item_paint( msg);
        }
        else if(current_list_layer == POP_LIST_LAYER)
        {
            general_attr->pop_old_focus = general_attr->pop_new_focus;
            if (general_attr->pop_new_focus <= 0)
                general_attr->pop_new_focus = general_attr->item_nr - 1;
            else
                general_attr->pop_new_focus--;
            setting_general_pop_item_paint(msg);
        }
        else
        {
            general_attr->pop_inf_old_focus = general_attr->pop_inf_new_focus;
            if(general_attr->pop_inf_new_focus <= 0)
                general_attr->pop_inf_new_focus = 2;
            else
                general_attr->pop_inf_new_focus--;
            setting_general_popinf_item_paint(msg);
        }

    }
    return EPDK_OK;
    case MSG_OP_DOWN:
    case MSG_OP_RIGHT:
    {
        setting_general_attr_t *general_attr;
        general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
        if(current_list_layer == MAIN_LIST_LAYER)
        {
        	__msg("current_list_layer == MAIN_LIST_LAYER\n");
            general_attr->old_focus = general_attr->new_focus;
            if (general_attr->new_focus >= general_attr->item_nr - 1)
            {
                general_attr->new_focus = 0;
            }
            else
            {
                general_attr->new_focus++;
            }
            setting_general_item_paint( msg);
        }
        else if(current_list_layer == POP_LIST_LAYER)
        {
        	__msg("current_list_layer == POP_LIST_LAYER\n");
            general_attr->pop_old_focus = general_attr->pop_new_focus;
            if (general_attr->pop_new_focus >= general_attr->item_nr - 1)
            {        	
                general_attr->pop_new_focus = 0;
            }
            else
            {
                general_attr->pop_new_focus++;
            }
            setting_general_pop_item_paint(msg);
        }
        else
        {
        	__msg("general_attr->pop_inf_new_focus = %d\n", general_attr->pop_inf_new_focus);
            general_attr->pop_inf_old_focus = general_attr->pop_inf_new_focus;
            if(general_attr->pop_inf_new_focus >= 2)
            {
                general_attr->pop_inf_new_focus = 0;
            }
            else
            {
                general_attr->pop_inf_new_focus++;
            }
            setting_general_popinf_item_paint(msg);
        }

    }
    return EPDK_OK;

    case MSG_OP_ENTER:
    {
        setting_general_attr_t *general_attr;
        setting_item_res_t  *p_item_res;

        general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
        if(current_list_layer == MAIN_LIST_LAYER)
        {
            current_list_layer = POP_LIST_LAYER;

			__msg("POP_LIST_LAYER\n");
			if((general_attr->new_focus  == 3) || (general_attr->new_focus  == 4))
			{
				current_list_layer = MAIN_LIST_LAYER;
				general_attr->old_focus = general_attr->new_focus;
				__msg("current_list_layer: %d\n", current_list_layer);
				if(general_attr->new_focus  == 3)
				{
					ES_DIR			*p_file;
                    p_file = eLIBs_opendir(USER_DISK);
                    if(!p_file)//没有卡
                    {
						__msg("ID_SWITCH_TO_TIPS: TIPS_NOCARD_NOTE\n");
                        main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_NOCARD_NOTE, 256);
                    }
                    else
                    {
                    	__msg("ID_SWITCH_TO_TIPS: TIPS_FORMAT_CARD\n");
                        main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_FORMAT_CARD, 192);
                    }
				}
				else
				{			
                    //default set
                    __msg("ID_SWITCH_TO_TIPS: TIPS_FACTORY_DEFAULT\n");
                    main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_FACTORY_DEFAULT, 64);
				}
				return EPDK_OK;
			}
            general_attr->pop_new_focus = 0;
            general_attr->pop_old_focus = 0;
            //_setting_general_res_uninit(general_attr);
            _setting_pop_general_res_init(general_attr);
            // _setting_general_focus_pop_res_reset(general_attr);
            _setting_general_pop_content_res_reset(general_attr);
            setting_pop_general_paint(msg);
        }
        else
        {
        	__msg("general_attr->new_focus: %d\n", general_attr->new_focus);
            if(general_attr->new_focus == 0)
            {
            	__msg("general_attr->pop_new_focus\n");
                if(general_attr->pop_new_focus == 0)
                {
                    p_item_res = &general_attr->res_jh_bglight;
                    //00  10  01  11
                    main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_BACK_LIGHT, (p_item_res->content_num) & 0x7f);
                }
                else if(general_attr->pop_new_focus == 1)
                {
                    p_item_res = &general_attr->res_jh_bgtime;
                    main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_BACK_LIGHT, (p_item_res->content_num) | 0x80);
                }
            }
            else if(general_attr->new_focus  == 1)
            {
                if(setting_reg_para)
                {
#ifdef SUPPORT_KOREAN_LANGUAGE
                    if(general_attr->pop_new_focus == 0)
                    {
                        setting_reg_para->language = EPDK_LANGUAGE_ENM_CHINESES;
                    }
                    else if(general_attr->pop_new_focus == 1)
                    {
                        setting_reg_para->language = EPDK_LANGUAGE_ENM_ENGLISH;
                    }
					else if(general_attr->pop_new_focus == 2)
					{
						setting_reg_para->language = EPDK_LANGUAGE_ENM_CHINESET;
					}
					else
					{
						setting_reg_para->language = EPDK_LANGUAGE_ENM_CHINESES;
					}
					
#else
					if(general_attr->pop_new_focus == 0)
					   setting_reg_para->language = EPDK_LANGUAGE_ENM_CHINESES;
				   else if(general_attr->pop_new_focus == 1)
					   setting_reg_para->language = EPDK_LANGUAGE_ENM_ENGLISH;
				   else
					   setting_reg_para->language = EPDK_LANGUAGE_ENM_CHINESES;
#endif					
					__msg("dsk_langres_set_type\n");
                    dsk_langres_set_type(setting_reg_para->language);
                    setting_pop_general_paint(msg);
                }
            }
            else if(general_attr->new_focus == 2)
            {
                if(general_attr->pop_new_focus == 0)//详细信息
                {
                    // seting_pop_information_paint(msg);
                    if(current_list_layer == POP_LIST_LAYER)
                    {
#ifdef SET_ITEM_SENSOR
						__msg("general_attr->pop_new_focus: 详细信息\n");
                        get_tf_size();
                        current_list_layer = POP_INF_LIST_LAYER;
                        general_attr->pop_inf_new_focus = 0;
                        general_attr->pop_inf_old_focus = 0;
                        _setting_general_popinf_init(general_attr);
                        seting_pop_information_paint(msg);
#endif						
                    }
                }
                else if(general_attr->pop_new_focus == 1)
                {
                    ES_DIR			*p_file;
                    p_file = eLIBs_opendir(USER_DISK);
                    if(!p_file)//没有卡
                    {
						__msg("ID_SWITCH_TO_TIPS: TIPS_NOCARD_NOTE\n");
                        main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_NOCARD_NOTE, 256);
                    }
                    else
                    {
                    	__msg("ID_SWITCH_TO_TIPS: TIPS_FORMAT_CARD\n");
                        main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_FORMAT_CARD, 192);
                    }
                }
                else
                {
                    //default set
                    __msg("ID_SWITCH_TO_TIPS: TIPS_FACTORY_DEFAULT\n");
                    main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_FACTORY_DEFAULT, 64);
                }
            }
#ifdef	SET_ITEM_PRO_INF
			else if(general_attr->new_focus == 3)
			{
				ES_DIR			*p_file;
                p_file = eLIBs_opendir(USER_DISK);
				__msg("Card format item\n");
                if(!p_file)//没有卡
                {
					__msg("ID_SWITCH_TO_TIPS: TIPS_NOCARD_NOTE\n");
                    main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_NOCARD_NOTE, 256);
                }
                else
                {
                	__msg("ID_SWITCH_TO_TIPS: TIPS_FORMAT_CARD\n");
                    main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_FORMAT_CARD, 192);
                }
				
			}
#endif
#ifdef	SET_ITEM_CARD_INF
			else if(general_attr->new_focus == 4)
			{
				__msg("Factory Defaut item\n");
				__msg("ID_SWITCH_TO_TIPS: TIPS_FACTORY_DEFAULT\n");
				
				main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_FACTORY_DEFAULT, 64);
			}
#endif

        }

    }
    return EPDK_OK;
    case MSG_FORMAT_CRAD:
    {
        //setting_general_attr_t *general_attr;
        // general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
        // format_card();
    }
    return EPDK_OK;
    case MSG_RESTORE_FACTORY:
        setting_restore_default(msg);

        return EPDK_OK;
    case MSG_RESTORE_BLACKLIGHT:
        dsk_display_set_lcd_bright((__lion_bright_t)(setting_reg_para->backlight));
        return EPDK_OK;
    case MSG_SET_BLACKLIGHT:
    {
        setting_general_attr_t *general_attr;
        setting_item_res_t  *p_item_res;
        general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
        setting_reg_para->backlight = dsk_display_get_lcd_bright();
        p_item_res = &general_attr->res_jh_bglight;
        p_item_res->content_num = 4 - (setting_reg_para->backlight - 2) / 3 ;
        setting_general_pop_item_paint(msg);
    }
    return EPDK_OK;
    case MSG_SET_BLACKTIME:
#if 1
    {
        __u32 black_lightoff_time;
        setting_general_attr_t *general_attr;
        setting_item_res_t  *p_item_res;
        general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
        if(msg->dwReserved == 0)
            black_lightoff_time = 0;
        else if(msg->dwReserved == 1)
            black_lightoff_time = 5;
        else if(msg->dwReserved == 2)
            black_lightoff_time = 10;
        else if(msg->dwReserved == 3)
            black_lightoff_time = 20;
        else if(msg->dwReserved == 4)
            black_lightoff_time = 30;
        else if(msg->dwReserved == 5)
            black_lightoff_time = 60;
        else
            black_lightoff_time = 300;
        g_set_close_scn_time(black_lightoff_time);
        if(setting_reg_para)
        {
            setting_reg_para->backlightoff = black_lightoff_time;
        }
        p_item_res = &general_attr->res_jh_bgtime;
        p_item_res->content_num = msg->dwReserved ;
        setting_general_pop_item_paint(msg);
    }
#endif
    return EPDK_OK;
    case MSG_OP_EXIT:
        if(current_list_layer == MAIN_LIST_LAYER)
        {
            __gui_msg_t my_msg;
            my_msg.id = GUI_MSG_CLOSE;
            my_msg.h_srcwin = msg->h_deswin;
            my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
            my_msg.dwAddData1 = 0;
            my_msg.dwAddData2 = 0;
            GUI_SendNotifyMessage(&my_msg);
        }
        else if(current_list_layer == POP_LIST_LAYER)
        {
            setting_general_attr_t *general_attr;
            general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

            current_list_layer = MAIN_LIST_LAYER;
            // _setting_general_pop_res_uninit(general_attr);
            _setting_general_res_init(general_attr);
            //_setting_general_focus_res_reset(general_attr);
            _setting_general_content_res_reset(general_attr);
            setting_general_paint(msg);
        }
        else
        {
            setting_general_attr_t *general_attr;
            general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);
            current_list_layer = POP_LIST_LAYER;
            _setting_pop_general_res_init(general_attr);
            _setting_general_pop_content_res_reset(general_attr);
            setting_pop_general_paint(msg);


        }
        return EPDK_OK;

    case GUI_MSG_COMMAND:
        break;

    default:
        break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN setting_general_win_create(H_WIN h_parent, setting_general_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    setting_general_para_t  *general_para;
    FB fb;

    GUI_LyrWinGetFB(para->layer, &fb);
	
	__msg("setting_general_win_create\n");
	
    general_para = (setting_general_para_t *)My_Balloc(sizeof(setting_general_para_t));
    if(!general_para)
    {

        __msg("memory balloc fail.........\n");
        return EPDK_FAIL;
    }
	
    eLIBs_memset((void *)general_para, 0, sizeof(setting_general_para_t));
    general_para->font = para->font;
    general_para->layer = para->layer;
    general_para->focus_id = para->focus_id;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =	"general setting win",
                  framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner	= NULL;
    framewin_para.id         = GENERAL_MENU_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_setting_general_Proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr =  (void *)general_para;
    framewin_para.hLayer = para->layer;

    return (GUI_FrmWinCreate(&framewin_para));
}


__s32 setting_general_win_delete(H_WIN list_win)
{
    GUI_FrmWinDelete(list_win);
    return EPDK_OK;
}

