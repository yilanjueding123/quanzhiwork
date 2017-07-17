/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Standby Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : mod_standby.c
* By      : kevin.z
* Version : v1.0
* Date    : 2010-7-8 18:55
* Descript:
* Update  : date                auther      ver     notes
*********************************************************************************************************
*/
#include "mod_standby_i.h"


//parameter for standby, it will be transfered from sys_pwm module
__sys_pwm_para_t  StandbyPar  = 0;
__sys_pwm_para_t *pStandbyPar = 0;


#pragma arm section code="ENTRY"

/*
*********************************************************************************************************
*                                   STANDBY MAIN PROCESS ENTRY
*
* Description: standby main process entry.
*
* Arguments  : pwm_para     pointer to the parameter that transfered from sys_pwm module.
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void sys_standby_entry(__sys_pwm_para_t *pwm_para)
{


    standby_puts("standby mod begin\n");


    if(!pwm_para)
    {
        //no parmaeter get, do nothing
        return;
    }


    pStandbyPar = &StandbyPar;


    //copy standby parameter from dram
    standby_memcpy(pStandbyPar, pwm_para, sizeof(__sys_pwm_para_t));
    //enter standby
    enter_standby();
    //copy standby parameter from sram
    standby_memcpy(pwm_para, pStandbyPar, sizeof(__sys_pwm_para_t));

    standby_puts("standby mod end\n");



    return;
}

