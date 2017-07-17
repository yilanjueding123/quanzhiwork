/*
**********************************************************************************************************************
*											        ePOS
*						           the Easy Portable/Player Operation System
*									           hid sub-system
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : sys_hid.h
* Version: V1.0
* By     : steven.zgj
**********************************************************************************************************************
*/

#ifndef _HID_H_
#define _HID_H_

#include "typedef.h"
#include "cfgs.h"

#ifdef EPDK_USED_BY_KRNL
#define __swi(x)
#endif


//************************************************
// 变量定义
/* GLOBAL VARIABLES */


//************************************************
// 函数定义
/* system call table */
typedef struct
{
    __pSWI_t esHID_SendMsg      ;
    __pSWI_t esHID_hiddevreg       ;
    __pSWI_t esHID_hiddevunreg     ;
} SWIHandler_HID_t;
#ifndef SIM_PC_WIN
#define SYSCALL_HID(x,y) __swi(SWINO(SWINO_HID, SWIHandler_HID_t, y))x y
#else
#define SYSCALL_HID(x,y) x y
#endif
SYSCALL_HID(__s32,  esHID_SendMsg      )(__u32 msgid);
SYSCALL_HID(__s32,  esHID_hiddevreg    )(__hdle hNode);
SYSCALL_HID(__s32,  esHID_hiddevunreg    )(__hdle hNode, __u32 mode);



//**********************************************************************************************************

//**********************************************************************************************************
//* define level 1( system level)
//************************************************
// 常数定义

//************************************************
// 数据类型定义

//************************************************
// 变量定义
extern SWIHandler_HID_t SWIHandler_HID;
//************************************************
// 函数定义
__s32 HID_Init (void);
__s32 HID_Exit (void);

//**********************************************************************************************************

#endif  /* _HID_H_ */


