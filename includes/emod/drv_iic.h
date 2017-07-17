/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                               hello world sample
*
*                                (c) Copyright 2010-2012, Steven.ZGJ China
*                                            All    Rights Reserved
*
* File    : drv_twi.h
* By      : Jerry & victor
* Version : V2.00
*********************************************************************************************************
*/
#ifndef  _DRV_TWI_H_
#define  _DRV_TWI_H_

#include "typedef.h"
#include "cfgs.h"

#define      TWI_SLAVE_ADDRESS_07BIT           (0)
#define      TWI_SLAVE_ADDRESS_10BIT           (1)

typedef  enum __TWI_CMD_SET
{
    TWI_WRITE_SPEC_RS  = 0,  //写规范，支持标准的twi写，带restart
    TWI_READ_SPEC_RS      ,  //读规范，支持标准的twi读，带restart
    TWI_READ_EX_NO_RS      ,  //新的twi命令，读，不带restart
    TWI_READ_EX_STP_RS    ,  //新的twi命令，读，stop之后再restart

    TWI_SET_SCL_CLOCK     ,  //设置twi的scl时钟，典型的为100khz或400khz，即100000 或400000 单位为HZ

} __twi_cmd_set_t;


typedef struct __TWI_DEV_PARA_EX
{
    __u16    slave_addr;             //从设备地址
    __u16    slave_addr_flag;        //从设置地址宽度  0：7bit； 1：10bit
    __u8    *byte_addr;              //需要读写的数据在从设备中的地址,低字节存放低地址，高字节存放高地址
    __u16    byte_addr_width;        //从设置地址宽度  0或1：8 bit; 其它：数字代表字节数
    __u16    byte_count;             //一次读写要完成的字节数
    __u32    reserved;               //保留位
    __u8    *data;                   //数据的地址
}
__twi_dev_para_ex_t;

/**********************************************************
*   TWI 使用说明：    更新兼容新版本硬件
*
*   ES_FILE   *ftwi;
*   __twi_dev_para_t        stwi;    // 需要填充的参数
*   __u8                 data[5];    // 数据的buffer
*   __u8            byte_addr[2];    // 从设备的寄存器地址，低字节存放低地址，高字节存放高地址
*
*
*   1.  打开twi设备：
*	AW1620共有3路TWI，用户应该知道设备挂在哪一路TWI总线上,打开没有激活的TWI路数会出错。
*       ftwi = eLIBs_fopen("b:\\BUS\\TWI0", "r");//第0路twi
*	ftwi = eLIBs_fopen("b:\\BUS\\TWI1", "r");//第1路twi
*	ftwi = eLIBs_fopen("b:\\BUS\\TWI2", "r");//第2路twi
*
*   2.  填充数据结构stwi
*       如：  stwi.slave_addr       =  0x10;     // 从设备中的地址
*             stwi.slave_addr_flag  =  0   ;     // 7 bit address
*             stwi.byte_addr        = byte_addr; //从设备的寄存器地址
*             stwi.byte_addr_width  =  2;        // 可以是一个到多个数据
*             stwi.byte_count      =   5;        //要发送或接收的数据大小
*             stwi.data             = data;      // 数据buffer的地址
*
*   3.  通过iic读取数据
*           eLIBs_fioctrl(ftwi, TWI_READ_SPEC_RS,  0, (void *)&stwi);
*   4.  通过iic写入数据
*           eLIBs_fioctrl(ftwi, TWI_WRITE_SPEC_RS, 0, (void *)&stwi);
*
*
*   5.  如果遇到特殊的twi从设备，特指不需要指定数据在设备中的寄存器地址的，不需要数据结构中byte_addr的值，
*       则将byte_addr的地址赋值为0x0,  如stwi.byte_addr = 0x0， 而byte_addr_width也设置为0；其它的不需要变化。
*       场景应用: 很多iic设备在直接读取的时候可以不带地址,那样访问出来的就是最后一次设定的地址的内容,
*                 需要访问固定不变的地址的时候会用，不过这样的访问也可以用带寄存器的方式代替只是多发了地址字节而已。
*                 所以一般建议写上地址字节。
*
*   6.  使用完毕后，需要关闭 twi设备，
*           eLIBs_fclose(ftwi);
*
*       补充说明:
*           stwi.byte_count = 字节数，即 设备中，数据所在的地址的字节个数。
*           通常一个设备中的地址数在0到0xff之间，在特殊设备中，地址数可能在0到0xffffff之间
*           这时，就需要指明字节数，
*               如果是 0x65431,则 stwi.baddr_count = 3;  __u8 tmp[3];tmp[0] = 0x31;tmp[1]=0x54;tmp[2]=0x06
*               如果是 0x4321, 则 stwi.baddr_count = 2;  __u8 tmp[2];tmp[0] = 0x21;tmp[1]=0x43;
*               如果是 0x21,   则 stwi.baddr_count = 1;  __u8 tmp; tmp =0x21;
*
*
***********************************************************/

#endif   /* _DRV_TWI_H_ */
