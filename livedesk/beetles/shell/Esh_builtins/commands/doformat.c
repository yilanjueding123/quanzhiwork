/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : doformat.c
* By      : Sunny
* Version : V1.00                                                                       2009.6.30
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                            doformat
*
*  Description: format specified disk
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int doformat(struct op *t , const char **args)
{
    ES_FMTPARA FmtData;

    Esh_memset(&FmtData, 0, sizeof(ES_FMTPARA));

    /* check input app name */
    if (NULL == args[1] || NULL == args[2])
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }

    /* save format label info */
    if (args[3])
    {
        Esh_strncpy(FmtData.label, args[3], sizeof(FmtData.label));
    }
    else
    {
        Esh_memset(FmtData.label, 0, sizeof(FmtData.label));
    }

    if (eLIBs_format(args[1], args[2], (__hdle)(&FmtData)))
    {
        Esh_Error("eLIBs_format failed!\n");
        return DOFAILED;
    }
    /* need format */
    return DOSUCCEED;
}
