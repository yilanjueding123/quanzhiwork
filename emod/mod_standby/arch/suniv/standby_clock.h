/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                               standby module
*
*                                   (c) Copyright 2008-2009, kevin China
*                                            All Rights Reserved
*
* File    : standby_clock.h
* By      : kevin
* Version : V1.0
* Date    : 2009-6-16 17:33
*********************************************************************************************************
*/
#ifndef _STANDBY_CLOCK_H_
#define _STANDBY_CLOCK_H_

#include "ePDK.h"
#include "standby_cfg.h"
#include "standby_reg.h"


struct sun4i_clk_div_t
{
    __u32   cpu_div: 4;     //division of cpu clock, divide core_pll
    __u32   ahb1_div: 4;     //division of ahb clock, divide axi clock
    __u32   apb_div: 4;     //division of apb clock, divide ahb clock
    __u32   reserved: 16;
};

struct pll_factor_t
{
    __u8    FactorN;
    __u8    FactorK;
    __u8    FactorM;
    __u8    FactorP;
    __u32   Pll;
};

typedef struct ___CCMU_PLL1_CORE_REG0000
{
    //PLL1_CORE output freq = (24*N*K)/(M*P)
    __u32   FactorM: 2;         //bit0,  PLL1 Factor M, M = FactorM+1
    __u32   reserved0: 2;       //bit2,  bit3 reserved
    __u32   FactorK: 2;         //bit4,  PLL1 factor K, K = FactorK+1
    __u32   reserved1: 2;       //bit6,  reserved
    __u32   FactorN: 5;         //bit8,  PLL1 Factor N, N = FactorN+1
    __u32   reserved2: 3;       //bit13, reserved
    __u32   PLLDivP: 2;         //bit16, PLL1 output external divider P,P=PLLDivP+1
    __u32   reserved3: 10;      //bit18, reserved
    __u32   IsLocked: 1;        //bit28, it indicates that the PLL has been stable
    __u32   reserved4: 2;       //bit18, reserved
    __u32   PLLEn: 1;           //bit31, 0-disable, 1-enable, (24Mhz*N*K)/(M*P)
} __ccmu_pll1_core_reg0000_t;


typedef struct __CCMU_PLL2_AUDIO_REG0008
{
    //PLL1_AUDIO output freq = (24*N*2)/M
    __u32   PreDivM: 5;         //bit0,  Pre-div factor(M=Factor+1)
    __u32   reserved0: 3;       //bit5,  reserved
    __u32   FactorN: 7;         //bit8,  PLL2 factor N,
    __u32   reserved1: 9;       //bit15, reserved
    __u32   PLLSdmEn: 1;        //bit24, PLL_SDM_EN
    __u32   reserved2: 3;       //bit25, reserved
    __u32   IsLocked: 1;        //bit28, PLL2 is stable
    __u32   reserved3: 2;       //bit29, reserved
    __u32   PLLEn: 1;           //bit31, PLL2 enable
} __ccmu_pll2_audio_reg0008_t;


typedef struct __CCMU_PLL3_VIDEO_REG0010
{
    __u32   PreDivM: 4;         //bit0,  Pre-div factor(M=Factor+1)
    __u32   reserved0: 4;       //bit4, bit5,bit6,bit7 reserved
    __u32   FactorN: 7;         //bit8,  PLL3 factor N,
    __u32   reserved1: 5;       //bit15, reserved
    __u32   PLLSdmEn: 1;        //bit20, PLL_SDM_EN
    __u32   reserved2: 3;       //bit21, reserved
    __u32   ModeSel: 1;         //bit24,
    __u32   FracClkOut: 1;      //bit25,
    __u32   reserved3: 2;       //bit26,bit27reserved
    __u32   IsLocked: 1;        //bit28, PLL3 is stable
    __u32   reserved4: 1;       //bit29, reserved
    __u32   PLLMode: 1;         //bit30, PLL_MODE; 0:Manual Mode; 1:Auto mode
    __u32   PLLEn: 1;           //bit31, PLL3 enable
} __ccmu_pll3_video_reg0010_t;


typedef struct __CCMU_PLL4_VE_REG0018
{
    __u32   PreDivM: 4;         //bit0,  Pre-div factor(M=Factor+1)
    __u32   reserved0: 4;       //bit4,  reserved
    __u32   FactorN: 7;         //bit8,  PLL4 factor N,
    __u32   reserved1: 9;       //bit15, reserved
    __u32   ModeSel: 1;         //bit24,
    __u32   FracClkOut: 1;      //bit25,
    __u32   reserved2: 2;       //bit26,bit27
    __u32   IsLocked: 1;        //bit28, PLL4 is stable
    __u32   reserved3: 2;       //bit29, reserved
    __u32   PLLEn: 1;           //bit31, PLL4 enable
} __ccmu_pll4_ve_reg0018_t;


