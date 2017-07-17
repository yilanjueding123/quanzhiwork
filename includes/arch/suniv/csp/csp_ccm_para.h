/*
*******************************************************************************
*           				eBase
*                 the Abstract of Hardware
*
*
*              (c) Copyright 2006-2010, ALL WINNER TECH.
*           								All Rights Reserved
*
* File     :  ccm_para.h
* Date     :  2010/11/27 12:19
* By       :  Sam.Wu
* Version  :  V1.00
* Description :
* Update   :  date      author      version
*
* notes    :
*******************************************************************************
*/
#ifndef _CSP_CCM_PARA_H_
#define _CSP_CCM_PARA_H_

enum CCM_CONST
{
    MHz                    = (1000 * 1000),
    FREQ_0                 = 0,
    FREQ_HOSC              = (24 * MHz),
    FREQ_LOSC              = (32768),
    FREQ_AUDIO_PLL_HIGH    =  24576000,
    FREQ_AUDIO_PLL_LOW     = 22579200,
};

typedef enum
{
    CSP_CCM_ERR_NONE,
    CSP_CCM_ERR_PARA_NULL,
    CSP_CCM_ERR_OSC_FREQ_CANNOT_BE_SET,
    CSP_CCM_ERR_PLL_FREQ_LOW,
    CSP_CCM_ERR_PLL_FREQ_HIGH,
    CSP_CCM_ERR_FREQ_NOT_STANDARD,
    CSP_CCM_ERR_CLK_NUM_NOT_SUPPORTED,
    CSP_CCM_ERR_DIVIDE_RATIO,
    CSP_CCM_ERR_CLK_IS_OFF,
    CSP_CCM_ERR_SRC_CLK_NOT_AVAILABLE,
    CSP_CCM_ERR_GET_CLK_FREQ,
    CSP_CCM_ERR_CLK_NO_INVALID,

    CSP_CCM_ERR_RESET_CONTROL_DENIED,
    CSP_CCM_ERR_NULL_PARA,
    CSP_CCM_ERR_PARA_VALUE,
} CSP_CCM_err_t;

/************************************************************************/
/* SYS CLK: system clocks, which are the source clocks of the chip
 * 3 kinds of system clock: oscillate, PLL output, and CPU/AHB/APB.
*Note: when the frequency of the system clock has been changed, the clock frequency of
*all the clocks sourced form it will changed. As a result, we must reconfigure the module
*clocks which  source clock is been changed!*/
/************************************************************************/
typedef enum _CSP_CCM_SYS_CLK
{

    CSP_CCM_SYS_CLK_HOSC,
    CSP_CCM_SYS_CLK_LOSC,

    CSP_CCM_SYS_CLK_CORE_PLL,
    CSP_CCM_SYS_CLK_VE_PLL,
    CSP_CCM_SYS_CLK_SDRAM_PLL,
    CSP_CCM_SYS_CLK_AUDIO_PLL_1X,
    CSP_CCM_SYS_CLK_AUDIO_PLL_2X,
    CSP_CCM_SYS_CLK_AUDIO_PLL_4X,
    CSP_CCM_SYS_CLK_AUDIO_PLL_8X,
    CSP_CCM_SYS_CLK_VIDEO_PLL_1X,
    CSP_CCM_SYS_CLK_VIDEO_PLL_2X,

    CSP_CCM_SYS_CLK_PERIPH_PLL,


    CSP_CCM_SYS_CLK_CPU,
    CSP_CCM_SYS_CLK_AHB,/*ahb1 */
    CSP_CCM_SYS_CLK_APB,
    CSP_CCM_SYS_CLK_SDRAM,

    CSP_CCM_SYS_CLK_TOTAL_NUM
} CSP_CCM_sysClkNo_t;


