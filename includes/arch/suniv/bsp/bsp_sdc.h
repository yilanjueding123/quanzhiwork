/*
****************************************************************************************************
*                                               MELIS
*                               the Easy Portable/Player Develop Kits
*                                           SDMMC Module
*
*                           (c) Copyright 2011-2014, All winners Co,Ld.
*                                       All Rights Reserved
*
* File    : bsp_sdc.h
* By      : james.deng
* Version : 1.0.0
* Date    : 2011-11-23
* Descript: the header of sdmmc module.
* Update  : <date>          <author>            <version>           <notes>
*           2011-11-23      james.deng          1.0.0               build the file.
****************************************************************************************************
*/

#ifndef __BSP_SDC_H__
#define __BSP_SDC_H__


// the count of sdcard controller
#define SDC_COUNT                       2

// sd clock source definition
typedef enum __SDC_CLK_SRC
{
    SDC_CLK_SRC_SDRAM_PLL_M,
    SDC_CLK_SRC_SDRAM_PLL_P,
    SDC_CLK_SRC_SATA_PLL_M,
    SDC_CLK_SRC_SATA_PLL_2
} __sdc_clk_src_e;

/*
****************************************************************************************************
*                                       BSP_SDC_Init
*
* Description: Init SD controller.
*
* Arguments  : card_no      : the card no.
*              dma_use      : 0 indicate not use dma, 1 indicate use dma.
*              int_use      : 0 indicate not use interrup, 1 indicate use interrupt.
*              bus_width    : 0 indicate normal card, 1 indicate 1 line card.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_SDC_Init(__u32 card_no, __u32 dma_use, __u32 int_use, __u32 bus_width);

/*
****************************************************************************************************
*                                       BSP_SDC_Exit
*
* Description: Exit SD controller.
*
* Arguments  : card_no      : the card no.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_SDC_Exit(__u32 card_no);

/*
****************************************************************************************************
*                                       BSP_SDC_AttachCard
*
* Description: Init card.
*
* Arguments  : card_no      : the card no.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_SDC_AttachCard(__u32 card_no);

/*
****************************************************************************************************
*                                       BSP_SDC_RemoveCard
*
* Description: Exit card.
*
* Arguments  : card_no      : the card no.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_SDC_RemoveCard(__u32 card_no);

/*
****************************************************************************************************
*                                       BSP_SDC_BlockRead
*
* Description: Read number of block from card.
*
* Arguments  : card_no      : the card no.
*              blk          : the start block no.
*              nblk         : number of block to be read.
*              buf          : buffer receives data, alloc by caller.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_SDC_BlockRead(__u32 card_no, __u32 blk, __u32 nblk, void *buf);

/*
****************************************************************************************************
*                                       BSP_SDC_BlockWrite
*
* Description: Write number of block to card.
*
* Arguments  : card_no      : the card no.
*              blk          : the start block no.
*              nblk         : number of block to be read.
*              buf          : buffer receives data, alloc by caller.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_SDC_BlockWrite(__u32 card_no, __u32 blk, __u32 nblk, const void *buf);

/*
****************************************************************************************************
*                                       BSP_SDC_Clock_Change
*
* Description: Clock change function.
*
* Arguments  : card_no      : the card no.
*              change_sclk  : the clock source of sd.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_SDC_ClockChange(__u32 card_no, __sdc_clk_src_e change_sclk);

#endif // __BSP_SDC_H__
