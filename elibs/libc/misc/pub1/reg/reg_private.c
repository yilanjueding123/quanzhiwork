/*
************************************************************************************************************************
*                                                     REGISTER TABLE
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : reg_private.c
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.05.27
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
* Gary.Wang      2008.05.27       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef __reg_private_c
#define __reg_private_c


#include "reg_cfg.h"
#include "reg_os.h"
#include "reg_private.h"


#define TAB                             '\t'
#define BLANK                           ' '
#define CR                              '\r'
#define LF                              '\n'
#define REG_LINE_END                    "\r\n"
#define REG_BUF_END                     "\2\4\6\7\5\3\1"
#define REG_FILE_END                    "\2\1\3\5\7\6\4"


#define OPEN_NODE_MAX_NR                16
#define INI_FILE_EXTENTION              ".ini"
#define BACKUP_FILE_EXTENTION           ".bak"
#define REG_FILE_EXTENTION              ".reg"
#define TEMP_REG_FILE_NAME              "temp_reg.tmp"

/* item VALID_STATUS status */
#define INVALID_STATUS                  0
#define VALID_STATUS                    1

/* sequence information status */
#define NOT_SEQ                         0
#define SEQ                             1

/* file modified status */
#define NOT_MODIFIED                    0
#define MODIFIED                        1


#define REG_MAGIC                       "REGISTER"
#define MAGIC_SIZE                      8
#define HASH_TBL_LEN                    1024
#define REG_FILE_VERSION                0x01010000          // 1.1.00

#define INVALID_SPACE_THRESHOLD         32 * 1024

#define NULL_POINTER                    0

#define HANDLE_LABEL                    0x5F0A6C39
#define INVALID_LABEL                   0

/* item type */
#define BLANK_ITEM                      0
#define COMMENT_ITEM                    1
#define SET_ITEM                        2
#define KEY_ITEM                        3
#define SEQ_END                         4


#define FIND_ITEM_PRE_IS_HEAD_NODE      0
#define FIND_ITEM_PRE_IN_CHAIN          1
#define NOT_FIND_ITEM_PRE_IS_HEAD_NODE  2
#define NOT_FIND_ITEM_PRE_IN_CHAIN      3


#define REG_INFO_FILE_NAME              "reg_info.inf"
#define REG_INFO_FILE_MAGIC             "REG_INFO"


typedef struct _reg_info_file_head_t
{
    char    magic[MAGIC_SIZE];
    int32   version;
    __Bool  is_valid;
    int32   sys_version;
    uint8   pad[512 - MAGIC_SIZE - 4 - 4 - 4];
} reg_info_file_head_t;


COMPILE_TIME_ASSERT( ( sizeof(reg_info_file_head_t) & ( 512 - 1 ) ) ==  0 );


typedef struct _reg_info_t
{
    int32   version;
    __Bool  is_valid;
    int32   sys_version;
} reg_info_t;


typedef struct _set_desc_t
{
    struct _set_desc_t *next_set;
    uint32  next_key_pos;               // for __GetNextKey().NULL_POINTER : the end of key list
    char    name[SET_NAME_LEN + 1];
    uint32  pos;
    uint32  next_set_pos;
    uint32  append_pos;                 // 0 : not append.
    int32   seq_status;
    uint32  set_id;
    uint32  key_nr;
    uint32  set_size;
    uint32  first_key_pos;              // NULL_POINTER : no key
    uint32  last_key_pos;               // NULL_POINTER : no key
} set_desc_t;


typedef struct _NODE_i_t
{
    struct _NODE_i_t  *pre_HNODE;
    struct _NODE_i_t  *next_HNODE;
    uint32  version;				    //配置信息结构的版本
    uint32  state;				        //配置文件的当前状态
    uint32  type;				        //配置文件的类型
    int32   label;                      // label of a handle
    char    *node;
    char    *reg_file;
    G_FILE *reg_fp;
    int32   mode;                       // mode for openning node
    uint32  open_nr;                    // the number of handles of the same node, only used for REG_READONLY mode.
    int32   err_no;                     // error code
    uint32  total_size;                 // register file size
    uint32  set_nr;                     // the number of sets.
    uint32  set_id_base;                // set id base. the id for the next new set.
    uint32  invalid_space_size;         // the size of space of deleted items. for deleting invalid space of file.
    int32   modified_status;       // indicate whether the content of file is changed. for backup.
    set_desc_t *first_set;              // point to the first set
    set_desc_t *last_set;               // point to the last set
    set_desc_t *next_set;               // for __GetNextSet().
    set_desc_t *cur_set;                // cache. point to the current set
    uint32  reserved;				    // reserved.
} NODE_i_t, *HNODE_i;


typedef struct _key_desc_t
{
    char   key_name[KEY_NAME_LEN + 1];
    char   key_value[KEY_VALUE_LEN + 1];
    uint32 next_pos;
} key_desc_t;


typedef struct _HNODE_head_node_t
{
    uint32 count;
    HNODE_i   next_HNODE;
} HNODE_head_node_t;


typedef struct _reg_file_head_t
{
    char   magic[MAGIC_SIZE];           // the magic of register file
    uint32 version;                     // version
    int32  valid_status;                // indicate whether the file is valid or not.
    uint32 total_size;                  // the size of the file
    uint32 set_nr;                      // the number of sets.
    uint32 set_id_base;                 // set id base. the id for the next new set.
    uint32 first_set_pos;               // the position of first set in file
    uint32 invalid_space_size;          // the size of space of deleted items. for deleting invalid space of file.
    int32  modified_status;             // indicate whether the content of file is changed. for backup.
} reg_file_head_t;

typedef struct _reg_file_org_t
{
    reg_file_head_t  file_head;         // register file head
    uint32  hash_tbl[HASH_TBL_LEN];     // hash address table for hash chain
    uint32  content[1];                 // the real content, include : blank line, comment, set and key.
} reg_file_org_t;

typedef struct _blank_item_t
{
    int32  type;                        // item type
} blank_item_t;

typedef struct _comment_item_t
{
    int32  type;                        // item type
    uint32 len;
} comment_item_t;

typedef struct _set_item_t
{
    int32  type;                        // item type
    int32  valid_status;                // indicate whether set item is valid or not. VALID_STATUS and INVALID_STATUS.
    uint32 next_set_pos;                // the position of next set item in file. NULL_POINTER means current set is the last one.
    uint32 pos;                         // the position of current set item in file.
    uint32 append_pos;                  // the position of added new key item in file. 0 means no added new key item.
    int32  seq_status;                  // indicate whether the set includes sequential information or not. SEQ and NOT_SEQ.
    uint32 set_id;                      // set id
    uint32 key_nr;                      // the number of keys that pertain to the set
    uint32 set_size;                    // the size of space that all the items that pertain to the set take up.
    uint32 first_key_pos;               // the position of the first key item that pertain to the set. NULL_POINTER means the set has no key.
    uint32 last_key_pos;                // the position of the last key item that pertain to the set.
    char   name[SET_NAME_LEN + 1];      // set name
} set_item_t;

typedef struct _key_item_t
{
    int32  type;                        // item type
    int32  valid_status;                // indicate whether key item is valid or not. VALID_STATUS and INVALID_STATUS.
    uint32 set_id;                      // the id of set that the key pertain to.
    uint32 pos;                         // the position of current key item in file.
    uint32 next_key_pos;                // the position of next key item in file. NULL_POINTER means current key is the last one.
    uint32 hash_next;                   // the position of next key item in hash chain in file. NULL_POINTER means current key is the last one.
    char   name[KEY_NAME_LEN + 1];      // the key name
    uint32 follow_size;                 // the size of key value following key item.
    uint32 value_pos;                   // value position. 0 means value follows key item, and other value means the real value's position in file.
    uint32 value_len;                   // the length of value.
    uint32 value_buf_size;              // the size of buffer that key value takes up.
} key_item_t;


typedef struct _item_t
{
    int32 type;
} item_t;



#define KI_M_NEXT_KEY_POS_OFFSET        MEMBER_OFFSET( key_item_t, next_key_pos )
#define KI_M_VALID_STATUS_OFFSET        MEMBER_OFFSET( key_item_t, valid_status )
#define KI_M_HASH_NEXT_OFFSET           MEMBER_OFFSET( key_item_t, hash_next )
#define KI_M_VALUE_OFFSET               MEMBER_OFFSET( key_item_t, value )
#define KI_M_NAME_OFFSET                MEMBER_OFFSET( key_item_t, name )
#define KI_M_VALUE_LEN_OFFSET           MEMBER_OFFSET( key_item_t, value_len )
#define ST_M_VALID_STATUS_OFFSET        MEMBER_OFFSET( set_item_t, valid_status )
#define RFO_M_HASH_TBL_OFFSET           MEMBER_OFFSET( reg_file_org_t, hash_tbl )
#define RFO_M_CONTENT_OFFSET            MEMBER_OFFSET( reg_file_org_t, content )
#define RFH_M_FIRST_SET_POS_OFFSET      MEMBER_OFFSET( reg_file_head_t, first_set_pos )
#define RFH_M_MODIFIED_STATUS_OFFSET    MEMBER_OFFSET( reg_file_head_t, modified_status )
#define RFH_M_VALID_STATUS_OFFSET       MEMBER_OFFSET( reg_file_head_t, valid_status )


/*-------------------------------- global data -------------------------------*/
HNODE_head_node_t  HNODE_head = { 0, NULL };
char reg_root_path[256];
g_mutex_t global_chain_mutex = NULL;


const int8 valid_char_tbl[256] =  // 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
{
    /*0*/	     0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0,
    /*1*/	     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /*2*/	     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /*3*/	     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /*4*/	     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /*5*/	     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /*6*/	     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /*7*/	     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
};


const int8 ident_char_tbl[256] =  // 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
{
    /*0*/	     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /*1*/	     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /*2*/	     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /*3*/	     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    /*4*/	     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /*5*/	     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
    /*6*/	     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /*7*/	     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
};

const int8 key_value_char_tbl[256] =  // 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
{
    /*0*/	     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /*1*/	     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /*2*/	     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /*3*/	     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
    /*4*/	     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /*5*/	     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /*6*/	     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /*7*/	     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
};


/*------------------------------ global function -----------------------------*/
#define is_white_space( ch )            ( ch == TAB  ||  ch == BLANK )
#define is_alpha( ch )                  ( ( 'a' <= ch && ch <= 'z' ) || ( 'A' <= ch && ch <= 'Z' ) )
#define is_number( ch )                 ( '0' <= ch  &&  ch <= '9' )
//#define is_ident_char( ch )             ( ( ch == '_' ) || is_number(ch) || is_alpha(ch) )
//#define is_key_value_char( ch )         ( ' ' <= ch && ch < 0x7F && ch != ';' )
static __s32  __CreateRegInfoFile( __s32 sys_version );
static __s32  __GetRegInfo( reg_info_t *info_p );
static __s32  __CopyReg( void );
static char  *gen_bak_file_name( const char *reg_file, char *bak_file );
static __s32  mark_dirty( G_FILE *fp );


__inline __Bool is_ident_char( uint8 ch )
{
    return ident_char_tbl[ch];
}


__inline __Bool is_key_value_char( uint8 ch )
{
    return  key_value_char_tbl[ch];
}


__inline __Bool is_valid_char( uint8 ch )
{
    return valid_char_tbl[ch];
}





__inline __Bool is_legal_key_value( const char *str )
{
    while( *str != '\0' )
    {
        if( !is_key_value_char( *str ) )
            return __FALSE;
        str++;
    }
    if( is_white_space( *(str - 1) ) )                      // white space can't appear in the end of key value
        return __FALSE;
    return __TRUE;
}





__inline static char *skip_white_space( const char *str )
{
    while( is_white_space( *str ) )
        str++;
    return (char *)str;
}




__inline static const char *reverse_skip_white_space( const char *str )
{
    const char *p;

    p = str + g_strlen( str ) - 1;
    while( is_white_space( *p ) )
        p--;
    return p;
}




static uint32 extract_identifier( const char *str, char *ident, uint32 max_len, char **end_p )
{
    const char *p;
    const char *p2;
    uint32 i;

    p2 = p = skip_white_space( str );
    if( *p != '_' && !is_alpha( *p ) )                      // check whether the first char is legal.
    {
        return 0;
    }
    i = 0;
    while( i < max_len && is_ident_char( *p ) )
    {
        *ident++ = *p++;
        i++;
    }
    if( i >= max_len )                                          // string is too long
        return 0;

    if( end_p != NULL )
        *end_p = (char *)p;
    *ident = '\0';

    return ( p - p2 );
}



static __Bool is_identifier( const char *str )
{
    if( *str != '_' && !is_alpha( *str ) )                  // check whether the first char is legal.
    {
        return __FALSE;
    }
    str++;
    while( *str != '\0' )
    {
        if( !is_ident_char( *str ) )
            return __FALSE;
        str++;
    }
    return __TRUE;
}



static __Bool is_valid_set_name( const char *set_name )
{
    if(     set_name == NULL
            ||  g_strlen( set_name ) == 0
            ||  g_strlen( set_name ) > SET_NAME_LEN
            || !is_identifier(set_name) )
        return __FALSE;
    else
        return __TRUE;
}



static __Bool is_valid_key_name( const char *key_name )
{
    if(     key_name == NULL
            ||  g_strlen( key_name ) == 0
            ||  g_strlen( key_name ) > KEY_NAME_LEN
            || !is_identifier(key_name) )
        return __FALSE;
    else
        return __TRUE;
}


static __Bool is_valid_key_value( const char *key_value )
{
    if(     key_value == NULL
            ||  g_strlen( key_value ) == 0
            ||  g_strlen( key_value ) > KEY_VALUE_LEN
            || !is_legal_key_value( key_value) )
        return __FALSE;
    else
        return __TRUE;
}



void str_to_upper( const char *input, char *output )
{
    const char  *p;
    char  *q;

    p = input;
    q = output;
    while( *p != '\0' )
        *q++ = (char)g_toupper( *p++ );
    *q = '\0';
}



static char *gen_reg_file_name( const char *node, char *reg_file )
{
    g_strcpy( reg_file, reg_root_path );
    g_strcat( reg_file, node );
    /* append register file extention */
    g_strcat( reg_file, REG_FILE_EXTENTION );
    return reg_file;
}



static void release_HNODE( HNODE_i hdl )
{
    set_desc_t *p;
    set_desc_t *head;

    if( hdl == NULL )
        return;

    head = hdl->first_set;
    while( head != NULL )
    {
        p = head;
        head = head->next_set;
        g_free( p );
    }
    hdl->first_set = NULL;

    if( hdl->node != NULL )
    {
        g_free( hdl->node );
        hdl->node = NULL;
    }
    if( hdl->reg_fp != NULL )
    {
        g_fclose( hdl->reg_fp );
        hdl->reg_fp = NULL;
    }
    if( hdl->reg_file != NULL )
    {
        g_free( hdl->reg_file );
        hdl->reg_file = NULL;
    }

    hdl->label = INVALID_LABEL;
    g_free( hdl );
}