typedef enum _CSP_CCM_MOD_CLK_
{

    /*Clocks for MOD Devices*/
    CSP_CCM_MOD_CLK_SDC0,
    CSP_CCM_MOD_CLK_SDC1,
    CSP_CCM_MOD_CLK_DAUDIO,
    CSP_CCM_MOD_CLK_SPDIF,
    CSP_CCM_MOD_CLK_IR,
    CSP_CCM_MOD_CLK_USB_PHY,
    CSP_CCM_MOD_CLK_DRAM_GATING,
    CSP_CCM_MOD_CLK_DE_BE,
    CSP_CCM_MOD_CLK_DE_FE,
    CSP_CCM_MOD_CLK_LCDC,
    CSP_CCM_MOD_CLK_DEINTERLACE,
    CSP_CCM_MOD_CLK_TVE_CLK1,
    CSP_CCM_MOD_CLK_TVD_CLK1,
    CSP_CCM_MOD_CLK_TVE_CLK2,
    CSP_CCM_MOD_CLK_TVD_CLK2,
    CSP_CCM_MOD_CLK_CSI,
    CSP_CCM_MOD_CLK_VE,
    CSP_CCM_MOD_CLK_AUDIO_CODEC,
    CSP_CCM_MOD_CLK_AVS,/*synchro audio and video*/


    /*Clocks for AHB Gating Devices*/
    CSP_CCM_MOD_CLK_AHB_USB,
    CSP_CCM_MOD_CLK_AHB_SPI0,
    CSP_CCM_MOD_CLK_AHB_SPI1,
    CSP_CCM_MOD_CLK_AHB_SDRAMC,
    CSP_CCM_MOD_CLK_AHB_SDC0,
    CSP_CCM_MOD_CLK_AHB_SDC1,
    CSP_CCM_MOD_CLK_AHB_DMAC,
    CSP_CCM_MOD_CLK_AHB_DE_FE,
    CSP_CCM_MOD_CLK_AHB_DE_BE,
    CSP_CCM_MOD_CLK_AHB_TVE,/*tv encoder*/
    CSP_CCM_MOD_CLK_AHB_TVD,/*tv decoder*/
    CSP_CCM_MOD_CLK_AHB_CSI,
    CSP_CCM_MOD_CLK_AHB_DEINTERLACE,
    CSP_CCM_MOD_CLK_AHB_LCDC,
    CSP_CCM_MOD_CLK_AHB_VE,


    /*Clocks for the APB Gating Devices*/
    CSP_CCM_MOD_CLK_APB_UART0,
    CSP_CCM_MOD_CLK_APB_UART1,
    CSP_CCM_MOD_CLK_APB_UART2,
    CSP_CCM_MOD_CLK_APB_TWI0,
    CSP_CCM_MOD_CLK_APB_TWI1,
    CSP_CCM_MOD_CLK_APB_TWI2,
    CSP_CCM_MOD_CLK_APB_DAUDIO,
    CSP_CCM_MOD_CLK_APB_IR,
    CSP_CCM_MOD_CLK_APB_SPDIF,
    CSP_CCM_MOD_CLK_APB_AUDIO_CODEC,
    CSP_CCM_MOD_CLK_APB_RSB,/*reduced serial bus*/
    CSP_CCM_MOD_CLK_APB_AVS,/*no gating*/

    /*Clocks for DRAM devices, i.e., which clock source is Dram clock*/
    CSP_CCM_MOD_CLK_SDRAM_OUTPUT,
    CSP_CCM_MOD_CLK_SDRAM_DE_BE,
    CSP_CCM_MOD_CLK_SDRAM_DE_FE,
    CSP_CCM_MOD_CLK_SDRAM_TVD,
    CSP_CCM_MOD_CLK_SDRAM_DEINTERLACE,
    CSP_CCM_MOD_CLK_SDRAM_CSI,
    CSP_CCM_MOD_CLK_SDRAM_VE,

    CSP_CCM_MOD_CLK_TOTAL_NUM

} CSP_CCM_modClkNo_t;


typedef struct _CSP_CCM_MOD_CLK
{
    CSP_CCM_modClkNo_t     clkNo;//Must be CSP_CCM_MOD_CLK_xxx
    CSP_CCM_sysClkNo_t     srcClk;
    __u8                   status;
    __u8                   divideRatio;
    __u8                   resetIsValid;
} CSP_CCM_modClkPara_t;

/*********************************************************************
* TypeName	 :    		CSP_CCM_sysClkInfo_t
* Description: data structure to store the system clock information.
* Members    :
    @CSP_CCM_sysClkNo_t  clkId;    //clock number of this clock
    @CSP_CCM_sysClkNo_t  srcClkId;//clock number of the source clock, CSP_CCM_SYS_CLK_TOTAL_NUM if no source
    @char*               pName;  //name  of this system clock
    @u32                 freq;   //rate of this clock
* Note       : The clock status may be off or disabled!
*********************************************************************/
typedef struct _CSP_CCM_SysClkInfo
{
    CSP_CCM_sysClkNo_t  clkId;    //clock number of this clock
    CSP_CCM_sysClkNo_t  srcClkId;//clock number of the source clock, CSP_CCM_SYS_CLK_TOTAL_NUM if no source
    char               *pName;  //name  of this system clock
    u32                 freq;   //rate of this clock
} CSP_CCM_sysClkInfo_t;

/*********************************************************************
* TypeName	 :    		CSP_CCM_modClkInfo_t
* Description: data structure to store the system clock information.
* Members    :
    @CSP_CCM_modClkNo_t  clkId;//clock number of this clock
    @CSP_CCM_sysClkNo_t  srcClkId;//clock number of this clock
    @char*               pName;//name of this clock
    @u16                 divideRatio;//divide ratio
* Note       : The clock status may be off or disabled!
*********************************************************************/
typedef struct _CSP_CCM_modClkInfo
{
    CSP_CCM_modClkNo_t  clkId;
    CSP_CCM_sysClkNo_t  srcClkId;
    char               *pName;
    u16                 divideRatio;
} CSP_CCM_modClkInfo_t;

#endif //#ifndef _CSP_CCM_PARA_H_




