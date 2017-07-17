/*
************************************************************************************************************************
*                                                     display output
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*											       All Rights Reserved
*
* File Name   : dsk_display_output.c
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
#ifndef  __DSK_DISPLAY_OUTPUT_C__
#define  __DSK_DISPLAY_OUTPUT_C__


#include "dsk_display_i.h"



/*
****************************************************************************************************
*Name        : dsk_display_set_format
*Prototype   : __s32 dsk_display_set_format( __lion_disp_format_e type )
*Arguments   : type       input. display format.
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : switch the display format.
*Other       :
****************************************************************************************************
*/
__s32 dsk_display_set_format( __lion_disp_format_e mode )
{
    __disp_output_type_t  	output;
    ES_FILE      		   *p_disp = NULL;
    __s32                   tv_mode;
    __u32 arg[3];

    /* open display driver */
    p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");
    if( p_disp == NULL )
    {
        __err("Error in opening display driver.\n");
        goto error;
    }

    tv_mode = dsk_display_dskfmt2epdkfmt( mode, &output );
    switch( output )
    {
    case DISP_OUTPUT_TYPE_LCD :
    {
        /*__u32 arg[3];

        arg[0] = 0;
        arg[1] = 0;
        arg[2] = 0;
        eLIBs_fioctrl(p_disp, DISP_CMD_LCD_SET_SRC, DISP_LCDC_INSRC_DE_CH1, 0);	*/
        break;
    }
    case DISP_OUTPUT_TYPE_TV :
    {
        arg[0] = tv_mode;
        arg[1] = 0;
        arg[2] = 0;
        //eLIBs_fioctrl(p_disp, DISP_CMD_TV_SET_SRC, DISP_LCDC_INSRC_DE_CH1, 0);
        eLIBs_fioctrl(p_disp, DISP_CMD_TV_SET_MODE, 0, (void *)arg);
        break;
    }
    case DISP_OUTPUT_TYPE_HDMI :
    {
        arg[0] = tv_mode;
        arg[1] = 0;
        arg[2] = 0;
        eLIBs_fioctrl(p_disp, DISP_CMD_HDMI_SET_MODE, 0, (void *)arg);
        break;
    }
    case DISP_OUTPUT_TYPE_VGA :
    {
        arg[0] = tv_mode;
        arg[1] = 0;
        arg[2] = 0;
        eLIBs_fioctrl(p_disp, DISP_CMD_VGA_SET_MODE, 0, (void *)arg);
        break;
    }
    default :
    {
        __wrn("Warning! the display format %d is NOT legal.\n", output);
        goto error;
    }
    }

    /* close display driver */
    eLIBs_fclose(p_disp);
    p_disp = NULL;

    return EPDK_OK;

error:
    if( p_disp != NULL )
    {
        eLIBs_fclose(p_disp);
        p_disp = NULL;
    }
    return EPDK_FAIL;
}



/*
****************************************************************************************************
*Name        : dsk_display_get_format
*Prototype   : __lion_disp_format_e  dsk_display_get_format( void )
*Arguments   : void
*Return      : the current display format.
*Description : get the current display format.
*Other       :
****************************************************************************************************
*/
__lion_disp_format_e  dsk_display_get_format( void )
{
    __disp_output_type_t  	output;
    __s32                   format;
    __lion_disp_format_e    ret;
    ES_FILE      		   *p_disp = NULL;

    /* open display driver */
    p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");
    if( p_disp == NULL )
    {
        __err("Error in opening display driver.\n");
        goto error;
    }

    /* get output type and format */
    output  = (__disp_output_type_t)eLIBs_fioctrl(p_disp, DISP_CMD_GET_OUTPUT_TYPE, 0, 0);

    switch( output )
    {
    case DISP_OUTPUT_TYPE_LCD :
    {
        ret = LION_DISP_LCD;
        break;
    }
    case DISP_OUTPUT_TYPE_TV :
    {
        format = eLIBs_fioctrl(p_disp, DISP_CMD_TV_GET_MODE, 0, 0);
        break;
    }
    case DISP_OUTPUT_TYPE_HDMI :
    {
        format = eLIBs_fioctrl(p_disp, DISP_CMD_HDMI_GET_MODE, 0, 0);
        break;
    }
    case DISP_OUTPUT_TYPE_VGA :
    {
        format = eLIBs_fioctrl(p_disp, DISP_CMD_VGA_GET_MODE, 0, 0);
        break;
    }
    default :
    {
        __wrn("Warning! the display format %d is NOT legal.\n", output);
        goto error;
    }
    }

    /* convert format */
    ret = dsk_display_epdkfmt2dskfmt( output, format );

    /* close display driver */
    eLIBs_fclose(p_disp);
    p_disp = NULL;

    return ret;

error:
    if( p_disp != NULL )
    {
        eLIBs_fclose(p_disp);
        p_disp = NULL;
    }
    return LION_DISP_UNKNOWN;
}



