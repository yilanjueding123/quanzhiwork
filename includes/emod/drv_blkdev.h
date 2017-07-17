/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : blkdev driver module
* File    : blkdev.h
*
* By      : Steven
* Version : v1.0
* Date    : 2008-9-12 8:50:21
**********************************************************************************************************************
*/



#ifndef _DRV_BLKDEV_H_
#define _DRV_BLKDEV_H_

#include "typedef.h"
#include "cfgs.h"


typedef enum __DRV_BLKDEV_CMD
{
    DRV_BLKDEV_CMD_GETCAP       = 0,
    DRV_BLKDEV_CMD_FLUSH,
    DRV_BLKDEV_CMD_GET_STATUS,

} __drv_blkdev_cmd_t;

#endif /* _DRV_BLKDEV_H_ */
