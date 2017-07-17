/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                               standby module
*
*                                   (c) Copyright 2008-2009, kevin China
*                                            All Rights Reserved
*
* File    : standby_tmr.h
* By      : kevin
* Version : V1.0
* Date    : 2009-7-22 20:19
*********************************************************************************************************
*/
#ifndef _STANDBY_TMR_H_
#define _STANDBY_TMR_H_

#include "ePDK.h"
#include "standby_cfg.h"
#include "standby_reg.h"

//define standby timer tick, based on 1s
#define STANDBY_TIMER_TICKS         (1)
#define STANDBY_TICKS_PER_SEC       (100)

//define timer controller registers
typedef struct __STANDBY_TMR_REG
{
    // offset:0x00
    volatile __u32   IntCtl;
    volatile __u32   IntSta;
    volatile __u32   Reserve0[2];

    // offset:0x10
    volatile __u32   Tmr0Ctl;
    volatile __u32   Tmr0IntVal;
    volatile __u32   Tmr0CntVal;
    volatile __u32   Reserve1;

    // offset:0x20
    volatile __u32   Tmr1Ctl;
    volatile __u32   Tmr1IntVal;
    volatile __u32   Tmr1CntVal;
    volatile __u32   Reserve2;

    // offset:0x30
    volatile __u32   Tmr2Ctl;
    volatile __u32   Tmr2IntVal;
    volatile __u32   Tmr2CntVal;
    volatile __u32   Reserve3[17];


    // offset:0x80
    volatile __u32   AvsCntCtl;
    volatile __u32   AvsCnt0;
    volatile __u32   AvsCnt1;
    volatile __u32   AvsCntDiv;
    volatile __u32   Reserve4[4];


    // offset:0xa0
    volatile __u32   WdogIrqEn;
    volatile __u32   WdogIrqSta;
    volatile __u32   Reserve5[2];

    // offset:0xb0
    volatile __u32   WdogCtrl;
    volatile __u32   WdogCfg;
    volatile __u32   WdogMode;




} __standby_tmr_reg_t;

__s32 standby_tmr_init(void);
__s32 standby_tmr_exit(void);
__s32 standby_tmr_query(void);
__u32 standby_clear_dog(void);


#endif  //_STANDBY_TMR_H_

