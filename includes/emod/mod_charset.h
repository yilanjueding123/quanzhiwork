/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Charset Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : mod_charset.h
* By      : kevin.z
* Version : v1.0
* Date    : 2010-11-23 20:40
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-23 20:40    kevin.z     1.0     build the file.
*********************************************************************************************************
*/
#ifndef __MOD_CHARSET_H__
#define __MOD_CHARSET_H__

#include "typedef.h"
#include "cfgs.h"
#include "mod_charset\\charset_inner.h"

#ifdef USED_BY_CHARSET
__s32 CHS_Uni2Char(__s32 charset_type, __u16 uni, __u8 *str, __u32 len);
__s32 CHS_Char2Uni(__s32 charset_type, const __u8 *str, __u32 len, __u16 *uni);
__u32 CHS_GetChUpperTbl(__s32 charset_type, void *buf, __u32 size);
__u32 CHS_GetChLowerTbl(__s32 charset_type, void *buf, __u32 size);
#endif


#endif  //__MOD_CHARSET_H__

