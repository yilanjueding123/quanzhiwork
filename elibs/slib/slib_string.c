/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Share Libary Module
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : slib_string.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-21
* Descript: shared libary module string functions.
* Update  : date                auther      ver     notes
*           2011-3-21 18:55:23  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "slib_i.h"

/*
**********************************************************************************************************************
*                                              SLIB_strlen
*
* Description : Calculate length of a string.
*
* Argument(s) : pstr        Pointer to string (see Note #1).
*
* Return(s)   : Length of string; number of characters in string before terminating NULL character.
*
* Caller(s)   : various.
*
* Note(s)     : (1) String buffer NOT modified.
*
*               (2) String length calculation terminates when :
*
*                   (a) String pointer points to NULL.
*                       (1) String buffer overlaps with NULL address.
*                       (2) String length calculated for string up to but NOT beyond or including
*                           the NULL address.
*
*                   (b) Terminating NULL character found.
*                       (1) String length calculated for string up to but NOT           including
*                           the NULL character.
**********************************************************************************************************************
*/
__size  SLIB_strlen(const char  *pstr)
{
    __size  len;

    len = 0;

    while (( pstr != (char *)0) &&              /* Calc str len until NULL ptr (see Note #2a) ...               */
            (*pstr != (char  )0))                /* ... or NULL char found      (see Note #2b).                  */
    {
        len++;
        pstr++;
    }

    return (len);
}

/*
**********************************************************************************************************************
*                                             SLIB_strcpy
*
* Description : Copy source string to destination string buffer.
*
* Argument(s) : pdest       Pointer to destination string buffer to receive source string copy (see Note #1).
*
*               psrc        Pointer to source      string to copy into destination string buffer.
*
* Return(s)   : Pointer to destination string, if NO errors (see Note #2).
*
*               Pointer to NULL,               otherwise.
*
* Caller(s)   : various.
*
* Note(s)     : (1) Destination buffer size NOT validated; buffer overruns MUST be prevented by caller.
*
*                   (a) Destination buffer size MUST be large enough to accommodate the entire source
*                       string size including the terminating NULL character.
*
*               (2) String copy terminates when :
*
*                   (a) Destination/Source string pointer(s) are passed NULL pointers.
*                       (1) No string copy performed; NULL pointer returned.
*
*                   (b) Destination/Source string pointer(s) points to NULL.
*                       (1) String buffer(s) overlap with NULL address.
*                       (2) Source string copied into destination string buffer up to but NOT beyond or
*                           including the NULL address; destination string buffer properly terminated
*                           with NULL character.
*
*                   (c) Source string's terminating NULL character found.
*                       (1) Entire source string copied into destination string buffer.
**********************************************************************************************************************
*/
char *SLIB_strcpy(char *pdest, const char *psrc)
{
    char  *pstr;
    char  *pstr_next;

    /* Rtn NULL if str ptr(s) NULL (see Note #2a).                  */
    if (pdest == (char *)0)
    {
        return  ((char *)0);
    }
    if (psrc  == (char *)0)
    {
        return  ((char *)0);
    }


    pstr      = pdest;
    pstr_next = pstr;
    pstr_next++;
    while (( pstr_next != (char *)0) &&         /* Copy str until NULL ptr(s) (see Note #2b) ...                */
            ( psrc      != (char *)0) &&
            (*psrc      != (char  )0))           /* ... or NULL char found     (see Note #2c).                   */
    {
        *pstr = *psrc;
        pstr++;
        pstr_next++;
        psrc++;
    }

    *pstr = (char)0;                             /* Append NULL char (see Note #2b2).                            */


    return (pdest);
}

/*
**********************************************************************************************************************
*                                            SLIB_strncpy
*
* Description : Copy source string to destination string buffer, up to a maximum number of characters.
*
* Argument(s) : pdest       Pointer to destination string buffer to receive source string copy (see Note #1).
*
*               psrc        Pointer to source      string to copy into destination string buffer.
*
*               len_max     Maximum number of characters to copy (see Note #2d).
*
* Return(s)   : Pointer to destination string, if NO errors (see Note #2).
*
*               Pointer to NULL,               otherwise.
*
* Caller(s)   : various.
*
* Note(s)     : (1) Destination buffer size NOT validated; buffer overruns MUST be prevented by caller.
*
*                   (a) Destination buffer size MUST be large enough to accommodate the entire source
*                       string size including the terminating NULL character.
*
*               (2) String copy terminates when :
*
*                   (a) Destination/Source string pointer(s) are passed NULL pointers.
*                       (1) No string copy performed; NULL pointer returned.
*
*                   (b) Destination/Source string pointer(s) points to NULL.
*                       (1) String buffer(s) overlap with NULL address.
*                       (2) Source string copied into destination string buffer up to but NOT beyond or
*                           including the NULL address; destination string buffer properly terminated
*                           with NULL character.
*
*                   (c) Source string's terminating NULL character found.
*                       (1) Entire source string copied into destination string buffer.
*
*                   (d) 'len_max' number of characters copied.
*                       (1) 'len_max' number of characters does NOT include the terminating NULL character.
*
*                           See also Note #1a.
**********************************************************************************************************************
*/
char *SLIB_strncpy(char *pdest, const char *psrc, __size len_max)
{
    char         *pstr;
    char         *pstr_next;
    __size_t      len_copy;

    /* Rtn NULL if str ptr(s) NULL      (see Note #2a).             */
    if (pdest == (char *)0)
    {
        return  ((char *)0);
    }
    if (psrc  == (char *)0)
    {
        return  ((char *)0);
    }

    if (len_max == (__size)0)                       /* Rtn NULL if copy len equals zero (see Note #2d).             */
    {
        return  ((char *)0);
    }


    pstr      = pdest;
    pstr_next = pstr;
    pstr_next++;
    len_copy  = 0;

    while (( pstr_next != (char *)0) &&             /* Copy str until NULL ptr(s)  (see Note #2b)  ...              */
            ( psrc      != (char *)0) &&
            (*psrc      != (char  )0) &&             /* ... or NULL char found      (see Note #2c); ...              */
            ( len_copy  <  (__size_t)len_max))       /* ... or max nbr chars copied (see Note #2d).                  */
    {
        *pstr = *psrc;
        pstr++;
        pstr_next++;
        psrc++;
        len_copy++;
    }

    *pstr = (char)0;                                 /* Append NULL char (see Note #2b2).                            */

    return (pdest);
}