typedef struct __CCMU_PLL5_DDR_REG0020
{
    //PLLout = 24*N*K/M
    __u32   FactorM: 2;          //bit0,  PLL5 factor M
    __u32   reserved0: 2;        //bit1,  reserved
    __u32   FactorK: 2;          //bit4,  PLL5 factor K
    __u32   reserved1: 2;        //bit6,  reserved
    __u32   FactorN: 5;          //bit8,  PLL5 factor N
    __u32   reserved2: 7;        //bit13-bit19, reserved
    __u32   pll_ddr_cfg_update: 1; //bit20, pll_ddr_cfg_update
    __u32   reserved3: 3;        //bit21,bit22,bit23 reserved
    __u32   SigmaDeltaEn: 1;     //bit24, Sdram sigma delta enable
    __u32   reserved4: 3;        //bit25,  reserved
    __u32   IsLocked: 1;         //bit28, PLL5 is stable
    __u32   reserved5: 2;        //bit29, bit30, reserved
    __u32   PLLEn: 1;            //bit31, PLL5 Enable
} __ccmu_pll5_ddr_reg0020_t;


typedef struct __CCMU_PLL6_PERIPH_REG0028
{
    //PLL output = 24*N*K
    __u32   FactorM: 2;         //bit0,  PLL6 factor M
    __u32   reserved0: 2;       //bit2,  reserved
    __u32   FactorK: 2;         //bit4,  PLL6 factor K
    __u32   reserved1: 2;       //bit6,  reserved
    __u32   FactorN: 5;         //bit8,  PLL6 factor N
    __u32   reserved2: 3;       //bit13,  reserved
    __u32   PLL24MPostDiv: 2;   //bit16
    __u32   PLL24MOutEn: 1;     //bit18
    __u32   reserved3: 9;       //bit19,  reserved
    __u32   IsLocked: 1;        //bit28, PLL5 is stable
    __u32   reserved4: 2;       //bit29,  reserved
    __u32   PLLEn: 1;           //bit31, PLL6 enable
} __ccmu_pll6_periph_reg0028_t;


typedef struct __CCMU_CPU_CLK_SEL_REG0050
{
    __u32   reserved0: 16;      //bit0,   reserved
    __u32   CpuClkSrcSel: 2;    //bit16
    __u32   reserved1: 14;      //bit18,  reserved
} __ccmu_cpu_clk_sel_reg0050_t;


typedef struct __CCMU_AMBA_CLK_RATIO_REG0054
{
    __u32   reserved0: 4;       //bit0,  reserved
    __u32   AHB1ClkRatio: 2;    //bit4,  AHB1 clock divide ration,  00-1, 01-2, 10-4, 11-8
    __u32   AHB1PreClkDiv: 2;   //bit6,  AHB1 pre-divide ratio,  00-1, 01-2, 10-3, 11-4
    __u32   APBClkRatio: 2;     //bit8,  APB clock divide ration,  APB clock source is AHB1 clock, 00-2, 01-2, 10-4, 11-8
    __u32   reserved1: 2;       //bit10, reserved
    __u32   AHB1ClkSrcSel: 2;   //bit12, 00:LOSC; 01:OSC24M; 10:CPUCLK; 11:PLL_PERIPH/AHB1_PRE_DIV
    __u32   reserved2: 2;       //bit14, reserved
    __u32   HCLK_C_DIV: 2;      //bit16, HCLK_C clock divide ratio,the clock source is the cpuclk
    __u32   reserved3: 14;      //bit18, reserved
} __ccmu_amba_clk_ratio_reg0054_t;


typedef struct __CCMU_AHBCLK_GATE0_REG0060
{
    __u32   reserved0: 6;       //bit0, reserved
    __u32   DmaGate: 1;         //bit6,  gating AHB clock for DMA, 0-mask, 1-pass
    __u32   reserved1: 1;       //bit7,  gating AHB clock for BIST, 0-mask, 1-pass
    __u32   Sdmmc0Gate: 1;      //bit8,  gating AHB clock for SD/MMC0, 0-mask, 1-pass
    __u32   Sdmmc1Gate: 1;      //bit9,  gating AHB clock for SD/MMC1, 0-mask, 1-pass
    __u32   reserved2: 4;       //bit10, gating AHB clock for SD/MMC2, 0-mask, 1-pass
    __u32   SdramGate: 1;       //bit14, gating AHB clock for SDRAM, 0-mask, 1-pass
    __u32   reserved3: 5;       //bit15, reserved
    __u32   Spi0Gate: 1;        //bit20, gating AHB clock for SPI0, 0-mask, 1-pass
    __u32   Spi1Gate: 1;        //bit21, gating AHB clock for SPI1, 0-mask, 1-pass
    __u32   reserved4: 2;       //bit22, reserved
    __u32   UsbOtgGate: 1;      //bit24, gating AHB clock for PATA, 0-mask, 1-pass
    __u32   reserved5: 7;       //bit25, reserved
} __ccmu_ahbclk_gate0_reg0060_t;


typedef struct __CCMU_AHBCLK_GATE1_REG0064
{
    __u32   VeGate: 1;          //bit0,  gating AHB clock for VE, 0-mask, 1-pass
    __u32   reserved0: 3;       //bit1,  reserved
    __u32   Lcd0Gate: 1;        //bit4,  gating AHB clock for LCD0, 0-mask, 1-pass
    __u32   DeInterlaceGate: 1; //bit5,  gating AHB clock for deinterlace
    __u32   reserved1: 2;       //bit6,  reserved
    __u32   Csi0Gate: 1;        //bit8,  gating AHB clock for CSI0, 0-mask, 1-pass
    __u32   TvdGate: 1;         //bit9,  gating AHB clock for TVD
    __u32   TveGate: 1;         //bit10, reserved
    __u32   reserved2: 1;       //bit11, reserved
    __u32   DeBe0Gate: 1;       //bit12, gating AHB clock for DE-BE0, 0-mask, 1-pass
    __u32   reserved3: 1;       //bit13, reserved
    __u32   DeFe0Gate: 1;       //bit14, gating AHB clock for DE-FE0, 0-mask, 1-pass
    __u32   reserved4: 17;      //bit15,
} __ccmu_ahbclk_gate1_reg0064_t;


