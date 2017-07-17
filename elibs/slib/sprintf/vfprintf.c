/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               ANSIC Libary
*
*                                    (c) Copyright 2010-2011, Sunny China
*                                             All Rights Reserved
*
* File    : vfprintf.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-11-9
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-9 15:08:01  Sunny       1.0
*********************************************************************************************************
*/

#define WANT_FLOAT_ERROR      0

/**************************************************************************/

#define _ISOC99_SOURCE			/* for ULLONG primarily... */
#define _GNU_SOURCE				/* for strnlen */
#include "stdio.h"
#include <stdarg.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "sprintf_i.h"
#include "_uintmaxtostr.h"
#define __PRINTF_INFO_NO_BITFIELD


/*  #undef __UCLIBC_HAS_FLOATS__ */
/*  #undef WANT_FLOAT_ERROR */
/*  #define WANT_FLOAT_ERROR      1 */

/*  #define __isdigit(c) (((unsigned int)(c - '0')) < 10) */

#define WANT_GNU_ERRNO         0

#undef PUTC
#undef OUTNSTR
#undef _outnstr


typedef struct
{
    unsigned char *bufend;		/* pointer to 1 past end of buffer */
    unsigned char *bufpos;
} __FILE_vsnprintf;



#ifdef __UCLIBC_HAS_FLOATS__
static void _outnstr(__FILE_vsnprintf *stream, const unsigned char *s, size_t n)
{
    __FILE_vsnprintf *f = (__FILE_vsnprintf *) stream;

    if (f->bufend > f->bufpos)
    {
        size_t r = f->bufend - f->bufpos;
        if (r > n)
        {
            r = n;
        }
        memcpy(f->bufpos, s, r);
        f->bufpos += r;
    }
}
#endif

static void putc_unlocked_sprintf(int c, __FILE_vsnprintf *f)
{
    if (f->bufpos < f->bufend)
    {
        *f->bufpos++ = (unsigned char)c;
    }
}


#define PUTC(C,F)      putc_unlocked_sprintf((C),(__FILE_vsnprintf *)(F))
#define OUTNSTR        _outnstr


#ifdef __UCLIBC_HAS_FLOATS__
#include <float.h>
#include "_fpmax.h"

typedef void (__fp_outfunc_t)(__FILE_vsnprintf *fp, intptr_t type, intptr_t len,
                              intptr_t buf);

extern size_t _fpmaxtostr(__FILE_vsnprintf *fp, __fpmax_t x, struct printf_info *info,
                          __fp_outfunc_t fp_outfunc);

static void _charpad(__FILE_vsnprintf *__restrict stream, int padchar, size_t numpad)
{
    /* TODO -- Use a buffer to cut down on function calls... */
    char pad[1];

    *pad = (char)padchar;
    while (numpad)
    {
        OUTNSTR(stream, (const unsigned char *)pad, 1);
        --numpad;
    }
}

static void _fp_out_narrow(__FILE_vsnprintf *fp, intptr_t type, intptr_t len, intptr_t buf)
{
    if (type & 0x80)  			/* Some type of padding needed. */
    {
        int buflen = strlen((const char *) buf);
        if ((len -= buflen) > 0)
        {
            _charpad(fp, (type & 0x7f), len);
        }
        len = buflen;
    }
    if (len)
    {
        OUTNSTR(fp, (const unsigned char *)buf, len);
    }
}

#endif


