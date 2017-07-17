/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_insert_i.h
* By      : Sunny
* Version : v1.0
* Date    : 2010-9-5
* Descript: dbms insert operations internal header file.
* Update  : date                auther      ver     notes
*           2010-9-5 11:24:43   Sunny       1.0     create this file.
*********************************************************************************************************
*/

#ifndef     __DB_INSERT_I_H__
#define     __DB_INSERT_I_H__
#include    <stdarg.h>

int     DBCursor_PackColumns(DBCursor_t *pCursor, DBRow_t *pRow, const char *format, va_list args);
__bool  DBCursor_RowIsNew(DBRow_t *pRow, int ColumnNumber);

#endif      /* __DB_INSERT_H__ */
