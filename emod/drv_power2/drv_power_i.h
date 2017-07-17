/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                Power Driver
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : drv_power_i.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-4-30
* Descript: no pmu power dirver.
* Update  : date                auther      ver     notes
*           2011-4-30 9:45:35   Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __DRV_POWER_I_H__
#define __DRV_POWER_I_H__

#include "epdk.h"

//power sample period : 5s = 5000 * 1ms
#define POWER_TIME_PERIOD       1500

// define no PMU para
#define LRADC_CRTL_ADDR         ((volatile __u32 *)0xf1c23400)
#define LRADC_INTCRTL_ADDR      ((volatile __u32 *)0xf1c23404)
#define LRADC_INTSTAT_ADDR      ((volatile __u32 *)0xf1c23408)
#define LRADC_DATA_ADDR         ((volatile __u32 *)0xf1c2340c)
#define LRADC_CRTL_VALUE        (*(LRADC_CRTL_ADDR))
#define LRADC_DATA_VALUE        (*(LRADC_DATA_ADDR))

// define watch-dog
#define TMRC_WACHDOG_CTL_ADDR   ((volatile __u32 *)0Xf1C20C30)
#define TMRC_WATCH_DOG_EN_BIT   (0)

//define power_on pin address
#define POWER_ON_PIO_DATA_ADDR  ((volatile __u32 *)0xf1c20858)
#define POWER_ON_PIO_NUM        (8)

#define	BAT_HIGHT_RESISITOR     1500
#define	BAT_LOW_RESISITOR       1000
#define NO_PMU_BATTERY_LEVEL0	0
#define NO_PMU_BATTERY_LEVEL1   20
#define NO_PMU_BATTERY_LEVEL2   40
#define NO_PMU_BATTERY_LEVEL3   60
#define NO_PMU_BATTERY_LEVEL4   80
#define NO_PMU_BATTERY_LEVEL5   100


typedef struct __POWER_DEV
{
    __u32 nouse;
} __power_dev_t;

typedef struct __POWER_DRV
{
    __u8            mid;
    __u8            used;
    __power_dev_t   PowerDev;
    __hdle          hReg;
    __krnl_stmr_t  *hTimer;
} __power_drv_t;

//drv_power.c
__s32 DRV_PowerMInit(void);
__s32 DRV_PowerMExit(void);
__mp *DRV_PowerMOpen(__u32 mid, __u32 mode);
__s32 DRV_PowerMClose(__mp *mp);
__u32 DRV_PowerMRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 DRV_PowerMWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
__s32 DRV_PowerMIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);

//dev_power.c
extern __dev_devop_t power_dev_ops;

//drv_power.c, dev_power.c
extern __hdle hPowerOn;
extern __hdle hChargeStatus;

//power_core.c
extern void PowerTask(void *parg);

#endif  //__DRV_POWER_I_H__
