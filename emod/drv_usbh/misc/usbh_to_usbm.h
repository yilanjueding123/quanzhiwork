/*
********************************************************************************************************************
*                                              usb device
*
*                              (c) Copyright 2007-2009, javen.China
*										All	Rights Reserved
*
* File Name 	: usbd_to_usbm.h
*
* Author 		: javen
*
* Version 		: 1.0
*
* Date 			: 2009.05.27
*
* Description 	: usbd和usb_monitor的交互窗口
*
* History 		:
*
********************************************************************************************************************
*/
#ifndef    __USBD_MESSAGE_H__
#define    __USBD_MESSAGE_H__

//------------------------------------------------
s32 init_usbm_op(void);
s32 exit_usbm_op(void);
s32 usbm_sendcmd(u32 cmd, void *p_buff);

#endif     //__USBD_MESSAGE_H__