/*
****************************************************************************************************
*Name        : dsk_display_get_output_type
*Prototype   : __disp_output_type_t  dsk_display_get_output_type( void )
*Arguments   : void
*Return      : the current display output type: TV, HDMI, and so on.
*Description : get the current display output type.
*Other       :
****************************************************************************************************
*/
__disp_output_type_t  dsk_display_get_output_type( void )
{
    ES_FILE 			 *p_disp = NULL;
    __disp_output_type_t  output;

    /* open display driver */
    p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");
    if( p_disp == NULL )
    {
        __err("Error in opening display driver.\n");
        return DISP_OUTPUT_TYPE_LCD;
    }

    /* get current display output type */

    output  = (__disp_output_type_t)eLIBs_fioctrl(p_disp, DISP_CMD_GET_OUTPUT_TYPE, 0, 0);

    /* close display driver */
    eLIBs_fclose(p_disp);
    p_disp = NULL;

    return output;
}



/*
****************************************************************************************************
*Name        : dsk_display_dskfmt2epdkfmt
*Prototype   : __s32  dsk_display_dskfmt2epdkfmt( __lion_disp_format_e format, __disp_output_type_t *output_p )
*Arguments   : format   input. the output format defined in desktop.
*              output_p output. the output type defined in ePDK.
*Return      : the format corresponding to the output type defined in ePDK.
*Description : convert the output format defined in desktop to the output format defined in ePDK.
*Other       :
****************************************************************************************************
*/
__s32  dsk_display_dskfmt2epdkfmt(__lion_disp_format_e mode, __disp_output_type_t *output_p )
{
    switch(mode)
    {
    case LION_DISP_LCD:
        *output_p = DISP_OUTPUT_TYPE_LCD;
        return DISP_VGA_H1920_V1080;

    case LION_DISP_TV_PAL:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_PAL;

    case LION_DISP_TV_NTSC:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_NTSC;

    case LION_DISP_TV_PAL_SVIDEO:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_PAL_SVIDEO;

    case LION_DISP_TV_NTSC_SVIDEO:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_NTSC_SVIDEO;

    case LION_DISP_TV_PAL_CVBS_SVIDEO:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_PAL_CVBS_SVIDEO;

    case LION_DISP_TV_NTSC_CVBS_SVIDEO:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_NTSC_CVBS_SVIDEO;

    case LION_DISP_TV_480I:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_480I;

    case LION_DISP_TV_480P:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_480P;

    case LION_DISP_TV_576I:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_576I;

    case LION_DISP_TV_576P:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_576P;

    case LION_DISP_TV_720P_50HZ:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_720P_50HZ;

    case LION_DISP_TV_720P_60HZ:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_720P_60HZ;

    case LION_DISP_TV_1080I_50HZ:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_1080I_50HZ;

    case LION_DISP_TV_1080I_60HZ:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_1080I_60HZ;

    case LION_DISP_TV_1080P_24HZ:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_1080P_24HZ;

    case LION_DISP_TV_1080P_50HZ:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_1080P_50HZ;

    case LION_DISP_TV_1080P_60HZ:
        *output_p = DISP_OUTPUT_TYPE_TV;
        return DISP_TV_MOD_1080P_60HZ;

    case LION_DISP_HDMI_480I:
        *output_p = DISP_OUTPUT_TYPE_HDMI;
        return DISP_TV_MOD_480I;

    case LION_DISP_HDMI_480P:
        *output_p = DISP_OUTPUT_TYPE_HDMI;
        return DISP_TV_MOD_480P;

    case LION_DISP_HDMI_576I:
        *output_p = DISP_OUTPUT_TYPE_HDMI;
        return DISP_TV_MOD_576I;

    case LION_DISP_HDMI_576P:
        *output_p = DISP_OUTPUT_TYPE_HDMI;
        return DISP_TV_MOD_576P;

    case LION_DISP_HDMI_720P_50HZ:
        *output_p = DISP_OUTPUT_TYPE_HDMI;
        return DISP_TV_MOD_720P_50HZ;

    case LION_DISP_HDMI_720P_60HZ:
        *output_p = DISP_OUTPUT_TYPE_HDMI;
        return DISP_TV_MOD_720P_60HZ;

    case LION_DISP_HDMI_1080I_50HZ:
        *output_p = DISP_OUTPUT_TYPE_HDMI;
        return DISP_TV_MOD_1080I_50HZ;

    case LION_DISP_HDMI_1080I_60HZ:
        *output_p = DISP_OUTPUT_TYPE_HDMI;
        return DISP_TV_MOD_1080I_60HZ;

    case LION_DISP_HDMI_1080P_24HZ:
        *output_p = DISP_OUTPUT_TYPE_HDMI;
        return DISP_TV_MOD_1080P_24HZ;

    case LION_DISP_HDMI_1080P_50HZ:
        *output_p = DISP_OUTPUT_TYPE_HDMI;
        return DISP_TV_MOD_1080P_50HZ;

    case LION_DISP_HDMI_1080P_60HZ:
        *output_p = DISP_OUTPUT_TYPE_HDMI;
        return DISP_TV_MOD_1080P_60HZ;

    case LION_DISP_VGA_H1680_V1050:
        *output_p = DISP_OUTPUT_TYPE_VGA;
        return DISP_VGA_H1680_V1050;

    case LION_DISP_VGA_H1440_V900:
        *output_p = DISP_OUTPUT_TYPE_VGA;
        return DISP_VGA_H1440_V900;

    case LION_DISP_VGA_H1360_V768:
        *output_p = DISP_OUTPUT_TYPE_VGA;
        return DISP_VGA_H1360_V768;

    case LION_DISP_VGA_H1280_V1024:
        *output_p = DISP_OUTPUT_TYPE_VGA;
        return DISP_VGA_H1280_V1024;

    case LION_DISP_VGA_H800_V600:
        *output_p = DISP_OUTPUT_TYPE_VGA;
        return DISP_VGA_H800_V600;

    case LION_DISP_VGA_H640_V480:
        *output_p = DISP_OUTPUT_TYPE_VGA;
        return DISP_VGA_H640_V480;

    case LION_DISP_VGA_H1440_V900_RB:
        *output_p = DISP_OUTPUT_TYPE_VGA;
        return DISP_VGA_H1440_V900_RB;

    case LION_DISP_VGA_H1680_V1050_RB:
        *output_p = DISP_OUTPUT_TYPE_VGA;
        return DISP_VGA_H1680_V1050_RB;

    case LION_DISP_VGA_H1920_V1080_RB:
        *output_p = DISP_OUTPUT_TYPE_VGA;
        return DISP_VGA_H1920_V1080_RB;

    case LION_DISP_VGA_H1920_V1080:
        *output_p = DISP_OUTPUT_TYPE_VGA;
        return DISP_VGA_H1920_V1080;

    default:
        *output_p = DISP_OUTPUT_TYPE_HDMI;
        return DISP_TV_MOD_720P_50HZ;
    }
}



