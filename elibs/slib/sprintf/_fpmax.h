/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               ANSIC Libary
*
*                                    (c) Copyright 2010-2011, Sunny China
*                                             All Rights Reserved
*
* File    : _fpmax.h
* By      : Sunny
* Version : v1.0
* Date    : 2010-11-9
* Descript:
* Update  : date                auther      ver     notes
*           2020-11-9 15:05:01  Sunny       1.0
*********************************************************************************************************
*/

#ifndef __FPMAX_H__
#define __FPMAX_H__

#ifndef _ISOC99_SOURCE
#define _ISOC99_SOURCE 1
#endif

#include <float.h>

#ifdef __UCLIBC_HAS_FLOATS__

#if defined(LDBL_MANT_DIG)
#ifdef PC_SIMULATOR
typedef long double __fpmax_t;
#endif
#define FPMAX_TYPE           3

#define FPMAX_MANT_DIG       LDBL_MANT_DIG
#define FPMAX_DIG            LDBL_DIG
#define FPMAX_EPSILON        LDBL_EPSILON
#define FPMAX_MIN_EXP        LDBL_MIN_EXP
#define FPMAX_MIN            LDBL_MIN
#define FPMAX_MIN_10_EXP     LDBL_MIN_10_EXP
#define FPMAX_MAX_EXP        LDBL_MAX_EXP
#define FPMAX_MAX            LDBL_MAX
#define FPMAX_MAX_10_EXP     LDBL_MAX_10_EXP

#elif defined(DBL_MANT_DIG)

typedef double __fpmax_t;
#define FPMAX_TYPE           2

#define FPMAX_MANT_DIG       DBL_MANT_DIG
#define FPMAX_DIG            DBL_DIG
#define FPMAX_EPSILON        DBL_EPSILON
#define FPMAX_MIN_EXP        DBL_MIN_EXP
#define FPMAX_MIN            DBL_MIN
#define FPMAX_MIN_10_EXP     DBL_MIN_10_EXP
#define FPMAX_MAX_EXP        DBL_MAX_EXP
#define FPMAX_MAX            DBL_MAX
#define FPMAX_MAX_10_EXP     DBL_MAX_10_EXP

#elif defined(FLT_MANT_DIG)

typedef float __fpmax_t;
#define FPMAX_TYPE           1

#define FPMAX_MANT_DIG       FLT_MANT_DIG
#define FPMAX_DIG            FLT_DIG
#define FPMAX_EPSILON        FLT_EPSILON
#define FPMAX_MIN_EXP        FLT_MIN_EXP
#define FPMAX_MIN            FLT_MIN
#define FPMAX_MIN_10_EXP     FLT_MIN_10_EXP
#define FPMAX_MAX_EXP        FLT_MAX_EXP
#define FPMAX_MAX            FLT_MAX
#define FPMAX_MAX_10_EXP     FLT_MAX_10_EXP

#else
#error unable to determine appropriate type for __fpmax_t!
#endif

#ifndef DECIMAL_DIG

#ifdef L___strtofpmax
/* Emit warning only once. */
#warning DECIMAL_DIG is not defined! If you are using gcc, it may not be defining __STDC_VERSION__ as it should.
#endif
#if !defined(FLT_RADIX) || (FLT_RADIX != 2)
#error unable to compensate for missing DECIMAL_DIG!
#endif

/*  ceil (1 + #mantissa * log10 (FLT_RADIX)) */
#define DECIMAL_DIG   (1 + (((FPMAX_MANT_DIG * 100) + 331) / 332))

#endif /* DECIMAL_DIG */

extern __fpmax_t __strtofpmax(const char *str, char **endptr, int exp_adjust);

#ifdef __UCLIBC_HAS_XLOCALE__
extern __fpmax_t __strtofpmax_l(const char *str, char **endptr, int exp_adjust,
                                __locale_t locale_arg);
#endif

#ifdef __UCLIBC_HAS_WCHAR__
extern __fpmax_t __wcstofpmax(const wchar_t *wcs, wchar_t **endptr,
                              int exp_adjust);

#ifdef __UCLIBC_HAS_XLOCALE__
extern __fpmax_t __wcstofpmax_l(const wchar_t *wcs, wchar_t **endptr,
                                int exp_adjust, __locale_t locale_arg);
#endif
#endif /* __UCLIBC_HAS_WCHAR__ */

/* The following checks in an __fpmax_t is either 0 or +/- infinity.
 *
 * WARNING!!!   WARNING!!!   WARNING!!!   WARNING!!!   WARNING!!!   WARNING!!!
 *
 * This only works if __fpmax_t is the actual maximal floating point type used
 * in intermediate calculations.  Otherwise, excess precision in the
 * intermediate values can cause the test to fail.
 *
 * WARNING!!!   WARNING!!!   WARNING!!!   WARNING!!!   WARNING!!!   WARNING!!!
 */

#define __FPMAX_ZERO_OR_INF_CHECK(x)  ((x) == ((x)/4) )

#endif /* __UCLIBC_HAS_FLOATS__ */

#endif /* __FPMAX_H__ */
