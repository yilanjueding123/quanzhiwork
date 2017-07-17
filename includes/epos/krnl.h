/*
**********************************************************************************************************************
*                                                    ePOS
*                                   the Easy Portable/Player Operation System
*                                               krnl sub-system
*
*                          (c) Copyright 1992-2006, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File    : krnl.h
* By      : Steven.ZGJ
* Version : V1.00
**********************************************************************************************************************
*/

#ifndef   _KRNL_H_
#define   _KRNL_H_

#include "typedef.h"
#include "cfgs.h"


#ifdef EPDK_USED_BY_KRNL
#define __swi(x)
#endif

/* MISCELLANEOUS */
#ifdef   OS_GLOBALS
#define  OS_EXT
#else
#define  OS_EXT  extern
#endif

/*****************************************************************************************************************  */
/* define level 0( system level)                                                                                    */
/*****************************************************************************************************************  */
/* 常数定义                                     ***/
/**************************************************/

/* Version */
#define KRNL_VERSION            100u

/* Config */
#define OS_TASK_CHANGE_PRIO_EN  1                   /* Include code for OSTaskChangePrio()                          */
#define OS_TASK_CREATE_EN       0                   /* Include code for OSTaskCreate()                              */
#define OS_TASK_CREATE_EXT_EN   1                   /* Include code for OSTaskCreateExt()                           */
#define OS_TASK_DEL_EN          1                   /* Include code for OSTaskDel()                                 */
#define OS_TASK_NAME_SIZE       16                  /* Determine the size of a task name                            */
#define OS_TASK_PROFILE_EN      1                   /* Include variables in OS_TCB for profiling                    */
#define OS_TASK_QUERY_EN        1                   /* Include code for OSTaskQuery()                               */
#define OS_TASK_SUSPEND_EN      1                   /* Include code for OSTaskSuspend() and OSTaskResume()          */
#define OS_TASK_SW_HOOK_EN      1                   /* Include code for OSTaskSwHook()                              */

#define OS_CPU_HOOKS_EN         1                   /* uC/OS-II hooks are found in the processor port files         */

#define OS_DEBUG_EN             0                   /* Enable(1) debug variables                                    */


#define OS_EVENT_NAME_SIZE      16                  /* Determine the size of the name of a Sem, Mutex, Mbox or Q    */


#define OS_SCHED_LOCK_EN        1                   /* Include code for OSSchedLock() and OSSchedUnlock()           */

#define OS_TASK_STAT_EN         1                   /* Enable (1) or Disable(0) the statistics task                 */
#define OS_TASK_STAT_STK_CHK_EN 1                   /* Check task stacks from statistic task                        */

#define OS_TICK_STEP_EN         1                   /* Enable tick stepping feature for uC/OS-View                  */
#define OS_TICKS_PER_SEC        100                 /* Set the number of ticks in one second                        */

/* ----------------------- EVENT FLAGS -----------------------  */
#define OS_FLAG_WAIT_CLR_EN     1                   /* Include code for Wait on Clear EVENT FLAGS                   */
#define OS_FLAG_ACCEPT_EN       1                   /* Include code for OSFlagAccept()                              */
#define OS_FLAG_DEL_EN          1                   /* Include code for OSFlagDel()                                 */
#define OS_FLAG_NAME_SIZE       16                  /* Determine the size of the name of an event flag group        */
#define OS_FLAG_QUERY_EN        1                   /* Include code for OSFlagQuery()                               */

#define OS_FLAGS_NBITS          32                  /* Size in #bits of __krnl_flags_t data type (8, 16 or 32)      */

/* --------------- MESSAGE MAILBOXES -------------------------- */
#define OS_MBOX_EN             	1                   /* Enable (1) or Disable (0) code generation for MBOX          	*/
#define OS_MBOX_ACCEPT_EN       1                   /* Include code for OSMboxAccept()                              */
#define OS_MBOX_DEL_EN          1                   /* Include code for OSMboxDel()                                 */
#define OS_MBOX_POST_EN         1                   /* Include code for OSMboxPost()                                */
#define OS_MBOX_POST_OPT_EN     1                   /* Include code for OSMboxPostOpt()                             */
#define OS_MBOX_QUERY_EN        1                   /* Include code for OSMboxQuery()                               */

/* =============== MUTUAL EXCLUSION SEMAPHORES ===============  */
#define OS_MUTEX_EN             1                   /* Enable (1) or Disable (0) code generation for MUTEX          */
#define OS_MUTEX_ACCEPT_EN      1                   /* Include code for OSMutexAccept()                             */
#define OS_MUTEX_DEL_EN         1                   /* Include code for OSMutexDel()                                */
#define OS_MUTEX_QUERY_EN       1                   /* Include code for OSMutexQuery()                              */


/* ------------------------ SEMAPHORES -----------------------  */
#define OS_SEM_EN               1                   /* Enable (1) or Disable (0) code generation for SEMAPHORES     */
#define OS_SEM_ACCEPT_EN        1                   /* Include code for OSSemAccept()                               */
#define OS_SEM_DEL_EN           1                   /* Include code for OSSemDel()                                  */
#define OS_SEM_QUERY_EN         1                   /* Include code for OSSemQuery()                                */
#define OS_SEM_SET_EN           1                   /* Include code for OSSemSet()                                  */

/* --------------------- TIME MANAGEMENT ---------------------  */
#define OS_TIME_DLY_HMSM_EN     1                   /* Include code for OSTimeDlyHMSM()                             */
#define OS_TIME_DLY_RESUME_EN   1                   /* Include code for OSTimeDlyResume()                           */
#define OS_TIME_GET_SET_EN      1                   /* Include code for OSTimeGet() and OSTimeSet()                 */
#define OS_TIME_TICK_HOOK_EN    1                   /* Include code for OSTimeTickHook()                            */


/* Thread MANAGEMENT */
#define KRNL_priolevelstep      32
#define KRNL_priolevelmin       1                   /* 可以分配给应用和模块的prio为32~223，剩下的预留给系统使用     */
#define KRNL_priolevelmax       6                   /* 32个优先级为一个level，共可以分成6个level(32 * 6 = 192)      */

