/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_column.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-8-31
* Descript: dbms column level operations.
* Update  : date                auther      ver     notes
*           2010-8-31 19:29:17  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include    "dbms_i.h"

/*
*********************************************************************************************************
*                                      GET NUMBER OF COLUMNS
*
* Description: Get the number of columns in the query cursor.
*
* Arguments  : cursor -The target query cursor handle.
*
* Returns    : The number of columns in the query cursor, -1 if some things have error.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_GetColumnsNumber(HDBCursor cursor)
{
    DBCursor_t *pCursor;

    if (cursor == NULL)
    {
        __wrn("invalid cursor handle for get number of columns\n");
        return -1;
    }
    pCursor = (DBCursor_t *)cursor;

    /* the last ROWID column is hide for user */
    return (pCursor->ColumnNumber - 1);
}

/*
*********************************************************************************************************
*                                      GET COLUMN INDEX BY NAME
*
* Description: Get the zero-based index for the given column name, or -1 if the column doesn't exist.
*
* Arguments  : cursor    -The target query cursor handle.
*              olumnName -The name of the target column.
*
* Returns    : the zero-based column index for the given column name,
*              or -1 if the column name does not exist.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_GetColumnIdx(HDBCursor cursor, const char *columnName)
{
    int         ColumnIdx;
    DBCursor_t *pCursor;

    if (cursor == NULL || columnName == NULL)
    {
        __wrn("invalid argument for get column index\n");
        return -1;
    }
    pCursor = (DBCursor_t *)cursor;

    ColumnIdx = DBCurosr_FindColumn(pCursor, columnName);
    if (ColumnIdx == -1)
    {
        /* find target column failed */
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADNAME);
    }
    return ColumnIdx;
}

/*
*********************************************************************************************************
*                                      GET COLUMN NAME BY INDEX
*
* Description: Get the column name at the given zero-based column index.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*
* Returns    : The column name for the given column index,
*              NULL if the column index is invalid.
*
* Note       :
*********************************************************************************************************
*/
const char *DBCursor_GetColumnName(HDBCursor cursor, int columnIdx)
{
    DBCursor_t *pCursor;

    if (cursor == NULL)
    {
        __wrn("invalid cursor handle for get column name\n");
        return NULL;
    }
    pCursor = (DBCursor_t *)cursor;

    if (INVALID_COLUMN_INDEX(pCursor, columnIdx))
    {
        /* invalid column index */
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return NULL;
    }
    if ((pCursor->ColumnNames == NULL) || (pCursor->ColumnNames[columnIdx] == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return NULL;
    }

    return (const char *)(pCursor->ColumnNames[columnIdx]);
}

/*
*********************************************************************************************************
*                                GET COLUMN DATA TYPE BY COLUMN INDEX
*
* Description: Get the target column's data type. The constants returned by this function are defined
*              in dbms.h with the prefix "COLUMN_TYPE_".
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*
* Returns    : The target field's data type, or COLUMN_TYPE_ERROR in case of an error.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_GetColumnTypeByIdx(HDBCursor cursor, int columnIdx)
{
    DBCursor_t *pCursor;

    if (cursor == NULL)
    {
        __wrn("invalid argument for get column data type by column name\n");
        return COLUMN_TYPE_ERROR;
    }
    pCursor = (DBCursor_t *)cursor;

    if (INVALID_COLUMN_INDEX(pCursor, columnIdx))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return COLUMN_TYPE_ERROR;
    }
    if (pCursor->ColumnTypes == NULL)
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return COLUMN_TYPE_ERROR;
    }

    return pCursor->ColumnTypes[columnIdx];
}

/*
*********************************************************************************************************
*                                GET COLUMN DATA LENGTH BY COLUMN INDEX
*
* Description: Get the length of the value stored in the specified field which contains a string
*              or a binary data.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*
* Returns    : The length in bytes of the target column data, excluding the terminating zero byte,
*              or DB_LENGTH_ERROR in case of an error. The return value is 0 for column types other than
*              string or binary data.
*
* Note       :
*********************************************************************************************************
*/
__size_t DBCursor_GetColumnLengthByIdx(HDBCursor cursor, int columnIdx)
{
    DBCursor_t *pCursor;
    int         CurRowIdx;

    if (cursor == NULL)
    {
        __wrn("invalid argument for get column data length by column index\n");
        return DB_LENGTH_ERROR;
    }
    pCursor = (DBCursor_t *)cursor;

    if (INVALID_COLUMN_INDEX(pCursor, columnIdx))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return DB_LENGTH_ERROR;
    }

    if (pCursor->Rows == NULL)
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return DB_LENGTH_ERROR;
    }

    CurRowIdx = pCursor->CurrentRowIdx;
    if ((pCursor->Rows[CurRowIdx] == NULL) || (pCursor->Rows[CurRowIdx]->ColumnSizes == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADOBJECT);
        return DB_LENGTH_ERROR;
    }

    return (__size_t)(pCursor->Rows[CurRowIdx]->ColumnSizes[columnIdx]);
}