/*
**********************************************************************************************************************
*                                              SLIB_strcat
*
* Description : Append concatenation string to destination string.
*
* Argument(s) : pdest       Pointer to destination   string to append concatenation  string (see Note #1).
*
*               pstr_cat    Pointer to concatenation string to append to destination string.
*
* Return(s)   : Pointer to destination string, if NO errors (see Note #2).
*
*               Pointer to NULL,               otherwise.
*
* Caller(s)   : various.
*
* Note(s)     : (1) Destination string buffer size NOT validated; buffer overruns MUST be prevented by caller.
*
*                   (a) Destination buffer size MUST be large enough to accommodate the entire concatenated
*                       string size including the terminating NULL character.
*
*               (2) String concatenation terminates when :
*
*                   (a) Destination/Concatenation string pointer(s) are passed NULL pointers.
*                       (1) No string concatenation performed; NULL pointer returned.
*
*                   (b) Destination string overlaps with NULL address.
*                       (1) No string concatenation performed; NULL pointer returned.
*
*                   (c) Destination/Concatenation string pointer(s) points to NULL.
*                       (1) String buffer(s) overlap with NULL address.
*                       (2) Concatenation string appended into destination string buffer up to but NOT
*                           beyond or including the NULL address; destination string buffer properly
*                           terminated with NULL character.
*
*                   (d) Concatenation string's terminating NULL character found.
*                       (1) Entire concatenation string appended to destination string.
**********************************************************************************************************************
*/
char *SLIB_strcat(char *pdest, const char *pstr_cat)
{
    char     *pstr;
    char     *pstr_next;

    /* Rtn NULL if str ptr(s) NULL (see Note #2a).                  */
    if (pdest == (char *)0)
    {
        return  ((char *)0);
    }
    if (pstr_cat == (char *)0)
    {
        return  ((char *)0);
    }


    pstr = pdest;
    while (( pstr != (char *)0) &&                  /* Adv to end of cur dest str until NULL ptr ...                */
            (*pstr != (char  )0))                    /* ... or NULL char found..                                     */
    {
        pstr++;
    }
    if (pstr == (char *)0)                          /* If NULL str overrun, rtn NULL (see Note #2b).                */
    {
        return ((char *)0);
    }

    pstr_next = pstr;
    pstr_next++;
    while (( pstr_next != (char *)0) &&             /* Cat str until NULL ptr(s) (see Note #2c) ...                 */
            ( pstr_cat  != (char *)0) &&
            (*pstr_cat  != (char  )0))               /* ... or NULL char found    (see Note #2d).                    */
    {
        *pstr = *pstr_cat;
        pstr++;
        pstr_next++;
        pstr_cat++;
    }

    *pstr = (char)0;                                 /* Append NULL char (see Note #2c2).                            */


    return (pdest);
}

/*
**********************************************************************************************************************
*                                             SLIB_strncat
*
* Description : Append concatenation string to destination string, up to a maximum number of characters.
*
* Argument(s) : pdest       Pointer to destination   string to append concatenation  string (see Note #1).
*
*               pstr_cat    Pointer to concatenation string to append to destination string.
*
*               len_max     Maximum number of characters to concatenate (see Note #2e).
*
* Return(s)   : Pointer to destination string, if NO errors (see Note #2).
*
*               Pointer to NULL,               otherwise.
*
* Caller(s)   : various.
*
* Note(s)     : (1) Destination string buffer size NOT validated; buffer overruns MUST be prevented by caller.
*
*                   (a) Destination buffer size MUST be large enough to accommodate the entire concatenated
*                       string size including the terminating NULL character.
*
*               (2) String concatenation terminates when :
*
*                   (a) Destination/Concatenation string pointer(s) are passed NULL pointers.
*                       (1) No string concatenation performed; NULL pointer returned.
*
*                   (b) Destination string overlaps with NULL address.
*                       (1) No string concatenation performed; NULL pointer returned.
*
*                   (c) Destination/Concatenation string pointer(s) points to NULL.
*                       (1) String buffer(s) overlap with NULL address.
*                       (2) Concatenation string appended into destination string buffer up to but NOT
*                           beyond or including the NULL address; destination string buffer properly
*                           terminated with NULL character.
*
*                   (d) Concatenation string's terminating NULL character found.
*                       (1) Entire concatenation string appended to destination string.
*
*                   (e) 'len_max' number of characters concatenated.
*                       (1) 'len_max' number of characters does NOT include the terminating NULL character.
*
*                           See also Note #1a.
**********************************************************************************************************************
*/
char *SLIB_strncat(char *pdest, char *pstr_cat, __size_t len_max)
{
    char       *pstr;
    char       *pstr_next;
    __size_t    len_cat;

    /* Rtn NULL if str ptr(s) NULL     (see Note #2a).              */
    if (pdest == (char *)0)
    {
        return  ((char *)0);
    }
    if (pstr_cat == (char *)0)
    {
        return  ((char *)0);
    }

    if (len_max == (__size_t)0)                     /* Rtn NULL if cat len equals zero (see Note #2e).              */
    {
        return  ((char *)0);
    }


    pstr = pdest;
    while (( pstr != (char *)0) &&                  /* Adv to end of cur dest str until NULL ptr ...                */
            (*pstr != (char  )0))                    /* ... or NULL char found..                                     */
    {
        pstr++;
    }
    if (pstr == (char *)0)                          /* If NULL str overrun, rtn NULL (see Note #2b).                */
    {
        return ((char *)0);
    }

    pstr_next = pstr;
    pstr_next++;
    len_cat   = 0;

    while (( pstr_next != (char *)0) &&             /* Cat str until NULL ptr(s)  (see Note #2c)  ...               */
            ( pstr_cat  != (char *)0) &&
            (*pstr_cat  != (char  )0) &&             /* ... or NULL char found     (see Note #2d); ...               */
            ( len_cat   <  (__size_t)len_max))     /* ... or max nbr chars cat'd (see Note #2d).                   */
    {
        *pstr = *pstr_cat;
        pstr++;
        pstr_next++;
        pstr_cat++;
        len_cat++;
    }

    *pstr = (char)0;                                 /* Append NULL char (see Note #2c2).                            */


    return (pdest);
}

