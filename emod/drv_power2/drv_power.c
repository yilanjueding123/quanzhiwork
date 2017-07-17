/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                Power Driver
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : drv_power.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-4-30
* Descript: no pmu power dirver.
* Update  : date                auther      ver     notes
*           2011-4-30 9:49:31   Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "drv_power_i.h"
//#include "csi_init.h"

__power_drv_t           powerdrv;
__drv_power_fuelguage_t fuelguage;

__hdle hPowerOn = NULL;
__hdle hChargeStatus = NULL;
__hdle csi_chice0 = NULL;
__hdle csi_chice1 = NULL;
__hdle csi_chice2 = NULL;


extern __s32 g_adc_exp;

#if 1

#define __msg(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
							     eLIBs_printf(__VA_ARGS__)									        )
#else
#define __msg(...)
#endif


/*
*********************************************************************************************************
*                                       POWER DRIVER INITIALIZE
*
* Description: power dirver initialize.
*
* Arguments  : NONE
*
* Returns    : EPDK_OK if succeeded, EPDK_FAIL if failed.
*
* Note       :
*********************************************************************************************************
*/
__s32 DRV_PowerMInit(void)
{
    __s32           ret;
    user_gpio_set_t gpio_set;
    eLIBs_memset(&powerdrv, 0, sizeof(__power_drv_t));
    eLIBs_memset(&fuelguage, 0, sizeof(__drv_power_fuelguage_t));

	__msg("DRV_PowerMInit\n");
    // initialize global variable
    ret = esCFG_GetKeyValue("power_para", "power_on", (int *)&gpio_set, sizeof(user_gpio_set_t) / 4);
    if (ret)
    {
        __wrn("fetch para from script failed\n");
        goto err_out;
    }

    hPowerOn = esPINS_PinGrpReq(&gpio_set, 1);
    if (!hPowerOn)
    {
        __wrn("request power on pin failed\n");
        goto err_out;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//CSI_init_req();
#if 0	
    ret = esCFG_GetKeyValue("power_para", "csi_set0", (int *)&gpio_set, sizeof(user_gpio_set_t) / 4);
    if(ret)
    {
        goto err_out;
    }

 	csi_chice0 = esPINS_PinGrpReq(&gpio_set, 1);
    if(!csi_chice0)
        goto err_out;

    ret = esCFG_GetKeyValue("power_para", "csi_set1", (int *)&gpio_set, sizeof(user_gpio_set_t) / 4);
    if(ret)
    {
        goto err_out;
    }

    csi_chice1 = esPINS_PinGrpReq(&gpio_set, 1);
    if(!csi_chice1)
        goto err_out;


    ret = esCFG_GetKeyValue("power_para", "csi_set2", (int *)&gpio_set, sizeof(user_gpio_set_t) / 4);
    if(ret)
    {
        goto err_out;
    }

    csi_chice2 = esPINS_PinGrpReq(&gpio_set, 1);
    if(!csi_chice2)
        goto err_out;
#endif	
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
        //获取AD采样后的系数，对生产批次不同，系数也可能不同
        if(EPDK_OK == esCFG_GetKeyValue("power_para", "adc_exp", &g_adc_exp, 1))
        {
            __msg("***********g_adc_exp********=%d\n", g_adc_exp);
        }
        else
        {
            __msg("get key val fail...\n");
            g_adc_exp = 1027;
        }
        __msg("***********g_adc_exp********=%d\n", g_adc_exp);
    }

    {
        //如果是debug模式，则把PA4口给串口输入用
        __target_para_t target;
        esKSRV_GetTargetPara(&target);
        __msg("target.debugenable=%d\n", target.debugenable);
        if(target.debugenable == EPDK_TRUE)
        {
        }
        else
        {
#if 0
            ret = esCFG_GetKeyValue("power_para", "charge_status", (int *)&gpio_set, sizeof(user_gpio_set_t) / 4);
            if (ret)
            {
                __wrn("fetch para from script failed\n");
                goto err_out;
            }

            hChargeStatus = esPINS_PinGrpReq(&gpio_set, 1);
            if (!hChargeStatus)
            {
                __wrn("request charge status pin failed\n");
                goto err_out;
            }
#endif
        }
    }

    return EPDK_OK;

err_out:
    if (hPowerOn)
    {
        esPINS_PinGrpRel(hPowerOn, 0);
        hPowerOn = NULL;
    }

    if (hChargeStatus)
    {
        esPINS_PinGrpRel(hChargeStatus, 0);
        hChargeStatus = NULL;
    }

    return EPDK_FAIL;
}

