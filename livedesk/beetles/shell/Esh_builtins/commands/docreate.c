/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : docreate.c
* By      : sunny
* Version : V2.00                                                                         2009.3.3
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                            docreate
*
*  Description: Creat a file
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int docreate(struct op *t, const char **args)
{
    ES_FILE *fp;
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
    /* create file */
    fp = Esh_fopen(filename, "w+");
    if ( NULL == fp )
    {
        Esh_Error("Create file: \"%s\" failed\n", filename);
        Esh_Global->ErrorNum = ESH_ERROR_ACCESS_DENIED;
        Esh_free(filename);
        return DOFAILED;
    }
    Esh_fclose(fp);
    Esh_free_name(filename);

    /* no any error */
    return DOSUCCEED;
}
