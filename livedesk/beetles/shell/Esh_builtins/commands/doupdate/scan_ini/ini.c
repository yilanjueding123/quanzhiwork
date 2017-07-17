/*
************************************************************************************************************************
*                                                        INI
*                                                   the Audio Player
*
*                             Copyright(C), 2006-2008, Allwinner Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : ini.c
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
#ifndef  __ini_c
#define  __ini_c


#include "ini.h"


#define TAB                             '\t'
#define BLANK                           ' '
#define CR                              '\r'
#define LF                              '\n'



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


__Bool is_ident_char( uint8 ch )
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





__Bool is_legal_key_value( const char *str )
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





__Bool is_identifier( const char *str )
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



__Bool is_valid_set_name( const char *set_name )
{
    if(     set_name == NULL
            ||  g_strlen( set_name ) == 0
            ||  g_strlen( set_name ) > SET_NAME_LEN
            || !is_identifier(set_name) )
        return __FALSE;
    else
        return __TRUE;
}



__Bool is_valid_key_name( const char *key_name )
{
    if(     key_name == NULL
            ||  g_strlen( key_name ) == 0
            ||  g_strlen( key_name ) > KEY_NAME_LEN
            || !is_identifier(key_name) )
        return __FALSE;
    else
        return __TRUE;
}


__Bool is_valid_key_value( const char *key_value )
{
    if(     key_value == NULL
            ||  g_strlen( key_value ) == 0
            ||  g_strlen( key_value ) > KEY_VALUE_LEN
            || !is_legal_key_value( key_value) )
        return __FALSE;
    else
        return __TRUE;
}



#endif     //  ifndef __ini_c

/* end of ini.c */
