/*
************************************************************************************************************************
*                                                        SWAN
*                                                   the Video Player
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : fifo_db.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.03.31
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
* Gary.Wang      2009.03.31       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __fifo_db_c
#define  __fifo_db_c

#include "beetles_app.h"
#include "fifo_db.h"

#define DB_PATH                         "e:"
#define FIFO_DB_FILE_MAGIC              "FIFO_DB"
#define FIFO_DB_FILE_EXTS               ".fdb"
#define MAGIC_SIZE                      8
#define FIFO_DB_FILE_HEAD_SIZE          512
#define FIFO_DB_VERSION                 0X10100000          // 10.10.0000


#if ( FIFO_DB_ITEM_MAX_NR & ( 8 - 1 ) != 0 )
#error FIFO_DB_ITEM_MAX_NR must be 8-byte-align
#endif

#define SECTOR_SIZE                     512

#define FIFO_DB_FILE_MAX_LEN            ( sizeof(DB_PATH) + 1 + FIFO_DB_TITLE_MAX_LEN + sizeof(FIFO_DB_FILE_EXTS) + 1 )

#define NULL_POINTER                    ((__u32)-1)




typedef enum _global_sta_e
{
    IS_VALID                    = 0,
    FIND_ITEM_PRE_IS_HEAD_NODE    ,
    FIND_ITEM_PRE_IN_CHAIN        ,
    NEED_NEW_ITEM_SPACE           ,
    IS_INVALID                    ,
    NOT_FIND_ITEM_PRE_IS_HEAD_NODE,
    NOT_FIND_ITEM_PRE_IN_CHAIN    ,
    FIND_FREE_ITEM                ,
    FIFO_DB_IS_FULL
} global_sta_e;


typedef struct _space_desc_t
{
    __u32  max_N;                       // once initialized, the value of max_N is unalterable.
    __u32  total_N;                     // the value of total_N can only be increased rather than be decreased.
    __u32  free_N;                      // free_N <= total_N <= max_N
} space_desc_t;

#define SD_M_TOTAL_N_OFFSET             MEMBER_OFFSET( space_desc_t, total_N )
#define SD_M_FREE_N_OFFSET              MEMBER_OFFSET( space_desc_t, free_N )
#define SD_M_MAX_N_OFFSET               MEMBER_OFFSET( space_desc_t, max_N )


typedef struct _fifo_db_file_head_t
{
    char   magic[MAGIC_SIZE];
    __s32  valid_status;
    __s32  version;
    char   title[FIFO_DB_TITLE_MAX_LEN + 1];
    space_desc_t space_desc;
    __u32  content_size;
    __u32  first_item;
    __u8   reserved[FIFO_DB_FILE_HEAD_SIZE - MAGIC_SIZE - 4 - 4 - (FIFO_DB_TITLE_MAX_LEN + 1) - sizeof(space_desc_t) - 4 - 4];
} fifo_db_file_head_t;                              // FIFO_DB_FILE_HEAD_SIZE

//COMPILE_TIME_ASSERT( sizeof( fifo_db_file_head_t ) == FIFO_DB_FILE_HEAD_SIZE );

#define FDFH_M_SPACE_DESC_OFFSET               MEMBER_OFFSET( fifo_db_file_head_t, space_desc )
#define FDFH_M_VALID_STATUS                    MEMBER_OFFSET( fifo_db_file_head_t, valid_status )



#define FIFO_DB_SPACE_BIT_SIZE            ( FIFO_DB_ITEM_MAX_NR >> 3 )       // each bit indicates one item space.
#define FIFO_DB_HASH_ADDR_TBL_LEN         FIFO_DB_ITEM_MAX_NR
#define FIFO_DB_HASH_ADDR_TBL_SIZE        ( FIFO_DB_HASH_ADDR_TBL_LEN * sizeof(__u32 ) )

typedef struct _fifo_db_file_t
{
    fifo_db_file_head_t  file_head;
    __u8                space_bit[FIFO_DB_SPACE_BIT_SIZE];
    __u32               hash_addr_tbl[FIFO_DB_HASH_ADDR_TBL_LEN];
    __u32                item_tbl[1];
} fifo_db_file_t;

#define FDF_M_SPACE_BIT_OFFSET          MEMBER_OFFSET( fifo_db_file_t, space_bit )
#define FDF_M_HASH_ADDR_TBL_OFFSET      MEMBER_OFFSET( fifo_db_file_t, hash_addr_tbl )
#define FDF_M_ITEM_TBL_OFFSET           MEMBER_OFFSET( fifo_db_file_t, item_tbl )


COMPILE_TIME_ASSERT( sizeof( fifo_db_file_head_t ) == FIFO_DB_FILE_HEAD_SIZE );




typedef struct _fifo_db_item_t
{
    char   key[FIFO_DB_KEY_MAX_LEN + 1];
    __s32  valid_status;
    __u32  hash_next;
    __u32  fifo_pre;
    __u32  fifo_next;
} item_info_t;

#define II_M_VALID_STATUS_OFFSET        MEMBER_OFFSET( item_info_t, valid_status )
#define II_M_HASH_NEXT_OFFSET           MEMBER_OFFSET( item_info_t, hash_next    )
#define II_M_FIFO_PRE_OFFSET            MEMBER_OFFSET( item_info_t, fifo_pre     )
#define II_M_FIFO_NEXT_OFFSET           MEMBER_OFFSET( item_info_t, fifo_next    )



static void    compose_fifo_db_file( const char *title, char *fifo_db_file );
static __s32   create_fifo_db_i( const char *title, __u32 item_max_nr, __u32 item_size );
static __s32   search_audio( G_FILE *fp, const char *key, __u32 *hash_pre, __u32 *cur_node, __u32 content_size );
static void    clear_space_bit( G_FILE *fp, __u32 space_bit_offset, __u32 item_pointer );
static void    set_space_bit( G_FILE *fp, __u32 space_bit_offset, __u32 item_pointer );
static __s32   string_hash( const char *s );
static __u32   fifo_db_hash_func( const char *str, __u32 hash_table_len );
static __s32   fifo_db_get_free_item( G_FILE *fp, __u32 *index_p );
static __s32   init_space_bits( G_FILE *fp, __u32 item_count );
static __s32   init_hash_addr_tbl( G_FILE *fp, __u32  hash_tbl_len );



__bool fifo_db_has_exist( const char *title )
{
    G_FILE  *fp = NULL;
    char     file[FIFO_DB_FILE_MAX_LEN + 1] = {0};

    if( g_strlen(title) > FIFO_DB_TITLE_MAX_LEN )
        return EPDK_FALSE;

    compose_fifo_db_file( title, file );
    fp = g_fopen( file, "rb" );
    if( fp == NULL )
    {
        return EPDK_FALSE;
    }
    else
    {
        fifo_db_file_head_t  head;

        /* check valid flag */
        g_fread( &head, 1, sizeof(head), fp );
        if( head.valid_status != IS_VALID || head.version != FIFO_DB_VERSION )       // file is NOT valid
        {
            g_fclose( fp );
            fp = NULL;

            /* delete the file */
            g_remove( file );

            return EPDK_FALSE;
        }
        else
        {
            g_fclose( fp );
            fp = NULL;

            return EPDK_TRUE;
        }
    }
}



