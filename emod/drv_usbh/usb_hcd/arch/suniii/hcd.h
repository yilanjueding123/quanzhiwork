/*
********************************************************************************************************************
*                                              USB Host Drvier
*
*                              (c) Copyright 2007-2009, softwinners.China
*										All	Rights Reserved
*
* File Name 	: hcd.h
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

#ifndef  __HCD__
#define  __HDC__

#include "usb_list.h"
#include "usb_host_common.h"
#include "usb_gen_hcd.h"


#define	 HCD_MOV_DATA_BY_DMA			//开启dma传输
#define  HCD_FIFO_NUM				0  	//由于接入HUB后，会接入很多设备，而且USB得FIFO大小有限，因此只能用单FIFO
//#define  HCD_INT_ISO_TRANSFER			//是否开发中断传输、同步传输



#define  HCD_CONTROLLER_NAME	"usb host controller"


//------------------------------------------------------------------------------
//  USB Host Controller Driver部分
//------------------------------------------------------------------------------

#define  FULLSPEED     0
#define  HIGHSPEED     1
#define  LOWSPEED      2

//port change
#define  PORT_C_MASK  	((USB_PORT_STAT_C_CONNECTION \
							| USB_PORT_STAT_C_ENABLE \
							| USB_PORT_STAT_C_SUSPEND \
							| USB_PORT_STAT_C_OVERCURRENT \
							| USB_PORT_STAT_C_RESET) << 16)

#define	 LOG2_PERIODIC_SIZE				5	/* arbitrary; this matches OHCI */
#define	 PERIODIC_SIZE					(1 << LOG2_PERIODIC_SIZE)
#define  MAX_PERIODIC_LOAD				500 //50%

#define  FRAME_TIME_USECS	1000L
#define  BitTime(bytecount)  (7 * 8 * bytecount / 6)  	/* with integer truncation */
/* Trying not to use worst-case bit-stuffing
           of (7/6 * 8 * bytecount) = 9.33 * bytecount */
/* bytecount = data payload byte count */
/*
 * Full/low speed bandwidth allocation constants/support.
 */
#define BW_HOST_DELAY	1000L		/* nanoseconds */
#define BW_HUB_LS_SETUP	333L		/* nanoseconds */
/* 4 full-speed bit times (est.) */

#define FRAME_TIME_BITS         12000L		/* frame = 1 millisecond */
#define FRAME_TIME_MAX_BITS_ALLOC	(90L * FRAME_TIME_BITS / 100L)
#define FRAME_TIME_MAX_USECS_ALLOC	(90L * FRAME_TIME_USECS / 100L)

/*
 * Ceiling [nano/micro]seconds (typical) for that many bytes at high speed
 * ISO is a bit less, no ACK ... from USB 2.0 spec, 5.11.3 (and needed
 * to preallocate bandwidth)
 */
#define USB2_HOST_DELAY	5	/* nsec, guess */
#define HS_NSECS(bytes) ( ((55 * 8 * 2083)/1000) \
							+ ((2083UL * (3167 + BitTime (bytes)))/1000) \
							+ USB2_HOST_DELAY)
#define HS_NSECS_ISO(bytes) ( ((38 * 8 * 2083)/1000) \
							+ ((2083UL * (3167 + BitTime (bytes)))/1000) \
							+ USB2_HOST_DELAY)

/* ep list 管理 */
#define	HCD_EP_LIST_MAX_EP_NUM			(1 + USBC_MAX_EP_NUM * 2)
#define	HCD_EP_LIST_EP0_INDEX			0

#define	HCD_EP_LIST_IN_BEGIN			1
#define	HCD_EP_LIST_IN_END				(HCD_EP_LIST_IN_BEGIN + USBC_MAX_EP_NUM - 1)
#define	HCD_EP_LIST_OUT_BEGIN			(HCD_EP_LIST_IN_END + 1)
#define	HCD_EP_LIST_OUT_END				(HCD_EP_LIST_OUT_BEGIN + USBC_MAX_EP_NUM - 1)

/* ep list 中 ep index 和 物理 ep 的转化 */
#define ep_index_list_2_phy(is_out, list_index)   	(is_out ? (list_index - HCD_EP_LIST_IN_END) : (list_index))
#define ep_index_phy_2_list(is_out, phy_index) 		(is_out ? (phy_index + HCD_EP_LIST_IN_END) : (phy_index))


#define  EP_PRIV_MASK_DIR_IN			0x10 	//其实就是高4bit来表示方向
#define  EP_PRIV_MASK_DIR_OUT 			0x00
#define  get_phy_ep_index(ep_priv)		((ep_priv)->mask_dir_local_ep_nr & 0x0f)
#define  is_in_ep(ep_priv)				((ep_priv)->mask_dir_local_ep_nr & EP_PRIV_MASK_DIR_IN)

//ctrl data阶段的方向
#define  EP_PRIV_EP_DATA_DIR_NULL		0x00
#define  EP_PRIV_EP_DATA_DIR_IN			0x01
#define  EP_PRIV_EP_DATA_DIR_OUT		0x02

enum rh_state
{
    RH_POWEROFF = 0,
    RH_POWERED,
    RH_ATTACHED,
    RH_NOATTACHED,
    RH_RESET,
    RH_ENABLE,
    RH_DISABLE,
    RH_SUSPEND,
    RH_ERR
};


