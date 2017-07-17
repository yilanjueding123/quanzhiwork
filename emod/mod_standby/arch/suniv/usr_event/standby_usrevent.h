/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               standby Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : standby_usrevent.c
* By      : kevin.z
* Version : v1.0
* Date    : 2010-7-8 13:08
* Descript: wakeup systen standby by user defined event.
* Update  : date                auther      ver     notes
*********************************************************************************************************
*/
#ifndef _STANDBY_USRENENT_H_
#define _STANDBY_USRENENT_H_

#include "ePDK.h"
#include "standby_cfg.h"

extern __s32 standby_usrevent_init(void);
extern __s32 standby_usrevent_exit(void);
extern __s32 standby_usrevent_query(void);

#endif  //_STANDBY_USRENENT_H_


