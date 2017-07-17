/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                DBMS Module
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                             All Rights Reserved
*
* File    : db_util.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-8-21
* Descript: dbms misc functions.
* Update  : date                auther      ver     notes
*           2010-8-21 19:24:38  Sunny       1.0     create this file.
*********************************************************************************************************
*/

#include    "dbms_i.h"

/*
*********************************************************************************************************
*                                       DB QUOTE STRING
*
* Description: Escapes any special characters in a string and places the string itself in
*              quotes so the string can be sent to the database engine as a query string.
*
* Arguments  :
*              orig  --pointer to the string to quote and escape.
*              dest  --pointer to the quoted and escaped string.
*              length--length of 'orig' string.
*
* Returns    : The new string's length in bytes, excluding the terminating zero byte,
*              or 0 in case of an error.
*
* Note       :
*********************************************************************************************************
*/
__size_t  DB_QuoteString(const char *orig, char *dest, __size_t length)
{
    const char  *start = dest;
    const char  *end;

    /* add ' to string start */
    *dest++ = '\'';

    for(end = orig + length; orig != end ; orig++)
    {
        switch (*orig)
        {
        case 0:
            break;              /* string end           */
        case '\'':
            *dest++ = '\'';     /* double single quote  */
            *dest++ = '\'';
        default:
            *dest++ = *orig;
        }
    }
    /* add ' to string end */
    *dest++ = '\'';
    *dest   = 0;
    return (__size_t)(dest - start);
}
