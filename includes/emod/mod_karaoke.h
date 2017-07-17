/*
********************************************************************************
*                                    eMOD
*                   the Easy Portable/Player Develop Kits
*                               mod_cedar sub-system
*                          (module name, e.g.mpeg4 decoder plug-in) module
*
*          (c) Copyright 2010-2012, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : mod_karaoke.h
* Version: V1.0
* By     : Eric_wang
* Date   : 2011-8-11
* Description:
    基于cedar的各插件而设计,故大量复用cedar的数据结构定义,数据类型定义,命令集.
********************************************************************************
*/
#ifndef _MOD_KARAOKE_H_
#define _MOD_KARAOKE_H_

#include "mod_cedar.h"
#include "mod_ginkgo.h"

typedef enum __KARAOKE_CMD_SET            /* 卡拉OK中间件命令列表(IOCtrol) */
{
    /* karaoke中间件,基于cedar core的各插件,与cedar平级 */
    KARAOKE_CMD_SET_MEDIAFILE = 0x2000 ,         /* 设置伴音的文件路径                                                                                   */
    KARAOKE_CMD_SET_RECORD_MEDIAFILE,           /* 设置要录制的文件参数信息, 其中的采样率就会作为microphone的采样率, pbuffer = __ginkgo_media_file_inf* */
    KARAOKE_CMD_SET_CHAN_OUTPUT_MODE,           /* 设置播放时的声道输出模式(左、右、立体声), aux = AudioChanOutputMode, return = EPDK_OK/EPDK_FAIL      */
    KARAOKE_CMD_GET_MESSAGE_CHN,                /* 获取karaoke主控的消息反馈通道                                                                        */

    KARAOKE_CMD_ENABLE_MUSIC = 0x2100,          /* (1)music和microphone至少选一个, aux = 1: 打开music; aux=0: 关闭music;                                */
    KARAOKE_CMD_ENABLE_MICROPHONE,              /* (2)aux = 1: 打开microphone; aux=0: 关闭microphone；                                                  */
    KARAOKE_CMD_ENABLE_RECORD,                  /* (3)aux = 1: 加载录制插件,进行录制; aux = 0: 卸载录制插件,文件写结束;                                 */
    //KARAOKE_CMD_ENABLE_AUDIOPLYBK,            /* (4)系统默认 aux = 1: enable，播放音乐;                                                               */

    KARAOKE_CMD_SET_RECORD_SOURCE,              /*设置录音音源, 默认GINKGO_AUDIO_SRC_MIC*/


    KARAOKE_CMD_PLAY = 0x2200,                  /* 启动卡拉OK                                                                    */
    KARAOKE_CMD_STOP,                           /* 卡拉OK模块停止播放和录制，卸载插件                                            */
    KARAOKE_CMD_PAUSE,                          /* 设置music和录制的暂停，但如果此时microphone打开，microphone的声音可以正常输出 */
    KARAOKE_CMD_GET_STATUS,                     /* 获取karaoke主控的状态 */

    KARAOKE_CMD_GET_TOTAL_TIME = 0x2300,        /* 获取播放文件总时间，单位ms                                 */
    KARAOKE_CMD_GET_CUR_TIME,             /* 获取当前播放时间 return = current play time(ms)              */
    //    KARAOKE_CMD_GET_E_FREE_SPACE,               /* 获取E磁盘剩余磁盘容量大小，单位MB                          */
    //    KARAOKE_CMD_GET_F_FREE_SPACE,               /* 获取F磁盘剩余磁盘容量大小，单位MB                          */
    KARAOKE_CMD_GET_TAG,

    KARAOKE_CMD_ENABLE_ECHO = 0x2400,           /* 设置回音消除使能；aux = 1: enable; aux =0: disable         */
    KARAOKE_CMD_ENABLE_ECHO_EFFECT,             /* 设置混音效果使能，aux = 1: enable; aux =0: disable         */
    KARAOKE_CMD_SET_ECHO_EFFECT_OUTPUT_MODE,    /* 设置混音效果模式                                           */

    KARAOKE_CMD_SET_ECHO_EFFECT_MODE,           /* 设置输出音效模式                                           */

    KARAOKE_CMD_SET_VID_LAYERHDL = 0x2504,      /* 设置视频图层句柄 pbuffer = video_layer_hdl                 */
    KARAOKE_CMD_SET_VID_WINDOW,                 /* 设置视频窗口属性 pbuffer = (__rect_t *)                    */

    KARAOKE_CMD_

} __karaoke_cmd_set_t;


typedef enum tag_KaraokeStatus          /* the status os cedar                  */
{
    KARAOKE_STAT_PLAY = 0,              /* play                                 */
    KARAOKE_STAT_PAUSE,                 /* PAUSE                                */
    KARAOKE_STAT_STOP,                  /* stop                                 */
    KARAOKE_STAT_
} KaraokeStatus;


typedef enum tag_AudioChanOutputMode
{
    AUDIO_CHANNEL_MODE_STEREO = 0,           /* 立体声模式                       */
    AUDIO_CHANNEL_MODE_LEFT,                 /* 左声模式                         */
    AUDIO_CHANNEL_MODE_RIGHT,                /* 右声模式                         */

    AUDIO_CHANNEL_MODE_
} AudioChanOutputMode;