static int32 hash_func( const char *s )
{
    uint8  *p;
    uint32 h;
    uint32 g;

    for ( h = 0, p = (unsigned char *)s; *p != '\0'; p = p + 1 )
    {
        h = (h << 4) + (*p);

        g = (h & 0xf0000000);
        if ( g )
        {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
    }

    return h;
}




static uint32 key_hash_func( uint32 set_id, const char *key_name, uint32 hash_table_len )
{
    uint32 hash_value;

    hash_value = hash_func( key_name ) + set_id;

    return ( hash_value & ( hash_table_len - 1 ) );
}





/***************************************************************************************************
*Name        : search_key_in_hash
*Prototype   : int32 search_key_in_hash( G_FILE *fp, uint32 set_id, const char *key_name,
*                                        uint32 *pre_node, uint32 *cur_node )
*Arguments   : fp             pointer to file
*              set_id         id of set
*              key_name       key name
*              pre_node       return information
*              cur_node       return information
*Return      : NOT_FIND_ITEM_PRE_IS_HEAD_NODE     not find destination. previous item is head node.
*                                                 pre_node is the index of head node in hash table.
*                                                 cur_node is the position of next item in file.
*              NOT_FIND_ITEM_PRE_IN_CHAIN         not find destination. pre_node is the position of
*                                                 previous item in file. cur_node is the position of
*                                                 next item in file.
*              FIND_ITEM_PRE_IS_HEAD_NODE         find destination. previous item is head node.
*                                                 pre_node is the index of head node in hash table.
*                                                 cur_node is the position of current item in file.
*              FIND_ITEM_PRE_IN_CHAIN             find destination. pre_node is the position of
*                                                 previous item in file. cur_node is the position of
*                                                 current item in file.
*Description : search a key of a set in hash chain.
*Other       :
***************************************************************************************************/
static int32 search_key_in_hash( G_FILE *fp, uint32 set_id, const char *key_name, uint32 *pre_node, uint32 *cur_node )
{
    uint32 hash_value;
    uint32 cur_p;
    uint32 pre_p;
    uint32 count;
    key_item_t key_item;

    hash_value = key_hash_func( set_id, key_name, HASH_TBL_LEN );
    pre_p = hash_value;
    g_fseek( fp, RFO_M_HASH_TBL_OFFSET + hash_value * sizeof(uint32), SEEK_SET );
    g_fread( &cur_p, 1, sizeof(uint32), fp );
    if( cur_p == NULL_POINTER )
    {
        *pre_node = hash_value;
        *cur_node = NULL_POINTER;
        return NOT_FIND_ITEM_PRE_IS_HEAD_NODE;
    }

    count = 0;
    g_fseek( fp, cur_p, SEEK_SET );
    g_fread( &key_item, 1, sizeof(key_item), fp );
    while( !( key_item.set_id == set_id && g_strcmp( key_name, key_item.name ) == 0 )
            && key_item.hash_next != NULL_POINTER )
    {
        pre_p = cur_p;
        cur_p = key_item.hash_next;
        g_fseek( fp, cur_p, SEEK_SET );
        g_fread( &key_item, 1, sizeof(key_item), fp );
        count++;
    }
    if( key_item.set_id == set_id && g_strcmp( key_name, key_item.name ) == 0 )   // destination is found
    {
        *pre_node = pre_p;
        *cur_node = cur_p;
        if( count == 0 )
            return FIND_ITEM_PRE_IS_HEAD_NODE;              // pre_node is in hash address table
        else
            return FIND_ITEM_PRE_IN_CHAIN;                  // pre_node is in chain.
    }
    else                                                    // current node's value is larger than destination's value
    {
        *pre_node = pre_p;
        *cur_node = cur_p;
        if( count == 0 )
            return NOT_FIND_ITEM_PRE_IS_HEAD_NODE;          // pre_node is in hash address table
        else
            return NOT_FIND_ITEM_PRE_IN_CHAIN;              // pre_node is in chain.
    }
}





static set_desc_t *search_set_in_set_list( set_desc_t *first_set, const char *set_name )
{
    set_desc_t *p;

    for( p = first_set; p != NULL; p = p->next_set )
    {
        if( g_strcmp( p->name, set_name ) == 0 )
            return p;
    }
    return NULL;
}





static set_desc_t *search_set( HNODE_i hdl, const char *set_name )
{
    set_desc_t *p;

    if( hdl->cur_set != NULL  &&  g_strcmp( hdl->cur_set->name, set_name ) == 0 )
        return hdl->cur_set;
    /* search set in set list */
    p = search_set_in_set_list( hdl->first_set, set_name );
    if( p != NULL )
        hdl->cur_set = p;

    return p;
}





#define judge_valid_char( )      { \
if( !is_valid_char( *buf_p ) )\
{\
	if( g_strcmp( buf_p, REG_BUF_END ) == 0 )\
	{\
		count = g_fread( buf, 1, BUF_LEN, ini_fp );\
		stage += BUF_LEN;\
		buf_p = buf;\
		if( count < BUF_LEN )\
		{\
			if( g_feof( ini_fp ) )\
			{\
				if( count == 0 || buf[count-1] != '\n')\
				{\
					buf[count] = '\r';\
					buf[count+1] = '\n';\
					g_strcpy( buf + count + 2, REG_FILE_END );\
				}\
				else\
					g_strcpy( buf + count, REG_FILE_END );\
			}\
			else\
			{\
				__wrn("FATAL ERROR. errer occurs in line %u of file %s.\n", __LINE__, __FILE__ );\
				return REG_ERR;\
			}\
		}\
	}\
	else if( g_strcmp( buf_p, REG_FILE_END ) == 0 )\
		goto exit;\
	else\
	{\
		__wrn("Fail in scan reg file. Because find illegal char in file.\n" );\
		return REG_ERR;\
	}\
}\
}





#define get_line( )            {	\
p = line_buf;\
while( buf_p + stage - start_p <= LINE_MAX_LEN  )\
{\
	judge_valid_char( );\
	if( *buf_p != '\r' && *buf_p != '\n' )\
	{\
		judge_valid_char( );\
		*p++ = *buf_p++;\
	}\
	else\
	{\
		buf_p++;\
		judge_valid_char( );\
		if( *buf_p == '\r' || *buf_p == '\n' )\
			buf_p++;\
		*p = '\0';\
		p = line_buf;\
		stage = 0;\
		start_p = buf_p;\
		break;\
	}\
}\
if( buf_p + stage - start_p > LINE_MAX_LEN )\
{\
	__wrn("Fail in scan reg file. Because some line is too long, beyond the max line length %u.in line %d.\n", LINE_MAX_LEN, __LINE__ );\
	return REG_ERR;\
}\
}





static void init_hash_tbl( G_FILE *fp )
{
#define POINTER_NR      256
    uint32 i;
    uint32 count;
    uint32 null_hash_tbl[POINTER_NR];
    uint32 *t;

    t = null_hash_tbl;
    for( i = 0; i < POINTER_NR; i++ )
        *t++ = NULL_POINTER;

    for( i = 0, count = HASH_TBL_LEN / POINTER_NR;  i < count;  i++ )
        g_fwrite( null_hash_tbl, sizeof(uint32), POINTER_NR, fp );
}





static void release_set_chain( set_desc_t *head )
{
    set_desc_t *p;

    while( head != NULL )
    {
        p = head;
        head = head->next_set;
        g_free( p );
    }
}






/***************************************************************************************************
*Name        : scan_ini_file
*Prototype   : int32 scan_ini_file( G_FILE *ini_fp, G_FILE *reg_fp )
*Arguments   : ini_fp         pointer to ini file
*              reg_fp         pointer to format   file
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : scan ini file, extract information and then write these information to register file
*              in some data structure.
*Other       :
***************************************************************************************************/
//#if ( EPDK_OS == EPDK_OS_EPOS )
//	#pragma arm section code="scan_ini_file"
//#endif
static int32 scan_ini_file( G_FILE *ini_fp, G_FILE *reg_fp )
{
#define BUF_LEN    256
    char  buf[ BUF_LEN + sizeof(REG_FILE_END) ];
    char  line_buf[ LINE_MAX_LEN + 1 ];
    const char *buf_p;
    char *p;
    char   set_name[ SET_NAME_LEN + 1 ];
    char   key_name[ KEY_NAME_LEN + 1 ];
    char   key_value[ KEY_VALUE_LEN + 1 ];
    blank_item_t  blank_item;
    comment_item_t  comment_item;
    set_item_t  set_item;
    key_item_t  key_item;
    set_desc_t  *cur_set_p;
    uint32  cur_key_pos;
    uint32 pre_node;
    uint32 cur_node;
    int32  key_exist_status;
    uint32 count;
    uint32 stage;
    const char *start_p;
    uint32 total_size;
    uint32 set_nr;
    uint32 set_id_base;
    set_desc_t  *first_set;
    set_desc_t  *last_set;
    set_desc_t  *pre_set;
    reg_file_head_t  ffh;

    /* initialisze register file head */
    g_strncpy( ffh.magic, REG_MAGIC, MAGIC_SIZE );
    ffh.version = REG_FILE_VERSION;
    ffh.valid_status = INVALID_STATUS;
    ffh.set_nr = 0;
    ffh.first_set_pos = NULL_POINTER;
    ffh.total_size = 0;
    ffh.set_id_base = 0;
    g_fwrite( &ffh, 1, sizeof(ffh), reg_fp );

    /* initialisze hash table */
    init_hash_tbl( reg_fp );

    total_size = RFO_M_CONTENT_OFFSET;

    g_memset( &blank_item, '_', sizeof(blank_item_t) );
    g_memset( &comment_item, 0, sizeof(comment_item_t) );
    g_memset( &set_item, 0, sizeof(set_item_t) );
    g_memset( &key_item, 0, sizeof(key_item_t) );
    set_item.type = SET_ITEM;
    comment_item.type = COMMENT_ITEM;
    blank_item.type = BLANK_ITEM;
    key_item.type = KEY_ITEM;
    cur_set_p = NULL;
    cur_key_pos = NULL_POINTER;

    set_nr = 0;
    set_id_base = 0;
    first_set = NULL;
    last_set = NULL;

    g_strcpy( buf + BUF_LEN, REG_BUF_END );
    buf_p = buf + BUF_LEN;
    stage = 0;
    start_p = buf_p;
    while( 1 )
    {
        get_line( );
        p = skip_white_space( p );
        if( *p == '\0' )                                              // blank line
        {
            g_fwrite( &blank_item, sizeof(blank_item_t), 1, reg_fp );
            if( cur_set_p != NULL )
            {
                cur_set_p->seq_status  = SEQ;
                cur_set_p->set_size    += sizeof(blank_item_t);
            }
            total_size += sizeof(blank_item_t);
        }
        else if( *p == ';' )                                          // comment line
        {
            uint32 len;

            len = g_strlen( line_buf );
            comment_item.len = len;
            g_fwrite( &comment_item, sizeof(comment_item_t), 1, reg_fp );
            g_fwrite( line_buf, 1, len + 1, reg_fp );
            if( cur_set_p != NULL )
            {
                cur_set_p->seq_status  = SEQ;
                cur_set_p->set_size    += sizeof(comment_item_t) + len + 1;
            }
            total_size += sizeof(comment_item_t) + len + 1;
        }
        else if( *p == '[' )                                          // set line
        {
            if( extract_identifier( p + 1, set_name, SET_NAME_LEN, &p ) == 0 )
            {
                __wrn("Fail in scan reg file. Because error in parsing the line \"%s\".\n", line_buf );
                release_set_chain( first_set );
                return REG_ERR;
            }
            p = skip_white_space( p );
            if( *p != ']' )
            {
                __wrn("Fail in scan reg file. Because error in parsing the line \"%s\".\n", line_buf );
                release_set_chain( first_set );
                return REG_ERR;
            }
            p++;
            p = skip_white_space( p );
            if( *p != '\0' )
            {
                __wrn("Fail in scan reg file. Because error in parsing the line \"%s\".\n", line_buf );
                release_set_chain( first_set );
                return REG_ERR;
            }
            if( search_set_in_set_list( first_set, set_name ) != NULL )
            {
                __wrn("Fail in scan reg file. Because the set \"%s\" appears more than once.\n", set_name );
                release_set_chain( first_set );
                return REG_ERR;
            }
            if( cur_set_p != NULL )
                cur_set_p->next_set_pos = total_size;
            /* create a new set description node */
            cur_set_p = (set_desc_t *) g_malloc( sizeof(set_desc_t) );
            if( cur_set_p == NULL )
            {
                __wrn("Fail in scan reg file. Because allocating memory fail.\n");
                release_set_chain( first_set );
                return REG_ERR;
            }
            /* put current set in the end of chain */
            if( first_set == NULL )                            // current set is the first set
                first_set = cur_set_p;
            else
                last_set->next_set = cur_set_p;
            last_set = cur_set_p;
            set_nr++;
            cur_set_p->append_pos    = NULL_POINTER;
            cur_set_p->seq_status    = NOT_SEQ;
            cur_set_p->next_set      = NULL;
            cur_set_p->next_set_pos  = NULL_POINTER;
            cur_set_p->set_id        = set_id_base++;
            cur_set_p->key_nr        = 0;
            cur_set_p->set_size      = sizeof(set_item_t);
            cur_set_p->pos           = total_size;
            cur_set_p->first_key_pos = NULL_POINTER;
            cur_set_p->last_key_pos  = NULL_POINTER;
            cur_set_p->next_key_pos  = NULL_POINTER;
            g_strcpy( cur_set_p->name, set_name );
            /* set set item information */
            g_fwrite( &set_item, sizeof(set_item_t), 1, reg_fp );
            total_size += sizeof(set_item_t);
        }
        else
        {
            const char *end_p;
            char *temp_p;
            uint32 len;
            uint32 pre_node_m_next_pos;

            if( extract_identifier( p, key_name, KEY_NAME_LEN, &p ) == 0 )
            {
                __wrn("Fail in scan reg file. Because error in parsing the line \"%s\".\n", line_buf );
                release_set_chain( first_set );
                return REG_ERR;
            }
            p = skip_white_space( p );
            if( *p != '=' )
            {
                __wrn("Fail in scan reg file. Because error in parsing the line \"%s\".\n", line_buf );
                release_set_chain( first_set );
                return REG_ERR;
            }
            p++;
            p = skip_white_space( p );
            end_p = reverse_skip_white_space( line_buf );
            if( p > end_p )
            {
                __wrn("Fail in scan reg file. Because can't find key value in parsing the line \"%s\".\n", line_buf );
                release_set_chain( first_set );
                return REG_ERR;
            }
            if( ( end_p - p ) >= KEY_VALUE_LEN )
            {
                __wrn("Fail in scan reg file. Because key value is too long in parsing the line \"%s\".\n", line_buf );
                release_set_chain( first_set );
                return REG_ERR;
            }
            for( temp_p = key_value, len = 0; p <= end_p;  )
            {
                *temp_p++ = *p++;
                len++;
            }
            *temp_p = '\0';
            if( !is_legal_key_value( key_value ) )
            {
                __wrn("Fail in scan reg file. Because the key value \"%s\" contian illegal char.\n", key_value );
                release_set_chain( first_set );
                return REG_ERR;
            }
            if( cur_set_p == NULL )
            {
                __wrn("Fail in scan reg file. Because the key line \"%s\" doesn't belong to some set.\n", line_buf );
                release_set_chain( first_set );
                return REG_ERR;
            }
            /* check whether the key has existed in the set */
            key_exist_status = search_key_in_hash( reg_fp, cur_set_p->set_id, key_name, &pre_node, &cur_node );
            if( key_exist_status == FIND_ITEM_PRE_IS_HEAD_NODE  ||  key_exist_status == FIND_ITEM_PRE_IN_CHAIN )
            {
                __wrn("Fail in scan reg file. Because the key \"%s\" appears in set \"%s\" more than once.\n", key_name, cur_set_p->name );
                release_set_chain( first_set );
                return REG_ERR;
            }
            /* add the key to hash chain */
            if( key_exist_status == NOT_FIND_ITEM_PRE_IS_HEAD_NODE )   // current node's previous node is in hash address table
                pre_node_m_next_pos = RFO_M_HASH_TBL_OFFSET + sizeof(uint32) * pre_node;
            else                                                      // current node's previous node is in chain
                pre_node_m_next_pos = pre_node + KI_M_HASH_NEXT_OFFSET;
            g_fseek( reg_fp, pre_node_m_next_pos, SEEK_SET );
            g_fwrite( &(total_size), sizeof(total_size), 1, reg_fp );
            key_item.hash_next = cur_node;
            /* add the key to sequence chain */
            if( cur_set_p->first_key_pos == NULL_POINTER )
            {
                cur_key_pos = cur_set_p->first_key_pos = total_size;
            }
            else
            {
                g_fseek( reg_fp, cur_key_pos + KI_M_NEXT_KEY_POS_OFFSET, SEEK_SET );
                cur_key_pos = total_size;
                g_fwrite( &cur_key_pos, sizeof(cur_key_pos), 1, reg_fp );
            }
            /* modify set information */
            cur_set_p->last_key_pos = cur_key_pos;
            cur_set_p->key_nr++;
            cur_set_p->set_size     += ( sizeof(key_item_t) + len + 1 );
            cur_set_p->seq_status   = SEQ;
            /* set key information */
            g_strcpy( key_item.name, key_name );
            key_item.valid_status   = VALID_STATUS;
            key_item.set_id         = cur_set_p->set_id;
            key_item.pos            = total_size;
            key_item.next_key_pos   = NULL_POINTER;
            key_item.follow_size    = len + 1;
            key_item.value_pos      = 0;
            key_item.value_len      = len;
            key_item.value_buf_size = len + 1;
            /* write key item and value */
            g_fseek( reg_fp, cur_key_pos, SEEK_SET );
            g_fwrite( &key_item, 1, sizeof(key_item_t), reg_fp );
            g_fwrite( key_value, 1, len + 1, reg_fp );
            total_size += ( sizeof(key_item_t) + len + 1 );
        }
    }

exit:
    /* write SEQ_END */
    {
        item_t item;

        item.type = SEQ_END;
        g_fwrite( &item, 1, sizeof(item), reg_fp );
        total_size += sizeof(item);
    }
    /* modify file head */
    ffh.valid_status = VALID_STATUS;
    ffh.set_nr = set_nr;
    ffh.total_size = total_size;
    ffh.set_id_base = set_id_base;
    ffh.invalid_space_size = 0;
    ffh.modified_status = MODIFIED;
    if( first_set != NULL )
        ffh.first_set_pos = first_set->pos;
    else
        ffh.first_set_pos = NULL_POINTER;

    g_fseek( reg_fp, 0, SEEK_SET );
    g_fwrite( &ffh, 1, sizeof(ffh), reg_fp );

    /* modify set item information */
    set_item.type = SET_ITEM;
    set_item.valid_status = VALID_STATUS;
    pre_set = NULL;
    cur_set_p = first_set;
    while( cur_set_p != NULL )
    {
        set_item.append_pos    = cur_set_p->append_pos;
        set_item.seq_status    = cur_set_p->seq_status;
        set_item.set_id        = cur_set_p->set_id;
        set_item.key_nr        = cur_set_p->key_nr;
        set_item.set_size      = cur_set_p->set_size;
        set_item.next_set_pos  = cur_set_p->next_set_pos;
        set_item.pos           = cur_set_p->pos;
        set_item.first_key_pos = cur_set_p->first_key_pos;
        set_item.last_key_pos  = cur_set_p->last_key_pos;
        g_strcpy( set_item.name, cur_set_p->name );

        g_fseek( reg_fp, set_item.pos, SEEK_SET );
        g_fwrite( &set_item, 1, sizeof(set_item), reg_fp );

        pre_set = cur_set_p;
        cur_set_p = cur_set_p->next_set;
        g_free( pre_set );
    }
    return REG_OK;
}



