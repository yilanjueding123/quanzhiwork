/*
*********************************************
*
* File    : configs.h
* By      : dianguai14
* Version : v0.1
* Date    : 2013-06-25
* Descript: configuration of CPU
*********************************************
*/
#ifndef __CONFIGS_H
#define __CONFIGS_H

#define   AW1663_PLATFORM

/* -------------- runtime control -------------- */
//#define SYSTEM_SIMULATION
//#define FPGA_PLATFORM
#define CHIP_PLATFORM

//#define PMU_SUPPORT
/* -------------- Clock Configuration -------------- */
#ifdef SYSTEM_SIMULATION
#ifdef FPGA_PLATFORM
//FPGA Simualtion
#define PLL_CPU_CLK				(36000000)
#define PLL_AUDIO_CLK 			(0)
#define PLL_VIDEO_CLK			(36000000)
#define PLL_VE_CLK_MODE 		(1)	//0-fractional mode, 1-integer mode
#define PLL_VE_CLK				(36000000)
#define PLL_DDR_CLK				(36000000)
#define PLL_PERH_CLK			(36000000)
#define AHBPREDIV				(1) //(3) // just used when source is PLL_PERH_CLK/PREDIV, 0:/1, 1:/2, 2:/3, 3:/4,
#define AHBDIV					(1) // 0:/1, 1:/2, 2:/4, 3:/8
#define APBDIV					(1) //(4) // 0:/1, 1:/2, 2:/4, 3:/8
#define AHBCLK					(PLL_PERH_CLK/AHBPREDIV/AHBDIV)
#define APBCLK					(AHBCLK/APBDIV)
#define PLL_STABLE_TIME			(1)
#define CPUPLL_STABLE_TIME		(1)
#endif //#ifdef FPGA_PLATFORM

#ifdef CHIP_PLATFORM
//Chip Simualtion
#define PLL_CPU_CLK				(408000000)
#define PLL_AUDIO_CLK 			(0)
#define PLL_VIDEO_CLK			(297000000)
#define PLL_VE_CLK_MODE 		(1)	//0-fractional mode, 1-integer mode
#define PLL_VE_CLK				(297000000)
#define PLL_DDR_CLK				(150000000)
#define PLL_PERH_CLK			(600000000)
#define AHBPREDIV				(3) // just used when source is PLL_PERH_CLK/PREDIV, 0:/1, 1:/2, 2:/3, 3:/4,
#define AHBDIV					(1) // 0:/1, 1:/2, 2:/4, 3:/8
#define APBDIV					(4) // 0:/1, 1:/2, 2:/4, 3:/8
#define AHBCLK					(PLL_PERH_CLK/AHBPREDIV/AHBDIV)
#define APBCLK					(AHBCLK/APBDIV)
#define PLL_STABLE_TIME			(1)
#define CPUPLL_STABLE_TIME		(1)
#endif //#ifdef CHIP_PLATFORM

#else
/* define debug for chip and fpga verification */
//#define DEBUG_ENABLE

#ifdef FPGA_PLATFORM
//FPGA Verification
#define PLL_CPU_CLK				(36000000)
#define PLL_AUDIO_CLK 			(0)
#define PLL_VIDEO_CLK			(36000000)
#define PLL_VE_CLK_MODE 		(1)	//0-fractional mode, 1-integer mode
#define PLL_VE_CLK				(36000000)
#define PLL_DDR_CLK				(36000000)
#define PLL_PERH_CLK			(36000000)
#define AHBPREDIV				(1) // just used when source is PLL_PERH_CLK/PREDIV, 0:/1, 1:/2, 2:/3, 3:/4,
#define AHBDIV					(1) // 0:/1, 1:/2, 2:/4, 3:/8
#define APBDIV					(1) // 0:/, 1:/2, 2:/4, 3:/8
#define AHBCLK					(PLL_PERH_CLK/AHBPREDIV/AHBDIV)
#define APBCLK					(AHBCLK/APBDIV)
#define PLL_STABLE_TIME			(500)
#define CPUPLL_STABLE_TIME		(500)
#endif //#ifdef FPGA_PLATFORM

