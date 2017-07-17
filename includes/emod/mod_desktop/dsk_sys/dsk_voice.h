/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: dsk_voice.h
* By      	: Andy.zhang
* Func		: voice control
* Version	: v1.0
* ============================================================================================================
* 2009-7-8 10:02:17  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __DSK_VOICE_H__
#define __DSK_VOICE_H__

/***************************************************************************************************
*Name        : dsk_voice_set_volume
*Prototype   : __s32 dsk_voice_set_volume(int volume);
*Arguments   :
*Return      :
*Description : 设置当前音量，音量最大值AUDIO_DEVICE_VOLUME_MAX(30)， 最小值0
*Other       :
***************************************************************************************************/
__s32 dsk_voice_set_volume(int volume);

/***************************************************************************************************
*Name        : dsk_voice_set_volume
*Prototype   : __s32 dsk_voice_set_volume(int volume);
*Arguments   :
*Return      :
*Description : 查询当前音量大小
*Other       :
***************************************************************************************************/
__s32 dsk_voice_get_volume(void);

/***************************************************************************************************
*Name        : dsk_voice_set_volume
*Prototype   : __s32 dsk_voice_set_volume(int volume);
*Arguments   :
*Return      :
*Description : 音量增加一个单元
*Other       :
***************************************************************************************************/
__s32 dsk_voice_inc_volume(void);

/***************************************************************************************************
*Name        : dsk_voice_dec_volume
*Prototype   : __s32 dsk_voice_dec_volume(void);
*Arguments   :
*Return      : 音量减小一个单元
*Other       :
***************************************************************************************************/
__s32 dsk_voice_dec_volume(void);		// 减小音量

/***************************************************************************************************
*Name        : dsk_voice_ban_volume
*Prototype   : __s32 dsk_voice_ban_volume(void);
*Arguments   :
*Return      : 禁音
*Other       :
***************************************************************************************************/
__s32 dsk_voice_ban_volume(void);

/***************************************************************************************************
*Name        : dsk_voice_ban_volume
*Prototype   : __s32 dsk_voice_ban_volume(void);
*Arguments   :
*Return      : 恢复音量
*Other       :
***************************************************************************************************/
__s32 dsk_voice_resume_volume(void);

/***************************************************************************************************
*Name        : dsk_voice_ban_volume
*Prototype   : __s32 dsk_voice_ban_volume(void);
*Arguments   :
*Return      : 是否禁音
*Other       :
***************************************************************************************************/
__s32 dsk_voice_is_ban(void);

#endif /* __DSK_VOICE_H__ */

