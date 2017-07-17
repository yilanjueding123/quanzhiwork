/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                               mod_mmp sub-system
*
*                                (c) Copyright 2010-2011, Xiaoyan.Liu China
*                                               All Rights Reserved
*
* File   : mod_dvdnavi.h
* Version: V1.0
* By     : Xiaoyan. Liu
* Date   : Xiaoyan.Liu  1010_06_01
*********************************************************************************************************
*/
#ifndef _MOD_DVDNAVI_H_
#define _MOD_DVDNAVI_H_

#include "typedef.h"
#include "cfgs.h"

#define YULAN_TAG_INF_SIZE      (256)      /* buffer size for audio tag information                    */
#define YULAN_FPATH_SIZE        (1024)    /* 媒体文件路径名长度最大值                                 */

//define variable for audio render interface
#define MAX_SPEC_GROUP_NUM      (10)    /* 频谱输出的最大组数，应用程序参考该值申请空间                 */
#define USER_DEFINE_EQ_SIZE     (10)    /* 用户自定义EQ时，需要传输的一组参数长度                       */

#define AUDIO_VPS_SLOWEST       (-4)    /* 音频变速播放的最慢速度值                                     */
#define AUDIO_VPS_NOMAL         (0)     /* 音频正常播放速度值                                           */
#define AUDIO_VPS_FASTEST       (10)    /* 音频变速播放的最快速度值                                     */


#define MIN_VOLUME_VAL          (0)     /* 音量最小值，此时为静音状态                                   */
#define MAX_VOLUME_VAL          (30)    /* 音量最大值，只有在音量范围内的值被认为有效，否则为非法值     */

#define YULAN_FASTEST_FRSPEED   (128)   /* 快进、快退的最大倍速                                         */
#define YULAN_SLOWEST_FRSPEED   (1)     /* 快进、快退的最小倍速                                         */


#define YULAN_FRPIC_HOLDTIME_LEAST      (30)    /* 快进、快退状态下，图片显示停留最小时间，(30ms)       */
#define YULAN_FRPIC_HOLDTIME_LARGEST    (5000)  /* 快进、快退状态下，图片显示停留最大时间，(5000ms)     */

#define YULAN_MAX_LANG_CHAR_SIZE             (32)
#define YULAN_MAX_AUDIO_STREAM_NUM           (8)
#define YULAN_MAX_SUBTITLE_STREAM_NUM        (32)
#define YULAN_MAX_MENU_BTN_NUM               (36)
#define YULAN_MAX_PALETTE_CNT                 16
#define YULAN_MAX_CMD_LEN                     8

