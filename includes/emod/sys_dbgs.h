/*
*********************************************************************************************************
*											        ePOS
*						           the Easy Portable/Player Operation System
*									           emod sub-system
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : sys_dbgs.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#ifndef _SYS_DBGS_H_
#define _SYS_DBGS_H_

#include "typedef.h"
#include "cfgs.h"


#ifdef EPDK_USED_BY_KRNL
#define __swi(x)
#endif


typedef struct
{
    __pSWI_t esSIOS_getchar    ;
    __pSWI_t esSIOS_putchar    ;
    __pSWI_t esSIOS_gets       ;
    __pSWI_t esSIOS_puts       ;
    __pSWI_t esSIOS_putarg     ;
    __pSWI_t esSIOS_putstr     ;
    __pSWI_t esSIOS_SetBaudRate;

} SWIHandler_DBGS_t;

extern SWIHandler_DBGS_t SWIHandler_DBGS;

#define SYSCALL_DBGS(x,y) __swi(SWINO(SWINO_DBGS, SWIHandler_DBGS_t, y))x y

__s32 esSIOS_Init(void);
__s32 esSIOS_Exit(void );

SYSCALL_DBGS(__u8,         esSIOS_getchar      )(void);
SYSCALL_DBGS(__s32,        esSIOS_putchar      )(__u8/*ch*/);
SYSCALL_DBGS(void,         esSIOS_gets       )(char *str );
SYSCALL_DBGS(void,         esSIOS_puts       )(const char *str );
SYSCALL_DBGS(void,         esSIOS_putarg     )(__u32 arg, char format);
SYSCALL_DBGS(void,         esSIOS_putstr     )(const char *str );
SYSCALL_DBGS(void,         esSIOS_SetBaudRate)(__u32 baudrate);
void esSIOS_printf(const char *str, ...);

#endif     //  ifndef _SYS_DBGS_H_

