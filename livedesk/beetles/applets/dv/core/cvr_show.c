/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           home app sample
*
*						        	(c) Copyright 2006-2009, LYN, China
*											 All Rights Reserved
*
* File    : cvr_show.c
* By      : lyn
* Version : V1.00
*********************************************************************************************************
*/

#include "cvr_show.h"

#define __msg(...)          eLIBs_printf(__VA_ARGS__)
#define __inf(...)          eLIBs_printf(__VA_ARGS__)
#define __wrn(...)          eLIBs_printf(__VA_ARGS__)

typedef struct tag_CVR_SHOW
{
    ES_FILE    *g_pdisp;

    __u32       width;
    __u32       height;
    __s32       layer_hdle;
    __disp_fb_t             fb_para;
    __disp_layer_info_t     layer_para;
} __cvr_show_t;

static  __cvr_show_t   *cvr_show = NULL;


static  __s32  cvr_request_layer(void)
{
    __u32       buf[3];

    cvr_show->fb_para.addr[0]     = NULL;
    cvr_show->fb_para.size.width  = cvr_show->width;
    cvr_show->fb_para.size.height = cvr_show->height;
    cvr_show->fb_para.format      = DISP_FORMAT_ARGB8888;
    cvr_show->fb_para.seq         = DISP_SEQ_BGRA;
    cvr_show->fb_para.mode        = DISP_MOD_NON_MB_PLANAR;
    cvr_show->fb_para.br_swap     = 0;
    cvr_show->fb_para.cs_mode     = DISP_BT601;

    cvr_show->layer_para.mode         = DISP_LAYER_WORK_MODE_NORMAL;
    cvr_show->layer_para.pipe         = 0;
    cvr_show->layer_para.prio         = 0xff;
    cvr_show->layer_para.alpha_en     = 0;
    cvr_show->layer_para.alpha_val    = 0xff;
    cvr_show->layer_para.ck_enable    = 0;
    cvr_show->layer_para.src_win.x        = 0;
    cvr_show->layer_para.src_win.y        = 0;
    cvr_show->layer_para.src_win.width    = cvr_show->width;
    cvr_show->layer_para.src_win.height   = cvr_show->height;
    cvr_show->layer_para.scn_win.x        = 0;
    cvr_show->layer_para.scn_win.y        = 0;
    cvr_show->layer_para.scn_win.width    = cvr_show->width;
    cvr_show->layer_para.scn_win.height   = cvr_show->height;
    cvr_show->layer_para.fb               = cvr_show->fb_para;

    buf[0] = DISP_LAYER_WORK_MODE_NORMAL;
    buf[1] = 0;
    buf[2] = 0;
    cvr_show->layer_hdle = eLIBs_fioctrl(cvr_show->g_pdisp, DISP_CMD_LAYER_REQUEST, 0, (void *)buf);
    if(cvr_show->layer_hdle == NULL)
    {
        __wrn("Error in applying for the show layer.\n");
        return EPDK_FAIL;
    }

    buf[0] = cvr_show->layer_hdle;
    buf[1] = (__u32)&cvr_show->layer_para;
    buf[2] = 0;
    eLIBs_fioctrl(cvr_show->g_pdisp, DISP_CMD_LAYER_SET_PARA, 0, (void *)buf);

    return EPDK_OK;
}

__s32   Cvr_ShowOpen(void)
{
    __s32       ret;

    cvr_show = esMEMS_Malloc(0, sizeof(__cvr_show_t));
    if(cvr_show == NULL)
    {
        __wrn("request memery for __cvr_show_t fail\n");
        return NULL;
    }
    eLIBs_memset(cvr_show, 0, sizeof(__cvr_show_t));
    // 打开显示驱动
    cvr_show->g_pdisp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if(cvr_show->g_pdisp == NULL)
    {
        __wrn("can not open display driver\n");
        esMEMS_Mfree(0, cvr_show);
        cvr_show = NULL;

        return EPDK_FAIL;
    }
    // 获取当前显示设备宽高
    cvr_show->width   = eLIBs_fioctrl(cvr_show->g_pdisp, DISP_CMD_SCN_GET_WIDTH, 0, NULL) / 2;
    cvr_show->height  = eLIBs_fioctrl(cvr_show->g_pdisp, DISP_CMD_SCN_GET_HEIGHT , 0, NULL) / 2;
    // 申请显示图层



    ret = cvr_request_layer();
    if(ret == EPDK_FAIL)
    {
        __wrn("request layer for the show fail\n");
        eLIBs_fclose(cvr_show->g_pdisp);
        cvr_show->g_pdisp = NULL;

        esMEMS_Mfree(0, cvr_show);
        cvr_show = NULL;

        return EPDK_FAIL;
    }

    return EPDK_OK;
}

