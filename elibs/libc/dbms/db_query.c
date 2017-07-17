/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_query.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-8-31
* Descript: dbms query operations implement.
* Update  : date                auther      ver     notes
*           2010-8-31 11:18:24  Sunny       1.0     create this file.
*********************************************************************************************************
*/

#include    "dbms_i.h"

/*
*********************************************************************************************************
*                                       QUERY ALL ROWS
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
DBCursor_t *DBConn_QueryAll(DBConn_t *pConn, const char *statement)
{
    sqlite3_stmt *stmt;
    const char  *tail;
    int          columns;
    int          RowIdx;
    int          columnidx;
    DBCursor_t  *pCursor;

    pCursor = DBCursor_Construct(pConn, NULL, 0); /* unkown rows number now */
    if (pCursor == NULL)
    {
        __wrn("construct query cursor failed\n");
        return NULL;
    }

    /* compile the query into VM code */
    if (sqlite3_prepare_v2(pConn->Connection, statement, -1, &stmt, &tail) != SQLITE_OK)
    {
        pConn->ErrorNumber = sqlite3_errcode(pConn->Connection);
        __wrn("query [%s] failed, error message: [%s]\n", statement, sqlite3_errmsg(pConn->Connection));
        DBCursor_Destory(pCursor);
        return NULL;
    }

    /* set cursor columns information   */
    columns = sqlite3_column_count(stmt);

    /* set cursor columns number.
     * ROWID column use for internal managemant,
     * is hide for use.
     */
    DBCursor_SetColumnsNumber(pCursor, columns);

    /* rowid is the last column */
    pCursor->RowIdColumnIdx = columns - 1;

    /* execute VM step to get columns type and name */
    sqlite3_step(stmt);
    for (columnidx = 0; columnidx < columns; columnidx++)
    {
        const char *ColumnName;
        ColumnName = sqlite3_column_name(stmt, columnidx);
        switch (sqlite3_column_type(stmt, columnidx))
        {
        case SQLITE_NULL:
        {
            DBCursor_AddColumn(pCursor, columnidx, ColumnName, COLUMN_TYPE_NULL);
            break;
        }

        case SQLITE_INTEGER:
        {
            DBCursor_AddColumn(pCursor, columnidx, ColumnName, COLUMN_TYPE_INTEGER);
            break;
        }

        case SQLITE_FLOAT:
        {
            DBCursor_AddColumn(pCursor, columnidx, ColumnName, COLUMN_TYPE_DECIMAL);
            break;
        }

        case SQLITE_TEXT:
        {
            DBCursor_AddColumn(pCursor, columnidx, ColumnName, COLUMN_TYPE_STRING);
            break;
        }

        case SQLITE_BLOB:
        {
            DBCursor_AddColumn(pCursor, columnidx, ColumnName, COLUMN_TYPE_BINARY);
            break;
        }

        default:
        {
            /* default data type is string */
            DBCursor_AddColumn(pCursor, columnidx, ColumnName, COLUMN_TYPE_STRING);
            break;
        }
        }
    }

    /* build rows of query cursor,
     * reset VM to starting state.
     */
    RowIdx  = 0;
    sqlite3_reset(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        DBRow_t  *pRow;

        /* allocate a new row */
        pRow = DBCursor_AllocateRow(columns);

        /* fill row content */
        DBCursor_FillRowContent(stmt, pRow, columns);

        /* add row to query cursor */
        DBCursor_AddRow(pCursor, pRow, RowIdx);
        RowIdx++;
    }
    /* query finalize */
    if (sqlite3_finalize(stmt) != SQLITE_OK)
    {
        pConn->ErrorNumber = sqlite3_errcode(pConn->Connection);
        __wrn("query finalize failed, error message: [%s]\n", \
              sqlite3_errmsg(pConn->Connection));
        DBCursor_Destory(pCursor);
        return NULL;
    }
    /* all rows have query out */
    pCursor->FinishFlag = 1;
    pCursor->Handle     = NULL;

    return pCursor;
}

