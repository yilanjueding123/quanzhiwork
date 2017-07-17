/*
*********************************************************************************************************
*											        eMOD
*						           the Easy Portable/Player Operation System
*									           mod_dbms sub-system
*
*						        (c) Copyright 2010-2012, Sunny China
*											All	Rights Reserved
*
* File   : mod_bookengine.h
* Version: V1.0
* By     : Sunny
* Date	 : 2011-5-2 13:30:30
*********************************************************************************************************
*/
#ifndef __MOD_BOOKENGINE_H__
#define __MOD_BOOKENGINE_H__

#include "typedef.h"
#include "cfgs.h"
typedef enum EBOOK_LANGUAGE_STRU
{
    EBOOK_LANGUAGE_ALBANIAN                                    ,
    EBOOK_LANGUAGE_BELARUSIAN                                  ,
    EBOOK_LANGUAGE_BOSNIAN                                     ,
    EBOOK_LANGUAGE_BULGARIAN                                   ,
    EBOOK_LANGUAGE_CHS                                         ,
    EBOOK_LANGUAGE_CHT                                         ,
    EBOOK_LANGUAGE_CROATIAN                                    ,
    EBOOK_LANGUAGE_CZECH                                       ,
    EBOOK_LANGUAGE_DUTCH                                       ,
    EBOOK_LANGUAGE_ENGLISH                         = 0x100     ,
    EBOOK_LANGUAGE_ESPERANTO                                   ,
    EBOOK_LANGUAGE_ESTONIAN                                    ,
    EBOOK_LANGUAGE_FRENCH                                      ,
    EBOOK_LANGUAGE_GERMAN                                      ,
    EBOOK_LANGUAGE_GREENLANDIC                                 ,
    EBOOK_LANGUAGE_HUNGARIAN                                   ,
    EBOOK_LANGUAGE_ITALIAN                                     ,
    EBOOK_LANGUAGE_LATVIAN                                     ,
    EBOOK_LANGUAGE_LITHUANIAN                                  ,
    EBOOK_LANGUAGE_LOWER_SORBIAN                               ,
    EBOOK_LANGUAGE_MACEDONIAN                                  ,
    EBOOK_LANGUAGE_MALTESE                                     ,
    EBOOK_LANGUAGE_NORWEGIAN                                   ,
    EBOOK_LANGUAGE_POLISH                                      ,
    EBOOK_LANGUAGE_PORTUGUESE                                  ,
    EBOOK_LANGUAGE_ROMANIAN                                    ,
    EBOOK_LANGUAGE_RUSSIAN                                     ,
    EBOOK_LANGUAGE_SAMI                                        ,
    EBOOK_LANGUAGE_SERBIAN                                     ,
    EBOOK_LANGUAGE_SERBIAN_LATIN_SCRIPT                        ,
    EBOOK_LANGUAGE_SERBIAN_CYRILLIC                            ,
    EBOOK_LANGUAGE_SERBIAN_LATIN                               ,
    EBOOK_LANGUAGE_SLOVAK                                      ,
    EBOOK_LANGUAGE_SLOVENE                                     ,
    EBOOK_LANGUAGE_SPANISH                                     ,
    EBOOK_LANGUAGE_SWEDISH                                     ,
    EBOOK_LANGUAGE_TURKISH                                     ,
    EBOOK_LANGUAGE_UKRAINIAN                                   ,
    EBOOK_LANGUAGE_UPPER_SORBIAN

} EBOOK_LANGUAGE;


