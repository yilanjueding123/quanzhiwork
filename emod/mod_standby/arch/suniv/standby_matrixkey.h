/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                               standby module
*
*                                   (c) Copyright 2011-2014, sunny China
*                                            All Rights Reserved
*
* File    : standby_matrixkey.h
* By      : sunny
* Version : V1.0
* Date    : 2011-5-11 9:27:10
*********************************************************************************************************
*/

#ifndef __STANDBY_MATRIXKEY_H___
#define __STANDBY_MATRIXKEY_H___

#include "ePDK.h"
#include "standby_cfg.h"
#include "standby_reg.h"
#include "standby_pio.h"

extern __s32 standby_matrixkey_init(void);
extern __s32 standby_matrixkey_exit(void);
extern __s32 standby_query_matrixkey(void);

#endif  //__STANDBY_MATRIXKEY_H___
