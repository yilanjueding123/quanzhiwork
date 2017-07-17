/*
********************************************************************************************************************
*                                              USB Host Drvier
*
*                              (c) Copyright 2007-2009, softwinners.China
*										All	Rights Reserved
*
* File Name 	: hcd.c
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

#include "usb_host_base_types.h"
#include "usb_host_config.h"
#include "usb_os_platform.h"

#include "usb_list.h"
#include "list_head_ext.h"

#include "usb_gen_hcd.h"
#include "usb_host_hub.h"

#include "usbh_to_usbm.h"
#include "usbh_debug.h"

#include "hcd.h"
#include "hcd_i.h"


//------------------------------------------------------------------------------
//  宏定义区
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  全局变量区
//------------------------------------------------------------------------------
static struct hc_driver hc_driver_g;					//在_hcd_init_hc_driver()中初始化
static struct hc_private *host_controller = NULL;
static struct hc_priv_env_s env_all;
static usb_board_info_t  usbh_board_info_g;
static hcd_sys_cfg_t hcd_sys_cfg_g;

static __hdle usbh_bsp_hdle  	= NULL;
static __hdle usbh_clock_ahb 	= NULL;
static __hdle usbh_clock_phy 	= NULL;

static __hdle hDMA_Hdle = NULL;	//申请的dma中断号

static u32 usbh_port = 0;	//host使用的端口号


//------------------------------------------------------------------------------
//  函数定义区
//------------------------------------------------------------------------------
static void start_sched(struct hc_private *hc_priv);
static struct ep_private *find_a_valid_aysnc_ep_from_list(struct hc_private *hc_priv);


//------------------------------------------------------------------------------
//  程序区
//------------------------------------------------------------------------------
#if 1
void debug_hcd_print_ep_priv(ep_private_t *ep_priv, char *str)
{
    DMSG_INFO("\n---------------------ep_priv: %x, %s--------------------\n", ep_priv, str);
    DMSG_INFO("hep                     = %d\n", ep_priv->hep);
    DMSG_INFO("udev                    = %d\n", ep_priv->udev);
    DMSG_INFO("hc                      = %d\n", ep_priv->hc);
    DMSG_INFO("current_urb             = %d\n", ep_priv->current_urb);
    DMSG_INFO("next                    = %d\n", ep_priv->next);
    DMSG_INFO("is_added_schedule       = %d\n", ep_priv->is_added_schedule);
    DMSG_INFO("dma_working             = %d\n", ep_priv->dma_working);
    DMSG_INFO("dma_bytes               = %d\n", ep_priv->dma_bytes);

    DMSG_INFO("ep_list_index           = %d\n", ep_priv->ep_list_index);
    DMSG_INFO("defctrl                 = %d\n", ep_priv->defctrl);
    DMSG_INFO("mask_dir_local_ep_nr    = %x\n", ep_priv->mask_dir_local_ep_nr);
    DMSG_INFO("maxpacket               = %d\n", ep_priv->maxpacket);
    DMSG_INFO("nextpid                 = %d\n", ep_priv->nextpid);
    DMSG_INFO("stopped                 = %d\n", ep_priv->stopped);
    DMSG_INFO("error_count             = %d\n", ep_priv->error_count);
    DMSG_INFO("nak_count               = %d\n", ep_priv->nak_count);
    DMSG_INFO("length                  = %d\n", ep_priv->length);

    DMSG_INFO("ep_index                = %d\n", ep_priv->ep_index);
    DMSG_INFO("ep_num                  = %d\n", ep_priv->ep_num);
    DMSG_INFO("ep_ts_type              = %d\n", ep_priv->ep_ts_type);
    DMSG_INFO("ep_fifo_start           = %d\n", ep_priv->ep_fifo_start);
    DMSG_INFO("ep_fifo_size            = %d\n", ep_priv->ep_fifo_size);
    DMSG_INFO("ep_fun_addr             = %d\n", ep_priv->ep_fun_addr);
    DMSG_INFO("ep_hub_addr             = %d\n", ep_priv->ep_hub_addr);
    DMSG_INFO("ep_hub_port             = %d\n", ep_priv->ep_hub_port);
    DMSG_INFO("ep_interval             = %d\n", ep_priv->ep_interval);
    DMSG_INFO("period                  = %d\n", ep_priv->period);
    DMSG_INFO("branch                  = %d\n", ep_priv->branch);
    DMSG_INFO("load                    = %d\n", ep_priv->load);
    DMSG_INFO("ep0_data_dir            = %d\n", ep_priv->ep0_data_dir);
    //     DMSG_INFO("periodic_done_counter   = %d\n", ep_priv->periodic_done_counter);

    list_print_all_node(&(ep_priv->hep->urb_list), str);
    DMSG_INFO("--------------------------------------------------\n");
}

#endif


#if 0

void debug_hcd_print_hcd_priv(hc_private_t *hc_priv, char *str)
{
    __u32 i = 0;

    if(hc_priv == NULL)
    {
        DMSG_PANIC("ERR: input error\n");
        return;
    }

    DMSG_INFO("\n---------------------hc_priv: %x, %s--------------------\n", hc_priv, str);
    DMSG_INFO("point_gen_dev        = %x\n", hc_priv->point_gen_dev);
    DMSG_INFO("stat_insrmv          = %d\n", hc_priv->stat_insrmv);
    DMSG_INFO("stat_sof             = %d\n", hc_priv->stat_sof);
    DMSG_INFO("stat_lost            = %d\n", hc_priv->stat_lost);
    DMSG_INFO("stat_overrun         = %d\n", hc_priv->stat_overrun);
    DMSG_INFO("speed                = %d\n", hc_priv->speed);
    DMSG_INFO("urb_counter          = %d\n", hc_priv->urb_counter);
    DMSG_INFO("port                 = %x\n", hc_priv->port);
    DMSG_INFO("rhstate              = %d\n", hc_priv->rhstate);

    for(i = 0; i < HCD_EP_LIST_MAX_EP_NUM; i++)
    {
        DMSG_INFO("ep_list[%d] = %x\n", i, hc_priv->ep_list[i]);
        if(hc_priv->ep_list[i])
        {
            list_print_all_node(&(hc_priv->ep_list[i]->hep->urb_list), str);
            debug_hcd_print_ep_priv(hc_priv->ep_list[i], str);
        }
    }

    DMSG_INFO("\n", hc_priv->fifo_point);
    for(i = 0; i < HCD_EP_LIST_MAX_EP_NUM; i++)
    {
        DMSG_INFO("fifo_alloc_map[%d].start = %d\n", i, hc_priv->fifo_alloc_map[i].start);
        DMSG_INFO("fifo_alloc_map[%d].end   = %d\n", i, hc_priv->fifo_alloc_map[i].end);
    }

    list_print_all_node(&(hc_priv->async), str);
    for(i = 0; i < PERIODIC_SIZE; i++)
    {
        DMSG_INFO("load[%d] = %d\n", i, hc_priv->load[i]);
    }

    DMSG_INFO("curr_async               = %x\n", hc_priv->curr_async);
    DMSG_INFO("curr_aync_node           = %x\n", hc_priv->curr_aync_node);
    DMSG_INFO("root_hub_plug_event      = %d\n", hc_priv->root_hub_plug_event);
    DMSG_INFO("plug_event_is_proc_comp  = %d\n", hc_priv->plug_event_is_proc_comp);
    DMSG_INFO("last_not_proc_event      = %d\n", hc_priv->last_not_proc_event);

    DMSG_INFO("next_periodic            = %x\n", hc_priv->next_periodic);
    DMSG_INFO("next_async               = %x\n", hc_priv->next_async);
    DMSG_INFO("sof_periodic_entry       = %x\n", hc_priv->sof_periodic_entry);

    for(i = 0; i < PERIODIC_SIZE; i++)
    {
        DMSG_INFO("periodic[%d] = %x\n", i, hc_priv->periodic[i]);
        if(hc_priv->periodic[i])
        {
            list_print_all_node(&(hc_priv->periodic[i]->hep->urb_list), str);
        }
    }

    DMSG_INFO("periodic_count           = %d\n", hc_priv->periodic_count);
    DMSG_INFO("frame                    = %d\n", hc_priv->frame);
    DMSG_INFO("stat_overrun             = %d\n", hc_priv->stat_overrun);
    DMSG_INFO("urb_counter              = %d\n", hc_priv->urb_counter);
    DMSG_INFO("--------------------------------------------------\n");
}

void debug_hcd_print_all_aysn_ep(hc_private_t *hc_priv, ep_private_t *curr_ep_priv, char *str)
{
    struct list_head *list_head = NULL;
    struct list_head *list_now  = NULL;
    struct list_head *list_next = NULL;
    ep_private_t *ep_priv = NULL;
    __u32 i = 0;

    DMSG_INFO("\n---------------ep start---%x, %x, %s--------------------\n", hc_priv, curr_ep_priv, str);
    if(list_empty(&hc_priv->async))
    {
        DMSG_PANIC("debug_hcd_print_all_aysn_ep\n");
        DMSG_INFO("-----------------------ep end------------------------\n\n");
        return;
    }

    list_head = &hc_priv->async;
    list_now  = list_head->next;
    list_next = NULL;

    while(list_now != list_head)
    {
        list_next = list_now->next;

        ep_priv = (ep_private_t *)list_now->data;
        if(ep_priv)
        {
            DMSG_INFO("ep%d: ep_priv = %x, (%d, %d)\n",
                      i,
                      ep_priv,
                      ep_priv->udev->descriptor.idVendor,
                      ep_priv->udev->descriptor.idProduct);
            list_print_all_node(&(ep_priv->hep->urb_list), str);
        }

        list_now = list_next;

        i++;
    }
    DMSG_INFO("-----------------------ep end------------------------\n\n");

    return ;
}
#endif

/*
**********************************************************
*    开USB模块中断
**********************************************************
*/
void enable_usb_irq(void)
{
    USB_OS_ISR_ENABLE_INT(INTC_IRQNO_USB0);
}

/*
**********************************************************
*    关USB模块中断
**********************************************************
*/
void disable_usb_irq(void)
{
    USB_OS_ISR_DISABLE_INT(INTC_IRQNO_USB0);
}

/* 使能 ep tx/rx 中断 */
static void enable_ep_irq(struct ep_private *ep_priv)
{
    if(is_in_ep(ep_priv))  /* rx */
    {
        USBC_INT_EnableEp(usbh_bsp_hdle, USBC_EP_TYPE_RX, get_phy_ep_index(ep_priv));
    }
    else   /* tx */
    {
        USBC_INT_EnableEp(usbh_bsp_hdle, USBC_EP_TYPE_TX, get_phy_ep_index(ep_priv));
    }
}

/* 禁止 ep tx/rx 中断 */
static void disable_ep_irq(struct ep_private *ep_priv)
{
    if(is_in_ep(ep_priv))  /* rx */
    {
        USBC_INT_DisableEp(usbh_bsp_hdle, USBC_EP_TYPE_RX, get_phy_ep_index(ep_priv));
    }
    else   /* tx */
    {
        USBC_INT_DisableEp(usbh_bsp_hdle, USBC_EP_TYPE_TX, get_phy_ep_index(ep_priv));
    }
}

/*
**********************************************************
*   ep 使能
**********************************************************
*/
static void select_active_ep(u8 ep_index)
{
    USBC_SelectActiveEp(usbh_bsp_hdle, ep_index);
}

/*
**********************************************************
*   获得当前活动端点的信息
**********************************************************
*/
static u8 get_active_ep(void)
{
    return USBC_GetActiveEp(usbh_bsp_hdle);
}

/*
**********************************************************
*   清空等待处理的中断请求
**********************************************************
*/
/*
static void clear_all_irq_pendding(void)
{
    USBC_INT_ClearEpPendingAll(usbh_bsp_hdle, USBC_EP_TYPE_TX);
	USBC_INT_ClearEpPendingAll(usbh_bsp_hdle, USBC_EP_TYPE_RX);
	USBC_INT_ClearMiscPendingAll(usbh_bsp_hdle);
}
*/

/* 读取中断pendding */
static USB_INLINE u32 read_INT_pendding_tx(struct hc_priv_env_s *env)
{
    if(env->int_pendding.int_pendding_tx == 0)
    {
        env->int_pendding.int_pendding_tx = USBC_INT_EpPending(usbh_bsp_hdle, USBC_EP_TYPE_TX);
    }

    return env->int_pendding.int_pendding_tx;
}

/* 读取RX 中断pendding */
static USB_INLINE u32 read_INT_pendding_rx(struct hc_priv_env_s *env)
{
    if(env->int_pendding.int_pendding_rx == 0)
    {
        env->int_pendding.int_pendding_rx = USBC_INT_EpPending(usbh_bsp_hdle, USBC_EP_TYPE_RX);
    }

    return env->int_pendding.int_pendding_rx;
}

/* 读取USB 中断pendding */
static USB_INLINE u32 read_INT_pendding_misc(struct hc_priv_env_s *env)
{
    //如软INT为空，则更新之
    if(env->int_pendding.int_pendding_misc == 0)
    {
        env->int_pendding.int_pendding_misc = USBC_INT_MiscPending(usbh_bsp_hdle);
    }

    return env->int_pendding.int_pendding_misc;
}

//清空某bit
static USB_INLINE u32 clear_INT_pendding_bits_tx(struct hc_priv_env_s *env, u8 mask)
{
    return ((env->int_pendding.int_pendding_tx) &= (~mask));
}

static USB_INLINE u32 clear_INT_pendding_bits_rx(struct hc_priv_env_s *env, u8 mask)
{
    return ((env->int_pendding.int_pendding_rx) &= (~mask));
}

static USB_INLINE u32 clear_INT_pendding_bits_misc(struct hc_priv_env_s *env, u8 mask)
{
    return ((env->int_pendding.int_pendding_misc) &= (~mask));
}

/*
static USB_INLINE int clear_INT_pendding_all(struct hc_priv_env_s * env)
{
	env->int_pendding_regs.reg_int_pendding_misc = 0;
	env->int_pendding_regs.reg_int_pendding_tx = 0;
	env->int_pendding_regs.reg_int_pendding_rx = 0;

	return 0;
}
*/

/* 使能sof中断 */
static USB_INLINE void enable_SOF_irq(void)
{
    USBC_INT_EnableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_SOF);
}

static USB_INLINE void disable_SOF_irq(void)
{
    USBC_INT_DisableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_SOF);
}

