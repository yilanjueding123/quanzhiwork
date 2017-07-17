;/*
;*********************************************************************************************************
;*                                                    MELIS
;*                                    the Easy Portable/Player Develop Kits
;*                                         Melis Configuration Module
;*
;*                                    (c) Copyright 2006-2010, kevin.z China
;*                                             All Rights Reserved
;*
;* File    : cfgs.inc
;* By      : kevin.z
;* Version : v2.0
;* Date    : 2010-11-23 11:10
;* Descript: config some parameter for melis.
;* Update  : date                auther      ver     notes
;*           2010-11-23 11:10    kevin.z     2.0     build the file;
;*********************************************************************************************************
;*/
    IF :LNOT::DEF:__CFGS_INC
    GBLA  __CFGS_INC


    include const.inc
    include cfgs\epdk_arch_cfgs.inc



REGS_vBASE  equ 0xf0000000  ;//i/o virtual address base
DRAM_vBASE  equ 0xc2000000  ;//dram virtual address base
SRAM_vBASE  equ 0xf0000000  ;//sram virtual address base

;// define max dram size
DRAM_SIZE   equ (128 * 1024 * 1024)

    ;///////////////////////////

    ENDIF     ;// IF :LNOT::DEF:__CFGS_INC

    END       ;// end of cfgs.inc

