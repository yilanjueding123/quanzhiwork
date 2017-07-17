/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                            Bios Interface Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : ebios.h
* By      : kevin.z
* Version : v2.0
* Date    : 2010-11-27 9:37
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-27 9:37     kevin.z     2.0     build the file;
*********************************************************************************************************
*/
#ifndef _EBIOS_H_
#define _EBIOS_H_

#include "typedef.h"
#include "cfgs.h"

#include "arch\\arm9.h"

#if(EPDK_ARCH == EPDK_ARCH_SUNIII)
#include "arch\\suniii\\csp.h"
#include "arch\\suniii\\bsp.h"
#endif

#if(EPDK_ARCH == EPDK_ARCH_SUNIV)
#include "arch\\suniv\\csp.h"
#include "arch\\suniv\\bsp.h"
#endif

#endif /*#ifndef EBIOS_H*/
