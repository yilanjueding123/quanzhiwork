/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : robin_palette.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.10.10
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
* Gary.Wang      2009.10.10       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_palette_c
#define  __robin_palette_c


#include "..\\robin_i.h"

/***************************************************************************************************
*Name        : robin_set_palette
*Prototype   : __s32 robin_set_palette( const __u32 *palette_buf )
*Arguments   : palette_buf   input. the palette data buffer, whose size must be
*                            ( ROBIN_PALETTE_COLOR_COUNT * 4 ).
*Return      : ==0       succeed
*              !=0       fail
*Description : set the whole palette.
*Other       :
***************************************************************************************************/
__s32 robin_set_palette( const __u32 *palette_buf )
{
    __u32  palette[ROBIN_PALETTE_COLOR_COUNT];
    __s32  ret;
    __u32 arg[3] = {0};

    if( palette_buf == NULL )
        return -1;

    g_memcpy( palette, palette_buf, ROBIN_PALETTE_COLOR_COUNT * 4 );

    /* get palette */
    arg[0] = (__u32)palette;
    arg[2] = ROBIN_PALETTE_COLOR_COUNT * 4;
    ret = g_fioctrl( robin_hdis, DISP_CMD_SET_PALETTE_TBL, 0,  (void *)arg);

    if( ret != 0 )
    {
        __wrn("Fail in setting palette.\n");
        return -1;
    }

    return 0;
}



/***************************************************************************************************
*Name        : robin_get_palette
*Prototype   : __s32 robin_get_palette( __u32 *palette_buf )
*Arguments   : palette_buf   output. the buffer to store palette data, whose size must be
*                            ( ROBIN_PALETTE_COLOR_COUNT * 4 ).
*Return      : ==0       succeed
*              !=0       fail
*Description : get the whole palette.
*Other       :
***************************************************************************************************/
__s32 robin_get_palette( __u32 *palette_buf )
{
    __u32  palette[ROBIN_PALETTE_COLOR_COUNT];
    __s32  ret;
    __u32 arg[3] = {0};

    if( palette_buf == NULL )
        return -1;

    /* get palette */
    arg[0] = (__u32)palette;
    arg[2] = ROBIN_PALETTE_COLOR_COUNT * 4;
    ret = g_fioctrl( robin_hdis, DISP_CMD_GET_PALETTE_TBL, 0,  (void *)arg);

    if( ret != 0 )
    {
        __wrn("Fail in getting palette.\n");
        return -1;
    }

    g_memcpy( palette_buf, palette, ROBIN_PALETTE_COLOR_COUNT * 4 );

    return 0;
}



/***************************************************************************************************
*Name        : robin_modify_palette
*Prototype   : __s32 robin_modify_palette( __u32 first_index, const __u32 *color_array, __u32 nr )
*Arguments   : first_index   input. the index of the first color to be modified in the palette, it
*                            rangs 0~~~~(ROBIN_PALETTE_COLOR_COUNT-1).
*              color_array   input. the new color list, it is used to replace partial colors in
*                            palette.
*              nr            input. the color count of the color array.
*Return      : ==0       succeed
*              !=0       fail
*Description : modify the partial colors of the palette.
*Other       :
***************************************************************************************************/
__s32 robin_modify_palette( __u32 first_index, const __u32 *color_array, __u32 nr )
{
    __u32  palette[ROBIN_PALETTE_COLOR_COUNT];
    __s32  ret;
    __u32  real_nr;
    __u32 arg[3] = {0};

    if( first_index > ROBIN_PALETTE_COLOR_COUNT )
        return -1;

    if( color_array == NULL )
        return -1;

    /* get palette */
    arg[0] = (__u32)palette;
    arg[2] = ROBIN_PALETTE_COLOR_COUNT * 4;
    ret = g_fioctrl( robin_hdis, DISP_CMD_GET_PALETTE_TBL, 0,  (void *)arg);

    if( ret != 0 )
    {
        __wrn("Fail in getting palette.\n");
        return -1;
    }

    /* modify palette */
    real_nr = min( ROBIN_PALETTE_COLOR_COUNT - first_index, nr );
    g_memcpy( palette + first_index, color_array, sizeof(__u32) * real_nr );

    arg[0] = (__u32)palette;
    arg[2] = ROBIN_PALETTE_COLOR_COUNT * 4;
    ret = g_fioctrl( robin_hdis, DISP_CMD_SET_PALETTE_TBL, 0,  (void *)arg);

    if( ret != 0 )
    {
        __wrn("Fail in setting palette.\n");
        return -1;
    }

    return 0;
}



#endif     //  ifndef __robin_palette_c

/* end of robin_palette.c */
