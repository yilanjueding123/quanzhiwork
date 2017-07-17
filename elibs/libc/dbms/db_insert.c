/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_insert.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-9-2
* Descript: dbms insert operations implement.
* Update  : date                auther      ver     notes
*           2010-9-2 19:38:19   Sunny       1.0     create this file.
*********************************************************************************************************
*/

#include    "dbms_i.h"

/*
*********************************************************************************************************
*                                           INSERT ONE ROW
*
* Description: insert one row.
*
* Arguments  : cursor -The target query cursor handle,
*              format -Format-control string,
*              ...    -Optional arguments.
*
* Returns    : the zero-base index of inserted row, -1 if error occurs.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_InsertRow(HDBCursor cursor, const char *format, ...)
{
    DBCursor_t *pCursor;
    DBRow_t    *pNewRow;
    int         rc;
    va_list     args;
    int         rowIdx;

    if ((cursor == NULL) || (format == NULL))
    {
        __wrn("invalid handle for insert row\n");
        return -1;
    }
    pCursor = (DBCursor_t *)cursor;

    /* allocate a new row */
    pNewRow = DBCursor_AllocateRow(pCursor->ColumnNumber);
    if (pNewRow == NULL)
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_NOMEM);
        return -1;
    }
    va_start(args, format);
    rc = DBCursor_PackColumns(pCursor, pNewRow, format, args);
    va_end(args);
    if (rc != EPDK_OK)
    {
        /* free allocate row */
        DBCursor_FreeRow(pCursor, pNewRow);
        return rc;
    }
    /* insert to the end of cursor */
    rowIdx = pCursor->ValidRows;

    /* add new row to end of cursor */
    DBCursor_AddRow(pCursor, pNewRow, rowIdx);
    return rowIdx;
}

