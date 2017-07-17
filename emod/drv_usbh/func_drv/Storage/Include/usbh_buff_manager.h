/*
********************************************************************************************************************
*                                              usb_host
*
*                              (c) Copyright 2007-2009, holi.China
*										All	Rights Reserved
*
* File Name 	: msc_buff_manager.h
*
* Author 		: javen
*
* Version 		: 2.0
*
* Date 			: 2009.08.19
*
* Description 	:
*
* History 		:
*     v1.0  holi  2008.11.22 - 读写速度快, 但是只支持单个lun
*     v2.0  javen 2009.08.19 - 支持多个lun 和 多种类型的设备
********************************************************************************************************************
*/
#ifndef	_USBH_BUFF_MANAGER_H_
#define	_USBH_BUFF_MANAGER_H_

#include "usb_host_base_types.h"


/* usb host 临时 buffer */
typedef struct tag_usbh_temp_buff
{
    u32 num;          	/* buffer在整个buffer管理里面的号码 	*/

    u32 dev_id;       	/* 设备id, 确定设备身份的唯一标识 		*/
    u32 start_lba;    	/* 起始扇区 							*/
    u32 end_lba;      	/* 结束扇区, 最后一个有效扇区 			*/
    u32 sector_size;  	/* 扇区大小 							*/

    u32 used_time;      /* 被文件系统使用的次数               	*/

    void *buff;       	/* 存放从设备读取的数据 				*/
    u32 buff_len;     	/* buffer的大小 						*/

    u32 is_valid;    	/* 这个buff的内容是否有效 				*/
    u32 is_busy;      	/* 防止buff正在接收device数据，但是
	                       又被usbh_buff_manager设置为无效 		*/
} usbh_temp_buff_t;


#define  USBH_TEMP_BUFFER_MAX_NUM    8   			//buffer的个数, 先分4个，不够用再加
#define  USBH_TEMP_BUFFER_MAX_LEN    (32 * 1024)	//32k, 必须是k的倍数, 因为要palloc

typedef struct tag_usbh_buff_manager
{
    u32 temp_buff_len;    /* 临时buff的大小, 它决定了usbh_temp_buff_t里buff_len的大小 */
    u32 temp_buff_nr;  	  /* 临时buff的个数 */

    usbh_temp_buff_t buff_array[USBH_TEMP_BUFFER_MAX_NUM];
} usbh_buff_manager_t;


//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
s32 usbh_msc_special_read(void *pBuffer,
                          __u32 blk,
                          __u32 n,
                          USB_OS_HANDLE hDev,
                          u32 dev_id,
                          u32 secter_size,
                          void *blk_read_entry);

s32 usbh_msc_special_write(void *pBuffer,
                           __u32 blk,
                           __u32 n,
                           USB_OS_HANDLE hDev,
                           u32 dev_id,
                           u32 secter_size,
                           void *blk_write_entry);

s32 set_usbh_temp_buff_invalid_by_dev(u32 dev_id);
s32 set_all_usbh_temp_buff_invalid(void);

s32 init_usbh_buff_manager(void);
s32 exit_usbh_buff_manager(void);

#endif