typedef enum __YULAN_CMD_SET            /* 媒体播放中间件命令列表(IOCtrol)                                         */
{
    YULAN_CMD_SET_MEDIAFILE = 0x0100,   /* 设置待播放的媒体文件路径及断点信息, pbuffer = (__yulan_media_file_inf *) */
    YULAN_CMD_GET_MESSAGE_CHN,          /* 获取返馈消息通道，return = __krnl_event_t*, (__yulan_feedback_msg_t)     */
    YULAN_CMD_GET_ERROR_TYPE,           /* 获取媒体播放器的错误类型, error in __yulan_feedback_msg_t                */

    YULAN_CMD_PLAY = 0x0200,            /* 播放命令                                                                 */
    YULAN_CMD_STOP,                     /* 停止播放                                                                 */
    YULAN_CMD_PAUSE,                    /* 暂停播放                                                                 */
    YULAN_CMD_FF,                       /* 快进                                                                     */
    YULAN_CMD_REV,                      /* 快退                                                                     */
    YULAN_CMD_GET_STATUS,               /* 获取播放模式     return = __yulan_status_t                               */

    YULAN_CMD_GET_TOTAL_TIME = 0x300,   /* 获取播放总时间   return = total time(ms),若返回值为-1，则不显示总播放时间*/
    YULAN_CMD_GET_CUR_TIME,             /* 获取当前播放时间 return = current play time(ms)                           */
    YULAN_CMD_GET_TAG,                  /* 获取当前播放时间点的标签信息                                              */
    YULAN_CMD_SET_FRSPEED,              /* 设置快进快退速度 aux    = YULAN_SLOWEST_FRSPEED ~ YULAN_FASTEST_FRSPEED  */
    YULAN_CMD_GET_FRSPEED,              /* 获取快进快退速度 return = speed                                           */

    YULAN_CMD_GET_ABSTYPE = 0x0400,     /* 获取当前正在播放的音频码流格式 return = __yulan_audio_fmt_t               */
    YULAN_CMD_GET_AUDBPS,               /* 获取音频流的码率 return = bps                                             */
    YULAN_CMD_GET_SAMPRATE,             /* 获取音频采样率   return = sample rate                                     */
    YULAN_CMD_SET_CHN,                  /* 设置声道类型     aux = __yulan_audio_chn_t                                */
    YULAN_CMD_GET_CHN,                  /* 获取音频的通道数                                                          */
    YULAN_CMD_SET_VOL,                  /* 设置音量         aux = MIN_VOLUME_VAL~MAX_VOLUME_VAL, return = new vol    */
    YULAN_CMD_GET_VOL,                  /* 获取音量         return = volume                                          */
    YULAN_CMD_VOLUP,                    /* 音量值上调一档                                                            */
    YULAN_CMD_VOLDOWN,                  /* 音量值下调一档，                                                          */
    YULAN_CMD_SET_EQ,                   /* 设置EQ           aux = __yulan_audio_eq_t                                 */
    YULAN_CMD_GET_EQ,                   /* 获取EQ           return = __yulan_audio_eq_t                              */
    //YULAN_CMD_SET_VPS,                  /* 设置变速播放速度 aux    = -4~10，=0-normal; <0-slow; >0-fast              */
    //YULAN_CMD_GET_VPS,                  /* 获取变速播放速度 return = -4~10                                           */
    //YULAN_CMD_SET_AB_A,                 /* 设置A-B播放的起始点(A点)                                     */
    //YULAN_CMD_SET_AB_B,                 /* 设置A-B播放的结束点(B点)                                     */
    //YULAN_CMD_SET_AB_LOOPCNT,           /* 设置A-B播放的循环次数                                        */
    //YULAN_CMD_CLEAR_AB,                 /* 取消A-B播放                                                  */
    YULAN_CMD_SEL_AUDSTREAM,            /* 选择音频流. aux = 数组下标, ret = EPDK_OK/EPDK_FAIL，表示是否将命令发送给YUAN.并不表示YULAN处理成功 */
    YULAN_CMD_GET_AUDSTREAM,            /* 得到当前正在播放的音频流的下标号, ret = suffix or -1(if not find)          */
    YULAN_CMD_GET_AUDSTREAM_PROFILE,    /* 获取所有音频流的说明信息:
                                            pbuffer = __audstream_profile_t, app malloc it.
                                            return = EPDK_OK or EPDK_FAIL*/
    YULAN_CMD_GET_VBSTYPE = 0x0500,     /* 获取视频码流格式                                                           */
    YULAN_CMD_GET_VIDBITRATE,           /* 获取视频流的码率                                                           */
    YULAN_CMD_GET_VIDFPS,               /* 获取图像帧率     return = frame rate                                       */
    YULAN_CMD_GET_FRAMESIZE,            /* frame size       return = (width << 16) + height                           */
    YULAN_CMD_SET_VID_LAYERHDL,         /* 设置视频图层句柄 pbuffer = video_layer_hdl                                 */
    YULAN_CMD_SET_VID_WINDOW,           /* 设置视频窗口属性 pbuffer = (__rect_t *)                                    */
    YULAN_CMD_GET_VID_WINDOW,           /* 获取视频窗口属性 return = (__rect_t *)                                     */
    YULAN_CMD_SET_VID_SHOW_MODE,        /* 设置视频图象显示的模式，aux = __yulan_vide_window_ratio_mode_t,
                                           return = 设置后的当前显示模式                                              */
    YULAN_CMD_GET_VID_SHOW_MODE,        /* 获取视频图象显示模式，return = __yulan_vide_window_ratio_mode_t            */
    YULAN_CMD_SWITCH_VID_SHOW,          /* 打开或关闭图片显示，aux = 0:关闭图片显示，aux = 1:打开图片显示             */
    YULAN_CMD_SET_FRPIC_SHOWTIME,       /* 设置快进快退状态下，视频帧显示的持续时间                                   */
    YULAN_CMD_GET_FRPIC_SHOWTIME,       /* 获取快进快退状态下，图频帧显示的持续时间                                   */

    YULAN_CMD_GET_LBSTYPE = 0x0600,     /* 获取字幕码流格式                                                           */
    YULAN_CMD_GET_SUB_INFO,             /* 获取字幕的条目信息, aux = time, pbuffer = __yulan_get_sub_inf_t,
                                           (return = __yulan_subtitle_item_t *)                                       */
    YULAN_CMD_GET_SUBTITLE_PROFILE,              /* 获取所有字幕的说明信息:例如，所在的stream_idx,语言等
                                            pbuffer = __subtitle_profile_t, app malloc it
                                            return = EPDK_OK or EPDK_FAIL*/
    YULAN_CMD_SELECT_SUBTITLE,          /* 选择一个字幕播放，用于切换字幕, aux:字幕下标in __subtitle_profile_t        */
    YULAN_CMD_GET_SUBTITLE,             /* 得到当前正在播放字幕的下标，if not find, return -1                         */

    YULAN_CMD_CAPTURE_PIC = 0x0700,     /* 截图, pbuffer = __yulan_pic_info_t     */
    YULAN_CMD_ASK_PIC_BUFSIZE,          /* 向YULAN查询需要多少buffer以便APP分配，pbuffer = __yulan_pic_info_t, ret = size, -1代表失败*/
    YULAN_CMD_BACK_LAST_MENU   = 0x0800,    /*返回上一层菜单 */
    YULAN_CMD_BACK_MAIN_MENU,               /*返回主菜单     */
    YULAN_CMD_CHECK_BUTTON,            /*检测button 是否存在，return EPDK_OK/EPDK_FAIL，若为EPDK_OK,表示成功检测到button*/
    YULAN_CMD_GET_BUTTON,              /*获得btn的信息buffer = (__YULAN_menuBtn_info_t *)*/
    YULAN_CMD_SEL_UPPER_BUTTON,        /*将用户对按钮的操作传到中间件同时获得新的按钮的位置信息 buffer = (__yulan_menuBtn_pos_t *)*/
    YULAN_CMD_SEL_LOWER_BUTTON,        /*将用户对按钮的操作传到中间件同时获得新的按钮的位置信息 buffer = (__yulan_menuBtn_pos_t *)*/
    YULAN_CMD_SEL_LEFT_BUTTON,         /*将用户对按钮的操作传到中间件同时获得新的按钮的位置信息 buffer = (__yulan_menuBtn_pos_t *)*/
    YULAN_CMD_SEL_RIGHT_BUTTON,        /*将用户对按钮的操作传到中间件同时获得新的按钮的位置信息 buffer = (__yulan_menuBtn_pos_t *)*/
    YULAN_CMD_PRESS_BUTTON,            /*将用户对按钮的操作传到中间件                                                             */
    YULAN_CMD_GOTO_CHAPTR,             /*跳转到某个chapter播放， aux=用户输入的章节号，(>=1)                            */
    YULAN_CMD_GOTO_TIME,               /*跳转到某个时刻进行播放,aux = 用户输入的时间，单位s                             */
    YULAN_CMD_GOTO_TITLE,              /*跳转到某个title进行播放，aux=用户输入的title数，>=1                            */
    YULAN_CMD_GET_CUR_CHAPTER,         /*获取当前正在播放的chapter数                                                    */
    YULAN_CMD_GET_CUR_TITLE,           /*获取当前正在播放的title数                                                      */
    YULAN_CMD_GET_TOTAL_CHAPTER,       /*获取当前title包含的chapter数                                                   */
    YULAN_CMD_GET_TOTAL_TITLE,         /*获取当前碟片包含的title数                                                      */
    YULAN_CMD_GET_PIC_SHOW_INFO,       /*获得当前图像的显示信息                                                         */
    YULAN_CMD_SET_PLAY_MODE,           /*设置播放模式:重复播放当前title，重复播放chapter 或关闭重复播放                 */
    YULAN_CMD_GET_PLAY_MODE,
    YULAN_CMD_GET_CUR_ANGLE,           /*获取当前正在播放的angle数                                                      */
    YULAN_CMD_GET_TOTAL_ANGLE,         /*获取当前title包含的angle数                                                     */
    YULAN_CMD_

} __yulan_cmd_set_t;


