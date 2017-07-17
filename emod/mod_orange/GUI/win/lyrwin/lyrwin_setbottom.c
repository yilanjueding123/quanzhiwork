/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lion
* File    : lyr_setbottom.c
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:24:32
**********************************************************************************************************************
*/

#include "lyrwin_i.h"
/*
**********************************************************************************************************************
*                                               GUI_LyrSetBottom
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 GUI_LyrWinSetBottom(H_LYR h_lyr)
{
    __s32          			 ret = EPDK_FAIL;
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;

    __u32 dispbuffer[3];

#if GUI_CHECK_ARG_EN > 0
    if(!lyr_attr)
    {
        __wrn("h_lyr empty!\n");
        return EPDK_FAIL;
    }
#endif
    GUI_LyrLock();

    if(lyr_attr->h_hwlyr)
    {
        if(lyr_attr->type == GUI_LYRWIN_NORMAL)
        {
            /*modified by Derek,2010-12-9 19:26:03*/
            dispbuffer[0] = lyr_attr->h_hwlyr;
            dispbuffer[1] = 0;
            dispbuffer[2] = 0;
            ret = eLIBs_fioctrl(lyr_attr->devfp, DISP_CMD_LAYER_BOTTOM, SEL_SCREEN, (void *)dispbuffer); //end modified
            if(ret != EPDK_OK)
            {
                __wrn("layer set top failed!\n");
            }
        }
        else
        {
            __wrn("don't support sprite!\n");
        }

        if(lyr_attr->status == GUI_LYRWIN_STA_ON)
        {
            Layer_ListSetBottom(h_lyr);
        }
    }


    GUI_LyrUnlock();

    return ret;
}

