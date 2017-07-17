/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Standby Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : standby_spi.c
* By      : Alex.Leaf
* Version : v1.0
* Date    : 2010-7-8 19:07
* Descript:
* Update  : date                auther      ver     notes
*********************************************************************************************************
*/
#include "ePDK.h"
#include "standby_spi.h"


#if(ALLOW_USREVENT_WAKEUP_STANDBY)

/*
*********************************************************************************************************
*                                   STANDBY SPI PIN CONFIGURATION
*
* Description: standby spi pin configuration.
*
* Arguments  : pin_grp      pin group.
*              onoff
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
static void standby_spi_pincfg(__u32 pin_grp, __u32 onoff)
{
    __u32   rval;

    switch(pin_grp)
    {
    case SPI_GROUPA:
    {
        //SPI0_DO-PIOC0, SPI0_DI-PIOC1, SPI0_CLK-PIOC2, SPI0_CS-PIOC6
        rval = PIOC_REG_C_CFG0;
        rval &= ~(0x777);
        rval &= ~(0x7 << 24);
        if (onoff)
        {
            rval |= 0x333;
            rval |= 0x3 << 24;
        }
        PIOC_REG_C_CFG0 = rval;

        //pull up CS
        rval = PIOC_REG_C_PULL0;
        rval &= ~(0x3 << 12);
        if (onoff)
        {
            rval |= 0x1 << 12;
        }
        PIOC_REG_C_PULL0 = rval;

        //SPI0_CLK, SPI0_DI driver level
        rval = PIOC_REG_C_MULDRV0;
        rval &= ~(0x3f);
        rval &= ~(0x3 << 12);
        if (onoff)
        {
            rval |= 0x15 | (0x01 << 12);
        }
        rval |= 0x15 | (0x01 << 12);
        PIOC_REG_C_MULDRV0 = rval;

        break;
    }

    case SPI_GROUPB_0:
    {
        //SPI1_CS0-PIOC3, SPI1_CLK-PIOC4, SPI1_DO-PIOC10, SPI1_DI-PIOC11
        rval = PIOC_REG_C_CFG0;
        rval &= ~(0x77 << 12);
        if (onoff)
        {
            rval |= 0x33 << 12;
        }
        PIOC_REG_C_CFG0 = rval;

        rval = PIOC_REG_C_CFG1;
        rval &= ~(0x77 << 8);
        if (onoff)
        {
            rval |= 0x33 << 8;
        }
        PIOC_REG_C_CFG1 = rval;

        //pull up CS
        rval = PIOC_REG_C_PULL0;
        rval &= ~(0x3 << 6);
        if (onoff)
        {
            rval |= (0x1 << 6);
        }
        PIOC_REG_C_PULL0 = rval;

        //SPI1_CLK, SPI1_DI driver level
        rval = PIOC_REG_C_MULDRV0;
        rval &= ~(0x3 << 8);
        rval &= ~(0x3 << 22);
        if (onoff)
        {
            rval |= 0x1 << 8;
            rval |= 0x1 << 22;
        }
        PIOC_REG_C_MULDRV0 = rval;

        break;
    }

    case SPI_GROUPB_1:
    {
        //SPI1_CS0-PIOA2, SPI1_CLK-PIOA3, SPI1_DO-PIOA4, SPI1_DI-PIOA5
        rval = PIOC_REG_A_CFG0;
        rval &= ~(0x7777 << 8);
        if (onoff)
        {
            rval |= 0x3333 << 8;
        }
        PIOC_REG_A_CFG0 = rval;

        //pull up CS
        rval = PIOC_REG_A_PULL0;
        rval &= ~(0xff << 4);
        if (onoff)
        {
            rval |= 0x55 << 4;
        }
        PIOC_REG_A_PULL0 = rval;

        //SPI1_CLK, SPI1_DI driver level
        rval = PIOC_REG_A_MULDRV0;
        rval &= ~(0xff << 4);
        if (onoff)
        {
            rval |= 0x55 << 4;
        }
        PIOC_REG_A_MULDRV0 = rval;

        break;
    }

    case SPI_GROUPB_2:
    {
        //SPI1_CS0-PIOB11, SPI1_CLK-PIOB12, SPI1_DI-PIOB13, SPI1_DO-PIOB14, SPI1_CS1-PIOB15
        rval = PIOC_REG_B_CFG1;
        rval &= ~(0x77777 << 12);
        if (onoff)
        {
            rval |= 0x55555 << 12;
        }
        PIOC_REG_B_CFG1 = rval;

        //pull up CS
        rval = PIOC_REG_B_PULL0;
        rval &= ~(0x303U << 22);
        if (onoff)
        {
            rval |= 0x101U << 22;
        }
        PIOC_REG_B_PULL0 = rval;

        //SPI1_CLK, SPI1_DI driver level
        rval = PIOC_REG_B_MULDRV0;
        rval &= ~(0xfU << 24);
        if (onoff)
        {
            rval |= 0x5 << 24;
        }
        PIOC_REG_B_MULDRV0 = rval;

        break;
    }
    }
}


/*
*********************************************************************************************************
*                                   STANDBY SPI HCLOCK ON/OFF
*
* Description: open or close spi hclock.
*
* Arguments  : spi_no   spi number.
*
*              onoff    clock on/off.
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void standby_spi_hclkonoff(__u32 spi_no, __u32 onoff)
{
    __u32   rval = CCMU_REG_AHBMOD;

    if (spi_no == 0)
    {
        rval &= ~(1 << 19);
        if (onoff)
        {
            rval |= 1 << 19;
        }
    }
    else
    {
        rval &= ~(1 << 20);
        if (onoff)
        {
            rval |= 1 << 20;
        }
    }
    CCMU_REG_AHBMOD = rval;
}


/*
*********************************************************************************************************
*                                   STANDBY SPI INITIATE
*
* Description: standby spi initiate.
*
* Arguments  : group    group number.
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void standby_spi_init(__u32 group)
{
    __u32   spi_no = group == SPI_GROUPA ? 0 : 1;
    spic_reg_t  *spi_reg = (spic_reg_t *)(SPIC_REGS_BASE + spi_no * SPI_OFFSET);
    __u32   cfg;

    //config pin mapping
    standby_spi_pincfg(group, 1);
    //open ahb clock
    standby_spi_hclkonoff(spi_no, 1);

    //initial controller
    cfg = (1 << 15) | (1 << 11) | (0x03 << 8) | (1 << 4) | (0x03 << 2) | (1 << 1) | (1 << 0);
    spi_reg->ctl = cfg;

    //disable dma and interrupt
    spi_reg->dmactl = 0;
    spi_reg->intctl = 0;
    //clock divider, hclk=32000, so bypass
    spi_reg->clkratectl = 0;
}


/*
*********************************************************************************************************
*                                   STANDBY SPI EXIT
*
* Description: standby spi exit.
*
* Arguments  : group    spi group number.
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void standby_spi_exit(__u32 group)
{
    __u32   spi_no = (group == SPI_GROUPA) ? 0 : 1;

    standby_spi_hclkonoff(spi_no, 0);
    standby_spi_pincfg(group, 0);
}


/*
*********************************************************************************************************
*                                   STANDBY SPI CONFIGURATION
*
* Description: standby spi configuration.
*
* Arguments  : group
*              mode
*              cs
*              clk
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void standby_spi_cfg(__u32 group, __u32 mode, __u32 cs, __u32 clk)
{
    __u32   spi_no = (group == SPI_GROUPA) ? 0 : 1;
    spic_reg_t  *spi_reg = (spic_reg_t *)(SPIC_REGS_BASE + spi_no * SPI_OFFSET);
    __u32   rval;

    rval = spi_reg->ctl;
    rval &= ~(0x03 << 2);
    rval |= mode << 2;
    rval &= ~(0x03 << 12);
    rval |= cs << 12;
    spi_reg->ctl = rval;

    if (clk >= 12000000)
    {
        spi_reg->clkratectl = (1 << 12) | 0x2;
    }
    else
    {
        spi_reg->clkratectl = 0;
    }
}


/*
*********************************************************************************************************
*                                   STANDBY SPI TX/RX
*
* Description: standby spi tx/rx data.
*
* Arguments  : group
*              tcnt
*              tbuf
*              rcnt
*              rbuf
*
* Returns    : result
*
* Note       :
*********************************************************************************************************
*/
__s32 standby_spi_txrx(__u32 group, __u32 tcnt, __u8 *tbuf, __u32 rcnt, __u8 *rbuf)
{
    __u32   spi_no = group == SPI_GROUPA ? 0 : 1;
    spic_reg_t  *spi_reg = (spic_reg_t *)(SPIC_REGS_BASE + spi_no * SPI_OFFSET);
    volatile unsigned char *spi_txfifo = (volatile unsigned char *) & (spi_reg->txdata);
    volatile unsigned char *spi_rxfifo = (volatile unsigned char *) & (spi_reg->rxdata);
    __u32   i;
    __u32   time = 0xffff;

    //clear flags
    spi_reg->status |= 0xffff;
    //start transmission
    spi_reg->burstcount = (tcnt << 16) | (tcnt + rcnt);
    spi_reg->ctl |= 1 << 10;

    //tx
    for (i = 0; i < tcnt; i++)
    {
        time = 0xffff;
        while(((spi_reg->status >> 20) & 0xf) == SPI_FIFO_SIZE)
        {
            time--;
            if (time == 0)
                goto _err_;
        }
        *spi_txfifo = tbuf[i];
    }

    //rx
    for (i = 0; i < rcnt; i++)
    {
        time = 0xffff;
        while(((spi_reg->status >> 16) & 0xf) == 0)
        {
            time--;
            if (time == 0)
                goto _err_;
        }
        rbuf[i] = *spi_rxfifo;
    }

    //wait transfer conplete
    time = 0xffff;
    while(!(spi_reg->status & 0x2000))
    {
        time--;
        if (time == 0)
        {
            goto _err_;
        }
    }
    if (spi_reg->status & 0xff1818)
    {
        goto _err_;
    }

    if(spi_reg->ctl & (1 << 10))
    {
        goto _err_;
    }

    spi_reg->status |= 0xffff;
    return EPDK_OK;

_err_:
    spi_reg->status |= 0xffff;
    return EPDK_FAIL;
}


#endif  //ALLOW_USREVENT_WAKEUP_STANDBY