static char *gen_reg_file_name_from_ini_file_name( const char *ini_file, char *reg_file )
{
    g_strcpy( reg_file, ini_file );
    /* delete ini file extention */
    reg_file[ g_strlen(reg_file) - g_strlen(INI_FILE_EXTENTION) ] = '\0';
    /* append register file extention */
    g_strcat( reg_file, REG_FILE_EXTENTION );
    return reg_file;
}





/***************************************************************************************************
*Name        : __Ini2Reg
*Prototype   : int32 __Ini2Reg( const char *ini_file )
*Arguments   : ini_file       full name of ini file
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : create a register file corresponding to ini file.
*Other       :
***************************************************************************************************/
int32 __Ini2Reg( const char *ini_file )
{
    char reg_file[256];
    G_FILE *ini_fp;
    G_FILE *reg_fp;
    int32  status;

    /* compose register file name */
    gen_reg_file_name_from_ini_file_name( ini_file, reg_file );

    /* open ini file */
    ini_fp = g_fopen( ini_file, "rb" );
    if( ini_fp == NULL )
    {
        __wrn("Fail in generating register file based on ini file. Because can't open ini file.\n");
        return REG_ERR;
    }

    /* open register file */
    reg_fp = g_fopen( reg_file, "wb+" );
    if( reg_fp == NULL )
    {
        __wrn("Fail in generating register file based on ini file. Because can't create register file.\n");
        g_fclose( ini_fp );
        return REG_ERR;
    }

    status = scan_ini_file( ini_fp, reg_fp );
    g_fclose( reg_fp );
    g_fclose( ini_fp );
    return status;
}





static void add_node( HNODE_i hdl )
{
    hdl->pre_HNODE = NULL;
    hdl->next_HNODE = HNODE_head.next_HNODE;
    HNODE_head.next_HNODE = hdl;
    if( hdl->next_HNODE != NULL )
        hdl->next_HNODE->pre_HNODE = hdl;

    HNODE_head.count++;
}





static void delete_node( HNODE_i hdl )
{
    if( hdl->pre_HNODE == NULL )
        HNODE_head.next_HNODE = hdl->next_HNODE;
    else
        hdl->pre_HNODE->next_HNODE = hdl->next_HNODE;
    if( hdl->next_HNODE != NULL )
        hdl->next_HNODE->pre_HNODE = hdl->pre_HNODE;

    HNODE_head.count--;
}





/***************************************************************************************************
*Name        : open_node_copy
*Prototype   : HNODE_i open_node_copy( HNODE_i hdl )
*Arguments   : hdl         an existing HNODE_i hdl
*Return      : NULL           operation fail
*              !NULL          operation succeed, return a new HNODE_i hdl
*Description : in REG_READONLY mode, create a new hdl base on existiong hdl for the same node.
*Other       :
***************************************************************************************************/
//#if ( EPDK_OS == EPDK_OS_EPOS )
//	#pragma arm section code="open_node_copy"
//#endif
static HNODE_i open_node_copy( HNODE_i hdl )
{
    HNODE_i new_handle;
    HNODE_i cursor;
    set_desc_t *set_p;
    set_desc_t *new_set_p = 0;
    set_desc_t *pre_p = 0;

    new_handle = (HNODE_i) g_malloc( sizeof(NODE_i_t) );
    if( new_handle == NULL )
    {
        __wrn("Error in open node \"%s\" in mode READONLY. Because allocating memory fail.\n", hdl->node );
        return NULL;
    }
    g_memcpy( new_handle, hdl, sizeof(NODE_i_t) );
    new_handle->first_set = NULL;
    new_handle->last_set = NULL;
    new_handle->next_set = NULL;
    for( set_p = hdl->first_set; set_p != NULL; pre_p = new_set_p, set_p = set_p->next_set)
    {
        new_set_p = (set_desc_t *) g_malloc( sizeof(set_desc_t) );
        if( new_set_p == NULL )
        {
            set_desc_t *head;
            set_desc_t *next;

            __wrn("Fail in open node \"%s\" in mode READONLY. Because allocating memory fail.\n", hdl->node );
            head = new_handle->first_set;
            while( head != NULL )
            {
                next = head;
                head = head->next_set;
                g_free( next );
            }
            g_free( new_handle );
            return NULL;
        }
        g_memcpy( new_set_p, set_p, sizeof(set_desc_t) );
        new_set_p->next_key_pos = NULL_POINTER;
        new_set_p->next_set = NULL;
        if( new_handle->first_set == NULL)
            new_handle->first_set = new_set_p;
        else
            pre_p->next_set = new_set_p;
    }
    new_handle->last_set = new_set_p;

    /* add the node to the node list */
    add_node( new_handle );

    /* the count of node hdl is modified */
    for( cursor = HNODE_head.next_HNODE; cursor != NULL; cursor = cursor->next_HNODE )
    {
        if( g_strcmp( cursor->node, new_handle->node ) == 0 )
            cursor->open_nr++;
    }

    //	__msg("Succeed in open node \"%s\" in REG_READONLY mode. the node has been open in REG_READONLY mode.\n", hdl->node );
    new_handle->label = HANDLE_LABEL;
    return new_handle;
}



/***************************************************************************************************
*Name        : copy_bak_to_reg
*Prototype   : G_FILE *copy_bak_to_reg( const char *reg_file )
*Arguments   : reg_file       full name of register file
*Return      : NULL           operation fail
*              !NULL          opeartion succeed, a pointer to register file
*Description : clear origin register file and then copy the content of corresponding backup file to
*              the register file.
*Other       :
***************************************************************************************************/
static G_FILE *copy_bak_to_reg( const char *reg_file )
{
    char   bak_file[255];
    int32  i;
    G_FILE *bak_fp;
    G_FILE *reg_fp;
    char   buf[512];
    uint32 count;

    g_strcpy( bak_file, reg_file );
    for( i = g_strlen( bak_file ) - 1; i >= 0; i-- )
    {
        if( bak_file[i] == '\\' )
            break;
    }
    if( i < 0 )                                             // not find '\'
    {
        return NULL;
    }

    gen_bak_file_name( reg_file, bak_file );
    bak_fp = g_fopen( bak_file, "rb" );
    if( bak_fp == NULL )
    {
        __wrn("Fail in copying backup file %s to register file %s. Because can't open backup file.\n", bak_file, reg_file );
        return NULL;
    }
    reg_fp = g_fopen( reg_file, "wb+" );
    if( reg_fp == NULL )
    {
        __wrn("Fail in copying backup file %s to register file %s. Because can't open register file.\n", bak_file, reg_file );
        return NULL;
    }
    /* copy content of backup file to register file */
    while( !g_feof( bak_fp ) )
    {
        count = g_fread( buf, 1, sizeof(buf), bak_fp );
        g_fwrite( buf, 1, count, reg_fp );
    }
    g_fclose( bak_fp );
    g_fseek( reg_fp, 0, SEEK_SET );
    return reg_fp;
}





/***************************************************************************************************
*Name        : scan_reg_file
*Prototype   : int32 scan_reg_file( HNODE_i hdl, G_FILE *reg_fp )
*Arguments   : handle         a handle of node
*              reg_fp         a pointer to a register file
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : scan register file and then extract some important information.
*Other       :
***************************************************************************************************/
static int32 scan_reg_file( HNODE_i hdl, G_FILE *reg_fp )
{
    reg_file_head_t ffh;
    set_desc_t *cur_set_p;
    uint32  next_set_pos;
    set_item_t  set_item;
    uint32 count;

    g_fseek( reg_fp, 0, SEEK_SET );
    g_fread( &ffh, 1, sizeof(ffh), reg_fp );
    hdl->total_size  = ffh.total_size;
    hdl->set_nr      = ffh.set_nr;
    hdl->set_id_base = ffh.set_id_base;
    hdl->invalid_space_size = ffh.invalid_space_size;
    hdl->modified_status = ffh.modified_status;

    /* invalidate file head */
    if( hdl->mode != REG_READONLY )
    {
        ffh.valid_status = INVALID_STATUS;
        g_fseek( reg_fp, 0, SEEK_SET );
        g_fwrite( &ffh, 1, sizeof(ffh), reg_fp );
    }

    /* scan sets */
    for( count = 0, next_set_pos = ffh.first_set_pos; next_set_pos != NULL_POINTER; next_set_pos = set_item.next_set_pos )
    {
        g_fseek( reg_fp, next_set_pos, SEEK_SET );
        g_fread( &set_item, 1, sizeof(set_item), reg_fp );
        if( set_item.valid_status == VALID_STATUS )
        {
            cur_set_p = (set_desc_t *) g_malloc( sizeof(set_desc_t) );
            if( cur_set_p == NULL )
            {
                __wrn("Error in scan register file. Because allocating memory fail.\n");
                release_set_chain( hdl->first_set );
                return REG_ERR;
            }
            /* put current set in the end of chain */
            if( hdl->first_set == NULL )                            // current set is the first set
                hdl->first_set = cur_set_p;
            else
                hdl->last_set->next_set = cur_set_p;
            hdl->last_set = cur_set_p;

            cur_set_p->next_set      = NULL;
            cur_set_p->next_key_pos  = NULL_POINTER;
            cur_set_p->append_pos    = set_item.append_pos;
            cur_set_p->seq_status    = set_item.seq_status;
            cur_set_p->set_id        = set_item.set_id;
            cur_set_p->key_nr        = set_item.key_nr;
            cur_set_p->set_size      = set_item.set_size;
            cur_set_p->next_set_pos  = set_item.next_set_pos;
            cur_set_p->pos           = set_item.pos;
            cur_set_p->first_key_pos = set_item.first_key_pos;
            cur_set_p->last_key_pos  = set_item.last_key_pos;
            g_strcpy( cur_set_p->name, set_item.name );

            count++;
        }
    }
    if( count != ffh.set_nr )
    {
        __wrn("FATAL ERROR! the number of total set doesn't match.\n");
        return REG_ERR;
    }
    else
        return REG_OK;
}





static int32 node_has_existed( const char *node )
{
    char reg_file[255];
    G_FILE *reg_fp;

    gen_reg_file_name( node, reg_file );
    reg_fp = g_fopen( reg_file, "rb" );
    if( reg_fp == NULL )
        return 0;
    else
    {
        g_fclose( reg_fp );
        return 1;
    }
}





/***************************************************************************************************
*Name        : __OpenNode
*Prototype   : HNODE_i __OpenNode( const char *node, int32 mode, __s32 *err_p )
*Arguments   : node           node to be open
*              mode           open mode. optional mode: REG_READONLY, REG_READWRITE and REG_CREATE.
*                             in REG_READONLY mode, node can be only read and can be open several
*                             handles at the same time. In REG_READWRITE mode, node can be read and
*                             write, and can be open only one hdl at the same time. In REG_CREATE
*                             mode, create a new node.
*              err_p          output. error code.
*Return      : NULL           operation fail
*              !NULL          operation succeed, a handle of node.
*Description : open a node in certain mode.
*Other       :
***************************************************************************************************/
HNODE __OpenNode( const char *node, int32 mode, __s32 *err_p )
{
    HNODE_i   p;
    reg_file_head_t ffh;
    uint8  mutex_err;

    /* lock global chain */
    g_pend_mutex( global_chain_mutex, &mutex_err );
    if( mutex_err != EVENT_OK )
    {
        __wrn("Error in open node \"%s\" in mode %d. Because fail in applying mutex.\n", node, mode );
        *err_p = REG_ERR_NORMAL_OPEN_ERR;
        return NULL;
    }
    /* check mode value */
    if( mode != REG_READONLY && mode != REG_READWRITE && mode != REG_CREATE )
    {
        __wrn("Fail in open node \"%s\" in mode %d. Because the mode is illegal.\n", node, mode );
        g_post_mutex( global_chain_mutex );
        *err_p = REG_ERR_NORMAL_OPEN_ERR;
        return NULL;
    }

    /* up to maximum open number */
    if( HNODE_head.count >= OPEN_NODE_MAX_NR )
    {
        __wrn("Fail in open node \"%s\" in mode %d. Because up to maximum open number %d.\n", node, mode, OPEN_NODE_MAX_NR );
        g_post_mutex( global_chain_mutex );
        *err_p = REG_ERR_NORMAL_OPEN_ERR;
        return NULL;
    }

    /* search node */
    for( p = HNODE_head.next_HNODE; p != NULL;  p = p->next_HNODE )
    {
        if( g_strcmp( p->node, node ) == 0 )
            break;
    }
    if( p != NULL )                                                   // the node has been open
    {
        if( p->mode == REG_READONLY  &&  mode == REG_READONLY )
        {
            HNODE_i temp_p;

            temp_p = open_node_copy( p );
            g_post_mutex( global_chain_mutex );

            if( temp_p != NULL )
            {
                __msg("Succeed in open node \"%s\" in mode %d.\n", node, mode );
            }
            else
            {
                __wrn("Fail in open node \"%s\" in mode %d. At line %u of file %s.\n", node, mode, __LINE__, __FILE__);
            }
            *err_p = REG_OK;
            return (HNODE)temp_p;
        }
        else
        {
            __wrn("Fail in open node \"%s\" in mode %d. Because the node has been open in NOT REG_READONLY mode.\n", node, mode );
            g_post_mutex( global_chain_mutex );
            *err_p = REG_ERR_NORMAL_OPEN_ERR;
            return NULL;
        }
    }

    if( mode == REG_CREATE  && node_has_existed( node ) )
    {
        __wrn("Fail in open node \"%s\" in REG_CREATE mode. Because the node has existed.\n", node );
        g_post_mutex( global_chain_mutex );
        *err_p = REG_ERR_NORMAL_OPEN_ERR;
        return NULL;
    }

    /* create a new HNODE_i */
    p = (HNODE_i) g_malloc( sizeof(NODE_i_t) );
    if( p == NULL )
    {
        __wrn("Error in open node \"%s\" in mode %d. Because allocating memory fail.\n", node, mode );
        g_post_mutex( global_chain_mutex );
        *err_p = REG_ERR_NORMAL_OPEN_ERR;
        return NULL;
    }
    g_memset( p, 0, sizeof(NODE_i_t) );
    p->node = (char *)g_malloc( g_strlen(node) + 1 );
    if( p->node == NULL )
    {
        __wrn("Fail in open node \"%s\" in mode %d. Because allocating memory fail.\n", node, mode );
        g_free( p );
        g_post_mutex( global_chain_mutex );
        *err_p = REG_ERR_NORMAL_OPEN_ERR;
        return NULL;
    }
    g_strcpy( p->node, node );
    p->mode = mode;
    p->open_nr = 1;

    /* open register file */
    p->reg_file = (char *) g_malloc( g_strlen(reg_root_path) + g_strlen(node) + g_strlen(REG_FILE_EXTENTION) + 1 );
    if( p->reg_file == NULL )
    {
        __wrn("Error in open node \"%s\" in mode %d. Fail in allocating memory.\n", node, mode );
        *err_p = REG_ERR_NORMAL_OPEN_ERR;
        goto error;
    }
    gen_reg_file_name( node, p->reg_file );
    if( mode == REG_READONLY )
        p->reg_fp = g_fopen( p->reg_file, "rb" );
    else if( mode == REG_READWRITE )
        p->reg_fp = g_fopen( p->reg_file, "rb+" );
    else if( mode == REG_CREATE )
        p->reg_fp = g_fopen( p->reg_file, "wb+" );
    if( p->reg_fp == NULL )
    {
        __wrn("Fail in open node \"%s\" in mode %d. Because open register file %s fail.\n", node, mode, p->reg_file );
        *err_p = REG_ERR_NORMAL_OPEN_ERR;
        goto error;
    }
    if( mode == REG_READONLY || mode == REG_READWRITE )
    {
        g_fread( &ffh, 1, sizeof(ffh), p->reg_fp );
        if( g_strncmp( ffh.magic, REG_MAGIC, MAGIC_SIZE ) != 0 || ffh.valid_status == INVALID_STATUS )
        {
            g_fclose( p->reg_fp );
            __wrn("Warning. reg file %s is NOT valid, should use backup file.\n", p->reg_file);
            p->reg_fp = copy_bak_to_reg( p->reg_file );
            if( p->reg_fp == NULL )
            {
                __wrn("Fail in open node \"%s\" in mode %d. Because creating backup file fail.\n", node, mode );
                *err_p = REG_ERR_DIRTY_DATA;
                goto error;
            }
        }
    }

    if( mode == REG_READONLY || mode == REG_READWRITE )
    {
        /* scan ini file and add information to backup file */
        if( scan_reg_file( p, p->reg_fp ) == REG_ERR )
        {
            __wrn("Fail in open node \"%s\" in mode %d. Because scan ini file fail.\n", node, mode );
            *err_p = REG_ERR_DIRTY_DATA;
            goto error;
        }
        /* mark file invalid */
        if( mode == REG_READWRITE )
        {
            mark_dirty( p->reg_fp );
        }
    }
    else if( mode == REG_CREATE )
    {
        /* initialisze register file head */
        g_strncpy( ffh.magic, REG_MAGIC, MAGIC_SIZE );
        ffh.version = REG_FILE_VERSION;
        ffh.valid_status = INVALID_STATUS;
        ffh.set_nr = 0;
        ffh.first_set_pos = NULL_POINTER;
        ffh.total_size = 0;
        ffh.set_id_base = 0;
        g_fwrite( &ffh, 1, sizeof(ffh), p->reg_fp );

        /* initialisze hash table */
        init_hash_tbl( p->reg_fp );

        p->total_size = RFO_M_CONTENT_OFFSET;
        p->set_nr = 0;
        p->set_id_base = 0;
        p->invalid_space_size = 0;
        p->modified_status = MODIFIED;
        p->first_set = NULL;
        p->last_set  = NULL;
    }

    /* add the node to the node list */
    add_node( p );

    __msg("Succeed in open node \"%s\" in mode %d.\n", node, mode );
    p->label = HANDLE_LABEL;
    g_post_mutex( global_chain_mutex );
    *err_p = REG_OK;
    return (HNODE)p;

error:
    release_HNODE( p );
    g_post_mutex( global_chain_mutex );

    return NULL;
}