typedef enum __YULAN_FEEDBACK_MSG           /* 定义媒体播放中间件反馈给应用程序的消息类型               */
{
    YULAN_ERROR_NOP                 =   0,  /* 空消息                                                   */

    //YULAN feed back error type
    YULAN_ERROR_OPEN_FILE_FAIL      = -20,  /* 打开当前媒体文件失败                                     */
    YULAN_ERROR_FILE_FMT_UNKNOWN,           /* 无法识别当前媒体文件格式                                 */
    YULAN_ERROR_FILE_NOT_SUPPORT,           /* 当前媒体文件不支持                                       */
    YULAN_ERROR_FILE_DATA_ERROR,            /* 媒体文件数据错，无法解析                                 */
    YULAN_ERROR_NO_FILE_INPUT,              /* 未设置要播放的媒体文件                                   */
    YULAN_ERROR_READ_FILE_FAIL,             /* 读取文件数据失败                                         */

    YULAN_ERROR_START_PLG_FAIL      = -40,  /* 启动插件失败                                             */
    YULAN_ERROR_ADEC_ERROR,                 /* 音频解码插件解码错误                                     */
    YULAN_ERROR_VDEC_ERROR,                 /* 视频解码插件解码错误                                     */
    YULAN_ERROR_TDEC_ERROR,                 /* 字幕解码插件解码错误                                     */

    YULAN_ERROR_PIC_SIZE_TOO_LARGE  = -60,  /* 图片尺寸太大，无法支持解码                               */
    YULAN_ERROR_REQ_MEMORY_FAIL,            /* 申请内存失败                                             */

    YULAN_ERROR_UNKOWN              = -80,  /* 未知错误                                                 */

    //YULAN feed back status type
    YULAN_FEDBAK_NO_ERROR           =  1,   /* 空消息                                                   */
    YULAN_FEDBAK_PLAY_END,                  /* 播放媒体文件结束                                         */

    YULAN_FEDBAK_

} __yulan_feedback_msg_t;


