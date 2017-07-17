/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               DRAM CSP Module
*
*                                    (c) Copyright 2006-2010, Berg.Xing China
*                                             All Rights Reserved
*
* File    : csp_dram_ops.h
* By      : Berg.Xing
* Version : v1.0
* Date    : 2010-12-2 13:24
* Descript:
* Update  : date                auther      ver     notes
*           2010-12-2 13:24     Berg.Xing   1.0     build the file;
*           2011-1-26 14:00     cpl         1.1     modify for aw1619 system
*********************************************************************************************************
*/
#ifndef __CSP_DRAM_OPS_H__
#define __CSP_DRAM_OPS_H__

/*
*********************************************************************************************************
*                                   DRAM INITIALISE
*
* Description: dram initialise;
*
* Arguments  : para     dram config parameter;
*              mode     dram initialise mode;
*
* Returns    : EBSP_FALSE: dram parameter is invalid
*			   EBSP_TRUE: initialzation success
* Note       :
*********************************************************************************************************
*/
__s32 CSP_DRAMC_init(__dram_para_t *para, __s32 mode);

/*
*********************************************************************************************************
*                                   DRAM EXIT
*
* Description: dram exit;
*
* Arguments  : none;
*
* Returns    : result;
*
* Note       :
*********************************************************************************************************
*/
__s32 CSP_DRAMC_exit(void);

/*
*********************************************************************************************************
*                                   DRAM SET EMRS
*
* Description: set EMRS for DDR memory and Mobile memory;
*
* Arguments  : emrs_id  EMRS ID;
*              emrs_val EMRS register value;
*
* Returns    : result, EBSP_TRUE;
*
* Note       :
*********************************************************************************************************
*/
__u32 CSP_DRAMC_set_emrs(__u32 emrs_id, __u32 emrs_val);

void CSP_DRAMC_master_open(void);
void CSP_DRAMC_master_close(void);
void CSP_DRAM_store_register_data(void);
void CSP_DRAM_restore_register_data(void);
/*
*********************************************************************************************************
*                                   DRAM ENTER SELF REFRESH
*
* Description: dram enter self-refresh;
*
* Arguments  : none
*
* Returns    : result, EBSP_TRUE;
*
* Note       :
*********************************************************************************************************
*/
void CSP_DRAMC_enter_selfrefresh(void);

/*
*********************************************************************************************************
*                                   DRAM EXIT SELF REFRESH
*
* Description: dram exit self-refresh;
*
* Arguments  : none;
*
* Returns    : result, EBSP_TRUE;
*
* Note       :
*********************************************************************************************************
*/
void CSP_DRAMC_exit_selfrefresh(void);

/*
*********************************************************************************************************
*                                   DRAM POWER CONTROL
*
* Description: dram power down or not;
*
* Arguments  : down     if need set dram power down;
*
* Returns    : result, EBSP_TRUE;
*
* Note       :
*********************************************************************************************************
*/
void CSP_DRAMC_power_down(__u32 down);

/*
*********************************************************************************************************
*                                   DRAM SET AUTO FRESH CYCLE
*
* Description: setup auto refresh interval value;
*
* Arguments  :  para DRAM config parameter
*
* Returns    : result, EBSP_TRUE;
*
* Note       :
*********************************************************************************************************
*/
void CSP_DRAMC_set_autofresh_cycle(__u32 clk);

/*
*********************************************************************************************************
*                                   DRAM CLK OUTPUT CONTROL
*
* Description: dram output enable/disable;
*
* Arguments  : switch    1: enter power down mode
*                        0: exit power down mode
* Returns    : result, EBSP_TRUE;
*
* Note       :
*********************************************************************************************************
*/
__s32 CSP_DRAMC_access_enable(void);


/*
**********************************************************************************************************************
*                                   DRAM ACCESS DISABLE
*
* Description: dram access disable;
*
* Arguments  : none
*
* Returns    : result, EBSP_TRUE/EBSP_FALSE;
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 CSP_DRAMC_access_disable(void);

/*
*********************************************************************************************************
*                                   DRAM SET ACCESS BLOCK SIZE
*
* Description: dram set hardware access block size;
*
* Arguments  : mod      hardware module id;
*              blksize  access block size;
*
* Returns    : result, EBSP_TRUE/EBSP_FALSE;
*
* Note       :
*********************************************************************************************************
*/

/*
**********************************************************************************************************************
*                                               DRAM_SetPrioMode
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 CSP_DRAMC_set_master_configuration(__dram_dev_e mod, __dram_master_t *master);

/*
**********************************************************************************************************************
*                                               DRAM_Setwaitnum
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 CSP_DRAMC_set_bandwidth_window_configuration(void);

/*
**********************************************************************************************************************
*                                               GET DRAM SIZE
*
* Description: Get DRAM Size in MB unit;
*
* Arguments  : None
*
* Returns    : 32/64/128
*
* Notes      :
*
**********************************************************************************************************************
*/
__u32 CSP_DRAMC_get_dram_size(void);

/*
**********************************************************************************************************************
*                                               DRAM_check_ddr_readpipe
*
* Description: check the best dram readpipe value
*
* Arguments  : para: dram configure parameters
*
* Returns    : regurn EBSP_TRUE
*
* Notes      :
**********************************************************************************************************************
*/
__u32 CSP_DRAMC_scan_readpipe(__u32 clk);

/*
*********************************************************************************************************
*                                   DRAM SET PAD DRIVE
*
* Description: dram set pad drive;
*
* Arguments  : clk_freq
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void CSP_DRAMC_set_pad_drive(__u32 clk_freq);

#endif  //__CSP_DRAM_OPS_H__
