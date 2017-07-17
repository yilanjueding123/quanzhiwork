#ifndef _CSI_IIC_H
#define _CSI_IIC_H

#ifndef iic_read_cmd_type
#define iic_read_cmd_type TWI_READ_SPEC_RS//for standard iic read
//when some special device need extra stop before restart use "TWI_READ_EX_STP_RS"
//for no-restart reading use "TWI_READ_EX_NO_RS"
#endif

__s32 iic_set_clock(ES_FILE *hdl_iic, __u32 iic_clk);

__s32 iic_read_8_8(ES_FILE *hdl_iic, __u8 slv_addr, __u8 addr, __u8 *data);
__s32 iic_write_8_8(ES_FILE *hdl_iic, __u8 slv_addr, __u8 addr, __u8 data);

__s32 iic_read_16_8(ES_FILE *hdl_iic, __u8 slv_addr, __u8 reg_msb, __u8 reg_lsb, __u8 *data);
__s32 iic_write_16_8(ES_FILE *hdl_iic, __u8 slv_addr, __u8 reg_msb, __u8 reg_lsb, __u8 data);

__s32 iic_read_16_16(ES_FILE *hdl_iic, __u8 slv_addr, __u16 addr, __u16 *data);
__s32 iic_write_16_16(ES_FILE *hdl_iic, __u8 slv_addr, __u16 addr, __u16 data);

__s32 iic_write_16_32(ES_FILE *hdl_iic, __u8 slv_addr, __u16 addr, __u32 data);


static __s32 iic_set_clock(ES_FILE *hdl_iic, __u32 iic_clk)
{
    __s32 ret = EPDK_FAIL;
    __u32 clock;

    clock = iic_clk;

    if(clock > 400 * 1000)
    {
        clock = 400 * 1000;
        __wrn("csi iic speed setting limmit to 400kHz.\n");
    }
    else if (clock < 10 * 1000)
    {
        clock = 100 * 1000;
        __wrn("csi iic speed setting error.\n");
    }
    //else

    __inf("change iic clock to %dk\n", clock / 1000); //for some i2c device can not ran at the rate of 100k

    ret = csi_fioctrl(hdl_iic, TWI_SET_SCL_CLOCK,  0, (void *)clock);

    if(ret != EPDK_OK)
    {
        __inf("change iic clock failed!\n");
    }

    return ret;
}

static __s32 iic_read_8_8(ES_FILE *hdl_iic, __u8 slv_addr, __u8 addr, __u8 *data)
{
    __u8 iic_ret;
    __twi_dev_para_ex_t  siic;
    __u8 iic_addr[1];
    __u8 *iic_data;

    iic_addr[0] = addr;
    iic_data = data;

    siic.slave_addr        = slv_addr;
    siic.slave_addr_flag   = TWI_SLAVE_ADDRESS_07BIT;
    siic.byte_addr         = iic_addr;
    siic.byte_addr_width   = 1;
    siic.data              = iic_data;
    siic.byte_count        = 1;

    iic_ret = csi_fioctrl(hdl_iic, iic_read_cmd_type,  0, (void *)&siic);
    //__inf("csi iic read data %s %x=%x\n",(iic_ret==EPDK_OK)?"OK":"FAIL", iic_addr[0],iic_data[0]);

    return iic_ret;
}

static __s32 iic_write_8_8(ES_FILE *hdl_iic, __u8 slv_addr, __u8 addr, __u8 data)
{
    __u8 iic_ret;
    __twi_dev_para_ex_t  siic;
    __u8 iic_addr[1];
    __u8 iic_data[1];

    iic_addr[0] = addr;
    iic_data[0] = data;

    siic.slave_addr        = slv_addr;
    siic.slave_addr_flag   = TWI_SLAVE_ADDRESS_07BIT;
    siic.byte_addr         = iic_addr;
    siic.byte_addr_width   = 1;
    siic.data              = iic_data;
    siic.byte_count        = 1;

    iic_ret = csi_fioctrl(hdl_iic, TWI_WRITE_SPEC_RS,  0, (void *)&siic);
    if (iic_ret != EPDK_OK)
    {
        __inf("csi iic write data %s %x=%x\n", (iic_ret == EPDK_OK) ? "OK" : "FAIL", iic_addr[0], iic_data[0]);
    }
    //if (iic_ret!=EPDK_OK) __wrn("set sensor iic write error! %x=%x\n",reg[0],reg[1]);
    return iic_ret;
}


static __s32 iic_write_16_8_onetime(ES_FILE *hdl_iic, __u8 slv_addr, __u16 reg_addr, __u8 data)
{
    __s32 iic_ret = EPDK_FAIL;
    __twi_dev_para_ex_t  siic;
    __u16 iic_addr[1];
    __u8 iic_data[1];

    iic_addr[0] = reg_addr;
    iic_data[0] = data;

    siic.slave_addr        = slv_addr;
    siic.slave_addr_flag   = TWI_SLAVE_ADDRESS_07BIT;
    siic.byte_addr         = (__u8 *)iic_addr;
    siic.byte_addr_width   = 2;
    siic.data              = iic_data;
    siic.byte_count        = 1;

    iic_ret = csi_fioctrl(hdl_iic, TWI_WRITE_SPEC_RS,  0, (void *)&siic);

    return iic_ret;
}