/*
*********************************************************************************************************
*                                       POWER DRIVER EXIT
*
* Description: power dirver exit.
*
* Arguments  : NONE
*
* Returns    : EPDK_OK if succeeded, EPDK_FAIL if failed.
*
* Note       :
*********************************************************************************************************
*/
__s32 DRV_PowerMExit(void)
{
    if (hPowerOn)
    {
        esPINS_PinGrpRel(hPowerOn, 0);
        hPowerOn = NULL;
    }

    if (hChargeStatus)
    {
        esPINS_PinGrpRel(hChargeStatus, 0);
        hChargeStatus = NULL;
    }

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       POWER DRIVER OPEN
*
* Description: power dirver open.
*
* Arguments  : NONE
*
* Returns    : handle of dirver.
*
* Note       :
*********************************************************************************************************
*/
__mp *DRV_PowerMOpen(__u32 mid, __u32 mod)
{
    powerdrv.mid = mid;
    powerdrv.used++;

    return (__mp *)&powerdrv;
}

/*
*********************************************************************************************************
*                                       POWER DRIVER CLOSE
*
* Description: power dirver close.
*
* Arguments  : handler of driver, returned by driver open.
*
* Returns    : EPDK_OK if succeeded, EPDK_FAIL if failed.
*
* Note       :
*********************************************************************************************************
*/
__s32 DRV_PowerMClose(__mp *mp)
{
    if(powerdrv.used)
    {
        powerdrv.used--;
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                       POWER DRIVER READ
*
* Description: power dirver read.
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__u32 DRV_PowerMRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return 0;
}

/*
*********************************************************************************************************
*                                       POWER DRIVER WRITE
*
* Description: power dirver write.
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__u32 DRV_PowerMWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return 0;
}

/*
*********************************************************************************************************
*                                       POWER DRIVER IOCTL
*
* Description: power dirver io control.
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__s32 DRV_PowerMIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    switch(cmd)
    {
    case DRV_CMD_PLUGIN:
    {
        powerdrv.hReg = esDEV_DevReg("HID", "POWER", &power_dev_ops, 0);
        if(powerdrv.hReg == NULL)
        {
            __wrn("reg power device failed!\n");
            return EPDK_FAIL;
        }

        //create power task timer
        powerdrv.hTimer = esKRNL_TmrCreate(POWER_TIME_PERIOD,
                                           OS_TMR_OPT_PRIO_LOW | OS_TMR_OPT_PERIODIC,
                                           (OS_TMR_CALLBACK)PowerTask, &fuelguage);
        if (powerdrv.hTimer == NULL)
        {
            __wrn("create power timer failed\n");
            esDEV_DevUnreg(powerdrv.hReg);
            return EPDK_FAIL;
        }

        //start power task timer
        if(esKRNL_TmrStart(powerdrv.hTimer) !=  EPDK_TRUE)
        {
            __wrn("power task timer start fail!\n");
            esKRNL_TmrDel(powerdrv.hTimer);
            esDEV_DevUnreg(powerdrv.hReg);
            return EPDK_FAIL;
        }

        break;
    }

    case DRV_CMD_PLUGOUT:
    {
        //stop power task timer
        if (powerdrv.hTimer)
        {
            esKRNL_TmrStop(powerdrv.hTimer, OS_TMR_OPT_NONE, NULL);
        }

        //delete power task timer
        if (powerdrv.hTimer)
        {
            esKRNL_TmrDel(powerdrv.hTimer);
        }
        if (powerdrv.hReg)
        {
            esDEV_DevUnreg(powerdrv.hReg);
        }
        break;
    }

    case DRV_CMD_GET_STATUS:
    {
        if(powerdrv.used)
        {
            return DRV_STA_BUSY;
        }
        else
        {
            return DRV_STA_FREE;
        }
    }
    default:
    {
        __wrn("invalid command for power driver\n");
        return EPDK_FAIL;
    }
    }

    return EPDK_OK;
}
