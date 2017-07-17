/*
********************************************************************************************************************
*                                              USB Host Drvier
*
*                              (c) Copyright 2007-2009, softwinners.China
*										All	Rights Reserved
*
* File Name 	: hcd_i.h
*
* Author 		: javen
*
* Version 		: 1.0
*
* Date 			: 2009.10.12
*
* Description 	:
*
* History 		:
*
********************************************************************************************************************
*/

#ifndef  __HCD_I__
#define  __HDC_I__


//------------------------------------------------------------------------------
//  系统配置相关
//------------------------------------------------------------------------------

//----------------------------------------------------------
//
//----------------------------------------------------------
#define  SET_USB_PARA				"usb_para"
#define  SET_USB0					"usbc0"
#define  SET_USB1					"usbc1"
#define  SET_USB2					"usbc2"

#define  KEY_USB_GLOBAL_ENABLE		"usb_global_enable"
#define  KEY_USBC_NUM				"usbc_num"

#define  KEY_USB_ENABLE				"usb_enable"
#define  KEY_USB_PORT_TYPE			"usb_port_type"
#define  KEY_USB_DETECT_TYPE		"usb_detect_type"
#define  KEY_USB_ID_GPIO			"usb_id_gpio"
#define  KEY_USB_DETVBUS_GPIO		"usb_det_vbus_gpio"
#define  KEY_USB_DRVVBUS_GPIO		"usb_drv_vbus_gpio"
#define  KEY_USB_SCAN_DELAY			"usb_scan_delay"


enum _usb_pio_group_type
{
    GROUP_TYPE_PIO = 0,
    GROUP_TYPE_P1
};

enum usb_port_type
{
    USB_PORT_TYPE_DEVICE = 0,
    USB_PORT_TYPE_HOST,
    USB_PORT_TYPE_OTG
};

enum usb_detect_type
{
    USB_DETECT_TYPE_DP_DM  = 0,
    USB_DETECT_TYPE_VBUS_ID,
    USB_DETECT_TYPE_ID_DP_DM,
    USB_DETECT_TYPE_BALD,
    USB_DETECT_TYPE_SWITCH_DPDM
};

enum usb_controller
{
    USB_CONTROLLER_NONE = 0,
    USB_CONTROLLER_0,
    USB_CONTROLLER_1,
    USB_CONTROLLER_ALL
};

/* pio信息 */
typedef struct tag_borad_pio
{
    __u32 valid;          	/* pio是否可用。 0:无效, !0:有效	*/

    __u32 group_type;		/* pio类型 							*/
    __u32 ex_pin_no;
    user_gpio_set_t gpio_set;
}
board_pio_t;

/* usb port 信息 */
typedef struct tag_borad_usb_port
{
    __u32 valid;          	/* port是否可用。 0:无效, !0:有效		*/

    __u32 port;				/* usb端口号 							*/
    /* 0: port0;   	1: port1;   else: port无效 */
    __u32 port_type;    		/* usb端口类型。						*/
    /* 0: device only; 1: host only; 2: otg */
    __u32 detect_type;    	/* usb检测方式。						*/
    /* 0: vbus/id检测;	1: dp/dm检测 		*/
    __u32 scan_delay;    	/* usb host检测延时时间	         		*/
    /* 1: 延时3s  2:延时6s 3:延时9s 4:延时15s*/

    board_pio_t id;			/* usb id pin信息 						*/
    board_pio_t det_vbus;	/* usb vbus pin信息 					*/
    board_pio_t drv_vbus;	/* usb drv_vbus pin信息 				*/
} board_usb_port_t;

typedef struct tag_board_usb_cfg
{
    board_usb_port_t port[USBC_MAX_CTL_NUM];	/* usb port 配置信息 	*/
} __board_usb_cfg_t;

/* 板级信息 */
typedef struct tag_usb_board_info
{
    u32  chip;
    u32  pid;
    u32  sid;
    u32  bid;

    u32 usb_global_enable;
    u32 usbc_num;

    __board_usb_cfg_t usb_port;
} usb_board_info_t;

/* 系统配置 */
typedef struct tag_hcd_sys_cfg
{
    void *id_pin_hdle;
    void *vbus_pin_hdle;
    void *drv_vbus_pin_hdle;
} hcd_sys_cfg_t;

//------------------------------------------------------------------------------
//  DMA 部分
//------------------------------------------------------------------------------
#define  DRQ_TYPE_USB0   	    0x04
#define  DRQ_TYPE_SDRAM   	    0x01

#define  USB_Width   			0x02    //0-8bit; 1-16bit; 2-32bit
#define  MEM_Width(x)   		(((x) == 8) ?	0x00 : 0x02) /* 非4字节对其就使用0x00 */

#define  USB_Burst   			0x01    //0-Single; 1-Burst 4
#define  MEM_Burst(x) 			(((x) == 8) ?	0x00 : 0x01)

#define  ADDR_MODE_USB          0x01
#define  ADDR_MODE_SDRAM        0x00

#define  DMA_REST_CNT			(1 << 15)   /* 剩余字节数 */

#define  USB_DMA_CONFIG 		((USB_Width << 8) | (USB_Burst << 7) | (ADDR_MODE_USB << 4) | DRQ_TYPE_USB0)
#define  MEM_DMA_CONFIG(x) 		((MEM_Width(x) << 8) | (MEM_Burst(x) << 7) | (ADDR_MODE_SDRAM << 0) | DRQ_TYPE_SDRAM)

#define  MEM2USB_CONFIG(x) 		(0x80000000 | (USB_DMA_CONFIG << 16) | MEM_DMA_CONFIG(x) | DMA_REST_CNT)
#define  USB2MEM_CONFIG(x) 		(0x80000000 | (MEM_DMA_CONFIG(x) << 16) | USB_DMA_CONFIG | DMA_REST_CNT)

//------------------------------------------------------------------------------
//  USB 部分
//------------------------------------------------------------------------------
#define  USB_EP1_START_ADDR		512  /* ep固定使用0~64 */


#endif   //__HDC_I__




