/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                              Mussy support
*
* File    : Esh_support.h
* By      : sunny
* Version : V1.00                                                           2009.3.2
*********************************************************************************************************
*/

#ifndef         _ESH_SUPPORT_H_
#define         _ESH_SUPPORT_H_

#include        "Esh_libc.h"
#include        "Esh_platform.h"

extern          char *Esh_alloc_name(unsigned int size);
extern          char *Esh_get_name(const char *name);
extern          void  Esh_free_name(char *name);
extern          char *Esh_get_app_name(const char *name);
extern          char *Esh_Seach_Executable_Program(const char *name);
extern          void  frt_compensate_name(char *cmd_str, char *pwdir, char *cps_str, __u32 *cps_len, __u32 *valid_len);

#endif        /* _ESH_SUPPORT_H_ */
