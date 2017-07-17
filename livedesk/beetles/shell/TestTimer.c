/*
*********************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Development Kit
*                                               test timer tool
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                              All Rights Reserved
*
* File    : TestTimer.c
* By      : Sunny
* Version : V1.00
* Date    : 2010-5-20 22:10:21
*********************************************************************************************************
*/

#include "epdk.h"
#include "TestTimer.h"

__hdle  hTestTimer  =   NULL;
__u32   StartTime   =   0;
__u32   FinishTime  =   0;

void TestTimerInit(void)
{
    /* request one counter */
#if(EPDK_ARCH == EPDK_ARCH_SUNI)
    hTestTimer = esTIME_RequestCnt(NULL);
#elif(EPDK_ARCH == EPDK_ARCH_SUNII)
    hTestTimer = esTIME_RequestCnt(NULL);
#elif(EPDK_ARCH == EPDK_ARCH_SUNIII)
    hTestTimer = esTIME_RequestCntr(0, "USRTEST");
#elif(EPDK_ARCH == EPDK_ARCH_SUNIV)
    hTestTimer = esTIME_RequestCntr(0, "USRTEST");
#else
#error platform can not support(EPDK_ARCH)!
#endif

    if (NULL == hTestTimer)
    {
        __wrn("XXX Request counter fail\n");
        return ;
    }

    /* set counter prescale : source counter preacale depend on platform */
#if (EPDK_ARCH == EPDK_ARCH_SUNI)
    esTIME_SetCntPrescale(hTestTimer, 135);
    esTIME_SetCntValue(hTestTimer, 0);
#elif(EPDK_ARCH == EPDK_ARCH_SUNII)
    esTIME_SetCntPrescale(hTestTimer, 24);
    esTIME_SetCntValue(hTestTimer, 0);
#elif(EPDK_ARCH == EPDK_ARCH_SUNIII)
    esTIME_SetCntrPrescale(hTestTimer, 24);
    esTIME_SetCntrValue(hTestTimer, 0);
#elif(EPDK_ARCH == EPDK_ARCH_SUNIV)
    esTIME_SetCntrPrescale(hTestTimer, 24);
    esTIME_SetCntrValue(hTestTimer, 0);
#else
#error platform can not support(EPDK_ARCH)!
#endif

    /* start count */
#if	(EPDK_ARCH == EPDK_ARCH_SUNI)
    esTIME_StartCnt(hTestTimer, 135);
#elif(EPDK_ARCH == EPDK_ARCH_SUNII)
    esTIME_StartCnt(hTestTimer, 24);
#elif(EPDK_ARCH == EPDK_ARCH_SUNIII)
    esTIME_StartCntr(hTestTimer);
#elif(EPDK_ARCH == EPDK_ARCH_SUNIV)
    esTIME_StartCntr(hTestTimer);
#else
#error "platform can not support(EPDK_ARCH)!"
#endif

    StartTime   = 0;
    FinishTime  = 0;
}

void TestTimerExit(void)
{
    /* release timer counter */
    if (hTestTimer)
    {
#if	(EPDK_ARCH == EPDK_ARCH_SUNI)
        esTIME_ReleaseCnt(hTestTimer);
#elif(EPDK_ARCH == EPDK_ARCH_SUNII)
        esTIME_ReleaseCnt(hTestTimer);
#elif(EPDK_ARCH == EPDK_ARCH_SUNIII)
        esTIME_ReleaseCntr(hTestTimer);
#elif(EPDK_ARCH == EPDK_ARCH_SUNIV)
        esTIME_ReleaseCntr(hTestTimer);
#else
#error "platform can not support(EPDK_ARCH)!"
#endif
    }
}

void TestTimerStart(void)
{
    if (hTestTimer)
    {
#if	(EPDK_ARCH == EPDK_ARCH_SUNI)
        StartTime = esTIME_QuerryCnt(hTestTimer) << 1;
#elif(EPDK_ARCH == EPDK_ARCH_SUNII)
        StartTime = esTIME_QuerryCnt(hTestTimer) << 1;
#elif(EPDK_ARCH == EPDK_ARCH_SUNIII)
        StartTime = esTIME_QuerryCntr(hTestTimer) << 1;
#elif(EPDK_ARCH == EPDK_ARCH_SUNIV)
        StartTime = esTIME_QuerryCntr(hTestTimer) << 1;
#else
#error "platform can not support(EPDK_ARCH)!"
#endif
    }
}

void TestTimerReport(int num)
{
    __u32 ElapsedTime;

    if (hTestTimer)
    {
#if(EPDK_ARCH == EPDK_ARCH_SUNI)
        FinishTime = esTIME_QuerryCnt(hTestTimer) << 1;
#elif(EPDK_ARCH == EPDK_ARCH_SUNII)
        FinishTime = esTIME_QuerryCnt(hTestTimer) << 1;
#elif(EPDK_ARCH == EPDK_ARCH_SUNIII)
        FinishTime = esTIME_QuerryCntr(hTestTimer) << 1;
#elif(EPDK_ARCH == EPDK_ARCH_SUNIV)
        FinishTime = esTIME_QuerryCntr(hTestTimer) << 1;
#else
#error "platform can not support(EPDK_ARCH)!"
#endif
    }
    else
    {
        FinishTime = 0;
    }

    ElapsedTime = FinishTime - StartTime;
    eLIBs_printf(" line num is %d. elapsed time [%d.", num, ElapsedTime / 1000000);
    {
        __u32 j;
        __u32 precision = ElapsedTime % 1000000;
        for(j = 0; j < 6; j++)
        {
            precision *= 10;
            eLIBs_printf("%d", precision / 1000000);
            precision %= 1000000;
        }
        eLIBs_printf("]\n");
    }
}

void TestCPUStatStart(void)
{
    esKRNL_CPUStatStart();
}

void TestCPUStatReport(void)
{
    esKRNL_CPUStatReport();

}
