/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_sqlite3.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-8-22
* Descript: load sqlite3 module, query functions's entry for sqlite3 module stub.
* Update  : date                auther      ver     notes
*           2010-8-22 11:01:08  Sunny       1.0     create this file.
*********************************************************************************************************
*/

#include    "dbms_i.h"

__dbms_api_t    SQLite3Api;

/* DBMS module path */
#define	 DBMS_MOD_PATH	"D:\\mod\\dbms.mod"

/* DBMS module id   */
#define	 EMOD_DBMS_ID	(EPOS_smid_min + EMOD_TYPE_MOD_DBMS)

static int 		ModDBMSID;
static __mp    *ModDBMS;

/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
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
int DBLibary_LoadSQLite3Libary(void)
{
    /* install dbms module */
    ModDBMSID = esMODS_MInstall(DBMS_MOD_PATH, 0);
    if(ModDBMSID == 0)
    {
        __wrn("Install dbms module failed");
        return EPDK_FAIL;
    }
    __inf("Install dbms module succeeded\n");

    /* open dbms module */
    ModDBMS = esMODS_MOpen(ModDBMSID, 0);
    if(ModDBMS == (__mp *)0)
    {
        __wrn("Open dbms module failed");
        return EPDK_FAIL;
    }
    __inf("Open dbms module succeeded\n");

    /* maybe will check dbms module version here....
     * By Sunny at 2010-9-17 11:17:44
     */

    /* get dbms module functions entry address to fill dbms stub */
    SQLite3Api.ptr_sqlite3_initialize          = (API_sqlite3_initialize         )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_INITIALIZE         );
    SQLite3Api.ptr_sqlite3_shutdown            = (API_sqlite3_shutdown           )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_SHUTDOWN           );
    SQLite3Api.ptr_sqlite3_config              = (API_sqlite3_config             )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_CONFIG             );
    SQLite3Api.ptr_sqlite3_db_config           = (API_sqlite3_db_config          )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_DB_CONFIG          );
    SQLite3Api.ptr_sqlite3_last_insert_rowid   = (API_sqlite3_last_insert_rowid  )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_LAST_INSERT_ROWID  );
    SQLite3Api.ptr_sqlite3_changes             = (API_sqlite3_changes            )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_CHANGES            );
    SQLite3Api.ptr_sqlite3_busy_handler        = (API_sqlite3_busy_handler       )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_BUSY_HANDLER       );
    SQLite3Api.ptr_sqlite3_busy_timeout        = (API_sqlite3_busy_timeout       )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_BUSY_TIMEOUT       );
    SQLite3Api.ptr_sqlite3_exec                = (API_sqlite3_exec               )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_EXEC               );
    SQLite3Api.ptr_sqlite3_get_table           = (API_sqlite3_get_table          )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_GET_TABLE          );
    SQLite3Api.ptr_sqlite3_free_table          = (API_sqlite3_free_table         )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_FREE_TABLE         );
    SQLite3Api.ptr_sqlite3_memory_used         = (API_sqlite3_memory_used        )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_MEMORY_USED        );
    SQLite3Api.ptr_sqlite3_memory_highwater    = (API_sqlite3_memory_highwater   )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_MEMORY_HIGHWATER   );
    SQLite3Api.ptr_sqlite3_enable_shared_cache = (API_sqlite3_enable_shared_cache)esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_ENABLE_SHARED_CACHE);
    SQLite3Api.ptr_sqlite3_release_memory      = (API_sqlite3_release_memory     )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_RELEASE_MEMORY     );
    SQLite3Api.ptr_sqlite3_soft_heap_limit     = (API_sqlite3_soft_heap_limit    )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_SOFT_HEAP_LIMIT    );
    SQLite3Api.ptr_sqlite3_open_v2             = (API_sqlite3_open_v2            )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_OPEN_V2            );
    SQLite3Api.ptr_sqlite3_close               = (API_sqlite3_close              )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_CLOSE              );
    SQLite3Api.ptr_sqlite3_errcode             = (API_sqlite3_errcode            )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_ERRCODE            );
    SQLite3Api.ptr_sqlite3_errmsg              = (API_sqlite3_errmsg             )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_ERRMSG             );
    SQLite3Api.ptr_sqlite3_bind_blob           = (API_sqlite3_bind_blob          )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_BIND_BLOB          );
    SQLite3Api.ptr_sqlite3_bind_double         = (API_sqlite3_bind_double        )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_BIND_DOUBLE        );
    SQLite3Api.ptr_sqlite3_bind_int            = (API_sqlite3_bind_int           )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_BIND_INT           );
    SQLite3Api.ptr_sqlite3_bind_int64          = (API_sqlite3_bind_int64         )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_BIND_INT64         );
    SQLite3Api.ptr_sqlite3_bind_null           = (API_sqlite3_bind_null          )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_BIND_NULL          );
    SQLite3Api.ptr_sqlite3_bind_text           = (API_sqlite3_bind_text          )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_BIND_TEXT          );
    SQLite3Api.ptr_sqlite3_bind_text16         = (API_sqlite3_bind_text16        )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_BIND_TEXT16        );
    SQLite3Api.ptr_sqlite3_bind_zeroblob       = (API_sqlite3_bind_zeroblob      )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_BIND_ZEROBLOB      );
    SQLite3Api.ptr_sqlite3_column_count        = (API_sqlite3_column_count       )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_COLUMN_COUNT       );
    SQLite3Api.ptr_sqlite3_column_name         = (API_sqlite3_column_name        )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_COLUMN_NAME        );
    SQLite3Api.ptr_sqlite3_column_blob         = (API_sqlite3_column_blob        )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_COLUMN_BLOB        );
    SQLite3Api.ptr_sqlite3_column_bytes        = (API_sqlite3_column_bytes       )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_COLUMN_BYTES       );
    SQLite3Api.ptr_sqlite3_column_double       = (API_sqlite3_column_double      )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_COLUMN_DOUBLE      );
    SQLite3Api.ptr_sqlite3_column_int          = (API_sqlite3_column_int         )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_COLUMN_INT         );
    SQLite3Api.ptr_sqlite3_column_int64        = (API_sqlite3_column_int64       )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_COLUMN_INT64       );
    SQLite3Api.ptr_sqlite3_column_text         = (API_sqlite3_column_text        )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_COLUMN_TEXT        );
    SQLite3Api.ptr_sqlite3_column_type         = (API_sqlite3_column_type        )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_COLUMN_TYPE        );
    SQLite3Api.ptr_sqlite3_prepare_v2          = (API_sqlite3_prepare_v2         )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_PREPARE_V2         );
    SQLite3Api.ptr_sqlite3_reset               = (API_sqlite3_reset              )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_RESET              );
    SQLite3Api.ptr_sqlite3_step                = (API_sqlite3_step               )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_STEP               );
    SQLite3Api.ptr_sqlite3_finalize            = (API_sqlite3_finalize           )esMSTUB_GetFuncEntry(EMOD_DBMS_ID, API_SQLITE3_FINALIZE           );

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
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
int DBLibary_FreeSQLite3Libary(void)
{
    /* destory SQLite3 API stub */
    eLIBs_memset(&SQLite3Api, 0, sizeof(SQLite3Api));

    /* uninstall dbms module    */
    if (ModDBMS)
    {
        esMODS_MClose(ModDBMS);
    }
    if (ModDBMSID)
    {
        esMODS_MUninstall(ModDBMSID);
    }
    ModDBMS   = NULL;
    ModDBMSID = 0;

    return EPDK_OK;
}
