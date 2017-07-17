/*
**********************************************************************************************************************
*											        ePDK
*						           the Easy Portable/Player Development Kits
*									           eMod sub-system
*
*						        (c) Copyright 2007-2008, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : emod.h
* By      : Steven.ZGJ
* Version : V0.1
**********************************************************************************************************************
*/
#ifndef _EMOD_H_
#define _EMOD_H_

//level0
#include "typedef.h"
#include "cfgs.h"
#include "configs.h"
#include "dbgs.h"

//level1
#include "elibs\\libc.h"
#include "ebios.h"
#include "epos.h"

//level2
#include "emod\\sys_time.h"
#include "emod\\sys_dma.h"
#include "emod\\sys_int.h"
#include "emod\\sys_ipipe.h"
#include "emod\\sys_sios.h"
#include "emod\\sys_dev.h"
#include "emod\\sys_fsys.h"
#include "emod\\sys_svc.h"
#include "emod\\sys_pins.h"
#include "emod\\sys_clock.h"
#include "emod\\sys_mem.h"
#include "emod\\sys_hid.h"
#include "emod\\sys_hwsc.h"
#include "emod\\sys_pwm.h"
#include "emod\\sys_charset.h"
#include "emod\\sys_modstub.h"
#include "emod\\sys_event.h"
#include "emod\\sys_input.h"
#include "emod\\sys_config.h"

#include "emod\\drv_nop.h"
#include "emod\\drv_display.h"
#include "emod\\drv_key.h"
#include "emod\\drv_audio.h"
#include "emod\\drv_uart.h"
#include "emod\\drv_power.h"
#include "emod\\drv_iic.h"
#include "emod\\drv_hdmi.h"
#include "emod\\drv_fm.h"
#include "emod\\drv_spi.h"
#include "emod\\drv_udisk.h"
#include "emod\\drv_host.h"
#include "emod\\drv_touchpanel.h"
#include "emod\\drv_csi.h"
#include "emod\\drv_rtc.h"
#include "emod\\drv_filedisk.h"
#include "emod\\drv_mp.h"
#include "emod\\drv_di.h"
#include "emod\\drv_tvd.h"
#include "emod\\drv_pwm.h"

#include "emod\\mod_cedar.h"
#include "emod\\mod_willow.h"
#include "emod\\drv_orchid.h"
#include "emod\\mod_ginkgo.h"
#include "emod\\mod_maple.h"
#include "emod\\mod_poplar.h"
#include "emod\\mod_update.h"
#include "emod\\drv_streaming.h"
#include "emod\\mod_dbms.h"
#include "emod\\mod_bookengine.h"
#include "emod\\mod_lotus.h"
#include "emod\\mod_dvdnavi.h"
#include "emod\\mod_charset.h"
#include "emod\\mod_duckweed.h"
#include "emod\\mod_slib.h"
#include "emod\\mod_karaoke.h"

#define EMOD_MAGIC0     'e'
#define EMOD_MAGIC1     'P'
#define EMOD_MAGIC2     'D'
#define EMOD_MAGIC3     'K'
#define EMOD_MAGIC4     '.'
#define EMOD_MAGIC5     'm'
#define EMOD_MAGIC6     'o'
#define EMOD_MAGIC7     'd'

/* base drivers                                     */
#define EMOD_TYPE_DRV_NOP               0x00        /* 0ºÅÇý¶¯£¬ÏµÍ³±£Áô    */
#define EMOD_TYPE_DRV_RTC               0x01        /* rtcÇý¶¯type±àºÅ      */
#define EMOD_TYPE_DRV_KEY               0x02        /* ¼üÅÌÇý¶¯type±àºÅ     */
#define EMOD_TYPE_DRV_DISP              0x03        /* ÏÔÊ¾Çý¶¯type±àºÅ     */
#define EMOD_TYPE_DRV_MOUSE             0x04        /* Êó±êÇý¶¯type±àºÅ     */
#define EMOD_TYPE_DRV_TP                0x05        /* tpÇý¶¯type±àºÅ       */
#define EMOD_TYPE_DRV_AUDIO             0x06        /* AudioÇý¶¯type±àºÅ    */
#define EMOD_TYPE_DRV_FM                0x07        /* FM radio driver±àºÅ  */
#define EMOD_TYPE_DRV_MP                0x08        /* MP driver±àºÅ        */
#define EMOD_TYPE_DRV_CE                0x09        /* CE driver±àºÅ        */
#define EMOD_TYPE_DRV_MATRIXKEY         0x0a        /* matrix key driver    */
#define EMOD_TYPE_DRV_DI                0x0b        /* de-interlace driver±àºÅ        */
#define EMOD_TYPE_DRV_BT                0x0c        /* bluetooth driver±àºÅ  */
#define EMOD_TYPE_DRV_PWM               0x0d        /* pwm driver±àºÅ  */

