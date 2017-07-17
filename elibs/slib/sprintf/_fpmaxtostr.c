/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               ANSIC Libary
*
*                                    (c) Copyright 2010-2011, Sunny China
*                                             All Rights Reserved
*
* File    : _fpmaxtostr.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-11-9
* Descript:
* Update  : date                auther      ver     notes
*           2020-11-9 15:05:01  Sunny       1.0
*********************************************************************************************************
*/

#include "stdio.h"
#include <float.h>
#include <locale.h>
#include "sprintf_i.h"
#include "_fpmax.h"
#include "string.h"


typedef size_t (__fp_outfunc_t)(FILE *fp, intptr_t type, intptr_t len,
                                intptr_t buf);

/*****************************************************************************/
/* Don't change anything that follows unless you know what you're doing.     */
/*****************************************************************************/
/* Fairly portable nan check.  Bitwise for i386 generated larger code.
 * If you have a better version, comment this out.
 */
#define isnan(x)             ((x) != (x))

/* Without seminumerical functions to examine the sign bit, this is
 * about the best we can do to test for '-0'.
 */
#define zeroisnegative(x)    ((1./(x)) < 0)

/*****************************************************************************/
/* Don't change anything that follows peroid!!!  ;-)                         */
/*****************************************************************************/

#define NUM_HEX_DIGITS      ((FPMAX_MANT_DIG + 3)/ 4)

/* WARNING: Adjust _fp_out_wide() below if this changes! */
/* With 32 bit ints, we can get 9 decimal digits per block. */
#define DIGITS_PER_BLOCK     9
#define HEX_DIGITS_PER_BLOCK 8

/* Maximum number of subcases to output double is...
 *  0 - sign
 *  1 - padding and initial digit
 *  2 - digits left of the radix
 *  3 - 0s left of the radix        or   radix
 *  4 - radix                       or   digits right of the radix
 *  5 - 0s right of the radix
 *  6 - exponent
 *  7 - trailing space padding
 * although not all cases may occur.
 */
#define MAX_CALLS 8

/*****************************************************************************/

#define NUM_DIGIT_BLOCKS   ((DECIMAL_DIG+DIGITS_PER_BLOCK-1)/DIGITS_PER_BLOCK)
#define NUM_HEX_DIGIT_BLOCKS \
   ((NUM_HEX_DIGITS+HEX_DIGITS_PER_BLOCK-1)/HEX_DIGITS_PER_BLOCK)

/* WARNING: Adjust _fp_out_wide() below if this changes! */

/* extra space for '-', '.', 'e+###', and nul */
#define BUF_SIZE  ( 3 + NUM_DIGIT_BLOCKS * DIGITS_PER_BLOCK )

/*****************************************************************************/

static const char fmt[] = "inf\0INF\0nan\0NAN\0.\0,";

#define INF_OFFSET        0		/* must be 1st */
#define NAN_OFFSET        8		/* must be 2nd.. see hex sign handling */
#define DECPT_OFFSET     16
#define THOUSEP_OFFSET   18

#define EMPTY_STRING_OFFSET 3

/*****************************************************************************/
#if FPMAX_MAX_10_EXP < -FPMAX_MIN_10_EXP
#error scaling code can not handle FPMAX_MAX_10_EXP < -FPMAX_MIN_10_EXP
#endif

static const __fpmax_t exp10_table[] =
{
    1e1L, 1e2L, 1e4L, 1e8L, 1e16L, 1e32L,	/* floats */
#if FPMAX_MAX_10_EXP < 32
#error unsupported FPMAX_MAX_10_EXP (< 32).  ANSI/ISO C requires >= 37.
#endif
#if FPMAX_MAX_10_EXP >= 64
    1e64L,
#endif
#if FPMAX_MAX_10_EXP >= 128
    1e128L,
#endif
#if FPMAX_MAX_10_EXP >= 256
    1e256L,
#endif
#if FPMAX_MAX_10_EXP >= 512
    1e512L,
#endif
#if FPMAX_MAX_10_EXP >= 1024
    1e1024L,
#endif
#if FPMAX_MAX_10_EXP >= 2048
    1e2048L,
#endif
#if FPMAX_MAX_10_EXP >= 4096
    1e4096L
#endif
#if FPMAX_MAX_10_EXP >= 8192
#error unsupported FPMAX_MAX_10_EXP.  please increase table
#endif
};

