/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : doshowfsize.c
* By      : sunny
* Version : V2.00                                                                         2009.7.6
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                              doshowfsize()
*
*  Description: show size of a file
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int doshowfsize(struct op *t, const char **args)
{
    __s64 filesize;
    char *filename;

    if ( NULL == args[1] )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }

    filename = Esh_get_name(args[1]);

    filesize = eLIBs_GetFileSize(filename);
    if (filesize < 0)
    {
        Esh_Error("GetDirSize failed\n");
        Esh_free_name(filename);
        return DOFAILED;
    }
    Esh_printf("File \"%s\" size : %u\n", filename, (__u32)filesize);
    Esh_free_name(filename);
    return DOSUCCEED;
}