/*
*********************************************************************************************************
*                                   GET COLUMN DATA AS INT BY COLUMN INDEX
*
* Description: Get the data stored in the speficied column, which contains an signed integer.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*
* Returns    : The value of that column as an int, or 0 (zero) if an error occurs.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_GetIntByIdx(HDBCursor cursor, int columnIdx)
{
    return (int)DBCursor_GetInt64ByIdx(cursor, columnIdx);
}

/*
*********************************************************************************************************
*                                   GET COLUMN DATA AS INT64 BY COLUMN INDEX
*
* Description: Get the data stored in the speficied column, which contains a signed int64 integer.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*
* Returns    : The value of that column as a int64, or 0 (zero) if an error occurs.
*
* Note       :
*********************************************************************************************************
*/
__s64 DBCursor_GetInt64ByIdx(HDBCursor cursor, int columnIdx)
{
    DBCursor_t *pCursor;
    int         CurRowIdx;

    if (cursor == NULL)
    {
        __wrn("invalid argument for get column data as int64 by column index\n");
        return 0;
    }
    pCursor = (DBCursor_t *)cursor;

    if (INVALID_COLUMN_INDEX(pCursor, columnIdx))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return 0;
    }
    if ((pCursor->Rows == NULL) || (pCursor->ColumnTypes == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return 0;
    }

    if (pCursor->ColumnTypes[columnIdx] != COLUMN_TYPE_INTEGER)
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADTYPE);
        return 0;
    }

    CurRowIdx = pCursor->CurrentRowIdx;
    if ((pCursor->Rows[CurRowIdx] == NULL) || (pCursor->Rows[CurRowIdx]->ColumnValues == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADOBJECT);
        return 0;
    }

    return (pCursor->Rows[CurRowIdx]->ColumnValues[columnIdx].d_int64);
}

/*
*********************************************************************************************************
*                                   GET COLUMN DATA AS DOUBLE BY COLUMN INDEX
*
* Description: Get the data stored in the speficied column,
*              which contains a double-precision decimal number.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*
* Returns    : The value of that column as a double, or 0 (zero) if an error occurs.
*
* Note       :
*********************************************************************************************************
*/
double DBCursor_GetDoubleByIdx(HDBCursor cursor, int columnIdx)
{
    DBCursor_t *pCursor;
    int         CurRowIdx;

    if (cursor == NULL)
    {
        __wrn("invalid argument for get column data as double by column index\n");
        return 0.0;
    }
    pCursor = (DBCursor_t *)cursor;

    if (INVALID_COLUMN_INDEX(pCursor, columnIdx))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return 0.0;
    }
    if ((pCursor->Rows == NULL) || (pCursor->ColumnTypes == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return 0.0;
    }

    if (pCursor->ColumnTypes[columnIdx] != COLUMN_TYPE_DECIMAL)
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADTYPE);
        return 0.0;
    }

    CurRowIdx = pCursor->CurrentRowIdx;
    if ((pCursor->Rows[CurRowIdx] == NULL) || (pCursor->Rows[CurRowIdx]->ColumnValues == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADOBJECT);
        return 0.0;
    }

    return (pCursor->Rows[CurRowIdx]->ColumnValues[columnIdx].d_double);
}