/*
**********************************************************************************************************************
*                                              SLIB_strcmp
*
* Description : Determine if two strings are identical.
*
* Argument(s) : p1_str      Pointer to first  string (see Note #1).
*
*               p2_str      Pointer to second string (see Note #1).
*
* Return(s)   : 0,              if strings are identical             (see Notes #2a, #2e, & #2f).
*
*               Negative value, if 'p1_str' is less    than 'p2_str' (see Notes #2b, #2g, & #2d).
*
*               Positive value, if 'p1_str' is greater than 'p2_str' (see Notes #2c, #2h, & #2d).
*
* Caller(s)   : various.
*
* Note(s)     : (1) String buffers NOT modified.
*
*               (2) String comparison terminates when :
*
*                   (a) BOTH string pointer(s) are passed NULL pointers.
*                       (1) NULL strings identical; return 0.
*
*                   (b) 'p1_str' passed a NULL pointer.
*                       (1) Return negative value of character pointed to by 'p2_str'.
*
*                   (c) 'p2_str' passed a NULL pointer.
*                       (1) Return positive value of character pointed to by 'p1_str'.
*
*                   (d) Non-matching characters found.
*                       (1) Return signed-integer difference of the character pointed to by 'p2_str'
*                           from the character pointed to by 'p1_str'.
*
*                   (e) Terminating NULL character found in both strings.
*                       (1) Strings identical; return 0.
*                       (2) Only one NULL character test required in conditional since previous condition
*                           tested character equality.
*
*                   (f) BOTH strings point to NULL.
*                       (1) Strings overlap with NULL address.
*                       (2) Strings identical up to but NOT beyond or including the NULL address; return 0.
*
*                   (g) 'p1_str_next' points to NULL.
*                       (1) 'p1_str' overlaps with NULL address.
*                       (2) Strings compared up to but NOT beyond or including the NULL address.
*                       (3) Return  negative value of character pointed to by 'p2_str_next'.
*
*                   (h) 'p2_str_next' points to NULL.
*                       (1) 'p2_str' overlaps with NULL address.
*                       (2) Strings compared up to but NOT beyond or including the NULL address.
*                       (3) Return  positive value of character pointed to by 'p1_str_next'.
*
*               (3) Since 16-bit signed arithmetic is performed to calculate a non-identical comparison
*                   return value, 'char' native data type size MUST be 8-bit.
**********************************************************************************************************************
*/
int  SLIB_strcmp(const char *p1_str, const char *p2_str)
{
    char    *p1_str_next;
    char    *p2_str_next;
    int    cmp_val;


    if (p1_str == (char *)0)
    {
        if (p2_str == (char *)0)
        {
            return ((int )0);                      /* If BOTH str ptrs NULL, rtn 0 (see Note #2a).                 */
        }
        cmp_val = (int )0 - (int )(*p2_str);
        return (cmp_val);                           /* If p1_str NULL, rtn neg p2_str val (see Note #2b).           */
    }
    if (p2_str == (char *)0)
    {
        cmp_val = (int )(*p1_str);
        return (cmp_val);                           /* If p2_str NULL, rtn pos p1_str val (see Note #2c).           */
    }


    p1_str_next = (char *)p1_str;
    p2_str_next = (char *)p2_str;
    p1_str_next++;
    p2_str_next++;
    while ((*p1_str      == *p2_str)   &&           /* Cmp strs until non-matching char (see Note #2d) ..           */
            (*p1_str      != (char  )0) &&           /* .. or NULL char(s)               (see Note #2e) ..           */
            ( p1_str_next != (char *)0) &&           /* .. or NULL ptr(s) found (see Notes #2f, #2g, & #2h).         */
            ( p2_str_next != (char *)0))
    {
        p1_str_next++;
        p2_str_next++;
        p1_str++;
        p2_str++;
    }


    if (*p1_str != *p2_str)                         /* If strs NOT identical, ...                                   */
    {
        cmp_val = (int )(*p1_str) - (int )(*p2_str);       /* ... calc & rtn char diff  (see Note #2d1).         */
    }
    else if (*p1_str == (char)0)                    /* If NULL char(s) found, ...                                   */
    {
        cmp_val = 0;                               /* ... strs identical; rtn 0 (see Note #2e).                    */

    }
    else
    {
        if (p1_str_next == (char *)0)
        {
            if (p2_str_next == (char *)0)           /* If BOTH next str ptrs NULL, ...                              */
            {
                cmp_val = (int )0;                 /* ... rtn 0                   (see Note #2f).                  */
            }
            else
            {
                /* If p1_str_next NULL, ...                                     */
                cmp_val = (int )0 - (int )(*p2_str_next);   /* ... rtn neg p2_str_next val (see Note #2g).        */
            }
        }
        else                                        /* If p2_str_next NULL, ...                                     */
        {
            cmp_val = (int )(*p1_str_next);        /* ... rtn pos p1_str_next val (see Note #2h).                  */
        }
    }

    return (cmp_val);
}

