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
* File    : doinsmod.c
*
* By      : Steven
* Version : v1.0
* Date    : 2008-9-11 13:29:27
**********************************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
**********************************************************************************************************************
*                                               __exec_insmod
*
* Description:  insert one mod
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static void __exec_insmod(const char *drvfile)
{
    if(eLIBs_strcmp(drvfile, "\\drv\\nand.drv") == 0)
    {
        __dev_dskinfo_t dskinfo;
        __dev_blkinfo_t blkinfo[2];

        __inf(".Nand Disk Mount.....................................................................................................\n");
        eLIBs_strncpy((char *)dskinfo.name, "NAND-DISK", 16);
        dskinfo.n = 2;
        dskinfo.info = blkinfo;
        blkinfo[0].headnum   = 2;
        blkinfo[0].secnum    = 4;
        blkinfo[0].partsize  = 32 * 1024 * 2;
        blkinfo[1].hiddennum = 0;
        blkinfo[1].headnum   = 2;
        blkinfo[1].secnum    = 4;
        blkinfo[1].partsize  = 0;

        esDEV_Insmod("\\drv\\nand.drv", 0, (void *)&dskinfo);

    }
    else if(eLIBs_strcmp(drvfile, "\\drv\\sdmmc.drv") == 0)
    {
        __dev_dskinfo_t dskinfo;
        __dev_blkinfo_t blkinfo[1];

        __inf(".Nand Disk Mount.....................................................................................................\n");
        eLIBs_strncpy((char *)dskinfo.name, "SDMMC-DISK", 16);
        dskinfo.n = 1;
        dskinfo.info = blkinfo;
        blkinfo[0].hiddennum = 0;
        blkinfo[0].headnum   = 0;
        blkinfo[0].secnum    = 0;
        blkinfo[0].partsize  = 0;

        esDEV_Insmod("\\drv\\sdmmc.drv", 0, (void *)&dskinfo);
    }
    else
    {
        esDEV_Insmod((char *)drvfile, 0, 0);
    }
    __inf(".Driver:%s Installed!...........................................................................................\n", drvfile);
}

/*
*****************************************************************************
*                            doinsmod
*
*  Description: insert one mod
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int doinsmod(struct op *t , const char **args)
{
    char *DrvFile;

    /* check input arguments */
    if ( (NULL == args[1]) )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    /* get drive file name */
    DrvFile = (char *)args[1];

    /* do insmod core: write by Steven */
    __exec_insmod(DrvFile);

    /* no any error */
    return DOSUCCEED;
}
