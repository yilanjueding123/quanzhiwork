/*
************************************************************************************************************************
*                                                     G_os_related
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : G_os_related.c
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.07.16
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
* Gary.Wang      2008.07.16       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef __G_os_related_c
#define __G_os_related_c


#include "G_os_related.h"
#include "G_debug.h"



static void extract_dir( const char *file, char *dir )
{
    const char *r;
    const char *p;
    char       *q;

    r = file + g_strlen(file);
    while( *r-- != '\\' )
        ;

    for( q = dir, p = file;  p <= r;  )
        *q++ = *p++;
    *q = '\0';
}


#if defined( TEST_FILE_MEM_LEAK )
uint32  file_count = 0;
G_FILE  *g_fopen( const char *f, const char *m )
{
    G_FILE *fp;
    fp = t_fopen( f, m );
    if( fp != NULL )
        __msg("OPEN FILE %s. The number of file open is %u.\n", f, ++file_count);
    return fp;
}
int32 g_fclose( G_FILE *fp )
{
    __msg("CLOSE FILE ! The number of file open is %u.\n", --file_count);
    return t_fclose( fp );
}
#endif  // #if defined( TEST_FILE_MEM_LEAK )


#if defined( TEST_MALLOC_MEM_LEAK )
uint32  malloc_count = 0;
void     *malloc_addr = NULL;
#endif  // #if defined( TEST_MALLOC_MEM_LEAK )





/*---------------------------------------- WINDOWS OS ----------------------------------------*/
#if ( EPDK_OS == EPDK_OS_WINXP ) || ( EPDK_OS == EPDK_OS_WIN2K ) || ( EPDK_OS == EPDK_OS_VISTA )



size_t g_fread(void *ptr, size_t size, size_t nobj, G_FILE *stream)
{
    fseek( stream, 0, SEEK_CUR );
    return fread( ptr, size, nobj, stream );
}

size_t g_fwrite(const void *ptr, size_t size, size_t nobj, G_FILE *stream)
{
    fseek( stream, 0, SEEK_CUR );
    return fwrite( ptr, size, nobj, stream );
}


#include <direct.h>
#include <windows.h>
int32 g_mkdir( const char *dirname )
{
    if (0 == _mkdir(dirname))
        return 0;

    return -1;
}


int32 g_rmdir( const char *dirname )
{
    if (!dirname)
        return __LINE__;

    if (0 == g_strlen(dirname))
        return __LINE__;

    if (RemoveDirectory(dirname))
        return 0;
    else
        return __LINE__;
}



/*----------------------------------- mutex ----------------------------------*/
g_mutex_t g_create_mutex( void )
{
    return (g_mutex_t)CreateMutex( NULL, 0, NULL);
}

g_mutex_t g_delete_mutex( g_mutex_t mutex, uint8 *err_p )
{
    if( CloseHandle( (HANDLE)mutex )  ==  0 )
    {
        *err_p = EVENT_ERR;
        return mutex;
    }
    else
    {
        *err_p = EVENT_OK;
        return NULL;
    }
}

void      g_pend_mutex( g_mutex_t mutex, uint8 *err_p )
{
    if ( WaitForSingleObject( (HANDLE)mutex, INFINITE ) != WAIT_OBJECT_0 )
        *err_p = EVENT_ERR;
    else
        *err_p = EVENT_OK;
}

uint8     g_post_mutex( g_mutex_t mutex )
{
    if( ReleaseMutex( (HANDLE)mutex )  ==  0 )
        return EVENT_ERR;
    else
        return EVENT_OK;
}




/*----------------------------------- rwlock ---------------------------------*/
g_rwlock_t  g_create_rwlock( void )
{
    return (g_rwlock_t)1;
}

g_rwlock_t  g_delete_rwlock( g_rwlock_t rwlock, uint8 *err_p )
{
    if( err_p != NULL )
        *err_p = EVENT_OK;
    return rwlock;
}

void g_pend_read ( g_rwlock_t rwlock, uint8 *err_p )
{
    if( err_p != NULL )
        *err_p = EVENT_OK;
}

uint8 g_post_read ( g_rwlock_t rwlock )
{
    return EVENT_OK;
}

void  g_pend_write( g_rwlock_t rwlock, uint8 *err_p )
{
    if( err_p != NULL )
        *err_p = EVENT_OK;
}

uint8 g_post_write( g_rwlock_t rwlock )
{
    return EVENT_OK;
}


#include <stdio.h>
#pragma comment(lib,"kernel32.lib")

