/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                               ir key driver
*
*                                (c) Copyright 2006-2007, Steven.ZGJ China
*                                            All    Rights Reserved
*
* File    : DEV_Power.c
* By      : Steven.ZGJ
* Version : V1.00
*********************************************************************************************************
*/

#include "drv_power_i.h"

extern __power_drv_t            powerdrv;
extern __drv_power_fuelguage_t  fuelguage;

//以下定义用于连USB关机后开机的检测
static __u32 g_pw_on_pin = 0;
static __krnl_stmr_t *g_pw_on_timer_hdl = NULL;
static __u32 g_pw_on_period = 1000;//2秒长按则开机(减去1秒，看门狗没复位到系统重启还要一点时间)
static __u32 g_pw_on_check_period = 50;//50毫秒检测一次

static __u8     dc2set = 1, ldo1set = 1;
static __u16    Vldo1_set = 1250;
static __u16    Vldo2_set = 3000;
static __u16    Vldo3_set = 1800;
static __u16    Vldo4_set = 3300;
static __u16    Vldo5_set = 3300;
static __u16    Vdcdc1_set = 3300;
static __u16    Vdcdc2_set = 1240;
static __u16    Vdcdc3_set = 2500;

//以下定义为watch dog设定服务
#define TMRC_MEM_BASE   (0XF1C20C00)
#define  __TMRC_REG(ofst)       *(volatile unsigned *)(TMRC_MEM_BASE + (ofst))
#define  TMRC_o_WACHDOG_CTL_REG        (0X30)  //TIMER2(watch-dog)  control REG     REG
#define  TMRC_o_WACHDOG_INTVAL_REG     (0X34)  //TIMER2(watch-dog)  interval REG    REG
#define  TMRC_WACHDOG_CTL_REG        __TMRC_REG(TMRC_o_WACHDOG_CTL_REG   )  //TIMER2(watch-dog)  control REG     REG
#define  TMRC_WACHDOG_INTVAL_REG     __TMRC_REG(TMRC_o_WACHDOG_INTVAL_REG)  //TIMER2(watch-dog)  interval REG    REG
#if 1 //watch-dog control
#define WATCH_DOG_EN_BIT            0U
#define WATCH_DOG_REST_VALID_BIT    1U
#define WATCH_DOG_CLK_SRC_SEL_BIT   3U
#endif // watch-dog control

/*
 * 设置狗的周期，单位是ms，如果要设置周期为4s,则参数为4*1000
*/
static void set_watchDog(__u32 nMilliSec)
{
    __u32 regVal = 0;

    TMRC_WACHDOG_CTL_REG &= ~(1U << 0); //To stop the watch dog first

    regVal = TMRC_WACHDOG_CTL_REG;
    regVal |= 1U << WATCH_DOG_CLK_SRC_SEL_BIT;
    regVal |= 1U << WATCH_DOG_REST_VALID_BIT;
    TMRC_WACHDOG_CTL_REG = regVal;

    TMRC_WACHDOG_INTVAL_REG = 24 * 1000 * nMilliSec;

    TMRC_WACHDOG_CTL_REG |= 1U << 0; //To Enable the watch dog
    __inf("Enable watch dog!\n");

    return;
}


/*
****************************************************************************************************
*
*             DEV_KEY_Open
*
*  Description:
*       DRV_MOpen
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__hdle DEV_PowerOpen(void *open_arg, __u32 mode)
{
    return (__hdle) & (powerdrv.PowerDev);
}


/*
****************************************************************************************************
*
*             DEV_KEY_Close
*
*  Description:
*       DRV_irkey_MClose
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 DEV_PowerClose(__hdle hPower)
{
    return EPDK_OK;
}


/*
****************************************************************************************************
*
*             DEV_KEY_Read
*
*  Description:
*       DRV_irkey_MRead
*
*  Parameters:
*
*  Return value:
*       size*n
*
****************************************************************************************************
*/
__u32 DEV_PowerRead(void *pdata, __u32 size, __u32 n, __hdle hPower)
{
    return 0;
}


/*
****************************************************************************************************
*
*             DEV_KEY_Write
*
*  Description:
*       DRV_irkey_MWrite
*
*  Parameters:
*
*  Return value:
*       size*n
*
****************************************************************************************************
*/
__u32 DEV_PowerWrite(const void *pdata, __u32 size, __u32 n, __hdle hPower)
{
    return size * n;
}

//开机流程
static void power_on_proc(void)
{
    __here__;

    set_watchDog(1);

    __here__;
    while(1)//等待直到系统复位
    {
        ;
    }
}

