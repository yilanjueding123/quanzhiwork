/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : doformatcheck.c
* By      : Sunny
* Version : V1.00                                                                       2009.6.30
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                            doformatcheck
*
*  Description: check need format or not
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int doformatcheck(struct op *t , const char **args)
{
    /* check input app name */
    if (NULL == args[1])
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    if (eLIBs_IsPartFormated(args[1]))
    {
        return DOFAILED;
    }
    /* need format */
    return DOSUCCEED;
}
