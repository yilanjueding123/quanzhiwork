/*
*******************************************************************************
*           				eBase
*                 the Abstract of Hardware
*
*
*              (c) Copyright 2006-2010, ALL WINNER TECH.
*           								All Rights Reserved
*
* File     :  CSP_TMRC.h
* Date     :  2010/11/23 16:36
* By       :  Sam.Wu
* Version  :  V1.00
* Description :  CSP timer controller
* Update   :  date      author      version     notes
*
* Notes: After start the timer, the timer will count down from Interval you set.
         If it counts to Zero it will send IRQ if its IRQ enabled and reset the chip
         if it is the watch-dog and reset control is valid.
*******************************************************************************
*/
#ifndef _CSP_TIMER_OPS_H_
#define _CSP_TIMER_OPS_H_


s32 CSP_TMRC_init(CSP_TMRC_LoscSrc_t loscSrc);

s32 CSP_TMRC_exit(void);
/************************************************************************/
/* RTC                    */
/************************************************************************/
s32 CSP_TMRC_rtc_set_date(u32 year, u32 month, u32 day);
s32 CSP_TMRC_rtc_get_date(u32 *year, u32 *month, u32 *day);

/*********************************************************************
* Method	 :    		CSP_TMRC_rtc_set_time
* Description:
* Parameters :
	@u32 hour : must be 0~23
	@u32 minute : must be 0~59
	@u32 second : must be 0~59
* Returns    :   s32
* Note       :
*********************************************************************/
s32 CSP_TMRC_rtc_set_time(u32 hour, u32 minute, u32 second);
s32 CSP_TMRC_rtc_get_time(u32 *hour, u32 *minute, u32 *second);

s32 CSP_TMRC_rtc_set_week_no(CSP_TMRC_RTC_WeekNo_t weekNo);
CSP_TMRC_RTC_WeekNo_t CSP_TMRC_rtc_get_week_no(void);

/************************************************************************/
/* Alarm                       */
/************************************************************************/

/*********************************************************************
* Method	 :    		CSP_TMRC_alarm_preset
* Description: set when the alarm will alarm.
* Parameters :
	@const CSP_TMRC_AlarmMode_t * alarmMode
* Returns    :   s32
* Note       :the type CSP_TMRC_AlarmMode_t is important, if the alarm mode
            is timer, it will alarm after the duration you set; if the alarm
            mode is normal, it will alarm at the time which match the RTC.
*********************************************************************/
s32 CSP_TMRC_alarm_preset(const CSP_TMRC_AlarmMode_t *alarmMode);

/*********************************************************************
* Method	 :    		CSP_TMRC_alarm_query_alarm_time
* Description: query when the alarm will alarm.
* Parameters :
	@CSP_TMRC_AlarmMode_t * alarmMode
* Returns    :   s32
* Note       :the type CSP_TMRC_AlarmMode_t is important, if the alarm mode
        is timer, the argument alarmMode get the duration the alarm will alarm;
        if the alarm mode is normal, the argument alarmMode get the alarm time referenced to RTC.
*********************************************************************/
s32 CSP_TMRC_alarm_query_alarm_time(CSP_TMRC_AlarmMode_t *alarmMode);

u32 CSP_TMRC_alarm_get_irqNo(void);

/*********************************************************************
* Method	 :    		CSP_TMRC_alarm_enable_irq
* Description: enable the irq of alarm
* Parameters :
	@void
* Returns    :   whether the irq is enabled
* Note       :
*********************************************************************/
s32     CSP_TMRC_alarm_enable_irq(void);

/*********************************************************************
* Method	 :    		CSP_TMRC_alarm_disable_irq
* Description: disable the irq of alarm
* Parameters :
	@void
* Returns    :  whether the irq is disabled.
* Note       :
*********************************************************************/
s32     CSP_TMRC_alarm_disable_irq(void);

/*********************************************************************
* Method	 :    		CSP_TMRC_alarm_is_pending
* Description: check if the alarm irq is pending.
* Parameters :
	@void
* Returns    :  if the alarm irq is pending the 1, else 0.
* Note       :
*********************************************************************/
__bool CSP_TMRC_alarm_is_pending(void);

