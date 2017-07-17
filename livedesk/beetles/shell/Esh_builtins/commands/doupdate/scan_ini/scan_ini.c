/*
************************************************************************************************************************
*                                                        SCAN_INI
*                                                   the Audio Player
*
*                             Copyright(C), 2006-2008, Allwinner Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : scan_ini.c
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
#ifndef  __scan_ini_c
#define  __scan_ini_c


#include "scan_ini_cfg.h"
#include "scan_ini.h"


#define TAB                             '\t'
#define BLANK                           ' '
#define CR                              '\r'
#define LF                              '\n'


typedef enum
{
    __FILE_END  = 0,
    LINE_TOO_LONG,
    LINE_OK
} line_status_e;


typedef struct _set_desc_t
{
    char    name[SET_NAME_LEN + 1];
    struct _set_desc_t *next;
} set_desc_t;


typedef struct _key_desc_t
{
    char    name[KEY_NAME_LEN + 1];
    struct _key_desc_t *next;
} key_desc_t;



#define INVALID_LABEL         _4_char_to_int32( 'X', 'X', 'X', 'X' )


typedef struct _HLIN_i_t
{
    int32  label;
    int32  attr;
    char   *part1_start;
    char   *part1_end;
    char   *part2_start;
    char   *part2_end;
    char   content[LINE_MAX_LEN + 3];
} HLIN_i_t, *HLIN_i;

#define HLIN_LABEL            _4_char_to_int32( 'H', 'L', 'I', 'N' )


static __Bool is_valid_hlin( HLIN_i hlin )
{
    if( hlin == NULL  ||  hlin->label != HLIN_LABEL )
        return __FALSE;
    else
        return __TRUE;
}



typedef struct _HINI_i_t
{
    int32      label;
    G_FILE     *fp;
    HLIN_i_t   line;
    set_desc_t *set;
    key_desc_t *key;
    int32      error;
} HINI_i_t, *HINI_i;

#define HINI_LABEL            _4_char_to_int32( 'H', 'I', 'N', 'I' )


static __Bool is_valid_hini( HINI_i hini )
{
    if( hini == NULL  ||  hini->label != HINI_LABEL )
        return __FALSE;
    else
        return __TRUE;
}

#define is_white_space( ch )            ( ch == TAB  ||  ch == BLANK )
#define is_alpha( ch )                  ( ( 'a' <= ch && ch <= 'z' ) || ( 'A' <= ch && ch <= 'Z' ) )
#define is_number( ch )                 ( '0' <= ch  &&  ch <= '9' )

__inline char *skip_white_space( const char *str )
{
    while( is_white_space( *str ) )
        str++;
    return (char *)str;
}

__inline char *reverse_skip_white_space( const char *str )
{
    const char *p;

    p = str + g_strlen( str ) - 1;
    while( is_white_space( *p ) )
        p--;
    return (char *)p;
}


static __Bool get_line( FILE *fp, char *str, uint32 line_max );
static uint32 extract_identifier( const char *str, char *ident, uint32 max_len, char **end_p );
//static set_desc_t * search_set( set_desc_t *first_set, const char *set_name );
//static key_desc_t * search_key( key_desc_t *first_key, const char *key_name );
static void destroy_key_chain( key_desc_t *first_key );
static void destroy_set_chain(  set_desc_t *first_set );
static void spec_strcpy( char *dst, const char *start, const char *end );



HINI   open_ini( const char *file )
{
    G_FILE  *fp    = NULL;
    HINI_i  hini_i = NULL;

    fp = g_fopen( file, "rb" );
    if( fp == NULL )
    {
        __wrn("Error in opening file %s. At line %u of file %s.\n", file, __LINE__, __FILE__);
        return NULL;
    }

    hini_i = (HINI_i_t *) g_malloc( sizeof(HINI_i_t) );
    if( hini_i == NULL )
    {
        __wrn("Error in allocating memory. At line %u of file %s.\n", __LINE__, __FILE__);
        g_fclose( fp );
        return NULL;
    }

    hini_i->label      = HINI_LABEL;
    hini_i->fp         = fp;
    hini_i->set        = NULL;
    hini_i->key        = NULL;
    hini_i->line.label = INVALID_LABEL;

    return (HINI)hini_i;
}



int32  close_ini( HINI hini )
{
    HINI_i   hini_i = (HINI_i)hini;

    if( !is_valid_hini( hini_i) )
    {
        __wrn("Fail in closing ini. because handle is invalid. At line %u of file %s.\n", __LINE__, __FILE__);
        return -1;
    }

    g_fclose( hini_i->fp );
    hini_i->fp = NULL;

    destroy_key_chain( hini_i->key );
    hini_i->key = NULL;

    destroy_set_chain( hini_i->set );
    hini_i->set = NULL;

    g_free( hini_i );

    return 0;
}


int32  line_rewind( HINI hini )
{
    HINI_i   hini_i = (HINI_i)hini;

    g_fseek( hini_i->fp, 0, SEEK_SET );

    return 0;
}

HLIN   get_next_line( HINI hini )
{
    HINI_i   hini_i = (HINI_i)hini;
    G_FILE   *fp = NULL;
    char     *p  = NULL;
    HLIN_i   hlin = NULL;
    int32    ret;
    char     set_name[ SET_NAME_LEN + 1 ];
    char     key_name[ KEY_NAME_LEN + 1 ];
    char     key_value[ KEY_VALUE_LEN + 1 ];
    set_desc_t  *new_set = NULL;
    key_desc_t  *new_key = NULL;

    if( !is_valid_hini( hini_i) )
    {
        __wrn("Fail. because handle is invalid. At line %u of file %s.\n", __LINE__, __FILE__);
        return NULL;
    }

    fp = hini_i->fp;
    hlin = &(hini_i->line);
    ret = get_line( fp, hlin->content, LINE_MAX_LEN + 1 ) ;
    if( ret == __FILE_END )                                   // file end
    {
        hini_i->error = INI_ERR_FILE_END;
        hini_i->line.label = INVALID_LABEL;
        return NULL;
    }
    else if( ret == LINE_TOO_LONG )
    {
        goto error;
    }

    p = hlin->content;
    p = skip_white_space( p );
    if( *p == '\0' )                                        // blank line
    {
        hlin->label   = HLIN_LABEL;
        hlin->attr    = LINE_ATTR_BLANK;
        hini_i->error = INI_OK;
        return (HLIN)hlin;
    }
    else if( *p == ';' )                                    // comment line
    {
        hlin->part1_start = p + 1;
        hlin->part1_end   = hlin->content + g_strlen( hlin->content);
        hlin->label       = HLIN_LABEL;
        hlin->attr        = LINE_ATTR_COMMENT;
        hini_i->error     = INI_OK;
        return (HLIN)hlin;
    }
    else if( *p == '[' )                                    // set line
    {
        p++;
        p = skip_white_space( p );
        hlin->part1_start = p;
        if( extract_identifier( p, set_name, SET_NAME_LEN, &p ) == 0 )
        {
            __wrn("Fail in parser line %s. At line %u of file %s.\n", hlin->content, __LINE__, __FILE__);
            goto error;
        }
        hlin->part1_end = p;
        p = skip_white_space( p );
        if( *p != ']' )
        {
            __wrn("Fail in parser line %s. At line %u of file %s.\n", hlin->content, __LINE__, __FILE__);
            goto error;
        }
        p++;
        p = skip_white_space( p );
        if( *p != '\0' )
        {
            __wrn("Fail in parser line %s. At line %u of file %s.\n", hlin->content, __LINE__, __FILE__);
            goto error;
        }
        //		if( search_set( hini_i->set, set_name ) != NULL )
        //		{
        //			__wrn("Error. the Set %s has more definition. At line %u of file %s.\n", set_name, __LINE__, __FILE__);
        //			goto error;
        //		}
        new_set = (set_desc_t *) g_malloc( sizeof(set_desc_t) );
        if( new_set == NULL )
        {
            __wrn("Error in allocating memory. At line %u of file %s.\n", __LINE__, __FILE__);
            goto error;
        }
        g_strcpy( new_set->name, set_name );
        /* put current set in the end of chain */
        new_set->next = hini_i->set;
        hini_i->set   = new_set;
        /* destroy key chain of the last set */
        destroy_key_chain( hini_i->key );
        hini_i->key = NULL;

        hlin->label       = HLIN_LABEL;
        hlin->attr        = LINE_ATTR_SET;
        hini_i->error     = INI_OK;
        return (HLIN)hlin;
    }
    else
    {
        char *end_p;
        char *temp_p;

        p = skip_white_space( p );
        hlin->part1_start = p;
        if( extract_identifier( p, key_name, KEY_NAME_LEN, &p ) == 0 )
        {
            __wrn("Fail in parser line %s. At line %u of file %s.\n", hlin->content, __LINE__, __FILE__);
            goto error;
        }
        hlin->part1_end = p;
        p = skip_white_space( p );
        if( *p != '=' )
        {
            __wrn("Fail in parser line %s. At line %u of file %s.\n", hlin->content, __LINE__, __FILE__);
            goto error;
        }
        p++;
        p = skip_white_space( p );
        hlin->part2_start = p;
        end_p = reverse_skip_white_space( hlin->content );
        if( p > end_p || ( end_p - p ) >= KEY_VALUE_LEN )
        {
            __wrn("Fail in parser line %s. At line %u of file %s.\n", hlin->content, __LINE__, __FILE__);
            goto error;
        }
        hlin->part2_end = end_p + 1;
        for( temp_p = key_value; p <= end_p;  )
            *temp_p++ = *p++;
        *temp_p = '\0';
        if( !is_valid_key_value( key_value ) )
        {
            __wrn("Fail in parser line %s. At line %u of file %s.\n", hlin->content, __LINE__, __FILE__);
            goto error;
        }
        /* check whether the key has existed in the set */
        //		if( search_key( hini_i->key, key_name ) )
        //		{
        //			__wrn("Fail in parser line %s. At line %u of file %s.\n", hlin->content, __LINE__, __FILE__);
        //			goto error;
        //		}
        new_key = (key_desc_t *) g_malloc( sizeof(key_desc_t) );
        if( new_key == NULL )
        {
            __wrn("Error in allocating memory. At line %u of file %s.\n", __LINE__, __FILE__);
            goto error;
        }
        g_strcpy( new_key->name, key_name );
        /* put current key in the end of chain */
        new_key->next = hini_i->key;
        hini_i->key   = new_key;

        hlin->label       = HLIN_LABEL;
        hlin->attr        = LINE_ATTR_KEY;
        hini_i->error     = INI_OK;
        return (HLIN)hlin;
    }