__s32  create_fifo_db( const char *title, __u32 item_max_nr, __u32 item_size )
{
    return create_fifo_db_i( title, item_max_nr, item_size );
}



__s32  delete_fifo_db( const char *title )
{
    char     file[FIFO_DB_FILE_MAX_LEN + 1] = {0};

    if( g_strlen(title) > FIFO_DB_TITLE_MAX_LEN )
        return -1;

    compose_fifo_db_file( title, file );

    g_remove( file );

    return 0;
}



__s32  get_item_content_from_fifo_db( const char *title, const char *key, void *buf )
{
    G_FILE  *fp = NULL;
    char     file[FIFO_DB_FILE_MAX_LEN + 1] = {0};
    fifo_db_file_head_t   head;
    __s32  status;
    __u32  hash_pre;
    __u32  cur_node;
    __s32  ret;
    __u32  item_size;

    if(    g_strlen(title) > FIFO_DB_TITLE_MAX_LEN
            || g_strlen(key) > FIFO_DB_KEY_MAX_LEN )
        return -1;

    /* open fifo db file */
    compose_fifo_db_file( title, file );
    fp = g_fopen( file, "rb+" );
    if( fp == NULL )
    {
        __wrn("Fail in opening fifo db file.\n");
        return -1;
    }

    /* read file head */
    g_fread( &head, 1, sizeof(head), fp );

    /* check valid */
    if( head.valid_status != IS_VALID || head.version != FIFO_DB_VERSION )
    {
        g_fclose( fp );
        fp = NULL;
        /* delete file */
        g_remove( file );
        goto del_file;
    }

    /* mark invalid status */
    head.valid_status = IS_INVALID;
    g_fseek( fp, 0, SEEK_SET );
    g_fwrite( &head, 1, sizeof(head), fp );

    item_size = sizeof(item_info_t) + head.content_size;

    /* search item */
    status = search_audio( fp, key, &hash_pre, &cur_node, head.content_size );
    if(     status == NOT_FIND_ITEM_PRE_IS_HEAD_NODE
            ||  status == NOT_FIND_ITEM_PRE_IN_CHAIN )
    {
        __msg("the item %s has not in fifo db.\n", key);
        ret = -1;
        goto exit;
    }

    g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * cur_node + sizeof(item_info_t), SEEK_SET );
    g_fread( buf, 1, head.content_size, fp );

    ret = 0;
    goto exit;


