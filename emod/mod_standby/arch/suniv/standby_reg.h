/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                               standby module
*
*                                   (c) Copyright 2008-2011, kevin China
*                                            All Rights Reserved
*
* File    : standby_reg.h
* By      : kevin
* Version : V1.0
* Date    : 2011-1-18 11:01
*********************************************************************************************************
*/
#ifndef __STANDBY_REG_H__
#define __STANDBY_REG_H__

#include "mod_standby_i.h"

#define STANDBY_RREG(reg)           (*(volatile __u32 *)(reg))
#define STANDBY_WREG(reg, val)      (*(volatile __u32 *)(reg) = val)

#define CCU_REG_BASE    (0xf1c20000)/*it is ok*/
#define TWI0_REG_BASE   (0xf1c27000)/*it is ok*/
#define TWI1_REG_BASE   (0xf1c27400)/*it is ok*/
#define TWI2_REG_BASE   (0xf1c27800)/*it is ok*/
#define UART0_REGS_BASE (0xf1c25000)/*it is ok*/
#define UART1_REGS_BASE (0xf1c25400)/*it is ok*/
#define UART2_REGS_BASE (0xf1c25800)/*it is ok*/
#define INT_REG_BASE    (0xf1c20400)/*it is ok*/
#define TMR_REG_BASE    (0xf1c20c00)/*it is ok*/
#define LRADC_REG_BASE  (0xf1c23400)/*it is ok*/
#define IR_REG_BASE     (0xf1c22c00)/*it is ok*/
#define DMA_REG_BASE    (0xf1c02000)/*it is ok*/
#define PIOC_REG_BASE   (0xf1c20800)/*it is ok*/


#define USB0_REG_BASE   (0xf1c13000)/*there is only one OTG*/


#endif  //__STANDBY_REG_H__