static s32 hcd_port_power_supply_by_p1(u32 pin, u32 is_on)
{
    s32 Ret = 0;
    ES_FILE *pfile = (ES_FILE *)hcd_sys_cfg_g.drv_vbus_pin_hdle;

    DMSG_INFO("hcd power on by p1, %d\n", is_on);
    if(is_on)
    {
        Ret = eLIBs_fioctrl(pfile, DRV_POWER_CMD_PWREPT_SET, pin, (void *)(POWER_EPT_GATE_ON | POWER_EPT_MOS_P));
        if(Ret == EPDK_FAIL)
        {
            DMSG_PANIC("ERR:  power eLIBs_fioctrl failed\n");
            return EPDK_FAIL;
        }
    }
    else
    {
        Ret = eLIBs_fioctrl(pfile, DRV_POWER_CMD_PWREPT_SET, pin, (void *)(POWER_EPT_GATE_ON | POWER_EPT_MOS_N));
        if(Ret == EPDK_FAIL)
        {
            DMSG_PANIC("ERR:  power eLIBs_fioctrl failed\n");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

/*
*******************************************************************************
*                     hcd_port_power_supply_by_pio
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
static __s32 hcd_port_power_supply_by_pio(u32 is_on)
{
    s32 Ret = 0;
    __hdle pin_hdle = (__hdle)hcd_sys_cfg_g.drv_vbus_pin_hdle;
    __u32 pin_value = 0;    /* pin 值 */

    DMSG_INFO("hcd power on by pio, %d\n", is_on);

    pin_value = (is_on == 0 ? 0 : 1);
    Ret = esPINS_WritePinData(pin_hdle, pin_value, 0);
    if(Ret != EPDK_OK)
    {
        DMSG_PANIC("ERR: power esPINS_PioPinsDataOps failed\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

/*
*******************************************************************************
*                     __port_power_supply
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
static void __port_power_supply(u32 is_on)
{
    usb_board_info_t *board_info = &usbh_board_info_g;
    board_usb_port_t *usb_port = &(board_info->usb_port.port[usbh_port]);

    switch(usb_port->drv_vbus.group_type)
    {
    case GROUP_TYPE_PIO:
        hcd_port_power_supply_by_pio(is_on);
        break;

    case GROUP_TYPE_P1:
        hcd_port_power_supply_by_p1(usb_port->drv_vbus.ex_pin_no, is_on);
        break;

    default:
        DMSG_PANIC("ERR: unkown group_type(%x)\n", usb_port->drv_vbus.group_type);
        return ;
    }

    return;
}

/*
*******************************************************************************
*                     __port_power
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
void __port_power(u32 is_on)
{
    __cpu_sr cpu_sr = 0;

    __port_power_supply(is_on);

    USB_OS_ENTER_CRITICAL(cpu_sr);
    if(is_on)
    {
        USBC_Host_StartSession(usbh_bsp_hdle);
        USBC_ForceVbusValid(usbh_bsp_hdle, USBC_VBUS_TYPE_HIGH);
        DMSG_INFO("INFO : USB VBus power ON\n");
    }
    else
    {
        USBC_Host_EndSession(usbh_bsp_hdle);
        USBC_ForceVbusValid(usbh_bsp_hdle, USBC_VBUS_TYPE_DISABLE);
        DMSG_INFO("INFO : USB VBus power OFF\n");
    }
    USB_OS_EXIT_CRITICAL(cpu_sr);

    return;
}

/*
*******************************************************************************
*                     __port_suspend
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
void __port_suspend(void)
{
    __cpu_sr cpu_sr = 0;

    USB_OS_ENTER_CRITICAL(cpu_sr);
    USBC_Host_SuspendPort(usbh_bsp_hdle);
    USB_OS_EXIT_CRITICAL(cpu_sr);

    USB_OS_DELAY_x_MS(1000);
}

/*
*******************************************************************************
*                     __port_resume
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
void __port_resume(void)
{
    __cpu_sr cpu_sr = 0;

    USB_OS_ENTER_CRITICAL(cpu_sr);
    USBC_Host_RusumePort(usbh_bsp_hdle);
    USB_OS_EXIT_CRITICAL(cpu_sr);

    USB_OS_DELAY_x_MS(500);

    USB_OS_ENTER_CRITICAL(cpu_sr);
    USBC_Host_ClearRusumePortFlag(usbh_bsp_hdle);
    USB_OS_EXIT_CRITICAL(cpu_sr);
}

/*
*******************************************************************************
*                     __port_reset
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
void __port_reset(void)
{
    __cpu_sr cpu_sr = 0;
    __u32 i = 0;

    __port_resume();

    //至少20ms
    USB_OS_ENTER_CRITICAL(cpu_sr);
    USBC_Host_ResetPort(usbh_bsp_hdle);
    USB_OS_EXIT_CRITICAL(cpu_sr);

    USB_OS_DELAY_x_MS(50);  /* 参考windows, reset时间改为50ms */

    UsbPhyEndReset(usbh_bsp_hdle);

    USB_OS_ENTER_CRITICAL(cpu_sr);
    usb_phy_new_tp_write(usbh_bsp_hdle, 0x3c, 2, 2);
    for(i = 0; i < 0x100; i++);
    usb_phy_new_tp_write(usbh_bsp_hdle, 0x3c, 0, 2);
    __msg("-----------------usb_phy_new_tp_write, 0x3c, 0x00, 0x02");
    USBC_Host_ClearResetPortFlag(usbh_bsp_hdle);
    USB_OS_EXIT_CRITICAL(cpu_sr);

    USB_OS_DELAY_x_MS(500);

    enable_SOF_irq();
}

static void port_power(struct hc_private *hcd, u32 is_on)
{
    __port_power(is_on);
}

void port_suspend(struct hc_private *hcd)
{
    __port_suspend();
}

static void port_resume(struct hc_private *hcd)
{
    __port_resume();
}

static void port_reset(struct hc_private *hcd)
{
    __u8 old_ep_index = 0;

    __port_reset();

    old_ep_index = USBC_GetActiveEp(usbh_bsp_hdle);

    USBC_SelectActiveEp(usbh_bsp_hdle, 0);
    USBC_Host_SetFunctionAddress_Deafult(usbh_bsp_hdle, USBC_EP_TYPE_TX, 0);

    //set address ep0
    {
        u32 i = 1;

        for( i = 1 ; i <= 5; i++)
        {
            USBC_SelectActiveEp(usbh_bsp_hdle, i);

            USBC_Host_SetFunctionAddress_Deafult(usbh_bsp_hdle, USBC_EP_TYPE_TX, i);
            USBC_Host_SetFunctionAddress_Deafult(usbh_bsp_hdle, USBC_EP_TYPE_RX, i);
        }
    }

    USBC_SelectActiveEp(usbh_bsp_hdle, old_ep_index);
}

/*
**********************************************************************************************************************
*                       hcd_ep_fifo_alloc
*
* Description:
*     配置ep fifo
*
* Arguments  :
*     hc_priv  		:  input.
*     ep_priv       :  input.
*     ep_list_index	:  input. ep号
*     fifo_size		:  input. fifo大小
*     is_dual_buff	:  input. 是否使用双fifo
*
* Returns    :
*     返回成功或者失败
*
* Notes      :
*
**********************************************************************************************************************
*/
static s32 hcd_ep_fifo_alloc(struct hc_private *hc_priv,
                             struct ep_private *ep_priv,
                             u32 ep_list_index,
                             u32 fifo_size,
                             u8 is_dual_buff)
{
    u8 	old_ep_index = 0;

    //--<1>--check input
    if(hc_priv == NULL || ep_priv == NULL)
    {
        DMSG_PANIC("ERR: input error\n");
        return -1;
    }

    //hc_priv->fifo_point初始化
    if((hc_priv->fifo_point == 0) && (get_phy_ep_index(ep_priv) != 0))
    {
        hc_priv->fifo_point = USB_EP1_START_ADDR;
        DMSG_PANIC("hcd_ep_fifo_alloc: hc_priv->fifo_point initialize to %d\n", (u32)USB_EP1_START_ADDR);
    }

    //--<2>--本地fifo分配的地址不能够超过最大地址
    if((hc_priv->fifo_point + (fifo_size << is_dual_buff)) >= hc_priv->fifo_total_size)
    {
        DMSG_PANIC("ERR: no free fifo, fifo_point = %x, fifo_size = %x, fifo_total_size = %x\n",
                   hc_priv->fifo_point, fifo_size, hc_priv->fifo_total_size);
        hc_priv->fifo_point = USB_EP1_START_ADDR;
    }

    //--<3>--记录本次的信息, 使用dual_buff, fifo_size = fifo_size * 2
    hc_priv->fifo_alloc_map[ep_list_index].start	= hc_priv->fifo_point;
    hc_priv->fifo_alloc_map[ep_list_index].end		= hc_priv->fifo_point + (fifo_size << is_dual_buff);

    //--<4>--修正当前的offset, 使用dual_buff, fifo_size = fifo_size * 2
    hc_priv->fifo_point += (fifo_size << is_dual_buff);

    //--<5>--选择当前活动的ep
    old_ep_index = get_active_ep();
    select_active_ep(get_phy_ep_index(ep_priv));

    //--<6>--配置fifo
    ep_priv->ep_fifo_start = hc_priv->fifo_alloc_map[ep_list_index].start;
    ep_priv->ep_fifo_size  = fifo_size;
    if(is_in_ep(ep_priv))  /* ep in, rx */
    {
        USBC_ConfigFifo(usbh_bsp_hdle,
                        USBC_EP_TYPE_RX,
                        is_dual_buff,
                        ep_priv->ep_fifo_size,
                        ep_priv->ep_fifo_start);
    }
    else   /* ep out, tx */
    {
        USBC_ConfigFifo(usbh_bsp_hdle,
                        USBC_EP_TYPE_TX,
                        is_dual_buff,
                        ep_priv->ep_fifo_size,
                        ep_priv->ep_fifo_start);
    }

    //--<7>--还原旧的ep
    select_active_ep(old_ep_index);

    return 0;
}

/*
********************************************************************************
*                     hcd_ep_config
* Description:
*     配置ep。
*         1、ep本地化
*         2、配置ep
*         3、分配FIFO
*
*
* Arguments:
*     type	    :  如PIPE_INTERRUPT
*     buftype	:  单buff，双buff,如HCD_FIFO_BUFF_NR_SINGLE
*     is_out	:  方向，是否为out，非0为out
*     interval	:  时间间隔
* Return value:
*     void
* note:
*
*********************************************************************************
*/
static s32 hcd_ep_config(struct hc_private *hc_priv,
                         struct ep_private *ep_priv,
                         u8 pipe_type,
                         u8 is_double_fifo,
                         int is_out,
                         int interval)
{
    struct usb_host_virt_dev *udev 	= NULL;
    __u32 i 				= 0;
    __u32 ep_index_begin 	= 0;
    __u32 ep_index_end 		= 0;
    __u8  old_ep_index 		= 0;
    __u8  ts_type 			= 0;
    __s32 ret 				= 0;
    __u32 speed				= 0;

    //--<1>--check input
    if(hc_priv == NULL || ep_priv == NULL)
    {
        DMSG_PANIC("ERR: hcd_ep_config: input error\n");
        return -1;
    }

    udev = ep_priv->udev;
    if(udev == NULL)
    {
        DMSG_PANIC("udev == NULL\n");
        return -1;
    }

    //--<2>--选一个空闲的本地ep
    if(is_out)
    {
        ep_index_begin = HCD_EP_LIST_OUT_BEGIN;
        ep_index_end   = HCD_EP_LIST_OUT_END;
    }
    else
    {
        ep_index_begin = HCD_EP_LIST_IN_BEGIN;
        ep_index_end   = HCD_EP_LIST_IN_END;
    }

    /* 寻找未使用的ep, 一个物理ep同时作为tx and rx */
    for(i = ep_index_begin; i <= ep_index_end; i++)
    {
        if(hc_priv->ep_list[i] == NULL)
        {
            hc_priv->ep_list[i] 	= ep_priv;
            ep_priv->ep_list_index 	= i;

            break;
        }
    }

    if(i > ep_index_end)
    {
        DMSG_PANIC("ERR: hcd_ep_config, ep_list not have no more space for new ep\n");
        return -1;
    }

    //--<3>--将新加入的ep本地化
    ep_priv->ep_index = ep_index_list_2_phy(is_out, ep_priv->ep_list_index);

    if(is_out)
    {
        ep_priv->mask_dir_local_ep_nr =  (u8)(EP_PRIV_MASK_DIR_OUT | ep_priv->ep_index);
    }
    else
    {
        ep_priv->mask_dir_local_ep_nr =  (u8)(EP_PRIV_MASK_DIR_IN | ep_priv->ep_index);
    }

    //--<4>--将通用pipe_type类型来确定ep类型
    switch(pipe_type)
    {
    case PIPE_ISOCHRONOUS:
        ts_type = USBC_TS_TYPE_ISO;
        break;

    case PIPE_INTERRUPT:
        ts_type = USBC_TS_TYPE_INT;
        break;

    case PIPE_BULK:
        ts_type = USBC_TS_TYPE_BULK;
        break;

    default:
        ts_type = USBC_TS_TYPE_IDLE;
        DMSG_PANIC("ERR: hcd_ep_config, unknow pipe_type(%d)\n", pipe_type);
        break;
    }

    switch(udev->speed)
    {
    case USB_SPEED_HIGH:
        speed = USBC_TS_MODE_HS;
        break;

    case USB_SPEED_FULL:
        speed = USBC_TS_MODE_FS;
        break;

    case USB_SPEED_LOW:
        speed = USBC_TS_MODE_LS;
        break;

    default:
        speed = USBC_TS_MODE_HS;
    }

    //--<5>--选中ep
    old_ep_index = get_active_ep();
    select_active_ep(get_phy_ep_index(ep_priv));

    DMSG_HCD_TEST("config ep: ep_type = %d, ep_index = %d,"
                  "fun_addr = %d, ep_target_muiltTT = %d, "
                  "hub_addr = %d, hub_port = %d\n",
                  is_out ? USBC_EP_TYPE_TX : USBC_EP_TYPE_RX,
                  ep_priv->ep_index, ep_priv->ep_fun_addr,
                  ep_priv->ep_target_muiltTT, ep_priv->ep_hub_addr, ep_priv->ep_hub_port);

    //--<6>--配置ep
    if(is_out)  /* ep out, tx */
    {
        USBC_Host_SetFunctionAddress(usbh_bsp_hdle,
                                     USBC_EP_TYPE_TX,
                                     ep_priv->ep_index,
                                     ep_priv->ep_fun_addr,
                                     ep_priv->ep_target_muiltTT,
                                     ep_priv->ep_hub_addr,
                                     ep_priv->ep_hub_port);
        USBC_Host_ConfigEp(usbh_bsp_hdle,
                           USBC_EP_TYPE_TX,
                           ep_priv->ep_num,
                           speed,
                           ts_type,
                           is_double_fifo,
                           ep_priv->maxpacket,
                           interval);
    }
    else   /* ep in, rx */
    {
        USBC_Host_SetFunctionAddress(usbh_bsp_hdle,
                                     USBC_EP_TYPE_RX,
                                     ep_priv->ep_index,
                                     ep_priv->ep_fun_addr,
                                     ep_priv->ep_target_muiltTT,
                                     ep_priv->ep_hub_addr,
                                     ep_priv->ep_hub_port);
        USBC_Host_ConfigEp(usbh_bsp_hdle,
                           USBC_EP_TYPE_RX,
                           ep_priv->ep_num,
                           speed,
                           ts_type,
                           is_double_fifo,
                           ep_priv->maxpacket,
                           interval);
    }

    //--<7>--分配FIFO
    ret = hcd_ep_fifo_alloc(hc_priv,
                            ep_priv,
                            ep_priv->ep_list_index,
                            ep_priv->maxpacket,
                            is_double_fifo);
    if(ret != 0)
    {
        DMSG_PANIC("ERR: hcd_ep_config, hc_ep_fifo_alloc failed\n");
        select_active_ep(old_ep_index);
        return -1;
    }

    //--<8>--enable ep irq
    enable_ep_irq(ep_priv);

    //--<9>--还原旧的ep
    select_active_ep(old_ep_index);

    return 0;
}

/*
*******************************************************************************
*                     hcd_ep_config_default
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
static s32 hcd_ep_config_default(struct ep_private *ep_priv)
{
    struct hc_private *hc_priv = NULL;
    u8 old_ep_index = 0;

    //--<1>--check input
    if(ep_priv == NULL)
    {
        DMSG_PANIC("ERR: hcd_ep_config_default, input error\n");
        return -1;
    }

    //--<2>--initialize parameter
    hc_priv = ep_priv->hc;
    if(hc_priv == NULL)
    {
        DMSG_PANIC("ERR: hcd_ep_config_default, hc_priv == NULL\n");
        return -1;
    }

    old_ep_index = get_active_ep();
    select_active_ep(get_phy_ep_index(ep_priv));

    //--<3>--disable ep irq
    disable_ep_irq(ep_priv);

    //--<4>--disable ep fifo
    if(is_in_ep(ep_priv))  /* ep in, rx */
    {
        USBC_Host_SetFunctionAddress_Deafult(usbh_bsp_hdle, USBC_EP_TYPE_RX, ep_priv->ep_index);
        USBC_ConfigFifo_Default(usbh_bsp_hdle, USBC_EP_TYPE_RX);
    }
    else   /* ep out, tx */
    {
        USBC_Host_SetFunctionAddress_Deafult(usbh_bsp_hdle, USBC_EP_TYPE_TX, ep_priv->ep_index);
        USBC_ConfigFifo_Default(usbh_bsp_hdle, USBC_EP_TYPE_TX);
    }

    //--<5>--disable ep csr
    if(is_in_ep(ep_priv))  /* ep in, rx */
    {
        USBC_Host_ConfigEp_Default(usbh_bsp_hdle, USBC_EP_TYPE_RX);
    }
    else   /* ep out, tx */
    {
        USBC_Host_ConfigEp_Default(usbh_bsp_hdle, USBC_EP_TYPE_TX);
    }

    //--<6>--disable ep struct
    ep_priv->mask_dir_local_ep_nr = 0;

    //--<7>--disable hc_priv->ep_list[i]
    hc_priv->ep_list[ep_priv->ep_list_index] = NULL;

    if(ep_priv == hc_priv->curr_async)
    {
        hc_priv->curr_async		= NULL;
        hc_priv->curr_aync_node = NULL;
    }

    select_active_ep(old_ep_index);

    return 0;
}

static void hcd_set_urb_busy(struct urb *urb)
{
    __cpu_sr cpu_sr = 0;

    USB_OS_ENTER_CRITICAL(cpu_sr);
    urb->is_busy = 1;
    USB_OS_EXIT_CRITICAL(cpu_sr);
}

static void hcd_set_urb_free(struct urb *urb)
{
    __cpu_sr cpu_sr = 0;

    USB_OS_ENTER_CRITICAL(cpu_sr);
    urb->is_busy = 0;
    USB_OS_EXIT_CRITICAL(cpu_sr);
}

static u32 hcd_is_urb_busy(struct urb *urb)
{
    return urb->is_busy;
}

/* 完成ep0 的status阶段 */
static void finish_ep0_status_after_IN(void)
{
    USBC_Host_SendCtrlStatus(usbh_bsp_hdle, 1);
}

/* 完成ep0 的status阶段 */
static void finish_ep0_status_after_OUT_or_SETUP(void)
{
    USBC_Host_SendCtrlStatus(usbh_bsp_hdle, 0);
}

/*
********************************************************************************
*                              finish_request
* Description:
*     完成urb请求。为下一次传输做准备
*
* Arguments:
*
*
* Return value:
*     void
*
* note:
*     void
*
*********************************************************************************
*/
static void finish_request(struct ep_private *ep_priv, struct urb *urb)
{
    struct hc_private *hc_priv = NULL;
    u8 stopped 		= 0;
    __cpu_sr cup_sr	= 0;


    //--<1>--check input
    if(ep_priv == NULL || urb == NULL)
    {
        DMSG_PANIC("ERR: finish_request: input error\n");
        return ;
    }

    if(urb->need_dispose == 0)
    {
        DMSG_PANIC("wrn: finish_request: urb is not need dispose\n");
    }

    //--<2>--initialize parameter
    stopped = ep_priv->stopped;
    hc_priv = ep_priv->hc;
    if(hc_priv == NULL)
    {
        DMSG_PANIC("err: finish_request, hc_priv == NULL\n");
        return;
    }

    DMSG_HCD_TEST("finish_request: ep_priv = %x, ep_list_index = %x, stopped = %d, curr_async = %x\n",
                  ep_priv, ep_priv->ep_list_index, ep_priv->stopped, hc_priv->curr_async);

    //--<3>--处理urb
    USB_OS_ENTER_CRITICAL(cup_sr);

    /* ep0的请求处理完成后, 就从ep_list里删除, 并且关ep0中断 */
    if(hc_priv->ep_list[HCD_EP_LIST_EP0_INDEX] == ep_priv)
    {
        hc_priv->ep_list[HCD_EP_LIST_EP0_INDEX] = NULL;

        USBC_Host_ConfigEp_Default(usbh_bsp_hdle, USBC_EP_TYPE_EP0);
        USBC_Host_SetFunctionAddress_Deafult(usbh_bsp_hdle, USBC_EP_TYPE_TX, 0);
        USBC_INT_DisableEp(usbh_bsp_hdle, USBC_EP_TYPE_TX, 0);
    }

    if(ep_priv->current_urb == urb)
    {
        ep_priv->current_urb = NULL;
    }
    else
    {
        DMSG_PANIC("wrn: finish_request: ep current_urb is different from input urb\n");
        DMSG_PANIC("current_urb = %x, finish_urb = %x\n", ep_priv->current_urb, urb);
    }

    ep_priv->nextpid = USB_PID_UNDEF_0;

    if (urb->status == -EINPROGRESS)
    {
        urb->status = 0;
    }

    urb->hcpriv = NULL;
    urb->need_dispose = 0;

#ifdef  HCD_INT_ISO_TRANSFER
    if(usb_pipetype(urb->pipe) == PIPE_INTERRUPT)
    {
        hc_priv->urb_counter--;
        if(hc_priv->urb_counter <= 0)
        {
            DMSG_INFO("urb counter is zero, close SOF irq\n");
            disable_SOF_irq();
        }
    }
#else
    if(ep_priv->ep_ts_type == PIPE_INTERRUPT)
    {
        __u32 i = 0;

        for(i = 0; i < PERIODIC_SIZE; i++)
        {
            if(hc_priv->periodic[i] == ep_priv)
            {
                hc_priv->periodic[i] = NULL;
                break;
            }
        }

        if(i >= PERIODIC_SIZE)
        {
            DMSG_PANIC("ERR: finish_request: can not find ep in periodic array\n");
        }
    }
#endif

    /*
    	在hub urb callback的时候，新的hub urb会queue，此时finish_request还未完成，
    	这样会造成ep_priv->stopped。
    */
    ep_priv->length = 0;
    ep_priv->stopped = 1;
    usb_hcd_giveback_urb(to_hcd(hc_priv), urb);
    ep_priv->stopped = stopped;

    //--<5>--从curr_async ep_list中删除ep
    if(ep_priv->ep_ts_type == PIPE_CONTROL || ep_priv->ep_ts_type == PIPE_BULK)
    {
        list_head_ext_remov_node_from_list(ep_priv, &(hc_priv->async));

        //--<6>--设置curr_async ep_list，为下一次准备. 这里只管ep, 不管urb是否有效
        if(ep_priv != hc_priv->curr_async)
        {
            /* curr_async是新的, 就不用更新了 */
            DMSG_PANIC("wrn: finish_request, ep is not curr_async. index = %d\n", get_phy_ep_index(ep_priv));
        }
        else
        {
            /* curr_async是旧的, 就更新 */
            hc_priv->curr_async 	= NULL;
            hc_priv->curr_aync_node = NULL;
        }
    }

    DMSG_HCD_TEST("f: ep(%x, %d, %d), urb(%x, %d, %d, %d)\n",
                  ep_priv, ep_priv->ep_index, ep_priv->stopped,
                  urb, urb->transfer_buffer_length, urb->actual_length, urb->status);

    //	DMSG_INFO("f:%d\n", urb->transfer_buffer_length);

    USB_OS_EXIT_CRITICAL(cup_sr);

    /* 发起下一次异步传输 */
    if(hc_priv->rhstate == RH_ENABLE)
    {
        start_sched(hc_priv);
    }
    else
    {
        DMSG_PANIC("finish_request: root hub(%d) is not enable, can not transfer\n", hc_priv->rhstate);
    }

    return;
}

/* 从ep的fifo中读取数据 */
static u32 pio_read_data(struct ep_private *ep_priv,
                         struct urb *urb,
                         u8 ep_index,
                         u32 fifo_addr,
                         u8 *buffer,
                         u32 len)
{
    u32 this_len   	= 0;
    u32 is_last  	= 0;
    s32 ret 		= 0;

    //--<1>--check input
    if(ep_priv == NULL || urb == NULL)
    {
        DMSG_PANIC("ERR: inpur error\n");
        return 0;
    }

    //--<2>--read data from fifo
    this_len = USBC_ReadPacket(usbh_bsp_hdle, fifo_addr, len, (void *)buffer);

    //--<3>--半包可以看做是传输的最后一个包
    urb->actual_length  += this_len;
    ep_priv->length		= this_len;
    if((this_len != len) || (urb->actual_length > urb->transfer_buffer_length))
    {
        is_last = 2;
        urb->status = -EFIFO;

        DMSG_PANIC("ERR: pio_read_fifo: dispose urb error\n");
        DMSG_PANIC("ERR: pio_read_fifo: want(%d), real(%d), req->length(%d), req->actual(%d)\n",
                   len, this_len, urb->transfer_buffer_length, urb->actual_length);
        goto Data_ERR;
    }

    if(this_len < ep_priv->maxpacket)  //半包或者零包
    {
        is_last = 1;
    }
    else if(urb->actual_length == urb->transfer_buffer_length)    //整包, 但是传输结束了
    {
        is_last = 1;
    }
    else
    {
        is_last = 0;
    }

Data_ERR:
    //--<4>--告诉rxcsr读的结果
    if(ep_index)
    {
        ret = USBC_Host_ReadDataStatus(usbh_bsp_hdle, USBC_EP_TYPE_RX, is_last);
        if(ret != EPDK_OK)
        {
            DMSG_PANIC("ERR: USBC_Host_ReadDataStatus epx, failed\n");
            urb->status = -EFIFO;
        }
    }
    else
    {
        ret = USBC_Host_ReadDataStatus(usbh_bsp_hdle, USBC_EP_TYPE_EP0, is_last);
        if(ret != EPDK_OK)
        {
            DMSG_PANIC("ERR: USBC_Host_ReadDataStatus ep0, failed\n");
            urb->status = -EFIFO;
        }
    }

    return is_last;
}

/* write fifo */
static u32 pio_write_data(struct ep_private *ep_priv,
                          struct urb *urb,
                          u8 ep_index,
                          u32 fifo_addr,
                          u8 *buffer,
                          u32 len)
{
    u32 this_len   	= 0;
    u32 is_last  	= 0;
    s32 ret 		= 0;

    //--<1>--check input
    if(ep_priv == NULL || urb == NULL)
    {
        DMSG_PANIC("ERR: pio_write_data, inpur error\n");
        return 0;
    }

    //--<2>--write data to fifo
    this_len = USBC_WritePacket(usbh_bsp_hdle, fifo_addr, len, (void *)buffer);

    //--<3>--判断是否是最后一个包
    urb->actual_length	+= this_len;
    ep_priv->length		= this_len;
    if((this_len != len) || (urb->actual_length > urb->transfer_buffer_length))
    {
        is_last = 2;
        urb->status = -EFIFO;

        DMSG_PANIC("ERR: pio_read_fifo: dispose urb error\n");
        DMSG_PANIC("ERR: pio_read_fifo: want(%d), real(%d), req->length(%d), req->actual(%d)\n",
                   len, this_len, urb->transfer_buffer_length, urb->actual_length);
        goto Data_ERR;
    }

    if(this_len < ep_priv->maxpacket)  //半包
    {
        is_last = 1;
    }
    else if(urb->actual_length == urb->transfer_buffer_length)    //整包, 但是传输结束了
    {
        is_last = 1;
    }
    else
    {
        is_last = 0;
    }

Data_ERR:
    //--<4>--告诉rxcsr读的结果
    if(ep_index)
    {
        ret = USBC_Host_WriteDataStatus(usbh_bsp_hdle, USBC_EP_TYPE_TX, is_last);
        if(ret != EPDK_OK)
        {
            DMSG_PANIC("ERR: pio_write_data, USBC_Host_WriteDataStatus epx, failed\n");
            urb->status = -EFIFO;
        }
    }
    else
    {
        ret = USBC_Host_WriteDataStatus(usbh_bsp_hdle, USBC_EP_TYPE_EP0, is_last);
        if(ret != EPDK_OK)
        {
            DMSG_PANIC("ERR: pio_write_data, USBC_Host_WriteDataStatus ep0, failed\n");
            urb->status = -EFIFO;
        }
    }

    return is_last;
}

#ifdef  HCD_MOV_DATA_BY_DMA

/* 切换总线到DMA */
static void switch_bus_to_dma(struct ep_private *ep_priv)
{
    if(is_in_ep(ep_priv))  /* ep in, rx */
    {
        USBC_SelectBus(usbh_bsp_hdle, USBC_IO_TYPE_DMA, USBC_EP_TYPE_RX, get_phy_ep_index(ep_priv));
    }
    else    /* ep out, tx */
    {
        USBC_SelectBus(usbh_bsp_hdle, USBC_IO_TYPE_DMA, USBC_EP_TYPE_TX, get_phy_ep_index(ep_priv));
    }
}

/* 切换总线到PIO */
static void switch_bus_to_pio(void)
{
    USBC_SelectBus(usbh_bsp_hdle, USBC_IO_TYPE_PIO, 0, 0);
}

/* 开DMA子中断 */
static void enable_dma_sub_irq(void)
{
    USB_OS_DMA_EnableINT(hDMA_Hdle, CSP_DMA_TRANSFER_END_INT);
}

/* 关DMA子中断 */
static void disable_dma_sub_irq(void)
{
    USB_OS_DMA_DisableINT(hDMA_Hdle, CSP_DMA_TRANSFER_END_INT);
}

/* 配置DMA，这里使用的dedicate DMA */
static void dma_set_config(struct ep_private *ep, u32 buff_addr)
{
    CSP_dma_config_t  parg;
    __u32 usbc_no          = 0;
    __u32 usb_cmt_blk_cnt  = 0;
    __u32 dram_cmt_blk_cnt = 0;

    USB_OS_MEMSET(&parg, 0, sizeof(CSP_dma_config_t));
    usb_cmt_blk_cnt  = ((((ep->hep->desc.wMaxPacketSize) >> 2) - 1) << 8) | 0x0f;
    dram_cmt_blk_cnt = ((((ep->hep->desc.wMaxPacketSize) >> 2) - 1) << 8);

    if(ep->hep->desc.bEndpointAddress & USB_DIR_IN)
    {
        usbc_no = CSP_DMAC_CFG_SRC_TYPE_USB0;

        parg.src_drq_type 		= usbc_no;
        parg.src_addr_type		= CSP_DMAC_CFG_SRC_ADDR_TYPE_IO_MODE;
        parg.src_burst_length	= CSP_DMAC_CFG_SRC_4_BURST;
        parg.src_data_width		= CSP_DMAC_CFG_SRC_DATA_WIDTH_32BIT;

        parg.dst_drq_type		= CSP_DMAC_CFG_DEST_TYPE_D_SDRAM;
        parg.dst_addr_type		= CSP_DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE;
        if((u32)buff_addr & 0x03)
        {
            parg.dst_burst_length	= CSP_DMAC_CFG_DEST_1_BURST;
            parg.dst_data_width		= CSP_DMAC_CFG_DEST_DATA_WIDTH_8BIT;
        }
        else
        {
            parg.dst_burst_length	= CSP_DMAC_CFG_DEST_4_BURST;
            parg.dst_data_width		= CSP_DMAC_CFG_DEST_DATA_WIDTH_32BIT;
        }

        parg.wait_state			= 0;
        parg.continuous_mode	= CSP_DMAC_CFG_CONTINUOUS_DISABLE;
        parg.cmt_blk_cnt		= usb_cmt_blk_cnt | (dram_cmt_blk_cnt << 16);
    }
    else
    {
        usbc_no = CSP_DMAC_CFG_DEST_TYPE_USB0;

        parg.src_drq_type 		= CSP_DMAC_CFG_SRC_TYPE_D_SDRAM;
        parg.src_addr_type		= CSP_DMAC_CFG_SRC_ADDR_TYPE_LINEAR_MODE;
        if((u32)buff_addr & 0x03)
        {
            parg.src_burst_length   = CSP_DMAC_CFG_SRC_1_BURST;
            parg.src_data_width     = CSP_DMAC_CFG_SRC_DATA_WIDTH_8BIT;
        }
        else
        {
            parg.src_burst_length   = CSP_DMAC_CFG_SRC_4_BURST;
            parg.src_data_width		= CSP_DMAC_CFG_SRC_DATA_WIDTH_32BIT;
        }

        parg.dst_drq_type		= usbc_no;
        parg.dst_addr_type		= CSP_DMAC_CFG_DEST_ADDR_TYPE_IO_MODE;
        parg.dst_burst_length	= CSP_DMAC_CFG_DEST_4_BURST;
        parg.dst_data_width		= CSP_DMAC_CFG_DEST_DATA_WIDTH_32BIT;

        parg.wait_state			= 0;
        parg.continuous_mode	= CSP_DMAC_CFG_CONTINUOUS_DISABLE;
        parg.cmt_blk_cnt		= (usb_cmt_blk_cnt << 16) | dram_cmt_blk_cnt;
    }

    USB_OS_DMA_Setting(hDMA_Hdle, (void *)&parg);

    return;
}

/*
*******************************************************************************
*                     hcd_dma_stop
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
static s32 hcd_dma_stop(void)
{
    if(hDMA_Hdle)
    {
        USB_OS_DMA_Stop(hDMA_Hdle);
    }
    else
    {
        DMSG_PANIC("ERR: hcd_dma_stop, usbh_dma_hdle == NULL\n");
    }

    return 0;
}

/* 清空ep关于DMA的所有状态, 一般在DMA异常的时间调用 */
static void clean_ep_dma_status(struct ep_private *ep_priv)
{
    u8  old_ep_index = 0;

    if(ep_priv == NULL)
    {
        DMSG_PANIC("ERR: clean_ep_dma_status: input error\n");
        return;
    }

    old_ep_index = get_active_ep();

    //--<1>--找到需要处理的ep
    select_active_ep(get_phy_ep_index(ep_priv));

    //--<2>--清除autoset/autoclear、ep_dma_en、ep_dma_mode
    if(is_in_ep(ep_priv))  /* ep in, rx*/
    {
        USBC_Host_ClearEpDma(usbh_bsp_hdle, USBC_EP_TYPE_RX);
        USBC_Host_ClearRqPktCount(usbh_bsp_hdle, get_phy_ep_index(ep_priv));
        USBC_Host_StopInToken(usbh_bsp_hdle, USBC_EP_TYPE_RX);
    }
    else   /* ep out, tx*/
    {
        USBC_Host_ClearEpDma(usbh_bsp_hdle, USBC_EP_TYPE_TX);
    }

    //--<3>--清理ep的dma相关状态
    ep_priv->dma_bytes 	 = 0;
    ep_priv->dma_working = 0;

    //--<4>--清除dma中断标志位


    //--<5>--选择vbus,关dma中断,开usb中断, 选择传输方式为bus
    disable_dma_sub_irq();
    switch_bus_to_pio();
    //	enable_ep_irq(ep_priv);

    //--<6>--还原旧的ep
    select_active_ep(old_ep_index);
}

static u32 dma_read_data(struct ep_private *ep_priv,
                         struct urb *urb,
                         u8 ep_index,
                         u32 fifo_addr,
                         u8 *buffer,
                         u32 len)
{
    u32 saddr 		= 0;
    u32 daddr		= 0;
    u32 RqPktCount  = 0;

    //--<1>--check input
    if(ep_priv == NULL || urb == NULL || buffer == NULL)
    {
        DMSG_PANIC("ERR: input error, ep_priv = %x, urb = %x, buffer = %x\n", ep_priv, urb, buffer);
        return 0;
    }

    //__log("dma_read_data:%d\n", len);

    //--<2>--init parameter
    saddr	= (u32)USB_OS_VA2PA((void *)fifo_addr);
    daddr 	= (u32)USB_OS_VA2PA((void *)buffer);

    //--<3>--ep使用DMA传输，记录本次传输大小
    ep_priv->dma_bytes   = len;
    ep_priv->dma_working = 1;

    //--<4>--选择传输方式为dma, 关usb中断，开dma中断
    //	disable_ep_irq(ep_priv);
    switch_bus_to_dma(ep_priv);
    enable_dma_sub_irq();

    //--<5>--刷cache
    USB_OS_FLUSH_DCACHE(buffer, len);

    //--<7>--配置 RqPktCount, 收到的packet个数
    RqPktCount = len / (ep_priv->hep->desc.wMaxPacketSize);
    USBC_Host_ConfigRqPktCount(usbh_bsp_hdle, ep_index, RqPktCount);

    //--<6>--配置ep，auto_clear, dma_rx_en, mode0
    USBC_Host_ConfigEpDma(usbh_bsp_hdle, USBC_EP_TYPE_RX);

    if(!USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_RX)
            && !USBC_Host_IsReqPktSet(usbh_bsp_hdle, USBC_EP_TYPE_RX))
    {
        USBC_Host_StartInToken(usbh_bsp_hdle, USBC_EP_TYPE_RX);
    }

    //--<8>--配置DMA
    dma_set_config(ep_priv, (u32)buffer);

    //--<9>--start dma
    USB_OS_DMA_Start(hDMA_Hdle, saddr, daddr, len);

    return 0;
}

static u32 dma_write_data(struct ep_private *ep_priv,
                          struct urb *urb,
                          u8 ep_index,
                          u32 fifo_addr,
                          u8 *buffer,
                          u32 len)
{
    u32 saddr	= 0;
    u32 daddr	= 0;

    //--<1>--check input
    if(ep_priv == NULL || urb == NULL || buffer == NULL)
    {
        DMSG_PANIC("ERR: input error, ep_priv = %x, urb = %x, buffer = %x\n", ep_priv, urb, buffer);
        return 0;
    }

    //__log("dma_write_data:%d\n", len);

    //--<2>--init parameter
    saddr 	= (u32)USB_OS_VA2PA((void *)buffer);
    daddr	= (u32)USB_OS_VA2PA((void *)fifo_addr);

    //--<3>--ep使用DMA传输，记录本次传输大小
    ep_priv->dma_bytes   = len;
    ep_priv->dma_working = 1;

    //--<4>--选择传输方式为dma, 关usb中断，开dma中断
    //	disable_ep_irq(ep_priv);
    switch_bus_to_dma(ep_priv);
    enable_dma_sub_irq();

    //--<5>--刷cache
    USB_OS_CLEAN_DCACHE(buffer, len);

    //--<6>--配置ep，auto_set, tx_mode, dma_tx_en, mode1
    USBC_Host_ConfigEpDma(usbh_bsp_hdle, USBC_EP_TYPE_TX);

    //--<7>--配置DMA
    dma_set_config(ep_priv, (u32)buffer);

    //--<8>--start dma
    USB_OS_DMA_Start(hDMA_Hdle, saddr, daddr, len);

    return 0;
}

/*
********************************************************************************
*                              __read_data
* Description:
*     从fifo里面读数据
*
* Arguments:
*     ep_priv : input. 记录了ep所有的信息
*     urb     : input. 记录了数据请求包所有的信息
* Return value:
*     返回是否读完
*
* note:
*     void
*
*********************************************************************************
*/
static u32 __read_data(struct ep_private *ep_priv, struct urb *urb)
{
    struct usb_host_virt_dev *udev = NULL;
    u8  *buffer    	= 0;
    u32 fifo_count 	= 0;	/* fifo 里可以读取的数据长度 	*/
    u32 want_len   	= 0;	/* 本次可以读取的数据长度 		*/

    u32 ep_index    = 0;	/* ep索引				 		*/
    u32 fifo_addr	= 0;	/* ep的fifo起始地址		 		*/

    u32 is_last  	= 0;	/* urb 是否处理完毕				*/

    //--<1>--check input
    if((ep_priv == NULL) || (urb == NULL))
    {
        DMSG_PANIC("ERR: input error\n");
        return 0;
    }

    //--<2>--init parameter
    udev    		= urb->dev;
    usb_dotoggle(udev, ep_priv->ep_num, 0);

    buffer  		= (u8 *)((u32)(urb->transfer_buffer) + (u32)(urb->actual_length));
    ep_index        = get_phy_ep_index(ep_priv);
    fifo_addr		= USBC_SelectFIFO(usbh_bsp_hdle, ep_index);

    if(ep_index)
    {
        fifo_count	= USBC_ReadLenFromFifo(usbh_bsp_hdle, USBC_EP_TYPE_RX);
    }
    else
    {
        fifo_count	= USBC_ReadLenFromFifo(usbh_bsp_hdle, USBC_EP_TYPE_EP0);
    }

    //--<5>--本次传输大于整包就用DMA传输，否则使用PIO传输
    if(ep_index)
    {
        want_len = USB_MAX(fifo_count, (urb->transfer_buffer_length - urb->actual_length));

        if(want_len >= ep_priv->maxpacket)
        {
            /* 截取非整包部分 */
            want_len = want_len - (want_len % ep_priv->maxpacket);

            /* dma不忙的时候, 才用dma传输 */
            if(ep_priv->dma_working == 0)
            {
                is_last = dma_read_data(ep_priv, urb, ep_index, fifo_addr, buffer, want_len);
            }
        }
        else
        {
            is_last = pio_read_data(ep_priv, urb, ep_index, fifo_addr, buffer, fifo_count);
        }
    }
    else
    {
        is_last = pio_read_data(ep_priv, urb, ep_index, fifo_addr, buffer, fifo_count);
    }

    return is_last;
}

/*
********************************************************************************
*                              __write_data
* Description:
*     往fifo里面写数据
*
* Arguments:
*     ep_priv : input. 记录了ep所有的信息
*     urb     : input. 记录了数据请求包所有的信息
* Return value:
*     返回是否写完
*
* note:
*     void
*
*********************************************************************************
*/
static u32 __write_data(struct ep_private *ep_priv, struct urb *urb)
{
    struct usb_host_virt_dev *udev = NULL;
    u8  *buffer    	= 0;
    u32 want_len   	= 0;	/* 本次要写的数据长度 			*/

    u8  ep_index    = 0;	/* ep索引				 		*/
    u32 fifo_addr	= 0;	/* ep的fifo起始地址		 		*/

    u32 is_last  	= 0;	/* urb 是否处理完毕				*/

    //--<1>--check input
    if((ep_priv == NULL) || (urb == NULL))
    {
        DMSG_PANIC("ERR: input error\n");
        return 0;
    }

    //--<2>--init parameter
    udev    		= urb->dev;
    usb_dotoggle(udev, ep_priv->ep_num, 1);

    buffer  		= (u8 *)((u32)(urb->transfer_buffer) + (u32)(urb->actual_length));
    ep_index        = get_phy_ep_index(ep_priv);
    fifo_addr = USBC_SelectFIFO(usbh_bsp_hdle, ep_index);

    //--<3>--修正本次读的数据长度
    want_len = urb->transfer_buffer_length - urb->actual_length;

    //--<4>--开始传输

    //--<5>--本次传输大于整包就用DMA传输，否则使用PIO传输
    if(ep_index)
    {
        if(want_len >= ep_priv->maxpacket)
        {
            /* 截取非整包部分 */
            want_len = want_len - (want_len & (ep_priv->maxpacket - 1));

            /* dma不忙的时候, 才用dma传输 */
            if(ep_priv->dma_working == 0)
            {
                is_last = dma_write_data(ep_priv, urb, ep_index, fifo_addr, buffer, want_len);
            }
        }
        else
        {
            is_last = pio_write_data(ep_priv, urb, ep_index, fifo_addr, buffer, want_len);
        }
    }
    else
    {
        is_last = pio_write_data(ep_priv, urb, ep_index, fifo_addr, buffer, want_len);
    }

    return is_last;
}

/*
*******************************************************************************
*                     read_data
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
static u32 read_data(struct ep_private *ep_priv, struct urb *urb)
{
    u32 is_last 		= 0;
    u32 ep_index    	= 0;	/* ep索引				 		*/
    u8  old_ep_index 	= 0;   	/* 旧ep索引				 		*/

    if((ep_priv == NULL) || (urb == NULL))
    {
        DMSG_PANIC("ERR: input error\n");
        return 0;
    }

    ep_index     = get_phy_ep_index(ep_priv);
    old_ep_index = get_active_ep();
    select_active_ep(ep_index);

    //--<1>--准备传输
    hcd_set_urb_busy(urb);
    if(ep_index)
    {
        /* 使用Dma传输的, 就不要判断了 */
        if((USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_RX)
                || (urb->transfer_buffer_length >= ep_priv->maxpacket && ep_index))
                && urb->need_dispose)
        {
            is_last = __read_data(ep_priv, urb);
        }
    }
    else
    {
        if(USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_EP0) && urb->need_dispose)
        {
            is_last = __read_data(ep_priv, urb);
        }
    }
    hcd_set_urb_free(urb);

    //--<2>--如果命令被取消, 就直接finish_request
    if(urb->need_dispose == 0)
    {
        DMSG_PANIC("wrn: read data finish, but happen urb dequeue\n");

        if(ep_priv->dma_working)
        {
            DMSG_PANIC("wrn: urb is done by dma, but happen urb dequeue\n");
            hcd_dma_stop();
            clean_ep_dma_status(ep_priv);
        }

        finish_request(ep_priv, urb);
        select_active_ep(old_ep_index);
        return 1;
    }

    //--<3>--传输完毕后的处理
    if(is_last)
    {
        ep_priv->nextpid = USB_PID_ACK;
    }

    select_active_ep(old_ep_index);

    return is_last;
}

