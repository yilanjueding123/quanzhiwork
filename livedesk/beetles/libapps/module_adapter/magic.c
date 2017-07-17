/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           module magic segment
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : magic.h
* By      : Andy.zhang
* Version : V1.00
*********************************************************************************************************
*/

#include "common_module_i.h"

#pragma arm section rodata="MAGIC"
const __mods_mgsec_t Common_modinfo =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'}, //.magic
    0x01000000,                             //.version
    EMOD_TYPE_USER,                         //.type
    0xF0000,                                //.heapaddr
    0x400,                                  //.heapsize
    {
        //.mif
        &Common_MInit,
        &Common_MExit,
        &Common_MOpen,
        &Common_MClose,
        &Common_MRead,
        &Common_MWrite,
        &Common_MIoctrl
    }
};
#pragma arm section




