/*
**********************************************************************************************************************
*											        ePOS
*								the Easy Portable/Player Operation System
*									           Krnl sub-system
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : sys_hwsc.h
* Version: V1.0
* By     : steven.zgj
**********************************************************************************************************************
*/
#ifndef _SYS_HWSC_H_
#define _SYS_HWSC_H_

#include "sys_dev.h"

/* hwsc - USB */
#define DEV_IOC_USR_HWSC_USBD_SUSPEND			(DEV_IOC_USR_BASE + 200)    /* 保留                                         */
#define DEV_IOC_USR_HWSC_USBD_RESET			    (DEV_IOC_USR_BASE + 201)    /* 保留                                         */
#define DEV_IOC_USR_HWSC_USBD_START			    (DEV_IOC_USR_BASE + 202)    /* 保留                                         */
#define DEV_IOC_USR_HWSC_USB_AUDIO_START		(DEV_IOC_USR_BASE + 203)    /* 保留                                         */


#define DEV_IOC_USR_HWSC_USBH_DISCONNECT		(DEV_IOC_USR_BASE + 208)    /* 保留                                         */
#define DEV_IOC_USR_HWSC_USBH_CONNECT			(DEV_IOC_USR_BASE + 209)    /* 保留                                         */
#define DEV_IOC_USR_HWSC_USBH_REMOVE			(DEV_IOC_USR_BASE + 210)    /* 保留                                         */

#define DEV_IOC_USR_HWSC_APP_INSMOD_USBH		(DEV_IOC_USR_BASE + 216)    /* 应用程序加载 Deivce Driver                   */
#define DEV_IOC_USR_HWSC_APP_RMMOD_USBH			(DEV_IOC_USR_BASE + 217)    /* 应用程序卸载 Deivce Driver                   */
#define DEV_IOC_USR_HWSC_APP_INSMOD_USBD		(DEV_IOC_USR_BASE + 218)    /* 应用程序加载 Host Driver                     */
#define DEV_IOC_USR_HWSC_APP_RMMOD_USBD			(DEV_IOC_USR_BASE + 219)    /* 应用程序卸载 Host Driver                     */
#define DEV_IOC_USR_HWSC_APP_DRV_NULL			(DEV_IOC_USR_BASE + 220)    /* 保留                                         */
#define DEV_IOC_USR_HWSC_TV_INSMOD_USBH			(DEV_IOC_USR_BASE + 221)    /* 保留                                         */
#define DEV_IOC_USR_HWSC_TV_RMMOD_USBH			(DEV_IOC_USR_BASE + 222)    /* 保留                                         */

#define DEV_IOC_USR_HWSC_GET_USB_INFO			(DEV_IOC_USR_BASE + 224)    /* 获得当前USB驱动的类型，即device模式、Host模式 */
#define DEV_IOC_USR_HWSC_USBH_INSMOD_STATUS     (DEV_IOC_USR_BASE + 225)    /* Host Driver是否加载？                        */

#define DEV_IOC_USR_HWSC_USBH_MSC_DEV_REG_SET   (DEV_IOC_USR_BASE + 233)    /* 保留                                         */
#define DEV_IOC_USR_HWSC_USBH_MSC_DEV_REG_GET   (DEV_IOC_USR_BASE + 234)    /* 保留                                         */
#define DEV_IOC_USR_START_USB_MONITOR           (DEV_IOC_USR_BASE + 235)    /* 系统保留，开启USB自动检测                    */
#define DEV_IOC_USR_STOP_USB_MONITOR            (DEV_IOC_USR_BASE + 236)    /* 系统保留，关闭USB自动检测                    */
#define DEV_IOC_USR_GET_USB_CHARGE_SOURCE       (DEV_IOC_USR_BASE + 237)    /* 获得USB供电类型，PC供电，USB火牛充电？       */

#define DEV_IOC_USR_HWSC_SET_USBH_WORK_STATUS   (DEV_IOC_USR_BASE + 238)    /* 保留                                         */
#define DEV_IOC_USR_HWSC_GET_USBH_WORK_STATUS   (DEV_IOC_USR_BASE + 239)    /* 检测USB Host driver是否在工作？              */
#define DEV_IOC_USR_IS_NEED_APP_INSMOD_USBH     (DEV_IOC_USR_BASE + 240)    /* 保留, 是否需要应用程序加载Host Driver?       */
#define DEV_IOC_USR_GET_CURRENT_PORT            (DEV_IOC_USR_BASE + 241)    /* 保留, 获得USB的端口号                        */

/* for suniii 的 USB 控制器 1 和 USB 控制器 2 */
#define DEV_IOC_USR_HWSC_APP_INSMOD_USBH_1      (DEV_IOC_USR_BASE + 242)    /* 应用程序加载 usbc1 的 Host Driver            */
#define DEV_IOC_USR_HWSC_APP_RMMOD_USBH_1       (DEV_IOC_USR_BASE + 243)    /* 应用程序卸载 usbc1 的 Host Driver            */
#define DEV_IOC_USR_HWSC_APP_INSMOD_USBH_2      (DEV_IOC_USR_BASE + 244)    /* 应用程序加载 usbc2 的 Host Driver            */
#define DEV_IOC_USR_HWSC_APP_RMMOD_USBH_2       (DEV_IOC_USR_BASE + 245)    /* 应用程序卸载 usbc2 的 Host Driver            */
#define DEV_IOC_USR_HWSC_SET_USBH1_WORK_STATUS  (DEV_IOC_USR_BASE + 246)    /* 设置 usbc1 Host Driver 的工作状态            */
#define DEV_IOC_USR_HWSC_GET_USBH1_WORK_STATUS  (DEV_IOC_USR_BASE + 247)    /* 查询 usbc1 Host Driver 的工作状态            */
#define DEV_IOC_USR_HWSC_SET_USBH2_WORK_STATUS  (DEV_IOC_USR_BASE + 248)    /* 设置 usbc2 Host Driver 的工作状态            */
#define DEV_IOC_USR_HWSC_GET_USBH2_WORK_STATUS  (DEV_IOC_USR_BASE + 249)    /* 查询 usbc2 Host Driver 的工作状态            */

