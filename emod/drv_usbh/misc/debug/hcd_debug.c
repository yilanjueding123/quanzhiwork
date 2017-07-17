/*
********************************************************************************************************************
*                                              USB Host Drvier
*
*                              (c) Copyright 2007-2009, softwinners.China
*										All	Rights Reserved
*
* File Name 	: hcd_debug.c
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

#include "usb_host_config.h"
#include "usb_os_platform.h"

#include "hcd_debug.h"

#ifdef  USBH_HCD_DEBUG_GATE

static void hcd_print_all_usb_reg_suni(u32 ep_index, char *str)
{
#if(EPDK_ARCH == EPDK_ARCH_SUNI)
    __u32 usbc_base = USBC_REGS_BASE;
    __u32 old_ep_index = 0;

    DMSG_INFO("\n");
    DMSG_INFO("------------------------------%s--------------------------\n", str);

    if(ep_index != 0xff)
    {
        old_ep_index = USBC_Readw(usbc_base + USBC_REG_o_EPIND);
        writew(ep_index, (usbc_base + USBC_REG_o_EPIND));
        DMSG_INFO("old_ep_index = %d, ep_index = %d\n", old_ep_index, ep_index);
    }

    //-------------------------------------------------------------------------
    DMSG_INFO("USBC_REG_o_FADDR         = %x\n", USBC_Readb(usbc_base + USBC_REG_o_FADDR));
    DMSG_INFO("USBC_REG_o_PCTL          = %x\n", USBC_Readb(usbc_base + USBC_REG_o_PCTL));
    DMSG_INFO("USBC_REG_o_INTTxE        = %x\n", USBC_Readw(usbc_base + USBC_REG_o_INTTxE));
    DMSG_INFO("USBC_REG_o_INTRxE        = %x\n", USBC_Readw(usbc_base + USBC_REG_o_INTRxE));
    DMSG_INFO("USBC_REG_o_INTUSBE       = %x\n", USBC_Readb(usbc_base + USBC_REG_o_INTUSBE));
    DMSG_INFO("USBC_REG_o_EPIND         = %x\n", USBC_Readw(usbc_base + USBC_REG_o_EPIND));
    DMSG_INFO("USBC_REG_o_TXMAXP        = %x\n", USBC_Readw(usbc_base + USBC_REG_o_TXMAXP));
    DMSG_INFO("USBC_REG_o_CSR0          = %x\n", USBC_Readw(usbc_base + USBC_REG_o_CSR0));
    DMSG_INFO("USBC_REG_o_TXCSR         = %x\n", USBC_Readw(usbc_base + USBC_REG_o_TXCSR));
    DMSG_INFO("USBC_REG_o_RXMAXP        = %x\n", USBC_Readw(usbc_base + USBC_REG_o_RXMAXP));
    DMSG_INFO("USBC_REG_o_RXCSR         = %x\n", USBC_Readw(usbc_base + USBC_REG_o_RXCSR));

    DMSG_INFO("USBC_REG_o_COUNT0        = %x\n", USBC_Readw(usbc_base + USBC_REG_o_COUNT0));
    DMSG_INFO("USBC_REG_o_RXCOUNT       = %x\n", USBC_Readw(usbc_base + USBC_REG_o_RXCOUNT));

    //-------------------------------------------------------------------------

    if(ep_index != 0xff)
    {
        writew(old_ep_index, (usbc_base + USBC_REG_o_EPIND));
    }

    DMSG_INFO("---------------------------------------------------------------------------\n");
    DMSG_INFO("\n");
#endif
}

static void hcd_print_all_usb_reg_sunii(__u32 port, u32 ep_index, char *str)
{
#if(EPDK_ARCH == EPDK_ARCH_SUNII)
    __u32 usbc_base = 0;
    __u32 i = 0;
    __u32 old_ep_index = 0;

    if(port == 0)
    {
        usbc_base = USBC0_REGS_BASE;
    }
    else
    {
        usbc_base = USBC1_REGS_BASE;
    }

    DMSG_INFO("\n");
    DMSG_INFO("------------------------------%s--------------------------\n", str);

    if(ep_index != 0xff)
    {
        old_ep_index = USBC_Readw(usbc_base + USBC_REG_o_EPIND);
        writew(ep_index, (usbc_base + USBC_REG_o_EPIND));
        DMSG_INFO("old_ep_index = %d, ep_index = %d\n", old_ep_index, ep_index);
    }

    DMSG_INFO("USBC_REG_o_FADDR         = %x\n", USBC_Readb(usbc_base + USBC_REG_o_FADDR));
    DMSG_INFO("USBC_REG_o_PCTL          = %x\n", USBC_Readb(usbc_base + USBC_REG_o_PCTL));
    DMSG_INFO("USBC_REG_o_INTTx         = %x\n", USBC_Readw(usbc_base + USBC_REG_o_INTTx));
    DMSG_INFO("USBC_REG_o_INTRx         = %x\n", USBC_Readw(usbc_base + USBC_REG_o_INTRx));
    DMSG_INFO("USBC_REG_o_INTTxE        = %x\n", USBC_Readw(usbc_base + USBC_REG_o_INTTxE));
    DMSG_INFO("USBC_REG_o_INTRxE        = %x\n", USBC_Readw(usbc_base + USBC_REG_o_INTRxE));
    DMSG_INFO("USBC_REG_o_INTUSB        = %x\n", USBC_Readb(usbc_base + USBC_REG_o_INTUSB));
    DMSG_INFO("USBC_REG_o_INTUSBE       = %x\n", USBC_Readb(usbc_base + USBC_REG_o_INTUSBE));
    DMSG_INFO("USBC_REG_o_EPIND         = %x\n", USBC_Readw(usbc_base + USBC_REG_o_EPIND));
    DMSG_INFO("USBC_REG_o_TXMAXP        = %x\n", USBC_Readw(usbc_base + USBC_REG_o_TXMAXP));
    DMSG_INFO("USBC_REG_o_CSR0          = %x\n", USBC_Readw(usbc_base + USBC_REG_o_CSR0));
    DMSG_INFO("USBC_REG_o_TXCSR         = %x\n", USBC_Readw(usbc_base + USBC_REG_o_TXCSR));
    DMSG_INFO("USBC_REG_o_RXMAXP        = %x\n", USBC_Readw(usbc_base + USBC_REG_o_RXMAXP));
    DMSG_INFO("USBC_REG_o_RXCSR         = %x\n", USBC_Readw(usbc_base + USBC_REG_o_RXCSR));

    DMSG_INFO("USBC_REG_o_COUNT0        = %x\n", USBC_Readw(usbc_base + USBC_REG_o_COUNT0));
    DMSG_INFO("USBC_REG_o_RXCOUNT       = %x\n", USBC_Readw(usbc_base + USBC_REG_o_RXCOUNT));
    DMSG_INFO("USBC_REG_o_TXTYPE        = %x\n", USBC_Readb(usbc_base + USBC_REG_o_TXTYPE));
    DMSG_INFO("USBC_REG_o_NAKLIMIT0     = %x\n", USBC_Readb(usbc_base + USBC_REG_o_NAKLIMIT0));
    DMSG_INFO("USBC_REG_o_TXINTERVAL    = %x\n", USBC_Readb(usbc_base + USBC_REG_o_TXINTERVAL));
    DMSG_INFO("USBC_REG_o_RXTYPE        = %x\n", USBC_Readb(usbc_base + USBC_REG_o_RXTYPE));
    DMSG_INFO("USBC_REG_o_RXINTERVAL    = %x\n", USBC_Readb(usbc_base + USBC_REG_o_RXINTERVAL));
    DMSG_INFO("USBC_REG_o_CONFIGDATA    = %x\n", USBC_Readb(usbc_base + USBC_REG_o_CONFIGDATA));
    /*
        for(i = 0; i <= USBC_MAX_EP_NUM; i++){
        	DMSG_INFO("EPFIFOx(%d)              = %x\n", i, USBC_Readl(usbc_base + USBC_REG_o_EPFIFOx(i)));
    	}
    */
    DMSG_INFO("USBC_REG_o_DEVCTL        = %x\n", USBC_Readb(usbc_base + USBC_REG_o_DEVCTL));
    DMSG_INFO("USBC_REG_o_TXFIFOSZ      = %x\n", USBC_Readb(usbc_base + USBC_REG_o_TXFIFOSZ));
    DMSG_INFO("USBC_REG_o_RXFIFOSZ      = %x\n", USBC_Readb(usbc_base + USBC_REG_o_RXFIFOSZ));
    //    DMSG_INFO("USBC_REG_o_TXFIFOAD      = %x\n", USBC_Readw(usbc_base + USBC_REG_o_TXFIFOAD));
    //    DMSG_INFO("USBC_REG_o_RXFIFOAD      = %x\n", USBC_Readw(usbc_base + USBC_REG_o_RXFIFOAD));
    DMSG_INFO("USBC_REG_o_VEND0         = %x\n", USBC_Readb(usbc_base + USBC_REG_o_VEND0));
    DMSG_INFO("USBC_REG_o_VEND1         = %x\n", USBC_Readb(usbc_base + USBC_REG_o_VEND1));

    for(i = 0; i <= USBC_MAX_EP_NUM; i++)
    {
        DMSG_INFO("TXFADDRx(%d)             = %x\n", i, USBC_Readb(usbc_base + USBC_REG_o_TXFADDRx(i)));
        DMSG_INFO("TXHADDRx(%d              = %x\n", i, USBC_Readb(usbc_base + USBC_REG_o_TXHADDRx(i)));
        DMSG_INFO("TXHPORTx(%d)             = %x\n", i, USBC_Readb(usbc_base + USBC_REG_o_TXHPORTx(i)));
        DMSG_INFO("RXFADDRx(%d)             = %x\n", i, USBC_Readb(usbc_base + USBC_REG_o_RXFADDRx(i)));
        DMSG_INFO("RXHADDRx(%d)             = %x\n", i, USBC_Readb(usbc_base + USBC_REG_o_RXHADDRx(i)));
        DMSG_INFO("RXHPORTx(%d)             = %x\n", i, USBC_Readb(usbc_base + USBC_REG_o_RXHPORTx(i)));
    }

    for(i = 0; i <= USBC_MAX_EP_NUM; i++)
    {
        DMSG_INFO("RPCOUNTx(%d)             = %x\n", i, USBC_Readl(usbc_base + USBC_REG_o_RPCOUNTx(i)));
    }

    DMSG_INFO("USBC_REG_o_ISCR          = %x\n", USBC_Readl(usbc_base + USBC_REG_o_ISCR));
    DMSG_INFO("USBC_REG_o_PHYCTL        = %x\n", USBC_Readl(usbc_base + USBC_REG_o_PHYCTL));
    DMSG_INFO("USBC_REG_o_PHYBIST       = %x\n", USBC_Readl(usbc_base + USBC_REG_o_PHYBIST));

    if(ep_index != 0xff)
    {
        writew(old_ep_index, (usbc_base + USBC_REG_o_EPIND));
    }

    DMSG_INFO("---------------------------------------------------------------------------\n");
    DMSG_INFO("\n");