/*
*******************************************************************************
*                     FunctionName
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
static u32 write_data(struct ep_private *ep_priv, struct urb *urb)
{
    u32 is_last 		= 0;
    u32 ep_index    	= 0;	/* ep索引				 		*/
    u8  old_ep_index 	= 0;   	/* 旧ep索引				 		*/

    if((ep_priv == NULL) || (urb == NULL))
    {
        DMSG_PANIC("ERR: input error\n");
        return 0;
    }

    ep_index     = get_phy_ep_index(ep_priv);
    old_ep_index = get_active_ep();
    select_active_ep(ep_index);


    //--<1>--开始传输
    hcd_set_urb_busy(urb);
    if(ep_index)
    {
        if(!USBC_Host_IsWriteDataReady(usbh_bsp_hdle, USBC_EP_TYPE_TX) && urb->need_dispose)
        {
            is_last = __write_data(ep_priv, urb);
        }
    }
    else
    {
        if(!USBC_Host_IsWriteDataReady(usbh_bsp_hdle, USBC_EP_TYPE_EP0) && urb->need_dispose)
        {
            is_last = __write_data(ep_priv, urb);
        }
    }
    hcd_set_urb_free(urb);

    //--<2>--如果命令被取消, 就直接finish_request
    if(urb->need_dispose == 0)
    {
        DMSG_PANIC("wrn: write data finish, but happen urb dequeue\n");

        if(ep_priv->dma_working)
        {
            DMSG_PANIC("wrn: urb is done by dma, but happen urb dequeue\n");
            hcd_dma_stop();
            clean_ep_dma_status(ep_priv);
        }

        finish_request(ep_priv, urb);
        select_active_ep(old_ep_index);

        return 1;
    }

    //--<3>--传输完毕后的处理
    if(is_last)
    {
        //ep0有自己的处理方式,这里不关心
        if(ep_index)
        {
            //finish_request(ep_priv, urb);
            ep_priv->nextpid = USB_PID_ACK;
        }
    }

    select_active_ep(old_ep_index);

    return is_last;
}

static __u32 read_data_fs(struct ep_private *ep_priv, struct urb *urb)
{
    __u32 is_last 		= 0;
    __u32 ep_index    	= 0;	/* ep索引	*/
    __u8  old_ep_index 	= 0;   	/* 旧ep索引	*/
    __u32 fifo_addr		= 0;	/* ep的fifo起始地址	*/
    __u8  *buffer    	= 0;
    __u32 fifo_count    = 0;

    if((ep_priv == NULL) || (urb == NULL))
    {
        DMSG_PANIC("ERR: input error\n");
        return 0;
    }

    ep_index     = get_phy_ep_index(ep_priv);
    old_ep_index = get_active_ep();
    select_active_ep(ep_index);

    /* init paramter */
    fifo_addr = USBC_SelectFIFO(usbh_bsp_hdle, ep_index);

    if(!USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_RX)
            && !USBC_Host_IsReqPktSet(usbh_bsp_hdle, USBC_EP_TYPE_RX)
            && !is_last)
    {
        USBC_Host_StartInToken(usbh_bsp_hdle, USBC_EP_TYPE_RX);
    }

    //--<1>--准备传输
    hcd_set_urb_busy(urb);
    while(USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_RX) && urb->need_dispose && !is_last)
    {
        buffer      = (u8 *)((u32)(urb->transfer_buffer) + (u32)(urb->actual_length));
        fifo_count	= USBC_ReadLenFromFifo(usbh_bsp_hdle, USBC_EP_TYPE_RX);

        is_last = pio_read_data(ep_priv, urb, ep_index, fifo_addr, buffer, fifo_count);

        if(!USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_RX)
                && !USBC_Host_IsReqPktSet(usbh_bsp_hdle, USBC_EP_TYPE_RX)
                && !is_last)
        {
            USBC_Host_StartInToken(usbh_bsp_hdle, USBC_EP_TYPE_RX);
        }
    }
    hcd_set_urb_free(urb);

    //--<2>--如果命令被取消, 就直接finish_request
    if(urb->need_dispose == 0)
    {
        DMSG_PANIC("wrn: read data finish, but happen urb dequeue\n");

        if(ep_priv->dma_working)
        {
            DMSG_PANIC("wrn: urb is done by dma, but happen urb dequeue\n");
            hcd_dma_stop();
            clean_ep_dma_status(ep_priv);
        }

        finish_request(ep_priv, urb);
        select_active_ep(old_ep_index);
        return 1;
    }

    //--<3>--传输完毕后的处理
    if(is_last)
    {
        ep_priv->nextpid = USB_PID_ACK;
    }

    select_active_ep(old_ep_index);

    return is_last;
}

static u32 write_data_fs(struct ep_private *ep_priv, struct urb *urb)
{
    u32 is_last 		= 0;
    u32 ep_index    	= 0;	/* ep索引				 		*/
    u8  old_ep_index 	= 0;   	/* 旧ep索引				 		*/
    __u8 *buffer  		= NULL;
    __u32 fifo_addr		= 0;
    __u32 want_len 		= 0;

    if((ep_priv == NULL) || (urb == NULL))
    {
        DMSG_PANIC("ERR: input error\n");
        return 0;
    }

    ep_index     = get_phy_ep_index(ep_priv);
    old_ep_index = get_active_ep();
    select_active_ep(ep_index);

    fifo_addr = USBC_SelectFIFO(usbh_bsp_hdle, ep_index);

    //--<1>--开始传输
    hcd_set_urb_busy(urb);
    while(!USBC_Host_IsWriteDataReady(usbh_bsp_hdle, USBC_EP_TYPE_TX) && urb->need_dispose && !is_last)
    {
        buffer = (u8 *)((u32)(urb->transfer_buffer) + (u32)(urb->actual_length));
        want_len = min((urb->transfer_buffer_length - urb->actual_length), ep_priv->maxpacket);

        is_last = pio_write_data(ep_priv, urb, ep_index, fifo_addr, buffer, want_len);
    }
    hcd_set_urb_free(urb);

    //--<2>--如果命令被取消, 就直接finish_request
    if(urb->need_dispose == 0)
    {
        DMSG_PANIC("wrn: write data finish, but happen urb dequeue\n");

        if(ep_priv->dma_working)
        {
            DMSG_PANIC("wrn: urb is done by dma, but happen urb dequeue\n");
            hcd_dma_stop();
            clean_ep_dma_status(ep_priv);
        }

        finish_request(ep_priv, urb);
        select_active_ep(old_ep_index);

        return 1;
    }

    //--<3>--传输完毕后的处理
    if(is_last)
    {
        //ep0有自己的处理方式,这里不关心
        if(ep_index)
        {
            ep_priv->nextpid = USB_PID_ACK;
        }
    }

    select_active_ep(old_ep_index);

    return is_last;
}

/*
***************************************************************
*                             usbd_udc_probe
*
* Description:
*     dma中断处理函数
* Parameters:
*     无
* Return value:
*     无
* Notes:
*     无
* history:
*     注意这里的状态清理步骤, 没有十足的把握，请不要改动
*
*     2008.12.18 修改了清理tx/rx的dma state的方法，
*                以前是取整个16bit，修改高8bit，再写回去，现在
*                改为只修改高8bit，后写回去
***************************************************************
*/
static void handle_dma(struct  hc_private *hc_priv)
{
    struct ep_private *ep_priv = NULL;
    struct urb *urb			= NULL;
    u32 completed 			= 0;
    u32 half 				= 0;     //半包
    u8  old_ep_index 		= 0;
    u32 ep_index			= 0;
    u32 i 					= 0;
    __cpu_sr cup_sr 		= 0;

    DMSG_HCD_TEST("handle_dma start\n");

    //--<1>--check input
    if(hc_priv == NULL)
    {
        DMSG_PANIC("ERR: handle_dma, input error\n");
        return;
    }

    USB_OS_ENTER_CRITICAL(cup_sr);

    //--<2>--找到中断所属的ep
    for(i = 0 ; i < HCD_EP_LIST_MAX_EP_NUM ; i++)
    {
        if(hc_priv->ep_list[i] != NULL)
        {
            if(hc_priv->ep_list[i]->dma_working == 1)
            {
                ep_priv = hc_priv->ep_list[i];
                break;
            }
        }
    }

    if((i > HCD_EP_LIST_MAX_EP_NUM) || ep_priv == NULL)
    {
        DMSG_PANIC("ERR: not find a ep belong to dma irq, i = %d, ep_priv = %x\n", i, ep_priv);
        USB_OS_EXIT_CRITICAL(cup_sr);
        return;
    }

    if(ep_priv->dma_working == 0 || ep_priv->hep == NULL)
    {
        DMSG_PANIC("ERR: ep not work in dma status, dma_working = %d, hep = %x\n",
                   ep_priv->dma_working, ep_priv->hep);
        USB_OS_EXIT_CRITICAL(cup_sr);
        return;
    }

    urb = (struct urb *)ep_priv->hep->urb_list.next->data;
    if(urb == NULL)
    {
        DMSG_PANIC("ERR: handle_dma(), urb = NULL\n");
        USB_OS_EXIT_CRITICAL(cup_sr);
        return ;
    }

    //	DMSG_INFO("dma: %d\n", urb->transfer_buffer_length);

    //--<3>--initialize parameter
    ep_index 		= get_phy_ep_index(ep_priv);
    old_ep_index 	= get_active_ep();
    select_active_ep(ep_index);

    //--<4>--如果最后一个包是半包，清除tx的autoset或rx的autoclear
    half = ep_priv->dma_bytes % (ep_priv->maxpacket);
    if(half)
    {
        if(is_in_ep(ep_priv))  /* ep in, rx*/
        {
            USBC_Host_ClearEpDma(usbh_bsp_hdle, USBC_EP_TYPE_RX);
            USBC_Host_ClearRqPktCount(usbh_bsp_hdle, ep_index);
            USBC_Host_ReadDataStatus(usbh_bsp_hdle, USBC_EP_TYPE_RX, 1);
        }
        else   /* ep out, tx*/
        {
            USBC_Host_ClearEpDma(usbh_bsp_hdle, USBC_EP_TYPE_TX);
            USBC_Host_WriteDataStatus(usbh_bsp_hdle, USBC_EP_TYPE_TX, 1);
        }
    }

    //--<5>--清除autoset/autoclear、ep_dma_en、ep_dma_mode
    if(is_in_ep(ep_priv))  /* ep in, rx*/
    {
        USBC_Host_ClearEpDma(usbh_bsp_hdle, USBC_EP_TYPE_RX);
        USBC_Host_ClearRqPktCount(usbh_bsp_hdle, ep_index);
    }
    else   /* ep out, tx*/
    {
        USBC_Host_ClearEpDma(usbh_bsp_hdle, USBC_EP_TYPE_TX);
    }

    //--<6>--选择vbus, 关dma中断,开usb中断
    disable_dma_sub_irq();
    switch_bus_to_pio();
    //	enable_ep_irq(ep_priv);

    //--<7>--清理ep的dma相关状态. 半包处理，如果完了，就done掉；如果没有完就继续传
    urb->actual_length 		+= ep_priv->dma_bytes;
    ep_priv->dma_bytes 		= 0;
    ep_priv->dma_working 	= 0;
    ep_priv->length			= 0;
    if(urb->actual_length == urb->transfer_buffer_length)
    {
        finish_request(ep_priv, urb);
        completed = 1;
    }
    else if(urb->actual_length < urb->transfer_buffer_length)
    {
        DMSG_PANIC("ERR: handle_dma, dma half dispose. want(%d), real(%d)\n",
                   urb->transfer_buffer_length, urb->actual_length);
        if(is_in_ep(ep_priv))  /* ep in, rx*/
        {
            completed = read_data(ep_priv, urb);
        }
        else   /* ep out, tx*/
        {
            completed = write_data(ep_priv, urb);
        }

        if(completed)
        {
            finish_request(ep_priv, urb);
        }
    }
    else
    {
        DMSG_PANIC("ERR: handle_dma, get more data. want(%d), real(%d)\n",
                   urb->transfer_buffer_length, urb->actual_length);
        urb->status = -EFIFO;
        finish_request(ep_priv, urb);
        completed = 1;
    }

    //--<9>--还原旧的ep
    select_active_ep(old_ep_index);

    //--<10>--取出ep的下一个req,可能是double buffer的req
    if(completed)
    {
        start_sched(hc_priv);
    }

    USB_OS_EXIT_CRITICAL(cup_sr);

    DMSG_HCD_TEST("handle_dma end\n");

    return;
}

/*
***********************************************************
* dma中断处理
***********************************************************
*/
__s32 dma_irq_os(void *p_arg)
{
    struct  hc_private *hc_priv = host_controller;

    //只处理属于自己的中断
    handle_dma(hc_priv);

    return EPDK_FALSE;
}

/*
***********************************************************
* dma初始化
*
* bit31~24     main interrupt no
* bit23~16     sub interrupt no
* bit1         dma end interrupt flag
* bit0         dma half end interrupt flag
*
***********************************************************
*/
static s32 hcd_dma_probe(void)
{
    s32 ret;
    char UserName[32];
    char str[32];

    USB_OS_MEMSET(UserName, 0, sizeof(UserName));
    USB_OS_MEMSET(str, 0, sizeof(str));

    USB_OS_STRCPY(UserName, "USB_HOST_");
    USB_OS_uint2str(usbh_port, str);
    USB_OS_STRCAT(UserName, str);

    /* 申请 DMA */
    hDMA_Hdle = USB_OS_DMA_Request(UserName, CSP_DMAC_DMATYPE_DEDICATED);
    if(hDMA_Hdle == NULL)
    {
        DMSG_PANIC("ERR: USB_OS_DMA_Request failed\n");
        return -1;
    }

    /* 注册 DMA 中断 */
    ret = USB_OS_DMA_RegDmaHdler(hDMA_Hdle, CSP_DMA_TRANSFER_END_INT, (__pCBK_t)dma_irq_os, NULL);
    if(ret != EPDK_OK)
    {
        DMSG_PANIC("ERR: USB_OS_DMA_RegDmaHdler failed\n");
        USB_OS_DMA_Release(hDMA_Hdle);
        return -1;
    }

    return 0;
}

/*
***********************************************************
* dma注销
***********************************************************
*/
static s32 hcd_dma_remove(void)
{
    /* 把dma停下来，并且把bus切回到pio */
    disable_dma_sub_irq();
    USB_OS_DMA_Stop(hDMA_Hdle);

    /* 注销 DMA 中断 */
    USB_OS_DMA_UnregDmaHdler(hDMA_Hdle, CSP_DMA_TRANSFER_END_INT, (__pCBK_t)dma_irq_os);

    /* 释放 DMA 句柄 */
    USB_OS_DMA_Release(hDMA_Hdle);

    return 0;
}

#else

/*
********************************************************************************
*                              __read_data
* Description:
*     从fifo里面读数据, 一次至多读maxpacket
*
* Arguments:
*     ep_priv : input. 记录了ep所有的信息
*     urb     : input. 记录了数据请求包所有的信息
* Return value:
*     返回是否读完
*
* note:
*     void
*
*********************************************************************************
*/
static u32 __read_data(struct ep_private *ep_priv, struct urb *urb)
{
    struct hc_private *hc_priv = NULL;
    struct usb_host_virt_dev *udev = NULL;
    u8  *buffer    	= 0;
    u32 fifo_count 	= 0;	/* fifo 里可以读取的数据长度 	*/
    u32 want_len   	= 0;	/* 本次可以读取的数据长度 		*/

    u8  ep_index    = 0;	/* ep索引				 		*/
    u32 fifo_addr	= 0;	/* ep的fifo起始地址		 		*/

    u32 is_last  	= 0;	/* urb 是否处理完毕				*/

    //--<1>--check input
    if((ep_priv == NULL) || (urb == NULL))
    {
        DMSG_PANIC("ERR: input error\n");
        return 0;
    }

    //--<2>--init parameter
    hc_priv 		= ep_priv->hc;
    udev    		= urb->dev;
    usb_dotoggle(udev, ep_priv->ep_num, 0);

    buffer  		= (u8 *)((u32)(urb->transfer_buffer) + (u32)(urb->actual_length));
    ep_index        = get_phy_ep_index(ep_priv);
    fifo_addr		= USBC_SelectFIFO(usbh_bsp_hdle, ep_index);

    if(ep_index)
    {
        fifo_count	= USBC_ReadLenFromFifo(usbh_bsp_hdle, USBC_EP_TYPE_RX);
    }
    else
    {
        fifo_count	= USBC_ReadLenFromFifo(usbh_bsp_hdle, USBC_EP_TYPE_EP0);
    }

    //--<3>--修正本次读的数据长度
    //一定要保证fifo里的数据被读完, 这样USB的状态机就可以继续转起来
    want_len = fifo_count;

    //--<5>--使用PIO传输
    is_last = pio_read_data(ep_priv, urb, ep_index, fifo_addr, buffer, want_len);

    return is_last;
}

/*
********************************************************************************
*                              __write_data
* Description:
*     往fifo里面写数据, 一次至多写maxpacket
*
* Arguments:
*     ep_priv : input. 记录了ep所有的信息
*     urb     : input. 记录了数据请求包所有的信息
* Return value:
*     返回是否写完
*
* note:
*     void
*
*********************************************************************************
*/
static u32 __write_data(struct ep_private *ep_priv, struct urb *urb)
{
    struct hc_private *hc_priv = NULL;
    struct usb_host_virt_dev *udev = NULL;
    u8  *buffer    	= 0;
    u32 want_len   	= 0;	/* 本次要写的数据长度 			*/

    u8  ep_index    = 0;	/* ep索引				 		*/
    u32 fifo_addr	= 0;	/* ep的fifo起始地址		 		*/

    u32 is_last  	= 0;	/* urb 是否处理完毕				*/

    //--<1>--check input
    if((ep_priv == NULL) || (urb == NULL))
    {
        DMSG_PANIC("ERR: input error\n");
        return 0;
    }

    //--<2>--init parameter
    hc_priv 		= ep_priv->hc;
    udev    		= urb->dev;
    usb_dotoggle(udev, ep_priv->ep_num, 1);

    buffer  		= (u8 *)((u32)(urb->transfer_buffer) + (u32)(urb->actual_length));
    ep_index        = get_phy_ep_index(ep_priv);
    fifo_addr = USBC_SelectFIFO(usbh_bsp_hdle, ep_index);

    //--<3>--修正本次读的数据长度
    want_len = urb->transfer_buffer_length - urb->actual_length;

    //--<4>--开始传输


    //--<5>--本次传输大于整包就用DMA传输，否则使用PIO传输
    is_last = pio_write_data(ep_priv, urb, ep_index, fifo_addr, buffer, want_len);

    return is_last;
}

static u32 read_data(struct ep_private *ep_priv, struct urb *urb)
{
    u32 is_last 		= 0;
    u32 ep_index    	= 0;	/* ep索引				 		*/
    u8  old_ep_index 	= 0;   	/* 旧ep索引				 		*/

    if((ep_priv == NULL) || (urb == NULL))
    {
        DMSG_PANIC("ERR: input error\n");
        return 0;
    }

    ep_index     = get_phy_ep_index(ep_priv);
    old_ep_index = get_active_ep();
    select_active_ep(ep_index);

    hcd_set_urb_busy(urb);

    //--<1>--开始传输
    if(ep_index)
    {
        while(USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_RX) && !is_last && urb->need_dispose)
        {
            is_last = __read_data(ep_priv, urb);
        }
    }
    else
    {
        while(USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_EP0) && !is_last && urb->need_dispose)
        {
            is_last = __read_data(ep_priv, urb);
        }
    }

    hcd_set_urb_free(urb);

    //--<2>--如果命令被取消, 就直接finish_request
    if(urb->need_dispose == 0)
    {
        DMSG_PANIC("wrn: read data finish, but happen urb dequeue\n");

        if(ep_priv->dma_working)
        {
            DMSG_PANIC("wrn: urb is done by dma, but happen urb dequeue\n");
            hcd_dma_stop();
            clean_ep_dma_status(ep_priv);
        }

        finish_request(ep_priv, urb);
        select_active_ep(old_ep_index);

        return 1;
    }

    //--<3>--传输完毕后的处理
    if(is_last)
    {
        //ep0有自己的处理方式,这里不关心
        if(ep_index)
        {
            finish_request(ep_priv, urb);
        }
    }

    select_active_ep(old_ep_index);

    return is_last;
}

static u32 write_data(struct ep_private *ep_priv, struct urb *urb)
{
    u32 is_last = 0;
    u32 ep_index    	= 0;	/* ep索引				 		*/
    u8  old_ep_index 	= 0;   	/* 旧ep索引				 		*/

    if((ep_priv == NULL) || (urb == NULL))
    {
        DMSG_PANIC("ERR: input error\n");
        return 0;
    }

    ep_index     = get_phy_ep_index(ep_priv);
    old_ep_index = get_active_ep();
    select_active_ep(ep_index);

    hcd_set_urb_busy(urb);

    //--<1>--开始传输
    if(ep_index)
    {
        while(!USBC_Host_IsWriteDataReady(usbh_bsp_hdle, USBC_EP_TYPE_TX) && !is_last && urb->need_dispose)
        {
            is_last = __write_data(ep_priv, urb);
        }
    }
    else
    {
        while(!USBC_Host_IsWriteDataReady(usbh_bsp_hdle, USBC_EP_TYPE_EP0) && !is_last && urb->need_dispose)
        {
            is_last = __write_data(ep_priv, urb);
        }
    }

    hcd_set_urb_free(urb);

    //--<2>--如果命令被取消, 就直接finish_request
    if(urb->need_dispose == 0)
    {
        DMSG_PANIC("wrn: write data finish, but happen urb dequeue\n");

        if(ep_priv->dma_working)
        {
            DMSG_PANIC("wrn: urb is done by dma, but happen urb dequeue\n");
            hcd_dma_stop();
            clean_ep_dma_status(ep_priv);
        }

        finish_request(ep_priv, urb);
        select_active_ep(old_ep_index);

        return 1;
    }

    //--<3>--传输完毕后的处理
    if(is_last)
    {
        //ep0有自己的处理方式,这里不关心
        if(ep_index)
        {
            finish_request(ep_priv, urb);
        }
    }

    select_active_ep(old_ep_index);

    return is_last;
}