exit:
    if( fp != NULL )
    {
        head.valid_status = IS_VALID;
        g_fseek( fp, 0, SEEK_SET );
        g_fwrite( &head, 1, sizeof(head), fp );
        g_fclose( fp );
        fp = NULL;
    }
    return ret;


del_file:
    if( fp != NULL )
    {
        g_fclose( fp );
        fp = NULL;
    }
    /* delete file */
    g_remove( file );
    __msg("to delete db file %s.\n", file);
    return -1;
}



__s32  add_item_to_fifo_db( const char *title, const char *key, const void *buf )
{
    G_FILE  *fp = NULL;
    char     file[FIFO_DB_FILE_MAX_LEN + 1] = {0};
    fifo_db_file_head_t   head;
    __s32  status;
    __u32  hash_pre;
    __u32  cur_node;
    __u32  fifo_pre;
    __u32  fifo_next;
    item_info_t  item_info;
    __s32  ret;
    __s32  free_status;
    __u32  pre_node_m_next_pos;
    __u32  index;
    __u32  item_size;

    if(    g_strlen(title) > FIFO_DB_TITLE_MAX_LEN
            || g_strlen(key) > FIFO_DB_KEY_MAX_LEN )
        return -1;

    /* open fifo db file */
    compose_fifo_db_file( title, file );
    fp = g_fopen( file, "rb+" );
    if( fp == NULL )
    {
        __wrn("Fail in opening fifo db file.\n");
        return -1;
    }

    /* read file head */
    g_fread( &head, 1, sizeof(head), fp );

    /* check valid */
    if( head.valid_status != IS_VALID || head.version != FIFO_DB_VERSION )
    {
        g_fclose( fp );
        fp = NULL;
        /* delete file */
        g_remove( file );
        goto del_file;
    }

    /* mark invalid status */
    head.valid_status = IS_INVALID;
    g_fseek( fp, 0, SEEK_SET );
    g_fwrite( &head, 1, sizeof(head), fp );

    item_size = sizeof(item_info_t) + head.content_size;

    /* search item */
    status = search_audio( fp, key, &hash_pre, &cur_node, head.content_size );
    if(    status == FIND_ITEM_PRE_IS_HEAD_NODE
            || status == FIND_ITEM_PRE_IN_CHAIN )     // item has existed in fifo_db
    {
        g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * cur_node, SEEK_SET );
        g_fread( &item_info, 1, sizeof(item_info), fp );
        fifo_pre  = item_info.fifo_pre;
        fifo_next = item_info.fifo_next;

        /* modify fifo chain */

        if( fifo_next == head.first_item )      // current item is the last in fifo
        {
            /* write content */
            g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * cur_node + sizeof(item_info_t), SEEK_SET );
            g_fwrite( buf, 1, head.content_size, fp );
            ret = 0;
            goto exit;
        }
        else if( cur_node == head.first_item )         // current item is the first in fifo
        {
            /* modify first poiner */
            head.first_item = fifo_next;
            /* write content */
            g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * cur_node + sizeof(item_info_t), SEEK_SET );
            g_fwrite( buf, 1, head.content_size, fp );
            ret = 0;
            goto exit;
        }
        else
        {
            __u32  last_item;

            /* delete node from fifo chain */
            g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * fifo_pre + II_M_FIFO_NEXT_OFFSET, SEEK_SET );
            g_fwrite( &fifo_next, 1, sizeof(fifo_next), fp );
            g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * fifo_next + II_M_FIFO_PRE_OFFSET, SEEK_SET );
            g_fwrite( &fifo_pre, 1, sizeof(fifo_pre), fp );
            /* add node to the end of fifo chain */
            g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * head.first_item + II_M_FIFO_PRE_OFFSET, SEEK_SET );
            g_fread( &last_item, 1, sizeof(last_item), fp );
            g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * head.first_item + II_M_FIFO_PRE_OFFSET, SEEK_SET );
            g_fwrite( &cur_node, 1, sizeof(cur_node), fp );
            g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * last_item + II_M_FIFO_NEXT_OFFSET, SEEK_SET );
            g_fwrite( &cur_node, 1, sizeof(cur_node), fp );
            item_info.fifo_next = head.first_item;
            item_info.fifo_pre  = last_item;
            g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * cur_node, SEEK_SET );
            g_fwrite( &item_info, 1, sizeof(item_info), fp );
            g_fwrite( buf, 1, head.content_size, fp );

            ret = 0;
            goto exit;
        }
    }

    /* item has not in fifo db*/

    /* search free item space */
    free_status = fifo_db_get_free_item( fp, &index );
    if( free_status == FIFO_DB_IS_FULL )            // fifo dbis full
    {
        index = head.first_item;
        /* modify fifo chain */
        __msg("first item is %x.\n", head.first_item);
        g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * head.first_item, SEEK_SET );
        g_fread( &item_info, 1, sizeof(item_info), fp );
        head.first_item = item_info.fifo_next;
        /* modify hash chain */
        /* delete old key from hash chain */
        {
            __s32  status;
            __u32  hash_pre;
            __u32  cur_node;

            status = search_audio( fp, item_info.key, &hash_pre, &cur_node, head.content_size );
            if(     status == NOT_FIND_ITEM_PRE_IS_HEAD_NODE
                    ||  status == NOT_FIND_ITEM_PRE_IN_CHAIN )
            {
                __wrn("FATAL ERROR!\n");
                goto del_file;
            }

            /* modify previous node's next pointer */
            if( status == FIND_ITEM_PRE_IS_HEAD_NODE )            // current node's previous node is in hash address table
                pre_node_m_next_pos = FDF_M_HASH_ADDR_TBL_OFFSET + sizeof(__u32 ) * hash_pre;
            else                                                    // current node's previous node is in chain
                pre_node_m_next_pos = FDF_M_ITEM_TBL_OFFSET + item_size * hash_pre + II_M_HASH_NEXT_OFFSET;
            g_fseek( fp, pre_node_m_next_pos, SEEK_SET );
            g_fwrite( &(item_info.hash_next), 1, sizeof(__u32), fp );
        }
        /* add the new key item to hash chain */
        /* current node's previous node is in hash address table */
        if( status == NOT_FIND_ITEM_PRE_IS_HEAD_NODE )
            pre_node_m_next_pos = FDF_M_HASH_ADDR_TBL_OFFSET + sizeof(__u32 ) * hash_pre;
        else /* current node's previous node is in chain */
            pre_node_m_next_pos = FDF_M_ITEM_TBL_OFFSET + item_size * hash_pre + II_M_HASH_NEXT_OFFSET;
        g_fseek( fp, pre_node_m_next_pos, SEEK_SET );
        g_fwrite( &index, 1, sizeof(index), fp );
        item_info.hash_next = cur_node;
        /* write item */
        g_strcpy( item_info.key, key );
        item_info.valid_status = IS_VALID;
        g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * index, SEEK_SET );
        g_fwrite( &item_info, 1, sizeof(item_info), fp );
        g_fwrite( buf, 1, head.content_size, fp );

        ret = 0;
        goto exit;
    }

    /* free or new item space */

    /* modify fifo chain */
    if( head.first_item == NULL_POINTER )
    {
        item_info.fifo_pre  = index;
        item_info.fifo_next = index;
        head.first_item = index;
    }
    else
    {
        g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * head.first_item + II_M_FIFO_PRE_OFFSET, SEEK_SET );
        g_fread( &fifo_pre, 1, sizeof(fifo_pre), fp );
        g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * head.first_item + II_M_FIFO_PRE_OFFSET, SEEK_SET );
        g_fwrite( &index, 1, sizeof(index), fp );
        g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * fifo_pre + II_M_FIFO_NEXT_OFFSET, SEEK_SET );
        g_fwrite( &index, 1, sizeof(index), fp );
        item_info.fifo_pre = fifo_pre;
        item_info.fifo_next = head.first_item;
    }
    /* modify hash chain */
    /* current node's previous node is in hash address table */
    if( status == NOT_FIND_ITEM_PRE_IS_HEAD_NODE )
        pre_node_m_next_pos = FDF_M_HASH_ADDR_TBL_OFFSET + sizeof(__u32 ) * hash_pre;
    else /* current node's previous node is in chain */
        pre_node_m_next_pos = FDF_M_ITEM_TBL_OFFSET + item_size * hash_pre + II_M_HASH_NEXT_OFFSET;
    g_fseek( fp, pre_node_m_next_pos, SEEK_SET );
    g_fwrite( &index, 1, sizeof(index), fp );
    item_info.hash_next = cur_node;
    /* modify space_desc section */
    if( free_status == NEED_NEW_ITEM_SPACE )
        head.space_desc.total_N++;
    else
        head.space_desc.free_N--;
    /* modify space_bit section */
    set_space_bit( fp, FDF_M_SPACE_BIT_OFFSET, index );
    /* write item */
    g_strcpy( item_info.key, key );
    item_info.valid_status = IS_VALID;
    g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * index, SEEK_SET );
    g_fwrite( &item_info, 1, sizeof(item_info), fp );
    g_fwrite( buf, 1, head.content_size, fp );

    ret = 0;
    goto exit;


