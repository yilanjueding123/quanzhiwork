/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_cursor.h
* By      : Sunny
* Version : v1.0
* Date    : 2010-8-21
* Descript: dbms cursor, manage query result.
* Update  : date                auther      ver     notes
*           2010-8-21 10:24:47  Sunny       1.0     create this file.
*********************************************************************************************************
*/

#ifndef     __DB_CURSOR_H__
#define     __DB_CURSOR_H__

#include    "db_connect.h"

/* rows array grows once step,
 * if rows array have no freed row for filled new row,
 * rows array will grow by ROWS_GROW_STEP one time.
 */
#define     ROWS_GROW_STEP      32

/* get cursor query work mode by cursor handle */
#define CURSOR_QUERY_MODE(Cursor)   (Cursor->Connection->Options[DBCONFIG_QUERY_MODE])

typedef union DBDATA_U
{
    __s64      d_int64;     /* all integer store use __int64 type */
    double     d_double;
    char      *d_string;
    void      *d_binary;
} DBData_t;

typedef struct DBROW_S
{
    DBData_t      *ColumnValues;
    __size_t      *ColumnSizes;     /* strlen() for strings, length for binary, 0 otherwise */
    int           *ColumnStatus;    /* status of column data : dirty or clear               */
} DBRow_t;

typedef struct DBCURSOR_S
{
    __krnl_event_t *Lock;         /* Mutex controlling the lock 	  */
    DBConn_t     *Connection;     /* the connection cursor belong to  */
    sqlite3_stmt *Handle;         /* handle for fetch row             */
    int           FinishFlag;     /* finish flag for step by step query mode*/
    char         *TableName;      /* query table name, use for update and delete */
    int           RowsNumber;     /* rows number within Rows array    */
    int           ValidRows;      /* valid rows number                */
    int           CurrentRowIdx;  /* current row index(0-Base)        */
    DBRow_t     **Rows;           /* array of filled rows, elements set to NULL if not fetched yet */
    int           ColumnNumber;   /* can be zero or NULL until first fetch row */
    int           RowIdColumnIdx; /* index of the row ID column, use for support update, delete */
    char        **ColumnNames;
    int          *ColumnTypes;
    struct DBCURSOR_S *Next;
} DBCursor_t;

/* cursor support services */
DBCursor_t *DBCursor_Construct(DBConn_t *Conn, void *handle, int RowsNumber);
int         DBCursor_Destory(DBCursor_t *pCursor);
int         DBCursor_SetTableName(DBCursor_t *pCursor, const char *table);
int         DBCursor_Lock(DBCursor_t *pCursor);
int         DBCursor_Unlock(DBCursor_t *pCursor);
int         DBCursor_SetColumnsNumber(DBCursor_t *pCursor, int ColumnNumber);
int         DBCursor_AddColumn(DBCursor_t *pCursor, int Idx, const char *Name, int Type);
DBRow_t    *DBCursor_AllocateRow(int ColumnsNumber);
void        DBCursor_FreeRow(DBCursor_t *pCursor, DBRow_t *pRow);
int         DBCursor_AddRow(DBCursor_t *pCursor, DBRow_t *pRow, int RowIdx);
int         DBCursor_RemoveRow(DBCursor_t *pCursor, int RowIdx);

#endif      /* __DB_CURSOR_H__ */
