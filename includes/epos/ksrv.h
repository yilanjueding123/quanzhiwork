/*
*********************************************************************************************************
*											        ePOS
*								the Easy Portable/Player Operation System
*									     kernel server  sub-system
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : ksrv.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/
#ifndef _KSRV_H_
#define _KSRV_H_

#include "kmsg.h"

#ifdef EPDK_USED_BY_KRNL
#define __swi(x)
#endif
//**********************************************************************************************************
//* define level 0( system level)
//************************************************
// 常数定义


//************************************************
// 数据类型定义

typedef enum enum_EPDK_VER_TYPE
{
    EPDK_VER,
    EPDK_VER_OS,
    EPDK_VER_CHIP,
    EPDK_VER_PID,
    EPDK_VER_SID,
    EPDK_VER_BID,
    EPDK_VER_CHIP_SUB,
} __epdk_ver_type_t;


typedef enum EPDK_PARA_TYPE
{
    EPDK_PARA_TYPE_NAND = (2 << 24),
    EPDK_PARA_TYPE_

} __epdk_para_type_e;


//************************************************
// 变量定义
/* GLOBAL VARIABLES */

//************************************************
// 函数定义
/* system call table */
typedef struct
{
    __pSWI_t esKSRV_SysInfo             ;
    __pSWI_t esKSRV_SendMsgEx           ;
    __pSWI_t esKSRV_GetMsg              ;
    __pSWI_t esKSRV_GetVersion          ;
    __pSWI_t esKSRV_Random              ;
    __pSWI_t esKSRV_Reset               ;
    __pSWI_t esKSRV_PowerOff            ;
    __pSWI_t esKSRV_SendMsg             ;
    __pSWI_t esKSRV_GetTargetPara       ;

    __pSWI_t esKSRV_EnableWatchDog      ;
    __pSWI_t esKSRV_DisableWatchDog     ;
    __pSWI_t esKSRV_ClearWatchDog       ;
    __pSWI_t esKSRV_EnableWatchDogSrv   ;
    __pSWI_t esKSRV_DisableWatchDogSrv  ;

    __pSWI_t esKSRV_memcpy              ;

    __pSWI_t esKSRV_GetLowMsg           ;
    __pSWI_t esKSRV_GetHighMsg          ;
    __pSWI_t esKSRV_GetPara             ;
    __pSWI_t esKSRV_GetDramCfgPara      ;
    __pSWI_t esKSRV_memset              ;
    __pSWI_t esKSRV_GetAddPara          ;

    __pSWI_t esKSRV_close_logo          ;
    __pSWI_t esKSRV_release_logo_buf	;
} SWIHandler_KSRV_t;



#ifndef SIM_PC_WIN
#define SYSCALL_KSRV(x,y) __swi(SWINO(SWINO_KSRV, SWIHandler_KSRV_t, y))x y
#else
#define SYSCALL_KSRV(x,y) x y
#endif

SYSCALL_KSRV(__s32, esKSRV_SysInfo     )(void);
SYSCALL_KSRV(__s32, esKSRV_SendMsgEx   )(void *msg);
SYSCALL_KSRV(__u32, esKSRV_GetMsg      )(void);
SYSCALL_KSRV(__s32, esKSRV_SendMsg     )(__u32 msgid, __u32 prio);
SYSCALL_KSRV(__u32, esKSRV_GetVersion  )(__epdk_ver_type_t type);
SYSCALL_KSRV(__u32, esKSRV_Random      )(__u32 max);
SYSCALL_KSRV(void , esKSRV_Reset       )(void);
SYSCALL_KSRV(void , esKSRV_PowerOff    )(void);
SYSCALL_KSRV(__s32, esKSRV_GetTargetPara)(__target_para_t   *tgtpara);
SYSCALL_KSRV(__s32, esKSRV_GetAddPara)(__ksrv_add_para *ksrv_add_para);

SYSCALL_KSRV(__s32, esKSRV_EnableWatchDog     )(__s32 /*time(ms)*/);
SYSCALL_KSRV(__s32, esKSRV_DisableWatchDog    )(void);
SYSCALL_KSRV(__s32, esKSRV_ClearWatchDog      )(void);
SYSCALL_KSRV(__s32, esKSRV_EnableWatchDogSrv  )(void);
SYSCALL_KSRV(__s32, esKSRV_DisableWatchDogSrv )(void);

SYSCALL_KSRV(__s32, esKSRV_memcpy       )(void *pdest, const void *psrc, __size_t size);
SYSCALL_KSRV(__u32, esKSRV_GetLowMsg    )(void);
SYSCALL_KSRV(__u32, esKSRV_GetHighMsg   )(void);
SYSCALL_KSRV(__s32, esKSRV_GetPara      )(__u32 type, void *format, void *para);
SYSCALL_KSRV(__s32, esKSRV_GetDramCfgPara)(__dram_para_t *drampara);
SYSCALL_KSRV(__s32, esKSRV_memset       )(void *pmem, __u8 value, __size_t size);


SYSCALL_KSRV(__u32, esKSRV_close_logo      )(void);
SYSCALL_KSRV(__u32, esKSRV_release_logo_buf      )(void);

//**********************************************************************************************************

//**********************************************************************************************************
//* define level 1( system level)
//************************************************
// 常数定义

//************************************************
// 数据类型定义

//************************************************
// 变量定义

extern SWIHandler_KSRV_t SWIHandler_KSRV;
//************************************************
// 函数定义

__s32 KSRV_Init(void);
__s32 KSRV_Exit(void);

typedef struct KSRV_MSG_TDEL
{
    __u8            id;
    __u8            length;
    __u8            prio;
    __krnl_tcb_t    *tcb;
} __ksrv_msg_tdel_t;

#endif /* #ifndef _KSRV_H_ */

