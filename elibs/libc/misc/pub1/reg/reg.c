/*
************************************************************************************************************************
*                                                     REGISTER TABLE
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : reg.c
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.04.27
*
* Description :
*
* Functions list :
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2008.04.27       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef __reg_c
#define __reg_c


#include "reg_cfg.h"
#include "reg_private.h"


#define	START_THREAD                    1
#define END_THREAD                      0




/*-----------------------------------------global data--------------------------------------------*/
#if ( EPDK_OS == EPDK_OS_WINXP ) || ( EPDK_OS == EPDK_OS_WIN2K ) || ( EPDK_OS == EPDK_OS_VISTA )

#include <windows.h>
#include <winbase.h>
HANDLE refresh_hdl = NULL;
HANDLE backup_hdl  = NULL;

#ifdef REG_LOG
G_FILE *log_fp = NULL;
#define REG_LOG_FILE      "d:\reg_log.txt"
#endif

#elif ( EPDK_OS == EPDK_OS_EPOS)

uint8  tid0;
uint8  tid1;

/*----------------------------------------- platform end -----------------------------------------*/
#else
#error PLATFORM must be selected.
#endif



/*-----------------------------------------function list------------------------------------------*/


/***************************************************************************************************
*Name        : REG_Ini2Reg
*Prototype   : int32 REG_Ini2Reg( const char *ini_file )
*Arguments   : ini_file       full name of ini file
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : create a register file corresponding to an ini file.
*Other       :
***************************************************************************************************/
int32 REG_Ini2Reg( const char *ini_file )
{
    return __Ini2Reg( ini_file );
}





/***************************************************************************************************
*Name        : REG_Reg2Ini
*Prototype   : int32 REG_Reg2Ini( const char *ini_file )
*Arguments   : ini_file       full name of ini file
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : create a ini file corresponding to register file
*Other       :
***************************************************************************************************/
int32 REG_Reg2Ini( const char *ini_file )
{
    return __Reg2Ini( ini_file );
}





/***************************************************************************************************
*Name        : RefreshAllNode
*Prototype   : int32 RefreshAllNode( void * )
*Arguments   : void
*Return      : REG_OK        operation succeed
*              REG_ERR       operation fail
*Description : refresh all register file
*Other       :
***************************************************************************************************/
/*---------------------------------------- WINDOWS OS ----------------------------------------*/
#if ( EPDK_OS == EPDK_OS_WINXP ) || ( EPDK_OS == EPDK_OS_WIN2K ) || ( EPDK_OS == EPDK_OS_VISTA )

//UINT
DWORD WINAPI RefreshAllNode( LPVOID pParam )
{
    uint32 i = 0;

    while( 1 )
    {
        g_delay( REFRESH_ALL_NODE_DELAY_TIME );

        i++;
        __msg("\n\n\n\nThread RefreshAllNode has been executed %d times.\n", i);

        if( __refresh_all_reg_file( ) != REG_OK )
            return REG_ERR;
    }
    return REG_OK;
}

/*----------------------------------------- EPOS OS -------------------------------------------*/
#elif ( EPDK_OS == EPDK_OS_EPOS)

