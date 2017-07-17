
/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: spsc_prog_ui.c
* By      	: Bayden.chen
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#include "spsc_prog_ui.h"

static movie_spsc_prog_uipara_t uipara_400_240 =
{
    {
        0, 77, 384, 16
        //0,200,384,16
    },
    {
        {
            76, 75, 30, 15,
            //90, 198,30,15,
            ID_MOVIE_SPSC_PROG_PLAYSTA_PAUSE_BMP,
            ID_MOVIE_SPSC_PROG_PLAYSTA_PLAY_BMP,
        },
        {
            110, 80, 146, 4,
            //130,203,146,4,
            ID_MOVIE_SPSC_PROG_BG_UF_BMP
        },
        {
            110, 80, 5, 4,
            //130,203,5,4,
            ID_MOVIE_SPSC_PROG_CURSOR_UF_BMP
        }
    },
    {
        {
            0, 73, 20, 16
            //20, 198,16,16
        },
        {
            28, 73, 20, 16
            //44,198,16,16
        },
        {
            56, 73, 20, 16
            //68,198,16,16
        },
    },
    {
        {
            20, 73, 8, 16
            //36,198,8,16
        },
        {
            48, 73, 8, 16
            //60,198,8,16
        }
    },
    {
        {
            257 + 0, 73, 20, 16
            //260+20, 198, 16, 16
        },
        {
            257 + 28, 73, 20, 16
            //260+24+20, 198, 16, 16
        },
        {
            257 + 56, 73, 20, 16
            //260+48+20, 198, 16, 16
        },
    },
    {
        {
            257 + 20, 73, 8, 16
            //260+16+20, 198, 8, 16
        },
        {
            257 + 48, 73, 8, 16
            //260+40+20, 198, 8, 16
        }
    },
};

movie_spsc_prog_uipara_t *movie_spsc_prog_get_uipara(__s32 rotate)
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

    }
    case GUI_SCNDIR_ROTATE90:
    case GUI_SCNDIR_ROTATE270:

        break;
    }

    return &uipara_400_240;
}