#define KRNL_priolevel0         0                   /* reserved for system(highest level)                           */
#define KRNL_priolevel1         1
#define KRNL_priolevel2         2
#define KRNL_priolevel3         3
#define KRNL_priolevel4         4
#define KRNL_priolevel5         5
#define KRNL_priolevel6         6
#define KRNL_priolevel7         7                   /* reserved for system(lowest level)                            */



#define OS_LOWEST_PRIO          254                 /* Defines the lowest priority that can be assigned ...         */
/* ... MUST NEVER be higher than 254!                           */
#define  OS_PRIO_SELF           0xFFu               /* Indicate SELF priority                                       */
#define  OS_N_SYS_TASKS         16                  /* Number of system tasks                                       */

#define  OS_EVENT_TBL_SIZE ((OS_LOWEST_PRIO)/16+1)  /* Size of event table                                          */
#define  OS_RDY_TBL_SIZE   ((OS_LOWEST_PRIO)/16+1)  /* Size of ready table                                          */
#define  OS_TASK_IDLE_ID        65535u              /* ID numbers for Idle, Stat and Timer tasks                    */
#define  OS_TASK_STAT_ID        65534u
#define  OS_TASK_TMR_ID         65533u


/* ---------------------- MISCELLANEOUS ----------------------  */


#define  OS_ASCII_NUL           (__u8)0
/* TASK STATUS (Bit definition for OSTCBStat) */
#define  OS_STAT_RDY            0x00u               /* Ready to run                                                 */
#define  OS_STAT_SEM            0x01u               /* Pending on semaphore                                         */
#define  OS_STAT_MBOX           0x02u               /* Pending on mailbox                                           */
#define  OS_STAT_Q              0x04u               /* Pending on queue                                             */
#define  OS_STAT_SUSPEND        0x08u               /* Task is suspended                                            */
#define  OS_STAT_MUTEX          0x10u               /* Pending on mutual exclusion semaphore                        */
#define  OS_STAT_FLAG           0x20u               /* Pending on event flag group                                  */
#define  OS_STAT_PEND_ANY       (OS_STAT_SEM | OS_STAT_MBOX | OS_STAT_Q | OS_STAT_MUTEX | OS_STAT_FLAG)
/* OS_EVENT types */
#define  OS_EVENT_TYPE_UNUSED     0u
#define  OS_EVENT_TYPE_MBOX       1u
#define  OS_EVENT_TYPE_Q          2u
#define  OS_EVENT_TYPE_SEM        3u
#define  OS_EVENT_TYPE_MUTEX      4u
#define  OS_EVENT_TYPE_FLAG       5u
#define  OS_TMR_TYPE            100u                /* Used to identify Timers ...                                  */
/* ... (Must be different value than OS_EVENT_TYPE_XXX)         */
/* EVENT FLAGS */
#define  OS_FLAG_WAIT_CLR_ALL     0u                /* Wait for ALL    the bits specified to be CLR (i.e. 0)        */
#define  OS_FLAG_WAIT_CLR_AND     0u
#define  OS_FLAG_WAIT_CLR_ANY     1u                /* Wait for ANY of the bits specified to be CLR (i.e. 0)        */
#define  OS_FLAG_WAIT_CLR_OR      1u
#define  OS_FLAG_WAIT_SET_ALL     2u                /* Wait for ALL    the bits specified to be SET (i.e. 1)        */
#define  OS_FLAG_WAIT_SET_AND     2u
#define  OS_FLAG_WAIT_SET_ANY     3u                /* Wait for ANY of the bits specified to be SET (i.e. 1)        */
#define  OS_FLAG_WAIT_SET_OR      3u
#define  OS_FLAG_CONSUME       0x80u                /* Consume the flags if condition(s) satisfied                  */
#define  OS_FLAG_CLR              0u
#define  OS_FLAG_SET              1u
/* Values for OSTickStepState */
#if OS_TICK_STEP_EN > 0
#define  OS_TICK_STEP_DIS         0u                /* Stepping is disabled, tick runs as mormal                    */
#define  OS_TICK_STEP_WAIT        1u                /* Waiting for uC/OS-View to set OSTickStepState to _ONCE       */
#define  OS_TICK_STEP_ONCE        2u                /* Process tick once and wait for next cmd from uC/OS-View      */
#endif
/* Possible values for 'opt' argument of OSSemDel(), OSMboxDel(), OSQDel() and OSMutexDel() */
#define  OS_DEL_NO_PEND                0u
#define  OS_DEL_ALWAYS                 1u
/* OS_XXX_PostOpt() OPTIONS */
#define  OS_POST_OPT_NONE           0x00u           /* NO option selected                                           */
#define  OS_POST_OPT_BROADCAST      0x01u           /* Broadcast message to ALL tasks waiting                       */
#define  OS_POST_OPT_FRONT          0x02u           /* Post to highest priority task waiting                        */
#define  OS_POST_OPT_NO_SCHED       0x04u           /* Do not call the scheduler if this option is selected         */
/* TASK OPTIONS (see OSTaskCreateExt()) */
#define  OS_TASK_OPT_NONE         0x0000u           /* NO option selected                                           */
#define  OS_TASK_OPT_STK_CHK      0x0001u           /* Enable stack checking for the task                           */
#define  OS_TASK_OPT_STK_CLR      0x0002u           /* Clear the stack when the task is create                      */
#define  OS_TASK_OPT_SAVE_FP      0x0004u           /* Save the contents of any floating-point registers            */
/* TIMER OPTIONS (see OSTmrStart() and OSTmrStop()) */
#define  OS_TMR_OPT_NONE                0u          /* No option selected                                           */
#define  OS_TMR_OPT_ONE_SHOT            1u          /* Timer will not automatically restart when it expires         */
#define  OS_TMR_OPT_PERIODIC            2u          /* Timer will     automatically restart when it expires         */
#define  OS_TMR_OPT_CALLBACK            3u          /* OSTmrStop() option to call 'callback' w/ timer arg.          */
#define  OS_TMR_OPT_CALLBACK_ARG        4u          /* OSTmrStop() option to call 'callback' w/ new   arg.          */
#define  OS_TMR_OPT_NORMAL_MASK      0x0fu          /* soft timer normal option mask                                */
#define  OS_TMR_OPT_PRIO_MASK        0x30u          /* mask of the priority soft timer                              */
#define  OS_TMR_OPT_PRIO_LOW         0x00u          /* low priority soft timer                                      */
#define  OS_TMR_OPT_PRIO_HIGH        0x10u          /* special soft timer, can be operated in isr, used be careful  */

