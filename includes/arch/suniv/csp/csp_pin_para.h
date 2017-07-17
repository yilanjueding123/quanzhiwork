/*
*********************************************************************************************************
*											        eBase
*						                the Abstract of Hardware
*									           the OAL of PIO
*
*						        (c) Copyright 2006-2010, AW China
*											All	Rights Reserved
*
* File    	: 	pin.h
* Date		:	2010-11-15
* By      	: 	holi
* Version 	: 	V1.00
* Description:
*
*********************************************************************************************************
*/
#ifndef	__CSP_PIN_PARA_H__
#define	__CSP_PIN_PARA_H__

#define   DATA_TYPE_SINGLE_WORD  (1)
#define   DATA_TYPE_STRING       (2)
#define   DATA_TYPE_MULTI_WORD   (3)
#define   DATA_TYPE_GPIO_WORD    (4)

#define   SCRIPT_PARSER_OK       (0)
#define   SCRIPT_PARSER_EMPTY_BUFFER   	   (-1)
#define   SCRIPT_PARSER_KEYNAME_NULL   	   (-2)
#define   SCRIPT_PARSER_DATA_VALUE_NULL	   (-3)
#define   SCRIPT_PARSER_KEY_NOT_FIND       (-4)
#define   SCRIPT_PARSER_BUFFER_NOT_ENOUGH  (-5)

typedef unsigned long H_PARSER;


typedef struct
{
    char  gpio_name[32];
    int port;
    int port_num;
    int mul_sel;
    int pull;
    int drv_level;
    int data;
}
user_gpio_set_t;

typedef struct
{
    char  gpio_name[32];
    int port;
    int port_num;
    int mul_sel;
    int pull;
    int drv_level;
    int data;
}
script_gpio_set_t;
//=======================================================================================================
//======================================                        =========================================
//=======================================================================================================

#define	PIN_LOG_DEV_MAX		(PIN_DEV_MAX + 0x04)				//目前支持的最大逻辑设备数
#define	PIN_MAX_NR			512				//目前支持的最大PIN数目

#define	PIN_CFG_NAME_LEN	16


//=======================================================================================================
//======================================                        =========================================
//=======================================================================================================

#define	PORT_PINS_MAX			32
#define	FUNC_SELECT_ITEM_MAX	9			//
#define	PIN_NR_MAX				(PORT_PINS_MAX*FUNC_SELECT_ITEM_MAX)		//pin的最大数目
#define	DEV_NR_MAX				128			//系统内设备的最大数目
#define	DEVID_2_LOGPIN_MAX		64			//一个设备允许的最大log管脚数目


//=========================================================================
//==由于一类外设可能存在多个控制器，但这些控制器又未必是完全对称，
//=========================================================================



typedef enum
{
    PIN_PULL_DEFAULT 	= 	0xFF,
    PIN_PULL_DISABLE 	=	0x00,
    PIN_PULL_UP			=	0x01,
    PIN_PULL_DOWN		=	0x02,
    PIN_PULL_RESERVED	=	0x03
} pin_pull_t;



typedef	enum
{
    PIN_MULTI_DRIVING_DEFAULT	=	0xFF,
    PIN_MULTI_DRIVING_0			=	0x00,
    PIN_MULTI_DRIVING_1			=	0x01,
    PIN_MULTI_DRIVING_2			=	0x02,
    PIN_MULTI_DRIVING_3			=	0x03
} pin_multi_driving_t;

typedef enum
{
    PIN_DATA_LOW    ,
    PIN_DATA_HIGH   ,
    PIN_DATA_DEFAULT = 0XFF
} pin_data_t;


typedef enum
{
    PIN_INT_POSITIVE_EDGE   =   0x0,
    PIN_INT_NEGATIVE_EDGE   =   0x1,
    PIN_INT_HIGN_LEVEL      =   0x2,
    PIN_INT_LOW_LEVEL       =   0x3,
    PIN_INT_DOUBLE_EDGE     =   0x4
} pin_int_mode_t;

typedef enum
{
    PIN_INT_SRC_CLK_32K = 0x0,
    PIN_INT_SRC_CLK_24M = 0x1
} pin_int_source_clk_t;

#define	PRIV_0_IS_USED	0


//=======================================================================================================
//======================================         handler         =========================================
//=======================================================================================================
#define	PIN_TYPE_DEFAULT		0x00
#define	PIN_TYPE_DEV			0x01
#define	PIN_TYPE_GPIO			0x02

struct gpio_pin_info
{
    u16 phy_pin_group;
    u16 phy_pin_offset;
};

#define	PIN_PHY_GROUP_A			0x00
#define	PIN_PHY_GROUP_B			0x01
#define	PIN_PHY_GROUP_C			0x02
#define	PIN_PHY_GROUP_D			0x03
#define	PIN_PHY_GROUP_E			0x04
#define	PIN_PHY_GROUP_F			0x05
#define	PIN_PHY_GROUP_G			0x06
#define	PIN_PHY_GROUP_H			0x07
#define	PIN_PHY_GROUP_I			0x08
#define	PIN_PHY_GROUP_J			0x09
#define	PIN_NOT_USED	        0xff	//没有使用的PIN，如SPI的CS，IP中有4个，但实际可能只用2个，没有的两个则用NOT_USED表示


#define	PIN_XXX_NA		0xFF
#define PIN_FUNC_NA 	PIN_XXX_NA
#define PIN_PULL_NA 	PIN_XXX_NA
#define PIN_DRIV_NA 	PIN_XXX_NA
#define PIN_FUNC_NA 	PIN_XXX_NA

#endif	//__CSP_PIN_PARA_H__

