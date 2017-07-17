/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           calendar app sample
*
*						        	(c) Copyright 2006-2009, ANDY, China
*											 All Rights Reserved
*
* File    : adjust.c
* By      :
* Version : V1.00
*********************************************************************************************************
*/

#include "..\\mod_init_i.h"


/*
**********************************************************************************************************************
*                                            tpdrv_open
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

ES_FILE *tpdrv_open(void)
{
    ES_FILE  *hdle = NULL;

    hdle = eLIBs_fopen("b:\\input\\TP", "r+");
    if(!hdle)
    {
        __wrn("open tp dev file fail\n");

        return NULL;
    }
    return hdle;
}

/*
**********************************************************************************************************************
*                                            tpdrv_close
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

__s32 tpdrv_close(ES_FILE *hdle)
{
    if(hdle)
    {
        eLIBs_fclose(hdle);
    }
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                            tp_adjust
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

__s32 tp_adjust(ES_FILE *hdle, __u8 cnt, void *pbuf)
{
    __s32	ret;

    ret = eLIBs_fioctrl(hdle, DRV_TP_CMD_ADJUST, cnt, pbuf);

    return ret;
}