static s32 hcd_dma_stop(void)
{
    return 0;
}

static void clean_ep_dma_status(struct ep_private *ep_priv)
{

}

static s32 hcd_dma_probe(void)
{
    return 0;
}

static s32 hcd_dma_remove(void)
{
    return 0;
}

#endif

static void after_reset_detect(void)
{
    struct hc_private *hcd_priv = host_controller;

    hcd_priv->speed = FULLSPEED; 	//FS is the default

    hcd_priv->port 		|= (USB_PORT_STAT_C_RESET << 16);
    hcd_priv->port 		&=  ~USB_PORT_STAT_RESET;		//exit reset state
    hcd_priv->port 		|= USB_PORT_STAT_ENABLE;			//enter enable state
    hcd_priv->rhstate 	= RH_ENABLE; 				//now root port is enabled

    //detect ,whether HS
    if(USBC_Host_QueryTransferMode(usbh_bsp_hdle) == USBC_TS_MODE_HS)
    {
        hcd_priv->speed	= HIGHSPEED;
        hcd_priv->port 	|= USB_PORT_STAT_HIGH_SPEED;

        DMSG_INFO("INFO : High Speed\n");
    }
    else if(USBC_Host_PeripheralType(usbh_bsp_hdle) == USBC_DEVICE_LSDEV)
    {
        hcd_priv->speed	= LOWSPEED;
        hcd_priv->port	|= USB_PORT_STAT_LOW_SPEED;

        DMSG_INFO("INFO : Low Speed\n");
    }
    else
    {
        hcd_priv->speed  = FULLSPEED;
        //hcd_priv->port |= USB_PORT_STAT_FULL_SPEED;

        DMSG_INFO("INFO : Full Speed\n");
    }

    return;
}

/*
********************************************************************************
*                              hcd_nuke_urb
* Description:
*     删除所有与ep相关的urb
*
* Arguments:
*     ep_priv : input. 记录了ep所有的信息
*
* Return value:
*
* note:
*     void
*
*********************************************************************************
*/
static void hcd_nuke_urb(struct ep_private *ep_priv)
{
    struct list_head *list_head  	= NULL;
    struct list_head *list_now  	= NULL;
    struct list_head *list_next 	= NULL;
    struct urb *urb 				= NULL;
    //	struct hc_private * hc_priv		= NULL;
    __cpu_sr cup_sr					= 0;

    //--<1>--check input
    if(ep_priv == NULL)
    {
        DMSG_PANIC("ERR: hcd_nuke_urb, input error\n");
        return;
    }

    //--<2>--initialize parameter
    if(ep_priv->hep == NULL)
    {
        DMSG_PANIC("ERR: ep_priv->hep == NULL\n");
        return;
    }

    list_head = &(ep_priv->hep->urb_list);
    list_now  = list_head->next;
    list_next = NULL;

    //--<3>--delete node form list
    while(list_now != list_head)
    {
        //--<1>--预先取得下一个node
        list_next = list_now->next;

        //--<2>--删除当前node, 并且释放其资源
        urb = list_now->data;
        if(urb)
        {
            DMSG_PANIC("hcd_nuke_urb: urb = %x\n", urb);
            urb->status = -ECONNRESET;
            finish_request(ep_priv, urb);
        }
        else
        {
            DMSG_PANIC("ERR: urb is NULL\n");
        }

        USB_OS_ENTER_CRITICAL(cup_sr);
        list_del_init(list_now);
        _list_head_free(list_now);
        list_now = NULL;
        USB_OS_EXIT_CRITICAL(cup_sr);

        //--<3>--取得下一个node
        list_now = list_next;
    }

    return;
}

/*
********************************************************************************
*                              hcd_nuke_ep
* Description:
*     从hc_priv->async队列中删除所有与ep_priv相关的node, 并且处理ep上的所有urb
*
* Arguments:
*     hc_priv : input. 记录了ep所有的信息
*     ep_priv : input. 记录了hc所有的信息
*
* Return value:
*
* note:
*     void
*
*********************************************************************************
*/
static void hcd_nuke_aysnc_ep(struct hc_private *hc_priv, struct ep_private *ep_priv)
{
    __cpu_sr cup_sr	= 0;

    //--<1>--check input
    if((hc_priv == NULL) || (ep_priv == NULL))
    {
        DMSG_PANIC("ERR: hcd_nuke_urb, input error\n");
        return ;
    }

    USB_OS_ENTER_CRITICAL(cup_sr);

    //--<2>--delete all node refer to ep_priv
    list_del_node_by_data((void *)ep_priv, &(hc_priv->async));

    //--<3>--delete all urb on ep_urb_list
    hcd_nuke_urb(ep_priv);

    USB_OS_EXIT_CRITICAL(cup_sr);

    return ;
}

static void hcd_nuke_periodic_ep(struct hc_private *hc_priv, struct ep_private *ep_priv)
{
    __cpu_sr cup_sr	= 0;
    __u32 i = 0;

    //--<1>--check input
    if((hc_priv == NULL) || (ep_priv == NULL))
    {
        DMSG_PANIC("ERR: hcd_nuke_urb, input error\n");
        return ;
    }

    USB_OS_ENTER_CRITICAL(cup_sr);
    for(i = 0; i < PERIODIC_SIZE; i++)
    {
        if(hc_priv->periodic[i] == ep_priv)
        {
            DMSG_INFO("hcd_nuke_periodic_ep: ep_priv = %x\n", ep_priv);
            hc_priv->periodic[i] = NULL;
            hcd_nuke_urb(ep_priv);
        }
    }
    USB_OS_EXIT_CRITICAL(cup_sr);

    return ;
}

/*
********************************************************************************
*                              hcd_nuke_ep
* Description:
*     从hc_priv->async队列中删除所有与ep_priv相关的node, 并且处理ep上的所有urb
*
* Arguments:
*     hc_priv : input. 记录了ep所有的信息
*     ep_priv : input. 记录了hc所有的信息
*
* Return value:
*
* note:
*     void
*
*********************************************************************************
*/
static void hcd_nuke_aysnc_ep_list(struct hc_private *hc_priv)
{
    struct ep_private *ep_priv 		= NULL;
    struct list_head *list_head  	= NULL;
    struct list_head *list_now  	= NULL;
    struct list_head *list_next 	= NULL;
    __cpu_sr cup_sr					= 0;

    //--<1>--check input
    if(hc_priv == NULL)
    {
        DMSG_PANIC("ERR: hcd_nuke_hc_ep_list: input error\n");
        return ;
    }

    USB_OS_ENTER_CRITICAL(cup_sr);

    hc_priv->curr_async = NULL;
    hc_priv->curr_aync_node = NULL;

    list_head = &(hc_priv->async);
    list_now  = list_head->next;
    list_next = NULL;

    while(list_head != list_now)
    {
        list_next = list_now->next;

        ep_priv = (struct ep_private *)list_now->data;
        if(ep_priv)
        {
            if(ep_priv->dma_working)
            {
                DMSG_PANIC("wrn: urb is done by dma, but hc ep list is nuke\n");
                hcd_dma_stop();
                clean_ep_dma_status(ep_priv);
            }

            hcd_nuke_urb(ep_priv);
        }
        else
        {
            DMSG_PANIC("ERR: ep_priv is NULL\n");
        }

        list_del_init(list_now);
        _list_head_free(list_now);
        list_now = NULL;
        ep_priv = NULL;

        list_now = list_next;
    }

    USB_OS_EXIT_CRITICAL(cup_sr);

    return ;
}

static void hcd_nuke_periodic_ep_list(struct hc_private *hc_priv)
{
    struct ep_private *ep_priv = NULL;
    __u32 i = 0;

    for(i = 0; i < PERIODIC_SIZE; i++)
    {
        ep_priv = hc_priv->periodic[i];
        if(ep_priv)
        {
            hcd_nuke_urb(ep_priv);
        }
    }

    return ;
}

static void hcd_nuke_hc_ep_list(struct hc_private *hc_priv)
{
    hcd_nuke_aysnc_ep_list(hc_priv);
    hcd_nuke_periodic_ep_list(hc_priv);

    return ;
}


static struct ep_private *find_a_valid_aysnc_ep_from_list(struct hc_private *hc_priv)
{
    struct ep_private *ep_priv 	= NULL;

    //--<1>--check input
    if(hc_priv == NULL)
    {
        DMSG_PANIC("ERR: find_a_valid_ep_from_list, input error\n");
        return NULL;
    }

    if(!list_empty(&(hc_priv->async)))
    {
        ep_priv = (struct ep_private *)hc_priv->async.next->data;
    }

    return ep_priv;
}

static struct urb *find_a_valid_urb_from_list(struct ep_private *ep_priv)
{
    struct urb *urb = NULL;
    struct list_head *list_head = NULL;
    struct list_head *list_now 	= NULL;
    struct list_head *list_next = NULL;
    __cpu_sr cup_sr	= 0;

    //--<1>--check input
    if(ep_priv == NULL)
    {
        DMSG_PANIC("ERR: find_a_valid_urb_from_list, input error\n");
        return NULL;
    }

    if(ep_priv->hep == NULL)
    {
        DMSG_PANIC("ERR: find_a_valid_urb_from_list, ep_priv->hep == NULL\n");
        return NULL;
    }

    if(ep_priv->hep->urb_list.next == NULL)
    {
        DMSG_PANIC("ERR: find_a_valid_urb_from_list, ep_priv->hep->urb_list is not initialize\n");
        return NULL;
    }

    USB_OS_ENTER_CRITICAL(cup_sr);

    list_head = &(ep_priv->hep->urb_list);
    list_now  = list_head->next;
    list_next = NULL;

    //--<2>--check urb_list
    if(list_empty(list_head))
    {
        DMSG_HCD_TEST("ERR: find_a_valid_urb_from_list, urb_list is enpty\n");
        USB_OS_EXIT_CRITICAL(cup_sr);
        return NULL;
    }

    //--<3>--
    while(list_now != list_head)
    {
        list_next = list_now->next;

        urb = (struct urb *)list_now->data;
        if(urb)
        {
            break;
        }
        else
        {
            DMSG_PANIC("ERR: find_a_valid_urb_from_list, urb == NULL\n");
        }

        list_now = list_next;
    }

    USB_OS_EXIT_CRITICAL(cup_sr);

    return urb;
}


/*
********************************************************************************
*                              send_setup_packet
* Description:
*     发送setup命令
*
* Arguments:
*     ep_priv : input. 记录了ep所有的信息
*     urb     : input. 记录了数据请求包所有的信息
* Return value:
*     返回是否写完
*
* note:
*     void
*
*********************************************************************************
*/
static s32 send_setup_packet(struct ep_private *ep_priv, struct urb *urb)
{
    struct usb_ctrlrequest  *ctrlreq = NULL;
    u8  *buffer 		= NULL;
    u32 want_len 		= 0;
    u32 real_len		= 0;
    u32 fifo_addr		= 0;
    u8  old_ep_index	= 0;
    __cpu_sr cup_sr		= 0;

    //--<1>--check input
    if(ep_priv == NULL || urb == NULL)
    {
        DMSG_PANIC("ERR: send_setup_packet, input error\n");
        return -1;
    }

    USB_OS_ENTER_CRITICAL(cup_sr);

    hcd_set_urb_busy(urb);

    //--<2>--init parameter
    ctrlreq   = (struct usb_ctrlrequest *)urb->setup_packet;
    buffer    = (u8 *)urb->setup_packet;
    want_len  = sizeof(struct usb_ctrlrequest);
    fifo_addr = USBC_SelectFIFO(usbh_bsp_hdle, 0);

    //--<3>--do specail ctrl req, set_addr.

    //--<4>--select ep
    old_ep_index = get_active_ep();
    select_active_ep(0);

    //--<5>--write data
    real_len = USBC_WritePacket(usbh_bsp_hdle, fifo_addr, want_len, buffer);
    if(real_len != want_len)
    {
        DMSG_PANIC("ERR: send_setup_packet, USBC_WritePacket failed, want_len = %d, real_len = %d\n",
                   want_len, real_len);
        goto setup_err;
    }

    //--<6>--do write status
    USBC_Host_SendSetupPkt(usbh_bsp_hdle);

    //--<7>--设置data阶段的方向
    if(ctrlreq->bRequestType & 0x80)
    {
        ep_priv->ep0_data_dir = EP_PRIV_EP_DATA_DIR_IN;
    }
    else
    {
        ep_priv->ep0_data_dir = EP_PRIV_EP_DATA_DIR_OUT;
    }

setup_err:
    //--<8>--comeback old ep
    select_active_ep(old_ep_index);

    hcd_set_urb_free(urb);

    USB_OS_EXIT_CRITICAL(cup_sr);

    return 0;
}

static u32 start_transfer_aysnc_urb(struct ep_private *ep_priv, struct urb *urb)
{
    u32 completed	= 0;
    __cpu_sr cup_sr	= 0;

    //--<1>--check input
    if(ep_priv == NULL || urb == NULL)
    {
        DMSG_PANIC("ERR: start_transfer, input error\n");
        return 0;
    }

    USB_OS_ENTER_CRITICAL(cup_sr);

    //--<2>--check urb, 如果urb已经dequeue, 这里就直接返回
    if(urb->need_dispose == 0)
    {
        DMSG_PANIC("ERR: start_transfer_aysnc_urb, urb is dequeue, (%d, %d, %x)\n",
                   urb->transfer_buffer_length, urb->actual_length, urb->status);
        finish_request(ep_priv, urb);
        USB_OS_EXIT_CRITICAL(cup_sr);
        return 1;
    }

    if(ep_priv->current_urb == NULL)
    {
        ep_priv->current_urb = urb;
    }
    else
    {
        if(ep_priv->current_urb != urb)
        {
            DMSG_PANIC("ep current_urb is different from input urb\n");
            USB_OS_EXIT_CRITICAL(cup_sr);
            return 1;
        }
    }

    //--<3>--done urb
    if(ep_priv->ep_ts_type == PIPE_CONTROL)
    {
        switch(ep_priv->nextpid)
        {
        case USB_PID_SETUP: 	/* 发送setup命令 */
            send_setup_packet(ep_priv, urb);
            break;

        case USB_PID_IN:
            /* 数据没有收完, 就发送in tocken */
            if(!USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_EP0)
                    && !USBC_Host_IsReqPktSet(usbh_bsp_hdle, USBC_EP_TYPE_EP0))
            {
                USBC_Host_StartInToken(usbh_bsp_hdle, USBC_EP_TYPE_EP0);
            }
            break;

        case USB_PID_OUT:
            completed = write_data(ep_priv, urb);
            break;

        case USB_PID_ACK:  		/* ack */
            /* nothing to do */
            break;

        default:
            DMSG_PANIC("[hcd]: unkown nextpid(%x)\n", ep_priv->nextpid);
            break;
        }
    }
    else if(ep_priv->ep_ts_type == PIPE_BULK)
    {
        switch(ep_priv->nextpid)
        {
        case USB_PID_IN:
        {
            if(ep_priv->udev->speed == USB_SPEED_HIGH)
            {
                u32 old_ep_index = 0;

                old_ep_index = get_active_ep();
                select_active_ep(get_phy_ep_index(ep_priv));

                /* 如果是high speed DMA传输, 这里就不用发Intoken了 */
                if(!USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_RX)
                        && !USBC_Host_IsReqPktSet(usbh_bsp_hdle, USBC_EP_TYPE_RX)
                        && (urb->transfer_buffer_length < ep_priv->maxpacket))
                {
                    USBC_Host_StartInToken(usbh_bsp_hdle, USBC_EP_TYPE_RX);
                }

                select_active_ep(old_ep_index);

                completed = read_data(ep_priv, urb);
                if(ep_priv->nextpid == USB_PID_ACK)
                {
                    finish_request(ep_priv, urb);
                    urb = NULL;
                    break;
                }
            }
            else
            {
                /* 请求包大于512，就启动DMA */
                if(urb->transfer_buffer_length > 512)
                {
                    completed = read_data(ep_priv, urb);
                    if(ep_priv->nextpid == USB_PID_ACK)
                    {
                        finish_request(ep_priv, urb);
                        urb = NULL;
                        break;
                    }
                }
                else
                {
                    completed = read_data_fs(ep_priv, urb);
                    if(ep_priv->nextpid == USB_PID_ACK)
                    {
                        finish_request(ep_priv, urb);
                        urb = NULL;
                        break;
                    }
                }
            }
        }
        break;

        case USB_PID_OUT:
            if(ep_priv->udev->speed == USB_SPEED_HIGH)
            {
                completed = write_data(ep_priv, urb);
            }
            else
            {
                /* 请求包大于512，就启动DMA */
                if(urb->transfer_buffer_length > 512)
                {
                    completed = write_data(ep_priv, urb);
                }
                else
                {
                    completed = write_data_fs(ep_priv, urb);
                }
            }
            break;

        case USB_PID_ACK:  		/* ack */
            if(!is_in_ep(ep_priv))
            {
                finish_request(ep_priv, urb);
                urb = NULL;
            }
            else
            {
                DMSG_PANIC("wrn: bulk USB_PID_ACK from read data\n");
            }
            break;

        default:
            DMSG_PANIC("[hcd]: unkown nextpid(%x)\n", ep_priv->nextpid);
            break;
        }
    }
    else
    {
        DMSG_PANIC("ERR: not support transfer type(%x)\n", ep_priv->ep_ts_type);
    }

    USB_OS_EXIT_CRITICAL(cup_sr);

    return completed;
}

static u32 start_transfer_aysnc_ep(struct ep_private *ep_priv)
{
    struct urb *urb = NULL;

    //--<1>--check input
    if(ep_priv == NULL)
    {
        DMSG_PANIC("ERR: start_transfer_ep, input error\n");
        return 0;
    }

    //--<3>--find a valid urb
    urb = find_a_valid_urb_from_list(ep_priv);
    if(urb == NULL)
    {
        //DMSG_PANIC("ERR: start_transfer_ep, there is no valid urb\n");
        return 0;
    }

    //--<4>--do urb
    return start_transfer_aysnc_urb(ep_priv, urb);
}

/*
********************************************************************************
*                              start_async_transfer
* Description:
*     开始异步传输
*
* Arguments:
*     hc_priv : input. 记录了hcd所有的信息
* Return value:
*     返回是否写完
*
* note:
*     void
*
*********************************************************************************
*/
static u32 start_async_transfer(struct hc_private *hc_priv)
{
    struct ep_private *ep_priv 	= NULL;

    //--<1>--check input
    if(hc_priv == NULL)
    {
        DMSG_PANIC("ERR: start_async_transfer, input error\n");
        return 0;
    }

    if (!HC_GEN_DEV_IS_RUNNING(to_hcd(hc_priv)->state) || (hc_priv->port & USB_PORT_STAT_SUSPEND))
    {
        DMSG_PANIC("stop async schedule,HC is disable or root hub suspend\n");
        return 0;
    }

    //--<2>--find a valid ep.
    ep_priv = hc_priv->curr_async;
    if(ep_priv == NULL)
    {
        ep_priv = find_a_valid_aysnc_ep_from_list(hc_priv);
        if(ep_priv == NULL)
        {
            //DMSG_PANIC("ERR: start_async_transfer, there is no valid ep\n");
            return 0;
        }

        hc_priv->curr_async = ep_priv;
    }

    if((ep_priv->ep_ts_type != PIPE_CONTROL) && (ep_priv->ep_ts_type != PIPE_BULK))
    {
        DMSG_PANIC("ERR: only support contrl & bulk transfer, %d\n", ep_priv->ep_ts_type);
        return 0;
    }

    return start_transfer_aysnc_ep(ep_priv);
}


#ifdef  HCD_INT_ISO_TRANSFER

static int balance(struct hc_private *hcd , u16 period , u16 load)
{
    int i = 0, branch = -ENOSPC;

    //	DMSG_INFO("<balance>: period = %d load = %d\n",period,load);
    for(i = 0; i < period; i++)
    {
        if(branch < 0 || hcd->load[branch] > hcd->load[i])
        {
            int j = 0;

            for(j = i; j < PERIODIC_SIZE; j += period)
            {
                ////FS
                if((hcd->load[j] + load) > MAX_PERIODIC_LOAD)
                {
                    break;
                }
            }

            if(j < PERIODIC_SIZE)
            {
                continue;
            }

            branch = i;
        }
    }

    return branch;
}

/* 读取剩余的帧 */
static __u32 hcd_get_left_frame(void)
{
    return USBC_GetLastFrameNumber(usbh_bsp_hdle);
}

/* return 1 means no time */
static __u32 hcd_is_stop_async_transfer(struct hc_private *hc_priv)
{
    if(hc_priv->speed == (u8)FULLSPEED)
    {
        if(hcd_get_left_frame() <=  79)	//1500/19 =79
            return 1;
    }
    else if (hc_priv->speed == HIGHSPEED)
    {
        if(hcd_get_left_frame() <= (7500 / 13))
            return 1;
    }
    else
    {
        DMSG_INFO("ERR: not support low speed transfer\n");
    }

    return 0;
}


static __u32 read_data_period(struct ep_private *ep_priv, struct urb *urb)
{
    __u32 is_last 		= 0;
    __u32 ep_index    	= 0;	/* ep索引	*/
    __u8  old_ep_index 	= 0;   	/* 旧ep索引	*/
    __u32 fifo_addr		= 0;	/* ep的fifo起始地址	*/
    __u8  *buffer    	= 0;
    __u32 fifo_count    = 0;
    struct hc_private *hc_priv = NULL;

    if((ep_priv == NULL) || (urb == NULL))
    {
        DMSG_PANIC("ERR: input error\n");
        return 0;
    }

    hc_priv = ep_priv->hc;
    if(hc_priv == NULL)
    {
        DMSG_PANIC("ERR: hc_priv == NULL\n");
        return 0;
    }

    ep_index     = get_phy_ep_index(ep_priv);
    old_ep_index = get_active_ep();
    select_active_ep(ep_index);

    /* init paramter */
    fifo_addr = USBC_SelectFIFO(usbh_bsp_hdle, ep_index);

    if(!USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_RX)
            && !USBC_Host_IsReqPktSet(usbh_bsp_hdle, USBC_EP_TYPE_RX)
            && !is_last)
    {
        USBC_Host_StartInToken(usbh_bsp_hdle, USBC_EP_TYPE_RX);
    }

    //--<1>--准备传输
    hcd_set_urb_busy(urb);
    while(USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_RX)
            && urb->need_dispose
            && !is_last
            && !hcd_is_stop_async_transfer(hc_priv))
    {
        buffer      = (u8 *)((u32)(urb->transfer_buffer) + (u32)(urb->actual_length));
        fifo_count	= USBC_ReadLenFromFifo(usbh_bsp_hdle, USBC_EP_TYPE_RX);

        is_last = pio_read_data(ep_priv, urb, ep_index, fifo_addr, buffer, fifo_count);

        if(!USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_RX)
                && !USBC_Host_IsReqPktSet(usbh_bsp_hdle, USBC_EP_TYPE_RX)
                && !is_last)
        {
            USBC_Host_StartInToken(usbh_bsp_hdle, USBC_EP_TYPE_RX);
        }

    }
    hcd_set_urb_free(urb);

    //--<2>--如果命令被取消, 就直接finish_request
    if(urb->need_dispose == 0)
    {
        DMSG_PANIC("wrn: read data finish, but happen urb dequeue\n");

        if(ep_priv->dma_working)
        {
            DMSG_PANIC("wrn: urb is done by dma, but happen urb dequeue\n");
            hcd_dma_stop();
            clean_ep_dma_status(ep_priv);
        }

        finish_request(ep_priv, urb);
        select_active_ep(old_ep_index);
        return 1;
    }

    select_active_ep(old_ep_index);

    //--<3>--传输完毕后的处理
    if(is_last)
    {
        ep_priv->nextpid = USB_PID_ACK;
        finish_request(ep_priv, urb);
        urb = NULL;
    }

    return is_last;
}

static u32 write_data_period(struct ep_private *ep_priv, struct urb *urb)
{
    u32 is_last 		= 0;
    u32 ep_index    	= 0;	/* ep索引				 		*/
    u8  old_ep_index 	= 0;   	/* 旧ep索引				 		*/
    __u8 *buffer  		= NULL;
    __u32 fifo_addr		= 0;
    __u32 want_len 		= 0;

    if((ep_priv == NULL) || (urb == NULL))
    {
        DMSG_PANIC("ERR: input error\n");
        return 0;
    }

    ep_index     = get_phy_ep_index(ep_priv);
    old_ep_index = get_active_ep();
    select_active_ep(ep_index);

    fifo_addr = USBC_SelectFIFO(usbh_bsp_hdle, ep_index);

    //--<1>--开始传输
    hcd_set_urb_busy(urb);
    while(!USBC_Host_IsWriteDataReady(usbh_bsp_hdle, USBC_EP_TYPE_TX) && urb->need_dispose && !is_last)
    {
        buffer = (u8 *)((u32)(urb->transfer_buffer) + (u32)(urb->actual_length));
        want_len = min((urb->transfer_buffer_length - urb->actual_length), ep_priv->maxpacket);

        is_last = pio_write_data(ep_priv, urb, ep_index, fifo_addr, buffer, want_len);
    }
    hcd_set_urb_free(urb);

    //--<2>--如果命令被取消, 就直接finish_request
    if(urb->need_dispose == 0)
    {
        DMSG_PANIC("wrn: write data finish, but happen urb dequeue\n");

        if(ep_priv->dma_working)
        {
            DMSG_PANIC("wrn: urb is done by dma, but happen urb dequeue\n");
            hcd_dma_stop();
            clean_ep_dma_status(ep_priv);
        }

        finish_request(ep_priv, urb);
        select_active_ep(old_ep_index);

        return 1;
    }

    //--<3>--传输完毕后的处理
    if(is_last)
    {
        //ep0有自己的处理方式,这里不关心
        if(ep_index)
        {
            //finish_request(ep_priv, urb);
            ep_priv->nextpid = USB_PID_ACK;
        }
    }

    select_active_ep(old_ep_index);

    return is_last;
}



static __s32 start_period_transfer(struct hc_private *hc_priv)
{
    struct ep_private *ep_priv = NULL;
    struct urb *urb = NULL;
    __u32 complete  = 0;
    __cpu_sr cup_sr = 0;

    if(hc_priv == NULL)
    {
        DMSG_PANIC("ERR: input error\n");
        return complete;
    }

    /* 调整下一次需调度的ep */
    if(hc_priv->next_periodic)
    {
        ep_priv = hc_priv->next_periodic;
        hc_priv->next_periodic = ep_priv->next;
    }
    else
    {
        //DMSG_INFO("ERR: No ep to schedule\n");
        return complete;
    }

    /* 查找有效的待处理ep */
    urb = find_a_valid_urb_from_list(ep_priv);
    if(urb == NULL)
    {
        DMSG_PANIC("ERR: there is no valid urb\n");
        return complete;
    }

    /* check urb, 如果urb已经dequeue, 这里就直接返回 */
    if(urb->need_dispose == 0)
    {
        DMSG_PANIC("ERR: urb is dequeue\n");
        finish_request(ep_priv, urb);
        return 1;
    }

    if(ep_priv->current_urb == NULL)
    {
        ep_priv->current_urb = urb;
    }
    else
    {
        if(ep_priv->current_urb != urb)
        {
            DMSG_PANIC("ep current_urb is different from input urb\n");
            return 1;
        }
    }

    /* transfer */
    if(ep_priv->ep_ts_type == PIPE_INTERRUPT)
    {
        switch(ep_priv->nextpid)
        {
        case USB_PID_IN:
            USB_OS_ENTER_CRITICAL(cup_sr);
            complete = read_data_period(ep_priv, urb);
            USB_OS_EXIT_CRITICAL(cup_sr);
            break;

        case USB_PID_OUT:
            USB_OS_ENTER_CRITICAL(cup_sr);
            complete = write_data_period(ep_priv, urb);
            USB_OS_EXIT_CRITICAL(cup_sr);
            break;

        case USB_PID_ACK:  		/* ack */
            if(!is_in_ep(ep_priv))
            {
                finish_request(ep_priv, urb);
                urb = NULL;
            }
            else
            {
                DMSG_PANIC("wrn: bulk USB_PID_ACK from read data\n");
            }
            break;

        default:
            DMSG_PANIC("ERR: unkown ep_ts_type(%x)\n", ep_priv->ep_ts_type);
        }
    }
    else
    {
        DMSG_PANIC("ERR: Not support periodic transfer(%x)\n", ep_priv->ep_ts_type);
    }

    return complete;
}