error:
    hini_i->error = INI_ERR;
    hini_i->line.label = INVALID_LABEL;
    return NULL;
}



int32  line2attr( HLIN hlin )
{
    HLIN_i  hlin_i = (HLIN_i)hlin;

    if( !is_valid_hlin( hlin_i) )
    {
        __wrn("Fail. because handle is invalid. At line %u of file %s.\n", __LINE__, __FILE__);
        return -1;
    }

    return (hlin_i->attr);
}



int32  line2comment( HLIN hlin, char *comment )
{
    HLIN_i  hlin_i = (HLIN_i)hlin;

    if( !is_valid_hlin( hlin_i) || hlin_i->attr != LINE_ATTR_COMMENT )
    {
        __wrn("Fail. because handle is invalid. At line %u of file %s.\n", __LINE__, __FILE__);
        return -1;
    }

    spec_strcpy( comment, hlin_i->part1_start, hlin_i->part1_end );

    return 0;
}



int32  line2set( HLIN hlin, char *set_name )
{
    HLIN_i  hlin_i = (HLIN_i)hlin;

    if( !is_valid_hlin( hlin_i) || hlin_i->attr != LINE_ATTR_SET )
    {
        __wrn("Fail. because handle is invalid. At line %u of file %s.\n", __LINE__, __FILE__);
        return -1;
    }

    spec_strcpy( set_name, hlin_i->part1_start, hlin_i->part1_end );

    return 0;
}



