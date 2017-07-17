/*
************************************************************************************************************************
*                                                        SCAN_INI
*                                                Scan and parser ini file
*
*                             Copyright(C), 2006-2008, Allwinner Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : scan_ini_cfg.h
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
#ifndef  __scan_ini_cfg_h
#define  __scan_ini_cfg_h

#include    "epdk.h"

/* typedef.h */
typedef signed char			  s8;
typedef signed char			  __s8;
typedef signed char		      int8;

typedef unsigned char		  u8;
typedef unsigned char		  __u8;
typedef unsigned char		  uint8;


typedef short int			  s16;
typedef short int			  __s16;
typedef short int			  int16;

typedef unsigned short		  u16;
typedef unsigned short		  __u16;
typedef unsigned short		  uint16;

typedef int					  s32;
typedef int					  __s32;
typedef int					  int32;

typedef unsigned int		  u32;
typedef unsigned int		  __u32;
typedef unsigned int		  uint32;

typedef int32                 __Bool;
#define __TRUE                1
#define __FALSE               0

/* os_related.h */
#define G_FILE                ES_FILE
#define FILE                  ES_FILE
/* file operaton interfaces */
#define g_fopen               eLIBs_fopen
#define g_fclose              eLIBs_fclose
#define g_fseek               eLIBs_fseek
#define g_remove              eLIBs_remove
#define g_rename              eLIBs_rename
#define g_fread               eLIBs_fread
#define g_fwrite              eLIBs_fwrite
#define g_ftell               eLIBs_ftell

#define g_malloc( size )      esMEMS_Malloc( 0, size )
#define g_free( p )           esMEMS_Mfree( 0, p )

#define g_memcpy              eLIBs_memcpy
#define g_memset              eLIBs_memset
#define g_memmove             eLIBs_memmove
#define g_strcmp              eLIBs_strcmp
#define g_strncmp             eLIBs_strncmp
#define g_strcat              eLIBs_strcat
#define g_strncat             eLIBs_strncat
#define g_strcpy              eLIBs_strcpy
#define g_strncpy             eLIBs_strncpy
#define g_strlen              eLIBs_strlen
#define g_toupper             eLIBs_toupper
#define g_strtol              eLIBs_strtol

#undef  _4_char_to_int32
#define _4_char_to_int32(a,b,c,d)       ( (a)<<24 | (b)<<16 | (c)<<8 | (d) )

//#include "..\\..\\elibs\\misc\\pub1\\common\\type_def.h"
//#include "..\\..\\elibs\\misc\\pub1\\common\\g_os_related.h"
//#include "..\\..\\elibs\\misc\\pub1\\common\\g_debug.h"

#endif     //  ifndef __scan_ini_cfg_h

/* end of scan_ini_cfg.h */