typedef enum EBOOK_CHARENCODE_STRU
{
    EBOOK_CHARENCODE_UNKNOWN     = 0X800 ,
    EBOOK_CHARENCODE_AUTO                ,
    EBOOK_CHARENCODE_ISO8859_1           ,
    EBOOK_CHARENCODE_ISO8859_2           ,
    EBOOK_CHARENCODE_ISO8859_3           ,
    EBOOK_CHARENCODE_ISO8859_4           ,
    EBOOK_CHARENCODE_ISO8859_5           ,
    EBOOK_CHARENCODE_ISO8859_6           ,
    EBOOK_CHARENCODE_ISO8859_7           ,
    EBOOK_CHARENCODE_ISO8859_8           ,
    EBOOK_CHARENCODE_ISO8859_9           ,
    EBOOK_CHARENCODE_ISO8859_10          ,
    EBOOK_CHARENCODE_ISO8859_11          ,
    EBOOK_CHARENCODE_ISO8859_13          ,
    EBOOK_CHARENCODE_ISO8859_14          ,
    EBOOK_CHARENCODE_ISO8859_15          ,
    EBOOK_CHARENCODE_ISO8859_16          ,
    EBOOK_CHARENCODE_WINDOWS_1250        ,
    EBOOK_CHARENCODE_WINDOWS_1251        ,
    EBOOK_CHARENCODE_WINDOWS_1252        ,
    EBOOK_CHARENCODE_WINDOWS_1253        ,
    EBOOK_CHARENCODE_WINDOWS_1254        ,
    EBOOK_CHARENCODE_WINDOWS_1255        ,
    EBOOK_CHARENCODE_WINDOWS_1256        ,
    EBOOK_CHARENCODE_WINDOWS_1257        ,
    EBOOK_CHARENCODE_WINDOWS_1258        ,
    EBOOK_CHARENCODE_KOI8_R              ,
    EBOOK_CHARENCODE_KOI8_U              ,
    EBOOK_CHARENCODE_UTF8                ,
    EBOOK_CHARENCODE_UTF16_LE            ,
    EBOOK_CHARENCODE_UTF16_BE            ,
    EBOOK_CHARENCODE_GB2312              ,
    EBOOK_CHARENCODE_GBK                 ,
    EBOOK_CHARENCODE_ANSI
} EBOOK_CHARENCODE;

typedef enum TIME_ZONE_E
{
    //西经
    TIMEZONE_GMTSUB12  = 0,
    TIMEZONE_GMTSUB11    ,
    TIMEZONE_GMTSUB10    ,
    TIMEZONE_GMTSUB9H    ,
    TIMEZONE_GMTSUB9     ,
    TIMEZONE_GMTSUB8H    ,
    TIMEZONE_GMTSUB8     ,
    TIMEZONE_GMTSUB7     ,
    TIMEZONE_GMTSUB6     ,
    TIMEZONE_GMTSUB5     ,
    TIMEZONE_GMTSUB4     ,
    TIMEZONE_GMTSUB3H    ,
    TIMEZONE_GMTSUB3     ,
    TIMEZONE_GMTSUB2     ,
    TIMEZONE_GMTSUB1     ,

    //零时区
    TIMEZONE_GMT         ,

    //东经
    TIMEZONE_GMTADD1     ,
    TIMEZONE_GMTADD2     ,
    TIMEZONE_GMTADD3     ,
    TIMEZONE_GMTADD3H    ,
    TIMEZONE_GMTADD4     ,
    TIMEZONE_GMTADD4H    ,
    TIMEZONE_GMTADD5     ,
    TIMEZONE_GMTADD5H    ,
    TIMEZONE_GMTADD6     ,
    TIMEZONE_GMTADD6H    ,
    TIMEZONE_GMTADD7     ,
    TIMEZONE_GMTADD8     ,
    TIMEZONE_GMTADD9     ,
    TIMEZONE_GMTADD9H    ,
    TIMEZONE_GMTADD10    ,
    TIMEZONE_GMTADD10H   ,
    TIMEZONE_GMTADD11    ,
    TIMEZONE_GMTADD11H   ,
    TIMEZONE_GMTADD12    ,
    TIMEZONE_GMTADD13    ,
    TIMEZONE_GMTADD14    ,
} time_zone_e;

typedef enum
{
    JM_FONT_COLOR_WHITE = 0x100,
    JM_FONT_COLOR_BLACK ,
    JM_FONT_COLOR_RED,
    JM_FONT_COLOR_GREEN,
    JM_FONT_COLOR_BLUE,
    JM_FONT_COLOR_CYAN,
    JM_FONT_COLOR_FUCHSIA,
    JM_FONT_COLOR_YELLOW
} JM_FONT_COLOR;

typedef enum
{
    Could_not_initialize_the_document 		= 0x01,
    Document_loading_was_not_successful		= 0x02,
    Could_not_create_renderer				= 0x03,
    The_pageno_was_out_of_the_range			= 0x04,
    The_book_was_protected					= 0x51,
    Out_Of_Memory							= 0x61,
    Cound_Not_Extract_Location				= 0x71
} BOOKENGINE_ERROR;

