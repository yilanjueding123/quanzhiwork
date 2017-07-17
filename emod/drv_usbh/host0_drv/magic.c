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
#include "..\usb_host_config.h"
#include "host0_drv.h"

#pragma arm section rodata="MAGIC"
const __mods_mgsec_t modinfo =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'},		//.magic
    0x01000000,                				//.version
    EMOD_TYPE_DRV_USBH_BASE,               //.type
    0xF0000,                                //.heapaddr
    0x400,                      			//.heapsize
    {
        //.mif
        &usb_host0_Init,
        &usb_host0_Exit,
        &usb_host0_Open,
        &usb_host0_Close,
        &usb_host0_Read,
        &usb_host0_Write,
        &usb_host0_Ioctrl
    }
};
#pragma arm section