/*
*********************************************************************************************************
*                                       SETUP QUERY PROCESS
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
DBCursor_t *DBConn_SetupQueryProcess(DBConn_t *pConn, const char *statement)
{
    sqlite3_stmt *stmt;
    const char  *tail;
    int          columns;
    int          RowIdx;
    int          columnidx;
    DBCursor_t  *pCursor;

    pCursor = DBCursor_Construct(pConn, NULL, 0); /* unkown rows number now */
    if (pCursor == NULL)
    {
        __wrn("construct query cursor failed\n");
        return NULL;
    }

    /* compile the query into VM code */
    if (sqlite3_prepare_v2(pConn->Connection, statement, -1, &stmt, &tail) != SQLITE_OK)
    {
        pConn->ErrorNumber = sqlite3_errcode(pConn->Connection);
        __wrn("query [%s] failed, error message: [%s]\n", statement, sqlite3_errmsg(pConn->Connection));
        DBCursor_Destory(pCursor);
        return NULL;
    }

    /* set cursor columns information */
    columns = sqlite3_column_count(stmt);

    /* set cursor columns number.
     * ROWID column use for internal managemant,
     * is hide for use.
     */
    DBCursor_SetColumnsNumber(pCursor, columns);

    /* rowid is the last column */
    pCursor->RowIdColumnIdx = columns - 1;

    /* execute VM step to get columns type and name */
    sqlite3_step(stmt);
    for (columnidx = 0; columnidx < columns; columnidx++)
    {
        const char *ColumnName;
        ColumnName = sqlite3_column_name(stmt, columnidx);
        switch (sqlite3_column_type(stmt, columnidx))
        {
        case SQLITE_NULL:
        {
            DBCursor_AddColumn(pCursor, columnidx, ColumnName, COLUMN_TYPE_NULL);
            break;
        }

        case SQLITE_INTEGER:
        {
            DBCursor_AddColumn(pCursor, columnidx, ColumnName, COLUMN_TYPE_INTEGER);
            break;
        }

        case SQLITE_FLOAT:
        {
            DBCursor_AddColumn(pCursor, columnidx, ColumnName, COLUMN_TYPE_DECIMAL);
            break;
        }

        case SQLITE_TEXT:
        {
            DBCursor_AddColumn(pCursor, columnidx, ColumnName, COLUMN_TYPE_STRING);
            break;
        }

        case SQLITE_BLOB:
        {
            DBCursor_AddColumn(pCursor, columnidx, ColumnName, COLUMN_TYPE_BINARY);
            break;
        }

        default:
        {
            /* default data type is string */
            DBCursor_AddColumn(pCursor, columnidx, ColumnName, COLUMN_TYPE_STRING);
            break;
        }
        }
    }

    /* build rows of query cursor, no need to query all rows. */
    RowIdx = 0;
    sqlite3_reset(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        DBRow_t  *pRow;

        /* allocate a new row */
        pRow = DBCursor_AllocateRow(columns);

        /* fill row content */
        DBCursor_FillRowContent(stmt, pRow, columns);

        /* add row to query cursor */
        DBCursor_AddRow(pCursor, pRow, RowIdx);
        RowIdx++;

        if (RowIdx >= pConn->QueryRowNr)
        {
            /* prefetch rows finished */
            break;
        }
    }
    /* for later fetch left rows */
    pCursor->Handle = (void *)stmt;
    if (RowIdx >= pConn->QueryRowNr)
    {
        /* query process not finished now */
        pCursor->FinishFlag = 0;
    }
    else
    {
        /* all rows have query out */
        pCursor->FinishFlag = 1;
    }
    return pCursor;
}


