/*
*********************************************************************************************************
*											        eBase
*						                the Abstract of Hardware
*									           the OAL of CSP
*
*						        (c) Copyright 2006-2010, AW China
*											All	Rights Reserved
*
* File    	: 	csp.c
* Date		:	2011-01-20
* By      	: 	holigun
* Version 	: 	V1.00
* History     :
*   aw1619的中断表，中断源
*********************************************************************************************************
*/

#ifndef	_CSP_INTC_PARA_H_
#define	_CSP_INTC_PARA_H_


#define CSP_IRQ_SOUCE_COUNT     (64)

//=======================================================================================================


/* mask */
#define INTC_IRQNO_FIQ           0

#define INTC_IRQNO_UART0         1
#define INTC_IRQNO_UART1         2
#define INTC_IRQNO_UART2         3

#define INTC_IRQNO_SPDIF         5

#define INTC_IRQNO_IR            6
#define INTC_IRQNO_TWI0          7
#define INTC_IRQNO_TWI1          8
#define INTC_IRQNO_TWI2          9

#define INTC_IRQNO_SPI0          10
#define INTC_IRQNO_SPI1          11

#define INTC_IRQNO_TIMER0        13
#define INTC_IRQNO_TIMER1        14
#define INTC_IRQNO_TIMER2        15
#define INTC_IRQNO_TIMER2345     15
#define INTC_IRQNO_WATCHDOG      16

#define INTC_IRQNO_RSB           17
#define INTC_IRQNO_DMA           18

#define INTC_IRQNO_TP            20
#define INTC_IRQNO_ADDA          21
#define INTC_IRQNO_LRADC         22
#define INTC_IRQNO_SDMMC0        23
#define INTC_IRQNO_SDMMC1        24

#define INTC_IRQNO_USB0          26

#define INTC_IRQNO_TVENC         28
#define INTC_IRQNO_TVDEC         27
#define INTC_IRQNO_TCON0         29
#define INTC_IRQNO_DE_SCALE0     30
#define INTC_IRQNO_DE_IMAGE0     31
#define INTC_IRQNO_CSI           32
#define INTC_IRQNO_DEINTERLACE   33

#define INTC_IRQNO_VE            34
#define INTC_IRQNO_DAUDIO        35
#define INTC_IRQNO_PIOD          38
#define INTC_IRQNO_PIOE          39
#define INTC_IRQNO_PIOF          40

#define INTC_IRQNO_SOFTIRQ0      52
//=======================================================================================================

#define INTC_NMI_TRIGGER_MOD_LOW_LEVEL		0x00
#define INTC_NMI_TRIGGER_MOD_HIGH_LEVEL		0x01
#define INTC_NMI_TRIGGER_MOD_NEGATIVE_EDGE	0x02
#define INTC_NMI_TRIGGER_MOD_POSITIVE_EDGE	0x03
#define INTC_NMI_TRIGGER_MOD_MAX			0x03
//=======================================================================================================





#endif	//_CSP_INTC_PARA_H_
