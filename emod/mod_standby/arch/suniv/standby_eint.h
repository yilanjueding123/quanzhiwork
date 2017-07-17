/*
*********************************************************************************************************
*                                                    ePOS
*                                   the Easy Portable/Player Operation System
*                                        architecture related source file
*
*                                      (c) Copyright 2009-2010, jerry China
*                                            All Rights Reserved
*
* File    : standby_ir.h
* By      : jerry
* Version : V1.00
*********************************************************************************************************
*/

#ifndef  _STANDBY_EINT_H_
#define  _STANDBY_EINT_H_

#include "epdk.h"
#include "standby_cfg.h"
#include "standby_reg.h"

enum trigger_condition_e
{
    TRIGGER_POSITIVE_EDGE = 0,
    TRIGGER_NEGATIVE_EDGE = 1,
    TRIGGER_HIGH_LEVEL = 2,
    TRIGGER_LOW_LEVEL = 3,
    TRIGGER_DOUBLE_EDGE = 4,
};

extern __s32 standby_eint_init( __u32 eint_no, enum trigger_condition_e trigger_condition, __u32 port);
extern __s32 standby_eint_exit( __u32 eint_no, enum trigger_condition_e trigger_condition, __u32 port);
extern __s32 standby_query_eint( __u32 eint_no, __u32 port);
__s32 standby_clear_eint( __u32 eint_no, __u32 port);

#endif  /*_STANDBY_EINT_H_*/