#else

static __u32 read_data_period(struct ep_private *ep_priv, struct urb *urb)
{
    __u32 is_last 		= 0;
    __u32 ep_index    	= 0;	/* ep索引	*/
    __u8  old_ep_index 	= 0;   	/* 旧ep索引	*/
    __u32 fifo_addr		= 0;	/* ep的fifo起始地址	*/
    __u8  *buffer    	= 0;
    __u32 fifo_count    = 0;
    struct hc_private *hc_priv = NULL;

    if((ep_priv == NULL) || (urb == NULL))
    {
        DMSG_PANIC("ERR: input error\n");
        return 0;
    }

    hc_priv = ep_priv->hc;
    if(hc_priv == NULL)
    {
        DMSG_PANIC("ERR: hc_priv == NULL\n");
        return 0;
    }

    ep_index     = get_phy_ep_index(ep_priv);
    old_ep_index = get_active_ep();
    select_active_ep(ep_index);

    /* init paramter */
    fifo_addr = USBC_SelectFIFO(usbh_bsp_hdle, ep_index);

    if(!USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_RX)
            && !USBC_Host_IsReqPktSet(usbh_bsp_hdle, USBC_EP_TYPE_RX)
            && !is_last)
    {
        USBC_Host_StartInToken(usbh_bsp_hdle, USBC_EP_TYPE_RX);
    }

    //--<1>--准备传输
    hcd_set_urb_busy(urb);
    while(USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_RX)
            && urb->need_dispose
            && !is_last)
    {
        buffer      = (u8 *)((u32)(urb->transfer_buffer) + (u32)(urb->actual_length));
        fifo_count	= USBC_ReadLenFromFifo(usbh_bsp_hdle, USBC_EP_TYPE_RX);

        is_last = pio_read_data(ep_priv, urb, ep_index, fifo_addr, buffer, fifo_count);

        if(!USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_RX)
                && !USBC_Host_IsReqPktSet(usbh_bsp_hdle, USBC_EP_TYPE_RX)
                && !is_last)
        {
            USBC_Host_StartInToken(usbh_bsp_hdle, USBC_EP_TYPE_RX);
        }

    }
    hcd_set_urb_free(urb);

    //--<2>--如果命令被取消, 就直接finish_request
    if(urb->need_dispose == 0)
    {
        DMSG_PANIC("wrn: read data finish, but happen urb dequeue\n");

        if(ep_priv->dma_working)
        {
            DMSG_PANIC("wrn: urb is done by dma, but happen urb dequeue\n");
            hcd_dma_stop();
            clean_ep_dma_status(ep_priv);
        }

        finish_request(ep_priv, urb);
        select_active_ep(old_ep_index);
        return 1;
    }

    select_active_ep(old_ep_index);

    //--<3>--传输完毕后的处理
    if(is_last)
    {
        ep_priv->nextpid = USB_PID_ACK;
        finish_request(ep_priv, urb);
        urb = NULL;
    }

    return is_last;
}

static u32 write_data_period(struct ep_private *ep_priv, struct urb *urb)
{
    u32 is_last 		= 0;
    u32 ep_index    	= 0;	/* ep索引				 		*/
    u8  old_ep_index 	= 0;   	/* 旧ep索引				 		*/
    __u8 *buffer  		= NULL;
    __u32 fifo_addr		= 0;
    __u32 want_len 		= 0;

    if((ep_priv == NULL) || (urb == NULL))
    {
        DMSG_PANIC("ERR: input error\n");
        return 0;
    }

    ep_index     = get_phy_ep_index(ep_priv);
    old_ep_index = get_active_ep();
    select_active_ep(ep_index);

    fifo_addr = USBC_SelectFIFO(usbh_bsp_hdle, ep_index);

    //--<1>--开始传输
    hcd_set_urb_busy(urb);
    while(!USBC_Host_IsWriteDataReady(usbh_bsp_hdle, USBC_EP_TYPE_TX) && urb->need_dispose && !is_last)
    {
        buffer = (u8 *)((u32)(urb->transfer_buffer) + (u32)(urb->actual_length));
        want_len = min((urb->transfer_buffer_length - urb->actual_length), ep_priv->maxpacket);

        is_last = pio_write_data(ep_priv, urb, ep_index, fifo_addr, buffer, want_len);
    }
    hcd_set_urb_free(urb);

    //--<2>--如果命令被取消, 就直接finish_request
    if(urb->need_dispose == 0)
    {
        DMSG_PANIC("wrn: write data finish, but happen urb dequeue\n");

        if(ep_priv->dma_working)
        {
            DMSG_PANIC("wrn: urb is done by dma, but happen urb dequeue\n");
            hcd_dma_stop();
            clean_ep_dma_status(ep_priv);
        }

        finish_request(ep_priv, urb);
        select_active_ep(old_ep_index);

        return 1;
    }

    //--<3>--传输完毕后的处理
    if(is_last)
    {
        //ep0有自己的处理方式,这里不关心
        if(ep_index)
        {
            //finish_request(ep_priv, urb);
            ep_priv->nextpid = USB_PID_ACK;
        }
    }

    select_active_ep(old_ep_index);

    return is_last;
}



static __s32 start_period_transfer_ep(struct ep_private *ep_priv)
{
    struct hc_private *hc_priv = NULL;
    struct urb *urb = NULL;
    __u32 complete  = 0;
    __cpu_sr cup_sr = 0;

    if(ep_priv == NULL)
    {
        DMSG_PANIC("ERR: start_period_transfer_ep, input error\n");
        return complete;
    }

    hc_priv = ep_priv->hc;
    if(hc_priv == NULL)
    {
        DMSG_PANIC("ERR: start_period_transfer_ep, hc_priv == NULL\n");
        return complete;
    }

    if (!HC_GEN_DEV_IS_RUNNING(to_hcd(hc_priv)->state) || (hc_priv->port & USB_PORT_STAT_SUSPEND))
    {
        DMSG_PANIC("stop period schedule,HC is disable or root hub suspend\n");
        return complete;
    }

    /* 查找有效的待处理ep */
    urb = find_a_valid_urb_from_list(ep_priv);
    if(urb == NULL)
    {
        DMSG_PANIC("ERR: there is no valid urb\n");
        return complete;
    }

    /* check urb, 如果urb已经dequeue, 这里就直接返回 */
    if(urb->need_dispose == 0)
    {
        DMSG_PANIC("ERR: urb is dequeue\n");
        finish_request(ep_priv, urb);
        return 1;
    }

    if(ep_priv->current_urb == NULL)
    {
        ep_priv->current_urb = urb;
    }
    else
    {
        if(ep_priv->current_urb != urb)
        {
            DMSG_PANIC("ep current_urb is different from input urb\n");
            return 1;
        }
    }

    /* transfer */
    if(ep_priv->ep_ts_type == PIPE_INTERRUPT)
    {
        switch(ep_priv->nextpid)
        {
        case USB_PID_IN:
            USB_OS_ENTER_CRITICAL(cup_sr);
            complete = read_data_period(ep_priv, urb);
            USB_OS_EXIT_CRITICAL(cup_sr);
            break;

        case USB_PID_OUT:
            USB_OS_ENTER_CRITICAL(cup_sr);
            complete = write_data_period(ep_priv, urb);
            USB_OS_EXIT_CRITICAL(cup_sr);
            break;

        case USB_PID_ACK:  		/* ack */
            if(!is_in_ep(ep_priv))
            {
                finish_request(ep_priv, urb);
                urb = NULL;
            }
            else
            {
                DMSG_PANIC("wrn: bulk USB_PID_ACK from read data\n");
            }
            break;

        default:
            DMSG_PANIC("ERR: unkown ep_ts_type(%x)\n", ep_priv->ep_ts_type);
        }
    }
    else
    {
        DMSG_PANIC("ERR: Not support periodic transfer(%x)\n", ep_priv->ep_ts_type);
    }

    return complete;
}

#endif


static void start_sched(struct hc_private *hc_priv)
{
    if(hc_priv == NULL)
    {
        DMSG_PANIC("ERR: start_sched, input == NULL\n");
        return ;
    }

    if (!HC_GEN_DEV_IS_RUNNING(to_hcd(hc_priv)->state) || (hc_priv->port & USB_PORT_STAT_SUSPEND))
    {
        DMSG_PANIC("stop schedule,HC is disable or root hub suspend\n");
        return;
    }

#ifdef HCD_INT_ISO_TRANSFER
    start_period_transfer(hc_priv);
#endif

    if(hc_priv->curr_async == NULL)
    {
        start_async_transfer(hc_priv);
    }
    else
    {
        //DMSG_PANIC("ERR: One ep is already scheduled, can not do async schedule\n");
    }
}

static void hcd_handle_misc_irq(u32 int_pendding_misc, struct hc_private *hc_priv)
{
    //--<1>--suspend
    if (int_pendding_misc & USBC_INTUSB_SUSPEND)
    {
        DMSG_INFO("\n[hcd]: ---suspend Irq---\n\n");
        clear_INT_pendding_bits_misc(&env_all, USBC_INTUSB_SUSPEND);
        USBC_INT_ClearMiscPending(usbh_bsp_hdle, USBC_INTUSB_SUSPEND);
    }

    //--<2>--resume
    if (int_pendding_misc & USBC_INTUSB_RESUME)
    {
        DMSG_INFO("\n[hcd]: ---resume Irq---\n\n");
        clear_INT_pendding_bits_misc(&env_all, USBC_INTUSB_RESUME);
        USBC_INT_ClearMiscPending(usbh_bsp_hdle, USBC_INTUSB_RESUME);
    }

    //--<3>--reset. host这个为babble，是错误，
    if (int_pendding_misc & USBC_INTUSB_RESET)
    {
        DMSG_INFO("\n[hcd]: ---babble Irq---\n\n");

        clear_INT_pendding_bits_misc(&env_all, USBC_INTUSB_RESET);
        USBC_INT_ClearMiscPending(usbh_bsp_hdle, USBC_INTUSB_RESET);

        //把babble当作disconnect处理
        USBC_Host_SetFunctionAddress_Deafult(usbh_bsp_hdle, USBC_EP_TYPE_TX, 0);
        {
            u32 i = 1;

            for( i = 1 ; i <= 5; i++)
            {
                USBC_Host_SetFunctionAddress_Deafult(usbh_bsp_hdle, USBC_EP_TYPE_TX, i);
                USBC_Host_SetFunctionAddress_Deafult(usbh_bsp_hdle, USBC_EP_TYPE_RX, i);
            }
        }

        /* 清除关于拔出设备的所有中断, 目前没有hub, 所以可以清除所有中断 */
        USBC_INT_ClearMiscPendingAll(usbh_bsp_hdle);
        USBC_INT_ClearEpPendingAll(usbh_bsp_hdle, USBC_EP_TYPE_TX);
        USBC_INT_ClearEpPendingAll(usbh_bsp_hdle, USBC_EP_TYPE_RX);

        /* dequeue 所有ep urb */
        hcd_nuke_hc_ep_list(hc_priv);

        //把babble当作disconnect处理
        hc_priv->port &= ~(USB_PORT_STAT_CONNECTION |
                           USB_PORT_STAT_ENABLE |
                           USB_PORT_STAT_LOW_SPEED |
                           USB_PORT_STAT_HIGH_SPEED |
                           USB_PORT_STAT_SUSPEND);
        hc_priv->port |=  (USB_PORT_STAT_C_CONNECTION << 16);
        hc_priv->rhstate = RH_NOATTACHED;

        usb_hcd_poll_rh_status(hc_priv->point_gen_dev);

        DMSG_INFO("host disconnect...\n");
        __here__;
        if(usbh_board_info_g.usb_port.port[usbh_port].detect_type == USB_DETECT_TYPE_BALD
                || usbh_board_info_g.usb_port.port[usbh_port].detect_type == USB_DETECT_TYPE_SWITCH_DPDM)
        {
            __here__;
            DMSG_INFO("send DEV_IOC_USR_HWSC_USBH_DISCONNECT msg to monitor...\n");
            usbm_sendcmd(DEV_IOC_USR_HWSC_USBH_DISCONNECT, NULL);
        }

        /* power down */
        USBC_Host_EndSession(usbh_bsp_hdle);
        USBC_ForceVbusValid(usbh_bsp_hdle, USBC_VBUS_TYPE_LOW);
        port_power(hc_priv, 0);

        hcd_nuke_hc_ep_list(hc_priv);

        /* delay */
        {
            u32 cnt = 10000;
            while(cnt--);
        }

        /* power on */
        USBC_A_valid_InputSelect(usbh_bsp_hdle, USBC_A_VALID_SOURCE_UTMI_AVALID);
        USBC_Host_StartSession(usbh_bsp_hdle);
        USBC_ForceVbusValid(usbh_bsp_hdle, USBC_VBUS_TYPE_HIGH);
        port_power(hc_priv, 1);


    }

    //--<4>--用来判断是否reset完毕
#if 0
    if(int_pendding_misc & USBC_INTUSB_SOF)
    {
        __u32 index = 0;
        struct ep_private *ep = NULL;

        /* clear the SOF irq */
        clear_INT_pendding_bits_misc(&env_all, USBC_INTUSB_SOF);
        USBC_INT_ClearMiscPending(usbh_bsp_hdle, USBC_INTUSB_SOF);

        /* start period transfer */
        index = (hc_priv->frame++) % (PERIODIC_SIZE - 1);
        if(hc_priv->next_periodic)
        {
            hc_priv->stat_overrun++;
            DMSG_PANIC("ERR: SOF error, periodic schedule not complete overrun = %d\n", hc_priv->stat_overrun);
        }

        if(hc_priv->periodic[index])
        {
            hc_priv->next_periodic = hc_priv->periodic[index];
            hc_priv->sof_periodic_entry = hc_priv->periodic[index];
            ep = hc_priv->periodic[index];
            do
            {
                ep->periodic_done_counter = 0;
                ep = ep->next;
            }
            while(ep);
        }
        else
        {
            hc_priv->next_periodic = NULL;
        }

        /* NOTIC: ...... */

        /* If there is no periodic urb, then close sof */
        if(hc_priv->urb_counter == 0)
        {
            disable_SOF_irq();
        }

        start_sched(hc_priv);
    }
#else
    if(int_pendding_misc & USBC_INTUSB_SOF)
    {
        //DMSG_HCD_TEST("\n[hcd]: ---SOF Irq---\n\n");

        /* clear the SOF irq */
        clear_INT_pendding_bits_misc(&env_all, USBC_INTUSB_SOF);
        USBC_INT_ClearMiscPending(usbh_bsp_hdle, USBC_INTUSB_SOF);

        disable_SOF_irq();
    }
#endif

    //--<5>--有设备连接上了
    if (int_pendding_misc & USBC_INTUSB_CONNECT)
    {
        DMSG_INFO("\n[hcd]: ---Connect Irq---\n\n");

        clear_INT_pendding_bits_misc(&env_all, USBC_INTUSB_CONNECT);
        USBC_INT_ClearMiscPending(usbh_bsp_hdle, USBC_INTUSB_CONNECT);

        hc_priv->root_hub_plug_event 		= RHUB_EVNET_PLUGIN_ACT;
        hc_priv->last_not_proc_event 		= RHUB_EVNET_NULL;
        hc_priv->plug_event_is_proc_comp 	= 1;	//处理中

        hc_priv->port    |= (USB_PORT_STAT_C_CONNECTION << 16);
        hc_priv->port    |= USB_PORT_STAT_CONNECTION;
        hc_priv->rhstate = RH_ATTACHED;

        //通知
        usb_hcd_poll_rh_status(hc_priv->point_gen_dev);

        __here__;
        if(usbh_board_info_g.usb_port.port[usbh_port].detect_type == USB_DETECT_TYPE_BALD
                || usbh_board_info_g.usb_port.port[usbh_port].detect_type == USB_DETECT_TYPE_SWITCH_DPDM)
        {
            usbm_sendcmd(DEV_IOC_USR_HWSC_USBH_CONNECT, NULL);
        }
    }

    //--<6>--disconnect
    if (int_pendding_misc & USBC_INTUSB_DISCONNECT)
    {
        DMSG_INFO("\n[hcd]: ---Disconnect Irq---\n\n");

        clear_INT_pendding_bits_misc(&env_all, USBC_INTUSB_DISCONNECT);
        USBC_INT_ClearMiscPending(usbh_bsp_hdle, USBC_INTUSB_DISCONNECT);

        USBC_Host_SetFunctionAddress_Deafult(usbh_bsp_hdle, USBC_EP_TYPE_TX, 0);
        {
            u32 i = 1;

            for( i = 1 ; i <= 5; i++)
            {
                USBC_Host_SetFunctionAddress_Deafult(usbh_bsp_hdle, USBC_EP_TYPE_TX, i);
                USBC_Host_SetFunctionAddress_Deafult(usbh_bsp_hdle, USBC_EP_TYPE_RX, i);
            }
        }

        /* 清除关于拔出设备的所有中断, 目前没有hub, 所以可以清除所有中断 */
        USBC_INT_ClearMiscPendingAll(usbh_bsp_hdle);
        USBC_INT_ClearEpPendingAll(usbh_bsp_hdle, USBC_EP_TYPE_TX);
        USBC_INT_ClearEpPendingAll(usbh_bsp_hdle, USBC_EP_TYPE_RX);

        /* dequeue 所有ep urb */
        hcd_nuke_hc_ep_list(hc_priv);

        hc_priv->root_hub_plug_event = RHUB_EVNET_PLUGOUT_ACT;
        hc_priv->last_not_proc_event = RHUB_EVNET_NULL;
        hc_priv->plug_event_is_proc_comp = 1;				//处理中

        hc_priv->port &= ~(USB_PORT_STAT_CONNECTION |
                           USB_PORT_STAT_ENABLE |
                           USB_PORT_STAT_LOW_SPEED |
                           USB_PORT_STAT_HIGH_SPEED |
                           USB_PORT_STAT_SUSPEND);
        hc_priv->port |=  (USB_PORT_STAT_C_CONNECTION << 16);
        hc_priv->rhstate = RH_NOATTACHED;

        usb_hcd_poll_rh_status(hc_priv->point_gen_dev);

        if(usbh_board_info_g.usb_port.port[usbh_port].detect_type == USB_DETECT_TYPE_BALD
                || usbh_board_info_g.usb_port.port[usbh_port].detect_type == USB_DETECT_TYPE_SWITCH_DPDM)
        {
            usbm_sendcmd(DEV_IOC_USR_HWSC_USBH_DISCONNECT, NULL);
        }
    }

    //--<7>--session req
    if (int_pendding_misc & USBC_INTUSB_SESSION_REQ)
    {
        clear_INT_pendding_bits_misc(&env_all, USBC_INTUSB_SESSION_REQ);
        USBC_INT_ClearMiscPending(usbh_bsp_hdle, USBC_INTUSB_SESSION_REQ);

        DMSG_INFO("\n[hcd]: ---Session Req Irq---\n\n");

        /* power down */
        USBC_Host_EndSession(usbh_bsp_hdle);
        USBC_ForceVbusValid(usbh_bsp_hdle, USBC_VBUS_TYPE_LOW);
        port_power(hc_priv, 0);

        hcd_nuke_hc_ep_list(hc_priv);

        /* delay */
        {
            u32 cnt = 10000;
            while(cnt--);
        }

        /* power on */
        USBC_A_valid_InputSelect(usbh_bsp_hdle, USBC_A_VALID_SOURCE_UTMI_AVALID);
        USBC_Host_StartSession(usbh_bsp_hdle);
        USBC_ForceVbusValid(usbh_bsp_hdle, USBC_VBUS_TYPE_HIGH);
        port_power(hc_priv, 1);
    }

    //--<8>--vbus error
    if (int_pendding_misc & USBC_INTUSB_VBUS_ERROR)
    {
        clear_INT_pendding_bits_misc(&env_all, USBC_INTUSB_VBUS_ERROR);
        USBC_INT_ClearMiscPending(usbh_bsp_hdle, USBC_INTUSB_VBUS_ERROR);

        DMSG_INFO("\n[hcd]: ---Vbus Error Irq---\n\n");

        /* power down */
        USBC_Host_EndSession(usbh_bsp_hdle);
        USBC_ForceVbusValid(usbh_bsp_hdle, USBC_VBUS_TYPE_LOW);
        port_power(hc_priv, 0);

        /* delay */
        {
            u32 cnt = 10000;
            while(cnt--);
        }

        /* power on */
        USBC_A_valid_InputSelect(usbh_bsp_hdle, USBC_A_VALID_SOURCE_UTMI_AVALID);
        USBC_Host_StartSession(usbh_bsp_hdle);
        USBC_ForceVbusValid(usbh_bsp_hdle, USBC_VBUS_TYPE_HIGH);
        port_power(hc_priv, 1);
    }

    return;
}

static void hcd_handle_ep0_irq(u32 int_pendding_ep0, struct hc_private *hc_priv)
{
    struct ep_private *ep_priv			= NULL;
    struct urb *urb 					= NULL;
    struct usb_host_virt_dev *virt_dev = NULL;
    u32 old_ep_index	= 0;
    u32 is_error 		= 0;

    //--<1>--check input
    if(hc_priv == NULL)
    {
        DMSG_PANIC("ERR: hcd_handle_ep0_irq, input error\n");
        return;
    }

    //--<2>--clear irq
    if(int_pendding_ep0 & USBC_INTTx_FLAG_EP0)
    {
        clear_INT_pendding_bits_tx(&env_all, USBC_INTTx_FLAG_EP0);
        USBC_INT_ClearEpPending(usbh_bsp_hdle, USBC_EP_TYPE_TX, 0);
    }
    else
    {
        DMSG_PANIC("ERR: in ep0 handle, buf not found irq pendding\n");
        return ;
    }

    //--<3>--initialze parameter
    ep_priv	= hc_priv->ep_list[0];
    if(ep_priv == NULL)
    {
        DMSG_PANIC("ERR: ep_priv == NULL\n");
        return ;
    }

    if(ep_priv->hep == NULL)
    {
        DMSG_PANIC("ERR: ep_priv->hep == NULL\n");
        return ;
    }

    urb = ep_priv->current_urb;
    if(urb == NULL)
    {
        DMSG_PANIC("ERR: hcd_handle_ep0_irq, urb = NULL\n");
        return;
    }

    old_ep_index = get_active_ep();
    select_active_ep(0);

    //--<4>--ep0 error
    if(USBC_Host_IsEpError(usbh_bsp_hdle, USBC_EP_TYPE_EP0))
    {
        DMSG_PANIC("ERR: ep0 is error\n");
        USBC_Host_FlushFifo(usbh_bsp_hdle, USBC_EP_TYPE_EP0);
        USBC_Host_ClearEpError(usbh_bsp_hdle, USBC_EP_TYPE_EP0);
        is_error = 1;
    }

    //--<5>--ep0 stall
    if(USBC_Host_IsEpStall(usbh_bsp_hdle, USBC_EP_TYPE_EP0))
    {
        DMSG_PANIC("ERR: ep0 is stall\n");
        USBC_Host_FlushFifo(usbh_bsp_hdle, USBC_EP_TYPE_EP0);
        USBC_Host_ClearEpStall(usbh_bsp_hdle, USBC_EP_TYPE_EP0);
        is_error = 1;
    }

    //--<6>--ep0 nak timeout
    if(USBC_Host_IsEpNakTimeOut(usbh_bsp_hdle, USBC_EP_TYPE_EP0))
    {
        DMSG_PANIC("ERR: ep0 is nak timeout\n");
        USBC_Host_FlushFifo(usbh_bsp_hdle, USBC_EP_TYPE_EP0);
        USBC_Host_ClearEpNakTimeOut(usbh_bsp_hdle, USBC_EP_TYPE_EP0);
        is_error = 1;
    }

    if(is_error)
    {
        DMSG_PANIC("ERR: ep0 has something wrong\n");

        is_error = 0;

        if(urb)
        {
            urb->status = -EPIPE;
        }

        finish_request(ep_priv, urb);
        USBC_Host_ClearEpCSR(usbh_bsp_hdle, USBC_EP_TYPE_EP0);

        select_active_ep(old_ep_index);

        return;
    }

    //--<7>--do urb
    virt_dev = urb->dev;
    switch(ep_priv->nextpid)
    {
    case USB_PID_SETUP:
        if(urb->transfer_buffer_length == urb->actual_length)
        {
            ep_priv->nextpid = USB_PID_ACK;

            //发送无data阶段的status
            finish_ep0_status_after_OUT_or_SETUP();
            select_active_ep(old_ep_index);

            return ;  //no need to schedule the async list
        }
        else if(usb_pipeout(urb->pipe))
        {
            usb_settoggle(virt_dev, 0, 1, 1);
            ep_priv->nextpid = USB_PID_OUT;
        }
        else
        {
            usb_settoggle(virt_dev, 0, 0, 1);
            ep_priv->nextpid = USB_PID_IN;
        }
        break;

    case USB_PID_OUT:
        /* 中断来了, 表示数据已经写完 */
        usb_dotoggle(virt_dev, ep_priv->ep_num, 1);
        if((urb->actual_length == urb->transfer_buffer_length)
                || (ep_priv->length < ep_priv->maxpacket))
        {
            ep_priv->nextpid = USB_PID_ACK;
            finish_ep0_status_after_OUT_or_SETUP();

            select_active_ep(old_ep_index);
            return;  //no need to schedule the async list
        }
        break;

    case USB_PID_IN:
    {
        u32 complete = 0;

        complete = read_data(ep_priv, urb);
        if(complete)
        {
            finish_ep0_status_after_IN();

            select_active_ep(old_ep_index);
            return;  //no need to schedule the async list
        }
    }
    break;

    case USB_PID_ACK:
        finish_request(ep_priv, urb);
        urb = NULL;
        USBC_Host_ClearEpCSR(usbh_bsp_hdle, USBC_EP_TYPE_EP0);

        //一次ctrl传输完成
        select_active_ep(old_ep_index);

        return ;    //no need to schedule the async list
    //break;

    default:
        DMSG_PANIC("ERR: unkown nextpid(%x)\n", ep_priv->nextpid);
        break;
    }

    //--<8>--还原旧的ep
    select_active_ep(old_ep_index);

    //--<9>--发起ctrl传输的下一个阶段
    start_transfer_aysnc_urb(ep_priv, urb);

    return ;
}

