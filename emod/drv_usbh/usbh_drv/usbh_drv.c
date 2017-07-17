/*
*************************************************************************************
*                         		             Melis
*					                    USB Host Driver
*
*                          (c) Copyright 2006-2010, All winners Co,Ld.
*							           All Rights Reserved
*
* File Name 	: usbh_drv.c
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

#include  "usb_host_config.h"
#include  "usb_os_platform.h"
#include  "usb_host_common.h"

#include  "usb_gen_hub.h"
#include  "usb_core_init.h"
#include  "usbh_to_usbm.h"

#include  "usbh_debug.h"
#include  "usb_msc.h"
#include  "usbh_disk_remove_time.h"

#include  "Hid.h"

#include  "memory_allot.h"
#include  "horse.h"
#include  "usbh_drv.h"

extern s32 hcd_init(void);
extern s32 hcd_exit(void);
extern s32 hcd_open_clk(void);
extern s32 hcd_close_clk(void);
extern void hcd_stop(__u32 time);
extern s32 usbh_set_usbd_port(__u32 port_num);

/*
*******************************************************************************
*                     host_Init
*
* Description:
*    驱动初始化
*
* Parameters:
*    usbc_no  :  input.  USB 控制器编号
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
__s32 host_drv_Init(__u32 usbc_no)
{
    __u32 usbh_status = 0;

    DMSG_INFO("\n[usb host]: usb host%d init start.\n", usbc_no);

    /* 设置端口号 */
    usbh_set_usbd_port(usbc_no);

    DMSG_INFO("[usb host]: 1. open clock.\n");

    /* usbh起来以后必须第一时间抢到usb时钟, 这样usb_monitor才能确认usbh确实工作了 */
    if(hcd_open_clk() != EPDK_OK)
    {
        DMSG_PANIC("ERR : hcd_open_clk failed\n");
        goto host_failed_1;
    }

    DMSG_INFO("[usb host]: 2. init_usbm_op.\n");

    /* 初始化usbh和usb_monitor的通信接口, 通知usb_monitor USBH已经开始工作 */
    if(init_usbm_op() != EPDK_OK)
    {
        DMSG_PANIC("ERR : init_usbm_op failed\n");
        goto host_failed_2;
    }

    usbh_status = 1;
    if(usbc_no == 0)
    {
        usbm_sendcmd(DEV_IOC_USR_HWSC_SET_USBH_WORK_STATUS , &usbh_status);
    }
    else if(usbc_no == 1)
    {
        usbm_sendcmd(DEV_IOC_USR_HWSC_SET_USBH1_WORK_STATUS , &usbh_status);
    }
    else
    {
        usbm_sendcmd(DEV_IOC_USR_HWSC_SET_USBH2_WORK_STATUS , &usbh_status);
    }

#ifdef USBH_DEBUG_GATE
    usbh_debug_init();
#endif

    DMSG_INFO("[usb host]: 3. initialize memeroy manager.\n");

    /* 初始化内存管理 */
    if(HorseInit() != 0)
    {
        DMSG_PANIC("ERR : HorseInit failed\n");
        goto host_failed_3;
    }

    DMSG_INFO("[usb host]: 4. initialize list manager.\n");

    /* 初始化list管理 */
    if(ListMemoryInit() != EPDK_OK)
    {
        DMSG_PANIC("ERR : ListMemoryInit failed\n");
        HorseExit();
        goto host_failed_4;
    }

    DMSG_INFO("[usb host]: 5. usb_core_init.\n");

    /* 初始化usb core */
    if(usb_core_init() != EPDK_OK)
    {
        DMSG_PANIC("ERR : usb_core_init failed\n");
        goto host_failed_5;
    }

    DMSG_INFO("[usb host]: 6. initialize mass storage.\n");

    /* 初始化msc */
    if(mscInit() != EPDK_OK)
    {
        DMSG_PANIC("ERR : mscInit failed\n");
        goto host_failed_6;
    }

    DMSG_INFO("[usb host]: 7. initialize hid.\n");

    /* 初始化 hid */
    if(HidInit() != EPDK_OK)
    {
        DMSG_PANIC("ERR : HidInit failed\n");
        goto host_failed_7;
    }

    DMSG_INFO("[usb host]: 8. initialize host controller driver.\n");

    /* 初始化 hcd */
    if(hcd_init() != EPDK_OK)
    {
        DMSG_PANIC("ERR : hcd_init failed\n");
        goto host_failed_8;
    }

    DMSG_INFO("[usb host]: 9. host driver initialize complete.\n");

    return EPDK_OK;

