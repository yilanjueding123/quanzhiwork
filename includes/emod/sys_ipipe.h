/*
*********************************************************************************************************
*											        ePOS
*						           the Easy Portable/Player Operation System
*									           interpipe sub-system
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : sys_ipipe.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#ifndef _SYS_IPIPE_H_
#define _SYS_IPIPE_H_

#include "typedef.h"
#include "cfgs.h"


#ifdef EPDK_USED_BY_KRNL
#define __swi(x)
#endif
//**********************************************************************************************************
//* define level 0( system level)
//************************************************
// 常数定义
#define IPIPE_ATTRIB_LOWSPEED       (0<<0)      //低速
#define IPIPE_ATTRIB_MIDSPEED       (1<<0)      //中等速度
#define IPIPE_ATTRIB_HIGHSPEED      (2<<0)      //高速
#define IPIPE_ATTRIB_H2D            (0<<2)      //主从式
#define IPIPE_ATTRIB_P2P            (1<<2)      //双向式

#define IPIPE_USER_WINSHELL = "WINSHELL"
#define IPIPE_USER_GDB      = "GDB"
#define IPIPE_USER_UFS      = "UFS"

//************************************************
// 数据类型定义
typedef struct __IPIPE_CBARG
{
    __u8    type;
    __hdle  h_pipe;
} __ipipe_cbarg_t;

//************************************************
// 变量定义
/* GLOBAL VARIABLES */

//************************************************
// 函数定义
/* system call table */
typedef struct
{
    __pSWI_t esIPIPE_Open          ;
    __pSWI_t esIPIPE_Close         ;
    __pSWI_t esIPIPE_Request       ;
    __pSWI_t esIPIPE_Release       ;
    __pSWI_t esIPIPE_Query         ;
    __pSWI_t esIPIPE_Reset         ;
    __pSWI_t esIPIPE_RX            ;
    __pSWI_t esIPIPE_TX            ;
} SWIHandler_IPIPE_t;
#ifndef SIM_PC_WIN
#define SYSCALL_IPIPE(x,y) __swi(SWINO(SWINO_IPIPE, SWIHandler_IPIPE_t, y))x y
#else
#define SYSCALL_IPIPE(x,y) x y
#endif


SYSCALL_IPIPE(__hdle,   esIPIPE_Open    )(char *user, __pCBK_t cb);
SYSCALL_IPIPE(__s32 ,   esIPIPE_Close   )(__hdle user);
SYSCALL_IPIPE(__hdle,   esIPIPE_Request )(__hdle user, __u32 maxfrmsize, __u32 attrib);
SYSCALL_IPIPE(__s32 ,   esIPIPE_Release )(__hdle ipipe);
SYSCALL_IPIPE(__s32 ,   esIPIPE_Query   )(__hdle ipipe);
SYSCALL_IPIPE(__s32 ,   esIPIPE_Reset   )(__hdle ipipe);
SYSCALL_IPIPE(__u32 ,   esIPIPE_RX      )(__hdle ipipe, void *buffer);
SYSCALL_IPIPE(__u32 ,   esIPIPE_TX      )(__hdle ipipe, void *buffer, __u32 len);
//**********************************************************************************************************

//**********************************************************************************************************
//* define level 1( system level)
//************************************************
// 常数定义

//************************************************
// 数据类型定义

//************************************************
// 变量定义

extern SWIHandler_IPIPE_t SWIHandler_IPIPE;

//************************************************
// 函数定义
__s32       esIPIPE_Init    (void);
__s32       esIPIPE_Exit    (void);

//**********************************************************************************************************

#endif  /* _SYS_IPIPE_H_ */

