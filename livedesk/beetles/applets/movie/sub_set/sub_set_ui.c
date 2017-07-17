
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
#include "sub_set.h"
#include "sub_set_ui.h"

static movie_sub_set_uipara_t uipara_400_240 =
{
    1,
    {
        210, 32, 157, 88
    },
    {
        0, 0, 157, 88
    },
    0,
    {
        0, 0, 157, 88,
        ID_MOVIE_SUB_SET_BG_BMP
    },
    {

        {
            {
                {
                    56, 5, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_LBTN_F_BMP
                },
                {
                    136, 5, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_RBTN_F_BMP
                }
            },
            {
                {
                    65, 5, 70, 16
                }
            }
        },
        {
            {
                {
                    56, 23, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_LBTN_F_BMP
                },
                {
                    136, 23, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_RBTN_F_BMP
                }
            },
            {
                {
                    65, 23, 70, 16
                }
            }
        },
        {

            {
                {
                    56, 41, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_LBTN_F_BMP
                },
                {
                    136, 41, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_RBTN_F_BMP
                }
            },
            {
                {
                    65, 41, 70, 16
                }
            }
        },
        {

            {
                {
                    56, 59, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_LBTN_F_BMP
                },
                {
                    136, 59, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_RBTN_F_BMP
                }
            },
            {
                {
                    65, 59, 70, 16
                }
            }
        }
    },
    {
        {
            0, 5, 56, 16,
            {0, 0},
            {0, 0},
            STRING_MOVIE_SUB_STATE
        },
        {
            0, 23, 56, 16,
            {0, 0},
            {0, 0},
            STRING_MOVIE_SUB_COLOR
        },
        {
            0, 41, 56, 16,
            {0, 0},
            {0, 0},
            STRING_MOVIE_SUB_POS
        },
        {
            0, 59, 56, 16,
            {0, 0},
            {0, 0},
            STRING_MOVIE_SUB_STREAM
        }
    },
    {
        {
            STRING_MOVIE_SUB_OFF,
            STRING_MOVIE_SUB_ON
        },
        {
            STRING_MOVIE_SUB_WHITE,
            STRING_MOVIE_SUB_RED,
            STRING_MOVIE_SUB_BLUE,
        },
        {
            STRING_MOVIE_SUB_BOTTOM,
            STRING_MOVIE_SUB_MID,
            STRING_MOVIE_SUB_TOP
        },
        {
            STRING_MOVIE_SUB_1,
            STRING_MOVIE_SUB_2
        }
    }
};

static movie_sub_set_uipara_t uipara_720_576 =
{
    1,
    {
        400, 52, 157, 88
    },
    {
        0, 0, 157, 88
    },
    0,
    {
        0, 0, 157, 88,
        ID_MOVIE_SUB_SET_BG_BMP
    },
    {

        {
            {
                {
                    56, 5, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_LBTN_F_BMP
                },
                {
                    136, 5, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_RBTN_F_BMP
                }
            },
            {
                {
                    65, 5, 70, 16
                }
            }
        },
        {
            {
                {
                    56, 23, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_LBTN_F_BMP
                },
                {
                    136, 23, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_RBTN_F_BMP
                }
            },
            {
                {
                    65, 23, 70, 16
                }
            }
        },
        {

            {
                {
                    56, 41, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_LBTN_F_BMP
                },
                {
                    136, 41, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_RBTN_F_BMP
                }
            },
            {
                {
                    65, 41, 70, 16
                }
            }
        },
        {

            {
                {
                    56, 59, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_LBTN_F_BMP
                },
                {
                    136, 59, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_RBTN_F_BMP
                }
            },
            {
                {
                    65, 59, 70, 16
                }
            }
        }
    },
    {
        {
            0, 5, 56, 16,
            {0, 0},
            {0, 0},
            STRING_MOVIE_SUB_STATE
        },
        {
            0, 23, 56, 16,
            {0, 0},
            {0, 0},
            STRING_MOVIE_SUB_COLOR
        },
        {
            0, 41, 56, 16,
            {0, 0},
            {0, 0},
            STRING_MOVIE_SUB_POS
        },
        {
            0, 59, 56, 16,
            {0, 0},
            {0, 0},
            STRING_MOVIE_SUB_STREAM
        }
    },
    {
        {
            STRING_MOVIE_SUB_OFF,
            STRING_MOVIE_SUB_ON
        },
        {
            STRING_MOVIE_SUB_WHITE,
            STRING_MOVIE_SUB_RED,
            STRING_MOVIE_SUB_BLUE,
        },
        {
            STRING_MOVIE_SUB_BOTTOM,
            STRING_MOVIE_SUB_MID,
            STRING_MOVIE_SUB_TOP
        },
        {
            STRING_MOVIE_SUB_1,
            STRING_MOVIE_SUB_2
        }
    }
};


static movie_sub_set_uipara_t uipara_720_480 =
{
    1,
    {
        400, 52, 157, 88
    },
    {
        0, 0, 157, 88
    },
    0,
    {
        0, 0, 157, 88,
        ID_MOVIE_SUB_SET_BG_BMP
    },
    {

        {
            {
                {
                    56, 5, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_LBTN_F_BMP
                },
                {
                    136, 5, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_RBTN_F_BMP
                }
            },
            {
                {
                    65, 5, 70, 16
                }
            }
        },
        {
            {
                {
                    56, 23, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_LBTN_F_BMP
                },
                {
                    136, 23, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_RBTN_F_BMP
                }
            },
            {
                {
                    65, 23, 70, 16
                }
            }
        },
        {

            {
                {
                    56, 41, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_LBTN_F_BMP
                },
                {
                    136, 41, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_RBTN_F_BMP
                }
            },
            {
                {
                    65, 41, 70, 16
                }
            }
        },
        {

            {
                {
                    56, 59, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_LBTN_F_BMP
                },
                {
                    136, 59, 9, 13,
                    0,
                    ID_MOVIE_SUB_SET_RBTN_F_BMP
                }
            },
            {
                {
                    65, 59, 70, 16
                }
            }
        }
    },
    {
        {
            0, 5, 56, 16,
            {0, 0},
            {0, 0},
            STRING_MOVIE_SUB_STATE
        },
        {
            0, 23, 56, 16,
            {0, 0},
            {0, 0},
            STRING_MOVIE_SUB_COLOR
        },
        {
            0, 41, 56, 16,
            {0, 0},
            {0, 0},
            STRING_MOVIE_SUB_POS
        },
        {
            0, 59, 56, 16,
            {0, 0},
            {0, 0},
            STRING_MOVIE_SUB_STREAM
        }
    },
    {
        {
            STRING_MOVIE_SUB_OFF,
            STRING_MOVIE_SUB_ON
        },
        {
            STRING_MOVIE_SUB_WHITE,
            STRING_MOVIE_SUB_RED,
            STRING_MOVIE_SUB_BLUE,
        },
        {
            STRING_MOVIE_SUB_BOTTOM,
            STRING_MOVIE_SUB_MID,
            STRING_MOVIE_SUB_TOP
        },
        {
            STRING_MOVIE_SUB_1,
            STRING_MOVIE_SUB_2
        }
    }
};

movie_sub_set_uipara_t *movie_sub_set_get_uipara(__s32 rotate)
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

