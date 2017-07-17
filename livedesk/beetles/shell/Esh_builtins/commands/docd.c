/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : docd.c
* By      : sunny
* Version : V2.00                                                                         2009.3.3
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                              docd()
*
*  Description:Change current work directory
*   CD ..   :   change work directroy upper
*   CD \    :   change to root directory one time
*   CD <dir>:   change work directory to new directory
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int docd(struct op *t, const char **args)
{
    int  i;
    int  PWDirLen;
    ES_DIR *dirpt;

    char *CdDirFullName;
    const char *NewDir;
    NewDir = args[1];
    CdDirFullName = NULL;

    if ( NULL == NewDir )                               /*No argument*/
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;

    }
    else if ( !Esh_strcmp(NewDir, "..") )                /*CD ..*/
    {

        PWDirLen = Esh_strlen(Esh_Global->PWdir);
        for ( i = PWDirLen; i >= 0; i-- )
        {

            /* find last directory begin slash "\\" */
            if ( '\\' == Esh_Global->PWdir[i] )
            {
                Esh_Global->PWdir[i] = '\0';             /* cut down the end of "Esh_Global->PWdir" */
                break;
            }
        }

    }
    else if ( '\\' == NewDir[0] )                /* CD  \ */
    {

        PWDirLen = Esh_strlen(Esh_Global->PWdir);
        for ( i = 0; i < PWDirLen; i++ )
        {

            /* find first directory end slash "\\" */
            if ( '\\' == Esh_Global->PWdir[i] )
            {
                Esh_Global->PWdir[i] = '\0';             /* cut down the end of "Esh_Global->PWdir" */
                break;
            }
        }
    }
    else                                                /* Normal cd: change Esh_Global->PWdir to specific dir*/
    {

        /* get directory name */
        CdDirFullName = Esh_get_name(args[1]);

        if ( NULL == CdDirFullName )
        {
            Esh_Error("Getname : %s failed\n", args[1]);
            return DOFAILED;
        }
        /* try to open new directory */
        dirpt = Esh_opendir(CdDirFullName);
        if ( NULL == dirpt )
        {
            Esh_Error("Open: path \"%s\" failed!\nPlease check argument\n", CdDirFullName);
            Esh_Global->ErrorNum = ESH_ERROR_PATH_NOT_FOUND;
            if ( CdDirFullName )
                Esh_free_name(CdDirFullName);
            return DOFAILED;
        }
        Esh_closedir(dirpt);
        Esh_strcpy(Esh_Global->PWdir, CdDirFullName);
    }
    if ( CdDirFullName )
        Esh_free_name(CdDirFullName);

    /* no any error */
    return DOSUCCEED;
}
