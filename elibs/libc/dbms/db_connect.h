/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_connect.h
* By      : Sunny
* Version : v1.0
* Date    : 2010-8-25
* Descript: database connections manager header file.
* Update  : date                auther      ver     notes
*           2010-8-25 14:40:05  Sunny       1.0     create this file.
*********************************************************************************************************
*/

#ifndef     __DB_CONNECT_H__
#define     __DB_CONNECT_H__

#include    "db_cursor.h"

/* database connect config option types */
#define DBCONFIG_QUERY_MODE     0
#define DBCONFIG_UNKOWN         1       /* must be the last one */

/* SQL statement wapper buffer length */
#define	DB_SQLBUF_LEN	        4096


/* DBLibary intrenal error handing */
#define SQLITE_ERROR_NUM(err)       (err >= 0)
#define DB_INVALID_ERROR_NUM(err)   ((err < DB_ERROR_BADOBJECT) || (err > DB_ERROR_NONE))
#define DB_INVALID_ERROR_NUM_MSG    "invalid error number"

/* database connect struture */
typedef struct DBCONN_S
{
    sqlite3         *Connection;        /* database connection to SQLite                    */
    int              ErrorNumber;       /* the most recent database operation error number  */
    int              QueryRowNr;        /* rows number of query one step, just valid for DB_QUERY_STEP work mode */
    int              Options[DBCONFIG_UNKOWN];/* database connection config information     */
    char             SQLBuf[DB_SQLBUF_LEN];/* temp buffer use for SQL statement wapper      */
    struct DBCURSOR_S *Cursor;           /* so dbms can detect unreleaseed cursor at exit     */
    struct DBCONN_S *Next;               /* so dbms can detect unreleaseed connections at exit*/
} DBConn_t;

/* connection support services */
int     DBConn_AttachCursor(DBConn_t *pConn, struct DBCURSOR_S *pCursor);
int     DBConn_DisattachCursor(DBConn_t *pConn, struct DBCURSOR_S *pCursor);
void    DBConn_ResetError(DBConn_t *pConn);
void    DBConn_SetErrorNumber(DBConn_t *pConn, int ErrorNum);
int     DBConn_SetDefalutOptions(DBConn_t *pConn);
int     DBConn_BuildQueryString(char *pSQL,  __bool distinct, const char *table,
                                const char *columns, const char *pattern);

#endif      /* __DB_CONNECT_H__ */
