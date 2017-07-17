/*
*********************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Development Kit
*                                               eLibC sub-system
*
*                                    (c) Copyright 2009-2010, kevin China
*                                              All Rights Reserved
*
* File    : elibs_ebook.c
* By      : Sunny
* Version : V1.00
* Date    : 2011-5-2 13:02:24
*********************************************************************************************************
*/


#include "elibs_bookengine_i.h"


Hebook ebook_open(char *filename, unsigned char *errno, EBOOK_LANGUAGE lang, EBOOK_CHARENCODE encode)
{
    typedef Hebook (*ebook_open_t)(char *, unsigned char *, EBOOK_LANGUAGE, EBOOK_CHARENCODE);

    __u32   apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                      | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_open));
    ebook_open_t  entry = (ebook_open_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return entry(filename, errno, lang, encode);
}

Hebookbitmap ebook_get_page(Hebook host, int pageno, unsigned char *errno, unsigned char *destbuf)
{
    typedef Hebookbitmap (*ebook_get_page_t)(Hebook, int, unsigned char *, unsigned char * );

    __u32   apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                      | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_get_page));
    ebook_get_page_t  entry = (ebook_get_page_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return entry(host, pageno, errno, destbuf);
}

Hebookbitmap ebook_goto_booktag(Hebook host, EBOOKTAG *tag, unsigned char *errno, unsigned char *destbuf)
{
    typedef Hebookbitmap (*ebook_goto_booktag_t)(Hebook, EBOOKTAG *, unsigned char *, unsigned char *);

    __u32   apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                      | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_goto_booktag));
    ebook_goto_booktag_t entry = (ebook_goto_booktag_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return entry(host, tag, errno, destbuf);
}

Hebookbitmap ebook_goto_end(Hebook host)
{
    typedef Hebookbitmap (*ebook_goto_end_t)(Hebook);

    __u32   apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                      | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_goto_end));
    ebook_goto_end_t entry = (ebook_goto_end_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return entry(host);
}

Hebookbitmap ebook_find_text(Hebook host, char text[])
{
    typedef Hebookbitmap (*ebook_find_text_t)(Hebook, char *);

    __u32   apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                      | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_find_text));
    ebook_find_text_t entry = (ebook_find_text_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return entry(host, text);
}

void ebook_free_pagemem(Hebookbitmap surface)
{
    typedef void (*ebook_free_pagemem_t)(Hebookbitmap);

    __u32   apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                      | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_free_pagemem));
    ebook_free_pagemem_t entry = (ebook_free_pagemem_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(surface);
}

void ebook_close(Hebook host)
{
    typedef void (*ebook_close_t)(Hebook);

    __u32   apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                      | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_close));
    ebook_close_t entry = (ebook_close_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(host);
}


int ebook_set_scnwidth(Hebook host, unsigned int scnwidth)
{
    typedef int (*ebook_set_scnwidth_t)(Hebook, unsigned int);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_set_scnwidth));
    ebook_set_scnwidth_t entry = (ebook_set_scnwidth_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return entry(host, scnwidth);
}

int ebook_set_scnheight(Hebook host, unsigned int scnheight)
{
    typedef int (*ebook_set_scnheight_t)(Hebook, unsigned int);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_set_scnheight));
    ebook_set_scnheight_t entry = (ebook_set_scnheight_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return entry(host, scnheight);
}

int ebook_set_fontsize(Hebook host, unsigned int fontsize)
{
    typedef int (*ebook_set_fontsize_t)(Hebook, unsigned int);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_set_fontsize));
    ebook_set_fontsize_t entry = (ebook_set_fontsize_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return entry(host, fontsize);
}

void ebook_set_reflow_mode(Hebook host)
{
    typedef void (*ebook_set_reflow_mode_t)(Hebook);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_set_reflow_mode));
    ebook_set_reflow_mode_t entry = (ebook_set_reflow_mode_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(host);
}

void ebook_cancel_reflow_mode(Hebook host)
{
    typedef void (*ebook_cancel_reflow_mode_t)(Hebook);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_cancel_reflow_mode));
    ebook_cancel_reflow_mode_t entry = (ebook_cancel_reflow_mode_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(host);
}

void ebook_set_scale(Hebook host, float s)
{
    typedef void (*ebook_set_scale_t)(Hebook, float);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_set_scale));
    ebook_set_scale_t entry = (ebook_set_scale_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(host, s);
}

void ebook_set_margin_left(double left)
{
    typedef void (*ebook_set_margin_left_t)(double);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_set_margin_left));
    ebook_set_margin_left_t entry = (ebook_set_margin_left_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(left);
}

void ebook_set_margin_right(double right)
{
    typedef void (*ebook_set_margin_right_t)(double);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_set_margin_right));
    ebook_set_margin_right_t entry = (ebook_set_margin_right_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(right);
}

void ebook_set_margin_top(double top)
{
    typedef void (*ebook_set_margin_top_t)(double);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_set_margin_top));
    ebook_set_margin_top_t entry = (ebook_set_margin_top_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(top);
}

void ebook_set_margin_bottom(double bottom)
{
    typedef void (*ebook_set_margin_bottom_t)(double);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_set_margin_bottom));
    ebook_set_margin_bottom_t entry = (ebook_set_margin_bottom_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(bottom);
}

void ebook_setbackcolor(int red, int green, int blue)
{
    typedef void (*ebook_setbackcolor_t)(int, int, int);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_setbackcolor));
    ebook_setbackcolor_t entry = (ebook_setbackcolor_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(red, green, blue);
}

