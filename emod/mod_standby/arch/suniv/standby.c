/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                            power manager sub-system
*
*                                     (c) Copyright 2008-2009, kevin.z China
*                                              All Rights Reserved
*
* File   : standby.c
* Version: V1.0
* By     : kevin.z
* Date   : 2009-6-6 16:09
*********************************************************************************************************
*/

#include "standby_i.h"

static __s32 standby_exit(void);
static __s32 standby_init(void);
static __s32 standby_wait_event(void);

struct pll_factor_t orig_pll;
struct pll_factor_t local_pll;
struct sun4i_clk_div_t  clk_div;
struct sun4i_clk_div_t  tmp_clk_div;

extern __u32 cpu_ms_loopcnt;

void standby_delay(__u32 ms)
{
    int i;

    for(i = ms; i > 0; i--);

    return ;
}

void standby_mdelay(__u32 ms)
{
    standby_delay(ms * cpu_ms_loopcnt);
}


void standby_memcpy(void *dest, void *src, int n)
{
    char    *tmp_src = (char *)src;
    char    *tmp_dst = (char *)dest;

    if(!dest || !src)
    {
        /* parameter is invalid */
        return;
    }

    for( ; n > 0; n--)
    {
        *tmp_dst ++ = *tmp_src ++;
    }

    return;
}

/*
*********************************************************************************************************
*                                    ENTER STANDBY MODE
*
* Description: enter standby mode.
*
* Arguments  : none
*
* Returns    : none;
*********************************************************************************************************
*/
void enter_standby(void)
{
    //enable dram selfrefresh
    dram_power_save_process();

    //initialise standby modules
    standby_init();

    //wait for wakeup events
    standby_wait_event();

    //exit standby modules
    standby_exit();

    //disable dram selfrefresh
    mctl_init_stadby();

    return;
}

