/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           ir key driver
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : bsp_rk.c
* By      :
* Version : V1.00
*********************************************************************************************************
*/

#include "epdk.h"
#include "bsp_rk.h"



#define  RKC_REGS_BASE                0xF1C24800
#define  RK_REG_o_CTRL                0x00
#define  RK_REG_o_CTRL1               0x04
#define  RK_REG_o_FITER				  0X0C
#define  RK_REG_o_INT                 0x10
#define  RK_REG_o_FIFOS               0x14
#define  RK_REG_o_DATA                0x24
#define  RKC_AD_REGS_BASE             0xF1C20800
#define  RKC_AD_REGS_90               0x00

#define  TP_Y1						(0X2<<8)
#define  TP_Y2						(0X2<<12)
#define  TP_X1						(0X2<<0)
#define  TP_X2						(0X2<<4)

#define TP_MASK						(0XFFFFF)

#define readl(reg)			(*(volatile unsigned long *)(reg))


#define writel(value,reg)	(*(volatile unsigned long *)(reg) = (value))
static   __u32      save_ctrl;
static   __u32      save_int;
static   __u32      save_fifo;
static void tp_io_config(u32 channel)
{
    u32 reg_vale;
    u16 tp_io = 0;
    u16 tp_mask = 0;


    if((TP_X1 & channel) == TP_X1)
    {
        tp_io |= TP_X1;
        tp_mask |= (0xf << 0);
        __wrn("config io tpx1\n");

    }
    if((TP_X2 & channel) == TP_X2)
    {
        tp_io |= TP_X2;
        tp_mask |= 0xf << 4;
        __wrn("config io tpx2\n");

    }
    if((TP_Y1 & channel) == TP_Y1)
    {
        tp_io |= TP_Y1;
        tp_mask |= 0xf << 8;
        __wrn("config io tpy1\n");

    }
    if((TP_Y2 & channel) == TP_Y2)
    {

        tp_io |= TP_Y2;
        tp_mask |= 0xf << 12;
        __wrn("config io tpy2\n");
    }


    reg_vale = RK_AD_READ_REG(RKC_AD_REGS_90); //
    //__wrn("tp_io_config1 is 0x%x\n",reg_vale);
    reg_vale &= ~tp_mask;
    reg_vale |= tp_io;

    //	writel(reg_vale,0xf1c20800);
    RK_AD_WRITE_REG(RKC_AD_REGS_90, reg_vale);


}

__s32   RK_Init(void)
{
    __u32       reg_value;

    __wrn("RK_Init.\n");
    //   writel(0x2222,0xf1c20800);
    // 初始化REG_CTRL
    reg_value = RK_READ_REG(RK_REG_o_CTRL1);
    save_ctrl = reg_value;
    reg_value |= (1 << 5); //enable tp fuction
    reg_value |= (1 << 4); //sellect adc mode
    //	reg_value = reg_value | ~(0x00 << 4);        // set 4 channel select robin round
    //	reg_value |=((1 << 0)|(1<<1)|(1<<2)|(1<<3));//select all 4 channel
    //	reg_value = reg_value | (0x0f << 13);       // set CLK_IN/32
    RK_WRITE_REG(RK_REG_o_CTRL1, reg_value);

    reg_value = RK_READ_REG(RK_REG_o_CTRL);
    reg_value |= (0XF << 24);
    reg_value |= (1 << 23);

    reg_value &= ~(1 << 22)	;	//sellect HOSC(24MHz)

    reg_value |= (0x3 << 20); //00:CLK_IN/2  ,01:CLK_IN/3 ,10:CLK_IN/6,11:CLK_IN/1,

    reg_value |= (0x7f << 0); //FS DIV



    RK_WRITE_REG(RK_REG_o_CTRL, reg_value);
    //writel(reg_value,0xF1C24800);
    //esKRNL_TimeDly(100);

    //eLIBs_printf("0xF1C24800:0x%x\n", RK_READ_REG(RK_REG_o_CTRL));

    // 初始化REG_INT
    reg_value = RK_READ_REG(RK_REG_o_INT);
    save_int  = reg_value;
    //reg_value = reg_value & ~(RK_DATA_IRQ | RK_LTOUCH_IRQ | RK_FTOUCH_IRQ | RK_FIFO_OV_IRQ);  // disable int
    reg_value = reg_value | (RK_DATA_IRQ ); //| RK_LTOUCH_IRQ | RK_FTOUCH_IRQ | RK_FIFO_OV_IRQ);  // disable int
    RK_WRITE_REG(RK_REG_o_INT, reg_value);

    __wrn("fifoc write:0x%x\n", reg_value);
    //    __wrn("fifoc read the register:0x%x\n", RK_READ_REG(RK_REG_o_INT));

    // 初始化REG_FIFOS
#if 0
    reg_value = RK_READ_REG(RK_REG_o_FIFOS);
    save_fifo = reg_value;
    reg_value = reg_value | (0x01 << 0);        // FIRST Touch IRQ DISABLE
    reg_value = reg_value | (0x01 << 1);        // LAST Touch IRQ DISABLE
    reg_value = reg_value | (0x01 << 2);        // TOUCH PANEL IDLE
    reg_value = reg_value | (0x01 << 4);        // FIFO DATA IRQ ENABLE
    reg_value = reg_value | (0x01 << 5);        // FIFO OVER RUN IRQ ENABLE
    RK_WRITE_REG(RK_REG_o_FIFOS, reg_value);
#endif

#if 0

    reg_value = readl(0xf1c20800);//RK_AD_READ_REG(RKC_AD_REGS_90);
    reg_value &= ~(0xfff);
    reg_value |= (0x222);
    //	reg_value = (reg_value & ~0xffff)|0x2222;
    //RK_AD_WRITE_REG(RKC_AD_REGS_90, reg_value);		// PE0 output
    writel(reg_value, 0xf1c20800);

    reg_value = readl(0xf1c20800);
    __wrn("RKC_AD_REGS_90 :0x%x\n", reg_value);
#endif

    //tp_io_config(TP_X2|TP_Y2|TP_Y1);
    tp_io_config(TP_X2);


    //	__wrn("RKC_AD_REGS_90 :0x%x\n",RK_AD_READ_REG(RKC_AD_REGS_90));
    return EPDK_OK;
}

