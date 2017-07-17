/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                                 TWI module
*
*                                   (c) Copyright 2008-2009, kevin China
*                                             All Rights Reserved
*
* File    : twi.c
* By      : kevin
* Version : V1.00
* Date    : 2009-6-16 15:50
*********************************************************************************************************
*/
#include "standby_twi.h"


typedef struct tag_twic_reg
{
    volatile __u32 reg_saddr;
    volatile __u32 reg_xsaddr;
    volatile __u32 reg_data;
    volatile __u32 reg_ctl;
    volatile __u32 reg_status;
    volatile __u32 reg_clkr;
    volatile __u32 reg_reset;
    volatile __u32 reg_efr;
    volatile __u32 reg_lctl;

} __twic_reg_t;


static __u32            TwiClkRegBak = 0;
static __u32            TwiCtlRegBak = 0;/* mainly for interrup enable bit */
static __twic_reg_t     *twi_reg  = NULL;

/*
*********************************************************************************************************
*                                   TWI TRANSFER INIT
*
*Description: init twi transfer.
*
*Arguments  :
*
*Return     :
*
*********************************************************************************************************
*/
__s32 standby_twi_init(void)
{
    twi_reg  = (__twic_reg_t *)(TWI0_REG_BASE + pStandbyPar->PowerCfg.twi_controller * 0x400);
    TwiClkRegBak = twi_reg->reg_clkr;
    TwiCtlRegBak = 0x80 & twi_reg->reg_ctl; /* backup INT_EN;no need for BUS_EN(0xc0)  */
    twi_reg->reg_clkr = 0;
    twi_reg->reg_reset |= 0x1;

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   TWI TRANSFER EXIT
*
*Description: exit twi transfer.
*
*Arguments  :
*
*Return     :
*
*********************************************************************************************************
*/
__s32 standby_twi_exit(void)
{
    __u32 i = 0xff;
    /* softreset twi module  */
    twi_reg->reg_reset |= 0x1;
    /* delay */
    for(; i > 0; i--);

    /* restore clock division */
    twi_reg->reg_clkr = TwiClkRegBak;
    /* restore INT_EN */
    twi_reg->reg_ctl |= TwiCtlRegBak;
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   STOP CURRENT TWI TRANSFER
*
*Description: stop current twi transfer.
*
*Arguments  :
*
*Return     :
*
*********************************************************************************************************
*/
static __s32 _standby_twi_stop(void)
{
    __u32   nop_read;
    __u32   timeout = 0xff;

    twi_reg->reg_ctl = (twi_reg->reg_ctl & 0xc0) | 0x10;/* set stop+clear int flag */

    nop_read = twi_reg->reg_ctl;/* apb时钟低时必须假读一次stop bit,下一个周期才生效 */
    nop_read = nop_read;
    // 1. stop bit is zero.
    while((twi_reg->reg_ctl & 0x10) && (timeout--));
    if(timeout == 0)
    {
        return EPDK_FAIL;
    }
    // 2. twi fsm is idle(0xf8).
    timeout = 0xff;
    while((0xf8 != twi_reg->reg_status) && (timeout--));
    if(timeout == 0)
    {
        return EPDK_FAIL;
    }
    // 3. twi scl & sda must high level.
    timeout = 0xff;
    while((0x3a != twi_reg->reg_lctl) && (timeout--));
    if(timeout == 0)
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   TWI BYTE READ AND WRITE
*
*Description: twi byte read and write.
*
*Arguments  : op_type   operation read or write;
*             saddr     slave address;
*             baddr     byte address;
*             data      pointer to the data to be read or write;
*
*Return     : result;
*               = EPDK_OK,      byte read or write successed;
*               = EPDK_FAIL,    btye read or write failed!
*********************************************************************************************************
*/
__s32 twi_byte_rw(__s32 op_type, __u8 saddr, __u8 baddr, __u8 *data)
{
    __u8    state_tmp;
    __u32   timeout;
    __s32   ret = EPDK_FAIL;

    twi_reg->reg_efr = 0;/* 标准读写必须置0 */

    state_tmp = twi_reg->reg_status;
    if(state_tmp != 0xf8)
    {
        goto stop_out;
    }

    /* control registser bitmap
         7      6       5     4       3       2    1    0
      INT_EN  BUS_EN  START  STOP  INT_FLAG  ACK  NOT  NOT
    */

    //1.Send Start
    twi_reg->reg_ctl |= 0x20;
    timeout = 0xff;
    while((!(twi_reg->reg_ctl & 0x08)) && (timeout--));
    if(timeout == 0)
    {
        goto stop_out;
    }
    state_tmp = twi_reg->reg_status;
    if(state_tmp != 0x08)
    {
        goto stop_out;
    }

    //2.Send Slave Address
    twi_reg->reg_data = (saddr << 1) | 0; /* slave address + write */
    twi_reg->reg_ctl &= 0xf7;/* clear int flag */
    timeout = 0xff;
    while((!(twi_reg->reg_ctl & 0x08)) && (timeout--));
    if(timeout == 0)
    {
        goto stop_out;
    }
    state_tmp = twi_reg->reg_status;
    if(state_tmp != 0x18)
    {
        goto stop_out;
    }

    //3.Send Byte Address
    twi_reg->reg_data = baddr;
    twi_reg->reg_ctl &= 0xf7;/* clear int flag */
    timeout = 0xff;
    while((!(twi_reg->reg_ctl & 0x08)) && (timeout--));
    if(timeout == 0)
    {
        goto stop_out;
    }
    state_tmp = twi_reg->reg_status;
    if(state_tmp != 0x28)
    {
        goto stop_out;
    }

    if(op_type == TWI_OP_WR)
    {
        //4.Send Data to be write
        twi_reg->reg_data = *data;
        twi_reg->reg_ctl &= 0xf7;/* clear int flag */
        timeout = 0xff;
        while((!(twi_reg->reg_ctl & 0x08)) && (timeout--));
        if(timeout == 0)
        {
            goto stop_out;
        }
        state_tmp = twi_reg->reg_status;
        if(state_tmp != 0x28)
        {
            goto stop_out;
        }
    }
    else
    {
        //4. Send restart for read
        twi_reg->reg_ctl = (twi_reg->reg_ctl & 0xc0) | 0x20;/* set start+clear int flag */
        timeout = 0xff;
        while((!(twi_reg->reg_ctl & 0x08)) && (timeout--));
        if(timeout == 0)
        {
            goto stop_out;
        }
        state_tmp = twi_reg->reg_status;
        if(state_tmp != 0x10)
        {
            goto stop_out;
        }

        //5.Send Slave Address
        twi_reg->reg_data = (saddr << 1) | 1; /* slave address+ read */
        twi_reg->reg_ctl &= 0xf7;/* clear int flag then 0x40 come in */
        timeout = 0xff;
        while((!(twi_reg->reg_ctl & 0x08)) && (timeout--));
        if(timeout == 0)
        {
            goto stop_out;
        }
        state_tmp = twi_reg->reg_status;
        if(state_tmp != 0x40)
        {
            goto stop_out;
        }

        //6.Get data
        twi_reg->reg_ctl &= 0xf7;/* clear int flag then data come in */
        timeout = 0xff;
        while((!(twi_reg->reg_ctl & 0x08)) && (timeout--));
        if(timeout == 0)
        {
            goto stop_out;
        }
        *data = twi_reg->reg_data;
        state_tmp = twi_reg->reg_status;
        if(state_tmp != 0x58)
        {
            goto stop_out;
        }
    }

    ret = EPDK_OK;

stop_out:
    //WRITE: step 5; READ: step 7
    //Send Stop
    _standby_twi_stop();

    return ret;
}

