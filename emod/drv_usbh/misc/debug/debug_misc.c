/*
********************************************************************************************************************
*                                              USB Host Drvier
*
*                              (c) Copyright 2007-2009, softwinners.China
*										All	Rights Reserved
*
* File Name 	: platform_debug.h
*
* Author 		: javen
*
* Version 		: 1.0
*
* Date 			: 2009.10.12
*
* Description 	:
*
* History 		:
*
********************************************************************************************************************
*/
#include "usbh_debug.h"

/*
*******************************************************************************
*                     print_buffer_1
*
* Description:
*    一个一个字节打印
*
* Parameters:
*
*
* Return value:
*
*
* note:
*
*
*******************************************************************************
*/
void print_buffer_1(__u8 *buffer, __u32 len, char *str)
{
    __u32 i = 0;

    for(i = 0; i < len; i++)
    {
        if(((i % 6) == 0) && i != 0)
        {
            DMSG_INFO("\n");
        }

        DMSG_INFO("%x", buffer[i]);
    }
}

/*
*******************************************************************************
*                     print_buffer_4
*
* Description:
*    每4个字节打印一次
*
* Parameters:
*
*
* Return value:
*
*
* note:
*
*
*******************************************************************************
*/
void print_buffer_4(__u8 *buffer, __u32 len, char *str)
{
    __u32 i = 0;

    DMSG_INFO("\n----------------------%s---------------\n", str);
    for(i = 0; i < len / 4; i++)
    {
        DMSG_INFO("%x", ((__u32 *)buffer)[i]);
    }
    DMSG_INFO("\n---------------------------------------\n");
}


