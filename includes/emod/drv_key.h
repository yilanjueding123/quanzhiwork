/*
*********************************************************************************************************
*											        eMOD
*						                    the Easy Module System
*                                               drv_key sub-system
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : drv_key.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/
#ifndef _DRV_KEY_H_
#define _DRV_KEY_H_

#include "typedef.h"
#include "cfgs.h"


typedef struct xy
{
    __u32 key0: 8;
    __u32 key1: 8;
    __u32 ctrkey0: 8;
    __u32 ctrkey1: 8;
} __xy_t;

#define DRV_KEYBOARD_PRO1   (0x80000000 | (0x01 << 27))          //Lowest
#define DRV_KEYBOARD_PRO2   (0x80000000 | (0x02 << 27))
#define DRV_KEYBOARD_PRO3   (0x80000000 | (0x03 << 27))
#define DRV_KEYBOARD_PRO4   (0x80000000 | (0x04 << 27))          //
#define DRV_KEYBOARD_PRO5   (0x80000000 | (0x05 << 27))          //Normal
#define DRV_KEYBOARD_PRO6   (0x80000000 | (0x06 << 27))
#define DRV_KEYBOARD_PRO7   (0x80000000 | (0x07 << 27))          //
#define DRV_KEYBOARD_PRO8   (0x80000000 | (0x08 << 27))          //
#define DRV_KEYBOARD_PRO9   (0x80000000 | (0x09 << 27))          //Hight

#define DRV_KEYBOARD_LRADC  DRV_KEYBOARD_PRO6
#define DRV_KEYBOARD_IR     DRV_KEYBOARD_PRO4

#define DRV_IRKEY_MAST1     128
#define DRV_IRKEY_MAST2     127

#define DRV_KEY_POWER       1
#define DRV_KEY_SOURCE      2
#define DRV_KEY_CLEAR       3
#define DRV_KEY_DISPLAY     4

#define DRV_KEY_NUM0        48
#define DRV_KEY_NUM1        49
#define DRV_KEY_NUM2        50
#define DRV_KEY_NUM3        51
#define DRV_KEY_NUM4        52
#define DRV_KEY_NUM5        53
#define DRV_KEY_NUM6        54
#define DRV_KEY_NUM7        55
#define DRV_KEY_NUM8        56
#define DRV_KEY_NUM9        57

#define DRV_KEY_REPEATE     60
#define DRV_KEY_BLOCK       0
#define DRV_KEY_PLAY_PAUSE  62
#define DRV_KEY_TITLE       63
#define DRV_KEY_MENU        64
#define DRV_KEY_ENTER       13
#define DRV_KEY_SETUP       65
#define DRV_KEY_GOTO        66

#define DRV_KEY_LEFT        16
#define DRV_KEY_RIGHT       18
#define DRV_KEY_UP          17
#define DRV_KEY_DOWN        19

#define DRV_KEY_SEL         67
#define DRV_KEY_SHIFT       68
#define DRV_KEY_DISC        69
#define DRV_KEY_ATT         70

#define DRV_KEY_VOICE_UP    117
#define DRV_KEY_VOICE_DOWN  100

#define DRV_KEY_POWEROFF    255
#define DRV_KEY_RST         254


#define DRV_KEY_KEYUP		0x0000ffff
#define DRV_KEY_KEY0_MASK	0x000000ff
#define DRV_KEY_KEY1_MASK	0x0000ff00
#define DRV_KEY_CKY0_MASK	0x00ff0000
#define DRV_KEY_CKY1_MASK	0xff000000

#define DRV_IRKEY_POWER_VALUE  0

typedef enum __DRV_KEY_CMD
{
    /* lradc key cmd */
    DRV_KEY_CMD_GETKEY       = 0,
    DRV_KEY_CMD_PUTKEY,
    DRV_KEY_CMD_SET_FIRST_DEBOUNCE_TIME,
    DRV_KEY_CMD_SET_FIRST_RPT_TIME,
    DRV_KEY_CMD_SET_SBSEQ_RPT_TIME,
    DRV_KEY_CMD_GET_FIRST_DEBOUNCE_TIME,
    DRV_KEY_CMD_GET_FIRST_RPT_TIME,
    DRV_KEY_CMD_GET_SBSEQ_RPT_TIME,
    DRV_KEY_CMD_GET_IRMASK,
    DRV_KEY_CMD_GET_IRPOWERVALUE,
    DRV_KEY_CMD_GET_HOLDKEYVALUEMAX,
    DRV_KEY_CMD_GET_HOLDKEYVALUEMIN,

    /*ir key cmd */
    DRV_IRKEY_CMD_PUTKEY,
    DRV_IRKEY_CMD_SET_SBSEQ_RPT_TIME,
    DRV_IRKEY_CMD_GET_SBSEQ_RPT_TIME,
    DRV_IRKEY_CMD_DISPLAY_SCANCODE,
    DRV_KEY_CMD_AUTO_SEARCH_SPI,
    DRV_KEY_CMD_AUTO_SEARCH_SPI_BEGIN,
    DRV_KEY_CMD_AUTO_SEARCH_SPI_FINSH,

} __drv_key_cmd_t;

#endif
