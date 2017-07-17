/*
*************************************************************************************
*                         		             Melis
*					                    USB Host Driver
*
*                          (c) Copyright 2006-2010, All winners Co,Ld.
*							           All Rights Reserved
*
* File Name 	: host0_drv.h
*
* Author 		: javen
*
* Description 	:
*
* History 		:
*      <author>    		<time>       	<version >    		<desc>
*       javen     	  2010-12-8            1.0          create this file
*
*************************************************************************************
*/
#ifndef  __HOST0_DRV_H__
#define  __HOST0_DRV_H__

//---------------------------------------------------------------
//  º¯Êý ¶¨Òå
//---------------------------------------------------------------
__s32 usb_host0_Init(void);
__s32 usb_host0_Exit(void);
__mp  *usb_host0_Open(__u32 mid, __u32 mode);
__s32 usb_host0_Close(__mp *mp);
__u32 usb_host0_Read(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 usb_host0_Write(const void *pdata, __u32 size, __u32 n, __mp *mp);
__s32 usb_host0_Ioctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);

#endif   //__HOST0_DRV_H__