/*
****************************************************************************************************
*Name        : dsk_display_epdkfmt2dskfmt
*Prototype   : __lion_disp_format_e  dsk_display_epdkfmt2dskfmt( __disp_output_type_t output, __s32 format );
*Arguments   : output   input. the output type defined in ePDK.
*              format   input. the format corresponding to the output type defined in ePDK. if the
*                       output type is LCD, this parameter may be ignored.
*Return      :
*Description : convert the output format defined in ePDK to the output format defined in desktop.
*Other       :
****************************************************************************************************
*/
__lion_disp_format_e  dsk_display_epdkfmt2dskfmt(__disp_output_type_t output, __s32 tv_mode )
{
    if(output == DISP_OUTPUT_TYPE_LCD)
    {
        return LION_DISP_LCD;
    }
    else if(output == DISP_OUTPUT_TYPE_TV && tv_mode == DISP_TV_MOD_PAL)
    {
        return LION_DISP_TV_PAL;
    }
    else if(output == DISP_OUTPUT_TYPE_TV && tv_mode == DISP_TV_MOD_NTSC)
    {
        return LION_DISP_TV_NTSC;
    }
    else if(output == DISP_OUTPUT_TYPE_TV && tv_mode == DISP_TV_MOD_PAL_SVIDEO)
    {
        return LION_DISP_TV_PAL_SVIDEO;
    }
    else if(output == DISP_OUTPUT_TYPE_TV && tv_mode == DISP_TV_MOD_NTSC_SVIDEO)
    {
        return LION_DISP_TV_NTSC_SVIDEO;
    }
    else if(output == DISP_OUTPUT_TYPE_TV && tv_mode == DISP_TV_MOD_PAL_CVBS_SVIDEO)
    {
        return LION_DISP_TV_PAL_CVBS_SVIDEO;
    }
    else if(output == DISP_OUTPUT_TYPE_TV && tv_mode == DISP_TV_MOD_NTSC_CVBS_SVIDEO)
    {
        return LION_DISP_TV_NTSC_CVBS_SVIDEO;
    }
    else if(output == DISP_OUTPUT_TYPE_TV && tv_mode == DISP_TV_MOD_480I)
    {
        return LION_DISP_TV_480I;
    }
    else if(output == DISP_OUTPUT_TYPE_TV && tv_mode == DISP_TV_MOD_480P)
    {
        return LION_DISP_TV_480P;
    }
    else if(output == DISP_OUTPUT_TYPE_TV && tv_mode == DISP_TV_MOD_576I)
    {
        return LION_DISP_TV_576I;
    }
    else if(output == DISP_OUTPUT_TYPE_TV && tv_mode == DISP_TV_MOD_576P)
    {
        return LION_DISP_TV_576P;
    }
    else if(output == DISP_OUTPUT_TYPE_TV && tv_mode == DISP_TV_MOD_720P_50HZ)
    {
        return LION_DISP_TV_720P_50HZ;
    }
    else if(output == DISP_OUTPUT_TYPE_TV && tv_mode == DISP_TV_MOD_720P_60HZ)
    {
        return LION_DISP_TV_720P_60HZ;
    }
    else if(output == DISP_OUTPUT_TYPE_TV && tv_mode == DISP_TV_MOD_1080I_50HZ)
    {
        return LION_DISP_TV_1080I_50HZ;
    }
    else if(output == DISP_OUTPUT_TYPE_TV && tv_mode == DISP_TV_MOD_1080I_60HZ)
    {
        return LION_DISP_TV_1080I_60HZ;
    }
    else if(output == DISP_OUTPUT_TYPE_TV && tv_mode == DISP_TV_MOD_1080P_50HZ)
    {
        return LION_DISP_TV_1080P_50HZ;
    }
    else if(output == DISP_OUTPUT_TYPE_TV && tv_mode == DISP_TV_MOD_1080P_60HZ)
    {
        return LION_DISP_TV_1080P_60HZ;
    }
    else if(output == DISP_OUTPUT_TYPE_HDMI && tv_mode == DISP_TV_MOD_480I)
    {
        return LION_DISP_HDMI_480I;
    }
    else if(output == DISP_OUTPUT_TYPE_HDMI && tv_mode == DISP_TV_MOD_480P)
    {
        return LION_DISP_HDMI_480P;
    }
    else if(output == DISP_OUTPUT_TYPE_HDMI && tv_mode == DISP_TV_MOD_576I)
    {
        return LION_DISP_HDMI_576I;
    }
    else if(output == DISP_OUTPUT_TYPE_HDMI && tv_mode == DISP_TV_MOD_576P)
    {
        return LION_DISP_HDMI_576P;
    }
    else if(output == DISP_OUTPUT_TYPE_HDMI && tv_mode == DISP_TV_MOD_720P_50HZ)
    {
        return LION_DISP_HDMI_720P_50HZ;
    }
    else if(output == DISP_OUTPUT_TYPE_HDMI && tv_mode == DISP_TV_MOD_720P_60HZ)
    {
        return LION_DISP_HDMI_720P_60HZ;
    }
    else if(output == DISP_OUTPUT_TYPE_HDMI && tv_mode == DISP_TV_MOD_1080I_50HZ)
    {
        return LION_DISP_HDMI_1080I_50HZ;
    }
    else if(output == DISP_OUTPUT_TYPE_HDMI && tv_mode == DISP_TV_MOD_1080I_60HZ)
    {
        return LION_DISP_HDMI_1080I_60HZ;
    }
    else if(output == DISP_OUTPUT_TYPE_HDMI && tv_mode == DISP_TV_MOD_1080P_24HZ)
    {
        return LION_DISP_HDMI_1080P_24HZ;
    }
    else if(output == DISP_OUTPUT_TYPE_HDMI && tv_mode == DISP_TV_MOD_1080P_50HZ)
    {
        return LION_DISP_HDMI_1080P_50HZ;
    }
    else if(output == DISP_OUTPUT_TYPE_HDMI && tv_mode == DISP_TV_MOD_1080P_60HZ)
    {
        return LION_DISP_HDMI_1080P_60HZ;
    }
    else if(output == DISP_OUTPUT_TYPE_VGA && tv_mode == DISP_VGA_H1680_V1050)
    {
        return LION_DISP_VGA_H1680_V1050;
    }
    else if(output == DISP_OUTPUT_TYPE_VGA && tv_mode == DISP_VGA_H1440_V900)
    {
        return LION_DISP_VGA_H1440_V900;
    }
    else if(output == DISP_OUTPUT_TYPE_VGA && tv_mode == DISP_VGA_H1360_V768 )
    {
        return LION_DISP_VGA_H1360_V768;
    }
    else if(output == DISP_OUTPUT_TYPE_VGA && tv_mode == DISP_VGA_H1280_V1024)
    {
        return LION_DISP_VGA_H1280_V1024;
    }
    else if(output == DISP_OUTPUT_TYPE_VGA && tv_mode == DISP_VGA_H1024_V768)
    {
        return LION_DISP_VGA_H1024_V768;
    }
    else if(output == DISP_OUTPUT_TYPE_VGA && tv_mode == DISP_VGA_H800_V600)
    {
        return LION_DISP_VGA_H800_V600;
    }
    else if(output == DISP_OUTPUT_TYPE_VGA && tv_mode == DISP_VGA_H640_V480)
    {
        return LION_DISP_VGA_H640_V480;
    }
    else if(output == DISP_OUTPUT_TYPE_VGA && tv_mode == DISP_VGA_H1440_V900_RB)
    {
        return LION_DISP_VGA_H1440_V900_RB;
    }
    else if(output == DISP_OUTPUT_TYPE_VGA && tv_mode == DISP_VGA_H1680_V1050_RB)
    {
        return LION_DISP_VGA_H1680_V1050_RB;
    }
    else if(output == DISP_OUTPUT_TYPE_VGA && tv_mode == DISP_VGA_H1920_V1080_RB)
    {
        return LION_DISP_VGA_H1920_V1080_RB;
    }
    else if(output == DISP_OUTPUT_TYPE_VGA && tv_mode == DISP_VGA_H1920_V1080)
    {
        return LION_DISP_VGA_H1920_V1080;
    }
    else
    {
        return LION_DISP_UNKNOWN;
    }
}