typedef struct	/* 书签信息，用于内部解码用... */
{
    int				scnwidth;
    int				scnheight;
    double			scale;
    int 			reflow;
    int				fontsize;
    int 			currentpageno;
    JM_FONT_COLOR	fontcolor;
    int 			backcolor_red;
    int 			backcolor_green;
    int 			backcolor_blue;
    EBOOK_LANGUAGE  language; //当前语言
    EBOOK_CHARENCODE charencSetting;//当前编码方式
    char			bookmarkstr[512];
} EBOOKMARK;

typedef struct	/* 断点信息 */
{
    EBOOKMARK		bookmark;
    char			synopsis[512];		/* 断点处的文字信息 */
} EBOOKTAG;

typedef struct	/* ebook书籍信息	 */
{
    char 	booktitle[256];				/* ebook书名	*/
    char 	bookauthor[256];			/* ebook作者	*/
    char 	booksubject[128];			/* ebook主题    */
    int 	booktotlepages;				/* ebook总页数  */
    char	bookpublisher[128];			/*ebookpublisher信息*/
    char	bookexpiration[128];		/*ebookExpiration信息（过期时间信息）*/

} EBOOKINFO;


typedef struct
{
    int 				w;			/* 图片的宽度信息              */
    int 				h;			/* 图片的高度信息              */
    unsigned char 		*buf;		/* 图片的数据指针(rgb格式存放) */
    int 				pageno;		/* 该buf对应的页码信息	       */
    void				*surface;	/* 解码私有句柄，用户不需关心  */

} EBOOKBITMAP;

typedef EBOOKBITMAP *Hebookbitmap;

typedef void *Hebook;	/* 此为打开一个ebook所获得的句柄 */

typedef struct
{
    void *API_ebook_open					;
    void *API_ebook_get_page				;
    void *API_ebook_goto_booktag			;
    void *API_ebook_goto_end				;
    void *API_ebook_find_text				;
    void *API_ebook_free_pagemem			;
    void *API_ebook_close					;
    void *API_ebook_set_scnwidth			;
    void *API_ebook_set_scnheight			;
    void *API_ebook_set_fontsize			;
    void *API_ebook_set_reflow_mode			;
    void *API_ebook_cancel_reflow_mode		;
    void *API_ebook_set_scale				;
    void *API_ebook_set_margin_left			;
    void *API_ebook_set_margin_right		;
    void *API_ebook_set_margin_top			;
    void *API_ebook_set_margin_bottom		;
    void *API_ebook_setbackcolor			;
    void *API_ebook_setfontcolor			;
    void *API_ebook_get_bookcover			;
    void *API_ebook_free_bookcover			;
    void *API_ebook_get_bookinfo			;
    void *API_ebook_get_totalpagecount		;
    void *API_ebook_get_screen				;
    void *API_ebook_save_booktag			;
    void *API_ebook_settimezone			    ;
    void *API_ebook_get_fontcolor			;
    void *API_ebook_get_fontsize			;
    void *API_ebook_get_backcolor			;
    void *API_ebook_close_and_getbookcover	;
    void *API_ebook_set_language            ;
    void *API_ebook_set_charencode          ;
    void *API_ebook_get_language            ;
    void *API_ebook_get_charencode          ;

} __api_bookengine_t;

typedef struct
{
    void    *API_BOOKENGINE_ApiTbl;
} __BOOKENGINE_FuncTbl_MAIN_t;

#define BOOKENGINECALL_FUNCNO(x, y)    (SWINO(0, x, y))

#define BOOKENGINE_FUNCNO(ApiName) ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) | \
                                     BOOKENGINECALL_FUNCNO(__api_bookengine_t, ApiName))

