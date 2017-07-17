/*
*********************************************************************************************************
*                                                    eMOD
*                                         the melis Operation System
*                                               input sub-system
*                                        input sub-system internal head file
*
*                                    (c) Copyright 2010-2012, sunny China
*                                              All Rights Reserved
*
* File   : sys_input.h
* Version: V1.0
* By     : Sunny
* Date   : 2010-10-27 8:47:23
* Note   : input system extern service for input driver and event user.
*********************************************************************************************************
*/
#ifndef __SYS_INPUT_H__
#define __SYS_INPUT_H__

#include "typedef.h"
#include "cfgs.h"

#ifdef EPDK_USED_BY_KRNL
#define __swi(x)
#endif

/* The class name of INPUT devices */
#define INPUT_CLASS_NAME           "INPUT"

/* INPUT logical devices name, use for graber logical device */
#define INPUT_LKEYBOARD_DEV_NAME   "LKeyBoardDev"
#define INPUT_LMOUSE_DEV_NAME      "LMouseDev"
#define INPUT_LTS_DEV_NAME         "LTSDev"

/* input system limits */
#define INPUT_LOGICALDEV_MAX        16   /*  max logical device number              */
#define INPUT_CHILDREN_MAX          8    /*  max children number of logical device  */
#define INPUT_EV_BUFFER_MAXLEN      256  /*  max event buffer length                */
#define INPUT_MAX_EVENT_NR          64   /*  max event number                       */

/* input device classes */
#define INPUT_KEYBOARD_CLASS  0    /* keyboard class device   */
#define INPUT_MOUSE_CLASS     1    /* mouse class device      */
#define INPUT_TS_CLASS        2    /* touchpanel class device */

/* basic bit operations */
#define INPUT_BITS_PER_BYTE		    8
#define INPUT_DIV_ROUND_UP(n,d)     (((n) + (d) - 1) / (d))
#define INPUT_BITS_PER_LONG         (sizeof(long) * INPUT_BITS_PER_BYTE)
#define INPUT_BIT(nr)			    (1UL << (nr))
#define INPUT_BIT_MASK(nr)		    (1UL << ((nr) % INPUT_BITS_PER_LONG))
#define INPUT_BIT_WORD(nr)		    ((nr) / INPUT_BITS_PER_LONG)
#define INPUT_BITS_TO_LONGS(nr)	    INPUT_DIV_ROUND_UP(nr, INPUT_BITS_PER_LONG)

/* logical device ioctl command */
#define INPUT_SET_REP_PERIOD        0x100

typedef struct __INPUT_PID
{
    __u16 product;
    __u16 version;
} __input_pid_t;

typedef struct __INPUT_DEV
{
    /* 以下4个成员为注册通用设备所用 */
    const char      *classname;
    const char      *name;
    __dev_devop_t   *ops;
    void            *parg;

    /* 指向通用设备 */
    __hdle          devnode;

    /* 在逻辑设备的物理设备子表数组中的index序号 */
    unsigned char   seq;

    /* 指向该物理设备对应的逻辑设备 */
    __hdle          ldevp;

    /* 设备类型，用于物理输入设备注册时匹配相应的逻辑输入设备 */
    __u32           device_class;

    /* 物理输入设备的身份信息 */
    __input_pid_t     id;

    /* 设备支持的事件bitmap，相应位为1表示支持该种事件 */
    unsigned long evbit[INPUT_BITS_TO_LONGS(EV_CNT)];
    /* 设备支持的按键bitmap，相应位为1表示支持该按键 */
    unsigned long keybit[INPUT_BITS_TO_LONGS(KEY_CNT)];
    /* 设备支持的相对位移类别bitmap，如x、y、z，相应位为1表示支持该种相对位移 */
    unsigned long relbit[INPUT_BITS_TO_LONGS(REL_CNT)];
    /* 设备支持的绝对位移类别bitmap，如x、y、volume，相应位为1表示支持该种绝对位移 */
    unsigned long absbit[INPUT_BITS_TO_LONGS(ABS_CNT)];
    /* 设备支持的led灯种类bitmap，如capslock灯、scroll灯，相应位为1表示支持该种灯 */
    unsigned long ledbit[INPUT_BITS_TO_LONGS(LED_CNT)];
    /* 设备支持的声音种类bitmap，如click、bell，相应位为1表示支持该种声音 */
    unsigned long sndbit[INPUT_BITS_TO_LONGS(SND_CNT)];
    /* 设备支持的力反馈种类bitmap，如手柄震动，相应位为1表示支持该种力反馈 */
    unsigned long ffbit[INPUT_BITS_TO_LONGS(FF_CNT)];

    /* 重复键码 */
    unsigned int    repeat_key;

    /* 重复按键timer */
    __krnl_stmr_t   *timer;

    /* 存放当前的绝对值，下一次将被拿来做降噪参考 */
    int abs[ABS_MAX + 1];

    /*  存放重复按键的各阶段时间间隔 */
    /*  rep[REP_DELAY]存放被忽略的第一次重复按键次数，默认为0 */
    int rep[REP_MAX + 1];

    /* key/led/snd状态bitmap，用来check对应code的值是否反转 */
    unsigned long key[INPUT_BITS_TO_LONGS(KEY_CNT)];
    unsigned long led[INPUT_BITS_TO_LONGS(LED_CNT)];
    unsigned long snd[INPUT_BITS_TO_LONGS(SND_CNT)];

    /* 用来校正绝对值的信息数组 */
    int absmax[ABS_MAX + 1];
    int absmin[ABS_MAX + 1];

    /* 用来降噪的参考值 */
    int absfuzz[ABS_MAX + 1];

    /* 物理事件处理函数 */
    int (*event)(struct __INPUT_DEV *dev, __u32 type, __u32 code, int value);

    /* 物理事件缓存区，收到EV_SYN后会转移到循环队列 */
    unsigned int evbuf[INPUT_EV_BUFFER_MAXLEN];
    /*  物理事件当前指针 */
    int ev_pos;

} __input_dev_t;