static int32 handle_is_invalid( HNODE_i hdl )
{
    if( hdl == NULL )
        return 1;
    if( hdl->label != HANDLE_LABEL )
        return 1;

    return 0;
}





static void compose_set_line( char *buf, const char *set_name )
{
    buf[0] = '[';
    buf[1] = '\0';
    g_strcat( buf, set_name );
    g_strcat( buf, "]"REG_LINE_END );
}




static void compose_key_line( char *key_line, const char *key_name, const char *key_value )
{
    g_strcpy( key_line, key_name );
    g_strcat( key_line, " = " );
    g_strcat( key_line, key_value );
    g_strcat( key_line, REG_LINE_END );
}





/***************************************************************************************************
*Name        : __Reg2Ini
*Prototype   : int32 __Reg2Ini( const char *ini_file )
*Arguments   : ini_file       full name of ini file
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : create a ini file corresponding to register file
*Other       :
***************************************************************************************************/
int32 __Reg2Ini( const char *ini_file )
{
    char reg_file[256];
    G_FILE  *ini_fp;
    G_FILE  *reg_fp;
    reg_file_head_t ffh;
    char   line_buf[ LINE_MAX_LEN + 2 ];
    uint32 set_pos;
    uint32 append_pos;

    comment_item_t  comment_item;
    key_item_t  key_item;
    set_item_t  set_item;
    item_t      item;                         // \0   =   REG_LINE_END
    char key_line[ KEY_NAME_LEN + KEY_VALUE_LEN + 1 + 1 + sizeof(REG_LINE_END) - 1 ];
    char key_value[ KEY_VALUE_LEN + 1 ];

    /* compose register file name */
    gen_reg_file_name_from_ini_file_name( ini_file, reg_file );
    /* open register file */
    reg_fp = g_fopen( reg_file, "rb" );
    if( reg_fp == NULL )
    {
        __wrn("Fail in generate ini file based on register file. Because can't open register file.\n");
        return REG_ERR;
    }
    g_fread( &ffh, 1, sizeof(ffh), reg_fp );
    if( ffh.valid_status == INVALID_STATUS )
    {
        __wrn("Fail in generate ini file based on register file. Because current register file is invalid.\n");
        return REG_ERR;
    }
    /* open and clear ini file */
    ini_fp = g_fopen( ini_file, "wb+" );
    if( ini_fp == NULL )
    {
        __wrn("Fail in generate ini file based on register file. "
              "Because can't create a new empty ini file.\n");
        g_fclose( reg_fp );
        return REG_ERR;
    }

    /* scan the comment line and blank line before the first set */
    g_fseek( reg_fp, RFO_M_CONTENT_OFFSET, SEEK_SET );
    while( 1 )
    {
        g_fread( &item,  1, sizeof(item), reg_fp );
        if( g_feof( reg_fp ) )
        {
            //			__msg("Succeed in recovering ini file \"%s\" from backup file.\n", ini_file);
            g_fclose( ini_fp );
            g_fclose( reg_fp );
            return REG_OK;
        }
        g_fseek( reg_fp, -(int32)sizeof(item), SEEK_CUR );
        switch( item.type )
        {
        case COMMENT_ITEM :
            g_fread( &comment_item, 1, sizeof(comment_item), reg_fp );
            g_fread( line_buf, 1, comment_item.len + 1, reg_fp );
            g_strcat( line_buf, REG_LINE_END );
            g_fwrite( line_buf, 1, g_strlen(line_buf), ini_fp );
            break;
        case BLANK_ITEM :
            g_fwrite( REG_LINE_END, g_strlen(REG_LINE_END), 1, ini_fp );
            g_fseek( reg_fp, sizeof(blank_item_t), SEEK_CUR );
            break;
        case SET_ITEM :
            goto goon;
        case SEQ_END :
            goto goon;
        default :
            __wrn("FATAL ERROR ! in line %u.\n", __LINE__ );
            g_fclose( ini_fp );
            g_fclose( reg_fp );
            return REG_ERR;
        }
    }

goon :
    /* scan each set */
    for( set_pos = ffh.first_set_pos;  set_pos != NULL_POINTER; set_pos = set_item.next_set_pos )
    {
        char set_line[ SET_NAME_LEN + 1 + 2 + sizeof(REG_LINE_END) - 1 ];

        g_fseek( reg_fp, set_pos, SEEK_SET );
        g_fread( &set_item, 1, sizeof(set_item), reg_fp );
        if( set_item.valid_status == INVALID_STATUS )
            continue;
        compose_set_line( set_line, set_item.name );
        g_fwrite( set_line, g_strlen( set_line ), 1,  ini_fp );

        append_pos = set_item.append_pos;
        /* scan sequential information of set */
        if( set_item.seq_status == SEQ )
        {
            g_fseek( reg_fp, ( set_item.pos + sizeof(set_item_t) ), SEEK_SET );
            while( 1 )
            {
                g_fread( &item, 1, sizeof(item), reg_fp );
                if( g_feof( reg_fp ) )
                {
                    //					__msg("Succeed in recovering ini file \"%s\" from backup file.\n", ini_file);
                    g_fclose( ini_fp );
                    g_fclose( reg_fp );
                    return REG_OK;
                }
                g_fseek( reg_fp, -(int32)sizeof(item), SEEK_CUR );
                switch( item.type )
                {
                case COMMENT_ITEM :
                    g_fread( &comment_item, 1, sizeof(comment_item), reg_fp );
                    g_fread( line_buf, 1, comment_item.len + 1, reg_fp );
                    g_strcat( line_buf, REG_LINE_END );
                    g_fwrite( line_buf, 1, g_strlen(line_buf), ini_fp );
                    break;
                case BLANK_ITEM :
                    g_fwrite( REG_LINE_END, g_strlen(REG_LINE_END), 1, ini_fp );
                    g_fseek( reg_fp, sizeof(blank_item_t), SEEK_CUR );
                    break;
                case SET_ITEM :
                    goto goon2;
                case SEQ_END :
                    goto goon2;
                case KEY_ITEM :
                    g_fread( &key_item, 1, sizeof(key_item), reg_fp );
                    if( key_item.set_id != set_item.set_id )
                        goto goon2;
                    if( key_item.valid_status == VALID_STATUS )
                    {
                        if( key_item.value_pos == 0 )
                        {
                            g_fread( key_value, 1, key_item.value_len + 1, reg_fp );
                            g_fseek( reg_fp, key_item.follow_size - key_item.value_len - 1, SEEK_CUR );
                        }
                        else
                        {
                            g_fseek( reg_fp, key_item.value_pos, SEEK_SET );
                            g_fread( key_value, 1, key_item.value_len + 1, reg_fp );
                            g_fseek( reg_fp, key_item.pos + sizeof(key_item_t) + key_item.follow_size, SEEK_SET );
                        }
                        compose_key_line( key_line, key_item.name, key_value );
                        g_fwrite( key_line, g_strlen(key_line), 1, ini_fp );
                    }
                    else
                    {
                        g_fseek( reg_fp, key_item.follow_size, SEEK_CUR );
                    }
                    append_pos = key_item.next_key_pos;
                    break;
                default :
                    __wrn("FATAL ERROR ! in line %u.\n", __LINE__ );
                    g_fclose( ini_fp );
                    g_fclose( reg_fp );
                    return REG_ERR;
                }
            }
        }

goon2 :
        /* scan appended keys of set */
        if( append_pos != NULL_POINTER )
        {
            uint32 next_key_pos = append_pos;
            while( next_key_pos != NULL_POINTER )
            {
                g_fseek( reg_fp, next_key_pos, SEEK_SET );
                g_fread( &key_item, 1, sizeof(key_item), reg_fp );
                if( key_item.valid_status == VALID_STATUS )
                {
                    if( key_item.value_pos == 0 )
                    {
                        g_fread( key_value, 1, key_item.value_len + 1, reg_fp );
                    }
                    else
                    {
                        g_fseek( reg_fp, key_item.value_pos, SEEK_SET );
                        g_fread( key_value, 1, key_item.value_len + 1, reg_fp );
                    }
                    compose_key_line( key_line, key_item.name, key_value );
                    g_fwrite( key_line, g_strlen(key_line), 1, ini_fp );
                }
                next_key_pos = key_item.next_key_pos;
            }
        }
    }

    __msg("Succeed in recovering ini file \"%s\" from backup file.\n", ini_file);
    g_fclose( ini_fp );
    g_fclose( reg_fp );
    return REG_OK;
}





/***************************************************************************************************
*Name        : reg_file_save_as
*Prototype   : int32 reg_file_save_as( G_FILE  *src_fp, G_FILE  *dst_fp )
*Arguments   : src_fp         pointer to source register file
*              dst_fp         pointer to destination register file
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : transfer the valid content of source register file to destination register file. The
*              invalid information of the source file mustn't be transferred to destination file
*              during the process.
*Other       :
***************************************************************************************************/
int32 reg_file_save_as( G_FILE  *src_fp, G_FILE  *dst_fp )
{
    uint32 set_pos;
    uint32 append_pos;
    int32  key_exist_status;
    blank_item_t    blank_item;
    comment_item_t  comment_item;
    key_item_t  key_item;
    set_item_t  set_item;
    set_item_t  new_set_item;
    item_t      item;
    uint32 cur_key_pos;
    uint32 pre_node;
    uint32 cur_node;
    uint32 total_size;
    uint32 set_nr;
    uint32 set_id_base;
    uint32 first_set_pos;
    reg_file_head_t  ffh;
    uint32 pre_node_m_next_pos;
    char   key_value[ KEY_VALUE_LEN + 1 ];
    char   line_buf[ LINE_MAX_LEN + 2 ];


    /* initialisze register file head */
    g_fseek( src_fp, 0, SEEK_SET );
    g_fread( &ffh, 1, sizeof(ffh), src_fp );

    ffh.valid_status = INVALID_STATUS;
    ffh.version = REG_FILE_VERSION;
    ffh.set_nr = 0;
    ffh.first_set_pos = NULL_POINTER;
    ffh.total_size = 0;
    ffh.set_id_base = 0;
    g_fseek( dst_fp, 0, SEEK_SET );
    g_fwrite( &ffh, 1, sizeof(ffh), dst_fp );

    /* initialisze hash table */
    init_hash_tbl( dst_fp );

    total_size = RFO_M_CONTENT_OFFSET;

    cur_key_pos = NULL_POINTER;
    set_nr = 0;
    set_id_base = 0;
    first_set_pos = NULL_POINTER;

    /* scan the comment line and blank line before the first set */
    g_fseek( src_fp, RFO_M_CONTENT_OFFSET, SEEK_SET );
    while( 1 )
    {
        g_fread( &item,  1, sizeof(item), src_fp );
        if( g_feof( src_fp ) )
        {
            //			__msg("Succeed in saving as another register file.\n");
            goto modify_file_head;
        }
        g_fseek( src_fp, -(int32)sizeof(item), SEEK_CUR );
        switch( item.type )
        {
        case COMMENT_ITEM :
            g_fread( &comment_item, 1, sizeof(comment_item), src_fp );
            g_fwrite( &comment_item, 1, sizeof(comment_item), dst_fp );
            g_fread( line_buf, 1, comment_item.len + 1, src_fp );
            g_fwrite( line_buf, 1, comment_item.len + 1, dst_fp );
            total_size += ( sizeof(comment_item_t) + comment_item.len + 1 );
            break;
        case BLANK_ITEM :
            g_fread( &blank_item, 1, sizeof(blank_item), src_fp );
            g_fwrite( &blank_item, 1, sizeof(blank_item), dst_fp );
            total_size += sizeof(blank_item_t);
            break;
        case SET_ITEM :
            goto goon;
        case SEQ_END :
            goto goon;
        default :
            __wrn("FATAL ERROR ! in line %u.\n", __LINE__ );
            return REG_ERR;
        }
    }

goon :

    /* scan each set */
    g_fseek( src_fp, RFH_M_FIRST_SET_POS_OFFSET, SEEK_SET );
    g_fread( &set_pos, 1, sizeof(set_pos), src_fp );
    for( new_set_item.pos = NULL_POINTER;  set_pos != NULL_POINTER; set_pos = set_item.next_set_pos  )
    {
        g_fseek( src_fp, set_pos, SEEK_SET );
        g_fread( &set_item, 1, sizeof(set_item), src_fp );
        if( set_item.valid_status == INVALID_STATUS )
        {
            continue;
        }

        if( first_set_pos == NULL_POINTER )
            first_set_pos = set_pos;
        else
        {
            new_set_item.next_set_pos = set_pos;
            g_fseek( dst_fp, new_set_item.pos, SEEK_SET );
            g_fwrite( &new_set_item, 1, sizeof(new_set_item), dst_fp );
            g_fseek( dst_fp, total_size, SEEK_SET );
        }

        new_set_item.type          = SET_ITEM;
        new_set_item.valid_status  = VALID_STATUS;
        new_set_item.append_pos    = NULL_POINTER;
        new_set_item.seq_status    = NOT_SEQ;
        new_set_item.next_set_pos  = NULL_POINTER;
        new_set_item.set_id        = set_id_base++;
        new_set_item.key_nr        = 0;
        new_set_item.set_size      = sizeof(set_item_t);
        new_set_item.pos           = total_size;
        new_set_item.first_key_pos = NULL_POINTER;
        new_set_item.last_key_pos  = NULL_POINTER;
        g_strcpy( new_set_item.name, set_item.name );

        g_fwrite( &new_set_item, sizeof(set_item_t), 1, dst_fp );
        total_size += sizeof(set_item_t);
        set_nr++;

        append_pos = set_item.append_pos;
        /* scan sequential information of set */
        if( set_item.seq_status == SEQ )
        {
            while( 1 )
            {
                g_fread( &item, 1, sizeof(item), src_fp );
                if( g_feof( src_fp ) )
                {
                    //					__msg("Succeed in saving as another register file.\n");
                    goto modify_last_set_item;
                }
                g_fseek( src_fp, -(int32)sizeof(item), SEEK_CUR );
                switch( item.type )
                {
                case COMMENT_ITEM :
                    g_fread( &comment_item, 1, sizeof(comment_item), src_fp );
                    g_fwrite( &comment_item, 1, sizeof(comment_item), dst_fp );
                    g_fread( line_buf, 1, comment_item.len + 1, src_fp );
                    g_fwrite( line_buf, 1, comment_item.len + 1, dst_fp );
                    total_size += ( sizeof(comment_item_t) + comment_item.len + 1 );
                    if( new_set_item.pos != NULL_POINTER )
                    {
                        new_set_item.seq_status = SEQ;
                        new_set_item.set_size   += ( sizeof(comment_item_t) + comment_item.len );
                    }
                    break;
                case BLANK_ITEM :
                    g_fread( &blank_item, 1, sizeof(blank_item), src_fp );
                    g_fwrite( &blank_item, 1, sizeof(blank_item), dst_fp );
                    total_size += sizeof(blank_item_t);
                    if( new_set_item.pos != NULL_POINTER )
                    {
                        new_set_item.seq_status = SEQ;
                        new_set_item.set_size   += sizeof(blank_item_t);
                    }
                    break;
                case SET_ITEM :
                    goto goon2;
                case SEQ_END :
                    goto goon2;
                case KEY_ITEM :
                    g_fread( &key_item, 1, sizeof(key_item), src_fp );
                    if( key_item.set_id != set_item.set_id )
                        goto goon2;
                    append_pos = key_item.next_key_pos;
                    if( key_item.valid_status == VALID_STATUS )
                    {
                        key_exist_status = search_key_in_hash( dst_fp, new_set_item.set_id, key_item.name, &pre_node, &cur_node );
                        if( key_exist_status == FIND_ITEM_PRE_IS_HEAD_NODE  ||  key_exist_status == FIND_ITEM_PRE_IN_CHAIN )
                        {
                            __wrn("FATAL ERROR ! in line %u.\n", __LINE__ );
                            return REG_ERR;
                        }
                        /* add the key to hash chain */
                        if( key_exist_status == NOT_FIND_ITEM_PRE_IS_HEAD_NODE )   // current node's previous node is in hash address table
                            pre_node_m_next_pos = RFO_M_HASH_TBL_OFFSET + sizeof(uint32) * pre_node;
                        else                                                      // current node's previous node is in chain
                            pre_node_m_next_pos = pre_node + KI_M_HASH_NEXT_OFFSET;
                        g_fseek( dst_fp, pre_node_m_next_pos, SEEK_SET );
                        g_fwrite( &(total_size), sizeof(total_size), 1, dst_fp );
                        key_item.hash_next = cur_node;
                        /* add the key to sequence chain */
                        if( new_set_item.first_key_pos == NULL_POINTER )
                        {
                            cur_key_pos = new_set_item.first_key_pos = total_size;
                        }
                        else
                        {
                            g_fseek( dst_fp, cur_key_pos + KI_M_NEXT_KEY_POS_OFFSET, SEEK_SET );
                            cur_key_pos = total_size;
                            g_fwrite( &cur_key_pos, sizeof(cur_key_pos), 1, dst_fp );
                        }
                        /* get the key value */
                        if( key_item.value_pos == 0 )
                        {
                            g_fread( key_value, 1, key_item.value_len + 1, src_fp );
                            g_fseek( src_fp, key_item.follow_size - key_item.value_len - 1, SEEK_CUR );
                        }
                        else
                        {
                            g_fseek( src_fp, key_item.value_pos, SEEK_SET );
                            g_fread( key_value, 1, key_item.value_len + 1, src_fp );
                            g_fseek( src_fp, key_item.pos + sizeof(key_item_t) + key_item.follow_size, SEEK_SET );
                        }
                        /* modify set information */
                        new_set_item.last_key_pos = cur_key_pos;
                        new_set_item.key_nr++;
                        new_set_item.set_size     += ( sizeof(key_item_t) + key_item.value_len + 1 );
                        new_set_item.seq_status   = SEQ;
                        /* set key information */
                        key_item.set_id = new_set_item.set_id;
                        key_item.pos = cur_key_pos;
                        key_item.next_key_pos = NULL_POINTER;
                        key_item.follow_size = key_item.value_len + 1;
                        key_item.value_pos = 0;
                        key_item.value_buf_size = key_item.value_len + 1;
                        /* write key item and value */
                        g_fseek( dst_fp, cur_key_pos, SEEK_SET );
                        g_fwrite( &key_item, 1, sizeof(key_item_t), dst_fp );
                        g_fwrite( key_value, 1, g_strlen(key_value) + 1, dst_fp );
                        total_size += ( sizeof(key_item_t) + g_strlen(key_value) + 1 );
                    }
                    else
                    {
                        g_fseek( src_fp, key_item.follow_size, SEEK_CUR );
                    }
                    break;
                default :
                    __wrn("FATAL ERROR ! in line %u.\n", __LINE__ );
                    return REG_ERR;
                }
            }
        }

goon2 :
        /* scan appended keys of set */
        if( append_pos != NULL_POINTER )
        {
            uint32 next_key_pos = append_pos;
            while( next_key_pos != NULL_POINTER )
            {
                g_fseek( src_fp, next_key_pos, SEEK_SET );
                g_fread( &key_item, 1, sizeof(key_item), src_fp );
                next_key_pos = key_item.next_key_pos;
                if( key_item.valid_status == VALID_STATUS )
                {
                    key_exist_status = search_key_in_hash( dst_fp, new_set_item.set_id, key_item.name, &pre_node, &cur_node );
                    if( key_exist_status == FIND_ITEM_PRE_IS_HEAD_NODE  ||  key_exist_status == FIND_ITEM_PRE_IN_CHAIN )
                    {
                        __wrn("FATAL ERROR ! in line %u.\n", __LINE__ );
                        return REG_ERR;
                    }
                    /* add the key to hash chain */
                    if( key_exist_status == NOT_FIND_ITEM_PRE_IS_HEAD_NODE )   // current node's previous node is in hash address table
                        pre_node_m_next_pos = RFO_M_HASH_TBL_OFFSET + sizeof(uint32) * pre_node;
                    else                                                      // current node's previous node is in chain
                        pre_node_m_next_pos = pre_node + KI_M_HASH_NEXT_OFFSET;
                    g_fseek( dst_fp, pre_node_m_next_pos, SEEK_SET );
                    g_fwrite( &(total_size), sizeof(total_size), 1, dst_fp );
                    key_item.hash_next = cur_node;
                    /* add the key to sequence chain */
                    if( new_set_item.first_key_pos == NULL_POINTER )
                    {
                        cur_key_pos = new_set_item.first_key_pos = total_size;
                    }
                    else
                    {
                        g_fseek( dst_fp, cur_key_pos + KI_M_NEXT_KEY_POS_OFFSET, SEEK_SET );
                        cur_key_pos = total_size;
                        g_fwrite( &cur_key_pos, sizeof(cur_key_pos), 1, dst_fp );
                    }
                    /* get the key value */
                    if( key_item.value_pos == 0 )
                    {
                        g_fread( key_value, 1, key_item.value_len + 1, src_fp );
                    }
                    else
                    {
                        g_fseek( src_fp, key_item.value_pos, SEEK_SET );
                        g_fread( key_value, 1, key_item.value_len + 1, src_fp );
                    }
                    /* modify set information */
                    new_set_item.last_key_pos = cur_key_pos;
                    new_set_item.key_nr++;
                    new_set_item.set_size     += ( sizeof(key_item_t) + key_item.value_len + 1 );
                    new_set_item.seq_status   = SEQ;
                    /* set key information */
                    key_item.set_id = new_set_item.set_id;
                    key_item.pos = cur_key_pos;
                    key_item.next_key_pos = NULL_POINTER;
                    key_item.follow_size = key_item.value_len + 1;
                    key_item.value_pos = 0;
                    key_item.value_buf_size = key_item.value_len + 1;
                    /* write key item and value */
                    g_fseek( dst_fp, cur_key_pos, SEEK_SET );
                    g_fwrite( &key_item, 1, sizeof(key_item_t), dst_fp );
                    g_fwrite( key_value, 1, g_strlen(key_value) + 1, dst_fp );
                    total_size += ( sizeof(key_item_t) + g_strlen(key_value) + 1 );
                }
            }
        }
    }
modify_last_set_item:
    if( new_set_item.pos != NULL_POINTER )
    {
        g_fseek( dst_fp, new_set_item.pos, SEEK_SET );
        g_fwrite( &new_set_item, 1, sizeof(new_set_item), dst_fp );
    }

modify_file_head:
    ffh.valid_status = VALID_STATUS;
    ffh.set_nr = set_nr;
    ffh.total_size = total_size;
    ffh.set_id_base = set_id_base;
    ffh.invalid_space_size = 0;
    ffh.first_set_pos = first_set_pos;
    g_fseek( dst_fp, 0, SEEK_SET );
    g_fwrite( &ffh, 1, sizeof(ffh), dst_fp );

    //	__msg("Succeed in recovering ini file \"%s\" from backup file.\n",ini_file);
    return REG_OK;
}





