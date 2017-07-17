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

#include "bsp_rk.h"



#define  RKC_REGS_BASE                0xF1C24800
#define  RK_REG_o_CTRL0               0x00
#define  RK_REG_o_CTRL1               0x04
#define  RK_REG_o_FITER		      0X0C
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


static   __u32      save_ctrl0;
static   __u32      save_ctrl1;

static   __u32      save_int;
static   __u32      save_fifo;







__s32 standby_get_adc_valume(void)
{
    __u32 ret;
    __s32 data[32];
    __s32 average;
    __s32 i;
    __s32 j;
    __s32 cnt;

    // 读取数据
    cnt = 0;
    while(1)
    {
        ret = RK_QueryIntPending();
        if(ret & RK_DATA_PENDING)
        {
            RK_GetData(data, sizeof(data) / sizeof(data[0]));
            RK_ClearIntPending(ret);
            RK_ClearFifo();
            break;
        }
        if(cnt++ > 1)
        {
            RK_ClearIntPending(ret);
            RK_ClearFifo();
            break;
        }
    }


    if(!(ret & RK_DATA_PENDING))
    {
        for(i = sizeof(data) / sizeof(data[0]) - 1 ; i >= 0 ; i--)
        {
            data[i] = 0xfff;
        }
    }

    average = 0;
    for(i = sizeof(data) / sizeof(data[0]) - 1 ; i >= 0 ; i--)
    {
        average += data[i];
    }

    average /= sizeof(data) / sizeof(data[0]);




    return average;
}



__s32   RK_Init(void)
{
    __u32       reg_value;

    reg_value  = RK_READ_REG(RK_REG_o_CTRL1);
    save_ctrl1 = reg_value;
    reg_value |= (1 << 5); //enable tp fuction
    reg_value |= (1 << 4); //sellect adc mode
    RK_WRITE_REG(RK_REG_o_CTRL1, reg_value);



    reg_value  = RK_READ_REG(RK_REG_o_CTRL0);
    save_ctrl0 = reg_value;
    reg_value |= (0XF << 24);
    reg_value |= (1 << 23);
    reg_value &= ~(1 << 22);	//sellect HOSC(24MHz)
    reg_value |= (0x3 << 20); //00:CLK_IN/2  ,01:CLK_IN/3 ,10:CLK_IN/6,11:CLK_IN/1,
    reg_value |= (0x7f << 0); //FS DIV
    RK_WRITE_REG(RK_REG_o_CTRL0, reg_value);


    // 初始化REG_INT
    reg_value = RK_READ_REG(RK_REG_o_INT);
    save_int  = reg_value;
    reg_value = reg_value | (RK_DATA_IRQ );
    RK_WRITE_REG(RK_REG_o_INT, reg_value);

    return EPDK_OK;
}

__s32   RK_Exit(void)
{

    RK_WRITE_REG(RK_REG_o_CTRL0 , save_ctrl0);
    RK_WRITE_REG(RK_REG_o_CTRL1 , save_ctrl1);
    RK_WRITE_REG(RK_REG_o_INT   , save_int);
    RK_WRITE_REG(RK_REG_o_FIFOS , save_fifo);

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
        data[i] = RK_READ_REG(RK_REG_o_DATA) & 0xfff;   // 读取数据
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

    ret = RK_READ_REG(RK_REG_o_FIFOS);
    ret |= pending;
    RK_WRITE_REG(RK_REG_o_FIFOS, ret);

}





__s32   RK_SelectChannel(__rk_channel_e channel)
{
    __u32       reg_value;



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


