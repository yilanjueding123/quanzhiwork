;//******************************************************************************************************
;//*                                                    ePOS
;//*                                the Easy Portable/Player Operation System
;//*                                           epos configurations
;//*
;//*                                (c) Copyright 2009-2010, kevin.z.m China
;//*                                            All Rights Reserved
;//*
;//* File   : epdk_arch_cfgs.inc
;//* Version: V1.0
;//* By     : kevin.z.m
;//* Date   : 2009-12-4 16:43
;//******************************************************************************************************

    IF :LNOT::DEF:__EPDK_ARCH_CFGS_INC
    GBLA  __EPDK_ARCH_CFGS_INC

    include  const.inc

EPDK_ARCH       equ EPDK_ARCH_SUNIV
EPDK_CHIP       equ EPDK_CHIP_SUNIV
EPDK_CPU        equ EPDK_CPU_AC320

    ENDIF
    END       ;// end of __EPDK_ARCH_CFGS_INC
