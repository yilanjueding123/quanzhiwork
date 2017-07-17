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
* File    : Esh_script_eval.h
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#ifndef  _ESH_SCRIPT_EVAL_H_
#define  _ESH_SCRIPT_EVAL_H_

/*
 * flags to control evaluation of words
 */
#define DOSUB    1      /* interpret $, quotes */
#define DOKEY    8      /* move words with `=' to 2nd arg. list */
#define DOTRIM   16     /* trim resulting string */

#define DOALL    (DOSUB|DOKEY|DOTRIM)

#define MAXARGLEN    256  /* the max length of args */

/* external function */
extern   char *evalstr(char *cp, int f);
extern   char **eval(char **ap, int f);

#endif     /* _ESH_SCRIPT_EVAL_H_ */