typedef enum __YULAN_VID_WINDOW_RATIO_MODE
{
    YULAN_VID_WINDOW_NOTCARE = 0x00,        /* 不关心图片显示比例，以当前设置的比例         */
    YULAN_VID_WINDOW_ORIGINAL,              /* 以图片原始大小在窗口内显示，不能溢出窗口     */
    YULAN_VID_WINDOW_BESTSHOW,              /* 以图片本身的比例缩放至满窗口显示，图片不变形 */
    YULAN_VID_WINDOW_FULLSCN,               /* 以窗口的比例缩放图片至满窗口显示，可能会变形 */
    YULAN_VID_WINDOW_4R3MODE,               /* 强制以4:3的模式满屏显示视频图像，图像会变形  */
    YULAN_VID_WINDOW_16R9MODE,              /* 强制以16:9的模式满屏显示视频图像，图像会变形 */
    YULAN_VID_WINDOW_CUTEDGE,               /* 图片本身有黑边，需裁掉                       */

    YULAN_VID_WINDOW_

} __yulan_vide_window_ratio_mode_t;

typedef enum __YULAN_STATUS             /* the status os YULAN
                  */
{
    YULAN_STAT_PLAY = 0,                /* play                                 */
    YULAN_STAT_PAUSE,                   /* pause                                */
    YULAN_STAT_STOP,                    /* stop                                 */
    YULAN_STAT_FF,                      /* fast forward                         */
    YULAN_STAT_RR,                      /* fast backward                        */
    YULAN_STAT_PLAYEND,                /* play end,应用层需要重新检测total time，字幕等的信息*/
    YULAN_STAT_

} __yulan_status_t;