/*void print_init()
{
	__u32       reg_value;
	reg_value = RK_READ_REG(0x00);
	eLIBs_printf("&&&&&&&&&&&&&& INIT 0xF1C24800 + 0x00 = %x &&&&&&&&&&&&&\n",reg_value);
	reg_value = RK_READ_REG(0x04);
	eLIBs_printf("&&&&&&&&&&&&&& INIT 0xF1C24800 + 0x04 = %x &&&&&&&&&&&&&\n",reg_value);
	reg_value = RK_READ_REG(0x10);
	eLIBs_printf("&&&&&&&&&&&&&& INIT 0xF1C24800 + 0x10 = %x &&&&&&&&&&&&&\n",reg_value);
	reg_value = RK_READ_REG(0x14);
	eLIBs_printf("&&&&&&&&&&&&&& INIT 0xF1C24800 + 0x14 = %x &&&&&&&&&&&&&\n",reg_value);
	reg_value = RK_READ_REG(0x18);
	eLIBs_printf("&&&&&&&&&&&&&& INIT 0xF1C24800 + 0x18 = %x &&&&&&&&&&&&&\n",reg_value);
	eLIBs_printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
	reg_value = RK_AD_READ_REG(0x90);
	eLIBs_printf("&&&&&&&&&&&&&& INIT 0xF1C20800 + 0x90 = %x &&&&&&&&&&&&&\n",reg_value);


}

void print_init_later()
{
		__u32       reg_value;
		reg_value = RK_READ_REG(0x00);
		eLIBs_printf("&&&&&&&&&&&&&& INIT LATER 0xF1C24800 + 0x00 = %x &&&&&&&&&&&&&\n",reg_value);
		reg_value = RK_READ_REG(0x04);
		eLIBs_printf("&&&&&&&&&&&&&& INIT LATER 0xF1C24800 + 0x04 = %x &&&&&&&&&&&&&\n",reg_value);
		reg_value = RK_READ_REG(0x10);
		eLIBs_printf("&&&&&&&&&&&&&& INIT LATER 0xF1C24800 + 0x10 = %x &&&&&&&&&&&&&\n",reg_value);
		reg_value = RK_READ_REG(0x14);
		eLIBs_printf("&&&&&&&&&&&&&& INIT LATER 0xF1C24800 + 0x14 = %x &&&&&&&&&&&&&\n",reg_value);
		reg_value = RK_READ_REG(0x18);
		eLIBs_printf("&&&&&&&&&&&&&& INIT LATER 0xF1C24800 + 0x18 = %x &&&&&&&&&&&&&\n",reg_value);
		eLIBs_printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
		reg_value = RK_AD_READ_REG(0x90);
		eLIBs_printf("&&&&&&&&&&&&&& INIT LATER 0xF1C20800 + 0x90 = %x &&&&&&&&&&&&&\n",reg_value);

}*/


