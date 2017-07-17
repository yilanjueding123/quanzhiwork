/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                Flash Module
*
*                                    (c) Copyright 2010-2012, wfsun China
*                                             All Rights Reserved
*
* File    : mod_flash_api.h
* By      : wfsun
* Version : v1.0
* Date    : 2011-1-18
* Descript: flash module external interfaces.
* Update  : date                auther      ver     notes
*           2011-1-18 17:05:31  wfsun       1.0     create this file.
*********************************************************************************************************
*/

#ifndef __MOD_FLASH_API_H__
#define __MOD_FLASH_API_H__

#ifndef USED_BY_FLASH_MOD

#define flash_open            (FLASH_MOD_FUNC_ENTRY->ptr_flash_open)
#define flash_control         (FLASH_MOD_FUNC_ENTRY->ptr_flash_control)
#define flash_close           (FLASH_MOD_FUNC_ENTRY->ptr_flash_close)
#endif  /* USED_BY_FLASH_MOD */

#endif  /* __MOD_FLASH_H__ */
