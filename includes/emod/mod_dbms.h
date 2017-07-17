/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : mod_dbms.h
* By      : Sunny
* Version : v1.0
* Date    : 2010-9-16
* Descript: melis sqlite3 external functions interface.
* Update  : date                auther      ver     notes
*           2010-9-16 19:15:11  Sunny       1.0     create this file.
*********************************************************************************************************
*/

#ifndef __MOD_DBMS_H__
#define __MOD_DBMS_H__

#include "typedef.h"
#include "cfgs.h"

/*
** CAPI3REF: Result Codes {H10210} <S10700>
** KEYWORDS: SQLITE_OK {error code} {error codes}
** KEYWORDS: {result code} {result codes}
**
** Many SQLite functions return an integer result code from the set shown
** here in order to indicates success or failure.
**
** New error codes may be added in future versions of SQLite.
**
** See also: [SQLITE_IOERR_READ | extended result codes]
*/
#define SQLITE_OK           0   /* Successful result */
/* beginning-of-error-codes */
#define SQLITE_ERROR        1   /* SQL error or missing database */
#define SQLITE_INTERNAL     2   /* Internal logic error in SQLite */
#define SQLITE_PERM         3   /* Access permission denied */
#define SQLITE_ABORT        4   /* Callback routine requested an abort */
#define SQLITE_BUSY         5   /* The database file is locked */
#define SQLITE_LOCKED       6   /* A table in the database is locked */
#define SQLITE_NOMEM        7   /* A malloc() failed */
#define SQLITE_READONLY     8   /* Attempt to write a readonly database */
#define SQLITE_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
#define SQLITE_IOERR       10   /* Some kind of disk I/O error occurred */
#define SQLITE_CORRUPT     11   /* The database disk image is malformed */
#define SQLITE_NOTFOUND    12   /* NOT USED. Table or record not found */
#define SQLITE_FULL        13   /* Insertion failed because database is full */
#define SQLITE_CANTOPEN    14   /* Unable to open the database file */
#define SQLITE_PROTOCOL    15   /* NOT USED. Database lock protocol error */
#define SQLITE_EMPTY       16   /* Database is empty */
#define SQLITE_SCHEMA      17   /* The database schema changed */
#define SQLITE_TOOBIG      18   /* String or BLOB exceeds size limit */
#define SQLITE_CONSTRAINT  19   /* Abort due to constraint violation */
#define SQLITE_MISMATCH    20   /* Data type mismatch */
#define SQLITE_MISUSE      21   /* Library used incorrectly */
#define SQLITE_NOLFS       22   /* Uses OS features not supported on host */
#define SQLITE_AUTH        23   /* Authorization denied */
#define SQLITE_FORMAT      24   /* Auxiliary database format error */
#define SQLITE_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
#define SQLITE_NOTADB      26   /* File opened that is not a database file */
#define SQLITE_ROW         100  /* sqlite3_step() has another row ready */
#define SQLITE_DONE        101  /* sqlite3_step() has finished executing */
/* end-of-error-codes */

/*
** Flags For File Open Operations
**
** These bit values are intended for use in the
** 3rd parameter to the [sqlite3_open_v2()] interface and
** in the 4th parameter to the xOpen method of the
** [sqlite3_vfs] object.
*/
#define SQLITE_OPEN_READONLY         0x00000001  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_READWRITE        0x00000002  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_CREATE           0x00000004  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_DELETEONCLOSE    0x00000008  /* VFS only */
#define SQLITE_OPEN_EXCLUSIVE        0x00000010  /* VFS only */
#define SQLITE_OPEN_MAIN_DB          0x00000100  /* VFS only */
#define SQLITE_OPEN_TEMP_DB          0x00000200  /* VFS only */
#define SQLITE_OPEN_TRANSIENT_DB     0x00000400  /* VFS only */
#define SQLITE_OPEN_MAIN_JOURNAL     0x00000800  /* VFS only */
#define SQLITE_OPEN_TEMP_JOURNAL     0x00001000  /* VFS only */
#define SQLITE_OPEN_SUBJOURNAL       0x00002000  /* VFS only */
#define SQLITE_OPEN_MASTER_JOURNAL   0x00004000  /* VFS only */
#define SQLITE_OPEN_NOMUTEX          0x00008000  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_FULLMUTEX        0x00010000  /* Ok for sqlite3_open_v2() */

