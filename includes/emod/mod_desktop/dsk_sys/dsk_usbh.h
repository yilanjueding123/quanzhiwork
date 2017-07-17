/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: dsk_usbh.h
* By      	: Andy.zhang
* Func		: usb device control
* Version	: v1.0
* ============================================================================================================
* 2009-7-8 10:02:17  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __DSK_USBH_H__
#define __DSK_USBH_H__

__s32 dsk_send_insmod_usbh_cmd(void);
__s32 dsk_send_rmmod_usbh_cmd(void);

__bool dsk_usbh_is_working(void);
#endif /* __DSK_USBH_H__ */

