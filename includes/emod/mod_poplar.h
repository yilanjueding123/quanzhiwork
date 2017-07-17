/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                               mod_mmp sub-system
*
*                                      (c) Copyright 2009-2010, kevin.z China
*                                               All Rights Reserved
*
* File   : mod_poplar.h
* Version: V1.0
* By     : kevin.z
* Date   : 2009-6-24 16:30
*********************************************************************************************************
*/
#ifndef _MOD_POPLAR_H_
#define _MOD_POPLAR_H_

#include "typedef.h"
#include "cfgs.h"


typedef enum __POPLAR_SUBTITLE_ENCODE
{
    POPLAR_CHAR_ENCODE_GB2312,    // subtitle encode type is GB2312
    POPLAR_CHAR_ENCODE_GB18030,
    POPLAR_CHAR_ENCODE_GB13000,
    POPLAR_CHAR_ENCODE_UTF8,      // subtitle encode type is UTF8

    POPLAR_CHAR_ENCODE_

} __poplar_subtitle_encode_t;

typedef enum __POPLAR_CMD_SET           /* CMMB中间件命令列表(IOCtrol)                                  */
{
    POPLAR_CMD_GET_MESSAGE_CHN = 0x100, /* 获取返馈消息通道，return = __krnl_event_t*, (__poplar_feedback_msg_t)    */
    POPLAR_CMD_GET_ERROR_TYPE,          /* 获取媒体播放器的错误类型, error in __poplar_feedback_msg_t   */
    POPLAR_CMD_SET_FREQ_INFO,         /* 设置数字广播的频点, pbuffer = __freq_info_t,
                                        buffer由调用者malloc, return = 频点编号, -1代表异常,
                                        频点编号参考CMMB第七部分, 表B.1, Page 11,
                                        如果是其他频点，系统自行分配一个编号返回。*/

    POPLAR_CMD_FREQ_SCAN,            /* 频段搜索,获取该频段的节目列表, aux = 频道编号,  return = 是否成功*/
    POPLAR_CMD_NETWORK_SCAN,            /* 网络搜索，根据当前频段的网络信息表，搜索网络包含的所有频段*/
    POPLAR_CMD_AUTO_SCAN,               /* 盲扫所有预置频点*/
    POPLAR_CMD_STOP_SCAN,               /* 中止搜索，channel scan之间可以中止，chanel scan 本身不能中止*/
    POPLAR_CMD_CHECK_SCAN_PROGRESS,     /* 检测scan进度，0:0%, 100:100%, -1:搜索失败*/
    POPLAR_CMD_GET_FREQ_LIST,           /* 获取频点信息表, pbuffer = (__freq_list_t**),  ret = EPDK_OK*/

    POPLAR_CMD_GET_PROG_LIST,           /* 获取所有的节目表, pbuffer =  *__service_list_t，APP传buffer        */
    POPLAR_CMD_GET_FREQ_PROG_LIST,      /* 获取指定频点的已存储的节目表, aux = 频点编号, pbuffer =  *__service_list_t */
    POPLAR_CMD_SET_PROG_ITEM,           /* 设置要播放的数字广播的节目参数                               */

    POPLAR_CMD_PLAY = 0x0200,           /* 启动播放指定的数字广播节目, aux= ItemID(service_ent_t)*/
    POPLAR_CMD_STOP,                    /* 停止播放数字广播节目                                         */
    POPLAR_CMD_GET_STATUS,              /* 获取数字广播中间件当前的状态                                 */
    POPLAR_CMD_GET_MJD_UTC,             /* 获取当前的世界时间                                           */

    POPLAR_CMD_GET_ABSTYPE = 0x0400,    /* 获取音频码流格式 return = __poplar_audio_fmt_t               */
    POPLAR_CMD_GET_ABS_CNT,             /* 获取当前数字广播节目的音频流个数                             */
    POPLAR_CMD_SET_ABS_NO,              /* 指定当前要播放的数字广播的音频流，default = NO.0             */
    POPLAR_CMD_GET_ABPS,                /* 获取音频流的码率 return = bps                                */
    POPLAR_CMD_GET_SAMPRATE,            /* 获取音频采样率   return = sample rate                        */
    POPLAR_CMD_SET_CHN,                 /* 设置声道类型     aux = __poplar_audio_chn_t                   */
    POPLAR_CMD_GET_CHN,                 /* 获取音频的通道数                                             */
    //POPLAR_CMD_SEL_AUD_TRACK,           /* 更换音轨*/

    POPLAR_CMD_GET_VBSTYPE = 0x0500,    /* 获取视频码流格式                                             */
    POPLAR_CMD_GET_VBS_CNT,             /* 获取当前数字广播节目的视频流个数                             */
    POPLAR_CMD_SET_VBS_NO,              /* 指定当前要播放的数字广播的视频流，default = NO.0             */
    POPLAR_CMD_GET_VBPS,                /* 获取视频流的码率                                             */
    POPLAR_CMD_GET_VFPS,                /* 获取图像帧率     return = frame rate                         */
    POPLAR_CMD_GET_FRAMESIZE,           /* frame size       return = (width << 16) + height             */
    POPLAR_CMD_SET_VIDEO_LAYERHDL,      /* 设置视频图层句柄 pbuffer = video_layer_hdl                   */
    POPLAR_CMD_SET_VIDEO_WINDOW,        /* 设置视频窗口属性 pbuffer = (__rect_t *)                      */
    POPLAR_CMD_GET_VIDEO_WINDOW,        /* 获取视频窗口属性 return = (__rect_t *)                       */
    POPLAR_CMD_SET_DISP_RATIO_MODE,     /* 设置视频图象显示的模式，aux = __poplar_vide_window_ratio_mode_t,
                                           return = 设置后的当前显示模式                                    */
    POPLAR_CMD_GET_DISP_RATIO_MODE,     /* 获取视频图象显示模式，return = __poplar_vide_window_ratio_mode_t */
    POPLAR_CMD_SWITCH_VIDEO_SHOW,       /* 打开或关闭图片显示，aux = 0:关闭图片显示，aux = 1:打开图片显示   */

    POPLAR_CMD_UPDATE_ESG,              /* 更新当前频点的ESG信息*/
    POPLAR_CMD_GET_SERV_ESG,            /* 获取一个业务的ESG信息, aux = servId, pbuffer = esg_serv_sched_list_t**, ret = EPDK_OK */
    POPLAR_CMD_RELEASE_ESG,             /* 释放ESG信息，退出节目预告菜单时调用，释放内存*/

    POPLAR_CMD_

} __poplar_cmd_set_t;