#endif
}

/*
********************************************************************************
*                              hcd_print_all_usb_reg
* Description:
*     打印所有USB寄存器的内容
*
* Arguments:
*     port		: USB端口号
*     ep_index	: 当前ep索引号
*     str       : 字符串
*
* Return value:
*     void
*
* note:
*     void
*
*********************************************************************************
*/
void hcd_print_all_usb_reg(u32 chip, __u32 port, u32 ep_index, char *str)
{
    switch(chip)
    {
    case HCD_DEBUG_CHIP_SUNI:
        hcd_print_all_usb_reg_suni(ep_index, str);
        break;

    case HCD_DEBUG_CHIP_SUNII:
        hcd_print_all_usb_reg_sunii(port, ep_index, str);
        break;

    default:
        DMSG_PANIC("ERR: unkown chip(%x)\n", chip);
    }

    return ;
}

/*
*****************************************************************************
*                     list_print_all_node
*
* Description:
*     打印队列中所有的成员
* Arguments:
*
* Returns:
*
* note:
*
*
*****************************************************************************
*/
void list_print_all_node(struct list_head  *list, char *str)
{
    struct list_head *list_now  = NULL;
    struct list_head *list_next = NULL;
    __cpu_sr sr = 0;

    if(list == NULL)
    {
        DMSG_PANIC("ERR: \"%s\", input == NULL\n", str);
        return ;
    }

    DMSG_INFO("\n-----start--------------------%s---------------------------\n", str);

    if(list_empty(list))
    {
        DMSG_PANIC("ERR: \"%s\", list is already empty\n", str);
        DMSG_INFO("------end---------------------%s-------------------------\n", str);
        return ;
    }

    USB_OS_ENTER_CRITICAL(sr);

    list_now  = list->next;
    list_next = NULL;

    //遇到链表头就退出
    while(list_now != list)
    {
        //--<1>--预先取得下一个node
        list_next = list_now->next;

        //--<2>--删除当前node, 并且释放其资源
        DMSG_INFO("list = %x, node  = %x, node_data = %x, node_list_next = %x\n",
                  list, list_now, list_now->data, list_next);

        //--<3>--取得下一个node
        list_now = list_next;
    }

    USB_OS_EXIT_CRITICAL(sr);

    DMSG_INFO("------end---------------------%s-------------------------\n", str);

    return ;
}

