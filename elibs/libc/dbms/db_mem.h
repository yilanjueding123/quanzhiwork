/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_mem.h
* By      : Sunny
* Version : v1.0
* Date    : 2010-8-21
* Descript: dbms memory allocator sub-system, external interface header file.
* Update  : date                auther      ver     notes
*           2010-8-21 15:36:29  Sunny       1.0     create this file.
*********************************************************************************************************
*/

#ifndef     __DB_MEM_H__
#define     __DB_MEM_H__

/* allocator initialize and exit */
int     DBMem_Init(void);
void    DBMem_Exit(void);

/* debug helper */
__u32   DBMem_GetTotalSize(void);
__u32   DBMem_GetCurSize(void);
__u32   DBMem_GetHighWater(void);

/* allocator comman operations */
void   *DBMem_Alloc(__u32 size);
void   *DBMem_Calloc(__u32 num, __u32 size);
void   *DBMem_Realloc(void *mem, __u32 size);
void    DBMem_Free(void *mem);
char   *DBMem_Strdup(const char *string);

#endif      /* __DB_MEM_H__ */