exit:
    if( fp != NULL )
    {
        __msg("first item is %x.\n", head.first_item);
        head.valid_status = IS_VALID;
        g_fseek( fp, 0, SEEK_SET );
        g_fwrite( &head, 1, sizeof(head), fp );
        g_fclose( fp );
        fp = NULL;
    }
    return ret;


del_file:
    if( fp != NULL )
    {
        g_fclose( fp );
        fp = NULL;
    }
    /* delete file */
    g_remove( file );
    __msg("to delete db file %s.\n", file);
    return -1;
}



__s32  delete_item_from_fifo_db( const char *title, const char *key )
{
    G_FILE  *fp = NULL;
    char     file[FIFO_DB_FILE_MAX_LEN + 1] = {0};
    fifo_db_file_head_t   head;
    __s32  status;
    __u32  hash_pre;
    __u32  cur_node;
    item_info_t  item_info;
    __s32  ret;
    __u32  pre_node_m_next_pos;
    __u32  item_size;
    __s32  valid_status;

    if(    g_strlen(title) > FIFO_DB_TITLE_MAX_LEN
            || g_strlen(key) > FIFO_DB_KEY_MAX_LEN )
        return -1;

    /* open fifo db file */
    compose_fifo_db_file( title, file );
    fp = g_fopen( file, "rb+" );
    if( fp == NULL )
    {
        __wrn("Fail in opening fifo db file.\n");
        return -1;
    }

    /* read file head */
    g_fread( &head, 1, sizeof(head), fp );

    /* check valid */
    if( head.valid_status != IS_VALID || head.version != FIFO_DB_VERSION )
    {
        g_fclose( fp );
        fp = NULL;
        /* delete file */
        g_remove( file );
        goto del_file;
    }

    /* mark invalid status */
    head.valid_status = IS_INVALID;
    g_fseek( fp, 0, SEEK_SET );
    g_fwrite( &head, 1, sizeof(head), fp );

    item_size = sizeof(item_info_t) + head.content_size;

    /* search item */
    status = search_audio( fp, key, &hash_pre, &cur_node, head.content_size );
    if(     status == NOT_FIND_ITEM_PRE_IS_HEAD_NODE
            ||  status == NOT_FIND_ITEM_PRE_IN_CHAIN )
    {
        __wrn("item %s has not in fifo db!\n", key);
        ret = 0;
        goto exit;
    }

    g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * cur_node, SEEK_SET );
    g_fread( &item_info, 1, sizeof(item_info), fp );
    if( item_info.valid_status != IS_VALID )
    {
        __wrn("FATAL ERROR! item %s is NOT valid.\n");
        ret = 0;
        goto exit;
    }

    /* modify hash chain */
    /* modify previous node's next pointer */
    if( status == FIND_ITEM_PRE_IS_HEAD_NODE )            // current node's previous node is in hash address table
        pre_node_m_next_pos = FDF_M_HASH_ADDR_TBL_OFFSET + sizeof(__u32 ) * hash_pre;
    else                                                    // current node's previous node is in chain
        pre_node_m_next_pos = FDF_M_ITEM_TBL_OFFSET + item_size * hash_pre + II_M_HASH_NEXT_OFFSET;
    g_fseek( fp, pre_node_m_next_pos, SEEK_SET );
    g_fwrite( &(item_info.hash_next), 1, sizeof(__u32), fp );
    /* modify fifo chain */
    if( cur_node == head.first_item )
        head.first_item = item_info.fifo_next;
    g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * item_info.fifo_pre + II_M_FIFO_NEXT_OFFSET, SEEK_SET );
    g_fwrite( &(item_info.fifo_next), 1, sizeof(__u32), fp );
    g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * item_info.fifo_next + II_M_FIFO_PRE_OFFSET, SEEK_SET );
    g_fwrite( &(item_info.fifo_pre), 1, sizeof(__u32), fp );
    /* invalid item */
    valid_status = IS_INVALID;
    g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * cur_node + II_M_VALID_STATUS_OFFSET, SEEK_SET );
    g_fwrite( &valid_status, 1, sizeof(valid_status), fp );
    /* modify space_desc section */
    head.space_desc.free_N++;
    /* modify space_bit section */
    clear_space_bit( fp, FDF_M_SPACE_BIT_OFFSET, cur_node );

    ret = 0;
    goto exit;