size_t strnlen (const char *str, size_t maxlen)
{
    maxlen = maxlen;
    return strlen(str);
#if 0
    const char *char_ptr, *end_ptr = str + maxlen;
    const unsigned long int *longword_ptr;
    unsigned long int longword, magic_bits, himagic, lomagic;

    if (maxlen == 0)
        return 0;

    //if (__builtin_expect (end_ptr < str, 0))
    //end_ptr = (const char *) ~0UL;

    /* Handle the first few characters by reading one character at a time.
       Do this until CHAR_PTR is aligned on a longword boundary.  */
    for (char_ptr = str; ((unsigned long int) char_ptr
                          & (sizeof (longword) - 1)) != 0;
            ++char_ptr)
        if (*char_ptr == '\0')
        {
            if (char_ptr > end_ptr)
                char_ptr = end_ptr;
            return char_ptr - str;
        }

    /* All these elucidatory comments refer to 4-byte longwords,
       but the theory applies equally well to 8-byte longwords.  */

    longword_ptr = (unsigned long int *) char_ptr;

    /* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
       the "holes."  Note that there is a hole just to the left of
       each byte, with an extra at the end:

       bits:  01111110 11111110 11111110 11111111
       bytes: AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD

       The 1-bits make sure that carries propagate to the next 0-bit.
       The 0-bits provide holes for carries to fall into.  */
    magic_bits = 0x7efefeffL;
    himagic = 0x80808080L;
    lomagic = 0x01010101L;
    if (sizeof (longword) > 4)
    {
        /* 64-bit version of the magic.  */
        /* Do the shift in two steps to avoid a warning if long has 32 bits.  */
        magic_bits = ((0x7efefefeL << 16) << 16) | 0xfefefeffL;
        himagic = ((himagic << 16) << 16) | himagic;
        lomagic = ((lomagic << 16) << 16) | lomagic;
    }
    if (sizeof (longword) > 8)
        abort ();

    /* Instead of the traditional loop which tests each character,
       we will test a longword at a time.  The tricky part is testing
       if *any of the four* bytes in the longword in question are zero.  */
    while (longword_ptr < (unsigned long int *) end_ptr)
    {
        /* We tentatively exit the loop if adding MAGIC_BITS to
        LONGWORD fails to change any of the hole bits of LONGWORD.

         1) Is this safe?  Will it catch all the zero bytes?
         Suppose there is a byte with all zeros.  Any carry bits
         propagating from its left will fall into the hole at its
         least significant bit and stop.  Since there will be no
         carry from its most significant bit, the LSB of the
         byte to the left will be unchanged, and the zero will be
         detected.

         2) Is this worthwhile?  Will it ignore everything except
         zero bytes?  Suppose every byte of LONGWORD has a bit set
         somewhere.  There will be a carry into bit 8.  If bit 8
         is set, this will carry into bit 16.  If bit 8 is clear,
         one of bits 9-15 must be set, so there will be a carry
         into bit 16.  Similarly, there will be a carry into bit
         24.  If one of bits 24-30 is set, there will be a carry
         into bit 31, so all of the hole bits will be changed.

         The one misfire occurs when bits 24-30 are clear and bit
         31 is set; in this case, the hole at bit 31 is not
         changed.  If we had access to the processor carry flag,
         we could close this loophole by putting the fourth hole
         at bit 32!

         So it ignores everything except 128's, when they're aligned
         properly.  */

        longword = *longword_ptr++;

        if ((longword - lomagic) & himagic)
        {
            /* Which of the bytes was the zero?  If none of them were, it was
               a misfire; continue the search.  */

            const char *cp = (const char *) (longword_ptr - 1);

            char_ptr = cp;
            if (cp[0] == 0)
                break;
            char_ptr = cp + 1;
            if (cp[1] == 0)
                break;
            char_ptr = cp + 2;
            if (cp[2] == 0)
                break;
            char_ptr = cp + 3;
            if (cp[3] == 0)
                break;
            if (sizeof (longword) > 4)
            {
                char_ptr = cp + 4;
                if (cp[4] == 0)
                    break;
                char_ptr = cp + 5;
                if (cp[5] == 0)
                    break;
                char_ptr = cp + 6;
                if (cp[6] == 0)
                    break;
                char_ptr = cp + 7;
                if (cp[7] == 0)
                    break;
            }
        }
        char_ptr = end_ptr;
    }

    if (char_ptr > end_ptr)
        char_ptr = end_ptr;
    return char_ptr - str;
#endif
}

