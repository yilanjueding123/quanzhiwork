/*
********************************************************************************************************************
*                                              USB Host Drvier
*
*                              (c) Copyright 2007-2009, softwinners.China
*										All	Rights Reserved
*
* File Name 	: usbh_debug.c
*
* Author 		: javen
*
* Version 		: 1.0
*
* Date 			: 2009.10.12
*
* Description 	:
*
* History 		:
*
********************************************************************************************************************
*/
#include  "usbh_debug.h"

#ifdef USBH_PF_DEBUG_GATE

__s32 usbh_debug_init(void)
{
    platform_debug_init();

    return 0;
}


__s32 usbh_debug_exit(void)
{
    platform_debug_exit();

    return 0;
}

#else

__s32 usbh_debug_init(void)
{
    return 0;
}


__s32 usbh_debug_exit(void)
{
    return 0;
}

#endif