#define SQLITE_CONFIG_SINGLETHREAD  1  /* nil */
#define SQLITE_CONFIG_MULTITHREAD   2  /* nil */
#define SQLITE_CONFIG_SERIALIZED    3  /* nil */
#define SQLITE_CONFIG_MALLOC        4  /* sqlite3_mem_methods* */
#define SQLITE_CONFIG_GETMALLOC     5  /* sqlite3_mem_methods* */
#define SQLITE_CONFIG_SCRATCH       6  /* void*, int sz, int N */
#define SQLITE_CONFIG_PAGECACHE     7  /* void*, int sz, int N */
#define SQLITE_CONFIG_HEAP          8  /* void*, int nByte, int min */
#define SQLITE_CONFIG_MEMSTATUS     9  /* boolean */
#define SQLITE_CONFIG_MUTEX        10  /* sqlite3_mutex_methods* */
#define SQLITE_CONFIG_GETMUTEX     11  /* sqlite3_mutex_methods* */
/* previously SQLITE_CONFIG_CHUNKALLOC 12 which is now unused. */
#define SQLITE_CONFIG_LOOKASIDE    13  /* int int */
#define SQLITE_CONFIG_PCACHE       14  /* sqlite3_pcache_methods* */
#define SQLITE_CONFIG_GETPCACHE    15  /* sqlite3_pcache_methods* */
#define SQLITE_DBCONFIG_LOOKASIDE  1001  /* void* int int */

/*
** Fundamental Datatypes
** KEYWORDS: SQLITE_TEXT
**
** Every value in SQLite has one of five fundamental datatypes:
**
** <ul>
** <li> 64-bit signed integer
** <li> 64-bit IEEE floating point number
** <li> string
** <li> BLOB
** <li> NULL
** </ul> {END}
**
** These constants are codes for each of those types.
**
** Note that the SQLITE_TEXT constant was also used in SQLite version 2
** for a completely different meaning.  Software that links against both
** SQLite version 2 and SQLite version 3 should use SQLITE3_TEXT, not
** SQLITE_TEXT.
*/
#define SQLITE_INTEGER  1
#define SQLITE_FLOAT    2
#define SQLITE_BLOB     4
#define SQLITE_NULL     5
#define SQLITE_TEXT     3
#define SQLITE3_TEXT    3

/*
** CAPI3REF: Constants Defining Special Destructor Behavior {H10280} <S30100>
**
** These are special values for the destructor that is passed in as the
** final argument to routines like [sqlite3_result_blob()].  If the destructor
** argument is SQLITE_STATIC, it means that the content pointer is constant
** and will never change.  It does not need to be destroyed.  The
** SQLITE_TRANSIENT value means that the content will likely change in
** the near future and that SQLite should make its own private copy of
** the content before returning.
**
** The typedef is necessary to work around problems in certain
** C++ compilers.  See ticket #2191.
*/
typedef void (*sqlite3_destructor_type)(void *);
#define SQLITE_STATIC      ((sqlite3_destructor_type)0)
#define SQLITE_TRANSIENT   ((sqlite3_destructor_type)-1)


/*
** CAPI3REF: 64-Bit Integer Types {H10200} <S10110>
** KEYWORDS: sqlite_int64 sqlite_uint64
**
** Because there is no cross-platform way to specify 64-bit integer types
** SQLite includes typedefs for 64-bit signed and unsigned integers.
**
** The sqlite3_int64 and sqlite3_uint64 are the preferred type definitions.
** The sqlite_int64 and sqlite_uint64 types are supported for backwards
** compatibility only.
**
** Requirements: [H10201] [H10202]
*/
#ifdef SQLITE_INT64_TYPE
typedef SQLITE_INT64_TYPE         sqlite_int64;
typedef unsigned SQLITE_INT64_TYPE sqlite_uint64;
#elif defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64                   sqlite_int64;
typedef unsigned __int64          sqlite_uint64;
#else
typedef long long int             sqlite_int64;
typedef unsigned long long int    sqlite_uint64;
#endif
typedef sqlite_int64                sqlite3_int64;
typedef sqlite_uint64               sqlite3_uint64;

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;

/*
** The type for a callback function.
** This is legacy and deprecated.  It is included for historical
** compatibility and is not documented.
*/
typedef int (*sqlite3_callback)(void *, int, char **, char **);

