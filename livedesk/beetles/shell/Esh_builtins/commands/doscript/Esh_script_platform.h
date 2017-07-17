/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              apps Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : Shell
* File    : Esh_script_platform.h
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#ifndef  _ESH_SCRIPT_PLATFORM_H_
#define  _ESH_SCRIPT_PLATFORM_H_

#include "epdk.h"

#define printf    eLIBs_printf

#define strcpy(src, dest)                 eLIBs_strcpy(src, dest)
#define strncpy(src, dest, count)         eLIBs_strncpy(src, dest, count)
#define strcat(s1, s2)                    eLIBs_strcat(s1, s2)
#define strncat(s1, s2, count)            eLIBs_strncat(s1, s2, count)
#define strcmp(s1, s2)                    eLIBs_strcmp(s1, s2)
#define strncmp(s1, s2, count)            eLIBs_strncmp(s1, s2, count)
#define strlen(s)                         eLIBs_strlen(s)
#define memset(pmem, data_val, size)      eLIBs_memset(pmem, data_val, size)
#define memcpy(dest, src, size)           eLIBs_memcpy(dest, src, size)


#define fopen(filename, mode)             eLIBs_fopen(filename, mode)
#define fclose(stream)                    eLIBs_fclose(stream)
#define fread(ptr, size, nmemb, stream)   eLIBs_fread(ptr, size, nmemb, stream)
#define fwrite(ptr, size, nmemb, stream)  eLIBs_fwrite(ptr, size, nmemb, stream)
#define fseek(stream, offset, whence)     eLIBs_fseek(stream, offset, whence)
#define ftell(stream)                     eLIBs_ftell(stream)
#define ferror(stream)                    eLIBs_ferror(stream)
#define ferrclr(stream)                   eLIBs_ferrclr(stream)
#define remove(filename)                  eLIBs_remove(filename)


#define opendir(path)                     eLIBs_opendir(path)
#define readdir(path)                     eLIBs_readdir(path)
#define closedir(path)                    eLIBs_closedir(path)
#define rmdir(dirname)                    eLIBs_rmdir(dirname)

#ifndef SIM_PC_WIN
#define malloc(size)                      esMEMS_Malloc(0, size)
#define free(memblock)                    esMEMS_Mfree(0, memblock)
#endif

#endif    /* _ESH_SCRIPT_PLATFORM_H_ */
