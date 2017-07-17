/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eLIBS Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : string op.
* File    : elibs_string.c
*
* By      : Steven.ZGJ
* Version : v1.0
* Date    : 2008-8-22 9:21:32
* Note(s) : (1) NO compiler-supplied standard library functions are used in library or product software.
*
*                     (a) ALL standard library functions are implemented in the custom library modules :
*
*                         (1) \<Custom Library Directory>\lib*.*
*
*                         (2) \<Custom Library Directory>\Ports\<cpu>\<compiler>\lib*_a.*
*
*                               where
*                                       <Custom Library Directory>      directory path for custom library software
*                                       <cpu>                           directory name for specific processor (CPU)
*                                       <compiler>                      directory name for specific compiler
*
*                     (b) Product-specific library functions are implemented in individual products.
**********************************************************************************************************************
*/

#include "elibs_i.h"

__size eLIBs_strlen(const char  *pstr)
{
    return SLIB_strlen(pstr);
}

char *eLIBs_strcpy(char *pdest, const char *psrc)
{
    return SLIB_strcpy(pdest, psrc);
}

char *eLIBs_strncpy(char *pdest, const char *psrc, __size len_max)
{
    return SLIB_strncpy(pdest, psrc, len_max);
}

char *eLIBs_strcat(char *pdest, const char *pstr_cat)
{
    return SLIB_strcat(pdest, pstr_cat);
}

char *eLIBs_strncat(char *pdest, char *pstr_cat, __size_t len_max)
{
    return SLIB_strncat(pdest, pstr_cat, len_max);
}

int eLIBs_strcmp(const char *p1_str, const char *p2_str)
{
    return SLIB_strcmp(p1_str, p2_str);
}

int eLIBs_stricmp(const char *p1_str, const char *p2_str)
{
    return SLIB_stricmp(p1_str, p2_str);
}

int eLIBs_strncmp(const char *p1_str, const char *p2_str, __size_t len_max)
{
    return SLIB_strncmp(p1_str, p2_str, len_max);
}

int eLIBs_strnicmp(const char *p1_str, const char *p2_str, __size_t len_max)
{
    return SLIB_strnicmp(p1_str, p2_str, len_max);
}

char *eLIBs_strchr(char *pstr, char srch_char)
{
    return SLIB_strchr(pstr, srch_char);
}

char *eLIBs_strnchr(char *pstr, __size_t len_max, char srch_char)
{
    return SLIB_strnchr(pstr, len_max, srch_char);
}

char *eLIBs_strchrlast(char *pstr, char srch_char)
{
    return SLIB_strchrlast(pstr, srch_char);
}

char *eLIBs_strstr(char *pstr, char *psrch_str)
{
    return SLIB_strstr(pstr, psrch_str);
}

void eLIBs_memclr(void *pmem, __size_t size)
{
    SLIB_memset((void *)pmem, (__u8)0, (__size_t)size);
}

void eLIBs_memset(void *pmem, __u8 data_val, __size_t size)
{
    SLIB_memset(pmem, data_val, size);
}

void *eLIBs_memcpy(void *pdest, const void *psrc, __size_t size)
{
    return SLIB_memcpy(pdest, psrc, size);
}

__bool eLIBs_memcmp(const void *p1_mem, const void *p2_mem, __size_t size)
{
    return SLIB_memcmp(p1_mem, p2_mem, size);
}

//maybe ANSI c fucntion should all use this.
//by sunny at 2011-4-30 14:19:14.
//__size strlen(const char  *pstr)
//{
//    return SLIB_strlen(pstr);
//}
