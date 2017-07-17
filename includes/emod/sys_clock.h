/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                             Clock Manager Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : sys_clock.h
* By      : kevin.z
* Version : v2.0
* Date    : 2010-11-26 17:41
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-26 17:42    kevin.z     2.0     build the file
*********************************************************************************************************
*/
#ifndef __SYS_CLOCK_H__
#define __SYS_CLOCK_H__


//command for call-back function of clock change
typedef enum __CLK_CMD
{
    CLK_CMD_SCLKCHG_REQ = 0,    //command for notify that clock will change
    CLK_CMD_SCLKCHG_DONE,       //command for notify that clock change finish
    CLK_CMD_

} __clk_cmd_t;


//command or status of clock on/off
typedef enum __CLK_ONOFF
{
    CLK_OFF = 0,                //clock off
    CLK_ON = 1,                 //clock on
} __CLK_onoff_t;


//status of clock, busy or free
typedef enum __CLK_FREE
{
    CLK_FREE = 0,               //clock is not used
    CLK_USED = 1,               //clock is used by someone
} __clk_free_t;

//mode of clock
typedef enum __CLK_MODE
{
    CLK_MODE_0 = 0,               //clock mode 0
    CLK_MODE_1 = 1,               //clock mode 1
    CLK_MODE_2 = 2,               //clock mode 2
    CLK_MODE_3 = 3,               //clock mode 3

} __clk_mode_t;


typedef struct __WAKEUP_SYS_SOURCE
{
    __u8    touch_panel;
    __u8    ext_nmi;
    __u8    keyboard;
    __u8    usb;
    __u8    alarm;
    __u8    lradc;

} __wakeup_sys_source;



typedef struct
{
    __pSWI_t esCLK_SetSrcFreq   ;
    __pSWI_t esCLK_GetSrcFreq   ;
    __pSWI_t esCLK_OpenMclk     ;
    __pSWI_t esCLK_CloseMclk    ;
    __pSWI_t esCLK_MclkRegCb    ;
    __pSWI_t esCLK_MclkUnregCb  ;
    __pSWI_t esCLK_SetMclkSrc   ;
    __pSWI_t esCLK_GetMclkSrc   ;
    __pSWI_t esCLK_SetMclkDiv   ;
    __pSWI_t esCLK_GetMclkDiv   ;
    __pSWI_t esCLK_MclkOnOff    ;
    __pSWI_t esCLK_MclkReset    ;
    __pSWI_t esCLK_Set_pll_video_pattern_ctl;
    __pSWI_t esCLK_Set_pll_audio_pattern_ctl;
    __pSWI_t esCLK_Set_pll_sdram_pattern_ctl;

} SWIHandler_CLK_t;


#ifndef SIM_PC_WIN
#define SYSCALL_CLK(x,y) __swi(SWINO(SWINO_CLK, SWIHandler_CLK_t, y))x y
#else
#define SYSCALL_CLK(x,y) x y
#endif


SYSCALL_CLK(__s32,  esCLK_SetSrcFreq  )(__u32 nSclkNo, __u32 nFreq);
SYSCALL_CLK(__u32,  esCLK_GetSrcFreq  )(__u32 nSclkNo);
SYSCALL_CLK(__hdle, esCLK_OpenMclk    )(__u32 nMclkNo);
SYSCALL_CLK(__s32,  esCLK_CloseMclk   )(__hdle hMclk);
SYSCALL_CLK(__s32,  esCLK_MclkRegCb   )(__u32 nMclkNo, __pCB_ClkCtl_t pCb);
SYSCALL_CLK(__s32,  esCLK_MclkUnregCb )(__u32 nMclkNo, __pCB_ClkCtl_t pCb);
SYSCALL_CLK(__s32,  esCLK_SetMclkSrc  )(__hdle hMclk, __s32 nSclkNo);
SYSCALL_CLK(__s32,  esCLK_GetMclkSrc  )(__hdle hMclk);
SYSCALL_CLK(__s32,  esCLK_SetMclkDiv  )(__hdle hMclk, __s32 nDiv);
SYSCALL_CLK(__s32,  esCLK_GetMclkDiv  )(__hdle hMclk);
SYSCALL_CLK(__s32,  esCLK_MclkOnOff   )(__hdle hMclk, __s32 bOnOff);
SYSCALL_CLK(__s32,  esCLK_MclkReset   )(__hdle hMclk, __s32 bReset);
SYSCALL_CLK(__s32,  esCLK_Set_pll_video_pattern_ctl  )(__clk_mode_t mode, __CLK_onoff_t onoff);
SYSCALL_CLK(__s32,  esCLK_Set_pll_audio_pattern_ctl  )(__clk_mode_t mode, __CLK_onoff_t onoff);
SYSCALL_CLK(__s32,  esCLK_Set_pll_sdram_pattern_ctl  )(__clk_mode_t mode, __CLK_onoff_t onoff);

//==============================================================
// define interface for kernel
//==============================================================
extern SWIHandler_CLK_t SWIHandler_CLK;
__s32 CLK_Init(void);
__s32 CLK_Exit(void);
void CLK_EnterStandby(void);

#endif //#ifndef __SYS_CLOCK_H__
