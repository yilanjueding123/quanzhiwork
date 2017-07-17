/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : robin_video_layer.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.28
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.09.28       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_video_layer_c
#define  __robin_video_layer_c


#include "..\\robin_i.h"


/***************************************************************************************************
*Name        : robin_request_video_layer
*Prototype   : __s32  robin_request_video_layer( const RECT *rect_p, __s32 pipe, __s32 prio )
*Arguments   : rect_p    input. the coordinate and size of the video layer.
*              pipe      input. layer pipe.
*              prio      input. layer priority.
*Return      : the video layer handle. if return NULL, means failing.
*Description : request the video layer.
*Other       :
***************************************************************************************************/
__hdle  robin_request_video_layer( const RECT *rect_p, __s32 pipe, __s32 prio )
{
    //__disp_layer_para_t     image_layer_para;
    //FB                      image_fb_para;
    __disp_layer_info_t     image_layer_info;
    __disp_fb_t             image_fb_para;
    RECT                    image_win;
    __hdle                  hlay = NULL;
    __u32 arg[3];

    if( rect_p == NULL )
        return NULL;

    /* frame buffer attribute is ignored here, that is set by Cedar */
    /*image_fb_para.size.height      	       = 0;                   // DONT'T CARE
    image_fb_para.size.width       	       = 0;                   // DONT'T CARE
    image_fb_para.addr[0]          	       = NULL;                // DONT'T CARE
    image_fb_para.fmt.type         	       = FB_TYPE_RGB;         // DONT'T CARE
    image_fb_para.fmt.fmt.rgb.pixelfmt     = PIXEL_COLOR_RGB565;  // DONT'T CARE
    image_fb_para.fmt.fmt.rgb.br_swap      = 0;                   // DONT'T CARE
    image_fb_para.fmt.fmt.rgb.pixseq       = 0x00;                // DONT'T CARE
    image_fb_para.fmt.fmt.rgb.palette.addr = NULL;
    image_fb_para.fmt.fmt.rgb.palette.size = 0;

    image_layer_para.mode           = DISP_LAYER_WORK_MODE_NORMAL;
    image_layer_para.channel        = DISP_LAYER_OUTPUT_CHN_DE_CH1;
    image_layer_para.ck_mode        = 0;
    image_layer_para.alpha_en       = 0;
    image_layer_para.alpha_val      = 255;
    image_layer_para.pipe           = pipe;          //set pipe 0
    image_layer_para.prio           = prio;
    image_layer_para.scn_win.x      = rect_p->x     ;
    image_layer_para.scn_win.y      = rect_p->y     ;
    image_layer_para.scn_win.width  = rect_p->width ;
    image_layer_para.scn_win.height = rect_p->height;
    image_layer_para.src_win.x      = 0;
    image_layer_para.src_win.y      = 0;
    image_layer_para.src_win.width  = rect_p->width ;
    image_layer_para.src_win.height = rect_p->height;
    image_layer_para.fb             = &image_fb_para;*/

    image_fb_para.size.height      	= 0;                   // DONT'T CARE
    image_fb_para.size.width       	= 0;                   // DONT'T CARE
    image_fb_para.addr[0]          	= NULL;
    image_fb_para.format         	= DISP_FORMAT_RGB565;         // DONT'T CARE
    image_fb_para.seq     			= DISP_SEQ_ARGB;  	  // DONT'T CARE
    image_fb_para.mode      		= 0;                   // DONT'T CARE
    image_fb_para.br_swap       	= 0;                // DONT'T CARE
    image_fb_para.cs_mode 			= NULL;

    image_layer_info.mode			= DISP_LAYER_WORK_MODE_NORMAL;
    image_layer_info.pipe			= pipe;
    image_layer_info.prio			= prio;
    image_layer_info.alpha_en		= 0;
    image_layer_info.alpha_val		= 255;
    image_layer_info.ck_enable		= 0;
    image_layer_info.src_win.x      = 0;
    image_layer_info.src_win.y      = 0;
    image_layer_info.src_win.width  = rect_p->width ;
    image_layer_info.src_win.height = rect_p->height;
    image_layer_info.scn_win.x      = rect_p->x     ;
    image_layer_info.scn_win.y      = rect_p->y     ;
    image_layer_info.scn_win.width  = rect_p->width ;
    image_layer_info.scn_win.height = rect_p->height;
    image_layer_info.fb				= image_fb_para;

    arg[0] = DISP_LAYER_WORK_MODE_NORMAL;
    arg[1] = 0;
    arg[2] = 0;
    hlay = g_fioctrl( robin_hdis, DISP_CMD_LAYER_REQUEST, 0, (void *)arg );

    if( hlay == NULL )
    {
        __err("Error in applying for the video layer.\n");
        goto error;
    }

    arg[0] = hlay;
    arg[1] = (__u32)&image_layer_info;
    arg[2] = 0;
    g_fioctrl( robin_hdis, DISP_CMD_LAYER_SET_PARA, 0, (void *)arg );

    image_win.x      = rect_p->x;
    image_win.y      = rect_p->y;
    image_win.width  = rect_p->width ;
    image_win.height = rect_p->height;

    if(esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_VID_LAYERHDL, 0, (void *)hlay) != EPDK_OK)
    {
        __wrn("Fail in setting video layer handle to cedar!\n");
        goto error;
    }
    //set video window information to cedar
    if(esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_VID_WINDOW, 0, &image_win) != EPDK_OK)
    {
        __wrn("Fail in setting video window information to cedar!\n");
        goto error;
    }

    return hlay;

error:
    if( hlay != NULL )
    {
        arg[0] = hlay;
        arg[1] = 0;
        arg[2] = 0;
        g_fioctrl( robin_hdis, DISP_CMD_LAYER_RELEASE, 0, (void *)arg );
        hlay = NULL;
    }
    return NULL;
}



/***************************************************************************************************
*Name        : robin_release_video_layer
*Prototype   : void   robin_release_video_layer( __hdle hlay_video )
*Arguments   : hlay_video     input. the video layer handle, gotten by call robin_request_video_layer().
*Return      : void
*Description : release the video layer.
*Other       :
***************************************************************************************************/
void   robin_release_video_layer( __hdle hlay_video )
{
    if( hlay_video != NULL )
    {
        __u32 arg[3];
        arg[0] = hlay_video;
        arg[1] = 0;
        arg[2] = 0;
        g_fioctrl( robin_hdis, DISP_CMD_LAYER_RELEASE, 0, (void *)arg );
        hlay_video = NULL;
    }
}



#endif     //  ifndef __robin_video_layer_c

/* end of robin_video_layer.c */
