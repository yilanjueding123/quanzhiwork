/*
*************************************************************************************
*                         		             Melis
*					                    USB Host Driver
*
*                          (c) Copyright 2006-2010, All winners Co,Ld.
*							           All Rights Reserved
*
* File Name 	: host0_drv.c
*
* Author 		: javen
*
* Description 	:
*
* History 		:
*      <author>    		<time>       	<version >    		<desc>
*       javen     	  2010-12-8            1.0          create this file
*
*************************************************************************************
*/

#include  "..\usb_host_config.h"
#include  "usb_os_platform.h"
#include  "..\usbh_drv\usbh_drv.h"
#include  "host0_drv.h"

static usb_host_drv_t host0_drv;

/*
*******************************************************************************
*                     usb_host0_Init
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
__s32 usb_host0_Init(void)
{
    USB_OS_MEMSET(&host0_drv, 0, sizeof(usb_host_drv_t));

    return EPDK_OK;
}

/*
*******************************************************************************
*                     usb_host0_Exit
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
__s32 usb_host0_Exit(void)
{
    USB_OS_MEMSET(&host0_drv, 0, sizeof(usb_host_drv_t));

    return EPDK_OK;
}

/*
*******************************************************************************
*                     usb_host0_Open
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
__mp  *usb_host0_Open(__u32 mid, __u32 mode)
{
    USB_OS_MEMSET(&(host0_drv.my_mp), 0, sizeof(__mp));
    host0_drv.my_mp.mid =  mid;

    return &(host0_drv.my_mp);
}

/*
*******************************************************************************
*                     usb_host0_Close
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
__s32 usb_host0_Close(__mp *mp)
{
    USB_OS_MEMSET(&(host0_drv.my_mp), 0, sizeof(__mp));

    return EPDK_OK;
}

/*
*******************************************************************************
*                     usb_host0_Read
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
__u32 usb_host0_Read(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    DMSG_PANIC("ERR: usb host0 driver read is not support\n");

    return 0;
}

/*
*******************************************************************************
*                     usb_host0_Write
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
__u32 usb_host0_Write(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    DMSG_PANIC("ERR: usb host0 driver write is not support\n");

    return 0;
}

/*
*******************************************************************************
*                     usb_host0_Ioctrl
*
* Description:
*    void
*
* Parameters:
*    mp      :  设备句柄
*    cmd     :  命令
*    aux     :  设备类型
*    pbuffer :  设备参数
*
* Return value:
*    0  :  成功
*   !0  :  失败
*
* note:
*    void
*
*******************************************************************************
*/
__s32 usb_host0_Ioctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    switch(cmd)
    {
    case  DRV_CMD_PLUGIN:
        host0_drv.ioctrl_counter ++;

        /* 驱动初始化 */
        if(host0_drv.First_Plugin == 0)
        {
            host0_drv.First_Plugin++;
            host_drv_Init(aux);
        }
        break;

    case  DRV_CMD_PLUGOUT:
        if(host0_drv.ioctrl_counter)
        {
            host0_drv.ioctrl_counter--;
        }

        /* 如果 DRV_CMD_PLUGIN 和 DRV_CMD_PLUGOUT 对称， 并且host_Init已经了执行，
           那么就可以注销驱动了。
        */
        if(host0_drv.ioctrl_counter == 0 && host0_drv.First_Plugin)
        {
            host0_drv.First_Plugin = 0;
            host_drv_Exit(aux);
        }
        break;

    case DRV_CMD_GET_STATUS:
        if(host0_drv.ioctrl_counter > 0 )
        {
            return DRV_STA_BUSY;
        }
        else if(host0_drv.ioctrl_counter == 0)
        {
            return DRV_STA_FREE;
        }
    //break;

    default:
        DMSG_PANIC("ERR:  cmd(%x) error\n", cmd);
        return  -1;
    }

    return 0;
}

