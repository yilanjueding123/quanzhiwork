/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_sqlite3.h
* By      : Sunny
* Version : v1.0
* Date    : 2010-8-22
* Descript: sqlite3 functions stub.
* Update  : date                auther      ver     notes
*           2010-8-22 11:01:08  Sunny       1.0     create this file.
*********************************************************************************************************
*/

#ifndef     __DB_SQLITE3_H__
#define     __DB_SQLITE3_H__

extern __dbms_api_t SQLite3Api;

#define     sqlite3_initialize          SQLite3Api.ptr_sqlite3_initialize
#define     sqlite3_shutdown            SQLite3Api.ptr_sqlite3_shutdown
#define     sqlite3_config              SQLite3Api.ptr_sqlite3_config
#define     sqlite3_db_config           SQLite3Api.ptr_sqlite3_db_config
#define     sqlite3_last_insert_rowid   SQLite3Api.ptr_sqlite3_last_insert_rowid
#define     sqlite3_changes             SQLite3Api.ptr_sqlite3_changes
#define     sqlite3_busy_handler        SQLite3Api.ptr_sqlite3_busy_handler
#define     sqlite3_busy_timeout        SQLite3Api.ptr_sqlite3_busy_timeout
#define     sqlite3_exec                SQLite3Api.ptr_sqlite3_exec
#define     sqlite3_get_table           SQLite3Api.ptr_sqlite3_get_table
#define     sqlite3_free_table          SQLite3Api.ptr_sqlite3_free_table
#define     sqlite3_memory_used         SQLite3Api.ptr_sqlite3_memory_used
#define     sqlite3_memory_highwater    SQLite3Api.ptr_sqlite3_memory_highwater
#define     sqlite3_enable_shared_cache SQLite3Api.ptr_sqlite3_enable_shared_cache
#define     sqlite3_release_memory      SQLite3Api.ptr_sqlite3_release_memory
#define     sqlite3_soft_heap_limit     SQLite3Api.ptr_sqlite3_soft_heap_limit
#define     sqlite3_open_v2             SQLite3Api.ptr_sqlite3_open_v2
#define     sqlite3_close               SQLite3Api.ptr_sqlite3_close
#define     sqlite3_errcode             SQLite3Api.ptr_sqlite3_errcode
#define     sqlite3_errmsg              SQLite3Api.ptr_sqlite3_errmsg
#define     sqlite3_bind_blob           SQLite3Api.ptr_sqlite3_bind_blob
#define     sqlite3_bind_double         SQLite3Api.ptr_sqlite3_bind_double
#define     sqlite3_bind_int            SQLite3Api.ptr_sqlite3_bind_int
#define     sqlite3_bind_int64          SQLite3Api.ptr_sqlite3_bind_int64
#define     sqlite3_bind_null           SQLite3Api.ptr_sqlite3_bind_null
#define     sqlite3_bind_text           SQLite3Api.ptr_sqlite3_bind_text
#define     sqlite3_bind_text16         SQLite3Api.ptr_sqlite3_bind_text16
#define     sqlite3_bind_zeroblob       SQLite3Api.ptr_sqlite3_bind_zeroblob
#define     sqlite3_column_count        SQLite3Api.ptr_sqlite3_column_count
#define     sqlite3_column_name         SQLite3Api.ptr_sqlite3_column_name
#define     sqlite3_column_blob         SQLite3Api.ptr_sqlite3_column_blob
#define     sqlite3_column_bytes        SQLite3Api.ptr_sqlite3_column_bytes
#define     sqlite3_column_double       SQLite3Api.ptr_sqlite3_column_double
#define     sqlite3_column_int          SQLite3Api.ptr_sqlite3_column_int
#define     sqlite3_column_int64        SQLite3Api.ptr_sqlite3_column_int64
#define     sqlite3_column_text         SQLite3Api.ptr_sqlite3_column_text
#define     sqlite3_column_type         SQLite3Api.ptr_sqlite3_column_type
#define     sqlite3_prepare_v2          SQLite3Api.ptr_sqlite3_prepare_v2
#define     sqlite3_reset               SQLite3Api.ptr_sqlite3_reset
#define     sqlite3_step                SQLite3Api.ptr_sqlite3_step
#define     sqlite3_finalize            SQLite3Api.ptr_sqlite3_finalize

int DBLibary_LoadSQLite3Libary(void);
int DBLibary_FreeSQLite3Libary(void);

#endif      /* __DB_SQLITE3_H__ */
