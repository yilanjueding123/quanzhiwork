/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_update.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-9-1
* Descript: dbms update operations implement.
* Update  : date                auther      ver     notes
*           2010-9-1 13:54:16   Sunny       1.0     create this file.
*********************************************************************************************************
*/

#include    "dbms_i.h"

/*
*********************************************************************************************************
*                                   UPDATE COLUMN DATA AS INT BY COLUMN INDEX
*
* Description: Updates the value for the given column in the row the cursor is
*              currently pointing at. Updates are not committed to the backing store
*              until DBCursor_CommitUpdate() is called.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*              value     -The update new value.
*
* Returns    : EPDK_OK   -update succeeded.
*              EPDK_FAIL -an error occurs.
* Note       :
*********************************************************************************************************
*/
int DBCursor_UpdateIntByIdx(HDBCursor cursor, int columnIdx, int value)
{
    return DBCursor_UpdateInt64ByIdx(cursor, columnIdx, value);
}

/*
*********************************************************************************************************
*                               UPDATE COLUMN DATA AS INT64 BY COLUMN INDEX
*
* Description: Updates the value for the given column in the row the cursor is
*              currently pointing at. Updates are not committed to the backing store
*              until DBCursor_CommitUpdate() is called.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*              value     -The update new value.
*
* Returns    : EPDK_OK   -update succeeded.
*              EPDK_FAIL -an error occurs.
* Note       :
*********************************************************************************************************
*/
int DBCursor_UpdateInt64ByIdx(HDBCursor cursor, int columnIdx, __s64 value)
{
    DBCursor_t *pCursor;
    int         CurRowIdx;

    if (cursor == NULL)
    {
        __wrn("invalid argument for update column data as int64 by column index\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    if (INVALID_COLUMN_INDEX(pCursor, columnIdx))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return EPDK_FAIL;
    }
    if ((pCursor->Rows == NULL) || (pCursor->ColumnTypes == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return EPDK_FAIL;
    }

    if (pCursor->ColumnTypes[columnIdx] != COLUMN_TYPE_INTEGER)
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADTYPE);
        return EPDK_FAIL;
    }

    CurRowIdx = pCursor->CurrentRowIdx;
    if ((pCursor->Rows[CurRowIdx] == NULL) || (pCursor->Rows[CurRowIdx]->ColumnValues == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADOBJECT);
        return EPDK_FAIL;
    }
    /* update column data to new value */
    pCursor->Rows[CurRowIdx]->ColumnValues[columnIdx].d_int64 = value;

    /* column status set to dirty */
    pCursor->Rows[CurRowIdx]->ColumnStatus[columnIdx] = DB_COLUMN_DIRTY;

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                   UPDATE COLUMN DATA AS DOUBLE BY COLUMN INDEX
*
* Description: Updates the value for the given column in the row the cursor is
*              currently pointing at. Updates are not committed to the backing store
*              until DBCursor_CommitUpdate() is called.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*              value     -The update new value.
*
* Returns    : EPDK_OK   -update succeeded.
*              EPDK_FAIL -an error occurs.
* Note       :
*********************************************************************************************************
*/
int DBCursor_UpdateDoubleByIdx(HDBCursor cursor, int columnIdx, double value)
{
    DBCursor_t *pCursor;
    int         CurRowIdx;

    if (cursor == NULL)
    {
        __wrn("invalid argument for update column data as int64 by column index\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    if (INVALID_COLUMN_INDEX(pCursor, columnIdx))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return EPDK_FAIL;
    }
    if ((pCursor->Rows == NULL) || (pCursor->ColumnTypes == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return EPDK_FAIL;
    }

    if (pCursor->ColumnTypes[columnIdx] != COLUMN_TYPE_DECIMAL)
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADTYPE);
        return EPDK_FAIL;
    }

    CurRowIdx = pCursor->CurrentRowIdx;
    if ((pCursor->Rows[CurRowIdx] == NULL) || (pCursor->Rows[CurRowIdx]->ColumnValues == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADOBJECT);
        return EPDK_FAIL;
    }
    /* update column data to new value */
    pCursor->Rows[CurRowIdx]->ColumnValues[columnIdx].d_double = value;

    /* column status set to dirty */
    pCursor->Rows[CurRowIdx]->ColumnStatus[columnIdx] = DB_COLUMN_DIRTY;

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                   UPDATE COLUMN DATA AS STRING BY COLUMN INDEX
*
* Description: Updates the value for the given column in the row the cursor is
*              currently pointing at. Updates are not committed to the backing store
*              until DBCursor_CommitUpdate() is called.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*              string    -The update new string point.
*
* Returns    : EPDK_OK   -update succeeded.
*              EPDK_FAIL -an error occurs.
* Note       :
*********************************************************************************************************
*/
int DBCursor_UpdateStringByIdx(HDBCursor cursor, int columnIdx, const char *string)
{
    DBCursor_t *pCursor;
    int         CurRowIdx;
    __size_t    Len;

    if (cursor == NULL)
    {
        __wrn("invalid argument for update column data as string by column index\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    if (INVALID_COLUMN_INDEX(pCursor, columnIdx))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return EPDK_FAIL;
    }
    if ((pCursor->Rows == NULL) || (pCursor->ColumnTypes == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return EPDK_FAIL;
    }

    if (pCursor->ColumnTypes[columnIdx] != COLUMN_TYPE_STRING)
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADTYPE);
        return EPDK_FAIL;
    }

    CurRowIdx = pCursor->CurrentRowIdx;
    if ((pCursor->Rows[CurRowIdx] == NULL) || (pCursor->Rows[CurRowIdx]->ColumnValues == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADOBJECT);
        return EPDK_FAIL;
    }
    /* check old string store space enough or not,
     * should include the null terminator.
     */
    Len = eLIBs_strlen(string) + 1;
    if (Len > pCursor->Rows[CurRowIdx]->ColumnSizes[columnIdx])
    {
        /* the old string space not enough, need to allocate more to store new string */
        DBMem_Free(pCursor->Rows[CurRowIdx]->ColumnValues[columnIdx].d_string);
        pCursor->Rows[CurRowIdx]->ColumnValues[columnIdx].d_string = DBMem_Strdup(string);
    }
    else
    {
        /* just overlay the old string */
        eLIBs_strncpy(pCursor->Rows[CurRowIdx]->ColumnValues[columnIdx].d_string, string, Len);
    }
    /* update string length */
    pCursor->Rows[CurRowIdx]->ColumnSizes[columnIdx] = Len;

    /* column status set to dirty */
    pCursor->Rows[CurRowIdx]->ColumnStatus[columnIdx] = DB_COLUMN_DIRTY;

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                   UPDATE COLUMN DATA AS BINARY BY COLUMN INDEX
*
* Description: Updates the value for the given column in the row the cursor is
*              currently pointing at. Updates are not committed to the backing store
*              until DBCursor_CommitUpdate() is called.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*              binary    -The update new value point.
*              size      -The update binary data size.
*
* Returns    : EPDK_OK   -update succeeded.
*              EPDK_FAIL -an error occurs.
* Note       :
*********************************************************************************************************
*/
int DBCursor_UpdateBinaryByIdx(HDBCursor cursor, int columnIdx, void *binary, __size_t size)
{
    DBCursor_t *pCursor;
    int         CurRowIdx;
    void       *pOldBinary;

    if (cursor == NULL)
    {
        __wrn("invalid argument for update column data as binary by column index\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    /* cursor must have ROWID column for update */
    // BY Sunny at 2010-9-13 20:23:48.
    // must add implement here


    if (INVALID_COLUMN_INDEX(pCursor, columnIdx))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return EPDK_FAIL;
    }
    if ((pCursor->Rows == NULL) || (pCursor->ColumnTypes == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return EPDK_FAIL;
    }

    if (pCursor->ColumnTypes[columnIdx] != COLUMN_TYPE_BINARY)
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADTYPE);
        return EPDK_FAIL;
    }

    CurRowIdx = pCursor->CurrentRowIdx;
    if ((pCursor->Rows[CurRowIdx] == NULL) || (pCursor->Rows[CurRowIdx]->ColumnValues == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADOBJECT);
        return EPDK_FAIL;
    }

    /* check old binary store space enough or not */
    pOldBinary = pCursor->Rows[CurRowIdx]->ColumnValues[columnIdx].d_binary;
    if (size > pCursor->Rows[CurRowIdx]->ColumnSizes[columnIdx])
    {
        /* the old binary store space not enough, need to allocate more to store new binary data */
        DBMem_Free(pOldBinary);
        pOldBinary = DBMem_Alloc(size);
        if (pOldBinary == NULL)
        {
            __wrn("allocate memory for update binary failed\n");
            DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_NOMEM);
            return EPDK_FAIL;
        }
        eLIBs_memcpy(pOldBinary, binary, size);
    }
    else
    {
        /* just overlay the old binary data */
        eLIBs_memcpy(pOldBinary, binary, size);
    }
    /* update binary data size */
    pCursor->Rows[CurRowIdx]->ColumnSizes[columnIdx] = size;

    /* column status set to dirty */
    pCursor->Rows[CurRowIdx]->ColumnStatus[columnIdx] = DB_COLUMN_DIRTY;

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                   UPDATE COLUMN DATA AS INT BY COLUMN NAME
*
* Description: Updates the value for the given column in the row the cursor is
*              currently pointing at. Updates are not committed to the backing store
*              until DBCursor_CommitUpdate() is called.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*              value     -The update new value.
*
* Returns    : EPDK_OK   -update succeeded.
*              EPDK_FAIL -an error occurs.
* Note       :
*********************************************************************************************************
*/
int DBCursor_UpdateInt(HDBCursor cursor, const char *columnName, int value)
{
    return DBCursor_UpdateInt64(cursor, columnName, (__s64)value);
}

/*
*********************************************************************************************************
*                                   UPDATE COLUMN DATA AS INT64 BY COLUMN NAME
*
* Description: Updates the value for the given column in the row the cursor is
*              currently pointing at. Updates are not committed to the backing store
*              until DBCursor_CommitUpdate() is called.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*              value     -The update new value.
*
* Returns    : EPDK_OK   -update succeeded.
*              EPDK_FAIL -an error occurs.
* Note       :
*********************************************************************************************************
*/
int DBCursor_UpdateInt64(HDBCursor cursor, const char *columnName, __s64 value)
{
    int         ColumnIdx;
    DBCursor_t *pCursor;

    if (cursor == NULL || columnName == NULL)
    {
        __wrn("invalid argument for update column data as int64 by column name\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    ColumnIdx = DBCurosr_FindColumn(pCursor, columnName);
    if (ColumnIdx == -1)
    {
        /* find target column failed */
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADNAME);
        return EPDK_FAIL;
    }
    /* update column data as int64 by column index */
    return DBCursor_UpdateInt64ByIdx(cursor, ColumnIdx, value);
}

/*
*********************************************************************************************************
*                                   UPDATE COLUMN DATA AS DOUBLE BY COLUMN NAME
*
* Description: Updates the value for the given column in the row the cursor is
*              currently pointing at. Updates are not committed to the backing store
*              until DBCursor_CommitUpdate() is called.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*              value     -The update new value.
*
* Returns    : EPDK_OK   -update succeeded.
*              EPDK_FAIL -an error occurs.
* Note       :
*********************************************************************************************************
*/
int DBCursor_UpdateDouble(HDBCursor cursor, const char *columnName, double value)
{
    int         ColumnIdx;
    DBCursor_t *pCursor;

    if (cursor == NULL || columnName == NULL)
    {
        __wrn("invalid argument for update column data as double by column name\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    ColumnIdx = DBCurosr_FindColumn(pCursor, columnName);
    if (ColumnIdx == -1)
    {
        /* find target column failed */
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADNAME);
        return EPDK_FAIL;
    }
    /* update column data as double by column index */
    return DBCursor_UpdateDoubleByIdx(cursor, ColumnIdx, value);
}

/*
*********************************************************************************************************
*                                   UPDATE COLUMN DATA AS STRING BY COLUMN NAME
*
* Description: Updates the value for the given column in the row the cursor is
*              currently pointing at. Updates are not committed to the backing store
*              until DBCursor_CommitUpdate() is called.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*              value     -The update new value.
*
* Returns    : EPDK_OK   -update succeeded.
*              EPDK_FAIL -an error occurs.
* Note       :
*********************************************************************************************************
*/
int DBCursor_UpdateString(HDBCursor cursor, const char *columnName, const char *string)
{
    int         ColumnIdx;
    DBCursor_t *pCursor;

    if (cursor == NULL || columnName == NULL)
    {
        __wrn("invalid argument for update column data as string by column name\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    ColumnIdx = DBCurosr_FindColumn(pCursor, columnName);
    if (ColumnIdx == -1)
    {
        /* find target column failed */
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADNAME);
        return EPDK_FAIL;
    }
    /* update column data as string by column index */
    return DBCursor_UpdateStringByIdx(cursor, ColumnIdx, string);
}

/*
*********************************************************************************************************
*                                   UPDATE COLUMN DATA AS BINARY BY COLUMN NAME
*
* Description: Updates the value for the given column in the row the cursor is
*              currently pointing at. Updates are not committed to the backing store
*              until DBCursor_CommitUpdate() is called.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*              binary    -The update new value point.
*              size      -The update binary data size.
*
* Returns    : EPDK_OK   -update succeeded.
*              EPDK_FAIL -an error occurs.
* Note       :
*********************************************************************************************************
*/
int DBCursor_UpdateBinary(HDBCursor cursor, const char *columnName, void *binary, __size_t size)
{
    int         ColumnIdx;
    DBCursor_t *pCursor;

    if (cursor == NULL || columnName == NULL)
    {
        __wrn("invalid argument for update column data as binary by column name\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    ColumnIdx = DBCurosr_FindColumn(pCursor, columnName);
    if (ColumnIdx == -1)
    {
        /* find target column failed */
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADNAME);
        return EPDK_FAIL;
    }
    /* update column data as binary by column index */
    return DBCursor_UpdateBinaryByIdx(cursor, ColumnIdx, binary, size);
}

/*
*********************************************************************************************************
*                                       COMMIT UPDATE COLUMNS
*
* Description: Atomically commits all updates to the backing store. After completion,
*              the backing store data and cursor data is in a consistent state.
*
* Arguments  : cursor    -The target query cursor handle.
*
* Returns    : EPDK_OK   -commit succeeded.
*              EPDK_FAIL -an error occurs.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_CommitUpdate(HDBCursor cursor)
{
    int         RowIdx;
    int         ColumnIdx;
    int         UpdateColumnIdx;
    DBCursor_t *pCursor;
    DBConn_t   *pConn;
    DBRow_t    *pCurrentRow;
    sqlite3_stmt *stmt;
    int         rc;
    const char *tail;

    if (cursor == NULL)
    {
        __wrn("invalid argument for commit update\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    /* check cursor status first */
    if (DBCursor_CursorIsDirty(pCursor) == EPDK_FALSE)
    {
        __wrn("all columns of [%x] are clear\n", pCursor);
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_UNSUPPORTED);
        return EPDK_FAIL;
    }
    /* commit all dirty columns within one transaction */
    pConn = pCursor->Connection;
    DBConn_BeginTransaction((HDBConn)pConn);

    for (RowIdx = 0; RowIdx < pCursor->ValidRows; RowIdx++)
    {
        pCurrentRow = pCursor->Rows[RowIdx];
        if (DBCursor_RowIsDirty(pCurrentRow, pCursor->ColumnNumber) == EPDK_FALSE)
        {
            /* skip clear row */
            continue;
        }

        /* build update SQL statement */
        DBCursor_BuildUpdateString((char *)(pConn->SQLBuf), pCurrentRow, pCursor);

        /* compile the statement into VM code */
        sqlite3_prepare_v2(pConn->Connection, pConn->SQLBuf, -1, &stmt, &tail);

        /* base index of SQLite column binding is 1 */
        UpdateColumnIdx = 1;

        /* bind update value to prepared statement */
        for (ColumnIdx = 0; ColumnIdx < (pCursor->ColumnNumber - 1); ColumnIdx++)
        {
            if (pCurrentRow->ColumnStatus[ColumnIdx] != DB_COLUMN_DIRTY)
            {
                /* clear column */
                continue;
            }
            switch (pCursor->ColumnTypes[ColumnIdx])
            {
            case COLUMN_TYPE_INTEGER:
            {
                sqlite3_bind_int64(stmt, UpdateColumnIdx, pCurrentRow->ColumnValues[ColumnIdx].d_int64);
                break;
            }

            case COLUMN_TYPE_DECIMAL:
            {
                sqlite3_bind_double(stmt, UpdateColumnIdx, pCurrentRow->ColumnValues[ColumnIdx].d_double);
                break;
            }

            case COLUMN_TYPE_STRING:
            {
                /* maybe use SQLITE_STATIC will have good performance,
                 * BY Sunny at 2010-9-3 19:03:48.
                 */
                sqlite3_bind_text(stmt, UpdateColumnIdx, pCurrentRow->ColumnValues[ColumnIdx].d_string,
                                  pCurrentRow->ColumnSizes[ColumnIdx], SQLITE_TRANSIENT);
                break;
            }

            case COLUMN_TYPE_BINARY:
            {
                /* maybe use SQLITE_STATIC will have good performance,
                 * BY Sunny at 2010-9-3 19:04:01.
                 */
                sqlite3_bind_blob(stmt, UpdateColumnIdx, pCurrentRow->ColumnValues[ColumnIdx].d_string,
                                  pCurrentRow->ColumnSizes[ColumnIdx], SQLITE_TRANSIENT);
                break;
            }

            default:
            {
                /* invalid column data type to update */
                __wrn("update column data type error\n");
                break;
            }
            }
            /* update next column */
            UpdateColumnIdx++;
        }
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_OK && rc != SQLITE_DONE)
        {
            pConn->ErrorNumber  = sqlite3_errcode(pConn->Connection);
            __wrn("update sqlite3_step(\"%s\") failed\n", pConn->SQLBuf);

            /* just skip update failed row, update next row continue */
        }
        sqlite3_finalize(stmt);
    }
    /* commit updates */
    DBConn_CommitTransaction((HDBConn)pConn);

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                           CURSOR IS DIRTY OR NOT
*
* Description: check the cursor status is dirty or not.
*
* Arguments  : cursor    -The target query cursor handle.
*
* Returns    : 1 : cursor is dirty,
*              0 : cursor is not dirty, all columns is clear.
*
* Note       :
*********************************************************************************************************
*/
__bool DBCursor_CursorIsDirty(DBCursor_t *pCursor)
{
    int RowIdx;
    int ColumnIdx;

    for (RowIdx = 0; RowIdx < pCursor->ValidRows; RowIdx++)
    {
        for (ColumnIdx = 0; ColumnIdx < pCursor->ColumnNumber; ColumnIdx++)
        {
            if (pCursor->Rows[RowIdx]->ColumnStatus[ColumnIdx] == DB_COLUMN_DIRTY)
            {
                /* find dirty column */
                return EPDK_TRUE;
            }
        }
    }
    /* all columns of cursor are clear */
    return EPDK_FALSE;
}

/*
*********************************************************************************************************
*                                           ROW IS DIRTY OR NOT
*
* Description: check the row status is dirty or not.
*
* Arguments  : pRow         -The target row point.
*              ColumnNumber -The number of columns.
*
* Returns    : 1 : row is dirty,
*              0 : row is not dirty, all columns is clear.
*
* Note       :
*********************************************************************************************************
*/
__bool DBCursor_RowIsDirty(DBRow_t *pRow, int ColumnNumber)
{
    int ColumnIdx;

    for (ColumnIdx = 0; ColumnIdx < ColumnNumber; ColumnIdx++)
    {
        if (pRow->ColumnStatus[ColumnIdx] == DB_COLUMN_DIRTY)
        {
            /* find dirty column */
            return EPDK_TRUE;
        }
    }
    /* all columns of row are clear */
    return EPDK_FALSE;
}

/*
*********************************************************************************************************
*                                     GET DIRTY COLUMNS NUMBER OF A ROW
*
* Description: get the dirty columns number of special row.
*
* Arguments  : pRow         -The target row point.
*              ColumnNumber -The number of columns.
*
* Returns    : the dirty columns number.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_GetDirtyColumnsNumber(DBRow_t *pRow, int ColumnNumber)
{
    int ColumnIdx;
    int DirtyCnt = 0;

    for (ColumnIdx = 0; ColumnIdx < ColumnNumber; ColumnIdx++)
    {
        if (pRow->ColumnStatus[ColumnIdx] == DB_COLUMN_DIRTY)
        {
            /* dirty column */
            DirtyCnt++;
        }
    }
    return DirtyCnt;
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
int DBCursor_BuildUpdateString(char *pSQL, DBRow_t *pRow, DBCursor_t *pCursor)
{
    int DirtyColumns;
    int Len;
    int ColumnIdx;

    /* get dirty columns number */
    DirtyColumns = DBCursor_GetDirtyColumnsNumber(pRow, pCursor->ColumnNumber);
    if (DirtyColumns == 0)
    {
        /* no dirty column */
        return EPDK_FAIL;
    }

    /* build update SQL statement */
    Len = sprintf(pSQL, "UPDATE %s SET ", pCursor->TableName);
    for (ColumnIdx = 0; ColumnIdx < pCursor->ColumnNumber - 1; ColumnIdx++)
    {
        if (pRow->ColumnStatus[ColumnIdx] == DB_COLUMN_DIRTY)
        {
            /* check if the last dirty column */
            if (DirtyColumns > 1)
            {
                Len += sprintf(pSQL + Len, "%s=?, ", pCursor->ColumnNames[ColumnIdx]);
            }
            else
            {
                Len += sprintf(pSQL + Len, "%s=? ", pCursor->ColumnNames[ColumnIdx]);
            }
            DirtyColumns--;
        }
    }
    sprintf(pSQL + Len, "WHERE %s=%d;", pCursor->ColumnNames[pCursor->RowIdColumnIdx], \
            (int)(pRow->ColumnValues[pCursor->RowIdColumnIdx].d_int64));

    return EPDK_OK;
}