static char *gen_temp_reg_file( const char *reg_file, char *tmp_file )
{
    int32 i;

    g_strcpy( tmp_file, reg_file );
    for( i = g_strlen( tmp_file ) - 1; i >= 0; i-- )
    {
        if( tmp_file[i] == '\\' )
            break;
    }
    if( i < 0 )                                             // not find '\'
    {
        return NULL;
    }
    tmp_file[i + 1] = '\0';
    /* append temp file name */
    g_strcat( tmp_file, TEMP_REG_FILE_NAME );
    return tmp_file;
}





static char *gen_node_name( const char *reg_file, char *node )
{
    g_strcpy( node, reg_file + g_strlen(reg_root_path) );
    node[g_strlen(node) - g_strlen(REG_FILE_EXTENTION)] = '\0';

    return node;
}





/***************************************************************************************************
*Name        : __refresh_reg_file
*Prototype   : int32 __refresh_reg_file( const char *reg_file )
*Arguments   : reg_file       full name of register file
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : refresh register file, delete invalid spaces of register file.
*Other       : a temp file is need in the process.
***************************************************************************************************/
int32 __refresh_reg_file( const char *reg_file )
{
    G_FILE *reg_fp;
    G_FILE *tmp_fp;
    char   tmp_file[255];
    uint8  buf[512];
    HNODE_i   p;
    reg_file_head_t ffh;
    uint32 count;
    char   *node;
    char   up_str1[8];
    char   up_str2[8];
    uint8  mutex_err;

    str_to_upper( reg_file + g_strlen(reg_file) - 4, up_str1 );
    str_to_upper( REG_FILE_EXTENTION, up_str2 );
    if( g_strcmp( up_str1, up_str2 ) != 0 )                   // invalid extention name
        return 0;

    /* lock global chain */
    g_pend_mutex( global_chain_mutex, &mutex_err );
    if( mutex_err != EVENT_OK )
    {
        __wrn("Error in refreshing register file. Because fail in applying mutex.\n");
        return REG_ERR;
    }

    /* up to maximum open number */
    if( HNODE_head.count >= OPEN_NODE_MAX_NR )
    {
        __wrn("Fail in refreshing register file. Because up to maximum open number %d.\n");
        g_post_mutex( global_chain_mutex );
        return REG_ERR;
    }

    /* search node */
    node = (char *)g_malloc( g_strlen(reg_file) - g_strlen(reg_root_path)  + 1 );
    if( node == NULL )
    {
        __wrn("Error in refreshing register file. Because allocating memory fail.\n");
        g_post_mutex( global_chain_mutex );
        return REG_ERR;
    }
    gen_node_name( reg_file, node );
    for( p = HNODE_head.next_HNODE; p != NULL;  p = p->next_HNODE )
    {
        if( g_strcmp( p->node, node ) == 0 )
            break;
    }
    if( p != NULL )
    {
        __msg("current register file is open and can't be refreshed.\n");
        g_post_mutex( global_chain_mutex );
        return REG_ERR;
    }

    /* create a new HNODE_i */
    p = (HNODE_i) g_malloc( sizeof(NODE_i_t) );
    if( p == NULL )
    {
        __wrn("Error in refreshing register file. Because allocating memory fail.\n");
        g_post_mutex( global_chain_mutex );
        return REG_ERR;
    }
    g_memset( p, 0, sizeof(NODE_i_t) );
    p->node = node;
    p->mode = REG_READWRITE;

    /* add the node to the node list */
    add_node( p );
    /* unlock */
    g_post_mutex( global_chain_mutex );

    /* open register file */
    reg_fp = g_fopen( reg_file, "rb" );
    if( reg_fp == NULL )
    {
        __wrn("Fail in refreshing register file. Because can't open register file.\n");
        goto error;
    }
    g_fread( &ffh, 1, sizeof(ffh), reg_fp );
    if( ffh.valid_status != VALID_STATUS  ||  ffh.invalid_space_size < INVALID_SPACE_THRESHOLD )
    {
        __msg("current register file %s doesn't need refreshed.\n", reg_file);
        g_fclose( reg_fp );
        goto ok;
    }

    /* create temp file */
    if( gen_temp_reg_file( reg_file, tmp_file )  ==  NULL )
    {
        __wrn("Fail in refresh register file. Because can't get temp file name.\n");
        g_fclose( reg_fp );
        goto ok;
    }
    tmp_fp = g_fopen( tmp_file, "wb+" );
    if( tmp_fp == NULL )
    {
        __wrn("Fail in refresh register file. Because can't create temp file.\n");
        g_fclose( reg_fp );
        goto error;
    }

    /* save register file as */
    if( reg_file_save_as( reg_fp, tmp_fp ) != REG_OK )
    {
        __wrn("Fail in refresh register file.\n");
        g_fclose( tmp_fp );
        g_fclose( reg_fp );
        goto error;
    }

    /* clear register file */
    g_fclose( reg_fp );
    reg_fp = g_fopen( reg_file, "wb+" );
    if( reg_fp == NULL )
    {
        __wrn("Fail in refreshing register file. Because can't clear register file.\n");
        g_fclose( tmp_fp );
        goto error;
    }

    /* copy content of temp file to register file */
    g_fseek( tmp_fp, 0, SEEK_SET );
    g_fread( &ffh, 1, sizeof(ffh), tmp_fp );
    ffh.valid_status = INVALID_STATUS;
    g_fwrite( &ffh, 1, sizeof(ffh), reg_fp );
    while( !g_feof( tmp_fp ) )
    {
        count = g_fread( buf, 1, sizeof(buf), tmp_fp );
        g_fwrite( buf, 1, count, reg_fp );
    }
    ffh.valid_status = VALID_STATUS;
    g_fseek( reg_fp, 0, SEEK_SET );
    g_fwrite( &ffh, 1, sizeof(ffh), reg_fp );

    g_fclose( tmp_fp );
    g_fclose( reg_fp );

ok:
    /* lock global chain */
    g_pend_mutex( global_chain_mutex, &mutex_err );
    if( mutex_err != EVENT_OK )
    {
        __wrn("Error in refreshing register file. Because fail in applying mutex.\n");
        g_free( node );
        return REG_ERR;
    }

    /* delete node from chain */
    delete_node( p );

    /* unlock */
    g_post_mutex( global_chain_mutex );

    g_free( node );

    __msg("Succeed in refreshing register file %s.\n", reg_file);
    return REG_OK;

error:
    /* lock global chain */
    g_pend_mutex( global_chain_mutex, &mutex_err );
    if( mutex_err != EVENT_OK )
    {
        __wrn("Error in refreshing register file. Because fail in applying mutex.\n");
        g_free( node );
        return REG_ERR;
    }

    /* delete node from chain */
    delete_node( p );

    /* unlock */
    g_post_mutex( global_chain_mutex );

    g_free( node );
    return REG_ERR;
}



static int32 refresh_one_reg_file( const char *file, void *arg )
{
    return __refresh_reg_file( file );
}


/***************************************************************************************************
*Name        : __refresh_all_reg_file
*Prototype   : int32 __refresh_all_reg_file( void )
*Arguments   : void
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : refresh all the register files
*Other       :
***************************************************************************************************/
int32 __refresh_all_reg_file( void )
{
    recursion_depth_info_t  depth_info;

    depth_info.cur_depth = 0;
    depth_info.max_depth = 8;

    if( g_traverse_folder( reg_root_path, &refresh_one_reg_file, &depth_info ) == 0 )
        return REG_OK;
    else
        return REG_ERR;
}




static char *gen_bak_file_name( const char *reg_file, char *bak_file )
{
    g_strcpy( bak_file, reg_file );
    /* delete ini file extention */
    bak_file[ g_strlen(bak_file) - g_strlen(REG_FILE_EXTENTION) ] = '\0';
    /* append backup file extention */
    g_strcat( bak_file, BACKUP_FILE_EXTENTION );
    return bak_file;
}





/***************************************************************************************************
*Name        : __backup_reg_file
*Prototype   : int32 __backup_reg_file( const char *reg_file )
*Arguments   : reg_file       full name of register file
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : copy the content of register file to the corresponding backup file.
*Other       : during the process, invalid information of register file mustn't be added to backup file.
***************************************************************************************************/
int32 __backup_reg_file( const char *reg_file )
{
    G_FILE *reg_fp;
    G_FILE *bak_fp;
    char   bak_file[255];
    HNODE_i   p;
    reg_file_head_t ffh;
    int32  modified_status;
    char   *node;
    char   up_str1[8];
    char   up_str2[8];
    uint8  mutex_err;

    str_to_upper( reg_file + g_strlen(reg_file) - 4, up_str1 );
    str_to_upper( REG_FILE_EXTENTION, up_str2 );
    if( g_strcmp( up_str1, up_str2 ) != 0 )                   // invalid extention name
        return 0;

    /* lock global chain */
    g_pend_mutex( global_chain_mutex, &mutex_err );
    if( mutex_err != EVENT_OK )
    {
        __wrn("Error in backuping register file. Because fail in applying mutex.\n");
        return REG_ERR;
    }

    /* up to maximum open number */
    if( HNODE_head.count >= OPEN_NODE_MAX_NR )
    {
        __wrn("Fail in refreshing register file. Because up to maximum open number %d.\n");
        g_post_mutex( global_chain_mutex );
        return REG_ERR;
    }

    /* search node */
    node = (char *)g_malloc( g_strlen(reg_file) - g_strlen(reg_root_path) + g_strlen(INI_FILE_EXTENTION) + 1 );
    if( node == NULL )
    {
        __wrn("Error in refreshing register file. Because allocating memory fail.\n");
        g_post_mutex( global_chain_mutex );
        return REG_ERR;
    }
    gen_node_name( reg_file, node );
    for( p = HNODE_head.next_HNODE; p != NULL;  p = p->next_HNODE )
    {
        if( g_strcmp( p->node, node ) == 0 )
            break;
    }
    if( p != NULL )
    {
        __msg("current register file is open and can't be refreshed.\n");
        g_post_mutex( global_chain_mutex );
        return REG_ERR;
    }

    /* create a new HNODE_i */
    p = (HNODE_i) g_malloc( sizeof(NODE_i_t) );
    if( p == NULL )
    {
        __wrn("Error in refreshing register file. Because allocating memory fail.\n");
        g_post_mutex( global_chain_mutex );
        return REG_ERR;
    }
    g_memset( p, 0, sizeof(NODE_i_t) );
    p->node = node;
    p->mode = REG_READWRITE;

    /* add the node to the node list */
    add_node( p );

    /* unlock */
    g_post_mutex( global_chain_mutex );

    /* open register file */
    reg_fp = g_fopen( reg_file, "rb+" );
    if( reg_fp == NULL )
    {
        __wrn("Fail in backuping register file. Because can't open register file.\n");
        goto error;
    }
    g_fread( &ffh, 1, sizeof(ffh), reg_fp );
    if( ffh.valid_status != VALID_STATUS  ||  ffh.modified_status == NOT_MODIFIED )
    {
        __msg("current register file %s doesn't need backuped.\n", reg_file);
        g_fclose( reg_fp );
        goto ok;
    }

    /* create backup file */
    if( gen_bak_file_name( reg_file, bak_file )  ==  NULL )
    {
        __wrn("Fail in backuping register file. Because can't get backup file name.\n");
        g_fclose( reg_fp );
        goto ok;
    }
    bak_fp = g_fopen( bak_file, "wb+" );
    if( bak_fp == NULL )
    {
        __wrn("Fail in backuping register file. Because can't create backup file.\n");
        g_fclose( reg_fp );
        goto error;
    }

    /* save register file as backup file*/
    if( reg_file_save_as( reg_fp, bak_fp ) != REG_OK )
    {
        __wrn("Fail in refresh register file. Because deleting invalid space fail.\n");
        g_fclose( bak_fp );
        g_fclose( reg_fp );
        goto error;
    }

    modified_status = NOT_MODIFIED;
    g_fseek( bak_fp, RFH_M_MODIFIED_STATUS_OFFSET, SEEK_SET );
    g_fwrite( &modified_status, 1, sizeof(modified_status), bak_fp );
    g_fclose( bak_fp );
    g_fseek( reg_fp, RFH_M_MODIFIED_STATUS_OFFSET, SEEK_SET );
    g_fwrite( &modified_status, 1, sizeof(modified_status), reg_fp );
    g_fclose( reg_fp );

ok:
    /* lock global chain */
    g_pend_mutex( global_chain_mutex, &mutex_err );
    if( mutex_err != EVENT_OK )
    {
        __wrn("Error in backuping register file. Because fail in applying mutex.\n");
        g_free( node );
        return REG_ERR;
    }

    /* delete node from chain */
    delete_node( p );

    /* unlock */
    g_post_mutex( global_chain_mutex );

    g_free( node );

    __msg("Succeed in backuping register file %s.\n", reg_file);
    return REG_OK;

error:
    /* lock global chain */
    g_pend_mutex( global_chain_mutex, &mutex_err );
    if( mutex_err != EVENT_OK )
    {
        __wrn("Error in backuping register file. Because fail in applying mutex.\n");
        g_free( node );
        return REG_ERR;
    }

    /* delete node from chain */
    delete_node( p );

    /* unlock */
    g_post_mutex( global_chain_mutex );

    g_free( node );
    return REG_ERR;
}




