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
* File    : Esh_script_lexer.h
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#ifndef _ESH_SCRIPT_LEXER_H_
#define _ESH_SCRIPT_LEXER_H_

#define MAX_SOURCE_LINE_SIZE        1024        // Maximum source line length

/* flags to yylex */
#define	CONTIN 01     /* skip new lines to complete command */

/* resaved key words */
#define WORD    256
#define LOGAND  257
#define LOGOR   258
#define BREAK   259
#define IF      260
#define THEN    261
#define ELSE    262
#define ELIF    263
#define FI      264
#define CASE    265
#define ESAC    266
#define FOR     267
#define WHILE   268
#define UNTIL   269
#define DO      270
#define DONE    271
#define IN      272
#define JUMP    273
/* Added for "." file expansion */
#define DOT     274

/* external function */
extern   int  yylex(int cf);
extern   int  my_getc(int ec);
extern   void unget(int c);
extern   int  readc(void);


#endif   /* _ESH_SCRIPT_LEXER_H_ */
