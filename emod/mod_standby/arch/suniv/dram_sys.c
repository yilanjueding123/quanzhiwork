#include "dram_sys.h"


__u32 save_data[32];





void  aw_delay(__u32 n)
{
    while(n)
    {
        n--;
    }
}

void dram_gate_CLK(__u32 set)
{
    if(set)
    {
        DRAM_CCM_DRAM_GATING_REG &= ~(0x1u << 31);
        DRAM_CCM_DRAM_GATING_REG |= 0x1u << 31;
    }
    else
    {
        DRAM_CCM_DRAM_GATING_REG &= ~(0x1u << 31);
    }
}


void dram_pll_CLK(__u32 set)
{

    if(set)
    {

        DRAM_CCM_SDRAM_PLL_REG &= ~(0x1 << 31);
        DRAM_CCM_SDRAM_PLL_REG |= (0x1 << 20);
        while(DRAM_CCM_SDRAM_PLL_REG & (0x1 << 20));
    }
    else
    {

        DRAM_CCM_SDRAM_PLL_REG |= (0x1 << 31);
        DRAM_CCM_SDRAM_PLL_REG |= (0x1 << 20);
        while((DRAM_CCM_SDRAM_PLL_REG & (0x1 << 28)) == 0);


    }
}

__u32 DRAMC_setpara(__dram_para_t *para)
{
    __u32   reg_val = 0;
    //setup SCONR register
    reg_val = (para->ddr8_remap) 			|
              (0x1 << 1)           			|
              ((para->bank_size >> 2) << 3)		|
              ((para->cs_num >> 1) << 4)        |
              ((para->row_width - 1) << 5)      |
              ((para->col_width - 1) << 9)      |
              ((para->type ? (para->bwidth >> 4) : (para->bwidth >> 5)) << 13)  |
              (para->access_mode << 15)       |
              (para->type << 16);

    DRAM_REG_SCONR = reg_val;

    return 1;


}

__u32 DRAMC_getpara(__dram_para_t *para)
{
    __u32   reg_val = 0;

    //setup SCONR register
    reg_val = (para->ddr8_remap) 			|
              (0x1 << 1)           			|
              ((para->bank_size >> 2) << 3)		|
              ((para->cs_num >> 1) << 4)        |
              ((para->row_width - 1) << 5)      |
              ((para->col_width - 1) << 9)      |
              ((para->type ? (para->bwidth >> 4) : (para->bwidth >> 5)) << 13)  |
              (para->access_mode << 15)       |
              (para->type << 16);

    DRAM_REG_SCONR = reg_val;

    return 1;


}


void DRAMC_enter_selfrefresh(void)
{
    //enter dram self fresh mode
    DRAM_REG_SCTLR |= 0x1 << 1;

    //wait dram enter self-refresh status
    while(!(DRAM_REG_SCTLR & (0x1 << 11)))
    {
    }
}

void DRAMC_exit_selfrefresh(void)
{
    //exit selfrefresh
    DRAM_REG_SCTLR &= ~(0x1 << 1);

    //wait dram exit self-refresh status
    while(DRAM_REG_SCTLR & (0x1 << 11))
    {
    }
}

void DRAMC_set_autofresh_cycle(__u32 clk)
{
    __u32 reg_val = 0;
    __u32 row = 0;
    __u32 temp = 0;

    row = DRAM_REG_SCONR;
    row &= 0x1E0;
    row >>= 0x5;

    //64 ms/8192 rows = 7.8us per refresh cycle
    if(row == 0xC)
    {
        if(clk >= 1000000)
        {
            //HZ
            temp = clk + (clk >> 3) + (clk >> 4) + (clk >> 5);
            while(temp >= (10000000 >> 6))
            {
                temp -= (10000000 >> 6);
                reg_val++;
            }
        }
        else
        {
            //MHZ
            reg_val = (clk * 499) >> 6;
        }
    }
    else if(row == 0xB) //64ms/4096 rows = 15.6us per refresh cycle
    {
        if(clk >= 1000000)
        {
            temp = clk + (clk >> 3) + (clk >> 4) + (clk >> 5);
            while(temp >= (10000000 >> 7))
            {
                temp -= (10000000 >> 7);
                reg_val++;
            }
        }
        else
        {
            reg_val = (clk * 499) >> 5;
        }
    }
    DRAM_REG_SREFR = reg_val;
}

void MASTER_CLOSE(void)
{
    DRAM_REG_MAE &= ~(0xfffc);
}

void MASTER_OPEN(void)
{
    DRAM_REG_MAE |= 0xffff;
}

void dram_power_save_process()
{

    MASTER_CLOSE();
    aw_delay(0x1000);

    save_data[0] = DRAM_REG_DDLYR; //save delay_scan value
    save_data[1] = DRAM_REG_SCTLR; //save readpipe
    //put DRAM into self-refresh state
    DRAMC_enter_selfrefresh();
    aw_delay(0x1000);
    dram_gate_CLK(1);//close gate clk
    dram_pll_CLK(1);//close pll clk

}


void  mctl_init_stadby()
{
    __u32 reg_val = 0;
    __u32 i;




    dram_pll_CLK(0);
    aw_delay(1000);

    dram_gate_CLK(0);//open dram clk
    aw_delay(0x10);

    //exit self refesh
    DRAMC_exit_selfrefresh();
    aw_delay(0x1000);//after exit selrefresh should wait for a litter


    //set DRAM control register
    DRAM_REG_SCTLR = DRAM_REG_SCTLR | 0x1 << 19; //enable auto refresh mode

    DRAM_REG_DDLYR = save_data[0]; //save delay_scan value
    DRAM_REG_SCTLR = save_data[1]; //save readpipe

    MASTER_OPEN();
    aw_delay(0x1000);



}




