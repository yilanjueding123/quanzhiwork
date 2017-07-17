/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                            power manager sub-system
*
*                                     (c) Copyright 2008-2009, kevin.z China
*                                              All Rights Reserved
*
* File   : standby_usb.c
* Version: V1.0
* By     : kevin.z
* Date   : 2009-6-16 17:45
*********************************************************************************************************
*/
#include "standby_usb.h"

#if(ALLOW_USB_WAKEUP_STANDBY)


//==============================================================================
// USB CHECK FOR WAKEUP SYSTEM FROM STANDBY
//==============================================================================
/* usb base address */
static __hdle standby_usb_port_base[USBC_MAX_CTL_NUM] = {USB0_REG_BASE/*, USB1_REG_BASE, USB2_REG_BASE*/};


/* vbus, id, dpdm变化位是写1清零, 因此我们在操作其他bit的时候清除这些位 */
static __u32 __Standby_WakeUpClearChangeDetect(__u32 reg_val)
{
    __u32 temp = reg_val;

    temp &= ~(1 << USBC_BP_ISCR_VBUS_CHANGE_DETECT);
    temp &= ~(1 << USBC_BP_ISCR_ID_CHANGE_DETECT);
    temp &= ~(1 << USBC_BP_ISCR_DPDM_CHANGE_DETECT);

    return temp;
}

/*
*********************************************************************************************************
*                                     INIT USB FOR STANDBY
*
* Description: init usb for standby.
*
* Arguments  : none;
*
* Returns    : none;
*********************************************************************************************************
*/
void standby_usb_init(void)
{
    __u32 i 		= 0;
    __u32 reg_val 	= 0;

    for(i = 0; i < USBC_MAX_CTL_NUM; i++)
    {
        //enable wakeup irq & dpdm
        reg_val = USBC_Readl(USBC_REG_ISCR(standby_usb_port_base[i]));
        reg_val |= 1 << USBC_BP_ISCR_IRQ_ENABLE;
        reg_val |= 1 << USBC_BP_ISCR_DPDM_CHANGE_DETECT_EN;
        reg_val = __Standby_WakeUpClearChangeDetect(reg_val);
        USBC_Writel(reg_val, USBC_REG_ISCR(standby_usb_port_base[i]));
    }

    return;
}


/*
*********************************************************************************************************
*                                     EXIT USB FOR STANDBY
*
* Description: exit usb for standby.
*
* Arguments  : none;
*
* Returns    : none;
*********************************************************************************************************
*/
void standby_usb_exit(void)
{
    /* 这里就不用关闭USB资源了, 因为这个时候USB可能在工作 */
}

/*
*********************************************************************************************************
*                                     standby_is_usb_status_change
*
* Description:
*	通过dpdm, id, vbus的变化来确定USB是否变化.
*
* Arguments  :
* 	port_no  :  input. usb端口号
*
* Returns    :
*   返回USB端口是否变化
*
*********************************************************************************************************
*/
__s32 standby_is_usb_status_change(__u32 port_no)
{
    __u32 dpdm 		= 0;
    __u32 id 		= 0;
    __u32 vbus		= 0;
    __u32 reg_val 	= 0;

    //some usb event happened, need not clear usb event, leave it for usb monitor
    //dpdm
    reg_val = USBC_Readl(USBC_REG_ISCR(standby_usb_port_base[port_no]));
    dpdm = reg_val & (1 << USBC_BP_ISCR_DPDM_CHANGE_DETECT);

    //id
    id = 0;

    //vbus
    vbus = 0;

    return (dpdm | id | vbus);
}

/*
*********************************************************************************************************
*                                     QUERY USB EVENT
*
* Description: query usb event for wakeup system from standby.
*
* Arguments  : none;
*
* Returns    : result;
*               EPDK_TRUE,  some usb event happenned;
*               EPDK_FALSE, none usb event;
*********************************************************************************************************
*/
__s32 standby_query_usb_event(void)
{
    __u32 i = 0;

    //只要有一个USB端口有变化，都可以唤醒系统
    for(i = 0; i < USBC_MAX_CTL_NUM; i++)
    {
        if(standby_is_usb_status_change(i))
        {
            return EPDK_TRUE;
        }
    }

    return EPDK_FALSE;
}


#endif  //ALLOW_USB_WAKEUP_STANDBY

