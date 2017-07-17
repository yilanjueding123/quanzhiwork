/*
*********************************************************************************************************
*                                                     eBase
*                                           the Abstract of Hardware
*
*                                   (c) Copyright 2006-2010, holigun China
*                                               All Rights Reserved
*
* File        :  bsp_tp.h
* Date        :  2011-3-2
* Author      :  Victor
* Version     :  v1.00
* Description:
*                                   Operation for TP module,aw1619
* History     :
*      <author>          <time>             <version >          <desc>
*       Victor         2011-3-2                1.0           create this file
*********************************************************************************************************
*/
#ifndef _BSP_TP_H_
#define _BSP_TP_H_


/* interrupt control */
#define  TP_DATA_IRQ           (0x01 << 16)// (0x01 << 5)
#define  TP_LTOUCH_IRQ         (0x01 << 1)
#define  TP_FTOUCH_IRQ         (0x01 << 0)
#define  TP_FIFO_OV_IRQ        (0x01 <<17)//(0x01 << 6)
#define  TP_FIFO_DATA_DRQ      (0x01 <<7)

/* interrupt status */
#define TP_FTOUCH_PENDING      (0x1 << 0)
#define TP_LTOUCH_PENDING      (0x1 << 1)
#define TP_DATA_PENDING        (0x1 << 16)
#define TP_FIFO_OV_PENDING     (0x1 << 17)

/* typical fifo level trigger value, maxlen is 32. */
#define TP_FIFO_LEVEL08   (8)  /* single mode    */
#define TP_FIFO_LEVEL16   (16) /* dual mode       */
#define TP_FIFO_LEVEL20   (20) /* pressure mode  */
#define TP_FIFO_LEVEL24   (24) /* debug mode     */



/* tp work mode select,three work modes */
typedef enum
{
    TP_XXXXXX_POINT_MODE = 0,
    TP_SINGLE_POINT_MODE    , /* x,y,x,y,... */
    TP_DUAL_POINT_MODE      , /* x,y,dx,dy,x,y,dx,dy,... */
    TP_PRESSURE_MODE         , /* x,y,dx,dy,dz,x,y,dx,dy,dz,... */
    TP_DEBUG_MODE           , /* x,y,dx,dy,z1,z2 */
}
__tp_work_mode_t;



/* sample rate ,frequency */
typedef enum
{
    TP_SAMP_LEVEL0 = 0,/* 采样频率为500hz  */
    TP_SAMP_LEVEL1    ,/* 采样频率为1khz  */
    TP_SAMP_LEVEL2    ,/* 采样频率为2khz  */
    TP_SAMP_LEVEL3    ,/* 采样频率为4khz  */
    TP_SAMP_LEVEL4    ,/* 采样频率为8khz */
    TP_SAMP_LEVEL5    ,/* 保留 */
    TP_SAMP_LEVEL6    ,/* 保留 */
    TP_SAMP_LEVEL7	  ,
    TP_SAMP_LEVEL8	  ,
    TP_SAMP_LEVEL9	  ,
} __tp_sample_rate_t;


/* ajust sensitivity of pen down detection,total 16 level */
typedef enum
{
    TP_SENSTIVITY_L1 = 0,
    TP_SENSTIVITY_L2    ,
    TP_SENSTIVITY_L3    ,
    TP_SENSTIVITY_L4    ,
    TP_SENSTIVITY_L5    ,
    TP_SENSTIVITY_L6    ,
    TP_SENSTIVITY_L7    ,
    TP_SENSTIVITY_L8    ,
    TP_SENSTIVITY_L9    ,
    TP_SENSTIVITY_L10    , /* 0xA */
    TP_SENSTIVITY_L11    , /* 0xB */
    TP_SENSTIVITY_L12    , /* 0xC */
    TP_SENSTIVITY_L13    , /* 0xD */
    TP_SENSTIVITY_L14    , /* 0xE */
    TP_SENSTIVITY_L15    , /* 0xF */

} __tp_senstivity_t;



extern __s32 BSP_TP_Init(__u32 freq, __u32 fifo_depth, __u32 sensitivity);
extern __s32 BSP_TP_Exit(void);
extern __s32 BSP_TP_SetWorkMode(__u32 work_mode);
extern __s32 BSP_TP_SetPressThreshold(__u32 pressure, __u32 p_current);
extern __s32 BSP_TP_SetSampleRate(__u32 level);
extern __s32 BSP_TP_GetData(__s16 *dataX, __s16 *dataY, __u32 data_count);
extern void  BSP_TP_ClearFifo(void);
extern __s32 BSP_TP_EnableInt(__u32 int_no);
extern __s32 BSP_TP_DisableInt(__u32 int_no);
extern __u32 BSP_TP_QueryIntPending(void);
extern __s32 BSP_TP_ClearIntPending(__u32 pending);


#endif
