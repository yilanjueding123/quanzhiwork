/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                            power manager sub-system
*
*                                     (c) Copyright 2008-2009, kevin.z China
*                                              All Rights Reserved
*
* File   : standby_int.c
* Version: V1.0
* By     : kevin.z
* Date   : 2010-6-21 15:54
*********************************************************************************************************
*/
#include "standby_int.h"

static __u32    IntEnable0Reg, IntEnable1Reg, IntMask0Reg, IntMask1Reg;
static __standby_int_reg_t  *IntReg = (__standby_int_reg_t *)INT_REG_BASE;




/*
*********************************************************************************************************
*                                       STANDBY INTERRUPT INITIALISE
*
* Description: standby interrupt initialise.
*
* Arguments  : none.
*
* Returns    : EPDK_TRUE/EPDK_FALSE;
*********************************************************************************************************
*/
__s32 standby_int_init(void)
{
    IntReg = (__standby_int_reg_t *)INT_REG_BASE;

    //backup intterupt controller registers
    IntEnable0Reg = IntReg->Enable0;
    IntEnable1Reg = IntReg->Enable1;
    IntMask0Reg   = IntReg->Mask0;
    IntMask1Reg   = IntReg->Mask1;

    //enable valid wakeup source intterupts
    IntReg->Enable0 = (1 << INT_SOURCE_EXTNMI)    \
                      | (1 << INT_SOURCE_IR)        \
                      | (1 << INT_SOURCE_TIMER0)    \
                      | (1 << INT_SOURCE_LRADC)     \
                      | (1 << INT_SOURCE_USB0);

    IntReg->Enable1 = (1 << (INT_SOURCE_PIOD - 32));
    IntReg->Enable1 = (1 << (INT_SOURCE_PIOE - 32));
    IntReg->Enable1 = (1 << (INT_SOURCE_PIOF - 32));

    IntReg->Mask0 = ~ (
                        (1 << INT_SOURCE_EXTNMI)    \
                        | (1 << INT_SOURCE_IR)        \
                        | (1 << INT_SOURCE_TIMER0)    \
                        | (1 << INT_SOURCE_LRADC)     \
                        | (1 << INT_SOURCE_USB0)
                    );

    IntReg->Mask1 = ~(1 << (INT_SOURCE_PIOD - 32));
    IntReg->Mask1 = ~(1 << (INT_SOURCE_PIOE - 32));
    IntReg->Mask1 = ~(1 << (INT_SOURCE_PIOF - 32));

    return EPDK_TRUE;
}


/*
*********************************************************************************************************
*                                       STANDBY INTERRUPT INITIALISE
*
* Description: standby interrupt initialise.
*
* Arguments  : none.
*
* Returns    : EPDK_TRUE/EPDK_FALSE;
*********************************************************************************************************
*/
__s32 standby_int_exit(void)
{
    //restore intterupt controller registers
    IntReg->Enable0 = IntEnable0Reg;
    IntReg->Enable1 = IntEnable1Reg;
    IntReg->Mask0   = IntMask0Reg;
    IntReg->Mask1   = IntMask1Reg;

    return EPDK_TRUE;
}


/*
*********************************************************************************************************
*                                       QUERY INTERRUPT
*
* Description: query interrupt.
*
* Arguments  : src  interrupt source number.
*
* Returns    : EPDK_TRUE/EPDK_FALSE;
*********************************************************************************************************
*/
__s32 standby_query_int(__interrupt_source_e src)
{
    __s32   result = 0;
    __u32   tmpSrc = (__u32)src;

    if(tmpSrc >= 32)
    {
        tmpSrc -= 32;
        result = IntReg->Pending1 & (1 << tmpSrc);
    }
    else
    {
        result = IntReg->Pending0 & (1 << tmpSrc);
    }

    return result ? EPDK_TRUE : EPDK_FALSE;
}

