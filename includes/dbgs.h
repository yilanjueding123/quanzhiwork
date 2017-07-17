/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eDBG Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : debug define module
* File    : dbgs.h
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-21 9:06:55
**********************************************************************************************************************
*/

#ifndef _DBGS_H_
#define _DBGS_H_

#if 1
#define DebugPrintf(...)
#else
#define DebugPrintf(...) (eLIBs_printf("L%d(%s):", __LINE__, __FILE__),                 \
    						     eLIBs_printf(__VA_ARGS__)									        )
#endif

#ifndef EPDK_DEBUG_LEVEL_GATE
#define EPDK_DEBUG_LEVEL_GATE       0//0// 4//4//2
#endif

//#define BOOT_FROM_SDMMC


#define EPDK_DEBUG_LEVEL_NOCHECK       0            /* No run time checks are performed                             */
#define EPDK_DEBUG_LEVEL_CHECK_ALL     1            /* Parameter checks and consistency checks are performed        */
#define EPDK_DEBUG_LEVEL_LOG_ERRORS    2            /* Errors are recorded                                          */
#define EPDK_DEBUG_LEVEL_LOG_WARNINGS  3            /* Errors & Warnings are recorded                               */
#define EPDK_DEBUG_LEVEL_LOG_ALL       4            /* Errors, Warnings and Messages are recorded.                  */
#ifdef SIM_PC_WIN
#define EPDK_DEBUG_LEVEL EPDK_DEBUG_LEVEL_LOG_ALL
#endif
#ifndef EPDK_DEBUG_LEVEL
#error EPDK_DEBUG_LEVEL not defined!
#endif
//#define    EPDK_DEBUG_LEVEL   0//EPDK_DEBUG_LEVEL_NOCHECK
#if  (EPDK_DEBUG_LEVEL      == EPDK_DEBUG_LEVEL_NOCHECK)

#elif(EPDK_DEBUG_LEVEL    == EPDK_DEBUG_LEVEL_CHECK_ALL)
#if(EPDK_DEBUG_LEVEL_GATE == 0)

#elif(EPDK_DEBUG_LEVEL_GATE >= 1)
#define __ASSERT
#endif
#elif(EPDK_DEBUG_LEVEL    == EPDK_DEBUG_LEVEL_LOG_ERRORS)
#if(EPDK_DEBUG_LEVEL_GATE == 0)

#elif(EPDK_DEBUG_LEVEL_GATE == 1)
#define __ASSERT
#elif(EPDK_DEBUG_LEVEL_GATE >= 2)
#define __ERR
#define __ASSERT
#endif

#elif(EPDK_DEBUG_LEVEL    == EPDK_DEBUG_LEVEL_LOG_WARNINGS)
#if(EPDK_DEBUG_LEVEL_GATE == 0)

#elif(EPDK_DEBUG_LEVEL_GATE == 1)
#define __ASSERT
#elif(EPDK_DEBUG_LEVEL_GATE == 2)
#define __ERR
#define __ASSERT
#elif(EPDK_DEBUG_LEVEL_GATE >= 3)
#define __ERR
#define __ASSERT
#define __WRN
#endif

#elif(EPDK_DEBUG_LEVEL    == EPDK_DEBUG_LEVEL_LOG_ALL)
#if(EPDK_DEBUG_LEVEL_GATE == 0)

#elif(EPDK_DEBUG_LEVEL_GATE == 1)
#define __ERR
#define __ASSERT
#elif(EPDK_DEBUG_LEVEL_GATE == 2)
#define __ERR
#define __ASSERT
#elif(EPDK_DEBUG_LEVEL_GATE == 3)
#define __ERR
#define __ASSERT
#define __WRN
#elif(EPDK_DEBUG_LEVEL_GATE == 4)
#define __ERR
#define __ASSERT
#define __WRN
#define __MSG
#define __HERE
#define __INF
#endif
#else
#error EPDK_DEBUG_LEVEL is error, please config DEBUGLEVEL to 0/1/2/3/4!
#endif

#ifndef SIM_PC_WIN

#ifdef __ASSERT
#define __ast(condition)    (if(!condition)                                                     \
                                 eLIBs_printf("AST:L%d(%s): condition err!", __LINE__, __FILE__)   )
#else
#define __ast(condition)
#endif

#if defined(__MSG)
#define __msg(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
							     eLIBs_printf(__VA_ARGS__)									        )
#else
#define __msg(...)
#endif

#if defined(__ERR)
#define __err(...)          (eLIBs_printf("ERR:L%d(%s):", __LINE__, __FILE__),                 \
    							    eLIBs_printf(__VA_ARGS__),                                          \
    						        esKSRV_Reset())
#else
#define __err(...)    		    esKSRV_Reset()
#endif

#if defined(__WRN)
#define __wrn(...)          (eLIBs_printf("WRN:L%d(%s):", __LINE__, __FILE__),                 \
    						     eLIBs_printf(__VA_ARGS__)									        )
#else
#define __wrn(...)
#endif

#if defined(__HERE)
#define __here__            eLIBs_printf("@L%d(%s)\n", __LINE__, __FILE__);
#define __wait__            (eLIBs_printf("@L%d(%s)(press any key:\n", __LINE__, __FILE__),                 \
                                __getc())
#else
#define __here__
#define __wait__
#endif


#if defined(__INF)
#define __inf(...)           eLIBs_printf(__VA_ARGS__)
#else
#define __inf(...)
#endif

#define __log(...)       // eLIBs_printf(__VA_ARGS__)
#define __getc()            esSIOS_getchar()
#define __putc(c)           esSIOS_putarg(c, 'c')
#define __gets(s)          	esSIOS_gets(s)

#else //#ifndef SIM_PC_WIN
#include "sim_comm.h"
extern AW_SIM_API void PCMsgTrae(const char *szMsg, ...);

#ifdef __ASSERT
#define __ast(condition)    if(!condition)                                                     \
                                 PCMsgTrae("AST:L%d(%s): condition err!", __LINE__, __FILE__);
#else
#define __ast(condition)
#endif

#if defined(__MSG)
#define __msg    		PCMsgTrae("MSG:L%d(%s):", __LINE__, __FILE__);             \
							     PCMsgTrae
#else
#define __msg
#endif

#if defined(__ERR)
#define __err           PCMsgTrae("ERR:L%d(%s):", __LINE__, __FILE__);
//PCMsgTrae
#else
#define __err     		    esKSRV_Reset()
#endif

#if defined(__WRN)
#define __wrn           PCMsgTrae("WRN:L%d(%s):", __LINE__, __FILE__);                 \
    						     PCMsgTrae
#else
#define __wrn
#endif

#if defined(__HERE)
#define __here__            PCMsgTrae("@L%d(%s)\n", __LINE__, __FILE__);
#define __wait__            PCMsgTrae("@L%d(%s)(press any key:\n", __LINE__, __FILE__);
#else
#define __here__
#define __wait__
#endif


#if defined(__INF)
#define __inf            PCMsgTrae
#else
#define __inf
#endif

#define __log          PCMsgTrae
#define __getc()            esSIOS_getchar()
#define __putc(c)           esSIOS_putarg(c, 'c')
#define __gets(s)          	esSIOS_gets(s)

#endif // SIM_PC_WIN

#endif  /* _DBGS_H_ */

