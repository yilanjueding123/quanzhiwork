/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : fs.c
* By      : peter
* Version : V1.00
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                            __exec_fs
*
*  Description:file system contrl
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
static void __exec_fs(const char *cmd, const char *para)
{
    /* check cmd */
    if ( NULL == cmd )
    {
        return;
    }

    esFSYS_fsdbg(cmd, para);
}
/*
*****************************************************************************
*                            dofs
*
*  Description: file system contrl
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int dofs(struct op *t , const char **args)
{
    /* do fs core: write by peter */
    __exec_fs(args[1], args[2]);

    /* no any error */
    return DOSUCCEED;
}