/*
**********************************************************************************************************************
*                                              SLIB_stricmp
*
* Description : Determine if two strings are identical,
*				compare characters of two strings without regard to case.
*
* Argument(s) : p1_str      Pointer to first  string (see Note #1).
*
*               p2_str      Pointer to second string (see Note #1).
*
* Return(s)   : 0,              if strings are identical             (see Notes #2a, #2e, & #2f).
*
*               Negative value, if 'p1_str' is less    than 'p2_str' (see Notes #2b, #2g, & #2d).
*
*               Positive value, if 'p1_str' is greater than 'p2_str' (see Notes #2c, #2h, & #2d).
*
* Caller(s)   : various.
*
* Note(s)     : (1) String buffers NOT modified.
*
*               (2) String comparison terminates when :
*
*                   (a) BOTH string pointer(s) are passed NULL pointers.
*                       (1) NULL strings identical; return 0.
*
*                   (b) 'p1_str' passed a NULL pointer.
*                       (1) Return negative value of character pointed to by 'p2_str'.
*
*                   (c) 'p2_str' passed a NULL pointer.
*                       (1) Return positive value of character pointed to by 'p1_str'.
*
*                   (d) Non-matching characters found.
*                       (1) Return signed-integer difference of the character pointed to by 'p2_str'
*                           from the character pointed to by 'p1_str'.
*
*                   (e) Terminating NULL character found in both strings.
*                       (1) Strings identical; return 0.
*                       (2) Only one NULL character test required in conditional since previous condition
*                           tested character equality.
*
*                   (f) BOTH strings point to NULL.
*                       (1) Strings overlap with NULL address.
*                       (2) Strings identical up to but NOT beyond or including the NULL address; return 0.
*
*                   (g) 'p1_str_next' points to NULL.
*                       (1) 'p1_str' overlaps with NULL address.
*                       (2) Strings compared up to but NOT beyond or including the NULL address.
*                       (3) Return  negative value of character pointed to by 'p2_str_next'.
*
*                   (h) 'p2_str_next' points to NULL.
*                       (1) 'p2_str' overlaps with NULL address.
*                       (2) Strings compared up to but NOT beyond or including the NULL address.
*                       (3) Return  positive value of character pointed to by 'p1_str_next'.
*
*               (3) Since 16-bit signed arithmetic is performed to calculate a non-identical comparison
*                   return value, 'char' native data type size MUST be 8-bit.
**********************************************************************************************************************
*/
int  SLIB_stricmp(const char *p1_str, const char *p2_str)
{
    char    *p1_str_next;
    char    *p2_str_next;
    int    cmp_val;


    if (p1_str == (char *)0)
    {
        if (p2_str == (char *)0)
        {
            return ((int )0);                      /* If BOTH str ptrs NULL, rtn 0 (see Note #2a).                 */
        }
        cmp_val = (int )0 - (int )SLIB_toupper(*p2_str);
        return (cmp_val);                           /* If p1_str NULL, rtn neg p2_str val (see Note #2b).           */
    }
    if (p2_str == (char *)0)
    {
        cmp_val = (int )SLIB_toupper(*p1_str);
        return (cmp_val);                           /* If p2_str NULL, rtn pos p1_str val (see Note #2c).           */
    }


    p1_str_next = (char *)p1_str;
    p2_str_next = (char *)p2_str;
    p1_str_next++;
    p2_str_next++;
    while ((SLIB_toupper(*p1_str) == SLIB_toupper(*p2_str)) && /* Cmp strs until non-matching char (see Note #2d) ..           */
            (*p1_str      != (char  )0) &&           /* .. or NULL char(s)               (see Note #2e) ..           */
            ( p1_str_next != (char *)0) &&           /* .. or NULL ptr(s) found (see Notes #2f, #2g, & #2h).         */
            ( p2_str_next != (char *)0))
    {
        p1_str_next++;
        p2_str_next++;
        p1_str++;
        p2_str++;
    }


    if (*p1_str != *p2_str)                         /* If strs NOT identical, ...                                   */
    {
        cmp_val = (int )SLIB_toupper(*p1_str) - (int )SLIB_toupper(*p2_str);       /* ... calc & rtn char diff  (see Note #2d1).         */
    }
    else if (*p1_str == (char)0)                    /* If NULL char(s) found, ...                                   */
    {
        cmp_val = 0;                               /* ... strs identical; rtn 0 (see Note #2e).                    */
    }
    else
    {
        if (p1_str_next == (char *)0)
        {
            if (p2_str_next == (char *)0)           /* If BOTH next str ptrs NULL, ...                              */
            {
                cmp_val = (int )0;                 /* ... rtn 0                   (see Note #2f).                  */
            }
            else
            {
                /* If p1_str_next NULL, ...                                     */
                cmp_val = (int )0 - (int )SLIB_toupper(*p2_str_next);   /* ... rtn neg p2_str_next val (see Note #2g).        */
            }
        }
        else                                        /* If p2_str_next NULL, ...                                     */
        {
            cmp_val = (int )SLIB_toupper(*p1_str_next);        /* ... rtn pos p1_str_next val (see Note #2h).                  */
        }
    }

    return (cmp_val);
}

/*
**********************************************************************************************************************
*                                             SLIB_strncmp
*
* Description : Determine if two strings are identical for up to a maximum number of characters,
*				compare characters of two strings without regard to case.
*
* Argument(s) : p1_str      Pointer to first  string (see Note #1).
*
*               p2_str      Pointer to second string (see Note #1).
*
*               len_max     Maximum number of characters to compare  (see Notes #2i & #2j).
*
* Return(s)   : 0,              if strings are identical             (see Notes #2a, #2e, #2f, #2i, & #2j).
*
*               Negative value, if 'p1_str' is less    than 'p2_str' (see Notes #2b, #2g, & #2d).
*
*               Positive value, if 'p1_str' is greater than 'p2_str' (see Notes #2c, #2h, & #2d).
*
* Caller(s)   : various.
*
* Note(s)     : (1) String buffers NOT modified.
*
*               (2) String comparison terminates when :
*
*                   (a) BOTH string pointer(s) are passed NULL pointers.
*                       (1) NULL strings identical; return 0.
*
*                   (b) 'p1_str' passed a NULL pointer.
*                       (1) Return negative value of character pointed to by 'p2_str'.
*
*                   (c) 'p2_str' passed a NULL pointer.
*                       (1) Return positive value of character pointed to by 'p1_str'.
*
*                   (d) Non-matching characters found.
*                       (1) Return signed-integer difference of the character pointed to by 'p2_str'
*                           from the character pointed to by 'p1_str'.
*
*                   (e) Terminating NULL character found in both strings.
*                       (1) Strings identical; return 0.
*                       (2) Only one NULL character test required in conditional since previous condition
*                           tested character equality.
*
*                   (f) BOTH strings point to NULL.
*                       (1) Strings overlap with NULL address.
*                       (2) Strings identical up to but NOT beyond or including the NULL address; return 0.
*
*                   (g) 'p1_str_next' points to NULL.
*                       (1) 'p1_str' overlaps with NULL address.
*                       (2) Strings compared up to but NOT beyond or including the NULL address.
*                       (3) Return  negative value of character pointed to by 'p2_str_next'.
*
*                   (h) 'p2_str_next' points to NULL.
*                       (1) 'p2_str' overlaps with NULL address.
*                       (2) Strings compared up to but NOT beyond or including the NULL address.
*                       (3) Return  positive value of character pointed to by 'p1_str_next'.
*
*                   (i) 'len_max' passed a zero length.
*                       (1) Zero-length strings identical; return 0.
*
*                   (j) First 'len_max' number of characters identical.
*                       (1) Strings identical; return 0.
*
*               (3) Since 16-bit signed arithmetic is performed to calculate a non-identical comparison
*                   return value, 'char' native data type size MUST be 8-bit.
**********************************************************************************************************************
*/
int  SLIB_strncmp(const char *p1_str, const char *p2_str, __size_t len_max)
{
    char    *p1_str_next;
    char    *p2_str_next;
    int     cmp_val;
    __size_t cmp_len;


    if (len_max == 0)                               /* If cmp len equals zero, rtn 0                                */
    {
        return ((int )0);
    }

    if (p1_str == (char *)0)
    {
        if (p2_str == (char *)0)
        {
            return ((int )0);                      /* If BOTH str ptrs NULL,  rtn 0      (see Note #2a).           */
        }
        cmp_val = (int )0 - (int )(*p2_str);
        return (cmp_val);                           /* If p1_str NULL, rtn neg p2_str val (see Note #2b).           */
    }
    if (p2_str == (char *)0)
    {
        cmp_val = (int )(*p1_str);
        return (cmp_val);                           /* If p2_str NULL, rtn pos p1_str val (see Note #2c).           */
    }


    p1_str_next = (char *)p1_str;
    p2_str_next = (char *)p2_str;
    p1_str_next++;
    p2_str_next++;
    cmp_len     = 0;
    while ((*p1_str      == *p2_str)   &&           /* Cmp strs until non-matching char (see Note #2d) ..           */
            (*p1_str      != (char  )0) &&           /* .. or NULL char(s)               (see Note #2e) ..           */
            ( p1_str_next != (char *)0) &&           /* .. or NULL ptr(s) found (see Notes #2f, #2g, & #2h);         */
            ( p2_str_next != (char *)0) &&
            ( cmp_len     <  (__size_t)len_max))     /* .. or len nbr chars cmp'd        (see Note #2j).             */
    {
        p1_str_next++;
        p2_str_next++;
        p1_str++;
        p2_str++;
        cmp_len++;
    }


    if (cmp_len == len_max)                         /* If strs     identical for len nbr of chars,                  */
    {
        return ((int )0);                          /* ... rtn 0 (see Note #2j).                                    */
    }

    if (*p1_str != *p2_str)                         /* If strs NOT identical, ...                                   */
    {
        cmp_val = (int )(*p1_str) - (int )(*p2_str);  /* ... calc & rtn char diff  (see Note #2d1).              */
    }
    else if (*p1_str == (char)0)                    /* If NULL char(s) found, ...                                   */
    {
        cmp_val = 0;                               /* ... strs identical; rtn 0 (see Note #2e).                    */
    }
    else
    {
        if (p1_str_next == (char *)0)
        {
            if (p2_str_next == (char *)0)           /* If BOTH next str ptrs NULL, ...                              */
            {
                cmp_val = (int )0;                 /* ... rtn 0                                                    */
            }
            else                                    /* If p1_str_next NULL, ...                                     */
            {
                cmp_val = (int )0 - (int )(*p2_str_next);   /* ... rtn neg p2_str_next val (see Note #2g).        */
            }
        }
        else                                        /* If p2_str_next NULL, ...                                     */
        {
            cmp_val = (int )(*p1_str_next);        /* ... rtn pos p1_str_next val (see Note #2h).                  */
        }
    }

    return (cmp_val);
}

