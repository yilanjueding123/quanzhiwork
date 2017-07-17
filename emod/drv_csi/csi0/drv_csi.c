/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           hello world sample
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : drv_csi.c
* By      : jshwang
* Version : V1.00
*********************************************************************************************************
*/

#include "drv_csi_i.h"

extern __s32 CSI_DEV_NODE_Init_Part1(void);  //CSI_DEV_NODE的初始化的第一部分
extern __s32 CSI_DEV_NODE_Init_Part2(void);
extern __s32 CSI_DEV_NODE_Exit(void);

extern __hdle DEV_CSI_Open(void *open_arg, __u32 mode);
extern __s32 DEV_CSI_Close(__hdle hcsi);
extern __u32 DEV_CSI_Read(void *pdata, __u32 size, __u32 n, __hdle hPower);
extern __u32 DEV_CSI_Write(const void *pdata, __u32 size, __u32 n, __hdle hPower);
extern __s32 DEV_CSI_Ioctrl(__hdle hPower, __u32 cmd, __s32 aux, void *pbuffer);

static 	__csi_drv_t       csi_drv;
//__krnl_event_t    *csi_lock;


/*
****************************************************************************************************
*
*             DRV_CSI_MInit
*
*  Description:
*       DRV_CSI_MInit
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/

__s32 DRV_CSI_MInit(void)
{
    __inf("DRV_CSI_MInit,%d\n", CSI_PORT);

    //csi_lock = esKRNL_SemCreate(1);
    //CSI设备节点的初始化，此时这些设备均未注册
    csi_memset(&csi_drv, 0, sizeof(__csi_drv_t));
    if(EPDK_OK == CSI_DEV_NODE_Init_Part1())
    {
        csi_drv.csi_dev_entry.Open  = DEV_CSI_Open;
        csi_drv.csi_dev_entry.Close = DEV_CSI_Close;
        csi_drv.csi_dev_entry.Read  = DEV_CSI_Read;
        csi_drv.csi_dev_entry.Write = DEV_CSI_Write;
        csi_drv.csi_dev_entry.Ioctl = DEV_CSI_Ioctrl;
    }
    else
    {
        goto _err0;
    }
    csi_drv.used = 0;
    // esMEM_RequestDramUsrMode(DRAM_USER_MODE_ENCODER);

    return EPDK_OK;

_err0:
    CSI_DEV_NODE_Exit();
    csi_memset(&csi_drv, 0, sizeof(__csi_drv_t));
    return EPDK_FAIL;
}
/*
****************************************************************************************************
*
*             DRV_CSI_MExit
*
*  Description:
*       DRV_CSI_MExit
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 DRV_CSI_MExit(void)
{
    __inf("DRV_CSI_MExit,%d\n", CSI_PORT);
    //__u8 err;

    //esKRNL_SemDel(csi_lock, OS_DEL_ALWAYS, &err);
    CSI_DEV_NODE_Exit();
    csi_memset(&csi_drv, 0, sizeof(__csi_drv_t));
    //esMEM_ReleaseDramUsrMode(DRAM_USER_MODE_ENCODER);

    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             DRV_CSI_MOpen
*
*  Description:
*       DRV_CSI_MOpen
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__mp *DRV_CSI_MOpen(__u32 mid, __u32 mod)
{
    __inf("DRV_CSI_MOpen,%d\n", CSI_PORT);
    csi_drv.mid = mid;

    return (__mp *)&csi_drv;
}

/*
****************************************************************************************************
*
*             DRV_CSI_MClose
*
*  Description:
*       DRV_CSI_MClose
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 DRV_CSI_MClose(__mp *mp)
{
    __inf("DRV_CSI_MClose,%d\n", CSI_PORT);

    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             DRV_CSI_MRead
*
*  Description:
*       DRV_CSI_MRead
*
*  Parameters:
*
*  Return value:
*       size*n
*
****************************************************************************************************
*/
__u32 DRV_CSI_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             DRV_CSI_MWrite
*
*  Description:
*       DRV_CSI_MWrite
*
*  Parameters:
*
*  Return value:
*       size*n
*
****************************************************************************************************
*/
__u32 DRV_CSI_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             DRV_CSI_MIoctrl
*
*  Description:
*       DRV_CSI_MIoctrl
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 DRV_CSI_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    __s32   ret;

    //__inf("DRV_CSI_MIoctrl\n");
    switch(cmd)
    {
    case DRV_CMD_PLUGIN:
    {
        __inf("DRV_CMD_PLUGIN,%d\n", CSI_PORT);
        if(csi_drv.used)
        {
            __inf("csi already used,%d\n", CSI_PORT);
            return EPDK_FAIL;
        }
        csi_drv.hReg_CsiDevHdl = esDEV_DevReg("USER", (CSI_PORT == 0) ? "CSI" : "CSI1", &csi_drv.csi_dev_entry, 0);
        if(!csi_drv.hReg_CsiDevHdl)
        {
            __wrn("user csi registered Error!,%d\n", CSI_PORT);
            return EPDK_FAIL;
        }
        __inf("csi drv reg ok,%d\n", CSI_PORT);
        //            csi_drv.csi_dev.iic_file = csi_fopen("b:\\BUS\\IIC1", "r+");//使用twi1,公板使用的是"b:\\BUS\\IIC"
        //          	if(!csi_drv.csi_dev.iic_file)
        //          	{
        //          		__wrn("Open IIC Dev File Failed\n");
        //          		return EPDK_FAIL;
        //          	}
        //add your code here
        //			csi_clk_init();
        ret = CSI_DEV_NODE_Init_Part2();
        if(ret != EPDK_OK)
        {
            return ret;
        }
        csi_drv.used = 1;
        __inf("csi drv plugin ok,%d\n", CSI_PORT);
        return EPDK_OK;
    }

    case DRV_CMD_PLUGOUT:
    {
        __inf("csi DRV_CMD_PLUGOUT,%d\n", CSI_PORT);
        if(csi_drv.used == 1)
        {
            if(!csi_drv.hReg_CsiDevHdl)
            {
                __wrn("Dev not plugin!,%d\n", CSI_PORT);
                return EPDK_FAIL;
            }
            //                if(csi_drv.csi_dev.iic_file)
            //                {
            //                    csi_fopen(csi_drv.csi_dev.iic_file);
            //                }
            esDEV_DevUnreg(csi_drv.hReg_CsiDevHdl);
            csi_drv.hReg_CsiDevHdl = NULL;
            CSI_DEV_NODE_Exit();
        }
        //add your code here
        //            csi_clk_exit();
        csi_drv.used = 0;
        return EPDK_OK;
    }

    case DRV_CMD_GET_STATUS:
    {
        __inf("DRV_CMD_GET_STATUS,%d\n", CSI_PORT);
        if(csi_drv.used)
        {
            return DRV_STA_BUSY;
        }
        else
        {
            return DRV_STA_FREE;
        }
    }
    default:
        return EPDK_FAIL;
    }
}

