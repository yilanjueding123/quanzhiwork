/*
********************************************************************************
*                                    eMOD
*                   the Easy Portable/Player Develop Kits
*                               mod_lotus sub-system
*
*          (c) Copyright 2009-2010, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : mod_lotus.h
* Version: V1.0
* By     : Eric_wang
* Date   : 2009-12-21
********************************************************************************
*/
#ifndef _MOD_LOTUS_H_
#define _MOD_LOTUS_H_

#define OVERLAY_MAX_SRC_NUM_PER_GROUP 32              	/*  每一组水印中图片个最大的数目*/
#define LOTUS_FPATH_SIZE         (1024)       /* 媒体文件路径名长度最大值  */
#define LOTUS_FILE_TITLE_SIZE    (1024)
#define LOTUS_FRAME_RATE_BASE         1000

#define LOTUS_MUTE_ON       (1)
#define LOTUS_MUTE_OFF      (0)

#define LOTUS_MD_SEN_LEVEL      (2)

/*******************************************************************************
* type name: __lotus_cmd_set_t
* Description:
*    媒体录制中间件命令列表(IOCtrol), 控制函数原型:
*    __s32 esMODS_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
*    mp: lotus中间件模块句柄
*    cmd: __lotus_cmd_set_t定义的命令字
*    aux: lotus object, 由 LOTUS_CMD_NEW_OBJ 命令获取
*    pbuffer: 具体命令的输入/输出参数, 不同的命令输入不同。
*             一般来说，如果是结构体需要传入指针，枚举值直接传入具体值
*    ret = EPDK_OK/EPDK_FAIL
*******************************************************************************/
typedef enum __LOTUS_CMD_SET
{
    LOTUS_CMD_NEW_OBJ                     = 0x1100,       /* 新建一个lotus object , ret = lotus object*/
    LOTUS_CMD_DEL_OBJ                     = 0x1101,       /* 新建一个lotus object , ret = lotus object*/
    LOTUS_CMD_SET_CSI                     = 0x1102,       /* 设置csi 设备给lotus obj, pbuffer = index of csi dev */
    LOTUS_CMD_ENABLE_PLAYBACK             = 0x1103,       /* 是否需要预览, pbuffer = 0表示关闭预览, 1表示打开预览*/



    //文件格式，音视频编码格式设置，插件手动加载操作
    LOTUS_CMD_SET_MODE                    = 0x1104,       /* IDLE下调用，设置使用模式，分拍照、录音、录像模式,
                                                           * pbuffer = __lotus_record_mode_t，
                                                           */

    LOTUS_CMD_SET_MEDIAFILE               = 0x1105,       /* IDLE,STANDBY设置录制的媒体文件的格式和参数信息。
                                                           * pbuffer = (__lotus_media_file_inf *)，
                                                           * 必须在LOTUS_CMD_SET_MODE之后调用
                                                           */

    LOTUS_CMD_GET_OUT_MODE_ALL            = 0x1106,       /* 得到CSI_DRV支持的输出格式列表，
                                                           * (pbuffer = __csi_mode_all_t*),
                                                           */

    LOTUS_CMD_SET_PREVIEW_OUT_MODE        = 0x1107,       /* 设置preview下的图像的大小和帧率和其他信息,
                                                           * (pbuffer = __csi_mode_t*)，
                                                           * 从__csi_mode_all_t中选一组设置即可
                                                           */

    LOTUS_CMD_NEED_PREVIEW_PIC_FOR_RECORD = 0x1108,       /* 设置录像时是否需要把第一帧编码为图片,
                                                           * pbuffer = FB*, 应用程序自己配置好
                                                           */

    //控制命令
    LOTUS_CMD_START                       = 0x1200,       /* 状态:idle->standby,启动中间件,
                                                           * 该命令执行后，用户可以看到图像
                                                           */
    LOTUS_CMD_STOP                        = 0x1201,       /* 停止中间件，卸载video_plybk模块和其他所有模块，准备退出*/
    LOTUS_CMD_START_RECORD                = 0x1202,       /* 开始录像 */
    LOTUS_CMD_PAUSE_RECORD                = 0x1203,       /* 暂停录制 */
    LOTUS_CMD_CONTI_RECORD                = 0x1204,       /* 恢复录制 */
    LOTUS_CMD_STOP_RECORD                 = 0x1205,       /* 停止录制 */
    LOTUS_CMD_TAKE_IMAGE                  = 0x1206,       /* 拍照,
                                                           * pbuffer = 目标(FB*)，内存必须已分配，而且是ARGB格式，
                                                           * 用于存放缩略图的buffer
                                                           */
    LOTUS_CMD_GET_STATUS                  = 0x1207,       /* 获取媒体录制中间件当前状态 */
    LOTUS_CMD_HOLD_ON                     = 0x1208,       /* 保存当前文件，同时保持录制 */
    LOTUS_CMD_SET_HOLDING_TIME            = 0x1209,       /* 设置存放在缓冲buffer中的编码帧的时间*/
    LOTUS_CMD_SEEK_TIME                   = 0x120A,       /* 用于设置开始录制的时间, 在开始录制之前调用,
                                                           * pbuffer = time(ms),pbuffer < 0表示从当前时间之前的pbuffer ms开始
                                                           * 录制 pbuffer > 0暂时不支持*/



    LOTUS_CMD_GET_CUR_TIME                = 0x1300,       /* 获取当前录制时间 ,
                                                           * return = current encode time(ms)
                                                           */
    LOTUS_CMD_GET_REC_FILELEN             = 0x1301,       /* 获取当前录制数据长度 ,
                                                           * pbuffer = (__s64 *),current length(bytes)
                                                           */
    // 音频相关
    LOTUS_CMD_MUTE                        = 0x1400,       /* 静音开/关, 设置后所有录像文件静音
                                                           * (pbuffer = LOTUS_MUTE_ON/LOTUS_MUTE_OFF)
                                                           */
    LOTUS_CMD_SET_REC_VOLUME			= 0x1401,		/* 设置录像时录音音量的大小pbuffer = volume*/


    //视频相关
    LOTUS_CMD_ADD_OVERLAY_SRC             = 0x1500,       /* 添加一组block资源,
                                                           * (pbuffer = __lotus_overlay_src_init_t*)
                                                           */
    LOTUS_CMD_SHOW_OVERLAY_BLK            = 0x1501,       /* 显示一个block,
                                                           * (pbuffer = __lotus_overlay_dis_par_t*)
                                                           */

    LOTUS_CMD_SET_MD_PARA                 = 0x1530,       /* 设置移动检测阈值,
                                                           * (pbuffer = __lotus_motion_dectect_para_t*)
                                                           */
    LOTUS_CMD_MD_ONOFF                    = 0x1531,       /* 开启/关闭移动检测,
                                                           * (pbuffer = 1)表示开启, (pbuffer = 0)表示关闭
                                                           */
    LOTUS_CMD_GET_MD_DETECT               = 0x1532,       /* 获取移动检测结果,
                                                           * (*pbuffer = 1)表示检测到移动,(*pbuffer = 0)表示未检测到移动
                                                           */
    LOTUS_CMD_REG_SYNC_CB                 = 0x1533,       /* 设置同步回调函数,
                                                           * 在开始编码后, 每编码一帧会调用GetCurExtData获取附属数据,
                                                           * muxer写文件完毕后会调用DealWithExtData
                                                           * (pbuffer = __lotus_sync_callback_t *)
                                                           */
    LOTUS_CMD_SET_PRESCALE                = 0x1544,       /* 设置视频帧采集系数
                                                           * (pbuffer = prescale)
                                                           * 设置后每prescale帧编码一帧, 若prescale<0, 表示每帧编码prescale次
                                                           */

    //显示相关
    LOTUS_CMD_SET_VID_LAYERHDL            = 0x1600,       /* 设置图层, pbuffer = layer handle ,*/
    LOTUS_CMD_SET_VID_SHOW_MODE           = 0x1601,       /* 设置显示模式 (pbuffer = __lotus_vid_window_ratio_mode_t)*/
    LOTUS_CMD_SET_VID_WINDOW              = 0x1602,       /* 设置显示的窗口大小,(pbuffer = __rect_t *)*/
    LOTUS_CMD_SWITCH_VID_SHOW             = 0x1603,       /* 是否显示视频,
                                                           * (pbuffer = 1),打开视频显示，(pbuffer = 0),关闭视频显示
                                                           */
    LOTUS_CMD_SHOW_HALF                   = 0x1604,       /* 多摄像头单路输入时，选择其中一路信号显示，
                                                           * pbuffer = (__lotus_src_scn_set_t )
                                                           */
    LOTUS_CMD_SET_PREVIEW_PRESCALE		= 0x1605,		/*	设置每pbuffer 帧，显示一帧*/
    LOTUS_CMD_SET_DIGITAL_ZOOM_FACTOR	= 0x1606,		/*	设置每pbuffer 帧，显示一帧*/
    LOTUS_CMD_SET_PREVIEW_ROTATE		= 0x1607,		 /* (pbuffer = __lotus_preview_rotate_t) */
    //CSI设置相关
    LOTUS_CMD_SET_COLOR_EFFECT            = 0x1700,       /* pbuffer = __csi_color_effect_t */
    LOTUS_CMD_GET_COLOR_EFFECT            = 0x1701,       /* ret = __csi_color_effect_t */
    LOTUS_CMD_SET_AWB                     = 0x1702,       /* pbuffer = __csi_awb_t */
    LOTUS_CMD_GET_AWB                     = 0x1703,       /* ret = __csi_awb_t */
    LOTUS_CMD_SET_AE                      = 0x1704,       /* pbuffer = __csi_ae_t */
    LOTUS_CMD_GET_AE                      = 0x1705,       /* ret = __csi_ae_t */
    LOTUS_CMD_SET_BRIGHT                  = 0x1706,       /* pbuffer = __csi_bright_t */
    LOTUS_CMD_GET_BRIGHT                  = 0x1707,       /* ret = __csi_bright_t */
    LOTUS_CMD_SET_CONTRAST                = 0x1708,       /* pbuffer = __csi_contrast_t */
    LOTUS_CMD_GET_CONTRAST                = 0x1709,       /* ret = __csi_contrast_t */
    LOTUS_CMD_SET_BAND                    = 0x170A,       /* pbuffer = __csi_band_t */
    LOTUS_CMD_GET_BAND                    = 0x170B,       /* ret = __csi_band_t */

    /* muxer 输出相关*/
    LOTUS_CMD_SET_MUXER_OUT               = 0x1800,       /* (pbuffer = __lotus_muxer_type_t) */

    /* sensor source rotate  */
    LOTUS_CMD_SET_SOURCE_ROTATE			  = 0x1900,		  /* (pbuffer = __lotus_source_rotate_t) */
    LOTUS_CMD_GET_SOURCE_ROTATE			  = 0x1901,		  /* (return = __lotus_source_rotate_t) */

    LOTUS_CMD_SET_PREVIEW_FACTOR		  = 0x2000,		  /* (pbuffer =  factor num : 1,2,3,4 */
    LOTUS_CMD_SET_EXCEPTION_CALLBACK	  = 0x2100,		  /* (aux  =  callback function ,  pbuffer =  ARG */
    LOTUS_CMD_UNKNOWN                     = 0xFFFF,
} __lotus_cmd_set_t;