/* TIMER STATES */
#define  OS_TMR_STATE_UNUSED            0u
#define  OS_TMR_STATE_STOPPED           1u
#define  OS_TMR_STATE_COMPLETED         2u
#define  OS_TMR_STATE_RUNNING           3u


/* ERROR CODES */
#define OS_NO_ERR                       0u
#define KRNL_NO_ERR                     0u

#define OS_ERR_EVENT_TYPE               1u
#define OS_ERR_PEND_ISR                 2u
#define OS_ERR_POST_NULL_PTR            3u
#define OS_ERR_PEVENT_NULL              4u
#define OS_ERR_POST_ISR                 5u
#define OS_ERR_QUERY_ISR                6u
#define OS_ERR_INVALID_OPT              7u
#define OS_ERR_TASK_WAITING             8u
#define OS_ERR_PDATA_NULL               9u

#define OS_TIMEOUT                     10u
#define OS_TASK_NOT_EXIST              11u
#define OS_ERR_EVENT_NAME_TOO_LONG     12u
#define OS_ERR_FLAG_NAME_TOO_LONG      13u
#define OS_ERR_TASK_NAME_TOO_LONG      14u
#define OS_ERR_PNAME_NULL              15u
#define OS_ERR_TASK_CREATE_ISR         16u
#define OS_ERR_PEND_LOCKED             17u

#define OS_MBOX_FULL                   20u

#define KRNL_Q_FULL                    30u
#define KRNL_Q_EMPTY                   31u

#define OS_PRIO_EXIST                  40u
#define OS_PRIO_ERR                    41u
#define OS_PRIO_INVALID                42u

#define OS_SEM_OVF                     50u

#define OS_TASK_DEL_ERR                60u
#define OS_TASK_DEL_IDLE               61u
#define OS_TASK_DEL_REQ                62u
#define OS_TASK_DEL_ISR                63u

#define OS_NO_MORE_TCB                 70u

#define OS_TIME_NOT_DLY                80u
#define OS_TIME_INVALID_MINUTES        81u
#define OS_TIME_INVALID_SECONDS        82u
#define OS_TIME_INVALID_MILLI          83u
#define OS_TIME_ZERO_DLY               84u

#define OS_TASK_SUSPEND_PRIO           90u
#define OS_TASK_SUSPEND_IDLE           91u

#define OS_TASK_RESUME_PRIO           100u
#define OS_TASK_NOT_SUSPENDED         101u


#define OS_ERR_NOT_MUTEX_OWNER        120u

#define OS_TASK_OPT_ERR               130u

#define OS_ERR_DEL_ISR                140u
#define OS_ERR_CREATE_ISR             141u

#define OS_FLAG_INVALID_PGRP          150u
#define OS_FLAG_ERR_WAIT_TYPE         151u
#define OS_FLAG_ERR_NOT_RDY           152u
#define OS_FLAG_INVALID_OPT           153u
#define KRNL_FLAG_GRP_DEPLETED        154u

#define OS_ERR_PIP_LOWER              160u

/* TIMER MANAGEMENT */
#define OS_TMR_CFG_MAX                  100         /* Maximum number of timers                                 */
#define OS_TMR_CFG_NAME_SIZE            16          /* Determine the size of a timer name                       */
#define OS_TMR_CFG_WHEEL_SIZE           8           /* Size of timer wheel (#Spokes)                            */

#define KRNL_ERR_TMR                    0x01        /* error managment                                          */
#define KRNL_ERR_TMR_INVALID_PERIOD     ((KRNL_ERR_TMR<<8)+0x00)
#define KRNL_ERR_TMR_INVALID_OPT        ((KRNL_ERR_TMR<<8)+0x01)
#define KRNL_ERR_TMR_INVALID_NAME       ((KRNL_ERR_TMR<<8)+0x02)
#define KRNL_ERR_TMR_NON_AVAIL          ((KRNL_ERR_TMR<<8)+0x03)
#define KRNL_ERR_TMR_INACTIVE           ((KRNL_ERR_TMR<<8)+0x04)
#define KRNL_ERR_TMR_INVALID_DEST       ((KRNL_ERR_TMR<<8)+0x05)
#define KRNL_ERR_TMR_INVALID_TYPE       ((KRNL_ERR_TMR<<8)+0x06)
#define KRNL_ERR_TMR_INVALID            ((KRNL_ERR_TMR<<8)+0x07)
#define KRNL_ERR_TMR_ISR                ((KRNL_ERR_TMR<<8)+0x08)
#define KRNL_ERR_TMR_NAME_TOO_LONG      ((KRNL_ERR_TMR<<8)+0x09)
#define KRNL_ERR_TMR_INVALID_STATE      ((KRNL_ERR_TMR<<8)+0x0a)
#define KRNL_ERR_TMR_STOPPED            ((KRNL_ERR_TMR<<8)+0x0b)
#define KRNL_ERR_TMR_NO_CALLBACK        ((KRNL_ERR_TMR<<8)+0x0c)


/* Socket */
#define KRNL_SKT_USR_IN                 0
#define KRNL_SKT_USR_OUT                1
#define KRNL_SKT_BUF_PHY_UNSEQ          (0x00000000)
#define KRNL_SKT_BUF_PHY_SEQ            (0x01000000)
#define KRNL_SKT_BUF_PHY_SEQMASK        (0x01000000)
#define KRNL_SKT_BUF_TYP_MASK           (0xff000000)

