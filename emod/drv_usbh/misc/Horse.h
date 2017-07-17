/*
********************************************************************************************************************
*
*
*                              (c) Copyright 2007-2010, javen.China
*										All	Rights Reserved
*
* File Name 	: horse.h
*
* Author 		: javen
*
* Version 		: 2.0
*
* Date 			: 2010.03.02
*
* Description 	: 内存预分配
*
* History 		:
*
********************************************************************************************************************
*/
#ifndef  __HORSE_H__
#define  __HORSE_H__

#define  MAX_HORSE_USER_NUM    10   /* 最大用户个数 */


//---------------------------------------------------------------
//
//---------------------------------------------------------------
typedef  struct __HorseBlock
{
    __u32  is_used;			//是否占用

    void *buff;			//
    __u32  size;			//buffer大小
} __HorseBlock_t;

typedef  struct __HorseHead
{
    __u32 used;				//是否占用

    __u32 nr;				//block的个数
    __HorseBlock_t *entry;  //block数组的入口
} __HorseHead_t;

typedef struct __HorseMgr
{
    __HorseHead_t Horse[MAX_HORSE_USER_NUM];
} __HorseMgr_t;

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void *HorseHeadInit(__u32 size, __u32 nr);
__s32 HorseHeadExit(void *hdle);
void *HorseBlockMalloc(void *hdle);
__s32 HorseBlockFree(void *hdle, void *buffer_addr);

__s32 HorseInit(void);
__s32 HorseExit(void);

#endif   //__HORSE_H__


