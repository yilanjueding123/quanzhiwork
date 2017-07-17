/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                           Matrix Key Driver Module
*
*                                    (c) Copyright 2006-2010, All winners Co,Ld.
*                                             All Rights Reserved
*
* File    : magic.c
* By      : james deng <csjamesdeng@gmail.com>
* Version : 1.0.0
* Date    : 2011-04-25
* Descript:
* Update  : date              author         ver       notes
*           2011-04-25        james deng     1.0.0     build the file.
*********************************************************************************************************
*/

#include "drv_matrix_key_i.h"

#pragma arm section rodata="MAGIC"
const __mods_mgsec_t modinfo =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'},		//.magic
    0x01000000,                				//.version
    EMOD_TYPE_DRV_MATRIXKEY,                //.type
    0xF0000,                                //.heapaddr
    0x400,                      			//.heapsize
    {
        //.mif
        &drv_matrix_key_init,
        &drv_matrix_key_exit,
        &drv_matrix_key_open,
        &drv_matrix_key_close,
        &drv_matrix_key_read,
        &drv_matrix_key_write,
        &drv_matrix_key_ioctrl
    }
};
#pragma arm section
