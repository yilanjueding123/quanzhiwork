/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                               hello world sample
*
*                                (c) Copyright 2006-2007, Steven.ZGJ China
*                                            All    Rights Reserved
*
* File    : drv_audio.h
* By      : kevin
* Version : V1.00
* Date    : 2009-4-6 18:34
*********************************************************************************************************
*/
#ifndef _DRV_STREAMING_H_
#define _DRV_STREAMING_H_

#include "typedef.h"
#include "cfgs.h"

typedef enum __STREAMING_DEV_CMD_SET
{
    STREAMING_DEV_CMD_NONE = 0,                  /* 空命令，不做响应                 */

    /*发送给streaming  manager dev的命令字*/
    STREAMING_DEV_CMD_NEWSTREAM = 0x100,         /* 注册一个新的streaming设备 */
    STREAMING_DEV_CMD_DELSTREAM,                 /* 删除streaming设备 */

    /*发送给streaming dev 的命令字*/
    STREAMING_DEV_FLUSH = 0x200,                 /* flush streaming */
    STREAMING_DEV_CMD_QUERY_BUFSIZE,             /* 查询用户缓冲区相关空间参数,aux = __streaming_dev_query_size_t   */
    STREAMING_DEV_CMD_QUERY_STATUS,             /* 查询设备状态: 返回__streaming_status_t*/
    STREAMING_DEV_CMD_SINK,                /*通知接收端写数据完毕*/
} __streaming_dev_cmd_set_t;

typedef enum __STREAMING_STATUS
{
    STREAMING_DEV_STATUS_TRANS = 0,             /* 数据传输状态，数据正常读出/写入*/
    STREAMING_DEV_STATUS_SINK,                  /* sink状态，写数据结束，读取时应当把剩余数据取完*/
} __streaming_status_t;


typedef enum __STREAMING_DEV_QUERY_BUF_SIZE_TYPE
{
    STREAMING_DEV_QUERY_BUF_SIZE_NONE = 0,      /* 未定义要获取空间的类型           */
    STREAMING_DEV_QUERY_BUF_SIZE_DATA,          /* 获取缓冲区内的数据size           */
    STREAMING_DEV_QUERY_BUF_SIZE_FREE,          /* 获取缓冲区内的空闲空间size       */
    STREAMING_DEV_QUERY_BUF_SIZE_TOTAL,         /* 获取缓冲区总空间                 */

    STREAMING_DEV_QUERY_BUF_SIZE_

} __streaming_dev_query_size_t;

#endif /* _DRV_STREAMING_H_ */

