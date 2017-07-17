;///**********************************************************************************************************************
;//*                                                     sun define
;//*                            SUN CPU hardware registers, memory, interrupts, ... define
;//*
;//*                             Copyright(C), 2009-2010, ulive
;//*											       All Rights Reserved
;//*
;//* File Name : suniv.inc
;//*
;//* Author : kevin.z
;//*
;//* Version : 1.1.0
;//*
;//* Date : 2009-9-7 11:08
;//*
;//* Description : This file provides some defination of suni hardware registers, memory, interrupt
;//*             and so on. This file is very similar to file "suniii.h"; the two files should be
;//*             modified at the same time to keep coherence of information.
;//*
;//* Others : None at present.
;//*
;//*
;//* History :
;//*
;//*  <Author>        <time>       <version>      <description>
;//*
;//* kevin.z       2009-9-7 11:09     1.1.0        build the file
;//* sunny         2011-1-25 19:06:29 1.1.0        modify for suniv(1619)
;//**********************************************************************************************************************/
	IF :LNOT::DEF:_SUNII_INC_
	GBLA  _SUNII_INC_


	INCLUDE    cfgs.inc


;///*
;//*********************************************************************************************************
;//*   hardware registers base define
;//*********************************************************************************************************
;//*/

REGS_pBASE		EQU    0x01C00000	   ;//寄存器物理地址
REGS_pSIZE      EQU    0x00300000      ;//寄存器物理空间大小
DRAM_pBASE      EQU    0x80000000
SRAM_pBASE      EQU    0x00000000
SRAM_SIZE       EQU    (32 * 1024)

	IF :DEF: USE_PHYSICAL_ADDRESS
SRAM_BASE   	EQU    SRAM_pBASE
DRAM_BASE   	EQU    DRAM_pBASE
	else
SRAM_BASE   	EQU    SRAM_vBASE
DRAM_BASE   	EQU    DRAM_vBASE
	ENDIF    ;// IF :DEF: USE_PHYSICAL_ADDRESS

	ENDIF
	END
