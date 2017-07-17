/*
********************************************************************************
*                                USB Hid Driver
*
*                (c) Copyright 2006-2010, All winners Co,Ld.
*                        All Right Reserved
*
* FileName		:  usbMouse.h
*
* Author		:  Javen
*
* Date			:  2010.06.02
*
* Description	:  USB Mouse
*
* Others		:  NULL
*
* History:
*		<time> 		<version >		<author>	 	<desc>
*	   2010.06.02	   1.0			 Javen			build this file
*
********************************************************************************
*/
#ifndef  __USBMOUSE_H__
#define  __USBMOUSE_H__

#define  USB_HID_MOUSE_DATA_LEN		64    	/* 鼠标数据的最大长度 	*/
#define  USB_HID_MOUSE_DITHER_AREA	127  	/* 鼠标去抖动范围 		*/

/* mouse report */
#define  USB_HID_MOUSE_REPORT_BUTTON_LEFT_BIT	0	/* 鼠标左键 */
#define  USB_HID_MOUSE_REPORT_BUTTON_RIGHT_BIT	1	/* 鼠标右键 */
#define  USB_HID_MOUSE_REPORT_BUTTON_MIDDLE_BIT	2	/* 鼠标中键，中间滚轮向下产生的按键 */
#define  USB_HID_MOUSE_REPORT_BUTTON_SIDE_BIT	3
#define  USB_HID_MOUSE_REPORT_BUTTON_EXTRA_BIT	4


/* 从鼠标取回来数据的定义 */
typedef struct _usbMouseDataDef
{
    usbHidEvnetExcursion_t Button;	/* button 	*/
    usbHidEvnetExcursion_t X;		/* X 		*/
    usbHidEvnetExcursion_t Y;		/* Y 		*/
    usbHidEvnetExcursion_t Wheel;	/* Wheel 	*/
} usbMouseDataDef_t;

#define  USB_HID_DEV_MAGIC    0x5a13d099
typedef struct _usbMouse
{
    __u32 Magic;	             	/* 表示设备是否合法					*/

    HidDev_t *HidDev;
    __u32 DevNo;					/* 此设备号							*/
    __u8  ClassName[32];			/* 设备类名, 如"disk" 				*/
    __u8  DevName[32];				/* 此设备名, 如"SCSI_DISK_000"		*/

    /* Disk information */
    __u32 used;                     /* 打开设备计数 					*/
    __dev_devop_t MouseOp;			/* 设备操作函数 					*/

    /* Disk manager */
    USB_OS_HANDLE MouseParaHdle;	/* open时的句柄						*/
    USB_OS_HANDLE MouseRegHdle;		/* reg时的句柄 						*/

    __u8 Vendor[256];
    __u8 Product[256];
    __u8 Serial[32];

    __u8 MouseThdId;						/* 接收鼠标事件线程的ID 			*/
    USB_OS_KERNEL_EVENT *MouseThreadSemi;
    USB_OS_KERNEL_EVENT *notify_complete;	/* 同步thread创建/删除 				*/

    HidRequest_t HidReq;					/* hid传输请求 						*/
    __u8 Data[USB_HID_MOUSE_DATA_LEN];		/* 接收鼠标返回的数据 				*/
    usbMouseDataDef_t DataDef;				/* 鼠标数据的定义 					*/
    __u32 StopWork;							/* 鼠标停止工作。在close时调用 		*/

    /* test */
    __u32 USBMouseTestFlag;					/* 测试标志 */
    USBHMouseTest_t *USBHMouseTest;

    USBHMouseEvent_t MouseEvent;	  		/* 当前发给app的鼠标事件 */
    USBHMouse_CallBack CallBack;

    void *Extern;							/* 扩展结构，如MouseDriftControl_t */
} usbMouse_t;

#endif   //__USBMOUSE_H__