/* tick define */
//#define MELIS_TICK_BASE                 5   //5ms
#define MELIS_TICK_BASE                 5   //5ms
#define MELIS_KRNLTICKS                 2   //2*MELIS_TICK_BASE
#define MELIS_STMRTICKS                 1   //1*MELIS_TICK_BASE


//**************************************************
// 数据类型定义
/* EVENT CONTROL BLOCK */
typedef struct KRNL_EVENT
{
    __u8    OSEventType;                            /* Type of event control block (see OS_EVENT_TYPE_xxxx)     */
    void   *OSEventPtr;                             /* Pointer to message or queue structure                    */
    __u16   OSEventCnt;                             /* Semaphore Count (not used if other EVENT type)           */
    __u16   OSEventGrp;                             /* Group corresponding to tasks waiting for event to occur  */
    __u16   OSEventTbl[OS_EVENT_TBL_SIZE];          /* List of tasks waiting for event to occur                 */
#if OS_EVENT_NAME_SIZE > 1
    __u8    OSEventName[OS_EVENT_NAME_SIZE];
#endif
} __krnl_event_t;

/* EVENT FLAGS CONTROL BLOCK */
#if OS_FLAGS_NBITS == 8                             /* Determine the size of __krnl_flags_t (8, 16 or 32 bits)  */
typedef  __u8    __krnl_flags_t;
#endif

#if OS_FLAGS_NBITS == 16
typedef  __u16   __krnl_flags_t;
#endif

#if OS_FLAGS_NBITS == 32
typedef  __u32   __krnl_flags_t;
#endif

typedef struct os_flag_grp                          /* Event Flag Group                                         */
{
    __u8            OSFlagType;                     /* Should be set to OS_EVENT_TYPE_FLAG                      */
    void         *OSFlagWaitList;                   /* Pointer to first NODE of task waiting on event flag      */
    __krnl_flags_t      OSFlagFlags;                /* 8, 16 or 32 bit flags                                    */
#if OS_FLAG_NAME_SIZE > 1
    __u8            OSFlagName[OS_FLAG_NAME_SIZE];
#endif
} __krnl_flag_grp_t;

typedef struct KRNL_FLAG_NODE                         /* Event Flag Wait List Node                              */
{
    void         *OSFlagNodeNext;                   /* Pointer to next     NODE in wait list                    */
    void         *OSFlagNodePrev;                   /* Pointer to previous NODE in wait list                    */
    void         *OSFlagNodeTCB;                    /* Pointer to TCB of waiting task                           */
    void         *OSFlagNodeFlagGrp;                /* Pointer to Event Flag Group                              */
    __krnl_flags_t  OSFlagNodeFlags;                /* Event flag to wait on                                    */
    __u8            OSFlagNodeWaitType;             /* Type of wait:                                            */
    /* OS_FLAG_WAIT_AND                                         */
    /* OS_FLAG_WAIT_ALL                                         */
    /* OS_FLAG_WAIT_OR                                          */
    /* OS_FLAG_WAIT_ANY                                         */
} __krnl_flag_node_t;

/* MESSAGE MAILBOX DATA */
typedef struct os_mbox_data
{
    void   *OSMsg;                                  /* Pointer to message in mailbox                            */
    __u16  OSEventTbl[OS_EVENT_TBL_SIZE];           /* List of tasks waiting for event to occur                 */
    __u16  OSEventGrp;                              /* Group corresponding to tasks waiting for event to occur  */
} OS_MBOX_DATA;

/* MUTUAL EXCLUSION SEMAPHORE DATA */
typedef struct os_mutex_data
{
    __u16    OSEventTbl[OS_EVENT_TBL_SIZE];         /* List of tasks waiting for event to occur                 */
    __u16    OSEventGrp;                            /* Group corresponding to tasks waiting for event to occur  */
    __u8     OSValue;                               /* Mutex value (0 = used, 1 = available)                    */
    __u8     OSOwnerPrio;                           /* Mutex owner's task priority or 0xFF if no owner          */
    __u8     OSMutexPIP;                            /* Priority Inheritance Priority or 0xFF if no owner        */
} OS_MUTEX_DATA;
/* MESSAGE QUEUE DATA */
typedef struct __KRNL_Q
{
    /* QUEUE CONTROL BLOCK                                      */
    struct __KRNL_Q   *OSQPtr;                      /* Link to next queue control block in list of free blocks  */
    void             **OSQStart;                    /* Pointer to start of queue data                           */
    void             **OSQEnd;                      /* Pointer to end   of queue data                           */
    void             **OSQIn;                       /* Pointer to where next message will be inserted  in   the */
    void             **OSQOut;                      /* Pointer to where next message will be extracted from the */
    __u16              OSQSize;                     /* Size of queue (maximum number of entries)                */
    __u16              OSQEntries;                  /* Current number of entries in the queue                   */
} __krnl_q_t;

typedef struct __krnl_q_data_t
{
    void         *OSMsg;                            /* Pointer to next message to be extracted from queue       */
    __u16         OSNMsgs;                          /* Number of messages in message queue                      */
    __u16         OSQSize;                          /* Size of message queue                                    */
    __u16         OSEventTbl[OS_EVENT_TBL_SIZE];    /* List of tasks waiting for event to occur         */
    __u16         OSEventGrp;                       /* Group corresponding to tasks waiting for event to occur  */
} __krnl_q_data_t;

/* SEMAPHORE DATA */
typedef struct os_sem_data
{
    __u16       OSCnt;                              /* Semaphore count                                          */
    __u16       OSEventTbl[OS_EVENT_TBL_SIZE];      /* List of tasks waiting for event to occur                 */
    __u16       OSEventGrp;                         /* Group corresponding to tasks waiting for event to occur  */
} OS_SEM_DATA;

/* TASK STACK DATA */
typedef struct os_stk_data
{
    __u32       OSFree;                             /* Number of free bytes on the stack                        */
    __u32       OSUsed;                             /* Number of bytes used on the stack                        */
} OS_STK_DATA;

