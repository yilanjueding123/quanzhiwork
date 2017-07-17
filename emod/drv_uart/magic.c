/*
****************************************************************************************************
*                                               MELIS
*                               the Easy Portable/Player Develop Kits
*                                           UART Driver
*
*                           (c) Copyright 2011-2014, All winners Co,Ld.
*                                       All Rights Reserved
*
* File    : magic.c
* By      : james.deng
* Version : 1.0.0
* Date    : 2011-12-24
* Descript:
* Update  : <date>          <author>            <version>           <notes>
*           2011-12-24      james.deng          1.0.0               build the file.
****************************************************************************************************
*/

#include "drv_uart_i.h"

#pragma arm section rodata="MAGIC"
const __mods_mgsec_t modinfo =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'}, //.magic
    0x01000000,                         //.version
    EMOD_TYPE_DRV_UART,                 //.type
    0xF0000,                            //.heapaddr
    0x400,                              //.heapsize
    {
        //.mif
        &DRV_UART_MInit,
        &DRV_UART_MExit,
        &DRV_UART_MOpen,
        &DRV_UART_MClose,
        &DRV_UART_MRead,
        &DRV_UART_MWrite,
        &DRV_UART_MIoctrl
    }
};
#pragma arm section
