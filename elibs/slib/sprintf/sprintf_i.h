/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               ANSIC Libary
*
*                                    (c) Copyright 2010-2011, Sunny China
*                                             All Rights Reserved
*
* File    : sprintf_i.h
* By      : Sunny
* Version : v1.0
* Date    : 2010-11-9
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-9 15:08:01  Sunny       1.0
*********************************************************************************************************
*/
#ifndef __SPRINTF_I__
#define __SPRINTF_I__
#define __restrict

#include    "epdk.h"

#  define __INT64_C(c)	c ## L
#  define __UINT64_C(c)	c ## UL

# define INTMAX_MIN		(-__INT64_C(9223372036854775807)-1)
/* Maximum for largest signed integral type.  */
# define INTMAX_MAX		(__INT64_C(9223372036854775807))


#  define SIZE_MAX		(4294967295U)

#  define PTRDIFF_MIN		(-2147483647-1)
#  define PTRDIFF_MAX		(2147483647)

#  define INTPTR_MIN		(-2147483647-1)
#  define INTPTR_MAX		(2147483647)
#  define UINTPTR_MAX		(4294967295U)

/* SUPPORT LONG LONG TYPE, By Sunny at 2010-11-09 */
#undef	LLONG_MAX
#define LLONG_MAX    0x7fffffffffffffffLL

enum
{
    /* C type: */
    PA_INT,			/* int */
    PA_CHAR,			/* int, cast to char */
    PA_WCHAR,			/* wide char */
    PA_STRING,			/* const char *, a '\0'-terminated string */
    PA_WSTRING,			/* const wchar_t *, wide character string */
    PA_POINTER,			/* void * */
    PA_FLOAT,			/* float */
    PA_DOUBLE,			/* double */
    __PA_NOARG,			/* non-glibc -- signals non-arg width or prec */
    PA_LAST
};

/* Flag bits that can be set in a type returned by `parse_printf_format'.  */
/* WARNING -- These differ in value from what glibc uses. */
#define PA_FLAG_MASK		(0xff00)
#define __PA_FLAG_CHAR		(0x0100) /* non-gnu -- to deal with hh */
#define PA_FLAG_SHORT		(0x0200)
#define PA_FLAG_LONG		(0x0400)
#define PA_FLAG_LONG_LONG	(0x0800)
#define PA_FLAG_LONG_DOUBLE	PA_FLAG_LONG_LONG
#define PA_FLAG_PTR		(0x1000) /* TODO -- make dynamic??? */

#define __PA_INTMASK		(0x0f00) /* non-gnu -- all int flags */


typedef __int64		intmax_t;
typedef unsigned __int64	uintmax_t;
typedef int			intptr_t;

struct printf_info
{
    int prec;			/* Precision.  */
    int width;			/* Width.  */
    int spec;

    unsigned int _flags;		/* non-gnu */
#define __PRINT_INFO_FLAG_space					(1<<0)
#define __PRINT_INFO_FLAG_showsign				(1<<1)
#define __PRINT_INFO_FLAG_extra					(1<<2)
#define __PRINT_INFO_FLAG_left					(1<<3)
#define __PRINT_INFO_FLAG_alt					(1<<4)
#define __PRINT_INFO_FLAG_group					(1<<5)
#define __PRINT_INFO_FLAG_i18n					(1<<6)
#define __PRINT_INFO_FLAG_wide					(1<<7)

#define __PRINT_INFO_FLAG_is_char				(1<<8)
#define __PRINT_INFO_FLAG_is_short				(1<<9)
#define __PRINT_INFO_FLAG_is_long				(1<<10)
#define __PRINT_INFO_FLAG_is_long_double		(1<<11)

#define PRINT_INFO_FLAG_VAL(INFO_PTR,BITFIELD) \
	((INFO_PTR)->_flags & __PRINT_INFO_FLAG_##BITFIELD)
#define PRINT_INFO_SET_FLAG(INFO_PTR,BITFIELD) \
	((INFO_PTR)->_flags |= __PRINT_INFO_FLAG_##BITFIELD)
#define PRINT_INFO_CLR_FLAG(INFO_PTR,BITFIELD) \
	((INFO_PTR)->_flags &= ~__PRINT_INFO_FLAG_##BITFIELD)
#define PRINT_INFO_SET_extra(INFO_PTR,VAL) \
	((INFO_PTR)->_flags |= (((INFO_PTR)->_flags & ~1) | ((VAL) & 1)))
    int pad;
};

#define __LOCALE_C_ONLY
#define __UCLIBC_HAS_FLOATS__

typedef double __fpmax_t;
typedef __int64  __ssize_t;		/* Type of a byte count, or error.  */
typedef __ssize_t ssize_t;
typedef unsigned int		uint_fast32_t;

#undef 	DECIMAL_DIG
#define DECIMAL_DIG   (1 + (((FPMAX_MANT_DIG * 100) + 331) / 332))

#define assert
#define unlikely(a)   (a)

#endif  /* __SPRINTF_I__ */