/* PROCESS/MODULE CONTROL BLOCK */
typedef struct __RESM_RSB
{
    __hdle      hResfile;
    __u32       offset;
    __u32       size;
} __resm_rsb_t;
typedef struct __KRNL_XCB
{
    __u8        id;                                 /* 子系统唯一的id号                                         */
    __u16       tcbtbl[OS_EVENT_TBL_SIZE];          /* 记录当前的进程里的线程                                   */
    __mems_heap_t  *heap;                           /* scb的局部堆                                              */
    char           *xfile;                          /* 当前文件的文件路径和文件名                               */
    __resm_rsb_t   *them;                           /* 其对应的当前资源文件                                     */
    __resm_rsb_t   *lang;                           /* 其对应的当前资源文件                                     */
} __krnl_xcb_t;

/* TASK CONTROL BLOCK */
typedef struct __KRNL_TCB
{
    __stk              *OSTCBStkPtr;                /* Pointer to current top of stack                          */
    void               *OSTCBExtPtr;                /* Pointer to user definable data for TCB extension         */
    __stk              *OSTCBStkBottom;             /* Pointer to bottom of stack                               */
    __u32               OSTCBStkSize;               /* Size of task stack (in number of stack elements)         */
    __u16               OSTCBOpt;                   /* Task options as passed by OSTaskCreateExt()              */
    __u16               OSTCBId;                    /* Task ID (0..65535)                                       */
    struct __KRNL_TCB  *OSTCBNext;                  /* Pointer to next     TCB in the TCB list                  */
    struct __KRNL_TCB  *OSTCBPrev;                  /* Pointer to previous TCB in the TCB list                  */
    __krnl_event_t     *OSTCBEventPtr;              /* Pointer to event control block                           */
    void               *OSTCBMsg;                   /* Message received from OSMboxPost() or OSQPost()          */
    __krnl_flag_node_t *OSTCBFlagNode;              /* Pointer to event flag node                               */
    __krnl_flags_t      OSTCBFlagsRdy;              /* Event flags that made task ready to run                  */
    __u16               OSTCBDly;                   /* Nbr ticks to delay task or, timeout waiting for event    */
    __u8                OSTCBStat;                  /* Task status                                              */
    __bool              OSTCBPendTO;                /* Flag indicating PEND timed out (EPDK_TRUE == timed out)  */
    __u8                OSTCBPrio;                  /* Task priority (0 == highest)                             */
    __u8                OSTCBX;                     /* Bit position in group  corresponding to task priority    */
    __u8                OSTCBY;                     /* Index into ready table corresponding to task priority    */
    __u16               OSTCBBitX;                  /* Bit mask to access bit position in ready table           */
    __u16               OSTCBBitY;                  /* Bit mask to access bit position in ready group           */
    __u8                OSTCBDelReq;                /* Indicates whether a task needs to delete itself          */
    __u32               OSTCBCtxSwCtr;              /* Number of time the task was switched in                  */
    __u32               OSTCBCyclesTot;             /* Total number of clock cycles the task has been running   */
    __u32               OSTCBCyclesStart;           /* Snapshot of cycle counter at start of task resumption    */
    __stk              *OSTCBStkBase;               /* Pointer to the beginning of the task stack               */
    __u32               OSTCBStkUsed;               /* Number of bytes used from the stack                      */
#if OS_TASK_NAME_SIZE > 1
    __u8                OSTCBTaskName[OS_TASK_NAME_SIZE];
#endif

    __krnl_xcb_t   *xcb;      //指向线程所在的scb

#if EPOS_CPU_STAT_EN > 0
    __u32               usetime_s;
    __u32               usetime_2us;
#endif

    __u8                task_pid;                   /* pid of the module which the task blonged to              */

} __krnl_tcb_t;

/* TIMER DATA TYPES */
typedef  void (*OS_TMR_CALLBACK)(void *parg);

typedef  struct  KNRL_STMR
{
    __u8            OSTmrType;                      /* Should be set to OS_TMR_TYPE                             */
    OS_TMR_CALLBACK OSTmrCallback;                  /* Function to call when timer expires                      */
    void            *OSTmrCallbackArg;              /* Argument to pass to function when timer expires          */
    void            *OSTmrNext;                     /* Double link list pointers                                */
    void            *OSTmrPrev;
    __u32           OSTmrMatch;                     /* Timer expires when OSTmrTime == OSTmrMatch               */
    __u32           OSTmrDly;                       /* Delay time before periodic update starts                 */
    __u32           OSTmrPeriod;                    /* Period to repeat timer                                   */
#if OS_TMR_CFG_NAME_SIZE > 0
    __u8            OSTmrName[OS_TMR_CFG_NAME_SIZE];/* Name to give the timer                                   */
#endif
    __u8            OSTmrOpt;                       /* Options (see OS_TMR_OPT_XXX)                             */
    __u8            OSTmrState;                     /* Indicates the state of the timer:                        */
    /* OS_TMR_STATE_UNUSED                                      */
    /* OS_TMR_STATE_RUNNING                                     */
    /* OS_TMR_STATE_STOPPED                                     */
    __u16           err;                            /* timer error flag                                         */
    __s32           linked;                         /* flag to mark if the timer has been linked                */
    void            *node;                          /* pinter to the soft timer node                            */
} __krnl_stmr_t;

typedef  struct  os_tmr_wheel
{
    __krnl_stmr_t  *OSTmrFirst;                     /* Pointer to first timer in linked list                    */
    __u16           OSTmrEntries;
} OS_TMR_WHEEL;

//socket
typedef struct __KRNL_SKTFRM
{
    __u32       size;                           /* 消息帧的大小                                             */

    void       *msg;
    __u32       msgtype;                        /* user define                                              */
    __u32       msgsize;
    __u32       msglen;

    void       *data;                           /* 数据buf的地址                                            */
    __u32       datsize;                        /* 数据buf的大小                                            */
    __u32       datatype;                       /* bit1~0: frame head/end flag 00: mid 01: end, 10: start, 11:start+end     */
    __u32       datalen;                        /* data的长度                                               */

    __u8        reserve[12];

} __krnl_sktfrm_t;


