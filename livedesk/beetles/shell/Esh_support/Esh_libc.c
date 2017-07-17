/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     Some backup function for shell
*
* File    : Esh_libc.c
* By      : sunny
* Version : V1.00                                                                 2009.3.2
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"
#include    <stdarg.h>

/*
 * convert the digits of the given value argument
 * to a null-terminated character string and stores the result
 * (up to 12 bytes) in string
 */
void Esh_itoa(int n, char *str)
{
    eLIBs_int2str_dec(n, str);
}
/*
 * convert string to int digit
 *
 */
int Esh_atoi(const char *s)
{
    int n, sign;

    if ( NULL == s )
    {
        return 0;
    }
    while ( Esh_isspace( *s ) )         //skip space
    {
        s++;
    }
    sign = ( *s == '-' ) ? -1 : 1;                  // get sign

    if( *s == '+' || *s == '-')                     //skip sign
        s++;

    n = 0;
    while ( Esh_isdigit(*s) )
    {
        n = 10 * n + ( *s - '0' ); //convert digit char to digits
        s++;
    }
    return sign * n;
}

__u32 Esh_atou(const char *s)
{
    __u32 n;

    if ( NULL == s )
    {
        return 0;
    }
    while ( Esh_isspace( *s ) )         //skip space
    {
        s++;
    }
    if ( '-' == *s )
    {
        Esh_Error("String:\"%s\" can't convert to __u32\n", s);
        return 0;
    }
    if( *s == '+' )                     //skip sign
        s++;

    n = 0;
    while ( Esh_isdigit(*s) )
    {
        n = 10 * n + ( *s - '0' );     //convert digit char to digits
        s++;
    }
    return n;
}
/*
 * convert string to long long type digit
 */
__int64 Esh_atoll(const char *s)
{
    int sign;
    __int64 n;
    while ( Esh_isspace( *s ) )         //skip space
    {
        s++;
    }
    sign = ( *s == '-' ) ? -1 : 1;                  // get sign

    if( *s == '+' || *s == '-')                     //skip sign
        s++;

    n = 0;
    while ( Esh_isdigit(*s) )
    {
        n = 10 * n + ( *s - '0' );                  //convert char to digit
        s++;
    }
    return sign * n;
}
/*
 * IO functions
 */
void Esh_prs(const char *s)
{
    if (*s)
        Esh_printf("%s", s);
}
void Esh_prc(const char ch)
{
    Esh_printf("%c", ch);
}
//static void prn(unsigned u)
//{
//	prs(Esh_itoa(u));
//}
int Esh_getn(char *as)
{
    char *s;
    int n, m;

    s = as;
    m = 1;
    if (*s == '-')
    {
        m = -1;
        s++;
    }
    for (n = 0; Esh_isdigit(*s); s++)
        n = (n * 10) + (*s - '0');
    if (*s)
    {
        Esh_prs(as);
        Esh_Error(": bad number");
    }
    return n * m;
}
/*
 * Is character c in s?
 */
int Esh_any(int c, const char *s)
{
    while (*s)
        if (*s++ == c)
            return 1;
    return 0;
}

/*
 * Is any character from s1 in s2?
 */
int Esh_anys(const char *s1, const char *s2)
{
    while (*s1)
        if (Esh_any(*s1++, s2))
            return 1;
    return 0;
}

/*
 *	Esh_isdigit ()
 *  cChar is a digit ?
 */

int Esh_isdigit ( int cChar )
{
    // Return true if the character is between 0 and 9 inclusive.
    if ( (cChar >= '0') && (cChar <= '9') )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
 *
 *	Esh_isalnum ()
 *  cChar is a digit or letter ?
 */

int Esh_isalnum ( int cChar )
{
    // Return true if the character is between 0 or 9 inclusive or is an uppercase or
    // lowercase letter
    if ( ( (cChar >= '0') && (cChar <= '9') ) ||
            ( (cChar >= 'A') && (cChar <= 'Z') ) ||
            ( (cChar >= 'a') && (cChar <= 'z') ) )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/*
 *
 *	Esh_isalpha ()
 *  cChar is a letter ?
 */
int Esh_isalpha(int cChar)
{
    // Return true if the character is an uppercase or
    // lowercase letter

    if ( ( (cChar >= 'A') && (cChar <= 'Z') ) ||
            ( (cChar >= 'a') && (cChar <= 'z') ) )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/*
 *	Esh_isspace ()
 *  cChar is a digit ?
 */

int Esh_isspace(int cChar)
{
    // Return true if the character is between 0 and 9 inclusive.
    if ( (cChar == ' ') && (cChar == '\t') )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
void Esh_sprintf( char *buffer, const char *str, ...)
{
    char string[13];
    char *p, *_p = buffer;
    __s32 hex_flag ;
    va_list argp;

    va_start( argp, str );

    while( *str )
    {
        if( *str == '%' )
        {
            ++str;
            p = string;
            hex_flag = HEX_X;
            switch( *str )
            {
            case 'd':
                eLIBs_int2str_dec( va_arg( argp,  __s32 ), string );
                eLIBs_strcpy(_p, p);
                _p += eLIBs_strlen(p);
                ++str;
                break;
            case 'x':
                hex_flag = HEX_x;	         // jump to " case 'X' "
            case 'p':
            case 'X':
                eLIBs_int2str_hex( va_arg( argp,  __s32 ), string, hex_flag );
                eLIBs_strcpy(_p, p);
                _p += eLIBs_strlen(p);
                ++str;
                break;
            case 'u':
                eLIBs_uint2str_dec( va_arg( argp,  __s32 ), string );
                eLIBs_strcpy(_p, p);
                _p += eLIBs_strlen(p);
                ++str;
                break;
            case 'c':
                _p[0] = va_arg( argp,  __s32 );
                _p ++;
                ++str;
                break;
            case 's':
                p =  va_arg( argp,  char * );
                eLIBs_strcpy(_p, p);
                _p += eLIBs_strlen(p);
                ++str;
                break;
            default :
                _p[0] = '%';
                _p ++;
                _p[0] = *str;
                _p ++;
                ++str;                        // let 'str' point to next character.
            }
        }
        else
        {
            _p[0] = *str++;
            _p ++;
        }
    }

    _p[0] = '\0';

    va_end( argp );
}
static char __Esh_ToLower( char c )
{
    if ( (c >= 'A') && (c <= 'Z') )
    {
        return ( c + ('a' - 'A') );
    }
    else
    {
        return c;
    }
}
/* stricmp, _wcsicmp, and _mbsicmp compare strings by first converting them to lowercase forms. */

int Esh_Stricmp(const char *s1, const char *s2)
{
    while ( ('\0' != *s1) && ('\0' != *s2) )
    {

        if ( __Esh_ToLower(*s1++) != __Esh_ToLower(*s2++) )
            return 1;
    }
    return (*s1 - *s2);
}
int Esh_Is_Digit_String(const char *str)
{
    if ( (const char *)NULL == str )
        return 0;

    /* skip whitespace */
    while ( Esh_isspace(*str++) )
        ;

    /* check string */
    while( Esh_isdigit(*str) && (*str != '\0') )
    {
        str++;
    }
    return (*str == '\0');
}
