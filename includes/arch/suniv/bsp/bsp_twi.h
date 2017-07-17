/*
*********************************************************************************************************
*                                                           eBase
*                                           the Abstract of Hardware
*
*                                   (c) Copyright 2006-2010, AW China
*                                                   All Rights Reserved
*
* File        :  bsp_twi.h
* Date        :  2011-01-22
* Author      :  Victor
* Version     :  v1.00
* Description:
*                                   Operation for TWI module,aw1619
* History     :
*      <author>          <time>             <version >          <desc>
*       Victor         2011-01-22              1.0           create this file
*
*********************************************************************************************************
*/
#ifndef _BSP_TWI_H_
#define _BSP_TWI_H_


/* 7 bits or 10 bits slave address  */
#define TWI_SLAVE_ADDR_7BITS  0
#define TWI_SLAVE_ADDR_10BITS 1

/* slave's data register address length */
#define TWI_REG_ADDR_1_BYTE    1
#define TWI_REG_ADDR_2_BYTE    2
#define TWI_REG_ADDR_3_BYTE    3
#define TWI_REG_ADDR_4_BYTE    4

/* TWI write or read command */
#define TWI_WR_FLAG  0
#define TWI_RD_FLAG  1

/* read or write format */
/* temporary support one types of write format and three types of read format */
#define TWI_WRITE_DATA_SPEC_RS            (0) //spec write
#define TWI_READ_DATA_SPEC_RS             (1) //spec read
#define TWI_READ_DATA_EX_NORS             (2) //no restart3
#define TWI_READ_DATA_EX_STPRS            (3) //stop + restart 

/* TWI 选择轮询方式或者中断方式传输*/
#define TWI_RW_POLLING_METHOD     1
#define TWI_RW_INTERRUPT_METHOD   0


#define TWI_SCLK_400KHZ    (400000) // 400kHz 
#define TWI_SCLK_100KHZ    (100000) //100kHz


typedef struct twi_para_set
{
    //读写的时候需要传入的参数
    __u16 slave_addr; //Slave地址
    __u8  slave_len;  //Slave地址长度

    __u32 data_addr; //Slave器件的数据寄存器地址
    __u8  data_len; //Slave器件的数据寄存器地址长度

    __u8  rw_flag; //读或写标志位；
    __u8  rw_format; //读方式或写方式,即各种规范或不规范的读写方式；

} TWI_PARA_SET_T;


typedef void (*TWI_CB)(__s32 ret);
/*   回调函数TWI_CB的参数ret的值如下，用户可根据返回值作调试使用。更多的可查看spec
    0   : succeed
    -1  : fail
    0xf8: On reset or stop the bus is idle
    0xd8: second slave address has been transmitted; NOT ACK has been received
    0x20: SLA+W has been transmitted; NOT ACK has been received
    0x30: Data byte in I2CDAT has been transmitted; NOT ACK has been received
    0x48: SLA+R has been transmitted; NOT ACK has been received
    0x58: Data byte has been received; NOT ACK has been transmitted
*/

typedef struct twi_setting
{
    //初始化或者申请I2C时需要传入的参数
    __s32  bus_num;
    __u32  sclk_req; //用户设定的 100kHz or 400kHz

    __u32  poll_flag;//0:使用中断方式，1:使用轮询方式，回调函数必须实现。
    // 当poll_flag为1时，可以不填写下面几个参数。回调函数必须实现。
    //irq variable，内部申请中断使用
    __u32  irq_no;
    __u32  irq_flags;
    __u32  irq_prio;

    TWI_CB wakup_callback;//wakup the upper layer,用户实现回调函数

} TWI_SETTING_T;

