/*
****************************************************************************************************
*                                               MELIS
*                               the Easy Portable/Player Develop Kits
*                                           UART Driver
*
*                           (c) Copyright 2011-2014, All winners Co,Ld.
*                                       All Rights Reserved
*
* File    : drv_uart_i.h
* By      : james.deng
* Version : 1.0.0
* Date    : 2011-12-24
* Descript:
* Update  : <date>          <author>            <version>           <notes>
*           2011-12-24      james.deng          1.0.0               build the file.
****************************************************************************************************
*/

#ifndef __DRV_UART_I_H__
#define __DRV_UART_I_H__

#include  "epdk.h"


// uart soft buffer size for reading
#define UART_BUFFER_SIZE                4096

typedef struct __DRV_UART
{
    __u32 mid;
    __u32 used;
    __u32 busycnt;
} __drv_uart_t;

typedef struct __DEV_UART
{
    __u32  port;
    __u32  baudrate;
    __hdle hdle;
    __u32  used;
    __hdle hReg;
} __dev_uart_t;

// define at dev_uart.c
extern __dev_devop_t uart_devop;

// define at drv_uart.c
extern __dev_uart_t dev_uart[UART_PORT_COUNT];

__s32 DRV_UART_MInit(void);
__s32 DRV_UART_MExit(void);
__mp  *DRV_UART_MOpen(__u32 mid, __u32 mod);
__s32 DRV_UART_MClose(__mp *mp);
__u32 DRV_UART_MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 DRV_UART_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
__s32 DRV_UART_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);

// define at clk_for_uart.c
__s32 uart_pm_init(__u32 nPort);
__s32 uart_pm_exit(__u32 nPort);
__s32 uart_lock(__u32 nPort);
__s32 uart_unlock(__u32 nPort);

#endif // __DRV_UART_I_H__
