/*
********************************************************************************
*                                    eMOD
*                   the Easy Portable/Player Develop Kits
*                               mod_duckweed sub-system
*                          (module name, e.g.mpeg4 decoder plug-in) module
*
*          (c) Copyright 2010-2012, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : dev_csi_i.h
* Version: V1.0
* By     : Eric_wang
* Date   : 2010-1-4
* Description:
********************************************************************************
*/
#ifndef _DEV_CSI_I_H_
#define _DEV_CSI_I_H_

#include "csi_inc.h"
#include "csi.h"
#include "csi_linklist_manager.h"


typedef struct STRUCT_CSI_DEV
{
    __u32           status;
    __u32						csi_channel;	//通道标志
    __krnl_event_t  *csi_lock;    ///对CSI_DEV操作进行互斥的semaphore
    ES_FILE         *iic_file;

    __hdle  csi_pin;
    __hdle  csi_ahbclk;
    __hdle  csi_sdramclk;
    __hdle  csi_outclk;


    //__u32 frame_id_lowest = 0;
    __s32   csi_frame_num;  //表示有效的csi_frame的个数, <= CSI_BUFFER_NUM
    __csi_frame_t csi_frame[CSI_BUFFER_NUM];
    __csi_frame_t *p_csi_frame[CSI_BUFFER_NUM];
    __s32 frame_id_last;//存放上一次index
    __s32 frame_id_last_b;//
    __csi_linklist_manager_t    *full2; //管理帧队列,分为装完的帧，和空闲的帧
    __csi_linklist_manager_t    *free2;

    __s64       pts;  //记录得到的当前的PTS
    __u32       pts_flag; //设置PTS的获取方式，aux = 0:CSI自己算PTS; 1:通过回调函数得到PTS; 2.不需要PTS
    __pCBK_t    CB_GetPts;

    __csi_mode_t csi_mode_last;//最后一次的配置，某些场合需要暂存下来

} __csi_dev_t;

#endif  /* _DEV_CSI_I_H_ */