__s32   RK_Exit(void)
{
    RK_WRITE_REG(RK_REG_o_CTRL , save_ctrl);
    RK_WRITE_REG(RK_REG_o_INT  , save_int);
    RK_WRITE_REG(RK_REG_o_FIFOS, save_fifo);

    return EPDK_OK;
}

void    RK_ClearFifo(void)
{
    __s32       reg_value;

    reg_value = RK_READ_REG(RK_REG_o_INT);
    reg_value = reg_value | (0x1 << 4);        // clear fifo
    RK_WRITE_REG(RK_REG_o_INT, reg_value);
}

__s32   RK_EnableInt(__u32 int_no)
{
    __s32       reg_value;

    reg_value = RK_READ_REG(RK_REG_o_INT);
    reg_value = reg_value | int_no;
    RK_WRITE_REG(RK_REG_o_INT, reg_value);

    return EPDK_OK;
}

__s32   RK_DisableInt(__u32 int_no)
{
    __s32       reg_value;

    reg_value = RK_READ_REG(RK_REG_o_INT);
    reg_value = reg_value & (~int_no);
    RK_WRITE_REG(RK_REG_o_INT, reg_value);

    return EPDK_OK;
}

__s32   RK_GetData(__s32 *data, __u32 data_cnt)
{
    __s32       i;

    for(i = 0; i < data_cnt; i++)
    {
        data[i] = RK_READ_REG(RK_REG_o_DATA) & 0xfff;//0x7ff;   // 读取数据
    }

    return EPDK_OK;
}

__s32   RK_QueryIntPending(void)
{
    __s32       ret;

    ret = RK_READ_REG(RK_REG_o_FIFOS);

    return ret;
}

void    RK_ClearIntPending(__s32 pending)
{
    __s32       ret;

    //   ret = RK_READ_REG(RK_REG_o_FIFOS);
    ret = pending;
    RK_WRITE_REG(RK_REG_o_FIFOS, ret);

}

__s32   RK_SetSampleRate(__rk_sample_rate_e rate)
{
    __s32       reg_value;

    switch(rate)
    {
    case RK_SAMPLE_RATE_01K:
    {
        reg_value = RK_READ_REG(RK_REG_o_CTRL);
        reg_value = reg_value & ~(0x0f << 16);
        reg_value = reg_value | (0x05 << 16);
        RK_WRITE_REG(RK_REG_o_CTRL, reg_value);

        break;
    }
    case RK_SAMPLE_RATE_02K:
    {
        reg_value = RK_READ_REG(RK_REG_o_CTRL);
        reg_value = reg_value & ~(0x0f << 16);
        reg_value = reg_value | (0x06 << 16);
        RK_WRITE_REG(RK_REG_o_CTRL, reg_value);

        break;
    }
    case RK_SAMPLE_RATE_04K:
    {
        reg_value = RK_READ_REG(RK_REG_o_CTRL);
        reg_value = reg_value & ~(0x0f << 16);
        reg_value = reg_value | (0x07 << 16);
        RK_WRITE_REG(RK_REG_o_CTRL, reg_value);

        break;
    }
    case RK_SAMPLE_RATE_08K:
    {
        reg_value = RK_READ_REG(RK_REG_o_CTRL);
        reg_value = reg_value & ~(0x0f << 16);
        reg_value = reg_value | (0x08 << 16);
        RK_WRITE_REG(RK_REG_o_CTRL, reg_value);

        break;
    }
    case RK_SAMPLE_RATE_16K:
    {
        reg_value = RK_READ_REG(RK_REG_o_CTRL);
        reg_value = reg_value & ~(0x0f << 16);
        reg_value = reg_value | (0x09 << 16);
        RK_WRITE_REG(RK_REG_o_CTRL, reg_value);

        break;
    }
    default:
    {
        reg_value = RK_READ_REG(RK_REG_o_CTRL);
        reg_value = reg_value & ~(0x0f << 16);
        reg_value = reg_value | (0x07 << 16);
        RK_WRITE_REG(RK_REG_o_CTRL, reg_value);

        break;
    }
    }

    return EPDK_OK;
}