/*
*********************************************************************************************************
*                                   GET COLUMN DATA AS STRING BY COLUMN INDEX
*
* Description: Get the data stored in the speficied column, which contains a zero-terminated string.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*
* Returns    : The value of that column as a string, or NULL if an error occurs.
*
* Note       :
*********************************************************************************************************
*/
const char *DBCursor_GetStringByIdx(HDBCursor cursor, int columnIdx)
{
    DBCursor_t *pCursor;
    int         CurRowIdx;

    if (cursor == NULL)
    {
        __wrn("invalid argument for get column data as string by column index\n");
        return NULL;
    }
    pCursor = (DBCursor_t *)cursor;

    if (INVALID_COLUMN_INDEX(pCursor, columnIdx))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return NULL;
    }
    if ((pCursor->Rows == NULL) || (pCursor->ColumnTypes == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return NULL;
    }

    if (pCursor->ColumnTypes[columnIdx] != COLUMN_TYPE_STRING)
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADTYPE);
        return NULL;
    }

    CurRowIdx = pCursor->CurrentRowIdx;
    if ((pCursor->Rows[CurRowIdx] == NULL) || (pCursor->Rows[CurRowIdx]->ColumnValues == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADOBJECT);
        return NULL;
    }

    return (const char *)(pCursor->Rows[CurRowIdx]->ColumnValues[columnIdx].d_string);
}

/*
*********************************************************************************************************
*                                   GET COLUMN DATA AS BINARY BY COLUMN INDEX
*
* Description: Get the data stored in the speficied column, which contains binary data.
*
* Arguments  : cursor    -The target query cursor handle.
*              columnIdx -The zero-based index of the target column.
*
* Returns    : The value of that column as binary data, or NULL if an error occurs.
*
* Note       : The binary data may contain zero bytes and non-printable characters.
*              Use DBCursor_GetColumnLength or DBCursor_GetColumnLengthByIdx() to determine the number of bytes
*              contained in the resulting binary data.
*********************************************************************************************************
*/
const void *DBCursor_GetBinaryByIdx(HDBCursor cursor, int columnIdx)
{
    DBCursor_t *pCursor;
    int         CurRowIdx;

    if (cursor == NULL)
    {
        __wrn("invalid argument for get column data as binary data by column index\n");
        return NULL;
    }
    pCursor = (DBCursor_t *)cursor;

    if (INVALID_COLUMN_INDEX(pCursor, columnIdx))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return NULL;
    }
    if ((pCursor->Rows == NULL) || (pCursor->ColumnTypes == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return NULL;
    }

    if (pCursor->ColumnTypes[columnIdx] != COLUMN_TYPE_BINARY)
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADTYPE);
        return NULL;
    }

    CurRowIdx = pCursor->CurrentRowIdx;
    if ((pCursor->Rows[CurRowIdx] == NULL) || (pCursor->Rows[CurRowIdx]->ColumnValues == NULL))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADOBJECT);
        return NULL;
    }

    return (const void *)(pCursor->Rows[CurRowIdx]->ColumnValues[columnIdx].d_binary);
}