typedef struct __CCMU_APB0CLK_GATE_REG0068
{
    __u32   AddaGate: 1;        //bit0,   gating APB clock for audio codec, 0-mask, 1-pass
    __u32   SpdifGate: 1;       //bit1,   gating APB clock for SPDIF, 0-mask, 1-pass
    __u32   CIRGate: 1;         //bit2,   gating APB clock for CIR
    __u32   RSBGate: 1;         //bit3,   gating APB clock for RSB, 0-mask, 1-pass
    __u32   reserved0: 8;       //bit4,   reserved
    __u32   DAudioGate: 1;      //bit12,  gating APB clock for DAUDIO, 0-mask, 1-pass
    __u32   reserved1: 3;       //bit13,
    __u32   Twi0Gate: 1;        //bit16,  gating APB clock for TWI0, 0-mask, 1-pass
    __u32   Twi1Gate: 1;        //bit17,  twi1 gate
    __u32   Twi2Gate: 1;        //bit18,  twi2 gate
    __u32   reserved2: 1;       //bit19,  reserved
    __u32   Uart0Gate: 1;       //bit20,  gating APB clock for uart0
    __u32   Uart1Gate: 1;       //bit21,  gating APB clock for uart1
    __u32   Uart2Gate: 1;       //bit22,  gating APB clock for uart1
    __u32   reserved3: 9;       //bit23,  reserved
} __ccmu_apb0clk_gate_reg0068_t;


typedef struct __CCMU_SDMMC0_CLK_REG0088
{
    __u32   ClkDivRatioM: 4;    //bit0 clock divide ratioM, divided by (m+1), 1~16 ex.
    __u32   reserved0: 4;       //bit4, reserved
    __u32   OutClkPhaseCtr: 3;  //bit8, output clock phase delay control.
    __u32   reserved1: 5;       //bit11, reserved
    __u32   ClkDivRatioN: 2;    //bit16  clock divide ratioN, divided by 2^N, the divider is 1,2,4,8
    __u32   reserved2: 2;       //bit18, reserved
    __u32   SampleClkPhaseCtr: 3; //bit20, sample clock phase delay control.
    __u32   reserved3: 1;       //bit23, reserved
    __u32   ClkSrcSel: 2;       //bit24, 00:OSC24M; 01 PLL_PERIPH;
    __u32   reserved4: 5;       //bit26-bit30, reserved
    __u32   SpecialClkGate: 1;  //bit31, the special clock = ClockSource/DividerN/DividerM
} __ccmu_sdmmc0_clk_reg0088_t;

typedef struct __CCMU_SDMMC1_CLK_REG008C
{
    __u32   ClkDivRatioM: 4;    //bit0 clock divide ratioM, divided by (m+1), 1~16 ex.
    __u32   reserved0: 4;       //bit4, reserved
    __u32   OutClkPhaseCtr: 3;  //bit8, output clock phase delay control.
    __u32   reserved1: 5;       //bit11, reserved
    __u32   ClkDivRatioN: 2;    //bit16  clock divide ratioN, divided by 2^N, the divider is 1,2,4,8
    __u32   reserved2: 2;       //bit18, reserved
    __u32   SampleClkPhaseCtr: 3; //bit20, sample clock phase delay control.
    __u32   reserved3: 1;       //bit23, reserved
    __u32   ClkSrcSel: 2;       //bit24, 00:OSC24M; 01 PLL_PERIPH;
    __u32   reserved4: 5;       //bit26-bit30, reserved
    __u32   SpecialClkGate: 1;  //bit31, the special clock = ClockSource/DividerN/DividerM
} __ccmu_sdmmc1_clk_reg008c_t;


typedef struct __CCMU_DAUDIO_CLK_REG00B0
{
    __u32   reserved0: 16;      //bit0,  reserved
    __u32   ClkSrcSel: 2;       //bit16, 00: PLL_AUDIO(8X); 01: PLL_AUDIO(8X)/2; 10:PLL_AUDIO(8X)/4; 11: PLL_AUDIO(8X)/8
    __u32   reserved1: 13;      //bit18, reserved
    __u32   SpecialClkGate: 1;  //bit31, reserved
} __ccmu_daudio_clk_reg00b0_t;

typedef struct __CCMU_SPDIF_CLK_REG00B4
{
    __u32   reserved0: 16;      //bit0,  reserved
    __u32   ClkSrcSel: 2;       //bit16, 00: PLL_AUDIO(8X); 01: PLL_AUDIO(8X)/2; 10:PLL_AUDIO(8X)/4; 11: PLL_AUDIO(8X)/8
    __u32   reserved1: 13;      //bit18, reserved
    __u32   SpecialClkGate: 1;  //bit31, reserved
} __ccmu_spdif_clk_reg00b4_t;

