/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              ir keyboard driver
*
*                                (c) Copyright 2009-2010, kevin.z.m China
*                                            All Rights Reserved
*
* File    : pcf8563.h
* By      : kevin.z.m
* Version : V1.00
* Date    : 2009-11-20 10:20
*********************************************************************************************************
*/

#ifndef __PCF8563_H__
#define __PCF8563_H__

#include "epdk.h"

/*  registers define  */
#define CTRL_STA1_REG           (0x00)
#define CTRL_STA2_REG           (0x01)
#define VL_SECOND_REG           (0x02)
#define MINUTES_REG             (0x03)
#define HOURS_REG               (0x04)
#define DAYS_REG                (0x05)
#define WEEKDAYS_REG            (0x06)
#define MONTHS_REG              (0x07)
#define YEARS_REG               (0x08)
#define MINUTE_ALM_REG          (0x09)
#define HOUR_ALM_REG            (0x0A)
#define DAY_ALM_REG             (0x0B)
#define WEEKDAY_ALM_REG         (0x0C)
#define CLKOUT_CTRL_REG         (0x0D)
#define TIMER_CTRL_REG          (0x0E)
#define TIMER_REG               (0x0F)

#define Int8ToBCD(x)            (((x)/10)<<4)|((x)%10)
#define BCDToInt8(x)            ((((x)>>4)&0xf)*10+((x)&0xf))

#define RTC_PCF8563_SADDR       (0xA2>>1)

#define ALARM_FLAG_BIT          (3)
#define TIMER_FLAG_BIT          (2)
#define ALARM_INT_BIT           (1)
#define TIMER_INT_BIT           (0)

extern ES_FILE                  **pRtcIICHdl;

#endif

