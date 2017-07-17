/*
**********************************************************************************************************************
*                                                    ePOS
*                                   the Easy Portable/Player Operation System
*                                              standby sub-system
*
*                                      (c) Copyright 2009-2010, jerr China
*                                              All Rights Reserved
*
* File    : standby_ir.c
* By      : jerry
* Version : V1.00
**********************************************************************************************************************
*/
#include  "standby_ir.h"

extern __u32    ir_mask;
extern __u32    ir_power;

#if(ALLOW_IR_WAKEUP_STANDBY && !ALLOW_DISABLE_HOSC)


/* normal dma 0 configuration register  */
#define NDMA_OFFSET   (0x20)

#define DMAC_REG_o_CFG              (0x100 + 1*NDMA_OFFSET + 0x00)
#define DMAC_REG_o_SRC_ADDR         (0x100 + 1*NDMA_OFFSET + 0x04)
#define DMAC_REG_o_DST_ADDR         (0x100 + 1*NDMA_OFFSET + 0x08)
#define DMAC_REG_o_BYTE_CNT         (0x100 + 1*NDMA_OFFSET + 0x0C)

#define DMAC_REG_N0_CFG             ( DMA_REG_BASE + DMAC_REG_o_CFG       )
#define DMAC_REG_N0_SRC_ADDR        ( DMA_REG_BASE + DMAC_REG_o_SRC_ADDR  )
#define DMAC_REG_N0_DST_ADDR        ( DMA_REG_BASE + DMAC_REG_o_DST_ADDR  )
#define DMAC_REG_N0_BYTE_CNT        ( DMA_REG_BASE + DMAC_REG_o_BYTE_CNT  )


/* ir module register for remote controller */
#define  IR_REG_CTRL     (IR_REG_BASE+0x00)  /*IR Control Register---used for ir */
#define  IR_REG_RXCTL    (IR_REG_BASE+0x10)  /*IR Rx Configure Register */
#define  IR_REG_RXFIFO   (IR_REG_BASE+0x20) /*IR Rx FIFO---used for ir */
#define  IR_REG_RXINT    (IR_REG_BASE+0x2C) /*IR Rx Interrupt Control Register---used for ir */
#define  IR_REG_RXSTA    (IR_REG_BASE+0x30) /*IR Rx Status Register---used for ir */
#define  IR_REG_CIR      (IR_REG_BASE+0x34) /*Consumer IR Configure Register---used for ir*/

#define IR_RXINT_ALL_MASK     (0x3f)
#define IR_RXINT_RPEI_EN   (0x1<<1) /* receive packet end interrupt */
#define IR_RXINT_RAI_EN     (0x1<<4) /* rxfifo available interrupt */
#define IR_RXINT_DRQ_EN   (0x01<<5)
#define IR_RXSTA_ALL_MASK   (0x1f)
#define IR_RXSTA_RAC_MASK  (0x1f<<8) /* rxFIFO Available Counter.0:no data,16:16 byte in rxFIFO */


/* NEC remote controller format decode variable */
#define NEC_IR_FORMAT_LEADING          380  /*sample_clk = 21.33us, 8.5ms   < sample_clk*380 ~= 8.2ms  < 9ms(NEC leading word) */
#define NEC_IR_NORMAL_OR_REPEAT_CODE   160  /*sample_clk = 21.33us, 2.25ms  < sample_clk*160 ~=3.4ms   < 4.5ms(NEC repeat word) */
#define NEC_IR_ONE_OR_ZERO_BIT         53   /*sample_clk = 21.33us, 0.565ms < sample_clk*53 ~=1.1275ms < 1.69ms(NEC high bit duration) */
#define NEC_IR_DMIN                    19   /*sample_clk = 21.33us, sample_clk*19 =  405us  < 560us */
#define NEC_IR_DMAX                    90   /*sample_clk = 21.33us, sample_clk*90 =  us > 1690us = 2250us - 560us */
#define NEC_IR_PMAX                    30   /*sample_clk = 21.33us, sample_clk*30 =  640us  >  560us */

#define IR_ERROR_CODE  0xffffffff
#define IR_REPEAT_CODE 0



/* data for store sample air */
#define IR_DMA_LEN     75

static  __irs_reserve_status_t   irs_status;
static  __u8  *ir_data;

