/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Standby Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : standby_usrevent.c
* By      : kevin.z
* Version : v1.0
* Date    : 2010-7-8 13:16
* Descript: process some user event for wake up system standby.
* Update  : date                auther      ver     notes
*********************************************************************************************************
*/
#include "standby_i.h"
#include "standby_cfg.h"
#include ".\spi_key\standby_iokey.h"


#if(ALLOW_USREVENT_WAKEUP_STANDBY)


/*
*********************************************************************************************************
*                                   USER EVENT DETECT INITIATE
*
* Description: user event detect initiate.
*
* Arguments  : none.
*
* Returns    : result.
*               EPDK_OK,    user event initiate successed.
*               EPDk_FAIL,  user event initiate failed.
*
* Note       :
*********************************************************************************************************
*/
__s32 standby_usrevent_init(void)
{
#if(ALLOW_AC320_RUN_HOSC)
    IOKEY_StandbyInit(24000000);
#else
    IOKEY_StandbyInit(32000);
#endif

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   USER EVENT DETECT EXIT
*
* Description: user event detect exit.
*
* Arguments  : none.
*
* Returns    : result.
*               EPDK_OK,    user event exit successed.
*               EPDk_FAIL,  user event exit failed.
*
* Note       :
*********************************************************************************************************
*/
__s32 standby_usrevent_exit(void)
{
    IOKEY_StandbyExit();
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   QUERY USER EVENT
*
* Description: query user event.
*
* Arguments  : none.
*
* Returns    : result.
*               EPDK_TRUE,  user event happend.
*               EPDK_FALSE, user event didn't happen.
*
* Note       :
*********************************************************************************************************
*/
__s32 standby_usrevent_query(void)
{
    if(IOKEY_Wakeup() == EPDK_TRUE)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}


#endif  //ALLOW_USREVENT_WAKEUP_STANDBY