/*
*********************************************************************************************************
*                                       INITIALIZE STANDBY MODULES
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
static __s32 standby_init(void)
{
    //initialise clk module
    standby_clk_init();

    //initialise interrupt module
    standby_int_init();

    //initialise timer module
    standby_tmr_init();

    //initialise twi module
    standby_twi_init();

    //initialise key module
#if (ALLOW_LRADCKEY_WAKEUP_STANDBY)
    standby_key_init();
#endif

    //initialise ir module
#if (ALLOW_IR_WAKEUP_STANDBY)
    //standby_ir_init(pStandbyPar->IrBuffer, 128);
#endif

    //initialise power module
    standby_power_init();

    //initialise usb module
#if (ALLOW_USB_WAKEUP_STANDBY)
    standby_usb_init();
#endif

    //initialize matrix key module
#if (ALLOW_MATRIXKEY_WAKEUP_STANDBY)
    standby_matrixkey_init();
#endif

#if (ALLOW_EINT_WAKEUP_STANDBY)
    standby_eint_init(pStandbyPar->eint_no, pStandbyPar->pio_int_trigger, pStandbyPar->port);
#endif



    /* backup cpu freq */
    standby_clk_get_pll_factor(&orig_pll);
    local_pll.FactorN = 16;
    local_pll.FactorK = 0;
    local_pll.FactorM = 0;
    local_pll.FactorP = 0;
    standby_clk_set_pll_factor(&local_pll);
    standby_mdelay(100);

    //switch cpu clock to HOSC
    standby_clk_core2hosc();
    standby_mdelay(100);

    /* switch cpu to 32k */
    standby_clk_core2losc();
    standby_mdelay(100);

    /* switch ahb1 to 32k */
    standby_clk_ahb2losc();
    standby_mdelay(100);


    //disable plls
    standby_clk_plldisable();
    standby_mdelay(100);

    //disable hosc
    //standby_clk_hoscdisable();
    //standby_mdelay(100);

    //standby initialize finished
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       EXIT STANDBY MODULES
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
static __s32 standby_exit(void)
{
    __s32   i;


    //enable hosc
    //standby_clk_hoscenable();
    //standby_mdelay(100);

    //enable plls
    standby_clk_pllenable();
    standby_mdelay(100);


    /* restore cpu freq */
    standby_clk_set_pll_factor(&orig_pll);
    standby_mdelay(100);


    /* switch ahb1 to pll6 */
    standby_clk_ahb2pll6();
    standby_mdelay(100);


    /* switch clock to hosc */
    standby_clk_core2hosc();
    standby_mdelay(100);


    //switch cpu clock to core pll
    standby_clk_core2pll();
    standby_mdelay(100);




#if (ALLOW_EINT_WAKEUP_STANDBY)
    standby_eint_exit(pStandbyPar->eint_no, pStandbyPar->pio_int_trigger, pStandbyPar->port);
#endif


    //exit matrix key module
#if (ALLOW_MATRIXKEY_WAKEUP_STANDBY)
    standby_matrixkey_exit();
#endif

    //exit usb module
#if(ALLOW_USB_WAKEUP_STANDBY)
    standby_usb_exit();
#endif

    //exit power module
    standby_power_exit();

    //exit ir module
#if (ALLOW_IR_WAKEUP_STANDBY)
    //standby_ir_exit();
#endif

    //exit key module
#if (ALLOW_LRADCKEY_WAKEUP_STANDBY)
    standby_key_exit();
#endif

    //exit twi module
    standby_twi_exit();

    //exit tmr module
    standby_tmr_exit();

    //exit interrupt module
    standby_int_exit();

    //initialise clk module
    standby_clk_exit();


    //standby exit finished
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       WAIT FOR WAKEUP EVENTS
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
static __s32 standby_wait_event(void)
{
    __u32 opdata        = 0;
    __u32 standbyticks  = 0;
    __u32 standbysecond  = 0;
    __s32 adc_valume = 0;

    //this just to make compiler happy
    opdata = opdata;



    //wait for valid wakeup event coming
    while (1)
    {


        //enter WFI(wait for interrupt) instruction
        __asm{mcr p15, 0, opdata, c7, c0, 4}

        //------------------------------------------------------------
        //
        //      wakeup interrupt coming,
        //      check the wakeup interrupt source.
        //
        //------------------------------------------------------------

        //clear watchdog to avoid system reset.
        standby_clear_dog();



        //check if periodic timer event comming
        if(standby_tmr_query() == EPDK_TRUE)
        {

            //increase standby ticks count,
            //second record time is to avoid use division,
            //standby code is limited, division code is big.
            standbyticks++;
            if ((standbyticks * STANDBY_TIMER_TICKS) >= STANDBY_TICKS_PER_SEC)
            {
                //standby one second reached,
                //clear standby time ticks.
                standbysecond++;
                standbyticks = 0;
            }
            // check if power-off time ticks reached
            if(standbysecond > pStandbyPar->PowerOffTime)
            {
                // power off time ticks reached
                pStandbyPar->EventFlag |= STANDBY_EVENT_POWEROFF;
                break;
            }
        }

#if (ALLOW_TP_KEY_WAKEUP_STANDBY)
        {

            RK_SelectChannel(RK_CHANNEL_X2);//RK_CHANNEL_X1
            RK_ClearFifo();
            adc_valume = standby_get_adc_valume();
            if(adc_valume < 4000)
            {
                break;
            }
            RK_SelectChannel(RK_CHANNEL_X2);
            RK_ClearFifo();
            adc_valume = standby_get_adc_valume();
            if(adc_valume < 4000)
            {
                break;
            }

        }
#endif


        //check if external nmi event coming
#if (ALLOW_NMI_WAKEUP_STANDBY)
        if(standby_query_int(INT_SOURCE_EXTNMI) == EPDK_TRUE)
        {
            // check if low power or external power
            if(pStandbyPar->PowerCfg.power_exist)
            {
                if((standby_low_power_dectect() == EPDK_TRUE) && (standby_ext_power_detect() == EPDK_FALSE))
                {
                    // low power happend
                    pStandbyPar->EventFlag |= STANDBY_EVENT_LOWPOWER;
                }
            }
            break;
        }
#endif



        //check if pio event coming
#if (ALLOW_EINT_WAKEUP_STANDBY)
        {
            if(standby_query_eint(pStandbyPar->eint_no, pStandbyPar->port) == EPDK_TRUE)
            {
                standby_clear_eint(pStandbyPar->eint_no, pStandbyPar->port);

                break;
            }
        }
#endif

        //check if ir event coming
#if (ALLOW_IR_WAKEUP_STANDBY)
        if(standby_query_int(INT_SOURCE_IR) == EPDK_TRUE)
        {

            //if(standby_ir_verify() == EPDK_OK)
            {
                pStandbyPar->WakeUpMode = 4;
                break;
            }
        }
#endif

        //check if key event coming
#if (ALLOW_LRADCKEY_WAKEUP_STANDBY)
        if(standby_query_int(INT_SOURCE_LRADC) == EPDK_TRUE)
        {

            //if(standby_query_key() == EPDK_TRUE)
            {

                pStandbyPar->WakeUpMode = 1;
                break;
            }
        }
#endif

        //check if usb event coming
#if (ALLOW_USB_WAKEUP_STANDBY)
        if(standby_query_usb_event() == EPDK_TRUE)
        {
            pStandbyPar->WakeUpMode = 2;
            break;
        }
#endif

        //check if matrix key event coming
#if (ALLOW_MATRIXKEY_WAKEUP_STANDBY)
        if(standby_query_matrixkey() == EPDK_TRUE)
        {
            pStandbyPar->WakeUpMode = 1;
            break;
        }
#endif



    }



    //valid wakeup event coming.
    return EPDK_OK;
}
