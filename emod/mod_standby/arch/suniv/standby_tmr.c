/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                            power manager sub-system
*
*                                     (c) Copyright 2008-2009, kevin.z China
*                                              All Rights Reserved
*
* File   : standby_tmr.c
* Version: V1.0
* By     : kevin.z
* Date   : 2009-7-22 18:31
*********************************************************************************************************
*/
#include "standby_tmr.h"


static __standby_tmr_reg_t  *TmrReg = (__standby_tmr_reg_t *)TMR_REG_BASE;
static __u32 TmrIntCtl;
static __u32 Tmr0Ctl, Tmr0IntVal, Tmr0CntVal;
static __u32 Tmr1Ctl, Tmr1IntVal, Tmr1CntVal;
static __u32 Tmr2Ctl, Tmr2IntVal, Tmr2CntVal;


/*
*********************************************************************************************************
*                                     TIMER INIT
*
* Description: initialise timer for standby.
*
* Arguments  : none
*
* Returns    : EPDK_TRUE/EPDK_FALSE;
*********************************************************************************************************
*/
__s32 standby_tmr_init(void)
{
    //set timer register base
    TmrReg = (__standby_tmr_reg_t *)TMR_REG_BASE;

    //backup timer0 registers
    Tmr0Ctl     = TmrReg->Tmr0Ctl;
    Tmr0IntVal  = TmrReg->Tmr0IntVal;
    Tmr0CntVal  = TmrReg->Tmr0CntVal;
    TmrIntCtl   = TmrReg->IntCtl;

#if 1
    TmrReg->Tmr0Ctl &= ~(1 << 2);
    TmrReg->Tmr0Ctl |=  (0 << 2);

    TmrReg->Tmr0Ctl &= ~(7 << 4);
    TmrReg->Tmr0Ctl |=  (5 << 4);

    TmrReg->Tmr0CntVal = 0;
    TmrReg->Tmr0IntVal = 10;
#endif

    //clear interrupt
    TmrReg->IntSta = 1;

    return EPDK_TRUE;
}


/*
*********************************************************************************************************
*                                     TIMER EXIT
*
* Description: exit timer for standby.
*
* Arguments  : none
*
* Returns    : EPDK_TRUE/EPDK_FALSE;
*********************************************************************************************************
*/
__s32 standby_tmr_exit(void)
{
    //restore timer0 registers
    TmrReg->Tmr0IntVal  = Tmr0IntVal;
    TmrReg->Tmr0CntVal  = Tmr0CntVal;
    TmrReg->Tmr0Ctl     = Tmr0Ctl;
    TmrReg->IntCtl      = TmrIntCtl;
    return EPDK_TRUE;
}


/*
*********************************************************************************************************
*                                     TIMER QUERY
*
* Description: standby query timer.
*
* Arguments  : type     type of timer for query.
*
* Returns    : EPDK_TRUE/EPDK_FALSE;
*********************************************************************************************************
*/
__s32 standby_tmr_query(void)
{
    //query timer1 interrupt
    if(TmrReg->IntSta & (1 << 0))
    {
        //clear interrupt
        TmrReg->IntSta = 1;
        return EPDK_TRUE;
    }

    return EPDK_FALSE;
}


/*
**********************************************************************************************************************
*                                               standby_clear_dog
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__u32 standby_clear_dog(void)
{
    //clear watch-dog
    TmrReg->WdogCtrl = (0xA57 << 1) | (1 << 0);

    return EPDK_OK;
}