static void handle_epx(struct hc_private *hc_priv, int ep_list_index)
{
    struct ep_private *ep_priv = NULL;
    u32 is_error		= 0;
    u32 old_ep_index	= 0;

    //	DMSG_INFO("ERR: handle_epx, ep_list_index = %d\n", ep_list_index);

    //--<1>--check input
    if(hc_priv == NULL)
    {
        DMSG_PANIC("ERR: handle_epx input error\n");
        return;
    }

    //--<2>--initialize parameter
    ep_priv = hc_priv->ep_list[ep_list_index];
    if(ep_priv == NULL)
    {
        DMSG_PANIC("ERR: ep_priv == NULL\n");
        return;
    }

    old_ep_index = get_active_ep();
    select_active_ep(get_phy_ep_index(ep_priv));

    //--<3>--clear ep error status & flush fifo
    if(is_in_ep(ep_priv))  /* rx */
    {
        //ep stall
        if(USBC_Host_IsEpStall(usbh_bsp_hdle, USBC_EP_TYPE_RX))
        {
            DMSG_PANIC("ERR: handle_epx, ep stall\n");
            USBC_Host_FlushFifo(usbh_bsp_hdle, USBC_EP_TYPE_RX);
            USBC_Host_ClearEpStall(usbh_bsp_hdle, USBC_EP_TYPE_RX);
            is_error = 1;
        }

        //ep nak timeout
        if(USBC_Host_IsEpNakTimeOut(usbh_bsp_hdle, USBC_EP_TYPE_RX))
        {
            DMSG_PANIC("ERR: handle_epx, ep nak timeout\n");
            USBC_Host_FlushFifo(usbh_bsp_hdle, USBC_EP_TYPE_RX);
            USBC_Host_ClearEpNakTimeOut(usbh_bsp_hdle, USBC_EP_TYPE_RX);
            is_error = 1;
        }

        //ep error
        if(USBC_Host_IsEpError(usbh_bsp_hdle, USBC_EP_TYPE_RX))
        {
            DMSG_PANIC("ERR: handle_epx, ep error\n");
            USBC_Host_FlushFifo(usbh_bsp_hdle, USBC_EP_TYPE_RX);
            USBC_Host_ClearEpError(usbh_bsp_hdle, USBC_EP_TYPE_RX);
            is_error = 1;
        }
    }
    else   /* tx */
    {
        //ep stall
        if(USBC_Host_IsEpStall(usbh_bsp_hdle, USBC_EP_TYPE_TX))
        {
            DMSG_PANIC("ERR: handle_epx, ep stall\n");
            USBC_Host_FlushFifo(usbh_bsp_hdle, USBC_EP_TYPE_TX);
            USBC_Host_ClearEpStall(usbh_bsp_hdle, USBC_EP_TYPE_TX);
            is_error = 1;
        }

        //ep nak timeout
        if(USBC_Host_IsEpNakTimeOut(usbh_bsp_hdle, USBC_EP_TYPE_TX))
        {
            DMSG_PANIC("ERR: handle_epx, ep nak timeout\n");
            USBC_Host_FlushFifo(usbh_bsp_hdle, USBC_EP_TYPE_TX);
            USBC_Host_ClearEpNakTimeOut(usbh_bsp_hdle, USBC_EP_TYPE_TX);
            is_error = 1;
        }

        //ep error
        if(USBC_Host_IsEpError(usbh_bsp_hdle, USBC_EP_TYPE_TX))
        {
            DMSG_PANIC("ERR: handle_epx, ep error\n");
            USBC_Host_FlushFifo(usbh_bsp_hdle, USBC_EP_TYPE_TX);
            USBC_Host_ClearEpError(usbh_bsp_hdle, USBC_EP_TYPE_TX);
            is_error = 1;
        }
    }

    //如果ep发生了错误, 并且置urb status非法, 并且finish_request
    if(is_error)
    {
        struct urb *urb = NULL;

        DMSG_PANIC("ERR: handle_epx, epx has something wrong\n");

        is_error = 0;

        urb = ep_priv->current_urb;
        if(urb)
        {
            urb->status = -EPIPE;
        }

        if(ep_priv->dma_working)
        {
            DMSG_PANIC("ERR: ep error during dma phase\n");
            hcd_dma_stop();
            clean_ep_dma_status(ep_priv);
        }

        if(is_in_ep(ep_priv))  /* ep in, rx */
        {
            USBC_Host_ClearEpCSR(usbh_bsp_hdle, USBC_EP_TYPE_RX);
        }
        else   /* ep out, tx */
        {
            USBC_Host_ClearEpCSR(usbh_bsp_hdle, USBC_EP_TYPE_TX);
        }

        finish_request(ep_priv, urb);

        select_active_ep(old_ep_index);

        return;
    }

    /* 在 DMA 期间接收到了短包，那么就停止 DMA 传输 */
    if(is_in_ep(ep_priv) && ep_priv->dma_working)
    {
        struct urb *urb = NULL;

        DMSG_PANIC("ERR: handle_epx : receive a short package during dma phase, USB transport stopped\n");

        hcd_dma_stop();
        clean_ep_dma_status(ep_priv);
        USBC_Host_ReadDataStatus(usbh_bsp_hdle, USBC_EP_TYPE_RX, 1);

        urb = ep_priv->current_urb;
        if(urb)
        {
            urb->actual_length = urb->transfer_buffer_length - USB_OS_DMA_QueryRestCount(hDMA_Hdle);

            DMSG_PANIC("ERR: handle_epx : dma have transprot %d btyes\n", urb->actual_length);

            finish_request(ep_priv, ep_priv->current_urb);
        }

        select_active_ep(old_ep_index);

        return;
    }

    //--<4>--do urb.因为dma期间会产生ep stall, 所以这里要把ep rx/tx irq打开
    if(ep_priv->ep_ts_type == PIPE_BULK)
    {
        if(ep_priv->dma_working == 0)
        {
            start_transfer_aysnc_ep(ep_priv);
        }
        else
        {
            DMSG_HCD_TEST("wrn: ep irq happen during dma working\n");
        }
    }
    else    //int or iso
    {
        //start_period_transfer(hc_priv);
        start_period_transfer_ep(ep_priv);

        /* 发起异步传输 */
        start_async_transfer(hc_priv);
    }

    select_active_ep(old_ep_index);

    return;
}

static void hcd_handle_epx_irq(u32 int_pendding_tx, u32 int_pendding_rx, struct hc_private *hc_priv)
{
    u32 i = 0;

    //rx,既IN 方向的ep中断
    if(int_pendding_rx != 0) 	//确信该方向上确实有INT发生
    {
        for(i = 1 ; i <= USBC_MAX_EP_NUM ; i++)
        {
            u32 tmp = 1 << i;

            if((int_pendding_rx & tmp) == 0)
            {
                continue;
            }

            clear_INT_pendding_bits_rx(&env_all , tmp);
            USBC_INT_ClearEpPending(usbh_bsp_hdle, USBC_EP_TYPE_RX, i);

            handle_epx(hc_priv, i);
        }
    }

    if(int_pendding_tx != 0)
    {
        for(i = 1; i <= USBC_MAX_EP_NUM ; i++)
        {
            u32  tmp = 1 << i;

            if((int_pendding_tx & tmp) == 0)
            {
                continue;
            }

            //该ep上中断发送了
            clear_INT_pendding_bits_tx(&env_all , tmp);
            USBC_INT_ClearEpPending(usbh_bsp_hdle, USBC_EP_TYPE_TX, i);

            handle_epx(hc_priv, (HCD_EP_LIST_OUT_BEGIN - 1 + i));
        }
    }

    return ;
}

static void hub_descriptor (struct usb_hub_descriptor *desc)
{
    USB_OS_MEMSET((void *)desc, 0, sizeof(struct usb_hub_descriptor));
    desc->bDescriptorType 		= 0x29;
    desc->bDescLength 			= 9;
    desc->wHubCharacteristics 	= 0x0001;
    desc->bNbrPorts 			= 1;

    DMSG_PANIC("PANIC : wa_hub_descriptor() not init all member\n");
}

/*
********************************************************************************
*                     hcd_ops_irq
* Description:
*     中断处理函数
* Arguments:
*     void
* Return value:
*     void
* note:
*     void
*********************************************************************************
*/
__s32 __hcd_ops_irq(void *arg)
{
    struct hc_private *hc_priv = host_controller;
    u32 int_pendding_misc 		= 0;
    u32 int_pendding_tx04 		= 0;
    u32 int_pendding_rx14 		= 0;
    __cpu_sr cup_sr 			= 0;

    USB_OS_ENTER_CRITICAL(cup_sr);
    int_pendding_misc = read_INT_pendding_misc(&env_all);
    int_pendding_tx04 = read_INT_pendding_tx(&env_all);
    int_pendding_rx14 = read_INT_pendding_rx(&env_all);
    USB_OS_EXIT_CRITICAL(cup_sr);

    DMSG_HCD_TEST("irq1(%x, %x, %x)\n", int_pendding_misc , int_pendding_tx04 , int_pendding_rx14);

    //--1--misc handle
    if(int_pendding_misc )
    {
        hcd_handle_misc_irq(int_pendding_misc, hc_priv);
    }

    //--2--ep0 handle
    if(int_pendding_tx04 & USBC_INTTx_FLAG_EP0)
    {
        hcd_handle_ep0_irq(int_pendding_tx04, hc_priv);
    }

    //--3--ep other handle
    if((int_pendding_tx04 & (~USBC_INTTx_FLAG_EP0)) || int_pendding_rx14)
    {
        hcd_handle_epx_irq(int_pendding_tx04, int_pendding_rx14, hc_priv);
    }

    DMSG_HCD_TEST("irq2\n");


    return EPDK_TRUE;
}

/*
********************************************************************************
*                     hcd_ops_start
* Description:
*     usb host start to work
* Arguments:
*     hcd  : input.
* Return value:
*     void
* note:
*     void
*********************************************************************************
*/
static s32 __hcd_ops_start(struct hc_gen_dev *hcd)
{
    __cpu_sr cpu_sr = 0;
    struct hc_private *hcd_priv = hcd_to(hcd);

    USB_OS_ENTER_CRITICAL(cpu_sr);
    hcd->state 				= HC_GEN_DEV_STATE_RUNNING;
    hcd->uses_new_polling 	= 1;
    USB_OS_EXIT_CRITICAL(cpu_sr);

    //往vbus供电
    port_power(hcd_priv, 1);

    return 0;
}

/*
********************************************************************************
*                     hcd_ops_stop
* Description:
*     usb host stop working
* Arguments:
*     hcd  : input.
* Return value:
*     void
* note:
*     void
*********************************************************************************
*/
static void __hcd_ops_stop(struct hc_gen_dev *hcd)
{
    __cpu_sr cpu_sr = 0;
    struct hc_private *hcd_priv = hcd_to(hcd);

    if(hcd_priv == NULL)
    {
        DMSG_PANIC("PANIC : hcd_ops_stop() : hcd_priv == NULL\n");
        return;
    }

    USB_OS_ENTER_CRITICAL(cpu_sr);
    hcd_priv->port = 0;
    hcd_priv->rhstate = RH_POWEROFF;
    USB_OS_EXIT_CRITICAL(cpu_sr);

    //vbus停电
    port_power(hcd_priv, 0);

    return ;
}

int usb_calc_bus_time (int speed, int is_input, int isoc, int bytecount)
{
    unsigned long	tmp = 0;

    switch (speed)
    {
    case USB_SPEED_LOW: 	/* INTR only */
        if (is_input)
        {
            tmp = (67667L * (31L + 10L * BitTime (bytecount))) / 1000L;
            return (64060L + (2 * BW_HUB_LS_SETUP) + BW_HOST_DELAY + tmp);
        }
        else
        {
            tmp = (66700L * (31L + 10L * BitTime (bytecount))) / 1000L;
            return (64107L + (2 * BW_HUB_LS_SETUP) + BW_HOST_DELAY + tmp);
        }
    //break;

    case USB_SPEED_FULL:	/* ISOC or INTR */
        if (isoc)
        {
            tmp = (8354L * (31L + 10L * BitTime (bytecount))) / 1000L;
            return (((is_input) ? 7268L : 6265L) + BW_HOST_DELAY + tmp);
        }
        else
        {
            tmp = (8354L * (31L + 10L * BitTime (bytecount))) / 1000L;
            return (9107L + BW_HOST_DELAY + tmp);
        }
    //break;

    case USB_SPEED_HIGH:	/* ISOC or INTR */
        // FIXME adjust for input vs output
        if (isoc)
            tmp = HS_NSECS_ISO (bytecount);
        else
            tmp = HS_NSECS (bytecount);
        return tmp;
    //break;

    default:
        DMSG_PANIC("ERR: bogus device speed!\n");
        return 0;
    }
}

/*
********************************************************************************
*                     hcd_ops_urb_enqueue
* Description:
*     add urbs to hcd queue. 虽然这里处理urb是以ep为单位，但是没有调度机制。
*     1、初始化ep
*     2、add ep to ep_list
*     3、为传输做准备
*     4、传输
*
* Arguments:
*     hcd  		: input.
*     hep  		: input.
*     urb  		: input.
*     mem_flags : input.
* Return value:
*     void
* note:
*     发送之前urb已经存在于usb_host_virt_endpoint
*
*********************************************************************************
*/

static s32 __hcd_ops_urb_queue(struct hc_gen_dev *hcd,
                               struct usb_host_virt_endpoint *hep,
                               struct urb *urb,
                               u32 mem_flags)
{
    struct hc_private *hc_priv 		= NULL;
    struct usb_host_virt_dev *udev 	= NULL;
    struct ep_private *ep_priv		= NULL;
    __cpu_sr cup_sr 				= 0;

    __u32	pipe   = 0;
    __u32	is_out = 0;
    __u32	type   = 0;
    __u32	ep_num = 0;
    __s32	retval = 0;

    //--<1>--check input
    if(hcd == NULL || hep == NULL || urb == NULL)
    {
        DMSG_PANIC("ERR: __hcd_ops_urb_queue, input error\n");
        return -EINVAL;
    }

    /* check urb */
    if(urb->status != -EINPROGRESS)
    {
        DMSG_PANIC("ERR: __hcd_ops_urb_queue, urb status(%d) is error\n", urb->status);
        return -EINVAL;
    }

    if(urb->transfer_buffer == NULL && urb->transfer_buffer_length != 0)
    {
        DMSG_PANIC("wrn: __hcd_ops_urb_queue, urb buffer is null\n");
    }

    if(urb->need_dispose)
    {
        DMSG_PANIC("wrn: __hcd_ops_urb_queue, urb is already enqueue\n");
    }

    //--<2>--init parameter
    hc_priv = hcd_to(hcd);
    udev 	= urb->dev;
    if(udev == NULL)
    {
        DMSG_PANIC("ERR: udev == NULL\n");
        return -EINVAL;
    }

    pipe   = urb->pipe;
    is_out = !usb_pipein(pipe);
    type   = usb_pipetype(pipe);
    ep_num = usb_pipeendpoint(pipe);

    //--<3>--check validity

    /* If hcd is stop, can't queue.  */
    if(!(hc_priv->port & (1 << USB_PORT_FEAT_ENABLE)) || !HC_GEN_DEV_IS_RUNNING(hcd->state))
    {
        DMSG_PANIC("ERR: the port is not work, the urb can't queue\n");
        urb->status = -ESHUTDOWN;
        return -ENODEV;
    }

    //--<4>--如果该virt ep是新来的，分配ep
    if(!hep->hcpriv)
    {
        ep_priv = USB_OS_MALLOC(sizeof(struct ep_private), USB_MEM_FILE_TRIGGER , USB_MEM_LINE_TRIGGER );
        if(ep_priv == NULL)
        {
            DMSG_PANIC("ERR: __hcd_ops_urb_queue, USB_OS_MALLOC failed\n");
            return -ENOMEM;
        }

        USB_OS_MEMSET(ep_priv , 0, sizeof(struct ep_private));
    }
    else
    {
        ep_priv = hep->hcpriv;
    }

    USB_OS_ENTER_CRITICAL(cup_sr);
    urb->need_dispose 	= 1;
    urb->hcpriv 		= hep;

    //--<5>--初始化ep
    if (hep->hcpriv)
    {
        /* 修正ep0包大小 */
        if(type == PIPE_CONTROL)
        {
            if((!hc_priv->ep_list[HCD_EP_LIST_EP0_INDEX]))
            {
                ep_priv->mask_dir_local_ep_nr = 0;
                hc_priv->ep_list[HCD_EP_LIST_EP0_INDEX] = ep_priv;	/*hook ep0 to ep_list*/
            }
            else
            {
                DMSG_PANIC("ERR : __hcd_ops_urb_queue, ep0 is not initialize\n");
                retval = -ENOSPC;
                goto enqueue_fail;
            }

            if(ep_priv->maxpacket != ep_priv->udev->ep0.desc.wMaxPacketSize)
            {
                ep_priv->maxpacket =  ep_priv->udev->ep0.desc.wMaxPacketSize;

                DMSG_PANIC("WRN: __hcd_ops_urb_queue, ep maxpacket must correct.\n");
                DMSG_PANIC("WRN: __hcd_ops_urb_queue, ep->maxpacket = %d, wMaxPacketSize = %d\n",
                           ep_priv->maxpacket, ep_priv->udev->ep0.desc.wMaxPacketSize);
            }

            /* config ep0 */
            {
                __u32 speed = 0;

                switch(udev->speed)
                {
                case USB_SPEED_HIGH:
                    speed = USBC_TS_MODE_HS;
                    break;

                case USB_SPEED_FULL:
                    speed = USBC_TS_MODE_FS;
                    break;

                case USB_SPEED_LOW:
                    speed = USBC_TS_MODE_LS;
                    break;

                default:
                    speed = USBC_TS_MODE_HS;
                }

                USBC_Host_ConfigEp(usbh_bsp_hdle,
                                   USBC_EP_TYPE_EP0,
                                   0,
                                   speed,
                                   USBC_TS_TYPE_CTRL,
                                   0,
                                   ep_priv->maxpacket,
                                   0);

                /* enable ep0 tx irq */
                USBC_INT_EnableEp(usbh_bsp_hdle, USBC_EP_TYPE_TX, 0);
            }

            /* 如果device的地址已经分配, 并且不是USB_REQ_SET_ADDRESS请求,
               那么就可以设置ep0的地址了 */
            {
                struct usb_ctrlrequest  *ctrlreq = NULL;

                ctrlreq = (struct usb_ctrlrequest *)urb->setup_packet;
                if(ctrlreq->bRequest != USB_REQ_SET_ADDRESS /* && ep_priv->ep_fun_addr */)
                {
                    USBC_Host_SetFunctionAddress(usbh_bsp_hdle,
                                                 USBC_EP_TYPE_TX,
                                                 0,
                                                 ep_priv->ep_fun_addr,
                                                 ep_priv->ep_target_muiltTT,
                                                 ep_priv->ep_hub_addr,
                                                 ep_priv->ep_hub_port);
                }
                else
                {
                    /* 如果是SET_ADDRESS请求, 那么就设置成0地址 */
                    USBC_Host_SetFunctionAddress(usbh_bsp_hdle,
                                                 USBC_EP_TYPE_TX,
                                                 0,
                                                 0,
                                                 ep_priv->ep_target_muiltTT,
                                                 ep_priv->ep_hub_addr,
                                                 ep_priv->ep_hub_port);
                }
            }
        }
    }
    else
    {
        /* 为刚构造的ep初始化 */
        ep_priv->udev      		= udev;
        ep_priv->ep_num    		= ep_num;
        ep_priv->maxpacket 		= usb_maxpacket(udev, urb->pipe, is_out);
        ep_priv->ep_ts_type		= type;
        ep_priv->stopped   		= 0;

#ifdef HCD_INT_ISO_TRANSFER
        ep_priv->periodic_done_counter = 0;
#endif

        ep_priv->ep_fun_addr    	= udev->devnum;
        if(udev->parent && udev->parent->devnum != 1)   /* 如果该设备是接到hub上, 就需要配置hub */
        {
            ep_priv->ep_hub_port    	= udev->hub_port;
            ep_priv->ep_hub_addr = udev->parent->devnum;

            /* 多TT */
            if(udev->parent->descriptor.bDeviceProtocol == 2)
            {
                ep_priv->ep_target_muiltTT	= 1;
            }
            else
            {
                ep_priv->ep_target_muiltTT	= 0;
            }
        }
        else    //没有hub
        {
            ep_priv->ep_hub_port        = 0;
            ep_priv->ep_hub_addr 		= 0;
            ep_priv->ep_target_muiltTT	= 0;
        }

        DMSG_HCD_TEST("urb_queue: ep_fun_addr = %d, ep_target_muiltTT = %d, ep_hub_port = %d, ep_hub_addr = %d\n",
                      ep_priv->ep_fun_addr,
                      ep_priv->ep_target_muiltTT,
                      ep_priv->ep_hub_port,
                      ep_priv->ep_hub_addr);

        /* 配置硬件ep */
        switch (type)
        {
        case PIPE_CONTROL:
        {
            if((!hc_priv->ep_list[HCD_EP_LIST_EP0_INDEX]))
            {
                ep_priv->mask_dir_local_ep_nr = 0;
                hc_priv->ep_list[HCD_EP_LIST_EP0_INDEX] = ep_priv;	/*hook ep0 to ep_list*/

            }
            else
            {
                DMSG_PANIC("ERR : __hcd_ops_urb_queue, ep0 is not initialize\n");
                retval = -ENOSPC;
                goto enqueue_fail;
            }

            /* config ep0 */
            {
                __u32 speed = 0;

                switch(udev->speed)
                {
                case USB_SPEED_HIGH:
                    speed = USBC_TS_MODE_HS;
                    break;

                case USB_SPEED_FULL:
                    speed = USBC_TS_MODE_FS;
                    break;

                case USB_SPEED_LOW:
                    speed = USBC_TS_MODE_LS;
                    break;

                default:
                    speed = USBC_TS_MODE_HS;
                }

                USBC_Host_ConfigEp(usbh_bsp_hdle,
                                   USBC_EP_TYPE_EP0,
                                   0,
                                   speed,
                                   USBC_TS_TYPE_CTRL,
                                   0,
                                   ep_priv->maxpacket,
                                   0);

                /* enable ep0 tx irq */
                USBC_INT_EnableEp(usbh_bsp_hdle, USBC_EP_TYPE_TX, 0);
            }

            /* 如果device的地址已经分配, 并且不是USB_REQ_SET_ADDRESS请求,
               那么就可以设置ep0的地址了 */
            {
                struct usb_ctrlrequest  *ctrlreq = NULL;

                ctrlreq = (struct usb_ctrlrequest *)urb->setup_packet;
                if(ctrlreq->bRequest != USB_REQ_SET_ADDRESS && ep_priv->ep_fun_addr)
                {
                    USBC_Host_SetFunctionAddress(usbh_bsp_hdle,
                                                 USBC_EP_TYPE_TX,
                                                 0,
                                                 ep_priv->ep_fun_addr,
                                                 ep_priv->ep_target_muiltTT,
                                                 ep_priv->ep_hub_addr,
                                                 ep_priv->ep_hub_port);
                }
                else
                {
                    /* 如果是SET_ADDRESS请求, 那么就设置成0地址 */
                    USBC_Host_SetFunctionAddress(usbh_bsp_hdle,
                                                 USBC_EP_TYPE_TX,
                                                 0,
                                                 0,
                                                 ep_priv->ep_target_muiltTT,
                                                 ep_priv->ep_hub_addr,
                                                 ep_priv->ep_hub_port);
                }
            }
        }
        break;

        case PIPE_BULK:
        {
            retval = hcd_ep_config(hc_priv, ep_priv, PIPE_BULK, HCD_FIFO_NUM, is_out, 0);/*quad-buffered*/
            if(retval != 0)
            {
                DMSG_PANIC("ERR: hc_ep_config failed\n");
                goto enqueue_fail;
            }
        }
        break;

        case PIPE_INTERRUPT:
        {
            /* 获得中断传输允许的时间间隔 */
            if(urb->interval > PERIODIC_SIZE)
            {
                urb->interval = PERIODIC_SIZE;
            }

            ep_priv->ep_interval = hep->desc.bInterval;
            ep_priv->period      = urb->interval;

            /* config ep */
            retval = hcd_ep_config(hc_priv,
                                   ep_priv,
                                   PIPE_INTERRUPT,
                                   HCD_FIFO_NUM,
                                   is_out,
                                   ep_priv->ep_interval);
            if(retval != 0)
            {
                DMSG_PANIC("ERR: hc_ep_config failed\n");
                goto enqueue_fail;
            }

#ifdef  HCD_INT_ISO_TRANSFER
            ep_priv->branch = PERIODIC_SIZE;
            ep_priv->load = usb_calc_bus_time(udev->speed,
                                              !is_out,
                                              (type == PIPE_ISOCHRONOUS),
                                              usb_maxpacket(udev , pipe, is_out)) / 1000;
#endif
        }
        break;

        default:
            DMSG_PANIC("ERR: unkown type(%d)\n", type);
            goto enqueue_fail;
        }

        /* 关联 hcd ep 和 core ep */
        ep_priv->hep 	= hep;
        ep_priv->hc		= hc_priv;
        hep->hcpriv		= ep_priv;
    }

    DMSG_HCD_TEST("q: ep(%x, %d, %d), urb(%x, %d, %d, %d)\n",
                  ep_priv, ep_priv->ep_index, ep_priv->stopped,
                  urb, urb->transfer_buffer_length, urb->actual_length, urb->status);
    //	DMSG_INFO("q:%d\n", urb->transfer_buffer_length);

    /* 根据这个包的性质(ctrl, bulk,...), 确定本次传输的性质(set_up, in, out,...) */
    usb_settoggle(udev, ep_num, is_out, 0);
    if (type == PIPE_CONTROL)
    {
        ep_priv->nextpid = USB_PID_SETUP;
    }
    else if (is_out)
    {
        ep_priv->nextpid = USB_PID_OUT;
    }
    else
    {
        ep_priv->nextpid = USB_PID_IN;
    }

    //--<7>--为传输做准备
    switch(type)
    {
    case PIPE_CONTROL:
    case PIPE_BULK:
        //add ep to ep_list
        list_head_malloc_and_add(ep_priv, &(hc_priv->async));
        break;

    case PIPE_INTERRUPT:
#ifdef  HCD_INT_ISO_TRANSFER
    {
        __u32 i = 0;

        urb->interval = ep_priv->period;
        hc_priv->urb_counter ++;
        if(ep_priv->branch < PERIODIC_SIZE)
        {
            urb->start_frame = (hc_priv->frame & (PERIODIC_SIZE - 1)) + ep_priv->branch;

            if(hc_priv->urb_counter > 0)
            {
                enable_SOF_irq();
            }

            break;
        }

        //if the ep is new
        retval = balance(hc_priv, ep_priv->period, ep_priv->load);
        if(retval < 0)
        {
            DMSG_PANIC("ERR: balance FAIL!\n");
            goto enqueue_fail;
        }
        ep_priv->branch = retval;
        retval = 0;
        urb->start_frame = (hc_priv->frame & (PERIODIC_SIZE - 1)) + ep_priv->branch;

        //create schedule table
        for(i = ep_priv->branch; i < PERIODIC_SIZE ; i += ep_priv->period)
        {
            struct ep_private **prev = &hc_priv->periodic[i];
            struct ep_private *here = *prev;

            while(here && ep_priv != here)
            {
                if(ep_priv->period > here->period)
                    break;
                prev = & here->next;
                here = *prev;
            }

            if(ep_priv != here)
            {
                ep_priv->next = here;
                *prev = ep_priv ;
            }

            hc_priv->load[i] += ep_priv->load;
        }

        hcd->self.bandwidth_allocated += ep_priv->load / ep_priv->period;

        if(hc_priv->urb_counter > 0)
        {
            enable_SOF_irq();
        }
    }
#else
    {
        __u32 i = 0;
        __u8  old_ep_index = 0;	/* ep索引	*/

        //create schedule table
        for(i = 0; i < PERIODIC_SIZE ; i++)
        {
            if(hc_priv->periodic[i] == NULL)
            {
                hc_priv->periodic[i] = ep_priv;
                break;
            }
        }

        if(i >= PERIODIC_SIZE)
        {
            DMSG_PANIC("ERR: periodic array is full, can not queue\n");
            break;
        }

        old_ep_index = get_active_ep();
        select_active_ep(ep_priv->ep_index);

        if(!is_out)
        {
            if(!USBC_Host_IsReadDataReady(usbh_bsp_hdle, USBC_EP_TYPE_RX)
                    && !USBC_Host_IsReqPktSet(usbh_bsp_hdle, USBC_EP_TYPE_RX))
            {
                USBC_Host_StartInToken(usbh_bsp_hdle, USBC_EP_TYPE_RX);
            }
        }

        select_active_ep(old_ep_index);
    }
#endif
    break;

    case PIPE_ISOCHRONOUS:
        DMSG_PANIC("wrn: isochronous transfer is not support\n");
        break;

    default:
        DMSG_PANIC("err: __hcd_ops_urb_queue, unkown type(%x)\n", type);
    }

    //--<8>--ep空闲就开始传输, 否则在irq里完成
    if(!ep_priv->stopped)
    {
        start_sched(hc_priv);
    }
    else
    {
        DMSG_PANIC("err: __hcd_ops_urb_queue, ep_priv(%x) is stopped\n", ep_priv);
    }

    USB_OS_EXIT_CRITICAL(cup_sr);

    return 0;

enqueue_fail:
    urb->need_dispose 	= 0;
    urb->hcpriv 		= NULL;
    USB_OS_EXIT_CRITICAL(cup_sr);

    return retval;
}