/*
*********************************************************************************************************
*                                       EXIT QUERY PROCESS
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
int DBConn_ShutDownQueryProcess(DBCursor_t *pCursor)
{
    /* query finalize */
    if (sqlite3_finalize((sqlite3_stmt *)(pCursor->Handle)) != SQLITE_OK)
    {
        pCursor->Connection->ErrorNumber = sqlite3_errcode(pCursor->Connection->Connection);
        __wrn("query finalize failed, error message: [%s]\n", \
              sqlite3_errmsg(pCursor->Connection->Connection));
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
int DBConn_FetchRow(DBCursor_t *pCursor, int rowIdx)
{
    sqlite3_stmt *stmt;
    int          columns;
    int          ValidRowsIdx;

    if (pCursor->FinishFlag)
    {
        /* rows query have finished */
        return EPDK_FAIL;
    }
    ValidRowsIdx = pCursor->ValidRows - 1;
    stmt         = pCursor->Handle;
    columns      = pCursor->ColumnNumber;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        DBRow_t  *pRow;

        /* allocate a new row */
        pRow = DBCursor_AllocateRow(columns);

        /* fill row content */
        DBCursor_FillRowContent(stmt, pRow, columns);

        /* add row to query cursor */
        ValidRowsIdx++;
        DBCursor_AddRow(pCursor, pRow, ValidRowsIdx);

        if (ValidRowsIdx >= rowIdx)
        {
            /* target row fetched out */
            break;
        }
    }
    if (ValidRowsIdx == rowIdx)
    {
        return EPDK_OK;
    }

    /* fetch special row failed, all rows have query out,
     * the requested rowIdx beyond the last row index.
     */
    pCursor->FinishFlag = 1;
    return EPDK_FAIL;
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
int DBConn_FetchLeftRows(DBCursor_t *pCursor)
{
    sqlite3_stmt *stmt;
    int          columns;
    int          RowsIdx;

    if (pCursor->FinishFlag)
    {
        /* rows query have finished */
        return EPDK_FAIL;
    }
    RowsIdx   = pCursor->ValidRows - 1;
    stmt      = pCursor->Handle;
    columns   = pCursor->ColumnNumber;
    while ((sqlite3_step(stmt) == SQLITE_ROW))
    {
        DBRow_t  *pRow;

        /* allocate a new row */
        pRow = DBCursor_AllocateRow(columns);

        /* fill row content */
        DBCursor_FillRowContent(stmt, pRow, columns);

        /* add row to query cursor */
        RowsIdx++;
        DBCursor_AddRow(pCursor, pRow, RowsIdx);
    }
    /* all rows have query out */
    pCursor->FinishFlag = 1;
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       FILL CONTENT OF ROW
*
* Description: fill the content of one row.
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_FillRowContent(sqlite3_stmt *stmt, DBRow_t *pRow, int columns)
{
    int       columnidx;
    DBData_t *pData;

    for (columnidx = 0; columnidx < columns; columnidx++)
    {
        pData = &(pRow->ColumnValues[columnidx]);
        /* this will be set to the string size later on
         * if the column is indeed a string or binary.
         */
        pRow->ColumnSizes[columnidx] = 0;
        switch (sqlite3_column_type(stmt, columnidx))
        {
        case SQLITE_NULL:
        {
            pData->d_string   = DB_NULL_STR;
            break;
        }

        case SQLITE_INTEGER:
        {
            pData->d_int64 = sqlite3_column_int64(stmt, columnidx);
            break;
        }

        case SQLITE_FLOAT:
        {
            pData->d_double = sqlite3_column_double(stmt, columnidx);
            break;
        }

        case SQLITE_TEXT:
        {
            /* SQLite does not include the NULL terminator in size, but does
             * ensure all strings are NULL terminated, so increase size by
             * one to make sure we store the terminator.
             */
            pData->d_string = DBMem_Strdup(sqlite3_column_text(stmt, columnidx));
            pRow->ColumnSizes[columnidx] = sqlite3_column_bytes(stmt, columnidx) + 1;
            break;
        }

        case SQLITE_BLOB:
        {
            pRow->ColumnSizes[columnidx] = sqlite3_column_bytes(stmt, columnidx);
            pData->d_binary = DBMem_Alloc(pRow->ColumnSizes[columnidx]);
            eLIBs_memcpy(pData->d_binary, sqlite3_column_blob(stmt, columnidx), pRow->ColumnSizes[columnidx]);
            break;
        }

        default:
        {
            /* default data type is string */
            pData->d_string = DBMem_Strdup(sqlite3_column_text(stmt, columnidx));
            pRow->ColumnSizes[columnidx] = sqlite3_column_bytes(stmt, columnidx) + 1;
            break;
        }
        }
    }
    return EPDK_OK;
}