/*
*********************************************************************************************************
*                                           BIND INT TO COLUMN
*
* Description: Bind the specified variable to the specified column, which holds an integer type data.
*
* Arguments  : cursor    -The target query cursor handle.
*              rowIdx    -The zero-based index of the target row, return by DBCursor_InsertRow().
*              columnIdx -The zero-based index of the target column.
*              value     -The binding value.
*
* Returns    : EPDK_OK   -binding column value succeeded.
*              EPDK_FAIL -an error occurs.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_BindInt(HDBCursor cursor, int rowIdx, int columnIdx, int value)
{
    return DBCursor_BindInt64(cursor, rowIdx, columnIdx, (__s64)value);
}

/*
*********************************************************************************************************
*                                           BIND INT64 TO COLUMN
*
* Description: Bind the specified variable to the specified column, which holds an integer type data.
*
* Arguments  : cursor    -The target query cursor handle.
*              rowIdx    -The zero-based index of the target row, return by DBCursor_InsertRow().
*              columnIdx -The zero-based index of the target column.
*              value     -The binding value.
*
* Returns    : EPDK_OK   -binding column value succeeded.
*              EPDK_FAIL -an error occurs.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_BindInt64(HDBCursor cursor, int rowIdx, int columnIdx, __s64 value)
{
    DBCursor_t *pCursor;

    if (cursor == NULL)
    {
        __wrn("invalid handle for bind int64\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    if (INVALID_ROW_INDEX(pCursor, rowIdx) || INVALID_COLUMN_INDEX(pCursor, columnIdx))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return EPDK_FAIL;
    }

    /* check column data type match or not */
    if (pCursor->ColumnTypes[columnIdx] != COLUMN_TYPE_INTEGER)
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADTYPE);
        return EPDK_FAIL;
    }

    if ((pCursor->Rows == NULL) || (pCursor->Rows[rowIdx] == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return EPDK_FAIL;
    }

    /* bind int64 value to column of row */
    pCursor->Rows[rowIdx]->ColumnValues[columnIdx].d_int64 = value;
    pCursor->Rows[rowIdx]->ColumnStatus[columnIdx] = DB_COLUMN_NEW;
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                           BIND DOUBLE TO COLUMN
*
* Description: Bind the specified variable to the specified column, which holds an integer type data.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*              value     -The binding value.
*
* Returns    : EPDK_OK   -binding column value succeeded.
*              EPDK_FAIL -an error occurs.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_BindDouble(HDBCursor cursor, int rowIdx, int columnIdx, double value)
{
    DBCursor_t *pCursor;

    if (cursor == NULL)
    {
        __wrn("invalid handle for bind double\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    if (INVALID_ROW_INDEX(pCursor, rowIdx) || INVALID_COLUMN_INDEX(pCursor, columnIdx))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return EPDK_FAIL;
    }

    /* check column data type match or not */
    if (pCursor->ColumnTypes[columnIdx] != COLUMN_TYPE_DECIMAL)
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADTYPE);
        return EPDK_FAIL;
    }

    if ((pCursor->Rows == NULL) || (pCursor->Rows[rowIdx] == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return EPDK_FAIL;
    }

    /* bind double value to column of row */
    pCursor->Rows[rowIdx]->ColumnValues[columnIdx].d_double = value;
    pCursor->Rows[rowIdx]->ColumnStatus[columnIdx] = DB_COLUMN_NEW;
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                           BIND STRING TO COLUMN
*
* Description: Bind the specified variable to the specified column, which holds an integer type data.
*
* Arguments  : cursor    -The target query cursor handle.
*              rowIdx    -The zero-based index of the target row, return by DBCursor_InsertRow().
*              columnIdx -The zero-based index of the target column.
*              value     -The binding value.
*
* Returns    : EPDK_OK   -binding column value succeeded.
*              EPDK_FAIL -an error occurs.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_BindString(HDBCursor cursor, int rowIdx, int columnIdx, const char *string)
{
    DBCursor_t *pCursor;
    char       *pColumnString;
    __size_t    Len;

    if ((cursor == NULL) || (string == NULL))
    {
        __wrn("invalid handle for bind string\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    if (INVALID_ROW_INDEX(pCursor, rowIdx) || INVALID_COLUMN_INDEX(pCursor, columnIdx))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return EPDK_FAIL;
    }

    /* check column data type match or not */
    if (pCursor->ColumnTypes[columnIdx] != COLUMN_TYPE_STRING)
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADTYPE);
        return EPDK_FAIL;
    }

    if ((pCursor->Rows == NULL) || (pCursor->Rows[rowIdx] == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return EPDK_FAIL;
    }

    /* bind string to column of row */
    Len = eLIBs_strlen(string) + 1;
    pColumnString = pCursor->Rows[rowIdx]->ColumnValues[columnIdx].d_string;
    if (pColumnString == NULL)
    {
        pColumnString = DBMem_Strdup(string);
    }
    else
    {
        if (Len > pCursor->Rows[rowIdx]->ColumnSizes[columnIdx])
        {
            /* string buffer not enough, should reallocate more memory space. */
            DBMem_Free(pColumnString);
            pColumnString = DBMem_Strdup(string);
        }
        else
        {
            /* overlay old string value directly */
            eLIBs_strcpy(pColumnString, string);
        }
    }
    /* reset string length */
    pCursor->Rows[rowIdx]->ColumnSizes[columnIdx] = Len;
    pCursor->Rows[rowIdx]->ColumnStatus[columnIdx] = DB_COLUMN_NEW;
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                           BIND BINARY TO COLUMN
*
* Description: Bind the specified variable to the specified column, which holds an integer type data.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*              value     -The binding value.
*
* Returns    : EPDK_OK   -binding column value succeeded.
*              EPDK_FAIL -an error occurs.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_BindBinary(HDBCursor cursor, int rowIdx, int columnIdx, const void *binary, __size_t size)
{
    DBCursor_t *pCursor;
    char       *pColumnBinary;

    if ((cursor == NULL) || (binary == NULL))
    {
        __wrn("invalid handle for bind binary\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    if (INVALID_ROW_INDEX(pCursor, rowIdx) || INVALID_COLUMN_INDEX(pCursor, columnIdx))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return EPDK_FAIL;
    }

    /* check column data type match or not */
    if (pCursor->ColumnTypes[columnIdx] != COLUMN_TYPE_BINARY)
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADTYPE);
        return EPDK_FAIL;
    }

    if ((pCursor->Rows == NULL) || (pCursor->Rows[rowIdx] == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return EPDK_FAIL;
    }

    /* check column binary buffer is enough to store user binary data or not */
    pColumnBinary = pCursor->Rows[rowIdx]->ColumnValues[columnIdx].d_binary;
    if (pColumnBinary == NULL)
    {
        pColumnBinary = DBMem_Alloc(size);
        if (pColumnBinary == NULL)
        {
            DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_NOMEM);
            return EPDK_FAIL;
        }
    }
    else
    {
        if (size > pCursor->Rows[rowIdx]->ColumnSizes[columnIdx])
        {
            /* string buffer not enough, should reallocate more memory space. */
            DBMem_Free(pColumnBinary);
            pColumnBinary = DBMem_Alloc(size);
            if (pColumnBinary == NULL)
            {
                DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_NOMEM);
                return EPDK_FAIL;
            }
        }
    }
    /* copy binary data to row binary buffer,
     * maybe can use user buffer, can decrease memory and good performance.
     * BY Sunny at 2010-9-3 18:22:49.
     */
    eLIBs_memcpy(pColumnBinary, binary, size);

    /* reset string length */
    pCursor->Rows[rowIdx]->ColumnSizes[columnIdx] = size;

    pCursor->Rows[rowIdx]->ColumnStatus[columnIdx] = DB_COLUMN_NEW;
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                           COMMIT INSERTED ROW
*
* Description: Bind the specified variable to the specified column, which holds an integer type data.
*
* Arguments  : cursor    -The target query cursor handle.
*              rowIdx    -The zero-based index of the target row, return by DBCursor_InsertRow().
*              columnIdx -The zero-based index of the target column.
*              value     -The binding value.
*
* Returns    : EPDK_OK   -binding column value succeeded.
*              EPDK_FAIL -an error occurs.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_CommitInsert(HDBCursor cursor, int rowIdx)
{
    DBConn_t   *pConn;
    DBCursor_t *pCursor;
    DBRow_t    *pInsertRow;
    sqlite3_stmt *stmt;
    int         rc;
    int			Len;
    int			ColumnIdx;
    const char *tail;

    if (cursor == NULL)
    {
        __wrn("invalid handle for commit insert\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    if (INVALID_ROW_INDEX(pCursor, rowIdx))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return EPDK_FAIL;
    }

    if ((pCursor->Rows == NULL) || (pCursor->Rows[rowIdx] == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return EPDK_FAIL;
    }
    pInsertRow = pCursor->Rows[rowIdx];

    /* check row status */
    if (DBCursor_RowIsNew(pInsertRow, pCursor->ColumnNumber) == EPDK_FALSE)
    {
        __wrn("insert row is not new\n");
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_UNSUPPORTED);
        return EPDK_FAIL;
    }

    /* wapper SQL statement */
    pConn = pCursor->Connection;
    Len = sprintf(pConn->SQLBuf, "INSERT INTO %s VALUES(", pCursor->TableName);
    for (ColumnIdx = 0; ColumnIdx < (pCursor->ColumnNumber - 2); ColumnIdx++)
    {
        Len += sprintf(pConn->SQLBuf + Len, "?, ");
    }
    sprintf(pConn->SQLBuf + Len, " ?);");

    /* compile the insert statement into VM code */
    sqlite3_prepare_v2(pConn->Connection, pConn->SQLBuf, -1, &stmt, &tail);

    /* bind insert value to prepared statement, the last column is ROWID, no need to binding. */
    for (ColumnIdx = 0; ColumnIdx < (pCursor->ColumnNumber - 1); ColumnIdx++)
    {
        /* the base index of SQLite column binding is 1 */
        int  SQLiteColumnIdx = ColumnIdx + 1;

        if (pInsertRow->ColumnStatus[ColumnIdx] != DB_COLUMN_NEW)
        {
            /* just insert new columns */
            continue;
        }

        switch (pCursor->ColumnTypes[ColumnIdx])
        {
        case COLUMN_TYPE_INTEGER:
        {
            sqlite3_bind_int64(stmt, SQLiteColumnIdx, pInsertRow->ColumnValues[ColumnIdx].d_int64);
            break;
        }

        case COLUMN_TYPE_DECIMAL:
        {
            sqlite3_bind_double(stmt, SQLiteColumnIdx, pInsertRow->ColumnValues[ColumnIdx].d_double);
            break;
        }

        case COLUMN_TYPE_STRING:
        {
            /* maybe use SQLITE_STATIC will have good performance,
             * BY Sunny at 2010-9-3 19:03:11.
             */
            sqlite3_bind_text(stmt, SQLiteColumnIdx, pInsertRow->ColumnValues[ColumnIdx].d_string,
                              pInsertRow->ColumnSizes[ColumnIdx], SQLITE_TRANSIENT);
            break;
        }

        case COLUMN_TYPE_BINARY:
        {
            /* maybe use SQLITE_STATIC will have good performance,
             * BY Sunny at 2010-9-3 19:03:25.
             */
            sqlite3_bind_blob(stmt, SQLiteColumnIdx, pInsertRow->ColumnValues[ColumnIdx].d_string,
                              pInsertRow->ColumnSizes[ColumnIdx], SQLITE_TRANSIENT);
            break;
        }

        default:
        {
            /* invalid column data type to update */
            __wrn("insert column data type error\n");
            break;
        }
        }
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_OK && rc != SQLITE_DONE)
    {
        pConn->ErrorNumber  = sqlite3_errcode(pConn->Connection);
        __wrn("insert sqlite3_step(\"%s\") failed\n", pConn->SQLBuf);
        sqlite3_finalize(stmt);
        return EPDK_FAIL;
    }

    sqlite3_finalize(stmt);

    /* update ROWID of inserted row */
    pInsertRow->ColumnValues[pCursor->RowIdColumnIdx].d_int64 = \
            (int)sqlite3_last_insert_rowid(pCursor->Connection->Connection);

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
int DBCursor_PackColumns(DBCursor_t *pCursor, DBRow_t *pRow, const char *format, va_list args)
{
    int ColumnIdx;

    /* pack columns value to row */
    ColumnIdx = 0;
    while(*format)
    {
        if(*format == '%')
        {
            ++format;
            switch(*format)
            {
            case 'd':
            {
                if (pCursor->ColumnTypes[ColumnIdx] != COLUMN_TYPE_INTEGER)
                {
                    /* insert column data type unmatch with column of cursor */
                    DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADTYPE);
                    return EPDK_FAIL;
                }
                pRow->ColumnValues[ColumnIdx].d_int64 = va_arg(args, int);
                pRow->ColumnStatus[ColumnIdx] = DB_COLUMN_NEW;
                ColumnIdx++;
                break;
            }

            case 's':
            {
                if (pCursor->ColumnTypes[ColumnIdx] != COLUMN_TYPE_STRING)
                {
                    /* insert column data type unmatch with column of cursor */
                    DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADTYPE);
                    return EPDK_FAIL;
                }
                pRow->ColumnValues[ColumnIdx].d_string = DBMem_Strdup(va_arg(args, char *));
                pRow->ColumnSizes[ColumnIdx] = eLIBs_strlen(pRow->ColumnValues[ColumnIdx].d_string) + 1;
                pRow->ColumnStatus[ColumnIdx] = DB_COLUMN_NEW;
                ColumnIdx++;
                break;
            }

            case 'f':
            {
                if (pCursor->ColumnTypes[ColumnIdx] != COLUMN_TYPE_DECIMAL)
                {
                    /* insert column data type unmatch with column of cursor */
                    DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADTYPE);
                    return EPDK_FAIL;
                }
                pRow->ColumnValues[ColumnIdx].d_double = va_arg(args, double);
                pRow->ColumnStatus[ColumnIdx] = DB_COLUMN_NEW;
                ColumnIdx++;
                break;
            }

            case '?':
            {
                /* column data value unkown now */
                ColumnIdx++;
                break;
            }

            case 'l':
            {
                ++format;
                if (*format == 'l')
                {
                    /* long long(int64) value */
                    if (pCursor->ColumnTypes[ColumnIdx] != COLUMN_TYPE_INTEGER)
                    {
                        /* insert column data type unmatch with column of cursor */
                        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADTYPE);
                        return EPDK_FAIL;
                    }
                    pRow->ColumnValues[ColumnIdx].d_int64 = va_arg(args, __int64);
                    pRow->ColumnStatus[ColumnIdx] = DB_COLUMN_NEW;
                    ColumnIdx++;
                }
                else
                {
                    /* unsupport format */
                    break;
                }
            }

            default :
            {
                /* unsupport format */
                break;
            }
            }
            ++format;
        }
        else
        {
            *format++;
        }
    }
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                           ROW IS NEW OR NOT
*
* Description: check the row status is new or not.
*
* Arguments  : pRow         -The target row point.
*              ColumnNumber -The number of columns.
*
* Returns    : 1 : row is new,
*              0 : row is not new.
*
* Note       :
*********************************************************************************************************
*/
__bool DBCursor_RowIsNew(DBRow_t *pRow, int ColumnNumber)
{
    int ColumnIdx;

    for (ColumnIdx = 0; ColumnIdx < ColumnNumber; ColumnIdx++)
    {
        if (pRow->ColumnStatus[ColumnIdx] == DB_COLUMN_NEW)
        {
            /* find dirty column */
            return EPDK_TRUE;
        }
    }
    /* all columns of row are clear */
    return EPDK_FALSE;
}
