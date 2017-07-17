/*
*********************************************************************************************************
*											        ePOS
*						           the Easy Portable/Player Operation System
*									           EXEC sub-system
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : EXEC.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#ifndef _EXEC_H_
#define _EXEC_H_

#include "typedef.h"
#include "cfgs.h"

#ifdef EPDK_USED_BY_KRNL
#define __swi(x)
#endif
//**********************************************************************************************************
//* define level 0( system level)
//************************************************
// 常数定义
#define EXEC_no_error           1
#define EXEC_execsrvmsgargmax   15

#define EXEC_pidself            0xff
#define EXEC_prioself           0xff

#define EXEC_maxseg             16      // exe 文件中最多可以有多少个段

#define EXEC_segendflag         0xff

/*wait process main function to return  */
#define EXEC_CREATE_WAIT_RET    (1<<0)

//************************************************
// 变量定义
/* GLOBAL VARIABLES */

//************************************************
// 函数定义
/* system call table */
typedef struct
{
    __pSWI_t esEXEC_PCreate       ;
    __pSWI_t esEXEC_PDel          ;
    __pSWI_t esEXEC_PDelReq       ;
    __pSWI_t esEXEC_Run           ;
} SWIHandler_EXEC_t;

#define SYSCALL_EXEC(x,y) __swi(SWINO(SWINO_EXEC, SWIHandler_EXEC_t, y))x y

SYSCALL_EXEC(__u8, esEXEC_PCreate  )(const char *pfilename, void *p_arg, __u32 mode, __u32 *ret);
SYSCALL_EXEC(__s8, esEXEC_PDel     )(__u8 pid);
SYSCALL_EXEC(__s8, esEXEC_PDelReq  )(__u8 pid);
SYSCALL_EXEC(__s8, esEXEC_Run      )(const char *pfilename, void *p_arg, __u32 mode, __u32 *ret);


//**********************************************************************************************************

//**********************************************************************************************************
//* define level 1( system level)
//************************************************
// 常数定义

//************************************************
// 数据类型定义

//************************************************
// 变量定义

extern SWIHandler_EXEC_t SWIHandler_EXEC;
//************************************************
// 函数定义
__s32 EXEC_Init (void);
__s32 EXEC_Exit (void);


//**********************************************************************************************************

#endif  /* _EXEC_H_ */