typedef struct __CCMU_CIR_CLK_REG00B8
{
    __u32   ClkDivRatioM: 4;    //bit0 clock divide ratioM, divided by (m+1), 1~16 ex.
    __u32   reserved0: 12;      //bit4, reserved
    __u32   ClkDivRatioN: 2;    //bit16  clock divide ratioN, divided by 2^N, the divider is 1,2,4,8
    __u32   reserved1: 6;       //bit18, reserved
    __u32   ClkSrcSel: 2;       //bit24, 00:LOSC; 01 OSC24M;
    __u32   reserved2: 5;       //bit26, reserved
    __u32   SpecialClkGate: 1;  //bit31, the special clock = ClockSource/DividerN/DividerM
} __ccmu_cir_clk_reg00b8_t;

typedef struct __CCMU_USB_CLK_REG00CC
{
    __u32   UsbPhy0Rst: 1;      //bit0,  USB PHY0 reset control, 0-reset valid, 1-reset invalid
    __u32   UsbSpecialClkGate: 1; //bit1,  USBPHY 24MHZ Clock Gating
    __u32   reserved2: 30;      //bit2,  reserved
} __ccmu_usb_clk_reg00cc_t;


typedef struct __CCMU_DRAM_CLK_GATE_REG0100
{
    __u32   VeGate: 1;          //bit0,  Gating dram clock for VE, 0-mask, 1-pass
    __u32   Csi0Gate: 1;        //bit1,  Gating dram clock for CSI0, 0-mask, 1-pass
    __u32   DeInterlaceGate: 1; //bit2,  Gating dram clock for DeInterlace
    __u32   TvdGate: 1;         //bit3,  Gating dram clock for TVD, 0-mask, 1-pass
    __u32   reserved0: 20;      //bit4,  reserved
    __u32   DeFeGate: 1;        //bit24, Gating dram clock for DE_FE0, 0-mask, 1-pass
    __u32   reserved1: 1;       //bit25,  reserved
    __u32   DeBeGate: 1;        //bit26, Gating dram clock for DE_BE0, 0-mask, 1-pass
    __u32   reserved2: 5;       //bit27-bit31, reserved
} __ccmu_dram_clk_gate_reg0100_t;



typedef struct __CCMU_DEBE_CLK_REG0104
{
    __u32   ClkDivRatioM: 4;    //bit0,  clock divide ratioM, divied by (m+1), 1~16 ex.
    __u32   reserved0: 20;      //bit4,  reserved
    __u32   ClkSrc: 3;          //bit24, clock source select, 000-PLL_VIDEO, 001-Reserved, 010-PLL_PERIPH, others-reserved
    __u32   reserved1: 4;       //bit27, reserved
    __u32   SpecialClkGate: 1;  //bit31, gating special clock, 0-clock off, 1-clock on,special clock = clock source/DividerM
} __ccmu_debe_clk_reg0104_t;

typedef struct __CCMU_DEFE_CLK_REG010C
{
    __u32   ClkDivRatioM: 4;    //bit0,  clock divide ratioM, divied by (m+1), 1~16 ex.
    __u32   reserved0: 20;      //bit4,  reserved
    __u32   ClkSrc: 3;          //bit24, clock source select, 000-PLL_VIDEO, 001-Reserved, 010-PLL_PERIPH, others-reserved
    __u32   reserved1: 4;       //bit27, reserved
    __u32   SpecialClkGate: 1;  //bit31, gating special clock, 0-clock off, 1-clock on,special clock = clock source/DividerM
} __ccmu_defe_clk_reg010c_t;


typedef struct __CCMU_LCD0_CLK_REG0118
{
    __u32   reserved0: 24;      //bit0,  reserved
    __u32   ClkSrcSel: 3;       //bit24, clock source select, 000-PLL_VIDEO(1x), 001-RESERVED, 010-PLL_VIDEO(2x), others:reserved
    __u32   reserved1: 4;       //bit27, reserved
    __u32   SpecialClkGate: 1;  //bit31, gating special clock, 0-clock off, 1-clock on
} __ccmu_lcd_clk_reg0118_t;


typedef struct __CCMU_DEINTERLACE_CLK_REG011C
{
    __u32   ClkDivRatioM: 4;    //bit0,  clock divide ratioM, divied by (m+1), 1~16 ex.
    __u32   reserved0: 20;      //bit4,  reserved
    __u32   ClkSrcSel: 3;       //bit24, clock source select, 000-PLL_VIDEO(1x), 001-RESERVED, 010-PLL_VIDEO(2x), others:reserved
    __u32   reserved1: 4;       //bit27, reserved
    __u32   SpecialClkGate: 1;  //bit31, gating special clock, 0-clock off, 1-clock on
} __ccmu_deinterlace_clk_reg011c_t;


