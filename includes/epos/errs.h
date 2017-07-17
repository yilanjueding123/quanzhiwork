/*
*********************************************************************************************************
*                                               error system
*                                         			esOS
*
*
*                                    (c) Copyright 2007-2008, esOS
*                                          All Rights Reserved
*
*                                          error system headfile
*
* File   : err.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#ifndef ERRS_H
#define ERRS_H

#include "typedef.h"
#include "cfgs.h"

#ifdef EPDK_USED_BY_KRNL
#define __swi(x)
#endif



#define ERRS_EXEC_no_free_prio_in_current_priolevel    0x00000000

/*
*********************************************************************************************************
*
*********************************************************************************************************
*/
#ifndef SIM_PC_WIN
__swi(SWINO_ERRS + 0 << 8) void esErrMsg(__u32 errno);
#else
void esErrMsg(__u32 errno);
#endif


#endif  //define ERRS_H
