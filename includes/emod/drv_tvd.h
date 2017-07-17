/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                 TVD Driver
*
*                                    (c) Copyright 2012-2016, Kingvan.Tong China
*                                             All Rights Reserved
*
* File    : drv_tvd.h
* By      : Kingvan
* Version : v1.0
* Date    : 2012-9-27
* Descript:
* Update  : date                auther         ver     notes
*           2012-9-27 11:09:13  Kingvan.Tong   2.0     build the file.
*********************************************************************************************************
*/
#ifndef __DRIVER_TVD_H__
#define __DRIVER_TVD_H__

#define TVD_MODE_NUM 1
#define TVD_BUFFER_NUM 6

typedef enum tag_BUF_SCRAB_MODE_TVD
{
    TVD0_FULL  = 0 ,       //TVD0 on one buffer
    TVD1_FULL  = 1 ,       //TVD1 on one buffer

    TVD_01_LR  = 2 ,       //TVD0(L) and TVD1(R) with same size and format parallel on one buffer
    TVD_10_LR  = 3 ,       //TVD1(L) and TVD0(R) with same size and format parallel on one buffer

    TVD_01_UD  = 4 ,       //TVD0(L) and TVD1(R) with same size and format sequential on one buffer
    TVD_10_UD  = 5 ,       //TVD1(L) and TVD0(R) with same size and format sequential on one buffer

    TVD0_TDM_2CH_2X1 = 6 ,     //TVD0 TDM CH0/1 with same size and format
    TVD0_TDM_4CH_2X2 = 7 ,     //TVD0 TDM CH0/1 + CH2/3 with same size and format

    TVD_SC_NA = 0xff,
} __buf_scrab_mode_tvd_t;

typedef struct tag_TVD_MODE
{

    __pixel_yuvfmt_t    color_format; //__pixel_yuvfmt_t
    __u32               component_seq;//__yuv_seq_t + __bayer_seq_t
    __u32               store_mode;   //__yuv_mod_t + __bayer_store_mode_t
    __rectsz_t          size;
    __u32               frame_rate;    //Hz, x1000
    __u32               frame_period;  //us
    __cs_mode_t    			color_space;
    //=========add for TVD0+TVD1 ===============
    __buf_scrab_mode_tvd_t  tvd_buf_scrab_mode;
    //========================
} __tvd_mode_t;

typedef struct tag_TVD_MODE_ALL
{
    __u32             number;
    __tvd_mode_t      tvd_mode[TVD_MODE_NUM];
} __tvd_mode_all_t;

