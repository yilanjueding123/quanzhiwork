/*
********************************************************************************************************************
*                                              usb device
*
*                              (c) Copyright 2007-2009, javen.China
*										All	Rights Reserved
*
* File Name 	: usbd_to_usbm.c
*
* Author 		: javen
*
* Version 		: 1.0
*
* Date 			: 2009.05.27
*
* Description 	: usbd和usb_monitor的交互窗口
*
* History 		:
*
********************************************************************************************************************
*/
#include  "usb_host_config.h"
#include  "usb_host_base_types.h"
#include  "usbh_to_usbm.h"

static ES_FILE *usbm_hdle = NULL;

/*
*********************************************************************
*                     init_usbm_op
*
* Description:
*    打开usb_monitor设备
* Arguments:
*    void
* Returns:
*    x_ok 	:   success
*    x_fail :   fail
* note:
*    无
*
*********************************************************************
*/
s32 init_usbm_op(void)
{
    usbm_hdle = NULL;

    usbm_hdle = eLIBs_fopen("b:\\HWSC\\hwsc", "r");
    if(usbm_hdle == NULL)
    {
        DMSG_PANIC("ERR: init_usbm_op: eLIBs_fopen failed\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

/*
*********************************************************************
*                     exit_usbm_op
*
* Description:
*    关闭usb_monitor设备
* Arguments:
*    void
* Returns:
*    x_ok 	:   success
*    x_fail :   fail
* note:
*    无
*
*********************************************************************
*/
s32 exit_usbm_op(void)
{
    s32 ret = 0;

    if(usbm_hdle == NULL)
    {
        DMSG_PANIC("ERR: init_usbm_op: usbm_hdle == NULL, failed\n");
        return EPDK_FAIL;
    }
    ret = eLIBs_fclose(usbm_hdle);
    if(ret == EPDK_FAIL)
    {
        DMSG_PANIC("ERR: exit_usbm_op: eLIBs_fclose failed\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

/*
*********************************************************************
*                     usbm_sendcmd
*
* Description:
*    给usb_monitor发送命令
* Arguments:
*    cmd  : input. command
* Returns:
*    x_ok 	:   success
*    x_fail :   fail
* note:
*    无
*
*********************************************************************
*/
s32 usbm_sendcmd(u32 cmd, void *p_buff)
{
    s32 ret = 0;

    if(usbm_hdle == NULL)
    {
        DMSG_PANIC("ERR: init_usbm_op: usbm_hdle == NULL, failed\n");
        return EPDK_FAIL;
    }

    ret = eLIBs_fioctrl(usbm_hdle, cmd, 0, p_buff);
    if(ret == EPDK_FAIL)
    {
        DMSG_PANIC("ERR: usbm_sendcmd: execute cmd(0x%x) failed\n", cmd);
        return EPDK_FAIL;
    }

    return EPDK_OK;
}


