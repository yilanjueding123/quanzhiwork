/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                           Matrix Key Driver Module
*
*                                    (c) Copyright 2006-2010, All winners Co,Ld.
*                                             All Rights Reserved
*
* File    : drv_matrix_key_i.h
* By      : james deng <csjamesdeng@gmail.com>
* Version : 1.0.0
* Date    : 2011-04-25
* Descript:
* Update  : date              author         ver       notes
*           2011-04-25        james deng     1.0.0     build the file.
*********************************************************************************************************
*/

#ifndef _DRV_MATRIX_KEY_I_H_
#define _DRV_MATRIX_KEY_I_H_

#include "epdk.h"
#include "bitops.h"


#if 0
#if (EPDK_ARCH == EPDK_ARCH_SUNI)
//#define  HOLDKEY_SUPPORT                //表示支持锁定键
//#define  HOLDKEY_SUPPORT_FOR_IR         //支持同时锁定遥控器
#define HOLDKEY_VALUE       7             //这个宏定义了哪个键值作为HOLD KEY，不要修改
#elif (EPDK_ARCH == EPDK_ARCH_SUNII)
#define HOLDKEY_SUPPORT                   //表示支持锁定键
//#define HOLDKEY_SUPPORT_FOR_IR          //支持同时锁定遥控器
#define HOLDKEY_VALUE       7             //这个宏定义了哪个键值作为HOLD KEY，不要修改
#else
#error "arch is invalid!"
#endif

/* key message */
#define DRV1_KEY_POWER         KMSG_USR_KEY_POWER
#define DRV1_KEY_SOURCE        KMSG_USR_KEY_SOURCE
#define DRV1_KEY_CLEAR         KMSG_USR_KEY_CLEAR
#define DRV1_KEY_DISPLAY       KMSG_USR_KEY_DISPLAY
#define DRV1_KEY_NUM0          KMSG_USR_KEY_NUM0
#define DRV1_KEY_NUM1          KMSG_USR_KEY_NUM1
#define DRV1_KEY_NUM2          KMSG_USR_KEY_NUM2
#define DRV1_KEY_NUM3          KMSG_USR_KEY_NUM3
#define DRV1_KEY_NUM4          KMSG_USR_KEY_NUM4
#define DRV1_KEY_NUM5          KMSG_USR_KEY_NUM5
#define DRV1_KEY_NUM6          KMSG_USR_KEY_NUM6
#define DRV1_KEY_NUM7          KMSG_USR_KEY_NUM7
#define DRV1_KEY_NUM8          KMSG_USR_KEY_NUM8
#define DRV1_KEY_NUM9          KMSG_USR_KEY_NUM9
#define DRV1_KEY_REPEATE       KMSG_USR_KEY_REPEATE
#define DRV1_KEY_BLOCK         KMSG_USR_KEY_BLOCK
#define DRV1_KEY_PLAY_PAUSE    KMSG_USR_KEY_PLAY_PAUSE
#define DRV1_KEY_TITLE         KMSG_USR_KEY_TITLE
#define DRV1_KEY_MENU          KMSG_USR_KEY_MENU
#define DRV1_KEY_ENTER         KMSG_USR_KEY_ENTER
#define DRV1_KEY_SETUP         KMSG_USR_KEY_SETUP
#define DRV1_KEY_GOTO          KMSG_USR_KEY_GOTO
#define DRV1_KEY_LEFT          KMSG_USR_KEY_LEFT
#define DRV1_KEY_RIGHT         KMSG_USR_KEY_RIGHT
#define DRV1_KEY_UP            KMSG_USR_KEY_UP
#define DRV1_KEY_DOWN          KMSG_USR_KEY_DOWN
#define DRV1_KEY_SEL           KMSG_USR_KEY_SEL
#define DRV1_KEY_SHIFT         KMSG_USR_KEY_SHIFT
#define DRV1_KEY_DISC          KMSG_USR_KEY_DISC
#define DRV1_KEY_ATT           KMSG_USR_KEY_ATT
#define DRV1_KEY_VOICE_UP      KMSG_USR_KEY_VOICE_UP
#define DRV1_KEY_VOICE_DOWN    KMSG_USR_KEY_VOICE_DOWN
#define DRV1_KEY_POWEROFF      KMSG_USR_KEY_POWEROFF
#define DRV1_KEY_RST           KMSG_USR_KEY_RST
#define DRV1_KEY_RISE          KMSG_USR_KEY_RISE
#endif

/* KEY Message */
typedef struct tag_key_msg
{
    __u32 key_value;  //key value.
    __u32 flag;       //1:keydown; 0:keyup.
} __key_msg_t;

typedef struct matrix_key_dev_t
{
    __u32              used;
} __matrix_key_dev_t;

typedef struct matrix_key_drv_t
{
    __u8               mid;
    __u8               used;
    __u8               status;
    __input_dev_t      *pKeyc_dev;
} __matrix_key_drv_t;

extern __matrix_key_drv_t matrix_key_drv;
extern __dev_devop_t matrix_key_dev_ops;

extern __s32 dev_matrix_key_ioctrl(__hdle hKey, __u32 cmd, __s32 aux, void *pbuffer);
extern __u32 get_single_value(void);
extern __s32 drv_keyc_event(__input_dev_t *dev, __u32 type, __u32 code, __s32 value);
extern __s32 matrix_key_init(void);
extern __s32 matrix_key_exit(void);
extern __u32 sinle_level_value;

extern __s32 drv_matrix_key_init(void);
extern __s32 drv_matrix_key_exit(void);
extern __mp *drv_matrix_key_open(__u32 mid, __u32 mode);
extern __s32 drv_matrix_key_close(__mp *mp);
extern __u32 drv_matrix_key_read(void *pdata, __u32 size, __u32 n, __mp *mp);
extern __u32 drv_matrix_key_write(const void *pdata, __u32 size, __u32 n, __mp *mp);
extern __s32 drv_matrix_key_ioctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);

#endif