/* SQLite module external interfaces */
typedef int             (*API_sqlite3_initialize         )(void);
typedef int             (*API_sqlite3_shutdown           )(void);
typedef int             (*API_sqlite3_config             )(int, ...);

typedef int             (*API_sqlite3_db_config          )(sqlite3 *, int op, ...);
typedef sqlite3_int64   (*API_sqlite3_last_insert_rowid  )(sqlite3 *);
typedef int             (*API_sqlite3_changes            )(sqlite3 *);

typedef int             (*API_sqlite3_busy_handler       )(sqlite3 *, int(*)(void *, int), void *);
typedef int             (*API_sqlite3_busy_timeout       )(sqlite3 *, int ms);

typedef int             (*API_sqlite3_exec               )(sqlite3 *, const char *sql, int (*callback)(void *, int, char **, char **), void *, char **errmsg);
typedef int             (*API_sqlite3_get_table          )(sqlite3 *db, const char *zSql, char ***pazResult, int *pnRow, int *pnColumn, char **pzErrmsg);
typedef void            (*API_sqlite3_free_table         )(char **result);

typedef sqlite3_int64   (*API_sqlite3_memory_used        )(void);
typedef sqlite3_int64   (*API_sqlite3_memory_highwater   )(int resetFlag);
typedef int             (*API_sqlite3_enable_shared_cache)(int);
typedef int             (*API_sqlite3_release_memory     )(int);
typedef void            (*API_sqlite3_soft_heap_limit    )(int);

typedef int             (*API_sqlite3_open_v2            )(const char *filename, sqlite3 **ppDb, int flags, const char *zVfs);
typedef int             (*API_sqlite3_close              )(sqlite3 *);

typedef int             (*API_sqlite3_errcode            )(sqlite3 *db);
typedef const char     *(*API_sqlite3_errmsg             )(sqlite3 *);

typedef int             (*API_sqlite3_bind_blob          )(sqlite3_stmt *, int, const void *, int n, void(*)(void *));
typedef int             (*API_sqlite3_bind_double        )(sqlite3_stmt *, int, double);
typedef int             (*API_sqlite3_bind_int           )(sqlite3_stmt *, int, int);
typedef int             (*API_sqlite3_bind_int64         )(sqlite3_stmt *, int, sqlite3_int64);
typedef int             (*API_sqlite3_bind_null          )(sqlite3_stmt *, int);
typedef int             (*API_sqlite3_bind_text          )(sqlite3_stmt *, int, const char *, int n, void(*)(void *));
typedef int             (*API_sqlite3_bind_text16        )(sqlite3_stmt *, int, const void *, int, void(*)(void *));
typedef int             (*API_sqlite3_bind_zeroblob      )(sqlite3_stmt *, int, int n);
typedef int             (*API_sqlite3_column_count       )(sqlite3_stmt *pStmt);
typedef const char     *(*API_sqlite3_column_name        )(sqlite3_stmt *, int N);
typedef const void     *(*API_sqlite3_column_blob        )(sqlite3_stmt *, int iCol);
typedef int             (*API_sqlite3_column_bytes       )(sqlite3_stmt *, int iCol);
typedef double          (*API_sqlite3_column_double      )(sqlite3_stmt *, int iCol);
typedef int             (*API_sqlite3_column_int         )(sqlite3_stmt *, int iCol);
typedef sqlite3_int64   (*API_sqlite3_column_int64       )(sqlite3_stmt *, int iCol);
typedef const char     *(*API_sqlite3_column_text        )(sqlite3_stmt *, int iCol);
typedef int             (*API_sqlite3_column_type        )(sqlite3_stmt *, int iCol);

typedef int             (*API_sqlite3_prepare_v2         )(sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail);
typedef int             (*API_sqlite3_reset              )(sqlite3_stmt *pStmt);
typedef int             (*API_sqlite3_step               )(sqlite3_stmt *);
typedef int             (*API_sqlite3_finalize           )(sqlite3_stmt *pStmt);

