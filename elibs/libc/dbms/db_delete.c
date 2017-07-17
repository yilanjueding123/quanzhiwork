/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_delete.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-9-2
* Descript: dbms delete operations implement.
* Update  : date                auther      ver     notes
*           2010-9-2 16:35:34   Sunny       1.0     create this file.
*********************************************************************************************************
*/

#include    "dbms_i.h"


/*
*********************************************************************************************************
*                                       DELETE CURRENT ROW
*
* Description: delete current row.
*
* Arguments  : cursor -The target query cursor handle.
*
* Returns    : EPDK_OK   -delete row succeeded.
*              EPDK_FAIL -an error occurs.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_DeleteCurrentRow(HDBCursor cursor)
{
    DBCursor_t *pCursor;

    if (cursor == NULL)
    {
        __wrn("invalid argument for delete current row\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    return  DBCursor_DeleteRowByIdx(cursor, pCursor->CurrentRowIdx);
}

/*
*********************************************************************************************************
*                                     DELETE ROW BY ROW INDEX
*
* Description: delete row by index.
*
* Arguments  : cursor -The target query cursor handle.
*              index  -The zero-based index of the delete row.
*
* Returns    : EPDK_OK   -delete row succeeded.
*              EPDK_FAIL -an error occurs.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_DeleteRowByIdx(HDBCursor cursor, int rowIdx)
{
    DBCursor_t *pCursor;
    DBConn_t   *pConn;
    DBRow_t    *pVictimRow;
    int         rc;

    if (cursor == NULL)
    {
        __wrn("invalid handle for delete row by index\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    if (INVALID_ROW_INDEX(pCursor, rowIdx))
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADIDX);
        return EPDK_FAIL;
    }

    if (pCursor->Rows == NULL || pCursor->Rows[rowIdx] == NULL)
    {
        DBConn_SetErrorNumber(pCursor->Connection, DB_ERROR_BADPTR);
        return EPDK_FAIL;
    }
    pVictimRow = pCursor->Rows[rowIdx];

    /* delete row in backing store */
    pConn = pCursor->Connection;
    sprintf(pConn->SQLBuf, "DELETE FROM %s WHERE %s=%d;",
            pCursor->TableName,
            pCursor->ColumnNames[pCursor->RowIdColumnIdx],
            (int)(pVictimRow->ColumnValues[pCursor->RowIdColumnIdx].d_int64));
    rc = sqlite3_exec(pConn->Connection, pConn->SQLBuf, NULL, NULL, NULL);
    if (rc != SQLITE_OK)
    {
        pConn->ErrorNumber  = sqlite3_errcode(pConn->Connection);
        __wrn("delete row [%d] failed, error message : [%s]\n",
              rowIdx, sqlite3_errmsg(pConn->Connection));
        return EPDK_FAIL;
    }

    /* delete row from cursor */
    DBCursor_RemoveRow(pCursor, rowIdx);

    return EPDK_OK;
}
