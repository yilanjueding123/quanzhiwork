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
* File    : domeminfo.c
*
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
**********************************************************************************************************************
*                                               domeminfo
*
* Description: Show system memory information
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int domeminfo(struct op *t , const char **args)
{
    /* call system poweroff */
    esMEMS_Info();

    return DOSUCCEED;
}
