/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Melis CSP Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : bsp.h
* By      : kevin.z
* Version : v1.0
* Date    : 2010-12-2 9:09
* Descript:
* Update  : date                auther      ver     notes
*           2010-12-2 9:09      kevin.z     1.0     build the file;
*********************************************************************************************************
*/
#ifndef __BSP_H__
#define __BSP_H__


#ifndef DATA_TYPE_X_s8
#define DATA_TYPE_X_s8
typedef __s8    s8;
#endif

#ifndef DATA_TYPE_X_u8
#define DATA_TYPE_X_u8
typedef __u8    u8;
#endif

#ifndef DATA_TYPE_X_s16
#define DATA_TYPE_X_s16
typedef __s16   s16;
#endif

#ifndef DATA_TYPE_X_u16
#define DATA_TYPE_X_u16
typedef __u16   u16;
#endif

#ifndef DATA_TYPE_X_s32
#define DATA_TYPE_X_s32
typedef __s32   s32;
#endif

#ifndef DATA_TYPE_X_u32
#define DATA_TYPE_X_u32
typedef __u32   u32;
#endif

#include "suniv.h"
#include "lib_dbg_uart.h"

#include "bsp/bsp_display.h"
#include "bsp/bsp_lradc.h"
#include "bsp/bsp_mp.h"
#include "bsp/bsp_nand.h"
#include "bsp/bsp_nop.h"
#include "bsp/bsp_spi.h"
#include "bsp/bsp_total_inc.h"
#include "bsp/bsp_twi.h"
#include "bsp/bsp_usb.h"
#include "bsp/bsp_uart.h"
#include "bsp/bsp_adda.h"
#include "bsp/bsp_tp.h"
#include "bsp/bsp_ir.h"
#include "bsp/bsp_iisc.h"
#include "bsp/bsp_rtc.h"
#include "bsp/bsp_sdc.h"
#include "bsp/audio.h"
#include "bsp/bsp_di.h"

#ifndef EBASE_TRUE
#define EBASE_TRUE      0
#endif

#ifndef EBASE_FALSE
#define EBASE_FALSE     (-1)
#endif

#ifndef EBSP_TRUE
#define EBSP_TRUE       EBASE_TRUE
#endif

#ifndef EBSP_FALSE
#define EBSP_FALSE      EBASE_FALSE
#endif


#define SRAM_TO_AC320   (0)
__s32 CSP_SRAM_set_srama_map(__s32 aux);
__s32 CSP_SRAM_get_srama_map(void);

#define TOUCHPANEL_WAKEUP_SYS_PENDING   (1<<0)
#define EXTNMI_WAKEUP_SYS_PENGDING      (1<<1)
#define USB_WAKEUP_SYS_PENDING          (1<<3)
#define ALARM_WAKEUP_SYS_PENDING        (1<<4)
#define LRADC_WAKEUP_SYS_PENDING        (1<<5)
#define PIO_WAKEUP_SYS_PENDING          (1<<6)
//wakeup moduel
__u32 CSP_CCU_get_wakeup_stat(void);


#endif  //__CSP_H__


