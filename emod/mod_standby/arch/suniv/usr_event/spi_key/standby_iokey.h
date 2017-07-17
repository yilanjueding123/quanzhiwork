/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Standby Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : standby_iokey.h
* By      : Alex.Leaf
* Version : v1.0
* Date    : 2010-7-8 19:19
* Descript:
* Update  : date                auther      ver     notes
*********************************************************************************************************
*/
#ifndef _STANDBY_IOKEY_H_
#define _STANDBY_IOKEY_H_

#include "standby_cfg.h"



#define Device0     0x00
#define Device1     0x40

#define	PA_Buf		0x00
#define	PB_Buf		0x01
#define	PC_Buf		0x02

#define	PA_Dir		0x04
#define	PB_Dir		0x05
#define	PC_Dir		0x06

#define	PA_Att		0x08
#define	PB_Att		0x09
#define	PC_Att		0x0A

#define PA_Data		0x0C
#define PB_Data		0x0D
#define PC_Data		0x0E

#define	P_Status	0x0F
#define	P_Ctrl  	0x0F


// GPBA Definition (AND IO)
#define	PA_AndBuf	0x10
#define	PB_AndBuf	0x11
#define	PC_AndBuf	0x12

#define	PA_AndDir	0x14
#define	PB_AndDir	0x15
#define	PC_AndDir	0x16

#define	PA_AndAtt	0x18
#define	PB_AndAtt	0x19
#define	PC_AndAtt	0x1A


// GPBA Definition (OR IO)
#define	PA_OrBuf	0x20
#define	PB_OrBuf	0x21
#define	PC_OrBuf	0x22

#define	PA_OrDir	0x24
#define	PB_OrDir	0x25
#define	PC_OrDir	0x26

#define	PA_OrAtt	0x28
#define	PB_OrAtt	0x29
#define	PC_OrAtt	0x2A


// GPBA Definition (XOR IO)
#define	PA_XorBuf	0x30
#define	PB_XorBuf	0x31
#define	PC_XorBuf	0x32

#define	PA_XorDir	0x34
#define	PB_XorDir	0x35
#define	PC_XorDir	0x36

#define	PA_XorAtt	0x38
#define	PB_XorAtt	0x39
#define	PC_XorAtt	0x3A


//====================================================================================
// Constants definition
//====================================================================================
// Define for Status
#define B_WakeUpEN	    0x80
#define	B_WakeUpPAEN    0x40
#define	B_WakeUpPBEN    0x20
#define	B_WakeUpPCEN    0x10
#define	B_WakeUpPAFlag  0x08
#define	B_WakeUpPBFlag  0x04
#define	B_WakeUpPCFlag  0x02
#define	B_WakeUpFlag    0x01


#define INPUT_KEY_PORT      8   //8个输入口
#define OUTPUT_KEY_PORT     16  //16个输出口



extern void IOKEY_StandbyInit(__u32 clk);
extern void IOKEY_StandbyExit(void);
extern __s32 IOKEY_Wakeup(void);

#endif  //_STANDBY_IOKEY_H_