static __s32 iic_read_16_8(ES_FILE *hdl_iic, __u8 slv_addr, __u8 reg_msb, __u8 reg_lsb, __u8 *data)
{
    __s32 iic_ret = EPDK_FAIL;
    __twi_dev_para_ex_t  siic;
    __u8 iic_addr[2];
    __u8 *iic_data;

    iic_addr[1] = reg_msb;
    iic_addr[0] = reg_lsb;
    iic_data = data;

    siic.slave_addr        = slv_addr;
    siic.slave_addr_flag   = TWI_SLAVE_ADDRESS_07BIT;
    siic.byte_addr         = iic_addr;
    siic.byte_addr_width   = 2;
    siic.data              = iic_data;
    siic.byte_count        = 1;

    iic_ret = csi_fioctrl(hdl_iic, iic_read_cmd_type,  0, (void *)&siic);

    //__inf("csi iic read data %s %x, %x=%x\n",(iic_ret==EPDK_OK)?"OK":"FAIL", iic_addr[1],iic_addr[0],iic_data[0]);
    return iic_ret;
}

static __s32 iic_write_16_8(ES_FILE *hdl_iic, __u8 slv_addr, __u8 reg_msb, __u8 reg_lsb, __u8 data)
{
    __s32 iic_ret = EPDK_FAIL;
    __twi_dev_para_ex_t  siic;
    __u8 iic_addr[2];
    __u8 iic_data[1];

    iic_addr[1] = reg_msb;
    iic_addr[0] = reg_lsb;
    iic_data[0] = data;

    siic.slave_addr        = slv_addr;
    siic.slave_addr_flag   = TWI_SLAVE_ADDRESS_07BIT;
    siic.byte_addr         = iic_addr;
    siic.byte_addr_width   = 2;
    siic.data              = iic_data;
    siic.byte_count        = 1;

    iic_ret = csi_fioctrl(hdl_iic, TWI_WRITE_SPEC_RS,  0, (void *)&siic);

    //__inf("csi iic write data %s %x, %x=%x\n",(iic_ret==EPDK_OK)?"OK":"FAIL", iic_addr[1],iic_addr[0],iic_data[0]);
    return iic_ret;
}


static __s32 iic_read_16_16(ES_FILE *hdl_iic, __u8 slv_addr, __u16 addr, __u16 *data)
{
    __s32 ret = EPDK_FAIL;
    __twi_dev_para_ex_t  siic;
    __u8 iic_addr[2];
    __u8 *iic_data;

    iic_addr[1] = addr >> 8;
    iic_addr[0] = addr;

    iic_data = (__u8 *) data;


    siic.slave_addr        = slv_addr;
    siic.slave_addr_flag   = TWI_SLAVE_ADDRESS_07BIT;
    siic.byte_addr         = iic_addr;
    siic.byte_addr_width   = 2;
    siic.data              = iic_data;
    siic.byte_count        = 2;

    ret = csi_fioctrl(hdl_iic, iic_read_cmd_type,  0, (void *)&siic);

    if(ret == EPDK_OK)
    {
        __u8 tmp = iic_data[0];
        iic_data[0] = iic_data[1];
        iic_data[1] = tmp;
    }

    //__inf("iic read ret=%d, %x=%x\n", ret, iic_addr[1]<<8|iic_addr[0], iic_data[1]<<8|iic_data[0]);

    return ret;
}


static __s32 iic_write_16_16(ES_FILE *hdl_iic, __u8 slv_addr, __u16 addr, __u16 data)
{
    __s32 ret = EPDK_FAIL;
    __twi_dev_para_ex_t  siic;
    __u8 iic_addr[2];
    __u8 iic_data[2];
    iic_addr[1] = addr >> 8;
    iic_addr[0] = addr;
    iic_data[0] = data >> 8;
    iic_data[1] = data;

    siic.slave_addr        = slv_addr;
    siic.slave_addr_flag   = TWI_SLAVE_ADDRESS_07BIT;
    siic.byte_addr         = iic_addr;
    siic.byte_addr_width   = 2;
    siic.data              = iic_data;
    siic.byte_count        = 2;

    ret = csi_fioctrl(hdl_iic, TWI_WRITE_SPEC_RS,  0, (void *)&siic);

    //__inf("iic write ret=%d, %x=%x\n", ret, iic_addr[1]<<8|iic_addr[0], iic_data[1]<<8|iic_data[0]);

    return ret;
}

static __s32 iic_write_16_32(ES_FILE *hdl_iic, __u8 slv_addr, __u16 addr, __u32 data) // reg aligned by __u16,__u16,__u16
{
    __s32 ret = EPDK_FAIL;
    __twi_dev_para_ex_t  siic;
    __u8 iic_addr[2];
    __u8 iic_data[4];
    iic_addr[1] = addr >> 8;
    iic_addr[0] = addr;
    iic_data[0] = data >> 24;
    iic_data[1] = data >> 16;
    iic_data[2] = data >> 8;
    iic_data[3] = data;

    siic.slave_addr        = slv_addr;
    siic.slave_addr_flag   = TWI_SLAVE_ADDRESS_07BIT;
    siic.byte_addr         = iic_addr;
    siic.byte_addr_width   = 2;
    siic.data              = iic_data;
    siic.byte_count        = 4;

    ret = csi_fioctrl(hdl_iic, TWI_WRITE_SPEC_RS,  0, (void *)&siic);

    //__inf("iic write ret=%d, %x=%x\n", ret, iic_addr[1]<<8|iic_addr[0], iic_data[3]<<24|iic_data[2]<<16|iic_data[1]<<8|iic_data[0]);

    return ret;
}





#endif