s32    CSP_TMRC_alarm_clear_pending(void);



/************************************************************************/
/* Watch-dog                      */
/************************************************************************/


/*********************************************************************
* Method	 :    		CSP_TMRC_watchDog_config
* Description: configure the watch dog
* Parameters :
	@const CSP_TMRC_WatchDogPara_t * pWdPara
* Returns    :   EBASE_TRUE/EBASE_FALSE. If the watch-dog is enabled, this
        method will return EBASE_FALSE, otherwise return EBASE_TRUE.
* Note       : This method should only called when the watch dog is in stop status!!
*********************************************************************/
s32 CSP_TMRC_watchDog_set_para(const CSP_TMRC_WatchDogPara_t *pWdPara);

s32 CSP_TMRC_watchDog_get_para(CSP_TMRC_WatchDogPara_t *pWdPara);

__bool CSP_TMRC_watchDog_is_enabled(void);

s32 CSP_TMRC_watchDog_enable(void);

s32 CSP_TMRC_watchDog_disable(void);

/*********************************************************************
* Method	 :    		CSP_TMRC_watchDog_clear
* Description: make the watch-dog recount from interval, or the watch-dog will reset
                if the reset control is valid.
* Parameters :
* Returns    :   EBASE_TRUE/EBASE_FALSE
* Note       :
*********************************************************************/
s32 CSP_TMRC_watchDog_clear(void);

/************************************************************************/
/* timer                      */
/************************************************************************/


/*********************************************************************
* Method	 :    		CSP_TMRC_tmr_request
* Description: Request a timer whose precision is @nMicroSecond and
            at least count to @leastCount in a cycle.
* Parameters :
	@u32 nMicroSecond : precision you demand the timer can take account.
	@u32 leastCount : the least number the timer should count before overflow.
* Returns    :   the handle of a timer, 0 if fail to request.
* Note       :
*********************************************************************/
__hdle CSP_TMRC_tmr_request(const CSP_TMRC_TmrType_t *pType);

s32    CSP_TMRC_tmr_release(__hdle hTmr);

/*********************************************************************
* Method	 :    		CSP_TMRC_tmr_enable_irq
* Description: Enable the timer make interrupt request if de-count to 0.
* Parameters :
	@__hdle hTmr : the handle returned from CSP_TMRC_tmr_request.
* Returns    :   s32
* Note       :
*********************************************************************/
s32 CSP_TMRC_tmr_enable_irq(__hdle hTmr);

s32 CSP_TMRC_tmr_disable_irq(__hdle hTmr);

/*********************************************************************
* Method	 :    		CSP_TMRC_tmr_set_mode
* Description: set timer work mode-- 1 is single, 0 is continuous.
* Parameters :
	@__hdle hTmr : handle returned from CSP_TMRC_tmr_request
	@CSP_TMRC_TmrMode_t tmrMode : oneShoot or  continuous, see type CSP_TMRC_TmrMode_t.
* Returns    :   s32
* Note       :
*********************************************************************/
s32     CSP_TMRC_tmr_set_mode(__hdle hTmr, CSP_TMRC_TmrMode_t tmrMode);

__bool  CSP_TMRC_tmr_mode_is_one_shoot(__hdle hTmr);

/*********************************************************************
* Method	 :    		CSP_TMRC_tmr_get_irqNO
* Description: Get irq number of the timer handled by @hTmr.
* Parameters :
	@__hdle hTmr :handle returned from CSP_TMRC_tmr_request
* Returns    :   irq number, invalid if 0.
* Note       :
*********************************************************************/
u32 CSP_TMRC_tmr_get_irqNO(__hdle hTmr);

/*********************************************************************
* Method	 :    		CSP_TMRC_tmr_set_cycle
* Description: make the timer down counts from @interval to 0.
* Parameters :
	@__hdle hTmr :handle returned from CSP_TMRC_tmr_request.
	@u32 nPeriod : the timer cycle is @nPeriod * precision. (precision is set in xx_request)
* Returns    :   EBASE_TRUE/EBASE_FALSE.
* Note       : The timer will down count from @interval to 0 after you "start" or "restart"
                the timer, and will repeat the cycle if the timer mode is continuous.
*********************************************************************/
s32 CSP_TMRC_tmr_set_cycle(__hdle hTmr, u32 nPeriod);

