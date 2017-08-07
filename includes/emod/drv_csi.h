/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                               hello world sample
*
*                                (c) Copyright 2006-2007, Steven.ZGJ China
*                                            All    Rights Reserved
*
* File    : drv_csi.h
* By      : jshwang
* Version : V1.00
* Date    : 2009-9
*********************************************************************************************************
*/
#ifndef  _DRV_CSI_H_
#define  _DRV_CSI_H_
#define CSI_MODE_NUM 8
#define CSI_BUFFER_NUM 8
#define CSI_DEV_MAX_NUM 2

typedef enum
{
    BAYER_SEQ_RG = 0x30, //采样第一行的初始两个值的顺序，下同
    BAYER_SEQ_GR,
    BAYER_SEQ_BG,
    BAYER_SEQ_GB,
} __bayer_seq_t;

typedef enum __BAYER_STORE_MODE
{
    BAYER_MOD_INTERLEAVED = YUV_MOD_INTERLEAVED,
    BAYER_MOD_NON_MB_PLANAR = YUV_MOD_NON_MB_PLANAR,
} __bayer_store_mode_t;

typedef enum tag_BUF_SCRAB_MODE
{
    CSI0_FULL  = 0 ,       //CSI0 on one buffer
    CSI1_FULL  = 1 ,       //CSI1 on one buffer

    CSI_01_LR  = 2 ,       //CSI0(L) and CSI1(R) with same size and format parallel on one buffer
    CSI_10_LR  = 3 ,       //CSI1(L) and CSI0(R) with same size and format parallel on one buffer

    CSI_01_UD  = 4 ,       //CSI0(L) and CSI1(R) with same size and format sequential on one buffer
    CSI_10_UD  = 5 ,       //CSI1(L) and CSI0(R) with same size and format sequential on one buffer

    CSI0_TDM_2CH_2X1 = 6 ,     //CSI0 TDM CH0/1 with same size and format
    CSI0_TDM_4CH_2X2 = 7 ,     //CSI0 TDM CH0/1 + CH2/3 with same size and format

    CSI_SC_NA = 0xff,
} __buf_scrab_mode_t;

typedef struct tag_CSI_MODE
{
    __pixel_yuvfmt_t    color_format; //__pixel_yuvfmt_t
    __u32               component_seq;//__yuv_seq_t + __bayer_seq_t
    __u32               store_mode;   //__yuv_mod_t + __bayer_store_mode_t
    __rectsz_t          size;
    __u32               frame_rate;    //Hz, x1000
    __u32               frame_period;  //us
    __cs_mode_t    			color_space;
    //=========add for CSI0+CSI1 ===============
    __buf_scrab_mode_t  csi_buf_scrab_mode;
    //========================
} __csi_mode_t;

typedef struct tag_CSI_MODE_ALL
{
    __u32             number;
    __csi_mode_t      csi_mode[CSI_MODE_NUM];
} __csi_mode_all_t;

typedef enum
{
    CSI_COLOR_EFFECT_NORMAL,//无
    CSI_COLOR_EFFECT_GRAYSCALE,//单色
    CSI_COLOR_EFFECT_COLORINV,//负片
    CSI_COLOR_EFFECT_SEPIA,//复古
    CSI_COLOR_EFFECT_SEPIAGREEN,//浅绿色
    CSI_COLOR_EFFECT_SEPIABLUE,//浅蓝色
    CSI_COLOR_EFFECT_SEPIARED,//浅红色
    CSI_COLOR_EFFECT_SEPIAYELLOW,//浅黄色
} __csi_color_effect_t;

typedef enum
{
    CSI_AWB_AUTO,//自动
    CSI_AWB_DAYLIGHT,//日光
    CSI_AWB_FLUORESCENT,//荧光
    CSI_AWB_CLOUDY,//阴天
    CSI_AWB_TUNGSTEN,//白炽光
} __csi_awb_t;

typedef enum
{
    CSI_AE_N3,//-3
    CSI_AE_N2,//-2
    CSI_AE_N1,//-1
    CSI_AE_0,//0
    CSI_AE_P1,//1
    CSI_AE_P2,//2
    CSI_AE_P3,//3
} __csi_ae_t;

typedef enum
{
    CSI_BRIGHT_N3,//-3
    CSI_BRIGHT_N2,//-2
    CSI_BRIGHT_N1,//-1
    CSI_BRIGHT_0,//0
    CSI_BRIGHT_P1,//1
    CSI_BRIGHT_P2,//2
    CSI_BRIGHT_P3,//3
} __csi_bright_t;

typedef enum
{
    CSI_CONTRAST_N3,//-3
    CSI_CONTRAST_N2,//-2
    CSI_CONTRAST_N1,//-1
    CSI_CONTRAST_0,//0
    CSI_CONTRAST_P1,//1
    CSI_CONTRAST_P2,//2
    CSI_CONTRAST_P3,//3
} __csi_contrast_t;


typedef enum
{
    CSI_BAND_50HZ = 0, //50Hz
    CSI_BAND_60HZ = 1, //60Hz
} __csi_band_t; //add 2011-8-3 13:13:13