#if OS_TMR_CFG_NAME_SIZE > 0
//cpu stat parameter
typedef struct __KRNL_CPU_STAT
{
    __u64       CpuCycleCnt;
    __u64       InstructCnt;

    __u64       ICacheAcesCnt;
    __u64       ICacheHitCnt;

    __u64       DCacheAcesCnt;
    __u64       DCacheHitCnt;

} __krnl_cpu_stat_t;
#endif


//********************************************************************************************************************
// 变量定义
/* GLOBAL VARIABLES */

//************************************************
// 函数定义
/* system call table */
typedef struct
{
    __pSWI_t esKRNL_TCreate             ;
    __pSWI_t esKRNL_TDel                ;
    __pSWI_t esKRNL_TDelReq             ;
    __pSWI_t esKRNL_GetPrio             ;
    __pSWI_t esKRNL_FreePrio            ;
    __pSWI_t esKRNL_TaskChangePrio      ;
    __pSWI_t esKRNL_TaskDel             ;
    __pSWI_t esKRNL_TaskDelReq          ;
    __pSWI_t esKRNL_TaskNameGet         ;
    __pSWI_t esKRNL_TaskNameSet         ;
    __pSWI_t esKRNL_TaskResume          ;
    __pSWI_t esKRNL_TaskSuspend         ;
    __pSWI_t esKRNL_TaskStkChk          ;
    __pSWI_t esKRNL_TaskQuery           ;
    __pSWI_t esKRNL_TimeDly             ;
    __pSWI_t esKRNL_TimeDlyHMSM         ;
    __pSWI_t esKRNL_TimeDlyResume       ;
    __pSWI_t esKRNL_TimeGet             ;
    __pSWI_t esKRNL_TimeSet             ;
    __pSWI_t esKRNL_SemAccept           ;
    __pSWI_t esKRNL_SemCreate           ;
    __pSWI_t esKRNL_SemDel              ;
    __pSWI_t esKRNL_SemPend             ;
    __pSWI_t esKRNL_SemPost             ;
    __pSWI_t esKRNL_SemQuery            ;
    __pSWI_t esKRNL_SemSet              ;
    __pSWI_t esKRNL_MboxAccept          ;
    __pSWI_t esKRNL_MboxCreate          ;
    __pSWI_t esKRNL_MboxDel             ;
    __pSWI_t esKRNL_MboxPend            ;
    __pSWI_t esKRNL_MboxPost            ;
    __pSWI_t esKRNL_MboxPostOpt         ;
    __pSWI_t esKRNL_MboxQuery           ;
    __pSWI_t esKRNL_MutexAccept         ;
    __pSWI_t esKRNL_MutexCreate         ;
    __pSWI_t esKRNL_MutexDel            ;
    __pSWI_t esKRNL_MutexPend           ;
    __pSWI_t esKRNL_MutexPost           ;
    __pSWI_t esKRNL_MutexQuery          ;
    __pSWI_t esKRNL_QAccept             ;
    __pSWI_t esKRNL_QCreate             ;
    __pSWI_t esKRNL_QDel                ;
    __pSWI_t esKRNL_QFlush              ;
    __pSWI_t esKRNL_QPend               ;
    __pSWI_t esKRNL_QPost               ;
    __pSWI_t esKRNL_QPostFront          ;
    __pSWI_t esKRNL_QPostOpt            ;
    __pSWI_t esKRNL_QQuery              ;
    __pSWI_t esKRNL_FlagAccept          ;
    __pSWI_t esKRNL_FlagCreate          ;
    __pSWI_t esKRNL_FlagDel             ;
    __pSWI_t esKRNL_FlagNameGet         ;
    __pSWI_t esKRNL_FlagNameSet         ;
    __pSWI_t esKRNL_FlagPend            ;
    __pSWI_t esKRNL_FlagPendGetFlagsRdy ;
    __pSWI_t esKRNL_FlagPost            ;
    __pSWI_t esKRNL_FlagQuery           ;

    __pSWI_t esKRNL_TmrCreate           ;
    __pSWI_t esKRNL_TmrDel              ;
    __pSWI_t esKRNL_TmrStart            ;
    __pSWI_t esKRNL_TmrStop             ;
    __pSWI_t esKRNL_TmrRemainGet        ;
    __pSWI_t esKRNL_TmrStateGet         ;
    __pSWI_t esKRNL_TmrError            ;

    __pSWI_t esKRNL_Version             ;
    __pSWI_t esKRNL_SchedLock           ;
    __pSWI_t esKRNL_SchedUnlock         ;
    __pSWI_t esKRNL_GetCallBack         ;
    __pSWI_t esKRNL_CallBack            ;

    __pSWI_t esKRNL_GetTIDCur           ;

    __pSWI_t esKRNL_SktCreate           ;
    __pSWI_t esKRNL_SktDel              ;
    __pSWI_t esKRNL_SktPend             ;
    __pSWI_t esKRNL_SktPost             ;
    __pSWI_t esKRNL_SktAccept           ;
    __pSWI_t esKRNL_SktFlush            ;
    __pSWI_t esKRNL_SktError            ;

    __pSWI_t esKRNL_Time                ;

    __pSWI_t esKRNL_CPUStatStart        ;
    __pSWI_t esKRNL_CPUStatStop         ;
    __pSWI_t esKRNL_CPUStatReport       ;


} SWIHandler_KRNL_t;
#ifndef SIM_PC_WIN
#define SYSCALL_KRNL(x,y) __swi(SWINO(SWINO_KRNL, SWIHandler_KRNL_t, y))x y
#else
#define SYSCALL_KRNL(x,y) x y
#endif
/* Thread MANAGEMENT */
SYSCALL_KRNL(__u8,              esKRNL_TCreate    )(__pTHD_t/*thd*/, void */*p_arg*/, __u32/*stksz*/, __u16/*plvl*/);
SYSCALL_KRNL(__s8,              esKRNL_TDel       )(__u8 prio);
SYSCALL_KRNL(__s8,              esKRNL_TDelReq    )(__u16 prio_ex);
SYSCALL_KRNL(__u8,              esKRNL_GetPrio    )(__u8 priolevel);
SYSCALL_KRNL(void,              esKRNL_FreePrio   )(__u8 prio);
SYSCALL_KRNL(__u8,              esKRNL_TaskChangePrio)(__u8 oldprio, __u8 newprio);
SYSCALL_KRNL(__u8,              esKRNL_TaskDel    )(__u8 prio);
SYSCALL_KRNL(__u8,              esKRNL_TaskDelReq )(__u16 prio_ex);
SYSCALL_KRNL(__u8,              esKRNL_TaskNameGet)(__u8 prio, __u8 *pname, __u8 *err);
SYSCALL_KRNL(void,              esKRNL_TaskNameSet)(__u8 prio, __u8 *pname, __u8 *err);
SYSCALL_KRNL(__u8,              esKRNL_TaskResume )(__u8 prio);
SYSCALL_KRNL(__u8,              esKRNL_TaskSuspend)(__u8 prio);
SYSCALL_KRNL(__u8,              esKRNL_TaskStkChk )(__u8 prio, OS_STK_DATA *p_stk_data);
SYSCALL_KRNL(__u8,              esKRNL_TaskQuery  )(__u8 prio, __krnl_tcb_t *p_task_data);
SYSCALL_KRNL(__u8,              esKRNL_GetTIDCur  )(void);
/* TIME MANAGEMENT */
SYSCALL_KRNL(void,              esKRNL_TimeDly    )(__u16 ticks);
SYSCALL_KRNL(__u8,              esKRNL_TimeDlyHMSM)(__u8 hours, __u8 minutes, __u8 seconds, __u16 milli);
SYSCALL_KRNL(__u8,              esKRNL_TimeDlyResume)(__u16 prio_ex);
SYSCALL_KRNL(__u32,             esKRNL_TimeGet    )(void);
SYSCALL_KRNL(void,              esKRNL_TimeSet    )(__u32 ticks);
/* SEMAPHORE MANAGEMENT */
SYSCALL_KRNL(__u16,             esKRNL_SemAccept  )(__krnl_event_t *pevent);
SYSCALL_KRNL(__krnl_event_t *,   esKRNL_SemCreate  )(__u16 cnt);
SYSCALL_KRNL(__krnl_event_t *,   esKRNL_SemDel     )(__krnl_event_t *pevent, __u8 opt, __u8 *err);
SYSCALL_KRNL(void,              esKRNL_SemPend    )(__krnl_event_t *pevent, __u16 timeout, __u8 *err);
SYSCALL_KRNL(__u8,              esKRNL_SemPost    )(__krnl_event_t *pevent);
SYSCALL_KRNL(__u8,              esKRNL_SemQuery   )(__krnl_event_t *pevent, OS_SEM_DATA *p_sem_data);
SYSCALL_KRNL(void,              esKRNL_SemSet     )(__krnl_event_t *pevent, __u16 cnt, __u8 *err);
/* MESSAGE MAILBOX MANAGEMENT */
SYSCALL_KRNL(void *,             esKRNL_MboxAccept )(__krnl_event_t *pevent);
SYSCALL_KRNL(__krnl_event_t *,   esKRNL_MboxCreate )(void *msg);
SYSCALL_KRNL(__krnl_event_t *,   esKRNL_MboxDel    )(__krnl_event_t *pevent, __u8 opt, __u8 *err);
SYSCALL_KRNL(void *,             esKRNL_MboxPend   )(__krnl_event_t *pevent, __u16 timeout, __u8 *err);
SYSCALL_KRNL(__u8,              esKRNL_MboxPost   )(__krnl_event_t *pevent, void *msg);
SYSCALL_KRNL(__u8,              esKRNL_MboxPostOpt)(__krnl_event_t *pevent, void *msg, __u8 opt);
SYSCALL_KRNL(__u8,              esKRNL_MboxQuery  )(__krnl_event_t *pevent, OS_MBOX_DATA *p_mbox_data);
/* MUTUAL EXCLUSION SEMAPHORE MANAGEMENT */
SYSCALL_KRNL(__u8,              esKRNL_MutexAccept)(__krnl_event_t *pevent, __u8 *err);
SYSCALL_KRNL(__krnl_event_t *,   esKRNL_MutexCreate)(__u8 prio, __u8 *err);
SYSCALL_KRNL(__krnl_event_t *,   esKRNL_MutexDel   )(__krnl_event_t *pevent, __u8 opt, __u8 *err);
SYSCALL_KRNL(void,              esKRNL_MutexPend  )(__krnl_event_t *pevent, __u16 timeout, __u8 *err);
SYSCALL_KRNL(__u8,              esKRNL_MutexPost  )(__krnl_event_t *pevent);
SYSCALL_KRNL(__u8,              esKRNL_MutexQuery )(__krnl_event_t *pevent, OS_MUTEX_DATA *p_mutex_data);
/* MESSAGE QUEUE MANAGEMENT */
SYSCALL_KRNL(void *,             esKRNL_QAccept    )(__krnl_event_t *pevent, __u8 *err);
SYSCALL_KRNL(__krnl_event_t *,   esKRNL_QCreate    )(__u16 size);
SYSCALL_KRNL(__krnl_event_t *,   esKRNL_QDel       )(__krnl_event_t *pevent, __u8 opt, __u8 *err);
SYSCALL_KRNL(__u8,              esKRNL_QFlush     )(__krnl_event_t *pevent);
SYSCALL_KRNL(void *,             esKRNL_QPend      )(__krnl_event_t *pevent, __u16 timeout, __u8 *err);
SYSCALL_KRNL(__u8,              esKRNL_QPost      )(__krnl_event_t *pevent, void *msg);
SYSCALL_KRNL(__u8,              esKRNL_QPostFront )(__krnl_event_t *pevent, void *msg);
SYSCALL_KRNL(__u8,              esKRNL_QPostOpt   )(__krnl_event_t *pevent, void *msg, __u8 opt);
SYSCALL_KRNL(__u8,              esKRNL_QQuery     )(__krnl_event_t *pevent, __krnl_q_data_t *p_q_data);
/* EVENT FLAGS MANAGEMENT */
SYSCALL_KRNL(__krnl_flags_t,    esKRNL_FlagAccept )(__krnl_flag_grp_t */*pgrp*/, __krnl_flags_t/*flags*/, __u8/*wait_type*/, __u8 */*err*/);
SYSCALL_KRNL(__krnl_flag_grp_t *, esKRNL_FlagCreate )(__krnl_flags_t flags, __u8 *err);
SYSCALL_KRNL(__krnl_flag_grp_t *, esKRNL_FlagDel    )(__krnl_flag_grp_t *pgrp, __u8 opt, __u8 *err);
SYSCALL_KRNL(__u8,              esKRNL_FlagNameGet)(__krnl_flag_grp_t *pgrp, __u8 *pname, __u8 *err);
SYSCALL_KRNL(void,              esKRNL_FlagNameSet)(__krnl_flag_grp_t *pgrp, __u8 *pname, __u8 *err);
SYSCALL_KRNL(__krnl_flags_t,    esKRNL_FlagPend   )(__krnl_flag_grp_t *pgrp, __krnl_flags_t flags, __u32 wtype_tout, __u8 *err);
SYSCALL_KRNL(__krnl_flags_t,    esKRNL_FlagPendGetFlagsRdy)(void);
SYSCALL_KRNL(__krnl_flags_t,    esKRNL_FlagPost   )(__krnl_flag_grp_t *pgrp, __krnl_flags_t flags, __u8 opt, __u8 *err);
SYSCALL_KRNL(__krnl_flags_t,    esKRNL_FlagQuery  )(__krnl_flag_grp_t *pgrp, __u8 *err);
/* TIMER MANAGEMENT */
SYSCALL_KRNL(__krnl_stmr_t *,    esKRNL_TmrCreate  )(__u32 period, __u8 opt, OS_TMR_CALLBACK  callback, void *callback_arg);
SYSCALL_KRNL(__s32,             esKRNL_TmrDel     )(__krnl_stmr_t */*ptmr*/);
SYSCALL_KRNL(__u32,             esKRNL_TmrRemainGet)(__krnl_stmr_t */*ptmr*/);
SYSCALL_KRNL(__u8,              esKRNL_TmrStateGet)(__krnl_stmr_t */*ptmr*/);
SYSCALL_KRNL(__s32,             esKRNL_TmrStart   )(__krnl_stmr_t */*ptmr*/);
SYSCALL_KRNL(__s32,             esKRNL_TmrStop    )(__krnl_stmr_t */*ptmr*/, __u8/*opt*/, void */*cbk_arg*/);
SYSCALL_KRNL(__u16,             esKRNL_TmrError   )(__krnl_stmr_t */*ptmr*/);
/* MISCELLANEOUS */
SYSCALL_KRNL(__u16,             esKRNL_Version    )(void);
SYSCALL_KRNL(void,              esKRNL_SchedLock  )(void);
SYSCALL_KRNL(void,              esKRNL_SchedUnlock)(void);
SYSCALL_KRNL(__pCBK_t,          esKRNL_GetCallBack)(__pCBK_t/*cb*/);
SYSCALL_KRNL(__s32,             esKRNL_CallBack   )(__pCBK_t/*cb*/, void */*p_arg*/);

