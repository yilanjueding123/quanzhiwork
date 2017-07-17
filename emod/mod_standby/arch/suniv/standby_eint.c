/*
**********************************************************************************************************************
*                                                    ePOS
*                                   the Easy Portable/Player Operation System
*                                              standby sub-system
*
*                                      (c) Copyright 2009-2010, jerr China
*                                              All Rights Reserved
*
* File    : standby_ir.c
* By      : jerry
* Version : V1.00
**********************************************************************************************************************
*/
#include  "standby_eint.h"

#if(ALLOW_EINT_WAKEUP_STANDBY)

//PD
#define  EINT_PIO_REG_PD_CFG0	   (PIOC_REG_BASE+0x200) //(n = 0~7)
#define  EINT_PIO_REG_PD_CFG1	   (PIOC_REG_BASE+0x204) //(n = 8~15) 
#define  EINT_PIO_REG_PD_CFG2	   (PIOC_REG_BASE+0x208) //(n = 16~21)  
#define  EINT_PIO_REG_INT_PD_ENA   (PIOC_REG_BASE+0x210) //(n = 0~21) 
#define  EINT_PIO_REG_INT_PD_STR   (PIOC_REG_BASE+0x214) //(n = 0~21) 


//PE
#define  EINT_PIO_REG_PE_CFG0	   (PIOC_REG_BASE+0x220) //(n = 0~7)
#define  EINT_PIO_REG_PE_CFG1	   (PIOC_REG_BASE+0x224) //(n = 8~12) 
#define  EINT_PIO_REG_INT_PE_ENA   (PIOC_REG_BASE+0x230) //(n = 0~12)  
#define  EINT_PIO_REG_INT_PE_STR   (PIOC_REG_BASE+0x234) //(n = 0~12)


//PF
#define  EINT_PIO_REG_PF_CFG0	   (PIOC_REG_BASE+0x240) //(n = 0~5)
#define  EINT_PIO_REG_INT_PF_ENA   (PIOC_REG_BASE+0x250) //(n = 0~5)  
#define  EINT_PIO_REG_INT_PF_STR   (PIOC_REG_BASE+0x254) //(n = 0~5)



static __u32   pio_pd_cfg0 = 0;
static __u32   pio_pd_cfg1 = 0;
static __u32   pio_pd_cfg2 = 0;
static __u32   pio_pd_int_cfg = 0; /* interrupt control register backup */

static __u32   pio_pe_cfg0 = 0;
static __u32   pio_pe_cfg1 = 0;
static __u32   pio_pe_int_cfg = 0; /* interrupt control register backup */

static __u32   pio_pf_cfg0 = 0;
static __u32   pio_pf_int_cfg = 0; /* interrupt control register backup */