typedef enum
{
    LOTUS_RECORD_MODE_NONE = 0,       //未设置，无意义
    LOTUS_RECORD_MODE_CAMCORDER,      //录像模式
    LOTUS_RECORD_MODE_SOUND_RECORD,   //录音模式
    LOTUS_RECORD_MODE_IMAGE,          //拍照模式
    LOTUS_RECORD_MODE_,
} __lotus_record_mode_t;


typedef enum
{
    LOTUS_SHOW_ALL           = 0,
    LOTUS_SHOW_HALF_UP       = 1,
    LOTUS_SHOW_HALF_DOWN     = 2,
    LOTUS_SHOW_HALF_RIGHT    = 3,
    LOTUS_SHOW_HALF_LEFT     = 4,
    LOTUS_SHOW_HALF_ERR      = 5
} __lotus_src_scn_set_t;

typedef enum __LOTUS_VID_WIN_RATIO_MODE
{
    LOTUS_VID_WIN_BESTSHOW = 0x00,        /* 以图片本身的比例缩放至满窗口显示，图片不变形 */
    LOTUS_VID_WIN_ORIGINAL,               /* 以图片原始大小在窗口内显示，不能溢出窗口     */
    LOTUS_VID_WIN_FULLSCN,                /* 以窗口的比例缩放图片至满窗口显示，可能会变形 */
    LOTUS_VID_WIN_CUTEDGE,                /* 裁边模式，在srcFrame区域再裁掉上下黑边，裁边后，以bestshow模式显示         */
    LOTUS_VID_WIN_NOTCARE,                /* 不关心图片显示比例，以当前设置的比例         */
    LOTUS_VID_WIN_ORIG_CUTEDGE_FULLSCN,		/* 以图片本身的比例缩放至满窗口显示，图片不变,图片超出部分裁剪掉     */
    LOTUS_VID_WIN_UNKNOWN
} __lotus_vid_win_ratio_mode_t;

