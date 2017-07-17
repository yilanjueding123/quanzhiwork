/*
*********************************************************************************************************
*											        eBase
*						                the Abstract of Hardware
*									           the OAL of DMA
*
*						        (c) Copyright 2006-2010, AW China
*											All	Rights Reserved
*
* File    	: 	csp_test.h
* Date		:	2010-07-20
* By      	: 	holigun
* Version 	: 	V1.00
*********************************************************************************************************
*/
#ifndef	__BSP_TEST_H__
#define	__BSP_TEST_H__

__s32 BSP_TEST_init(void);
__s32 BSP_TEST_exit(void);

__s32 BSP_TEST_get(void);
__s32 BSP_TEST_set(u32 value);

#endif	//__BSP_TEST_H__