static uint32 __IsDirectory(WIN32_FIND_DATA *pfinddata)
{
    if (!pfinddata )
        return __LINE__;

    if ((pfinddata->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        return 0;

    return __LINE__;
}


static uint32 __IsDot(WIN32_FIND_DATA *pfinddata)
{
    if (!pfinddata )
        return __LINE__;

    if (0 != __IsDirectory(pfinddata))
        return __LINE__;

    if (pfinddata->cFileName[0] == '.')
    {
        if (pfinddata->cFileName[1] == '\0' ||
                (pfinddata->cFileName[1] == '.' && pfinddata->cFileName[2] == '\0'))
        {
            return 0;
        }
    }

    return __LINE__;
}

static uint32 __IsDotDot(WIN32_FIND_DATA *pfinddata)
{
    if (!pfinddata )
        return __LINE__;

    if (0 != __IsDirectory(pfinddata))
        return __LINE__;

    if (pfinddata->cFileName[0] == '.' &&
            pfinddata->cFileName[1] == '.' &&
            pfinddata->cFileName[2] == '\0')
    {
        return 0;
    }

    return __LINE__;
}


int32 g_traverse_folder( const char *folder, TRAV_FUNC func, void *arg )
{
    HANDLE hFind = NULL;
    WIN32_FIND_DATA finddata;
    //char buffer[2048];
    char finddir[MAX_PATH];
    char findmode[MAX_PATH];
    char fullpath[MAX_PATH];

    //逐个目录遍历
    g_memset(finddir, 0, MAX_PATH);
    g_strcpy(finddir, (char *)folder);

    g_memset(findmode, 0, MAX_PATH);
    sprintf(findmode, "%s\\*.*", folder);//

    g_memset(&finddata, 0, sizeof(WIN32_FIND_DATA));
    lstrcpy(finddata.cFileName, findmode);

    hFind = FindFirstFile(findmode, &finddata);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        __wrn("Fail. At line %u of file %s.\n", __LINE__, __FILE__);
        return __LINE__;
    }
    //Start to find file
    while(1)
    {
        g_memset(fullpath, 0, MAX_PATH);
        sprintf(fullpath, "%s\\%s", finddir,  finddata.cFileName);

        if (0 == __IsDotDot(&finddata))
        {
            //			__msg("find %s.\n", fullpath );
        }
        else if (0 == __IsDot(&finddata))
        {
            //			__msg("find %s.\n", fullpath );
        }
        else if (0 == __IsDirectory(&finddata))
        {
            //			__msg("find %s.\n", fullpath );
            if( g_traverse_folder( fullpath, func, arg ) )
                __msg("Fail in traversing folder %s.\n", fullpath);
        }
        else  //file
        {
            if( 0 != func( fullpath, arg ) )
            {
                __wrn("Fail. At line %u of file %s.\n", __LINE__, __FILE__);
                return -1;
            }
        }

        if (!FindNextFile(hFind, &finddata))
        {
            if (GetLastError() != ERROR_NO_MORE_FILES)
            {
                char str[MAX_PATH];
                sprintf(str, "__traversal FindNextFile failed %d", GetLastError());
                //::OutputDebugString(str);
            }
            break;
        }
    }
    FindClose(hFind);
    hFind = NULL;

    return 0;
}


int32  g_scan_file_of_folder( const char *folder, TRAV_FUNC func, void *arg )
{
    HANDLE hFind = NULL;
    WIN32_FIND_DATA finddata;
    //char buffer[2048];
    char finddir[MAX_PATH];
    char findmode[MAX_PATH];
    char fullpath[MAX_PATH];

    //逐个目录遍历
    g_memset(finddir, 0, MAX_PATH);
    g_strcpy(finddir, (char *)folder);

    g_memset(findmode, 0, MAX_PATH);
    sprintf(findmode, "%s\\*.*", folder);//

    g_memset(&finddata, 0, sizeof(WIN32_FIND_DATA));
    lstrcpy(finddata.cFileName, findmode);

    hFind = FindFirstFile(findmode, &finddata);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        return __LINE__;
    }
    //Start to find file
    while(1)
    {
        g_memset(fullpath, 0, MAX_PATH);
        sprintf(fullpath, "%s\\%s", finddir,  finddata.cFileName);

        if (   0 != __IsDotDot(&finddata)
                && 0 != __IsDot(&finddata)
                && 0 != __IsDirectory(&finddata) )
        {
            if( 0 != func( fullpath, arg ) )
                return -1;
        }
        if (!FindNextFile(hFind, &finddata))
        {
            if (GetLastError() != ERROR_NO_MORE_FILES)
            {
                char str[MAX_PATH];
                sprintf(str, "__traversal FindNextFile failed %d", GetLastError());
                //::OutputDebugString(str);
            }
            break;
        }
    }
    FindClose(hFind);
    hFind = NULL;

    return 0;
}


int32  g_scan_dir_of_folder( const char *folder, TRAV_FUNC func, void *arg )
{
    HANDLE hFind = NULL;
    WIN32_FIND_DATA finddata;
    //char buffer[2048];
    char finddir[MAX_PATH];
    char findmode[MAX_PATH];

    //逐个目录遍历
    g_memset(finddir, 0, MAX_PATH);
    g_strcpy(finddir, (char *)folder);

    g_memset(findmode, 0, MAX_PATH);
    sprintf(findmode, "%s\\*.*", folder);//

    g_memset(&finddata, 0, sizeof(WIN32_FIND_DATA));
    lstrcpy(finddata.cFileName, findmode);

    hFind = FindFirstFile(findmode, &finddata);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        return __LINE__;
    }
    //Start to find file
    while(1)
    {
        if (   0 != __IsDotDot(&finddata)
                && 0 != __IsDot(&finddata)
                && 0 == __IsDirectory(&finddata) )
        {
            if( 0 != func( finddata.cFileName, arg ) )
                return -1;
        }

        if (!FindNextFile(hFind, &finddata))
        {
            if (GetLastError() != ERROR_NO_MORE_FILES)
            {
                char str[MAX_PATH];
                sprintf(str, "__traversal FindNextFile failed %d", GetLastError());
                //::OutputDebugString(str);
            }
            break;
        }
    }
    FindClose(hFind);
    hFind = NULL;

    return 0;
}


