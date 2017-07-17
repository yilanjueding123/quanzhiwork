
/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: app_movie_uipara.c
* By      	: Bayden.chen
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/
#include "spsc_ctrl.h"
#include "spsc_ctrl_ui.h"

static movie_spsc_ctrl_uipara_t uipara_400_240 =
{
    {
        0, 0, 384, 94
    },

    4,
    {
        0, 0, 384, 94,
        ID_MOVIE_SPSC_BG_BMP
    },
    {
        {
            7, 3, 24, 18,
            ID_MOVIE_SPSC_PREV_UF_BMP,
            ID_MOVIE_SPSC_PREV_F_BMP
        },
        {
            43, 3, 24, 18,
            ID_MOVIE_SPSC_NEXT_UF_BMP,
            ID_MOVIE_SPSC_NEXT_F_BMP,
        },
        {
            79, 3, 24, 18,
            ID_MOVIE_SPSC_RR_UF_BMP,
            ID_MOVIE_SPSC_RR_F_BMP,
        },
        {
            115, 3, 24, 18,
            ID_MOVIE_SPSC_FF_UF_BMP,
            ID_MOVIE_SPSC_FF_F_BMP,
        },
        {
            151, 3, 24, 18,
            ID_MOVIE_SPSC_PLAYPAUSE_UF_BMP,
            ID_MOVIE_SPSC_PLAYPAUSE_F_BMP,
        },
        {
            187, 3, 24, 18,
            ID_MOVIE_SPSC_CHANNEL_UF_BMP,
            ID_MOVIE_SPSC_CHANNEL_F_BMP,
        },
        {
            7, 25, 24, 18,
            ID_MOVIE_SPSC_TRACK_UF_BMP,
            ID_MOVIE_SPSC_TRACK_F_BMP,
        },
        {
            43, 25, 24, 18,
            ID_MOVIE_SPSC_TIMESET_UF_BMP,
            ID_MOVIE_SPSC_TIMESET_F_BMP,
        },
        {
            79, 25, 24, 18,
            ID_MOVIE_SPSC_PLAYMODE_UF_BMP,
            ID_MOVIE_SPSC_PLAYMODE_F_BMP,
        },
        {
            115, 25, 24, 18,
            ID_MOVIE_SPSC_BRIGHTSET_UF_BMP,
            ID_MOVIE_SPSC_BRIGHTSET_F_BMP,
        },
        {
            151, 25, 24, 18,
            ID_MOVIE_SPSC_SCREENSET_UF_BMP,
            ID_MOVIE_SPSC_SCREENSET_F_BMP,
        },
        {
            187, 25, 24, 18,
            ID_MOVIE_SPSC_TVOUT_UF_BMP,
            ID_MOVIE_SPSC_TVOUT_F_BMP,
        }
    },
    {
        {
            0, 0, 0, 0
        },
        {
            0, 0, 0, 0
        },
        {
            0, 0, 0, 0
        },
        {
            0, 0, 0, 0
        },
        {
            0, 0, 0, 0
        },
        {
            0, 0, 0, 0
        },
        {
            0, 0, 0, 0
        },
        {
            0, 0, 0, 0
        },
        {
            0, 0, 0, 0
        },
        {
            0, 0, 0, 0
        }
    },
    {
        {
            228, 4, 24, 18
        },
        {
            263, 4, 13, 12
        },
        {
            285, 4, 24, 18
        },
        {
            313, 4, 35, 15
        },
        {
            230, 25, 384 - 230, 16
        },
        {
            230, 42, 384 - 230, 16
        },
        {
            230, 59, 384 - 230, 16
        },
        {
            10, 45 + 5, 195, 18 + 2
        }
    },
    {
        {
            ID_MOVIE_SPSC_ONLY_ONCE_BMP
        },
        {
            ID_MOVIE_SPSC_ROTATE_ONE_BMP
        },
        {
            ID_MOVIE_SPSC_ROTATE_ALL_BMP
        },
        {
            ID_MOVIE_SPSC_SEQUENCE_BMP
        },
        {
            ID_MOVIE_SPSC_RANDOM_BMP
        }
    },
    {
        {
            ID_MOVIE_SPSC_VIDEO_RATIO_BMP
        },
        {
            ID_MOVIE_SPSC_SCREEN_RATIO_BMP
        },
        {
            ID_MOVIE_SPSC_ORIGIN_SIZE_BMP
        },
        {
            ID_MOVIE_SPSC_4R3_BMP
        },
        {
            ID_MOVIE_SPSC_16R9_BMP
        }
    },

    {
        ID_MOVIE_SPSC_BRIGHT_STATIC_ICON_BMP
    }
};

movie_spsc_ctrl_uipara_t *movie_spsc_ctrl_get_uipara(__s32 rotate)
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
