/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                                standby module
*
*                                   (c) Copyright 2008-2009, kevin China
*                                            All Rights Reserved
*
* File    : standby_power.h
* By      : kevin
* Version : V1.0
* Date    : 2009-6-16 17:25
*********************************************************************************************************
*/
#ifndef _STANDBY_POWER_H_
#define _STANDBY_POWER_H_

#include "ePDK.h"
#include "power_axp189.h"
#include "power_axp199.h"


extern __s32 standby_power_init(void);
extern __s32 standby_power_exit(void);
extern void  (*standby_set_voltage         )(__s32 type, __s32 voltage);
extern __u32 (*standby_get_voltage         )(__s32 type);
extern __s32 (*standby_low_power_dectect   )(void);
extern __s32 (*standby_ext_power_detect    )(void);
extern __s32 (*standby_power_key_detect    )(void);


#endif  //_STANDBY_POWER_H_


