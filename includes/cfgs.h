/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                         Melis Configuration Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : cfgs.h
* By      : kevin.z
* Version : v2.0
* Date    : 2010-11-23 11:10
* Descript: config some parameter for melis.
* Update  : date                auther      ver     notes
*           2010-11-23 11:10    kevin.z     2.0     build the file;
*********************************************************************************************************
*/
#ifndef __CFGS_H__
#define __CFGS_H__

#include "const.h"
#include "board.h"
#include ".\cfgs\epdk_arch_cfgs.h"

#ifndef EPDK_OS
#define EPDK_OS     EPDK_OS_EPOS
#endif

//define melis version and release date,
//the version will be "major.minor.fixed"
#define MELIS_MAJOR_VER     (2)         //melis major version
#define MELIS_MINOR_VER     (0)         //melis minor version
#define MELIS_FIXED_VER     (0)         //melis fixed version
#define MELIS_RELS_DATE     (20101201)  //melis release date


/********************************************/
/* hardware configurations                  */
/********************************************/

//config debug uart baundrate
#define SIOS_UART_BAUNDRATE (115200)


#ifndef SIM_PC_WIN

#define	REGS_vBASE  0xf0000000  // i/o area virtual address base
#define DRAM_vBASE	0xc2000000  // dram virtual address base
#define SRAM_vBASE  0xf0000000  // sram virtual address base

#else

#ifndef PC_KERNEL
#define AW_SIM_API __declspec(dllimport)
#else
#define AW_SIM_API
#endif

extern AW_SIM_API unsigned char g_RegBuff[];
extern AW_SIM_API unsigned char g_DramBuff[];
extern AW_SIM_API unsigned char g_RamDiskBuff[];

#define	REGS_vBASE  g_RegBuff
#define DRAM_vBASE	g_DramBuff
#define SRAM_vBASE  g_RamDiskBuff

#endif


/********************************************/
/* software configurations                  */
/********************************************/

#define KRNL_STMRTICK_PERIOD            5       /* 5ms                                              */
#define KRNL_TICK_STMRTICKS             2       /* 10ms(KRNL_STMRTICK_PERIOD*KRNL_TICK_STMRTICKS)   */

#define OS_TASK_KSRV_PRIO               1       /* os kernel server task priority                   */
#define OS_TASK_INIT_PRIO               2       /* os init task priority                            */
#define OS_TASK_PWM_PRIO                3       /* power manager task priority                      */
#define OS_TASK_WDOG_SRV_PRIO           4       /* watch-dog server task priority                   */
#define OS_TASK_INPUT_TP_SRV_PRIO       0x1b    /* input system touchpanel server task priority     */
#define OS_TASK_INPUT_MOUSE_SRV_PRIO    0x1c    /* input system mouse server task priority          */
#define OS_TASK_INPUT_KEY_SRV_PRIO      0x1d    /* input system key server task priority            */
#define OS_TASK_STANDBY_PRIO            0x1e    /* standby server task priority                     */
#define OS_TASK_HIGH_TMR_PRIO           0x1f    /* high priority timer task priority                */
#define OS_TASK_LOW_TMR_PRIO            0xbf    /* low priority timer task priority                 */
#define OS_TASK_STAT_PRIO               (OS_LOWEST_PRIO - 1)    /* Statistic task priority          */
#define OS_TASK_IDLE_PRIO               (OS_LOWEST_PRIO)        /* IDLE      task priority          */


#endif /* #ifndef __CFGS_H__ */

