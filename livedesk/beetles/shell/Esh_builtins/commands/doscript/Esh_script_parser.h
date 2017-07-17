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
* File    : Esh_script_pasrer.h
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/
/*
 * Description of a command or an operation on commands.
 * Might eventually use a union.
 */
#ifndef  _ESH_SCRIPT_PARSER_H_
#define  _ESH_SCRIPT_PARSER_H_

struct op
{
    int op_type;               /* operation type, see Txxxx below */
    char **op_words;           /* arguments to a command */
    struct op *left;           /* op left ptr */
    struct op *right;          /* op right ptr */
    char *str;                 /* identifier for FOR and CASE */
};

#define TCOM    1       /* command */
#define TPAREN  2       /* (c-list) */
#define TPIPE   3       /* a | b */
#define TLIST   4       /* a [&;] b */
#define TOR     5       /* || */
#define TAND    6       /* && */
#define TFOR    7
#define TDO     8
#define TCASE   9
#define TIF     10
#define TWHILE  11
#define TUNTIL  12
#define TELIF   13
#define TPAT    14      /* pattern in case */
#define TBRACE  15      /* {c-list} */

#define TDOT    17      /* Added to support "." file expansion */


/*
 * parsr components
 */
#define	NOBLOCK	((struct op *)NULL)
#define	NOWORD	((char *)NULL)
#define	NOWORDS	((char **)NULL)
#define	NOPIPE	((int *)NULL)

static const char *const T_CMD_NAMES[] =
{
    "PLACEHOLDER",
    "TCOM",
    "TPAREN",
    "TPIPE",
    "TLIST",
    "TOR",
    "TAND",
    "TFOR",
    "TDO",
    "TCASE",
    "TIF",
    "TWHILE",
    "TUNTIL",
    "TELIF",
    "TPAT",
    "TBRACE",
};

/* -------- word.h -------- */

#define	NSTART	16				/* default number of words to allow for initially */

struct wdblock
{
    short w_bsize;         //initial number of words
    short w_nword;         //the numbers of words
    char *w_words[1];      //words
};

//struct wdblock *iolist;      //iolist

/* external function */
extern  int    EndOfFile(void);
extern  void   zzerr(void);
extern  int    yyparse(void);
extern  struct wdblock *addword(char *wd, struct wdblock *wb);
extern  char **getwords(struct wdblock *wb);


#endif   /* _ESH_SCRIPT_PARSER_H_ */