/* other drivers                                    */
#define EMOD_TYPE_DRV_FILEDISK          0x11
#define EMOD_TYPE_DRV_MONITOR           0x12        /* monitor              */
#define EMOD_TYPE_DRV_POWER             0x13
#define EMOD_TYPE_DRV_CSI               0x14
#define EMOD_TYPE_DRV_STREAM            0x15
#define EMOD_TYPE_DRV_TVD               0x16

#define EMOD_TYPE_MOD_INIT              0x17        /* desktop initial module*/
#define EMOD_TYPE_MOD_LEMON             0x18
#define EMOD_TYPE_MOD_ORANGE            0x18
/* 0x18 and 0x19/0x1a/0x1b/0x1c/0x1d is used by lemon */
#define EMOD_TYPE_MOD_ORCHID            0x1e
#define EMOD_TYPE_MOD_DESKTOP		    0x1f

/* bus drivers(except usb, net)                     */
#define EMOD_TYPE_DRV_BUS_BASE          0x20
#define EMOD_TYPE_DRV_UART              (EMOD_TYPE_DRV_BUS_BASE  + 0)
#define EMOD_TYPE_DRV_IIS               (EMOD_TYPE_DRV_BUS_BASE  + 1)
#define EMOD_TYPE_DRV_SPI               (EMOD_TYPE_DRV_BUS_BASE  + 2)
#define EMOD_TYPE_DRV_IIC               (EMOD_TYPE_DRV_BUS_BASE  + 3)
#define EMOD_TYPE_DRV_IR                (EMOD_TYPE_DRV_BUS_BASE  + 4)
#define EMOD_TYPE_DRV_HDMI              (EMOD_TYPE_DRV_BUS_BASE  + 5)
#define EMOD_TYPE_DRV_UPRL              (EMOD_TYPE_DRV_BUS_BASE  + 6)
#define EMOD_TYPE_MOD_UPDATE            (EMOD_TYPE_DRV_BUS_BASE  + 7)


/* memory drivers(0~7)                             */
#define EMOD_TYPE_DRV_MEM_BASE          0x30
#define EMOD_TYPE_DRV_RAMDISK           (EMOD_TYPE_DRV_MEM_BASE  + 0)
#define EMOD_TYPE_DRV_NAND              (EMOD_TYPE_DRV_MEM_BASE  + 1)
#define EMOD_TYPE_DRV_SDMMC             (EMOD_TYPE_DRV_MEM_BASE  + 2)
#define EMOD_TYPE_DRV_NOR               (EMOD_TYPE_DRV_MEM_BASE  + 3)
#define EMOD_TYPE_DRV_HDD               (EMOD_TYPE_DRV_MEM_BASE  + 4)
#define EMOD_TYPE_DRV_MS                (EMOD_TYPE_DRV_MEM_BASE  + 5)
#define EMOD_TYPE_DRV_SDMMC2            (EMOD_TYPE_DRV_MEM_BASE  + 6)

/* part driver (0~7)                               */
#define EMOD_TYPE_DRV_PART_BASE         0x38
#define EMOD_TYPE_DRV_DOSPART           (EMOD_TYPE_DRV_PART_BASE + 0)
#define EMOD_TYPE_DRV_MACPART           (EMOD_TYPE_DRV_PART_BASE + 1)