typedef struct __CCMU_TVE_CLK_REG0120
{
    __u32   ClkDivRatioM: 4;    //bit0,  clock divide ratio, divided by (m+1), 1~16 ex.
    __u32   reserved0: 4;       //bit4,  reserved
    __u32   SpecClk1SrcSel: 1;  //bit8,  special clock source select,0:tve_SCLK2; 1: tve_SCLK2_divide by2
    __u32   reserved1: 1;       //bit9,  reserved
    __u32   reserved2: 5;       //bit10-bit14,  reserved
    __u32   SpecClk1Gate: 1;    //bit15, gating special clock, 0-clock off, 1-clock on
    __u32   reserved3: 8;       //bit16-bit23, 8bits, reserved
    __u32   SpecClk2SrcSel: 3;  //bit24, special clock source select,000:PLL_VIDEO(1X); 001:/; 010:PLL_VIDEO(2X)
    __u32   reserved4: 4;       //bit27, reserved
    __u32   SpecClk2Gate: 1;    //the special clock = clock/DividerM
} __ccmu_tve_clk_reg0120_t;

typedef struct __CCMU_TVD_CLK_REG0124
{
    __u32   ClkDivRatioM: 4;    //bit0,  clock divide ratio, divided by (m+1), 1~16 ex.
    __u32   reserved0: 20;      //bit4,  reserved
    __u32   ClkSrc: 3;          //bit24, clock source select, 000-PLL_VIDEO(1X), 001:OSC24M; 010:PLL_VIDEO(2X)
    __u32   reserved1: 4;       //bit27, reserved
    __u32   SpecClkGate: 1;     //bit31, gating special clock, 0-clock off, 1-clock on
} __ccmu_tvd_clk_reg0124_t;

/* CSI module clock type, used for CSI0/1 */
/* register address is 0x0134~0x0138 */
typedef struct __CCMU_CSI_CLK_REG0134
{
    __u32   MasterClkDivM: 4;   //bit0,  clock divide ratio, divided by (m+1), 1~16, ex.
    __u32   reserved0: 4;       //bit4,  reserved
    __u32   MasterClkSrc: 3;    //bit8,  clock source select, 000:PLL_VIDEO(1X); 101:OSC24M; others:reserved
    __u32   reserved1: 4;       //bit11-bit14, reserved
    __u32   MasterClkGate: 1;   //bit15, Gating special clock, 0-clock off, 1-clock on
    __u32   reserved2: 16;      //bit16, reserved
} __ccmu_csi_clk_reg0134_t;


typedef struct __CCMU_VE_CLK_REG013C
{
    __u32   reserved0: 31;      //bit1,  reserved
    __u32   SpecClkGate: 1;     //bit31, gating special clock for VE, 0-mask, 1-pass
} __ccmu_ve_clk_reg013c_t;


typedef struct __CCMU_ADDA_CLK_REG0140
{
    __u32   reserved0: 31;      //bit0,  reserved
    __u32   SpecClkGate: 1;     //bit31, Gating special clock, 0-clock off, 1-clock on
} __ccmu_adda_clk_reg0140_t;

typedef struct __CCMU_AVS_CLK_REG0144
{
    __u32   reserved0: 31;      //bit0,  reserved
    __u32   SpecClkGate: 1;     //bit31, Gating special clock, 0-clock off, 1-clock on
} __ccmu_avs_clk_reg0144_t;

typedef struct __CCMU_PLL_LOCK_TIME_REG0200
{
    __u32   PllLockTime: 16;    //bit0
    __u32   reserved0: 16;      //bit16,  reserved
} __ccmu_pll_lock_time_reg0200_t;


typedef struct __CCMU_PLL_CPU_LOCK_TIME_REG0204
{
    __u32   PllLockTime: 16;    //bit0
    __u32   reserved0: 16;      //bit16,  reserved
} __ccmu_pll_cpu_lock_time_reg0204_t;


typedef struct __CCMU_PLL_BIAS_REG0220
{
    __u32   DampFactorCtrl: 4;  //bit0
    __u32   reserved0: 4;       //bit4,  reserved
    __u32   LockCtrl: 3;        //bit8, pll lock time control
    __u32   reserved1: 5;       //bit11
    __u32   BiasCurCtrl: 5;     //bit16
    __u32   reserved2: 3;       //bit21
    __u32   VcoBiasCtrl: 4;     //bit24
    __u32   reserved3: 3;       //bit28
    __u32   VcoReset: 1;        //bit31  VCO reset in
} __ccmu_pll_bias_reg0220_t;


typedef struct __CCMU_PLL_AUDIO_BIAS_REG0224
{
    __u32   reserved0: 16;       //bit0, reserved
    __u32   BiasCur: 5;          //bit16
    __u32   reserved1: 3;       //bit121
    __u32   VcoBias: 5;         //bit24
    __u32   reserved2: 3;       //bit29
} __ccmu_pll_audio_bias_reg0224_t;

typedef struct __CCMU_PLL_VIDEO_BIAS_REG0228
{
    __u32   DampFactorCtrl: 3;   //bit0
    __u32   reserved0: 13;       //bit3, reserved
    __u32   BiasCtrl: 5;          //bit16
    __u32   reserved1: 3;       //bit121
    __u32   VcoBiasCtrl: 5;      //bit24
    __u32   reserved2: 3;        //bit29
} __ccmu_pll_video_bias_reg0228_t;


typedef struct __CCMU_PLL_VE_BIAS_REG022C
{
    __u32   DampFactorCtrl: 3;   //bit0
    __u32   reserved0: 13;       //bit3, reserved
    __u32   BiasCtrl: 5;         //bit16
    __u32   reserved1: 3;        //bit21
    __u32   VcoBiasCtrl: 5;      //bit24
    __u32   reserved2: 3;        //bit29
} __ccmu_pll_ve_bias_reg022c_t;