//定时检测pw-off口
static void power_on_timmer_proc(void *arg)
{
    static __s32    pw_off_hight_cnt = 0;
    __s32 d1;

    d1 = esPINS_ReadPinData(g_pw_on_pin, 0);
    __msg("d1=%d\n", d1);
    if(d1)
    {
        pw_off_hight_cnt++;
    }
    else
    {
        pw_off_hight_cnt = 0;
    }

    if(pw_off_hight_cnt > g_pw_on_period / g_pw_on_check_period)
    {
        if (esKRNL_TmrStateGet(g_pw_on_timer_hdl) == OS_TMR_STATE_RUNNING)
        {
            esKRNL_TmrStop(g_pw_on_timer_hdl, OS_TMR_OPT_NONE, NULL);
        }

        esPINS_PinGrpRel(g_pw_on_pin, 0);

        power_on_proc();
    }
}


static __s32 __power_decr_freq(void)
{
    __s32 ret;
    __u32 tmp_cpu_clk = 0;
    __u32 tmp_sdram_clk = 0;
    ES_FILE            *hMonitor = NULL;
    __u8                CurrentTID;
    __u8                err;

    //notify device monitor enter standby
    hMonitor = eLIBs_fopen("b:\\HWSC\\monitor", "rb+");
    if(hMonitor)
    {
        eLIBs_fioctrl(hMonitor, DEV_IOC_USR_HWSC_DISABLE_MONITOR, 0, NULL);
    }
    else
    {
        __wrn("Open usb moniter failed\n");
    }


    //CurrentTID = esKRNL_GetTIDCur();
    //err = esKRNL_TaskChangePrio(CurrentTID, OS_TASK_STANDBY_PRIO);
    //if (err != OS_NO_ERR)
    //{
    //    __wrn("change current thread priority to OS_TASK_STANDBY_PRIO failed\n");
    //}
    //else
    //{
    //    __inf("change current thread priority[%x] to OS_TASK_STANDBY_PRIO[%x] succeeded\n",
    //          CurrentTID, OS_TASK_STANDBY_PRIO);
    //}

    //esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_CORE_PLL, 6*1000*1000);
    //esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_SDRAM_PLL, 1*1000*1000);

    //esPWM_LockCpuFreq();
    __msg("esPWM_LockCpuFreq called\n");

    return EPDK_OK;
}

/*static void _close_lcd(void)
{
	ES_FILE *disp;
	__u32 outputtype;
	__u32 	arg[3];

	disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
	if(!disp)
	{
		__wrn("open display fail\n");
		return ;
	}

	outputtype = eLIBs_fioctrl(disp,DISP_CMD_GET_OUTPUT_TYPE,0,(void *)arg);
	if(outputtype == DISP_OUTPUT_TYPE_LCD)
	{
		arg[0] = 0;
		arg[1] = 0;
		arg[2] = 0;
		eLIBs_fioctrl(disp,DISP_CMD_LCD_OFF,0,(void *)arg);
	}

	eLIBs_fclose(disp);
}
*/


//解决在连USB或电源时，长按pw-off关机后，长按pw-on开机
static __s32 DEV_PowerOnProcess(void)
{
    __s32 ret;
    user_gpio_set_t  gpio_set[1];

    __here__;

    // 申请pin
    eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
    ret = esCFG_GetKeyValue("matrixkey_para", "pw_off", (int *)gpio_set, sizeof(user_gpio_set_t) / 4);
    if (!ret)
    {
        g_pw_on_pin = esPINS_PinGrpReq(gpio_set, 1);
        if (!g_pw_on_pin)
        {
            __wrn("request Power Off pin failed\n");
            return EPDK_FAIL;
        }
    }
    else
    {
        __wrn("fetch para from script failed\n");
        return EPDK_FAIL;
    }

    __here__;



    //申请定时器
    g_pw_on_timer_hdl = esKRNL_TmrCreate(
                            50,
                            OS_TMR_OPT_PRIO_HIGH | OS_TMR_OPT_PERIODIC,
                            (OS_TMR_CALLBACK)power_on_timmer_proc,
                            NULL);
    if (!g_pw_on_timer_hdl)
    {
        __wrn("create timer failed\n");
        return EPDK_FAIL;
    }
    __here__;
    __here__;
    __here__;
    __here__;
    __here__;

    //   _close_lcd();

    //让定时器开始跑
    if (esKRNL_TmrStateGet(g_pw_on_timer_hdl) == OS_TMR_STATE_STOPPED)
    {
        esKRNL_TmrStart(g_pw_on_timer_hdl);
    }

    __power_decr_freq();
    __here__;

    return EPDK_OK;
}