void print_ctrlrequest(struct usb_ctrlrequest *ctrlreq)
{
    DMSG_INFO("\n");
    DMSG_INFO("ctrlrequest:\n");
    DMSG_INFO("bRequestType = %x\n", ctrlreq->bRequestType);
    DMSG_INFO("bRequest     = %x\n", ctrlreq->bRequest);
    DMSG_INFO("wValue       = %x\n", ctrlreq->wValue);
    DMSG_INFO("wIndex       = %x\n", ctrlreq->wIndex);
    DMSG_INFO("wLength      = %x\n", ctrlreq->wLength);
    DMSG_INFO("\n");
}

#else

void hcd_print_all_usb_reg(u32 chip, __u32 port, u32 ep_index, char *str)
{
    return ;
}

void list_print_all_node(struct list_head  *list, char *str)
{
    return ;
}

void print_ctrlrequest(struct usb_ctrlrequest *ctrlreq)
{

}

#endif


#ifdef  USBH_HUB_DEBUG_GATE

void print_device_descriptor(struct usb_device_descriptor *descriptor)
{
    if(descriptor == NULL)
    {
        DMSG_PANIC("ERR: print_device_descriptor: input error\n");
        return;
    }

    DMSG_INFO("\n");
    DMSG_INFO("bLength              = %x\n", descriptor->bLength);
    DMSG_INFO("bDescriptorType      = %x\n", descriptor->bDescriptorType);
    DMSG_INFO("bcdUSB               = %x\n", descriptor->bcdUSB);
    DMSG_INFO("bDeviceClass         = %x\n", descriptor->bDeviceClass);
    DMSG_INFO("bDeviceSubClass      = %x\n", descriptor->bDeviceSubClass);
    DMSG_INFO("bDeviceProtocol      = %x\n", descriptor->bDeviceProtocol);
    DMSG_INFO("bMaxPacketSize0      = %x\n", descriptor->bMaxPacketSize0);
    DMSG_INFO("idVendor             = %x\n", descriptor->idVendor);
    DMSG_INFO("idProduct            = %x\n", descriptor->idProduct);
    DMSG_INFO("bcdDevice            = %x\n", descriptor->bcdDevice);
    DMSG_INFO("iManufacturer        = %x\n", descriptor->iManufacturer);
    DMSG_INFO("iProduct             = %x\n", descriptor->iProduct);
    DMSG_INFO("iSerialNumber        = %x\n", descriptor->iSerialNumber);
    DMSG_INFO("bNumConfigurations   = %x\n", descriptor->bNumConfigurations);
    DMSG_INFO("\n");

    return;
}

