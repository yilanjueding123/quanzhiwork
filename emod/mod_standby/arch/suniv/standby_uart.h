/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                             	Standby system
*
*                                    (c) Copyright 2012-2016, Sunny China
*                                             All Rights Reserved
*
* File    : standby_uart.h
* By      : Sunny
* Version : v1.0
* Date    : 2012-2-22
* Descript: uart operations for standby.
* Update  : date                auther      ver     notes
*           2012-2-22 15:24:27	Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __STANDBY_UART_H__
#define __STANDBY_UART_H__

#define UART_THR_REG (UART1_REGS_BASE+0x00)
#define UART_USR_REG (UART1_REGS_BASE+0x7c)

#define TXFIFO_FULL  (1<<1)
#define TXFIFO_EMPTY (1<<2)

void standby_putc(char c);
void standby_puts(const char *str);
void standby_put_uint(__u32 input);
void standby_put_hex(__u32 input);

#endif   //__STANDBY_UART_H__