typedef struct __LOTUS_AUDIO_INF
{
    __u32       uAbsFmt;                /* 音频码流类型，if(uAbsFmt == LOTUS_ABS_TYPE_NONE), 将
                                           忽略其它相关音频参数                             */
    __u32       uVolume;                /* 录音的音量值，如果设置非法，则采用default值,MIN_VOLUME_VAL~MAX_VOLUME_VAL      */
    __u32       uChn;                   /* 录音的声道模式，如果设置非法，则采用default值,LOTUS_AUDIO_CHN_MONO    */
    __u32       uSampWidth;             /* 录音样本点的位宽，如果设置非法，则采用default值  */
    __u32       uAudioBps;              /* 编码后的音频码流的码率，如果设置非法，则采用default值，单位:bit/s    */
    __u32       uAudioSampRate;         /* 音频码流的采样率，如果设置非法，则采用default值  */
    __u32       uAudioSource;           /* 音频源，如果设置非法，则采用default值            */
    __s32       frame_style;            /* for 0:add head,1:raw data                        */

    //额外的不需要应用程序填写的信息,这些信息由lotus内部填写，交给内部的其他模块。
    __s32       sample_cnt_per_frame;   /* 每个audio frame包含的sample的数量 */
} __lotus_audio_inf_t;

typedef enum __LOTUS_AUDIO_SRC         /* audio source type                    */
{
    LOTUS_AUDIO_SRC_NONE = 0,          /* none audio source                    */
    LOTUS_AUDIO_SRC_MIC,               /* microphone record                    */
    LOTUS_AUDIO_SRC_LINEIN,            /* line-in record                       */
    LOTUS_AUDIO_SRC_FM,                /* fm record                            */
    LOTUS_AUDIO_SRC_

} __lotus_audio_src_t;

