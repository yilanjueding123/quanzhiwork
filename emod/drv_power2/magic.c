/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                               hello world sample
*
*                                (c) Copyright 2006-2007, Steven.ZGJ China
*                                            All    Rights Reserved
*
* File    : pfhead.h
* By      : Steven.ZGJ
* Version : V1.00
*********************************************************************************************************
*/
#include "drv_power_i.h"

#pragma arm section rodata="MAGIC"
const __mods_mgsec_t modinfo =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'}, //.magic
    0x01000000,                                //.version
    EMOD_TYPE_DRV_POWER,                      //.type
    0xF0000,                                //.heapaddr
    0x400,                                  //.heapsize
    {
        //.mif
        &DRV_PowerMInit,
        &DRV_PowerMExit,
        &DRV_PowerMOpen,
        &DRV_PowerMClose,
        &DRV_PowerMRead,
        &DRV_PowerMWrite,
        &DRV_PowerMIoctrl
    }
};
#pragma arm section
