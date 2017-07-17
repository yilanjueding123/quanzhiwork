/*
*******************************************************************************************************************
*                                                         	Mod orange
*                                         			the embedded graphic support module
*
*                             				 Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*											               All Rights Reserved
*
*File Name£º 		gui_fb_dispfb_conv.h
*
*Author£º 		    Derek Lai
*
*Version :   		1.1.0
*
*Date:  			2010-12-8 16:25:07
*
*Description : 	    fb convert to dispfb, and dispfb convert to fb.
*
*Others : 		    None at present.
*
* History :
*
* <Author>          <time>        <version>     <description>
*
* Derek Lai     2010-12-8 16:27:10         1.1.0          Create File
*
*******************************************************************************************************************
*/
#include "gui_core_kernel_private.h"

//#include "..\\core_i.h"
//#include "gui_core_kernel_i.h"

/*
**********************************************************************************************************************
*                                               GUI_Fb2Dispfb
*
* author:
*
* date:             2010-12-8 16:29:59
*
* Description:      struct fb to struct dispfb
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_Fb2Dispfb(FB *fb, __disp_fb_t *dispfb)
{
    if((!fb) || (!dispfb))
    {
        __wrn("input invalid!");
        return EPDK_FAIL;
    }

    dispfb->addr[0] = (__u32)(fb->addr[0]);
    dispfb->addr[1] = (__u32)(fb->addr[1]);
    dispfb->addr[2] = (__u32)(fb->addr[2]);
    //dispfb->size = (__disp_rectsz_t)fb->size;
    dispfb->size.width = fb->size.width;
    dispfb->size.height = fb->size.height;
    dispfb->cs_mode = fb->fmt.cs_mode;
    if(fb->fmt.type == 0)
    {
        /*convert pixelfmt*/
        dispfb->format = fb->fmt.fmt.rgb.pixelfmt;

        /*convert seq*/
        if((0x0 == fb->fmt.fmt.rgb.pixseq) || (fb->fmt.fmt.rgb.pixseq == 0x2))
        {
            dispfb->seq = fb->fmt.fmt.rgb.pixseq;
        }
        else if((0x10 <= fb->fmt.fmt.rgb.pixseq) && (fb->fmt.fmt.rgb.pixseq <= 0x11))
        {
            dispfb->seq = fb->fmt.fmt.rgb.pixseq - 3;
        }
        else if((0x20 <= fb->fmt.fmt.rgb.pixseq) && (fb->fmt.fmt.rgb.pixseq <= 0x21))
        {
            dispfb->seq = fb->fmt.fmt.rgb.pixseq - 0x11;
        }
        else if((0x30 <= fb->fmt.fmt.rgb.pixseq) && (fb->fmt.fmt.rgb.pixseq <= 0x33))
        {
            dispfb->seq = fb->fmt.fmt.rgb.pixseq - 0x1F;
        }
        else if((0x40 <= fb->fmt.fmt.rgb.pixseq) && (fb->fmt.fmt.rgb.pixseq <= 0x43))
        {
            dispfb->seq = fb->fmt.fmt.rgb.pixseq - 0x2B;
        }
        else if((0x50 <= fb->fmt.fmt.rgb.pixseq) && (fb->fmt.fmt.rgb.pixseq <= 0x53))
        {
            dispfb->seq = fb->fmt.fmt.rgb.pixseq - 0x37;
        }
        else
        {
            __wrn("fb rgb seq is invalid!");
        }

        //dispfb->seq = fb->fmt.fmt.rgb.pixseq;
        /*convert mode*/
        dispfb->mode = DISP_MOD_INTERLEAVED;

        /*convert br_swap*/
        dispfb->br_swap = fb->fmt.fmt.rgb.br_swap;
    }
    else if(fb->fmt.type == 1)
    {
        /*convert pixelfmt*/
        if(fb->fmt.fmt.yuv.pixelfmt == PIXEL_OTHERFMT)
        {
            __wrn("fb pixelfmt is invalid!");
            return EPDK_FAIL;
        }
        else
        {
            dispfb->format = fb->fmt.fmt.yuv.pixelfmt - 5;
        }

        /*convert seq*/
        if((0x0 <= fb->fmt.fmt.yuv.yuvseq) && (fb->fmt.fmt.yuv.yuvseq <= 0x3))
        {
            dispfb->seq = fb->fmt.fmt.yuv.yuvseq + 3;
        }
        else if((0x10 <= fb->fmt.fmt.yuv.yuvseq) && (fb->fmt.fmt.yuv.yuvseq <= 0x11))
        {
            dispfb->seq = fb->fmt.fmt.yuv.yuvseq - 9;
        }
        else if((0x20 <= fb->fmt.fmt.yuv.yuvseq) && (fb->fmt.fmt.yuv.yuvseq <= 0x21))
        {
            dispfb->seq = fb->fmt.fmt.yuv.yuvseq - 0x17;
        }
        else if(fb->fmt.fmt.yuv.yuvseq == 0xff)
        {
            dispfb->seq = DISP_SEQ_P3210;
        }
        else
        {
            __wrn("fb yuv seq is invalid!");
            return EPDK_FAIL;
        }
        //dispfb->seq = fb->fmt.fmt.yuv.yuvseq;

        /*convert mode*/
        if(fb->fmt.fmt.yuv.mod == YUV_MOD_INTERLEAVED)
        {
            dispfb->mode = DISP_MOD_INTERLEAVED;
        }
        else if(fb->fmt.fmt.yuv.mod == YUV_MOD_NON_MB_PLANAR)
        {
            dispfb->mode = DISP_MOD_NON_MB_PLANAR;
        }
        else if(fb->fmt.fmt.yuv.mod == YUV_MOD_MB_PLANAR)
        {
            dispfb->mode = DISP_MOD_MB_PLANAR;
        }
        else if(fb->fmt.fmt.yuv.mod == YUV_MOD_UV_NON_MB_COMBINED)
        {
            dispfb->mode = DISP_MOD_NON_MB_UV_COMBINED;
        }
        else if(fb->fmt.fmt.yuv.mod == YUV_MOD_UV_MB_COMBINED)
        {
            dispfb->mode = DISP_MOD_MB_UV_COMBINED;
        }
        else
        {
            __wrn("yuv mode is invalid!");
            return EPDK_FAIL;
        }
        //dispfb->mode = fb->fmt.fmt.yuv.mod;

        /*convert br_swap*/
        dispfb->br_swap = 0;
    }
    else
    {
        __wrn("fb type is invalid!");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_Dispfb2Fb
*
* author:
*
* date:             2010-12-8 16:29:59
*
* Description:      struct fb to struct dispfb
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_Dispfb2Fb(__disp_fb_t *dispfb, FB *fb)
{
    if((!fb) || (!dispfb))
    {
        __wrn("input invalid!");
        return EPDK_FAIL;
    }

    fb->addr[0] = (void *)(dispfb->addr[0]);
    fb->addr[1] = (void *)(dispfb->addr[1]);
    fb->addr[2] = (void *)(dispfb->addr[2]);
    //fb->size = (SIZE)dispfb->size;
    fb->size.width = dispfb->size.width;
    fb->size.height = dispfb->size.height;
    fb->fmt.cs_mode = dispfb->cs_mode;

    /*convert pixel format*/
    if(dispfb->format <= 0xa)
    {
        fb->fmt.fmt.rgb.pixelfmt = dispfb->format;
        fb->fmt.type = 0;
    }
    else
    {
        fb->fmt.fmt.yuv.pixelfmt = dispfb->format + 5;
        fb->fmt.type = 1;
    }

    /*convert seq*/
    if((0x3 <= dispfb->seq) && (dispfb->seq <= 0x6))
    {
        fb->fmt.fmt.yuv.yuvseq = dispfb->seq - 3;
    }
    else if((0x7 <= dispfb->seq) && (dispfb->seq <= 0x8))
    {
        fb->fmt.fmt.yuv.yuvseq = dispfb->seq + 9;
    }
    else if((0x9 <= dispfb->seq) && (dispfb->seq <= 0xa))
    {
        fb->fmt.fmt.yuv.yuvseq = dispfb->seq + 0x17;
    }
    else if((0xb <= dispfb->seq) && (dispfb->seq <= 0xc))
    {
        fb->fmt.fmt.yuv.yuvseq = YUV_SEQ_OTHRS;
    }
    else if((0x0 == dispfb->seq) || (dispfb->seq == 0x2))
    {
        fb->fmt.fmt.rgb.pixseq = dispfb->seq;
    }
    else if((0xd <= dispfb->seq) && (dispfb->seq >= 0xe))
    {
        fb->fmt.fmt.rgb.pixseq = dispfb->seq + 3;
    }
    else if((0xf <= dispfb->seq) && (dispfb->seq >= 0x10))
    {
        fb->fmt.fmt.rgb.pixseq = dispfb->seq + 0x11;
    }
    else if((0x11 <= dispfb->seq) && (dispfb->seq >= 0x14))
    {
        fb->fmt.fmt.rgb.pixseq = dispfb->seq + 0x1f;
    }
    else if((0x15 <= dispfb->seq) && (dispfb->seq >= 0x18))
    {
        fb->fmt.fmt.rgb.pixseq = dispfb->seq + 0x2b;
    }
    else if((0x19 <= dispfb->seq) && (dispfb->seq >= 0x1c))
    {
        fb->fmt.fmt.rgb.pixseq = dispfb->seq + 0x37;
    }
    else
    {
        __wrn("dispfb seq is invalid!");
        return EPDK_FAIL;
    }

    /*convert mode*/
    if(dispfb->mode == DISP_MOD_INTERLEAVED)
    {
        fb->fmt.fmt.yuv.mod = YUV_MOD_INTERLEAVED;
    }
    else if(dispfb->mode == DISP_MOD_NON_MB_PLANAR)
    {
        fb->fmt.fmt.yuv.mod = YUV_MOD_NON_MB_PLANAR;
    }
    else if(dispfb->mode == DISP_MOD_MB_PLANAR)
    {
        fb->fmt.fmt.yuv.mod = YUV_MOD_MB_PLANAR;
    }
    else if(dispfb->mode == DISP_MOD_NON_MB_UV_COMBINED)
    {
        fb->fmt.fmt.yuv.mod = YUV_MOD_UV_NON_MB_COMBINED;
    }
    else if(dispfb->mode == DISP_MOD_MB_UV_COMBINED)
    {
        fb->fmt.fmt.yuv.mod = YUV_MOD_UV_MB_COMBINED;
    }
    else
    {
        __wrn("dispfb mod is invalid!");
    }

    /*convert br_swap*/
    fb->fmt.fmt.rgb.br_swap = dispfb->br_swap;

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_LogicLyr2DispLyr
*
* author:
*
* date:             2010-12-9 16:37:16
*
* Description:      logic lyare to display layer
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_LogicLyr2DispLyr(__disp_layer_para_t *log_lyr, __disp_layer_info_t *disp_lyr)
{
    if((!log_lyr) || (!disp_lyr))
    {
        __wrn("lyr pointer is invalid!");
        return EPDK_FAIL;
    }

    disp_lyr->alpha_en = log_lyr->alpha_en;
    disp_lyr->alpha_val = log_lyr->alpha_val;
    disp_lyr->mode = log_lyr->mode;
    disp_lyr->pipe = log_lyr->pipe;
    disp_lyr->prio = log_lyr->prio;
    //disp_lyr->scn_win = (__disp_rect_t)log_lyr->scn_win;
    disp_lyr->scn_win.x = log_lyr->scn_win.x;
    disp_lyr->scn_win.y = log_lyr->scn_win.y;
    disp_lyr->scn_win.width = log_lyr->scn_win.width;
    disp_lyr->scn_win.height = log_lyr->scn_win.height;

    //disp_lyr->src_win = (__disp_rect_t)log_lyr->src_win;
    disp_lyr->src_win.x = log_lyr->src_win.x;
    disp_lyr->src_win.y = log_lyr->src_win.y;
    disp_lyr->src_win.width = log_lyr->src_win.width;
    disp_lyr->src_win.height = log_lyr->src_win.height;

    disp_lyr->ck_enable = log_lyr->ck_mode;

    GUI_Fb2Dispfb(log_lyr->fb, &(disp_lyr->fb));

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_DispLyr2LogicLyr
*
* author:
*
* date:             2010-12-9 16:37:16
*
* Description:      logic lyare to display layer
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_DispLyr2LogicLyr(__disp_layer_para_t *log_lyr, __disp_layer_info_t *disp_lyr)
{
    if((!log_lyr) || (!disp_lyr))
    {
        __wrn("lyr pointer is invalid!");
        return EPDK_FAIL;
    }

    log_lyr->alpha_en = disp_lyr->alpha_en;
    log_lyr->alpha_val = disp_lyr->alpha_val;
    log_lyr->mode = disp_lyr->mode;
    log_lyr->pipe = disp_lyr->pipe;
    log_lyr->prio = disp_lyr->prio;
    //log_lyr->scn_win = (RECT)disp_lyr->scn_win;
    log_lyr->scn_win.x = disp_lyr->scn_win.x;
    log_lyr->scn_win.y = disp_lyr->scn_win.y;
    log_lyr->scn_win.width = disp_lyr->scn_win.width;
    log_lyr->scn_win.height = disp_lyr->scn_win.height;

    //log_lyr->src_win = (RECT)disp_lyr->src_win;
    log_lyr->src_win.x = disp_lyr->src_win.x;
    log_lyr->src_win.y = disp_lyr->src_win.y;
    log_lyr->src_win.width = disp_lyr->src_win.width;
    log_lyr->src_win.height = disp_lyr->src_win.height;

    log_lyr->ck_mode = disp_lyr->ck_enable;

    GUI_Dispfb2Fb(&(disp_lyr->fb), log_lyr->fb);

    log_lyr->channel = DISP_LAYER_OUTPUT_CHN_DE_CH1;

    return EPDK_OK;
}