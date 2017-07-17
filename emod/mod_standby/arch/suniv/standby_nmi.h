/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                               standby module
*
*                                   (c) Copyright 2008-2010, kevin China
*                                            All Rights Reserved
*
* File    : standby_nmi.h
* By      : kevin
* Version : V1.0
* Date    : 2010-6-21 15:53
*********************************************************************************************************
*/
#ifndef _STANDBY_NMI_H_
#define _STANDBY_NMI_H_

#include "ePDK.h"
#include "standby_cfg.h"
#include "standby_reg.h"


typedef struct __STANDBY_INT_REG
{
    volatile __u32   Vector;
    volatile __u32   BaseAddr;
    volatile __u32   Pending0;
    volatile __u32   Pending1;
    volatile __u32   IntCfg;
    volatile __u32   Enable0;
    volatile __u32   Enable1;
    volatile __u32   Mask0;
    volatile __u32   Mask1;
    volatile __u32   Resp0;
    volatile __u32   Resp1;
    volatile __u32   Force0;
    volatile __u32   Force1;
    volatile __u32   Priority[4];

} __standby_int_reg_t;



extern __s32 standby_check_nmi(void);


#endif  //_STANDBY_NMI_H_

