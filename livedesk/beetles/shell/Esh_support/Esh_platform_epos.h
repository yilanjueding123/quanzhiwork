/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     Some backup function for shell
*
* File    : Esh_platform_epos.h
* By      : sunny
* Version : V1.00                                                                 2009.3.2
*********************************************************************************************************
*/

#ifndef  _ESH_PLATFORM_EPOS_H_
#define  _ESH_PLATFORM_EPOS_H_

#include "epdk.h"

/* file attribute for fat fs */
#define FSYS_ATTR_READONLY    0x00000001
#define FSYS_ATTR_HIDDEN      0x00000002
#define FSYS_ATTR_SYSTEM      0x00000004
#define FSYS_FAT_VOLUME_ID    0x00000008
#define FSYS_ATTR_DIRECTORY   0x00000010
#define FSYS_ATTR_ARCHIVE     0x00000020

#define FILE                ES_FILE
#define DIR                 ES_DIR
#define DIRENT              ES_DIRENT
#define Esh_printf          eLIBs_printf

/* Esh_string.c */
#define Esh_strcpy(src, dest)                   eLIBs_strcpy(src, dest)
#define Esh_strncpy(src, dest, count)           eLIBs_strncpy(src, dest, count)
#define Esh_strcat(s1, s2)                      eLIBs_strcat(s1, s2)
#define Esh_strncat(s1, s2, count)              eLIBs_strncat(s1, s2, count)
#define Esh_strcmp(s1, s2)                      eLIBs_strcmp(s1, s2)
#define Esh_strncmp(s1, s2, count)              eLIBs_strncmp(s1, s2, count)
#define Esh_strlen(s)                           eLIBs_strlen(s)
#define Esh_memset(pmem, data_val, size)        eLIBs_memset(pmem, data_val, size)
#define Esh_memcpy(dest, src, size)             eLIBs_memcpy(dest, src, size)
#define Esh_strchr(pstr, srch_char)             eLIBs_strchr(pstr, srch_char)
#define Esh_strchrlast(pstr, srch_char)         eLIBs_strchrlast(pstr, srch_char)

/* Esh_file.c */
#define Esh_fopen(filename, mode)               eLIBs_fopen(filename, mode)
#define Esh_fclose(stream)                      eLIBs_fclose(stream)
#define Esh_fread(ptr, size, nmemb, stream)     eLIBs_fread(ptr, size, nmemb, stream)
#define Esh_fwrite(ptr, size, nmemb, stream)    eLIBs_fwrite(ptr, size, nmemb, stream)
#define Esh_fseek(stream, offset, whence)       eLIBs_fseek(stream, offset, whence)
#define Esh_ftell(stream)                       eLIBs_ftell(stream)
#define Esh_ferror(stream)                      eLIBs_ferror(stream)
#define Esh_ferrclr(stream)                     eLIBs_ferrclr(stream)
#define Esh_fioctrl(stream, Cmd, Aux, pBuffer)  eLIBs_fioctrl(stream, Cmd, Aux, pBuffer)

#define Esh_remove(filename)                    eLIBs_remove(filename)

/* Esh_directory.c */
#define Esh_opendir(path)                       eLIBs_opendir(path)
#define Esh_readdir(path)                       eLIBs_readdir(path)
#define Esh_closedir(path)                      eLIBs_closedir(path)
#define Esh_mkdir(dirname)                      eLIBs_mkdir(dirname)
#define Esh_rmdir(dirname)                      eLIBs_rmdir(dirname)

/* Esh_mems.c */
#define Esh_malloc(size)                        esMEMS_Malloc(0, size)
#define Esh_free(memblock)                      esMEMS_Mfree(0, memblock)

#define ESH_ALIGN(val,align)	(((val) + ((align) - 1)) & ~((align) - 1))
#define Esh_palloc(size)        esMEMS_Palloc(ESH_ALIGN((size), 1024)>>10,  \
                                MEMS_PALLOC_MODE_BND_NONE|MEMS_PALLOC_MODE_BNK_NONE)
#define Esh_pfree(addr, size)   do{if(addr) esMEMS_Pfree(addr, ESH_ALIGN((size), 1024)>>10);}while (0)

#endif          /* _ESH_PLATFORM_EPOS_H_ */
