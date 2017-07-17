/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           hello world sample
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : magic.c
* By      : Steven.ZGJ
* Version : V1.00
*********************************************************************************************************
*/
#include "drv_csi_i.h"

#pragma arm section rodata="MAGIC"
const __mods_mgsec_t modinfo =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'},		//.magic
    0x01000000,                				//.version
#if CSI_PORT==0
    EMOD_TYPE_DRV_CSI,                      //.type
#else
    EMOD_TYPE_DRV_CSI1,
#endif
    0xF0000,                                //.heapaddr
    0x400,                      			//.heapsize
    {
        //.mif
        &DRV_CSI_MInit,
        &DRV_CSI_MExit,
        &DRV_CSI_MOpen,
        &DRV_CSI_MClose,
        &DRV_CSI_MRead,
        &DRV_CSI_MWrite,
        &DRV_CSI_MIoctrl
    }
};
#pragma arm section
