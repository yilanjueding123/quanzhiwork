/*
*********************************************************************************************************
*											        ePOS
*								the Easy Portable/Player Operation System
*									       const define file
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : const.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#ifndef _CONST_H_
#define _CONST_H_

#define EPDK_CPU_AC320          0
#define EPDK_CPU_ARM926EJ_S     0

#define EPDK_ARCH_SUNI          1
#define EPDK_ARCH_SUNII         2
#define EPDK_ARCH_SUNIII        3
#define EPDK_ARCH_SUNIV        	4

#define EPDK_CPU_ARM920T        100
#define EPDK_CPU_ARM922T        101
#define EPDK_CPU_ARM940T        102
#define EPDK_CPU_ARM946E_S      103
#define EPDK_CPU_ARM1022E       104
#define EPDK_CPU_ARM1026EJ_S    105
#define EPDK_CPU_SA_110         106
#define EPDK_CPU_XSCALE         107

#define EPDK_OS_EPOS            0
#define EPDK_OS_WIN2K           100
#define EPDK_OS_WINXP           101
#define EPDK_OS_VISTA           102
#define EPDK_OS_LINUX           103
#define EPDK_OS_MOSH            104

#define EPDK_LIVE_CLICK         0
#define EPDK_LIVE_TOUCH         1
#define EPDK_LIVE_BOX           2
#define EPDK_COMPASS            3

typedef enum __EPDK_CHIP_T
{
    EPDK_CHIP_SUNI  = 0x01000000,   //suni
    EPDK_CHIP_SUNII = 0x02000000,   //sunii
    EPDK_CHIP_SUN2  = 0x03000000,   //sunii-new
    EPDK_CHIP_SUNIII = 0x04000000,  //suniii
} __epdk_chip_t;


typedef enum __EPDK_PID_T
{
    //10平台 product id
    EPDK_PID_INT001 = (EPDK_CHIP_SUNI | (0 << 16)),     /* 256pin 封装的10芯片  */
    EPDK_PID_PVP001 = (EPDK_CHIP_SUNI | (1 << 16)),     /* 176pin 封装的10芯片  */
    EPDK_PID_TVD001 = (EPDK_CHIP_SUNI | (2 << 16)),     /* 128pin 封装的10芯片  */
    EPDK_PID_PVP002 = (EPDK_CHIP_SUNI | (3 << 16)),     /* 216pin 封装的10芯片  */

    //15平台 product id
    EPDK_PID_INT101 = (EPDK_CHIP_SUNII | (0 << 16)),    /* 216pin 封装的15芯片      */
    EPDK_PID_PVP101 = (EPDK_CHIP_SUNII | (1 << 16)),    /* 176pin 封装的15 LDS 芯片 */
    EPDK_PID_PVP102 = (EPDK_CHIP_SUNII | (2 << 16)),    /* 176pin 封装的15 LCD 芯片 */
    EPDK_PID_PVP103 = (EPDK_CHIP_SUNII | (3 << 16)),    /* 128pin 封装的15 PVP 芯片 */
    EPDK_PID_TVD101 = (EPDK_CHIP_SUNII | (4 << 16)),    /* 128pin 封装的15 TVD 芯片 */

    //20平台 product id
    EPDK_PID_INT201 = (EPDK_CHIP_SUNIII | (0 << 16)),   /* 216pin 封装的20芯片      */


} __epdk_pid_t;

typedef enum __EPDK_SID_T
{
    //10平台 solution id
    EPDK_SID_INT001_DEV = (EPDK_PID_INT001 | (1 << 8)),     /* dev 方案1              */
    EPDK_SID_PVP001_001 = (EPDK_PID_PVP001 | (1 << 8)),     /* 176 pvp 方案1          */
    EPDK_SID_PVP001_002 = (EPDK_PID_PVP001 | (2 << 8)),     /* 176 tvd 方案1          */
    EPDK_SID_PVP001_003 = (EPDK_PID_PVP001 | (3 << 8)),     /* 176 nor 方案1          */
    EPDK_SID_PVP002_001 = (EPDK_PID_PVP002 | (1 << 8)),     /* 216 tp  方案1          */
    EPDK_SID_TVD001_001 = (EPDK_PID_TVD001 | (1 << 8)),     /* 128 tvd 方案1          */

    //15平台 solution id
    EPDK_SID_INT101_DEV = (EPDK_PID_INT101 | (1 << 8)),     /* dev 方案1              */
    EPDK_SID_PVP101_001 = (EPDK_PID_PVP101 | (1 << 8)),     /* 176 pvp 方案1          */
    EPDK_SID_PVP101_002 = (EPDK_PID_PVP101 | (2 << 8)),     /* 176 tp  方案1          */
    EPDK_SID_PVP103_001 = (EPDK_PID_PVP103 | (1 << 8)),     /* 128 pvp 方案1          */

    //20平台 solution id
    EPDK_SID_INT201_DEV = (EPDK_PID_INT201 | (1 << 8)),     /* dev 方案1              */

} __epdk_sid_t;


#endif /* #ifndef _CONST_H_ */

