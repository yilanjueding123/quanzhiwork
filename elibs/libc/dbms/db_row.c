/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_row.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-8-31
* Descript: dbms row level operations.
* Update  : date                auther      ver     notes
*           2010-8-31 19:29:17  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include    "dbms_i.h"

/*
*********************************************************************************************************
*                                       GET THE NUMBER OF ROWS
*
* Description: Get the number of rows in the specified query cursor.
*
* Arguments  : cursor -The target qurey cursor.
*
* Returns    : The number of rows in the result set, which may be 0 if the query did not
*              return any datasets.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_GetRowsNumber(HDBCursor cursor)
{
    DBCursor_t *pCursor;

    if (cursor == NULL)
    {
        __wrn("invalid cursor handle for get rows number\n");
        return 0;
    }
    pCursor = (DBCursor_t *)cursor;

    /* we should fetch all left rows from backing store now */
    DBConn_FetchLeftRows(pCursor);

    /* return valid rows number */
    return pCursor->ValidRows;
}

/*
*********************************************************************************************************
*                                MOVE CURSOR FROM CURRENT POSITION
*
* Description: Move the cursor by a relative amount, forward or backward, from the
*              current position. Positive offsets move forwards, negative offsets move
*              backwards.
*
* Arguments  : cursor -The target query cursor handle.
*              offset -The offset to be applied from the current position.
*
* Returns    : EPDK_OK   -if the requested destination was reachable,
*              EPDK_FAIL -if some thing false.
*
* Note       :  the row index base is 0.
*********************************************************************************************************
*/
int DBCursor_Move(HDBCursor cursor, int offset)
{
    DBCursor_t *pCursor;
    int         newPosition;

    if (cursor == NULL)
    {
        __wrn("invalid cursor handle for move\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;
    newPosition = pCursor->CurrentRowIdx + offset;
    if (newPosition < 0)
    {
        __wrn("move position beyond rows boundary\n");
        return EPDK_FAIL;
    }

    if (CURSOR_QUERY_MODE(pCursor) == DBCONN_QUERY_STEP)
    {
        /* query step by step work mode */
        if (newPosition >= pCursor->ValidRows)
        {
            /* move new position beyond current valid rows number,
             * need to fetch new position row.
             */
            return DBConn_FetchRow(pCursor, newPosition);
        }
        else
        {
            /* modify current row index to new postion */
            pCursor->CurrentRowIdx = newPosition;
            return EPDK_OK;
        }
    }

    /* query all rows work mode */
    if (newPosition >= pCursor->ValidRows)
    {
        __wrn("move beyond valid rows number\n");
        return EPDK_FAIL;
    }

    /* modify current row index to new postion */
    pCursor->CurrentRowIdx = newPosition;
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       MOVE CURSOR TO SPECIFIED POSITION
*
* Description: Move the cursor to an absolute position.
*
* Arguments  : cursor   -The target query cursor handle.
*              position -the zero-based position to move to.
*
* Returns    : EPDK_OK   -if the requested move fully succeeded,
*              EPDK_FAIL -if some thing false.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_MoveToPosition(HDBCursor cursor, int position)
{
    DBCursor_t *pCursor;

    if (cursor == NULL)
    {
        __wrn("invalid cursor handle for move position\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    if (position < 0)
    {
        __wrn("move position beyond rows boundary\n");
        return EPDK_FAIL;
    }
    if (CURSOR_QUERY_MODE(pCursor) == DBCONN_QUERY_STEP)
    {
        /* query step by step work mode */
        if (position >= pCursor->ValidRows)
        {
            /* move new position beyond current valid rows number,
             * need to fetch new position row.
             */
            DBConn_FetchRow(pCursor, position);
        }
    }

    /* query all rows work mode */
    if (position >= pCursor->ValidRows)
    {
        __wrn("move position beyond rows boundary\n");
        return EPDK_FAIL;
    }

    /* modify current row index to new postion */
    pCursor->CurrentRowIdx = position;
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                      MOVE CURSOR TO FIRST ROW
*
* Description: Move the cursor to the first row.
*
* Arguments  : cursor   -The target query cursor handle.
*
* Returns    : EPDK_OK   -if the requested move fully succeeded,
*              EPDK_FAIL -if some thing false.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_MoveToFirst(HDBCursor cursor)
{
    DBCursor_t *pCursor;

    if (cursor == NULL)
    {
        __wrn("invalid cursor handle for move to first\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    /* modify current row index to first row */
    pCursor->CurrentRowIdx = 0;
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                      MOVE CURSOR TO LAST ROW
*
* Description: Move the cursor to the last row.
*
* Arguments  : cursor   -The target query cursor handle.
*
* Returns    : EPDK_OK   -if the requested move fully succeeded,
*              EPDK_FAIL -if some thing false.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_MoveToLast(HDBCursor cursor)
{
    DBCursor_t *pCursor;

    if (cursor == NULL)
    {
        __wrn("invalid cursor handle for move to last\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    if (CURSOR_QUERY_MODE(pCursor) == DBCONN_QUERY_STEP)
    {
        /* need to query out all left rows */
        DBConn_FetchLeftRows(pCursor);
    }

    /* modify current row index to last row postion */
    pCursor->CurrentRowIdx = pCursor->ValidRows - 1;
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                      MOVE CURSOR TO PREV ROW
*
* Description: Move the cursor to the previous row.
*
* Arguments  : cursor   -The target query cursor handle.
*
* Returns    : EPDK_OK   -if the requested move fully succeeded,
*              EPDK_FAIL -if some thing false.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_MoveToPrev(HDBCursor cursor)
{
    DBCursor_t *pCursor;

    if (cursor == NULL)
    {
        __wrn("invalid cursor handle for move to previous\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    if (pCursor->CurrentRowIdx == 0)
    {
        __wrn("cursor have point to first row now\n");
        return EPDK_FAIL;
    }
    /* modify current row index to previous row */
    pCursor->CurrentRowIdx--;
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                      MOVE CURSOR TO NEXT ROW
*
* Description: Move the cursor to the next row.
*
* Arguments  : cursor   -The target query cursor handle.
*
* Returns    : EPDK_OK   -if the requested move fully succeeded,
*              EPDK_FAIL -if some thing false.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_MoveToNext(HDBCursor cursor)
{
    DBCursor_t *pCursor;
    int         position;

    if (cursor == NULL)
    {
        __wrn("invalid cursor handle for move position\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    position = pCursor->CurrentRowIdx + 1;
    if (CURSOR_QUERY_MODE(pCursor) == DBCONN_QUERY_STEP)
    {
        /* query step by step work mode */
        if (position >= pCursor->ValidRows)
        {
            /* the next row have not query out,
             * need to fetch the next row.
             */
            DBConn_FetchRow(pCursor, position);
        }
    }

    if (position >= pCursor->ValidRows)
    {
        __wrn("move beyond valid rows boundary\n");
        return EPDK_FAIL;
    }

    /* modify current row index to next row */
    pCursor->CurrentRowIdx = position;
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                      GET CURRENT ROW INDEX OF CURSOR
*
* Description: Get current position of the cursor in the row set, The value is zero-based.
*
* Arguments  : cursor -The target query cursor handle.
*
* Returns    : The current cursor position. return -1 if some thing false.
* Note       :
*********************************************************************************************************
*/
int DBCursor_GetCurRowIdx(HDBCursor cursor)
{
    DBCursor_t *pCursor;

    if (cursor == NULL)
    {
        __wrn("invalid cursor handle for get current row index\n");
        return -1;
    }
    pCursor = (DBCursor_t *)cursor;

    /* return current row index */
    return pCursor->CurrentRowIdx;
}

/*
*********************************************************************************************************
*                                       CURSOR IS MOVE TO THE FIRST ROW
*
* Description: Whether the cursor is pointing to the first row.
*
* Arguments  : cursor -The target query cursor handle.
*
* Returns    : EPDK_TRUE  -the cursor is pointing at the first entry,
*              EPDK_FALSE -the cursor is not pointing at the first entry.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_IsFirst(HDBCursor cursor)
{
    DBCursor_t *pCursor;

    if (cursor == NULL)
    {
        __wrn("invalid cursor handle for get current row index\n");
        return -1;
    }
    pCursor = (DBCursor_t *)cursor;

    if (pCursor->CurrentRowIdx == 0)
    {
        return EPDK_TRUE;
    }
    return EPDK_FALSE;
}

/*
*********************************************************************************************************
*                                       CURSOR IS MOVE TO THE LAST ROW
*
* Description: Whether the cursor is pointing to the last row.
*
* Arguments  : cursor -The target query cursor handle.
*
* Returns    : EPDK_TRUE  -the cursor is pointing at the last entry,
*              EPDK_FALSE -the cursor is not pointing at the last entry.
*
* Note       :
*********************************************************************************************************
*/
int DBCursor_IsLast(HDBCursor cursor)
{
    DBCursor_t *pCursor;

    if (cursor == NULL)
    {
        __wrn("invalid cursor handle for get rows number\n");
        return EPDK_FAIL;
    }
    pCursor = (DBCursor_t *)cursor;

    /* we should try to fetch all left rows
     * from lowlevel database engine now.
     */
    DBConn_FetchLeftRows(pCursor);

    if (pCursor->CurrentRowIdx == (pCursor->ValidRows - 1))
    {
        return EPDK_TRUE;
    }
    return EPDK_FALSE;
}
