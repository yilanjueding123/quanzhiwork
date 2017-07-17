/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                              Share Libary Module
*
*                                    (c) Copyright 2010-2012, sunny China
*                                             All Rights Reserved
*
* File    : mod_slib_api.h
* By      : sunny
* Version : v1.0
* Date    : 2011-3-18
* Descript: share libary exteral interfaces.
* Update  : date                auther      ver     notes
*           2011-3-18 18:15:26  sunny       1.0     create this file.
*********************************************************************************************************
*/

#ifndef __MOD_SLIB_API_H__
#define __MOD_SLIB_API_H__

#ifndef USED_SLIB_DIRECT

#define SLIB_int2str_dec    (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_int2str_dec )
#define SLIB_uint2str_dec   (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_uint2str_dec)
#define SLIB_int2str_hex    (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_int2str_hex )
#define SLIB_atoi           (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_atoi        )
#define SLIB_toupper        (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_toupper     )
#define SLIB_isspace        (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_isspace     )
#define SLIB_strtol         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strtol      )
#define SLIB_strlen         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strlen      )
#define SLIB_strcpy         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strcpy      )
#define SLIB_strncpy        (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strncpy     )
#define SLIB_strcat         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strcat      )
#define SLIB_strncat        (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strncat     )
#define SLIB_strcmp         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strcmp      )
#define SLIB_stricmp        (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_stricmp     )
#define SLIB_strncmp        (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strncmp     )
#define SLIB_strnicmp       (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strnicmp    )
#define SLIB_strchr         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strchr      )
#define SLIB_strnchr        (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strnchr     )
#define SLIB_strchrlast     (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strchrlast  )
#define SLIB_strstr         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strstr      )
#define SLIB_memclr         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_memclr      )
#define SLIB_memset         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_memset      )
#define SLIB_memcpy         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_memcpy      )
#define SLIB_memcmp         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_memcmp      )
#define SLIB_vsnprintf      (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_vsnprintf   )

#else

void    SLIB_int2str_dec (int input , char *str);
void    SLIB_uint2str_dec(unsigned int input, char *str);
void    SLIB_int2str_hex (int input, char *str, int hex_flag);
int     SLIB_atoi        (const char *s);
int     SLIB_toupper     (int c);
int     SLIB_isspace     (int ch);
long    SLIB_strtol      (const char *str, const char **err, int base);
__size  SLIB_strlen      (const char  *pstr);
char   *SLIB_strcpy      (char *pdest, const char *psrc);
char   *SLIB_strncpy     (char *pdest, const char *psrc, __size len_max);
char   *SLIB_strcat      (char *pdest, const char *pstr_cat);
char   *SLIB_strncat     (char *pdest, char *pstr_cat, __size_t len_max);
int     SLIB_strcmp      (const char *p1_str, const char *p2_str);
int     SLIB_stricmp     (const char *p1_str, const char *p2_str);
int     SLIB_strncmp     (const char *p1_str, const char *p2_str, __size_t len_max);
int     SLIB_strnicmp    (const char *p1_str, const char *p2_str, __size_t len_max);
char   *SLIB_strchr      (char *pstr, char srch_char);
char   *SLIB_strnchr     (char *pstr, __size_t len_max, char srch_char);
char   *SLIB_strchrlast  (char *pstr, char srch_char);
char   *SLIB_strstr      (char *pstr, char *psrch_str);
void    SLIB_memclr      (void *pmem, __size_t size);
void    SLIB_memset      (void *pmem, __u8 data_val, __size_t size);
void   *SLIB_memcpy      (void *pdest, const void *psrc, __size_t size);
int     SLIB_memcmp      (const void *p1_mem, const void *p2_mem, __size_t size);
int     SLIB_vsnprintf   (char *buf, __size_t size, const char *format, va_list arg);

#endif  /* USED_BY_SLIB_MOD */

#endif  /* __MOD_SLIB_API_H__ */
