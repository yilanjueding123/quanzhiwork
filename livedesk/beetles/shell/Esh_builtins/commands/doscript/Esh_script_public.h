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
* File    : Esh_script_public.h
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#ifndef  _ESH_SCRIPT_PUBLIC_H_
#define  _ESH_SCRIPT_PUBLIC_H_

#define  MAX_SOURCE_SIZE  (16 * 4096)       /*  Source file max size    */
#define  FILENAMELEN      128               /*  File name length        */
#define  LINELIM          4096              /*   size of one line       */

#define  TTURE     1                        /*  ture                    */
#define  FFALSE    0                        /*  false                   */

#define	CMASK	0377
#define	QUOTE	0200
#define	QMASK	(CMASK & ~QUOTE)
#define	NOT	'!'					            /* might use ^ */


/* Set DEBUG printf information level */
//#define  SCRIPT_DEBUG  9

#ifdef   SCRIPT_DEBUG
static int script_debug_level = SCRIPT_DEBUG;
#define DBGPRINTF(x)	if (script_debug_level > 0) Esh_printf x
#define DBGPRINTF0(x)	if (script_debug_level > 0) Esh_printf x
#define DBGPRINTF1(x)	if (script_debug_level > 1) Esh_printf x
#define DBGPRINTF2(x)	if (script_debug_level > 2) Esh_printf x
#define DBGPRINTF3(x)	if (script_debug_level > 3) Esh_printf x
#define DBGPRINTF4(x)	if (script_debug_level > 4) Esh_printf x
#define DBGPRINTF5(x)	if (script_debug_level > 5) Esh_printf x
#define DBGPRINTF6(x)	if (script_debug_level > 6) Esh_printf x
#define DBGPRINTF7(x)	if (script_debug_level > 7) Esh_printf x
#define DBGPRINTF8(x)	if (script_debug_level > 8) Esh_printf x
#define DBGPRINTF9(x)	if (script_debug_level > 9) Esh_printf x

#else
#define DBGPRINTF(x)
#define DBGPRINTF0(x) ((void)0)
#define DBGPRINTF1(x) ((void)0)
#define DBGPRINTF2(x) ((void)0)
#define DBGPRINTF3(x) ((void)0)
#define DBGPRINTF4(x) ((void)0)
#define DBGPRINTF5(x) ((void)0)
#define DBGPRINTF6(x) ((void)0)
#define DBGPRINTF7(x) ((void)0)
#define DBGPRINTF8(x) ((void)0)
#define DBGPRINTF9(x) ((void)0)

#endif  /* SCRIPT_DEBUG */

#endif               /* _ESH_SCRIPT_PUBLIC_H_ */