/*
****************************************************************************************************
*Name        : dsk_display_on
*Prototype   : __s32  dsk_display_on( __disp_output_type_t output )
*Arguments   : output     input. the display output type.
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : open the display output.
*Other       :
****************************************************************************************************
*/
__s32  dsk_display_on( __disp_output_type_t output )
{
    ES_FILE 			 *p_disp = NULL;

    /* open display driver */
    p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");
    if( p_disp == NULL )
    {
        __err("Error in opening display driver.\n");
        return EPDK_FAIL;
    }

    /* get current display output type */
    if(output == DISP_OUTPUT_TYPE_LCD)
    {
        eLIBs_fioctrl(p_disp, DISP_CMD_LCD_ON, 0, 0);
    }
    else if(output == DISP_OUTPUT_TYPE_TV)
    {
        eLIBs_fioctrl(p_disp, DISP_CMD_TV_ON, 0, 0);
    }
    else if(output == DISP_OUTPUT_TYPE_VGA)
    {
        eLIBs_fioctrl(p_disp, DISP_CMD_VGA_ON, 0, 0);
    }
    else if(output == DISP_OUTPUT_TYPE_HDMI)
    {
        eLIBs_fioctrl(p_disp, DISP_CMD_HDMI_ON, 0, 0);
    }

    /* close display driver */
    eLIBs_fclose(p_disp);
    p_disp = NULL;

    return EPDK_OK;
}