__s32   RK_SetFifoDepth(__rk_fifo_depth_e depth)
{
    __s32       reg_value;

    switch(depth)
    {
    case RK_FIFO_DEPTH_01:
    {
        reg_value = RK_READ_REG(RK_REG_o_INT);
        reg_value = reg_value & ~(0x1f << 8);
        reg_value = reg_value | ((0) << 8);
        RK_WRITE_REG(RK_REG_o_INT, reg_value);

        break;
    }
    case RK_FIFO_DEPTH_04:
    {
        reg_value = RK_READ_REG(RK_REG_o_INT);
        reg_value = reg_value & ~(0x1f << 8);
        reg_value = reg_value | ((4 - 1) << 8);
        RK_WRITE_REG(RK_REG_o_INT, reg_value);

        break;
    }
    case RK_FIFO_DEPTH_08:
    {
        reg_value = RK_READ_REG(RK_REG_o_INT);
        reg_value = reg_value & ~(0x1f << 8);
        reg_value = reg_value | ((8 - 1) << 8);
        RK_WRITE_REG(RK_REG_o_INT, reg_value);

        break;
    }
    case RK_FIFO_DEPTH_16:
    {
        reg_value = RK_READ_REG(RK_REG_o_INT);
        reg_value = reg_value & ~(0x1f << 8);
        reg_value = reg_value | ((16 - 1) << 8);
        RK_WRITE_REG(RK_REG_o_INT, reg_value);

        break;
    }
    case RK_FIFO_DEPTH_32:
    {
        reg_value = RK_READ_REG(RK_REG_o_INT);
        reg_value = reg_value & ~(0x1f << 8);
        reg_value = reg_value | ((32 - 1) << 8);
        RK_WRITE_REG(RK_REG_o_INT, reg_value);

        break;
    }
    default:
    {
        reg_value = RK_READ_REG(RK_REG_o_INT);
        reg_value = reg_value & ~(0x1f << 8);
        reg_value = reg_value | ((0x01 << 2) << 8);
        RK_WRITE_REG(RK_REG_o_INT, reg_value);

        break;
    }
    }

    return EPDK_OK;
}

__s32   RK_SelectChannel(__rk_channel_e channel)
{
    __u32       reg_value;

#if 0
    reg_value = RK_READ_REG(RK_REG_o_CTRL1);
    reg_value = reg_value & ~(0xf << 0);
    reg_value = reg_value | (0xf);        // select x2 channel
    RK_WRITE_REG(RK_REG_o_CTRL1, reg_value);

    return 0;
#endif

    //channel = RK_CHANNEL_X2;

    switch(channel)
    {
    case RK_CHANNEL_X1:
    {
        reg_value = RK_READ_REG(RK_REG_o_CTRL1);
        reg_value = reg_value & ~(0xf << 0);
        reg_value = reg_value | (1 << 0);        // select x1 channel
        RK_WRITE_REG(RK_REG_o_CTRL1, reg_value);

        break;
    }
    case RK_CHANNEL_X2:
    {
        reg_value = RK_READ_REG(RK_REG_o_CTRL1);
        reg_value = reg_value & ~(0xf << 0);
        reg_value = reg_value | (1 << 1);        // select x2 channel
        RK_WRITE_REG(RK_REG_o_CTRL1, reg_value);

        break;
    }
    case RK_CHANNEL_Y1:
    {
        reg_value = RK_READ_REG(RK_REG_o_CTRL1);
        reg_value = reg_value & ~(0xf << 0);
        reg_value = reg_value | (1 << 2);        // select y1 channel
        RK_WRITE_REG(RK_REG_o_CTRL1, reg_value);

        break;
    }
    case RK_CHANNEL_Y2:
    {
        reg_value = RK_READ_REG(RK_REG_o_CTRL1);
        reg_value = reg_value & ~(0xf << 0);
        reg_value = reg_value | (1 << 3);        // select y2 channel
        RK_WRITE_REG(RK_REG_o_CTRL1, reg_value);

        break;
    }
    case RK_CHANNEL_RD:
    {
        reg_value = RK_READ_REG(RK_REG_o_CTRL1);
        reg_value = reg_value & ~(0xf << 0);
        reg_value = reg_value | (0xf);        // select 4 channel
        RK_WRITE_REG(RK_REG_o_CTRL1, reg_value);

        break;
    }
    default:
    {
        break;
    }
    }

    return EPDK_OK;
}

__s32 pt_regs(__u32 base, __s32 size)
{
    __s32 i;
    __u32 *pbase = (__u32 *)base;



    esKRNL_SchedLock();
    eLIBs_printf("-----base:0x%x, size:%d\n", (__u32)base, size);

    for(i = 0 ; i < size / 4 ; i++)
    {
        eLIBs_printf("0x%x, ", pbase[i]);
    }

    eLIBs_printf("\n");
    esKRNL_SchedUnlock();

    return 0;
}

