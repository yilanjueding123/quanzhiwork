/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               ANSIC Libary
*
*                                    (c) Copyright 2010-2011, Sunny China
*                                             All Rights Reserved
*
* File    : vsnprintf.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-11-9
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-9 15:08:01  Sunny       1.0
*********************************************************************************************************
*/

#include <stdarg.h>
#include <string.h>
#include "sprintf_i.h"

typedef struct
{
    unsigned char *bufend;		/* pointer to 1 past end of buffer */
    unsigned char *bufpos;
} __FILE_vsnprintf;

extern int SLIB_vfprintf(__FILE_vsnprintf *__restrict op, register const char *__restrict fmt, va_list ap);
int SLIB_vsnprintf(char *buf, __size_t size, const char *format, va_list arg)
{
    __FILE_vsnprintf f;
    int rv;

    f.bufpos = (unsigned char *)buf;

    if (size > SIZE_MAX - (size_t) buf)
    {
        size = SIZE_MAX - (size_t) buf;
    }
    f.bufend = (unsigned char *)(buf + size);

    rv = SLIB_vfprintf((__FILE_vsnprintf *) &f, format, arg);
    if (size)
    {
        if (f.bufpos == f.bufend)
        {
            --f.bufpos;
        }
        *f.bufpos = 0;
    }
    return rv;
}



