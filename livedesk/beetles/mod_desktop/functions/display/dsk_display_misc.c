/*
************************************************************************************************************************
*                                                     display misc
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*											       All Rights Reserved
*
* File Name   : dsk_display_misc.c
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.09.02
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
* Gary.Wang      2010.09.02        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_DISPLAY_MISC_C__
#define  __DSK_DISPLAY_MISC_C__


#include "dsk_display_i.h"

/*
****************************************************************************************************
*Name        : dsk_display_hdmi_audio_enable
*Prototype   : __s32 dsk_display_hdmi_audio_enable( __bool flag )
*Arguments   : flag       input. true or false.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : enable or disable the audio output through HDMI.
*Other       :
****************************************************************************************************
*/
__s32 dsk_display_hdmi_audio_enable( __bool flag )
{
    ES_FILE	  *p_hdmi;
    __s32          ret;

    /*open display driver handle*/
    p_hdmi = eLIBs_fopen("b:\\BUS\\HDMI", "r+");
    if(!p_hdmi)
    {
        __err("open hdmi device fail!\n");
        return EPDK_FAIL;
    }
    ret = eLIBs_fioctrl(p_hdmi, HDMI_CMD_AUDIO_ENABLE, flag, 0);
    if( ret == EPDK_FAIL )
    {
        __wrn("fail in setting hdmi audio para.!\n");
        eLIBs_fclose(p_hdmi);
        return EPDK_FAIL;
    }

    eLIBs_fclose(p_hdmi);
    return EPDK_OK;
}



/*
****************************************************************************************************
*Name        : dsk_display_get_size
*Prototype   : __s32 dsk_display_get_size(__s32 *p_width, __s32 *p_height)
*Arguments   : p_width     output. width
*              p_height    output. height
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : get the current screen size.
*Other       :
****************************************************************************************************
*/
__s32 dsk_display_get_size(__s32 *p_width, __s32 *p_height)
{
    ES_FILE 			 *p_disp = NULL;

    /* open display driver */
    p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");
    if( p_disp == NULL )
    {
        __err("Error in opening display driver.\n");
        return EPDK_FAIL;
    }

    *p_width  = eLIBs_fioctrl(p_disp, DISP_CMD_SCN_GET_WIDTH, 0, 0);
    *p_height = eLIBs_fioctrl(p_disp, DISP_CMD_SCN_GET_HEIGHT, 0, 0);


    eLIBs_fclose(p_disp);

    return EPDK_OK;
}



/*
****************************************************************************************************
*Name        : dsk_display_set_backcolor
*Prototype   : __s32 dsk_display_set_backcolor(__u32 de_color)
*Arguments   : de_color    input. color format is DE format "ARGB".
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : set the backcolor of the display system.
*Other       :
****************************************************************************************************
*/
__s32 dsk_display_set_backcolor(__u32 de_color)
{
    ES_FILE 			 *p_disp = NULL;
    __u32 arg[3];

    /* open display driver */
    p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");
    if( p_disp == NULL )
    {
        __err("Error in opening display driver.\n");
        return EPDK_FAIL;
    }

    arg[0] = (__u32)&de_color;
    arg[1] = 0;
    arg[2] = 0;
    eLIBs_fioctrl(p_disp, DISP_CMD_SET_BKCOLOR, 0, (void *)arg);

    eLIBs_fclose(p_disp);

    return EPDK_OK;
}



/*
****************************************************************************************************
*Name        : dsk_display_get_backcolor
*Prototype   : __u32 dsk_display_get_backcolor(void)
*Arguments   : void
*Return      : the backcolor of the display system.
*Description : get the backcolor of the display system. color format is DE format "ARGB".
*Other       :
****************************************************************************************************
*/
__u32 dsk_display_get_backcolor(void)
{
    ES_FILE 			 *p_disp = NULL;
    __s32				  color;
    __u32 arg[3];

    /* open display driver */
    p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");
    if( p_disp == NULL )
    {
        __err("Error in opening display driver.\n");
        return 0;
    }

    arg[0] = (__u32)&color;
    arg[1] = 0;
    arg[2] = 0;
    eLIBs_fioctrl(p_disp, DISP_CMD_GET_BKCOLOR, 0, (void *)arg);

    eLIBs_fclose(p_disp);

    return color;
}

