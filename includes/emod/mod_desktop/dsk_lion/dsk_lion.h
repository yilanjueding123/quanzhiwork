/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: dsk_lion.h
* By      	: Andy.zhang
* Version	: v1.0
* ============================================================================================================
* 2009-11-3 9:39:42  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __DSK_LION_H__
#define __DSK_LION_H__


#define REGNODE_DISP    	"\\settings\\disp"
#define REGNODE_POWSAVE    	"\\settings\\powsave"
#define REGNODE_AUTOOFF		"\\settings\\autooff"
#define REGNODE_SOUND  		"\\settings\\sound"
#define REGNODE_VOLUME 		"\\settings\\voice_volume"
#define REGNODE_COLOR    	"\\settings\\gamma"
#define REGNODE_MODULE  	"\\settings\\module"

typedef enum tag_LION_CLOSETIME
{
    LION_5SEC,
    LION_10SEC,
    LION_15SEC,
    LION_20SEC,
    LION_30SEC,
    LION_60SEC,
    LION_ALWAYS
} __lion_closetime_t;

typedef enum tag_LION_SLEEPTIME
{
    LION_1MIN,
    LION_5MIN,
    LION_10MIN,
    LION_15MIN,
    LION_30MIN,
    LION_60MIN
} __lion_sleeptime_t;

typedef enum tag_LION_AUTOOFF
{
    LION_OFF1MIN = 0,
    LION_OFF5MIN,
    LION_OFF10MIN,
    LION_OFF15MIN,
    LION_OFF20MIN,
    LION_OFF30MIN,
    LION_OFF1HOUR,
    LION_OFF2HOUR,
    LION_ALWAYSON
} __lion_autooff_t;

typedef enum tag_LION_KEYTON
{
    LION_KEYTONE_ON = 0,
    LION_KEYTONE_OFF,
} __lion_keytone_t;

typedef enum tag_LION_OUTPUT_TYPE
{
    LION_OUTPUT_LCD,
    LION_OUTPUT_TV,
    LION_OUTPUT_HDMI,
    LION_OUTPUT_VGA,
    LION_OUTPUT_NONE
} __lion_output_type_t;

typedef enum tag_LION_DISP_TVFORMAT
{
    LION_DISP_TV_PAL,
    LION_DISP_TV_NTSC,
    LION_DISP_TV_PAL_SVIDEO,
    LION_DISP_TV_NTSC_SVIDEO,
    LION_DISP_TV_PAL_CVBS_SVIDEO,
    LION_DISP_TV_NTSC_CVBS_SVIDEO,

    LION_DISP_TV_480I,
    LION_DISP_TV_576I,
    LION_DISP_TV_480P,
    LION_DISP_TV_576P,
    LION_DISP_TV_720P_50HZ,
    LION_DISP_TV_720P_60HZ,
    LION_DISP_TV_1080I_50HZ,
    LION_DISP_TV_1080I_60HZ,
    LION_DISP_TV_1080P_24HZ,
    LION_DISP_TV_1080P_50HZ,
    LION_DISP_TV_1080P_60HZ,

    LION_DISP_HDMI_480I,
    LION_DISP_HDMI_576I,
    LION_DISP_HDMI_480P,
    LION_DISP_HDMI_576P,
    LION_DISP_HDMI_720P_50HZ,
    LION_DISP_HDMI_720P_60HZ,
    LION_DISP_HDMI_1080I_50HZ,
    LION_DISP_HDMI_1080I_60HZ,
    LION_DISP_HDMI_1080P_24HZ,
    LION_DISP_HDMI_1080P_50HZ,
    LION_DISP_HDMI_1080P_60HZ,

    LION_DISP_VGA_H1680_V1050,
    LION_DISP_VGA_H1440_V900,
    LION_DISP_VGA_H1360_V768,
    LION_DISP_VGA_H1280_V1024,
    LION_DISP_VGA_H1024_V768,
    LION_DISP_VGA_H800_V600,
    LION_DISP_VGA_H640_V480,
    LION_DISP_VGA_H1440_V900_RB,
    LION_DISP_VGA_H1680_V1050_RB,
    LION_DISP_VGA_H1920_V1080_RB,
    LION_DISP_VGA_H1920_V1080,

} __lion_disp_tvformat_t;

