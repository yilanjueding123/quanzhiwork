/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                Standby Module
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : standby_pio.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-5-11
* Descript: PIO register operations.
* Update  : date                auther      ver     notes
*           2011-5-11 9:59:32   Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __STANDBY_PIO_H__
#define __STANDBY_PIO_H__

#include "standby_reg.h"

/* offset */
#define PIOC_REG_o_CFG0      0x00
#define PIOC_REG_o_CFG1      0x04
#define PIOC_REG_o_CFG2      0x08
#define PIOC_REG_o_CFG3      0x0C
#define PIOC_REG_o_DATA      0x10
#define PIOC_REG_o_DRV0      0x14
#define PIOC_REG_o_DRV1      0x18
#define PIOC_REG_o_PUL0      0x1C
#define PIOC_REG_o_PUL1      0x20
#define PIOC_REG_o_INT_CFG0  0x200
#define PIOC_REG_o_INT_CFG1  0x204
#define PIOC_REG_o_INT_CFG2  0x208
#define PIOC_REG_o_INT_CFG3  0x20C
#define PIOC_REG_o_INT_CTL   0x210
#define PIOC_REG_o_INT_STA   0x214
#define PIOC_REG_o_INT_DEB   0x218
#define PIOC_REG_o_PAD_DRV   0x220
#define PIOC_REG_o_PAD_PUL   0x224

#define GPIO_GROUP_A         (0)
#define GPIO_GROUP_C         (2)
#define GPIO_GROUP_D         (3)
#define GPIO_GROUP_E         (4)

/* registers */
#define PIOC_REG_CFG0(port)  STANDBY_RREG(PIOC_REG_BASE + 0x24 * port + PIOC_REG_o_CFG0)
#define PIOC_REG_CFG1(port)  STANDBY_RREG(PIOC_REG_BASE + 0x24 * port + PIOC_REG_o_CFG1)
#define PIOC_REG_CFG2(port)  STANDBY_RREG(PIOC_REG_BASE + 0x24 * port + PIOC_REG_o_CFG2)
#define PIOC_REG_CFG3(port)  STANDBY_RREG(PIOC_REG_BASE + 0x24 * port + PIOC_REG_o_CFG3)
#define PIOC_REG_DATA(port)  STANDBY_RREG(PIOC_REG_BASE + 0x24 * port + PIOC_REG_o_DATA)
#define PIOC_REG_DRV0(port)  STANDBY_RREG(PIOC_REG_BASE + 0x24 * port + PIOC_REG_o_DRV0)
#define PIOC_REG_DRV1(port)  STANDBY_RREG(PIOC_REG_BASE + 0x24 * port + PIOC_REG_o_DRV1)
#define PIOC_REG_PUL0(port)  STANDBY_RREG(PIOC_REG_BASE + 0x24 * port + PIOC_REG_o_PUL0)
#define PIOC_REG_PUL1(port)  STANDBY_RREG(PIOC_REG_BASE + 0x24 * port + PIOC_REG_o_PUL1)
#define PIOC_REG_INT_CFG0    STANDBY_RREG(PIOC_REG_BASE + PIOC_REG_o_INT_CFG0          )
#define PIOC_REG_INT_CFG1    STANDBY_RREG(PIOC_REG_BASE + PIOC_REG_o_INT_CFG1          )
#define PIOC_REG_INT_CFG2    STANDBY_RREG(PIOC_REG_BASE + PIOC_REG_o_INT_CFG2          )
#define PIOC_REG_INT_CFG3    STANDBY_RREG(PIOC_REG_BASE + PIOC_REG_o_INT_CFG3          )
#define PIOC_REG_INT_CTL     STANDBY_RREG(PIOC_REG_BASE + PIOC_REG_o_INT_CTL           )
#define PIOC_REG_INT_STA     STANDBY_RREG(PIOC_REG_BASE + PIOC_REG_o_INT_STA           )
#define PIOC_REG_INT_DEB     STANDBY_RREG(PIOC_REG_BASE + PIOC_REG_o_INT_DEB           )
#define PIOC_REG_PAD_DRV     STANDBY_RREG(PIOC_REG_BASE + PIOC_REG_o_PAD_DRV           )
#define PIOC_REG_PAD_PUL     STANDBY_RREG(PIOC_REG_BASE + PIOC_REG_o_PAD_PUL           )


#define PIO_REG_CFG(n, i)    ((volatile unsigned int *)(PIOC_REG_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x00))
#define PIO_REG_DLEVEL(n, i) ((volatile unsigned int *)(PIOC_REG_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x14))
#define PIO_REG_PULL(n, i)   ((volatile unsigned int *)(PIOC_REG_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x1C))
#define PIO_REG_DATA(n) 	 ((volatile unsigned int *)(PIOC_REG_BASE + ((n)-1)*0x24 + 0x10))

#define PIO_REG_CFG_VALUE(n, i)    STANDBY_RREG(PIOC_REG_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x00)
#define PIO_REG_DLEVEL_VALUE(n, i) STANDBY_RREG(PIOC_REG_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x14)
#define PIO_REG_PULL_VALUE(n, i)   STANDBY_RREG(PIOC_REG_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x1C)
#define PIO_REG_DATA_VALUE(n) 	   STANDBY_RREG(PIOC_REG_BASE + ((n)-1)*0x24 + 0x10)
#endif  //__STANDBY_PIO_H__