host_failed_8:
    HidExit();

host_failed_7:
    mscExit();

host_failed_6:
    usb_core_exit();

host_failed_5:
    ListMemoryExit();

host_failed_4:
    HorseExit();

host_failed_3:
    exit_usbm_op();

host_failed_2:
    hcd_close_clk();

host_failed_1:

    DMSG_INFO("\n[usb host]: usb host init failed\n\n");

    return EPDK_FAIL;
}

/*
*******************************************************************************
*                     host_Exit
*
* Description:
*    驱动结束
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
__s32 host_drv_Exit(__u32 usbc_no)
{
    __u32 usbh_status = 0;
    __u32 stop_delay = 0;
    __s32 ret = EPDK_OK;

    DMSG_INFO("\n[usb host]: usb host rmmod.\n");

    stop_delay = get_max_disk_time();

    DMSG_INFO("[usb host]: 1. HidExit.\n");

    if(HidExit() != EPDK_OK)
    {
        DMSG_PANIC("ERR: HidExit failed\n");
        ret = EPDK_FAIL;
    }

    DMSG_INFO("[usb host]: 2. usbh_msc_exit.\n");

    //--<1>--注销msc
    if(mscExit() != EPDK_OK)
    {
        DMSG_PANIC("ERR: usbh_msc_exit failed\n");
        ret = EPDK_FAIL;
    }

    DMSG_INFO("[usb host]: 3. hcd stop.\n");

    hcd_stop(stop_delay);

    DMSG_INFO("[usb host]: 4. usb_gen_hub_exit.\n");

    if(usb_gen_hub_exit() != EPDK_OK)
    {
        DMSG_PANIC("ERR: usb_gen_hub_exit failed\n");
        ret = EPDK_FAIL;
    }

    DMSG_INFO("[usb host]: 5. hcd_exit.\n");

    //--<2>--注销hcd
    if(hcd_exit() != EPDK_OK)
    {
        DMSG_PANIC("ERR: hcd_exit failed\n");
        ret = EPDK_FAIL;
    }

    DMSG_INFO("[usb host]: 6. usb_core_exit.\n");

    //--<3>--注销core
    if(usb_core_exit() != EPDK_OK)
    {
        DMSG_PANIC("ERR: usb_core_exit failed\n");
        ret = EPDK_FAIL;
    }

    DMSG_INFO("[usb host]: 7. ListMemoryExit.\n");

    //--<4>--注销list管理
    if(ListMemoryExit() != 0)
    {
        DMSG_PANIC("ERR: ListMemoryExit failed\n");
        ret = EPDK_FAIL;
    }

    DMSG_INFO("[usb host]: 8. HorseExit.\n");

    if(HorseExit() != EPDK_OK)
    {
        DMSG_PANIC("ERR: HorseExit failed\n");
        ret = EPDK_FAIL;
    }


    DMSG_INFO("[usb host]: 9. exit_usbm_op.\n");

    //--<5>--通知usb_monitor usbh已经卸载了
    usbh_status = 0;
    if(usbc_no == 0)
    {
        usbm_sendcmd(DEV_IOC_USR_HWSC_SET_USBH_WORK_STATUS , &usbh_status);
    }
    else if(usbc_no == 1)
    {
        usbm_sendcmd(DEV_IOC_USR_HWSC_SET_USBH1_WORK_STATUS , &usbh_status);
    }
    else
    {
        usbm_sendcmd(DEV_IOC_USR_HWSC_SET_USBH2_WORK_STATUS , &usbh_status);
    }

    //--<6>--注销usbh和usb_monitor的接口
    if(exit_usbm_op() != EPDK_OK)
    {
        DMSG_PANIC("ERR: exit_usbm_op failed\n");
        ret = EPDK_FAIL;
    }

#ifdef  USBH_DEBUG_GATE
    usbh_debug_exit();
#endif

    DMSG_INFO("\n[usb host]: usb host rmmod complete.\n\n");

    return ret;
}

