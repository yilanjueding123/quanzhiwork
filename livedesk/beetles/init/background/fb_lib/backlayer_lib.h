/*
*********************************************************************************************************
*                                                   eMOD
*                                  the Easy Portable/Player Operation System
*                                              mod_willow sub-system
*
*                               (c) Copyright 2006-2009, ALLWINNERME, China
*                                           All Rights Reserved
*
* File   : backlayer_lib.h
* Version: V1.0
* By     : xiechuanrong
*********************************************************************************************************
*/
#ifndef _BACKLAYER_LIB_H_
#define _BACKLAYER_LIB_H_
#include "epdk.h"

#ifndef memset
#define memset(buf, val, size) eLIBs_memset(buf, val, size)
#endif
#ifndef palloc
#define palloc(npage, mode) esMEMS_Palloc(npage, mode)
#endif
#ifndef pfree
#define pfree(pblk, npage) esMEMS_Pfree(pblk, npage)
#endif

extern __s32 backlayer_create_layer (ES_FILE *de_hdle);
extern void  backlayer_delete_layer (ES_FILE *de_hdle, __u32 layer_handle);
extern void  backlayer_set_fb (ES_FILE *de_hdle, __u32 layer_handle, FB *fb, __u8 mod);
extern void  backlayer_set_top (ES_FILE *de_hdle, __u32 layer_handle);
extern void  backlayer_set_bottom (ES_FILE *de_hdle, __u32 layer_handle);

#endif