typedef enum tag_LION_BRIGHT
{
    LION_BRIGHT_LEVEL1 = 0,
    LION_BRIGHT_LEVEL2,
    LION_BRIGHT_LEVEL3,
    LION_BRIGHT_LEVEL4,
    LION_BRIGHT_LEVEL5,
    LION_BRIGHT_LEVEL6,
    LION_BRIGHT_LEVEL7,
    LION_BRIGHT_LEVEL8,
    LION_BRIGHT_LEVEL9,
    LION_BRIGHT_LEVEL10,
    LION_BRIGHT_LEVEL11,
    LION_BRIGHT_LEVEL12,
    LION_BRIGHT_LEVEL13,
    LION_BRIGHT_LEVEL14,
    LION_BRIGHT_LEVEL15
} __lion_bright_t;

typedef enum tag_LION_CHARSET
{
    LION_CHARSET_ENM_GB2312  = 0,       // 简体中文
    LION_CHARSET_ENM_UTF8       ,       // utf8
    LION_CHARSET_ENM_UTF16BE    ,       // utf16be
    LION_CHARSET_ENM_UTF16LE    ,       // utf16le
    LION_CHARSET_ENM_BIG5       ,       // 繁体中文
    LION_CHARSET_ENM_GBK        ,       // 中文
    LION_CHARSET_ENM_SJIS       ,       // 日文
    LION_CHARSET_ENM_EUC_JP     ,       // 日文
    LION_CHARSET_ENM_EUC_KR     ,       // 韩文
    LION_CHARSET_ENM_KIO8_R     ,       // 俄文
    LION_CHARSET_ENM_ISO_8859_1 ,       // cp1252.西欧语言
    LION_CHARSET_ENM_ISO_8859_2 ,       // 中欧语言
    LION_CHARSET_ENM_ISO_8859_3 ,       // 南欧语言
    LION_CHARSET_ENM_ISO_8859_4 ,       // 北欧语言
    LION_CHARSET_ENM_ISO_8859_5 ,       // 西里尔字母
    LION_CHARSET_ENM_ISO_8859_6 ,       // 阿拉伯语
    LION_CHARSET_ENM_ISO_8859_7 ,       // 希腊语
    LION_CHARSET_ENM_ISO_8859_8 ,       // 希伯来语
    LION_CHARSET_ENM_ISO_8859_9 ,       // cp1254，土耳其语
    LION_CHARSET_ENM_ISO_8859_10,       // 北欧斯堪的纳维亚语系
    LION_CHARSET_ENM_ISO_8859_11,       // 泰语
    LION_CHARSET_ENM_ISO_8859_12,       // 梵文
    LION_CHARSET_ENM_ISO_8859_13,       // 波罗的海语系
    LION_CHARSET_ENM_ISO_8859_14,       // 凯尔特人语系
    LION_CHARSET_ENM_ISO_8859_15,       // 扩展了法语和芬兰语的西欧语系
    LION_CHARSET_ENM_ISO_8859_16,       // 扩展的东南欧语系
    LION_CHARSET_ENM_CP874      ,       // 泰文
    LION_CHARSET_ENM_CP1250     ,       // 中欧
    LION_CHARSET_ENM_CP1251     ,       // 西里尔文
    LION_CHARSET_ENM_CP1253     ,       // 希腊文
    LION_CHARSET_ENM_CP1255     ,       // 希伯来文
    LION_CHARSET_ENM_CP1256     ,       // 阿拉伯文
    LION_CHARSET_ENM_CP1257     ,       // 波罗的海文
    LION_CHARSET_ENM_CP1258     ,       // 越南

    LION_CHARSET_ENM_UNKNOWN = -1       // always the last one
} __lion_charset_t;


