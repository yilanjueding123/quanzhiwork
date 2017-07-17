/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eBIOS Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : ebios
* File    : adda.h
*
* By      : Steven
* Version : v1.0
* Date    : 2008-9-5 14:31:03
**********************************************************************************************************************
*/
//#ifndef _AUDIO_CODEC_H_
//#define _AUDIO_CODEC_H_


#ifndef __BSP_ADDA_H__
#define __BSP_ADDA_H__

#define AUDIO_REC_DEFAULT             0xff




typedef enum
{
    BSP_ADDA_REC_ERR,
    BSP_ADDA_REC_MIC1,		//record source:mic
    BSP_ADDA_REC_MIC2,
    BSP_ADDA_REC_MIC1_AND_MIC2,
    BSP_ADDA_REC_MIXER,
    BSP_ADDA_REC_FM,				//record source:FM
    BSP_ADDA_REC_LINEIN			//record source:line_in
} bsp_adda_rec_source_e;


typedef enum
{
    BSP_ADDA_REC_PLAYBACK,		//recording and play back
    BSP_ADDA_REC_NO_PLAYBACK	//recording without play back
} bsp_adda_rec_ifplayback_e;

__s32 BSP_ADDA_Init(void);
__s32 BSP_ADDA_Exit(void);
__s32 BSP_ADDA_Enable(__u32 uDevType, __u32 uBoardVer);
__s32 BSP_ADDA_Disable(__u32 uDevType);
__s32 BSP_ADDA_Start(__u32 uDevType);
__s32 BSP_ADDA_Stop(__u32 uDevType);
__s32 BSP_ADDA_Pause(__u32 uDevType, __s32 nMode);
__s32 BSP_ADDA_SetSampWidth(__u32 uDevType, __u32 nSampWidth);
__s32 BSP_ADDA_GetSampleWidth(__u32 uDevType);
__s32 BSP_ADDA_SetSampRate(__u32 uDevType, __s32 nSampRate);
__s32 BSP_ADDA_GetSampRate(__u32 uDevType);

__s32 BSP_ADDA_SetChannel(__u32 uDevType, __u32 nChnCnt);
__s32 BSP_ADDA_GetChannel(__u32 uDevType);


__s32 BSP_ADDA_SetSampleCnt(__u32 uDevType, __u32 nCnt);
__s32 BSP_ADDA_GetSampleCnt(__u32 uDevType);

__s32 BSP_ADDA_SetVolume(__u32 uDevType, __s32 nVolume);
__s32 BSP_ADDA_GetVolume(__u32 uDevType);
__s32 BSP_ADDA_SetDigitalVolume(__s32 volume);
__s32 BSP_ADDA_GetDigitalVolume(void);
__s32 BSP_ADDA_GetDmaCfg(__u32 uDevType);
__s32 BSP_ADDA_GetFifoAddr(__u32 uDevType);
__s32 ADDA_SwVol2HwVol(__u32 uVolume);
__s32 ADDA_HwVol2SwVol(__u32 uVolume);

__s32 BSP_ADDA_SetRecMode(__u32 uDevType, __u32 RecMode);
__s32 BSP_ADDA_GetRecMode(__u32 uDevType);
__s32 BSP_ADDA_SetRecPlayMode(__u32 uDevType, __u32 ifplay);

__s32 BSP_ADDA_FM_Init(void);
__s32 BSP_ADDA_FM_Exit(void);

__s32 BSP_ADDA_EnterStandby(void);
__s32 BSP_ADDA_ExitStandby(void);

__s32 BSP_ADDA_LineIn_Init(void);
__s32 BSP_ADDA_LineIn_Exit(void);

typedef enum _AudioRecMode
{
    AUDIO_REC_MODE_NONE     = 0, //not record source
    ADUIO_REC_MODE_FM       = 1, //use fm as record source
    AUDIO_REC_MODE_MIC      = 2, //use mic as record source
    AUDIO_REC_MODE_MIXER    = 6,//use mixer as mixer
} AudioRecMode_e;

/*********************************************************************
* Method	 :    		BSP_ADDA_set_headPhone_drive_mode
* Description: set driver mode of head phone
* Parameters :
	@__u32 drvMode : 0 means AC mode, 1 means DC mode, other values are reserved
* Returns    :   0 if successful, -1 if fail
* Note       :
*********************************************************************/
__s32 BSP_ADDA_set_headPhone_drive_mode(__u32 drvMode);

/*********************************************************************
* Method	 :    		BSP_ADDA_DAC_output_channel_swap
* Description: swap the DAC output channel
* Parameters :
	@__bool channelSwap : if true(no 0), to swap the DAC channel,
                   that is, left DAC channel will output to right channel, and right DAC channel will output to left channel.
                   if 0, left channel output to left and right channel output to right.
* Returns    :   0 if successful, -1 if fail
* Note       :
*********************************************************************/
__s32 BSP_ADDA_DAC_output_channel_swap(__bool channelSwap);

//Get maximum Power Amplifier gain
__u32 BSP_ADDA_dac_max_gain(void);

//Get maximum ADC
__u32 BSP_ADDA_adc_max_gain(AudioRecMode_e recUsr);

__s32 BSP_ADDA_Mute_OnOff(__u8 b_mute);

//return 0: not mute, 1:mute
__s32 BSP_ADDA_Mute_Get_OnOff(void);


__s32 BSP_ADDA_set_mic_pregain(__s32 pregain);

__s32 BSP_ADDA_mic_pregain_onoff(__s32 onoff);

__s32 BSP_ADDA_FlushFifo(__u32 uDevType);

//sel:0 mute ,  1 not mute
__s32 BSP_ADDA_FifoSampleSel(__u32 uDevType, __u32 sel);


#endif  /* */

