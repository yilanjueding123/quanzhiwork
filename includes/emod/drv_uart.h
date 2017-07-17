/*
****************************************************************************************************
*                                               MELIS
*                               the Easy Portable/Player Develop Kits
*                                           UART Driver
*
*                           (c) Copyright 2011-2014, All winners Co,Ld.
*                                       All Rights Reserved
*
* File    : drv_uart.h
* By      : james.deng
* Version : 1.0.0
* Date    : 2011-12-25
* Descript:
* Update  : <date>          <author>            <version>           <notes>
*           2011-12-25      james.deng          1.0.0               build the file.
****************************************************************************************************
*/

#ifndef __DRV_UART_H__
#define __DRV_UART_H__

typedef enum __UART_CMD
{
    UART_CMD_SET_PARA,
    UART_CMD_SET_BAUDRATE,
    UART_CMD_FLUSH
} __uart_cmd_t;

#endif // __DRV_UART_H__