int32 file_has_exist( const char *file )
{
    HANDLE hFind = NULL;
    WIN32_FIND_DATA finddata;

    char finddir[MAX_PATH];
    char findmode[MAX_PATH];
    char folder[MAX_PATH];

    extract_dir( file, folder );
    //逐个目录遍历
    g_memset(finddir, 0, MAX_PATH);
    g_strcpy(finddir, (char *)folder);

    g_memset(findmode, 0, MAX_PATH);
    sprintf(findmode, "%s\\*.*", folder);//

    g_memset(&finddata, 0, sizeof(WIN32_FIND_DATA));
    lstrcpy(finddata.cFileName, findmode);

    hFind = FindFirstFile(findmode, &finddata);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        return 0;
    }
    //Start to find file
    while(1)
    {
        if (   0 != __IsDotDot(&finddata)
                && 0 != __IsDot(&finddata)
                && 0 != __IsDirectory(&finddata) )
        {
            if( g_strcmp( finddata.cFileName, file + g_strlen(folder) + 1 )  ==  0 )
                return 1;
        }

        if (!FindNextFile(hFind, &finddata))
        {
            if (GetLastError() != ERROR_NO_MORE_FILES)
            {
                char str[MAX_PATH];
                sprintf(str, "__traversal FindNextFile failed %d", GetLastError());
                //::OutputDebugString(str);
            }
            break;
        }
    }
    FindClose(hFind);
    hFind = NULL;

    return 0;
}


int32 g_delete_folder_all( const char *folder )
{
    HANDLE hFind = NULL;
    WIN32_FIND_DATA finddata;
    //char buffer[2048];
    char finddir[MAX_PATH];
    char findmode[MAX_PATH];
    char fullpath[MAX_PATH];

    //逐个目录遍历
    g_memset(finddir, 0, MAX_PATH);
    g_strcpy(finddir, (char *)folder);

    g_memset(findmode, 0, MAX_PATH);
    sprintf(findmode, "%s\\*.*", folder);//

    g_memset(&finddata, 0, sizeof(WIN32_FIND_DATA));
    lstrcpy(finddata.cFileName, findmode);

    hFind = FindFirstFile(findmode, &finddata);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        return __LINE__;
    }
    //Start to find file
    while(1)
    {
        g_memset(fullpath, 0, MAX_PATH);
        sprintf(fullpath, "%s\\%s", finddir,  finddata.cFileName);

        if (0 == __IsDotDot(&finddata))
        {
            //			__msg("find %s.\n", fullpath );
        }
        else if (0 == __IsDot(&finddata))
        {
            //			__msg("find %s.\n", fullpath );
        }
        else if (0 == __IsDirectory(&finddata))
        {
            //			__msg("find %s.\n", fullpath );
            if( g_delete_folder_all( fullpath ) )
                return __LINE__;
        }
        else  //file
        {
            /* remove file */
            if( g_remove( fullpath ) )
            {
                __wrn("Fail in removing file %s. At line %u of file %s.\n", fullpath, __LINE__, __FILE__);
                return __LINE__;
            }
        }

        if (!FindNextFile(hFind, &finddata))
        {
            if (GetLastError() != ERROR_NO_MORE_FILES)
            {
                char str[MAX_PATH];
                sprintf(str, "__traversal FindNextFile failed %d", GetLastError());
                //::OutputDebugString(str);
            }
            break;
        }
    }
    FindClose(hFind);
    hFind = NULL;

    /* remove the empty folder */
    if( g_rmdir( folder ) )
    {
        __wrn("Fail in removing dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);
        return __LINE__;
    }

    return 0;
}



__Bool  dir_has_existed( const char *folder )
{
    HANDLE hFind = NULL;
    WIN32_FIND_DATA finddata;
    //char buffer[2048];
    char finddir[MAX_PATH];
    char findmode[MAX_PATH];

    //逐个目录遍历
    g_memset(finddir, 0, MAX_PATH);
    g_strcpy(finddir, (char *)folder);

    g_memset(findmode, 0, MAX_PATH);
    sprintf(findmode, "%s\\*.*", folder);//

    g_memset(&finddata, 0, sizeof(WIN32_FIND_DATA));
    lstrcpy(finddata.cFileName, findmode);

    hFind = FindFirstFile(findmode, &finddata);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        return __FALSE;
    }
    else
    {
        FindClose(hFind);
        return __TRUE;
    }
}





/*----------------------------------------- EPOS OS -------------------------------------------*/
#elif ( EPDK_OS == EPDK_OS_EPOS)


int32 g_feof( G_FILE *stream )
{
    return ( eLIBs_ferror( stream ) == FSYS_ERR_EOF );
}


/*----------------------------------- mutex ----------------------------------*/
g_mutex_t g_create_mutex( void )
{
    return (g_mutex_t)esKRNL_SemCreate( 1 );
}

