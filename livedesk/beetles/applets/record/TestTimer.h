/*
*********************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Development Kit
*                                           test timer tool
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                              All Rights Reserved
*
* File    : TestTimer.h
* By      : Sunny
* Version : V1.00
* Date    : 2010-5-20 22:11:06
*********************************************************************************************************
*/

#ifndef		__TEST_TIMER_H__
#define	   	__TEST_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

void TestTimerInit(void);
void TestTimerExit(void);

void TestTimerStart(void);
void TestTimerReport(int);

void TestCPUStatStart(void);
void TestCPUStatReport(void);

#ifdef __cplusplus
}
#endif

#endif		/* __TEST_TIMER_H__ */
