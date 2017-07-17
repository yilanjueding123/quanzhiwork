/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              Apps-Shell Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : mount
* File    : mount.c
*
* By      : sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
**********************************************************************************************************************
*                                               domount
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int domount(struct op *t , const char **args)
{
    char *ImgFile;

    /* check input arguments */
    if ( (NULL == args[1]) )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    /* get file name */
    ImgFile = (char *)args[1];

    /* install driver and open it */
    esDEV_Insmod("\\drv\\filedisk.drv", 0, (void *)ImgFile);
    Esh_printf(".File Disk \"%s\" Installed!....................................................\n", ImgFile);

    /* no any error */
    return DOSUCCEED;
}