typedef enum  __DRV_CSI_CMD_SET
{
    DRV_CSI_CMD_NONE = 0,
    DRV_CSI_CMD_INIT, //ret = EPDK_OK/EPDK_FAIL
    DRV_CSI_CMD_EXIT, //ret = EPDK_OK/EPDK_FAIL
    DRV_CSI_CMD_SET_MODE, //pbuffer = __csi_mode_t*, ret = EPDK_OK/EPDK_FAIL
    DRV_CSI_CMD_GET_MODE, //pbuffer = __csi_mode_t*, ret = EPDK_OK/EPDK_FAIL
    DRV_CSI_CMD_GET_MODE_ALL, //pbuffer = __csi_mode_total_t *
    DRV_CSI_CMD_CAPTURE_ON,	//ret = EPDK_OK/EPDK_FAIL
    DRV_CSI_CMD_CAPTURE_OFF,  //ret = EPDK_OK/EPDK_FAIL
    DRV_CSI_CMD_REQUEST_FRAME, //pbuffer= (__csi_frame_t**), ret = EPDK_OK/EPDK_FAIL
    DRV_CSI_CMD_RELEASE_FRAME, //aux = frame_id, ret = EPDK_OK/EPDK_FAIL
    DRV_CSI_CMD_SET_PTS, //aux = pts(__u32)
    DRV_CSI_CMD_GET_PTS, //ret=pts(__u32)
    DRV_CSI_CMD_SET_PTS_MODE,   //设置PTS的获取方式，aux = 0:CSI自己算PTS; 1:通过回调函数得到PTS; 2.不需要PTS
    DRV_CSI_CMD_INSTALL_CALLBACK_GET_PTS, //注册回调函数,得到PTS。该callback的原型为CB_GetPTS,参数*arg是__s64*的指针,表示PTS，单位us. *pbuffer = callback
    DRV_CSI_CMD_SET_FRAME_QUEUE,        //设置frame queue.必须在capture_on之前设置。因为capture off会清掉. pbuffer = __csi_frame_queue_t*,主要是设置buffer和frame_id，其他的参数还是由csidrv决定.
    DRV_CSI_CMD_SET_COLOR_EFFECT,//aux = color effet
    DRV_CSI_CMD_GET_COLOR_EFFECT,//ret = color effet
    DRV_CSI_CMD_SET_AWB,//aux = awb
    DRV_CSI_CMD_GET_AWB,//ret = awb
    DRV_CSI_CMD_SET_AE,//aux = ae
    DRV_CSI_CMD_GET_AE,//ret = ae
    DRV_CSI_CMD_SET_BRIGHT,//aux = bright
    DRV_CSI_CMD_GET_BRIGHT,//ret = bright
    DRV_CSI_CMD_SET_CONTRAST,//aux = contrast
    DRV_CSI_CMD_GET_CONTRAST,//ret = contrast
    DRV_CSI_CMD_SET_BAND,//aux = band
    DRV_CSI_CMD_GET_BAND,//ret = band//add 2011-8-3 13:13:13
    DRV_CSI_CMD_GET_PROGRSSV_INFO,//ret = b_progr
    DRV_CSI_CMD_SET_MASTER_PORT = DRV_CSI_CMD_GET_PROGRSSV_INFO,
    DRV_CSI_CMD_GET_MASTER_PORT,
    DRV_CSI_CMD_SET_DAY_NIGHT,
    DRV_CSI_CMD_GET_INIT, 
    DRV_CSI_CMD_SET_EXIT,
	DRV_CSI_CMD_SET_MSG_Q = 0x200,	
    DRV_CSI_CMD_,
} __drv_csi_cmd_set_t;

typedef struct tag_CSI_FRAME
{
    __s32               frame_id;

    __bool              bProgressiveSrc;    // Indicating the source is progressive or not
    __bool              bTopFieldFirst;     // VPO should check this flag when bProgressiveSrc is FALSE
    __u16               eAspectRatio;       //the source picture aspect ratio
    __rect_t            src_rect;           // source valid size, 宏块对齐的图像大小, 一般就是frame buffer了
    __rect_t            dst_rect;           // source display size,真实图像的位置大小
    __u32               addr[3];            // data buffer address

    __u16               color_format; //same with __csi_mode_t
    __u32               component_seq;//same with __csi_mode_t
    __u32               store_mode;   //same with __csi_mode_t
    __u16               frame_rate;   //same with __csi_mode_t

    __s64               uPts;         //us
    __s32               bPtsValid;

} __csi_frame_t;

typedef struct tag_CSI_FRAME_QUEUE
{
    __s32           num;    //有效的帧个数，num <= CSI_BUFFER_NUM
    __csi_frame_t   csi_frame_array[CSI_BUFFER_NUM];
} __csi_frame_queue_t;

typedef struct tag_CSI_SDE
{
    __csi_color_effect_t  csi_color_effect;
    __csi_awb_t           csi_awb         ;
    __csi_ae_t            csi_ae          ;
    __csi_bright_t        csi_bright      ;
    __csi_contrast_t      csi_contrast    ;
    __csi_band_t          csi_band        ;//add 2011-8-3 13:13:13
} __csi_sde_t;


#endif   /* _DRV_CSI_H_ */