__s32 standby_eint_init(__u32 eint_no, enum trigger_condition_e trigger_condition, __u32 port)
{

    __u32 reg_value ;

    if(port == 4)
    {
        // back up config reg
        pio_pd_cfg0    = STANDBY_RREG(EINT_PIO_REG_PD_CFG0);
        pio_pd_cfg1    = STANDBY_RREG(EINT_PIO_REG_PD_CFG1);
        pio_pd_cfg2    = STANDBY_RREG(EINT_PIO_REG_PD_CFG2);
        pio_pd_int_cfg = STANDBY_RREG(EINT_PIO_REG_INT_PD_ENA);

        // config trigger level
        reg_value = trigger_condition << ((eint_no & 0x07) << 2) ;
        STANDBY_WREG((EINT_PIO_REG_PD_CFG0 + ((eint_no >> 3) << 2)), reg_value);

        // enable interrupt
        reg_value = (1 << eint_no) ;
        STANDBY_WREG(EINT_PIO_REG_INT_PD_ENA, reg_value);

        // clear interrupt
        reg_value = STANDBY_RREG(EINT_PIO_REG_INT_PD_STR);
        reg_value |= (1 << eint_no);
        STANDBY_WREG(EINT_PIO_REG_INT_PD_STR, reg_value);

    }
    else if(port == 5)
    {
        // back up config reg
        pio_pe_cfg0    = STANDBY_RREG(EINT_PIO_REG_PE_CFG0);
        pio_pe_cfg1    = STANDBY_RREG(EINT_PIO_REG_PE_CFG1);
        pio_pe_int_cfg = STANDBY_RREG(EINT_PIO_REG_INT_PE_ENA);

        // config trigger level
        reg_value = trigger_condition << ((eint_no & 0x07) << 2) ;
        STANDBY_WREG((EINT_PIO_REG_PE_CFG0 + ((eint_no >> 3) << 2)), reg_value);

        // enable interrupt
        reg_value = (1 << eint_no) ;
        STANDBY_WREG(EINT_PIO_REG_INT_PE_ENA, reg_value);


        // clear interrupt
        reg_value = STANDBY_RREG(EINT_PIO_REG_INT_PE_STR);
        reg_value |= (1 << eint_no);
        STANDBY_WREG(EINT_PIO_REG_INT_PE_STR, reg_value);

    }
    else if(port == 6)
    {
        pio_pf_cfg0    = STANDBY_RREG(EINT_PIO_REG_PF_CFG0);
        pio_pf_int_cfg = STANDBY_RREG(EINT_PIO_REG_INT_PF_ENA);

        // config trigger level
        reg_value = trigger_condition << ((eint_no & 0x07) << 2) ;
        STANDBY_WREG((EINT_PIO_REG_PF_CFG0 + ((eint_no >> 3) << 2)), reg_value);

        // enable interrupt
        reg_value = (1 << eint_no) ;
        STANDBY_WREG(EINT_PIO_REG_INT_PF_ENA, reg_value);

        // clear interrupt
        reg_value = STANDBY_RREG(EINT_PIO_REG_INT_PF_STR);
        reg_value |= (1 << eint_no);
        STANDBY_WREG(EINT_PIO_REG_INT_PF_STR, reg_value);

    }
    else
    {

    }


    return EPDK_OK;
}
__s32 standby_eint_exit(__u32 eint_no, enum trigger_condition_e trigger_condition, __u32 port)
{


    if(port == 4)
    {
        STANDBY_WREG(EINT_PIO_REG_PD_CFG0, pio_pd_cfg0);
        STANDBY_WREG(EINT_PIO_REG_PD_CFG1, pio_pd_cfg1);
        STANDBY_WREG(EINT_PIO_REG_PD_CFG2, pio_pd_cfg2);
        STANDBY_WREG(EINT_PIO_REG_INT_PD_ENA, pio_pd_int_cfg);
    }
    else if(port == 5)
    {
        STANDBY_WREG(EINT_PIO_REG_PE_CFG0, pio_pe_cfg0);
        STANDBY_WREG(EINT_PIO_REG_PE_CFG1, pio_pe_cfg1);
        STANDBY_WREG(EINT_PIO_REG_INT_PE_ENA, pio_pd_int_cfg);
    }
    else if(port == 6)
    {
        STANDBY_WREG(EINT_PIO_REG_PF_CFG0, pio_pf_cfg0);
        STANDBY_WREG(EINT_PIO_REG_INT_PF_ENA, pio_pf_int_cfg);
    }
    else
    {

    }

    return EPDK_OK;
}

__s32 standby_query_eint(__u32 eint_no, __u32 port)
{

    if(port == 4)
    {
        return (STANDBY_RREG(EINT_PIO_REG_INT_PD_STR) && (1 << eint_no));
    }
    else if(port == 5)
    {
        return (STANDBY_RREG(EINT_PIO_REG_INT_PE_STR) && (1 << eint_no));
    }
    else if(port == 6)
    {
        return (STANDBY_RREG(EINT_PIO_REG_INT_PF_STR) && (1 << eint_no));
    }
    else
    {

    }

    return EPDK_OK;
}
__s32 standby_clear_eint(__u32 eint_no, __u32 port)
{

    __u32 reg_value ;

    if(port == 4)
    {
        reg_value = STANDBY_RREG(EINT_PIO_REG_INT_PD_STR);
        reg_value |= (1 << eint_no);
        STANDBY_WREG(EINT_PIO_REG_INT_PD_STR, reg_value);
    }
    else if(port == 5)
    {
        reg_value = STANDBY_RREG(EINT_PIO_REG_INT_PE_STR);
        reg_value |= (1 << eint_no);
        STANDBY_WREG(EINT_PIO_REG_INT_PE_STR, reg_value);
    }
    else if(port == 6)
    {
        reg_value = STANDBY_RREG(EINT_PIO_REG_INT_PF_STR);
        reg_value |= (1 << eint_no);
        STANDBY_WREG(EINT_PIO_REG_INT_PF_STR, reg_value);
    }
    else
    {

    }


    return EPDK_OK;
}
#endif  //ALLOW_EINT_WAKEUP_STANDBY