#define API_EBOOK_OPEN                      BOOKENGINE_FUNCNO(API_ebook_open                    )
#define API_EBOOK_GET_PAGE                  BOOKENGINE_FUNCNO(API_ebook_get_page                )
#define API_EBOOK_GOTO_BOOKTAG              BOOKENGINE_FUNCNO(API_ebook_goto_booktag            )
#define API_EBOOK_GOTO_END                  BOOKENGINE_FUNCNO(API_ebook_goto_end                )
#define API_EBOOK_FIND_TEXT                 BOOKENGINE_FUNCNO(API_ebook_find_text               )
#define API_EBOOK_FREE_PAGEMEM              BOOKENGINE_FUNCNO(API_ebook_free_pagemem            )
#define API_EBOOK_CLOSE                     BOOKENGINE_FUNCNO(API_ebook_close                   )
#define API_EBOOK_SET_SCNWIDTH              BOOKENGINE_FUNCNO(API_ebook_set_scnwidth            )
#define API_EBOOK_SET_SCNHEIGHT             BOOKENGINE_FUNCNO(API_ebook_set_scnheight           )
#define API_EBOOK_SET_FONTSIZE              BOOKENGINE_FUNCNO(API_ebook_set_fontsize            )
#define API_EBOOK_SET_REFLOW_MODE           BOOKENGINE_FUNCNO(API_ebook_set_reflow_mode         )
#define API_EBOOK_CANCEL_REFLOW_MODE        BOOKENGINE_FUNCNO(API_ebook_cancel_reflow_mode      )
#define API_EBOOK_SET_SCALE                 BOOKENGINE_FUNCNO(API_ebook_set_scale               )
#define API_EBOOK_SET_MARGIN_LEFT           BOOKENGINE_FUNCNO(API_ebook_set_margin_left         )
#define API_EBOOK_SET_MARGIN_RIGHT          BOOKENGINE_FUNCNO(API_ebook_set_margin_right        )
#define API_EBOOK_SET_MARGIN_TOP            BOOKENGINE_FUNCNO(API_ebook_set_margin_top          )
#define API_EBOOK_SET_MARGIN_BOTTOM         BOOKENGINE_FUNCNO(API_ebook_set_margin_bottom       )
#define API_EBOOK_SETBACKCOLOR              BOOKENGINE_FUNCNO(API_ebook_setbackcolor            )
#define API_EBOOK_SETFONTCOLOR              BOOKENGINE_FUNCNO(API_ebook_setfontcolor            )
#define API_EBOOK_GET_BOOKCOVER             BOOKENGINE_FUNCNO(API_ebook_get_bookcover           )
#define API_EBOOK_FREE_BOOKCOVER            BOOKENGINE_FUNCNO(API_ebook_free_bookcover          )
#define API_EBOOK_GET_BOOKINFO              BOOKENGINE_FUNCNO(API_ebook_get_bookinfo            )
#define API_EBOOK_GET_TOTALPAGECOUNT        BOOKENGINE_FUNCNO(API_ebook_get_totalpagecount      )
#define API_EBOOK_GET_SCREEN                BOOKENGINE_FUNCNO(API_ebook_get_screen              )
#define API_EBOOK_SAVE_BOOKTAG              BOOKENGINE_FUNCNO(API_ebook_save_booktag            )
#define API_EBOOK_SAVE2PNG                  BOOKENGINE_FUNCNO(API_ebook_save2png                )
#define API_EBOOK_SETTIMEZONE               BOOKENGINE_FUNCNO(API_ebook_settimezone             )
#define API_EBOOK_GET_FONTCOLOR             BOOKENGINE_FUNCNO(API_ebook_get_fontcolor           )
#define API_EBOOK_GET_FONTSIZE              BOOKENGINE_FUNCNO(API_ebook_get_fontsize            )
#define API_EBOOK_GET_BACKCOLOR             BOOKENGINE_FUNCNO(API_ebook_get_backcolor           )
#define API_EBOOK_CLOSE_AND_GETBOOKCOVER    BOOKENGINE_FUNCNO(API_ebook_close_and_getbookcover  )
#define API_EBOOK_SET_LANGUAGE              BOOKENGINE_FUNCNO(API_ebook_set_language            )
#define API_EBOOK_SET_CHARENCODE            BOOKENGINE_FUNCNO(API_ebook_set_charencode          )
#define API_EBOOK_GET_LANGUAGE              BOOKENGINE_FUNCNO(API_ebook_get_language            )
#define API_EBOOK_GET_CHARENCODE            BOOKENGINE_FUNCNO(API_ebook_get_charencode          )

#endif  /* __MOD_BOOKENGINE_H__ */
