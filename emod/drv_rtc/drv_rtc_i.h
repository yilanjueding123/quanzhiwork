/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                            device monitor module
*
*                                   (c) Copyright 2006-2010, kevin.z.m China
*                                             All Rights Reserved
*
* File    : drv_rtc_i.h
* By      : kevin.z
* Version : v1.00
* Date    : 2010-6-20 15:16
*********************************************************************************************************
*/
#ifndef	_DRV_RTC_I_H_
#define	_DRV_RTC_I_H_

#include "epdk.h"

typedef struct __DRV_RTC
{
    __s32       mid;
    __s32       used;
    __hdle      devhdl;

} __drv_rtc_t;


extern __s32  RTC_Init(void);
extern __s32  RTC_Exit(void);
extern __s32  RTC_GetTime(__time_t *time);
extern __s32  RTC_SetTime(__time_t *time);
extern __s32  RTC_GetDate(__date_t *date);
extern __s32  RTC_SetDate(__date_t *date);
extern __hdle RTC_RequestAlarm(__u32 mode);
extern __s32  RTC_ReleaseAlarm(__hdle alarm);
extern __s32  RTC_StartAlarm(__hdle alarm, __u32 time);
extern __s32  RTC_StopAlarm(__hdle alarm);
extern __u32  RTC_QuerryAlarm(__hdle alarm);
extern __s32  RTC_QuerryInt(__s32 type);
extern __s32 RTC_SetClockOut(__u32 aux);


extern __s32 rtc_Init(void);
extern __s32 rtc_Exit(void);
extern __dev_devop_t rtc_ops;


#endif	/* _DRV_RTC_I_H_ */

