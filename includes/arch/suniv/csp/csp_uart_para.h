/*
*******************************************************************************
*           				eBase
*                 the Abstract of Hardware
*
*
*              (c) Copyright 2006-2010, ALL WINNER TECH.
*           								All Rights Reserved
*
* File     :  D:\winners\eBase\eBSP\CSP\sun_20\SW_UART\csp_uart_para.h
* Date     :  2010/11/29 9:33
* By       :  Sam.Wu
* Version  :  V1.00
* Description :
* Update   :  date      author      version
*
* notes    :
*******************************************************************************
*/
#ifndef _CSP_UART_PARA_H
#define _CSP_UART_PARA_H

#ifndef CSP_UART_PORT_NUM
#define CSP_UART_PORT_NUM  1 //only one port supported in csp
#endif //#ifndef UART_PORT_NUM


typedef struct _CSP_UART_port_paras
{
    u8  isFifoMode;//1/0 to enable/disable the hardware port FIFO mode. enable is recommended.
    u8  dataBitsLen;//data bits length, 5~8.
    u8  parityCheck;//0~2, 0 to disable parity check , 1 is odd parity check, 2 is even parity check.
    u8  stopBitsLen;///stop bits length, only support two value: 1 and 2
    u32 baudrate;
} CSP_UART_PortParas_t;



#endif //#ifndef _CSP_UART_PARA_H