#ifdef CHIP_PLATFORM
//Chip Verification
#define PLL_CPU_CLK				(408000000)
#define PLL_AUDIO_CLK 			(0)
#define PLL_VIDEO_CLK			(297000000)
#define PLL_VE_CLK_MODE 		(1)	//0-fractional mode, 1-integer mode
#define PLL_VE_CLK				(297000000)
#define PLL_DDR_CLK				(300000000)
#define PLL_PERH_CLK			(408000000)
#define AHBPREDIV				(2) // just used when source is PLL_PERH_CLK/PREDIV, 0:/1, 1:/2, 2:/3, 3:/4,
#define AHBDIV					(1) // 0:/1, 1:/2, 2:/4, 3:/8
#define APBDIV					(2) // 0:/1, 1:/2, 2:/4, 3:/8
#define AHBCLK					(PLL_PERH_CLK/AHBPREDIV/AHBDIV)
#define APBCLK					(AHBCLK/APBDIV)
#define PLL_STABLE_TIME			(500)
#define CPUPLL_STABLE_TIME		(500)
#endif //#ifdef CHIP_PLATFORM

#endif //#ifdef SYSTEM_SIMULATION

/* ------------------ DRAM Configuration ------------------ */
#define DRAM_MEM_BASE		(0x80000000)
#define HEAP_MEM_START		(0x80800000)	//heap start offset: 8MB
#define HEAP_MEM_SIZE		(0x00200000)	//2MB
#define DRAM_FREE_MEM_BASE	(0x80300000)	//free memory for test, size 7M

#ifdef FPGA_PLATFORM
//#define DDR1_32B
//#define DDR2_FPGA_BJ530 //DDR1_32B
#define DDR2_FPGA_S2C
#define MCTL_CLK			(PLL_DDR_CLK/500000)
#define MCTL_ACCESS_MODE	1				//0: sequence mode, 1: interleave mode
#define MCTL_CHANNEL_NUM	1
#ifdef DDR1_FPGA_S2C
#define MCTL_CS_NUM		1
#else
#define MCTL_CS_NUM		1
#endif
#else
//#define DDR2_32B
//#define DDR3_32B
//#define DDR3_32B_4GB
//#define LPDDR2_32B
#define MCTL_CLK			(PLL_DDR_CLK/500000)
#define MCTL_ACCESS_MODE	1				//0: sequence mode, 1: interleave mode
#define MCTL_CHANNEL_NUM	2
#define MCTL_CS_NUM			1
#endif

/* ------------- Module Selection ------------------- */
/* add module control here to control total code size */
/* -------------------------------------------------- */

//#define CONFIG_USE_STDCLIB
#define CONFIG_USE_DDR
//#define CONFIG_USE_HEAP
//#define CONFIG_USE_CONSOLE
//#define CONFIG_USE_INTC
//#define CONFIG_USE_DMA
//#define CONFIG_USE_TIMER
//#define CONFIG_USE_TWI
//#define CONFIG_USE_GPIO_IRQ

//#define CONFIG_USE_DEFAULT_TIMER
//#define CONFIG_USE_DEFAULT_DMA

//#define CONFIG_USE_MEMTESTER
//#define CONFIG_DDR_TEST
//#define CONFIG_GPIO_TEST
//#define CONFIG_BIST_TEST

#define  SDMMC_SPINOR_NOT_READY
/* ---------- Cache Configuration of ARM CPU -------- */
/*
 * If not enable MMU, D-cache would be ineffective, even d-cache is enabled.
 * Since SRAM is too little to store MMU table, so TTB address is in DRAM.
 */
#ifdef CONFIG_USE_DDR
//#define SYS_ENABLE_MMU
//#define SYS_ENABLE_DCACHE
#endif
//#define SYS_ENABLE_ICACHE

#endif
