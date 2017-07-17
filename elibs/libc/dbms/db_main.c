/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_main.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-8-21
* Descript: dbms wapper libary core functions.
* Update  : date                auther      ver     notes
*           2010-8-21 19:58:59  Sunny       1.0     create this file.
*********************************************************************************************************
*/

#include    "dbms_i.h"

static DBLibaryManager_t DBLibManager;

/*
*********************************************************************************************************
*                                       DB LIBARY INITIALIZE
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
int DBLibary_Init(void)
{
    int rc;

    /* initialize database libary manager */
    DBLibManager.ConnRoot = NULL;

    /* initialize memory allocator */
    rc = DBMem_Init();
    if (rc != EPDK_OK)
    {
        __wrn("initialize memory alloccator failed\n");
        return EPDK_FAIL;
    }

    /* load SQLite3 module */
    rc = DBLibary_LoadSQLite3Libary();
    if (rc != EPDK_OK)
    {
        __wrn("load SQLite3 module failed\n");
        DBMem_Exit();
        return EPDK_FAIL;
    }

    /* initialize sqlite3 libary */
    rc = sqlite3_initialize();
    if (rc != SQLITE_OK)
    {
        __wrn("initialize sqlite3 failed\n");
        DBLibary_FreeSQLite3Libary();
        DBMem_Exit();
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       DB LIBARY EXIT
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
int DBLibary_Exit(void)
{
    if (DBLibManager.ConnRoot)
    {
        __wrn("DBLibary have connection not released\n");
        return EPDK_FAIL;
    }

    /* shut down sqlite3 */
    sqlite3_shutdown();

    /* exit memory allocator */
    DBMem_Exit();

    /* unload SQLite3 module */
    DBLibary_FreeSQLite3Libary();

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     DB LIBARY VERSION
*
* Description: get the database system libary version information.
*
* Arguments  : NONE
*
* Returns    : database manager system libary version.
*
* Note       : The version information is a read-only string,
*              The calling program must not attempt to free or modify the returned string.
*********************************************************************************************************
*/
const char *DBLibary_Vesion(void)
{
    return DBLIBARY_VERSION;
}

int DBLibary_AttachConn(DBConn_t *pConn)
{
    /* attach pConn to DBLibManager.ConnRoot */
    pConn->Next             = DBLibManager.ConnRoot;
    DBLibManager.ConnRoot   = pConn;

    return EPDK_OK;
}

int DBLibary_DisattachConn(DBConn_t *pConn)
{
    DBConn_t *pVictimPrev;

    /* check disattach root connection or not */
    if (DBLibManager.ConnRoot == pConn)
    {
        DBLibManager.ConnRoot = pConn->Next;
        return EPDK_OK;
    }

    /* seach victim connection  */
    pVictimPrev = DBLibManager.ConnRoot;
    while (pVictimPrev)
    {
        if (pVictimPrev->Next == pConn)
        {
            /* find victim prev connection */
            break;
        }
        pVictimPrev = pVictimPrev->Next;
    }

    if (pVictimPrev == NULL)
    {
        __wrn("disattach one invalid connction\n");
        return EPDK_FAIL;
    }

    /* disattach pConn from DBLibManager */
    pVictimPrev->Next = pConn->Next;

    return EPDK_OK;
}