typedef struct lotus_overlay_pic_info
{
    __u8                ID;                 //src id
    __rectsz_t          size;               //for the size of one picture
    __u8 				*y;
    __u8 				*c;
    __u8 				*alph;
} __lotus_overlay_pic_info_t;

typedef struct lotus_overlay_src_init
{
    __u8                srcPicNum;                 // the number of source picture
    __lotus_overlay_pic_info_t  srcPic[OVERLAY_MAX_SRC_NUM_PER_GROUP];
} __lotus_overlay_src_init_t;

typedef enum __LOTUS_STATUS               /* the status of ginkgo mod     */
{
    LOTUS_STAT_IDLE = 0,                  /* idle                         */
    LOTUS_STAT_STANDBY,                   /* 处于待命状态，但并未录制 ( preview 状态)    */
    LOTUS_STAT_RECORD,                    /* record，处于录制状态         */
    LOTUS_STAT_PAUSE,                     /* pause，处于录制暂停状态      */
    LOTUS_STAT_UNKNOWN
} __lotus_status_t;

typedef struct LOTUS_MOTION_DETECT_PARA
{
    __u32   complexThreshold;   // It is not used now
    __u32   ratioThreahold;     // 0(most unsensetive)-6 (most sensortive), default :LOTUS_MD_SEN_LEVEL
} __lotus_motion_dectect_para_t;

