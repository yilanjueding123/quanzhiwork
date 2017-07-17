/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: spsc_ui.c
* By      	:
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/
#include "spsc_ui.h"

static movie_spsc_uipara_t uipara_400_240 =
{
    1,

    {
        70/*20*/, /*7*/150, 384, 94
    }
};

static movie_spsc_uipara_t uipara_720_576 =
{
    1,

    {
        (720 - 364) / 2, 40, 364, 94
    }
};

static movie_spsc_uipara_t uipara_720_480 =
{
    1,

    {
        (720 - 364) / 2, 40, 364, 94
    }
};

movie_spsc_uipara_t *movie_spsc_get_uipara(__s32 rotate)
{
    __s32 			screen_width;
    __s32 			screen_height;


    /* get lcd size*/
    dsk_display_get_size(&screen_width, &screen_height);
    switch(rotate)
    {
    case GUI_SCNDIR_NORMAL:
    case GUI_SCNDIR_ROTATE180:
    {
        if((screen_width == 400 ) && ( screen_height == 240))
            return &uipara_400_240;
        else if((screen_width == 720 ) && ( screen_height == 576))
            return &uipara_720_576;
        else if((screen_width == 720 ) && ( screen_height == 480))
            return &uipara_720_480;
    }
    case GUI_SCNDIR_ROTATE90:
    case GUI_SCNDIR_ROTATE270:

        break;
    }

    return &uipara_400_240;
}
