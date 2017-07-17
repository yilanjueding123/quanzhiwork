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
* File    : Esh_script_external_var.h
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#ifndef   _ESH_SCRIPT_EXTERNAL_VAR_H_
#define   _ESH_SCRIPT_EXTERNAL_VAR_H_

/* lexer skan */
extern  char *g_SourceCode;         /*  Source stream ptr */
extern  char *g_SourceCodeBase;     /*  bottom ptr of Source stream */
extern  char *g_SourceCodeTop;      /*  top ptr of source stream */
extern  int startl;                 /*  start of new command */
extern  int peeksym;                /*  prestrore a yylex skan word */
extern  char *yylvalcp;
extern  int linenumber;             /*  record number of line in a script */

extern  char line[LINELIM];
extern  char *linep;
extern  char *elinep;               /* done in main(): = line + sizeof(line) - 5 */

extern  int multiline;              /* '\n' changed to ';' (counter) */

/* syntax parser */
extern  struct op *outtree;         /* result from parser */
extern  int yynerrs;                /* Parser error flag */
extern  int IsBreakOut;

/* word */
extern  struct wdblock *wdlist;

/* var */
extern  struct var *vlist;		    /* dictionary : var list */

/* area */
extern  int areanum;                /* current allocation area */

/* other */
extern  int  gflg;                  /* Global flag ,indicate line buffer overflow */
extern  char *null;                 /* null value for variable */

#endif             /* _ESH_SCRIPT_EXTERNAL_VAR_H_ */
