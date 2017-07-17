/*
********************************************************************************************************************
*                                              USB Host Drvier
*
*                              (c) Copyright 2007-2009, softwinners.China
*										All	Rights Reserved
*
* File Name 	: usbh_debug.h
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
#ifndef  __USBH_DEBUG_H__
#define  __USBH_DEBUG_H__

#include  "hcd_debug.h"
#include  "platform_debug.h"

void print_buffer_1(__u8 *buffer, __u32 len, char *str);
void print_buffer_4(__u8 *buffer, __u32 len, char *str);


__s32 usbh_debug_init(void);
__s32 usbh_debug_exit(void);


#endif   //__USBH_DEBUG_H__