typedef struct __CCMU_PLL_DDR_BIAS_REG0230
{
    __u32   DampFactorCtrl: 4;   //bit0
    __u32   reserved0: 8;        //bit4, reserved
    __u32   VcoGainCtrl: 3;      //bit12,
    __u32   reserved1: 1;        //bit15
    __u32   BiasCurCtrl: 5;      //bit16; PLL Bias Current control
    __u32   reserved2: 3;        //bit21
    __u32   BandWidthCtrl: 1;    //bit24; 0:narrow; 1:wide
    __u32   VcoGainCtrlEn: 1;     //bit25
    __u32   reserved3: 2;        //bit26
    __u32   VcoBias: 4;           //VCO bias
} __ccmu_pll_ddr_bias_reg0230_t;

typedef struct __CCMU_PLL_PERIPH_BIAS_REG0234
{
    __u32   DampFactorCtrl: 2;   //bit0
    __u32   reserved0: 2;        //bit2, reserved
    __u32   BandWidthCtrl: 1;    //bit4; 0:narrow; 1:wide
    __u32   reserved1: 11;        //bit5
    __u32   BiasCurCtrl: 5;       //bit16
    __u32   reserved2: 3;        //bit21
    __u32   VcoBiasCtrl: 5;      //bit24
    __u32   reserved3: 3;        //bit29
} __ccmu_pll_periph_bias_reg0234_t;

typedef struct __CCMU_PLL_CPU_TUNING_REG0250
{
    __u32   c_b_out: 7;           //bit0; c-b-out[6:0]for verify
    __u32   c_od1: 1;             //bit7, c-reg-od1 for verify
    __u32   c_b_in: 7;            //bit8; c-b-in[6:0]for veryfy
    __u32   c_od: 1;              //bit15,c_reg-od for veryfy
    __u32   PllInitFreqCtrl: 7;   //bit16,
    __u32   VcoGainCtrl: 3;       //bit23
    __u32   VcoGainCtrlEn: 1;     //bit26
    __u32   BandWidthCtrl: 1;     //bit27
    __u32   reserved0: 4;         //bit28, reserved
} __ccmu_pll_cpu_tuning_reg0250_t;

typedef struct __CCMU_PLL_DDR_TUNING_REG0260
{
    __u32   b_out: 7;             //bit0; c-b-out[6:0]for verify
    __u32   od: 1;                //bit7, c-reg-od for verify
    __u32   b_in: 7;              //bit8; c-b-in[6:0]for veryfy
    __u32   od1: 1;               //bit15,c_reg-od1 for veryfy
    __u32   PllInitFreqCtrl: 7;   //bit16,
    __u32   VcoReset: 1;          //bit23
    __u32   VcoLockTimeCtrl: 3;   //bit24
    __u32   reserved0: 1;         //bit27, reserved
    __u32   Vreg1OutEn: 1;        //bit28
    __u32   reserved2: 3;         //bit29, reserved
} __ccmu_pll_ddr_tuning_reg0260_t;

typedef struct __CCMU_PLL_AUDIO_PATTERN_REG0284
{
    __u32   WaveBottom: 17;        //bit0; wave bottom
    __u32   Freq: 2;               //bit17; 00:31.5Khz, 01:32KHz; 10:32.5KHz;11:33KHz
    __u32   reserved0: 1;          //bit19
    __u32   WaveStep: 9;           //bit20 wave step
    __u32   SpreadFreqMode: 2;     //bit29 00:DC=0; 01:DC=1; 10:Triangular; 11:awmode
    __u32   SigmaDeltaPatternEn: 1; //bit31 ; sigma delta pattern enable
} __ccmu_pll_audio_pattern_reg0284_t;

typedef struct __CCMU_PLL_VIDEO_PATTERN_REG0288
{
    __u32   WaveBottom: 17;        //bit0; wave bottom
    __u32   Freq: 2;               //bit17; 00:31.5Khz, 01:32KHz; 10:32.5KHz;11:33KHz
    __u32   reserved0: 1;          //bit19
    __u32   WaveStep: 9;           //bit20 wave step
    __u32   SpreadFreqMode: 2;     //bit29 00:DC=0; 01:DC=1; 10:Triangular; 11:awmode
    __u32   SigmaDeltaPatternEn: 1; //bit31 ; sigma delta pattern enable
} __ccmu_pll_video_pattern_reg0288_t;

typedef struct __CCMU_PLL_DDR_PATTERN_REG0290
{
    __u32   WaveBottom: 17;        //bit0; wave bottom
    __u32   Freq: 2;               //bit17; 00:31.5Khz, 01:32KHz; 10:32.5KHz;11:33KHz
    __u32   reserved0: 1;          //bit19
    __u32   WaveStep: 9;           //bit20 wave step
    __u32   SpreadFreqMode: 2;     //bit29 00:DC=0; 01:DC=1; 10:Triangular; 11:awmode
    __u32   SigmaDeltaPatternEn: 1; //bit31 ; sigma delta pattern enable
} __ccmu_pll_ddr_pattern_reg0290_t;