/*
**********************************************************************************************************************
*                                             SLIB_strnicmp
*
* Description : Determine if two strings are identical for up to a maximum number of characters.
*				compare characters of two strings without regard to case.
* Argument(s) : p1_str      Pointer to first  string (see Note #1).
*
*               p2_str      Pointer to second string (see Note #1).
*
*               len_max     Maximum number of characters to compare  (see Notes #2i & #2j).
*
* Return(s)   : 0,              if strings are identical             (see Notes #2a, #2e, #2f, #2i, & #2j).
*
*               Negative value, if 'p1_str' is less    than 'p2_str' (see Notes #2b, #2g, & #2d).
*
*               Positive value, if 'p1_str' is greater than 'p2_str' (see Notes #2c, #2h, & #2d).
*
* Caller(s)   : various.
*
* Note(s)     : (1) String buffers NOT modified.
*
*               (2) String comparison terminates when :
*
*                   (a) BOTH string pointer(s) are passed NULL pointers.
*                       (1) NULL strings identical; return 0.
*
*                   (b) 'p1_str' passed a NULL pointer.
*                       (1) Return negative value of character pointed to by 'p2_str'.
*
*                   (c) 'p2_str' passed a NULL pointer.
*                       (1) Return positive value of character pointed to by 'p1_str'.
*
*                   (d) Non-matching characters found.
*                       (1) Return signed-integer difference of the character pointed to by 'p2_str'
*                           from the character pointed to by 'p1_str'.
*
*                   (e) Terminating NULL character found in both strings.
*                       (1) Strings identical; return 0.
*                       (2) Only one NULL character test required in conditional since previous condition
*                           tested character equality.
*
*                   (f) BOTH strings point to NULL.
*                       (1) Strings overlap with NULL address.
*                       (2) Strings identical up to but NOT beyond or including the NULL address; return 0.
*
*                   (g) 'p1_str_next' points to NULL.
*                       (1) 'p1_str' overlaps with NULL address.
*                       (2) Strings compared up to but NOT beyond or including the NULL address.
*                       (3) Return  negative value of character pointed to by 'p2_str_next'.
*
*                   (h) 'p2_str_next' points to NULL.
*                       (1) 'p2_str' overlaps with NULL address.
*                       (2) Strings compared up to but NOT beyond or including the NULL address.
*                       (3) Return  positive value of character pointed to by 'p1_str_next'.
*
*                   (i) 'len_max' passed a zero length.
*                       (1) Zero-length strings identical; return 0.
*
*                   (j) First 'len_max' number of characters identical.
*                       (1) Strings identical; return 0.
*
*               (3) Since 16-bit signed arithmetic is performed to calculate a non-identical comparison
*                   return value, 'char' native data type size MUST be 8-bit.
**********************************************************************************************************************
*/
int  SLIB_strnicmp(const char *p1_str, const char *p2_str, __size_t len_max)
{
    char    *p1_str_next;
    char    *p2_str_next;
    int     cmp_val;
    __size_t cmp_len;


    if (len_max == 0)                               /* If cmp len equals zero, rtn 0                                */
    {
        return ((int )0);
    }

    if (p1_str == (char *)0)
    {
        if (p2_str == (char *)0)
        {
            return ((int )0);                      /* If BOTH str ptrs NULL,  rtn 0      (see Note #2a).           */
        }
        cmp_val = (int )0 - (int )SLIB_toupper(*p2_str);
        return (cmp_val);                           /* If p1_str NULL, rtn neg p2_str val (see Note #2b).           */
    }
    if (p2_str == (char *)0)
    {
        cmp_val = (int )SLIB_toupper(*p1_str);
        return (cmp_val);                           /* If p2_str NULL, rtn pos p1_str val (see Note #2c).           */
    }


    p1_str_next = (char *)p1_str;
    p2_str_next = (char *)p2_str;
    p1_str_next++;
    p2_str_next++;
    cmp_len     = 0;
    while ((SLIB_toupper(*p1_str) == SLIB_toupper(*p2_str)) && /* Cmp strs until non-matching char (see Note #2d) ..           */
            (*p1_str      != (char  )0) &&           /* .. or NULL char(s)               (see Note #2e) ..           */
            ( p1_str_next != (char *)0) &&           /* .. or NULL ptr(s) found (see Notes #2f, #2g, & #2h);         */
            ( p2_str_next != (char *)0) &&
            ( cmp_len     <  (__size_t)len_max))     /* .. or len nbr chars cmp'd        (see Note #2j).             */
    {
        p1_str_next++;
        p2_str_next++;
        p1_str++;
        p2_str++;
        cmp_len++;
    }


    if (cmp_len == len_max)                         /* If strs     identical for len nbr of chars,                  */
    {
        return ((int )0);                          /* ... rtn 0 (see Note #2j).                                    */
    }

    if (*p1_str != *p2_str)                         /* If strs NOT identical, ...                                   */
    {
        cmp_val = (int )SLIB_toupper(*p1_str) - (int )SLIB_toupper(*p2_str);  /* ... calc & rtn char diff  (see Note #2d1).              */
    }
    else if (*p1_str == (char)0)                    /* If NULL char(s) found, ...                                   */
    {
        cmp_val = 0;                               /* ... strs identical; rtn 0 (see Note #2e).                    */
    }
    else
    {
        if (p1_str_next == (char *)0)
        {
            if (p2_str_next == (char *)0)           /* If BOTH next str ptrs NULL, ...                              */
            {
                cmp_val = (int )0;                 /* ... rtn 0                                                    */
            }
            else                                    /* If p1_str_next NULL, ...                                     */
            {
                cmp_val = (int )0 - (int )SLIB_toupper(*p2_str_next);   /* ... rtn neg p2_str_next val (see Note #2g).        */
            }
        }
        else                                        /* If p2_str_next NULL, ...                                     */
        {
            cmp_val = (int )SLIB_toupper(*p1_str_next);        /* ... rtn pos p1_str_next val (see Note #2h).                  */
        }
    }

    return (cmp_val);
}