/*
****************************************************************************************************
*
*             DEV_KEY_Ioctrl
*
*  Description:
*       DRV_irkey_MIoctrl
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 DEV_PowerIoctrl(__hdle hPower, __u32 cmd, __s32 aux, void *pbuffer)
{
    __s32 ret = EPDK_OK;
    switch(cmd)
    {
    case DRV_POWER_CMD_READ_REG:
    {
        __inf("drvpower DRV_POWER_CMD_READ_REG start\n");
        ret = 0xff;
        break;
    }

    case DRV_POWER_CMD_WRITE_REG:
    {
        __inf("drvpower DRV_POWER_CMD_WRITE_REG start\n");

        break;
    }

    case DRV_POWER_CMD_POWER_ON:
    {
        break;
    }

    case DRV_POWER_CMD_POWER_OFF:
    {
        __s32           cpu_sr;
        volatile __u32 *tmpAddr;
        __u32           tmpData;

        eLIBs_printf("power off enter....\n");

        if (hPowerOn)
        {
            eLIBs_printf("pull down poweron\n");
            esPINS_WritePinData(hPowerOn, 0, NULL);
        }

        {
            ES_FILE            *hMonitor = NULL;

            //notify device monitor enter standby
            hMonitor = eLIBs_fopen("b:\\HWSC\\monitor", "rb+");
            if(hMonitor)
            {
                //eLIBs_fioctrl(hMonitor, DEV_IOC_USR_HWSC_DISABLE_MONITOR, 0, NULL);
                eLIBs_fclose(hMonitor);
                hMonitor = NULL;
            }
            else
            {
                __wrn("Open usb moniter failed\n");
            }
        }

        //正常状况下,系统已关机,下面的代码是不会执行到,
        //为了防止插USB或充电器时无法关机的问题,
        //直接将CPU挂死在死循环内.

        //关掉watch-dog,防止系统复位
        tmpAddr = TMRC_WACHDOG_CTL_ADDR;
        tmpData = *tmpAddr;
        tmpData &= ~(1 << TMRC_WATCH_DOG_EN_BIT);
        *tmpAddr = tmpData;

        //关掉中断
        //ENTER_CRITICAL(cpu_sr);//中断不关，否则定时器不起作用

        //防止编译器warning
        //cpu_sr = cpu_sr;

        //死循环,挂死CPU
        eLIBs_printf("power no use, cpu dumping\n");

        DEV_PowerOnProcess();


        while (1)
        {
            ;
        }
        //unreached
        //break;
    }

    case DRV_POWER_CMD_INT_ENABLE:
    {
        ret = EPDK_OK;
        break;
    }

    case DRV_POWER_CMD_INT_DISABLE:
    {
        ret = EPDK_OK;
        break;
    }

    case DRV_POWER_CMD_INT_QUERY:
    {
        ret = EPDK_OK;
        break;
    }

    case DRV_POWER_CMD_INT_CLEAR:
    {
        ret = EPDK_OK;
        break;
    }

    case DRV_POWER_CMD_VOUT_GET:
    {
        __inf("drvpower DRV_POWER_CMD_VOUT_GET start\n");

        if(aux == POWER_VOL_LDO1)
        {
            if(ldo1set)
            {
                ret = Vldo1_set;
            }
            else
            {
                ret = Vldo1_set;
            }
        }
        else if((aux == POWER_VOL_DCDC1) || (aux == POWER_VOL_DCDC2))
        {
            if(aux == POWER_VOL_DCDC1)
            {
                ret = Vdcdc1_set;
            }
            else
            {
                if(dc2set)
                {
                    __inf("drvpower DRV_POWER_CMD_VOUT_GET dc2set\n");
                    ret =  Vdcdc2_set;
                }
                else
                {
                    ret = Vdcdc2_set;
                }
            }
        }
        else if((aux == POWER_VOL_LDO2) || (aux == POWER_VOL_LDO3) || (aux == POWER_VOL_DCDC3))
        {
            if(aux == POWER_VOL_LDO2)
            {
                ret = Vldo2_set;
            }
            else if(aux == POWER_VOL_DCDC3)
            {
                ret = Vdcdc3_set;
            }
            else
            {
                ret = Vldo3_set ;
            }
        }
        else if(aux == POWER_VOL_LDO4)
        {
            if(aux == POWER_VOL_LDO4)
            {
                ret = Vldo4_set ;
            }
            else
            {
                ret = Vldo5_set ;
            }
        }
        __inf("drvpower DRV_POWER_CMD_VOUT_GET dc2 return %d\n", ret);

        break;
    }

    case DRV_POWER_CMD_VOUT_SET:
    {
        __u32       tmp;

        tmp = (__u32)pbuffer;
        if((aux == POWER_VOL_DCDC1) || (aux == POWER_VOL_DCDC2))
        {
            if(aux == POWER_VOL_DCDC1)
            {
                Vdcdc1_set = tmp;
            }
            else
            {
                Vdcdc2_set = tmp;
            }
        }
        else if(aux == POWER_VOL_LDO1)
        {
            __wrn("LDO1 Voltage  cant be set!\n");
            Vldo1_set = tmp;
        }
        else if((aux == POWER_VOL_LDO2) || (aux == POWER_VOL_LDO3) || (aux == POWER_VOL_DCDC3))
        {
            if(aux == POWER_VOL_DCDC3)
            {
                Vdcdc3_set = tmp;
            }
            else if(aux == POWER_VOL_LDO2)
            {
                Vldo2_set = tmp;
            }
            else
            {
                Vldo3_set = tmp;
            }
        }
        else if(aux == POWER_VOL_LDO4)
        {
            if(aux == POWER_VOL_LDO4)
            {
                Vldo4_set = tmp;
            }
            else
            {
                Vldo5_set = tmp;
            }
        }
        ret = EPDK_OK;

        break;
    }
    case DRV_POWER_CMD_VSTS_GET:
    {
        break;
    }

    case DRV_POWER_CMD_VSTS_SET:
    {
        ret = EPDK_OK;
        break;
    }

    case DRV_POWER_CMD_PWREPT_GET:
    {
        break;
    }
    case DRV_POWER_CMD_PWREPT_SET:
    {
        break;
    }

    case DRV_POWER_CMD_BATADC_GET:
    {
        __drv_power_battery_status_t1   *bat_sts = (__drv_power_battery_status_t1 *)pbuffer;

        bat_sts->bat_vol = 2500;
        ret = EPDK_OK;
        break;
    }

    case DRV_POWER_CMD_BATADC_SET:
    {
        ret = EPDK_OK;
        break;
    }

    case DRV_POWER_CMD_GET_BAT_RDC:
    {
        ret = 500;
        break;
    }

    case DRV_POWER_CMD_BATSTATUS_TABLE_GET:
    {
        __drv_power_battery_status_t2  *bat_sts = (__drv_power_battery_status_t2 *)pbuffer;

        bat_sts->battery_exist = 0x0;
        bat_sts->dcin_valid = 0x01;
        ret = EPDK_OK;
        break;
    }

    case DRV_POWER_CMD_DCINADC_GET:
    {
        __drv_power_dcin_status_t   *dcin_sts = (__drv_power_dcin_status_t *)pbuffer;

        dcin_sts->dcin_exist = 0x01;
        ret = EPDK_OK;
        break;
    }

    case DRV_POWER_CMD_DCINADC_SET:
    {
        ret = EPDK_OK;
        break;
    }

    case DRV_POWER_CMD_CHARGE_GET:
    {
        ret = EPDK_OK;
        break;
    }

    case DRV_POWER_CMD_CHARGE_SET:
    {
        ret = EPDK_OK;
        break;
    }

    case DRV_POWER_CMD_BASIC_STATUS:
    {
        __drv_power_basic_status_t  *pbasic = (__drv_power_basic_status_t *)pbuffer;

        pbasic->bat_exist   = 0;
        pbasic->bat_charge_status = 0;
        ret = EPDK_OK;
        break;
    }

    case DRV_POWER_CMD_IPS_GET:
    {
        ret = EPDK_OK;
        break;
    }

    case DRV_POWER_CMD_IPS_SET:
    {
        ret = EPDK_OK;
        break;
    }
    case DRV_POWER_CMD_GET_DCIN:
    {
        __drv_power_battery_status_t3 *pstatus = (__drv_power_battery_status_t3 *)pbuffer;
        if (hChargeStatus)
        {
            pstatus->charge_status = esPINS_ReadPinData(hChargeStatus, NULL);
            __inf("charge status is %d\n", pstatus->charge_status);
            return EPDK_OK;
        }
        else
        {
            __wrn("request charge status pin failed\n");
            pstatus->charge_status = 0x0;
            return EPDK_FAIL;
        }
    }
    case DRV_POWER_CMD_GET_FUELGUAGE:
    {
        __drv_power_fuelguage_t *pbat = (__drv_power_fuelguage_t *)pbuffer;
        eLIBs_memcpy(pbat, &fuelguage, sizeof(__drv_power_fuelguage_t));
        ret = EPDK_OK;
        break;
    }
    case DRV_POWER_CMD_SET_USB_CHARGE:
    {
        ret = EPDK_OK;
        break;
    }
    case DRV_POWER_CMD_SET_STANDBY_FLAG:
    {
        __inf("set sdandby to  1 \n");
        break;
    }
    default:
    {
        __wrn("Unkonwn Command...\n");
        ret = EPDK_FAIL;
        break;
    }
    }
    return ret;
}

__dev_devop_t power_dev_ops =
{
    DEV_PowerOpen,
    DEV_PowerClose,
    DEV_PowerRead,
    DEV_PowerWrite,
    DEV_PowerIoctrl
};