typedef struct __CCMU_AHB_MODULE_RESET_0_REG02C0
{
    __u32   reserved0: 6;          //bit0
    __u32   DmaReset: 1;           //bit6
    __u32   reserved1: 1;          //bit7
    __u32   Sd0Reset: 1;           //bit8
    __u32   Sd1Reset: 1;           //bit9  sd1 AHB reset
    __u32   reserved2: 4;          //bit10
    __u32   SdramReset: 1;         //bit14  sdram AHB reset
    __u32   reserved3: 5;          //bit15; reserved
    __u32   Spi0Reset: 1;          //bit20; spi0 AHB reset
    __u32   Spi1Reset: 1;          //bit21
    __u32   reserved4: 2;          //bit22
    __u32   UsbOtgReset: 1;        //bit24
    __u32   reserved5: 7;          //bit25
} __ccmu_pll_ahb_rst_0_reg02c0_t;

typedef struct __CCMU_AHB_MODULE_RESET_1_REG02C4
{
    __u32   VeReset: 1;            //bit0    VE reset; 0:assert 1:de-assert
    __u32   reserved0: 3;          //bit1
    __u32   LcdReset: 1;           //bit4
    __u32   DeInterlaceReset: 1;   //bit5
    __u32   reserved1: 2;          //bit6  reserved
    __u32   CsiReset: 1;           //bit8
    __u32   TvdReset: 1;           //bit9
    __u32   TveReset: 1;           //bit10;
    __u32   reserved2: 1;          //bit11  reserved
    __u32   DeBeReset: 1;          //bit12
    __u32   reserved3: 1;          //bit13
    __u32   DeFeReset: 1;          //bit14
    __u32   reserved5: 17;          //bit15
} __ccmu_pll_ahb_rst_1_reg02c4_t;



typedef struct __CCMU_APB_MODULE_RESET_REG02D0
{
    __u32   Adda_D_Reset: 1;       //bit0    0:assert 1:de-assert
    __u32   SpdifReset: 1;         //bit1
    __u32   CirReset: 1;           //bit2
    __u32   RsbReset: 1;            //bit3
    __u32   reserved0: 8;          //bit4  reserved
    __u32   DAudioReset: 1;        //bit12
    __u32   reserved1: 3;          //bit13  reserved
    __u32   Twi0Reset: 1;          //bit16
    __u32   Twi1Reset: 1;          //bit17
    __u32   Twi2Reset: 1;          //bit18
    __u32   reserved2: 1;          //bit19  reserved
    __u32   Uart0Reset: 1;           //bit20;
    __u32   Uart1Reset: 1;           //bit21;
    __u32   Uart2Reset: 1;           //bit22;
    __u32   reserved3: 9;          //bit23, reserved
} __ccmu_pll_apb_rst_reg02d0_t;