/* 物理ep抽象, 用来描述一个物理ep. 同一时刻一个ep对应一个urb */
typedef struct ep_private
{
    struct usb_host_virt_endpoint *hep;	/* core层的ep 								*/
    struct usb_host_virt_dev	*udev;	/* core层的device 							*/
    struct hc_private *hc;				/* ep对应的host controller					*/
    struct urb *current_urb;	/* 当前正在被处理的urb, 方便出错的时候迅速处理urb	*/

    u8  ep_list_index;			/* 本地ep信息。在hc_priv->ep_list[]中的index 		*/
    u8	mask_dir_local_ep_nr;	/* 物理ep信息。 [4:0]: ep index, [7]: ep direction 	*/
    u8  defctrl;				/* 默认控制 									*/

    u32	maxpacket;				/* ep 最大包 									*/
    u8	nextpid;				/* flag. 记录下一次ep的动作 					*/
    u8	stopped;				/* flag. ep 是否停止工作						*/
    u16	error_count;			/* ep 出错的次数 								*/
    u16	nak_count;				/* NAK 的次数 									*/
    u32	length;					/* ep 一次传输的长度, 其目的是判断是否存在短包	*/

    /* ep 的物理信息 */
    __u8  ep_index;				/* local ep point index 						*/
    __u8  ep_num;				/* target device ep point index 				*/
    __u8  ep_ts_type;			/* ep 传输类型. ctrl, iso, int, bulk 			*/
    __u32 ep_fifo_start;		/* ep fifo start address 						*/
    __u32 ep_fifo_size;			/* ep fifo的大小 								*/
    __u32 ep_fun_addr;			/* ep function address 							*/
    __u32 ep_target_muiltTT;	/* ep对应的设备是muiltTT HUB 					*/
    __u32 ep_hub_addr;			/* ep hub address 								*/
    __u32 ep_hub_port;			/* ep hub port address 							*/
    __u32 ep_interval;			/* {rx,tx} interval register 					*/
    //	__u32 ep_host_speed;		/* ep 对应的 host 端的速度 						*/
    //	__u32 ep_target_speed;		/* ep 对应的 设备 端的速度 						*/

    //periodic schedule//
    u16	period;					/*  											*/
    u16	branch;					/*  											*/
    u16	load;					/*  											*/
    u8	ep0_data_dir;			/* ep0 数据阶段的方向 							*/
    struct ep_private *next;	/*  											*/

    //async schedule //
    u8	is_added_schedule;		/*  											*/

    volatile u8	dma_working;	/* flag. DMA is working? 						*/
    u32	dma_bytes;				/* DMA传输的数据长度  							*/

#ifdef HCD_INT_ISO_TRANSFER
    __u16 periodic_done_counter;  /* high bandwidth 							*/
#endif

    __u32 busy;					/* ep正在被调度 								*/
} ep_private_t;

/* fifo  管理  */
typedef struct tag_fifo_map
{
    u32 start;
    u32 end;
} fifo_map_t;

/* USB Host Controller */
#define	RHUB_EVNET_NULL			0x00
#define	RHUB_EVNET_PLUGIN_ACT	0x01
#define	RHUB_EVNET_PLUGOUT_ACT	0x02


/* 对host控制器的描述 */
typedef struct hc_private
{
    __cpu_sr lock;
    struct hc_gen_dev *point_gen_dev;	/* 指向hc_gen_dev						*/

    //统计信息
    u32	stat_insrmv;					/*	插入拔除							*/
    u32	stat_sof;						/*										*/
    u32	stat_lost;						/*										*/
    u8	speed;							/*										*/
    u32 port;							/* 高16为port change, 低16为port status	*/

    enum rh_state  rhstate; 			/* root hub state						*/

    struct ep_private *ep_list[HCD_EP_LIST_MAX_EP_NUM];

    u32	fifo_point;						/* 即目前fifo已经分配的offset			*/
    u32 fifo_total_size;                /* fifo大小								*/
    fifo_map_t fifo_alloc_map[HCD_EP_LIST_MAX_EP_NUM ];  /* 用来记录各个ep的fifo分配情况 */

    struct list_head async; 			/* 所有async的总入口, 存放ep_private	*/

    struct ep_private *curr_async;		/* current ep_private 					*/
    struct list_head *curr_aync_node;	/* 指向curr_aync节点,方便寻找下一个node	*/

    u32 root_hub_plug_event;			/* such as RHUB_EVNET_PLUGIN_ACT 		*/
    u32	plug_event_is_proc_comp;		/* 是否处理完毕 						*/
    u32 last_not_proc_event;			/*										*/

#ifdef HCD_INT_ISO_TRANSFER
    struct ep_private *next_periodic;
    struct ep_private *next_async;
    struct ep_private *sof_periodic_entry;
    u16 load[PERIODIC_SIZE];
    __u32 periodic_count;
    __u32 frame;
    __u32 stat_overrun;
    __u32 urb_counter;		/* for sof enable or disable */

#endif
    struct ep_private *periodic[PERIODIC_SIZE];
} hc_private_t;



static inline struct hc_private *hcd_to(struct hc_gen_dev *hcd)
{
    return (struct hc_private *) (hcd->hcd_private);
}

static inline struct hc_gen_dev *to_hcd(struct hc_private *hc_priv)
{
    return (hc_priv != NULL) ? hc_priv->point_gen_dev : NULL;
}

typedef struct tag_hc_int_pendding
{
    u32 int_pendding_tx;
    u32 int_pendding_rx;
    u32 int_pendding_misc;
} hc_int_pendding_t;

/* 是否设置地址成功 */
#define	HAS_SET_ADDR_HAPPEND_FAIL		0x00
#define	HAS_SET_ADDR_HAPPEND_TRUE		0x01

typedef struct hc_priv_env_s
{
    hc_int_pendding_t int_pendding;
    u32 has_set_addr_happend;
    u32 new_addr;
} hc_priv_env_t;



#endif   //__HDC__




