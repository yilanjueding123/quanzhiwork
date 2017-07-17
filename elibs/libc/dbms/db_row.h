/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_row.h
* By      : Sunny
* Version : v1.0
* Date    : 2010-9-1
* Descript: dbms row level operations.
* Update  : date                auther      ver     notes
*           2010-9-1 13:22:30   Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef     __DB_ROW_H__
#define     __DB_ROW_H__

/* invalid row index of a cursor */
#define     INVALID_ROW_INDEX(Cursor, Idx)   ((Idx < 0) || (Idx >= Cursor->ValidRows))

#endif      /* __DB_ROW_H__ */