typedef enum __POPLAR_FEEDBACK_MSG              /* 定义媒体播放中间件反馈给应用程序的消息类型               */
{
    POPLAR_ERROR_NOP                 =  0,      /* 空消息                                                   */

    POPLAR_ERROR_UNKOWN              = -1,      /* 未知错误                                                 */

    POPLAR_ERROR_START_PLG_FAIL      = -40,     /* 启动插件失败                                             */
    POPLAR_ERROR_ADEC_ERROR,                    /* 音频解码插件解码错误                                     */
    POPLAR_ERROR_VDEC_ERROR,                    /* 视频解码插件解码错误                                     */
    POPLAR_ERROR_REQ_MEMORY_FAIL,               /* 申请内存失败                                             */

    POPLAR_FEDBAK_NO_ERROR           =  1,      /* 空消息                                                   */

    POPLAR_FEDBAK_

} __poplar_feedback_msg_t;


typedef enum __POPLAR_VID_WINDOW_RATIO_MODE
{
    POPLAR_VIDEO_WINDOW_NOTCARE = 0x00, /* 不关心图片显示比例，以当前设置的比例         */
    POPLAR_VIDEO_WINDOW_ORIGINAL,       /* 以图片原始大小在窗口内显示，不能溢出窗口     */
    POPLAR_VIDEO_WINDOW_BESTSHOW,       /* 以图片本身的比例缩放至满窗口显示，图片不变形 */
    POPLAR_VIDEO_WINDOW_FULLSCN,        /* 以窗口的比例缩放图片至满窗口显示，可能会变形 */
    POPLAR_VIDEO_WINDOW_

} __poplar_vide_window_ratio_mode_t;

