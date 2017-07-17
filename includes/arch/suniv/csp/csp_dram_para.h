/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               DRAM CSP Module
*
*                                    (c) Copyright 2006-2010, Berg.Xing China
*                                             All Rights Reserved
*
* File    : csp_dram_para.h
* By      : Berg.Xing
* Version : v1.0
* Date    : 2010-12-2 13:24
* Descript:
* Update  : date                auther      ver     notes
*           2010-12-2 13:24     Berg.Xing   1.0     build the file;
*           2011-1-26 14:00     cpl         1.1     modify for aw1619 system
*********************************************************************************************************
*/
#ifndef __CSP_DRAM_PARA_H__
#define __CSP_DRAM_PARA_H__

#define DRAM_PIN_DEV_ID     (0)
#define DRAM_PIN_LIST       ((__u32 *)0)
#define DRAM_PIN_NUMBER     (0)

typedef enum __DRAM_TYPE
{
    DRAM_TYPE_SDR   = 0,
    DRAM_TYPE_DDR   = 1,
    DRAM_TYPE_DDR2  = 2,
    DRAM_TYPE_MDDR = 3
} __dram_type_e;

typedef enum __DRAM_BNKMOD
{
    DRAM_ACS_INTERLEAVE = 0,
    DRAM_ACS_SEQUENCE   = 1,
} __dram_bnkmod_e;


typedef enum __DRAM_PRIO_LEVEL
{
    DRAM_PRIO_LEVEL_0 = 0,
    DRAM_PRIO_LEVEL_1    ,
    DRAM_PRIO_LEVEL_2    ,
    DRAM_PRIO_LEVEL_3    ,
    DRAM_PRIO_LEVEL_

} __dram_prio_level_e;

/* 需要读取dram数据的设备名称 */
typedef struct __DRAM_MASTER
{
    __u32 dram_prio_level;
    __u32 dram_threshold0;
    __u32 dram_threshold1;
    __u32 dram_request_num;

} __dram_master_t;


/* 需要读取dram数据的设备名称 */
typedef enum __DRAM_DEV
{
    DRAM_DEVICE_NULL = 0,
    DRAM_DEVICE_DAHB,
    DRAM_DEVICE_IAHB,
    DRAM_DEVICE_DDMA,
    DRAM_DEVICE_NDMA,
    DRAM_DEVICE_SD0,
    DRAM_DEVICE_SD1,
    DRAM_DEVICE_DEBE,
    DRAM_DEVICE_DEFE,
    DRAM_DEVICE_VE,
    DRAM_DEVICE_CSI,
    DRAM_DEVICE_DeInterlace,
    DRAM_DEVICE_TVD,
    DRAM_DEVICE_

} __dram_dev_e;


typedef struct __DRAM_PARA
{
    __u32           base;           // dram base address
    __u32           size;           // dram size, based on     (unit: MByte)
    __u32           clk;            // dram work clock         (unit: MHz)
    __u32           access_mode;    // 0: interleave mode 1: sequence mode
    __u32           cs_num;         // dram chip count  1: one chip  2: two chip
    __u32           ddr8_remap;     // for 8bits data width DDR 0: normal  1: 8bits
    __dram_type_e   type;           // ddr/ddr2/sdr/mddr/lpddr/...
    __u32           bwidth;         // dram bus width
    __u32           col_width;      // column address width
    __u32           row_width;      // row address width
    __u32           bank_size;      // dram bank count
    __u32           cas;            // dram cas
} __dram_para_t;


#endif  //__CSP_DRAM_PARA_H__

