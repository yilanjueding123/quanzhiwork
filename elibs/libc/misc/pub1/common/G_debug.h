/*
************************************************************************************************************************
*                                                      G_debug
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : G_debug.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.07.16
*
* Description :
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2008.07.16       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __G_debug_h
#define  __G_debug_h


//#define  USE_MY_DEBUG


/*--------------------------------------- WINDOWS OS platform ------------------------------------*/
#if ( EPDK_OS == EPDK_OS_WINXP ) || ( EPDK_OS == EPDK_OS_WIN2K ) || ( EPDK_OS == EPDK_OS_VISTA )


#define EPDK_DEBUG_LEVEL_NOCHECK       0  /* No run time checks are performed */
#define EPDK_DEBUG_LEVEL_CHECK_ALL     1  /* Parameter checks and consistency checks are performed */
#define EPDK_DEBUG_LEVEL_LOG_ERRORS    2  /* Errors are recorded */
#define EPDK_DEBUG_LEVEL_LOG_WARNINGS  3  /* Errors & Warnings are recorded */
#define EPDK_DEBUG_LEVEL_LOG_ALL       4  /* Errors, Warnings and Messages are recorded. */



#if(EPDK_DEBUG_LEVEL      == EPDK_DEBUG_LEVEL_NOCHECK)

#elif(EPDK_DEBUG_LEVEL    == EPDK_DEBUG_LEVEL_CHECK_ALL)
#define __ASSERT
#elif(EPDK_DEBUG_LEVEL    == EPDK_DEBUG_LEVEL_LOG_ERRORS)
#define __ASSERT
#define __ERR
#elif(EPDK_DEBUG_LEVEL    == EPDK_DEBUG_LEVEL_LOG_WARNINGS)
#define __ERR
#define __ASSERT
#define __WRN
#elif(EPDK_DEBUG_LEVEL    == EPDK_DEBUG_LEVEL_LOG_ALL)
#define __ERR
#define __ASSERT
#define __WRN
#define __MSG
#else
#error EPDK_DEBUG_LEVEL must be defined!
#endif


#define __printf                    printf


#ifdef __ASSERT
#define __ast(condition)    {if(!condition)                                                     \
	                                __printf("AST:L%d(%s): condition err!", __LINE__, __FILE__);   }
#else
#define __ast(condition)
#endif

#if defined(__MSG)
#define __msg               __printf
#else
#define __msg(x)
#endif

#if defined(__ERR)
#define __err               __printf
#else
#define __err(x)
#endif

#if defined(__WRN)
#define __wrn               __printf
#else
#define __wrn(x)
#endif

#define __inf                   __printf

/*---------------------------------------  EPOS OS platform  ------------------------------------*/
#elif ( EPDK_OS == EPDK_OS_EPOS)
//	#include "epdk.h"


#ifdef USE_MY_DEBUG

#undef __msg
#undef __err
#undef __wrn
#undef __inf
#undef __ast

#define __printf                    eLIBs_printf


#ifdef __ASSERT
#define __ast(condition)    {if(!condition)                                                     \
	                                __printf("AST:L%d(%s): condition err!", __LINE__, __FILE__);   }
#else
#define __ast(condition)
#endif

#if defined(__MSG)
#define __msg               __printf
#else
#define __msg(...)
#endif

#if defined(__ERR)
#define __err               __printf
#else
#define __err(...)
#endif

#if defined(__WRN)
#define __wrn               __printf
#else
#define __wrn(...)
#endif

#define __inf                   __printf

#endif  // #ifdef USE_MY_DEBUG


/*------------------------------------------ platform end ----------------------------------------*/
#else
#error platform must be selected.
#endif






#endif     //  ifndef __G_debug_h

/* end of G_debug.h  */
