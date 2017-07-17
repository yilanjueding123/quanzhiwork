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
void axp199_set_voltage(__s32 type, __s32 voltage)
{
    __u8    tmpPmuRegVal = 0;
    __u8    tmpPmuRegAddr;
    __u8    tmpMask;
    __s32   tmpVoltage = 0;
    __s32   i;
    switch(type)
    {
    case POWER_VOL_DCDC1:
    {
        tmpPmuRegAddr = AXP19X_POWER_DC1OUT_VOL;
        tmpMask       = 0x80;
        break;
    }

    case POWER_VOL_DCDC2:
    {
        tmpPmuRegAddr = AXP19X_POWER_DC2OUT_VOL;
        tmpMask       = 0xc0;
        break;
    }

    case POWER_VOL_DCDC3:
    {
        tmpPmuRegAddr = AXP19X_POWER_DC3OUT_VOL;
        tmpMask       = 0x80;
        break;
    }

    default:
    {
        return;
    }
    }

    // set voltage
    if(voltage >= 700)
    {
        voltage = voltage - 700;
        while(voltage > 25)
        {
            voltage -= 25;
            tmpVoltage ++;
        }
    }
    twi_byte_rw(TWI_OP_RD, AXP19X_SLAVE_ADDR, tmpPmuRegAddr, &tmpPmuRegVal);
    while(tmpVoltage < (tmpPmuRegVal & (~tmpMask)))
    {
        tmpPmuRegVal--;
        twi_byte_rw(TWI_OP_WR, AXP19X_SLAVE_ADDR, tmpPmuRegAddr, &tmpPmuRegVal);
        for(i = 0; i < 2000; i++);
    }
    while(tmpVoltage > (tmpPmuRegVal & (~tmpMask)))
    {
        tmpPmuRegVal++;
        twi_byte_rw(TWI_OP_WR, AXP19X_SLAVE_ADDR, tmpPmuRegAddr, &tmpPmuRegVal);
        for(i = 0; i < 2000; i++);
    }

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
__u32 axp199_get_voltage(__s32 type)
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
__s32 axp199_low_power_dectect(void)
{
    __u8 temp[2];
    __u32 temp_bat_vol;
    twi_byte_rw(TWI_OP_RD, AXP19X_SLAVE_ADDR, AXP19X_POWER_BAT_VOL_H8, temp);
    twi_byte_rw(TWI_OP_RD, AXP19X_SLAVE_ADDR, AXP19X_POWER_BAT_VOL_L4, (temp + 1));
    temp_bat_vol = ((temp[0] << 4) + (temp[1] & 0x0f)) * 1.1;
    if(temp_bat_vol == 0)
    {
        //no battery, need not check low power
        return EPDK_FALSE;
    }

    if(temp_bat_vol < LOW_BAT_VOL)
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
__s32 axp199_ext_power_detect(void)
{
    __u8    temp = 0;
    twi_byte_rw(TWI_OP_RD, AXP19X_SLAVE_ADDR, AXP19X_POWER_STATUS, &temp);
    if((temp & 0x40) || (temp & 0x10))
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
__s32 axp199_power_key_detect(void)
{
    __u8    temp = 0;
    twi_byte_rw(TWI_OP_RD, AXP19X_SLAVE_ADDR, AXP19X_POWER_INTSTS3, &temp);
    if(temp & ((1 << 1) | (1 << 0)))
    {
        //clear power key
        twi_byte_rw(TWI_OP_WR, AXP19X_SLAVE_ADDR, AXP19X_POWER_INTSTS3, &temp);
        if(pStandbyPar->TvdFlag)
        {
            if(temp & (1 << 0))
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

