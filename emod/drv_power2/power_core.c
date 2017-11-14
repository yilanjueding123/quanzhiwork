/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                Power Driver
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : power_core.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-4-30
* Descript: no pmu power dirver.
* Update  : date                auther      ver     notes
*           2011-4-30 10:07:53  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "drv_power_i.h"

#if  0
//#define __here__            eLIBs_printf("@L%d(%s)\n", __LINE__, __FILE__);
#define __log(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
						     eLIBs_printf(__VA_ARGS__)									        )
#else
#define __log(...)    	
#endif


//以下定义为AD采用服务
#define POWER_ADC_SPMPLE_NUM 6//6次AD采样取平均
static __u32 g_adc_val[POWER_ADC_SPMPLE_NUM] = {0};
static __s32 g_cur_adc_sample_num = 0;
__s32 g_adc_exp = 1027;
static __u32 CalcVoltage(__u32 ADCVal)
{
    return ADCVal * 2000 / 63 * g_adc_exp / 1000.0 * (BAT_HIGHT_RESISITOR + BAT_LOW_RESISITOR) / BAT_LOW_RESISITOR;
}

static __u32 PowerGetADSampleVal(void)
{
    __s32 cnt;
    volatile __u32          *tmpAddr, *isrAddr;
    volatile __u32           tmpVal;

    //enable LADC
    tmpAddr = LRADC_CRTL_ADDR;
    tmpVal = *tmpAddr;
    tmpVal = tmpVal | 0x01;
    *tmpAddr = tmpVal;

    isrAddr = LRADC_INTCRTL_ADDR;
    *isrAddr |= 0x1;

    //wait data ready
    tmpVal = LRADC_DATA_VALUE;
    tmpAddr = LRADC_INTSTAT_ADDR;
    *tmpAddr |= 0x1;
    cnt = 0;
    while (1)
    {
        if ((*tmpAddr & 0x1) == 1)
        {
            //data ready
            break;
        }
        if(cnt++ > 2)
        {
            break;
        }
        esKRNL_TimeDly(10);
    }

    if((*tmpAddr & 0x1) == 1)
    {
        //获取ADC采样值
        tmpVal = LRADC_DATA_VALUE;
    }
    else
    {
        __msg("********get adc val fail...************\n");
        tmpVal = 0x3f;
    }

    //__msg("********tmpVal=%d********\n", tmpVal);

    tmpVal = tmpVal & 0x3f;

    return tmpVal;
}

static __u32 PowerGetAverage(__u32 *val, __s32 num)
{
    __u32 total;
    __s32 i;

    if(num <= 0)
    {
        return 0;
    }

    total = 0;
    for(i = 0 ; i < num ; i++)
    {
        total += val[i];
    }

    return total / num;
}

/************************************************
 *4.0v--->4726, 3.95----->4644, 3.90--->4562
 *3.85--->4482, 3.80----->4400, 3.70--->4318
 *3.65--->4236, 3.60----->4156, 3.55--->4074
 *3.47--->3992
************************************************/

void PowerTask(void *parg)
{
    __drv_power_fuelguage_t *pFuelguage;
    __u32                    tmpVal;
    __u32                    cur_adc_val;
    __u32                    Voltage;

    if (parg == NULL)
    {
        __wrn("invalid argument for power timer task\n");
        return ;
    }

    pFuelguage = (__drv_power_fuelguage_t *)parg;

    cur_adc_val = PowerGetADSampleVal();
    // cur_adc_val = 0x3f;//112350


    if(g_cur_adc_sample_num < POWER_ADC_SPMPLE_NUM)
    {
        g_adc_val[g_cur_adc_sample_num++] = cur_adc_val;
    }
    else
    {
        __s32 i;

        for(i = 0 ; i < POWER_ADC_SPMPLE_NUM - 1 ; i++)
        {
            g_adc_val[i] = g_adc_val[i + 1];
        }
        g_adc_val[POWER_ADC_SPMPLE_NUM - 1] = cur_adc_val;
    }

    tmpVal = PowerGetAverage(g_adc_val, g_cur_adc_sample_num);

    __msg("********tmpVal=%d********\n", tmpVal);

    __msg("********tmpVal*k=%f********\n", tmpVal * 2000 / 63 * 1.0276);


    //计算实际电池电压(mV)
    Voltage = CalcVoltage(tmpVal);

    eLIBs_printf("-----jh_Voltage=%d\n", Voltage);
#if 1
	if(Voltage < 4074)
    {
        //低电关机电压
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL0;

        //发送低电关机系统消息
        //__msg("low power voltage detect...\n");
        //esKSRV_SendMsg(KMSG_USR_KEY_POWEROFF, KMSG_PRIO_HIGH);
    }
    else if((Voltage >= 4074) && (Voltage <= 4156))
    {
        //电池电量为20%
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL1;
    }
    else if((Voltage > 4156) && (Voltage <= 4318))
    {
        //电池电量为40%
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL2;
    }
    else if((Voltage > 4318) && (Voltage <= 4482))
    {
        //电池电量为60%
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL3;
    }
    else if((Voltage > 4482) && (Voltage <= 4644))
    {
        //电池电量为80%
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL4;
    }
    else if(Voltage > 4644)
    {
        //电池电量为100%
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL5;
    }
/*
	if(Voltage < 3992)
    {
        //低电关机电压
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL0;

        //发送低电关机系统消息
        //__msg("low power voltage detect...\n");
        //esKSRV_SendMsg(KMSG_USR_KEY_POWEROFF, KMSG_PRIO_HIGH);
    }
    else if((Voltage >= 3992) && (Voltage <= 4074))
    {
        //电池电量为20%
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL1;
    }
    else if((Voltage > 4074) && (Voltage <= 4236))
    {
        //电池电量为40%
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL2;
    }
    else if((Voltage > 4236) && (Voltage <= 4318))
    {
        //电池电量为60%
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL3;
    }
    else if((Voltage > 4318) && (Voltage <= 4482))
    {
        //电池电量为80%
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL4;
    }
    else if(Voltage > 4482)
    {
        //电池电量为100%
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL5;
    }
	*/
#else
    if(Voltage <= 3096)
    {
        //低电关机电压
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL0;

        //发送低电关机系统消息
        //__msg("low power voltage detect...\n");
        //esKSRV_SendMsg(KMSG_USR_KEY_POWEROFF, KMSG_PRIO_HIGH);
    }
    else if((Voltage > 3096) && (Voltage <= 3258))
    {
        //电池电量为20%
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL1;
    }
    else if((Voltage > 3258) && (Voltage <= 3504))
    {
        //电池电量为40%
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL2;
    }
    else if((Voltage > 3504) && (Voltage <= 3666))
    {
        //电池电量为60%
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL3;
    }
    else if((Voltage > 3666) && (Voltage <= 3910))
    {
        //电池电量为80%
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL4;
    }
    else if(Voltage > 3910)
    {
        //电池电量为100%
        pFuelguage->rest_vol = NO_PMU_BATTERY_LEVEL5;
    }
#endif
    pFuelguage->charge_status = 0x07;

    //剩余时间暂时不支持
    pFuelguage->rest_time = 0;

    __msg("PowerTask, rest vol = %d\n", pFuelguage->rest_vol);
}

