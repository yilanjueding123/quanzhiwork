/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                            power manager sub-system
*
*                                     (c) Copyright 2008-2009, kevin.z China
*                                              All Rights Reserved
*
* File   : standby_clock.c
* Version: V1.0
* By     : kevin.z
* Date   : 2009-6-6 16:09
*********************************************************************************************************
*/
#include "standby_clock.h"



__standby_ccu_reg_t *CmuReg;;
__u32 ccu_reg_back[6];
__u32 cpu_ms_loopcnt;

/*
*********************************************************************************************************
*                           standby_clk_init
*
*Description: ccu init for platform standby
*
*Arguments  : none
*
*Return     : result,
*
*Notes      :
*
*********************************************************************************************************
*/
__s32 standby_clk_init(void)
{
    CmuReg = (__standby_ccu_reg_t *)CCU_REG_BASE;


#if 0
    //backup pll registers
    ccu_reg_back[0] = *(volatile __u32 *)CmuReg->Pll1Cpu;
    ccu_reg_back[1] = *(volatile __u32 *)CmuReg->Pll2Audio;
    ccu_reg_back[2] = *(volatile __u32 *)CmuReg->Pll3Video;
    ccu_reg_back[3] = *(volatile __u32 *)CmuReg->Pll4Ve;
    ccu_reg_back[4] = *(volatile __u32 *)CmuReg->Pll5DDR;
    ccu_reg_back[5] = *(volatile __u32 *)CmuReg->Pll6Periph;
#endif

    //cpu frequency is 60mhz now
    cpu_ms_loopcnt = 3000;

    return 0;
}


/*
*********************************************************************************************************
*                           standby_clk_exit
*
*Description: ccu exit for platform standby
*
*Arguments  : none
*
*Return     : result,
*
*Notes      :
*
*********************************************************************************************************
*/
__s32 standby_clk_exit(void)
{

#if 0
    //restore pll registers
    *(volatile __u32 *)CmuReg->Pll1Cpu    = ccu_reg_back[0];
    *(volatile __u32 *)CmuReg->Pll2Audio  = ccu_reg_back[1];
    *(volatile __u32 *)CmuReg->Pll3Video  = ccu_reg_back[2];
    *(volatile __u32 *)CmuReg->Pll4Ve     = ccu_reg_back[3];
    *(volatile __u32 *)CmuReg->Pll5DDR    = ccu_reg_back[4];
    *(volatile __u32 *)CmuReg->Pll6Periph = ccu_reg_back[5];
#endif

    return 0;
}