int32  line2key( HLIN hlin, char *key_name, char *key_value )
{
    HLIN_i  hlin_i = (HLIN_i)hlin;

    if( !is_valid_hlin( hlin_i) || hlin_i->attr != LINE_ATTR_KEY )
    {
        __wrn("Fail. because handle is invalid. At line %u of file %s.\n", __LINE__, __FILE__);
        return -1;
    }

    spec_strcpy( key_name,  hlin_i->part1_start, hlin_i->part1_end );
    spec_strcpy( key_value, hlin_i->part2_start, hlin_i->part2_end );

    return 0;
}



int32  get_error( HINI hini )
{
    HINI_i hini_i = (HINI_i)hini;

    if( !is_valid_hini( hini_i) )
    {
        __wrn("Fail. because handle is invalid. At line %u of file %s.\n", __LINE__, __FILE__);
        return INI_ERR;
    }

    return hini_i->error;
}


__Bool check_ini( const char *file )
{
    HINI   hini;
    HLIN   hlin;
    int32  ret;

    hini = open_ini( file );
    if( hini == NULL )
        return __FALSE;

    while( 1 )
    {
        hlin = get_next_line( hini );
        if( hlin == NULL )
        {
            close_ini( hini );
            ret = get_error( hini );
            if( ret == INI_ERR )
                return __FALSE;
            else
                return __TRUE;
        }
    }
}


