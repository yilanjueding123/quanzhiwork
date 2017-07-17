/*
********************************************************************************************************************
*
*
*                              (c) Copyright 2007-2010, javen.China
*										All	Rights Reserved
*
* File Name 	: memory_allot.h
*
* Author 		: javen
*
* Version 		: 2.0
*
* Date 			: 2010.03.02
*
* Description 	: ƒ⁄¥Ê‘§∑÷≈‰
*
* History 		:
*
********************************************************************************************************************
*/
#ifndef  __MEMORY_ALLOT_H__
#define  __MEMORY_ALLOT_H__

#define  MAX_MEMORY_BLOCK_NUM		32

void *memory_malloc(__u32 size, u8 *file_name , u32 line_nr);
void memory_free(void *addr, u8 *file_name , u32 line_nr);

__s32 memory_allot_init(void);
__s32 memory_allot_exit(void);


#endif   //__MEMORY_ALLOT_H__


