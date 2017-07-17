/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                            device monitor module
*
*                                   (c) Copyright 2006-2010, kevin.z.m China
*                                             All Rights Reserved
*
* File    : rtc_core.h
* By      : kevin.z
* Version : v1.00
* Date    : 2010-6-20 15:39
*********************************************************************************************************
*/
#ifndef	_RTC_CORE_H_
#define	_RTC_CORE_H_

#include "drv_rtc_i.h"

typedef struct __ALARM_T
{
    __u8  type;
    __u8  used;
    __u16 reserved;
    __u32 attrib;

} __alarm_t;


typedef struct __RTC_DEV
{
    ES_FILE *hIIC;

    //calendar function entry
    __s32 (* SetDate) (__u32 /*year*/, __u32 /*month*/, __u32 /*day*/);
    __s32 (* GetDate) (__u32 */*year*/, __u32 */*month*/, __u32 */*day*/);

    //time function entry
    __s32 (* SetTime) (__u32 /*hour*/, __u32 /*minute*/, __u32 /*second*/);
    __s32 (* GetTime) (__u32 */*hour*/, __u32 */*minute*/, __u32 */*second*/);

    //alarm function entry
    __s32 (* AlarmEnable     )(void);
    __s32 (* AlarmDisable    )(void);
    __s32 (* AlarmInstIsr    )(void);
    __s32 (* AlarmUninstIsr  )(void);
    __u32 (* AlarmQueryINT   )(void);
    __s32 (* AlarmEnableINT  )(__u32 mode, __u32 attr);
    __s32 (* AlarmDisableINT )(__u32 mode, __u32 attr);
    __s32 (* AlarmSetDuration)(__u32 duration);
    __u32 (* AlarmQuery      )(void);
    __u32 (* QueryInt        )(void);
    __s32 (* SetClockOut)(__u32 aux);

} __rtc_dev_t;




#endif	/* _DRV_RTC_I_H_ */

