/*
*******************************************************************************
*           				eBase
*                 the Abstract of Hardware
*
*
*              (c) Copyright 2006-2010, ALL WINNER TECH.
*           								All Rights Reserved
*
* File     :  bsp_rtc.h
* Date     :  2011/1/13 16:00
* By       :  Sam.Wu
* Version  :  V1.00
* Description :
* Update   :  date      author      version
*
* notes    : interfaces of bsp_rtc
*******************************************************************************
*/

#ifndef _BSP_RTC_H_
#define _BSP_RTC_H_

s32 BSP_RTC_init(void);
s32 BSP_RTC_exit(void);

/************************************************************************/
/* RTC                      */
/************************************************************************/
typedef enum __RTC_WEEK_NO
{
    BSP_RTC_WEEK_NO_MONDAY   = (u32)0,
    BSP_RTC_WEEK_NO_TUSDAY,
    BSP_RTC_WEEK_NO_WEDNESDAY,
    BSP_RTC_WEEK_NO_THURSDAY,
    BSP_RTC_WEEK_NO_FRIDAY,
    BSP_RTC_WEEK_NO_SATURDAY,
    BSP_RTC_WEEK_NO_SUNDAY
} BSP_RTC_WeekNo_t;

/************************************************************************/
/* Alarm                       */
/************************************************************************/

/*********************************************************************
* TypeName	 :    		BSP_RTC_AlarmMode_t
* Description:
* Members    :
    @timerMode: set TMRC_TRUE if you choose timer mode. TMRC_FALSE if normal mode.
    @.mode.timer: meaningful only when timerMode is TMRC_TRUE. If meaningful, this field
      means the interval to the next alarm time you want to set or get.
    @.mode.normal: meaningful only when timerMode is TMRC_FALSE. If meaningful, this field
      means the alarm time referenced to RTC.
* Note       :The alarm can work in one of the 2 work mode---timer mode and normal mode.
            1) If work in timer mode, the alarm is used the same as a timer, it will
              count from 0 to alarm_value + 1, the unit is in second and alarm_value =
              (day*24*60*60 + hour*60*60 + minute*60 + second).
            2) If work in normal mode, the alarm is the everyday alarm clock. If the time
              you preset is equal the RTC, the alarm will make irq if irq enabled.
*********************************************************************/
typedef struct _RTC_AlarmMode
{
    __bool timerMode;//TMRC_TRUE or TMRC_FALSE

    union
    {
        struct
        {
            u8 day;
            u8 hour;
            u8 minute;
            u8 second;
        } timer; //timer is meaningful only when timerMode is TMRC_TRUE

        struct
        {
            u8     hour;
            u8     minute;
            u8     second;
            __bool alarmInMonday;
            __bool alarmInTusday;
            __bool alarmInWesday;
            __bool alarmInTursday;
            __bool alarmInFriday;
            __bool alarmInSaturday;
            __bool alarmInSunday;
        } normal; //alarmTime is meaningful only when timerMode is TMRC_FALSE
    } mode;
} BSP_RTC_AlarmMode_t;

/************************************************************************/
/* RTC                    */
/************************************************************************/
s32 BSP_RTC_set_date(u32 year, u32 month, u32 day);
s32 BSP_RTC_get_date(u32 *year, u32 *month, u32 *day);

/*********************************************************************
* Method	 :    		BSP_RTC_set_time
* Description:
* Parameters :
	@u32 hour : must be 0~23
	@u32 minute : must be 0~59
	@u32 second : must be 0~59
* Returns    :   s32
* Note       :
*********************************************************************/
s32 BSP_RTC_set_time(u32 hour, u32 minute, u32 second);
s32 BSP_RTC_get_time(u32 *hour, u32 *minute, u32 *second);

s32 BSP_RTC_set_week_no(BSP_RTC_WeekNo_t weekNo);
BSP_RTC_WeekNo_t BSP_RTC_get_week_no(void);

/************************************************************************/
/* Alarm                       */
/************************************************************************/

/*********************************************************************
* Method	 :    		BSP_RTC_alarm_preset
* Description: set when the alarm will alarm.
* Parameters :
	@const CSP_TMRC_AlarmMode_t * alarmMode
* Returns    :   s32
* Note       :the type CSP_TMRC_AlarmMode_t is important, if the alarm mode
            is timer, it will alarm after the duration you set; if the alarm
            mode is normal, it will alarm at the time which match the RTC.
*********************************************************************/
s32 BSP_RTC_alarm_preset(const BSP_RTC_AlarmMode_t *alarmMode);

/*********************************************************************
* Method	 :    		BSP_RTC_alarm_query_alarm_time
* Description: query when the alarm will alarm.
* Parameters :
	@CSP_TMRC_AlarmMode_t * alarmMode
* Returns    :   s32
* Note       :the type CSP_TMRC_AlarmMode_t is important, if the alarm mode
        is timer, the argument alarmMode get the duration the alarm will alarm;
        if the alarm mode is normal, the argument alarmMode get the alarm time referenced to RTC.
*********************************************************************/
s32 BSP_RTC_alarm_query_alarm_time(BSP_RTC_AlarmMode_t *alarmMode);

u32 BSP_RTC_alarm_get_irqNo(void);

/*********************************************************************
* Method	 :    		BSP_RTC_alarm_enable_irq
* Description: enable the irq of alarm
* Parameters :
	@void
* Returns    :   whether the irq is enabled
* Note       :
*********************************************************************/
s32     BSP_RTC_alarm_enable_irq(void);

/*********************************************************************
* Method	 :    		BSP_RTC_alarm_disable_irq
* Description: disable the irq of alarm
* Parameters :
	@void
* Returns    :  whether the irq is disabled.
* Note       :
*********************************************************************/
s32     BSP_RTC_alarm_disable_irq(void);

/*********************************************************************
* Method	 :    		BSP_RTC_alarm_is_pending
* Description: check if the alarm irq is pending.
* Parameters :
	@void
* Returns    :  if the alarm irq is pending the 1, else 0.
* Note       :
*********************************************************************/
__bool BSP_RTC_alarm_is_pending(void);

s32    BSP_RTC_alarm_clear_pending(void);

#endif //#ifndef _BSP_RTC_H_