exit:
    if( fp != NULL )
    {
        head.valid_status = IS_VALID;
        g_fseek( fp, 0, SEEK_SET );
        g_fwrite( &head, 1, sizeof(head), fp );
        g_fclose( fp );
        fp = NULL;
    }
    return ret;


del_file:
    if( fp != NULL )
    {
        g_fclose( fp );
        fp = NULL;
    }
    /* delete file */
    g_remove( file );
    __msg("to delete db file %s.\n", file);
    return -1;
}



/***************************************************************************************************
*Name        : string_hash
*Prototype   : __s32 string_hash( const char *s )
*Arguments   : s              input. a string.
*Return      : hash value
*Description : generate the hash value of a string.
*Other       :
***************************************************************************************************/
static __s32 string_hash( const char *s )
{
    __u8  *p;
    __u32  h;
    __u32  g;

    if( s == NULL )
        return -1;

    for ( h = 0, p = (unsigned char *)s; *p != '\0'; p++ )
    {
        h = (h << 4) + (*p);
        if ( ( g = (h & 0xf0000000) ) != 0 )
        {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
    }

    return h;
}



static __u32  fifo_db_hash_func( const char *str, __u32 hash_table_len )
{
    __u32  sum;

    sum = string_hash( str );

    return ( sum & ( hash_table_len - 1 ) );
}



static __s32 search_audio( G_FILE *fp, const char *key, __u32 *hash_pre, __u32 *cur_node, __u32 content_size )
{
    __u32  hash_value;
    __u32  cur_p;
    __u32  pre_p;
    item_info_t item_info;
    __s32  cmp;
    __u32  count;
    __u32  item_size;

    item_size = sizeof(item_info_t) + content_size;

    hash_value = fifo_db_hash_func( key, FIFO_DB_HASH_ADDR_TBL_LEN );
    pre_p = hash_value;
    g_fseek( fp, FDF_M_HASH_ADDR_TBL_OFFSET + hash_value * sizeof(__u32 ), SEEK_SET );
    g_fread( &cur_p, sizeof(__u32 ), 1, fp );
    if( cur_p == NULL_POINTER )
    {
        *hash_pre = hash_value;
        *cur_node = NULL_POINTER;
        return NOT_FIND_ITEM_PRE_IS_HEAD_NODE;
    }

    count = 0;
    g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * cur_p, SEEK_SET );
    g_fread( &item_info, 1, sizeof(item_info), fp );
    while( ( cmp = g_strncmp( key, item_info.key , FIFO_DB_KEY_MAX_LEN + 1 ) ) > 0
            && item_info.hash_next != NULL_POINTER )
    {
        pre_p = cur_p;
        cur_p = item_info.hash_next;
        g_fseek( fp, FDF_M_ITEM_TBL_OFFSET + item_size * cur_p, SEEK_SET );
        g_fread( &item_info, sizeof(item_info), 1, fp );
        count++;
    }
    if( cmp == 0 )                                          // destination is found
    {
        *hash_pre = pre_p;
        *cur_node = cur_p;
        if( count == 0 )
            return FIND_ITEM_PRE_IS_HEAD_NODE;              // hash_pre is in hash address table
        else
            return FIND_ITEM_PRE_IN_CHAIN;                  // hash_pre is in chain.
    }
    if( cmp < 0 )                                           // current node's value is larger than destination's value
    {
        *hash_pre = pre_p;
        *cur_node = cur_p;
        if( count == 0 )
            return NOT_FIND_ITEM_PRE_IS_HEAD_NODE;          // hash_pre is in hash address table
        else
            return NOT_FIND_ITEM_PRE_IN_CHAIN;              // hash_pre is in chain.
    }
    else                                                    // current node is the last node of chain
    {
        *hash_pre = cur_p;
        *cur_node = NULL_POINTER;
        return NOT_FIND_ITEM_PRE_IN_CHAIN;
    }
}



