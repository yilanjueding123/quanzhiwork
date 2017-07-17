/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                            power manager sub-system
*
*                                     (c) Copyright 2008-2009, kevin.z China
*                                              All Rights Reserved
*
* File   : standby_nmi.c
* Version: V1.0
* By     : kevin.z
* Date   : 2010-6-21 15:54
*********************************************************************************************************
*/
#include "standby_nmi.h"


#if(ALLOW_NMI_WAKEUP_STANDBY)

static __standby_int_reg_t  *IntReg = (__standby_int_reg_t *)INT_REG_BASE;


/*
*********************************************************************************************************
*                                     CHECK NMI INTERRUPT
*
* Description: check nmi interrupt.
*
* Arguments  : none
*
* Returns    : EPDK_TRUE/EPDK_FALSE;
*********************************************************************************************************
*/
__s32 standby_check_nmi(void)
{
    __u32   tmpReg = IntReg->Pending0;

    return tmpReg &= 0x01 << 0;
}

#endif  //ALLOW_NMI_WAKEUP_STANDBY

