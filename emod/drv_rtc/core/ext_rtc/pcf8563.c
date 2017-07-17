/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              ir keyboard driver
*
*                                (c) Copyright 2009-2010, kevin.z.m China
*                                             All Rights Reserved
*
* File    : pcf8563.c
* By      : kevin.z.m
* Version : V1.00
* Date    : 2009-11-20 10:22
*********************************************************************************************************
*/
#include "pcf8563.h"

/*
*********************************************************************************************************
*                                   PCF8563 IIC WRITE
*
* Description: PCF8563 iic write data.
*
* Arguments	 : addr     register address;
*              data     pointer to the data need be writen;
*
* Returns	 : result, should be EPDK_OK/EPDK_FAIL;
*
*********************************************************************************************************
*/
static __s32 pcf8563_iic_write(__u8  addr, __u8 *data)
{
    __twi_dev_para_ex_t  piic;

    eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));
    piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
    piic.slave_addr      = RTC_PCF8563_SADDR;
    piic.byte_addr_width = 1;
    piic.byte_addr       = &addr;
    piic.data            = data;
    piic.byte_count      = 1;

    return  eLIBs_fioctrl(*pRtcIICHdl, TWI_WRITE_SPEC_RS, 0, &piic);
}


/*
*********************************************************************************************************
*                                   PCF8563 IIC READ
*
* Description: PCF8563 iic read data.
*
* Arguments	 : addr     register address;
*              data     the data read, for return;
*
* Returns	 : result, should be EPDK_OK/EPDK_FAIL;
*
*********************************************************************************************************
*/
static __s32 pcf8563_iic_read(__u8  addr, __u8 *data)
{
    __twi_dev_para_ex_t  piic;

    eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));
    piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
    piic.slave_addr      = RTC_PCF8563_SADDR;
    piic.byte_addr_width = 1;
    piic.byte_addr       = &addr;
    piic.data            = data;
    piic.byte_count      = 1;

    return  eLIBs_fioctrl(*pRtcIICHdl, TWI_READ_SPEC_RS, 0, (void *)&piic);
}


