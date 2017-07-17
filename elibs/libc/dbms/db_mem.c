/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_mem.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-8-21
* Descript: dbms memory allocator sub-system, have good performance for small objects.
* Update  : date                auther      ver     notes
*           2010-8-21 15:36:29  Sunny       1.0     create this file.
*********************************************************************************************************
*/

#include    "dbms_i.h"
#include	"db_mem_i.h"

static struct AllocatorManager AllocMan;

/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int DBMem_Init(void)
{
    __u32 k;
    __u32 sizeIndex = 0; // (size+3)/4 - 1
    __u32 index = 0;
    __u32 sizesPerRange;

    /* allocate static pool from system heap */
    AllocMan.staticPool = eLIBs_malloc(STATIC_POOL_SIZE);
    if (AllocMan.staticPool == NULL)
    {
        __wrn("allocate static pool from system heap failed\n");
        return EPDK_FAIL;
    }

    /* allocate sem lock for allocator */
    AllocMan.Lock = esKRNL_SemCreate(1);
    if (AllocMan.Lock == NULL)
    {
        __wrn("allocate sem lock for allocator failed\n");
        eLIBs_free(AllocMan.staticPool);
        return EPDK_FAIL;
    }

    AllocMan.staticPoolFree = AllocMan.staticPool;
    AllocMan.staticPoolEnd  = AllocMan.staticPool + STATIC_POOL_SIZE;

    /* initialize buckets */
    while(sizeIndex <= SMALL_SIZE / 4)
    {
        sizesPerRange = 1 + sizeIndex / MIN_WASTE;
        for(k = 0; (k < sizesPerRange) && (sizeIndex <= SMALL_SIZE / 4); k++)
        {
            AllocMan.bucketPtr[sizeIndex++] = &AllocMan.buckets[index];
        }
        AllocMan.buckets[index].chunkSize = sizeIndex * 4;
        AllocMan.buckets[index].freeList  = NULL;
        index++;
    }

#if ALLOC_STATUS_EN
    AllocMan.curAllocSize   = 0;
    AllocMan.allocCnt       = 0;
    AllocMan.freeCnt        = 0;
    AllocMan.highWater      = 0;
#endif

    __inf("DBMS allocator initialize succeeded\n");
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
void DBMem_Exit(void)
{
    __u8    err;

    if (AllocMan.staticPool)
    {
        eLIBs_free(AllocMan.staticPool);
    }
    if (AllocMan.Lock)
    {
        /* release sem lock */
        esKRNL_SemDel(AllocMan.Lock, OS_DEL_ALWAYS, &err);
    }
}

/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__u32 DBMem_GetTotalSize(void)
{
    return STATIC_POOL_SIZE;
}

/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__u32 DBMem_GetCurSize(void)
{
#if ALLOC_STATUS_EN
    return AllocMan.curAllocSize;
#else
    return 0;
#endif
}

/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__u32 DBMem_GetHighWater(void)
{
#if ALLOC_STATUS_EN
    return AllocMan.highWater;
#else
    return 0;
#endif
}

/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
void *DBMem_Alloc(__u32 size)
{
    if (size <= 0)
    {
        /* invalid block size to allocate */
        return NULL;
    }

    /* large blocks go through system allocator */
    if(size <= SMALL_SIZE)
    {
        __u8 *memLoc;
        __u8 *endLoc;
        struct MemoryBucket *bucket = AllocMan.bucketPtr[(size + 3) / 4 - 1];

        DBMem_Lock();
        if(bucket->freeList != NULL)
        {
            void *result = bucket->freeList;
            bucket->freeList = *(void **)((__u32)result + sizeof(__u32));

#if ALLOC_STATUS_EN
            AllocMan.curAllocSize += size;
            AllocMan.allocCnt++;
#endif

            /* reset allocate bucket size */
            *(__u32 *)result = size;

            DBMem_Unlock();
            return (void *)((__u32)result + sizeof(__u32));
        }

        /* see if still have space in static pool */
        memLoc = AllocMan.staticPoolFree;
        endLoc = memLoc + bucket->chunkSize + sizeof(__u32);
        if(endLoc <= AllocMan.staticPoolEnd)
        {
            AllocMan.staticPoolFree = endLoc;
            *(__u32 *)memLoc = size;

#if ALLOC_STATUS_EN
            AllocMan.curAllocSize += size;
            AllocMan.allocCnt++;
            if (AllocMan.curAllocSize > AllocMan.highWater)
            {
                AllocMan.highWater = AllocMan.curAllocSize;
            }
#endif

            DBMem_Unlock();
            return (void *)((__u32)memLoc + sizeof(__u32));
        }
        /* no space in static pool, use system allocator */
        DBMem_Unlock();
        __wrn("static pool empty...\n");
    }

    /* large block or static pool allocation failed,
     * use system allocator.
     */
    return eLIBs_malloc(size);
}

/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
void *DBMem_Realloc(void *mem, __u32 size)
{
    /* memory allocate from static pool */
    if((AllocMan.staticPool <= (__u8 *)mem) && ((__u8 *)mem < AllocMan.staticPoolEnd))
    {
        __u32    src_size;
        void    *new_mems;

        src_size = *(__u32 *)((__u32)mem - sizeof(__u32));
        new_mems = DBMem_Alloc(size);
        if (new_mems == NULL)
        {
            __wrn("malloc failed\n");
            return mem;
        }
        /*  copy data   */
        eLIBs_memcpy(new_mems, mem, size < src_size ? size : src_size);

        /* free old memory block */
        DBMem_Free(mem);

        return new_mems;
    }
    else
    {
        /* memory reallocate from system */
        return eLIBs_realloc(mem, size);
    }
}

/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
void *DBMem_Calloc(__u32 num, __u32 size)
{
    __u32   alloc_size = num * size;
    void   *mem;
    mem = DBMem_Alloc(alloc_size);
    if (mem)
    {
        eLIBs_memset(mem, 0, alloc_size);
    }
    return mem;
}

/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
void DBMem_Free(void *mem)
{
    __u32 size;

    if (mem == NULL)
    {
        return ;
    }
    if((AllocMan.staticPool <= (__u8 *)mem) && ((__u8 *)mem < AllocMan.staticPoolEnd))
    {
        struct MemoryBucket *bucket;

        mem = (void *)((__u32)mem - sizeof(__u32));
        size = *((__u32 *)mem);
        if (size <= 0 || size > SMALL_SIZE)
        {
            /* invalid block size to free */
            __wrn("invalid block size to free\n");
            return ;
        }

        /* allocation in static pool */
        bucket = AllocMan.bucketPtr[(size + 3) / 4 - 1];
        *(void **)((__u32)mem + sizeof(__u32)) = bucket->freeList;
        bucket->freeList = mem;

#if ALLOC_STATUS_EN
        AllocMan.freeCnt++;
        AllocMan.curAllocSize -= size;
#endif
    }
    else
    {
        eLIBs_free(mem);
    }
}


/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
char *DBMem_Strdup(const char *string)
{
    int     len;
    char   *newStr;
    if (string == NULL)
    {
        return NULL;
    }
    len    = eLIBs_strlen(string) + 1;
    newStr = DBMem_Alloc(len);
    if (newStr)
    {
        eLIBs_strncpy(newStr, string, len);
    }
    return newStr;
}

/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int DBMem_Lock(void)
{
    __u8 err;

    esKRNL_SemPend(AllocMan.Lock, 0, &err);
    if(err != OS_NO_ERR)
    {
        __wrn("DBMS allocator lock failed!\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int DBMem_Unlock(void)
{
    esKRNL_SemPost(AllocMan.Lock);
    return EPDK_OK;
}