typedef enum __YULAN_SELECT_BUTTON       /*选择按钮                             */
{
    YULAN_SEL_UPPER_BTN = 0,            /* select the upper button              */
    YULAN_SEL_LOWER_BTN,                /* select the lower button              */
    YULAN_SEL_LEFT_BTN,                 /* select the left button               */
    YULAN_SEL_RIGHT_BTN,                /* select the right button              */
    YULAN_SEL_PRESS_BTN,                /* press the button                     */
    YULAN_SEL_BUTTON_
} __yulan_sel_btn_t;

typedef enum __YULAN_AUDIO_FMT
{
    YULAN_ABS_TYPE_NONE = 0,            /* 没有音频流                           */
    YULAN_ABS_TYPE_UNKNOWN,             /* 有音频流,但无法识别                  */

    YULAN_ABS_TYPE_MPEG12_LAYER1,
    YULAN_ABS_TYPE_MPEG12_LAYER2,
    YULAN_ABS_TYPE_MPEG12_LAYER3,
    YULAN_ABS_TYPE_MPEG_AAC_LC,
    YULAN_ABS_TYPE_AC3,                 // AC-3
    YULAN_ABS_TYPE_DTS,                 // DTS audio for DVD-Video
    YULAN_ABS_TYPE_LPCM_V,
    YULAN_ABS_TYPE_LPCM_A,
    YULAN_ABS_TYPE_ADPCM,
    YULAN_ABS_TYPE_PCM,                 // pcm
    YULAN_ABS_TYPE_

} __yulan_audio_fmt_t;


typedef enum __YULAN_VIDEO_FMT
{
    YULAN_VBS_TYPE_NONE = 0,            /* 没有视频流                           */
    YULAN_VBS_TYPE_UNKNOWN,              /* 有视频流,但无法识别                  */

    YULAN_VBS_TYPE_MPEG1_ES,
    YULAN_VBS_TYPE_MPEG2_ES,
    YULAN_VBS_TYPE_

} __yulan_video_fmt_t;

typedef enum __YULAN_LYRIC_FMT
{
    YULAN_LRC_TYPE_NONE = 0,            /* 没有字幕/歌词流                      */
    YULAN_LRC_TYPE_NOTSPT,              /* 有字幕/歌词流,但无法识别             */
    YULAN_LRC_TYPE_ISUB,                /* 内嵌SUB格式字幕流                    */
    YULAN_LRC_TYPE_MSUB,                /* vob菜单字幕流                        */
    YULAN_LRC_TYPE_

} __yulan_lyric_fmt_t;


typedef enum __YULAN_PIC_FMT
{
    YULAN_PIC_TYPE_NONE = 0,            /* 没有图片流                           */
    YULAN_PIC_TYPE_NOTSPT,              /* 有图片流,但无法识别                  */

    YULAN_PIC_TYPE_

} __yulan_pic_fmt_t;


//用户自定义音效滤波器的频段数，用户需要自行设定各频段的滤波系数
#define USR_EQ_BAND_CNT             (10)
#define USR_EQ_NEGATIVE_PEAK_VALUE  (-12)
#define USR_EQ_POSITIVE_PEAK_VALUE  (+12)

typedef enum __YULAN_AUDIO_EQ           /* 音效类型定义                         */
{
    YULAN_AUD_EQ_TYPE_NORMAL = 0,       /* 自然                                 */
    YULAN_AUD_EQ_TYPE_DBB,              /* 重低音                               */
    YULAN_AUD_EQ_TYPE_POP,              /* 流行                                 */
    YULAN_AUD_EQ_TYPE_ROCK,             /* 摇滚                                 */
    YULAN_AUD_EQ_TYPE_CLASSIC,          /* 古典                                 */
    YULAN_AUD_EQ_TYPE_JAZZ,             /* 爵士                                 */
    YULAN_AUD_EQ_TYPE_VOCAL,            /* 语言                                 */
    YULAN_AUD_EQ_TYPE_DANCE,            /* 舞曲                                 */
    YULAN_AUD_EQ_TYPE_SOFT,             /* 柔和                                 */
    YULAN_AUD_EQ_TYPE_USR_MODE = 0xff,    /* 用户模式, pbuffer = __s8 UsrEqArray[USR_EQ_BAND_CNT],
                                           每个点的值为:
                                           USR_EQ_POSITIVE_PEAK_VALUE~USR_EQ_NEGATIVE_PEAK_VALUE        */
    YULAN_AUD_EQ_TYPE_

} __yulan_audio_eq_t;


