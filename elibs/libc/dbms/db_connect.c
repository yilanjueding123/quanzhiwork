/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_connect.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-8-21
* Descript: dbms connection functions.
* Update  : date                auther      ver     notes
*           2010-8-21 19:56:33  Sunny       1.0     create this file.
*********************************************************************************************************
*/

#include    "dbms_i.h"

/*
*********************************************************************************************************
*                                           DATABASE CREATE
*
* Description:  create an database file whose name is given by the dbname argument.
*               If the dbname is ":memory:", then a private, temporary in-memory database
*               is created for the connection.  This in-memory database will vanish when
*               the database connection is closed.
*
* Arguments  :  dbname  -database file full path.
*
* Returns    :  EPDK_OK   -create database succeeded.
*               EPDK_FAIL -some things have error.
*
* Note       :
*********************************************************************************************************
*/
int DB_Create(const char *dbname)
{
    int rc          = SQLITE_OK;
    sqlite3 *hDB    = NULL;

    if (dbname == NULL)
    {
        __wrn("invalid database name for create\n");
        return EPDK_FAIL;
    }

    /* create new database file */
    rc = sqlite3_open_v2(dbname, &hDB, (SQLITE_OPEN_READWRITE \
                                        | SQLITE_OPEN_CREATE), NULL);
    if (rc != SQLITE_OK)
    {
        __wrn("create database file [%s] failed\n\n", dbname);
        return EPDK_FAIL;
    }

    /* maybe other SQL statement operations here... */

    /* create database file succeeded */
    sqlite3_close(hDB);

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                           DATABASE DELETE
*
* Description:  Delete a database file.
*
* Arguments  :  dbname  -database file full path.
*
* Returns    :  EPDK_OK   -delete database succeeded.
*               EPDK_FAIL -some things have error.
*
* Note       :
*********************************************************************************************************
*/
int DB_Delete(const char *dbname)
{
    if (dbname == NULL)
    {
        __wrn("invalid database name for delete\n");
        return EPDK_FAIL;
    }
    /* delete book list database file directly */
    return eLIBs_remove(dbname);
}

/*
*********************************************************************************************************
*                                       DATABASE CONNECT
*
* Description: Connect an existed database file whose name is given by the dbname argument.
*
* Arguments  : dbname  -Database file full path.
*
* Returns    : An connection instance of the specified dbname, or NULL if there was an error.
*
* Note       :
*********************************************************************************************************
*/
HDBConn DB_Connect(const char *dbname)
{
    int       rc;
    sqlite3  *hDB;
    DBConn_t *pConn;

    if (dbname == NULL)
    {
        __wrn("invalid database name for connect\n");
        return NULL;
    }

    /* allocate a connection handle */
    pConn = DBMem_Alloc(sizeof(DBConn_t));
    if(pConn == NULL)
    {
        __wrn("allocate memory for connection handle failed\n");
        return NULL;
    }

    /* open database file */
    rc = sqlite3_open_v2(dbname, &hDB, SQLITE_OPEN_READWRITE, NULL);
    if (rc != SQLITE_OK)
    {
        __wrn("open database file [%s] failed\n", dbname);
        DBMem_Free(pConn);
        return NULL;
    }

    /* initialize connection handle */
    pConn->Connection = hDB;
    pConn->ErrorNumber = DB_ERROR_NONE;
    eLIBs_memset(pConn->SQLBuf, 0, DB_SQLBUF_LEN);
    pConn->Cursor     = NULL;
    pConn->Next       = NULL;

    DBConn_SetDefalutOptions(pConn);

    /* attach connection to DBLibaryManager  */
    DBLibary_AttachConn(pConn);

    return (HDBConn)pConn;
}

/*
*********************************************************************************************************
*                                       DATABASE DISCONNECT
*
* Description: Disconnects the specified connection from the database.
*
* Arguments  : conn - The database connection.
*
* Returns    :  EPDK_OK   -disconnect database succeeded.
*               EPDK_FAIL -some things have error.
* Note       :
*********************************************************************************************************
*/
int DB_Disconnect(HDBConn conn)
{
    DBConn_t *pConn;

    if (conn == NULL)
    {
        __wrn("invalid connection handle for disconnect\n");
        return EPDK_FAIL;
    }
    pConn = (DBConn_t *)conn;

    /* check connection have unreleased cursors */
    if (pConn->Cursor)
    {
        /* find not free cursor of connection */
        __wrn("disconnect connection have cursors have not freed\n");
        return EPDK_FAIL;
    }

    /* close database */
    sqlite3_close(pConn->Connection);

    /* disattach connection from DBLiabryManager */
    DBLibary_DisattachConn(pConn);

    /* free connection handle */
    DBMem_Free(pConn);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                      SET QUERY WORK MODE
*
* Description: set connection query work mode.
*              query have two work mode :
*              1 query all rows at one time,
*              2 query special rows number step by step.
*
* Arguments  : conn     -The target connection.
*              mode     -The work mode to set, should be DBCONN_QUERY_ALL or DBCONN_QUERY_STEP,
*              RowsNumber-The rows number of query one time, valid for DBCONN_QUERY_STEP work mode set.
*
* Returns    : EPDK_OK   -set query work mode succeeded.
*              EPDK_FAIL -some things have error.
*
* Note       :
*********************************************************************************************************
*/
int DBConn_SetQueryMode(HDBConn conn, int mode, int RowsNumber)
{
    DBConn_t *pConn;

    if (conn == NULL)
    {
        __wrn("invalid connection handle for set query work mode\n");
        return EPDK_FAIL;
    }
    pConn = (DBConn_t *)conn;

    /* check query work mode invalid or not */
    if ((mode != DBCONN_QUERY_ALL) && (mode != DBCONN_QUERY_STEP))
    {
        __wrn("invalid query work mode to set\n");
        return EPDK_FAIL;
    }

    pConn->Options[DBCONFIG_QUERY_MODE] = mode;
    if (RowsNumber)
    {
        /* set rows number of query one time by user specail value */
        pConn->QueryRowNr = RowsNumber;
    }
    else
    {
        /* set default rows number of query one time */
        pConn->QueryRowNr = DB_QUERY_DEFAULT_NR;
    }

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       GET DATBASE TABLE LIST
*
* Description: Queries the list of available tables in a particular database.
*
* Arguments  : conn     -The target connection.
*              pattern  -A string pattern (SQL regular expression) that each name must match,
*                        or NULL to show all available tables.
*
* Returns    : A query cursor object, which will contain table names in the first field
*              (for use with the by-index field functions). In case of an error, the function
*              returns NULL and sets the error number to a database engine-specific nonzero value.
*
* Note       :
*********************************************************************************************************
*/
HDBCursor DBConn_GetTableList(HDBConn conn, const char *pattern)
{
    DBConn_t    *pConn;
    HDBCursor    hCursor;

    if (conn == NULL)
    {
        __wrn("invalid connection handle for disconnect\n");
        return NULL;
    }
    pConn = (DBConn_t *)conn;

    /* sqlite3 does not support the SHOW command, so we have to extract the
     * information from the accessory sqlite3_master table .
     */
    if (pattern == NULL)
    {
        hCursor = DBConn_Query(conn, 0, "sqlite_master", "name", "WHERE type='table' ORDER BY name");
    }
    else
    {
        sprintf(pConn->SQLBuf, "WHERE type='table' AND name LIKE '%s' ORDER BY name", pattern);
        hCursor = DBConn_Query(conn, 0, "sqlitemaster", "name", (const char *)(pConn->SQLBuf));
    }
    return hCursor;
}

/*
*********************************************************************************************************
*                                       EXECUTE QUERY SQL STATEMENT
*
* Description: Execute the specified SQL query statement.
*
* Arguments  : Conn      -The target connection.
*              distinct  -1 if you want each row to be unique, 0 otherwise.
*              table     -The table name of query.
*              columns   -A list of which columns to return. column is separate by ','.
*                        -format:   "column0, column1, column2"
*                        -Passing NULL will return all columns.
*              pattern   -A string pattern(SQL regular expression, ORDERBY,WHERE and so on)
*                        -that column must match, or NULL to show all available columns.
*
* Returns    : A query cursor object, or NULL if there was an error.
*
* Note       :
*********************************************************************************************************
*/
HDBCursor DBConn_Query(HDBConn conn, __bool distinct, const char *table,
                       const char *columns, const char *pattern)
{
    DBConn_t   *pConn;
    DBCursor_t *pCursor;

    if ((conn == NULL) || (table == NULL))
    {
        __wrn("invalid connection handle for query\n");
        return NULL;
    }
    pConn = (DBConn_t *)conn;

    if (DBConn_BuildQueryString(pConn->SQLBuf, distinct, table, columns, pattern) != EPDK_OK)
    {
        __wrn("build query string failed\n");
        return NULL;
    }

    switch (pConn->Options[DBCONFIG_QUERY_MODE])
    {
    case DBCONN_QUERY_ALL:
        pCursor = DBConn_QueryAll(pConn, (const char *)(pConn->SQLBuf));
        break;

    case DBCONN_QUERY_STEP:
        /* just setup query process */
        pCursor = DBConn_SetupQueryProcess(pConn, (const char *)(pConn->SQLBuf));
        break;

    default:
        /* unkown query work mode */
        __wrn("invalid query work mode\n");
        pCursor = NULL;
        break;
    }

    if (pCursor)
    {
        /* attatch cursor to connection */
        DBConn_AttachCursor(pConn, pCursor);

        /* set table name of cursor */
        DBCursor_SetTableName(pCursor, table);
    }

    return (HDBCursor)pCursor;
}

/*
*********************************************************************************************************
*                                    EXECUTE SQL STATEMENT
*
* Description: Execute a single SQL statement that is not a query. For example, CREATE
*              TABLE, DELETE, INSERT, etc. Multiple statements separated by ';'.
*
* Arguments  : statement -SQL statement to be execute.
*
* Returns    : EPDK_OK   -execute sql statement succeeded.
*              EPDK_FAIL -execute sql statement failed.
*
* Note       :
*********************************************************************************************************
*/
int DBConn_Execute(HDBConn conn, const char *statement)
{
    int       rc;
    DBConn_t *pConn;

    if ((conn == NULL) || (statement == NULL))
    {
        __wrn("invalid connection handle for begin transaction\n");
        return EPDK_FAIL;
    }
    pConn = (DBConn_t *)conn;

    rc = sqlite3_exec(pConn->Connection, statement, NULL, NULL, NULL);
    if (rc != SQLITE_OK)
    {
        pConn->ErrorNumber  = sqlite3_errcode(pConn->Connection);
        __wrn("Execute SQL statement [%s] failed, error message : [%s]\n", \
              statement, sqlite3_errmsg(pConn->Connection));

        return EPDK_FAIL;
    }
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       BEGIN TRANSACTION
*
* Description:  Begins a transaction. Transactions can be nested. When the outer transaction is ended all of
*               the work done in that transaction and all of the nested transactions will be committed or
*               rolled back.
*
* Arguments  :  conn -the database connection.
*
* Returns    :  EPDK_OK   -begin transaction succeeded.
*               EPDK_FAIL -some things have error.
*
* Note       :  Here is the standard idiom for transactions:
*               DBConn_BeginTransaction();
*               { do your things; }
*               DBConn_CommitTransaction(); (or DBConn_RollbackTransaction();)
*********************************************************************************************************
*/
int DBConn_BeginTransaction(HDBConn conn)
{
    int       rc;
    DBConn_t *pConn;

    if (conn == NULL)
    {
        __wrn("invalid connection handle for begin transaction\n");
        return EPDK_FAIL;
    }
    pConn = (DBConn_t *)conn;

    rc = sqlite3_exec(pConn->Connection, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    if (rc != SQLITE_OK)
    {
        pConn->ErrorNumber  = sqlite3_errcode(pConn->Connection);
        __wrn("begin transaction failed, error message : [%s]\n", sqlite3_errmsg(pConn->Connection));
        return EPDK_FAIL;
    }
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       COMMIT TRANSACTION
*
* Description: End a transaction. See DBConn_BeginTransaction for notes about how to use this and
*              when transactions are committed and rolled back.
*
* Arguments  :  conn -the database connection.
*
* Returns    :  EPDK_OK   -commit transaction succeeded.
*               EPDK_FAIL -some things have error.
*
* Note       :
*********************************************************************************************************
*/
int DBConn_CommitTransaction(HDBConn conn)
{
    int       rc;
    DBConn_t *pConn;

    if (conn == NULL)
    {
        __wrn("invalid connection handle for commit transaction\n");
        return EPDK_FAIL;
    }
    pConn = (DBConn_t *)conn;

    rc = sqlite3_exec(pConn->Connection, "COMMIT TRANSACTION;", NULL, NULL, NULL);
    if (rc != SQLITE_OK)
    {
        pConn->ErrorNumber  = sqlite3_errcode(pConn->Connection);
        __wrn("commit transaction failed, error message : [%s]\n", sqlite3_errmsg(pConn->Connection));
        return EPDK_FAIL;
    }
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       ROLLBACK TRANSACTION
*
* Description: rollback a transaction. See DBConn_BeginTransaction for notes about how to use this and
*              when transactions are committed and rolled back.
*
* Arguments  :  conn -the target connection.
*
* Returns    :  EPDK_OK   -rollback transaction succeeded.
*               EPDK_FAIL -some things have error.
*
* Note       :
*********************************************************************************************************
*/
int DBConn_RollbackTransaction(HDBConn conn)
{
    int       rc;
    DBConn_t *pConn;

    if (conn == NULL)
    {
        __wrn("invalid connection handle for rollback transaction\n");
        return EPDK_FAIL;
    }
    pConn = (DBConn_t *)conn;

    rc = sqlite3_exec(pConn->Connection, "ROLLBACK TRANSACTION;", NULL, NULL, NULL);
    if (rc != SQLITE_OK)
    {
        pConn->ErrorNumber = sqlite3_errcode(pConn->Connection);
        __wrn("rollback transaction failed, error message : [%s]\n", sqlite3_errmsg(pConn->Connection));
        return EPDK_FAIL;
    }
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       GET LAST INSERT ROWID
*
* Description: Get the ROWID of the most recent insert.
*
* Arguments  : Conn -The database connection.
*
* Returns    : An integer value corresponding to the ID that was created by the last INSERT command.
*
* Note       :
*********************************************************************************************************
*/
int DBConn_LastInsertRowid(HDBConn conn)
{
    DBConn_t *pConn;

    if (conn == NULL)
    {
        __wrn("invalid connection handle for rollback transaction\n");
        return EPDK_FAIL;
    }
    pConn = (DBConn_t *)conn;

    return (int)sqlite3_last_insert_rowid(pConn->Connection);
}

/*
*********************************************************************************************************
*                                       THE NUMBER OF ROWS MODIFIED
*
* Description: Get number of database rows that were changed
*              or inserted or deleted by the most recently completed SQL statement
*              on the database connection specified by the first parameter.
*              Only changes that are directly specified by the [INSERT], [UPDATE],
*              or [DELETE] statement are counted.
*
* Arguments  : Conn -The database connection.
*
* Returns    : number of rows modified.
*
* Note       :
*********************************************************************************************************
*/
int DBConn_RowsChanged(HDBConn conn)
{
    DBConn_t *pConn;

    if (conn == NULL)
    {
        __wrn("invalid connection handle for rollback transaction\n");
        return EPDK_FAIL;
    }
    pConn = (DBConn_t *)conn;

    return sqlite3_changes(pConn->Connection);
}

/*
*********************************************************************************************************
*                                           ERROR MESSAGE
*
* Description: Return English-language text that describes the error,
*			   Memory to hold the error message string is managed internally.
*			   The user does not need to freeing the result.
*
* Arguments  : Conn -The database connection.
*
* Returns    : English-language text that describes the error.
*
* Note       : the returned English-language text is readoly, user can't modify it.
*********************************************************************************************************
*/
const char *DBConn_ErrorMsg(HDBConn conn)
{
    DBConn_t *pConn;

    static const char *ErrorFlagMessages[] =
    {
        /* DB_ERROR_BADOBJECT   */	"An invalid or NULL object was passed to libary",
        /* DB_ERROR_BADTYPE     */	"The requested variable type does not match what libary thinks it should be",
        /* DB_ERROR_BADIDX      */	"An invalid or out-of-range index was passed to libary",
        /* DB_ERROR_BADNAME     */	"An invalid name was passed to libary",
        /* DB_ERROR_UNSUPPORTED */	"This particular libary driver or connection does not support this feature",
        /* DB_ERROR_NOCONN      */	"libary could not establish a connection",
        /* DB_ERROR_NOMEM       */	"libary ran out of memory",
        /* DB_ERROR_BADPTR      */	"An invalid pointer was passed to libary",
        /* DB_ERROR_NONE        */	NULL
    };

    if (conn == NULL)
    {
        __wrn("invalid connection handle for rollback transaction\n");
        return NULL;
    }
    pConn = (DBConn_t *)conn;

    if (SQLITE_ERROR_NUM(pConn->ErrorNumber))
    {
        /* SQLite error */
        return sqlite3_errmsg(pConn->Connection);
    }

    if (DB_INVALID_ERROR_NUM(pConn->ErrorNumber))
    {
        /* invaid error number for DBLibary */
        return DB_INVALID_ERROR_NUM_MSG;
    }

    /* wapper libary error */
    return ErrorFlagMessages[pConn->ErrorNumber - DB_ERROR_BADOBJECT];
}

/*
*********************************************************************************************************
*                                           ERROR CODE
*
* Description: Returns the numeric result code of a database connection.
*
* Arguments  : Conn -The database connection.
*
* Returns    : result code. see error codes for more information.
*
* Note       :
*********************************************************************************************************
*/
int DBConn_ErrorCode(HDBConn conn)
{
    DBConn_t *pConn;

    if (conn == NULL)
    {
        __wrn("invalid connection handle for rollback transaction\n");
        return EPDK_FAIL;
    }
    pConn = (DBConn_t *)conn;

    return pConn->ErrorNumber;
}


int DBConn_AttachCursor(DBConn_t *pConn, DBCursor_t *pCursor)
{
    /* attach pCursor to pConn->Cursor */
    pCursor->Next = pConn->Cursor;
    pConn->Cursor = pCursor;

    return EPDK_OK;
}

int DBConn_DisattachCursor(DBConn_t *pConn, DBCursor_t *pCursor)
{
    DBCursor_t *pVictimPrev;

    /* check disattach the root cursor */
    if (pConn->Cursor == pCursor)
    {
        pConn->Cursor = pCursor->Next;
        return EPDK_OK;
    }

    /* seach victim cursor  */
    pVictimPrev = pConn->Cursor;
    while (pVictimPrev)
    {
        if (pVictimPrev->Next == pCursor)
        {
            /* find victim prev cursor */
            break;
        }
        pVictimPrev = pVictimPrev->Next;
    }

    if (pVictimPrev == NULL)
    {
        __wrn("disattach one invalid cursor\n");
        return EPDK_FAIL;
    }

    /* disattach pCursor from connection */
    pVictimPrev->Next = pCursor->Next;

    return EPDK_OK;
}

void DBConn_ResetError(DBConn_t *pConn)
{
    if (pConn)
    {
        pConn->ErrorNumber = DB_ERROR_NONE;
    }
}

void DBConn_SetErrorNumber(DBConn_t *pConn, int ErrorNum)
{
    if (pConn)
    {
        pConn->ErrorNumber = ErrorNum;
    }
}

int DBConn_SetDefalutOptions(DBConn_t *pConn)
{
    /* default query mode : DB_QUERY_ALL,
     * query all rows one time.
     */
    pConn->Options[DBCONFIG_QUERY_MODE] = DBCONN_QUERY_ALL;
    pConn->QueryRowNr                   = 0;

    return EPDK_OK;
}

int DBConn_BuildQueryString(char *pSQL,  __bool distinct, const char *table,
                            const char *columns, const char *pattern)
{
    eLIBs_strcpy(pSQL, "SELECT ");

    if (distinct)
    {
        /* each row should be unique */
        eLIBs_strcat(pSQL, "DISTINCT ");
    }
    if (columns == NULL)
    {
        /* query all columns,
         * ROWID use for update and delete row locate.
         */
        eLIBs_strcat(pSQL, "*, ROWID ");
    }
    else
    {
        eLIBs_strcat(pSQL, columns);
        eLIBs_strcat(pSQL, ", ROWID ");
    }

    eLIBs_strcat(pSQL, "FROM ");
    eLIBs_strcat(pSQL, table);

    if (pattern != NULL)
    {
        /* query pattern condition */
        eLIBs_strcat(pSQL, " ");
        eLIBs_strcat(pSQL, pattern);
    }
    return EPDK_OK;
}
