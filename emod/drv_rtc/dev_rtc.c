/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              rtc device module
*
*                                   (c) Copyright 2006-2010, kevin.z.m China
*                                             All Rights Reserved
*
* File    : dev_rtc.c
* By      : kevin.z.m
* Version : v1.00
* Date    : 2010-6-20 15:25
*********************************************************************************************************
*/
#include "drv_rtc_i.h"

//define rtc device handle
static __u32    rtc_dev_hdle = 0x00;



/*
*********************************************************************************************************
*                                       RTC DEVICE INIT
*
* Description: rtc device init.
*
* Arguments  : none
*
* Returns    : result;
*               EPDK_OK,    rtc init successed.
&               EPDK_FAIL,  rtc init failed.
*********************************************************************************************************
*/
__s32 rtc_Init(void)
{
    __inf("rtc device init!\n");
    RTC_Init();

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                       RTC DEVICE EXIT
*
* Description: rtc device exit.
*
* Arguments  : none
*
* Returns    : result.
*
*********************************************************************************************************
*/
__s32 rtc_Exit(void)
{
    __inf("rtc device exit!\n");
    RTC_Exit();

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                       RTC DEVICE OPEN
*
* Description: open rtc device.
*
* Arguments  : none
*
* Returns    : result.
*
*********************************************************************************************************
*/
__hdle rtc_Open  (void *open_arg, __u32 mode)
{
    rtc_dev_hdle++;
    return (__hdle)&rtc_dev_hdle;
}


/*
*********************************************************************************************************
*                                       RTC DEVICE CLOSE
*
* Description: rtc device close.
*
* Arguments  : none
*
* Returns    : result.
*
*********************************************************************************************************
*/
__s32 rtc_Close (__hdle hDev)
{
    if(rtc_dev_hdle)
    {
        rtc_dev_hdle--;
        return EPDK_OK;
    }

    __wrn("all rtc device handle have been closed!\n");
    return EPDK_FAIL;
}


/*
*********************************************************************************************************
*                                       RTC DEVICE READ
*
* Description: rtc device read.
*
* Arguments  : none
*
* Returns    : result.
*
*********************************************************************************************************
*/
__u32 rtc_Read  (void *pBuffer, __u32 size, __u32 n, __hdle hDev)
{
    return 0;
}


/*
*********************************************************************************************************
*                                       RTC DEVICE WRITE
*
* Description: rtc device write.
*
* Arguments  : none
*
* Returns    : result.
*
*********************************************************************************************************
*/
__u32 rtc_Write (const void *pBuffer, __u32 size, __u32 n, __hdle hDev)
{
    return 0;
}


/*
*********************************************************************************************************
*                                   RTC DEVICE I/O CONTROL
*
* Description: rtc device i/o interface.
*
* Arguments  : hDev     rtc handle;
*              Cmd      command;
*              Aux      optional parameter.
*              pBuffer  optional parameter.
*
* Returns    : result.
*
*********************************************************************************************************
*/
__s32 rtc_Ioctrl(__hdle hDev, __u32 Cmd, __s32 Aux, void *pBuffer)
{
    __s32   result = EPDK_OK;

    if(hDev != (__hdle)&rtc_dev_hdle)
    {
        __wrn("rtc device handle is invalid!\n");
        return EPDK_FAIL;
    }


    switch(Cmd)
    {
    case RTC_CMD_GET_TIME:
    {
        result = RTC_GetTime((__time_t *)pBuffer);
        break;
    }

    case RTC_CMD_SET_TIME:
    {
        result = RTC_SetTime((__time_t *)pBuffer);
        break;
    }

    case RTC_CMD_GET_DATE:
    {
        result = RTC_GetDate((__date_t *)pBuffer);
        break;
    }

    case RTC_CMD_SET_DATE:
    {
        result = RTC_SetDate((__date_t *)pBuffer);
        break;
    }

    case RTC_CMD_REQUEST_ALARM:
    {
        result = RTC_RequestAlarm(Aux);
        break;
    }
    case RTC_CMD_RELEASE_ALARM:
    {
        result = RTC_ReleaseAlarm((__hdle)pBuffer);
        break;
    }

    case RTC_CMD_START_ALARM:
    {
        result = RTC_StartAlarm((__hdle)pBuffer, Aux);
        break;
    }

    case RTC_CMD_STOP_ALARM:
    {
        result = RTC_StopAlarm((__hdle)pBuffer);
        break;
    }

    case RTC_CMD_QUERY_ALARM:
    {
        result = RTC_QuerryAlarm((__hdle)pBuffer);
        break;
    }

    case RTC_CMD_QUERY_INT:
    {
        result = RTC_QuerryInt(Aux);
        break;
    }

    case RTC_CMD_SETCLOCKOUT:
    {
        result = RTC_SetClockOut(Aux);
        break;
    }

    default:
    {
        __wrn("rtc device command is unrecognized!\n");
        result = EPDK_FAIL;
        break;
    }
    }

    return result;
}


__dev_devop_t rtc_ops =
{
    rtc_Open,
    rtc_Close,
    rtc_Read,
    rtc_Write,
    rtc_Ioctrl
};

