/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          ir keyboard driver
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : drv_csi_i.h
* By      : jshwang
* Version : V1.00
*********************************************************************************************************
*/
#ifndef _CSI_DEV_SELECT_H_
#define _CSI_DEV_SELECT_H_

#define CSI_PORT 0//connect device in CSI0//do not change this

#define CSI_DEV_TYPE_NONE       	0
//#define CSI_DEV_TYPE_OV7670     1
//#define CSI_DEV_TYPE_OV2642     2
//#define CSI_DEV_TYPE_BIT1605    3
//#define CSI_DEV_TYPE_OV3640     4
//#define CSI_DEV_TYPE_ADV7181C   5
//#define CSI_DEV_TYPE_S5K5CAG    6
#define CSI_DEV_TYPE_OV7725     	7
#define CSI_DEV_TYPE_OV2643     	8
#define CSI_DEV_TYPE_OV5640     9
#define CSI_DEV_TYPE_S5K4AWFX   	10
#define CSI_DEV_TYPE_TVP5150    	11
#define CSI_DEV_TYPE_TW2866     	12
///#define CSI_DEV_TYPE_MT9M113   10 //not finished
//========================以下需要修改====================================================================
//配置具体的CSI设置类型//CSI0/1
#define CSI_DEV_TYPE      CSI_DEV_TYPE_TVP5150//CSI_DEV_TYPE_OV5640

//注意下面对iic端口的定义，视方案电路填 TWI0/1/2，EVB上固定为TWI2
// for VER1 USE TWI1 , FOR per2 use twi0
#define CSI_IIC_PORT   "b:\\BUS\\TWI1"
//========================================================================================================

#define CSI_PIN_NUM    15

#endif  /* _CSI_DEV_SELECT_H_ */



