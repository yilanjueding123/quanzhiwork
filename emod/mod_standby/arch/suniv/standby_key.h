/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                               standby module
*
*                                   (c) Copyright 2008-2009, kevin China
*                                            All Rights Reserved
*
* File    : standby_key.h
* By      : kevin
* Version : V1.0
* Date    : 2009-6-16 17:33
*********************************************************************************************************
*/
#ifndef _STANDBY_KEY_H_
#define _STANDBY_KEY_H_

#include "ePDK.h"
#include "standby_cfg.h"
#include "standby_reg.h"


extern __s32 standby_key_init(void);
extern __s32 standby_key_exit(void);
extern __s32 standby_query_key(void);


#endif  //_STANDBY_KEY_H_