void RefreshAllNode( void *p )
{
    uint32 i = 0;

    while(1)
    {
        g_delay(REFRESH_ALL_NODE_DELAY_TIME);

        i++;
        //    	__msg("\n\n\n\nThread RefreshAllNode has been executed %d times.\n", i);

        __refresh_all_reg_file();

        if(esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
        {
            __inf("Thread RefreshAllNode has been deleted.\n");
            esKRNL_TDel(OS_PRIO_SELF);
        }
    }
}

/*----------------------------------------- platform end -----------------------------------------*/
#else
#error PLATFORM must be selected.
#endif





/***************************************************************************************************
*Name        : BackupAllNode
*Prototype   : int32 BackupAllNode( void * )
*Arguments   : void
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : backup all the nodes.
*Other       :
***************************************************************************************************/
/*---------------------------------------- WINDOWS OS ----------------------------------------*/
#if ( EPDK_OS == EPDK_OS_WINXP ) || ( EPDK_OS == EPDK_OS_WIN2K ) || ( EPDK_OS == EPDK_OS_VISTA )

//UINT
DWORD WINAPI BackupAllNode( LPVOID pParam )
{
    uint32 i = 0;

    while( 1 )
    {
        g_delay( BACKUP_ALL_NODE_DELAY_TIME );

        i++;
        __msg("\n\n\n\nThread BackupAllNode has been executed %d times.\n", i);

        if( __backup_all_reg_file( ) != REG_OK )
            return REG_ERR;
    }
    return REG_OK;
}

/*----------------------------------------- EPOS OS -------------------------------------------*/
#elif ( EPDK_OS == EPDK_OS_EPOS)

void BackupAllNode( void *p )
{
    uint32 i = 0;

    while(1)
    {
        g_delay(BACKUP_ALL_NODE_DELAY_TIME);

        i++;
        //    	__msg("\n\n\n\nThread BackupAllNode has been executed %d times.\n", i);

        __backup_all_reg_file( );

        if(esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
        {
            __inf("Thread BackupAllNode has been deleted.\n");
            esKRNL_TDel(OS_PRIO_SELF);
        }
    }
}

/*----------------------------------------- platform end -----------------------------------------*/
#else
#error PLATFORM must be selected.
#endif





/***************************************************************************************************
*Name        : REG_OpenNode
*Prototype   : HNODE REG_OpenNode( const char *node, int32 mode )
*Arguments   : node           node to be open
*              mode           open mode. optional mode: REG_READONLY, REG_READWRITE and REG_CREATE.
*                             in REG_READONLY mode, node can be only read and can be open several
*                             handles at the same time. In REG_READWRITE mode, node can be read and
*                             write, and can be open only one hdl at the same time. In REG_CREATE
*                             mode, create a new node.
*Return      : NULL           operation fail
*              !NULL          operation succeed, a hdl of node.
*Description : open a node in certain mode.
*Other       :
***************************************************************************************************/
HNODE REG_OpenNode( const char *node, int32 mode )
{
    HNODE handle = NULL;
    __s32 ret;

    handle = __OpenNode( node, mode, &ret );
    if( handle != NULL )
    {
        return handle;
    }
    else
    {
        if( ret == REG_ERR_DIRTY_DATA )
        {
            /* resume default node */
            ret = __resume_default_node( node );
            if( ret == REG_ERR )
            {
                return NULL;
            }
            else
            {
                /* reopen */
                return __OpenNode( node, mode, &ret );
            }
        }
        else
        {
            return NULL;
        }
    }
}





/***************************************************************************************************
*Name        : REG_DeleteNode
*Prototype   : int32 REG_DeleteNode( const char *node )
*Arguments   : node           node to be deleted
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : delete a node.
*Other       :
***************************************************************************************************/
int32 REG_DeleteNode( const char *node )
{
    return __DeleteNode( node );
}





/***************************************************************************************************
*Name        : REG_CloseNode
*Prototype   : int32 REG_CloseNode( HNODE handle )
*Arguments   : handle         a handle of a node
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : close node.
*Other       :
***************************************************************************************************/
int32 REG_CloseNode( HNODE handle )
{
    return __CloseNode( handle );
}





/***************************************************************************************************
*Name        : REG_GetSetCount
*Prototype   : int32 REG_GetSetCount( HNODE handle, uint32 *count_p )
*Arguments   : handle         a handle of a node
*              count_p        output. the total count of set of node.
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : get the total count of set of node.
*Other       :
***************************************************************************************************/
int32 REG_GetSetCount( HNODE handle, uint32 *count_p )
{
    return __GetSetCount( handle, count_p );
}





/***************************************************************************************************
*Name        : REG_GetFirstSet
*Prototype   : int32 REG_GetFirstSet( HNODE handle )
*Arguments   : handle         a handle of a node
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail.
*Description : the function is used to organise set list for REG_GetNextSet(). NOTE: before call
*              REG_GetNextSet(), REG_GetFirstSet() must be called first.
*Other       :
***************************************************************************************************/
int32 REG_GetFirstSet( HNODE handle )
{
    return __GetFirstSet( handle );
}





/***************************************************************************************************
*Name        : REG_GetNextSet
*Prototype   : int32 REG_GetNextSet( HNODE handle, char *set_name )
*Arguments   : handle         a handle of a node
*              set_name       output. a buffer to store set name. The size of the buffer must NOT be
*                             less than SET_NAME_LEN+1, (the constant is defined in head file).
*Return      : REG_OK                   operation succeed
*              REG_ERR                  operation fail
*              REG_ERR_SET_LIST_END     to the end of set list
*Description : get next set name.
*Other       :
***************************************************************************************************/
int32 REG_GetNextSet( HNODE handle, char *set_name )
{
    return __GetNextSet( handle, set_name );
}





/***************************************************************************************************
*Name        : REG_GetKeyCount
*Prototype   : int32 REG_GetKeyCount( HNODE handle, const char *set_name, uint32 *count_p )
*Arguments   : handle         a handle of a node
*              set_name       set name
*              count_p        output. the total key count of the specified set
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description :
*Other       :
***************************************************************************************************/
int32 REG_GetKeyCount( HNODE handle, const char *set_name, uint32 *count_p )
{
    return __GetKeyCount( handle, set_name, count_p );
}





/***************************************************************************************************
*Name        : REG_GetFirstKey
*Prototype   : int32 REG_GetFirstKey( HNODE handle, const char *set_name )
*Arguments   : handle         a handle of node
*              set_name       set name
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail.
*Description : the function is used to organise key list for REG_GetNextKey(). NOTE: before call
*              REG_GetNextKey(), REG_GetFirstKey() must be called first.
*Other       :
***************************************************************************************************/
int32 REG_GetFirstKey( HNODE handle, const char *set_name )
{
    return __GetFirstKey( handle, set_name );
}





/***************************************************************************************************
*Name        : REG_GetNextKey
*Prototype   : int32 REG_GetNextKey( HNODE handle, const char *set_name, char *key_name )
*Arguments   : handle         a handle of node
*              set_name       set name
*              key_name       output. a buffer to store key name. The size of the buffer must NOT be
*                             less than KEY_NAME_LEN+1, (the constant is defined in head file).
*Return      : REG_OK                   operation succeed
*              REG_ERR                  operation fail
*              REG_ERR_KEY_LIST_END     to the end of key list
*Description : get next key name of specified set.
*Other       :
***************************************************************************************************/
int32 REG_GetNextKey( HNODE handle, const char *set_name, char *key_name )
{
    return __GetNextKey( handle, set_name, key_name );
}





/***************************************************************************************************
*Name        : REG_CreateKey
*Prototype   : int32 REG_CreateKey( HNODE handle, const char *set_name, const char *key_name,
*                                 const char *key_value )
*Arguments   : handle         a handle of a node
*              set_name       set name
*              key_name       key name
*              key_value      key value
*Return      : REG_OK              operation succeed
*              REG_ERR_KEY_EXIST   key already existed
*              REG_ERR             operation fail
*Description : set the value of specified key.
*Other       :
***************************************************************************************************/
int32 REG_CreateKey( HNODE handle, const char *set_name, const char *key_name, const char *key_value )
{
    return __CreateKey( handle, set_name, key_name, key_value );
}





/***************************************************************************************************
*Name        : REG_SetKeyValue
*Prototype   : int32 REG_SetKeyValue( HNODE handle, const char *set_name, const char *key_name,
*                                   const char *key_value )
*Arguments   : handle         a handle of a node
*              set_name       set name
*              key_name       key name
*              key_value      key value
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : set the value of specified key.
*Other       :
***************************************************************************************************/
int32 REG_SetKeyValue( HNODE handle, const char *set_name, const char *key_name, const char *key_value )
{
    return __SetKeyValue( handle, set_name, key_name, key_value );
}





/***************************************************************************************************
*Name        : REG_GetKeyValue
*Prototype   : int32 REG_GetKeyValue( HNODE handle, const char *set_name, const char *key_name,
*                                 char *key_value )
*Arguments   : handle         a handle of a node
*              set_name       set name
*              key_name       key name
*              key_value      output. a buffer to store key value. The size of the buffer must NOT be
*                             less than KEY_VALUE_LEN+1, (the constant is defined in head file).
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : get the value of specified key.
*Other       :
***************************************************************************************************/
int32 REG_GetKeyValue( HNODE handle, const char *set_name, const char *key_name, char *key_value )
{
    return __GetKeyValue( handle, set_name, key_name, key_value );
}





/***************************************************************************************************
*Name        : REG_DeleteKey
*Prototype   : int32 REG_DeleteKey( HNODE handle, const char *set_name, const char *key_name )
*Arguments   : handle         a handle of node
*              set_name       set name
*              key_name       key name
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : delete a key from specified set.
*Other       :
***************************************************************************************************/
int32 REG_DeleteKey( HNODE handle, const char *set_name, const char *key_name )
{
    return __DeleteKey( handle, set_name, key_name );
}





/***************************************************************************************************
*Name        : REG_CreateSet
*Prototype   : int32 REG_CreateSet( HNODE handle, const char *set_name )
*Arguments   : handle         a handle of a node
*              set_name       set name
*Return      : REG_OK                   operation succeed
*              REG_ERR                  operation fail
*              REG_ERR_SET_EXIST        the set has existed.
*Description : create a set based on specified name.
*Other       :
***************************************************************************************************/
int32 REG_CreateSet( HNODE handle, const char *set_name )
{
    return __CreateSet( handle, set_name );
}





/***************************************************************************************************
*Name        : REG_DeleteSet
*Prototype   : int32 REG_DeleteSet( HNODE handle, const char *set_name )
*Arguments   : handle         a handle of a node
*              set_name       set name
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : delete a set. Once the set is deleted, all the keys that pertain to the set are deteled.
*Other       :
***************************************************************************************************/
int32 REG_DeleteSet( HNODE handle, const char *set_name )
{
    return __DeleteSet( handle, set_name );
}





/***************************************************************************************************
*Name        : REG_CreatePath
*Prototype   : int32 REG_CreatePath( const char *path )
*Arguments   : path           register path
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : create a path
*Other       :
***************************************************************************************************/
int32 REG_CreatePath( const char *path )
{
    return __CreatePath( path );
}





/***************************************************************************************************
*Name        : REG_DeletePath
*Prototype   : int32 REG_DeletePath( const char *path )
*Arguments   : path           register path
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : delete all the sub_folder and all the files.
*Other       :
***************************************************************************************************/
int32 REG_DeletePath( const char *path )
{
    return __DeletePath( path );
}





/***************************************************************************************************
*Name        : REG_SetRootPath
*Prototype   : int32 REG_SetRootPath( const char *path )
*Arguments   : path           register root path
*Return      : void
*Description : set register root path.
*Other       :
***************************************************************************************************/
void REG_SetRootPath( const char *path )
{
    __SetRootPath( path );
}





/***************************************************************************************************
*Name        : REG_GetRootPath
*Prototype   : int32 REG_GetRootPath( char *path )
*Arguments   : path           register root path
*Return      : void
*Description : get register root path.
*Other       :
***************************************************************************************************/
void REG_GetRootPath( char *path )
{
    __GetRootPath( path );
}





/***************************************************************************************************
*Name        : REG_Init
*Prototype   : int32 REG_Init( void )
*Arguments   : void
*Return      : EPDK_OK        operation succeed
*              EPDK_FAIL      operation fail
*Description : initialise register table module.
*Other       :
***************************************************************************************************/
/*---------------------------------------- WINDOWS OS ----------------------------------------*/
#if ( EPDK_OS == EPDK_OS_WINXP ) || ( EPDK_OS == EPDK_OS_WIN2K ) || ( EPDK_OS == EPDK_OS_VISTA )

int32 REG_Init( void )
{
    unsigned long thread_id;

    if( __reg_init( ) != REG_OK )
        return REG_ERR;

#ifdef ENABLE_THREAD
    refresh_hdl = CreateThread( 0, 2 * 1024, RefreshAllNode, NULL, 0, &thread_id );
    if( refresh_hdl == NULL )
        return REG_ERR;

    backup_hdl = CreateThread( 0, 2 * 1024, BackupAllNode, NULL, 0, &thread_id );
    if( backup_hdl == NULL )
        return REG_ERR;
#else
    __backup_all_reg_file( );
#endif  // #ifdef ENABLE_THREAD

#ifdef REG_LOG
    log_fp = freopen( REG_LOG_FILE, "w+", stdout );
    if( log_fp == NULL )
    {
        __wrn("Error in redirect debug information to log file.\n");
    }
#endif  // #ifdef REG_LOG

    return REG_OK;
}

/*----------------------------------------- EPOS OS -------------------------------------------*/
#elif ( EPDK_OS == EPDK_OS_EPOS)

int32 REG_Init( void )
{
    if( __reg_init( ) != REG_OK )
        return REG_ERR;
    __msg("Succeed in initialising part of register.\n");

#ifdef ENABLE_THREAD
    tid0 = esKRNL_TCreate( RefreshAllNode, NULL, 0x2000, (EPOS_curid << 8) | REFRESH_ALL_NODE_THREAD_PRIO );
    __msg("Succeed in creating thread \"refresh\".\n");
    tid1 = esKRNL_TCreate(  BackupAllNode, NULL, 0x2000, (EPOS_curid << 8) | BACKUP_ALL_NODE_THREAD_PRIO );
    __msg("Succeed in creating thread \"backup\".\n");
#else
    __backup_all_reg_file( );
    __msg("has backup all reg files.\n");
#endif  // #ifdef ENABLE_THREAD

    __msg("Succeed in initialising all register.\n");
    return REG_OK;
}

/*----------------------------------------- platform end -----------------------------------------*/
#else
#error PLATFORM must be selected.
#endif





/***************************************************************************************************
*Name        : REG_Exit
*Prototype   : int32 REG_Exit( void )
*Arguments   : void
*Return      : EPDK_OK        operation succeed
*              EPDK_FAIL      operation fail
*Description : exit register table module.
*Other       :
***************************************************************************************************/
/*---------------------------------------- WINDOWS OS ----------------------------------------*/
#if ( EPDK_OS == EPDK_OS_WINXP ) || ( EPDK_OS == EPDK_OS_WIN2K ) || ( EPDK_OS == EPDK_OS_VISTA )

int32 REG_Exit( void )
{

#ifdef ENABLE_THREAD
    if( backup_hdl != NULL )
        CloseHandle( backup_hdl );

    if( refresh_hdl != NULL )
        CloseHandle( refresh_hdl );
#else
    __msg("ready to refresh all reg files.\n");
    __refresh_all_reg_file( );
    __msg("has refreshed all reg files.\n");
#endif  // #ifdef ENABLE_THREAD

#ifdef REG_LOG
    g_close( log_fp );
    log_fp = NULL;
#endif  // #ifdef REG_LOG


    return __reg_exit( );
}

/*----------------------------------------- EPOS OS -------------------------------------------*/
#elif ( EPDK_OS == EPDK_OS_EPOS)

int32 REG_Exit( void )
{
#ifdef ENABLE_THREAD
    while( esKRNL_TDelReq(tid1) != OS_TASK_NOT_EXIST )
        esKRNL_TimeDly(100);

    while( esKRNL_TDelReq(tid0) != OS_TASK_NOT_EXIST )
        esKRNL_TimeDly(100);
#else
    __refresh_all_reg_file( );
    __msg("has refreshed all reg files.\n");
#endif  // #ifdef ENABLE_THREAD

    return __reg_exit( );
}

/*----------------------------------------- platform end -----------------------------------------*/
#else
#error PLATFORM must be selected.
#endif





/***************************************************************************************************
*Name        : REG_GetError
*Prototype   : int32 REG_GetError( HNODE handle )
*Arguments   : hdl        a handle of a node
*Return      : error code
*Description : get error code of current operation.
*Other       :
***************************************************************************************************/
int32 REG_GetError( HNODE handle )
{
    return __GetError( handle );
}





/***************************************************************************************************
*Name        : REG_ClrError
*Prototype   : int32 REG_ClrError( HNODE handle )
*Arguments   : hdl            a handle of a node
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : get error code of current operation.
*Other       :
***************************************************************************************************/
int32 REG_ClrError( HNODE handle )
{
    return __ClrError( handle );
}





#endif     //  ifndef __reg_c

/* end of reg.c  */