static void compose_fifo_db_file( const char *title, char *fifo_db_file )
{
    if (g_strlen(title) >= FIFO_DB_TITLE_MAX_LEN)
    {
        g_strcpy( fifo_db_file, DB_PATH );
        g_strcat( fifo_db_file, "\\" );
        g_strncpy( fifo_db_file + g_strlen(fifo_db_file), title, FIFO_DB_TITLE_MAX_LEN);
        g_strcat( fifo_db_file, FIFO_DB_FILE_EXTS );
    }
    else
    {
        g_strcpy( fifo_db_file, DB_PATH );
        g_strcat( fifo_db_file, "\\" );
        g_strcat( fifo_db_file, title );
        g_strcat( fifo_db_file, FIFO_DB_FILE_EXTS );
    }
}



static __s32 init_fifo_db_file( G_FILE *fp, const char *title, __u32 max_nr, __u32 content_size )
{
    __s32  ret;

    /* write FILE head */
    {
        fifo_db_file_head_t head;

        g_memset( &head, 0, sizeof(head) );
        g_strncpy( head.magic, FIFO_DB_FILE_MAGIC, MAGIC_SIZE );
        head.valid_status = IS_VALID;
        head.version      = FIFO_DB_VERSION;
        g_strcpy( head.title, title );
        head.space_desc.max_N   = max_nr;
        head.space_desc.total_N = 0;
        head.space_desc.free_N  = 0;
        head.content_size = content_size;
        head.first_item = NULL_POINTER;

        g_fwrite( &head, 1, sizeof(head), fp );
    }

    /* write space bit indicating the status of each item space */
    ret = init_space_bits( fp, FIFO_DB_ITEM_MAX_NR );
    if( ret != 0 )
    {
        __wrn("Error in initialising fifo dbfile.\n");
        return ret;
    }

    /* write hash address table */
    ret = init_hash_addr_tbl( fp, FIFO_DB_HASH_ADDR_TBL_LEN );
    if( ret != 0 )
    {
        __wrn("Error in initialising fifo dbfile. At line %u of file %s.\n", __LINE__, __FILE__);
        return ret;
    }

    /* modify valid status */
    {
        __s32  valid_status;

        valid_status = IS_VALID;
        g_fseek( fp, FDFH_M_VALID_STATUS, SEEK_SET );
        g_fwrite( &valid_status, 1, sizeof(valid_status), fp );
    }

    return 0;
}



