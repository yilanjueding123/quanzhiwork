/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_column.h
* By      : Sunny
* Version : v1.0
* Date    : 2010-9-1
* Descript: dbms column level operations.
* Update  : date                auther      ver     notes
*           2010-9-1 9:15:50    Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef     __DB_COLUMN_H__
#define     __DB_COLUMN_H__

/* column status */
#define DB_COLUMN_CLEAR     0   /* column data is clear                   */
#define DB_COLUMN_DIRTY     1   /* column data is dirty(have been update) */
#define DB_COLUMN_NEW       2   /* column data is new(inserted)           */

/* invalid column index of a cursor, the last ROWID column is hide for user */
#define INVALID_COLUMN_INDEX(Cursor, Idx)   ((Idx < 0) || (Idx >= (Cursor->ColumnNumber -1 )))

extern int DBCurosr_FindColumn(DBCursor_t *pCursor, const char *ColumnName);

#endif      /* __DB_COLUMN_H__ */