/* file system driver (0~15)                        */
#define EMOD_TYPE_DRV_VFS_BASE          0x40
#define EMOD_TYPE_DRV_FATFS             (EMOD_TYPE_DRV_VFS_BASE + 0)
#define EMOD_TYPE_DRV_NTFS              (EMOD_TYPE_DRV_VFS_BASE + 1)
#define EMOD_TYPE_MOD_DBMS              (EMOD_TYPE_DRV_VFS_BASE + 2)
#define EMOD_TYPE_MOD_EXT               (EMOD_TYPE_DRV_VFS_BASE + 3)
#define EMOD_TYPE_MOD_CHARSET           (EMOD_TYPE_DRV_VFS_BASE + 4)

/* multi media driver (0~15)                        */
#define EMOD_TYPE_DRV_MM_BASE           0x50
#define EMOD_TYPE_DRV_AVSYNC            (EMOD_TYPE_DRV_MM_BASE   + 0)
#define EMOD_TYPE_DRV_VDEC              (EMOD_TYPE_DRV_MM_BASE   + 1)
#define EMOD_TYPE_DRV_ADEC              (EMOD_TYPE_DRV_MM_BASE   + 2)
#define EMOD_TYPE_DRV_AENC              (EMOD_TYPE_DRV_MM_BASE   + 3)
#define EMOD_TYPE_DRV_CMMBRX            (EMOD_TYPE_DRV_MM_BASE   + 4)
#define EMOD_TYPE_DRV_TSC               (EMOD_TYPE_DRV_MM_BASE   + 4)   //TSC_DRV_INSTALL_TYPE,ºÍcmmbrx¹²ÓÃ
#define EMOD_TYPE_DRV_VENC              (EMOD_TYPE_DRV_MM_BASE   + 5)
#define EMOD_TYPE_DRV_DEMOD             (EMOD_TYPE_DRV_MM_BASE   + 5)   //DEMOD_DRV_INSTALL_TYPE,¿ØÖÆÇ°¶ËdemodulatorÍâ¹ÒÐ¾Æ¬µÄÇý¶¯
#define EMOD_TYPE_DRV_PSHOW             (EMOD_TYPE_DRV_MM_BASE   + 7)
#define EMOD_TYPE_DRV_JEPGDEC           (EMOD_TYPE_DRV_MM_BASE   + 8)

/* usb driver (0~15)                                */
#define EMOD_TYPE_DRV_USB_BASE          0x60
#define EMOD_TYPE_DRV_USBD_BASE         (EMOD_TYPE_DRV_USB_BASE + 0)
#define EMOD_TYPE_DRV_USBH_BASE         (EMOD_TYPE_DRV_USB_BASE + 4)
#define EMOD_TYPE_DRV_USBH1_BASE        (EMOD_TYPE_DRV_USB_BASE + 5)
#define EMOD_TYPE_DRV_USBH2_BASE        (EMOD_TYPE_DRV_USB_BASE + 6)

/* net protocal driver (0~15)                       */
#define EMOD_TYPE_DRV_NET_BASE          0x70

/* user case modules */
#define EMOD_TYPE_MOD_CASE_BASE         0x80
//#define EMOD_TYPE_MOD_UPDATE            (EMOD_TYPE_MOD_CASE_BASE + 0)
/* 0x80/0x81/0x82/0x83 used by bookengine */
#define EMOD_TYPE_MOD_TCLENGINE         (EMOD_TYPE_MOD_CASE_BASE + 5)   /* TCL engine                               */
/* flash module */
#define EMOD_TYPE_MOD_FLASH             (EMOD_TYPE_MOD_CASE_BASE + 6)
#define EMOD_TYPE_MOD_SLIB              (EMOD_TYPE_MOD_CASE_BASE + 7)
#define	EMOD_TYPE_MOD_BOOKENGINE	    (EMOD_TYPE_MOD_CASE_BASE + 0x10)
/* reserved driver, can't been used by anyone   */
#define EMOD_TYPE_DRV_RESERVED          0x9e

#define EMOD_TYPE_USER                  0xff


__s32 MInit(void);
__s32 MExit(void);
__mp  *MOpen(__u32 mid, __u32 mode);
__s32 MClose(__mp *mp);
__u32 MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
__s32 MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);

#endif  /* _EMOD_H_ */