g_mutex_t g_delete_mutex( g_mutex_t mutex, uint8 *err_p )
{
    __krnl_event_t *temp;

    temp = esKRNL_SemDel( (__krnl_event_t *)mutex, OS_DEL_NO_PEND, err_p );
    if( *err_p != EVENT_OK )
        *err_p = EVENT_ERR;
    else
        *err_p = EVENT_OK;

    return (g_mutex_t)temp;
}

void      g_pend_mutex( g_mutex_t mutex, uint8 *err_p )
{
    esKRNL_SemPend( (__krnl_event_t *)mutex, 0, err_p );
    if( *err_p != EVENT_OK )
        *err_p = EVENT_ERR;
    else
        *err_p = EVENT_OK;
}

uint8     g_post_mutex( g_mutex_t mutex )
{
    if( esKRNL_SemPost( (__krnl_event_t *)mutex ) != EVENT_OK )
        return EVENT_ERR;
    else
        return EVENT_OK;
}



/*----------------------------------- rwlock ---------------------------------*/
typedef struct _rwlock_i_t
{
    int32  lock_n;
    uint32 write_waiter_n;
    uint32 read_waiter_n;
    g_mutex_t lock_n_mutex;
    g_mutex_t reader_event;
    g_mutex_t writer_event;
} rw_lock_i_t, *HLCK;


g_rwlock_t  g_create_rwlock( void )
{
    HLCK   hlck;
    uint8  err;

    hlck = (HLCK)g_malloc( sizeof(rw_lock_i_t) );
    if( hlck == NULL )
        return NULL;

    hlck->lock_n         = 0;
    hlck->write_waiter_n = 0;
    hlck->read_waiter_n  = 0;
    hlck->lock_n_mutex = g_create_mutex( );
    if( hlck->lock_n_mutex == NULL )
        goto error;
    hlck->reader_event = g_create_mutex( );
    if( hlck->reader_event == NULL )
        goto error;
    hlck->writer_event = g_create_mutex( );
    if( hlck->writer_event == NULL )
        goto error;

    g_pend_mutex( hlck->reader_event, &err );
    if( err != EVENT_OK )
        goto error;
    g_pend_mutex( hlck->writer_event, &err );
    if( err != EVENT_OK )
    {
        g_post_mutex( hlck->reader_event );
        goto error;
    }

    return (g_rwlock_t)hlck;

error:
    g_free( hlck );
    hlck = NULL;
    return NULL;
}

g_rwlock_t  g_delete_rwlock( g_rwlock_t rwlock, uint8 *err_p )
{
    uint8  err;
    HLCK   hlck;

    hlck = (HLCK)rwlock;

    if( rwlock == NULL )
    {
        err = EVENT_ERR;
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }

    if( g_delete_mutex( hlck->lock_n_mutex, &err ) != NULL )
    {
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }
    if( g_delete_mutex( hlck->reader_event, &err ) != NULL )
    {
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }
    if( g_delete_mutex( hlck->writer_event, &err ) != NULL )
    {
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }

    g_free( hlck );
    if( err_p != NULL )
        *err_p = EVENT_OK;
    return NULL;

error:
    if( err_p != NULL )
        *err_p = err;
    return rwlock;
}

static int32 wait_reader( HLCK hlck )
{
    uint8  err;

    g_pend_mutex( hlck->lock_n_mutex, &err );
    if( err != EVENT_OK )
    {
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        return 1;
    }

    // One more thread is waiting on read event
    hlck->read_waiter_n++;

    err = g_post_mutex(hlck->lock_n_mutex);
    if( err != EVENT_OK )
    {
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        return 1;
    }

    return 0;
}

static int32 increase_reader( HLCK hlck )
{
    uint8  err;

    // InterlockedIncrement() could also be used, instead of this mutex
    g_pend_mutex( hlck->lock_n_mutex, &err);
    if( err != EVENT_OK )
    {
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        return 1;
    }

    // One more thread has obtained a read lock and is no longer waiting
    if ( hlck->lock_n >= 0 )
    {
        hlck->lock_n++;
        hlck->read_waiter_n--;
    }

    err = g_post_mutex( hlck->lock_n_mutex );
    if( err != EVENT_OK )
    {
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        return 1;
    }

    return 0;
}


void g_pend_read ( g_rwlock_t rwlock, uint8 *err_p )
{
    uint8  err;
    HLCK   hlck;

    hlck = (HLCK)rwlock;

    if( rwlock == NULL )
    {
        err = EVENT_ERR;
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }

    if ( wait_reader( hlck )  != 0 )
    {
        err = EVENT_ERR;
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }

    if ( hlck->lock_n < 0 )
    {
        // a reader thread must wait for the read event.. if a writer is holding lock
        g_pend_mutex( hlck->reader_event, &err );
        if( err != EVENT_OK )
        {
            __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
            goto error;
        }
    }

    if ( increase_reader( hlck )  != 0 )
    {
        err = EVENT_ERR;
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }

    if( err_p != NULL )
        *err_p = EVENT_OK;
    return;

error :
    if( err_p != NULL )
        *err_p = err;
}

static uint32 decrease_reader( HLCK hlck )
{
    uint8  err;

    g_pend_mutex( hlck->lock_n_mutex, &err );
    if( err != EVENT_OK )
    {
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        return 1;
    }

    if (hlck->lock_n > 0)
    {
        // One less thread holding read lock
        hlck->lock_n--;
    }

    err = g_post_mutex( hlck->lock_n_mutex );
    if( err != EVENT_OK )
    {
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        return 1;
    }

    return 0;
}