#if 1//def VIDEO_SETTING
__s32  dsk_set_contrast(__u32 contrast)
{ 
	__u32       buf[3];
	__s32 ret;

	ES_FILE* g_display_shell = NULL;
	
	g_display_shell  = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
	if( !g_display_shell )
	{
	    __wrn("Open display driver failed, test program exits.\n");
	    return 0;
	}
	ret=eLIBs_fioctrl(g_display_shell, DISP_CMD_ENHANCE_ON, 0, (void *)0);


	buf[0] = contrast%100;
	buf[1] = 0;
	buf[2] = 0;     
   	eLIBs_fioctrl(g_display_shell, DISP_CMD_SET_CONTRAST, 0, (void *)buf);

	__msg("write constract[0x%x],[%d]\n", contrast, contrast);

	ret = eLIBs_fioctrl(g_display_shell, DISP_CMD_GET_CONTRAST, 0, (void *)0);
       __msg(" second read constract[0x%x][%d]\n", ret, ret);

	eLIBs_fclose(g_display_shell);
	
	return ret;
}

__s32  dsk_set_saturation(__u32 saturation)
{ 
	__u32       buf[3];
	__s32 ret;

	ES_FILE* g_display_shell = NULL;
	
	g_display_shell  = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
	if( !g_display_shell )
	{
	    __wrn("Open display driver failed, test program exits.\n");
	    return 0;
	}
	ret=eLIBs_fioctrl(g_display_shell, DISP_CMD_ENHANCE_ON, 0, (void *)0);

	   
	buf[0] = saturation%100;
	buf[1] = 0;
	buf[2] = 0;     
   	eLIBs_fioctrl(g_display_shell, DISP_CMD_SET_SATURATION, 0, (void *)buf);

	__msg("write saturation[0x%x],[%d]\n", saturation, saturation);

	ret = eLIBs_fioctrl(g_display_shell, DISP_CMD_GET_SATURATION, 0, (void *)0);
       __msg(" second read saturation[0x%x][%d]\n", ret, ret);

	eLIBs_fclose(g_display_shell);
	
	return ret;
}

__s32  dsk_set_bright(__u32 bright)
{ 
	__u32       buf[3];
	__s32 ret;

	ES_FILE* g_display_shell = NULL;
	
	g_display_shell  = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
	if( !g_display_shell )
	{
	    __wrn("Open display driver failed, test program exits.\n");
	    return 0;
	}
	ret=eLIBs_fioctrl(g_display_shell, DISP_CMD_ENHANCE_ON, 0, (void *)0);

	   
	buf[0] = bright%100;
	buf[1] = 0;
	buf[2] = 0;     
   	eLIBs_fioctrl(g_display_shell, DISP_CMD_SET_BRIGHT, 0, (void *)buf);

	__msg("write bright[0x%x],[%d]\n", bright, bright);

	ret = eLIBs_fioctrl(g_display_shell, DISP_CMD_GET_BRIGHT, 0, (void *)0);
       __msg(" second read bright[0x%x][%d]\n", ret, ret);

	eLIBs_fclose(g_display_shell);
	
	return ret;
}

__s32  dsk_set_hue(__u32 hue)
{ 
	__u32       buf[3];
	__s32 ret;

	ES_FILE* g_display_shell = NULL;
	
	g_display_shell  = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
	if( !g_display_shell )
	{
	    __wrn("Open display driver failed, test program exits.\n");
	    return 0;
	}
	ret=eLIBs_fioctrl(g_display_shell, DISP_CMD_ENHANCE_ON, 0, (void *)0);

	   
	buf[0] = hue%100;
	buf[1] = 0;
	buf[2] = 0;     
   	eLIBs_fioctrl(g_display_shell, DISP_CMD_SET_HUE, 0, (void *)buf);

	//eLIBs_printf("write hue[0x%x],[%d]\n", hue, hue);

	ret = eLIBs_fioctrl(g_display_shell, DISP_CMD_GET_HUE, 0, (void *)0);
      // eLIBs_printf(" second read hue[0x%x][%d]\n", ret, ret);

	eLIBs_fclose(g_display_shell);
	
	return ret;
}


#endif


#endif     //  ifndef __DSK_DISPLAY_MISC_C__

/* end of dsk_display_misc.c */