typedef enum __YULAN_SUBTITLE_ENCODE
{
    YULAN_SUB_ENCODE_UNKNOWN = -1, // unknown subtitle encode type

    YULAN_SUB_ENCODE_NONE = 0,  // none subtitle bitstream
    YULAN_SUB_ENCODE_BITMAP,    // subtitle encode type is bitmap
    YULAN_SUB_ENCODE_UTF8,      // subtitle encode type is UTF8
    YULAN_SUB_ENCODE_GB2312,    // subtitle encode type is GB2312
    YULAN_SUB_ENCODE_UTF16LE,   // subtitle encode type is UTF16-LE
    YULAN_SUB_ENCODE_UTF16BE,   // subtitle encode type is UTF16-BE
    YULAN_SUB_ENCODE_UTF32LE,   // subtitle encode type is UTF32-LE
    YULAN_SUB_ENCODE_UTF32BE,   // subtitle encode type is UTF32-BE
    YULAN_SUB_ENCODE_BIG5,      // subtitle encode type is BIG5
    YULAN_SUB_ENCODE_GBK,       // subtitle encode type is GBK
    YULAN_SUB_ENCODE_ANSI,      // subtitle encode type is text, unknown character encode type

    YULAN_SUB_ENCODE_

} __yulan_subtitle_encode_t;


typedef enum __YULAN_GET_SUB_INF
{
    YULAN_GET_SUB_INF_NONE = 0, //invalid, return no data

    YULAN_GET_SUB_INF_ALL,      //get all subtitle item information
    YULAN_GET_SUB_INF_ITEM,     //get subtitle item information according to the given time

    YULAN_GET_SUB_INF_

} __yulan_get_sub_inf_t;

typedef struct __YULAN_TAG_INF
{
    __u8    tag_magic[4];            /* 媒体文件标签的标志符'A'/'V','T','A','G' */

    __s32   file_offset;                /* 媒体文件的断点在文件内的偏移         */
    __s32   time_offset;                /* 媒体文件的断点的时间偏移             */
    __u8    decinf[YULAN_TAG_INF_SIZE]; /* 解码器的现场恢复信息                 */

} __yulan_tag_inf_t;


//the information of the media file which will be input to play
typedef struct __YULAN_MEDIA_FILE_INF
{
    char    file_path[YULAN_FPATH_SIZE];/* 媒体文件的绝对路径名                 */
    __u32   tag_inf_validflag;          /* 文件断点信息是否有效的标志, =0(invalid), =1(valid)           */
    __yulan_tag_inf_t   tag_inf;        /* 媒体文件断点播放的相关信息           */
} __yulan_media_file_inf;


//the setting information for YULAN sub-system
typedef struct __YULAN_SETTING_INF
{
    //音频播放的相关设置
    __u32       volume;                     /* 音量值                           */
    __u32       eq;                         /* 音效类型                         */
    __u32       channel;                    /* 声道模式                         */
    __u32       vpsspeed;                   /* 播放速度                         */
    __u32       ab_loopcnt;                 /* A-B播放循环次数                  */
    __u32       spectrum;                   /* 是否使能频谱解析                 */
    __s16       usr_eq_filter[USR_EQ_BAND_CNT];
    /* 用户自定义音效的滤波器频段的滤波系数 */

    __u8        volume_flag;                /* volume参数是否有效的标志         */
    __u8        eq_flag;                    /* eq参数是否有效的标志             */
    __u8        channel_flag;               /* channel参数是否有效的标志        */
    __u8        vpsspeed_flag;              /* vpsspeed参数是否有效的标志       */
    __u8        ab_loopcnt_flag;            /* ab_loopcnt参数是否有效的标志     */
    __s32       aud_suffix;                 /* 当前播放的音频流在音频流数组的下标*/

    //视频播放的相关设置
    __s32       vid_layer_hdl;              /* 视频物理图层句柄                 */
    __rect_t    vid_window;                 /* 视频图像窗口属性                 */
    __s32       vid_show_mode;              /* 视频图像显示比例模式             */
    __u32       frpic_hold_time;            /* 快进、快退状态下视频图片的停留时间                   */

    __s8        vid_layer_hdl_flag;         /* vid_layer_hdl参数是否有效的标志                      */
    __s8        vid_window_flag;            /* vid_window参数是否有效的标志                         */
    __s8        vid_show_mode_flag;         /* vid_show_mode参数是否有效的标志  */
    __s8        frpic_hold_time_flag;       /* frpic_hold_time参数是否有效的标志                    */

    //音、视频播放共享的相关设置
    __u32       frspeed;                    /* 快进、快退速度                                       */
    __u8        frspeed_flag;               /* frspeed参数是否有效的标志                            */

} __yulan_setting_inf_t;



