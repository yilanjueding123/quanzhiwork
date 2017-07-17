/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              module stub Sub-System
*
*                                   (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* Moudle  : system module stub
* File    : sys_modstub.h
* By      : Sunny
* Version : v1.0
* Date    : 2010-4-15 14:39:12
**********************************************************************************************************************
*/
#ifndef __SYS_MOD_STUB_H__
#define __SYS_MOD_STUB_H__

/* system call table */
typedef struct
{
    __pSWI_t esMSTUB_RegFuncTbl       ;
    __pSWI_t esMSTUB_UnRegFuncTbl     ;
    __pSWI_t esMSTUB_GetFuncEntry     ;

} SWIHandler_MSTUB_t;

#define SYSCALL_MSTUB(x,y) __swi(SWINO(SWINO_MSTUB, SWIHandler_MSTUB_t, y))x y


SYSCALL_MSTUB(__s32,  esMSTUB_RegFuncTbl)   (__s32 /*id*/, void * /*tbl*/);
SYSCALL_MSTUB(__s32,  esMSTUB_UnRegFuncTbl) (__s32 /*id*/);
SYSCALL_MSTUB(__u32,  esMSTUB_GetFuncEntry) (__s32 /*id*/, __u32  /*funcnum*/);


extern SWIHandler_MSTUB_t SWIHandler_MSTUB;
extern __s32 MSTUB_Init(void);
extern __s32 MSTUB_Exit(void);

#endif 	/* __SYS_MOD_MSTUB_H__ */

