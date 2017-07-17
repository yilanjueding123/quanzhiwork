/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eBIOS Sub-System
*
*                                   (c) Copyright 2008-2009, kevin China
*                                             All Rights Reserved
*
* Moudle  : ebios
* File    : audio.h
* By      : kevin
* Version : v1.0
* Date    : 2009-4-4 15:17
* Note    : this file is for all audio device interface
**********************************************************************************************************************
*/
#ifndef _AUDIO_H_
#define _AUDIO_H_


#define AUDIO_PLAY_DMA_USE_SRAM     (0)
#define AUDIO_REC_DMA_USE_SRAM      (0)

#define AUDIO_BSP_DEV_TYPE_PLAY     (1<<0)  //physic audio play device
#define AUDIO_BSP_DEV_TYPE_REC      (1<<1)  //physic audio record device
#define AUDIO_BSP_DEV_TYPE_FM       (1<<2)  //physic audio fm device
#define AUDIO_BSP_DEV_TYPE_CTRL     (1<<3)  //physic audio control device
#define AUDIO_BSP_DEV_TYPE_AEC		(1<<4)
#define AUDIO_BSP_DEV_TYPE_USER		(1<<5)

#define AUDIO_BSP_DEV_PAUSE         (1<<0)  //pause physic audio device
#define AUDIO_BSP_DEV_CONINUE       (1<<1)  //coninue physic audio device

#endif //#ifndef _AUDIO_H_
