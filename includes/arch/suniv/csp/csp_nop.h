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
#ifndef	__CSP_TEST_H__
#define	__CSP_TEST_H__

__s32 CSP_TEST_init(void);
__s32 CSP_TEST_exit(void);

__u32 CSP_TEST_get(void);
void CSP_TEST_set(__u32 value);

#endif	//__CSP_TEST_H__

