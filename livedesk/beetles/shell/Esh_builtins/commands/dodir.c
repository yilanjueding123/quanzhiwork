/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : dodir.c
* By      : sunny
* Version : V2.00                                                                         2009.3.3
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

static int __Show_Directory_Content(char *dirname)
{
    ES_DIR		*dirp;
    ES_DIRENT   *direntp;
    char         namebuf[32];
    __int64      size;

    int file_num = 0;
    int dir_num = 0;

    if ( NULL == dirname )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    Esh_printf("Directory -- %s: \n", dirname);
    Esh_printf("    Attributes     Name \n");
    dirp = Esh_opendir(dirname);

    if ( NULL == dirp )
    {
        Esh_Wrn("open directory fail!\n");
        Esh_Global->ErrorNum = ESH_ERROR_OPEN_DIR_FAIL;
        return DOFAILED;
    }
    while (  NULL != (direntp = Esh_readdir(dirp)) )
    {

        char attr[7];

        attr[0] = (direntp->fatdirattr & FSYS_ATTR_READONLY)  ? 'r' : '-';
        attr[1] = (direntp->fatdirattr & FSYS_ATTR_HIDDEN)    ? 'h' : '-';
        attr[2] = (direntp->fatdirattr & FSYS_ATTR_SYSTEM)    ? 's' : '-';
        attr[3] = (direntp->fatdirattr & FSYS_FAT_VOLUME_ID)  ? 'v' : '-';
        attr[4] = (direntp->fatdirattr & FSYS_ATTR_DIRECTORY) ? 'd' : '-';
        attr[5] = (direntp->fatdirattr & FSYS_ATTR_ARCHIVE)   ? 'a' : '-';
        attr[6] = '\0';

        if ( direntp->fatdirattr & FSYS_ATTR_DIRECTORY )
        {
            dir_num++;
        }
        else
        {
            file_num++;
        }
        /* '?' '*' default postfix,compensate others forwards */

        Esh_printf("    %s             %s \n", attr, direntp->d_name);

    }
    Esh_closedir(dirp);

    Esh_printf("\n    file      number: %d\n", file_num);
    Esh_printf("    directory number: %d\n", dir_num);

    eLIBs_GetVolName(dirname, namebuf);
    Esh_printf("    [volname: %s  ", namebuf);
    eLIBs_GetFSName(dirname, namebuf);
    Esh_printf("    filesystem: %s]\n", namebuf);

    size = eLIBs_GetVolClustSize(dirname);
    Esh_printf("    [cluster size: %dB", (int)size);
    size = eLIBs_GetVolTSpace(dirname);
    Esh_printf("    driver size: %dM", (int)size >> 20);
    size = eLIBs_GetVolFSpace(dirname);
    Esh_printf("    driver free: %dM]\n", (int)size >> 20);

    /* no any error */
    return DOSUCCEED;
}
/*
*****************************************************************************
*                            dodir
*
*  Description: show a directory content
*
*
*  Parameters:
*
*
*  Return value:
*****************************************************************************
*/
int dodir(struct op *t , const char **args)
{
    int ShowRes;
    char *DirName;

    /* show current work directory content */
    if ( NULL == args[1] )
    {
        ShowRes = __Show_Directory_Content(Esh_Global->PWdir);
        return ShowRes;
    }
    /* show directory specified by "args[1]" */
    DirName = Esh_get_name(args[1]);

    if ( NULL == DirName )
    {
        Esh_Error("Getname : %s failed\n", args[1]);

        return DOFAILED;
    }
    /* show specified directory content */
    ShowRes = __Show_Directory_Content(DirName);

    Esh_free_name(DirName);
    return  ShowRes;
}
