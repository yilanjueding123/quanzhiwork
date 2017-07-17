/*
*********************************************************************************************************
*											        eMOD
*						           the Easy Portable/Player Operation System
*									           mod_mmp sub-system
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : mod_orange_i.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#include "mod_orange_i.h"


__orange_t   orange;


/*
****************************************************************************************************
*
*             orange_MInit
*
*  Description:
*       ORANGE_MInit
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 ORANGE_MInit(void)
{
    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             ORANGE_MExit
*
*  Description:
*       ORANGE_MExit
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 ORANGE_MExit(void)
{
    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             ORANGE_MOpen
*
*  Description:
*       ORANGE_MOpen
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__mp *ORANGE_MOpen(__u8 mid, __u8 mod)
{
    //ES_FILE * dispfile;

#if(EPDK_ARCH == EPDK_ARCH_SUN)
    //FB                  gui_fb;
    //__disp_layer_para_t   gui_layer;
    //__s32 handle;
#endif
    /* remove  default layer  mcl 08.12.11  */
    // orange.fbaddr[0] = (__u16*)esMEMS_Palloc(1500, MEMS_PALLOC_MODE_BND_1K | MEMS_PALLOC_MODE_BNK_4M);  // default
    // if(orange.fbaddr[0] == 0)
    // {
    //	return (__mp*)0;
    //}
    orange.used = 1;
    orange.mid = mid;
#ifdef SIM_PC_WIN
    g_nModualID = mid;
#endif
    GUI_Init();

#if  0
    //esKRNL_TCreate(_ORANGE_Exec, 0, 400, KRNL_priolevel3);
#if(EPDK_ARCH == EPDK_ARCH_SUN)
    /*set normal layer framebuffer */
    gui_fb.fmt.type = FB_TYPE_RGB;
    //gui_fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_RGB565;
    gui_fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
    gui_fb.fmt.fmt.rgb.br_swap = RGB_PS;
    gui_fb.fmt.fmt.rgb.pixseq = 0;
    gui_fb.fmt.fmt.rgb.palette.size = 0;
    gui_fb.fmt.fmt.rgb.palette.addr = NULL;
    gui_fb.size.width = 800;
    gui_fb.size.height = 480;
    gui_fb.addr[0] = (void *)orange.fbaddr[0];

    /*set normal layer information*/
    gui_layer.mode = DISP_LAYER_WORK_MODE_NORMAL;
    gui_layer.channel = DISP_LAYER_OUTPUT_CHN_DE_CH1;
    gui_layer.fb = &gui_fb;
    gui_layer.scn_win.x = 0;
    gui_layer.scn_win.y = 0;
    gui_layer.scn_win.width = 800;
    gui_layer.scn_win.height = 480;
    gui_layer.src_win.x = 0;
    gui_layer.src_win.y = 0;
    gui_layer.src_win.width = 800;
    gui_layer.src_win.height = 480;
    gui_layer.alpha_en = 0;
    gui_layer.alpha_val = 255;
    gui_layer.ck_mode = 0;
    gui_layer.pipe = 0;
    gui_layer.prio = 0xff;
    dispfile = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

    if(!dispfile)
    {
        __err("open display fail!\n");
    }
    handle = eLIBs_fioctrl(dispfile, DISP_CMD_LAYER_REQUEST, 0, &gui_layer);

    if(handle < EPDK_OK)
    {
        __inf("orange.mod request layer fail!\n");
    }

    if(eLIBs_fioctrl(dispfile, DISP_CMD_LAYER_OPEN, handle, NULL) != EPDK_OK)
    {
        __inf("orange.mod open layer fail!\n");
    }
#endif
#endif
    return (__mp *)&orange;
}
/*
****************************************************************************************************
*
*             ORANGE_MClose
*
*  Description:
*       ORANGE_MClose
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 ORANGE_MClose(__mp *mp)
{
    GUI_Exit();
    esMEMS_Pfree(((__orange_t *)mp)->fbaddr[0], 255);
    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             ORANGE_MRead
*
*  Description:
*       ORANGE_MRead
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__u32 ORANGE_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}
/*
****************************************************************************************************
*
*             ORANGE_MWrite
*
*  Description:
*       ORANGE_MWrite
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__u32 ORANGE_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    __msg("md_mmp->I am MWrite function!\n");
    return size * n;
}
/*
****************************************************************************************************
*
*             ORANGE_MIoctrl
*
*  Description:
*       ORANGE_MIoctrl
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 ORANGE_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    switch(cmd)
    {
    case ORANGE_CMD_GET_FBADDR:
        if(aux == 0)					/* 图层号，0为图层0									  */
            return (__s32)orange.fbaddr[0];
        else							/* 目前由于只支持一个硬件图层，所以其他的值返回NULL   */
            return	NULL;
    case ORANGE_CMD_SET_KEYID:
        //	drv_key = esMODS_MOpen(aux, 0);
        break;
    default:
        break;
    }
    return EPDK_OK;
}