void print_config_descriptor(struct usb_config_descriptor *descriptor)
{
    if(descriptor == NULL)
    {
        DMSG_PANIC("ERR: print_config_descriptor: input error\n");
        return;
    }

    DMSG_INFO("\n");
    DMSG_INFO("bLength              = %x\n", descriptor->bLength);
    DMSG_INFO("bDescriptorType      = %x\n", descriptor->bDescriptorType);
    DMSG_INFO("wTotalLength         = %x\n", descriptor->wTotalLength);
    DMSG_INFO("bNumInterfaces       = %x\n", descriptor->bNumInterfaces);
    DMSG_INFO("bConfigurationValue  = %x\n", descriptor->bConfigurationValue);
    DMSG_INFO("iConfiguration       = %x\n", descriptor->iConfiguration);
    DMSG_INFO("bmAttributes         = %x\n", descriptor->bmAttributes);
    DMSG_INFO("bMaxPower            = %x\n", descriptor->bMaxPower);
    DMSG_INFO("\n");

    return;
}

void print_interface_descriptor(struct usb_interface_descriptor *descriptor)
{
    if(descriptor == NULL)
    {
        DMSG_PANIC("ERR: print_interface_descriptor: input error\n");
        return;
    }

    DMSG_INFO("\n");
    DMSG_INFO("bLength              = %x\n", descriptor->bLength);
    DMSG_INFO("bDescriptorType      = %x\n", descriptor->bDescriptorType);
    DMSG_INFO("bInterfaceNumber     = %x\n", descriptor->bInterfaceNumber);
    DMSG_INFO("bAlternateSetting    = %x\n", descriptor->bAlternateSetting);
    DMSG_INFO("bNumEndpoints        = %x\n", descriptor->bNumEndpoints);
    DMSG_INFO("bInterfaceClass      = %x\n", descriptor->bInterfaceClass);
    DMSG_INFO("bInterfaceSubClass   = %x\n", descriptor->bInterfaceSubClass);
    DMSG_INFO("bInterfaceProtocol   = %x\n", descriptor->bInterfaceProtocol);
    DMSG_INFO("iInterface           = %x\n", descriptor->iInterface);
    DMSG_INFO("\n");

    return;
}

