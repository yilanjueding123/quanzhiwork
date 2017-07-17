/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               RTC Driver Module
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : nop_rtc.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-5-19
* Descript: nop rtc dirver.
* Update  : date                auther      ver     notes
*           2011-5-19 20:39:29  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __NOP_RTC_H__
#define __NOP_RTC_H__

#include "epdk.h"

//nop rtc function
extern __s32 NOP_SetDate(__u32 year, __u32 month, __u32 day);
extern __s32 NOP_GetDate(__u32 *year, __u32 *month, __u32 *day);
extern __s32 NOP_SetTime(__u32 hour, __u32 minute, __u32 second);
extern __s32 NOP_GetTime(__u32 *hour, __u32 *minute, __u32 *second);
extern __s32 NOP_AlarmEnable(void);
extern __s32 NOP_AlarmDisable(void);
extern __s32 NOP_AlarmInstIsr(void);
extern __s32 NOP_AlarmUninstIsr(void);
extern __u32 NOP_AlarmQueryINT(void);
extern __s32 NOP_AlarmEnableINT(__u32 mode, __u32 attr);
extern __s32 NOP_AlarmDisableINT(__u32 mode, __u32 attr);
extern __s32 NOP_AlarmSetDuration(__u32 duration);
extern __u32 NOP_AlarmQuery(void);

#endif //__NOP_RTC_H__

