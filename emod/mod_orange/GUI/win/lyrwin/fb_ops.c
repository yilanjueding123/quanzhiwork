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
* File    : fb_ops.c
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:24:32
**********************************************************************************************************************
*/

#include "lyrwin_i.h"
#include "fb_ops.h"

/*
**********************************************************************************************************************
*                                               GUI_FBSuspend
*
* Description: sleep fb, if a fb is free, its frame buffer will be free.
*
* Arguments  :  fb       frame buffer struct
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
static __u8  GUI_FBGetBPP(FB *fb)
{
    __u8  bpp;

    switch(fb->fmt.fmt.rgb.pixelfmt)
    {
    case  PIXEL_MONO_1BPP:
        bpp = 1;
        break;

    case PIXEL_MONO_2BPP:
        bpp = 2;
        break;

    case PIXEL_MONO_4BPP:
        bpp = 4;
        break;

    case PIXEL_MONO_8BPP:
        bpp = 8;
        break;

    case PIXEL_COLOR_RGB655:
    case PIXEL_COLOR_RGB565:
    case PIXEL_COLOR_RGB556:
    case PIXEL_COLOR_ARGB1555:
    case PIXEL_COLOR_RGBA5551:
        bpp = 16;
        break;

    case PIXEL_COLOR_RGB0888:
        bpp = 32;
        break;

    case PIXEL_COLOR_ARGB8888:
        bpp = 32;
        break;

    default:
        bpp = 0;
        break;
    }

    return bpp;

}
/*
**********************************************************************************************************************
*                                               GUI_FBSuspend
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
__s32 GUI_FBSuspend(FB *fb)
{
    __u8     bpp = 0;
    __u32    size = 0;
#if GUI_CHECK_ARG_EN > 0
    if(!fb)
    {
        __wrn("frame buffer is empty!\n");
        return EPDK_FAIL;
    }
#endif

    if(fb->fmt.type == FB_TYPE_RGB)
    {
        bpp = GUI_FBGetBPP(fb);
        if(bpp)
        {
            /*framebuffer has been released or not been alloced*/
            if(fb->addr[0] != NULL)
            {
                /*get the size of framebuffer*/
                size = (fb->size.width * fb->size.height * bpp + 7) >> 3;
                esMEMS_Pfree(fb->addr[0], (size + 1023) >> 10);
                fb->addr[0] = NULL;
            }
        }
        else
        {
            __wrn("frame buffer format error!\n");
            return EPDK_FAIL;
        }
    }
    else  /*not allow free yuv format framebuffer*/
    {
        __wrn("framebuffer format error!\n");
        return EPDK_FAIL;
    }
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_FBResume
*
* Description: wakeup a fb, the frame buffer information is missing, we need redraw all the information in the fb.
*
* Arguments  : fb       frame buffer struct
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 GUI_FBResume(FB *fb)
{
    __u8         bpp;
    __u32        size = 0;
    __u32        ret;
    ES_FILE     *gdispfile;
    __u32 		pdispbuffer[3]; //add by Derek,2010.12.07.15:05

#if GUI_CHECK_ARG_EN > 0
    if(!fb)
    {
        __wrn("frame buffer is empty!\n");
        return EPDK_FAIL;
    }
#endif
    if(fb->fmt.type == FB_TYPE_RGB)
    {
        bpp = GUI_FBGetBPP(fb);
        if(bpp)
        {
            /*framebuffer has been released or not been alloced*/
            if(fb->addr[0] == NULL)
            {
                /*get the size of framebuffer*/
                size = (fb->size.width * fb->size.height * bpp + 7) >> 3;

                fb->addr[0] = esMEMS_Palloc((size + 1023) >> 10, 0);
                if(!(fb->addr[0]))
                {
                    while(1)
                    {
                        __inf("errhere!\n");
                        if(__getc() == 49)
                        {
                            esMEMS_Palloc((size + 1023) >> 10, 0);
                            break;
                        }
                    }
                }
                __inf("framebuffer = %x,size = %d\n", fb->addr[0], size);
                eLIBs_memclr(fb->addr[0], size);
                if((fb->fmt.fmt.rgb.palette.addr != NULL) && (fb->fmt.fmt.rgb.palette.size != 0))
                {
                    gdispfile = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
                    if(!gdispfile)
                    {
                        __wrn("open display fail!\n");
                        goto error;
                    }

                    /*modified by Derek,2010.12.07.15:05*/
                    pdispbuffer[0] = (__u32)fb->fmt.fmt.rgb.palette.addr;
                    pdispbuffer[1] = 0;
                    pdispbuffer[2] = fb->fmt.fmt.rgb.palette.size;

                    /*set display system palette*/
                    ret = eLIBs_fioctrl(gdispfile,
                                        DISP_CMD_SET_PALETTE_TBL,
                                        SEL_SCREEN,
                                        (void *)pdispbuffer); //end modified
                    if(ret != EPDK_OK)
                    {
                        __wrn("set palette failed\n");
                        goto error;
                    }

                    eLIBs_fclose(gdispfile);
                }
            }
        }
        else
        {
            __wrn("frame buffer format error!\n");
            goto error;
        }
    }
    else
    {
        __wrn("framebuffer format error!\n");

    }

    return EPDK_OK;

error:
    if(fb->addr[0])
    {
        esMEMS_Pfree(fb->addr[0], (size + 1023) >> 10);
    }

    return EPDK_FAIL;
}
/*
**********************************************************************************************************************
*                                               FUNCTION
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
__s32 GUI_FBFlushCache(FB *fb)
{
    __u8         bpp;
    __u32        size = 0;

#if GUI_CHECK_ARG_EN > 0
    if(!fb)
    {
        __wrn("frame buffer is empty!\n");
        return GUI_FB_STA_ERR;
    }
#endif
    if(fb->fmt.type == FB_TYPE_RGB)
    {
        bpp = GUI_FBGetBPP(fb);
        if(bpp)
        {
            /*framebuffer has been released or not been alloced*/
            if(fb->addr[0] != NULL)
            {
                /*get the size of framebuffer*/
                size = (fb->size.width * fb->size.height * bpp + 7) >> 3;
                esMEMS_CleanFlushDCacheRegion(fb->addr[0], size);
            }
        }
        else
        {
            __wrn("frame buffer format error!\n");
            return EPDK_FAIL;
        }
    }
    else  /*not allow free yuv format framebuffer*/
    {
        __wrn("framebuffer format error!\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                               GUI_FBWakeup
*
* Description: get a fb status, the frame buffer information is missing, we need redraw all the information in the fb.
*
* Arguments  : fb       frame buffer struct
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/

__gui_fb_sta_t GUI_FBGetStatus(FB *fb)
{
#if GUI_CHECK_ARG_EN > 0
    if(!fb)
    {
        __wrn("frame buffer is empty!\n");
        return GUI_FB_STA_ERR;
    }
#endif

    if(fb->addr[0])
    {
        return GUI_FB_STA_WAKE;
    }
    else
    {
        return GUI_FB_STA_SLEEP;
    }
}
