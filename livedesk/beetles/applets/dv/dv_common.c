/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           home app sample
*
*						        	(c) Copyright 2006-2009, China
*											 All Rights Reserved
*
* File    : home.c
* By      : lyn
* Version : V1.00
*********************************************************************************************************
*/

#include "dv_i.h"
#include "dv_uipara.h"
#include "dv_common.h"


H_LYR dv_layer_create(RECT *rect, __disp_layer_work_mode_t mode)
{
    H_LYR layer = NULL;

    FB  fb =
    {
        {0, 0},                                   		/* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
    };

    __disp_layer_para_t lstlyr =
    {
        DISP_LAYER_WORK_MODE_PALETTE,                  	/* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        0xff,                                           /* prio      */
        {0, 0, 0, 0},                           		/* screen    */
        {0, 0, 0, 0},                               	/* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };

    __layerwincreate_para_t lyrcreate_info =
    {
        "layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };

    if( mode == DISP_LAYER_WORK_MODE_NORMAL )
    {
        fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888 ;
        lstlyr.mode = DISP_LAYER_WORK_MODE_NORMAL;
    }

    fb.size.width		= rect->width;
    fb.size.height		= rect->height;

    lstlyr.src_win.x  		= 0;
    lstlyr.src_win.y  		= 0;
    lstlyr.src_win.width 	= rect->width;
    lstlyr.src_win.height 	= rect->height;

    lstlyr.scn_win.x		= rect->x;
    lstlyr.scn_win.y		= rect->y;
    lstlyr.scn_win.width  	= rect->width;
    lstlyr.scn_win.height 	= rect->height;

    lstlyr.pipe = 1;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;

    layer = GUI_LyrWinCreate(&lyrcreate_info);
    if( !layer )
    {
        __err("layer create fail !\n");
    }

    return layer;
}

void dv_layer_destroy( H_LYR lyr_hdl)
{

    if( !lyr_hdl )
    {
        __wrn("layer handl is null!\n");
        return ;
    }
    GUI_LyrWinDelete(lyr_hdl);
}


/*********************************************************************************************************************************************************
*
*		向父窗口发信息
*
***********************************************************************************************************************************************************/
__s32 Dv_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;

    msg.h_deswin = GUI_WinGetParent(hwin);
    msg.h_srcwin = NULL;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;
    return (GUI_SendNotifyMessage(&msg));
    //return (GUI_SendMessage(&msg));
}


