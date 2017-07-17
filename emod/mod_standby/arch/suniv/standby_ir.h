/*
*********************************************************************************************************
*                                                    ePOS
*                                   the Easy Portable/Player Operation System
*                                        architecture related source file
*
*                                      (c) Copyright 2009-2010, jerry China
*                                            All Rights Reserved
*
* File    : standby_ir.h
* By      : jerry
* Version : V1.00
*********************************************************************************************************
*/

#ifndef  _STANDBY_IR_H_
#define  _STANDBY_IR_H_

#include "epdk.h"
#include "standby_cfg.h"
#include "standby_reg.h"

typedef  struct  __IRW_RESERVE_STATUS
{
    __u32  ir_int;        //保存IR的中断控制器，退出的时候恢复
    __u32  ir_dma;        //保留IR用到的DMA控制器，退出的时候恢复

}
__irs_reserve_status_t;

extern __s32 standby_ir_init(__u8 *pSRAMBuffer, __u32 Len);
extern __s32 standby_ir_exit(void);
extern __s32 standby_ir_detect(void);
extern __s32 standby_ir_verify(void);

#endif  /*_STANDBY_IR_H_*/
