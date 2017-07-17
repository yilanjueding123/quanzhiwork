/*
********************************************************************************************************************
*                                              USB Host Drvier
*
*                              (c) Copyright 2007-2009, softwinners.China
*										All	Rights Reserved
*
* File Name 	: platform_debug.h
*
* Author 		: javen
*
* Version 		: 1.0
*
* Date 			: 2009.10.12
*
* Description 	:
*
* History 		:
*
********************************************************************************************************************
*/
#ifndef  __PLATFORM_DEBUG_H__
#define  __PLATFORM_DEBUG_H__

/* 调试malloc和free */
typedef struct __memory_debug
{
    __u32 nr;			/* malloc的次数 */

    __u32 *addr;		/* malloc的地址 */
    __u8 file[256];		/* 文件名 		*/
    __u32 line;			/* 行号 		*/
} __memory_debug_t;


void *pf_malloc(u32 size, u8 *file_name , u32 line_nr);
void pf_free(void *addr);

__s32 platform_debug_init(void);
__s32 platform_debug_exit(void);

#endif   //__PLATFORM_DEBUG_H__


