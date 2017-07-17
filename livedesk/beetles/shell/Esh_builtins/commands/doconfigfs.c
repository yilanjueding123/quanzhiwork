/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              apps Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : Shell
* File    : doconfigfs.c
*
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
**********************************************************************************************************************
*                                               doconfigfs
*
* Description: Config fs parameter
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int doconfigfs(struct op *t , const char **args)
{
    __s32 res;
    __s32 aux;

    aux = Esh_atoi(args[1]);

    Esh_printf("Type : %d\n", aux);
    res = esFSYS_setfs(NULL, 0, aux, NULL);
    if (EPDK_OK != res)
    {
        Esh_Error("Call comfig fs system failed\n");
        return DOFAILED;
    }
    return DOSUCCEED;
}
