/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : doreset.c
* By      : sunny
* Version : V2.00                                                                         2009.3.3
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                            doreset
*
*  Description:Reset the system
*
*
*  Parameters: None
*
*
*  Return value:None
*****************************************************************************
*/
int doreset(struct op *t , const char **args)
{
    /* call system reset */
    Esh_printf("Reset system....................................................\n");
    esKSRV_Reset();

    return DOSUCCEED;
}
