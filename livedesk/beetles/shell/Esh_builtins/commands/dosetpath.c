/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : dosetpath.c
* By      : sunny
* Version : V2.00                                                                         2009.3.4
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                            dosetpath()
*
*  Description: set app seach path
*
*
*  Parameters:
*
*  Return value:
*****************************************************************************
*/
int dosetpath(struct op *t, const char **args)
{
    ES_DIR	*dirpt;

    if ( NULL == args[1] )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    /* open path for check */
    dirpt = Esh_opendir(args[1]);
    if ( NULL == dirpt )
    {
        Esh_Error("Open path : \"%s\" failed!\n", args[1]);
        return DOFAILED;
    }
    Esh_closedir(dirpt);

    /* set new path */
    Esh_strcpy(Esh_Global->AppPath1, args[1]);

    return DOSUCCEED;
}
