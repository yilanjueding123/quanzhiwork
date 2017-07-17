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
* File    : Esh_script_onecommand.c
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#include  "doscript_private.h"

/*
 * get one command, execute it
 * call yyparse: syntax parser
 * call execute: execute it
 */
int multiline;              /* '\n' changed to ';' (counter) */

int onecommand(void)
{
    //    int ExecuteResult = 0;

    areanum = 1;
    freearea(areanum);
    garbage();
    wdlist = NULL;
    yynerrs = 0;
    multiline = 0;
    IsBreakOut = 0;    //isbreak set to zero for loop control

    /* parser source script and execute it */
    if ( yyparse() )   //have syntax error in parser
    {
        return FFALSE;
    }

    //	ExecuteResult = execute(outtree, NOPIPE, NOPIPE, /* no_fork: */ 0);
    execute(outtree, NOPIPE, NOPIPE, /* no_fork: */ 0);
    return TTURE;
}