typedef struct __STANDBY_CCU_REG
{
    volatile __ccmu_pll1_core_reg0000_t         Pll1Cpu;       //0x0000, PLL1 control
    volatile __u32                              Reserved0[1];   //0x0004,
    volatile __ccmu_pll2_audio_reg0008_t        Pll2Audio;      //0x0008, PLL2 control
    volatile __u32                              Reserved1[1];   //0x000C,
    volatile __ccmu_pll3_video_reg0010_t        Pll3Video;      //0x0010, PLL3 control
    volatile __u32                              Reserved2[1];    //0x0014,
    volatile __ccmu_pll4_ve_reg0018_t           Pll4Ve;          //0x0018, PLL4 control
    volatile __u32                              Reserved3[1];    //0x001C,
    volatile __ccmu_pll5_ddr_reg0020_t          Pll5DDR;         //0x0020, PLL5 control
    volatile __u32                              Reserved4[1];    //0x0024,
    volatile __ccmu_pll6_periph_reg0028_t       Pll6Periph;      //0x0028, PLL6 control
    volatile __u32                              Reserved5[9];    //0x002C--0x004C
    volatile __ccmu_cpu_clk_sel_reg0050_t       CpuClkSrcSel;    //0x0050, OSC24M control
    volatile __ccmu_amba_clk_ratio_reg0054_t    AmbaClkRatio;    //0x0054, AC328/AHB/APB0 divide ratio
    volatile __u32                              Reserved6[2];    //0x0058--0x005C, APB1 clock dividor
    volatile __ccmu_ahbclk_gate0_reg0060_t      AhbGate0;       //0x0060, AHB module clock gating 0
    volatile __ccmu_ahbclk_gate1_reg0064_t      AhbGate1;       //0x0064, AHB module clock gating 1
    volatile __ccmu_apb0clk_gate_reg0068_t      Apb0Gate;       //0x0068, APB0 module clock gating
    volatile __u32                              reserved7[7];   //0x006C-0x0084, reserved
    volatile __ccmu_sdmmc0_clk_reg0088_t        SdMmc0Clk;      //0x0088, SD/MMC0 module clock control
    volatile __ccmu_sdmmc1_clk_reg008c_t        SdMmc1Clk;      //0x008C, SD/MMC1 module clock control
    volatile __u32                              reserved8[8];   //0x0090-0x00AC, reserved
    volatile __ccmu_daudio_clk_reg00b0_t        DAudioClk;      //0x00B0, IR0 module clock control
    volatile __ccmu_spdif_clk_reg00b4_t         SpdifClk;       //0x00B4, IR1 module clock control
    volatile __ccmu_cir_clk_reg00b8_t           CirClk;         //0x00B8, IIS module clock control
    volatile __u32                              reserved9[4];   //0x00BC-0x00C8, reserved
    volatile __ccmu_usb_clk_reg00cc_t           UsbClk;         //0x00CC, USB module clock control
    volatile __u32                              reserved10[12];  //0x00D0-0x00FC, reserved
    volatile __ccmu_dram_clk_gate_reg0100_t     DramGate;        //0x0100, DRAM gating
    volatile __ccmu_debe_clk_reg0104_t          DeBe0Clk;        //0x0104, DE-BE 0 module clock control
    volatile __u32                              reserved11[1];   //0x0108, reserved
    volatile __ccmu_defe_clk_reg010c_t          DeFe0Clk;        //0x010C, DE-FE 0 module clock control
    volatile __u32                              reserved12[2];   //0x0110-0x0114, reserved
    volatile __ccmu_lcd_clk_reg0118_t           Lcd0Clk;         //0x0118, LCD0 CH0 module clock control
    volatile __ccmu_deinterlace_clk_reg011c_t   DeInterlaceClk; //0x011C, LCD1 CH0 module clock control
    volatile __ccmu_tve_clk_reg0120_t           TveClk;         //0x0120,tve clock registor
    volatile __ccmu_tvd_clk_reg0124_t           TvdClk;         //0x0124, tvd clock
    volatile __u32                              reserved13[3];  //0x0128-0x0130, reserved
    volatile __ccmu_csi_clk_reg0134_t           Csi0Clk;          //0x0134, CSI0 module clock control
    volatile __u32                              reserved14[1];    //0x0138, reserved
    volatile __ccmu_ve_clk_reg013c_t            VeClk;            //0x013C, VE module clock control
    volatile __ccmu_adda_clk_reg0140_t          AddaClk;          //0x0140, audio codec clock control
    volatile __ccmu_avs_clk_reg0144_t           AvsClk;           //0x0144, AVS module clock control
    volatile __u32                              reserved15[46];   //0x0148-0x01FC, reserved
    volatile __ccmu_pll_lock_time_reg0200_t     PllLockTime;      //0x0200, pll lock time
    volatile __ccmu_pll_cpu_lock_time_reg0204_t PllCpuLockTime;   //0x0204, pll cpu lock time
    volatile __u32                              reserved16[6];    //0x0208-0x021C, reserved
    volatile __ccmu_pll_bias_reg0220_t          CpuBias;          //0x0220, cpu bias
    volatile __ccmu_pll_audio_bias_reg0224_t    AudioBias;        //0x0224, Audio pll bias
    volatile __ccmu_pll_video_bias_reg0228_t    VideoBias;        //0x0228, Video pll bias
    volatile __ccmu_pll_ve_bias_reg022c_t       VeBias;           //0x022C, VE pll bias
    volatile __ccmu_pll_ddr_bias_reg0230_t      DdrBias;          //0x0230, DDR pll bias
    volatile __ccmu_pll_periph_bias_reg0234_t   PeriphBias;       //0x0234, periph pll bias
    volatile __u32                              reserved17[6];    //0x0238-0x024C, reserved
    volatile __ccmu_pll_cpu_tuning_reg0250_t    CpuTuning;        //0x0250, CpuTuning
    volatile __u32                              reserved18[3];    //0x0254-0x025C, reserved
    volatile __ccmu_pll_ddr_tuning_reg0260_t    DdrTuning;        //0x0260, DdrTuning
    volatile __u32                              reserved19[8];    //0x0264-0x0280, reserved
    volatile __ccmu_pll_audio_pattern_reg0284_t AudioPattern;     //0x0284
    volatile __ccmu_pll_video_pattern_reg0288_t VideoPattern;     //0x0288
    volatile __u32                              reserved20[1];    //0x028C, reserved
    volatile __ccmu_pll_ddr_pattern_reg0290_t   DdrPattern;       //0x0290
    volatile __u32                              reserved21[11];   //0x0294--0x02BC, reserved
    volatile __ccmu_pll_ahb_rst_0_reg02c0_t     AhbReset0;        //0x02C0, ahb software module reset0
    volatile __ccmu_pll_ahb_rst_1_reg02c4_t     AhbReset1;        //0x02C4  ahb software module reset1
    volatile __u32                              reserved22[2];    //0x02C8-0x02CC
    volatile __ccmu_pll_apb_rst_reg02d0_t       ApbReset;         //0x02D0    apb software module reset0

} __standby_ccu_reg_t;



__s32 standby_clk_core2losc(void);
__s32 standby_clk_core2hosc(void);
__s32 standby_clk_core2pll(void);
__s32 standby_clk_dramplldisable(void);
__s32 standby_clk_drampllenable(void);
__s32 standby_clk_coreplldisable(void);
__s32 standby_clk_corepllenable(void);
__s32 standby_clk_videoplldisable(void);
__s32 standby_clk_videopllenable(void);
__s32 standby_clk_veplldisable(void);
__s32 standby_clk_vepllenable(void);
__s32 standby_clk_hoscdisable(void);
__s32 standby_clk_hoscenable(void);


#endif  //_STANDBY_CLOCK_H_

