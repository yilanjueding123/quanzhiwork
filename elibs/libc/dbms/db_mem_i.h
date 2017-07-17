/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_mem_i.h
* By      : Sunny
* Version : v1.0
* Date    : 2010-8-21
* Descript: dbms memory allocator sub-system, intrenal header file.
* Update  : date                auther      ver     notes
*           2010-8-21 15:36:29  Sunny       1.0     create this file.
*********************************************************************************************************
*/

#ifndef     __DB_MEM_I_H__
#define     __DB_MEM_I_H__

#include	"db_mem.h"

#define ALLOC_STATUS_EN     1

/* allocator config */
#define STATIC_POOL_SIZE    (2 * 1024 * 1024)
#define SMALL_SIZE          128
#define ALIGN_SIZE          4
#define MIN_WASTE           10

struct MemoryBucket
{
    void    *freeList;
    __u32	 chunkSize;
};

struct AllocatorManager
{
    /* small objects of certain size range are allocated from the same bucket,
    ** s_bucketPtr provides a pointer to the bucket based on the object size.
    */
    struct MemoryBucket *bucketPtr[SMALL_SIZE / ALIGN_SIZE + 1];
    struct MemoryBucket  buckets[SMALL_SIZE / ALIGN_SIZE + 1];

    __u8    *staticPool;
    __u8    *staticPoolEnd;
    __u8    *staticPoolFree;
    __krnl_event_t *Lock;         /* Mutex controlling the lock 	  */

#if ALLOC_STATUS_EN
    __u32   curAllocSize;
    __u32   highWater;
    __u32   allocCnt;
    __u32   freeCnt;
#endif
};

int DBMem_Lock(void);
int DBMem_Unlock(void);

#endif      /* __DB_MEM_I_H__ */
