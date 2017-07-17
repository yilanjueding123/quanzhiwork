/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : robin_channel.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.27
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
* Gary.Wang      2009.09.27       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_channel_c
#define  __robin_channel_c


#include "..\\robin_i.h"



/***************************************************************************************************
*Name        : robin_set_channel
*Prototype   : __s32  robin_set_channel( __audio_dev_chan_mode_t  channel )
*Arguments   : channel   input. channel mode are defined in drv_audio.h, see __audio_dev_chan_mode_t.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the audio channel.
*Other       :
***************************************************************************************************/
__s32  robin_set_channel( __audio_dev_chan_mode_t  channel )
{
    G_FILE  *fp = NULL;

    fp = g_fopen(AUDIO_DRV_FILE_PATH, "rb");
    if( fp == NULL )
    {
        __wrn("Fail in open dac device!\n");
        return -1;
    }

    g_fioctrl(fp, AUDIO_DEV_CMD_SET_CHAN_MODE, channel, 0);

    g_fclose(fp);

    return 0;
}

/*
__s32 robin_set_channel_mode(__audio_dev_change_lr_t mode)
{
	G_FILE  *fp = NULL;

	if(mode<0 || mode>1)
	{
		__wrn("invaild mode!\n");
		return EPDK_FAIL;
	}
	fp = g_fopen("b:\\AUDIO\\PLAY", "rb");
    if( fp == NULL )
    {
    	__wrn("Fail in open dac device!\n");
    	return -1;
    }

	g_fioctrl(fp, AUDIO_DEV_CMD_SET_LR_CHANNEL, mode, 0);

	g_fclose(fp);

	return EPDK_OK;
}
*/


/***************************************************************************************************
*Name        : robin_get_channel
*Prototype   : __audio_dev_chan_mode_t  robin_get_channel( void )
*Arguments   : void
*Return      : the current channel mode
*Description : get the current channel mode
*Other       :
***************************************************************************************************/
__audio_dev_chan_mode_t  robin_get_channel( void )
{
    G_FILE  *fp = NULL;
    __s32    channel;

    fp = g_fopen(AUDIO_DRV_FILE_PATH, "rb");
    if( fp == NULL )
    {
        __wrn("Fail in open dac device!\n");
        return (__audio_dev_chan_mode_t)0;
    }

    channel = g_fioctrl(fp, AUDIO_DEV_CMD_GET_CHAN_MODE, 0, 0);
    __wrn("current channel is %d.\n", channel);

    g_fclose(fp);

    return (__audio_dev_chan_mode_t)channel;
}



#endif     //  ifndef __robin_channel_c

/* end of robin_channel.c */
