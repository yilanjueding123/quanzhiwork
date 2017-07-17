/*
************************************************************************************************************************
*                                                        SCAN_INI
*                                                Scan and parser ini file
*
*                             Copyright(C), 2006-2008, Allwinner Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : scan_ini.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2008.11.04
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2008.11.04       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __scan_ini_h
#define  __scan_ini_h


#include "ini.h"


typedef enum
{
    LINE_ATTR_SET = 0,
    LINE_ATTR_KEY,
    LINE_ATTR_BLANK,
    LINE_ATTR_COMMENT,
    LINE_ATTR_COUNT  ,        // always the last one
} line_attr_e;


typedef enum
{
    INI_OK       = 0,
    INI_ERR         ,
    INI_ERR_FILE_END
} ini_err_e;


typedef void *HINI;
typedef void *HLIN;


/***************************************************************************************************
*Name        : open_ini
*Prototype   : HINI   open_ini( const char *file )
*Arguments   : file           ini file path
*Return      : NULL           open failed.
               !NULL          a valid handle of an ini file.
*Description : open an ini file.
*Other       :
***************************************************************************************************/
extern HINI   open_ini( const char *file );


/***************************************************************************************************
*Name        : close_ini
*Prototype   : int32  close_ini( HINI hini )
*Arguments   : hini           a handle of an ini file
*Return      : ==0            operation succeed
               !=0            operation fail
*Description : close an ini file.
*Other       :
***************************************************************************************************/
extern int32  close_ini( HINI hini );

/***************************************************************************************************
*Name        : line_rewind
*Prototype   : int32   line_rewind( HINI hini )
*Arguments   : hini           a handle of an ini file
*Return      : ==0            operation succeed
               !=0            operation fail
*Description : rewind to file begin.
*Other       :
***************************************************************************************************/
extern int32  line_rewind( HINI hini );

/***************************************************************************************************
*Name        : get_next_line
*Prototype   : HLIN   get_next_line( HINI hini )
*Arguments   : hini           a handle of an ini file
*Return      : ==NULL         operation fail. call "get_error" to get the reason for failure.
               !=NULL         a hanle of a line
*Description : get the next line of the ini file.
*Other       :
***************************************************************************************************/
extern HLIN   get_next_line( HINI hini );


/***************************************************************************************************
*Name        : line2attr
*Prototype   : int32  line2attr( HLIN hlin )
*Arguments   : hlin           a handle of a line
*Return      : ==-1           operation fail
               other value    the attribute of the line. check "line_attr_e" defined above.
*Description : get the attribute of the line.
*Other       :
***************************************************************************************************/
extern int32  line2attr( HLIN hlin );


/***************************************************************************************************
*Name        : line2comment
*Prototype   : int32  line2comment( HLIN hlin, char *comment )
*Arguments   : hlin           a handle of a line
*              comment        output. to store the content of the comment.
*Return      : ==0            operation succeed
               !=0            operation fail
*Description : get the content of the comment if the line's attribute is LINE_ATTR_COMMENT.
*Other       :
***************************************************************************************************/
extern int32  line2comment( HLIN hlin, char *comment );


/***************************************************************************************************
*Name        : line2set
*Prototype   : int32  line2set( HLIN hlin, char *set_name )
*Arguments   : hlin           a handle of a line
*              set_name       output. to store the name of the set.
*Return      : ==0            operation succeed
               !=0            operation fail
*Description : get the name of the set if the line's attribute is LINE_ATTR_SET.
*Other       :
***************************************************************************************************/
extern int32  line2set( HLIN hlin, char *set_name );


/***************************************************************************************************
*Name        : line2key
*Prototype   : int32  line2key( HLIN hlin, char *key_name, char *key_value )
*Arguments   : hlin           a handle of a line
*              key_name       output. to store the name  of the key.
*              key_value      output. to store the value of the key.
*Return      : ==0            operation succeed
               !=0            operation fail
*Description : get the name and the value of the key if the line's attribute is LINE_ATTR_KEY.
*Other       :
***************************************************************************************************/
extern int32  line2key( HLIN hlin, char *key_name, char *key_value );


/***************************************************************************************************
*Name        : get_error
*Prototype   : int32  get_error( HINI hini )
*Arguments   : hini           a handle of an ini file
*Return      : error number.  error numbers are defined above.
*Description : get the error number after an operation.
*Other       :
***************************************************************************************************/
extern int32  get_error( HINI hini );


/***************************************************************************************************
*Name        : check_ini
*Prototype   : __Bool check_ini( const char *file )
*Arguments   : file           ini file path
*Return      : bool           the syntax of the ini file is right or not.
*Description : check whether the syntax of the ini file is right or not.
*Other       :
***************************************************************************************************/
extern __Bool check_ini( const char *file );


#endif     //  ifndef __scan_ini_h

/* end of scan_ini.h */
