/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                            power manager sub-system
*
*                                     (c) Copyright 2008-2009, kevin.z China
*                                              All Rights Reserved
*
* File   : power.c
* Version: V1.0
* By     : kevin.z
* Date   : 2009-6-11 8:55
*********************************************************************************************************
*/
#include "standby_i.h"
#include "standby_twi.h"


//==============================================================================
// LOW POWER CHECK FOR SYSTEM STANDBY
//==============================================================================


/*
*********************************************************************************************************
*                                     SET VOLTAGE FOR STANDBY
*
* Description: set voltage for standby.
*
* Arguments  : type     voltage type, dc1/dc2/dc3/...
*              voltage  voltage value;
*
* Returns    : none;
*********************************************************************************************************
*/
void axp189_set_voltage(__s32 type, __s32 voltage)
{
    return;
}


/*
*********************************************************************************************************
*                                     GET VOLTAGE FOR STANDBY
*
* Description: get voltage for standby.
*
* Arguments  : type     voltage type, dc1/dc2/dc3/...
*
* Returns    : voltage value;
*********************************************************************************************************
*/
__u32 axp189_get_voltage(__s32 type)
{
    return 0;
}


/*
*********************************************************************************************************
*                                     LOW POWER DECTECT FOR STANDBY
*
* Description: low power dectect for standby.
*
* Arguments  : none
*
* Returns    : result;
*               EPDK_TRUE   got low power;
*               EPDK_FALSE  not low power;
*********************************************************************************************************
*/
__s32 axp189_low_power_dectect(void)
{
    __u16    temp = 0;

    twi_byte_rw(TWI_OP_RD, P1_SLAVE_ADDR, POWER_BATTERY_VOL, (__u8 *)&temp);
    temp *= 8;
    if(temp == 0)
    {
        //no battery, need not check low power
        return EPDK_FALSE;
    }

    temp += 2500;
    if(temp < LOW_BAT_VOL)
    {
        //battery low power happened
        return EPDK_TRUE;
    }

    return EPDK_FALSE;
}


/*
*********************************************************************************************************
*                                     EXTERNAL POWER DECTECT FOR STANDBY
*
* Description: external power dectect for standby.
*
* Arguments  : none
*
* Returns    : result;
*               EPDK_TRUE   find external power;
*               EPDK_FALSE  no external power;
*********************************************************************************************************
*/
__s32 axp189_ext_power_detect(void)
{
    __u8    temp = 0;

    twi_byte_rw(TWI_OP_RD, P1_SLAVE_ADDR, POWER_STATUS, &temp);
    if(temp & 0x10)
    {
        return EPDK_TRUE;
    }

    return EPDK_FALSE;
}


/*
*********************************************************************************************************
*                                     STANDBY POWER KEY DETECT
*
* Description: standby power key dectect.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/
__s32 axp189_power_key_detect(void)
{
    __u8    temp = 0;

    twi_byte_rw(TWI_OP_RD, P1_SLAVE_ADDR, POWER_INTSTS2, &temp);
    if(temp & ((1 << 2) | (1 << 3)))
    {
        //clear power key
        twi_byte_rw(TWI_OP_WR, P1_SLAVE_ADDR, POWER_INTSTS2, &temp);
        if(pStandbyPar->TvdFlag)
        {
            if(temp & (1 << 2))
            {
                return EPDK_TRUE;
            }
            else
            {
                return EPDK_FALSE;
            }
        }
        return EPDK_TRUE;
    }

    return EPDK_FALSE;
}