typedef struct
{
    /* init, exit, config */
    API_sqlite3_initialize           ptr_sqlite3_initialize         ;
    API_sqlite3_shutdown             ptr_sqlite3_shutdown           ;
    API_sqlite3_config               ptr_sqlite3_config             ;

    /* database */
    API_sqlite3_db_config            ptr_sqlite3_db_config          ;
    API_sqlite3_last_insert_rowid    ptr_sqlite3_last_insert_rowid  ;
    API_sqlite3_changes              ptr_sqlite3_changes            ;

    /* busy handle */
    API_sqlite3_busy_handler         ptr_sqlite3_busy_handler       ;
    API_sqlite3_busy_timeout         ptr_sqlite3_busy_timeout       ;

    /* convenient way of running one or more SQL statements */
    API_sqlite3_exec                 ptr_sqlite3_exec               ;

    /* Convenience Routines For Running Queries */
    API_sqlite3_get_table            ptr_sqlite3_get_table          ;
    API_sqlite3_free_table           ptr_sqlite3_free_table         ;

    /* memory managermant */
    API_sqlite3_memory_used          ptr_sqlite3_memory_used        ;
    API_sqlite3_memory_highwater     ptr_sqlite3_memory_highwater   ;
    API_sqlite3_enable_shared_cache  ptr_sqlite3_enable_shared_cache;
    API_sqlite3_release_memory       ptr_sqlite3_release_memory     ;
    API_sqlite3_soft_heap_limit      ptr_sqlite3_soft_heap_limit    ;

    /* open close database */
    API_sqlite3_open_v2              ptr_sqlite3_open_v2            ;
    API_sqlite3_close                ptr_sqlite3_close              ;

    /* error handling */
    API_sqlite3_errcode              ptr_sqlite3_errcode            ;
    API_sqlite3_errmsg               ptr_sqlite3_errmsg             ;

    /* binding operations */
    API_sqlite3_bind_blob            ptr_sqlite3_bind_blob          ;
    API_sqlite3_bind_double          ptr_sqlite3_bind_double        ;
    API_sqlite3_bind_int             ptr_sqlite3_bind_int           ;
    API_sqlite3_bind_int64           ptr_sqlite3_bind_int64         ;
    API_sqlite3_bind_null            ptr_sqlite3_bind_null          ;
    API_sqlite3_bind_text            ptr_sqlite3_bind_text          ;
    API_sqlite3_bind_text16          ptr_sqlite3_bind_text16        ;
    API_sqlite3_bind_zeroblob        ptr_sqlite3_bind_zeroblob      ;

    /* column content operations */
    API_sqlite3_column_count         ptr_sqlite3_column_count       ;
    API_sqlite3_column_name          ptr_sqlite3_column_name        ;
    API_sqlite3_column_blob          ptr_sqlite3_column_blob        ;
    API_sqlite3_column_bytes         ptr_sqlite3_column_bytes       ;
    API_sqlite3_column_double        ptr_sqlite3_column_double      ;
    API_sqlite3_column_int           ptr_sqlite3_column_int         ;
    API_sqlite3_column_int64         ptr_sqlite3_column_int64       ;
    API_sqlite3_column_text          ptr_sqlite3_column_text        ;
    API_sqlite3_column_type          ptr_sqlite3_column_type        ;

    /* VM execute control */
    API_sqlite3_prepare_v2           ptr_sqlite3_prepare_v2         ;
    API_sqlite3_reset                ptr_sqlite3_reset              ;
    API_sqlite3_step                 ptr_sqlite3_step               ;
    API_sqlite3_finalize             ptr_sqlite3_finalize           ;
} __dbms_api_t;

typedef struct
{
    void    *API_DBMSApiTbl;
} __DBMS_FuncTbl_t;

#define DBMSCALL_FUNCNO(x, y)    (SWINO(0, x, y))

#define DBMS_FUNCNO(ApiName)    ((DBMSCALL_FUNCNO(__DBMS_FuncTbl_t, API_DBMSApiTbl) << 16) | \
                                  DBMSCALL_FUNCNO(__dbms_api_t, ApiName))

