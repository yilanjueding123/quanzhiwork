/*
*********************************************************************************************************
*											        ePDK
*						           the Easy Portable/Player Development Kits
*									           elibs sub-system
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : pub0.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#ifndef _PUB0_H_
#define _PUB0_H_

#include "pub0\elibs_cacheop.h"
#include "pub0\elibs_reg.h"
#include "pub0\usb_ops.h"
#include "pub0\script.h"

#ifdef __cplusplus
extern "C" {
#endif

/*critical operation    */
extern __u32 CPU_SR_Save(void);
extern void CPU_SR_Restore(__u32);
#define ENTER_CRITICAL(cpu_sr) {cpu_sr = CPU_SR_Save();}    /* enter critical area                     */
#define EXIT_CRITICAL(cpu_sr)  {CPU_SR_Restore(cpu_sr);}    /* exit critical area                      */

#ifdef __cplusplus
}
#endif

#endif /* end of stdio.h */
