/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                               standby module
*
*                                   (c) Copyright 2011-2014, sunny China
*                                            All Rights Reserved
*
* File    : standby_matrixkey.c
* By      : sunny
* Version : V1.0
* Date    : 2011-5-11 9:27:10
*********************************************************************************************************
*/

#include "standby_matrixkey.h"

#if (ALLOW_MATRIXKEY_WAKEUP_STANDBY)

//PIO : PA13, PA14, PA15
static __u32 PC1CfgBak = 0;
static __u32 PCDataBak = 0;

//PIO : PA3
static __u32 PA0CfgBak = 0;
static __u32 PADataBak = 0;

/*
*********************************************************************************************************
*                                     INIT KEY FOR STANDBY
*
* Description: init key for standby.
*
* Arguments  : none
*
* Returns    : EPDK_OK;
*********************************************************************************************************
*/
__s32 standby_matrixkey_init(void)
{
    volatile  __u32    *tmp_addr;
    __u32               tmp_data;
    __u32               tmp1;
    __u32               tmp2;
    __u32               port;
    __u32               port_num;

    //back PIO config and data register
    PC1CfgBak = PIO_REG_CFG_VALUE(3, 15 >> 3);
    PA0CfgBak = PIO_REG_CFG_VALUE(1, 3 >> 3);
    PCDataBak = PIO_REG_DATA_VALUE(3);
    PADataBak = PIO_REG_DATA_VALUE(1);

    //设置PW, port:PA3
    port = 1;
    port_num = 3;

    //设置为输入
    tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
    tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x07 << tmp1);
    tmp_data |=  (0x00 << tmp1);
    *tmp_addr = tmp_data;

    //设置为下拉
    tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
    tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x03 << tmp2);
    tmp_data |=  (0x02 << tmp2);
    *tmp_addr = tmp_data;


    //设置SD2-D1, SD2_D1 = port:PC13
    port = 3;
    port_num = 13;

    //设置SD2-D1为输入
    tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
    tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x07 << tmp1);
    tmp_data |=  (0x00 << tmp1);
    *tmp_addr = tmp_data;

    //设置SD2-D1为下拉
    tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
    tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x03 << tmp2);
    tmp_data |=  (0x02 << tmp2);
    *tmp_addr = tmp_data;

    //设置SD2-D2, SD2_D2 = port:PC14
    port = 3;
    port_num = 14;

    //设置SD2-D2为输入
    tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
    tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x07 << tmp1);
    tmp_data |=  (0x00 << tmp1);
    *tmp_addr = tmp_data;

    //设置SD2-D2为下拉
    tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
    tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x03 << tmp2);
    tmp_data |=  (0x02 << tmp2);
    *tmp_addr = tmp_data;

    //设置SD2-D3, SD2_D3 = port:PC15
    port = 3;
    port_num = 15;

    //设置SD2-D3为输出
    tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
    tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x07 << tmp1);
    tmp_data |=  (0x01 << tmp1);
    *tmp_addr = tmp_data;

    //设置SD2-D3为上拉
    tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
    tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x03 << tmp2);
    tmp_data |=  (0x01 << tmp2);
    *tmp_addr = tmp_data;

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     EXIT KEY FOR STANDBY
*
* Description: exit key for standby.
*
* Arguments  : none
*
* Returns    : EPDK_OK;
*********************************************************************************************************
*/
__s32 standby_matrixkey_exit(void)
{
    //restore PIO config and data register
    STANDBY_WREG(PIO_REG_CFG(3, 15 >> 3), PC1CfgBak);
    STANDBY_WREG(PIO_REG_CFG(1, 3 >> 3), PA0CfgBak);
    STANDBY_WREG(PIO_REG_DATA(3), PCDataBak);
    STANDBY_WREG(PIO_REG_DATA(1), PADataBak);

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     QUERY KEY FOR WAKEUP STANDBY
*
* Description: query key for wakeup standby.
*
* Arguments  : none
*
* Returns    : result;
*               EPDK_TRUE,      get a key;
*               EPDK_FALSE,     no key;
*********************************************************************************************************
*/
__s32 standby_query_matrixkey(void)
{
    __u8                d1_status;
    __u8                d2_status;
    volatile  __u32    *tmp_addr;
    __u32               tmp_data;
    __u32               port;
    __u32               port_num;

    //检测是否pw键被按下
    port = 1;
    port_num = 3;
    tmp_data = PIO_REG_DATA_VALUE(port);
    d1_status = (tmp_data >> port_num) & 0x01;
    if (d1_status == 1)
    {
        //有键按下
        return EPDK_TRUE;
    }

    //SD2-D3输出高电平,读SD2-D1,SD2-D2状态位
    port = 3;
    port_num = 15;
    tmp_addr = PIO_REG_DATA(port);
    tmp_data = *tmp_addr;
    tmp_data &= ~(1 << port_num);
    tmp_data |=  (1 << port_num);
    *tmp_addr = tmp_data;

    tmp_data = PIO_REG_DATA_VALUE(port);
    port_num = 14;
    d2_status = (tmp_data >> port_num) & 0x01;
    port_num = 13;
    d1_status = (tmp_data >> port_num) & 0x01;
    if (d1_status == 1 || d2_status == 1)
    {
        //有键按下
        return EPDK_TRUE;
    }

    //无任何键按下
    return EPDK_FALSE;
}

#endif  //ALLOW_MATRIXKEY_WAKEUP_STANDBY
