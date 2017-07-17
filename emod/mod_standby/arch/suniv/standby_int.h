/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                               standby module
*
*                                   (c) Copyright 2008-2011, kevin China
*                                            All Rights Reserved
*
* File    : standby_int.h
* By      : kevin
* Version : V1.0
* Date    : 2011-1-26 14:11
*********************************************************************************************************
*/
#ifndef __STANDBY_INT_H__
#define __STANDBY_INT_H__

#include "ePDK.h"
#include "standby_cfg.h"
#include "standby_reg.h"


typedef enum __INTERRUPT_SOURCE
{
    INT_SOURCE_EXTNMI   = 0,
    INT_SOURCE_UART0    = 1,
    INT_SOURCE_UART1    = 2,
    INT_SOURCE_UART2    = 3,
    INT_SOURCE_SPDIF    = 5,
    INT_SOURCE_IR       = 6,
    INT_SOURCE_TWI0     = 7,
    INT_SOURCE_TWI1     = 8,
    INT_SOURCE_TWI2     = 9,
    INT_SOURCE_SPI0     = 10,
    INT_SOURCE_SPI1     = 11,
    INT_SOURCE_TIMER0   = 13,
    INT_SOURCE_TIMER1   = 14,
    INT_SOURCE_TIMER2   = 15,
    INT_SOURCE_TOUCHPNL = 20,
    INT_SOURCE_LRADC    = 22,
    INT_SOURCE_USB0     = 26,
    INT_SOURCE_PIOD     = 38,
    INT_SOURCE_PIOE     = 39,
    INT_SOURCE_PIOF     = 40,


} __interrupt_source_e;


typedef struct __STANDBY_INT_REG
{
    volatile __u32   Vector;       /*0x00*/
    volatile __u32   BaseAddr;     /*0x04*/
    volatile __u32   reserved0;    /*0x08*/
    volatile __u32   NmiIntCtrl;   /*0x0c*/
    volatile __u32   Pending0;     /*0x10*/
    volatile __u32   Pending1;     /*0x14*/
    volatile __u32   reserved1[2]; /*0x18*/
    volatile __u32   Enable0;      /*0x20*/
    volatile __u32   Enable1;      /*0x24*/
    volatile __u32   reserved2[2]; /*0x28*/
    volatile __u32   Mask0;        /*0x30*/
    volatile __u32   Mask1;        /*0x34*/
    volatile __u32   reserved3[2]; /*0x38*/
    volatile __u32   Resp0;        /*0x40*/
    volatile __u32   Resp1;        /*0x44*/
    volatile __u32   reserved4[2]; /*0x48*/
    volatile __u32   Force0;       /*0x50*/
    volatile __u32   Force1;       /*0x54*/
    volatile __u32   reserved5[2]; /*0x58*/
    volatile __u32   Priority[4];  /*0x60*/

} __standby_int_reg_t;



extern __s32 standby_int_init(void);
extern __s32 standby_int_exit(void);
extern __s32 standby_query_int(__interrupt_source_e src);


#endif  //__STANDBY_INT_H__