/*
********************************************************************************
*                     hcd_ops_urb_dequeue
* Description:
*     kick urbs from hcd queue
* Arguments:
*     hcd  		: input.
*     hep  		: input.
*     urb  		: input.
*     mem_flags : input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
static s32 __hcd_ops_urb_dequeue(struct hc_gen_dev *hcd, struct urb *urb)
{
    struct hc_private *hc_priv 		= NULL;
    struct usb_host_virt_endpoint *hep = NULL;
    struct ep_private *ep_priv			= NULL;
    __cpu_sr cup_sr 					= 0;
    __u8  old_ep_index 					= 0;	/* ep索引	*/

    //--<1>--chenck input
    if((hcd == NULL) || (urb == NULL))
    {
        DMSG_PANIC("err: __hcd_ops_urb_dequeue, input error\n");
        return -EINVAL;
    }

    //--<2>--initialize parameter
    hc_priv = hcd_to(hcd);
    if(hc_priv == NULL)
    {
        DMSG_PANIC("err: __hcd_ops_urb_dequeue, hc_priv = NULL\n");
        return -EINVAL;
    }

    hep = urb->hcpriv;
    if (hep == NULL)
    {
        DMSG_PANIC("err: __hcd_ops_urb_dequeue, hep = NULL\n");
        return -EINVAL;
    }

    ep_priv = hep->hcpriv;
    if(ep_priv == NULL)
    {
        DMSG_PANIC("err: __hcd_ops_urb_dequeue, ep = NULL\n");
        return -EINVAL;
    }

    USB_OS_ENTER_CRITICAL(cup_sr);

    DMSG_PANIC("dq: ep_priv(%x, %d, %d, stopped), urb(%x, %d, %d)\n",
               ep_priv, ep_priv->ep_index, ep_priv->ep_num, ep_priv->stopped,
               urb, urb->transfer_buffer_length, urb->status);

    //--<3>--如果URB正在处理, 就停止处理.
    //1、urb正在被DMA, 停止dma, 消除ep状态, finish_request
    //2、urb正在被pio, 等待它处理完毕
    //3、urb在队列中, 直接finish_request
    urb->need_dispose 	= 0;

    /* 如果urb正在被处理, 这里只是把urb设置成无效 */
    if(hcd_is_urb_busy(urb))
    {
        //nothing to do
        DMSG_PANIC("wrn: urb is busy, but happen urb dequeue\n");
    }
    else
    {
        /* urb正在被DMA, 停止dma, 消除ep状态, finish_request */
        if(ep_priv->dma_working)
        {
            DMSG_PANIC("wrn: urb is done by dma, but happen urb dequeue\n");
            hcd_dma_stop();
            clean_ep_dma_status(ep_priv);
        }

        old_ep_index = get_active_ep();
        select_active_ep(ep_priv->ep_index);

        if(is_in_ep(ep_priv))  /* ep in, rx */
        {
            USBC_Host_ClearEpCSR(usbh_bsp_hdle, USBC_EP_TYPE_RX);
            USBC_Host_FlushFifo(usbh_bsp_hdle, USBC_EP_TYPE_RX);
        }
        else   /* ep out, tx */
        {
            USBC_Host_ClearEpCSR(usbh_bsp_hdle, USBC_EP_TYPE_TX);
            USBC_Host_FlushFifo(usbh_bsp_hdle, USBC_EP_TYPE_TX);
        }

        select_active_ep(old_ep_index);

        finish_request(ep_priv, urb);
    }

    USB_OS_EXIT_CRITICAL(cup_sr);

    return 0;
}

/*
********************************************************************************
*                     hcd_ops_endpoint_disable
*
* Description:
*     just free ep_priv mem,and del form ep_list[]
*
* Arguments:
*     hcd  		: input.
*     hep  		: input.
*     urb  		: input.
*     mem_flags : input.
*
* Return value:
*     void
*
* note:
*
*********************************************************************************
*/
static void __hcd_ops_ep_disable(struct hc_gen_dev *hcd, struct usb_host_virt_endpoint *hep)
{
    struct hc_private *hc_priv	= NULL ;
    struct ep_private *ep_priv	= NULL;
    __cpu_sr cup_sr 			= 0;

    //--<1>--check input
    if((hcd == NULL) || (hep == NULL))
    {
        DMSG_PANIC("err: __hcd_ops_ep_disable: input error\n");
        return;
    }

    //--<2>--initialize parameter
    hc_priv = hcd_to(hcd);
    if(hc_priv == NULL)
    {
        DMSG_PANIC("err: __hcd_ops_ep_disable, hc_priv = NULL\n");
        return ;
    }

    ep_priv = hep->hcpriv;
    if(ep_priv == NULL)
    {
        DMSG_PANIC("err: __hcd_ops_ep_disable, ep = NULL\n");
        return ;
    }

    DMSG_INFO("[hcd]: ep is disable, ep_priv = %x, ep_index = %d\n",
              ep_priv, get_phy_ep_index(ep_priv));

    USB_OS_ENTER_CRITICAL(cup_sr);

    //--<3>--if ep is working, must stop
    if(ep_priv->dma_working)
    {
        DMSG_PANIC("wrn: urb is done by dma, but happen ep disable\n");
        hcd_dma_stop();
        clean_ep_dma_status(ep_priv);
    }

    //--<3>--从hc_priv->async队列中删除所有与ep_priv相关的node
    if(ep_priv->ep_ts_type == PIPE_BULK || ep_priv->ep_ts_type == PIPE_CONTROL)
    {
        hcd_nuke_aysnc_ep(hc_priv, ep_priv);
    }
    else
    {
        hcd_nuke_periodic_ep(hc_priv, ep_priv);
    }

    //--<4>--清除 ep 配置 和 hc_priv 资源
    if(ep_priv->ep_ts_type != PIPE_CONTROL)
    {
        hcd_ep_config_default(ep_priv);
    }

    //--<5>--清除 ep 和 hep的关联
    hep->hcpriv = NULL;

    //--<6>--free ep_priv
    USB_OS_MEMSET(ep_priv, 0, sizeof(struct ep_private));
    if(ep_priv)
    {
        USB_OS_FREE(ep_priv);
        ep_priv = NULL;
    }
    else
    {
        DMSG_PANIC("ERR: parameter is NULL, can not free\n");
    }

    USB_OS_EXIT_CRITICAL(cup_sr);

    return ;
}

/*
********************************************************************************
*                     __hcd_ops_get_frame
* Description:
*
* Arguments:
*     hcd  		: input.
*
* Return value:
*     void
*
* note:
*
*********************************************************************************
*/
static s32 __hcd_ops_get_frame(struct hc_gen_dev *hcd)
{
    DMSG_PANIC("PANIC : hcd_ops_get_frame() not support now\n");

    return 0;
}

/*
********************************************************************************
*                     hcd_ops_hub_status_data
* Description:
*     get hub status
* Arguments:
*     hcd  		: input.
*     hep  		: input.
*     urb  		: input.
*     mem_flags : input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
static s32 __hcd_ops_hub_status_data(struct hc_gen_dev *hcd, u8 *buf)
{
    struct hc_private *hc_priv = NULL ;
    s32	 retval 				= 0;
    __cpu_sr cup_sr 			= 0;

    hc_priv = hcd_to(hcd);

    USB_OS_ENTER_CRITICAL(cup_sr);
    if (hcd->state != HC_GEN_DEV_STATE_RUNNING)
    {
        goto done;
    }

    if ((hc_priv->port & PORT_C_MASK) != 0)
    {
        /* tell khubd port 1 changed */
        *buf = (1 << 1);
        retval = 1;
    }

done:
    USB_OS_EXIT_CRITICAL(cup_sr);

    return retval;
}

/*
********************************************************************************
*                     hcd_ops_hub_control
* Description:
*     控制hub端口的行为
* Arguments:
*     hcd  		: input.
*     typeReq  	: input.  hub请求类型
*     wValue  	: input.
*     wIndex 	: input.
*	  buffer	: input.
*	  wLength	: input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
static s32 __hcd_ops_hub_control(struct hc_gen_dev *hcd,
                                 u16 typeReq,
                                 u16 wValue,
                                 u16 wIndex,
                                 u8 *buffer,
                                 u16 wLength)
{
    struct hc_private *hc_priv = NULL;
    s32 retval 					= 0;
    __cpu_sr cup_sr 			= 0;

    //--<1>--check input
    if(hcd == NULL || buffer == NULL)
    {
        DMSG_PANIC("ERR: __hcd_ops_hub_control, input error\n");
        return -EINVAL;
    }

    USB_OS_ENTER_CRITICAL(cup_sr);

    //--<2>--initialize parameter
    hc_priv = hcd_to(hcd);
    if(hc_priv == NULL)
    {
        DMSG_PANIC("ERR: __hcd_ops_hub_control, hc_priv = NULL\n");
        USB_OS_EXIT_CRITICAL(cup_sr);
        return -EINVAL;
    }

    //--<3>--check validity
    if (hcd->state != HC_GEN_DEV_STATE_RUNNING)
    {
        DMSG_PANIC("<HUB_CTRL> hc state is not HC_STATE_RUNNING\n");
        USB_OS_EXIT_CRITICAL(cup_sr);
        return -ETIMEDOUT;
    }

    //--<4>--处理typeReq
    switch(typeReq)
    {
    case ClearHubFeature:
        DMSG_PANIC("ERR: ClearHubFeature, wValue: %x, wIndex: %x, wLength: %x\n"
                   , wValue, wIndex, wLength);
        break;

    case ClearPortFeature:
        if (wIndex != 1 || wLength != 0)
        {
            DMSG_PANIC("ERR: parameter is error, wIndex = %d, wLength = %d\n", wIndex, wLength);
            goto hub_error;
        }

        switch (wValue)
        {
        case USB_PORT_FEAT_ENABLE:
            hc_priv->port &= ~(USB_PORT_STAT_ENABLE
                               | USB_PORT_STAT_LOW_SPEED
                               | USB_PORT_STAT_HIGH_SPEED);
            hc_priv->rhstate = RH_DISABLE;
            if(hc_priv->port & USB_PORT_STAT_POWER)
            {
                port_power(hc_priv, 0);
            }
            break;

        case USB_PORT_FEAT_SUSPEND:
            port_resume(hc_priv);
            hc_priv->port &= ~(1 << wValue);
            break;

        case USB_PORT_FEAT_POWER:
            hc_priv->port = 0;
            hc_priv->rhstate = RH_POWEROFF;
            port_power(hc_priv, 0);
            break;

        case USB_PORT_FEAT_C_ENABLE:
        case USB_PORT_FEAT_C_SUSPEND:
        case USB_PORT_FEAT_C_CONNECTION:
        case USB_PORT_FEAT_C_OVER_CURRENT:
        case USB_PORT_FEAT_C_RESET:
            hc_priv->port &= ~(1 << wValue);
            break;

        default:
            goto hub_error;
        }
        break;

    case GetHubDescriptor:
        hub_descriptor((struct usb_hub_descriptor *) buffer);
        break;

    case GetHubStatus:
        *(s32 *) buffer = 0;
        break;

    case GetPortStatus:
        if (wIndex != 1)
        {
            DMSG_PANIC("ERR: parameter is error, wIndex = %d, wLength = %d\n", wIndex, wLength);
            goto hub_error;
        }

        *(s32 *) buffer = cpu_to_le32(hc_priv->port);
        break;

    case SetHubFeature:
        DMSG_PANIC("ERR: parameter is error, wIndex = %d, wLength = %d\n", wIndex, wLength);
        goto hub_error;
    //break;

    case SetPortFeature:
        switch(wValue)
        {
        case USB_PORT_FEAT_POWER:
            if(hc_priv->port & USB_PORT_STAT_POWER)
                break;
            hc_priv->port |= USB_PORT_STAT_POWER;
            hc_priv->rhstate = RH_POWERED;
            port_power(hc_priv, 1);
            break;

        case USB_PORT_FEAT_RESET:
            /* if it's already enabled, disable */
            hc_priv->port &= ~(USB_PORT_STAT_ENABLE
                               | USB_PORT_STAT_LOW_SPEED
                               | USB_PORT_STAT_HIGH_SPEED);
            hc_priv->port |= USB_PORT_STAT_RESET;
            hc_priv->rhstate = RH_RESET;
            USBC_INT_EnableUsbMiscUint(usbh_bsp_hdle, USBC_BP_INTUSB_RESET);

            if(hc_priv->rhstate == RH_ATTACHED)
            {
                DMSG_PANIC("ERR: reset start by connection event\n");
            }
            else
            {
                DMSG_PANIC("ERR: reset start by USB Core, and the port state currently is %d\n"
                           , hc_priv->rhstate);
                port_reset(hc_priv);
            }

            after_reset_detect();
            break;


        case USB_PORT_FEAT_SUSPEND:
            //hc_priv->port |= USB_PORT_FEAT_SUSPEND;
            hc_priv->port |= USB_PORT_STAT_SUSPEND;
            hc_priv->rhstate = RH_SUSPEND;

            port_suspend(hc_priv);
            break;

        default:
            if(hc_priv->port  & USB_PORT_STAT_POWER)
            {
                hc_priv->port |= (1 << wValue);
            }
        }
        break;

    default:
hub_error:
        retval = -EPIPE;
        DMSG_PANIC("PANIC : <HUB_CTRL> hub control error\n");
        break;

    }

    USB_OS_EXIT_CRITICAL(cup_sr);

    if ((hc_priv->port & PORT_C_MASK) != 0)
    {
        usb_hcd_poll_rh_status (hcd);
    }

    return retval;
}

/*
********************************************************************************
*                     __hcd_ops_hub_suspend
* Description:
*     suspend hub
*
* Arguments:
*     hcd  		: input.
*
* Return value:
*     void
* note:
*
*********************************************************************************
*/
static int __hcd_ops_hub_suspend(struct hc_gen_dev *hcd)
{
    DMSG_PANIC("ERR: suspend hub is not support\n");
    return 0;
}

/*
********************************************************************************
*                     __hcd_ops_hub_resume
* Description:
*     suspend hub
*
* Arguments:
*     hcd  		: input.
*
* Return value:
*     void
* note:
*
*********************************************************************************
*/
static int __hcd_ops_hub_resume(struct hc_gen_dev *hcd)
{
    DMSG_PANIC("ERR: resume hub is not support\n");
    return 0;
}

/*
********************************************************************************
*                     __hcd_ops_hub_resume
* Description:
*     suspend hub
*
* Arguments:
*     hcd  		: input.
*
* Return value:
*     void
* note:
*
*********************************************************************************
*/
static int __hcd_ops_hub_notify(struct hc_gen_dev *hcd , u32 event)
{
    struct hc_private *hc_priv = hcd_to( hcd);
    __cpu_sr cpu_sr = 0;

    //处理最后一次proc
    if(hc_priv->last_not_proc_event == RHUB_EVNET_PLUGIN_ACT)
    {
        if(hc_priv->root_hub_plug_event == RHUB_EVNET_PLUGOUT_ACT)
        {
            USB_OS_ENTER_CRITICAL(cpu_sr);

            hc_priv->root_hub_plug_event 		= RHUB_EVNET_PLUGIN_ACT;
            hc_priv->plug_event_is_proc_comp	= 0;
            hc_priv->last_not_proc_event 		= RHUB_EVNET_NULL;


            hc_priv->port 		&= ~(USB_PORT_STAT_CONNECTION |
                                     USB_PORT_STAT_ENABLE |
                                     USB_PORT_STAT_LOW_SPEED |
                                     USB_PORT_STAT_HIGH_SPEED |
                                     USB_PORT_STAT_SUSPEND);
            hc_priv->port 		|=  (USB_PORT_STAT_C_CONNECTION << 16);
            hc_priv->rhstate 	= RH_NOATTACHED;
            USB_OS_EXIT_CRITICAL(cpu_sr);

            //通知
            usb_hcd_poll_rh_status(hc_priv->point_gen_dev);
        }
    }
    else if(hc_priv->last_not_proc_event == RHUB_EVNET_PLUGOUT_ACT)
    {
        if(hc_priv->root_hub_plug_event == RHUB_EVNET_PLUGIN_ACT)
        {
            USB_OS_ENTER_CRITICAL(cpu_sr);

            hc_priv->root_hub_plug_event 		= RHUB_EVNET_PLUGOUT_ACT;
            hc_priv->plug_event_is_proc_comp 	= 0;
            hc_priv->last_not_proc_event 		= RHUB_EVNET_NULL;


            //修改port ,表示已经有连接
            hc_priv->port |= (USB_PORT_STAT_C_CONNECTION << 16);

            //set port status bit,and indicate the present of  a device
            hc_priv->port |= USB_PORT_STAT_CONNECTION;
            hc_priv->rhstate = RH_ATTACHED;

            USB_OS_EXIT_CRITICAL(cpu_sr);

            //通知
            usb_hcd_poll_rh_status(hc_priv->point_gen_dev);
        }
    }
    else if(hc_priv->root_hub_plug_event != RHUB_EVNET_NULL)
    {
        hc_priv->plug_event_is_proc_comp = 0;
    }
    else
    {
        //wait
    }

    return 0;
}

/* time : suspend多久以后再关电 */
void hcd_stop(__u32 time)
{
    struct hc_private *hc_priv = host_controller;
    __cpu_sr cup_sr = 0;

    if(hc_priv == NULL)
    {
        DMSG_PANIC("ERR: hcd_stop: hc_priv == NULL\n");
        return;
    }

    DMSG_INFO("hcd_stop: delay %d ms\n", time);

    //--<0>--(临时处理)关闭所有中断, 不让hcd工作
    __port_suspend();
    disable_usb_irq();
    USB_OS_DELAY_x_MS(time);
    __port_power(0);

    /* dequeue 所有ep urb */
    hcd_nuke_hc_ep_list(hc_priv);

    USB_OS_ENTER_CRITICAL(cup_sr);
    hc_priv->root_hub_plug_event 		= RHUB_EVNET_PLUGOUT_ACT;
    hc_priv->last_not_proc_event 		= RHUB_EVNET_NULL;
    hc_priv->plug_event_is_proc_comp 	= 1;				//处理中

    hc_priv->port &= ~(USB_PORT_STAT_CONNECTION |
                       USB_PORT_STAT_ENABLE |
                       USB_PORT_STAT_LOW_SPEED |
                       USB_PORT_STAT_HIGH_SPEED |
                       USB_PORT_STAT_SUSPEND);

    hc_priv->port 		|=  (USB_PORT_STAT_C_CONNECTION << 16);
    hc_priv->rhstate 	= RH_NOATTACHED;
    USB_OS_EXIT_CRITICAL(cup_sr);

    usb_hcd_poll_rh_status(hc_priv->point_gen_dev);

    return;
}

/*
********************************************************************************
*                     hcd_pirv_init
* Description:
*     初始化hc_priv
* Arguments:
*     hcd  		: input.
*     hep  		: input.
*     urb  		: input.
*     mem_flags : input.
* Return value:
*     void
* note:
*
*********************************************************************************
*/
static struct hc_private *hcd_pirv_init(struct hc_gen_dev *hcd)
{
    struct hc_private *hc_priv = hcd_to( hcd);
    __cpu_sr lock = 0;

    //由于hc_gen_dev和hc_priv一起分配
    host_controller = hc_priv;

    USB_OS_ENTER_CRITICAL(lock);

    USB_OS_MEMSET(hc_priv, 0, sizeof(struct hc_private));
    INIT_LIST_HEAD(&(hc_priv->async));
    hc_priv->rhstate			= RH_POWEROFF;
    hc_priv->point_gen_dev		= hcd;
    hc_priv->fifo_total_size	= USBC_GetPortFifoSize(usbh_bsp_hdle);

#ifdef 	HCD_INT_ISO_TRANSFER
    hc_priv->next_periodic		= NULL;
    hc_priv->sof_periodic_entry	= NULL;
    hc_priv->next_async 		= NULL;
    hc_priv->stat_overrun 		= 0;
    hc_priv->frame 				= 0;
    hc_priv->periodic_count 	= 0;
    hc_priv->urb_counter 		= 0;
#endif

    USB_OS_EXIT_CRITICAL(lock);

    return hc_priv;
}

/*
********************************************************************************
*                     hc_env_all_init
* Description:
*
* Arguments:
*
* Return value:
*
* note:
*
*********************************************************************************
*/
static void hc_env_all_init(struct hc_priv_env_s *env_input)
{
    USB_OS_MEMSET(env_input, 0, sizeof(struct hc_priv_env_s));
    env_input->has_set_addr_happend = HAS_SET_ADDR_HAPPEND_FAIL;
}

/*
********************************************************************************
*                     hcd_init_hc_driver
* Description:
*
* Arguments:
*
* Return value:
*
* note:
*
*********************************************************************************
*/
static s32 hcd_init_hc_driver(void)
{
    USB_OS_MEMSET(&hc_driver_g, 0, sizeof(struct hc_driver));

    hc_driver_g.description 		= "hcd";
    hc_driver_g.hcd_priv_size 		= sizeof(struct hc_private);

    hc_driver_g.flags 				= HC_DRIVER_FLAG_HCD_USB2;
    hc_driver_g.irq 				= NULL;
    hc_driver_g.start 				= __hcd_ops_start;
    hc_driver_g.stop 				= __hcd_ops_stop;
    hc_driver_g.urb_enqueue 		= __hcd_ops_urb_queue;
    hc_driver_g.urb_dequeue 		= __hcd_ops_urb_dequeue;
    hc_driver_g.endpoint_disable 	= __hcd_ops_ep_disable;
    hc_driver_g.get_frame_number 	= __hcd_ops_get_frame;
    hc_driver_g.hub_status_data 	= __hcd_ops_hub_status_data;
    hc_driver_g.hub_control 		= __hcd_ops_hub_control;
    hc_driver_g.hub_suspend 		= __hcd_ops_hub_suspend;
    hc_driver_g.hub_resume 			= __hcd_ops_hub_resume;
    hc_driver_g.hub_notify			= __hcd_ops_hub_notify;

    return 0;
}

static s32 hcd_init_driver(void)
{
    return hcd_init_hc_driver();
}

static s32 hcd_exit_driver(void)
{
    USB_OS_MEMSET(&hc_driver_g, 0, sizeof(struct hc_driver));

    return 0;
}

/*
********************************************************************************
*                     hcd_open_clk
* Description:
*
* Arguments:
*
* Return value:
*
* note:
*
*********************************************************************************
*/
#if 0
#define get_wvalue(addr)	(*((volatile unsigned long  *)(addr)))
#define put_wvalue(addr, v)	(*((volatile unsigned long  *)(addr)) = (unsigned long)(v))


#define CCM_BASE  (0xf1c20000)


void usb_phy_config()
{
    __u32 reg_val = 0;
    __u32 i;
    __u32 value;

    //__log("1,clock:60:%x, cc:%x, 2c0:%x\n", (*(volatile unsigned int *)(0xf1c20060)), (*(volatile unsigned int *)(0xf1c200cc)), (*(volatile unsigned int *)(0xf1c202c0)));

    reg_val = get_wvalue(CCM_BASE + 0x2c0);
    reg_val &= ~(0x01 << 24);
    put_wvalue(CCM_BASE + 0x2c0, reg_val);
    reg_val = get_wvalue(CCM_BASE + 0x60);
    reg_val &= ~(0x01 << 24);
    put_wvalue(CCM_BASE + 0x60, reg_val);
    for(i = 0; i < 10000000; i++);

    //USB PHY Config
    reg_val = get_wvalue(CCM_BASE + 0xCC);
    reg_val &= ~(0x01 << 0);
    //#ifndef FPGA_PLATFORM
    //	 reg_val |= (0x01<<1); //Special Clock Gating
    //#endif
    put_wvalue(CCM_BASE + 0xCC, reg_val);
    for(i = 0; i < 10000000; i++);
    reg_val = get_wvalue(CCM_BASE + 0xCC);
    reg_val |= (0x03 << 0); //Phy Reset
    put_wvalue(CCM_BASE + 0xCC, reg_val);
    for(i = 0; i < 10000; i++);

    __wrn("0xcc = %x\n", get_wvalue(CCM_BASE + 0xCC));
    for(i = 0; i < 10000; i++);

    //USB SIE Config
    reg_val = get_wvalue(CCM_BASE + 0x60);
    reg_val |= (0x01 << 24);
    put_wvalue(CCM_BASE + 0x60, reg_val);
    for(i = 0; i < 10000; i++);
    esKRNL_TimeDly(5);
    reg_val = get_wvalue(CCM_BASE + 0x2c0);
    reg_val |= (0x01 << 24);
    put_wvalue(CCM_BASE + 0x2c0, reg_val);
    for(i = 0; i < 10; i++);
    //__log("2,clock:60:%x, cc:%x, 2c0:%x\n", (*(volatile unsigned int *)(0xf1c20060)), (*(volatile unsigned int *)(0xf1c200cc)), (*(volatile unsigned int *)(0xf1c202c0)));

#if 0
    put_wvalue(0xf1c13040, get_wvalue(0xf1c13040) | (0x01 << 8));
    for(i = 0; i < 10000; i++);
    //printk("Role:0x%x\n",get_wvalue(0xf1c13040));

    //Map PC0 as input for simulation
    //  reg_val = get_wvalue(PIO_BASE+0x48);
    //  reg_val &= ~(0x00000007);
    //reg_val |=  (0x00000001);
    // put_wvalue(PIO_BASE+0x48, reg_val);


    do
    {

        value = readl(0xf0000000 + 0X01C13000 + 0x40);
        __msg("host or device value= %x\n", (value));
        esKRNL_TimeDly(10);
        i++;
        if(i > 5)
        {

            i = 0;
            //  goto lpppp;
        }

    }
    while(value & (1 << 15));


    __msg("host or device %x\n", (get_wvalue(0xf1c13040) & (0x1 << 15)) >> 15);

    for( i = 1; i < 4; i++)
    {

        __msg("i=%d\n", i);
        put_wvalue(0xf1c13040, (get_wvalue(0xf1c13040) & ~(0xf << 16)) | (i << 16));
        //esKRNL_TimeDly(10);
        USBC_Host_FlushFifo(usbh_bsp_hdle, USBC_EP_TYPE_RX);
        USBC_Host_FlushFifo(usbh_bsp_hdle, USBC_EP_TYPE_TX);
        //esKRNL_TimeDly(10);
        //HOTLEE_PRINT_EP1();

    }

    //esKRNL_TimeDly(10);

    put_wvalue(0xf1c13040, (get_wvalue(0xf1c13040) & ~(0xf << 16)) | (0 << 16));

#endif



}

