/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           hello world sample
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : pfhead.h
* By      : Steven.ZGJ
* Version : V1.00
*********************************************************************************************************
*/
#include "epdk.h"

extern __s32 shellmain(void *p_arg);
#pragma arm section rodata="MAGIC"
const __exec_mgsec_t exfinfo =
{
    {'e', 'P', 'D', 'K', '.', 'e', 'x', 'f'},		//.magic
    0x01000000,                				//.version
    0,                   	                //.type
    0x1F00000,                              //.heapaddr
    0x400,                      			//.heapsize
    shellmain,   							//.maintask
    0x4000,                      			//.mtskstksize
    KRNL_priolevel6                			//.mtskpriolevel
};
#pragma arm section

