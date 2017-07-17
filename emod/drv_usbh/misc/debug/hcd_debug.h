/*
********************************************************************************************************************
*                                              USB Host Drvier
*
*                              (c) Copyright 2007-2009, softwinners.China
*										All	Rights Reserved
*
* File Name 	: hcd_debug.h
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
#ifndef  __HCD_DEBUG_H__
#define  __HCD_DEBUG_H__

#include "usb_host_base_types.h"

#include "usb_ch9.h"
#include "usb_host_common.h"

#include "usb_list.h"
#include "list_head_ext.h"


#define  HCD_DEBUG_CHIP_SUNI		0x01
#define  HCD_DEBUG_CHIP_SUNII		0x02

void hcd_print_all_usb_reg(u32 chip, __u32 port, u32 ep_index, char *str);
void list_print_all_node(struct list_head  *list, char *str);
void print_ctrlrequest(struct usb_ctrlrequest *ctrlreq);

void print_device_descriptor(struct usb_device_descriptor *descriptor);
void print_config_descriptor(struct usb_config_descriptor *descriptor);
void print_interface_descriptor(struct usb_interface_descriptor *descriptor);
void print_endpoint_descriptor(struct usb_endpoint_descriptor *descriptor);
void print_all_interface_descriptor(struct usb_host_virt_interface *binterface);
void print_all_config_descriptor(struct usb_host_virt_config *bconfig);
void print_all_device_descriptor(struct usb_host_virt_dev *bdevice);
void print_id_table(const struct usb_drv_dev_match_table *id);


#endif   //__HCD_DEBUG_H__


