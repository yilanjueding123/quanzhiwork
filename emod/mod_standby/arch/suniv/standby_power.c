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


void  (*standby_set_voltage         )(__s32 type, __s32 voltage);
__u32 (*standby_get_voltage         )(__s32 type);
__s32 (*standby_low_power_dectect   )(void);
__s32 (*standby_ext_power_detect    )(void);
__s32 (*standby_power_key_detect    )(void);


//==============================================================================
// POWER CHECK FOR SYSTEM STANDBY
//==============================================================================



/*
*********************************************************************************************************
*                                     INIT POWER FOR STANDBY
*
* Description: init power for standby.
*
* Arguments  : none;
*
* Returns    : result;
*********************************************************************************************************
*/
__s32 standby_power_init(void)
{
    if(pStandbyPar->PowerCfg.power_exist == 2)
    {
        //pmu is axp199
        standby_set_voltage       = axp199_set_voltage;
        standby_get_voltage       = axp199_get_voltage;
        standby_low_power_dectect = axp199_low_power_dectect;
        standby_ext_power_detect  = axp199_ext_power_detect;
        standby_power_key_detect  = axp199_power_key_detect;
    }
    else
    {
        //pmu is axp189
        standby_set_voltage       = axp189_set_voltage;
        standby_get_voltage       = axp189_get_voltage;
        standby_low_power_dectect = axp189_low_power_dectect;
        standby_ext_power_detect  = axp189_ext_power_detect;
        standby_power_key_detect  = axp189_power_key_detect;
    }
    return 0;
}


/*
*********************************************************************************************************
*                                     EXIT POWER FOR STANDBY
*
* Description: exit power for standby.
*
* Arguments  : none;
*
* Returns    : result;
*********************************************************************************************************
*/
__s32 standby_power_exit(void)
{
    return 0;
}