static int32 backup_one_reg_file( const char *file, void *arg )
{
    return __backup_reg_file( file );
}



/***************************************************************************************************
*Name        : __backup_all_reg_file
*Prototype   : int32 __backup_all_reg_file( void )
*Arguments   : void
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : copy the content of all the register files to the corresponding backup files.
*Other       :
***************************************************************************************************/
int32 __backup_all_reg_file( void )
{
    recursion_depth_info_t  depth_info;

    depth_info.cur_depth = 0;
    depth_info.max_depth = 8;

    if( g_traverse_folder( reg_root_path, &backup_one_reg_file, &depth_info ) == 0 )
        return REG_OK;
    else
        return REG_ERR;
}




/***************************************************************************************************
*Name        : close_node_copy
*Prototype   : void close_node_copy( HNODE_i hdl )
*Arguments   : handle         a handle of a node
*Return      : void
*Description : in REG_READONLY mode, close the not_last hdl.
*Other       :
***************************************************************************************************/
static void close_node_copy( HNODE_i hdl )
{
    set_desc_t *head;
    set_desc_t *next;
    HNODE_i  cursor;

    /* the count of node hdl is modified */
    for( cursor = HNODE_head.next_HNODE; cursor != NULL; cursor = cursor->next_HNODE )
    {
        if( g_strcmp( cursor->node, hdl->node ) == 0 )
            cursor->open_nr--;
    }

    /* delete hdl from chain */
    delete_node( hdl );

    /* free memory */
    head = hdl->first_set;
    while( head != NULL )
    {
        next = head;
        head = head->next_set;
        g_free( next );
    }

    g_free( hdl );
}





/***************************************************************************************************
*Name        : write_back_reg_file
*Prototype   : void write_back_reg_file( HNODE_i hdl )
*Arguments   : handle         a handle of a node
*Return      : void
*Description : update some information of register file before closing node.
*Other       :
***************************************************************************************************/
static void write_back_reg_file( HNODE_i hdl )
{
    reg_file_head_t  ffh;
    set_item_t   set_item;
    set_desc_t   *cur_set_p;

    /* modify file head */
    g_strncpy( ffh.magic, REG_MAGIC, MAGIC_SIZE );
    ffh.valid_status = VALID_STATUS;
    ffh.version = REG_FILE_VERSION;
    ffh.set_nr = hdl->set_nr;
    ffh.total_size = hdl->total_size;
    ffh.set_id_base = hdl->set_id_base;
    ffh.invalid_space_size = hdl->invalid_space_size;
    ffh.modified_status = hdl->modified_status;
    if( hdl->first_set != NULL )
        ffh.first_set_pos = hdl->first_set->pos;
    else
        ffh.first_set_pos = NULL_POINTER;

    g_fseek( hdl->reg_fp, 0, SEEK_SET );
    g_fwrite( &ffh, 1, sizeof(ffh), hdl->reg_fp );

    /* modify set item information */
    set_item.type = SET_ITEM;
    set_item.valid_status = VALID_STATUS;
    cur_set_p = hdl->first_set;
    while( cur_set_p != NULL )
    {
        set_item.append_pos    = cur_set_p->append_pos;
        set_item.seq_status    = cur_set_p->seq_status;
        set_item.set_id        = cur_set_p->set_id;
        set_item.key_nr        = cur_set_p->key_nr;
        set_item.set_size      = cur_set_p->set_size;
        set_item.next_set_pos  = cur_set_p->next_set_pos;
        set_item.pos           = cur_set_p->pos;
        set_item.first_key_pos = cur_set_p->first_key_pos;
        set_item.last_key_pos  = cur_set_p->last_key_pos;
        g_strcpy( set_item.name, cur_set_p->name );

        g_fseek( hdl->reg_fp, set_item.pos, SEEK_SET );
        g_fwrite( &set_item, 1, sizeof(set_item), hdl->reg_fp );

        cur_set_p = cur_set_p->next_set;
    }
}





int32 CloseNode_i( HNODE_i hdl )
{
    if( hdl == NULL )
        return REG_ERR;

    if( hdl->mode == REG_READONLY && hdl->open_nr > 1 )
    {
        close_node_copy( hdl );
        return REG_OK;
    }

    if( hdl->mode != REG_READONLY )
    {
        write_back_reg_file( hdl );
    }

    /* delete hdl from chain */
    delete_node( hdl );

    release_HNODE( hdl );

    return REG_OK;
}



/***************************************************************************************************
*Name        : __CloseNode
*Prototype   : int32 __CloseNode( HNODE handle )
*Arguments   : handle         a handle of a node
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : close node.
*Other       :
***************************************************************************************************/
int32 __CloseNode( HNODE handle )
{
    uint8  mutex_err;
    HNODE_i hdl = (HNODE_i)handle;

    if( handle_is_invalid( hdl ) )
        return REG_ERR;

    /* lock global chain */
    g_pend_mutex( global_chain_mutex, &mutex_err );
    if( mutex_err != EVENT_OK )
    {
        __wrn("Error in closing node. Because fail in applying mutex.\n");
        return REG_ERR;
    }

    if( CloseNode_i( hdl ) != REG_OK )
    {
        __wrn("Fail in Closing node \"%s\".\n", hdl->node );
        g_post_mutex( global_chain_mutex );
        return REG_ERR;
    }
    g_post_mutex( global_chain_mutex );

    __msg("Succeed in closing register node.\n");
    return REG_OK;
}





/***************************************************************************************************
*Name        : __DeleteNode
*Prototype   : int32 __DeleteNode( const char *node )
*Arguments   : node           node to be deleted
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : delete node.
*Other       :
***************************************************************************************************/
int32 __DeleteNode( const char *node )
{
    char real_file[256];

    g_strcpy( real_file, reg_root_path );
    g_strcat( real_file, node );

    /* delete backup file */
    g_strcat( real_file, BACKUP_FILE_EXTENTION );
    g_remove( real_file );

    /* cut off backup file extention */
    real_file[ g_strlen(real_file) - g_strlen(BACKUP_FILE_EXTENTION) ] = '\0';
    /* append register file extention */
    g_strcat( real_file, REG_FILE_EXTENTION );
    /* delete register file */
    if( g_remove( real_file ) != 0 )
    {
        __wrn("Fail in deleting node %s. Because fail in removing file %s. At line %u of file %s.\n", node, real_file,
              __LINE__, __FILE__);
        return REG_ERR;
    }

    __msg("Succeed in deleting node %s.\n", node);
    return REG_OK;
}





/***************************************************************************************************
*Name        : __GetSetCount
*Prototype   : int32 __GetSetCount( HNODE handle, uint32 *count_p )
*Arguments   : handle         a handle of a node
*              count_p        output. the total count of set of node.
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : get the total count of set of node.
*Other       :
***************************************************************************************************/
int32 __GetSetCount( HNODE handle, uint32 *count_p )
{
    HNODE_i hdl = (HNODE_i)handle;

    if( handle_is_invalid( hdl ) )
        return REG_ERR;

    hdl->err_no = REG_OK;
    *count_p = hdl->set_nr;
    return REG_OK;
}





/***************************************************************************************************
*Name        : __GetFirstSet
*Prototype   : int32 __GetFirstSet( HNODE handle )
*Arguments   : handle         a handle of a node
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail.
*Description : the function is used to organise set list for __GetNextSet(). NOTE: before call
*              __GetNextSet(), __GetFirstSet() must be called first.
*Other       :
***************************************************************************************************/
int32 __GetFirstSet( HNODE handle )
{
    HNODE_i hdl = (HNODE_i)handle;

    if( handle_is_invalid( hdl ) )
        return REG_ERR;

    hdl->next_set = hdl->first_set;
    hdl->err_no = REG_OK;
    return REG_OK;
}





/***************************************************************************************************
*Name        : __GetNextSet
*Prototype   : int32 __GetNextSet( HNODE handle, char *set_name )
*Arguments   : handle         a handle of a node
*              set_name       output. a buffer to store set name. The size of the buffer must NOT be
*                             less than SET_NAME_LEN+1, (the constant is defined in head file).
*Return      : REG_OK                   operation succeed
*              REG_ERR                  operation fail
*              REG_ERR_SET_LIST_END     to the end of set list
*Description : get next set name.
*Other       :
***************************************************************************************************/
int32 __GetNextSet( HNODE handle, char *set_name )
{
    HNODE_i hdl = (HNODE_i)handle;

    if( handle_is_invalid( hdl ) )
        return REG_ERR;

    if( hdl->next_set == NULL )
    {
        __msg("NO more set.\n");
        hdl->err_no = REG_ERR_SET_LIST_END;
        return REG_ERR_SET_LIST_END;
    }
    g_strcpy( set_name, hdl->next_set->name );
    hdl->next_set = hdl->next_set->next_set;
    hdl->err_no = REG_OK;
    return REG_OK;
}





/***************************************************************************************************
*Name        : __GetKeyCount
*Prototype   : int32 __GetKeyCount( HNODE handle, const char *set_name, uint32 *count_p )
*Arguments   : handle         a handle of a node
*              set_name       set name
*              count_p        output. the total key count of the specified set
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description :
*Other       :
***************************************************************************************************/
int32 __GetKeyCount( HNODE handle, const char *set_name, uint32 *count_p )
{
    set_desc_t *p;
    HNODE_i hdl = (HNODE_i)handle;

    if( handle_is_invalid( hdl ) )
        return REG_ERR;

    if( !is_valid_set_name( set_name ) )
    {
        __wrn("Fail in getting key count of set \"%s\". Because set name is illegal.\n", set_name );
        hdl->err_no = REG_ERR_ILLEGAL_SET_NAME;
        return REG_ERR;
    }

    p = search_set( hdl, set_name );
    if( p == NULL )
    {
        __wrn("Fail in getting key count of set \"%s\". Because set doesn't exist.\n", set_name );
        hdl->err_no = REG_ERR_INVALID_SET_NAME;
        return REG_ERR;
    }
    else
    {
        //		__msg("Succeed in getting key count of set \"%s\".\n", set_name );
        *count_p = p->key_nr;
        hdl->err_no = REG_OK;
        return REG_OK;
    }
}





/***************************************************************************************************
*Name        : __GetFirstKey
*Prototype   : int32 __GetFirstKey( HNODE handle, const char *set_name )
*Arguments   : handle         a handle of node
*              set_name       set name
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail.
*Description : the function is used to organise key list for __GetNextKey(). NOTE: before call
*              __GetNextKey(), __GetFirstKey() must be called first.
*Other       :
***************************************************************************************************/
int32 __GetFirstKey( HNODE handle, const char *set_name )
{
    set_desc_t *p;
    HNODE_i hdl = (HNODE_i)handle;

    if( handle_is_invalid( hdl ) )
        return REG_ERR;

    if( !is_valid_set_name( set_name ) )
    {
        __wrn("Fail in getting first key of set \"%s\". Because set name is illegal.\n", set_name );
        hdl->err_no = REG_ERR_ILLEGAL_SET_NAME;
        return REG_ERR;
    }

    /* search set */
    p = search_set( hdl, set_name );
    if( p == NULL )
    {
        __wrn("Fail in getting first key of set \"%s\". Because set doesn't exist.\n", set_name );
        hdl->err_no = REG_ERR_INVALID_SET_NAME;
        return REG_ERR;
    }

    /* search the fisrt key position */
    p->next_key_pos = p->first_key_pos;
    hdl->err_no = REG_OK;
    return REG_OK;
}





/***************************************************************************************************
*Name        : __GetNextKey
*Prototype   : int32 __GetNextKey( HNODE handle, const char *set_name, char *key_name )
*Arguments   : handle         a handle of node
*              set_name       set name
*              key_name       output. a buffer to store key name. The size of the buffer must NOT be
*                             less than KEY_NAME_LEN+1, (the constant is defined in head file).
*Return      : REG_OK                   operation succeed
*              REG_ERR                  operation fail
*              REG_ERR_KEY_LIST_END     to the end of key list
*Description : get next key name of specified set.
*Other       :
***************************************************************************************************/
int32 __GetNextKey( HNODE handle, const char *set_name, char *key_name )
{
    set_desc_t *p;
    key_item_t key_item;
    uint32     next_key_pos;
    HNODE_i hdl = (HNODE_i)handle;

    if( handle_is_invalid( hdl ) )
        return REG_ERR;

    if( !is_valid_set_name( set_name ) )
    {
        __wrn("Fail in getting next key of set \"%s\". Because set name is illegal.\n", set_name );
        hdl->err_no = REG_ERR_ILLEGAL_SET_NAME;
        return REG_ERR;
    }

    /* search set */
    p = search_set( hdl, set_name );
    if( p == NULL )
    {
        __wrn("Fail in getting next key of set \"%s\". Because set doesn't exist.\n", set_name );
        hdl->err_no = REG_ERR_INVALID_SET_NAME;
        return REG_ERR;
    }

    /* search a VALID_STATUS key item */
    next_key_pos = p->next_key_pos;
    while( next_key_pos != NULL_POINTER )
    {
        g_fseek( hdl->reg_fp, next_key_pos, SEEK_SET );
        g_fread( &key_item, 1, sizeof(key_item), hdl->reg_fp );
        if( key_item.valid_status == VALID_STATUS )
        {
            //			__msg("Succeed in getting first key of set \"%s\".\n", set_name);
            g_strcpy( key_name, key_item.name );
            p->next_key_pos = key_item.next_key_pos;
            hdl->err_no = REG_OK;
            return REG_OK;
        }
        next_key_pos = key_item.next_key_pos;
    }
    __msg("NO more key in set \"%s\".\n", set_name);
    hdl->err_no = REG_ERR_KEY_LIST_END;
    return REG_ERR_KEY_LIST_END;
}




