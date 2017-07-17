/*
*********************************************************************************************************
*                                                   eMOD
*                                  the Easy Portable/Player Operation System
*                                              mod_willow sub-system
*
*                               (c) Copyright 2006-2009, ALLWINNERME, China
*                                           All Rights Reserved
*
* File   : backlayer_lib.c
* Version: V1.0
* By     : xiechuanrong
*********************************************************************************************************
*/
#include "mod_init_i.h"
#include "backlayer_lib.h"
#include "fb_file.h"

/************************************************************************
* Function: backlayer_create_layer
* Description: ´´½¨±³¾°Í¼²ã
* Input:
* Output:
* Return:
*     > 0: backlayer_handle
*     0: Ê§°Ü
*************************************************************************/
__s32 backlayer_create_layer (ES_FILE *de_hdle)
{
    __s32 layer_hdle;
    __u32 arg[3];

    arg[0] = DISP_LAYER_WORK_MODE_SCALER;
    arg[1] = 0;
    arg[2] = 0;

    layer_hdle = eLIBs_fioctrl(de_hdle, DISP_CMD_LAYER_REQUEST, 0, (void *)arg);


    return layer_hdle;
}


/************************************************************************
* Function: backlayer_delete_layer
* Description: É¾³ý±³¾°Í¼²ã
* Input:
    __u32 layer_handle: ±³¾°Í¼²ã¾ä±ú
* Output:
* Return:
*************************************************************************/
void backlayer_delete_layer (ES_FILE *de_hdle, __u32 layer_handle)
{
    if (layer_handle != 0)
    {
        __u32 arg[3];

        arg[0] = layer_handle;
        arg[1] = 0;
        arg[2] = 0;

        eLIBs_fioctrl(de_hdle, DISP_CMD_LAYER_RELEASE, 0, (void *)arg);
    }

    esKRNL_TimeDly(3);//add by bayden, ÑÓÊ±30ms£¬µÈ´ýÏÔÊ¾Çý¶¯VÖÐ¶Ïµ½´ï

    return;
}

/************************************************************************
* Function: backlayer_set_fb
* Description: ÉèÖÃ±³¾°Í¼²ãframe buffer
* Input:
*     __u32 layer_handle: ±³¾°Í¼²ã¾ä±ú
*     FB *fb: ´ýÉèÖÃµÄ±³¾°frame buffer
* Output:
* Return:
*************************************************************************/
void  backlayer_set_fb (ES_FILE *de_hdle, __u32 layer_handle, FB *fb, __u8 mod)
{
    __u32 lcd_height, lcd_width;
    __disp_layer_info_t layer;
    __disp_fb_t			disp_fb;
    __u32 arg[3];

    lcd_height = eLIBs_fioctrl(de_hdle, DISP_CMD_SCN_GET_HEIGHT, 0, 0);
    lcd_width = eLIBs_fioctrl(de_hdle, DISP_CMD_SCN_GET_WIDTH, 0, 0);

    memset(&layer, 0x00, sizeof(layer));
    layer.pipe = 0;
    if (fb->fmt.type == FB_TYPE_YUV)
    {
        layer.mode = DISP_LAYER_WORK_MODE_SCALER;
    }
    else
    {
        layer.mode = DISP_LAYER_WORK_MODE_NORMAL;
    }
    layer.src_win.x = 0;
    layer.src_win.y = 0;
    layer.src_win.height = fb->size.height;
    layer.src_win.width = fb->size.width;

    if (layer.mode == DISP_LAYER_WORK_MODE_NORMAL)
    {
        layer.scn_win.height = fb->size.height;
        layer.scn_win.width = fb->size.width;
    }
    else
    {
        if (mod == BACKLAYER_MOD_RATIO)
        {
            layer.scn_win.width = lcd_width;
            layer.scn_win.height = fb->size.height * layer.scn_win.width / fb->size.width;

            if(layer.scn_win.height > lcd_height)
            {
                layer.scn_win.height = lcd_height;
                layer.scn_win.width = fb->size.width * layer.scn_win.height / fb->size.height;
            }
        }
        else
        {
            layer.scn_win.width = lcd_width;
            layer.scn_win.height = lcd_height;
        }
    }
    layer.scn_win.x = ((__s32)lcd_width - (__s32)layer.scn_win.width) / 2;
    layer.scn_win.y = ((__s32)lcd_height - (__s32)layer.scn_win.height) / 2;

    GUI_Fb2Dispfb(fb, &disp_fb);
    layer.fb = disp_fb;
    //eLIBs_fioctrl(de_hdle, DISP_CMD_START_CMD_CACHE, 0, (void *)arg);
    arg[0] = layer_handle;
    arg[1] = (__u32)&layer;
    arg[2] = 0;
    eLIBs_fioctrl(de_hdle, DISP_CMD_LAYER_SET_PARA, 0, (void *)arg);
    arg[0] = layer_handle;
    arg[1] = (__u32)&layer.src_win;
    arg[2] = 0;
    eLIBs_fioctrl(de_hdle, DISP_CMD_LAYER_SET_SRC_WINDOW, 0, (void *)arg);
    arg[0] = 0;
    arg[1] = 0;
    arg[2] = 0;
    //eLIBs_fioctrl(de_hdle, DISP_CMD_EXECUTE_CMD_AND_STOP_CACHE, 0, (void *)arg);
    arg[0] = layer_handle;
    arg[1] = 0;
    arg[2] = 0;
    eLIBs_fioctrl(de_hdle, DISP_CMD_LAYER_OPEN, 0, (void *)arg);
}


/************************************************************************
* Function: backlayer_set_top
* Description: ÉèÖÃ±³¾°Í¼²ãÎªtop
* Input:
*     __u32 layer_handle: ±³¾°Í¼²ã¾ä±ú
* Output:
* Return:
*************************************************************************/
void  backlayer_set_top (ES_FILE *de_hdle, __u32 layer_handle)
{
    __u32 arg[3];

    arg[0] = layer_handle;
    arg[1] = 0;
    arg[2] = 0;

    eLIBs_fioctrl(de_hdle, DISP_CMD_LAYER_TOP, 0, (void *)arg);
}


/************************************************************************
* Function: backlayer_set_bottom
* Description: ÉèÖÃ±³¾°Í¼²ãÎªbottom
* Input:
*     __u32 layer_handle: ±³¾°Í¼²ã¾ä±ú
* Output:
* Return:
*************************************************************************/
void  backlayer_set_bottom (ES_FILE *de_hdle, __u32 layer_handle)
{
    __u32 arg[3];

    arg[0] = layer_handle;
    arg[1] = 0;
    arg[2] = 0;

    eLIBs_fioctrl(de_hdle, DISP_CMD_LAYER_BOTTOM, 0, (void *)arg);
}