/*
****************************************************************************************************
*
*  FunctionName:           ir_nec_data_decode
*
*  Description:
*            根据接收到的数据帧解码，主要依据NEC的接收波形进行解码，解码过程如下:
*               解码前提:接收波形需要反转。
*               1.以8.7ms区别NEC前导；
*               2.以3.4ms区别normal code还是repeat code
*               3.0.56ms为高电平,注意脉冲不能超过或少于这个值
*               4.低电平判断0或者1的标准为1.1275ms。
*
*  Parameters:
*       buf  :  接收到的数据buffer首地址
*       size :  buffer的长度
*
*  Return value:
*           出错返回IR_ERROR_CODE，成功返回code值，为0为repeat code 。
*  Notes:
*
****************************************************************************************************
*/
static __u32 ir_nec_data_decode(const __u8 *buf, const __u32 size)
{
    __u32 len    = 0;//low or high level time duration
    __u32 code   = 0;//decode result
    __s32 bitCnt = 0;// 32bit long  16bit user code+ 8bit data + 8bit invert data
    __u32 i      = 0;//counter for size
    __u8  val    = 0;//store sample data
    __u8  last   = 0;//low or high level flag

    if(buf == NULL)
    {
        return IR_ERROR_CODE;
    }
    if(size == 0)
    {
        return IR_ERROR_CODE;
    }

    /*step 1-区分9ms*/
    len = 0;
    for( i = 0; i < size; i++)
    {
        val = buf[i];
        if(val & 0x80)//找9ms，高电平
        {
            len += val & 0x7f;//累加
        }
        else //低电平开始2.25ms或者4.5ms
        {
            if(len > NEC_IR_FORMAT_LEADING)//为NEC格式
            {
                break;
            }
            else
            {
                return IR_ERROR_CODE;//非NEC格式，直接返回
            }
        }
    }

    /*step 2- 区分4.5ms或者2.25ms */
    len = 0;
    for(; i < size; i++)
    {
        val = buf[i];
        if(val & 0x80)//开始找到数据高电平
        {
            if(len > NEC_IR_NORMAL_OR_REPEAT_CODE)
            {
                break;//normal code 处理
            }
            len = 0;
        }
        else //找4.5ms或2.25ms ，低电平
        {
            len += val & 0x7f;//累加
        }
    }

    /*step3 -对用户码/地址码和数据解码*/
    code   = 0;  /*0 for Repeat Code*/
    bitCnt = 0;
    last   = 1;
    len    = 0;
    for(; i < size; i++)
    {
        val = buf[i];
        if(last)//统计高电平0.56ms，
        {
            if(val & 0x80)
            {
                len += val & 0x7f;
            }
            else //低电平出现
            {
                if(len > NEC_IR_PMAX) /*Error Pulse*/
                {
                    return IR_ERROR_CODE;
                }
                last = 0;//转入判断0,1
                len = val & 0x7f;//开始对低电平累加
            }
        }
        else //统计低电平识别0-1；
        {
            if(val & 0x80)//遇到高电平后
            {
                if(len > NEC_IR_DMAX) /*Error Distant*///必须保证低电平的持续时间不超过4个
                {
                    return IR_ERROR_CODE;
                }
                else//低电平 小于4个0.56ms
                {
                    if(len > NEC_IR_ONE_OR_ZERO_BIT)  //数据1，默认数据0
                    {
                        /*data '1'*/
                        code |= 1 << bitCnt;
                    }
                    bitCnt ++;
                    if(bitCnt == 32)
                    {
                        break;  /*decode over*/
                    }
                }
                last = 1;
                len = val & 0x7f;
            }
            else //统计低电平持续时间。
            {
                len += val & 0x7f;
            }
        }
    }
    return code;
}

