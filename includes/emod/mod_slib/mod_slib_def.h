/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                              Share Libary Module
*
*                                    (c) Copyright 2010-2012, sunny China
*                                             All Rights Reserved
*
* File    : mod_slib_def.h
* By      : sunny
* Version : v1.0
* Date    : 2011-3-18
* Descript: share libary exteral structures.
* Update  : date                auther      ver     notes
*           2011-3-18 18:15:49  sunny       1.0     create this file.
*********************************************************************************************************
*/

#ifndef __MOD_SLIB_DEF_H__
#define __MOD_SLIB_DEF_H__

typedef struct SLIB_API_ENTRY
{
    void     (*ptr_SLIB_int2str_dec )(int input , char *str);
    void     (*ptr_SLIB_uint2str_dec)(unsigned int input, char *str);
    void     (*ptr_SLIB_int2str_hex )(int input, char *str, int hex_flag);
    int      (*ptr_SLIB_atoi        )(const char *s);
    int      (*ptr_SLIB_toupper     )(int c);
    int      (*ptr_SLIB_isspace     )(int ch);
    long     (*ptr_SLIB_strtol      )(const char *str, const char **err, int base);
    __size   (*ptr_SLIB_strlen      )(const char  *pstr);
    char    *(*ptr_SLIB_strcpy      )(char *pdest, const char *psrc);
    char    *(*ptr_SLIB_strncpy     )(char *pdest, const char *psrc, __size len_max);
    char    *(*ptr_SLIB_strcat      )(char *pdest, const char *pstr_cat);
    char    *(*ptr_SLIB_strncat     )(char *pdest, char *pstr_cat, __size_t len_max);
    int      (*ptr_SLIB_strcmp      )(const char *p1_str, const char *p2_str);
    int      (*ptr_SLIB_stricmp     )(const char *p1_str, const char *p2_str);
    int      (*ptr_SLIB_strncmp     )(const char *p1_str, const char *p2_str, __size_t len_max);
    int      (*ptr_SLIB_strnicmp    )(const char *p1_str, const char *p2_str, __size_t len_max);
    char    *(*ptr_SLIB_strchr      )(char *pstr, char srch_char);
    char    *(*ptr_SLIB_strnchr     )(char *pstr, __size_t len_max, char srch_char);
    char    *(*ptr_SLIB_strchrlast  )(char *pstr, char srch_char);
    char    *(*ptr_SLIB_strstr      )(char *pstr, char *psrch_str);
    void     (*ptr_SLIB_memclr      )(void *pmem, __size_t size);
    void     (*ptr_SLIB_memset      )(void *pmem, __u8 data_val, __size_t size);
    void    *(*ptr_SLIB_memcpy      )(void *pdest, const void *psrc, __size_t size);
    int      (*ptr_SLIB_memcmp      )(const void *p1_mem, const void *p2_mem, __size_t size);
    int      (*ptr_SLIB_vsnprintf   )(char *buf, __size_t size, const char *format, va_list arg);
} slib_api_entry_t;

//not used by share libary module itsef

//this address is depended on the link address of share libary
#define SLIB_MOD_ENTRY_TBL   (0xe8700000)

#define SLIB_MOD_FUNC_ENTRY  ((slib_api_entry_t *)SLIB_MOD_ENTRY_TBL)


#endif  /* __MOD_SLIB_DEF_H__ */
