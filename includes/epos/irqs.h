/*
*********************************************************************************************************
*											        ePOS
*						           the Easy Portable/Player Operation System
*									           IRQS sub-system
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : IRQS.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#ifndef _IRQS_H_
#define _IRQS_H_

#include "typedef.h"
#include "cfgs.h"

#ifdef EPDK_USED_BY_KRNL
#define __swi(x)
#endif
//**********************************************************************************************************
//* define level 0( system level)
//************************************************
// 常数定义
/* 系统调用号分配 */
typedef struct
{
    void *SWIHandler_SIOS          ;
    void *SWIHandler_KRNL          ;
    void *SWIHandler_MEMS          ;
    void *SWIHandler_FSYS          ;
    void *SWIHandler_EXEC          ;
    void *SWIHandler_MODS          ;
    void *SWIHandler_RESM          ;
    void *SWIHandler_INT           ;
    void *SWIHandler_DMA           ;
    void *SWIHandler_TIME          ;
    void *SWIHandler_IPIPE			;

    void *SWIHandler_PWRS          ;
    void *SWIHandler_ERRS          ;

    void *SWIHandler_SVC           ;
    void *SWIHandler_DEV           ;
    void *SWIHandler_KSRV          ;
    void *SWIHandler_PINS          ;
    void *SWIHandler_CLK           ;

    void *SWIHandler_MEM           ;
    void *SWIHandler_HID           ;

    void *SWIHandler_PWM           ;
    void *SWIHandler_CHS           ;
    void *SWIHandler_MSTUB         ;
    void *SWIHandler_INPUT         ;
    void *SWIHandler_CONFIG        ;

} SWIHandler_SWIT_t;
#define SYSCALL_SWINO(x) (SWINO(0, SWIHandler_SWIT_t, x) << 8)

#define SWINO_SIOS 	SYSCALL_SWINO(SWIHandler_SIOS)  /* 标准IO模拟系统           */
#define SWINO_KRNL 	SYSCALL_SWINO(SWIHandler_KRNL)  /* 内核系统                 */
#define SWINO_MEMS 	SYSCALL_SWINO(SWIHandler_MEMS)  /* 内存管理系统             */
#define SWINO_FSYS 	SYSCALL_SWINO(SWIHandler_FSYS)  /* 文件系统                 */
#define SWINO_EXEC 	SYSCALL_SWINO(SWIHandler_EXEC)  /* 进程管理系统             */
#define SWINO_MODS 	SYSCALL_SWINO(SWIHandler_MODS)  /* 模块管理系统             */
#define SWINO_RESM 	SYSCALL_SWINO(SWIHandler_RESM)  /* 资源管理系统             */
#define SWINO_INT  	SYSCALL_SWINO(SWIHandler_INT)   /* 中断控制系统             */
#define SWINO_DMA  	SYSCALL_SWINO(SWIHandler_DMA )  /* DMA                      */
#define SWINO_TIME 	SYSCALL_SWINO(SWIHandler_TIME)  /* 时间管理系统             */
#define SWINO_IPIPE SYSCALL_SWINO(SWIHandler_IPIPE) /*                          */

#define SWINO_PWRS 	SYSCALL_SWINO(SWIHandler_PWRS)  /* 功耗管理系统             */
#define SWINO_ERRS 	SYSCALL_SWINO(SWIHandler_ERRS)  /* 错误报告和处理系统       */

#define SWINO_SVC   SYSCALL_SWINO(SWIHandler_SVC)   /*                          */
#define SWINO_DEV   SYSCALL_SWINO(SWIHandler_DEV)   /* 设备管理系统             */
#define SWINO_KSRV  SYSCALL_SWINO(SWIHandler_KSRV)  /* 内核服务                 */
#define SWINO_PINS  SYSCALL_SWINO(SWIHandler_PINS)  /* pin服务                  */
#define SWINO_CLK   SYSCALL_SWINO(SWIHandler_CLK)   /* 时钟服务                 */
#define SWINO_MEM   SYSCALL_SWINO(SWIHandler_MEM)   /* 内存设备管理             */
#define SWINO_HID   SYSCALL_SWINO(SWIHandler_HID)   /* hid子系统                */

#define SWINO_PWM   SYSCALL_SWINO(SWIHandler_PWM)   /* pwm子系统                */
#define SWINO_CHS   SYSCALL_SWINO(SWIHandler_CHS)   /* charset 子系统           */
#define SWINO_MSTUB SYSCALL_SWINO(SWIHandler_MSTUB) /* module stub 子系统       */
#define SWINO_INPUT SYSCALL_SWINO(SWIHandler_INPUT) /* 输入子系统               */
#define SWINO_CFG   SYSCALL_SWINO(SWIHandler_CONFIG)/* 配置子系统               */

extern __pSWI_t  const *SWIVectorTable[];

void esIRQS_EI(void);
void esIRQS_DI(void);
void esIRQS_irqsr_ei(void);
void esIRQS_irqsr_di(void);

#endif  /* _IRQS_H_ */

