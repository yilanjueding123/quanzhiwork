/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Standby Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : standby_cfg.h
* By      : kevin.z
* Version : v1.0
* Date    : 2010-7-8 13:14
* Descript: config for standby.
* Update  : date                auther      ver     notes
*********************************************************************************************************
*/
#ifndef _STANDBY_CFG_H_
#define _STANDBY_CFG_H_

#include "mod_standby_i.h"

//config wakeup source for standby
#define ALLOW_IR_WAKEUP_STANDBY         (0)     // if allow ir wakeup standby
#define ALLOW_USB_WAKEUP_STANDBY        (1)     // if allow usb plug-in/out wakeup standby
#define ALLOW_LRADCKEY_WAKEUP_STANDBY   (0)     // if allow lradc key wakeup standby
#define ALLOW_POWERKEY_WAKEUP_STANDBY   (0)     // if allow power key wakeup standby
#define ALLOW_NMI_WAKEUP_STANDBY        (0)     // if allow nmi wakeup standby
#define ALLOW_ALARM_WAKEUP_STANDBY      (0)     // if allow alarm wakeup standby
#define ALLOW_USREVENT_WAKEUP_STANDBY   (0)     // if allow user event wakeup standby
#define ALLOW_MATRIXKEY_WAKEUP_STANDBY  (0)     // if allow matrix key wakeup standby
#define ALLOW_EINT_WAKEUP_STANDBY       (1)     // if allow eint wakeup standby
#define ALLOW_TP_KEY_WAKEUP_STANDBY     (0)	// if allow tp key wakeup standby
#endif  //_STANDBY_CFG_H_