static int32 get_line( FILE *fp, char *str, uint32 line_max )
{
    uint32  nr;
    uint32  read_nr = line_max + 2;
    uint32  i;
    uint32  next_line = 0;

    nr = g_fread( str, 1, read_nr, fp );
    if( nr == 0 )
        return __FILE_END;

    /* search line end char */
    for( i = 0;  i < nr;  i++ )
    {
        if( str[i] == CR  || str[i] == LF )
        {
            if( str[i + 1] == CR  || str[i + 1] == LF )
                next_line = i + 2;
            else
                next_line = i + 1;
            break;
        }
    }

    if( i < nr )               // a valid string line
    {
        str[i] = '\0';
        g_fseek( fp, (int32)next_line - (int32)nr, SEEK_CUR );
        return LINE_OK;
    }
    else
    {
        if( nr < read_nr )        // the last line without LINE_END
        {
            str[nr] = '\0';
            return LINE_OK;
        }
        else                       // line is too long
            return LINE_TOO_LONG;
    }
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


//static set_desc_t * search_set( set_desc_t *first_set, const char *set_name )
//{
//	set_desc_t *p;
//
//	for( p = first_set; p != NULL; p = p->next )
//	{
//		if( g_strcmp( p->name, set_name ) == 0 )
//			return p;
//	}
//	return NULL;
//}
//
//
//static key_desc_t * search_key( key_desc_t *first_key, const char *key_name )
//{
//	key_desc_t *p;
//
//	for( p = first_key; p != NULL; p = p->next )
//	{
//		if( g_strcmp( p->name, key_name ) == 0 )
//			return p;
//	}
//	return NULL;
//}


static void destroy_key_chain( key_desc_t *first_key )
{
    key_desc_t  *p;
    key_desc_t  *pre;

    for( p = first_key;  p != NULL;  )
    {
        pre = p;
        p = p->next;
        g_free( pre );
    }
}


static void destroy_set_chain( set_desc_t *first_set )
{
    set_desc_t  *p;
    set_desc_t  *pre;

    for( p = first_set;  p != NULL;  )
    {
        pre = p;
        p = p->next;
        g_free( pre );
    }
}


static void spec_strcpy( char *dst, const char *start, const char *end )
{
    while( start < end )
        *dst++ = *start++;
    *dst = '\0';
}


#endif     //  ifndef __scan_ini_c

/* end of scan_ini.c */
