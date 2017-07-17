/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: dsk_power.h
* By      	: Andy.zhang
* Func		: power driver control
* Version	: v1.0
* ============================================================================================================
* 2009-7-8 10:02:17  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __DSK_POWER_H__
#define __DSK_POWER_H__

typedef enum tag_power_level
{
    DSK_POWER_LEVEL_0 = 1,
    DSK_POWER_LEVEL_1,
    DSK_POWER_LEVEL_2,
    DSK_POWER_LEVEL_3,
    DSK_POWER_LEVEL_4,
    DSK_POWER_LEVEL_5,
    DSK_POWER_LEVEL_
} power_level_e;

typedef enum tag_power_state
{
    DSK_POWER_BATTARY_CHARGE = 1,
    DSK_POWER_BATTARY_FULL,
    DSK_POWER_BATTARY_DISCHARGE,
    DSK_POWER_BATTARY_NOT_EXIST,
    DSK_POWER_BATTERY_DCIN_NOT_ENOUGH,
    DSK_POWER_BATTARY_
} power_state_e;

__s32 dsk_power_set_off_time(__u32 time);						// 设置自动关机时间 单位 1ms

__s32 dsk_power_get_battery_level(power_level_e *level);		// 剩余电量level

__s32 dsk_power_get_voltage_level(power_level_e *level);		// 工作电压level

__s32 dsk_power_get_battary_state(power_state_e *state);		// 查询 battary 状态

__s32 dsk_power_off(void);										// 请求桌面系统关机

#endif /* __DSK_POWER_H__ */

