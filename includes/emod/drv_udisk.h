/*
********************************************************************************************************************
*                                              usbd udisk
*
*                              (c) Copyright 2007-2009, javen.China
*										All	Rights Reserved
*
* File Name 	: drv_udisk.h
*
* Author 		: javen
*
* Version 		: 1.0
*
* Date 			: 2009.06.08
*
* Description 	:
*
* History 		:
*
********************************************************************************************************************
*/

#ifndef    __DRV_UDISK_H__
#define    __DRV_UDISK_H__

#define    USBD_UDISK_CMD_IDLE          0x00
#define    USBD_UDISK_CMD_GET_STATUS    0x01

//u盘所处的状态
typedef enum tag_udisk_status
{
    UDISK_STATUS_IDLE = 0,    //默认
    UDISK_STATUS_INSERT,      //插入
    UDISK_STATUS_REMOVE,      //拔除
    UDISK_STATUS_CONNECT,     //处于连接状态
    UDISK_STATUS_READ,        //读
    UDISK_STATUS_WRITE        //写
} udisk_status_t;

/* u盘的信息 */
typedef struct tag_udisk_info
{
    udisk_status_t udisk_status;
} udisk_info_t;


#endif     //__DRV_UDISK_H__