// 1. 注册表操作
/******************************************************************************/
__lion_disp_tvformat_t 	lion_get_disp_tvformat(void);
__lion_disp_tvformat_t 	lion_get_tvformatdefault(void);
__s32 					lion_set_disp_tvformat(__lion_disp_tvformat_t format);

/******************************************************************************/
__lion_autooff_t 		lion_get_autooff(void);
__lion_autooff_t 		lion_get_autooffdefault(void);
__s32 					lion_set_autooff(__lion_autooff_t autooff);

/******************************************************************************/
__lion_closetime_t 		lion_get_closescn(void);
__lion_closetime_t 		lion_get_closescndefault(void);
__s32 					lion_set_closescn(__lion_closetime_t closescn);

/******************************************************************************/
__lion_keytone_t 		lion_get_keytone(void);
__lion_keytone_t 		lion_get_keytonedefault(void);
__s32 					lion_set_keytone(__lion_keytone_t keytone);

/******************************************************************************/
__lion_bright_t 		lion_get_bright(void);
__lion_bright_t 		lion_get_defaultbright(void);
__s32 					lion_set_bright(__lion_bright_t bright);

/******************************************************************************/
__u32 					lion_get_volume(void);
__u32 					lion_get_volumedefault(void);
__s32 					lion_set_volume(__u32 volume);

/******************************************************************************/
__u32 					lion_reg_get_gamma(void);
__u32 					lion_reg_get_gammadefault(void);
__s32 					lion_reg_set_gamma(__u32 value);
__s32 					lion_reg_get_sysgamma(void);			// 获取当前实际的gamma值
__s32 					lion_reg_set_sysgamma(__u32 value);		// 保存当前实际的gamma值

/******************************************************************************/
__s32 					lion_setting_default(void);

__s32					lion_get_app_restore(char *app_name);
__s32					lion_set_app_restore(char *app_name, __s32 value);

// 2. 功能操作
/******************************************************************************/
__lion_output_type_t 	lion_display_cur_output			(void);
__s32                   lion_display_switch_output      (__lion_output_type_t output, __lion_disp_tvformat_t mode);
__s32 					lion_display_off				(void);
__s32 					lion_display_on					(__lion_output_type_t output);
void 					lion_audio_switch_to_iis		(void);
void 					lion_audio_switch_to_dac		(void);
void 					lion_audio_switch_to_spdif		(void);
__s32                   lion_set_hdmi_audio             ( __u32 value );
void                    lion_audio_switch_if            ( __audio_dev_interface_t dev_if_type );
__audio_dev_interface_t lion_get_cur_audio_if           (void);
__s32 					lion_set_gamma					(__u32 gamma);
__s32 					lion_set_gamma_on				(void);
__s32 					lion_set_gamma_off				(void);

/******************************************************************************/
__s32                   lion_sound_keytone_on           (void);
__s32                   lion_sound_keytone_off          (void);

/******************************************************************************/
__s32                   lion_set_bright_level         	(__lion_bright_t level);
__lion_bright_t         lion_get_birght_level         	(void);

/******************************************************************************/
__s32                   lion_sound_speaker_on           (void);
__s32                   lion_sound_speaker_off          (void);

/******************************************************************************/
__s32                   lion_set_autooff_time           (__u32 time);
__s32                   lion_get_autooff_time           (void);

/******************************************************************************/
__s32                   lion_set_autosleep_time         (__u32 time);
__s32                   lion_get_autosleep_time         (void);

/******************************************************************************/
void 					lion_set_closescreen_enable		(void);
void 					lion_set_closescreen_disable	(void);
__s32                   lion_set_closescreen_time       (__u32 time);
__u32                   lion_get_closescreen_time       (void);

/******************************************************************************/
__s32                   lion_set_charset                (__lion_charset_t charset);
__lion_charset_t        lion_get_charset                (void);

/******************************************************************************/
void 					lion_system_restore				(void);

/******************************************************************************/
__s32                   lion_set_volume_level         	(__s32 level);
__s32			        lion_get_volume_level         	(void);

/******************************************************************************/

#endif	/*	__DSK_LION_H__	*/

