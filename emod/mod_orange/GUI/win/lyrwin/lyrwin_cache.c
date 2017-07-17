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
* File    : lyrwin_cache.c
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:24:32
**********************************************************************************************************************
*/

#include "lyrwin_i.h"

/*
**********************************************************************************************************************
*                                               GUI_LyrWinCacheOn
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
__s32  GUI_LyrWinCacheOn(void)
{
    ES_FILE       *p_disp;

    /*open display driver handle*/
    p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if(!p_disp)
    {
        __err("open display device fail!\n");
    }
    /*start hardware cache layer*/
    eLIBs_fioctrl(p_disp, DISP_CMD_START_CMD_CACHE, SEL_SCREEN, 0);
    eLIBs_fclose(p_disp);
    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                               GUI_LyrWinCacheOff
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
__s32  GUI_LyrWinCacheOff(void)
{
    ES_FILE       *p_disp;

    /*open display driver handle*/
    p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if(!p_disp)
    {
        __err("open display device fail!\n");
    }
    /*request hardware layer*/
    eLIBs_fioctrl(p_disp, DISP_CMD_EXECUTE_CMD_AND_STOP_CACHE, SEL_SCREEN, 0);
    eLIBs_fclose(p_disp);
#ifdef SIM_PC_WIN
    RefreshDeContent(0);
#endif
    return EPDK_OK;
}
