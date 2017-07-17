
/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: prog_ui.c
* By      	: Bayden.chen
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#include "prog_ui.h"

static movie_prog_uipara_t uipara_400_240 =
{
    1,
    {
        (480 - 364) / 2, /*40*/220, 364, 32
    },
    {
        0, 0, 364, 24
    },
    {
        {
            110, /*(16-4)/2*/10, 146, 4,
            ID_MOVIE_PROG_BG_UF_BMP
        },
        {
            110, /*(16-4)/2*/10, 5, 4,
            ID_MOVIE_PROG_CURSOR_UF_BMP
        }
    },
    {
        {
            0 + 24, 0 + 3, 20, 16
        },
        {
            28 + 24, 0 + 3, 20, 16
        },
        {
            56 + 24, 0 + 3, 20, 16
        },
    },
    {
        {
            20 + 24, 0 + 3, 8, 16
        },
        {
            48 + 24, 0 + 3, 8, 16
        }
    },
    {
        {
            260 + 0 + 24, 0 + 3, 20, 16
        },
        {
            260 + 28 + 24, 0 + 3, 20, 16
        },
        {
            260 + 56 + 24, 0 + 3, 20, 16
        },
    },
    {
        {
            260 + 20 + 24, 0 + 3, 8, 16
        },
        {
            260 + 48 + 24, 0 + 3, 8, 16
        }
    },
};

static movie_prog_uipara_t uipara_720_576 =
{
    1,
    {
        (720 - 364) / 2, 60, 364, 16
    },
    {
        0, 0, 364, 16
    },
    {
        {
            110, (16 - 4) / 2, 146, 4,
            ID_MOVIE_PROG_BG_UF_BMP
        },
        {
            110, (16 - 4) / 2, 5, 4,
            ID_MOVIE_PROG_CURSOR_UF_BMP
        }
    },
    {
        {
            0 + 24, 0, 16, 16
        },
        {
            24 + 24, 0, 16, 16
        },
        {
            48 + 24, 0, 16, 16
        },
    },
    {
        {
            16 + 24, 0, 8, 16
        },
        {
            40 + 24, 0, 8, 16
        }
    },
    {
        {
            260 + 0 + 24, 0, 16, 16
        },
        {
            260 + 24 + 24, 0, 16, 16
        },
        {
            260 + 48 + 24, 0, 16, 16
        },
    },
    {
        {
            260 + 16 + 24, 0, 8, 16
        },
        {
            260 + 40 + 24, 0, 8, 16
        }
    },
};

static movie_prog_uipara_t uipara_720_480 =
{
    1,
    {
        (720 - 364) / 2, 60, 364, 16
    },
    {
        0, 0, 364, 16
    },
    {
        {
            110, (16 - 4) / 2, 146, 4,
            ID_MOVIE_PROG_BG_UF_BMP
        },
        {
            110, (16 - 4) / 2, 5, 4,
            ID_MOVIE_PROG_CURSOR_UF_BMP
        }
    },
    {
        {
            0 + 24, 0, 16, 16
        },
        {
            24 + 24, 0, 16, 16
        },
        {
            48 + 24, 0, 16, 16
        },
    },
    {
        {
            16 + 24, 0, 8, 16
        },
        {
            40 + 24, 0, 8, 16
        }
    },
    {
        {
            260 + 0 + 24, 0, 16, 16
        },
        {
            260 + 24 + 24, 0, 16, 16
        },
        {
            260 + 48 + 24, 0, 16, 16
        },
    },
    {
        {
            260 + 16 + 24, 0, 8, 16
        },
        {
            260 + 40 + 24, 0, 8, 16
        }
    },
};

movie_prog_uipara_t *movie_prog_get_uipara(__s32 rotate)
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

