/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eBIOS Sub-System
*
*                                   (c) Copyright 2007-2009, Jshwang
*                                             All Rights Reserved
*
* Moudle  : csi bios module
* File    : csi_i.h
*
* By      : Jshwang
* Version : v1.00
* Date    : 2009-9-4 11:17:13
**********************************************************************************************************************
*/
#ifndef _CSI_H_
#define _CSI_H_

#include "epdk.h"
#include "csi_inc.h"

/*
 * input data format
 */
typedef enum tag_CSI_INPUT_FMT
{
    CSI_RAW               = 0,      /* raw stream  */
    CSI_BAYER             = 1,     /* byer rgb242 */
    CSI_CCIR656           = 2,    /* ccir656     */
    CSI_YUV422            = 3,   /* yuv422      */
    //Reserved            = 4,   /* yuv422      */
    CSI_CCIR656_TDM_2CH   = 5,  /* ccir656 2ch */
    //Reserved            = 6,   /* yuv422      */
    CSI_CCIR656_TDM_4CH   = 7, /* ccir656 4ch */
} __csi_input_fmt_t;

/*
 * output data format
 */
typedef enum tag_CSI_OUTPUT_FMT
{
    /* only when input is raw */
    CSI_PASS_THROUTH = 0,                     /* raw */
    /* only when input is bayer */
    CSI_PLANAR_RGB242 = 0,           /* planar rgb242 */
    /* only when input is ccir656 */
    CSI_FIELD_PLANAR_YUV422 = 0,       					/* parse a field(odd or even) into planar yuv420 */
    CSI_FIELD_PLANAR_YUV420 = 1,       					/* parse a field(odd or even) into planar yuv420 */
    CSI_FRAME_PLANAR_YUV420 = 2,
    CSI_FRAME_PLANAR_YUV422 = 3,
    CSI_FIELD_UV_CB_YUV422 = 4,       					/* parse and reconstruct evry 2 fields(odd and even) into a frame, format is planar yuv420 */
    CSI_FIELD_UV_CB_YUV420 = 5,
    CSI_FRAME_UV_CB_YUV420 = 6,
    CSI_FRAME_UV_CB_YUV422 = 7,
    CSI_FIELD_MB_YUV422 = 8,
    CSI_FIELD_MB_YUV420 = 9,
    CSI_FRAME_MB_YUV420 = 10,
    CSI_FRAME_MB_YUV422 = 11,
    CSI_INTLC_INTLV_YUV422 = 15,
    /* only when input is yuv422 */
    CSI_PLANAR_YUV422 = 0,             					/* parse yuv422 into planar yuv422 */
    CSI_PLANAR_YUV420 = 1,              				/* parse yuv422 into planar yuv420 */
    CSI_UV_CB_YUV422 = 4,
    CSI_UV_CB_YUV420 = 5,
    CSI_MB_YUV422 = 8,
    CSI_MB_YUV420 = 9,
} __csi_output_fmt_t;

/*
 * input field selection, only when input is ccir656
 */
typedef enum tag_CSI_FIELD_SEL
{
    CSI_ODD,    /* odd field */
    CSI_EVEN,   /* even field */
    CSI_EITHER, /* either field */
} __csi_field_sel_t;

/*
 * input data sequence
 */
typedef enum tag_CSI_SEQ
{
    /* only when input is yuv422 */
    CSI_YUYV = 0,
    CSI_YVYU,
    CSI_UYVY,
    CSI_VYUY,
    //    /* only when input is byer */
    CSI_RGRG = 0,             /* first line sequence is RGRG... */
    CSI_GRGR,                 /* first line sequence is GRGR... */
    CSI_BGBG,                 /* first line sequence is BGBG... */
    CSI_GBGB,                 /* first line sequence is GBGB... */
} __csi_seq_t;

/*
 * input reference signal polarity
 */
typedef enum tag_CSI_REF
{
    CSI_LOW,    /* active low */
    CSI_HIGH,   /* active high */
} __csi_ref_t;

/*
 * input data valid of the input clock edge type
 */
typedef enum tag_CSI_CLK
{
    CSI_FALLING,    /* active falling */
    CSI_RISING,     /* active rising */
} __csi_clk_t;


/*
 * csi mode configuration
 */
typedef struct tag_CSI_CONF
{
    __csi_input_fmt_t  input_fmt;              /* input data format */
    __csi_output_fmt_t output_fmt;             /* output data format */
    __csi_field_sel_t  field_sel;              /* input field selection */
    __csi_seq_t        seq;                    /* input data sequence */
    __csi_ref_t        vref;                   /* input vref signal polarity */
    __csi_ref_t        href;                   /* input href signal polarity */
    __csi_clk_t        clock;                  /* input data valid of the input clock edge type */
} __csi_conf_t;