/*
*********************************************************************************************************
*                           INIT IR FOR STANDBY
*
*Description: init ir for standby;
*
*Arguments  : none
*
*Return     : result;
*               EPDK_OK,    init ir successed;
*               EPDK_FAIL,  init ir failed;
*********************************************************************************************************
*/
__s32  standby_ir_init(__u8 *pSRAMBuffer, __u32 Len)
{
    __s32   i;
    __u32   reg_val;
    __u32   saddr, daddr;

    ir_data = pSRAMBuffer;
    irs_status.ir_int = STANDBY_RREG(IR_REG_RXINT);/* 备份ir接收中断寄存器 */

    /* 将ROI_EN,RPEI_EN,RISI_EN,CRCI_EN,RAI_EN,DRQ_EN 置0，关闭所有中断，而触发fifo传输不变 */
    reg_val = STANDBY_RREG(IR_REG_RXINT);
    reg_val &= ~IR_RXINT_ALL_MASK;
    reg_val |= IR_RXINT_DRQ_EN;/* 开启DRQ中断 */
    reg_val |= IR_RXINT_RPEI_EN;/* 开启接收包完成中断 */
    // reg_val |= IR_RXINT_RAI_EN; /* 开启fifo有数据中断 */
    STANDBY_WREG(IR_REG_RXINT, reg_val);

    /* 把pending位清掉 */
    reg_val = STANDBY_RREG(IR_REG_RXSTA);
    reg_val |= IR_RXSTA_ALL_MASK;
    STANDBY_WREG(IR_REG_RXSTA, reg_val);

    saddr = (__u32)(0x0fffffff & IR_REG_RXFIFO);/* 设置接收寄存器为源地址 */
    daddr = (__u32)((__u32)ir_data & 0x000fffff);

    STANDBY_WREG(DMAC_REG_N0_CFG, 0x0C110020);/* 配置DMA, 双8字节，无burst  */
    STANDBY_WREG(DMAC_REG_N0_SRC_ADDR, saddr);/* 源物理地址 */
    STANDBY_WREG(DMAC_REG_N0_DST_ADDR, daddr);/* 目的物理地址 */

    reg_val = STANDBY_RREG(DMAC_REG_N0_BYTE_CNT);
    reg_val &= 0x3ffff;
    reg_val |= IR_DMA_LEN;/* 暂时定在取100个字节数据，这个值比实际的一帧IR数据字节数要少一点  */
    STANDBY_WREG(DMAC_REG_N0_BYTE_CNT, reg_val);

    while(IR_RXSTA_RAC_MASK & STANDBY_RREG(IR_REG_RXSTA))
    {
        daddr =  STANDBY_RREG(IR_REG_RXFIFO);/* 忽略无用的数据 */
    }

    //delay 1 second for ir init
    for(i = 0; i < 1 * 1000 * 1000; i++);
    while(IR_RXSTA_RAC_MASK & STANDBY_RREG(IR_REG_RXSTA))
    {
        daddr = STANDBY_RREG(IR_REG_RXFIFO);/* 忽略无用的数据 */
    }
    /* pending清零 */
    reg_val = STANDBY_RREG(IR_REG_RXSTA);
    reg_val |= IR_RXSTA_ALL_MASK;
    STANDBY_WREG(IR_REG_RXSTA, reg_val);
    /* 启动DMA接收数据 */
    reg_val = STANDBY_RREG(DMAC_REG_N0_CFG);
    reg_val |= (__u32)1 << 31;
    STANDBY_WREG(DMAC_REG_N0_CFG, reg_val);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                           EXIT IR FOR STANDBY
*
*Description: exit ir for standby;
*
*Arguments  : none;
*
*Return     : result.
*               EPDK_OK,    exit ir successed;
*               EPDK_FAIL,  exit ir failed;
*********************************************************************************************************
*/
__s32 standby_ir_exit(void)
{
    //stop dma
    STANDBY_WREG(DMAC_REG_N0_CFG, 0);
    //restore ir controller int registe
    STANDBY_WREG(IR_REG_RXINT, irs_status.ir_int);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                           DETECT IR FOR STANDBY
*
*Description: detect ir for standby;
*
*Arguments  : none
*
*Return     : result;
*               EPDK_OK,    receive some signal;
*               EPDK_FAIL,  no signal;
*********************************************************************************************************
*/
__s32 standby_ir_detect(void)
{
    if(0x80000000 & STANDBY_RREG(DMAC_REG_N0_CFG))
    {
        return EPDK_FAIL;/* BUSY */
    }

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                           VERIFY IR SIGNAL FOR STANDBY
*
*Description: verify ir signal for standby;
*
*Arguments  : none
*
*Return     : result;
*               EPDK_OK,    valid ir signal;
*               EPDK_FAIL,  invalid ir signal;
*********************************************************************************************************
*/
__s32 standby_ir_verify(void)
{
    __u32 code, reg_val;
    __u32 saddr, daddr;

    code = ir_nec_data_decode(ir_data, IR_DMA_LEN);
    if( IR_ERROR_CODE !=  code )
    {
        if(((code & 0xffff)  == pStandbyPar->IrMask)
                && (((code >> 16) & 0xff) == pStandbyPar->IrPowerVal))
        {
            return EPDK_OK;
        }

    }

    /* 把pending位清掉 */
    reg_val = STANDBY_RREG(IR_REG_RXSTA);
    reg_val |= IR_RXSTA_ALL_MASK;
    STANDBY_WREG(IR_REG_RXSTA, reg_val);

    saddr = (__u32)(0x0fffffff & IR_REG_RXFIFO);/* 设置接收寄存器为源地址 */
    daddr = (__u32)((__u32)ir_data & 0x000fffff);

    STANDBY_WREG(DMAC_REG_N0_CFG, 0x0C110020);/* 配置DMA, 双8字节，无burst  */
    STANDBY_WREG(DMAC_REG_N0_SRC_ADDR, saddr);/* 源物理地址 */
    STANDBY_WREG(DMAC_REG_N0_DST_ADDR, daddr);/* 目的物理地址 */

    reg_val = STANDBY_RREG(DMAC_REG_N0_BYTE_CNT);
    reg_val &= 0x3ffff;
    reg_val |= IR_DMA_LEN;/* 暂时定在取100个字节数据，这个值比实际的一帧IR数据字节数要少一点  */
    STANDBY_WREG(DMAC_REG_N0_BYTE_CNT, reg_val);

    while(IR_RXSTA_RAC_MASK & STANDBY_RREG(IR_REG_RXSTA))
    {
        daddr =  STANDBY_RREG(IR_REG_RXFIFO);/* 忽略无用的数据 */
    }

    /* 启动DMA接收数据 */
    reg_val = STANDBY_RREG(DMAC_REG_N0_CFG);
    reg_val |= (__u32)1 << 31;
    STANDBY_WREG(DMAC_REG_N0_CFG, reg_val);

    return EPDK_FAIL;
}


#endif  //ALLOW_IR_WAKEUP_STANDBY

