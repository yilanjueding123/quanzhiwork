/*
************************************************************************************************************************
*                                                     REGISTER TABLE
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : reg.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.04.27
*
* Description :
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
#ifndef  __reg_h
#define  __reg_h



#define KEY_NAME_LEN                   47
#define KEY_VALUE_LEN                 255
#define SET_NAME_LEN                   47
#define LINE_MAX_LEN                    ( KEY_NAME_LEN + KEY_VALUE_LEN + 32 )

/* open mode */
#define REG_READONLY                    0
#define REG_READWRITE                   1
#define REG_CREATE                      2

/* global errer */
#define REG_OK                          0
#define REG_ERR                        -1
#define REG_ERR_FILE_END                1
#define REG_ERR_SET_LIST_END            2
#define REG_ERR_INVALID_SET_NAME        3
#define REG_ERR_KEY_LIST_END            4
#define REG_ERR_INVALID_KEY_NAME        5
#define REG_ERR_ILLEGAL_SET_NAME        6
#define REG_ERR_ILLEGAL_KEY_NAME        7
#define REG_ERR_ILLEGAL_KEY_VALUE       8
#define REG_ERR_ALLOC_MEM_FAIL          9
#define REG_ERR_SET_EXIST              10
#define REG_ERR_ILLEGAL_OP             11
#define REG_ERR_KEY_EXIST              12
#define REG_ERR_INVALID_HANDLE         13
#define REG_ERR_DIRTY_DATA             14
#define REG_ERR_NORMAL_OPEN_ERR        15


typedef __u32 HNODE;


extern __s32 REG_Init( void );
extern __s32 REG_Exit( void );

extern HNODE REG_OpenNode( const char *node, __s32 mode );
extern __s32 REG_CloseNode  ( HNODE handle );
extern __s32 REG_DeleteNode( const char *node );

extern __s32 REG_GetSetCount( HNODE handle, __u32 *count_p );
extern __s32 REG_GetFirstSet( HNODE handle );
extern __s32 REG_GetNextSet ( HNODE handle, char *set_name );
extern __s32 REG_CreateSet  ( HNODE handle, const char *set_name );
extern __s32 REG_DeleteSet  ( HNODE handle, const char *set_name );

extern __s32 REG_GetKeyCount( HNODE handle, const char *set_name, __u32 *count_p );
extern __s32 REG_GetFirstKey( HNODE handle, const char *set_name );
extern __s32 REG_GetNextKey ( HNODE handle, const char *set_name, char *key_name );
extern __s32 REG_CreateKey  ( HNODE handle, const char *set_name, const char *key_name, const char *key_value );
extern __s32 REG_DeleteKey  ( HNODE handle, const char *set_name, const char *key_name );
extern __s32 REG_SetKeyValue( HNODE handle, const char *set_name, const char *key_name, const char *key_value );
extern __s32 REG_GetKeyValue( HNODE handle, const char *set_name, const char *key_name, char *key_value );

extern __s32 REG_GetError( HNODE handle );
extern __s32 REG_ClrError( HNODE handle );

extern __s32 REG_CreatePath ( const char *path );
extern __s32 REG_DeletePath( const char *path );

extern __s32 REG_Ini2Reg( const char *ini_file );
extern __s32 REG_Reg2Ini( const char *ini_file );
extern void  REG_SetRootPath( const char *path );
extern void  REG_GetRootPath( char *path );




#endif     //  ifndef __reg_h

/* end of reg.h  */