uintmax_t _load_inttype(int desttype, register const void *src, int uflag)
{
    if (uflag >= 0)  			/* unsigned */
    {
#if LONG_MAX != INT_MAX
        if (desttype & (PA_FLAG_LONG | PA_FLAG_LONG_LONG))
        {
#ifdef LLONG_MAX
            if (desttype == PA_FLAG_LONG_LONG)
            {
                return *((unsigned long long int *) src);
            }
#endif
            return *((unsigned long int *) src);
        }
#else  /* LONG_MAX != INT_MAX */
#ifdef LLONG_MAX
        if (desttype & PA_FLAG_LONG_LONG)
        {
            return *((unsigned long long int *) src);
        }
#endif
#endif /* LONG_MAX != INT_MAX */
        {
            unsigned int x;
            x = *((unsigned int *) src);
            if (desttype == __PA_FLAG_CHAR) x = (unsigned char) x;
#if SHRT_MAX != INT_MAX
            if (desttype == PA_FLAG_SHORT) x = (unsigned short int) x;
#endif
            return x;
        }
    }
    else  					/* signed */
    {
#if LONG_MAX != INT_MAX
        if (desttype & (PA_FLAG_LONG | PA_FLAG_LONG_LONG))
        {
#ifdef LLONG_MAX
            if (desttype == PA_FLAG_LONG_LONG)
            {
                return *((long long int *) src);
            }
#endif
            return *((long int *) src);
        }
#else  /* LONG_MAX != INT_MAX */
#ifdef LLONG_MAX
        if (desttype & PA_FLAG_LONG_LONG)
        {
            return *((long long int *) src);
        }
#endif
#endif /* LONG_MAX != INT_MAX */
        {
            int x;
            x = *((int *) src);
            if (desttype == __PA_FLAG_CHAR) x = (char) x;
#if SHRT_MAX != INT_MAX
            if (desttype == PA_FLAG_SHORT) x = (short int) x;
#endif
            return x;
        }
    }
}


enum
{
    FLAG_PLUS = 0,
    FLAG_MINUS_LJUSTIFY,
    FLAG_HASH,
    FLAG_0_PAD,
    FLAG_SPACE,
};

/* layout                   01234  */
static const char spec[] = "+-#0 ";

/**********************************************************************/

extern void _store_inttype(void *dest, int desttype, uintmax_t val);
extern uintmax_t _load_inttype(int desttype, const void *src, int uflag);

/*
 * In order to ease translation to what arginfo and _print_info._flags expect,
 * we map:  0:int  1:char  2:longlong 4:long  8:short
 * and then _flags |= (((q << 7) + q) & 0x701) and argtype |= (_flags & 0x701)
 */

#ifdef PDS
#error PDS already defined!
#endif
#ifdef SS
#error SS already defined!
#endif
#ifdef IMS
#error IMS already defined!
#endif

#define PDS		0

#define SS		0

#define IMS		8
#define QUAL_CHARS		{ \
	/* j:(u)intmax_t z:(s)size_t  t:ptrdiff_t  \0:int */ \
	/* q:long_long  Z:(s)size_t */ \
	'h',   'l',  'L',  'j',  'z',  't',  'q', 'Z',  0, \
	 2,     4,    8,  IMS,   SS,  PDS,    8,  SS,   0, /* TODO -- fix!!! */\
     1,     8 \
}

static const char qual_chars[] = QUAL_CHARS;

/* static const char qual[] = "hlLq"; */
/**********************************************************************/

#if !defined(__UCLIBC_HAS_FLOATS__) && WANT_FLOAT_ERROR
static const char dbl_err[] = "<DOUBLE>";
#endif

#if defined(__UCLIBC_HAS_FLOATS__) || WANT_FLOAT_ERROR
/* layout                     012345678901234567   */
static const char u_spec[] = "%nbopxXudicsfgGeEaA";
#else
/* layout                     0123456789012   */
static const char u_spec[] = "%nbopxXudics";
#endif

