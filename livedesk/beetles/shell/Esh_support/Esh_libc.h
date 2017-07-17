/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     Some backup function for shell
*
* File    : Esh_libc.h
* By      : sunny
* Version : V1.00                                                                 2009.3.2
*********************************************************************************************************
*/

#ifndef   _ESH_LIBC_H_
#define   _ESH_LIBC_H_

#include        "Esh_platform.h"

/* external function */
extern void Esh_prs(const char *s);
extern void Esh_prc(const char ch);
extern int  Esh_getn(char *as);

extern int Esh_any(int c, const char *s);
extern int Esh_anys(const char *s1, const char *s2);

extern void Esh_itoa(int n, char *str);
extern int  Esh_atoi(const char *s);
extern __u32 Esh_atou(const char *s);
extern __int64 Esh_atoll(const char *s);

extern void Esh_sprintf( char *buffer, const char *str, ...);

extern int  Esh_Stricmp(const char *s1, const char *s2);

/* clibs */
extern int Esh_isdigit(int cChar);
extern int Esh_isalnum(int cChar);
extern int Esh_isalpha(int cChar);
extern int Esh_isspace(int cChar);

extern int Esh_Is_Digit_String(const char *str);

#endif      /* _ESH_LIBC_H_ */
