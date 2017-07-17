/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : dohistory.c
* By      : sunny
* Version : V2.00                                                                         2009.3.3
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                            dohistory
*
*  Description: show command history records
*
*
*  Parameters:
*
*
*  Return value:None
*****************************************************************************
*/
int dohistory(struct op *t , const char **args)
{
    int i;
    cmd_history *pt;

    /* set command record table pointer ot 'pt' */
    pt = Esh_CMD_Table;

    Esh_printf("The history command:\n\n");
    for(i = 0; i < Esh_MAX_HISTORY_LEN; i++)
    {
        if ( *(pt->cmd) != '\0' )
            Esh_printf("      %s\n", pt->cmd);
        pt = pt->next;
    }
    Esh_printf("\n\n\n");

    return DOSUCCEED;
}
