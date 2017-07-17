/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_cursor.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-8-21
* Descript: dbms cursor, manage query reslut.
* Update  : date                auther      ver     notes
*           2010-8-21 13:29:25  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include    "dbms_i.h"

/*
*********************************************************************************************************
*                                      GET CONNECTION HANDLE
*
* Description: Get the connection handle, the specified cursor object belonging to.
*
* Arguments  : cursor -The target qurey cursor.
*
* Returns    : The connection belonging to the target query result. If an error occurs,
*              the return value is NULL.
*
* Note       :
*********************************************************************************************************
*/
HDBConn DBCursor_GetConnect(HDBCursor cursor)
{
    DBCursor_t *pCursor;

    if (cursor == NULL)
    {
        __wrn("invalid cursor handle for get connnection handle\n");
        return NULL;
    }
    pCursor = (DBCursor_t *)cursor;

    return (HDBConn)(pCursor->Connection);
}


/*
*********************************************************************************************************
*                                           CURSOR FREE
*
* Description: Frees the query cursor, releases internally stored variables.
*
* Arguments  : cursoe -The target query cursor handle.
*
* Returns    : EPDK_OK   -free query cursor succeeded.
*              EPDK_FAIL -an error occurs.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_Free(HDBCursor cursor)
{
    DBCursor_t *pCursor;

    if (cursor == NULL)
    {
        __wrn("invalid cursor handle for free\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    /* shutdown query process if cursor is work in step mode */
    if ((pCursor->Connection->Options[DBCONFIG_QUERY_MODE] == DBCONN_QUERY_STEP) \
            && (pCursor->Handle != NULL))
    {
        DBConn_ShutDownQueryProcess(cursor);
    }

    /* disattach cursor from connection */
    DBConn_DisattachCursor(pCursor->Connection, pCursor);

    /* destory cursor */
    DBCursor_Destory(pCursor);

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
DBCursor_t *DBCursor_Construct(DBConn_t *pConn, void *handle, int RowsNumber)
{
    DBCursor_t *pCursor;

    pCursor = DBMem_Alloc(sizeof(DBCursor_t));
    if (pCursor == NULL)
    {
        __wrn("allocate memory for DBCursor_t structure failed\n");
        return NULL;
    }

    /* allocate sem lock for cursor */
    pCursor->Lock = esKRNL_SemCreate(1);
    if (pCursor->Lock == NULL)
    {
        __wrn("allocate sem lock failed\n");
        DBMem_Free(pCursor);
        return NULL;
    }

    /* initialize cursor structure */
    pCursor->Connection = pConn;
    pCursor->Handle     = handle;
    pCursor->FinishFlag = 0;
    pCursor->TableName  = NULL;
    if (RowsNumber)
    {
        /* preallcote memory for rows array and rows state array */
        pCursor->Rows = DBMem_Calloc(RowsNumber, sizeof(DBRow_t *));
        pCursor->RowsNumber = RowsNumber;
    }
    else
    {
        /* preallcote memory for rows array and rows state array */
        pCursor->Rows = DBMem_Calloc(ROWS_GROW_STEP, sizeof(DBRow_t *));
        pCursor->RowsNumber = ROWS_GROW_STEP;
    }
    pCursor->ValidRows      = 0;
    pCursor->CurrentRowIdx  = 0;
    pCursor->ColumnNumber   = 0;
    pCursor->RowIdColumnIdx = -1;
    pCursor->ColumnNames    = NULL;
    pCursor->ColumnTypes    = NULL;
    pCursor->Next           = NULL;

    return pCursor;
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
int DBCursor_Destory(DBCursor_t *pCursor)
{
    int     RowIdx;
    int     ColumnIdx;
    __u8    err;

    /* free rows of cursor */
    for (RowIdx = 0; RowIdx < pCursor->ValidRows; RowIdx++)
    {
        if (pCursor->Rows[RowIdx] == NULL)
        {
            continue;
        }
        for (ColumnIdx = 0; ColumnIdx < pCursor->ColumnNumber; ColumnIdx++)
        {
            if ((pCursor->ColumnTypes[ColumnIdx] == COLUMN_TYPE_STRING)
                    && (pCursor->Rows[RowIdx]->ColumnValues[ColumnIdx].d_string))
            {
                DBMem_Free(pCursor->Rows[RowIdx]->ColumnValues[ColumnIdx].d_string);
            }

            if ((pCursor->ColumnTypes[ColumnIdx] == COLUMN_TYPE_BINARY)
                    && (pCursor->Rows[RowIdx]->ColumnValues[ColumnIdx].d_binary))
            {
                DBMem_Free(pCursor->Rows[RowIdx]->ColumnValues[ColumnIdx].d_binary);
            }
        }
        DBMem_Free(pCursor->Rows[RowIdx]->ColumnValues);
        DBMem_Free(pCursor->Rows[RowIdx]->ColumnSizes);
        DBMem_Free(pCursor->Rows[RowIdx]->ColumnStatus);
        DBMem_Free(pCursor->Rows[RowIdx]);
    }
    /* free columns of cursor */
    if (pCursor->ColumnNames)
    {
        for (ColumnIdx = 0; ColumnIdx < pCursor->ColumnNumber; ColumnIdx++)
        {
            DBMem_Free(pCursor->ColumnNames[ColumnIdx]);
        }
        DBMem_Free(pCursor->ColumnNames);
    }
    if (pCursor->ColumnTypes)
    {
        DBMem_Free(pCursor->ColumnTypes);
    }

    /* free cursor table name */
    if (pCursor->TableName)
    {
        DBMem_Free(pCursor->TableName);
    }

    /* free rows array */
    DBMem_Free(pCursor->Rows);

    /* release sem lock */
    esKRNL_SemDel(pCursor->Lock, OS_DEL_ALWAYS, &err);

    /* free cursor handle */
    DBMem_Free(pCursor);

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
int DBCursor_SetTableName(DBCursor_t *pCursor, const char *table)
{
    if (pCursor->TableName)
    {
        __wrn("cursor table name have set already\n");
        return EPDK_FAIL;
    }
    pCursor->TableName = DBMem_Strdup(table);
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
int DBCursor_Lock(DBCursor_t *pCursor)
{
    __u8 err;

    esKRNL_SemPend(pCursor->Lock, 0, &err);
    if(err != OS_NO_ERR)
    {
        __wrn("lock cursor [%x] failed!\n", pCursor);
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
int DBCursor_Unlock(DBCursor_t *pCursor)
{
    esKRNL_SemPost(pCursor->Lock);
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
int DBCursor_SetColumnsNumber(DBCursor_t *pCursor, int ColumnNumber)
{
    pCursor->ColumnNumber = ColumnNumber;
    if (ColumnNumber > 0)
    {
        pCursor->ColumnNames = DBMem_Calloc(ColumnNumber, sizeof(char *));
        if (pCursor->ColumnNames == NULL)
        {
            __wrn("allocate memory for cursor column name array failed\n");
            return EPDK_FAIL;
        }

        pCursor->ColumnTypes = DBMem_Calloc(ColumnNumber, sizeof(int));
        if (pCursor->ColumnTypes == NULL)
        {
            __wrn("allocate memory for cursor column type array failed\n");
            DBMem_Free(pCursor->ColumnNames);
            return EPDK_FAIL;
        }
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
int DBCursor_AddColumn(DBCursor_t *pCursor, int Idx, const char *Name, int Type)
{
    /* check column name add already or not */
    if (pCursor->ColumnNames[Idx])
    {
        return EPDK_FAIL;
    }
    pCursor->ColumnNames[Idx] = DBMem_Strdup(Name);
    pCursor->ColumnTypes[Idx] = Type;

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
DBRow_t *DBCursor_AllocateRow(int ColumnsNumber)
{
    DBRow_t *pRow;

    pRow = DBMem_Alloc(sizeof(DBRow_t));
    if (pRow)
    {
        /* allocate columns of row manage fields array */
        pRow->ColumnValues = DBMem_Calloc(ColumnsNumber, sizeof(DBData_t));
        pRow->ColumnSizes  = DBMem_Calloc(ColumnsNumber, sizeof(__size_t));
        pRow->ColumnStatus = DBMem_Alloc((ColumnsNumber * sizeof(int)));

        /* columns of row are all clear now */
        eLIBs_memset(pRow->ColumnStatus, DB_COLUMN_CLEAR, (ColumnsNumber * sizeof(int)));
    }
    return pRow;
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
void DBCursor_FreeRow(DBCursor_t *pCursor, DBRow_t *pRow)
{
    int Index;

    if (pRow)
    {
        for (Index = 0; Index < pCursor->ColumnNumber; Index++)
        {
            if ((pCursor->ColumnTypes[Index] == COLUMN_TYPE_STRING)
                    && (pRow->ColumnValues[Index].d_string))
            {
                DBMem_Free(pRow->ColumnValues[Index].d_string);
            }

            if ((pCursor->ColumnTypes[Index] == COLUMN_TYPE_BINARY)
                    && (pRow->ColumnValues[Index].d_binary))
            {
                DBMem_Free(pRow->ColumnValues[Index].d_binary);
            }
        }
        DBMem_Free(pRow->ColumnValues);
        DBMem_Free(pRow->ColumnSizes);
        DBMem_Free(pRow->ColumnStatus);
        DBMem_Free(pRow);
    }
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
int DBCursor_AddRow(DBCursor_t *pCursor, DBRow_t *pRow, int RowIdx)
{
    if ((pCursor == NULL) || (pRow == NULL) || (RowIdx < 0))
    {
        __wrn("invalid argument for add row\n");
        return EPDK_FAIL;
    }

    /* check row index have beyond cursor current rows number */
    if (RowIdx >= pCursor->RowsNumber)
    {
        int GrowNumber;

        /* grow rows number should aligned by ROWS_GROW_STEP */
        GrowNumber = DB_ALIGN((RowIdx + 1), ROWS_GROW_STEP);

        /* gorw cursor rows pointer array */
        pCursor->Rows = DBMem_Realloc(pCursor->Rows, \
                                      GrowNumber * sizeof(DBRow_t *));
        /* update rows number of cursor */
        pCursor->RowsNumber = GrowNumber;
    }
    pCursor->Rows[RowIdx] = pRow;

    /* increase one valid row */
    pCursor->ValidRows++;

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
int DBCursor_RemoveRow(DBCursor_t *pCursor, int RowIdx)
{
    int      Index;
    DBRow_t *pRemoveRow;

    if ((pCursor == NULL) || INVALID_ROW_INDEX(pCursor, RowIdx))
    {
        __wrn("invalid argument for remove row\n");
        return EPDK_FAIL;
    }

    /* remove associated datas of row */
    pRemoveRow = pCursor->Rows[RowIdx];
    for (Index = 0; Index < pCursor->ColumnNumber; Index++)
    {
        if ((pCursor->ColumnTypes[Index] == COLUMN_TYPE_STRING)
                && (pRemoveRow->ColumnValues[Index].d_string))
        {
            DBMem_Free(pRemoveRow->ColumnValues[Index].d_string);
        }

        if ((pCursor->ColumnTypes[Index] == COLUMN_TYPE_BINARY)
                && (pRemoveRow->ColumnValues[Index].d_binary))
        {
            DBMem_Free(pRemoveRow->ColumnValues[Index].d_binary);
        }
    }
    DBMem_Free(pRemoveRow->ColumnValues);
    DBMem_Free(pRemoveRow->ColumnSizes);
    DBMem_Free(pRemoveRow->ColumnStatus);
    DBMem_Free(pRemoveRow);

    /* move all rows pointer after removed row  */
    for(Index = RowIdx; Index < pCursor->ValidRows; Index++)
    {
        pCursor->Rows[Index] = pCursor->Rows[Index + 1];
    }

    /* decrease a valid row */
    pCursor->ValidRows--;

    if (pCursor->CurrentRowIdx >= pCursor->ValidRows)
    {
        /* current index beyond valid rows number,
         * adjust current index to the last row.
         */
        pCursor->CurrentRowIdx = pCursor->ValidRows - 1;
    }

    return EPDK_OK;
}
