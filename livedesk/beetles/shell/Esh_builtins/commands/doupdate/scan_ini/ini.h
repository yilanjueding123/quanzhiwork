/*
************************************************************************************************************************
*                                                        INI
*                                                Scan and parser ini file
*
*                             Copyright(C), 2006-2008, Allwinner Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : ini.h
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
#ifndef  __ini_h
#define  __ini_h


#include "scan_ini_cfg.h"


#define KEY_NAME_LEN                     47
#define KEY_VALUE_LEN                   255
#define SET_NAME_LEN                     47
#define LINE_MAX_LEN                    ( KEY_NAME_LEN + KEY_VALUE_LEN + 32 )



extern __Bool is_ident_char( uint8 ch );
extern __Bool is_valid_set_name( const char *set_name )  ;
extern __Bool is_valid_key_name( const char *key_name )  ;
extern __Bool is_valid_key_value( const char *key_value );



#endif     //  ifndef __ini_h

/* end of ini.h */
