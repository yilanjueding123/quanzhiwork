/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               ANSIC Libary
*
*                                    (c) Copyright 2010-2011, Sunny China
*                                             All Rights Reserved
*
* File    : _uintmaxtostr.h
* By      : Sunny
* Version : v1.0
* Date    : 2010-11-9
* Descript:
* Update  : date                auther      ver     notes
*           2020-11-9 15:05:01  Sunny       1.0
*********************************************************************************************************
*/
#ifndef __UINTMAXTOSTR_H__
#define __UINTMAXTOSTR_H__

#ifdef _FEATURES_H
# ifndef __USE_ISOC99
#  error features was included without defining _ISOC99_SOURCE!
# endif
#else
# ifndef _ISOC99_SOURCE
#  define _ISOC99_SOURCE
# endif
#endif

#include <limits.h>
//#include <stdint.h>
#include <string.h>

#define __UIM_BUFLEN			12 /* 10 digits + 1 nul + 1 sign */

#define __UIM_BUFLEN_LLONG		12 /* 10 digits + 1 nul + 1 sign */

#define __UIM_BUFLEN_LONG		12 /* 10 digits + 1 nul + 1 sign */

#define __UIM_BUFLEN_INT		12 /* 10 digits + 1 nul + 1 sign */

typedef enum
{
    __UIM_DECIMAL = 0,
    __UIM_GROUP = ',',			/* Base 10 locale-dependent grouping. */
    __UIM_LOWER = 'a' - 10,
    __UIM_UPPER = 'A' - 10,
} __UIM_CASE;

/* Convert the int val to a string in base abs(base).  val is treated as
 * an unsigned ??? int type if base > 0, and signed if base < 0.  This
 * is an internal function with _no_ error checking done unless assert()s
 * are enabled.
 *
 * Note: bufend is a pointer to the END of the buffer passed.
 * Call like this:
 *     char buf[SIZE], *p;
 *     p = _xltostr(buf + sizeof(buf) - 1, {unsigned int},  10, __UIM_DECIMAL)
 *     p = _xltostr(buf + sizeof(buf) - 1,          {int}, -10, __UIM_DECIMAL)
 *
 * WARNING: If base > 10, case _must_be_ either __UIM_LOWER or __UIM_UPPER
 *          for lower and upper case alphas respectively.
 * WARNING: If val is really a signed type, make sure base is negative!
 *          Otherwise, you could overflow your buffer.
 */
extern char *_uintmaxtostr(char *__restrict bufend, uintmax_t uval,
                           int base, __UIM_CASE alphacase);

/* TODO -- make this either a (possibly inline) function? */
#ifndef __BCC__
#define _int10tostr(bufend, intval) \
	_uintmaxtostr((bufend), (intval), -10, __UIM_DECIMAL)
#else  /* bcc doesn't do prototypes, we need to explicitly cast */
#define _int10tostr(bufend, intval) \
	_uintmaxtostr((bufend), (uintmax_t)(intval), -10, __UIM_DECIMAL)
#endif

#define __BUFLEN_INT10TOSTR		__UIM_BUFLEN_INT

#endif /* __UINTMAXTOSTR_H__ */
