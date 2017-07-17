/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, kevin.z China
*                                             All Rights Reserved
*
* Moudle  : system power manager
* File    : sys_pwm.h
* By      : kevin
* Version : v1.0
* Date    : 2009-5-29 15:42
**********************************************************************************************************************
*/
#ifndef _SYS_PWM_H_
#define _SYS_PWM_H_

#define PWM_PARA_REVERSED_NUM 2


typedef enum __SYS_PWM_MODE
{
    SYS_PWM_MODE_LEVEL0 = 0x00, /* level0 power mode (full speed)       */
    SYS_PWM_MODE_LEVEL1 = 0x01, /* level1 power mode (high speed)       */
    SYS_PWM_MODE_LEVEL2 = 0x02, /* level2 power mode (normal speed)     */
    SYS_PWM_MODE_LEVEL3 = 0x03, /* level3 power mode (low speed)        */

    SYS_PWM_MODE_USBD   = 0x20, /* usb device mode                      */
    SYS_PWM_MODE_USBH   = 0x21, /* usb host mode                        */

    SYS_PWM_MODE_

} __sys_pwm_mode_t;


typedef enum __SYS_PWM_DEV
{
    PWM_DEV_NONE = 0,

    //level1 pwm devices
    PWM_DEV_TP   = 0x100,
    PWM_DEV_IR,
    PWM_DEV_KEY,
    PWM_DEV_FM,
    PWM_DEV_POWER,
    PWM_DEV_HDMI,
    PWM_DEV_RTC,
    PWM_DEV_CSI,

    //level2 pwm devices
    PWM_DEV_MONITOR = 0x200,
    PWM_DEV_USBD,
    PWM_DEV_USBH,
    PWM_DEV_AUDIO,
    PWM_DEV_DISPLAY,
    PWM_DEV_MP,
    PWM_DEV_NAND,
    PWM_DEV_MS,
    PWM_DEV_SDMMC,

    //level3 pwm devices
    PWM_DEV_TWI = 0x300,
    PWM_DEV_SPI,
    PWM_DEV_UART,

    PWM_DEV_

} __sys_pwm_dev_e;


typedef enum __SYS_PWM_CMD
{
    PWM_CMD_ENTER_NONE = 0,
    PWM_CMD_ENTER_STANDBY,  //request device to enter standby
    PWM_CMD_EXIT_STANDBY,   //wake up device
    PWM_CMD_ENTER_

} __sys_pwm_cmd_e;


#define STANDBY_EVENT_POWEROFF      (1<<0)
#define STANDBY_EVENT_LOWPOWER      (1<<1)

typedef struct  __BOARD_POWER_CFG
{
    __u32    keyvalue               : 8;            //表示power按键短按后发出的按键消息
    __u32    twi_controller         : 4;            //表示power所使用的twi控制器编号
    __u32    power_exist            : 4;            //表示power是否存在, 0:不存在，非0：存在
    __u32    reserved               : 16;           //保留中
}
__board_power_cfg_t;


typedef struct __SYS_PWM_PARA
{
    __s32       PowerOffTime;       //timer value when power off
    __u32       CurVddVal;          //current vdd value

    __u32       IrMask;             //ir code mask
    __u32       IrPowerVal;         //value of the power key of ir

    __u8        HKeyMin;            //min value of the hold-key
    __u8        HKeyMax;            //max value of the hold-key
    __u8        WakeUpMode;         //mode of event wake up the system
    __u8        EventFlag;          //event flag that need system process when exit standby
    __u8        TvdFlag;            //flag to mark that if current board is tvd board

    __board_power_cfg_t PowerCfg;   //power configuration
    __dram_para_t   	dram_para;  // dram para
    __u8                IrBuffer[128]; //ir data buffer
    __u32		eint_no;	 /* GPIO 口外部中断号*/
    __u32		pio_int_trigger; /* trigger condition*/
    __hdle		gpio_hdl;
    __u32               port;
    __u32       standby_cnt;        //记录进入standby的时间
    __u32       standby_mode;       //
} __sys_pwm_para_t;


//************************************************
// 函数定义
/* system call table */
typedef struct
{
    __pSWI_t esPWM_RequestPwmMode   ;
    __pSWI_t esPWM_ReleasePwmMode   ;
    __pSWI_t esPWM_EnterStandby     ;
    __pSWI_t esPWM_UsrEventNotify   ;
    __pSWI_t esPWM_LockCpuFreq      ;
    __pSWI_t esPWM_UnlockCpuFreq    ;
    __pSWI_t esPWM_RegDevice        ;
    __pSWI_t esPWM_UnregDevice      ;
    __pSWI_t esPWM_GetStandbyPara   ;
    __pSWI_t esPWM_SetStandbyMode   ;

} SWIHandler_PWM_t;

#ifndef SIM_PC_WIN
#define SYSCALL_PWM(x,y) __swi(SWINO(SWINO_PWM, SWIHandler_PWM_t, y))x y
#else
#define SYSCALL_PWM(x,y) x y
#endif

SYSCALL_PWM(__s32,  esPWM_RequestPwmMode)(__s32 /*mode*/);
SYSCALL_PWM(__s32,  esPWM_ReleasePwmMode)(__s32 /*mode*/);
SYSCALL_PWM(void,   esPWM_EnterStandby)(__u32   /*time*/);
SYSCALL_PWM(void,   esPWM_UsrEventNotify)(void);
SYSCALL_PWM(__s32,  esPWM_LockCpuFreq)(void);
SYSCALL_PWM(__s32,  esPWM_UnlockCpuFreq)(void);
SYSCALL_PWM(__s32,  esPWM_RegDevice)(__sys_pwm_dev_e device, __pCB_DPMCtl_t cb, void *parg);
SYSCALL_PWM(__s32,  esPWM_UnregDevice)(__sys_pwm_dev_e device, __pCB_DPMCtl_t cb);
SYSCALL_PWM(__s32,  esPWM_GetStandbyPara)(__sys_pwm_para_t      *pStandbyPara);
SYSCALL_PWM(__s32,  esPWM_SetStandbyMode)(__u32 standby_mode);






//************************************************
// 变量定义
extern SWIHandler_PWM_t SWIHandler_PWM;
//************************************************
// 函数定义
extern __s32 PWM_Init(__dram_para_t *para);
extern __s32 PWM_Exit(void);
extern __s32 PWM_Start(void);
extern __s32 PWM_Stop(void);

// 定义standby的代码空间
#define STANDBY_ENTRY       0xffff8000
#define STANDBY_SIZE        0x3000

extern __sys_pwm_para_t     PwmStandbyPar;
typedef void (*standby_entry_t)(__sys_pwm_para_t *pVar);

#endif //#ifndef _SYS_PWM_H_