static __s32  create_fifo_db_i( const char *title, __u32 item_max_nr, __u32 item_size )
{
    char   file[FIFO_DB_FILE_MAX_LEN + 1] = {0};
    G_FILE *fp = NULL;
    DIR    *pdir = NULL;

    if( g_strlen(title) > FIFO_DB_TITLE_MAX_LEN )
        return -1;

    if( item_max_nr > FIFO_DB_ITEM_MAX_NR || item_size > FIFO_DB_ITEM_CONTENT_MAX_SIZE )
        goto error;

    /* check whether folder exist or not */
    pdir = g_opendir( DB_PATH );
    if( pdir == NULL )                            // folder not exist
    {
        g_mkdir( DB_PATH );             // create folder
    }
    else
    {
        g_closedir( pdir );
        pdir = NULL;
    }

    /* create fifo db file */
    compose_fifo_db_file( title, file );
    fp = g_fopen( file, "wb+" );
    if( fp == NULL )
    {
        __wrn("Fail in creating fifo db file.\n");
        goto error;
    }

    /* init fifo db file */
    if( init_fifo_db_file( fp, title, item_max_nr, item_size ) )
        goto error;

    g_fclose( fp );
    fp = NULL;

    return 0;

error:
    if( fp != NULL )
    {
        g_fclose( fp );
        fp = NULL;
        /* delete invalid file */
        g_remove( file );
    }
    return -1;
}