/*********************************************************************
* Method	 :    		CSP_TMRC_tmr_get_cycle
* Description: Get the interval you set in CSP_TMRC_tmr_set_cycle, 1 is default.
* Parameters :
	@__hdle hTmr :handle returned from CSP_TMRC_tmr_request.
* Returns    :   u32
* Note       :
*********************************************************************/
s32 CSP_TMRC_tmr_get_cycle(__hdle hTmr);

/*********************************************************************
* Method	 :    		CSP_TMRC_tmr_query_count
* Description: The count number before the timer down count to 0. That is,
            after precision * count, the timer will down count to 0.
* Parameters :
	@__hdle hTmr :handle returned from CSP_TMRC_tmr_request.
* Returns    :   u32
* Note       : Attention! If need call this method, the APB clock should at least
            1 times faster than the timer frequency!
*********************************************************************/
u32 CSP_TMRC_tmr_query_count(__hdle hTmr);

/*********************************************************************
* Method	 :    		CSP_TMRC_tmr_is_pending
* Description: Check if the the timer irq is pending now.
* Parameters :
	@__hdle hTmr :handle returned from CSP_TMRC_tmr_request.
* Returns    :   __bool
* Note       :
*********************************************************************/
__bool CSP_TMRC_tmr_is_pending(__hdle hTmr);

/*********************************************************************
* Method	 :    		CSP_TMRC_tmr_clear_pending
* Description: clear the irq pending of the timer.
* Parameters :
	@__hdle hTmr :handle returned from CSP_TMRC_tmr_request.
* Returns    :   1 is the pending cleared, else is 0.
* Note       :
*********************************************************************/
s32    CSP_TMRC_tmr_clear_pending(__hdle hTmr);


/*********************************************************************
* Method	 :    		CSP_TMRC_tmr_reStart
* Description: Make the timer down count from interval again.
* Parameters :
	@__hdle hTmr :handle returned from CSP_TMRC_tmr_request.
* Returns    :   s32
* Note       :
*********************************************************************/
s32 CSP_TMRC_tmr_reStart(__hdle hTmr);

/*********************************************************************
* Method	 :    		CSP_TMRC_tmr_start
* Description: start the timer.
* Parameters :
	@__hdle hTmr :handle returned from CSP_TMRC_tmr_request.
* Returns    :   s32
* Note       :After call this method the timer down counts from interval you set.
*********************************************************************/
s32 CSP_TMRC_tmr_start(__hdle hTmr);

/*********************************************************************
* Method	 :    		CSP_TMRC_tmr_stop
* Description: stop the timer.
* Parameters :
	@__hdle hTmr :handle returned from CSP_TMRC_tmr_request.
* Returns    :   1 if the timer stopped, else 0.
* Note       : Important! If you start the timer after invoke this method,
    the timer will down count from Interval no matter what count it is when it is stopped!
*********************************************************************/
s32 CSP_TMRC_tmr_stop(__hdle hTmr);

/************************************************************************/
/* For AVS timer: There is only 1 AVS count in F20-- AVS0 and AVS1. */
/************************************************************************/
__hdle  CSP_TMRC_avs_request(void);
s32     CSP_TMRC_avs_release(__hdle hAvs);

s32     CSP_TMRC_avs_enable(__hdle hAvs);
s32     CSP_TMRC_avs_disable(__hdle hAvs);

//clock source of AVS is HOSC
s32 CSP_TMRC_avs_set_divisor(__hdle hAvs, u32 divisor);
s32 CSP_TMRC_avs_get_divisor(__hdle hAvs);

//The count will increase 1 after 1s/(24M)*divisor if not paused
s32 CSP_TMRC_avs_set_count(__hdle hAvs, u32 divisor);
u32 CSP_TMRC_avs_get_count(__hdle hAvs);

s32 CSP_TMRC_avs_pause(__hdle hAvs);
s32 CSP_TMRC_avs_continue(__hdle hAvs);


#endif //#ifndef _CSP_TIMER_OPS_H_

