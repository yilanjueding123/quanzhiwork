/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Standby Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : mod_standby_i.h
* By      : kevin.z
* Version : v1.0
* Date    : 2010-7-8 19:03
* Descript:
* Update  : date                auther      ver     notes
*********************************************************************************************************
*/
#ifndef __MOD_STANDBY_I_H__
#define __MOD_STANDBY_I_H__

#include "ePDK.h"
#include "standby_cfg.h"


extern __sys_pwm_para_t    *pStandbyPar;

extern void enter_standby(void);

#endif  //__MOD_STANDBY_I_H__

