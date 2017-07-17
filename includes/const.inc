;/*
;*********************************************************************************************************
;*											        ePOS
;*						           the Easy Portable/Player Operation System
;*									           const headfile
;*
;*						        (c) Copyright 2006-2007, Steven.ZGJ China
;*											All	Rights Reserved
;*
;* File    : const.inc
;* By      : Steven.ZGJ
;* Version : V0.1
;*
;************************************************************************************************************************
;*/
	IF :LNOT::DEF:_CONST_INC_
	GBLA  _CONST_INC_

;/* platform      */


EPDK_CPU_AC320              equ 0
EPDK_CPU_ARM926EJ_S         equ 0

EPDK_CPU_ARM920T            equ 100
EPDK_CPU_ARM922T            equ 101
EPDK_CPU_ARM940T            equ 102
EPDK_CPU_ARM946E_S          equ 103
EPDK_CPU_ARM1022E           equ 104
EPDK_CPU_ARM1026EJ_S        equ 105
EPDK_CPU_SA_110             equ 106
EPDK_CPU_XSCALE             equ 107
EPDK_CPU_ARM720T            equ 108

EPDK_OS_EPOS                equ 0
EPDK_OS_WIN2K               equ 100
EPDK_OS_WINXP               equ 101
EPDK_OS_VISTA               equ 102
EPDK_OS_LINUX               equ 103
EPDK_OS_MOSH                equ 104

EPDK_LIVE_CLICK             equ 0
EPDK_LIVE_TOUCH             equ 1
EPDK_LIVE_BOX               equ 2
EPDK_COMPASS                equ 3

EPDK_ARCH_SUNI              equ 1
EPDK_ARCH_SUNII             equ 2
EPDK_ARCH_SUNIII            equ 3
EPDK_ARCH_SUNIV             equ 4

EPDK_CHIP_SUNI              equ 0x01000000
EPDK_CHIP_SUNII             equ 0x02000000
EPDK_CHIP_SUNIII            equ 0x03000000
EPDK_CHIP_SUNIV             equ 0x04000000

;//10平台product id
EPDK_PID_INT001             equ 0x01000000  ;(EPDK_CHIP_SUNI | (0 << 16))   /* 256pin 封装的10芯片  */
EPDK_PID_PVP001             equ 0x01010000  ;(EPDK_CHIP_SUNI | (1 << 16))   /* 176pin 封装的10芯片  */
EPDK_PID_TVD001             equ 0x01020000  ;(EPDK_CHIP_SUNI | (2 << 16))   /* 128pin 封装的10芯片  */
EPDK_PID_PVP002             equ 0x01030000  ;(EPDK_CHIP_SUNI | (3 << 16))   /* 216pin 封装的10芯片  */

;//15平台 product id
EPDK_PID_INT101             equ 0x02000000  ;(EPDK_CHIP_SUNII | (0 << 16))  /* 216pin 封装的15芯片      */
EPDK_PID_PVP101             equ 0x02010000  ;(EPDK_CHIP_SUNII | (1 << 16))  /* 176pin 封装的15 LDS 芯片 */
EPDK_PID_PVP102             equ 0x02020000  ;(EPDK_CHIP_SUNII | (2 << 16))  /* 176pin 封装的15 LCD 芯片 */
EPDK_PID_PVP103             equ 0x02030000  ;(EPDK_CHIP_SUNII | (3 << 16))  /* 128pin 封装的15 PVP 芯片 */
EPDK_PID_TVD101             equ 0x02040000  ;(EPDK_CHIP_SUNII | (4 << 16))  /* 128pin 封装的15 TVD 芯片 */

;//20平台 product id
EPDK_PID_INT201             equ 0x03000000  ;(EPDK_CHIP_SUNIII| (0 << 16))  /* 216pin 封装的20芯片      */


;//10平台 solution id
EPDK_SID_INT001_DEV         equ 0x01000100  ;(EPDK_PID_INT001 | (1 << 8)),  /* dev 方案1            */
EPDK_SID_PVP001_001         equ 0x01010100  ;(EPDK_PID_PVP001 | (1 << 8)),  /* 176 pvp 方案1        */
EPDK_SID_PVP001_002         equ 0x01010200  ;(EPDK_PID_PVP001 | (2 << 8)),  /* 176 tvd 方案1        */
EPDK_SID_PVP001_003         equ 0x01010300  ;(EPDK_PID_PVP001 | (3 << 8)),  /* 176 nor 方案1        */
EPDK_SID_PVP002_001         equ 0x01030100  ;(EPDK_PID_PVP002 | (1 << 8)),  /* 216 tp  方案1        */
EPDK_SID_TVD001_001         equ 0x01020100  ;(EPDK_PID_TVD001 | (1 << 8)),  /* 128 tvd 方案1        */

;//15平台 solution id
EPDK_SID_INT101_DEV         equ 0x02000100  ;(EPDK_PID_INT101 | (1 << 8)),  /* dev 方案1            */
EPDK_SID_PVP101_001         equ 0x02010100  ;(EPDK_PID_PVP101 | (1 << 8)),  /* 176 pvp 方案1        */
EPDK_SID_PVP101_002         equ 0x02010200  ;(EPDK_PID_PVP101 | (2 << 8)),  /* 176 tp 方案1         */
EPDK_SID_PVP103_001         equ 0x02030100  ;(EPDK_PID_PVP103 | (1 << 8)),  /* 128 pvp 方案1        */

;//20平台 solution id
EPDK_SID_INT201_DEV         equ 0x03000100  ;(EPDK_PID_INT201 | (1 << 8)),  /* dev 方案1            */


    ENDIF   ;/* #ifndef _CONST_INC_ */

    end
