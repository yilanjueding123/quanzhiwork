/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : doquit.c
* By      : Steven
* Version : V1.00
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                            doquit
*
*  Description: quit shell
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int doquit(struct op *t , const char **args)
{

    /* just set exit flag ture */
    Esh_Global->ExitFlag = ESH_TURE;

    /* no any error */
    return DOSUCCEED;
}