/*
*********************************************************************************************************
*                                GET COLUMN DATA TYPE BY COLUMN NAME
*
* Description: Get the target column's data type. The constants returned by this function are defined
*              in dbms.h with the prefix "COLUMN_TYPE_".
*
* Arguments  : cursor    -The target query cursor handle.
*              olumnName -The name of the target column.
*
* Returns    : The target field's data type, or COLUMN_TYPE_ERROR in case of an error.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_GetColumnType(HDBCursor cursor, const char *columnName)
{
    int         ColumnIdx;
    DBCursor_t *pCursor;

    if (cursor == NULL || columnName == NULL)
    {
        __wrn("invalid argument for get column data type\n");
        return COLUMN_TYPE_ERROR;
    }
    pCursor = (DBCursor_t *)cursor;

    ColumnIdx = DBCurosr_FindColumn(pCursor, columnName);
    if (ColumnIdx == -1)
    {
        /* find target column failed */
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADNAME);
        return COLUMN_TYPE_ERROR;
    }
    /* get column data type by index */
    return DBCursor_GetColumnTypeByIdx(cursor, ColumnIdx);
}

/*
*********************************************************************************************************
*                                GET COLUMN DATA LENGTH BY COLUMN NAME
*
* Description: Get the length of the value stored in the specified field which contains a string
*              or a binary data.
*
* Arguments  : cursor    -The target query cursor handle.
*              olumnName -The name of the target column.
*
* Returns    : The length in bytes of the target column data, excluding the terminating zero byte,
*              or DB_LENGTH_ERROR in case of an error. The return value is 0 for column types other than
*              string or binary data.
*
* Note       :
*********************************************************************************************************
*/
__size_t DBCursor_GetColumnLength(HDBCursor cursor, const char *columnName)
{
    int         ColumnIdx;
    DBCursor_t *pCursor;

    if (cursor == NULL || columnName == NULL)
    {
        __wrn("invalid argument for get column data length\n");
        return DB_LENGTH_ERROR;
    }
    pCursor = (DBCursor_t *)cursor;

    ColumnIdx = DBCurosr_FindColumn(pCursor, columnName);
    if (ColumnIdx == -1)
    {
        /* find target column failed */
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADNAME);
        return DB_LENGTH_ERROR;
    }
    /* get column length by column index */
    return DBCursor_GetColumnLengthByIdx(cursor, ColumnIdx);
}

/*
*********************************************************************************************************
*                                   GET COLUMN DATA AS INT BY COLUMN NAME
*
* Description: Get the data stored in the speficied column, which contains an signed integer.
*
* Arguments  : cursor     -The target query cursor handle.
*              columnName -The name of the target column.
*
* Returns    : The value of that column as an int, or 0 (zero) if an error occurs.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_GetInt(HDBCursor cursor, const char *columnName)
{
    return (int)DBCursor_GetInt64(cursor, columnName);
}

/*
*********************************************************************************************************
*                                   GET COLUMN DATA AS INT64 BY COLUMN NAME
*
* Description: Get the data stored in the speficied column, which contains a signed int64 integer.
*
* Arguments  : cursor     -The target query cursor handle.
*              columnName -The name of the target column.
*
* Returns    : The value of that column as a int64, or 0 (zero) if an error occurs.
*
* Note       :
*********************************************************************************************************
*/
__s64 DBCursor_GetInt64(HDBCursor cursor, const char *columnName)
{
    int         ColumnIdx;
    DBCursor_t *pCursor;

    if (cursor == NULL || columnName == NULL)
    {
        __wrn("invalid argument for get column data as int64 by column name\n");
        return 0;
    }
    pCursor = (DBCursor_t *)cursor;

    ColumnIdx = DBCurosr_FindColumn(pCursor, columnName);
    if (ColumnIdx == -1)
    {
        /* find target column failed */
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADNAME);
        return 0;
    }
    /* get column data as int64 by column index */
    return DBCursor_GetInt64ByIdx(cursor, ColumnIdx);
}