static __s32  fifo_db_get_free_item( G_FILE *fp, __u32 *index_p )
{
#define BUF_SIZE          256
    space_desc_t space_desc;
    __u8  buf[BUF_SIZE];
    __u32  total_byte;
    __u32  delt_N;
    __u32  cur_N;
    __u32  i;
    __u32  j;
    __u8  temp;

    g_fseek( fp, FDFH_M_SPACE_DESC_OFFSET, SEEK_SET );
    g_fread( &space_desc, sizeof(space_desc_t), 1, fp );
    if( space_desc.free_N == 0 && space_desc.total_N == space_desc.max_N )
        return FIFO_DB_IS_FULL;

    if( space_desc.free_N == 0 )
    {
        *index_p = space_desc.total_N;
        return NEED_NEW_ITEM_SPACE;
    }

    total_byte = ( space_desc.total_N + 7 ) >> 3 ;
    g_fseek( fp, FDF_M_SPACE_BIT_OFFSET, SEEK_SET );
    for( delt_N = 0;  delt_N < total_byte;  delt_N += cur_N )
    {
        cur_N = min( BUF_SIZE, total_byte - delt_N );
        g_fread( buf, 1, cur_N, fp );
        for( i = 0;  i < cur_N;  i++ )
        {
            if( buf[i] != (__u8)0xFF )
            {
                temp = buf[i];
                for( j = 0;  j < 8;  j++ )
                {
                    if( test_bit( temp, j ) == 0 )
                    {
                        *index_p = ( ( delt_N + i ) << 3 ) + j;
                        return FIND_FREE_ITEM;
                    }
                }
            }
        }
    }

    __err("Fatal ERROR! Can't get free item in fifo_db.At line %u of file %s.\n", __LINE__, __FILE__);
    return -1;
}



/***************************************************************************************************
*Name        : clear_space_bit
*Prototype   : void clear_space_bit( G_FILE *fp, __u32 space_bit_offclear, __u32 item_pointer )
*Arguments   : fp                input. a handle of a file.
*              space_bit_offset  input. the offset of space bit section in the file.
*              item_pointer      input. the index of item in the item table.
*Return      : void
*Description : clear space bit corresponding to an item.
*Other       :
***************************************************************************************************/
static void clear_space_bit( G_FILE *fp, __u32 space_bit_offset, __u32 item_pointer )
{
    __u8 temp;

    g_fseek( fp, space_bit_offset + ( item_pointer >> 3 ) , SEEK_SET );
    g_fread( &temp, 1, 1, fp );
    clear_bit( temp, ( item_pointer & 0x7 ) );
    g_fseek( fp, -1 , SEEK_CUR );
    g_fwrite( &temp, 1, 1, fp );
}



/***************************************************************************************************
*Name        : set_space_bit
*Prototype   : void set_space_bit( G_FILE *fp, __u32 space_bit_offset, __u32 item_pointer )
*Arguments   : fp                input. a handle of a file.
*              space_bit_offset  input. the offset of space bit section in the file.
*              item_pointer      input. the index of item in the item table.
*Return      : void
*Description : set space bit corresponding to an item.
*Other       :
***************************************************************************************************/
static void set_space_bit( G_FILE *fp, __u32 space_bit_offset, __u32 item_pointer )
{
    __u8 temp;

    g_fseek( fp, space_bit_offset + ( item_pointer >> 3 ) , SEEK_SET );
    g_fread( &temp, 1, 1, fp );
    set_bit( temp, ( item_pointer & 0x7 ) );
    g_fseek( fp, -1 , SEEK_CUR );
    g_fwrite( &temp, 1, 1, fp );
}



static __s32 write_file_space( G_FILE *fp, __u32 byte_n, __u8 value )
{
    char   buf[SECTOR_SIZE];
    __u32 delt;
    __u32 cur;
    __u32 write_n;

    g_memset( buf, value, SECTOR_SIZE );
    for( delt = 0;  delt < byte_n;  delt += cur )
    {
        cur = min( SECTOR_SIZE, byte_n - delt );
        write_n = g_fwrite( buf, 1, cur, fp );
        if( write_n != cur )
            return -1;
    }
    return 0;
}



static __s32 init_space_bits( G_FILE *fp, __u32 item_count )
{
    return write_file_space( fp, item_count >> 3, 0 );
}



static __s32 init_hash_addr_tbl( G_FILE *fp, __u32  hash_tbl_len )
{
    char   buf[SECTOR_SIZE];
    __u32  byte_n;
    __u32  delt;
    __u32  cur;
    __u32  write_n;
    __u32 *p;
    __u32  count;
    __u32  i;

    for( i = 0, p = (__u32 *)buf, count = sizeof(buf) >> 2; i < count; i++ )
        p[i] = NULL_POINTER;

    byte_n = hash_tbl_len << 2;
    for( delt = 0;  delt < byte_n;  delt += cur )
    {
        cur = min( SECTOR_SIZE, byte_n - delt );
        write_n = g_fwrite( buf, 1, cur, fp );
        if( write_n != cur )
            return -1;
    }

    return 0;
}



#endif     //  ifndef __fifo_db_c

/* end of fifo_db.c */