/* Socket management    */
SYSCALL_KRNL(__hdle,            esKRNL_SktCreate  )(__u32 /*depth*/, __u32/*dbuf_attr*/, __u32/*mbuf_attr*/);
SYSCALL_KRNL(__s32,             esKRNL_SktDel     )(__hdle/*hSKT */, __u8 /*opt */);
SYSCALL_KRNL(__krnl_sktfrm_t *,  esKRNL_SktPend    )(__hdle/*hSKT */, __u8 /*user*/, __u32/*timeout*/);
SYSCALL_KRNL(__s32,             esKRNL_SktPost    )(__hdle/*hSKT */, __u8 /*user*/, __krnl_sktfrm_t */*frm*/);
SYSCALL_KRNL(__krnl_sktfrm_t *,  esKRNL_SktAccept  )(__hdle/*pSKT */, __u8 /*user*/);
SYSCALL_KRNL(__s32,             esKRNL_SktFlush   )(__hdle/*hSKT */, __u8 /*user*/);
SYSCALL_KRNL(__u32,             esKRNL_SktError   )(__hdle/*hSKT*/);

SYSCALL_KRNL(__u32,             esKRNL_Time       )(void);

SYSCALL_KRNL(__s32,             esKRNL_CPUStatStart)(void);
SYSCALL_KRNL(__s32,             esKRNL_CPUStatStop)(void);
SYSCALL_KRNL(void,              esKRNL_CPUStatReport)(void);

//**********************************************************************************************************

//**********************************************************************************************************
//* define level 1( system level)
//************************************************
// 常数定义
#define KERN_priolevelstep  8

//************************************************
// 数据类型定义

//************************************************
// 变量定义
extern SWIHandler_KRNL_t SWIHandler_KRNL;
//************************************************
// 函数定义
void esKRNL_Init            (void);
void KRNL_Start             (void);
/* TIME MANAGEMENT */
void KRNL_TimeTick          (void);
/* MISCELLANEOUS */
#if (OS_EVENT_NAME_SIZE > 1)

__u8 OSEventNameGet(__krnl_event_t *pevent, __u8 *pname, __u8 *err);
void OSEventNameSet(__krnl_event_t *pevent, __u8 *pname, __u8 *err);

#endif
/* FUNCTION PROTOTYPES */
__stk *OSTaskStkInit(void (*task)(void *p_arg), void *p_arg, __stk *ptos, __u16 opt);

#endif  /* _KRNL_H_ */

