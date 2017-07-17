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
* File    : Esh_script_executer.h
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/
#ifndef  _ESH_SCRIPT_EXECUTER_H_
#define  _ESH_SCRIPT_EXECUTER_H_

#include   "Esh_script_parser.h"

extern int execute(struct op *t, int *pin, int *pout, int no_fork);

#endif                   /* _ESH_SCRIPT_EXECUTER_H_ */