s32  hcd_open_clk(void)

{
    usb_phy_config();
    return 0;


}

#else
s32  hcd_open_clk(void)
{
    s32  ret = 0;
    __u32 USB_AHB_CLK_NO = 0;
    __u32 USB_MOD_CLK_NO = 0;
    __u32 i;
    USB_AHB_CLK_NO = CSP_CCM_MOD_CLK_AHB_USB;
    USB_MOD_CLK_NO = CSP_CCM_MOD_CLK_USB_PHY;


    //__log("1,clock:60:%x, cc:%x, 2c0:%x\n", (*(volatile unsigned int *)(0xf1c20060)), (*(volatile unsigned int *)(0xf1c200cc)), (*(volatile unsigned int *)(0xf1c202c0)));
    //--<1>--开usb sie时钟
    usbh_clock_ahb = USB_esCLK_Open(USB_AHB_CLK_NO);
    if(usbh_clock_ahb == NULL)
    {
        DMSG_PANIC("usbd_drv: open usb AHB clock failed\n");
        return -1;
    }

    ret = USB_esCLK_OnOff(usbh_clock_ahb, CLK_ON);
    if(ret != EPDK_OK)
    {
        DMSG_PANIC("usbd_drv: on usbd AHB clock failed\n");
        return -1;
    }

    //--<2>--开usb phy时钟
    usbh_clock_phy = USB_esCLK_Open(USB_MOD_CLK_NO);
    if(usbh_clock_phy == NULL)
    {
        DMSG_PANIC("usbd_drv: open usb AHB clock failed\n");
        return -1;
    }
    esCLK_MclkReset(usbh_clock_phy, 1); //add by hot.lee for test   1---> 0

    for(i = 0; i < 1000; i++);
    ret = USB_esCLK_OnOff(usbh_clock_phy, CLK_ON);
    if(ret != EPDK_OK)
    {
        DMSG_PANIC("usbd_drv: on usbd PHY clock failed\n");
        return -1;
    }


    esCLK_MclkReset(usbh_clock_phy, 0); //add by hot.lee for test  0 ---->1
    esKRNL_TimeDly(3);

    //__log("2,clock:60:%x, cc:%x, 2c0:%x\n", (*(volatile unsigned int *)(0xf1c20060)), (*(volatile unsigned int *)(0xf1c200cc)), (*(volatile unsigned int *)(0xf1c202c0)));
    return 0;
}
#endif
/*
***********************************************************
*  关usb clock
***********************************************************
*/
s32  hcd_close_clk(void)
{
    s32  ret = 0;

    ret = USB_esCLK_OnOff(usbh_clock_ahb, CLK_OFF);
    if(ret != EPDK_OK)
    {
        DMSG_PANIC("usbd_drv: off usbd PHY clock failed\n");
        return -1;
    }

    ret = USB_esCLK_Close(usbh_clock_ahb);
    if(ret != EPDK_OK)
    {
        DMSG_PANIC("usbd_drv: close usbd AHB clock failed\n");
        return -1;
    }

    ret = USB_esCLK_OnOff(usbh_clock_phy, CLK_OFF);
    if(ret != EPDK_OK)
    {
        DMSG_PANIC("usbd_drv: off usbd PHY clock failed\n");
        return -1;
    }

    ret = USB_esCLK_Close(usbh_clock_phy);
    if(ret != EPDK_OK)
    {
        DMSG_PANIC("usbd_drv: close usbd PHY clock failed\n");
        return -1;
    }

    return 0;
}

/*
*******************************************************************************
*                     hcd_usb_init
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
static s32 hcd_usb_init(void)
{
    //--<1>--设置bus的访问方式
    USBC_SelectBus(usbh_bsp_hdle, USBC_IO_TYPE_PIO, 0, 0);

    //--<2>--映射SRAM D给usb使用
    USBC_ConfigFIFO_Base(usbh_bsp_hdle, USBC_FIFO_MODE_8K);

    //--<3>--usb信号加强
    USBC_EnhanceSignal(usbh_bsp_hdle);

    //--<4>--disable all irq
    USBC_INT_DisableEpAll(usbh_bsp_hdle, USBC_EP_TYPE_TX);
    USBC_INT_DisableEpAll(usbh_bsp_hdle, USBC_EP_TYPE_RX);
    USBC_INT_DisableUsbMiscAll(usbh_bsp_hdle);

    //--<5>--enable misc irq
    USBC_INT_EnableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_SUSPEND);
    USBC_INT_EnableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_RESUME);
    USBC_INT_EnableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_RESET);
    USBC_INT_EnableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_SOF);
    USBC_INT_EnableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_CONNECT);
    USBC_INT_EnableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_DISCONNECT);
    USBC_INT_EnableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_SESSION_REQ);
    USBC_INT_EnableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_VBUS_ERROR);

    return 0;
}

/*
*******************************************************************************
*                     hcd_usb_exit
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
static s32 hcd_usb_exit(void)
{
    //--<1>--设置bus的访问方式
    USBC_SelectBus(usbh_bsp_hdle, USBC_IO_TYPE_PIO, 0, 0);

    //--<4>--enable misc irq
    USBC_INT_DisableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_SUSPEND);
    USBC_INT_DisableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_RESUME);
    USBC_INT_DisableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_RESET);
    USBC_INT_DisableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_SOF);
    USBC_INT_DisableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_CONNECT);
    USBC_INT_DisableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_DISCONNECT);
    USBC_INT_DisableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_SESSION_REQ);
    USBC_INT_DisableUsbMiscUint(usbh_bsp_hdle, USBC_INTUSB_VBUS_ERROR);

    return 0;
}

void usb_wakeup_change_detect_enable(void)
{
    s32 cpu_sr = 0;

    ENTER_CRITICAL(cpu_sr);
    USBC_EnableWakeIrq(usbh_bsp_hdle);
    USBC_EnableVbusChange(usbh_bsp_hdle);
    USBC_EnableIdChange(usbh_bsp_hdle);
    USBC_EnableDpDmChange(usbh_bsp_hdle);
    EXIT_CRITICAL(cpu_sr);
}

void usb_wakeup_change_detect_disable(void)
{
    s32 cpu_sr = 0;

    ENTER_CRITICAL(cpu_sr);
    USBC_DisableWakeIrq(usbh_bsp_hdle);
    USBC_DisableVbusChange(usbh_bsp_hdle);
    USBC_DisableIdChange(usbh_bsp_hdle);
    USBC_DisableDpDmChange(usbh_bsp_hdle);
    EXIT_CRITICAL(cpu_sr);
}

void clear_usb_wakeup_detect_flag(void)
{
    s32 cpu_sr = 0;

    ENTER_CRITICAL(cpu_sr);
    USBC_IsVbusChange(usbh_bsp_hdle);
    USBC_IsIdChange(usbh_bsp_hdle);
    USBC_IsDpDmChange(usbh_bsp_hdle);
    EXIT_CRITICAL(cpu_sr);
}


static void print_gpio_set(user_gpio_set_t *gpio_set)
{
    __inf("gpio_name 	= %s\n", gpio_set->gpio_name);
    __inf("port 		= %x\n", gpio_set->port);
    __inf("port_num 	= %x\n", gpio_set->port_num);
    __inf("mul_sel 		= %x\n", gpio_set->mul_sel);
    __inf("pull 		= %x\n", gpio_set->pull);
    __inf("drv_level 	= %x\n", gpio_set->drv_level);
    __inf("data 		= %x\n", gpio_set->data);
}

static void print_usb_board_info(usb_board_info_t *board_info)
{
    u32 i = 0;

    __inf("\n-----------usb board information--------------\n");

    __inf("chip                 = %x\n", board_info->chip);
    __inf("pid                  = %x\n", board_info->pid);
    __inf("sid                  = %x\n", board_info->sid);
    __inf("bid                  = %x\n", board_info->bid);
    __inf("controller number    = %x\n", (u32)USBC_MAX_CTL_NUM);

    __inf("usb_global_enable	= %x\n", board_info->usb_global_enable);
    __inf("usbc_num        		= %x\n", board_info->usbc_num);

    for(i = 0; i < USBC_MAX_CTL_NUM; i++)
    {
        __inf("\n");
        __inf("port[%d]:\n", i);
        __inf("valid                = %x\n", board_info->usb_port.port[i].valid);
        __inf("port                 = %x\n", board_info->usb_port.port[i].port);
        __inf("port_type            = %x\n", board_info->usb_port.port[i].port_type);
        __inf("detect_type          = %x\n", board_info->usb_port.port[i].detect_type);

        __inf("id.valid             = %x\n", board_info->usb_port.port[i].id.valid);
        __inf("id.group_type        = %x\n", board_info->usb_port.port[i].id.group_type);
        print_gpio_set(&board_info->usb_port.port[i].id.gpio_set);

        __inf("vbus.valid           = %x\n", board_info->usb_port.port[i].det_vbus.valid);
        __inf("vbus.group_type      = %x\n", board_info->usb_port.port[i].det_vbus.group_type);
        print_gpio_set(&board_info->usb_port.port[i].det_vbus.gpio_set);

        __inf("drv_vbus.valid       = %x\n", board_info->usb_port.port[i].drv_vbus.valid);
        __inf("drv_vbus.group_type  = %x\n", board_info->usb_port.port[i].drv_vbus.group_type);
        print_gpio_set(&board_info->usb_port.port[i].drv_vbus.gpio_set);

        __inf("\n");
    }

    __inf("-------------------------------------------------\n");
}

static __s32 usb_script_parse(usb_board_info_t *board_info)
{
    s32 ret = 0;
    u32 i = 0;
    char *set_usbc = NULL;

    /* usb_global_enable */
    ret = esCFG_GetKeyValue(SET_USB_PARA, KEY_USB_GLOBAL_ENABLE, (int *)&board_info->usb_global_enable, 1);
    if(ret != 0)
    {
        __wrn("ERR: get usb_global_enable failed\n");
        //		return -1;
    }

    /* usbc_num */
    ret = esCFG_GetKeyValue(SET_USB_PARA, KEY_USBC_NUM, (int *)&board_info->usbc_num, 1);
    if(ret != 0)
    {
        __wrn("ERR: get usbc_num failed\n");
        //		return -1;
    }

    for(i = 0; i < board_info->usbc_num; i++)
    {
        if(i == 0)
        {
            set_usbc = SET_USB0;
        }
        else if(i == 1)
        {
            set_usbc = SET_USB1;
        }
        else
        {
            set_usbc = SET_USB2;
        }

        /* usbc enable */
        ret = esCFG_GetKeyValue(set_usbc, KEY_USB_ENABLE, (int *)&board_info->usb_port.port[i].valid, 1);
        if(ret != 0)
        {
            __wrn("ERR: get usbc(%d) enable failed\n", i);
            //			return -1;
        }

        /* usbc port type */
        ret = esCFG_GetKeyValue(set_usbc, KEY_USB_PORT_TYPE, (int *)&board_info->usb_port.port[i].port_type, 1);
        if(ret != 0)
        {
            __wrn("ERR: get usbc(%d) port type failed\n", i);
            //			return -1;
        }

        /* usbc detect type */
        ret = esCFG_GetKeyValue(set_usbc, KEY_USB_DETECT_TYPE, (int *)&board_info->usb_port.port[i].detect_type, 1);
        if(ret != 0)
        {
            __wrn("ERR: get usbc(%d) detect type failed\n", i);
            //			return -1;
        }

        /* usbc id */
        ret = esCFG_GetKeyValue(set_usbc, KEY_USB_ID_GPIO, (int *)&board_info->usb_port.port[i].id.gpio_set, 64);
        if(ret != 0)
        {
            __wrn("ERR: get usbc(%d) id failed\n", i);
            //			return -1;
        }

        /* usbc det_vbus */
        ret = esCFG_GetKeyValue(set_usbc, KEY_USB_DETVBUS_GPIO, (int *)&board_info->usb_port.port[i].det_vbus.gpio_set, 64);
        if(ret != 0)
        {
            __wrn("ERR: get usbc(%d) id failed\n", i);
            //			return -1;
        }

        /* usbc drv_vbus */
        ret = esCFG_GetKeyValue(set_usbc, KEY_USB_DRVVBUS_GPIO, (int *)&board_info->usb_port.port[i].drv_vbus.gpio_set, 64);
        if(ret != 0)
        {
            __wrn("ERR: get usbc(%d) id failed\n", i);
            //			return -1;
        }

    }

    return 0;
}

/*
*******************************************************************************
*                     modify_usb_borad_info
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
static void modify_usb_borad_info(usb_board_info_t *board_info)
{
    u32 i = 0;

    for(i = 0; i < board_info->usbc_num; i++)
    {
        if(board_info->usb_port.port[i].id.gpio_set.port)
        {
            board_info->usb_port.port[i].id.valid = 1;

            if(board_info->usb_port.port[i].id.gpio_set.port == 0xffff)
            {
                board_info->usb_port.port[i].id.group_type = GROUP_TYPE_P1;
                board_info->usb_port.port[i].id.ex_pin_no =
                    board_info->usb_port.port[i].id.gpio_set.port_num + POWER_EPT_1;
            }
            else
            {
                board_info->usb_port.port[i].id.group_type = GROUP_TYPE_PIO;
            }
        }

        if(board_info->usb_port.port[i].det_vbus.gpio_set.port)
        {
            board_info->usb_port.port[i].det_vbus.valid = 1;

            if(board_info->usb_port.port[i].det_vbus.gpio_set.port == 0xffff)
            {
                board_info->usb_port.port[i].det_vbus.group_type = GROUP_TYPE_P1;
                board_info->usb_port.port[i].det_vbus.ex_pin_no =
                    board_info->usb_port.port[i].det_vbus.gpio_set.port_num + POWER_EPT_1;
            }
            else
            {
                board_info->usb_port.port[i].det_vbus.group_type = GROUP_TYPE_PIO;
            }
        }

        if(board_info->usb_port.port[i].drv_vbus.gpio_set.port)
        {
            board_info->usb_port.port[i].drv_vbus.valid = 1;

            if(board_info->usb_port.port[i].drv_vbus.gpio_set.port == 0xffff)
            {
                board_info->usb_port.port[i].drv_vbus.group_type = GROUP_TYPE_P1;
                board_info->usb_port.port[i].drv_vbus.ex_pin_no =
                    board_info->usb_port.port[i].drv_vbus.gpio_set.port_num + POWER_EPT_1;
            }
            else
            {
                board_info->usb_port.port[i].drv_vbus.group_type = GROUP_TYPE_PIO;
            }
        }
    }

    return;
}
/*
*******************************************************************************
*                     get_usb_board_info
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
static void get_usb_board_info(usb_board_info_t *board_info)
{
    __s32 ret = 0;

    //	board_info->chip = esKSRV_GetVersion(EPDK_VER_CHIP);
    //	board_info->pid  = esKSRV_GetVersion(EPDK_VER_PID);
    //	board_info->sid  = esKSRV_GetVersion(EPDK_VER_SID);
    //	board_info->bid  = esKSRV_GetVersion(EPDK_VER_BID);

    ret = usb_script_parse(board_info);
    if(ret != 0)
    {
        __err("err: _cfg_parser failed\n");
    }

    modify_usb_borad_info(board_info);

    print_usb_board_info(board_info);

    return ;
}

/*
*******************************************************************************
*                     init_hcd_sys_cfg
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
static __s32 init_hcd_sys_cfg(__u32 port, usb_board_info_t *board_info, hcd_sys_cfg_t *hcd_sys_cfg)
{
    board_usb_port_t *usb_port = &(board_info->usb_port.port[port]);
    __u32 Pin_Dir_in = 1;    /* pin 的方向: 输出 */

    switch(usb_port->drv_vbus.group_type)
    {
    case GROUP_TYPE_PIO:
        hcd_sys_cfg->drv_vbus_pin_hdle = (void *)esPINS_PinGrpReq(&usb_port->drv_vbus.gpio_set, 1);
        if(hcd_sys_cfg->drv_vbus_pin_hdle == NULL)
        {
            DMSG_PANIC("ERR: init_hcd_sys_cfg: esPINS_PIOReq failed\n");
            return -1;
        }

        esPINS_SetPinIO((__hdle)hcd_sys_cfg->drv_vbus_pin_hdle, Pin_Dir_in, 0);
        esPINS_SetPinPull((__hdle)hcd_sys_cfg->drv_vbus_pin_hdle, 0, 0);
        break;

    case GROUP_TYPE_P1:
        hcd_sys_cfg->drv_vbus_pin_hdle = (void *)eLIBs_fopen("b:\\HID\\POWER", "r+");
        if(hcd_sys_cfg->drv_vbus_pin_hdle == NULL)
        {
            DMSG_PANIC("ERR: init_hcd_sys_cfg: eLIBs_fopen p1 failed\n");
            return -1;
        }
        break;

    default:
        DMSG_PANIC("ERR: unkown group_type(%x)\n", usb_port->drv_vbus.group_type);
        return -1;
    }

    return 0;
}

/*
*******************************************************************************
*                     exit_hcd_sys_cfg
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
static __s32 exit_hcd_sys_cfg(__u32 port, usb_board_info_t *board_info, hcd_sys_cfg_t *hcd_sys_cfg)
{
    board_usb_port_t *usb_port = &(board_info->usb_port.port[port]);
    __s32 ret = 0;

    if(hcd_sys_cfg->drv_vbus_pin_hdle == NULL)
    {
        DMSG_PANIC("ERR: drv_vbus_pin_hdle is NULL\n");
        return -1;
    }

    if(hcd_sys_cfg->drv_vbus_pin_hdle)
    {
        switch(usb_port->drv_vbus.group_type)
        {
        case GROUP_TYPE_PIO:
        {
            __hdle pin_hdle = (__hdle)hcd_sys_cfg->drv_vbus_pin_hdle;

            ret = esPINS_PinGrpRel(pin_hdle , 0);
            if(ret != EPDK_OK)
            {
                __wrn("ERR : free id hdle failed\n");
                return -1;
            }
        }
        break;

        case GROUP_TYPE_P1:
        {
            ES_FILE *pfile = (ES_FILE *)hcd_sys_cfg->drv_vbus_pin_hdle;

            ret = eLIBs_fclose(pfile);
            if(ret == EPDK_FAIL)
            {
                DMSG_PANIC("ERR: hcd_close_p1_device: eLIBs_fclose failed\n");
                return -1;
            }
        }
        break;

        default:
            DMSG_PANIC("ERR: unkown group_type(%x)\n", usb_port->drv_vbus.group_type);
            return -1;
        }
    }

    return 0;
}

/*
*******************************************************************************
*                     hcd_hardware_init
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
static s32 hcd_hardware_init(void)
{
    usb_board_info_t *board_info = &usbh_board_info_g;
    __s32 ret = 0;

    USB_OS_MEMSET(board_info, 0, sizeof(usb_board_info_t));
    get_usb_board_info(board_info);

    //--<1>--disable wakeup interruput
    usb_wakeup_change_detect_disable();
    clear_usb_wakeup_detect_flag();

    //--<2>--for id to low, as host
    USBC_ForceId(usbh_bsp_hdle, USBC_ID_TYPE_HOST);

    //--<3>--hcd sys config initialize
    ret = init_hcd_sys_cfg(usbh_port, board_info, &hcd_sys_cfg_g);
    if(ret != 0)
    {
        DMSG_PANIC("ERR: init_hcd_sys_cfg failed\n");
        return -1;
    }

    usb_phy_new_tp_write(usbh_bsp_hdle, 0x2a, 3, 2);
    ret = usb_phy_new_tp_read(usbh_bsp_hdle, 0x2a, 2);

    usb_phy_new_tp_write(usbh_bsp_hdle, 0x20, 0x14, 5);
    ret = usb_phy_new_tp_read(usbh_bsp_hdle, 0x20, 5);

    return 0;
}

/*
*******************************************************************************
*                     hcd_hardware_exit
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
static s32 hcd_hardware_exit(void)
{
    usb_board_info_t *board_info = &usbh_board_info_g;

    //--<1>--hcd sys config initialize
    exit_hcd_sys_cfg(usbh_port, board_info, &hcd_sys_cfg_g);

    //--<2>--enable wakeup interruput
    usb_wakeup_change_detect_enable();

    //--<3>--for id to disable
    USBC_ForceId(usbh_bsp_hdle, USBC_ID_TYPE_DISABLE);

    return 0;
}

/*
*******************************************************************************
*                     hcd_hc_probe
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
static s32 hcd_hc_probe(void)
{
    s32 ret = 0;
    __u32 usb_irq_no = 0;

    usb_irq_no = INTC_IRQNO_USB0;

    ret = USB_OS_ISR_INSTALL(usb_irq_no, (__pISR_t)__hcd_ops_irq, NULL);
    if(ret != EPDK_OK)
    {
        DMSG_PANIC("ERR: reg isr fail\n");
        return -1;
    }

    ret = hcd_dma_probe();
    if(ret != 0)
    {
        DMSG_PANIC("ERR: hcd_dma_probe failed\n");
        return -1;
    }

    return 0;
}

/*
*******************************************************************************
*                     hcd_hc_remove
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
static s32 hcd_hc_remove(void)
{
    s32 ret = 0;
    __u32 usb_irq_no = 0;

    usb_irq_no = INTC_IRQNO_USB0;

    ret = USB_OS_ISR_UNINSTALL(usb_irq_no, (__pISR_t)__hcd_ops_irq);
    if(ret != EPDK_OK)
    {
        DMSG_PANIC("ERR: USB_OS_ISR_UNINSTALL failed\n");
        return -1;
    }

    hcd_dma_stop();
    ret = hcd_dma_remove();
    if(ret != 0)
    {
        DMSG_PANIC("ERR: hcd_dma_remove failed\n");
        return -1;
    }

    return 0;
}

/*
*******************************************************************************
*                     usbh_get_usbd_port
*
* Description:
*    设置 usb 的端口号
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
__s32 usbh_set_usbd_port(__u32 port_num)
{
    __cpu_sr cup_sr = 0;

    USB_OS_ENTER_CRITICAL(cup_sr);
    usbh_port = port_num;
    USB_OS_EXIT_CRITICAL(cup_sr);

    return EPDK_OK;
}

/*
*******************************************************************************
*                     usbh_get_usbd_port
*
* Description:
*    获得USB的端口号
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
__u32 usbh_get_usbd_port(void)
{
    return usbh_port;
}

/*
*******************************************************************************
*                     hcd_init
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
s32 hcd_init(void)
{
    struct hc_gen_dev *hcd = NULL;
    bsp_usbc_t usbc;
    __u32 i = 0;
    //	__s32 ret = 0;
    //{
    //    __u32 temp = 1;
    //    while(temp);
    //}
    //--<1>--init variable
    USB_OS_MEMSET(&usbh_board_info_g, 0, sizeof(usb_board_info_t));
    USB_OS_MEMSET(&hc_driver_g, 0, sizeof(struct hc_driver));
    USB_OS_MEMSET(&env_all, 0, sizeof(struct hc_priv_env_s));
    USB_OS_MEMSET(&hcd_sys_cfg_g, 0, sizeof(hcd_sys_cfg_t));

    hDMA_Hdle     	= NULL;
    usbh_bsp_hdle	= NULL;

    DMSG_INFO("[hcd_init]: get usbh_port = %d.\n", usbh_port);

    //--<0>--disable usb irq
    disable_usb_irq();

    /* 初始化全局信息		*/
    hc_env_all_init(&env_all);

    //--<2>--open usb bsp
    eLIBs_memset(&usbc, 0, sizeof(bsp_usbc_t));
    for(i = 0; i < USBC_MAX_CTL_NUM; i++)
    {
        usbc.usbc_info[i].num = i;

        switch(i)
        {
        case 0:
            usbc.usbc_info[i].base = (__u32)esMEMS_GetIoVAByPA(USBC0_BASE, 0xffc);
            break;

        case 1:
            usbc.usbc_info[i].base = (__u32)esMEMS_GetIoVAByPA(USBC1_BASE, 0xffc);
            break;

        case 2:
            usbc.usbc_info[i].base = (__u32)esMEMS_GetIoVAByPA(USBC2_BASE, 0xffc);
            break;

        default:
            __wrn("ERR: unkown cnt(%d)\n", i);
            usbc.usbc_info[i].base = 0;
        }
    }
    usbc.sram_base = (__u32)esMEMS_GetIoVAByPA(0x01c00000, 0xfc);

    USBC_init(&usbc);
    usbh_bsp_hdle = USBC_open_otg(usbh_port);
    if(usbh_bsp_hdle == NULL)
    {
        DMSG_PANIC("ERR: hcd_wa0003_init: USBC_open_otg failed\n");
        return -1;
    }

    //--<3>--initialze other hardware
    if(hcd_hardware_init() != 0)
    {
        DMSG_PANIC("ERR: hcd_hardware_init failed\n");
        goto init_failed_1;
    }

    //--<4>--initialze usb hardware
    if(hcd_usb_init() != 0)
    {
        DMSG_PANIC("ERR: hcd_usb_init failed\n");
        goto init_failed_2;
    }

    //--<5>--init hc driver
    if(hcd_init_driver() != 0)
    {
        DMSG_PANIC("ERR: hcd_init_driver failed\n");
        goto init_failed_3;
    }

    //--<6>--创建hcd设备
    hcd = usb_create_hc_gen_dev((struct hc_driver *)(&hc_driver_g), HCD_CONTROLLER_NAME);
    if(hcd == NULL)
    {
        DMSG_PANIC("ERR : usb_create_hc_gen_dev failed\n");
        goto init_failed_4;
    }

    host_controller = hcd_pirv_init(hcd);

    //--<7>--hdc probe
    if(hcd_hc_probe() != 0)
    {
        DMSG_PANIC("ERR: hcd_hc_probe failed\n");
        goto init_failed_5;
    }

    //--<8>--add hcd to core
    if(usb_add_hc_gen_dev(hcd, 0, 0) != 0)
    {
        DMSG_PANIC("ERR: usb_add_hc_gen_dev failed\n");
        goto init_failed_6;
    }

    //--<9>--open usb irq
    enable_usb_irq();

    return EPDK_OK;

init_failed_6:
    hcd_hc_remove();
init_failed_5:
    usb_remove_hc_gen_dev(host_controller->point_gen_dev);
    host_controller = NULL;
init_failed_4:
    hcd_exit_driver();
init_failed_3:
    hcd_usb_exit();
init_failed_2:
    hcd_hardware_exit();
init_failed_1:
    USBC_close_otg(usbh_bsp_hdle);

    return EPDK_FAIL;
}

/*
*******************************************************************************
*                     hcd_exit
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
s32 hcd_exit(void)
{
    disable_usb_irq();

    hcd_hc_remove();

    usb_remove_hc_gen_dev(host_controller->point_gen_dev);
    host_controller = NULL;

    hcd_exit_driver();

    hcd_usb_exit();

    hcd_hardware_exit();

    hcd_close_clk();

    USBC_close_otg(usbh_bsp_hdle);

    return EPDK_OK;
}












