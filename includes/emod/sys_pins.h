/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Pin Manage Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : sys_pin.h
* By      : kevin.z
* Version : v2.0
* Date    : 2010-11-27 9:26
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-27 9:26     kevin.z     2.0     build the file;
*           2011-6-18 13:24:07  sunny       2.0     add pin int support.
*********************************************************************************************************
*/
#ifndef __SYS_PINS_H__
#define __SYS_PINS_H__

#include "typedef.h"
#include "cfgs.h"

#ifdef EPDK_USED_BY_KRNL
#define __swi(x)
#endif

typedef struct
{
    __pSWI_t esPINS_PinGrpReq    ;
    __pSWI_t esPINS_PinGrpRel    ;
    __pSWI_t esPINS_GetPinGrpStat;
    __pSWI_t esPINS_GetPinStat   ;
    __pSWI_t esPINS_SetPinStat   ;
    __pSWI_t esPINS_SetPinIO     ;
    __pSWI_t esPINS_SetPinPull   ;
    __pSWI_t esPINS_SetPinDrive  ;
    __pSWI_t esPINS_ReadPinData  ;
    __pSWI_t esPINS_WritePinData ;
    __pSWI_t esPINS_EnbaleInt    ;
    __pSWI_t esPINS_DisbaleInt   ;
    __pSWI_t esPINS_QueryInt     ;
    __pSWI_t esPINS_SetIntMode   ;
    __pSWI_t esPINS_RegIntHdler  ;
    __pSWI_t esPINS_UnregIntHdler;
    __pSWI_t esPINS_ClearPending;

} SWIHandler_PINS_t;

#ifndef SIM_PC_WIN
#define SYSCALL_PINS(x,y) __swi(SWINO(SWINO_PINS, SWIHandler_PINS_t, y))x y
#else
#define SYSCALL_PINS(x,y) x y
#endif

SYSCALL_PINS(__hdle, esPINS_PinGrpReq    )(user_gpio_set_t *pGrpStat, __u32 GrpSize);
SYSCALL_PINS(__s32 , esPINS_PinGrpRel    )(__hdle hPin, __s32 bRestore);
SYSCALL_PINS(__s32 , esPINS_GetPinGrpStat)(__hdle hPin, user_gpio_set_t *pGrpStat, __u32 GrpSize, __bool bFromHW);
SYSCALL_PINS(__s32 , esPINS_GetPinStat   )(__hdle hPin, user_gpio_set_t *pPinStat, const char *pPinName, __bool bFromHW);
SYSCALL_PINS(__s32 , esPINS_SetPinStat   )(__hdle hPin, user_gpio_set_t *pPinStat, const char *pPinName, __bool bSetUserStat);
SYSCALL_PINS(__s32 , esPINS_SetPinIO     )(__hdle hPin, __bool bOut, const char *pPinName);
SYSCALL_PINS(__s32 , esPINS_SetPinPull   )(__hdle hPin, __u32 PullStat, const char *pPinName);
SYSCALL_PINS(__s32 , esPINS_SetPinDrive  )(__hdle hPin, __u32 DriveLevel, const char *pPinName);
SYSCALL_PINS(__s32 , esPINS_ReadPinData  )(__hdle hPin, const char *pPinName);
SYSCALL_PINS(__s32 , esPINS_WritePinData )(__hdle hPin, __u32 Value, const char *pPinName);
SYSCALL_PINS(__s32 , esPINS_EnbaleInt    )(__hdle hPin);
SYSCALL_PINS(__s32 , esPINS_DisbaleInt   )(__hdle hPin);
SYSCALL_PINS(__s32 , esPINS_QueryInt     )(__hdle hPin, __bool *pIntStatus);
SYSCALL_PINS(__s32 , esPINS_SetIntMode   )(__hdle hPin, __u32 IntMode);
SYSCALL_PINS(__s32 , esPINS_RegIntHdler  )(__hdle hPin, __pCBK_t hdler, void *arg);
SYSCALL_PINS(__s32 , esPINS_UnregIntHdler)(__hdle hPin, __pCBK_t hdler);
SYSCALL_PINS(__s32 , esPINS_ClearPending )(__hdle hPin);

extern SWIHandler_PINS_t SWIHandler_PINS;

//************************************************
// º¯Êý¶¨Òå
//************************************************
extern __s32 PINS_Init(void);
extern __s32 PINS_Exit(void);

#endif  //__SYS_PINS_H__
