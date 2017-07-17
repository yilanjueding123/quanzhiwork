/*
********************************************************************************
*                                    eMOD
*                   the Easy Portable/Player Develop Kits
*                               mod_duckweed sub-system
*
*          (c) Copyright 2009-2010, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : mod_duckweed.h
* Version: V1.0
* By     : Eric_wang
* Date   : 2010-10-14
* Description:
    本模块只针对带封装格式的视频文件. 纯音频文件调用被模块无效.
********************************************************************************
*/
#ifndef _MOD_DUCKWEED_H_
#define _MOD_DUCKWEED_H_

#define DUCKWEED_FPATH_SIZE        (1024*4)    /* 媒体文件路径名长度最大值                                 */
#define DUCKWEED_MAX_LANG_CHAR_SIZE  (32)       // 存储音轨名称的数组的最大长度,\0也包括在内
typedef enum __DUCKWEED_STATUS             /* the status os cedar                  */
{
    DUCKWEED_STAT_IDLE = 0,                 /* idle                                 */
    DUCKWEED_STAT_PLAY = 1,                 /* play，表示为得到缩略图，正在解码过程中;IO接口DUCKWEED_CMD_GET_PREVIEW_FB 是不能同时调用的 */

    DUCKWEED_STAT_NOT_EXIST = -1,            //表示duckweed不存在
} __duckweed_status_t;

typedef enum __DUCKWEED_RETURN_VAL
{
    DUCKWEED_ERR_NONE           = EPDK_OK,        //no error
    DUCKWEED_ERR_FAIL           = EPDK_FAIL,        //错误，但是不表示具体错误类型

    DUCKWEED_ERR_VBSBUF_FULL    = 1,        //VBS BUF满了。
    DUCKWEED_ERR_FILE_READ_END  = 2,        //文件读完了。

    DUCKWEED_ERR_FILE_READ_FAIL      = -2,  //读文件失败
} __duckweed_return_val_t;

/*******************************************************************************
Function name: __DUCKWEED_CMD_SET
Description:
    DUCKWEED_CMD_GET_PREVIEW_FB的说明:
    FB的要求是必须是RGB格式，要求APP配的变量有:
    pfb->size.width = PREVIEW_WIDTH;
    pfb->size.height = PREVIEW_HEIGHT;
    pfb->fmt.type = FB_TYPE_RGB;
    pfb->addr[0] = address;

    IO接口完成后， duckweed填写的变量有:
    pout->fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
    pout->fmt.fmt.rgb.pixseq = RGB_SEQ_ARGB;
Parameters:

Return:

Time: 2010/11/22
*******************************************************************************/
typedef enum __DUCKWEED_CMD_SET             /* 媒体播放中间件命令列表(IOCtrol)                              */
{
    DUCKWEED_CMD_OPEN_MEDIAFILE = 0x0100,   /* 设置待播放的媒体文件路径及其他信息,打开文件pbuffer = (__duckweed_media_file_inf *) */
    DUCKWEED_CMD_CLOSE_MEDIAFILE,           /* 关闭当前的媒体文件，可能还要关闭解码库 */

    DUCKWEED_CMD_GET_FILE_FORMAT  = 0x0200, /* 得到文件信息，包括文件格式 */
    DUCKWEED_CMD_GET_FILE_SIZE,             /* 得到文件的真实大小, pbuffer = __s64*, ret = EPDK_OK,例如odf加密文件中实际的文件大小*/
    DUCKWEED_CMD_GET_TOTAL_TIME,            /* 获取播放总时间   return = total time(ms)                     */
    DUCKWEED_CMD_GET_VIDSTREAM_CNT,         /* 得到视频流的数量， ret = 数量(-1代表失败) */
    DUCKWEED_CMD_GET_VIDSTREAM_PROFILE,     /* 获取所有视频流的说明信息: pbuffer = __duckweed_vidstream_profile_t, 并且AudStrmListArray也由app分配. return = EPDK_OK or EPDK_FAIL */
    DUCKWEED_CMD_GET_AUDSTREAM_CNT,         /* 得到音频流的数量， ret = 数量(-1代表失败)*/
    DUCKWEED_CMD_GET_AUDSTREAM_PROFILE,     /* 获取所有音频流的说明信息: pbuffer = __duckweed_audstream_profile_t, 并且AudStrmListArray也由app分配. return = EPDK_OK or EPDK_FAIL */

    DUCKWEED_CMD_GET_PREVIEW_FB = 0x0300,       /* 得到预览图, pbuffer = FB*，FB的参数有要求,现在只支持拿第一帧，一个文件第一帧解完后，再调该接口，直接返回失败，将来可以做到不断解码 */
    DUCKWEED_CMD_GET_PREVIEW_FB_BY_PTS,         /* 得到预览图, pbuffer = FB*，FB的参数有要求,aux = pts(ms),表示需要在该PTS附近的关键帧。该接口目前不工作，将来扩展用     */

    DUCKWEED_CMD_SET_USER_FILEOP  = 0x05000,    // /* 设置用户的context,如回调函数,文件长度等, (pbuffer = DuckweedUsrFileOp*, )，不能直接使用cedar的数据结构__cedar_usr_file_op_t* */
    DUCKWEED_CMD_,
} __duckweed_cmd_set_t;