//define command to enable or disable device monitor
#define DEV_IOC_USR_HWSC_ENABLE_MONITOR         (DEV_IOC_USR_BASE + 250)
#define DEV_IOC_USR_HWSC_DISABLE_MONITOR        (DEV_IOC_USR_BASE + 251)

//设置usbd的模式:目前有两种模式udisk or uvc
#define DEV_IOC_USR_SET_USBD_MODE            	(DEV_IOC_USR_BASE + 252)
#define DEV_IOC_USR_ADD_USBD_MODE            	(DEV_IOC_USR_BASE + 253)
#define DEV_IOC_USR_RMMOD_USBD_DEV            	(DEV_IOC_USR_BASE + 254)

#define DEV_IOC_USR_HWSC_SET_USB_HOST_SPEED		(DEV_IOC_USR_BASE + 300)
#define DEV_IOC_USR_HWSC_QUERY_USB_HOST_SPEED	(DEV_IOC_USR_BASE + 301)

#define DEV_IOC_USR_HWSC_SET_USB_AUDIO_WORK_STATUS   (DEV_IOC_USR_BASE + 302)    /* 保留                                         */

#define DEV_IOC_USR_INIT_HOST_CONTROLER     (DEV_IOC_USR_BASE + 303)
#define DEV_IOC_USR_DEINIT_HOST_CONTROLER     (DEV_IOC_USR_BASE + 304)
#define DEV_IOC_USR_ENABLE_SD_MMC_CHECK      (DEV_IOC_USR_BASE + 305)//默认不检测卡，需调用enable后开始检测卡



//-----------------------------------------------------------------------------
//  usb device aux
//  mass storage: 0x00 ~ 0x99
//  vedio class:  0x100 ~ 0x199
//  audio class:  0x200 ~ 0x299
//-----------------------------------------------------------------------------
#define DEV_IOC_USR_USBD_AUX_MSC    0x00
#define DEV_IOC_USR_USBD_AUX_UVC    0x100
#define DEV_IOC_USR_USBD_AUX_UAC    0x200

//-----------------------------------------------
//   sd add by weiziheng 2011-5-4 16:24
//-----------------------------------------------
#define DEV_IOC_USR_HWSC_SD_DRV_INSMOD_OK       (DEV_IOC_USR_BASE + 500)
#define DEV_IOC_USR_HWSC_SD_DRV_RMMOD_OK        (DEV_IOC_USR_BASE + 501)
#define DEV_IOC_USR_HWSC_SD_DRV_SET_READ_WRITE_FLAG (DEV_IOC_USR_BASE + 502)


//-----------------------------------------------
//   usb audio add by Bayden 2011-12-12
//-----------------------------------------------
#define DEV_IOC_USR_HWSC_APP_INSMOD_USB_AUDIO        					(DEV_IOC_USR_BASE + 510)
#define DEV_IOC_USR_HWSC_APP_RMMOD_USB_AUDIO        					(DEV_IOC_USR_BASE + 511)
#define DEV_IOC_USR_HWSC_IS_ALLOW_APP_INSMOD_USB_AUDIO        (DEV_IOC_USR_BASE + 512)
#define DEV_IOC_USR_HWSC_GET_USB_AUDIO_WORK_STATUS		        (DEV_IOC_USR_BASE + 513)

/* usbd 几种工作模式，目前三种:udisk or uvc or charge*/
typedef enum usbd_mode
{
    USBD_UDISK = 0,
    USBD_UVC,
    USBD_CHARGE,
} usbm_usbd_mode_t;


typedef struct tag_usbm_ioctrl_para
{
    __u32  usb_drv;                 /* 当前USB驱动的类型                    */
    __u32  usbh_inmod_info;         /* usb host驱动的加载情况               */
    __u32  usbh_msc_dev_reg;        /* usb host是否向设备管理注册           */
    __u32  usb_cattle;              /* usb充电来源, 0: usb线; 1: usb火牛    */
} usbm_ioctrl_para_t;

/* usb host驱动的加载情况               */
typedef enum usbm_usbh_insmod
{
    USBH_INSMOD_FAIL = 0,
    USBH_INSMOD_BUSY,
    USBH_INSMOD_SUCCESS
} usbm_usbh_insmod_t;


typedef struct __nand_hwsc_info
{
    __u8 id[8];
    __u8 chip_cnt;
    __u8 chip_connect;
    __u8 rb_cnt;
    __u8 rb_connect;
    __u32 good_block_ratio;
} __nand_hwsc_info_t;

#endif //#ifndef _SYS_HWSC_H_
