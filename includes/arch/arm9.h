/*
******************************************************************************************************************
*                                                         eGON
*                                         the Embedded GO-ON Bootloader System
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : arm9.h
*
* Author : steven.zgj
*
* Version : 1.1.0
*
* Date : 2007.9.18
*
* Description £ºThis file provides system information for C language file. This file is very similar
*             to file "s3c2440.inc"; the two files should be modified at the same time to keep coherence
*             of system information.
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Wang Yugang     2007.9.18       1.1.0        build the file
*
*************************************************************************************************************************
*/

#ifndef  _ARM9_H_
#define  _ARM9_H_

#define ARM9_USR26_MODE		0x00
#define ARM9_FIQ26_MODE		0x01
#define ARM9_IRQ26_MODE		0x02
#define ARM9_SVC26_MODE		0x03
#define ARM9_USR_MODE		0x10
#define ARM9_FIQ_MODE		0x11
#define ARM9_IRQ_MODE		0x12
#define ARM9_SVC_MODE		0x13
#define ARM9_ABT_MODE		0x17
#define ARM9_UND_MODE		0x1b
#define ARM9_SYSTEM_MODE	0x1f
#define ARM9_MODE_MASK		0x1f
#define ARM9_FIQ_MASK		0x40
#define ARM9_IRQ_MASK		0x80
#define ARM9_CC_V_BIT		(1 << 28)
#define ARM9_CC_C_BIT		(1 << 29)
#define ARM9_CC_Z_BIT		(1 << 30)
#define ARM9_CC_N_BIT		(1 << 31)

#endif /* end of #ifndef _ARM9_H_ */