#define POPLAR_TMP_STATUS_FLAG 0x80     //表示状态转换的中间状态
typedef enum __POPLAR_STATUS            /* the status os poplar                 */
{
    POPLAR_STAT_PLAY = 0,               /* play                                 */
    POPLAR_STAT_IDLE,                   /* stop                                 */
    POPLAR_STAT_PAUSE,                  /* pause，暂时不支持*/
    POPLAR_STAT_SCAN,                   /* 搜索业务的状态*/
    POPLAR_STAT_
} __poplar_status_t;

typedef enum __POPLAR_AUDIO_FMT
{
    POPLAR_ABS_TYPE_NONE = 0,           /* 没有音频流                           */
    POPLAR_ABS_TYPE_UNKNOWN,            /* 有音频流,但无法识别                  */

    POPLAR_ABS_TYPE_AAC_LC,

    POPLAR__ABS_TYPE_

} __poplar_audio_fmt_t;


typedef enum __POPLAR_VIDEO_FMT
{
    POPLAR_VBS_TYPE_NONE = 0,           /* 没有视频流                           */
    POPLAR_VBS_TYPE_UNKNOWN,            /* 有视频流,但无法识别                  */

    POPLAR_VBS_TYPE_H264,

    POPLAR_VBS_TYPE_

} __poplar_video_fmt_t;


typedef enum __POPLAR_AUDIO_CHANNEL
{
    POPLAR_AUDIO_CHN_STEREO = 0,        /* 立体声模式                           */
    POPLAR_AUDIO_CHN_LEFT,              /* 左声道模式                           */
    POPLAR_AUDIO_CHN_RIGHT,             /* 右声道模式                           */
    POPLAR_AUDIO_CHN_

} __poplar_audio_channel_t;


//the setting information for poplar sub-system
typedef struct __POPLAR_SETTING_INF
{
    //音频播放的相关设置
    __s8        audio_index;            /* 音频流编号*/
    __s8        audio_chn_flag;         /* audio_chn参数是否有效的标志          */

    __u32       audio_chn;              /* 音频声道模式                         */
    //视频播放的相关设置
    __s8        video_index;            /* 视频流编号*/
    __s8        video_layer_hdl_flag;   /* video_layer_hdl参数是否有效的标志    */
    __s8        video_window_flag;      /* video_window参数是否有效的标志       */
    __s8        video_ratio_mode_flag;  /* video_show_mode参数是否有效的标志    */

    __s32       video_layer_hdl;        /* 视频物理图层句柄                     */
    __rect_t    video_window;           /* 视频图像窗口属性                     */
    __s32       video_ratio_mode;       /* 视频图像显示比例模式,__cedar_vide_window_ratio_mode_t*/
} __poplar_setting_inf_t;


#define MAX_SERVICE_NAME_LEN            (64)

typedef enum BROADCAST_TYPE
{
    BC_RESERVE = 0,
    BC_TV,
    BC_RADIO,
    BC_SHORT_PROGRAM,
    BC_DOWNLOAD,
    BC_SOFTWARE_MANAGE,
    BC_ESG,
    BC_DATA_STREAM,

    BC_TYPE_NUM
} BC_TYPE;

