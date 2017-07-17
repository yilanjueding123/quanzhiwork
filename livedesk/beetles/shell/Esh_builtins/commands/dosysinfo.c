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
* File    : dosysinfo.c
*
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
**********************************************************************************************************************
*                                               dosysinfo
*
* Description: Show system main information
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int dosysinfo(struct op *t , const char **args)
{
    /* call system poweroff */
    esKSRV_SysInfo();

    return DOSUCCEED;
}
