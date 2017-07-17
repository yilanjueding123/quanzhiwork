/*
************************************************************************************************************************
*                                                     REGISTER TABLE
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : reg_cfg.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.04.28
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
* Gary.Wang      2008.04.28       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __reg_cfg_h
#define  __reg_cfg_h

#include "elibs_i.h"

#if ( EPDK_OS == EPDK_OS_WINXP ) || ( EPDK_OS == EPDK_OS_WIN2K ) || ( EPDK_OS == EPDK_OS_VISTA )
#define DEFAULT_REG_ROOT_PATH       "D:\\winners\\ePDK\\workspace\\suni\\rootfs\\reg"
#else
#define DEFAULT_REG_ROOT_PATH       "Z:\\reg"
#define SOURCE_REG_ROOT_PATH        "D:\\reg"
#endif


#define BURN_FLAG_FILE_NAME             "magic.bin"


//#define ENABLE_THREAD

#define REFRESH_ALL_NODE_THREAD_PRIO    KRNL_priolevel5
#define BACKUP_ALL_NODE_THREAD_PRIO     KRNL_priolevel5

#define REFRESH_ALL_NODE_DELAY_TIME     1000
#define BACKUP_ALL_NODE_DELAY_TIME      2000


#include "..\\common\\type_def.h"
#include "..\\common\\G_debug.h"
#include "..\\common\\G_os_related.h"



#endif     //  ifndef __reg_cfg_h

/* end of reg_cfg.h  */
