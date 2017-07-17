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
*                                               startcpustat
*
* Description: start cpu stat.
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int startcpustat(struct op *t , const char **args)
{
    esKRNL_CPUStatStart();
    return DOSUCCEED;
}


/*
**********************************************************************************************************************
*                                               stopcpustat
*
* Description: stop cpu stat.
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int stopcpustat(struct op *t , const char **args)
{
    esKRNL_CPUStatStop();
    return DOSUCCEED;
}


/*
**********************************************************************************************************************
*                                               reportcpustat
*
* Description: report cpu stat.
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int reportcpustat(struct op *t , const char **args)
{
    esKRNL_CPUStatReport();
    return DOSUCCEED;
}