#define EXP10_TABLE_SIZE     (sizeof(exp10_table)/sizeof(exp10_table[0]))
#define EXP10_TABLE_MAX      (1U<<(EXP10_TABLE_SIZE-1))

/*****************************************************************************/

/*****************************************************************************/

#define FPO_ZERO_PAD    (0x80 | '0')
#define FPO_STR_WIDTH   (0x80 | ' ');
#define FPO_STR_PREC    'p'

ssize_t _fpmaxtostr(FILE *fp, __fpmax_t x, struct printf_info *info,
                    __fp_outfunc_t fp_outfunc)
{
    int round, o_exp;
    int exp;
    int width, preci;
    int cnt;
    char *s;
    char *e;
    intptr_t pc_fwi[3 * MAX_CALLS];
    intptr_t *ppc;
    intptr_t *ppc_last;
    char exp_buf[16];
    char buf[BUF_SIZE];
    char sign_str[6];			/* Last 2 are for 1st digit + nul. */
    char o_mode;
    char mode;


    width = info->width;
    preci = info->prec;
    mode = (char)(info->spec);

    *exp_buf = 'e';
    if ((mode | 0x20) == 'a')
    {
        mode += ('g' - 'a');
    }

    if (preci < 0)
    {
        preci = 6;
    }

    *sign_str = '\0';
    if (PRINT_INFO_FLAG_VAL(info, showsign))
    {
        *sign_str = '+';
    }
    else if (PRINT_INFO_FLAG_VAL(info, space))
    {
        *sign_str = ' ';
    }

    *(sign_str + 1) = 0;
    pc_fwi[5] = INF_OFFSET;
    if (isnan(x))  				/* First, check for nan. */
    {
        pc_fwi[5] = NAN_OFFSET;
        goto INF_NAN;
    }

    if (x == 0)  				/* Handle 0 now to avoid false positive. */
    {
#if 1
        if (zeroisnegative(x))   /* Handle 'signed' zero. */
        {
            *sign_str = '-';
        }
#endif
        exp = -1;
        goto GENERATE_DIGITS;
    }

    if (x < 0)  				/* Convert negatives to positives. */
    {
        *sign_str = '-';
        x = -x;
    }

    if (__FPMAX_ZERO_OR_INF_CHECK(x))  	/* Inf since zero handled above. */
    {
INF_NAN:
        info->pad = ' ';
        ppc = pc_fwi + 6;
        pc_fwi[3] = FPO_STR_PREC;
        pc_fwi[4] = 3;
        if (mode < 'a')
        {
            pc_fwi[5] += 4;
        }
        pc_fwi[5] = (intptr_t)(fmt + pc_fwi[5]);
        goto EXIT_SPECIAL;
    }

    {
        int i, j;


#define lower_bnd    1e8
#define upper_bnd    1e9
#define power_table  exp10_table
#define dpb          DIGITS_PER_BLOCK
#define base         10
#define ndb          NUM_DIGIT_BLOCKS
#define nd           DECIMAL_DIG

        exp = DIGITS_PER_BLOCK - 1;
        i = EXP10_TABLE_SIZE;
        j = EXP10_TABLE_MAX;


        {
            int exp_neg = 0;
            if (x < lower_bnd)   /* Do we need to scale up or down? */
            {
                exp_neg = 1;
            }

            do
            {
                --i;
                if (exp_neg)
                {
                    if (x * power_table[i] < upper_bnd)
                    {
                        x *= power_table[i];
                        exp -= j;
                    }
                }
                else
                {
                    if (x / power_table[i] >= lower_bnd)
                    {
                        x /= power_table[i];
                        exp += j;
                    }
                }
                j >>= 1;
            }
            while (i);
        }
    }
    if (x >= upper_bnd)  		/* Handle bad rounding case. */
    {
        x /= power_table[0];
        ++exp;
    }
    //assert(x < upper_bnd);

GENERATE_DIGITS:
    {
        int i, j;
        s = buf + 2;			/* Leave space for '\0' and '0'. */
        i = 0;
        do
        {
            uint_fast32_t digit_block = (uint_fast32_t) x;
            //assert(digit_block < upper_bnd);
            x = (x - digit_block) * upper_bnd;
            s += dpb;
            j = 0;
            do
            {
                s[- ++j] = (char)('0' + (digit_block % base));
                digit_block /= base;
            }
            while (j < dpb);
        }
        while (++i < ndb);
    }

    /*************************************************************************/

    if (mode < 'a')
    {
        *exp_buf -= ('a' - 'A'); /* e->E and p->P */
        mode += ('a' - 'A');
    }

    o_mode = mode;
    if ((mode == 'g') && (preci > 0))
    {
        --preci;
    }
    round = preci;

    if (mode == 'f')
    {
        round += exp;
        if (round < -1)
        {
            memset(buf, '0', DECIMAL_DIG); /* OK, since 'f' -> decimal case. */
            exp = -1;
            round = -1;
        }
    }

    s = buf;
    *s++ = 0;					/* Terminator for rounding and 0-triming. */
    *s = '0';					/* Space to round. */

    {
        int i;
        i = 0;
        e = s + nd + 1;
        if (round < nd)
        {
            e = s + round + 2;
            if (*e >= '0' + (base / 2))  	/* NOTE: We always round away from 0! */
            {
                i = 1;
            }
        }

        do  			   /* Handle rounding and trim trailing 0s. */
        {
            *--e += (char)(i);			/* Add the carry. */
        }
        while ((*e == '0') || (*e > '0' - 1 + base));
    }


    o_exp = exp;
    if (e <= s)  				/* We carried into an extra digit. */
    {
        ++o_exp;
        e = s;					/* Needed if all 0s. */
    }
    else
    {
        ++s;
    }
    *++e = 0;					/* Terminating nul char. */

    if ((mode == 'g') && ((o_exp >= -4) && (o_exp <= round)))
    {
        mode = 'f';
        preci = round - o_exp;
    }

    exp = o_exp;
    if (mode != 'f')
    {
        o_exp = 0;
    }

    if (o_exp < 0)  			/* Exponent is < 0, so */
    {
        *--s = '0';				/* fake the first 0 digit. */
    }

    pc_fwi[3] = FPO_ZERO_PAD;
    pc_fwi[4] = 1;
    pc_fwi[5] = (intptr_t)(sign_str + 4);
    sign_str[4] = *s++;
    sign_str[5] = 0;
    ppc = pc_fwi + 6;

    {
        int i = e - s;			/* Total digits is 'i'. */
        if (o_exp >= 0)
        {
            ppc[0] = FPO_STR_PREC;
            ppc[2] = (intptr_t)(s);
            if (o_exp >= i)  		/* all digit(s) left of decimal */
            {
                ppc[1] = i;
                ppc += 3;
                o_exp -= i;
                i = 0;
                if (o_exp > 0)  		/* have 0s left of decimal */
                {
                    ppc[0] = FPO_ZERO_PAD;
                    ppc[1] = o_exp;
                    ppc[2] = (intptr_t)(fmt + EMPTY_STRING_OFFSET);
                    ppc += 3;
                }
            }
            else if (o_exp > 0)  	/* decimal between digits */
            {
                ppc[1] = o_exp;
                ppc += 3;
                s += o_exp;
                i -= o_exp;
            }
            o_exp = -1;
        }

        if (PRINT_INFO_FLAG_VAL(info, alt)
                || (i)
                || ((o_mode != 'g')
                    && (preci > 0))
           )
        {
            ppc[0] = FPO_STR_PREC;
#ifdef __LOCALE_C_ONLY
            ppc[1] = 1;
            ppc[2] = (intptr_t)(fmt + DECPT_OFFSET);
#else  /* __LOCALE_C_ONLY */
            ppc[1] = __UCLIBC_CURLOCALE_DATA.decimal_point_len;
            ppc[2] = (intptr_t)(__UCLIBC_CURLOCALE_DATA.decimal_point);
#endif /* __LOCALE_C_ONLY */
            ppc += 3;
        }

        if (++o_exp < 0)  			/* Have 0s right of decimal. */
        {
            ppc[0] = FPO_ZERO_PAD;
            ppc[1] = -o_exp;
            ppc[2] = (intptr_t)(fmt + EMPTY_STRING_OFFSET);
            ppc += 3;
        }
        if (i)  					/* Have digit(s) right of decimal. */
        {
            ppc[0] = FPO_STR_PREC;
            ppc[1] = i;
            ppc[2] = (intptr_t)(s);
            ppc += 3;
        }

        if (((o_mode != 'g') || PRINT_INFO_FLAG_VAL(info, alt))
           )
        {
            i -= o_exp;
            if (i < preci)  		/* Have 0s right of digits. */
            {
                i = preci - i;
                ppc[0] = FPO_ZERO_PAD;
                ppc[1] = i;
                ppc[2] = (intptr_t)(fmt + EMPTY_STRING_OFFSET);
                ppc += 3;
            }
        }
    }

    /* Build exponent string. */
    if (mode != 'f')
    {
        char *p = exp_buf + sizeof(exp_buf);
        int j;
        char exp_char = *exp_buf;
        char exp_sign = '+';
#define min_exp_dig_plus_2  (2+2)

        if (exp < 0)
        {
            exp_sign = '-';
            exp = -exp;
        }

        *--p = 0;			/* nul-terminate */
        j = 2;				/* Count exp_char and exp_sign. */
        do
        {
            *--p = (char)('0' + (exp % 10));
            exp /= 10;
        }
        while ((++j < min_exp_dig_plus_2) || exp);   /* char+sign+mindigits */
        *--p = exp_sign;
        *--p = exp_char;

        ppc[0] = FPO_STR_PREC;
        ppc[1] = j;
        ppc[2] = (intptr_t)(p);
        ppc += 3;
    }

EXIT_SPECIAL:
    {
        int i;
        ppc_last = ppc;
        ppc = pc_fwi + 4;	 /* Need width fields starting with second. */
        do
        {
            width -= *ppc;
            ppc += 3;
        }
        while (ppc < ppc_last);

        ppc = pc_fwi;
        ppc[0] = FPO_STR_WIDTH;
        ppc[1] = i = ((*sign_str) != 0);
        ppc[2] = (intptr_t) sign_str;


        if ((width -= i) > 0)
        {
            if (PRINT_INFO_FLAG_VAL(info, left))  /* Left-justified. */
            {
                ppc_last[0] = FPO_STR_WIDTH;
                ppc_last[1] = width;
                ppc_last[2] = (intptr_t)(fmt + EMPTY_STRING_OFFSET);
                ppc_last += 3;
            }
            else if (info->pad == '0')     /* 0 padding */
            {
                ppc[4] += width;	/* Pad second field. */
            }
            else
            {
                ppc[1] += width;	/* Pad first (sign) field. */
            }
        }

        cnt = 0;
    }

    do
    {
        {
            /* NOTE: Remember 'else' above! */
            //if (fp_outfunc(fp, *ppc, ppc[1], ppc[2]) != ppc[1]) {
            //	return -1;
            fp_outfunc(fp, *ppc, ppc[1], ppc[2]);
            //	}
        }

        cnt += ppc[1];
        ppc += 3;
    }
    while (ppc < ppc_last);

    return cnt;
}