typedef enum  __DRV_TVD_CMD_SET
{
    DRV_TVD_CMD_NONE = 0,
    DRV_TVD_CMD_INIT, 						//ret = EPDK_OK/EPDK_FAIL
    DRV_TVD_CMD_EXIT, 						//ret = EPDK_OK/EPDK_FAIL
    DRV_TVD_CMD_SET_MODE, 					//pbuffer = __csi_mode_t*, ret = EPDK_OK/EPDK_FAIL
    DRV_TVD_CMD_GET_MODE, 					//pbuffer = __csi_mode_t*, ret = EPDK_OK/EPDK_FAIL
    DRV_TVD_CMD_GET_MODE_ALL, 				//pbuffer = __csi_mode_total_t *
    DRV_TVD_CMD_CAPTURE_ON,					//ret = EPDK_OK/EPDK_FAIL
    DRV_TVD_CMD_CAPTURE_OFF,  				//ret = EPDK_OK/EPDK_FAIL
    DRV_TVD_CMD_REQUEST_FRAME, 				//pbuffer= (__csi_frame_t**), ret = EPDK_OK/EPDK_FAIL
    DRV_TVD_CMD_RELEASE_FRAME, 				//aux = frame_id, ret = EPDK_OK/EPDK_FAIL
    DRV_TVD_CMD_SET_PTS, 					//aux = pts(__u32)
    DRV_TVD_CMD_GET_PTS, 					//ret=pts(__u32)
    DRV_TVD_CMD_SET_PTS_MODE,   			//设置PTS的获取方式，aux = 0:CSI自己算PTS; 1:通过回调函数得到PTS; 2.不需要PTS
    DRV_TVD_CMD_INSTALL_CALLBACK_GET_PTS, 	//注册回调函数,得到PTS。该callback的原型为CB_GetPTS,参数*arg是__s64*的指针,表示PTS，单位us. *pbuffer = callback
    DRV_TVD_CMD_SET_FRAME_QUEUE,        	//设置frame queue.必须在capture_on之前设置。因为capture off会清掉. pbuffer = __csi_frame_queue_t*,主要是设置buffer和frame_id，其他的参数还是由csidrv决定.
    DRV_TVD_CMD_SET_COLOR_EFFECT,			//**absence**//aux = color effet
    DRV_TVD_CMD_GET_COLOR_EFFECT,			//**absence**//ret = color effet
    DRV_TVD_CMD_SET_AWB,					//**absence**//aux = awb
    DRV_TVD_CMD_GET_AWB,					//**absence**//ret = awb
    DRV_TVD_CMD_SET_AE,						//**absence**//aux = ae
    DRV_TVD_CMD_GET_AE,						//**absence**//ret = ae
    DRV_TVD_CMD_SET_BRIGHT,					//**absence**//aux = bright
    DRV_TVD_CMD_GET_BRIGHT,					//**absence**//ret = bright
    DRV_TVD_CMD_SET_CONTRAST,				//**absence**//aux = contrast
    DRV_TVD_CMD_GET_CONTRAST,				//**absence**//ret = contrast
    DRV_TVD_CMD_SET_BAND,					//**absence**//aux = band
    DRV_TVD_CMD_GET_BAND,					//**absence**//ret = band//add 2011-8-3 13:13:13
    DRV_TVD_CMD_GET_PROGRSSV_INFO,			//**absence**//ret = b_progr
    DRV_TVD_CMD_SET_MASTER_PORT = DRV_TVD_CMD_GET_PROGRSSV_INFO,//**absence**
    DRV_TVD_CMD_GET_MASTER_PORT,			//**absence**

    DRV_TVD_CMD_SET_TV_SEL = 0x100,               //tv0/tv1

    DRV_TVD_CMD_SEL_CHANNEL,				//aux = __drv_TVD_sel_channel
    DRV_TVD_CMD_SEL_SOURCE,					//aux = TVD_SOURCE_CVBS, TVD_SOURCE_YPbPr
    DRV_TVD_CMD_PAL_NTSC,					//if TVD_SOURCE_CVBS, aux = __drv_TVD_pal_ntsc
    DRV_TVD_CMD_YPbPr_SIZE,					//if TVD_SOURCE_YPbPr, aux = __drv_TVD_YPbPr_size

    DRV_TVD_CMD_SEL_FORMAT,					//aux = tvd_mod_fmt_t
    DRV_TVD_CMD_SET_INPUT_CHANNEL,			//aux = __drv_TVD_INPUT_CHANNEL
    DRV_TVD_CMD_GET_ADC_STATUS,				// return : bit0: 1->plugin, 0->plugout ; bit4:1->pal,0:ntsc;bit8:
    DRV_TVD_CMD_ENABLE_DI_FUNTION,			// 0: disable di ,1 : enable id
    DRV_TVD_CMD_CONFIG, 					//ret = EPDK_OK/EPDK_FAIL,pbuffer = __tvd_config_para_t ;
    DRV_TVD_CMD_CHANGE_CHANNEL, 			//aux =  channel num , send command to change channel when tvd is work state
    DRV_TVD_CMD_,


} __drv_TVD_cmd_set_t;
typedef struct tag_TVD_FRAME
{
    __s32               frame_id;

    __bool              bProgressiveSrc;    // Indicating the source is progressive or not
    __bool              bTopFieldFirst;     // VPO should check this flag when bProgressiveSrc is FALSE
    __u16               eAspectRatio;       //the source picture aspect ratio
    __rect_t            src_rect;           // source valid size, 宏块对齐的图像大小, 一般就是frame buffer了
    __rect_t            dst_rect;           // source display size,真实图像的位置大小
    __u32               addr[3];            // data buffer address

    __u16               color_format; //same with __tvd_mode_t
    __u32               component_seq;//same with __tvd_mode_t
    __u32               store_mode;   //same with __tvd_mode_t
    __u16               frame_rate;   //same with __tvd_mode_t

    __s64               uPts;         //us
    __s32               bPtsValid;
} __tvd_frame_t;

typedef struct tag_TVD_FRAME_QUEUE
{
    __s32           num;    //有效的帧个数，num <= TVD_BUFFER_NUM
    __tvd_frame_t   tvd_frame_array[TVD_BUFFER_NUM];
} __tvd_frame_queue_t;

typedef enum  e_TVD_SEL_CHANNEL
{
    TVD_CHANNEL_0,
    TVD_CHANNEL_1,
    //TVD_CHANNEL_2,
    //TVD_CHANNEL_3,

    TVD_SEL_CHANNEL_MAX

} __drv_TVD_sel_channel;

typedef enum  e_TVD_SEL_SOURCE
{
    TVD_SOURCE_CVBS,
    TVD_SOURCE_YPbPr,

    TVD_SEL_SOURCE_MAX

} __drv_TVD_sel_source;

typedef enum  e_TVD_PAL_NTSC
{
    TVD_SOURCE_NTSC,
    TVD_SOURCE_PAL,

    TVD_SYSTEM_FORMAT_MAX

} __drv_TVD_system;

typedef enum  e_TVD_YPbPr_SIZE
{
    TVD_YPbPr_480i,
    TVD_YPbPr_576i,

    TVD_YPbPr_SIZE_MAX

} __drv_TVD_YPbPr_size;

typedef enum  e_TVD_INPUT_CHANNEL
{
    TVD_INPUT_CHANNEL_0,
    TVD_INPUT_CHANNEL_1,

    TVD_INPUT_CHANNEL_MAX

} __drv_TVD_INPUT_CHANNEL;
typedef enum
{
    TVD_UV_NON_MB_COMBINED_YUV420,
    TVD_UV_NON_MB_COMBINED_YUV422,
    TVD_UV_MB_COMBINED_YUV420,

    TVD_MOD_FMT_MAX

} __drv_TVD_mod_fmt_t;

typedef struct tag_TVD_CONFIG_PARA
{
    __drv_TVD_INPUT_CHANNEL	tvd_channel;
    __drv_TVD_mod_fmt_t		format ;
    __drv_TVD_system 		system ;
    __u32					filter_3D_en ;
} __tvd_config_para_t;

#endif	// __DRIVER_TVD_H__