/* WARNING: u_spec and u_radix need to stay in agreement!!! */
/* u_radix[i] <-> u_spec[i+2] for unsigned entries only */
static const char u_radix[] = "\x02\x08\x10\x10\x10\x0a";

int SLIB_vfprintf(__FILE_vsnprintf *__restrict op, register const char *__restrict fmt, va_list ap)
{
    union
    {
#ifdef LLONG_MAX
        long long ll;
#endif
#if LONG_MAX != INT_MAX
        long l;
#endif
        int i;
    } intarg;
    int i, cnt, dataargtype, len;
    const void *argptr = NULL;			/* This does not need to be initialized. */
    register char *p;
    const char *fmt0;
    int preci, width;
#define upcase i
    int radix, dpoint /*, upcase*/;
    char tmp[65];				/* TODO - determing needed size from headers */
    char flag[sizeof(spec)];

    cnt = 0;


    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt0 = fmt;			/* save our position in case of bad format */
            ++fmt;
            width = -1;			/* min field width */
            preci = -5;			/* max string width or mininum digits */
            radix = 10;			/* number base */
            dpoint = 0;			/* found decimal point */

            /* init flags */
            for (p = (char *) spec ; *p ; p++)
            {
                flag[p - spec] = '\0';
            }
            flag[FLAG_0_PAD] = ' ';

            /* process optional flags */
            for (p = (char *)spec ; *p ; )
            {
                if (*fmt == *p)
                {
                    flag[p - spec] = *fmt++;
                    p = (char *)spec; /* restart scan */
                }
                else
                {
                    p++;
                }
            }

            if (!flag[FLAG_PLUS])
            {
                flag[FLAG_PLUS] = flag[FLAG_SPACE];
            }

            /* process optional width and precision */
            do
            {
                if (*fmt == '.')
                {
                    ++fmt;
                    dpoint = 1;
                }
                if (*fmt == '*')  	/* parameter width value */
                {
                    ++fmt;
                    i = va_arg(ap, int);
                }
                else
                {
                    for ( i = 0 ; (*fmt >= '0') && (*fmt <= '9') ; ++fmt )
                    {
                        i = (i * 10) + (*fmt - '0');
                    }
                }

                if (dpoint)
                {
                    preci = i;
                    if (i < 0)
                    {
                        preci = -5;
                    }
                }
                else
                {
                    width = i;
                    if (i < 0)
                    {
                        width = -i;
                        flag[FLAG_MINUS_LJUSTIFY] = 1;
                    }
                }
            }
            while ((*fmt == '.') && !dpoint );

            /* process optional qualifier */
            p = (char *) qual_chars;
            do
            {
                if (*fmt == *p)
                {
                    ++fmt;
                    break;
                }
            }
            while (*++p);
            if ((p - qual_chars < 2) && (*fmt == *p))
            {
                p += ((sizeof(qual_chars) - 2) / 2);
                ++fmt;
            }
            dataargtype = ((int)(p[(sizeof(qual_chars) - 2) / 2])) << 8;

#if WANT_GNU_ERRNO
            if (*fmt == 'm')
            {
                flag[FLAG_PLUS] = '\0';
                flag[FLAG_0_PAD] = ' ';
                p = __glibc_strerror_r(errno, tmp, sizeof(tmp));
                goto print;
            }
#endif

            /* process format specifier */
            for (p = (char *) u_spec ; *p ; p++)
            {
                if (*fmt != *p) continue;
                if (p - u_spec < 1)  	/* print a % */
                {
                    goto charout;
                }
                if (p - u_spec < 2)  	/* store output count in int ptr */
                {
                    _store_inttype(va_arg(ap, void *),
                                   dataargtype,
                                   (intmax_t) (cnt));
                    goto nextfmt;
                }

                if (p - u_spec < 10)
                {
                    if (*p == 'p')
                    {
#if INTPTR_MAX == INT_MAX
                        dataargtype = 0;
#else
#error Fix dataargtype for pointers!
#endif
                    }

                    switch(dataargtype)
                    {
                    case (PA_INT|PA_FLAG_LONG_LONG):
#ifdef LLONG_MAX
                        intarg.ll = va_arg(ap, long long);
                        argptr = &intarg.ll;
                        break;
#endif
                    case (PA_INT|PA_FLAG_LONG):
#if LONG_MAX != INT_MAX
                        intarg.l = va_arg(ap, long);
                        argptr = &intarg.l;
                        break;
#endif
                    default:
                        intarg.i = va_arg(ap, int);
                        argptr = &intarg.i;
                        break;
                    }
                }

                if (p - u_spec < 8) /* unsigned conversion */
                {
                    radix = u_radix[p - u_spec - 2];
                    upcase = ((*p == 'x') ? __UIM_LOWER : __UIM_UPPER);
                    if (*p == 'p')
                    {
                        upcase = __UIM_LOWER;
                        flag[FLAG_HASH] = 'p';
                    }
                    p = _uintmaxtostr(tmp + sizeof(tmp) - 1,
                                      (uintmax_t)
                                      _load_inttype(dataargtype, argptr, radix),
                                      radix, (__UIM_CASE)upcase);

                    flag[FLAG_PLUS] = '\0';	/* meaningless for unsigned */
                    if (*p != '0')   /* non-zero */
                    {
                        if (flag[FLAG_HASH])
                        {
                            if (radix == 8)
                            {
                                *--p = '0';	/* add leadding zero */
                            }
                            else if (radix != 10)     /* either 2 or 16 */
                            {
                                flag[FLAG_PLUS] = '0';
                                *--p = 'b';
                                if (radix == 16)
                                {
                                    *p = 'x';
                                    if (*fmt == 'X')
                                    {
                                        *p = 'X';
                                    }
                                }
                            }
                        }
                    }
                    else if (flag[FLAG_HASH] == 'p')     /* null pointer */
                    {
                        p = "(nil)";
                    }
                }
                else if (p - u_spec < 10)   /* signed conversion */
                {
                    p = _uintmaxtostr(tmp + sizeof(tmp) - 1,
                                      (uintmax_t)
                                      _load_inttype(dataargtype, argptr, -radix),
                                      -radix, (__UIM_CASE)upcase);

                }
                else if (p - u_spec < 12)  	/* character or string */
                {
                    flag[FLAG_PLUS] = '\0';
                    flag[FLAG_0_PAD] = ' ';
                    if (*p == 'c')  	/* character */
                    {
                        p = tmp;
                        *p = (char)va_arg(ap, int);
                        /* This takes care of the "%c",0 case */
                        len = 1;
                        goto print_len_set;
                    }
                    else  	/* string */
                    {
                        p = va_arg(ap, char *);
                        if (!p)
                        {
                            p = "(null)";
                            preci = 6;
                        }
                        else
                        {
                            if (preci < 0)
                            {
                                preci = INT_MAX;
                            }
                        }
                        len = strnlen(p, preci);
                        goto print_len_set;
                    }
#if defined(__UCLIBC_HAS_FLOATS__) || WANT_FLOAT_ERROR
                }
                else if (p - u_spec < 27)  		/* floating point */
                {
#endif /* defined(__UCLIBC_HAS_FLOATS__) || WANT_FLOAT_ERROR */
#if defined(__UCLIBC_HAS_FLOATS__)
                    struct printf_info info;
                    if (preci < 0)
                    {
                        preci = 6;
                    }
                    info.width = width;
                    info.prec = preci;
                    info.spec = *fmt;
                    info.pad = flag[FLAG_0_PAD];
                    info._flags = 0;
                    if (flag[FLAG_PLUS] == '+')
                    {
                        PRINT_INFO_SET_FLAG(&info, showsign);
                    }
                    else if (flag[FLAG_PLUS] == ' ')
                    {
                        PRINT_INFO_SET_FLAG(&info, space);
                    }
                    if (flag[FLAG_HASH])
                    {
                        PRINT_INFO_SET_FLAG(&info, alt);
                    }
                    if (flag[FLAG_MINUS_LJUSTIFY])
                    {
                        PRINT_INFO_SET_FLAG(&info, left);
                    }
#if 1
                    cnt += _fpmaxtostr(op,
                                       (__fpmax_t)
                                       ((dataargtype == (8 << 8))
                                        ? va_arg(ap, long double)
                                        : (long double) va_arg(ap, double)),
                                       &info, _fp_out_narrow);
#else
                    cnt += _fpmaxtostr(op,
                                       (__fpmax_t)
                                       ((lval > 1)
                                        ? va_arg(ap, long double)
                                        : (long double) va_arg(ap, double)),
                                       &info, _fp_out_narrow);
#endif
                    goto nextfmt;
#elif WANT_FLOAT_ERROR
                    (void) ((lval > 1) ? va_arg(ap, long double)
                            : va_arg(ap, double)); /* carry on */
                    p = (char *) dbl_err;
#endif /* defined(__UCLIBC_HAS_FLOATS__) */
                }

#if WANT_GNU_ERRNO
print:
#endif
                {
                    /* this used to be printfield */
                    /* cheaper than strlen call */
                    /*  					for ( len = 0 ; p[len] ; len++ ) { } */
                    len = strnlen(p, SIZE_MAX);
print_len_set:
                    if ((*p == '-')
#if WANT_GNU_ERRNO
                            && (*fmt != 'm')
#endif
                            && (*fmt != 's'))
                    {
                        flag[FLAG_PLUS] = *p++;
                        --len;
                    }
                    if (flag[FLAG_PLUS])
                    {
                        ++len;
                        ++preci;
                        if (flag[FLAG_PLUS] == '0')   /* base 16 */
                        {
                            ++preci; /* account for x or X */
                        }
                    }

                    if (preci >= 0)
                    {
                        if ((*fmt == 's')
#if WANT_GNU_ERRNO
                                || (*fmt == 'm')
#endif
                           )
                        {
                            if (len > preci)
                            {
                                len = preci;
                            }
                            else
                            {
                                preci = len;
                            }
                        }
                        preci -= len;
                        if (preci < 0)
                        {
                            preci = 0;
                        }
                        width -= preci;
                    }

                    width -= len;
                    if (width < 0)
                    {
                        width = 0;
                    }

                    if (preci < 0)
                    {
                        preci = 0;
                        if (!flag[FLAG_MINUS_LJUSTIFY]
                                /* && flag[FLAG_PLUS] */
                                && (flag[FLAG_0_PAD] == '0'))
                        {
                            preci = width;
                            width = 0;
                        }
                    }

                    while (width + len + preci)
                    {
                        unsigned char ch;
                        /* right padding || left padding */
                        if ((!len && !preci)
                                || (width && !flag[FLAG_MINUS_LJUSTIFY]))
                        {
                            ch = ' ';
                            --width;
                        }
                        else if (flag[FLAG_PLUS])
                        {
                            ch = flag[FLAG_PLUS]; /* sign */
                            if (flag[FLAG_PLUS] == '0')  	/* base 16 case */
                            {
                                flag[FLAG_PLUS] = *p++;	/* get the x|X */
                            }
                            else
                            {
                                flag[FLAG_PLUS] = '\0';
                            }
                            --len;
                        }
                        else if (preci)
                        {
                            ch = '0';
                            --preci;
                        }
                        else
                        {
                            ch = *p++; /* main field */
                            --len;
                        }
                        ++cnt;
                        PUTC(ch, op);
                    }
                }
                goto nextfmt;
            }

            fmt = fmt0;	/* this was an illegal format */
        }

charout:
        ++cnt;
        PUTC(*fmt, op); /* normal char out */

nextfmt:
        ++fmt;
    }

    i = cnt;
    return i;
}