typedef struct
{
    OFFSET   pos;               //the position of the water mark to show depend on the backgroud
    __u8     total;
    __u8     IDlist[OVERLAY_MAX_SRC_NUM_PER_GROUP];    //the index of the display of the picture
} __lotus_overlay_dis_par_t;


typedef enum __LOTUS_AUDIO_CHN         /* audio channel type                   */
{
    LOTUS_AUDIO_CHN_NONE = 0,          /* none audio channel                   */
    LOTUS_AUDIO_CHN_MONO,              /* mono                                 */
    LOTUS_AUDIO_CHN_STEREO,            /* stereo                               */
    LOTUS_AUDIO_CHN_
} __lotus_audio_chn_t;


typedef enum __LOTUS_AUDIO_SAMP_WIDTH
{
    //NOTE:we only support 16bits right now!!!
    LOTUS_AUDIO_SAMP_WIDTH_0   = 0,      /* invalid audio sample width         */
    LOTUS_AUDIO_SAMP_WIDTH_8   = 8,      /* 8 bits audio sample                */
    LOTUS_AUDIO_SAMP_WIDTH_16  = 16,     /* 16 bits audio sample               */
    LOTUS_AUDIO_SAMP_WIDTH_24  = 24,     /* 24 bits audio sample               */
    LOTUS_AUDIO_SAMP_WIDTH_32  = 32,     /* 32 bits audio sample               */
} __lotus_audio_samp_width_t;

#define ENCODER_CFG_ENC_FRAME 0X0
#define ENCODER_CFG_ENC_FIELD 0X1

#define ENCODER_CFG_ENC_MODE_MASK 0X1

typedef enum
{
    LOTUS_VIDEO_SOURCE_CSI_0 = 0,
    LOTUS_VIDEO_SOURCE_CSI_1 ,
    LOTUS_VIDEO_SOURCE_CSI_2 ,
    LOTUS_VIDEO_SOURCE_CSI_3 ,
    LOTUS_VIDEO_SOURCE_TVD_0 ,
    LOTUS_VIDEO_SOURCE_TVD_1 ,
    LOTUS_VIDEO_SOURCE_TVD_2 ,
    LOTUS_VIDEO_SOURCE_TVD_3 ,
    LOTUS_VIDEO_SOURCE_USB_0 ,
    LOTUS_VIDEO_SOURCE_USB_1 ,
    LOTUS_VIDEO_SOURCE_USB_2 ,
    LOTUS_VIDEO_SOURCE_USB_3 ,

} __lotus_video_source_t;