/*
 * csi clock out source
 */
//typedef enum tag_CSI_CLK_OUT_SRC
//{
//    CSI_HOSC,                  /* hosc */
//    CSI_CORE_PLL,              /* core pll */
//    CSI_IDU_PLL,               /* idu pll */
//    CSI_VIDEO_PLL,             /* video pll */
//}__csi_clk_out_src_t;

/*
 * csi buffer
 */
typedef enum tag_CSI_BUF
{
    CSI_BUF_0_A,                  /* hosc */
    CSI_BUF_0_B,              /* core pll */
    CSI_BUF_1_A,               /* idu pll */
    CSI_BUF_1_B,             /* video pll */
    CSI_BUF_2_A,             /* video pll */
    CSI_BUF_2_B,             /* video pll */
} __csi_buf_t;

/*
 * csi capture status
 */
typedef struct tag_CSI_CAPTURE_STATUS
{
    __bool picture_in_progress;                  /* hosc */
    __bool video_in_progress;              /* core pll */
} __csi_capture_status;

/*
 * csi  scale down
 */
typedef enum tag_CSI_SCALE_DOWN
{
    CSI_ORIGIN,
    CSI_HALF,
    CSI_QUARTER,
} __csi_scale_down_t;

/*
 * csi double buffer
 */
typedef enum tag_CSI_DOUBLE_BUF
{
    CSI_BUF_A,                  /* */
    CSI_BUF_B,              /*  */
} __csi_double_buf_t;

/*
 * csi double buffer status
 */
typedef struct tag_CSI_DOUBLE_BUF_STATUS
{
    __bool             enable;            /* double buffer enable */
    __csi_double_buf_t current;           /* current frame selected output type, buffer A or B*/
    __csi_double_buf_t next;              /* next frame output type, buffer A or B */
} __csi_double_buf_status_t;

/*
 * csi interrupt
 */
typedef enum tag_CSI_INT
{
    CSI_INT_CAPTURE_DONE = 0X1,                /* */
    CSI_INT_FRAME_DONE = 0X2,            /*  */
    CSI_INT_BUF_0_OVERFLOW = 0X4,            /*  */
    CSI_INT_BUF_1_OVERFLOW = 0X8,            /*  */
    CSI_INT_BUF_2_OVERFLOW = 0X10,            /*  */
    CSI_INT_PROTECTION_ERROR = 0X20,            /*  */
    CSI_INT_HBLANK_OVERFLOW = 0X40,            /*  */
    CSI_INT_VSYNC_TRIG = 0X80,
} __csi_int_t;

/*
 * csi interrupt status
 */
typedef struct tag_CSI_INT_STATUS
{
    __bool capture_done;
    __bool frame_done;
    __bool buf_0_overflow;
    __bool buf_1_overflow;
    __bool buf_2_overflow;
    __bool protection_error;
    __bool hblank_overflow;
    __bool vsync_trig;
} __csi_int_status_t;

void CSI_set_base(__u32 sel);

__s32  CSI_init(void);
__s32  CSI_exit(void);

__s32  CSI_open(void);
__s32  CSI_close(void);

__s32  CSI_configure(__csi_conf_t *mode);

__s32  CSI_set_buffer_address(__csi_buf_t buf, __u32 addr);
__u32  CSI_get_buffer_address(__csi_buf_t buf);
__s32  CSI_double_buffer_enable(void);
__s32  CSI_double_buffer_disable(void);
__s32  CSI_double_buffer_select_next(__csi_double_buf_t type);
__s32  CSI_double_buffer_get_status(__csi_double_buf_status_t *status);

__s32  CSI_capture_video_start(void);
__s32  CSI_capture_video_stop(void);
__s32  CSI_capture_picture(void);
__u32  CSI_capture_get_status(__csi_capture_status *status);

__s32  CSI_set_size(__u32 start_h, __u32 length_h, __u32 start_v, __u32 length_v, __u32 buf_length_h);

__s32  CSI_scale_down(__csi_scale_down_t scale_h, __csi_scale_down_t scale_v);

__s32  CSI_int_enable(__csi_int_t interrupt);
__s32  CSI_int_disable(__csi_int_t interrupt);
__s32  CSI_int_get_status(__csi_int_status_t *status);
__s32  CSI_int_clear_status(void);
//__s32  CSI_int_clear_status(__csi_int_t interrupt);


#endif  /* _CSI_H_ */