/*
*********************************************************************************************************
*                                   PCF8563 INIT
*
* Description: PCF8563 init.
*
* Arguments	 : none;
*
* Returns	 : result, should be EPDK_OK/EPDK_FAIL;
*
*********************************************************************************************************
*/
__s32 pcf8563_init(void)
{
    return EPDK_OK;
}
/*
*********************************************************************************************************
*                                   PCF8563 EXIT
*
* Description: PCF8563 exit.
*
* Arguments	 : none;
*
* Returns	 : result, should be EPDK_OK/EPDK_FAIL;
*
*********************************************************************************************************
*/
__s32 pcf8563_exit(void)
{
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   PCF8563 SET DATE
*
* Description: PCF8563 set date.
*
* Arguments	 : year;
               month;
               day;
*
* Returns	 : result, should be EPDK_OK/EPDK_FAIL;
*
*********************************************************************************************************
*/
__s32 PCF8563_SetDate(__u32 year, __u32 month, __u32 day)
{
    __u8    tmpDay, tmpMonth, tmpYear;

    if(   (year < 1900 || year > 2100)
            || (month > 12) || (day > 31))
    {
        __wrn("Date parameter is invalid!(Y%d, M%d, D%d)\n", year, month, day);
        return EPDK_FAIL;
    }

    tmpDay   = Int8ToBCD(day);
    tmpMonth = Int8ToBCD(month);
    if(year < 2000)
    {
        tmpMonth |= (1 << 7);
        year     -= 1900;
    }
    else
    {
        year     -= 2000;
    }
    tmpYear  = Int8ToBCD(year);

    if(pcf8563_iic_write(DAYS_REG, &tmpDay) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    if(pcf8563_iic_write(MONTHS_REG, &tmpMonth) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    //set year
    if(pcf8563_iic_write(YEARS_REG, &tmpYear) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   PCF8563 SET DATE
*
* Description: PCF8563 set date.
*
* Arguments	 : year;
               month;
               day;
*
* Returns	 : result, should be EPDK_OK/EPDK_FAIL;
*
*********************************************************************************************************
*/
__s32 PCF8563_GetDate(__u32 *year, __u32 *month, __u32 *day)
{
    __u8    tmpDay, tmpMonth, tmpYear;

    if(pcf8563_iic_read(DAYS_REG, &tmpDay) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    if(pcf8563_iic_read(MONTHS_REG, &tmpMonth) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    if(pcf8563_iic_read(YEARS_REG, &tmpYear) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    *day  = BCDToInt8(tmpDay & 0x3f);
    *year = BCDToInt8(tmpYear);

    if(tmpMonth & 0x80)
    {
        *year += 1900;
    }
    else
    {
        *year += 2000;
    }
    *month = BCDToInt8(tmpMonth & 0x1f);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   PCF8563 SET TIME
*
* Description: PCF8563 set time.
*
* Arguments	 : hour;
               minute;
               second;
*
* Returns	 : result, should be EPDK_OK/EPDK_FAIL;
*
*********************************************************************************************************
*/
__s32 PCF8563_SetTime(__u32 hour, __u32 minute, __u32 second)
{
    __u8    tmpHour, tmpMinute, tmpSecond;

    if((hour > 24) || (minute > 59) || (second > 59))
    {
        __wrn("time parameter is invalid!\n");
        return EPDK_FAIL;
    }

    //convert the value of calendar's member from integer to BCD code
    tmpHour   = Int8ToBCD(hour);
    tmpMinute = Int8ToBCD(minute);
    tmpSecond =  Int8ToBCD(second);

    if(pcf8563_iic_write(VL_SECOND_REG, &tmpSecond) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    if(pcf8563_iic_write(MINUTES_REG, &tmpMinute) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    if(pcf8563_iic_write(HOURS_REG, &tmpHour) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   PCF8563 GET TIME
*
* Description: PCF8563 get time.
*
* Arguments	 : hour;
               minute;
               second;
*
* Returns	 : result, should be EPDK_OK/EPDK_FAIL;
*
*********************************************************************************************************
*/
__s32 PCF8563_GetTime(__u32 *hour, __u32 *minute, __u32 *second)
{
    __u8    tmpHour, tmpMinute, tmpSecond;

    if(pcf8563_iic_read(VL_SECOND_REG, &tmpSecond) != EPDK_OK)
    {
        return EPDK_FAIL;
    }
    if(pcf8563_iic_read(MINUTES_REG, &tmpMinute) != EPDK_OK)
    {
        return EPDK_FAIL;
    }
    if(pcf8563_iic_read(HOURS_REG, &tmpHour) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    *hour = BCDToInt8(tmpHour & 0x3f);
    *minute = BCDToInt8(tmpMinute & 0x7f);
    *second = BCDToInt8(tmpSecond & 0x7f);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   PCF8563 ENABLE ALARM
*
* Description: PCF8563 enable alarm.
*
* Arguments	 : none;
*
* Returns	 : result;
*
*********************************************************************************************************
*/
__s32 PCF8563_AlarmEnable(void)
{
    __u8    tmpReg;

    //enable minute alarm
    if(pcf8563_iic_read(MINUTE_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }
    tmpReg &= ~(1 << 7);
    if(pcf8563_iic_write(MINUTE_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    //enable hour alarm
    if(pcf8563_iic_read(HOUR_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }
    tmpReg &= ~(1 << 7);
    if(pcf8563_iic_write(HOUR_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    //enable day alarm
    if(pcf8563_iic_read(DAY_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }
    tmpReg &= ~(1 << 7);
    if(pcf8563_iic_write(DAY_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    //disable weekday alarm
    if(pcf8563_iic_read(WEEKDAY_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }
    tmpReg |= (1 << 7);
    if(pcf8563_iic_write(WEEKDAY_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   PCF8563 DISABLE ALARM
*
* Description: PCF8563 disable alarm.
*
* Arguments	 : none;
*
* Returns	 : result;
*
*********************************************************************************************************
*/
__s32 PCF8563_AlarmDisable(void)
{
    __u8    tmpReg;

    //disable minute alarm
    if(pcf8563_iic_read(MINUTE_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }
    tmpReg |= (1 << 7);
    if(pcf8563_iic_write(MINUTE_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    //disable hour alarm
    if(pcf8563_iic_read(HOUR_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }
    tmpReg |= (1 << 7);
    if(pcf8563_iic_write(HOUR_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    //disable day alarm
    if(pcf8563_iic_read(DAY_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }
    tmpReg |= (1 << 7);
    if(pcf8563_iic_write(DAY_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   PCF8563 QUERY ALARM INTERRUPT
*
* Description: PCF8563 query alarm interrupt.
*
* Arguments	 : none;
*
* Returns	 : result;
*
*********************************************************************************************************
*/
__u32 PCF8563_AlarmQueryINT(void)
{
    __u8    tmpReg;

    //read status register
    if(pcf8563_iic_read(CTRL_STA2_REG, &tmpReg) != EPDK_OK)
    {
        return (INTC_IRQNO_FIQ << 24) | 0;
    }

    //check if the alarm flag has been set up
    if(tmpReg & (1 << ALARM_FLAG_BIT))
    {
        //clear alarm flag
        tmpReg &= ~(1 << ALARM_FLAG_BIT);

        //write status register
        pcf8563_iic_write(CTRL_STA2_REG, &tmpReg);

        return (INTC_IRQNO_FIQ << 24) | 1;
    }

    return (INTC_IRQNO_FIQ << 24) | 0;
}


/*
*********************************************************************************************************
*                                   PCF8563 ENABLE ALARM INTERRUPT
*
* Description: PCF8563 enable alarm interrupt.
*
* Arguments	 : mode     don't care;
*              attr     don't care;
*
* Returns	 : result, should be EPDK_OK/EPDK_FAIL;
*
*********************************************************************************************************
*/
__s32 PCF8563_AlarmEnableINT(__u32 mode, __u32 attr)
{
    __u8    tmpReg;

    //read status register
    if(pcf8563_iic_read(CTRL_STA2_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    //clear alarm flag and enable alarm interrupt
    tmpReg &= ~(1 << ALARM_FLAG_BIT);
    tmpReg |=  (1 << ALARM_INT_BIT);

    //write status register
    if(pcf8563_iic_write(CTRL_STA2_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   PCF8563 DISABLE ALARM INTERRUPT
*
* Description: PCF8563 disable alarm interrupt.
*
* Arguments	 : mode     don't care;
*              attr     don't care;
*
* Returns	 : result, should be EPDK_OK/EPDK_FAIL;
*
*********************************************************************************************************
*/
__s32 PCF8563_AlarmDisableINT(__u32 mode, __u32 attr)
{
    __u8    tmpReg;

    //read status register
    if(pcf8563_iic_read(CTRL_STA2_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    //clear alarm flag and disable alarm interrupt
    tmpReg &= ~(1 << ALARM_FLAG_BIT);
    tmpReg &= ~(1 << ALARM_INT_BIT);

    //write status register
    if(pcf8563_iic_write(CTRL_STA2_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}


static __u32 CommYear[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
static __u32 LeapYear[13] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};
/*
*********************************************************************************************************
*                                   PCF8563 SET ALARM DURATION
*
* Description: PCF8563 set alarm duration.
*
* Arguments	 : duration     the duration of alarm coming from (2000-01-01, 00:00)
*
* Returns	 : result, should be EPDK_OK/EPDK_FAIL;
*
*********************************************************************************************************
*/
__s32 PCF8563_AlarmSetDuration(__u32 duration)
{
    __u32   tmpHour, tmpMinute;
    __u32   tmpDay = 0, tmpMonth = 0, tmpYear;
    __u32   *tmpArray;
    __s32   i;
    __u8    tmpReg;

    //calculate the day hour and minute
    tmpDay    = duration / (60 * 60 * 24);
    tmpHour   = (duration % (60 * 60 * 24)) / (60 * 60);
    tmpMinute = (duration % (60 * 60)) / 60;

    //calculate the day in one year
    tmpYear = (tmpDay / (366 + 365 + 365 + 365)) * 4;
    tmpDay %= 366 + 365 + 365 + 365;
    if(tmpDay < 366)
    {
        tmpArray = LeapYear;
    }
    else
    {
        tmpDay -= 366;
        tmpYear++;
        while(tmpDay > 365)
        {
            tmpDay -= 365;
            tmpYear++;
        }
        tmpArray = CommYear;
    }

    //calculate the day in one month
    for(i = 1; i < 13; i++)
    {
        if(tmpDay < tmpArray[i])
        {
            tmpDay -= tmpArray[i - 1];
            tmpMonth = i;
            break;
        }
    }
    tmpDay += 1;

    //translate integer code to BCD code
    tmpDay    = Int8ToBCD(tmpDay);
    tmpHour   = Int8ToBCD(tmpHour);
    tmpMinute = Int8ToBCD(tmpMinute);

    //setup minute alarm
    if(pcf8563_iic_read(MINUTE_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }
    tmpReg &= ~0x7f;
    tmpReg |= (tmpMinute & 0x7f);
    if(pcf8563_iic_write(MINUTE_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    //setup hour alarm
    if(pcf8563_iic_read(HOUR_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }
    tmpReg &= ~0x3f;
    tmpReg |= (tmpHour & 0x3f);
    if(pcf8563_iic_write(HOUR_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    //setup day alarm
    if(pcf8563_iic_read(DAY_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }
    tmpReg &= ~0x3f;
    tmpReg |= (tmpDay & 0x3f);
    if(pcf8563_iic_write(DAY_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }

#if 0 //not internal RTC in this chip
    //save year and month to the internal register
    while(TMRC_REG_LOSCCTRL & (1 << 7));
    TMRC_REG_TM3YMD = (tmpYear << 16) | (tmpMonth << 8);
    while(TMRC_REG_LOSCCTRL & (1 << 7));
#endif // not internal RTC in this chip

    return EPDK_OK;
}
/*
*********************************************************************************************************
*                                   PCF8563 QUERY ALARM
*
* Description: PCF8563 query alarm.
*
* Arguments	 : none
*
* Returns	 : result, time of current alarm;
*
*********************************************************************************************************
*/
__u32 PCF8563_AlarmQuery(void)
{
    __u32   tmpHour, tmpMinute;
    __u32   tmpDay = 0, tmpMonth = 0, tmpYear;
    __u32   tmpDuration = 0;
    __u32   *tmpArray;
    __u8    tmpReg;

    tmpDuration = 0;

    //get year and month information
    if(pcf8563_iic_read(YEARS_REG, &tmpReg))
    {
        __wrn("FAil to read year from extern rtc by twi.\n");
        return 0;
    }
    tmpYear = BCDToInt8(tmpReg & 0x7f);

    if(pcf8563_iic_read(MONTHS_REG, &tmpReg))
    {
        __wrn("FAil to read year from extern rtc by twi.\n");
        return 0;
    }
    tmpMonth = BCDToInt8(tmpReg & 0x7f);


    //get minute information
    if(pcf8563_iic_read(MINUTE_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return 0;
    }
    tmpMinute = BCDToInt8(tmpReg & 0x7f);

    //get hour information
    if(pcf8563_iic_read(HOUR_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return 0;
    }
    tmpHour = BCDToInt8(tmpReg & 0x3f);

    //get day information
    if(pcf8563_iic_read(DAY_ALM_REG, &tmpReg) != EPDK_OK)
    {
        return 0;
    }
    tmpDay = BCDToInt8(tmpReg & 0x3f);

    //calculate time by year
    tmpDuration = (tmpYear / 4) * (366 + 365 + 365 + 365) * 24 * 60 * 60;
    tmpYear %= 4;
    switch(tmpYear)
    {
    case 1:
        tmpDuration += 366 * 24 * 60 * 60;
        break;
    case 2:
        tmpDuration += (366 + 365) * 24 * 60 * 60;
        break;
    case 3:
        tmpDuration += (366 + 365 + 365) * 24 * 60 * 60;
        break;
    default:
        break;
    }

    //calculate time by month
    if(tmpYear)
    {
        tmpArray = CommYear;
    }
    else
    {
        tmpArray = LeapYear;
    }
    tmpDuration += tmpArray[tmpMonth - 1] * 24 * 60 * 60;

    //calculate time by day
    tmpDuration += (tmpDay - 1) * 24 * 60 * 60;

    //calculate time by hour
    tmpDuration += tmpHour * 60 * 60;

    //calcualte time by minute
    tmpDuration += tmpMinute * 60;

    return tmpDuration;
}


/*
*********************************************************************************************************
*                                   ISR MAIN PROCESS
*
* Description: isr main process;
*
* Arguments  : parg     don't care;
*
* Returns    : EPDK_TRUE/EPDK_FALSE;
*
* Note       : do nothing;
*********************************************************************************************************
*/
static __s32 PCF8563_AlarmIsrMain(void *pArg)
{
    return EPDK_TRUE;
}


/*
*********************************************************************************************************
*                                   ISR TAIL PROCESS
*
* Description: isr tail process;
*
* Arguments  : parg     don't care;
*
* Returns    : EPDK_TRUE/EPDK_FALSE;
*
* Note       : if the alarm interrupt happenned, send message to system message pool;
*********************************************************************************************************
*/
static __s32 PCF8563_AlarmIsrTail(void *pArg)
{
    if(PCF8563_AlarmQueryINT() & 1)
    {
        __inf("alarm from ext-rtc send interrupt!\n");
        PCF8563_AlarmDisable();
        //wait 1 32k cycle for clear alarm pending
        while(PCF8563_AlarmQueryINT() & 1);
        esKSRV_SendMsg(KMSG_USR_SYSTEM_ALARM_COMMING, KMSG_PRIO_HIGH);

        return EPDK_TRUE;
    }

    return EPDK_FALSE;
}


/*
*********************************************************************************************************
*                                              INSTALL ALARM ISR
*
* Description: install alarm isr;
*
* Arguments     : none;
*
* Returns     : EPDK_OK/EPDK_FAIL;
*********************************************************************************************************
*/
__s32 PCF8563_AlarmInstIsr(void)
{
    esINT_InsFIR(PCF8563_AlarmQueryINT() >> 24, (__pISR_t)PCF8563_AlarmIsrMain, \
                 (__pCBK_t)PCF8563_AlarmIsrTail, 0);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                              UNINSTALL ALARM ISR
*
* Description: uninstall alarm isr;
*
* Arguments     : none;
*
* Returns     : EPDK_OK/EPDK_FAIL;
*********************************************************************************************************
*/
__s32 PCF8563_AlarmUninstIsr(void)
{
    esINT_UniFIR(PCF8563_AlarmQueryINT() >> 24, (__pISR_t)PCF8563_AlarmIsrMain,     \
                 (__pCBK_t)PCF8563_AlarmIsrTail);

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                              SET CLOCK OUT
*
* Description: set clock out;
*
* Arguments     : on:open clock out    clock:freq of out clock;
*
* Returns     : EPDK_OK/EPDK_FAIL;
2011-1-6 11:20:12
*********************************************************************************************************
*/
__s32 PCF8563_SetClockOut(__u32 aux)
{
    __u8 tmpreg;

    tmpreg = aux & 0x00000083;

    if(pcf8563_iic_write(CLKOUT_CTRL_REG, &tmpreg) != EPDK_OK)
    {
        return EPDK_FAIL;
    }
    return EPDK_OK;
}