/*
****************************************************************************************************
*
*  FunctionName:           BSP_TWI_init
*
*  Description:
*           完成TWI的基本初始化功能，调用内部接口函数，配置寄存器，比如打开总线，设置PIN脚和APB clock，
*        设置twi模块时钟，软复位等。传入虚拟地址，根据中断号申请中断处理函数。可供上层init初始化函数。
*
*  Parameters:
*       ioremap   :   虚拟地址
*       twi       :   外部的资源句柄
*  Return value:
*           初始化成功返回0，失败返回-1。
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_TWI_init(TWI_SETTING_T *twi);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_TWI_exit
*
*  Description:
*           完成TWI的退出清理功能，调用内部接口函数，比如关闭总线，停止时钟，
*        软复位等。取消中断注册，释放twi资源句柄。可供上层exit初始化函数。
*       把apb时钟关闭，释放pin脚。
*  Parameters:
*       bus_num:  twi模块的总线号
*
*  Return value:
*         退出成功返回0，失败返回-1
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_TWI_exit(TWI_SETTING_T *twi);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_TWI_read
*
*  Description:
*               接收数据，即读操作。读操作要填充i2c的slave地址，寄存器地址，以及选择读的方式。
*           目前支持三种读方式。读规范方式支持7bit和10bit地址。其他两种读方式支持7比特地址。10bit地址
*        尚不明确。
*
*  Parameters:
*            bus_num: twi总线号码
*           para   :  设置的读写命令方式
*            buffer:  接收数据的缓冲区首地址
*            len   :  接收数据的长度。
*  Return value:
*           读成功返回0,需要pend；一般失败返回-1,不需要pend;轮询方式出错执行回调返回-2，外部需要pend.
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_TWI_read(__s32 bus_num, TWI_PARA_SET_T *para, __u8 *buffer, __u32 len);
/*
****************************************************************************************************
*
*  FunctionName:           BSP_TWI_write
*
*  Description:
*               发送数据，即写操作。写操作要填充i2c的slave地址，寄存器地址，以及选择写方式。
*           目前支持一种写方式。写规范方式支持7bit和10bit地址。
*
*  Parameters:
*           bus_num: twi总线号码
*           para   :  设置的读写命令方式
*            buffer:  发送数据的缓冲区首地址
*            len   :  发送数据的长度
*  Return value:
*           写成功返回0,需要pend；一般失败返回-1,不需要pend;轮询方式出错执行回调返回-2，外部需要pend.
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_TWI_write(__s32 bus_num, TWI_PARA_SET_T *para, const __u8 *buffer, __u32 len);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_TWI_set_clock
*
*  Description:
*              设置工作时钟，scl的时钟。
*  Parameters:
*       busnum    :  twi的总线号
*       sclk_reg : 100000 or 400000即100khz或400khz
*
*  Return value:
*         退出成功返回EBSP_TRUE，失败返回EBSP_FALSE
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_TWI_set_clock(__s32 bus_num, __u32 sclk_req);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_TWI_get_line_status
*
*  Description:
*               供检测i2c通信出错用，比如SDA或SCL被长期拉低等，通过此接口用户可以知道SDA和SCL的状态，
*           以作出错处理。调用内部函数aw_twi_get_sda和aw_twi_get_scl实现。
*
*  Parameters:
*           bus_num:     TWI模块的总线号
*            sda_scl:  选择SDA或SCL，0：SDA，1：SCL.
*  Return value:
*           返回sda或scl的状态，高电平为1， 低电平为0。错误返回-1。
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_TWI_get_line_status(__s32 bus_num, __u32 sda_scl);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_TWI_set_line_status
*
*  Description:
*           设置SDA或SCL的状态，即拉高或拉低，使用此接口前应该先调用eBSP_twi_usr_ctrl开启用户控制功能。
*        要关闭用户自控twi状态则需要调用外部接口eBSP_twi_self_ctrl。
*
*  Parameters:
*            bus_num    :  twi的总线号
*            sda_scl:  选择SDA或SCL，0：SDA，1：SCL.
*            hi_lo  :  1:高电平；0：低电平
*  Return value:
*           返回sda或scl的状态，高电平为1， 低电平为0。
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_TWI_set_line_status(__s32 bus_num, __u32 sda_scl, __u32 hi_low);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_TWI_line_ctrl
*
*  Description:
*               设置SDA或SCL的是否处于受控状态。注意设置是直接开启或关闭SDA或SCL的受控状态，
*            此函数关闭SDA和SCL受控状态，返回自控状态。
*
*  Parameters:
*            bus_num    :  twi的总线号
*            sda_scl:  选择SDA或SCL，0：SDA，1：SCL.
*            ctl    :  0：关闭用户控制，返回自控。1：使能用户控制，用户可以控制SDA或SCL的电平。
*  Return value:
*          成功返回EBSP_TRUE,失败返回EBSP_FALSE.
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_TWI_line_ctrl(__s32 bus_num, __u32 sda_scl, __u32 ctl);

#endif