typedef struct SERVICE_ENTRY
{
    __s16   ItemID;                               //* 本item的唯一标识号，内部编号
    __u16   servID;                               //* 业务标识号，从流中解析得到
    __u8    freqID;                               //* 业务所在频段的频点编号
    __u8    ts_start;                             //* 复用帧的起始时隙号
    __u8    ts_num;                               //* 复用帧占用的时隙个数
    __u8    chanParam;                            //* 信道配置参数，IF202使用
    __u8    MF_ID;                                //* 业务所在复用帧的标识号号
    __u8    subFrmID;                             //* 业务所在复用子帧的标识号
    char    ServiceName[MAX_SERVICE_NAME_LEN];    //* 业务名称，如"CCTV-1", "CCTV-2"等
    __u8    valid;
    BC_TYPE bcType;                               //* 广播类型

} service_ent_t;

typedef struct SERVICE_LIST
{
    __s32           num;
    __poplar_subtitle_encode_t charType;
    service_ent_t  *pServEnt;

} __service_list_t;


typedef struct FREQUENCY_ENTRY
{
    __u32   freqId;             //* 频点编号
    __u32   freq;               //* 频点的频率值
    __s8    city[16];           //* 城市名
} freq_ent_t;


typedef struct FREQUENCY_LIST
{
    __s32       num;
    freq_ent_t *pFreqEnt;

} __freq_list_t;


typedef struct ESG_EVENT
{
    __u16 servId;
    __u8  lang[8];
    __u8  title[64];
    __u8  date[16];
    __u8  time[16];
    __u8  free;
    __u8  isLive;
    __u8  isRepeate;

} esg_event_t;

typedef struct ESG_SERV_SCHEDULE
{
    __u32        servId;
    __u32        eventNum;
    esg_event_t *events;
    __u32        charType;
} esg_serv_sched_t;


typedef struct ESG_SCHEDULE
{
    __u32               eventNum;
    esg_event_t        *events;

    __u32               numServSched;
    esg_serv_sched_t   *servSched;

    __u32        charType;

} esg_serv_sched_list_t;

typedef enum
{
    POPLAR_ERR_NONE         = EPDK_OK,        //no error
    POPLAR_ERR_FAIL         = EPDK_FAIL,       //common fail, not care reason.
    POPLAR_ERR_PARA_ERR     = -2,       //some parameter error
    POPLAR_ERR_HARDWARE_ERR = -3,       //硬件故障
    POPLAR_ERR_REQMEM_FAIL  = -7,       //request memory failed
    POPLAR_ERR_POST_FAIL    = -8,
    POPLAR_ERR_EXCEPTION    = -9,
    POPLAR_ERR_IGNORE       = 1,        //命令被忽略，直接返回。例如状态处于中间态，不接收命令。
    POPLAR_ERR_STOPSCAN     = 2,        //中止scan
    //    FILE_PARSER_READ_END        = 1,        //read file or idx table end.以读不到数据作为标识
    //    FILE_PARSER_BUF_LACK        = 2,        //BUFFER is not enough to contain data.
    //    FILE_PARSER_GET_NONE        = 3,        //read file success for (>0) bytes, but get nothing.
    //    FILE_PARSER_ERROR_IGNORE    = 4,        //some exception happens, but can continue process.
    //    FILE_PARSER_PAUSE_GET_DATA  = 5,        //pause get data from file, when in FF,RR mode,
    //
    //    FILE_PARSER_PARA_ERR        = -1,       //some parameter error
    //    FILE_PARSER_OPEN_FILE_FAIL  = -2,       //open media file failed
    //    FILE_PARSER_READ_FILE_FAIL  = -3,       //read media file data failed
    //    FILE_PARSER_FILE_FMT_ERR    = -4,       //media file format error
    //    FILE_PARSER_NO_AV           = -5,       //neither audio nor video supported in media file
    //    FILE_PARSER_END_OF_MOVI     = -6,       //movi data end
    //
    //    FILE_PARSER_EXCEPTION       = -8,       //some impossible case happens.
    POPLAR_ERR_

} __poplar_return_val_t;

#endif  /* _MOD_POPLAR_H_ */