typedef struct __LOTUS_VIDEO_INF
{
    __u32       uVbsFmt;                /* 视频流类型, if(uVbsFmt == SW_CODEC_TYPE_NONE), 将忽略其它相关的视频参数, __herb_codec_t */
    __s32       srcWidth;               /* 外部原视频图像的宽度            A10为720p插值到1080p*/
    __s32       srcHeight;              /* 外部原的视频图像的高度       A10为720p插值到1080p*/
    __s32       nWidth;                 /* 要编码的视频图像的宽度 */
    __s32       nHeight;                /* 要编码的视频图像的高度 */
    __u32       uVideoFrmRate;          /* 视频码流的帧率,如果设置非法，则采用default值, 数值放大1000倍     */
    __s32       nVideoBps;              /* 视频码流的码率,如果设置非法，则采用default值，单位:bit/s     */

    __s32       color_format;           /* yuv, bayerRGB(4种), PIXEL_YUV420*/
    __s32       color_space;            /* BT601, bt709*/

    __s16       qp_max;
    __s16       qp_min;
    __s16       maxKeyInterval;         /*keyframe Interval，指2个关键帧之间的P帧的最大数量             */
    __s16       encoderCfg;             /* encoder 驱动编码配置*/
    __s16		jpgenc_quality;			// jpeg encoder quality setting 	 good:92 mid:85 low 75
    __u32		videoTimes;             /*设置视频录制时间解决文件碎片问题. 0:不设置时间; 非0:设置多少秒为录制时间片段*/
    __lotus_video_source_t    uVideoSource;         	/* 视频源，如果设置非法，则采用default值*/
} __lotus_video_inf_t;



//the information of the media file which will be input to play
typedef struct __LOTUS_MEDIA_FILE_INF
{
    //要录制的文件格式
    __u32       uFileFmt;                   /* 要录制的文件格式,__herb_media_file_fmt_t                         */
    char        file_path[LOTUS_FPATH_SIZE]; /* 录制的媒体文件保存的绝对路径名，如果未设置，则保存到default路径  */

    //音频相关信息
    __lotus_audio_inf_t    audio_inf;        /* 音频录制格式信息                                         */

    //视频相关信息
    __lotus_video_inf_t    video_inf;        /* 视频录制格式信息,参考__video_encode_format_t             */

    //标题，时间等
    char        title[LOTUS_FILE_TITLE_SIZE];
    __s32       create_time;                /* 数字的规格定义，由mux决定，应用程序遵守                  */
} __lotus_media_file_inf;

typedef struct __LOTUS_CALLBACK_INFO
{
    __pCBK_t    GetCurExtData;      /* __s32 GetCurExtData(__lotus_sync_cb_para_t *psyncPara),返回值为当前的data长度*/
    __pCBK_t    DealWithExtData;    /* void GetCurExtData(__lotus_sync_cb_para_t *psyncPara) */
    __u32       ExtDatalenPerFrame; /* 单帧ext data数据最大长度 */
    __u32       ExtDatalenTotal;    /* 整个文件ext data数据总长度最大值 */
    void       *pcontex;
} __lotus_sync_callback_t;

typedef struct __LOTUS_SYNC_CB_PARA
{
    void   *pbuffer;
    __s32   bufLen;
    void   *pcontex;
} __lotus_sync_cb_para_t;

typedef enum
{
    LOTUS_MUXER_TYPE_FILE = 0,            //输出到文件
    LOTUS_MUXER_TYPE_NULL = 1,                //muxer无输出
    LOTUS_MUXER_TYPE_ERR  = 2,                //muxer无输出
} __lotus_muxer_type_t;

typedef enum
{
    LOTUS_SOURCE_ROTATE_0 = 0,
    LOTUS_SOURCE_ROTATE_90 = 1,
    LOTUS_SOURCE_ROTATE_180 = 2,
    LOTUS_SOURCE_ROTATE_270 = 3,
    LOTUS_SOURCE_FLIP_HOR = 4,
    LOTUS_SOURCE_FLIP_VER = 5,
} __lotus_source_rotate_t;

typedef enum
{
    LOTUS_PREVIEW_ROTATE_0 = 0,
    LOTUS_PREVIEW_ROTATE_90 = 1,
    LOTUS_PREVIEW_ROTATE_180 = 2,
    LOTUS_PREVIEW_ROTATE_270 = 3,
    LOTUS_PREVIEW_FLIP_HOR = 4,
    LOTUS_PREVIEW_FLIP_VER = 5,
} __lotus_preview_rotate_t;

#endif