uint8 g_post_read( g_rwlock_t rwlock )
{
    HLCK   hlck;
    uint8  err;

    hlck = (HLCK)rwlock;

    if( rwlock == NULL )
    {
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        return EVENT_ERR;
    }

    if ( decrease_reader( hlck ) != 0 )
    {
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        return EVENT_ERR;
    }

    if ( !hlck->read_waiter_n && !hlck->lock_n )
    {
        err = g_post_mutex( hlck->writer_event ); // will release one waiting writer
        if( err != EVENT_OK )
        {
            __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
            return EVENT_ERR;
        }
    }

    return EVENT_OK;
}


static uint32 waitingWriter( HLCK hlck )
{
    uint8  err;

    g_pend_mutex( hlck->lock_n_mutex, &err );
    if( err != EVENT_OK )
    {
        __wrn("Fail in pending mutex. At line %u of file %s.\n", __LINE__, __FILE__);
        return 1;
    }

    hlck->write_waiter_n++;

    err = g_post_mutex( hlck->lock_n_mutex );
    if( err != EVENT_OK )
    {
        __wrn("Fail in posting mutex. At line %u of file %s.\n", __LINE__, __FILE__);
        return 1;
    }

    return 0;
}

static uint32 increase_writer( HLCK hlck )
{
    uint8  err;

    g_pend_mutex( hlck->lock_n_mutex, &err );
    if( err != EVENT_OK )
    {
        __wrn("Fail in pending mutex. At line %u of file %s.\n", __LINE__, __FILE__);
        return 1;
    }
    // Only one thread can obtain a write lock .. so lock count goes underground
    if ( hlck->lock_n == 0 )
    {
        hlck->lock_n = -1;
        hlck->write_waiter_n--;
    }

    err = g_post_mutex( hlck->lock_n_mutex );
    if( err != EVENT_OK )
    {
        __wrn("Fail in posting mutex. At line %u of file %s.\n", __LINE__, __FILE__);
        return 1;
    }

    return 0;
}

void  g_pend_write( g_rwlock_t rwlock, uint8 *err_p )
{
    uint8  err;
    HLCK   hlck;

    hlck = (HLCK)rwlock;

    if( rwlock == NULL )
    {
        err = EVENT_ERR;
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }

    if ( waitingWriter( hlck )  != 0)
    {
        err = EVENT_ERR;
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }

    // writers compete with readers and fellow-writers ...
    while ( hlck->lock_n )
    {
        g_pend_mutex( hlck->writer_event, &err );
        if( err != EVENT_OK )
        {
            __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
            goto error;
        }
    }

    if ( increase_writer( hlck )  != 0 )
    {
        err = EVENT_ERR;
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }

    if( err_p != NULL )
        *err_p = EVENT_OK;
    return;

error :
    if( err_p != NULL )
        *err_p = err;
}

static uint32 decreame_writer( HLCK hlck )
{
    uint8  err;

    g_pend_mutex( hlck->lock_n_mutex, &err );
    if( err != EVENT_OK )
    {
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        return 1;
    }
    if ( hlck->lock_n == -1 )
    {
        hlck->lock_n = 0;
    }

    err = g_post_mutex( hlck->lock_n_mutex );
    if( err != EVENT_OK )
    {
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        return 1;
    }

    return 0;
}

uint8 g_post_write( g_rwlock_t rwlock )
{
    uint8  err;
    HLCK   hlck;

    hlck = (HLCK)rwlock;

    if( rwlock == NULL )
    {
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        return EVENT_ERR;
    }

    if ( decreame_writer( hlck )  != 0)
    {
        __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
        return EVENT_ERR;
    }

    // Readers are preferred in the following section....
    if ( hlck->write_waiter_n && !hlck->read_waiter_n )
    {
        // when no readers are waiting... signal writers, if any
        err = g_post_mutex( hlck->writer_event ); // will release one waiting writer
        if( err != EVENT_OK )
        {
            __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
            return EVENT_ERR;
        }
    }
    else
    {
        while ( hlck->read_waiter_n > 0 )
        {
            err = g_post_mutex(hlck->reader_event); // will release ALL waiting readers
            if( err != EVENT_OK )
            {
                __wrn("Fail! At line %u of file %s.\n", __LINE__, __FILE__);
                return EVENT_ERR;
            }
        }
    }

    return EVENT_OK;
}




static int32 is_dot( const char *name )
{
    if( name[0] == '.'  &&  name[1] != '.' )
        return 1;
    else
        return 0;
}

static int32 is_dotdot( const char *name )
{
    if( name[0] == '.'  &&  name[1] == '.' )
        return 1;
    else
        return 0;
}


static void parse_dir_name( const char *dir, char *out )
{
    /*
    	if( dir[8] == '.' )                 // 8.3 format
    	{
    		for( p = out, i = 0;  i < 8+3+1+1;  i++ )
    		{
    			if( dir[i] != ' ' )
    				*p++ = dir[i];
    		}
    	}
    	else
    */		g_strcpy( out, dir );
    //	__msg("input is %s and out is %s.\n", dir, out );
}