//the information from media file
typedef struct __YUAN_MEDIA_FILE_INFO
{
    __s8        nHasAudio;          //媒体文件是否含有音频流
    __s8        nHasLrc;            //媒体文件是否含有视频流
    __s8        nHasVideo;          //媒体文件是否含用字幕流

    __u8        nStreamNum;         //媒体文件内流的总数
    __u8        nAudStrmNum;        //媒体文件内音频流的个数
    __u8        nVidStrmNum;        //媒体文件内视频流的个数
    __u8        nLrcStrmNum;        //媒体文件内字幕流的个数

    __u32       nDuration;          //媒本文件的持续总时间

    __audio_bs_info_t          pAudStrmList[YULAN_MAX_AUDIO_STREAM_NUM];    //音频流信息队列
    __video_bs_info_t          pVidStrmList;                               //视频流信息队列
    __subtitle_stream_info_t   pSubStrmList[YULAN_MAX_SUBTITLE_STREAM_NUM];

} __yulan_media_file_info_t;

typedef struct __YULAN_SUBTITLE_ITEM
{
    __u32           uPts;           // time stamp of the subtitle item, (ms)
    __u32           uDuration;      // duration of the item, (ms)
    __u16           uDataType;      // subtitle data type, text,bitmap,encode type,
    //__cedar_subtitle_encode_t
    __s32           nPicWidth;      //如果是SUB等图像格式的字幕，需要传送宽高信息
    __s32           nPicHeight;
    __colorcode_t   colorCode;  //如果是SUB等图像格式的字幕，需要传送color code以便从调色板选择颜色
    __contrast_t    contrast;   //如果是SUB等图像格式的字幕，需要传送contrast 以确定是否显示
    __u8            *pSubtitle;     // subtitle data buffer
    __s32           nSrcWidth;      //字幕的最初宽度
    __s32           nSrcHeight;     //字幕的最初长度
    __u8            isFstSubFlag;
} __yulan_subtitle_item_t;



typedef struct ___YULAN_MENUBTN_POS
{
    __u8      colorTabIdx;
    __u32     btnStartX;
    __u32     btnStartY;
    __u32     btnEndX;
    __u32     btnEndY;
} __yulan_menuBtn_pos_t;


typedef struct  __YULAN_MEUBTN_INFO_ITEM
{
    __u8                   nValidBtn;
    __u8                   slBtnColorCode[3][4];                //选择按钮的colorcode值
    __u8                   slBtnContrast[3][4];                 //选择按钮的contrast值
    __u8                   acBtnColorCode[3][4];                //激活按钮的colorcode值
    __u8                   acBtnContrast[3][4];                 //激活按钮的contrast值
    __yulan_menuBtn_pos_t  selBtnPosInfo;                       //选择按钮的位置信息
    __yulan_menuBtn_pos_t  totalBtnPosInfo;                    //所有按钮的位置信息，最小的起始地址，最大的结束地址

} __yulan_menuBtn_info_t;


/* play mode */
typedef enum __YULAN_PLAY_MODE
{
    YULAN_CLOSE_REPEAT_MODE      = 0,
    YULAN_REPEAT_TITLE_MODE      = 1,
    YULAN_REPEAT_CHAPTER_MODE    = 2,
    YULAN_REPEAT_MODE
} yulan_play_mode_t;

#endif  /* _MOD_DVDNAVI_H_ */


