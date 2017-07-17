/*
*********************************************************************************************************
*                                                    eMOD
*                                            the Easy Module System
*                                               drv_rtc sub-system
*
*                                   (c) Copyright 2006-20010, kevin.z China
*                                              All Rights Reserved
*
* File   : drv_rtc.h
* Version: v1.0
* By     : kevin.z.m
* Date   : 2010-6-20 15:08
*********************************************************************************************************
*/
#ifndef _DRV_RTC_H_
#define _DRV_RTC_H_

#include "typedef.h"
#include "cfgs.h"

typedef enum __RTC_INT_TYPE
{
    RTC_INT_TYPE_ALARM,     /* alarm interrupt  */
    RTC_INT_TYPE_TIMER,     /* timer interrupt  */
    RTC_INT_TYPE_CNTER,     /* conter interrupt */
    RTC_INT_TYPE_

} __rtc_int_type_e;



typedef enum __RTC_DEVICE_CMD_SET
{
    RTC_CMD_GET_TIME,       /* get current time */
    RTC_CMD_SET_TIME,       /* set current time */

    RTC_CMD_GET_DATE,       /* get current date */
    RTC_CMD_SET_DATE,       /* set current date */

    RTC_CMD_REQUEST_ALARM,  /* request alarm    */
    RTC_CMD_RELEASE_ALARM,  /* release alarm    */
    RTC_CMD_START_ALARM,    /* start alarm      */
    RTC_CMD_STOP_ALARM,     /* stop alarm       */
    RTC_CMD_QUERY_ALARM,    /* query alarm      */

    RTC_CMD_QUERY_INT,      /* qurey rtc interrupt  */
    RTC_CMD_SETCLOCKOUT,    /* set rtc clock out*/

    RTC_CMD_

} __rtc_device_cmd_set_e;


#endif