/*
*********************************************************************************************************
*                                     standby_clk_core2losc
*
* Description: switch core clock to 32k low osc.
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/
__s32 standby_clk_core2losc(void)
{
    CmuReg->CpuClkSrcSel.CpuClkSrcSel = 0;

    //cpu frequency is 32k hz
    cpu_ms_loopcnt = 1;

    return 0;
}


/*
*********************************************************************************************************
*                                     standby_clk_core2hosc
*
* Description: switch core clock to 24M high osc.
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/
__s32 standby_clk_core2hosc(void)
{
    CmuReg->CpuClkSrcSel.CpuClkSrcSel = 1;

    //cpu frequency is 24M hz
    cpu_ms_loopcnt = 600;

    return 0;
}


/*
*********************************************************************************************************
*                                     standby_clk_core2pll
*
* Description: switch core clock to core pll.
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/
__s32 standby_clk_core2pll(void)
{
    CmuReg->CpuClkSrcSel.CpuClkSrcSel = 2;

    //cpu frequency is 60M hz
    cpu_ms_loopcnt = 2000;

    return 0;
}


/*
*********************************************************************************************************
*                                     standby_clk_plldisable
*
* Description: disable dram pll.
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/
__s32 standby_clk_plldisable(void)
{
    CmuReg->Pll1Cpu.PLLEn = 0;
    CmuReg->Pll2Audio.PLLEn = 0;
    CmuReg->Pll3Video.PLLEn = 0;
    CmuReg->Pll4Ve.PLLEn = 0;
    CmuReg->Pll6Periph.PLLEn = 0;


    return 0;
}


/*
*********************************************************************************************************
*                                     standby_clk_pllenable
*
* Description: enable dram pll.
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/
__s32 standby_clk_pllenable(void)
{
    CmuReg->Pll1Cpu.PLLEn = 1;
    CmuReg->Pll2Audio.PLLEn = 1;
    CmuReg->Pll3Video.PLLEn = 1;
    CmuReg->Pll4Ve.PLLEn = 1;
    CmuReg->Pll6Periph.PLLEn = 1;

    return 0;
}


/*
*********************************************************************************************************
*                                     standby_clk_hoscdisable
*
* Description: disable HOSC.
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/
__s32 standby_clk_hoscdisable(void)
{

    __u32 *base = (__u32 *)(0xf1c000a4);

    *base &= ~(0x7 << 0);
    *base |=  (0xa7 << 24);




    return 0;
}


/*
*********************************************************************************************************
*                                     standby_clk_hoscenable
*
* Description: enable HOSC.
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/
__s32 standby_clk_hoscenable(void)
{

    __u32 *base = (__u32 *)(0xf1c000a4);


    *base |= (0xa7 << 24);
    *base |= (0x7 << 0);

    return 0;
}


/*
*********************************************************************************************************
*                                     standby_clk_ldodisable
*
* Description: disable LDO.
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/
__s32 standby_clk_ldodisable(void)
{

    return 0;
}


/*
*********************************************************************************************************
*                                     standby_clk_ldoenable
*
* Description: enable LDO.
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/
__s32 standby_clk_ldoenable(void)
{

    return 0;
}


/*
*********************************************************************************************************
*                                     standby_clk_setdiv
*
* Description: switch core clock to 32k low osc.
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/
__s32 standby_clk_setdiv(struct sun4i_clk_div_t  *clk_div)
{
    if(!clk_div)
    {
        return -1;
    }



    return 0;
}


/*
*********************************************************************************************************
*                                     standby_clk_getdiv
*
* Description: switch core clock to 32k low osc.
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/
__s32 standby_clk_getdiv(struct sun4i_clk_div_t  *clk_div)
{
    if(!clk_div)
    {
        return -1;
    }



    return 0;
}

/*
*********************************************************************************************************
*                                     standby_clk_set_pll_factor
*
* Description: set pll factor, target cpu freq is 384M hz
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/

__s32 standby_clk_set_pll_factor(struct pll_factor_t *pll_factor)
{
    if(!pll_factor)
    {
        return -1;
    }

    CmuReg->Pll1Cpu.FactorN = pll_factor->FactorN;
    CmuReg->Pll1Cpu.FactorK = pll_factor->FactorK;
    CmuReg->Pll1Cpu.FactorM = pll_factor->FactorM;
    CmuReg->Pll1Cpu.PLLDivP = pll_factor->FactorP;


    return 0;
}

/*
*********************************************************************************************************
*                                     standby_clk_get_pll_factor
*
* Description:
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/

__s32 standby_clk_get_pll_factor(struct pll_factor_t *pll_factor)
{
    if(!pll_factor)
    {
        return -1;
    }

    pll_factor->FactorN = CmuReg->Pll1Cpu.FactorN;
    pll_factor->FactorK = CmuReg->Pll1Cpu.FactorK;
    pll_factor->FactorM = CmuReg->Pll1Cpu.FactorM;
    pll_factor->FactorP = CmuReg->Pll1Cpu.PLLDivP;


    return 0;
}

/*
*********************************************************************************************************
*                                     standby_clk_dramgating
*
* Description: gating dram clock.
*
* Arguments  : onoff    dram clock gating on or off;
*
* Returns    : 0;
*********************************************************************************************************
*/
void standby_clk_dramgating(int onoff)
{

}


/*
*********************************************************************************************************
*                                     standby_clk_ahb2losc
*
* Description: switch apb1 clock to 32k low osc.
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/
__s32 standby_clk_ahb2losc(void)
{

    CmuReg->AmbaClkRatio.AHB1ClkSrcSel = 0;

    return 0;
}


/*
*********************************************************************************************************
*                                     standby_clk_apb2hosc
*
* Description: switch apb1 clock to 24M hosc.
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/
__s32 standby_clk_ahb2pll6(void)
{

    CmuReg->AmbaClkRatio.AHB1ClkSrcSel = 3;

    return 0;
}



/*
*********************************************************************************************************
*                                     standby_clk_apb2hosc
*
* Description: switch apb1 clock to 24M hosc.
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/
__s32 standby_clk_apbinit(void)
{

    return 0;
}


/*
*********************************************************************************************************
*                                     standby_clk_apb2hosc
*
* Description: switch apb1 clock to 24M hosc.
*
* Arguments  : none
*
* Returns    : 0;
*********************************************************************************************************
*/
__s32 standby_clk_apbexit(void)
{

    return 0;
}