__s32   Cvr_ShowClose(void)
{
    __u32       buf[3];

    if(cvr_show == NULL)
    {
        return EPDK_FAIL;
    }
    if(cvr_show->layer_hdle)
    {
        buf[0] = cvr_show->layer_hdle;
        buf[1] = 0;
        buf[2] = 0;
        eLIBs_fioctrl(cvr_show->g_pdisp, DISP_CMD_LAYER_RELEASE, 0, (void *)buf);

        cvr_show->layer_hdle = NULL;
    }
    if(cvr_show->g_pdisp)
    {
        eLIBs_fclose(cvr_show->g_pdisp);
        cvr_show->g_pdisp = NULL;
    }
    esMEMS_Mfree(0, cvr_show);
    cvr_show = NULL;

    return EPDK_OK;
}

__s32   Cvr_ShowStatus(__s32 enable)
{
    __u32           buf[3];

    if(cvr_show == NULL)
    {
        return EPDK_FAIL;
    }
    if(enable == 1)
    {
        // 打开显示
        buf[0] = cvr_show->layer_hdle;
        buf[1] = 0;
        buf[2] = 0;
        eLIBs_fioctrl(cvr_show->g_pdisp, DISP_CMD_LAYER_BOTTOM, 0, (void *)buf);
        eLIBs_fioctrl(cvr_show->g_pdisp, DISP_CMD_LAYER_OPEN, 0, (void *)buf);
    }
    else
    {
        buf[0] = cvr_show->layer_hdle;
        buf[1] = 0;
        buf[2] = 0;
        eLIBs_fioctrl(cvr_show->g_pdisp, DISP_CMD_LAYER_CLOSE, 0, (void *)buf);
    }

    return EPDK_OK;
}

__s32   Cvr_ShowSetData(void *data, SIZE buf_size)
{
    __u32                   buf[3];
    __disp_layer_info_t     lyr_para;

    if(cvr_show == NULL)
    {
        return EPDK_FAIL;
    }
    buf[0] = cvr_show->layer_hdle;
    buf[1] = (__u32)&lyr_para;
    buf[2] = 0;
    eLIBs_fioctrl(cvr_show->g_pdisp, DISP_CMD_LAYER_GET_PARA, 0, (void *)buf);
    // 设置FB参数
    lyr_para.fb.size.width      = buf_size.width;
    lyr_para.fb.size.height     = buf_size.height;
    lyr_para.fb.addr[0]         = (__u32)data;

    lyr_para.src_win.x          = 0;
    lyr_para.src_win.y          = 0;
    lyr_para.src_win.width      = buf_size.width;
    lyr_para.src_win.height     = buf_size.height;
    lyr_para.scn_win.x          = 0;
    lyr_para.scn_win.y          = 0;
    lyr_para.scn_win.width      = buf_size.width;
    lyr_para.scn_win.height     = buf_size.height;

    buf[0] = cvr_show->layer_hdle;
    buf[1] = (__u32)&lyr_para;
    buf[2] = 0;
    eLIBs_fioctrl(cvr_show->g_pdisp, DISP_CMD_LAYER_SET_PARA, 0, (void *)buf);

    return EPDK_OK;
}

__s32   Cvr_ShowSetMode(show_mode_e mode)
{
    return EPDK_OK;
}

__s32   Cvr_ShowGetSize(SIZE *size)
{
    if(cvr_show == NULL)
    {
        return EPDK_FAIL;
    }
    // 获取当前显示设备宽高
    cvr_show->width   = eLIBs_fioctrl(cvr_show->g_pdisp, DISP_CMD_SCN_GET_WIDTH, 0, NULL);
    cvr_show->height  = eLIBs_fioctrl(cvr_show->g_pdisp, DISP_CMD_SCN_GET_HEIGHT , 0, NULL);

    size->width     = cvr_show->width;
    size->height    = cvr_show->height;

    return EPDK_OK;
}


