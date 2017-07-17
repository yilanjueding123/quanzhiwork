/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : dbms_i.h
* By      : Sunny
* Version : v1.0
* Date    : 2010-8-21
* Descript: dbms wappper internal header file.
* Update  : date                auther      ver     notes
*           2010-8-21 10:24:47  Sunny       1.0     create this file.
*********************************************************************************************************
*/

#ifndef     __DBMS_I_H__
#define     __DBMS_I_H__

#include    "epdk.h"
#include    <stdarg.h>

#include    "db_sqlite3.h"
#include    "db_mem.h"
#include    "db_debug.h"
#include    "db_cursor.h"
#include    "db_connect.h"
#include    "db_row.h"
#include    "db_column.h"

#define     DBLIBARY_VERSION        "DBLibary1.0"

#define     DB_NULL_STR             ""

#define     DB_QUERY_DEFAULT_NR     (64) /* the default rows number of query one step */

#define     DB_ALIGN(val, align)    (((val) + ((align) - 1)) & ~((align) - 1))

typedef struct DBLibaryManager_S
{
    DBConn_t *ConnRoot;
} DBLibaryManager_t;

/* SQL syntax helper */
__size_t  DB_QuoteString(const char *orig, char *dest, __size_t length);

/* sprintf, use melis C-Libary */
#define     sprintf     eLIBs_sprintf

/* main support services */
int DBLibary_AttachConn(DBConn_t *pConn);
int DBLibary_DisattachConn(DBConn_t *pConn);

/* query support services */
DBCursor_t *DBConn_QueryAll(DBConn_t *pConn, const char *statement);
DBCursor_t *DBConn_SetupQueryProcess(DBConn_t *pConn, const char *statement);
int         DBConn_ShutDownQueryProcess(DBCursor_t *pCursor);
int         DBConn_FetchRow(DBCursor_t *pCursor, int rowIdx);
int         DBConn_FetchLeftRows(DBCursor_t *pCursor);
int         DBCursor_FillRowContent(sqlite3_stmt *stmt, DBRow_t *pRow, int columns);

/* insert support services */
int     DBCursor_PackColumns(DBCursor_t *pCursor, DBRow_t *pRow, const char *format, va_list args);
__bool  DBCursor_RowIsNew(DBRow_t *pRow, int ColumnNumber);

/* update support services */
__bool DBCursor_CursorIsDirty(DBCursor_t *pCursor);
__bool DBCursor_RowIsDirty(DBRow_t *pRow, int ColumnNumber);
int    DBCursor_GetDirtyColumnsNumber(DBRow_t *pRow, int ColumnNumber);
int    DBCursor_BuildUpdateString(char *pSQL, DBRow_t *pRow, DBCursor_t *pCursor);

#endif      /* __DBMS_I_H__ */
