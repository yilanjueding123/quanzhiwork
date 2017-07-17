/*
*************************************************************************************
*                         		             Melis
*					                    USB Host Driver
*
*                          (c) Copyright 2006-2010, All winners Co,Ld.
*							           All Rights Reserved
*
* File Name 	: usbh_drv.h
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
#ifndef  __USBH_DRV_H__
#define  __USBH_DRV_H__

//---------------------------------------------------------------
//  数据结构 定义
//---------------------------------------------------------------
typedef struct usb_host_drv
{
    __mp my_mp;

    __u32 First_Plugin;         /* flag. 第一次接收到 plugin 命令 */
    __u32 ioctrl_counter;       /* ioctrl 的次数 */
} usb_host_drv_t;

//---------------------------------------------------------------
//  函数 定义
//---------------------------------------------------------------
__s32 host_drv_Init(__u32 usbc_no);
__s32 host_drv_Exit(__u32 usbc_no);

#endif   //__USBH_DRV_H__


