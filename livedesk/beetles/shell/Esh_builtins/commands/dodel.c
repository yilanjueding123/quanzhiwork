/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : dodel.c
* By      : sunny
* Version : V2.00                                                                         2009.3.3
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                            dodel
*
*  Description: delete a file
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int dodel(struct op *t, const char **args)
{
    char *filename;

    /* check input file name */
    if ( NULL == args[1] )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    /* get file name */
    filename = Esh_get_name(args[1]);

    if ( NULL == filename )
    {
        Esh_Error("Getname : %s failed\n", args[1]);
        return DOFAILED;
    }
    /* delete file */
    if ( Esh_remove(filename) )
    {
        Esh_Error("Delete file: \"%s\" failed\n", filename);
        Esh_Global->ErrorNum = ESH_ERROR_ACCESS_DENIED;
        Esh_free_name(filename);
        return DOFAILED;
    }
    Esh_free_name(filename);

    /* no any error */
    return DOSUCCEED;
}