typedef struct __INPUT_EVENT
{
    __u32       type;           /*  事件类型，key/rel/abs...    */
    __u32       code;           /*  事件代码                    */
    __s32       value;          /*  事件值                      */
} __input_event_t;


typedef struct __INPUT_EVENT_PACKET
{
    void           *pGrabArg;               /*  注册回调函数时传入的私有句柄*/
    __u32           ldev_id;                /*  逻辑设备ID                  */
    __u32           pdev_id;                /*  物理设备ID                  */
    __s32           event_cnt;              /*  事件的个数                  */
    __input_event_t events[INPUT_MAX_EVENT_NR];/*  事件缓冲区               */
} __input_event_packet_t;


//************************************************
// 变量定义
/* GLOBAL VARIABLES */


//************************************************
// 函数定义
/* system call table */
typedef struct
{
    __pSWI_t esINPUT_RegDev      ;
    __pSWI_t esINPUT_UnregDev    ;
    __pSWI_t esINPUT_LdevGrab    ;
    __pSWI_t esINPUT_LdevRelease ;
    __pSWI_t esINPUT_SendEvent   ;
    __pSWI_t esINPUT_LdevFeedback;
    __pSWI_t esINPUT_GetLdevID   ;
    __pSWI_t esINPUT_LdevCtl     ;
} SWIHandler_INPUT_t;

#ifndef SIM_PC_WIN
#define SYSCALL_INPUT(x,y) __swi(SWINO(SWINO_INPUT, SWIHandler_INPUT_t, y))x y
#else
#define SYSCALL_INPUT(x,y) x y
#endif

SYSCALL_INPUT(__s32 ,  esINPUT_RegDev      )(__input_dev_t *dev);
SYSCALL_INPUT(__s32 ,  esINPUT_UnregDev    )(__input_dev_t *dev);
SYSCALL_INPUT(__hdle,  esINPUT_LdevGrab    )(char *ldev, __pCBK_t callback, void *pArg, __s32 aux);
SYSCALL_INPUT(__s32 ,  esINPUT_LdevRelease )(__hdle graber);
SYSCALL_INPUT(__s32 ,  esINPUT_SendEvent   )(__input_dev_t *dev, __u32 type, __u32 code, int value);
SYSCALL_INPUT(__s32 ,  esINPUT_LdevFeedback)(__hdle graber, __input_event_packet_t *packet);
SYSCALL_INPUT(__s32 ,  esINPUT_GetLdevID   )(__hdle graber);
SYSCALL_INPUT(__s32 ,  esINPUT_LdevCtl     )(__s32 LdevId, __s32 cmd, __s32 aux, void *pBuffer);

//************************************************
// 数据类型定义


//************************************************
// 变量定义
extern SWIHandler_INPUT_t SWIHandler_INPUT;

//************************************************
// 函数定义
__s32 INPUT_Init(void);
__s32 INPUT_Exit(void);

//**********************************************************************************************************

#endif  /* __SYS_INPUT_H__ */