/*
*********************************************************************************************************
*                                   GET COLUMN DATA AS DOUBLE BY COLUMN NAME
*
* Description: Get the data stored in the speficied column,
*              which contains a double-precision decimal number.
*
* Arguments  : cursor     -The target query cursor handle.
*              columnName -The name of the target column.
*
* Returns    : The value of that column as a double, or 0 (zero) if an error occurs.
*
* Note       :
*********************************************************************************************************
*/
double DBCursor_GetDouble(HDBCursor cursor, const char *columnName)
{
    int         ColumnIdx;
    DBCursor_t *pCursor;

    if (cursor == NULL || columnName == NULL)
    {
        __wrn("invalid argument for get column data as double by column name\n");
        return 0.0;
    }
    pCursor = (DBCursor_t *)cursor;

    ColumnIdx = DBCurosr_FindColumn(pCursor, columnName);
    if (ColumnIdx == -1)
    {
        /* find target column failed */
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADNAME);
        return 0.0;
    }
    /* get column data as double by column index */
    return DBCursor_GetDoubleByIdx(cursor, ColumnIdx);
}

/*
*********************************************************************************************************
*                                   GET COLUMN DATA AS STRING BY COLUMN NAME
*
* Description: Get the data stored in the speficied column, which contains a zero-terminated string.
*
* Arguments  : cursor     -The target query cursor handle.
*              columnName -The name of the target column.
*
* Returns    : The value of that column as a string, or NULL if an error occurs.
*
* Note       :
*********************************************************************************************************
*/
const char *DBCursor_GetString(HDBCursor cursor, const char *columnName)
{
    int         ColumnIdx;
    DBCursor_t *pCursor;

    if (cursor == NULL || columnName == NULL)
    {
        __wrn("invalid argument for get column data as string by column name\n");
        return NULL;
    }
    pCursor = (DBCursor_t *)cursor;

    ColumnIdx = DBCurosr_FindColumn(pCursor, columnName);
    if (ColumnIdx == -1)
    {
        /* find target column failed */
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADNAME);
        return NULL;
    }
    /* get column data as string by column index */
    return DBCursor_GetStringByIdx(cursor, ColumnIdx);
}

/*
*********************************************************************************************************
*                       GET COLUMN DATA AS BINARY BY COLUMN NAME
*
* Description: Get the data stored in the speficied column, which contains binary data.
*
* Arguments  : cursor     -The target query cursor handle.
*              columnName -The name of the target column.
*
* Returns    : The value of that column as binary data, or NULL if an error occurs.
*
* Note       : The binary data may contain zero bytes and non-printable characters.
*              Use DBCursor_GetColumnLength or DBCursor_GetColumnLengthByIdx() to determine the number of bytes
*              contained in the resulting binary data.
*********************************************************************************************************
*/
const void *DBCursor_GetBinary(HDBCursor cursor, const char *columnName)
{
    int         ColumnIdx;
    DBCursor_t *pCursor;

    if (cursor == NULL || columnName == NULL)
    {
        __wrn("invalid argument for get column data as binary by column name\n");
        return NULL;
    }
    pCursor = (DBCursor_t *)cursor;

    ColumnIdx = DBCurosr_FindColumn(pCursor, columnName);
    if (ColumnIdx == -1)
    {
        /* find target column failed */
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADNAME);
        return NULL;
    }
    /* get column data as binary by column index */
    return DBCursor_GetBinaryByIdx(cursor, ColumnIdx);
}


/*
*********************************************************************************************************
*                                FIND COLUMN INDEX BY COLUMN NAME
*
* Description: Find the column index by column name.
*
* Arguments  : cursor     -The target query cursor handle.
*              columnName -The name of the target column.
*
* Returns    : the index of column, or -1 if an error occurs.
*
* Note       :
*********************************************************************************************************
*/
int DBCurosr_FindColumn(DBCursor_t *pCursor, const char *ColumnName)
{
    int ColumnIdx;
    if (pCursor->ColumnNames == NULL)
    {
        /* invalid columns name array */
        return -1;
    }

    ColumnIdx = 0;
    while (ColumnIdx < (pCursor->ColumnNumber - 1))
    {
        if (eLIBs_stricmp(pCursor->ColumnNames[ColumnIdx], ColumnName) == 0)
        {
            return ColumnIdx;
        }
        ColumnIdx++;
    }
    __wrn("cursor row has no column: '%s'\n", ColumnName);
    return -1;
}