/***************************************************************************************************
*Name        : __CreateKey
*Prototype   : int32 __CreateKey( HNODE handle, const char *set_name, const char *key_name,
*                                 const char *key_value )
*Arguments   : handle         a handle of a node
*              set_name       set name
*              key_name       key name
*              key_value      key value
*Return      : REG_OK              operation succeed
*              REG_ERR_KEY_EXIST   key already existed
*              REG_ERR             operation fail
*Description : set the value of specified key.
*Other       :
***************************************************************************************************/
int32 __CreateKey( HNODE handle, const char *set_name, const char *key_name, const char *key_value )
{
    uint32 pre_node;
    uint32 cur_node;
    int32  status;
    set_desc_t *p;
    key_item_t key_item;
    G_FILE  *fp;
    uint32 pre_node_m_next_pos;
    uint32 len;
    HNODE_i hdl = (HNODE_i)handle;

    if( handle_is_invalid( hdl ) )
        return REG_ERR;

    if( hdl->mode == REG_READONLY )
    {
        __wrn("Fail in creating key\"%s\" of set \"%s\" as value \"%s\". Because the operation is forbidden in READONLY mode.\n", key_name, set_name, key_value );
        hdl->err_no = REG_ERR_ILLEGAL_OP;
        return REG_ERR;
    }

    if( !is_valid_set_name( set_name ) )
    {
        __wrn("Fail in creating key\"%s\" of set \"%s\" as value \"%s\". Because set name is illegal.\n", key_name, set_name, key_value );
        hdl->err_no = REG_ERR_ILLEGAL_SET_NAME;
        return REG_ERR;
    }

    if( !is_valid_key_name( key_name ) )
    {
        __wrn("Fail in creating key\"%s\" of set \"%s\" as value \"%s\". Because key name is illegal.\n", key_name, set_name, key_value );
        hdl->err_no = REG_ERR_ILLEGAL_KEY_NAME;
        return REG_ERR;
    }

    if( !is_valid_key_value( key_value ) )
    {
        __wrn("Fail in creating key\"%s\" of set \"%s\" as value \"%s\". Because key value is illegal.\n", key_name, set_name, key_value );
        hdl->err_no = REG_ERR_ILLEGAL_KEY_VALUE;
        return REG_ERR;
    }

    fp = hdl->reg_fp;
    /* search set */
    p = search_set( hdl, set_name );
    if( p == NULL )
    {
        __wrn("Fail in creating key\"%s\" of set \"%s\" as value \"%s\". Because set doesn't exist.\n", key_name, set_name, key_value );
        hdl->err_no = REG_ERR_INVALID_SET_NAME;
        return REG_ERR;
    }

    len = g_strlen( key_value );
    /* search key in hash chain */
    status = search_key_in_hash( fp, p->set_id, key_name, &pre_node, &cur_node );
    if(     status == FIND_ITEM_PRE_IS_HEAD_NODE
            ||  status == FIND_ITEM_PRE_IN_CHAIN )              // the key has existed
    {
        __msg("The key %s has existed.\n", key_name);
        hdl->err_no = REG_ERR_KEY_EXIST;
        return REG_ERR_KEY_EXIST;
    }

    /* the key doesn't exist */

    /* add the key to hash chain */
    if( status == NOT_FIND_ITEM_PRE_IS_HEAD_NODE )      // current node's previous node is in hash address table
        pre_node_m_next_pos = RFO_M_HASH_TBL_OFFSET + sizeof(uint32) * pre_node;
    else                                                // current node's previous node is in chain
        pre_node_m_next_pos = pre_node + KI_M_HASH_NEXT_OFFSET;
    g_fseek( fp, pre_node_m_next_pos, SEEK_SET );
    g_fwrite( &(hdl->total_size), sizeof(hdl->total_size), 1, fp );
    key_item.hash_next = cur_node;
    /* add the key to sequence chain */
    if( p->first_key_pos == NULL_POINTER )
    {
        p->first_key_pos = hdl->total_size;
        p->append_pos = hdl->total_size;
    }
    else
    {
        g_fseek( fp, p->last_key_pos + KI_M_NEXT_KEY_POS_OFFSET, SEEK_SET );
        g_fwrite( &(hdl->total_size), sizeof(hdl->total_size), 1, fp );
    }
    /* modify set information */
    p->last_key_pos = hdl->total_size;
    p->key_nr++;
    p->set_size += ( sizeof(key_item_t) + len + 1 );
    /* set key information */
    key_item.type = KEY_ITEM;
    key_item.valid_status = VALID_STATUS;
    key_item.set_id = p->set_id;
    key_item.pos = hdl->total_size;
    key_item.next_key_pos = NULL_POINTER;
    key_item.follow_size = len + 1;
    key_item.value_pos = 0;
    key_item.value_len = len;
    key_item.value_buf_size = len + 1;
    g_strcpy( key_item.name, key_name );
    /* write key item and value */
    g_fseek( fp, hdl->total_size, SEEK_SET );
    g_fwrite( &key_item, sizeof(key_item_t), 1, fp );
    g_fwrite( key_value, 1, len + 1, fp );
    /* modify hdl information */
    hdl->total_size += ( sizeof(key_item_t) + len + 1 );
    hdl->modified_status = MODIFIED;

    __msg("Succeed in creating key\"%s\" of set \"%s\" as value \"%s\".\n", key_name, set_name, key_value );
    hdl->err_no = REG_OK;
    return REG_OK;
}





/***************************************************************************************************
*Name        : __SetKeyValue
*Prototype   : int32 __SetKeyValue( HNODE handle, const char *set_name, const char *key_name,
*                                   const char *key_value )
*Arguments   : handle         a handle of a node
*              set_name       set name
*              key_name       key name
*              key_value      key value
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : set the value of specified key.
*Other       :
***************************************************************************************************/
int32 __SetKeyValue( HNODE handle, const char *set_name, const char *key_name, const char *key_value )
{
    uint32 pre_node;
    uint32 cur_node;
    int32  status;
    set_desc_t *p;
    key_item_t key_item;
    G_FILE  *fp;
    uint32 len;
    HNODE_i hdl = (HNODE_i)handle;

    if( handle_is_invalid( hdl ) )
        return REG_ERR;

    if( hdl->mode == REG_READONLY )
    {
        __wrn("Fail in setting key \"%s\" of set \"%s\" as value \"%s\". Because the operation is forbidden in READONLY mode.\n", key_name, set_name, key_value );
        hdl->err_no = REG_ERR_ILLEGAL_OP;
        return REG_ERR;
    }

    if( !is_valid_set_name( set_name ) )
    {
        __wrn("Fail in setting key \"%s\" of set \"%s\" as value \"%s\". Because set name is illegal.\n", key_name, set_name, key_value );
        hdl->err_no = REG_ERR_ILLEGAL_SET_NAME;
        return REG_ERR;
    }

    if( !is_valid_key_name( key_name ) )
    {
        __wrn("Fail in setting key \"%s\" of set \"%s\" as value \"%s\". Because key name is illegal.\n", key_name, set_name, key_value );
        hdl->err_no = REG_ERR_ILLEGAL_KEY_NAME;
        return REG_ERR;
    }

    if( !is_valid_key_value( key_value ) )
    {
        __wrn("Fail in setting key \"%s\" of set \"%s\" as value \"%s\". Because key value is illegal.\n", key_name, set_name, key_value );
        hdl->err_no = REG_ERR_ILLEGAL_KEY_VALUE;
        return REG_ERR;
    }

    fp = hdl->reg_fp;
    /* search set */
    p = search_set( hdl, set_name );
    if( p == NULL )
    {
        __wrn("Fail in setting key \"%s\" of set \"%s\" as value \"%s\". Because set doesn't exist.\n", key_name, set_name, key_value );
        hdl->err_no = REG_ERR_INVALID_SET_NAME;
        return REG_ERR;
    }

    len = g_strlen( key_value );
    /* search key in hash chain */
    status = search_key_in_hash( fp, p->set_id, key_name, &pre_node, &cur_node );
    if(     status == FIND_ITEM_PRE_IS_HEAD_NODE
            ||  status == FIND_ITEM_PRE_IN_CHAIN )              // the key has existed
    {
        g_fseek( fp, cur_node, SEEK_SET );
        g_fread( &key_item, 1, sizeof(key_item), fp );
        if( key_item.value_buf_size >= len + 1 )
        {
            /* set key value */
            if( key_item.value_pos != 0 )
                g_fseek( fp, key_item.value_pos, SEEK_SET );
            //			else
            //				g_fseek( fp, 0, SEEK_CUR );
            g_fwrite( key_value, 1, len + 1, fp );

            /* modify value length */
            if( key_item.value_len != len )
            {
                key_item.value_len = len;
                g_fseek( fp, cur_node + KI_M_VALUE_LEN_OFFSET, SEEK_SET );
                g_fwrite( &(key_item.value_len), 1, sizeof(uint32), fp );
            }
        }
        else
        {
            uint32 del_value_buf;

            del_value_buf = key_item.value_buf_size;
            /* modify key item */
            key_item.value_pos = hdl->total_size;
            key_item.value_len = len;
            key_item.value_buf_size = len + 1;
            g_fseek( fp, -(int32)sizeof(key_item_t), SEEK_CUR );
            g_fwrite( &key_item, 1, sizeof(key_item), fp );

            /* add new key value at end of file */
            g_fseek( fp, hdl->total_size, SEEK_SET );
            g_fwrite( key_value, 1, len + 1, fp );

            p->set_size += ( len - del_value_buf );
            hdl->invalid_space_size += del_value_buf;
            hdl->total_size += (len + 1);
        }

        hdl->modified_status = MODIFIED;
        __msg("Succeed in setting key \"%s\" of set \"%s\" as value \"%s\". the key has existed.\n", key_name, set_name, key_value );
        hdl->err_no = REG_OK;
        return REG_OK;
    }
    else
    {
        __wrn("Fail in setting key \"%s\" of set \"%s\" as value \"%s\". Because key doesn't exist.\n", key_name, set_name, key_value );
        hdl->err_no = REG_ERR_INVALID_KEY_NAME;
        return REG_ERR;
    }
}





/***************************************************************************************************
*Name        : __GetKeyValue
*Prototype   : int32 __GetKeyValue( HNODE handle, const char *set_name, const char *key_name,
*                                 char *key_value )
*Arguments   : handle         a handle of a node
*              set_name       set name
*              key_name       key name
*              key_value      output. a buffer to store key value. The size of the buffer must NOT be
*                             less than KEY_VALUE_LEN+1, (the constant is defined in head file).
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : get the value of specified key.
*Other       :
***************************************************************************************************/
int32 __GetKeyValue( HNODE handle, const char *set_name, const char *key_name, char *key_value )
{
    uint32 pre_node;
    uint32 cur_node;
    int32  status;
    set_desc_t *p;
    G_FILE  *fp;
    key_item_t  key_item;
    HNODE_i hdl = (HNODE_i)handle;

    if( handle_is_invalid( hdl ) )
        return REG_ERR;

    if( !is_valid_set_name( set_name ) )
    {
        __wrn("Fail in getting the value of key \"%s\" of set \"%s\". Because set name is illegal.\n", key_name, set_name );
        hdl->err_no = REG_ERR_ILLEGAL_SET_NAME;
        return REG_ERR;
    }

    if( !is_valid_key_name( key_name ) )
    {
        __wrn("Fail in getting the value of key \"%s\" of set \"%s\". Because key name is illegal.\n", key_name, set_name );
        hdl->err_no = REG_ERR_ILLEGAL_KEY_NAME;
        return REG_ERR;
    }

    fp = hdl->reg_fp;
    /* search set */
    p = search_set( hdl, set_name );
    if( p == NULL )
    {
        __wrn("Fail in getting the value of key \"%s\" of set \"%s\". Because set doesn't exist.\n", key_name, set_name );
        hdl->err_no = REG_ERR_INVALID_SET_NAME;
        return REG_ERR;
    }

    /* search key in hash chain */
    status = search_key_in_hash( fp, p->set_id, key_name, &pre_node, &cur_node );
    if(     status == NOT_FIND_ITEM_PRE_IS_HEAD_NODE
            ||  status == NOT_FIND_ITEM_PRE_IN_CHAIN )              // the key has not existed
    {
        __wrn("Fail in getting the value of key \"%s\" of set \"%s\". Because key doesn't exist.\n", key_name, set_name );
        hdl->err_no = REG_ERR_INVALID_KEY_NAME;
        return REG_ERR;
    }

    g_fseek( fp, cur_node, SEEK_SET );
    g_fread( &key_item, 1, sizeof(key_item), fp );
    if( key_item.value_pos == 0 )
    {
        g_fread( key_value, 1, key_item.value_len + 1, fp );
    }
    else
    {
        g_fseek( fp, key_item.value_pos, SEEK_SET );
        g_fread( key_value, 1, key_item.value_len + 1, fp );
    }

    //	__msg("Succeed in getting the value of key \"%s\" of set \"%s\".\n", key_name, set_name );
    hdl->err_no = REG_OK;
    return REG_OK;
}





/***************************************************************************************************
*Name        : __DeleteKey
*Prototype   : int32 __DeleteKey( HNODE handle, const char *set_name, const char *key_name )
*Arguments   : handle         a handle of node
*              set_name       set name
*              key_name       key name
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : delete a key from specified set.
*Other       :
***************************************************************************************************/
int32 __DeleteKey( HNODE handle, const char *set_name, const char *key_name )
{
    uint32 pre_node;
    uint32 cur_node;
    int32  status;
    set_desc_t *p;
    key_item_t key_item;
    G_FILE  *fp;
    uint32 pointer;
    uint32 pre_node_m_next_pos;
    HNODE_i hdl = (HNODE_i)handle;

    if( handle_is_invalid( hdl ) )
        return REG_ERR;

    if( hdl->mode == REG_READONLY )
    {
        __wrn("Fail in deleting key \"%s\" of set \"%s\". Because the operation is forbidden in READONLY mode.\n", key_name, set_name );
        hdl->err_no = REG_ERR_ILLEGAL_OP;
        return REG_ERR;
    }

    if( !is_valid_set_name( set_name ) )
    {
        __wrn("Fail in deleting key \"%s\" of set \"%s\". Because set name is illegal.\n", key_name, set_name );
        hdl->err_no = REG_ERR_ILLEGAL_SET_NAME;
        return REG_ERR;
    }

    if( !is_valid_key_name( key_name ) )
    {
        __wrn("Fail in deleting key \"%s\" of set \"%s\". Because key name is illegal.\n", key_name, set_name );
        hdl->err_no = REG_ERR_ILLEGAL_KEY_NAME;
        return REG_ERR;
    }

    fp = hdl->reg_fp;

    /* search set */
    p = search_set( hdl, set_name );
    if( p == NULL )
    {
        __wrn("Fail in deleting key \"%s\" of set \"%s\". Because set doesn't exist.\n", key_name, set_name );
        hdl->err_no = REG_ERR_INVALID_SET_NAME;
        return REG_ERR;
    }

    /* search key in hash chain */
    status = search_key_in_hash( fp, p->set_id, key_name, &pre_node, &cur_node );
    if( status == NOT_FIND_ITEM_PRE_IS_HEAD_NODE  ||  status == NOT_FIND_ITEM_PRE_IN_CHAIN )
    {
        __wrn("Fail in deleting key \"%s\" of set \"%s\". Because the key doesn't exist.\n", key_name, set_name );
        hdl->err_no = REG_ERR_INVALID_KEY_NAME;
        return REG_ERR;
    }

    /* invalidate key item */
    g_fseek( fp, cur_node, SEEK_SET );
    g_fread( &key_item, 1, sizeof(key_item), fp );
    key_item.valid_status = INVALID_STATUS;
    g_fseek( fp, -(int32)sizeof(key_item), SEEK_CUR );
    g_fwrite( &key_item, sizeof(key_item), 1, fp );

    /* delete the key node from hash chain */
    pointer = key_item.hash_next;
    if( status == FIND_ITEM_PRE_IS_HEAD_NODE )              // current node's previous node is in hash address table
        pre_node_m_next_pos = RFO_M_HASH_TBL_OFFSET + sizeof(uint32) * pre_node;
    else                                                    // current node's previous node is in chain
        pre_node_m_next_pos = pre_node + KI_M_HASH_NEXT_OFFSET;
    g_fseek( fp, pre_node_m_next_pos, SEEK_SET );
    g_fwrite( &pointer, sizeof(uint32), 1, fp );

    /* modify set information */
    p->key_nr--;
    p->set_size -= ( sizeof(key_item_t) + key_item.value_buf_size );

    /* modify hdl information */
    hdl->invalid_space_size   += ( sizeof(key_item_t) + key_item.value_buf_size );
    hdl->modified_status = MODIFIED;

    __msg("Succeed in deleting key \"%s\" of set \"%s\".\n", key_name, set_name );
    hdl->err_no = REG_OK;
    return REG_OK;
}





