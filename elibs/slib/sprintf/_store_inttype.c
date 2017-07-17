/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               ANSIC Libary
*
*                                    (c) Copyright 2010-2011, Sunny China
*                                             All Rights Reserved
*
* File    : _store_inttype.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-11-9
* Descript:
* Update  : date                auther      ver     notes
*           2020-11-9 15:05:01  Sunny       1.0
*********************************************************************************************************
*/
#include "stdio.h"
#include "sprintf_i.h"
/* Right now, we assume intmax_t is either long or long long */


/* We assume int may be short or long, but short and long are different. */

void _store_inttype(register void *dest, int desttype, uintmax_t val)
{
    if (desttype == __PA_FLAG_CHAR)   /* assume char not int */
    {
        *((unsigned char *) dest) = (unsigned char)val;
        return;
    }
#if defined(LLONG_MAX) && (LONG_MAX != LLONG_MAX)
    if (desttype == PA_FLAG_LONG_LONG)
    {
        *((unsigned long long int *) dest) = (unsigned char)val;
        return;
    }
#endif /* LLONG_MAX */
#if SHRT_MAX != INT_MAX
    if (desttype == PA_FLAG_SHORT)
    {
        *((unsigned short int *) dest) = val;
        return;
    }
#endif /* SHRT_MAX */
#if LONG_MAX != INT_MAX
    if (desttype == PA_FLAG_LONG)
    {
        *((unsigned long int *) dest) = val;
        return;
    }
#endif /* LONG_MAX */

    *((unsigned int *) dest) = (unsigned int)val;
}
