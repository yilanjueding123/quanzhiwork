/*
************************************************************************************************************************
*                                                     suni define
*                            suni CPU hardware registers, memory, interrupts, ... define
*
*                             Copyright(C), 2009-2010, uLive Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : suniv.h
*
* Author : kevin.z
*
* Version : 1.1.0
*
* Date : 2009-9-7 10:53
*
* Description : This file provides some defination of suni's hardware registers, memory, interrupt
*             and so on. This file is very similar to file "sunii.inc"; the two files should be
*             modified at the same time to keep coherence of information.
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* kevin.z      2009-9-7 10:53     1.0.0        build the file
* sunny        2011-1-25 19:05:08 1.0.0        modify for suniv(1619)
************************************************************************************************************************
*/

#ifndef  __SUNIV_H_
#define  __SUNIV_H_

/*
*********************************************************************************************************
*   hardware registers base define
*********************************************************************************************************
*/
#define REGS_pBASE		0x01C00000		//寄存器物理地址
#define REGS_pSIZE      0x00300000      //寄存器物理空间大小
#define DRAM_pBASE      0x80000000
#define SRAM_pBASE      0x00000000
#define SRAM_SIZE      (32 * 1024)

#ifdef  USE_PHYSICAL_ADDRESS
#define SRAM_BASE               SRAM_pBASE
#define DRAM_BASE               DRAM_pBASE
#else
#define SRAM_BASE               SRAM_vBASE
#define DRAM_BASE               DRAM_vBASE
#endif    // #ifdef  USE_PHYSICAL_ADDRESS

#endif // end of #ifndef __SUNIV_H_