void print_endpoint_descriptor(struct usb_endpoint_descriptor *descriptor)
{
    if(descriptor == NULL)
    {
        DMSG_PANIC("ERR: print_endpoint_descriptor: input error\n");
        return;
    }

    DMSG_INFO("\n");
    DMSG_INFO("bLength          = %x\n", descriptor->bLength);
    DMSG_INFO("bDescriptorType  = %x\n", descriptor->bDescriptorType);
    DMSG_INFO("bEndpointAddress = %x\n", descriptor->bEndpointAddress);
    DMSG_INFO("bmAttributes     = %x\n", descriptor->bmAttributes);
    DMSG_INFO("wMaxPacketSize   = %x\n", descriptor->wMaxPacketSize);
    DMSG_INFO("bInterval        = %x\n", descriptor->bInterval);
    DMSG_INFO("bRefresh         = %x\n", descriptor->bRefresh);
    DMSG_INFO("bSynchAddress    = %x\n", descriptor->bSynchAddress);
    DMSG_INFO("\n");

    return;
}

void print_all_interface_descriptor(struct usb_host_virt_interface *binterface)
{

    u32 i = 0;

    if(binterface == NULL)
    {
        DMSG_PANIC("ERR: print_all_interface_descriptor: input error\n");
        return;
    }

    print_interface_descriptor(&(binterface->desc));

    for(i = 0; i < binterface->desc.bNumEndpoints; i++)
    {
        struct usb_host_virt_endpoint *endpoint = binterface->endpoint + (i * sizeof(struct usb_host_virt_endpoint));

        if(endpoint)
        {
            DMSG_INFO("endpoint %d:\n", i);
            print_endpoint_descriptor(&(endpoint->desc));
        }
        else
        {
            DMSG_PANIC("ERR: endpoint is NULL\n");
        }
    }

    return ;
}