/*
****************************************************************************************************
*Name        : dsk_display_off
*Prototype   : __s32  dsk_display_off( void )
*Arguments   : void
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : close the current display output.
*Other       :
****************************************************************************************************
*/
__s32  dsk_display_off( void )
{
    ES_FILE 			 *p_disp = NULL;
    __disp_output_type_t  output;

    /* open display driver */
    p_disp = eLIBs_fopen(DISP_DRV_PATH, "r+");
    if( p_disp == NULL )
    {
        __err("Error in opening display driver.\n");
        return EPDK_FAIL;
    }

    /* get current display output type */
    output  = (__disp_output_type_t)eLIBs_fioctrl(p_disp, DISP_CMD_GET_OUTPUT_TYPE, 0, 0);

    if(output == DISP_OUTPUT_TYPE_LCD)
    {
        eLIBs_fioctrl(p_disp, DISP_CMD_LCD_OFF, 0, 0);
    }
    else if(output == DISP_OUTPUT_TYPE_TV)
    {
        eLIBs_fioctrl(p_disp, DISP_CMD_TV_OFF, 0, 0);
    }
    else if(output == DISP_OUTPUT_TYPE_VGA)
    {
        eLIBs_fioctrl(p_disp, DISP_CMD_VGA_OFF, 0, 0);
    }
    else if(output == DISP_OUTPUT_TYPE_HDMI)
    {
        eLIBs_fioctrl(p_disp, DISP_CMD_HDMI_OFF, 0, 0);
    }

    /* close display driver */
    eLIBs_fclose(p_disp);
    p_disp = NULL;

    return EPDK_OK;
}



#endif     //  ifndef __DSK_DISPLAY_OUTPUT_C__

/* end of dsk_display_output.c */
