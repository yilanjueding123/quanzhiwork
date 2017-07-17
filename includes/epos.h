/*
*********************************************************************************************************
*											        ePOS
*						           the Easy Portable/Player Operation System
*									           epos headfile
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : epos.h
* By      : Steven.ZGJ
* Version : V0.1
*
* note：所有的模块内部定义的全局性的常量，都需要添加"模块名_xxx", 比如"MEMS_xxx", "EXEC_xxx"
************************************************************************************************************************
*/

#ifndef EPOS_H
#define EPOS_H

#include "typedef.h"

#ifdef 	EPOS_GLOBALS
#define EPOS_EXT
#else
#define EPOS_EXT  extern
#endif

/* ePOS VERSION NUMBER */
#define 	EPOSVer	0x01000000				//1.00.0000

/*
**************************
* INCLUDE HEADER FILES
**************************
*/
//level0
#include    "typedef.h"
#include    "cfgs.h"
#include    "configs.h"
#include    "dbgs.h"
//level1
#include    "elibs\\libc.h"
#include    "ebios.h"

//level2
#include    "epos\\epos_para.h"
#include    "epos\\irqs.h"
#include    "epos\\mems.h"
#include    "epos\\krnl.h"
#include    "epos\\ksrv.h"
#include    "epos\\eexf.h"
#include    "epos\\exec.h"
#include    "epos\\mods.h"
#include    "epos\\resm.h"
#include    "epos\\errs.h"

#define EPOS_MODULE_DEBUG
/*
*****************
*   空间分配相关的全局定义
*****************
*/
#define EPOS_proc_segsize	(32 * 1024 * 1024)		// process 内存中地址段大小
#define EPOS_umod_segsize	(32 * 1024 * 1024)		// user module 内存中地址段大小
#define EPOS_smod_segsize	(1024 * 1024)			// sys module 内存中地址段大小

#define EPOS_pid_min        1                       // 最小pid
#define EPOS_pid_max        63                      // 最多可以有64个进程(pid = 0 ~ 63)占用2G的空间(32M*64)
#define EPOS_umid_min       64                      //
#define EPOS_umid_max       95                      // 最多可以有32个用户模块(mid = 64 ~ 95)占用1G的空间(32M*32))
#define EPOS_smid_min       96                      // 最多可以有128个系统模块(mid = 96 ~ 223)
#define EPOS_smid_max       0xfc                    // 最多可以有127个系统模块(mid = 96 ~ 223)

#define EPOS_rsvid          0xfd                    // 系统保留id
#define EPOS_sysid          0xfe                    // 系统内核id
#define EPOS_curid          0xff                    // 当前id

#define EPOS_id_mumber      255
#define EPOS_umod_startaddr 0x80000000
#define EPOS_smod_startaddr 0xe0000000



extern __epos_para_t start_para;
extern __u8          DebugEnable;

EPOS_EXT __krnl_xcb_t   *esXCBTbl[EPOS_id_mumber];      // pcb/mcb地址表, 记录当前打开的pcb/mcb
#define  esPCBTbl   	((__exec_pcb_t **)esXCBTbl)
#define  esMCBTbl   	((__mods_mcb_t **)esXCBTbl)
#define  esIDCur    	(((__krnl_tcb_t *)OSTCBCur)->xcb->id)

EPOS_EXT __krnl_event_t *esEposSrvQ;

#ifdef EPDK_USED_BY_KRNL
void epossrv(void *p_arg);
#endif

#endif						//EPOS_H