#define API_SQLITE3_INITIALIZE           DBMS_FUNCNO(ptr_sqlite3_initialize         )
#define API_SQLITE3_SHUTDOWN             DBMS_FUNCNO(ptr_sqlite3_shutdown           )
#define API_SQLITE3_CONFIG               DBMS_FUNCNO(ptr_sqlite3_config             )
#define API_SQLITE3_DB_CONFIG            DBMS_FUNCNO(ptr_sqlite3_db_config          )
#define API_SQLITE3_LAST_INSERT_ROWID    DBMS_FUNCNO(ptr_sqlite3_last_insert_rowid  )
#define API_SQLITE3_CHANGES              DBMS_FUNCNO(ptr_sqlite3_changes            )
#define API_SQLITE3_BUSY_HANDLER         DBMS_FUNCNO(ptr_sqlite3_busy_handler       )
#define API_SQLITE3_BUSY_TIMEOUT         DBMS_FUNCNO(ptr_sqlite3_busy_timeout       )
#define API_SQLITE3_EXEC                 DBMS_FUNCNO(ptr_sqlite3_exec               )
#define API_SQLITE3_GET_TABLE            DBMS_FUNCNO(ptr_sqlite3_get_table          )
#define API_SQLITE3_FREE_TABLE           DBMS_FUNCNO(ptr_sqlite3_free_table         )
#define API_SQLITE3_MEMORY_USED          DBMS_FUNCNO(ptr_sqlite3_memory_used        )
#define API_SQLITE3_MEMORY_HIGHWATER     DBMS_FUNCNO(ptr_sqlite3_memory_highwater   )
#define API_SQLITE3_ENABLE_SHARED_CACHE  DBMS_FUNCNO(ptr_sqlite3_enable_shared_cache)
#define API_SQLITE3_RELEASE_MEMORY       DBMS_FUNCNO(ptr_sqlite3_release_memory     )
#define API_SQLITE3_SOFT_HEAP_LIMIT      DBMS_FUNCNO(ptr_sqlite3_soft_heap_limit    )
#define API_SQLITE3_OPEN_V2              DBMS_FUNCNO(ptr_sqlite3_open_v2            )
#define API_SQLITE3_CLOSE                DBMS_FUNCNO(ptr_sqlite3_close              )
#define API_SQLITE3_ERRCODE              DBMS_FUNCNO(ptr_sqlite3_errcode            )
#define API_SQLITE3_ERRMSG               DBMS_FUNCNO(ptr_sqlite3_errmsg             )
#define API_SQLITE3_BIND_BLOB            DBMS_FUNCNO(ptr_sqlite3_bind_blob          )
#define API_SQLITE3_BIND_DOUBLE          DBMS_FUNCNO(ptr_sqlite3_bind_double        )
#define API_SQLITE3_BIND_INT             DBMS_FUNCNO(ptr_sqlite3_bind_int           )
#define API_SQLITE3_BIND_INT64           DBMS_FUNCNO(ptr_sqlite3_bind_int64         )
#define API_SQLITE3_BIND_NULL            DBMS_FUNCNO(ptr_sqlite3_bind_null          )
#define API_SQLITE3_BIND_TEXT            DBMS_FUNCNO(ptr_sqlite3_bind_text          )
#define API_SQLITE3_BIND_TEXT16          DBMS_FUNCNO(ptr_sqlite3_bind_text16        )
#define API_SQLITE3_BIND_ZEROBLOB        DBMS_FUNCNO(ptr_sqlite3_bind_zeroblob      )
#define API_SQLITE3_COLUMN_COUNT         DBMS_FUNCNO(ptr_sqlite3_column_count       )
#define API_SQLITE3_COLUMN_NAME          DBMS_FUNCNO(ptr_sqlite3_column_name        )
#define API_SQLITE3_COLUMN_BLOB          DBMS_FUNCNO(ptr_sqlite3_column_blob        )
#define API_SQLITE3_COLUMN_BYTES         DBMS_FUNCNO(ptr_sqlite3_column_bytes       )
#define API_SQLITE3_COLUMN_DOUBLE        DBMS_FUNCNO(ptr_sqlite3_column_double      )
#define API_SQLITE3_COLUMN_INT           DBMS_FUNCNO(ptr_sqlite3_column_int         )
#define API_SQLITE3_COLUMN_INT64         DBMS_FUNCNO(ptr_sqlite3_column_int64       )
#define API_SQLITE3_COLUMN_TEXT          DBMS_FUNCNO(ptr_sqlite3_column_text        )
#define API_SQLITE3_COLUMN_TYPE          DBMS_FUNCNO(ptr_sqlite3_column_type        )
#define API_SQLITE3_PREPARE_V2           DBMS_FUNCNO(ptr_sqlite3_prepare_v2         )
#define API_SQLITE3_RESET                DBMS_FUNCNO(ptr_sqlite3_reset              )
#define API_SQLITE3_STEP                 DBMS_FUNCNO(ptr_sqlite3_step               )
#define API_SQLITE3_FINALIZE             DBMS_FUNCNO(ptr_sqlite3_finalize           )

#endif /*__MOD_DBMS_H__*/