/*
**********************************************************************************************************************
*                                             SLIB_strchr
*
* Description : Search string for first occurrence of specific character.
*
* Argument(s) : pstr            Pointer to string (see Note #1).
*
*               srch_char       Search character.
*
* Return(s)   : Pointer to first occurrence of search character in string, if any.
*
*               Pointer to NULL,                                           otherwise.
*
* Caller(s)   : various.
*
* Note(s)     : (1) String buffer NOT modified.
*
*               (2) String search terminates when :
*
*                   (a) String pointer passed a NULL pointer.
*                       (1) No string search performed; NULL pointer returned.
*
*                   (b) String pointer points to NULL.
*                       (1) String overlaps with NULL address.
*                       (2) String searched up to but NOT beyond or including the NULL address.
*
*                   (c) String's terminating NULL character found.
*                       (1) Search character NOT found in search string; NULL pointer returned.
*                       (2) Applicable ONLY IF search character is NOT the terminating NULL character.
*
*                   (d) Search character found.
*                       (1) Return pointer to first occurrence of search character in search string.
**********************************************************************************************************************
*/
char *SLIB_strchr (char *pstr, char srch_char)
{
    char *pstr_next;


    if (pstr == (char *)0)                          /* Rtn NULL if srch str ptr NULL (see Note #2a).                */
    {
        return ((char *)0);
    }


    pstr_next = pstr;
    pstr_next++;
    while (( pstr_next != (char *)0) &&             /* Srch str until NULL ptr(s) (see Note #2b) ...                */
            (*pstr      != (char  )0) &&             /* ... or NULL char           (see Note #2c) ...                */
            (*pstr      != (char  )srch_char))       /* ... or srch char found     (see Note #2d).                   */
    {
        pstr++;
        pstr_next++;
    }

    if (*pstr != srch_char)                         /* If srch char NOT found, str points to NULL; ...              */
    {
        return ((char *)0);                         /* ... rtn NULL (see Notes #2b & #2c).                          */
    }

    return (pstr);                                  /* Else rtn ptr to found srch char (see Note #2d).              */
}

/*
**********************************************************************************************************************
*                                            SLIB_strnchr
*
* Description : Search string for first occurrence of specific character, up to a maximum number of characters.
*
* Argument(s) : pstr            Pointer to string (see Note #1).
*
*               len_max         Maximum number of characters to search (see Notes #2e & #3).
*
*               srch_char       Search character.
*
* Return(s)   : Pointer to first occurrence of search character in string, if any.
*
*               Pointer to NULL,                                           otherwise.
*
* Caller(s)   : various.
*
* Note(s)     : (1) String buffer NOT modified.
*
*               (2) String search terminates when :
*
*                   (a) String pointer passed a NULL pointer.
*                       (1) No string search performed; NULL pointer returned.
*
*                   (b) String pointer points to NULL.
*                       (1) String overlaps with NULL address.
*                       (2) String searched up to but NOT beyond or including the NULL address.
*
*                   (c) String's terminating NULL character found.
*                       (1) Search character NOT found in search string; NULL pointer returned.
*                       (2) Applicable ONLY IF search character is NOT the terminating NULL character.
*
*                   (d) Search character found.
*                       (1) Return pointer to first occurrence of search character in search string.
*
*                   (e) 'len_max' number of characters searched.
*                       (1) 'len_max' number of characters does NOT include terminating NULL character.
*
*               (3) Ideally, the 'len_max' parameter would be the last parameter in this function's
*                   paramter list for consistency with all other custom string library functions.
*                   However, the 'len_max' parameter is ordered to comply with the standard library
*                   function's parameter list.
**********************************************************************************************************************
*/
char *SLIB_strnchr (char *pstr, __size_t len_max, char srch_char)
{
    char     *pstr_next;
    __size_t  len_srch;


    if (pstr == (char *)0)                          /* Rtn NULL if srch str ptr NULL    (see Note #2a).             */
    {
        return ((char *)0);
    }

    if (len_max == (__size_t)0)                     /* Rtn NULL if srch len equals zero (see Note #2e).             */
    {
        return ((char *)0);
    }

    pstr_next = pstr;
    pstr_next++;
    len_srch  = 0;
    while (( pstr_next != (char *)0)         &&     /* Srch str until NULL ptr(s)  (see Note #2b)  ...              */
            (*pstr      != (char  )0)         &&     /* ... or NULL char            (see Note #2c)  ...              */
            (*pstr      != (char  )srch_char) &&     /* ... or srch char found      (see Note #2d); ...              */
            ( len_srch  <  (__size_t)len_max))       /* ... or max nbr chars srch'd (see Note #2e).                  */
    {
        pstr++;
        pstr_next++;
        len_srch++;
    }

    if (*pstr != srch_char)                         /* If srch char NOT found, str points to NULL; ...              */
    {
        return ((char *)0);                         /* ... rtn NULL (see Notes #2b & #2c).                          */
    }

    return (pstr);                                  /* Else rtn ptr to found srch char (see Note #2d).              */
}

