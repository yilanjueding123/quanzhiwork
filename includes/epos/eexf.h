/*
*********************************************************************************************************
*													ePDK
*								   the Easy	Portable/Player	Development	Kits
*											   exf sub-system
*
*								(c)	Copyright 2006-2007, Steven.ZGJ	China
*											All	Rights Reserved
*
* File	 : eexf.h
* Version: V1.0
* By	 : steven.zgj
*********************************************************************************************************
*/

#ifndef	EEXF_H
#define	EEXF_H

#include "typedef.h"
#include "cfgs.h"

#include "krnl.h"
#include "mems.h"

#define		EEXF_TYPE_NORMAL	 0

#define		EEXF_MAGIC0		'e'
#define		EEXF_MAGIC1		'P'
#define		EEXF_MAGIC2		'D'
#define		EEXF_MAGIC3		'K'
#define		EEXF_MAGIC4		'.'
#define		EEXF_MAGIC5		'e'
#define		EEXF_MAGIC6		'x'
#define		EEXF_MAGIC7		'f'

typedef	struct __EXEC_MGSEC
{
    char		    magic[8];			// "ePDK.exf"
    __u32			version;
    __u8			type;				// 0: reserved

    __u32			heapaddr;			//heap在32M空间内的位置，通常在最后1M(31*1024*1024)
    //若地址等于0，那么表示进程不需要局部堆，将共用系统堆
    __u32			heapsize;			//heap的初始大小

    __s32           (*main)(void *p_arg);   //pcb里第一个任务的首地址
    __u32			mtskstksize;		/* 第一个任务的stack	size                        */
    __u8			mtskprio;			/* 第一个任务的优先级                               */

} __exec_mgsec_t;


#endif	//define EEXF_H
