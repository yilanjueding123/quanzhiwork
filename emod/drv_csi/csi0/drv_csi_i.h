/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          ir keyboard driver
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : drv_csi_i.h
* By      : jshwang
* Version : V1.00
*********************************************************************************************************
*/
#ifndef  __DRV_CSI_I_H__
#define  __DRV_CSI_I_H__


#include "epdk.h"
#include "csi_inc.h"

typedef struct tag_CSI_DRV
{
    __u32           mid;
    __u32           used;               //表示向drv注册了多少个设备节点
    __hdle          hReg_CsiDevHdl;     //hReg;   //csi_dev_hdl
    __dev_devop_t   csi_dev_entry;
} __csi_drv_t;


extern __s32   DRV_CSI_MInit(void);
extern __s32   DRV_CSI_MExit(void);
extern __mp   *DRV_CSI_MOpen(__u32 mid, __u32 mode);
extern __s32   DRV_CSI_MClose(__mp *mp);
extern __u32   DRV_CSI_MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
extern __u32   DRV_CSI_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
extern __s32   DRV_CSI_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);

#endif /*__DRV_CSI_I_H__*/