/*
**********************************************************************************************************************
*                                           SLIB_strchrlast
*
* Description : Search string for last occurrence of specific character.
*
* Argument(s) : pstr            Pointer to string (see Note #1).
*
*               srch_char       Search character.
*
* Return(s)   : Pointer to last occurrence of search character in string, if any.
*
*               Pointer to NULL,                                          otherwise.
*
* Caller(s)   : various.
*
* Note(s)     : (1) String buffer NOT modified.
*
*               (2) String search terminates when :
*
*                   (a) String pointer passed a NULL pointer.
*                       (1) No string search performed; NULL pointer returned.
*
*                   (b) String pointer points to NULL.
*                       (1) String overlaps with NULL address.
*                       (2) String searched up to but NOT beyond or including the NULL address.
*                       (3) NULL address boundary handled in Str_Len().
*
*                   (c) String searched from end to beginning.
*                       (1) Search character NOT found in search string; NULL pointer returned.
*                       (2) Applicable ONLY IF search character is NOT the terminating NULL character.
*
*                   (d) Search character found.
*                       (1) Return pointer to first occurrence of search character in search string.
**********************************************************************************************************************
*/
char *SLIB_strchrlast(char *pstr, char srch_char)
{
    char    *pstr_next;
    __size_t str_len;

    if (pstr == (char *)0)                          /* Rtn NULL if srch str ptr NULL (see Note #2a).                */
    {
        return ((char *)0);
    }

    pstr_next  = pstr;
    str_len    = SLIB_strlen(pstr);
    pstr_next += str_len;
    while (( pstr_next != pstr) &&                  /* Srch str from end until beg (see Note #2c) ...               */
            (*pstr_next != srch_char))               /* ... until srch char found   (see Note #2d).                  */
    {
        pstr_next--;
    }

    if (*pstr_next != srch_char)                    /* If srch char NOT found, str points to NULL; ...              */
    {
        return ((char *)0);                         /* ... rtn NULL (see Notes #2b & #2c).                          */
    }

    return (pstr_next);                             /* Else rtn ptr to found srch char (see Note #2d).              */
}

/*
**********************************************************************************************************************
*                                             SLIB_strstr
*
* Description : Search string for first occurence of a specific search string.
*
* Argument(s) : pstr            Pointer to        string (see Note #1).
*
*               psrch_str       Pointer to search string (see Note #1).
*
* Return(s)   : Pointer to first occurrence of search string in string, if any.
*
*               Pointer to NULL,                                        otherwise.
*
* Caller(s)   : various.
*
* Note(s)     : (1) String buffers NOT modified.
*
*               (2) String search terminates when :
*
*                   (a) String pointer passed a NULL pointer.
*                       (1) No string search performed; NULL pointer returned.
*
*                   (b) Search string length greater than string length.
*                       (1) No string search performed; NULL pointer returned.
*
*                   (c) Search string length equal to zero.
*                       (1) NULL search string at end of string returned.
*
*                   (d) Entire string has been searched.
*                       (1) Maximum size of the search is defined as the subtraction of the
*                           search string length from the string length.
*                       (2) Search string not found; NULL pointer returned.
*
*                   (e) Search string found.
*                       (1) Search string found according to Str_Cmp_N() return value.
*                       (2) Return pointer to first occurrence of search string in string.
**********************************************************************************************************************
*/
char *SLIB_strstr(char *pstr, char *psrch_str)
{
    __size_t    str_len;
    __size_t    srch_str_len;
    __size_t    srch_len;
    __size_t    srch_ix;
    int       srch_done;
    int        srch_cmp = 0;
    char       *pstr_srch_ix = 0;

    /* Rtn NULL if str ptr(s) NULL (see Note #2a).                  */
    if (pstr == (char *)0)
    {
        return ((char *)0);
    }
    if (psrch_str == (char *)0)
    {
        return ((char *)0);
    }


    str_len      = SLIB_strlen(pstr);
    srch_str_len = SLIB_strlen(psrch_str);
    if (srch_str_len > str_len)                     /* If srch str len > str len, rtn NULL  (see Note #2b).         */
    {
        return ((char *)0);
    }
    if (srch_str_len == 0)                          /* If srch str len = 0, srch str equal NULL str; ...            */
    {
        pstr_srch_ix = (char *)(pstr + str_len);    /* ... rtn ptr to NULL str found in str (see Note #2c).         */
        return (pstr_srch_ix);
    }

    srch_len  = str_len - srch_str_len;             /* Determine srch len (see Note #2d1).                          */
    srch_ix   = 0;
    srch_done = EPDK_NO;
    while ((srch_done == EPDK_NO) && (srch_ix <= srch_len))
    {
        pstr_srch_ix = (char *)(pstr + srch_ix);
        srch_cmp     =  SLIB_strncmp(pstr_srch_ix, psrch_str, srch_str_len);
        srch_done    = (srch_cmp == 0) ? EPDK_YES : EPDK_NO;
        srch_ix++;
    }


    if (srch_cmp != 0)                              /* If srch str NOT found, rtn NULL  (see Note #2d).             */
    {
        return ((char *)0);
    }

    return (pstr_srch_ix);                          /* Rtn ptr to srch str found in str (see Note #2e).             */
}

/*
**********************************************************************************************************************
*                                              SLIB_memclr
*
* Description : Clear data buffer (see Note #2).
*
* Argument(s) : pmem        Pointer to memory buffer to clear.
*
*               size        Number of data buffer octets to clear.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : (1) Null clears allowed (i.e. 0-octet size).
*
*                   See also 'Mem_Set()  Note #1'.
*
*               (2) Clear data by setting each data octet to 0.
**********************************************************************************************************************
*/
void  SLIB_memclr(void *pmem, __size_t size)
{
    SLIB_memset((void *)pmem, (__u8)0, (__size_t)size);
}

#include <string.h>
/*
**********************************************************************************************************************
*                                              SLIB_memset
*
* Description : Fill data buffer with specified data octet.
*
* Argument(s) : pmem        Pointer to memory buffer to fill with specified data octet.
*
*               data_val    Data fill octet value.
*
*               size        Number of data buffer octets to fill.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : (1) Null sets allowed (i.e. 0-octet size).
*
*               (2) For best CPU performance, optimized to fill data buffer using '__s32'-sized data words.
*
*                   (a) Since many word-aligned processors REQUIRE that multi-octet words be accessed on
*                       word-aligned addresses, '__s32'd words MUST be accessed on '__s32'd addresses.
*
*               (3) Modulo arithmetic is used to determine whether a memory buffer starts on a '__s32'
*                   address boundary.
*
*                   Modulo arithmetic in ANSI-C REQUIREs operations performed on integer values.  Thus,
*                   address values MUST be cast to an appropriately-sized integer value PRIOR to any
*                   mem_align_modulo arithmetic operation.
**********************************************************************************************************************
*/
void  SLIB_memset(void *pmem, __u8 data_val, __size_t size)
{
    /* use arm compiler libary directly,
     * maybe we should implement as asm by ourself.
     * by sunny at 2011-3-21 19:03:38.
     */
    memset(pmem, data_val, size);
}

