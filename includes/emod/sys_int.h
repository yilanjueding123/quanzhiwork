/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                            Interrupt manage Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : sys_int.h
* By      : kevin.z
* Version : v2.0
* Date    : 2010-11-24 16:44
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-24 16:44    kevin.z     2.0     build the file.
*********************************************************************************************************
*/
#ifndef __SYS_INT_H__
#define __SYS_INT_H__

#include "typedef.h"
#include "cfgs.h"


#ifdef EPDK_USED_BY_KRNL
#define __swi(x)
#endif


//define interrupt priority level, the lower value the higher priority
typedef enum __INT_PRIO_LEVEL
{
    INT_PRIO_LEVEL_0 = 0,
    INT_PRIO_LEVEL_1 = 1,
    INT_PRIO_LEVEL_2 = 2,
    INT_PRIO_LEVEL_3 = 3,
    INT_PRIO_LEVEL_

} __int_prio_level_e;


typedef struct
{
    __pSWI_t esINT_InsISR            ;
    __pSWI_t esINT_UniISR            ;
    __pSWI_t esINT_InsFIR            ;
    __pSWI_t esINT_UniFIR            ;
    __pSWI_t esINT_SetIRQPrio        ;
    __pSWI_t esINT_DisableINT        ;
    __pSWI_t esINT_EnableINT         ;

} SWIHandler_INT_t;


#ifndef SIM_PC_WIN
#define SYSCALL_INT(x,y) __swi(SWINO(SWINO_INT, SWIHandler_INT_t, y))x y
#else
#define SYSCALL_INT(x,y) x y
#endif


SYSCALL_INT(__s32, esINT_EnableINT  )(__s32 irqno);
SYSCALL_INT(__s32, esINT_DisableINT )(__s32 irqno);
SYSCALL_INT(__s32, esINT_InsISR     )(__s32 irqno, __pISR_t pIsr, void *arg);
SYSCALL_INT(__s32, esINT_UniISR     )(__s32 irqno, __pISR_t pIsr);
SYSCALL_INT(__s32, esINT_InsFIR     )(__s32 fiqno, __pISR_t pFirMain, __pCBK_t pFirTail, void *pArg);
SYSCALL_INT(__s32, esINT_UniFIR     )(__s32 fiqno, __pISR_t pFirMain, __pCBK_t pFirTail);
SYSCALL_INT(__s32, esINT_SetIRQPrio )(__s32 irqno, __s32 prio);


//==============================================================
// define interface for kernel
//==============================================================
extern SWIHandler_INT_t SWIHandler_INT;
__s32 INT_Init(void);
__s32 INT_Exit(void);
__s32 INT_GetCurIrq(void);
void  INT_SwitchMask(__s32 irqno);
void  INT_RestoreMask(void);
__s32 INT_EnterIsr(__s32 irqno);
__s32 INT_EnterFIsr(void);


#endif  //__SYS_INT_H__