void ebook_setfontcolor(JM_FONT_COLOR color)
{
    typedef void (*ebook_setfontcolor_t)(JM_FONT_COLOR);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_setfontcolor));
    ebook_setfontcolor_t entry = (ebook_setfontcolor_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(color);
}

Hebookbitmap ebook_get_bookcover(char *filename, int width, int height, unsigned char *errno, unsigned char *destbuf)
{
    typedef Hebookbitmap (*ebook_get_bookcover_t)(char *, int, int, unsigned char *, unsigned char * );

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_get_bookcover));
    ebook_get_bookcover_t entry = (ebook_get_bookcover_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return entry(filename, width, height, errno, destbuf);
}

void ebook_free_bookcover(Hebookbitmap surface)
{
    typedef void (*ebook_free_bookcover_t)(Hebookbitmap);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_free_bookcover));
    ebook_free_bookcover_t entry = (ebook_free_bookcover_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(surface);
}

void ebook_get_bookinfo(char *filename,	EBOOKINFO *bookinfo, unsigned char *errno)
{
    typedef void (*ebook_get_bookinfo_t)(char *, EBOOKINFO *, unsigned char *);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_get_bookinfo));
    ebook_get_bookinfo_t entry = (ebook_get_bookinfo_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(filename, bookinfo, errno);
}

int ebook_get_totalpagecount(Hebook host)
{
    typedef int (*ebook_get_totalpagecount_t)(Hebook);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_get_totalpagecount));
    ebook_get_totalpagecount_t entry = (ebook_get_totalpagecount_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return entry(host);
}

Hebookbitmap ebook_get_screen(Hebook host_1, int num, unsigned char *errno, unsigned char *destbuf)
{
    typedef Hebookbitmap (*ebook_get_screen_t)(Hebook, int, unsigned char *, unsigned char *);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_get_screen));
    ebook_get_screen_t entry = (ebook_get_screen_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return entry(host_1, num, errno, destbuf);
}

void ebook_save_booktag(Hebook host, int num, EBOOKTAG *sbooktag, unsigned char *errno)
{
    typedef void (*ebook_save_booktag_t)(Hebook, int, EBOOKTAG *, unsigned char *);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_save_booktag));
    ebook_save_booktag_t entry = (ebook_save_booktag_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(host, num, sbooktag, errno);
}

void ebook_settimezone(time_zone_e tz, int is_daylight)
{
    typedef void (*ebook_settimezone_t)(time_zone_e, int);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_settimezone));
    ebook_settimezone_t entry = (ebook_settimezone_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(tz, is_daylight);
}

JM_FONT_COLOR ebook_get_fontcolor(Hebook host)
{
    typedef JM_FONT_COLOR (*ebook_get_fontcolor_t)(Hebook);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_get_fontcolor));
    ebook_get_fontcolor_t entry = (ebook_get_fontcolor_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return entry(host);
}


int ebook_get_fontsize(Hebook host)
{
    typedef int (*ebook_get_fontsize_t)(Hebook);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_get_fontsize));
    ebook_get_fontsize_t entry = (ebook_get_fontsize_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return entry(host);
}

int ebook_get_backcolor(Hebook host)
{
    typedef int (*ebook_get_backcolor_t)(Hebook);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_get_backcolor));
    ebook_get_backcolor_t entry = (ebook_get_backcolor_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return entry(host);
}

Hebookbitmap ebook_close_and_getbookcover(Hebook host, int width, int height, unsigned char *errno, unsigned char *destbuf)
{
    typedef Hebookbitmap (*ebook_close_and_getbookcover_t)(char *, int, int, unsigned char *, unsigned char * );

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_close_and_getbookcover));
    ebook_close_and_getbookcover_t entry = (ebook_close_and_getbookcover_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return entry(host, width, height, errno, destbuf);
}

void   ebook_set_language(Hebook host, EBOOK_LANGUAGE lang)
{
    typedef void (*ebook_set_language_t)(Hebook, EBOOK_LANGUAGE);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_set_language));
    ebook_set_language_t entry = (ebook_set_language_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(host, lang);
}

void ebook_set_charencode(Hebook host, EBOOK_CHARENCODE encode)
{
    typedef void (*ebook_set_charencode_t)(Hebook, EBOOK_CHARENCODE);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_set_charencode));
    ebook_set_charencode_t entry = (ebook_set_charencode_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    entry(host, encode);
}

EBOOK_LANGUAGE  ebook_get_language(Hebook host)
{
    typedef EBOOK_LANGUAGE (*ebook_get_language_t)(Hebook);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_get_language));
    ebook_get_language_t entry = (ebook_get_language_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return (EBOOK_LANGUAGE)entry(host);
}

EBOOK_CHARENCODE ebook_get_charencode(Hebook host)
{
    typedef EBOOK_CHARENCODE (*ebook_get_charencode_t)(Hebook);

    __u32 apinum = ((BOOKENGINECALL_FUNCNO(__BOOKENGINE_FuncTbl_MAIN_t, API_BOOKENGINE_ApiTbl) << 16) \
                    | BOOKENGINECALL_FUNCNO(__api_bookengine_t, API_ebook_get_charencode));
    ebook_get_charencode_t entry = (ebook_get_charencode_t)esMSTUB_GetFuncEntry(EMOD_BOOKENGINE_ID, apinum);

    return (EBOOK_CHARENCODE)entry(host);

}