/*
*********************************************************************************************************
*                                             SLIB_memcpy
*
* Description : Copy data octets from one buffer to another buffer.
*
* Argument(s) : pdest       Pointer to destination memory buffer.
*
*               psrc        Pointer to source      memory buffer.
*
*               size        Number of data buffer octets to copy.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : (1) Null copies allowed (i.e. 0-octet size).
*
*               (2) Memory buffers NOT checked for overlapping.
*
*               (3) For best CPU performance, optimized to fill data buffer using '__s32'-sized data words.
*
*                   (a) Since many word-aligned processors REQUIRE that multi-octet words be accessed on
*                       word-aligned addresses, '__s32'd words MUST be accessed on '__s32'd addresses.
*
*               (4) Modulo arithmetic is used to determine whether a memory buffer starts on a '__s32'
*                   address boundary.
*
*                   Modulo arithmetic in ANSI-C REQUIREs operations performed on integer values.  Thus,
*                   address values MUST be cast to an appropriately-sized integer value PRIOR to any
*                   mem_align_modulo arithmetic operation.
*********************************************************************************************************
*/
void *SLIB_memcpy(void *pdest, const void *psrc, __size_t size)
{
    /* use arm compiler libary directly,
     * maybe we should implement as asm by ourself.
     * by sunny at 2011-3-21 19:03:38.
     */
    return memcpy(pdest, psrc, size);
}

/*
*********************************************************************************************************
*                                              SLIB_memcmp
*
* Description : Verify that ALL data octets in two memory buffers are identical in sequence.
*
* Argument(s) : p1_mem      Pointer to first  memory buffer.
*
*               p2_mem      Pointer to second memory buffer.
*
*               size        Number of data buffer octets to compare.
*
* Return(s)   : DEF_YES, if 'size' number of data octets are identical in both memory buffers.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : various.
*
* Note(s)     : (1) Null compares allowed (i.e. 0-octet size); 'DEF_YES' returned to indicate identical
*                   null compare.
*
*               (2) Many memory buffer comparisons vary ONLY in the least significant octets -- e.g.
*                   network address buffers.  Consequently, memory buffer comparison is more efficient
*                   if the comparison starts from the end of the memory buffers which will abort sooner
*                   on dissimilar memory buffers that vary only in the least significant octets.
*
*               (3) For best CPU performance, optimized to fill data buffer using '__s32'-sized data words.
*
*                   (a) Since many word-aligned processors REQUIRE that multi-octet words be accessed on
*                       word-aligned addresses, '__s32'd words MUST be accessed on '__s32'd addresses.
*
*               (4) Modulo arithmetic is used to determine whether a memory buffer starts on a '__s32'
*                   address boundary.
*
*                   Modulo arithmetic in ANSI-C REQUIREs operations performed on integer values.  Thus,
*                   address values MUST be cast to an appropriately-sized integer value PRIOR to any
*                   mem_align_modulo arithmetic operation.
********************************************************************************************************
*/
int  SLIB_memcmp(const void *p1_mem, const void *p2_mem, __size_t size)
{
    __size_t     size_rem;
    __s32       *p1_mem_align;
    __s32       *p2_mem_align;
    __u8        *p1_mem_08;
    __u8        *p2_mem_08;
    __u8         i;
    __u8         mem_align_modulo_1;
    __u8         mem_align_modulo_2;
    int        mem_aligned;
    int        mem_cmp;


    if (size < 1)                                               /* See Note #1.                                         */
    {
        return (EPDK_YES);
    }
    if (p1_mem == (void *)0)
    {
        return (EPDK_NO);
    }
    if (p2_mem == (void *)0)
    {
        return (EPDK_NO);
    }
    mem_cmp            =  EPDK_YES;
    size_rem           =  size;
    /* Start @ end of mem bufs (see Note #2).               */
    p1_mem_08          = (__u8 *)p1_mem + size;
    p2_mem_08          = (__u8 *)p2_mem + size;
    /* See Note #4.                                         */
    mem_align_modulo_1 = (__u8  )((__u32)p1_mem_08 % sizeof(__s32));
    mem_align_modulo_2 = (__u8  )((__u32)p2_mem_08 % sizeof(__s32));

    mem_aligned        = (mem_align_modulo_1 == mem_align_modulo_2) ? EPDK_YES : EPDK_NO;

    if (mem_aligned == EPDK_YES)                                 /* If mem bufs' alignment offset equal, ...             */
    {
        /* ... optimize cmp for mem buf alignment.              */
        if (mem_align_modulo_1 != 0)                            /* If trailing octets avail,                  ...       */
        {
            i = mem_align_modulo_1;
            while ((mem_cmp == EPDK_YES) &&                      /* ... cmp mem bufs while identical &         ...       */
                    (size_rem > 0)       &&                      /* ... start mem buf cmp with trailing octets ...       */
                    (i        > 0))                              /* ... until next __s32 word boundary.              */
            {
                p1_mem_08--;
                p2_mem_08--;
                if (*p1_mem_08 != *p2_mem_08)                   /* If ANY data octet(s) NOT identical, cmp fails.       */
                {
                    mem_cmp = EPDK_NO;
                }
                size_rem -= sizeof(__u8);
                i--;
            }
        }

        if (mem_cmp == EPDK_YES)                                 /* If cmp still identical, cmp aligned mem bufs.        */
        {
            p1_mem_align = (__s32 *)p1_mem_08;              /* See Note #3a.                                        */
            p2_mem_align = (__s32 *)p2_mem_08;

            while ((mem_cmp  == EPDK_YES) &&                     /* Cmp mem bufs while identical & ...                   */
                    (size_rem >= sizeof(__s32))) /* ... mem bufs aligned on __s32 word boundaries.           */
            {
                p1_mem_align--;
                p2_mem_align--;
                if (*p1_mem_align != *p2_mem_align) /* If ANY data octet(s) NOT identical, cmp fails.               */
                {
                    mem_cmp = EPDK_NO;
                }
                size_rem -= sizeof(__s32);
            }

            p1_mem_08 = (__u8 *)p1_mem_align;
            p2_mem_08 = (__u8 *)p2_mem_align;
        }
    }

    while ((mem_cmp == EPDK_YES) &&                  /* Cmp mem bufs while identical ...                             */
            (size_rem > 0))                          /* ... for unaligned mem bufs or trailing octets.               */
    {
        p1_mem_08--;
        p2_mem_08--;
        if (*p1_mem_08 != *p2_mem_08)               /* If ANY data octet(s) NOT identical, cmp fails.               */
        {
            mem_cmp = EPDK_NO;
        }
        size_rem -= sizeof(__u8);
    }

    return (mem_cmp);
}