int32 g_traverse_folder( const char *folder, TRAV_FUNC func, void *arg )
{
    uint32  attr;
    DIR    *pdir = NULL;
    DIRENT *pdirent = NULL;
    //	char   new_dir_file[FS_FILE_NAME_MAX_SIZE+1];
    //	char   full_dir_file[FS_FULL_PATH_MAX_SIZE+1];
    char   *new_dir_file = NULL;
    char   *full_dir_file = NULL;
    recursion_depth_info_t  *depth_info_p = NULL;
    char   *buf = NULL;
    uint32  size;

    /* judge depth */
    depth_info_p = (recursion_depth_info_t *)arg;
    if( depth_info_p != NULL )
    {
        depth_info_p->cur_depth++;
        if( depth_info_p->cur_depth > depth_info_p->max_depth )
        {
            __wrn("Fail in traversing. too deep, beyond max depth %d. At line %u of file %s.\n",
                  depth_info_p->max_depth, __LINE__, __FILE__);
            goto exit;
        }
    }

    size = (FS_FILE_NAME_MAX_SIZE + 1) + (FS_FULL_PATH_MAX_SIZE + 1);
    buf = g_malloc( size );
    if( buf == NULL )
        goto exit;
    g_memset( buf, '\0', size );

    new_dir_file  = buf;
    full_dir_file = new_dir_file + (FS_FILE_NAME_MAX_SIZE + 1);


    pdir = g_opendir( folder );
    if( pdir == NULL )
    {
        __wrn("Fail in opening dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);
        goto exit;
    }
    //	__msg("Succeed in opening dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);

    while( 1 )
    {
        pdirent = g_readdir( pdir );
        if( pdirent == NULL )
        {
            //			__msg("Succeed in closing dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);
            goto exit;
        }
        g_dirent2attr( pdirent, &attr );
        g_dirent2name( pdirent, new_dir_file );
        //		__inf("entry name is %s.\n", new_dir_file);
        //		parse_dir_name( name, new_dir_file );
        if( g_strlen( new_dir_file ) > FS_FILE_NAME_MAX_SIZE )
            continue;
        g_strcpy( full_dir_file, folder );
        g_strcat( full_dir_file, "\\");
        g_strcat( full_dir_file, new_dir_file );
        if( attr & FSYS_ATTR_DIRECTORY )
        {
            if( !is_dot( new_dir_file )  &&  !is_dotdot( new_dir_file ) )
            {
                if(	g_traverse_folder( full_dir_file, func, arg ) != 0 )
                {
                    __wrn("Fail in traversing folder %s. At line %u of file %s.\n", full_dir_file, __LINE__, __FILE__);
                    goto exit;
                }
            }
        }
        else
        {
            if( 0 != func( full_dir_file, arg ) )
            {
                __wrn("Fail in handle file %s. At line %u of file %s.\n", full_dir_file, __LINE__, __FILE__);
                goto exit;
            }
        }
    }

exit:
    if( pdir != NULL )
    {
        g_closedir( pdir );
        pdir = NULL;
    }
    if( buf != NULL )
    {
        g_free( buf );
        buf = NULL;
    }
    if( depth_info_p != NULL )
    {
        depth_info_p->cur_depth--;
    }
    return 0;
}


static void __str_to_upper( const char *input, char *output )
{
    const char  *p;
    char  *q;

    p = input;
    q = output;
    while( *p != '\0' )
        *q++ = g_toupper( *p++ );
    *q = '\0';
}


__Bool file_has_exist( const char *file )
{
    uint32  attr;
    DIR    *pdir;
    DIRENT *pdirent;
    char   new_dir_file[FS_FILE_NAME_MAX_SIZE + 1];
    char   name[FS_FILE_NAME_MAX_SIZE + 1];
    char   upper_name[FS_FILE_NAME_MAX_SIZE + 1];
    char   upper_new_dir_file[FS_FILE_NAME_MAX_SIZE + 1];
    char   folder[FS_FULL_PATH_MAX_SIZE + 1];

    extract_dir( file, folder );
    pdir = g_opendir( folder );
    if( pdir == NULL )
    {
        __wrn("Fail in opening dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);
        return __FALSE;
    }
    //	__msg("Succeed in opening dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);

    __str_to_upper( file + g_strlen(folder) + 1, upper_name );
    while( 1 )
    {
        pdirent = g_readdir( pdir );
        if( pdirent == NULL )
        {
            //			__msg("Succeed in closing dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);
            g_closedir( pdir );
            return __FALSE;
        }
        g_dirent2attr( pdirent, &attr );
        g_dirent2name( pdirent, name );
        parse_dir_name( name, new_dir_file );
        if( !( attr & FSYS_ATTR_DIRECTORY ) )
        {
            //			__msg("find file name %s.\n", new_dir_file );
            __str_to_upper( new_dir_file, upper_new_dir_file );
            if( g_strcmp( upper_new_dir_file, upper_name )  ==  0 )
            {
                g_closedir( pdir );
                //				__msg("closing dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);
                return __TRUE;
            }
        }
    }
}


int32  g_scan_file_of_folder( const char *folder, TRAV_FUNC func, void *arg )
{
    uint32  attr;
    DIR    *pdir;
    DIRENT *pdirent;
    char   new_dir_file[FS_FILE_NAME_MAX_SIZE + 1];
    char   name[FS_FILE_NAME_MAX_SIZE + 1];
    char   full_dir_file[FS_FULL_PATH_MAX_SIZE + 1];

    g_memset( new_dir_file, '\0', sizeof(new_dir_file) );
    g_memset( full_dir_file, '\0', sizeof(full_dir_file) );
    pdir = g_opendir( folder );
    if( pdir == NULL )
    {
        __wrn("Fail in opening dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);
        return __LINE__;
    }
    //	__msg("Succeed in opening dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);

    while( 1 )
    {
        pdirent = g_readdir( pdir );
        if( pdirent == NULL )
        {
            //			__msg("Succeed in closing dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);
            g_closedir( pdir );
            return 0;
        }
        g_dirent2attr( pdirent, &attr );
        g_dirent2name( pdirent, name );
        parse_dir_name( name, new_dir_file );
        g_strcpy( full_dir_file, folder );
        g_strcat( full_dir_file, "\\");
        g_strcat( full_dir_file, new_dir_file );
        if( !( attr & FSYS_ATTR_DIRECTORY ) )
        {
            if(	0 != func( full_dir_file, arg ) )
            {
                g_closedir( pdir );
                //				__msg("closing dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);
                return __LINE__;
            }
        }
    }
}


int32  g_scan_dir_of_folder( const char *folder, TRAV_FUNC func, void *arg )
{
    uint32  attr;
    DIR    *pdir;
    DIRENT *pdirent;
    char   new_dir_file[FS_FILE_NAME_MAX_SIZE + 1];
    char   name[FS_FILE_NAME_MAX_SIZE + 1];

    g_memset( new_dir_file, '\0', sizeof(new_dir_file) );
    pdir = g_opendir( folder );
    if( pdir == NULL )
    {
        __wrn("Fail in opening dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);
        return __LINE__;
    }
    //	__msg("Succeed in opening dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);

    while( 1 )
    {
        pdirent = g_readdir( pdir );
        if( pdirent == NULL )
        {
            //			__msg("Succeed in closing dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);
            g_closedir( pdir );
            return 0;
        }
        g_dirent2attr( pdirent, &attr );
        g_dirent2name( pdirent, name );
        parse_dir_name( name, new_dir_file );
        if( ( attr & FSYS_ATTR_DIRECTORY ) )
        {
            if( !is_dot( new_dir_file )  &&  !is_dotdot( new_dir_file ) )
            {
                if( 0 != func( new_dir_file, arg ) )
                {
                    __wrn("Fail. At line %u of file %s.\n", __LINE__, __FILE__);
                    g_closedir( pdir );
                    //					__msg("closing dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);
                    return __LINE__;
                }
            }
        }
    }
}



int32 g_delete_folder_all( const char *folder )
{
    uint32  attr;
    DIR    *pdir;
    DIRENT *pdirent;
    char   new_dir_file[FS_FILE_NAME_MAX_SIZE + 1];
    char   name[FS_FILE_NAME_MAX_SIZE + 1];
    char   full_dir_file[FS_FULL_PATH_MAX_SIZE + 1];

    g_memset( new_dir_file, '\0', sizeof(new_dir_file) );
    g_memset( full_dir_file, '\0', sizeof(full_dir_file) );
    pdir = g_opendir( folder );
    if( pdir == NULL )
    {
        __wrn("Fail in opening dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);
        return __LINE__;
    }
    //	__inf("open foldr \"%s\" ok.\n", folder);

    while( 1 )
    {
        pdirent = g_readdir( pdir );
        if( pdirent == NULL )
        {
            g_closedir( pdir );
            //			__msg("Succeed in closing dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);
            /* delete the empty folder */
            if( g_rmdir( folder ) )
            {
                __wrn("Fail in removing dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);
                return __LINE__;
            }
            return 0;
        }
        g_dirent2attr( pdirent, &attr );
        g_dirent2name( pdirent, name );
        //		__inf("entry name is %s.\n", name);
        parse_dir_name( name, new_dir_file );
        //		__inf("converted name is %s.\n", new_dir_file);
        g_strcpy( full_dir_file, folder );
        g_strcat( full_dir_file, "\\");
        g_strcat( full_dir_file, new_dir_file );
        if( attr & FSYS_ATTR_DIRECTORY )
        {
            if( !is_dot( new_dir_file )  &&  !is_dotdot( new_dir_file ) )
            {
                //				__inf("\"%s\" is folder.\n", full_dir_file);
                if(	g_delete_folder_all( full_dir_file ) != 0 )
                {
                    g_closedir( pdir );
                    //					__msg("closing dir %s. At line %u of file %s.\n", folder, __LINE__, __FILE__);
                    return __LINE__;
                }
                //				else
                //					__inf("Succeed in Deleting dir %s. At line %u of file %s.\n", full_dir_file, __LINE__, __FILE__);
            }
        }
        else                            // is file
        {
            /* delete files */
            if( g_remove( full_dir_file ) )
            {
                __wrn("Fail in removing file %s. At line %u of file %s.\n", full_dir_file, __LINE__, __FILE__);
                return __LINE__;
            }
            //			else
            //				__inf("Succeed in Deleting file %s. At line %u of file %s.\n", full_dir_file, __LINE__, __FILE__);
        }
    }
}



__Bool  dir_has_existed( const char *folder )
{
    DIR    *pdir;

    pdir = g_opendir( folder );
    if( pdir == NULL )
    {
        return __FALSE;
    }
    else
    {
        g_closedir( pdir );
        return __TRUE;
    }
}



static __s32  create_folder( const char *path )
{
    DIR    *pdir = NULL;

    pdir = g_opendir( path );
    if( pdir == NULL )                  // the folder doesn't exist
    {
        if( g_mkdir( path ) )
            return -1;
        else
            return 0;
    }
    else
    {
        g_closedir( pdir );
        pdir = NULL;
        return 0;
    }
}




static __s32  file_copy( const char *src_file, const char *dst_file )
{
#define TEMP_FILE_BUF_SIZE              SZ_8K
    char   *buf = NULL;
    uint32  size;
    G_FILE *src_fp = NULL;
    G_FILE *dst_fp = NULL;
    __s32   ret;

    size = TEMP_FILE_BUF_SIZE;
    buf = g_malloc( size );
    if( buf == NULL )
    {
        ret = -1;
        goto exit;
    }

    src_fp = g_fopen( src_file, "rb" );
    if( src_fp == NULL )
    {
        ret = -1;
        goto exit;
    }

    dst_fp = g_fopen( dst_file, "wb+" );
    if( dst_fp == NULL )
    {
        ret = -1;
        goto exit;
    }

    while( !g_feof( src_fp ) )
    {
        size = g_fread( buf, 1, TEMP_FILE_BUF_SIZE, src_fp );
        g_fwrite( buf, 1, size, dst_fp );
    }

    ret = 0;

exit:
    if( dst_fp != NULL )
    {
        g_fclose( dst_fp );
        dst_fp = NULL;
    }
    if( src_fp != NULL )
    {
        g_fclose( src_fp );
        src_fp = NULL;
    }
    if( buf != NULL )
    {
        g_free( buf );
        buf = NULL;
    }
    return ret;
}



__s32  folder_copy( const char *src_folder, const char *dst_folder )
{
    uint32  attr;
    DIR    *pdir = NULL;
    DIRENT *pdirent = NULL;
    char   *new_dir_file = NULL;
    char   *src_full_dir_file = NULL;
    char   *dst_full_dir_file = NULL;
    char   *buf = NULL;
    uint32  size;
    __s32   ret;

    if( src_folder == NULL || dst_folder == NULL )
        return -1;

    size = (FS_FILE_NAME_MAX_SIZE + 1) + (FS_FULL_PATH_MAX_SIZE + 1) * 2;
    buf = g_malloc( size );
    if( buf == NULL )
    {
        ret = -1;
        goto exit;
    }
    g_memset( buf, '\0', size );

    new_dir_file  = buf;
    src_full_dir_file = new_dir_file + (FS_FILE_NAME_MAX_SIZE + 1);
    dst_full_dir_file = src_full_dir_file + (FS_FULL_PATH_MAX_SIZE + 1);

    /* create the corresponding src_folder */
    if( create_folder( dst_folder ) )
    {
        ret = -1;
        goto exit;
    }

    pdir = g_opendir( src_folder );
    if( pdir == NULL )
    {
        __wrn("Fail in opening dir %s. At line %u of file %s.\n", src_folder, __LINE__, __FILE__);
        ret = 0;
        goto exit;
    }
    //	__msg("Succeed in opening dir %s. At line %u of file %s.\n", src_folder, __LINE__, __FILE__);

    while( 1 )
    {
        pdirent = g_readdir( pdir );
        if( pdirent == NULL )
        {
            //			__msg("Succeed in closing dir %s. At line %u of file %s.\n", src_folder, __LINE__, __FILE__);
            ret = 0;
            goto exit;
        }
        g_dirent2attr( pdirent, &attr );
        g_dirent2name( pdirent, new_dir_file );
        if( g_strlen( new_dir_file ) > FS_FILE_NAME_MAX_SIZE )
            continue;

        g_strcpy( src_full_dir_file, src_folder );
        g_strcat( src_full_dir_file, "\\");
        g_strcat( src_full_dir_file, new_dir_file );

        g_strcpy( dst_full_dir_file, dst_folder );
        g_strcat( dst_full_dir_file, "\\");
        g_strcat( dst_full_dir_file, new_dir_file );

        if( attr & FSYS_ATTR_DIRECTORY )
        {
            if( !is_dot( new_dir_file )  &&  !is_dotdot( new_dir_file ) )
            {
                if(	folder_copy( src_full_dir_file, dst_full_dir_file ) != 0 )
                {
                    __wrn("Fail in copying src_folder %s. At line %u of file %s.\n", src_full_dir_file, __LINE__, __FILE__);
                    ret = -1;
                    goto exit;
                }
            }
        }
        else
        {
            file_copy( src_full_dir_file, dst_full_dir_file );
        }
    }

exit:
    if( pdir != NULL )
    {
        g_closedir( pdir );
        pdir = NULL;
    }
    if( buf != NULL )
    {
        g_free( buf );
        buf = NULL;
    }
    return ret;
}




/*----------------------------------------- platform end -----------------------------------------*/
#else
#error PLATFORM must be selected.
#endif







#endif     //  ifndef __G_os_related_c

/* end of G_os_related.c  */
