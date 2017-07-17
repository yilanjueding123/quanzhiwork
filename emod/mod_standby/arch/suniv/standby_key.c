/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                            power manager sub-system
*
*                                     (c) Copyright 2008-2009, kevin.z China
*                                              All Rights Reserved
*
* File   : power.c
* Version: V1.0
* By     : kevin.z
* Date   : 2009-6-16 17:31
*********************************************************************************************************
*/
#include "standby_key.h"


#if(ALLOW_LRADCKEY_WAKEUP_STANDBY)

/* register  offset */
#define LRADCK_CTRL_REG          (LRADC_REG_BASE+0x00) //control register
#define LRADCK_INTCTRL_REG       (LRADC_REG_BASE+0x04) //interrupt control register
#define LRADCK_INTSTAT_REG       (LRADC_REG_BASE+0x08) //interrupt status register
#define LRADCK_DATA_REG          (LRADC_REG_BASE+0x0C) //data register for ADC

#define LRADCK_SAMPLE_MASK  (0x3<<2)
#define LRADCK_SAMPLE_250H  (0x0)

/*-lradc Interrupt status register default: 0x0000_0000 -*/
#define LRADCK_DATA_PENDING    (0x01<<0)  /* write 1 to clear it */
#define LRADCK_FST_PENDING     (0x01<<1)  /* write 1 to clear it */
#define LRADCK_LAST_PENDING    (0x01<<4)  /* write 1 to clear it */
#define LRADCK_PENDING_MASK    (LRADCK_DATA_PENDING|LRADCK_FST_PENDING|LRADCK_LAST_PENDING)

#define LRADCK_CHECK_PENDING   (LRADCK_FST_PENDING|LRADCK_DATA_PENDING)



static __u32   KeyCtlBak = 0; /* control register backup */
static __u32   KeyIntBak = 0; /* interrupt control register backup */

//==============================================================================
// QUERRY KEY FOR WAKE UP SYSTEM FROM STANDBY
//==============================================================================


/*
*********************************************************************************************************
*                                     INIT KEY FOR STANDBY
*
* Description: init key for standby.
*
* Arguments  : none
*
* Returns    : EPDK_OK;
*********************************************************************************************************
*/
__s32 standby_key_init(void)
{
    __u32 reg_val;

    /* 1.backup key configuration */
    KeyCtlBak = STANDBY_RREG(LRADCK_CTRL_REG);
    KeyIntBak = STANDBY_RREG(LRADCK_INTCTRL_REG);

    /* 2.set lradc frequency to 250Hz */
    reg_val = STANDBY_RREG(LRADCK_CTRL_REG);
    reg_val &= ~LRADCK_SAMPLE_MASK;
    reg_val |= LRADCK_SAMPLE_250H;
    STANDBY_WREG(LRADCK_CTRL_REG, reg_val);

    /* 3. clear adc0 irq pending */
    reg_val = STANDBY_RREG(LRADCK_INTSTAT_REG);
    reg_val |= LRADCK_PENDING_MASK;
    STANDBY_WREG(LRADCK_INTSTAT_REG, reg_val);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     EXIT KEY FOR STANDBY
*
* Description: exit key for standby.
*
* Arguments  : none
*
* Returns    : EPDK_OK;
*********************************************************************************************************
*/
__s32 standby_key_exit(void)
{
    __u32 reg_val = STANDBY_RREG(LRADCK_INTSTAT_REG);
    reg_val |= LRADCK_PENDING_MASK;
    /* clear adc0 irq pending */
    STANDBY_WREG(LRADCK_INTSTAT_REG, reg_val);/* write 1 to clear 0 */

    /* restore key configuration */
    STANDBY_WREG(LRADCK_CTRL_REG, KeyCtlBak);
    STANDBY_WREG(LRADCK_INTCTRL_REG, KeyIntBak);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     QUERY KEY FOR WAKEUP STANDBY
*
* Description: query key for wakeup standby.
*
* Arguments  : none
*
* Returns    : result;
*               EPDK_TRUE,      get a key;
*               EPDK_FALSE,     no key;
*********************************************************************************************************
*/
__s32 standby_query_key(void)
{
    __u32 reg_val, i = 0;
    reg_val = i;

    /* check if key-down pending is valid */
    reg_val = STANDBY_RREG(LRADCK_INTSTAT_REG);
    if( (reg_val & 0x03) != LRADCK_CHECK_PENDING )
    {
        return EPDK_FALSE;
    }

    //clear pending, and wait some time
    reg_val = STANDBY_RREG(LRADCK_INTSTAT_REG);
    STANDBY_WREG(LRADCK_INTSTAT_REG, reg_val);

    //cpu run HOSC
    for(i = 0; i < 24000; i++)
    {
        ;
    }

    //check key again
    reg_val = STANDBY_RREG(LRADCK_INTSTAT_REG);
    if( (reg_val & 0x01) != LRADCK_DATA_PENDING ) /* 19: first pending still exsit */
    {
        return EPDK_FALSE;
    }

    //get key value, and check if the key is hold key,ignore holdkey.
    reg_val = STANDBY_RREG(LRADCK_DATA_REG) & 0x3f;
    if((pStandbyPar->HKeyMin <= reg_val) && (reg_val <= pStandbyPar->HKeyMax))
    {
        return EPDK_FALSE;
    }

    //don't clear the key pending, leave it to key driver
    return EPDK_TRUE;
}

#endif  //ALLOW_LRADCKEY_WAKEUP_STANDBY

