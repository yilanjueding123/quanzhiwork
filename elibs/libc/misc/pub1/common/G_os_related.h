/*
************************************************************************************************************************
*                                                     G_os_related
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : G_os_related.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.07.16
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
* Gary.Wang      2008.07.16       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __G_os_related_h
#define  __G_os_related_h

#include ".\..\..\..\..\elibs_i.h"


//#define TEST_MALLOC_MEM_LEAK
//#define TEST_FILE_MEM_LEAK

/*---------------------------------------- WINDOWS OS ----------------------------------------*/
#if ( EPDK_OS == EPDK_OS_WINXP ) || ( EPDK_OS == EPDK_OS_WIN2K ) || ( EPDK_OS == EPDK_OS_VISTA )

#undef min
#undef max

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "type_def.h"


#define t_malloc              malloc
#define t_free                free

#define g_memcpy              memcpy
#define g_memset              memset
#define g_memmove             memmove
#define g_strcmp              strcmp
#define g_strncmp             strncmp
#define g_strcat              strcat
#define g_strncat             strncat
#define g_strcpy              strcpy
#define g_strncpy             strncpy
#define g_strlen              strlen
#define g_toupper             toupper
#define g_strtol              strtol

#define G_FILE                FILE
#define t_fopen               fopen
#define t_fclose              fclose
#define g_fseek               fseek
#define g_remove              remove
#define g_rename              rename
#define g_feof                feof
extern size_t  g_fread(void *ptr, size_t size, size_t nobj, G_FILE *stream) ;
extern size_t  g_fwrite(const void *ptr, size_t size, size_t nobj, G_FILE *stream) ;
extern int32   g_mkdir( const char *dirname );
extern int32   g_rmdir( const char *dirname );

#define g_delay               Sleep

#ifndef EPDK_FAIL
#define EPDK_OK               0
#define EPDK_FAIL            -1
#endif  //#ifndef EPDK_FAIL

typedef uint32 __mp;

#define _inline_              __inline

#undef NULL
#define NULL                  0

#define g_rand                rand
#define g_printf              printf
#define __PACKED

/*----------------------------------------- EPOS OS -------------------------------------------*/
#elif ( EPDK_OS == EPDK_OS_EPOS)

#include "epdk.h"
#include "type_def.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define t_malloc( size )      esMEMS_Malloc( 0, size )
#define t_free( p )           esMEMS_Mfree( 0, p )

#define g_memcpy              eLIBs_memcpy
#define g_memset              eLIBs_memset
#define g_memmove             eLIBs_memmove
#define g_strcmp              eLIBs_strcmp
#define g_strncmp             eLIBs_strncmp
#define g_strcat                    strcat
#define g_strncat                   strncat
#define g_strcpy              eLIBs_strcpy
#define g_strncpy             eLIBs_strncpy
#define g_strlen              eLIBs_strlen
#define g_toupper             eLIBs_toupper
#define g_strtol              eLIBs_strtol


#define G_FILE                ES_FILE
#define DIR         		  ES_DIR
#define DIRENT      		  ES_DIRENT
/* file operaton interfaces */
#define t_fopen               eLIBs_fopen
#define t_fclose              eLIBs_fclose
#define g_fseek               eLIBs_fseek
#define g_remove              eLIBs_remove
#define g_rename              eLIBs_rename
#define g_fread               eLIBs_fread
#define g_fwrite              eLIBs_fwrite
#define g_ftell               eLIBs_ftell
#define g_mkdir               eLIBs_mkdir
#define g_rmdir               eLIBs_rmdir
#define g_opendir             eLIBs_opendir
#define g_closedir            eLIBs_closedir
#define g_readdir             eLIBs_readdir
#define g_dirent2name         eLIBs_DirEnt2Name
#define g_dirent2attr         eLIBs_DirEnt2Attr
#define g_fioctrl             eLIBs_fioctrl
extern int32 g_feof( G_FILE *stream );


#define g_delay               esKRNL_TimeDly

#define _inline_              __inline

#define g_rand( )             esKSRV_Random( 1000 )
#define g_printf              __inf
#define g_putchar             __putc
#define __PACKED              __packed

extern __s32  folder_copy( const char *src_folder, const char *dst_folder );

/*----------------------------------------- platform end -----------------------------------------*/
#else
#error PLATFORM must be selected.
#endif

#define EVENT_OK                                  0
#define EVENT_ERR                                 1


typedef uint32  *g_mutex_t;
extern g_mutex_t g_create_mutex( void );
extern g_mutex_t g_delete_mutex( g_mutex_t mutex, uint8 *err_p );
extern void      g_pend_mutex( g_mutex_t mutex, uint8 *err_p );
extern uint8     g_post_mutex( g_mutex_t mutex );

typedef uint32     g_rwlock_t;
extern g_rwlock_t  g_create_rwlock( void );
extern g_rwlock_t  g_delete_rwlock( g_rwlock_t rwlock, uint8 *err_p );
extern void        g_pend_read ( g_rwlock_t rwlock, uint8 *err_p );
extern uint8       g_post_read ( g_rwlock_t rwlock );
extern void        g_pend_write( g_rwlock_t rwlock, uint8 *err_p );
extern uint8       g_post_write( g_rwlock_t rwlock );

typedef int32 (* TRAV_FUNC )( const char *file, void *arg );
extern int32  g_traverse_folder    ( const char *folder, TRAV_FUNC func, void *arg );
extern int32  g_scan_file_of_folder( const char *folder, TRAV_FUNC func, void *arg );
extern int32  g_scan_dir_of_folder ( const char *folder, TRAV_FUNC func, void *arg );
extern int32  file_has_exist( const char *file );
extern __Bool dir_has_existed( const char *folder );
extern int32  g_delete_folder_all( const char *folder );


#if defined( TEST_FILE_MEM_LEAK )
extern uint32   file_count;
extern G_FILE  *g_fopen( const char *f, const char *m );
extern int32    g_fclose( G_FILE *fp );
#else
#define g_fopen           t_fopen
#define g_fclose          t_fclose
#endif  // #if defined( TEST_FILE_MEM_LEAK )

#if defined( TEST_MALLOC_MEM_LEAK )
extern uint32   malloc_count;
extern void     *malloc_addr;
#define g_malloc( size )  ( malloc_addr = t_malloc( size ), \
								__msg("MALLOC ! Address is %x. at line %u of file %s. the number of malloc is %u.\n",\
								        malloc_addr, __LINE__,__FILE__, ++malloc_count), \
								malloc_addr )
#define g_free( p )       ( __msg("FREE ! Address is %x. at line %u of file %s. the number of malloc is %u.\n", \
	                                   p, __LINE__, __FILE__, --malloc_count), \
	                             t_free( p ) )
#else
#define g_malloc          t_malloc
#define g_free            t_free
#endif  // #if defined( TEST_MALLOC_MEM_LEAK )


typedef struct _recursion_depth_info_t
{
    uint32   cur_depth;
    uint32   max_depth;
} recursion_depth_info_t;



#endif     //  ifndef __G_os_related_h

/* end of G_os_related.h  */
