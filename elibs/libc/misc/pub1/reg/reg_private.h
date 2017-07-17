/*
************************************************************************************************************************
*                                                     REGISTER TABLE
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : reg_private.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.05.27
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
* Gary.Wang      2008.05.27       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __reg_private_h
#define  __reg_private_h


#include "reg_cfg.h"
#include "reg.h"


extern char reg_root_path[256];


extern __s32 __reg_init( void );
extern __s32 __reg_exit( void );

extern HNODE __OpenNode( const char *node, __s32 mode, __s32 *err_p );
extern __s32 __CloseNode  ( HNODE handle );
extern __s32 __DeleteNode( const char *node );

extern __s32 __GetSetCount( HNODE handle, __u32 *count_p );
extern __s32 __GetFirstSet( HNODE handle );
extern __s32 __GetNextSet ( HNODE handle, char *set_name );
extern __s32 __CreateSet  ( HNODE handle, const char *set_name );
extern __s32 __DeleteSet  ( HNODE handle, const char *set_name );

extern __s32 __GetKeyCount( HNODE handle, const char *set_name, __u32 *count_p );
extern __s32 __GetFirstKey( HNODE handle, const char *set_name );
extern __s32 __GetNextKey ( HNODE handle, const char *set_name, char *key_name );
extern __s32 __CreateKey  ( HNODE handle, const char *set_name, const char *key_name, const char *key_value );
extern __s32 __DeleteKey  ( HNODE handle, const char *set_name, const char *key_name );
extern __s32 __SetKeyValue( HNODE handle, const char *set_name, const char *key_name, const char *key_value );
extern __s32 __GetKeyValue( HNODE handle, const char *set_name, const char *key_name, char *key_value );

extern __s32 __GetError( HNODE handle );
extern __s32 __ClrError( HNODE handle );

extern __s32 __CreatePath ( const char *path );
extern __s32 __DeletePath( const char *path );

extern __s32 __Ini2Reg( const char *ini_file );
extern __s32 __Reg2Ini( const char *ini_file );
extern void  __SetRootPath( const char *path );
extern void  __GetRootPath( char *path );



extern int32 __refresh_reg_file( const char *reg_file );
extern int32 __backup_reg_file ( const char *reg_file );
extern int32 __refresh_all_reg_file( void );
extern int32 __backup_all_reg_file( void );
extern __s32 __resume_default_node( const char *node );





#endif     //  ifndef __reg_private_h

/* end of reg_private.h  */