typedef enum tag_KaraokeEchoOputMode
{
    AUDIO_ECHO_MODE_0 = 0,              /* do nothing                       */
    AUDIO_ECHO_MODE_1,                  /* 模式1                            */
    AUDIO_ECHO_MODE_2,                  /* 模式2                            */
    AUDIO_ECHO_MODE_USER =  0xff,       /* 用户模式                         */

    AUDIO_ECHO_MODE_
} KaraokeEchoOutputMode;


typedef enum tag_KaraokeFeedbackMsg           /* 定义媒体播放中间件反馈给应用程序的消息类型               */
{
    //    CEDAR_ERROR_NOP                 =   0,  /* 空消息                                                   */
    //
    //cedar feed back error type
    KARAOKE_ERROR_OPEN_FILE_FAIL      = -20,  /* 打开当前媒体文件失败                                     */
    KARAOKE_ERROR_FILE_FMT_UNKNOWN    = -19,  /* 无法识别当前媒体文件格式                                 */
    KARAOKE_ERROR_FILE_NOT_SUPPORT    = -18,  /* 当前媒体文件不支持                                       */
    //    CEDAR_ERROR_FILE_DATA_ERROR,            /* 媒体文件数据错，无法解析                                 */
    KARAOKE_ERROR_NO_FILE_INPUT       = -16,  /* 未设置要播放的媒体文件                                   */
    //    CEDAR_ERROR_READ_FILE_FAIL,             /* 读取文件数据失败                                         */
    //
    KARAOKE_ERROR_START_PLG_FAIL      = -40,  /* 启动插件失败                                             */
    //    CEDAR_ERROR_ADEC_ERROR,                 /* 音频解码插件解码错误                                     */
    //    CEDAR_ERROR_VDEC_ERROR,                 /* 视频解码插件解码错误                                     */
    //    CEDAR_ERROR_TDEC_ERROR,                 /* 字幕解码插件解码错误                                     */
    //
    //    CEDAR_ERROR_PIC_SIZE_TOO_LARGE  = -60,  /* 图片尺寸太大，无法支持解码                               */
    //    CEDAR_ERROR_REQ_MEMORY_FAIL,            /* 申请内存失败                                             */
    //
    //    CEDAR_ERROR_UNKOWN               = -80, /* 未知错误                                                 */
    //

    KARAOKE_GINKGO_ERROR_CREATE_FILE_FAIL = -60,    //GINKGO_ERROR_CREATE_FILE_FAIL
    KARAOKE_GINKGO_ERROR_START_PLG_FAIL,            //GINKGO_ERROR_START_PLG_FAIL

    //cedar feed back status type
    KARAOKE_FEDBAK_NO_ERROR           =  1,   /* 空消息                                                   */
    KARAOKE_FEDBAK_PLAY_END,                  /* 播放媒体文件结束                                         */

    KARAOKE_ERROR_

} KaraokeFeedbackMsg;



//   in_gain;			//输入增益
//   out_gain; 		//输出增益
//   num_delays;		//回音次数
//   delay[MAX_ECHOS]; //回音时间ms,
//dealy[0], if uEchoOutputMode==0xff,dealy[0] use,(ms)
//delay[1], 0,no nothing,1,2,0xff 用户模式,echo effect；
//delay[2], user echo_effects mode
//int   decay[MAX_ECHOS]; //比例系数,now use for voice change
//[0]:0,nothing,1,2,3,0xff用户模式
//[1]:tempo,节拍[-50 100]
//[2]:pitch,音调[-12 12]
//[3]:speed,速度[-50 100]

typedef struct tag_KaraokeEchoAudioInf
{
    __s32       uInGain;
    __s32       uOutGain;
    __s32       uNumDelay;
    __s32       uDelay[16];
    __s32       uDecay[16];

} KaraokeEchoAudioInf;

typedef struct tag_KaraokeEchoEffectInf
{
    __s8       nEchoEnable;              //echo  enable;回音消除       0
    __s8       nMixVoiceEnable;          //echo effect enable;混音开关 1
    __s8       nChangeVoiceEnable;       //变声；                      2
    __s8       nChorusVoiceEnable;       //合声；                      3
    __s8       nSpecialVoiceEnable;      //特殊音效；                  4
    KaraokeEchoOutputMode   uEchoOutputMode;       //echo effect,for delay[1],
    //0,no nothing,1,2,0xff 用户模式,echo effect
    KaraokeEchoAudioInf     uEchoAudioInf;
} KaraokeEchoEffectInf;


//typedef struct tag_CedarAudioInputInf   /* 音频输入的参数,设置adc, 现在不需要, 等需要时再用                            */
//{
//    __u32       uChn;                   /* 录音的声道模式，如果设置非法，则采用default值,类型为__ginkgo_audio_chn_t    */
//    __u32       uSampWidth;             /* 录音样本点的位宽，如果设置非法，则采用default值                             */
//    __u32       uAudioSampRate;         /* 音频码流的采样率，如果设置非法，则采用default值                             */
//    __u32       uAudioSource;           /* 音频源，如果设置非法，则采用default值, GINKGO_AUDIO_SRC_MIC                 */
//    __u32       uVolume;                /* 录音的音量值，如果设置非法，则采用default值                                 */
//    __u32       uAbsFmt;                /* 音频码流类型，if(uAbsFmt == GINKGO_ABS_TYPE_NONE), 将忽略其它相关音频参数   */
//    __u32       uAudioBps;              /* 音频码流的码率，如果设置非法，则采用default值                               */
//    __u32       reserved[12];
//} CedarAudioInputInf;
//__ginkgo_audio_inf_t

#endif  /* _MOD_KARAOKE_H_ */


