/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              Apps-Shell Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* File    : domkdir.c
* By      : sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                            domkdir
*
*  Description: Create a directory
*
*
*  Parameters:
*
*  Return value:
*****************************************************************************
*/
int domkdir(struct op *t, const char **args)
{
    char *DirName;

    /* check input directory name */
    if ( NULL == args[1] )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    /* get directory name */
    DirName = Esh_get_name(args[1]);

    if ( NULL == DirName )
    {
        Esh_Error("Getname : %s failed\n", args[1]);
        return DOFAILED;
    }
    /* create directory */
    if ( Esh_mkdir(DirName) )
    {
        Esh_Error("Create directory: \"%s\" failed\n", DirName);
        Esh_Global->ErrorNum = ESH_ERROR_ACCESS_DENIED;
        Esh_free_name(DirName);
        return DOFAILED;
    }
    Esh_free_name(DirName);

    /* no any error */
    return DOSUCCEED;
}
