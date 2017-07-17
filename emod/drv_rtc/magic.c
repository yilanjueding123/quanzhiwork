/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                            device monitor module
*
*                                   (c) Copyright 2006-2010, kevin.z.m China
*                                             All Rights Reserved
*
* File    : magic.c
* By      : kevin.z.m
* Version : v1.00
* Date    : 2010-6-20 15:15
*********************************************************************************************************
*/
#include  "drv_rtc_i.h"

#pragma arm section rodata="MAGIC"
const __mods_mgsec_t modinfo =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'}, //.magic
    0x01000000,                             //.version
    EMOD_TYPE_DRV_RTC,                      //.type
    0xF0000,                                //.heapaddr
    0x400,                                  //.heapsize
    {
        //.mif
        &MInit,
        &MExit,
        &MOpen,
        &MClose,
        &MRead,
        &MWrite,
        &MIoctrl
    }
};
#pragma arm section
