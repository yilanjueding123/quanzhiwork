/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           ir key driver
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : bsp_rk.h
* By      :
* Version : V1.00
*********************************************************************************************************
*/



#ifndef __BSP_RK_H__
#define __BSP_RK_H__

#include "epdk.h"

/* interrupt control */
#define RK_DATA_IRQ           (1 << 16)
#define RK_LTOUCH_IRQ         (1 << 0)
#define RK_FTOUCH_IRQ         (1 << 1)
#define RK_FIFO_OV_IRQ        (1 << 17)


/* interrupt status */
#define RK_FTOUCH_PENDING      (0x1 << 0)
#define RK_LTOUCH_PENDING      (0x1 << 1)
#define RK_DATA_PENDING        (0x1 << 16)
#define RK_FIFO_OV_PENDING     (0x1 << 17)

#ifndef __REG
#define __REG(x)    (*(volatile unsigned int *)(x))
#endif

#define  RK_READ_REG(Nb)              __REG(RKC_REGS_BASE + (Nb))
#define  RK_WRITE_REG(Nb, value)      (__REG(RKC_REGS_BASE + (Nb)) = (value))

#define  RK_AD_READ_REG(Nb)              __REG(RKC_AD_REGS_BASE + (Nb))
#define  RK_AD_WRITE_REG(Nb, value)      (__REG(RKC_AD_REGS_BASE + (Nb)) = (value))


typedef enum
{
    RK_FIFO_DEPTH_01,
    RK_FIFO_DEPTH_04,
    RK_FIFO_DEPTH_08,
    RK_FIFO_DEPTH_16,
    RK_FIFO_DEPTH_32,
} __rk_fifo_depth_e;

typedef enum
{
    RK_SAMPLE_RATE_01K,
    RK_SAMPLE_RATE_02K,
    RK_SAMPLE_RATE_04K,
    RK_SAMPLE_RATE_08K,
    RK_SAMPLE_RATE_16K,
} __rk_sample_rate_e;

typedef enum
{
    RK_CHANNEL_X1,
    RK_CHANNEL_X2,
    RK_CHANNEL_Y1,
    RK_CHANNEL_Y2,
    RK_CHANNEL_RD,
} __rk_channel_e;

__s32   RK_Init(void);
__s32   RK_Exit(void);

void    RK_ClearFifo(void);

__s32   RK_EnableInt(__u32 int_no);

__s32   RK_DisableInt(__u32 int_no);

__s32   RK_GetData(__s32 *data, __u32 data_cnt);

__s32   RK_QueryIntPending(void);

void    RK_ClearIntPending(__s32 pending);

__s32   RK_SetSampleRate(__rk_sample_rate_e rate);

__s32   RK_SetFifoDepth(__rk_fifo_depth_e depth);

__s32   RK_SelectChannel(__rk_channel_e channel);

__s32  standby_get_adc_valume(void);


#endif /* #ifndef __BSP_RK_H__ */

