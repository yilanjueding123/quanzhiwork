/*
********************************************************************************************************************
*                                              usb_host
*
*                              (c) Copyright 2007-2009, javen.China
*										All	Rights Reserved
*
* File Name 	: usbh_disk_remove_time.c
*
* Author 		: javen
*
* Version 		: 1.0
*
* Date 			: 2010.05.03
*
* Description 	:
*
* History 		:
*
********************************************************************************************************************
*/
#include  "usb_host_config.h"
#include  "usb_host_base_types.h"
#include  "usb_os_platform.h"

#include  "usbh_disk_remove_time.h"

static __u32 usbh_disk_time[USBH_DISK_REMOVE_TIME_UNIT];

/*
*********************************************************************
*                     usbh_disk_save_time
*
* Description:
*
* Arguments:
*    void
* Returns:
*    void
* note:
*    无
*
*********************************************************************
*/
void usbh_disk_save_time(__u32 time)
{
    __u32 i = 0;

    for(i = 0; i < USBH_DISK_REMOVE_TIME_UNIT; i++)
    {
        if(usbh_disk_time[i] == 0)
        {
            usbh_disk_time[i] = time;
            break;
        }
    }

    return;
}

/*
*********************************************************************
*                     usbh_disk_del_time
*
* Description:
*
* Arguments:
*    void
* Returns:
*    void
* note:
*    无
*
*********************************************************************
*/
void usbh_disk_del_time(__u32 time)
{
    __u32 i = 0;

    for(i = 0; i < USBH_DISK_REMOVE_TIME_UNIT; i++)
    {
        if(usbh_disk_time[i] == time)
        {
            usbh_disk_time[i] = 0;
            break;
        }
    }

    return;
}

/*
*********************************************************************
*                     get_max_disk_time
*
* Description:
*
* Arguments:
*    void
* Returns:
*    void
* note:
*    无
*
*********************************************************************
*/
__u32 get_max_disk_time(void)
{
    __u32 i = 0;
    __u32 max = 0;

    for(i = 0; i < USBH_DISK_REMOVE_TIME_UNIT; i++)
    {
        if(usbh_disk_time[i] > max)
        {
            max = usbh_disk_time[i];
        }
    }

    return max;
}

/*
*********************************************************************
*                     usbh_disk_time_init
*
* Description:
*
* Arguments:
*    void
* Returns:
*    void
* note:
*    无
*
*********************************************************************
*/
void usbh_disk_time_init(void)
{
    USB_OS_MEMSET(usbh_disk_time, 0, 4 * USBH_DISK_REMOVE_TIME_UNIT);
}

/*
*********************************************************************
*                     usbh_disk_time_exit
*
* Description:
*
* Arguments:
*    void
* Returns:
*    void
* note:
*    无
*
*********************************************************************
*/
void usbh_disk_time_exit(void)
{
    USB_OS_MEMSET(usbh_disk_time, 0, 4 * USBH_DISK_REMOVE_TIME_UNIT);
}

