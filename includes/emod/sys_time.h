/*
*********************************************************************************************************
*											        ePOS
*						           the Easy Portable/Player Operation System
*									           time sub-system
*
*                                    (c) Copyright 2007-2008, Steven.ZGJ China
*                                          All Rights Reserved
*
*                                     module management headfile
*
* File   : sys_time.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_

#include "typedef.h"
#include "cfgs.h"

#ifdef EPDK_USED_BY_KRNL
#define __swi(x)
#endif

#define TIME_TMRMODE_T_1US          0   /*us timer */
#define TIME_TMRMODE_T_1MS          1   /*ms timer */

#define ALARM_INTERRUPT_NORMAL  0
#define ALARM_INTERRUPT_EXTNMI  1

//hardware timer type for request timer
typedef CSP_TMRC_TmrType_t      __hw_tmr_type_t;

typedef struct __DATE_T
{
    __u16   year;
    __u8    month;
    __u8    day;

} __date_t;

typedef struct __TIME_T
{
    __u8    hour;
    __u8    minute;
    __u8    second;

} __time_t;

typedef enum __TMR_CNTR_STAT
{
    TMR_CNT_STAT_INVALID = 0,
    TMR_CNT_STAT_STOP   = 1,
    TMR_CNT_STAT_RUN    = 2,
    TMR_CNT_STAT_PAUSE  = 3,
    TMR_CNT_STAT_

} __tmr_cntr_stat_e;


typedef CSP_TMRC_TmrMode_t      __hw_tmr_mode_e;
#define TMR_SHOT_MODE_ONLYONE   CSP_TMRC_TMR_MODE_ONE_SHOOT //Timer will not automatically restart when it expires
#define TMR_SHOT_MODE_PERIODIC  CSP_TMRC_TMR_MODE_CONTINU   //Timer will     automatically restart when it expires


typedef struct
{
    __pSWI_t esTIME_RequestTimer        ;
    __pSWI_t esTIME_ReleaseTimer        ;
    __pSWI_t esTIME_StartTimer          ;
    __pSWI_t esTIME_StopTimer           ;
    __pSWI_t esTIME_QuerryTimer         ;

    __pSWI_t esTIME_GetTime             ;
    __pSWI_t esTIME_SetTime             ;
    __pSWI_t esTIME_GetDate             ;
    __pSWI_t esTIME_SetDate             ;
    __pSWI_t esTIME_RequestAlarm        ;
    __pSWI_t esTIME_ReleaseAlarm        ;
    __pSWI_t esTIME_StartAlarm          ;
    __pSWI_t esTIME_StopAlarm           ;
    __pSWI_t esTIME_QuerryAlarm         ;

    __pSWI_t esTIME_RequestCntr         ;
    __pSWI_t esTIME_ReleaseCntr         ;
    __pSWI_t esTIME_StartCntr           ;
    __pSWI_t esTIME_StopCntr            ;
    __pSWI_t esTIME_PauseCntr           ;
    __pSWI_t esTIME_ContiCntr           ;
    __pSWI_t esTIME_SetCntrValue        ;
    __pSWI_t esTIME_QuerryCntr          ;
    __pSWI_t esTIME_SetCntrPrescale     ;
    __pSWI_t esTIME_QuerryCntrStat      ;

} SWIHandler_TIME_t;


extern SWIHandler_TIME_t SWIHandler_TIME;
#ifndef SIM_PC_WIN
#define SYSCALL_TIME(x,y) __swi(SWINO(SWINO_TIME, SWIHandler_TIME_t, y))x y
#else
#define SYSCALL_TIME(x,y) x y
#endif


SYSCALL_TIME(__hdle, esTIME_RequestTimer    )(__hw_tmr_type_t *pType, __pCBK_t pHdlr, void *pArg, char *pUsr);
SYSCALL_TIME(__s32,  esTIME_ReleaseTimer    )(__hdle hTmr);
SYSCALL_TIME(__s32,  esTIME_StartTimer      )(__hdle hTmr, __u32 nPeriod, __hw_tmr_mode_e nMode);
SYSCALL_TIME(__s32,  esTIME_StopTimer       )(__hdle hTmr);
SYSCALL_TIME(__u32,  esTIME_QuerryTimer     )(__hdle hTmr);

SYSCALL_TIME(__s32,  esTIME_GetTime         )(__time_t *time);
SYSCALL_TIME(__s32,  esTIME_SetTime         )(__time_t *time);
SYSCALL_TIME(__s32,  esTIME_GetDate         )(__date_t *date);
SYSCALL_TIME(__s32,  esTIME_SetDate         )(__date_t *date);
SYSCALL_TIME(__hdle, esTIME_RequestAlarm    )(__u32 mode);
SYSCALL_TIME(__s32,  esTIME_ReleaseAlarm    )(__hdle alarm);
SYSCALL_TIME(__s32,  esTIME_StartAlarm      )(__hdle alarm, __u32 time);
SYSCALL_TIME(__s32,  esTIME_StopAlarm       )(__hdle alarm);
SYSCALL_TIME(__u32,  esTIME_QuerryAlarm     )(__hdle alarm);

SYSCALL_TIME(__hdle, esTIME_RequestCntr     )(__pCB_ClkCtl_t cb, char *pUsr);
SYSCALL_TIME(__s32,  esTIME_ReleaseCntr     )(__hdle hCntr);
SYSCALL_TIME(__s32,  esTIME_StartCntr       )(__hdle hCntr);
SYSCALL_TIME(__s32,  esTIME_StopCntr        )(__hdle hCntr);
SYSCALL_TIME(__s32,  esTIME_PauseCntr       )(__hdle hCntr);
SYSCALL_TIME(__s32,  esTIME_ContiCntr       )(__hdle hCntr);
SYSCALL_TIME(__s32,  esTIME_SetCntrValue    )(__hdle hCntr, __u32 value);
SYSCALL_TIME(__u32,  esTIME_QuerryCntr      )(__hdle hCntr);
SYSCALL_TIME(__s32,  esTIME_SetCntrPrescale )(__hdle hCntr, __s32 prescl);
SYSCALL_TIME(__s32,  esTIME_QuerryCntrStat  )(__hdle hCntr);


//==============================================================
// define interface for kernel
//==============================================================
extern SWIHandler_TIME_t SWIHandler_TIME;
__s32 TIME_Init(void);
__s32 TIME_Exit(void);
__s32 TMR_Init(void);
__s32 TMR_Exit(void);
__s32 RTC_Init(void);
__s32 RTC_Exit(void);
__s32 CNTR_Init(void);
__s32 CNTR_Exit(void);
__s32 RTC_QueryInt(__s32 type);
__s32 TIME_KRNLTickInit(void);
__s32 TIME_SetKRNLTick(__pISR_t kerntick, __u32 period);
__s32 TIME_SetSTMRTick(__pISR_t stmrtick, __u32 period);



#endif  //define _SYS_TIME_H_