void print_all_config_descriptor(struct usb_host_virt_config *bconfig)
{

    u32 i = 0;

    if(bconfig == NULL)
    {
        DMSG_PANIC("ERR: print_all_interface_descriptor: input error\n");
        return;
    }

    print_config_descriptor(&(bconfig->desc));

    for(i = 0; i < bconfig->desc.bNumInterfaces; i++)
    {
        if(bconfig->intf_cache[i])
        {
            DMSG_INFO("device interface %d:\n", i);
            print_all_interface_descriptor(bconfig->intf_cache[i]->altsetting_array);
        }
        else
        {
            DMSG_PANIC("ERR: device interface is NULL\n");
        }
    }

    for(i = 0; i < bconfig->desc.bNumInterfaces; i++)
    {
        if(bconfig->interfac[i])
        {
            DMSG_INFO("select interface %d:\n", i);
            print_all_interface_descriptor(bconfig->interfac[i]->altsetting);
        }
        else
        {
            DMSG_PANIC("ERR: select interface is NULL\n");
        }
    }

    for(i = 0; i < bconfig->desc.bNumInterfaces; i++)
    {
        if(bconfig->interfac[i])
        {
            DMSG_INFO("current select interface %d:\n", i);
            print_all_interface_descriptor(bconfig->interfac[i]->cur_altsetting);
        }
        else
        {
            DMSG_PANIC("ERR: current select interface is NULL\n");
        }
    }

    return ;
}

void print_all_device_descriptor(struct usb_host_virt_dev *bdevice)
{
    u32 i = 0;

    if(bdevice == NULL)
    {
        DMSG_PANIC("ERR: print_all_interface_descriptor: input error\n");
        return;
    }

    print_device_descriptor(&(bdevice->descriptor));

    for(i = 0; i < bdevice->descriptor.bNumConfigurations; i++)
    {
        struct usb_host_virt_config *config = bdevice->config + (i * sizeof(struct usb_host_virt_config));

        if(config)
        {
            DMSG_INFO("Configuration %d:\n", i);
            print_all_config_descriptor(config);
        }
        else
        {
            DMSG_PANIC("ERR: Configuration is NULL\n");
        }
    }

    return ;
}

void print_id_table(const struct usb_drv_dev_match_table *id)
{
    DMSG_INFO("------------------id table---------------------\n");
    DMSG_INFO("match_flags         = %x\n", id->match_flags);

    DMSG_INFO("idVendor            = %x\n", id->idVendor);
    DMSG_INFO("idProduct           = %x\n", id->idProduct);
    DMSG_INFO("bcdDevice_lo        = %x\n", id->bcdDevice_lo);
    DMSG_INFO("bcdDevice_hi        = %x\n", id->bcdDevice_hi);

    DMSG_INFO("bDeviceClass        = %x\n", id->bDeviceClass);
    DMSG_INFO("bDeviceSubClass     = %x\n", id->bDeviceSubClass);
    DMSG_INFO("bDeviceProtocol     = %x\n", id->bDeviceProtocol);

    DMSG_INFO("bInterfaceClass     = %x\n", id->bInterfaceClass);
    DMSG_INFO("bInterfaceSubClass  = %x\n", id->bInterfaceSubClass);
    DMSG_INFO("bInterfaceProtocol  = %x\n", id->bInterfaceProtocol);

    DMSG_INFO("driver_info         = %x\n", id->driver_info);
    DMSG_INFO("-----------------------------------------------\n");
}

#else

void print_device_descriptor(struct usb_device_descriptor *descriptor)
{

}

void print_config_descriptor(struct usb_config_descriptor *descriptor)
{

}

void print_interface_descriptor(struct usb_interface_descriptor *descriptor)
{

}

void print_endpoint_descriptor(struct usb_endpoint_descriptor *descriptor)
{

}

void print_all_interface_descriptor(struct usb_host_virt_interface *binterface)
{
    return ;
}

void print_all_config_descriptor(struct usb_host_virt_config *bconfig)
{
    return ;
}

void print_all_device_descriptor(struct usb_host_virt_dev *bdevice)
{
    return ;
}

void print_id_table(const struct usb_drv_dev_match_table *id)
{

}

#endif









