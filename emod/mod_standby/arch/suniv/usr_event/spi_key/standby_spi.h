/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Standby Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : standby_spi.h
* By      : Alex.Leaf
* Version : v1.0
* Date    : 2010-7-8 19:06
* Descript:
* Update  : date                auther      ver     notes
*********************************************************************************************************
*/
#ifndef __STANDBY_SPI_H__
#define __STANDBY_SPI_H__

#include "ePDK.h"
#include "standby_cfg.h"



#define SPI_FIFO_SIZE   8

#define SPI_GROUPA      0   //SPI0_DO-PIOC0, SPI0_DI-PIOC1, SPI0_CLK-PIOC2, SPI0_CS-PIOC6
#define SPI_GROUPB_0    1   //SPI1_CS0-PIOC3, SPI1_CLK-PIOC4, SPI1_DO-PIOC10, SPI1_DI-PIOC11
#define SPI_GROUPB_1    2   //SPI1_CS0-PIOA2, SPI1_CLK-PIOA3, SPI1_DO-PIOA4, SPI1_DI-PIOA5
#define SPI_GROUPB_2    3   //SPI1_CS0-PIOB11, SPI1_CLK-PIOB12, SPI1_DI-PIOB13, SPI1_DO-PIOB14, SPI1_CS1-PIOB15

typedef struct
{
    __u32       rxdata;
    __u32       txdata;
    __u32       ctl;
    __u32       intctl;
    __u32       status;
    __u32       dmactl;
    __u32       waitclkcount;
    __u32       clkratectl;
    __u32       burstcount;

} spic_reg_t;

extern void standby_spi_init(__u32 group);
extern void standby_spi_exit(__u32 group);
extern void standby_spi_cfg(__u32 group, __u32 mode, __u32 cs, __u32 clk);
extern __s32 standby_spi_txrx(__u32 group, __u32 tcnt, __u8 *tbuf, __u32 rcnt, __u8 *rbuf);

#endif  //__STANDBY_SPI_H__

