/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                            rtc device module
*
*                                   (c) Copyright 2006-2010, kevin.z.m China
*                                             All Rights Reserved
*
* File    : drv_rtc.c
* By      : kevin.z
* Version : v1.00
* Date    : 2010-6-20 15:18
*********************************************************************************************************
*/
#include  "drv_rtc_i.h"


static __drv_rtc_t      drv_rtc;
extern __dev_devop_t    rtc_ops;



/*
*********************************************************************************************************
*                           RTC DRIVER MODULE INIT
*
*Description: rtc driver module init.
*
*Arguments  : none.
*
*Return     : result.
*********************************************************************************************************
*/
__s32 MInit(void)
{
    __inf("rtc driver init!\n");
    eLIBs_memset(&drv_rtc, 0, sizeof(__drv_rtc_t));
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                           RTC DRIVER MODULE EXIT
*
*Description: rtc driver module exit.
*
*Arguments  : none.
*
*Return     : result;
*********************************************************************************************************
*/
__s32 MExit(void)
{
    __inf("rtc driver exit!\n");
    eLIBs_memset(&drv_rtc, 0, sizeof(__drv_rtc_t));
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                           OPEN RTC DRIVER
*
*Description: open rtc driver.
*
*Arguments  : none.
*
*Return     : result;
*               0,  open rtc driver failed;
*              !0,  rtc driver handle.
*********************************************************************************************************
*/
__mp *MOpen(__u32 mid, __u32 mod)
{
    drv_rtc.mid = mid;
    return (__mp *)&drv_rtc;
}


/*
*********************************************************************************************************
*                           CLOSE RTC DRIVER
*
*Description: close rtc driver.
*
*Arguments  : None.
*
*Return     : result;
*               EPDK_OK,    close rtc driver successed;
*               EPDK_FAIL,  close rtc driver failed;
*********************************************************************************************************
*/
__s32 MClose(__mp *mp)
{
    if(drv_rtc.used || drv_rtc.devhdl)
    {
        __wrn("rtc driver is used by someone!");
        return EPDK_FAIL;
    }
    drv_rtc.mid = 0;
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                           RTC DRIVER MODULE READ
*
*Description: rtc driver module read, do nothing.
*
*Arguments  :
*
*Return     :
*
*Note       :
*********************************************************************************************************
*/
__u32 MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return 0;
}
/*
*********************************************************************************************************
*                           RTC DRIVER MODULE WRITE
*
*Description: rtc driver module write, do nothing.
*
*Arguments  :
*
*Return     :
*
*Note       :
*********************************************************************************************************
*/
__u32 MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return 0;
}

/*
*********************************************************************************************************
*                           RTC DRIVER MODULE IO CONTROL
*
*Description: rtc driver module IO control.
*
*Arguments  : mp        the handle of the dac driver module;
*             cmd       the command send to dac driver;
*             aux       user define;
*             pbuffer   user define;
*
*Return     :
*********************************************************************************************************
*/
__s32 MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    switch(cmd)
    {
    case DRV_CMD_PLUGIN:
    {
        rtc_Init();
        drv_rtc.devhdl = esDEV_DevReg(DEV_CLASS_HWSC, "rtc", &rtc_ops, 0);
        if(!drv_rtc.devhdl)
        {
            __err("Register rtc device failed!\n");
        }
        drv_rtc.used++;
        break;
    }

    case DRV_CMD_PLUGOUT:
    {
        rtc_Exit();
        esDEV_DevUnreg(drv_rtc.devhdl);
        drv_rtc.devhdl = 0;
        drv_rtc.used--;
        break;
    }

    case DRV_CMD_GET_STATUS:
    {
        return drv_rtc.used;
    }
    }

    return EPDK_OK;
}

