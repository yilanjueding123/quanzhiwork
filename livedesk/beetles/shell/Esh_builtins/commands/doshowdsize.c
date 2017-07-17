/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : doshowdsize.c
* By      : sunny
* Version : V2.00                                                                         2009.7.3
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                              doshowdsize()
*
*  Description: show size of directory
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int doshowdsize(struct op *t, const char **args)
{
    __s64 dirsize;
    char *dirpath;

    if ( NULL == args[1] )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    dirpath = Esh_get_name(args[1]);

    dirsize = eLIBs_GetDirSize(dirpath);

    if (dirsize < 0)
    {
        if (dirsize == -1)
            Esh_Error("GetDirSize : recursion level too deep\n");

        if (dirsize == -2)
            Esh_Error("GetDirSize : malloc failed\n");

        if (dirsize == -3)
            Esh_Error("GetDirSize : open diretory failed\n");

        Esh_free_name(dirpath);
        return DOFAILED;
    }
    Esh_printf("Directory \"%s\" total size : %u\n", dirpath, (__u32)dirsize);
    Esh_free_name(dirpath);
    return DOSUCCEED;
}