//the information of the media file which will be input to play
typedef struct
{
    char    file_path[DUCKWEED_FPATH_SIZE];    /* 媒体文件的绝对路径名,由app填写                 */
    __s32   file_format; //__sw_file_format_t, 由duckweed填写
    __s64   file_size;  //指真正音视频文件的大小，由duckweed填写,(例如如果是ODF加密格式文件，file_size指的是其内部包含的音视频文件的大小，并非odf文件的大小)
    __s32   ext[8];     //扩展用
} __duckweed_media_file_inf;

//#if 1
//typedef enum
//{
//    DUCKWEED_AUD_CTRL_CODEC_TYPE         = (1<<0),
//    DUCKWEED_AUD_CTRL_CHANNELS           = (1<<1),
//    DUCKWEED_AUD_CTRL_BITS_PER_SAMPLE    = (1<<2),
//    DUCKWEED_AUD_CTRL_SAMPLE_PER_SECOND  = (1<<3),
//    DUCKWEED_AUD_CTRL_AVG_BIT_RATE       = (1<<4),
//    DUCKWEED_AUD_CTRL_MAX_BIT_RATE       = (1<<5),
//    DUCKWEED_AUD_CTRL_CBR                = (1<<6),
//    DUCKWEED_AUD_CTRL_LANG               = (1<<7),
//} __duckweed_aud_ctrlbit_t;

//audio format information
typedef struct  __DUCKWEED_AUDIO_BS_INFO
{
    //__u32       ctrl_bits;      //__duckweed_aud_ctrlbit_t
    __s32       codec_type;     //采用__sw_audio_format_e, 不用__cedar_audio_fmt_t
    __s32       channels;
    __s32       bits_per_sample;
    __s32       sample_per_second;
    __s32       avg_bit_rate;
    __s32       max_bit_rate;
    __s32       cbr_flg;    //cbr还是vbr
    __s32       lang_encode_type;    //采用__epdk_charset_enm_e, 不用__cedar_subtitle_encode_t，音轨名称的字符的编码格式
    __u8        lang[DUCKWEED_MAX_LANG_CHAR_SIZE];  //音轨名称:例如中文，英文等
    __s32       ext[8];
} __duckweed_audio_bs_info_t;

typedef struct
{
    __u8                        nAudStrmNum;    //audio number, <= nAudStrmMaxCnt
    __s32                       nAudStrmMaxCnt; //数组AudStrmListArray的元素个数
    __duckweed_audio_bs_info_t  *AudStrmArray;
} __duckweed_audstream_profile_t;

//typedef enum
//{
//    DUCKWEED_VID_CTRL_CODEC_TYPE    = (1<<0),
//    DUCKWEED_VID_CTRL_PIC_SIZE      = (1<<1),
//    DUCKWEED_VID_CTRL_FRAME_RATE    = (1<<2),
//    DUCKWEED_VID_CTRL_AVG_BIT_RATE  = (1<<3),
//    DUCKWEED_VID_CTRL_MAX_BIT_RATE  = (1<<4),
//    DUCKWEED_VID_CTRL_CBR           = (1<<5),
//} __duckweed_vid_ctrlbit_t;

//video format inforamtion
typedef struct __DUCKWEED_VIDEO_BS_INFO
{
    //__u32       ctrl_bits;  //__duckweed_vid_ctrlbit_t
    __u16       codec_type; //__sw_codec_type_t
    __u16       width;
    __u16       height;
    __u16       frame_rate; //数值乘以1000
    __s32       avg_bit_rate;
    __s32       max_bit_rate;
    __u8        cbr_flg;    //可变帧率还是固定帧率
    __s32       ext[8];
} __duckweed_video_bs_info_t;

typedef struct
{
    __s32                        nVidStrmNum;    //video number, <= nVidStrmMaxCnt
    __s32                       nVidStrmMaxCnt; //数组VidStrmListArray的元素个数
    __duckweed_video_bs_info_t  *VidStrmArray;
} __duckweed_vidstream_profile_t;
//#endif


typedef struct tag_DuckweedUsrFileOp
{
    __pCBK_t usr_fread;                               /*回调函数(读文件)*/
    __u32 fp;                                         /*文件句柄*/
    __sw_file_format_t media_fmt;               /*多媒体文件文件格式*/
    __s32 file_size;                                  /*文件长度*/
    __u32 flag;
    __pCBK_t usr_fseek;
    __u32 reserved[4];                                /*保留,请设置为0,*/
} DuckweedUsrFileOp;

#endif  /* _MOD_DUCKWEED_H_ */