/***************************************************************************************************
*Name        : __CreateSet
*Prototype   : int32 __CreateSet( HNODE handle, const char *set_name )
*Arguments   : handle         a handle of a node
*              set_name       set name
*Return      : REG_OK                   operation succeed
*              REG_ERR                  operation fail
*              REG_ERR_SET_EXIST        the set has existed.
*Description : create a set based on specified name.
*Other       :
***************************************************************************************************/
int32 __CreateSet( HNODE handle, const char *set_name )
{
    set_desc_t *p;
    set_item_t set_item;
    HNODE_i hdl = (HNODE_i)handle;

    if( handle_is_invalid( hdl ) )
        return REG_ERR;

    if( hdl->mode == REG_READONLY )
    {
        __wrn("Fail in creating set \"%s\". Because the operation is forbidden in READONLY mode.\n", set_name );
        hdl->err_no = REG_ERR_ILLEGAL_OP;
        return REG_ERR;
    }

    if( !is_valid_set_name( set_name ) )
    {
        __wrn("Fail in creating set \"%s\". Because set name is illegal.\n", set_name );
        hdl->err_no = REG_ERR_ILLEGAL_SET_NAME;
        return REG_ERR;
    }

    /* search set */
    p = search_set( hdl, set_name );
    if( p != NULL )
    {
        __msg("the set \"%s\" has already existed.\n", set_name );
        hdl->err_no = REG_ERR_SET_EXIST;
        return REG_ERR_SET_EXIST;
    }

    p = (set_desc_t *) g_malloc( sizeof(set_desc_t) );
    if( p == NULL )
    {
        __wrn("Error in creating set \"%s\". Because allocating memory fail.\n", set_name );
        hdl->err_no = REG_ERR_ALLOC_MEM_FAIL;
        return REG_ERR;
    }

    /* put current set in the end of chain */
    if( hdl->first_set == NULL )                            // current set is the first set
        hdl->first_set = p;
    else
        hdl->last_set->next_set = p;
    if( hdl->last_set != NULL )
        hdl->last_set->next_set_pos = hdl->total_size;
    hdl->last_set = p;
    hdl->cur_set  = p;
    hdl->set_nr++;

    p->next_set = NULL;
    p->next_key_pos = NULL_POINTER;

    set_item.next_set_pos  = p->next_set_pos  = NULL_POINTER;
    set_item.append_pos    = p->append_pos    = NULL_POINTER;
    set_item.seq_status    = p->seq_status    = NOT_SEQ;
    set_item.set_id        = p->set_id        = hdl->set_id_base++;
    set_item.key_nr        = p->key_nr        = 0;
    set_item.set_size      = p->set_size      = sizeof(set_item_t);
    set_item.pos           = p->pos           = hdl->total_size;
    set_item.first_key_pos = p->first_key_pos = NULL_POINTER;
    set_item.last_key_pos  = p->last_key_pos  = NULL_POINTER;
    g_strcpy( p->name, set_name );
    g_strcpy( set_item.name, set_name );

    /* set set item information */
    set_item.type = SET_ITEM;
    set_item.valid_status = VALID_STATUS;
    g_fseek( hdl->reg_fp, hdl->total_size, SEEK_SET );
    g_fwrite( &set_item, sizeof(set_item_t), 1, hdl->reg_fp );
    /* modify hdl information */
    hdl->total_size += sizeof(set_item_t);
    hdl->modified_status = MODIFIED;

    __msg("Succeed in creating set \"%s\".\n", set_name );
    hdl->err_no = REG_OK;
    return REG_OK;
}





/***************************************************************************************************
*Name        : __DeleteSet
*Prototype   : int32 __DeleteSet( HNODE handle, const char *set_name )
*Arguments   : handle         a handle of a node
*              set_name       set name
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : delete a set. Once the set is deleted, all the keys that pertain to the set are deteled.
*Other       :
***************************************************************************************************/
int32 __DeleteSet( HNODE handle, const char *set_name )
{
    set_desc_t *pre;
    set_desc_t *p;
    int32  valid_status;
    uint32 next_key_pos;
    key_item_t  key_item;
    HNODE_i hdl = (HNODE_i)handle;

    if( handle_is_invalid( hdl ) )
        return REG_ERR;

    if( hdl->mode == REG_READONLY )
    {
        __wrn("Fail in deleting set \"%s\". Because the operation is forbidden in READONLY mode.\n", set_name );
        hdl->err_no = REG_ERR_ILLEGAL_OP;
        return REG_ERR;
    }

    if( !is_valid_set_name( set_name ) )
    {
        __wrn("Fail in deleting set \"%s\". Because set name is illegal.\n", set_name );
        hdl->err_no = REG_ERR_ILLEGAL_SET_NAME;
        return REG_ERR;
    }

    for( pre = NULL, p = hdl->first_set; p != NULL; pre = p, p = p->next_set )
    {
        if( g_strcmp( p->name, set_name ) == 0 )
            break;
    }
    if( p == NULL )                                         // not find set
    {
        __wrn("Fail in deleting set \"%s\". Because set doesn't exist.\n", set_name );
        hdl->err_no = REG_ERR_INVALID_SET_NAME;
        return REG_ERR;
    }
    /* delete set from chain */
    if( p == hdl->first_set )                               // the first of the chain
        hdl->first_set = p->next_set;
    else
    {
        pre->next_set = p->next_set;
        pre->next_set_pos = p->next_set_pos;
    }

    hdl->set_nr--;
    if( p == hdl->last_set )                                // the last of chain
        hdl->last_set = pre;
    if( p == hdl->next_set )
        hdl->next_set = p->next_set;
    if( p == hdl->cur_set )
        hdl->cur_set = NULL;

    /* invalidate set item */
    valid_status = INVALID_STATUS;
    g_fseek( hdl->reg_fp, p->pos + ST_M_VALID_STATUS_OFFSET, SEEK_SET );
    g_fwrite( &valid_status, 1, sizeof(valid_status), hdl->reg_fp );

    /* delete all keys of the set from hash chain */
    for( next_key_pos = p->first_key_pos; next_key_pos != NULL_POINTER; next_key_pos = key_item.next_key_pos )
    {
        int32  status;
        uint32 pre_node;
        uint32 cur_node;
        uint32 pre_node_m_next_pos;

        g_fseek( hdl->reg_fp, next_key_pos, SEEK_SET );
        g_fread( &key_item, 1, sizeof(key_item), hdl->reg_fp );

        if( key_item.valid_status != VALID_STATUS )           // invalid item
            continue;
        /* search key in hash chain */
        status = search_key_in_hash( hdl->reg_fp, p->set_id, key_item.name, &pre_node, &cur_node );
        if( status == NOT_FIND_ITEM_PRE_IS_HEAD_NODE  ||  status == NOT_FIND_ITEM_PRE_IN_CHAIN )
        {
            __wrn("FATAL ERROR ! in line %u. can't find key %s in hash chain.\n", __LINE__, key_item.name );
            hdl->err_no = REG_ERR_INVALID_KEY_NAME;
            return REG_ERR;
        }

        /* delete the key node from hash chain */
        if( status == FIND_ITEM_PRE_IS_HEAD_NODE )              // current node's previous node is in hash address table
            pre_node_m_next_pos = RFO_M_HASH_TBL_OFFSET + sizeof(uint32) * pre_node;
        else                                                    // current node's previous node is in chain
            pre_node_m_next_pos = pre_node + KI_M_HASH_NEXT_OFFSET;
        g_fseek( hdl->reg_fp, pre_node_m_next_pos, SEEK_SET );
        g_fwrite( &(key_item.hash_next), sizeof(uint32), 1, hdl->reg_fp );
    }

    /* modify hdl information */
    hdl->modified_status = MODIFIED;
    hdl->invalid_space_size   += p->set_size;

    g_free( p );

    __msg("Succeed in deleting set \"%s\".\n" );
    hdl->err_no = REG_OK;
    return REG_OK;
}





/***************************************************************************************************
*Name        : __CreatePath
*Prototype   : int32 __CreatePath( const char *path )
*Arguments   : path           register path
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : create a path
*Other       :
***************************************************************************************************/
int32 __CreatePath( const char *path )
{
    char real_path[256];

    g_strcpy( real_path, reg_root_path );
    g_strcat( real_path, path );

    if( g_mkdir( real_path ) != 0 )
    {
        __wrn("Error in Creating path %s. Because fail in creating dir %s. At line %u of file %s.\n", path, real_path,
              __LINE__, __FILE__);
        return REG_ERR;
    }

    __msg("Succeed in creating path %s.\n", path );
    return REG_OK;
}





/***************************************************************************************************
*Name        : __DeletePath
*Prototype   : int32 __DeletePath( const char *path )
*Arguments   : path           register path
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : delete all the sub_folder and all the files.
*Other       :
***************************************************************************************************/
int32 __DeletePath( const char *path )
{
    char real_path[256];

    /* remove temp reg file */
    g_strcpy( real_path, reg_root_path );
    g_strcat( real_path, path );
    g_strcat( real_path, "\\" );
    g_strcat( real_path, TEMP_REG_FILE_NAME );
    g_remove( real_path );

    /* remove directory */
    g_strcpy( real_path, reg_root_path );
    g_strcat( real_path, path );
    if( g_rmdir( real_path ) != 0 )
    {
        __wrn("Fail in deleting path %s. Because fail in removing dir %s. At line %u of file %s.\n", path, real_path,
              __LINE__, __FILE__);
        return REG_ERR;
    }

    __msg("Succeed in deleting path %s.\n", path);
    return REG_ERR;
}





/***************************************************************************************************
*Name        : __reg_init
*Prototype   : int32 __reg_init( void )
*Arguments   : void
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : initialise register table module.
*Other       :
***************************************************************************************************/
int32 __reg_init( void )
{
    global_chain_mutex = g_create_mutex( );
    if( global_chain_mutex == NULL )
    {
        __wrn("Error in initialising register. Because fail in initialising global_chain_mutex.\n");
        return REG_ERR;
    }

    HNODE_head.count = 0;
    HNODE_head.next_HNODE = NULL;

    g_strcpy( reg_root_path, DEFAULT_REG_ROOT_PATH );

    /* copy source reg to destination reg */
    __CopyReg( );

    return REG_OK;
}





/***************************************************************************************************
*Name        : __reg_exit
*Prototype   : int32 __reg_exit( void )
*Arguments   : void
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : exit register table module.
*Other       :
***************************************************************************************************/
int32 __reg_exit( void )
{
    uint8  mutex_err;

    g_delete_mutex( global_chain_mutex, &mutex_err );
    if( mutex_err != EVENT_OK )
    {
        __wrn("Error in quitting register table module. Because fail in deleting global_chain_mutex.\n");
        return REG_ERR;
    }
    if( HNODE_head.next_HNODE != NULL )
    {
        HNODE_i p;
        HNODE_i pre;

        __wrn("Fail in quitting register table module. Bucause some nodes aren't closed.\n");
        /* close all the rest node handles */
        for( p = HNODE_head.next_HNODE;  p != NULL;  )
        {
            pre = p;
            p = p->next_HNODE;
            __msg("ready to close node \"%s\".\n", pre->node );
            CloseNode_i( pre );
        }
        return REG_ERR;
    }

    //	__msg("Succeed in quitting register table module.");
    return REG_OK;
}





/***************************************************************************************************
*Name        : __GetError
*Prototype   : int32 __GetError( HNODE handle )
*Arguments   : hdl        a handle of a node
*Return      : error code
*Description : get error code of current operation.
*Other       :
***************************************************************************************************/
int32 __GetError( HNODE handle )
{
    HNODE_i hdl = (HNODE_i)handle;

    if( handle_is_invalid( hdl ) )
        return REG_ERR_INVALID_HANDLE;

    return hdl->err_no;
}





/***************************************************************************************************
*Name        : __ClrError
*Prototype   : int32 __ClrError( HNODE handle )
*Arguments   : hdl            a handle of a node
*Return      : REG_OK         operation succeed
*              REG_ERR        operation fail
*Description : get error code of current operation.
*Other       :
***************************************************************************************************/
int32 __ClrError( HNODE handle )
{
    HNODE_i hdl = (HNODE_i)handle;

    if( handle_is_invalid( hdl ) )
        return REG_ERR;

    hdl->err_no = REG_OK;

    return REG_OK;
}





/***************************************************************************************************
*Name        : __SetRootPath
*Prototype   : int32 __SetRootPath( const char *path )
*Arguments   : path           register root path
*Return      : void
*Description : set register root path.
*Other       :
***************************************************************************************************/
void __SetRootPath( const char *path )
{
    g_strcpy( reg_root_path, path );
}





/***************************************************************************************************
*Name        : __GetRootPath
*Prototype   : int32 __GetRootPath( char *path )
*Arguments   : path           register root path
*Return      : void
*Description : get register root path.
*Other       :
***************************************************************************************************/
void __GetRootPath( char *path )
{
    g_strcpy( path, reg_root_path );
}




static __s32  __CreateRegInfoFile( __s32 sys_version )
{
    char     info_file[ 256];
    G_FILE  *fp = NULL;
    reg_info_file_head_t  head;

    g_strcpy( info_file, reg_root_path );
    g_strcat( info_file, "\\");
    g_strcat( info_file, REG_INFO_FILE_NAME );
    fp = g_fopen( info_file, "wb+");
    if( fp == NULL )
    {
        __wrn("Fail in creating reg info file %s.\n", info_file);
        return REG_ERR;
    }

    g_memset( &head, 0, sizeof(head) );

    g_strncpy( head.magic, REG_INFO_FILE_MAGIC, MAGIC_SIZE );
    head.version     = REG_FILE_VERSION;
    head.is_valid    = 1;
    head.sys_version = sys_version;

    g_fwrite( &head, 1, sizeof(head), fp );

    g_fclose( fp );
    fp = NULL;

    return REG_OK;
}




static __s32  __GetRegInfo( reg_info_t *info_p )
{
    char     info_file[ 256];
    G_FILE  *fp = NULL;
    reg_info_file_head_t  head;

    g_strcpy( info_file, reg_root_path );
    g_strcat( info_file, "\\");
    g_strcat( info_file, REG_INFO_FILE_NAME );
    fp = g_fopen( info_file, "rb");
    if( fp == NULL )
    {
        __wrn("Fail in opening reg info file %s.\n", info_file);
        return REG_ERR;
    }

    g_fread( &head, 1, sizeof(head), fp );

    info_p->version     = head.version;
    info_p->is_valid    = head.is_valid;
    info_p->sys_version = head.sys_version;

    g_fclose( fp );
    fp = NULL;

    return ORC_OK;
}



static __s32  __CopyReg( void )
{
    reg_info_t   info;
    uint32       sys_version;
    char         burn_flag_file[256];
    G_FILE      *fp = NULL;

    sys_version = esKSRV_GetVersion( EPDK_VER );

    g_strcpy( burn_flag_file, SOURCE_REG_ROOT_PATH );
    g_strcat( burn_flag_file, "\\" );
    g_strcat( burn_flag_file, BURN_FLAG_FILE_NAME );
    fp = g_fopen( burn_flag_file, "rb" );
    if( fp != NULL )
    {
        g_fclose( fp );
        fp = NULL;
        __msg("has open reg burn file. to remove the file.\n");
        g_remove( burn_flag_file );
        __msg("to delete old reg.\n");
        g_delete_folder_all( reg_root_path );
        goto copy;
    }
    __msg("can't open reg burn flag file %s.\n", burn_flag_file );

    if( __GetRegInfo( &info ) != REG_OK )
    {
        __msg("REG info file NOT exist.\n");
        goto copy;
    }
    if( info.version != REG_FILE_VERSION )
    {
        __msg("reg old version of reg is %x and current version is %x.\n", info.version, REG_FILE_VERSION );
        __msg("to delete old reg.\n");
        g_delete_folder_all( reg_root_path );
        goto copy;
    }


    if( info.sys_version != sys_version )
    {
        __msg("sys version of reg is %x and current version is %x.\n", info.sys_version, sys_version );
        goto copy;
    }

    return REG_OK;

copy:
    __msg("to copy original reg files to operation area.\n");
    if( folder_copy( SOURCE_REG_ROOT_PATH, reg_root_path ) )
    {
        __msg("copry fail.\n");
        return REG_ERR;
    }
    else
    {
        __msg("copy succeed.\n");
        __CreateRegInfoFile( sys_version );
        return REG_OK;
    }
}



static __s32  mark_dirty( G_FILE *fp )
{
    __u32  offset;
    __s32  valid_status;

    offset = g_ftell( fp );

    valid_status = INVALID_STATUS;
    g_fseek( fp, RFH_M_VALID_STATUS_OFFSET, SEEK_SET );
    g_fwrite( &valid_status, 1, sizeof(valid_status), fp );

    g_fseek( fp, offset, SEEK_SET );

    return 0;
}


static void _gen_reg_file_path( const char *root, const char *node, char *reg_file )
{
    g_strcpy( reg_file, root );
    g_strcat( reg_file, node );
    g_strcat( reg_file, REG_FILE_EXTENTION );
}


static __s32 _copy_file( const char *src, const char *dst )
{
#define COPY_BUF_SIZE         SZ_16K
    G_FILE *src_fp = NULL;
    G_FILE *dst_fp = NULL;
    char   *buf    = NULL;
    __s32   size;

    src_fp = g_fopen( src, "rb" );
    if( src_fp == NULL )
    {
        __wrn("Fail in open file %s.\n", src );
        goto error;
    }

    dst_fp = g_fopen( src, "wb+" );
    if( dst_fp == NULL )
    {
        __wrn("Fail in open file %s.\n", dst );
        goto error;
    }

    buf = (char *)g_malloc( COPY_BUF_SIZE );
    if( buf == NULL )
    {
        __wrn("Error in allocating memory %d.\n", COPY_BUF_SIZE);
        goto error;
    }

    while( !g_feof( src_fp ) )
    {
        size = g_fread( buf, 1, COPY_BUF_SIZE, src_fp );
        g_fwrite( buf, 1, size, dst_fp );
    }

    g_fclose( src_fp );
    g_fclose( dst_fp );
    g_free( buf );

    return REG_OK;

error:
    if( src_fp != NULL )
    {
        g_fclose( src_fp );
        src_fp = NULL;
    }
    if( dst_fp != NULL )
    {
        g_fclose( dst_fp );
        dst_fp = NULL;
    }
    if( buf != NULL )
    {
        g_free( buf );
        buf = NULL;
    }

    return REG_ERR;
}


__s32 __resume_default_node( const char *node )
{
    char  src_file[256];
    char  dst_file[256];

    _gen_reg_file_path( SOURCE_REG_ROOT_PATH, node, src_file );
    _gen_reg_file_path( reg_root_path, node, src_file );

    /* copy */
    if( _copy_file( src_file, dst_file ) == REG_OK )
    {
        return REG_OK;
    }
    else
    {
        return REG_ERR;
    }
}



#endif     //  ifndef __reg_private_c

/* end of reg_private.c  */
