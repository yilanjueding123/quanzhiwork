/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               DataType Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : typedef.h
* By      : kevin.z
* Version : v2.0
* Date    : 2010-12-2 9:11
* Descript:
* Update  : date                auther      ver     notes
*           2010-12-2 9:11      kevin.z     2.0     build the file;
*********************************************************************************************************
*/
#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

#include "mediafmt.h"

/*
**********************************************************************************************************************
*                                              常量定义
**********************************************************************************************************************
*/
#ifndef EPDK_OK
#define EPDK_OK         0                   /* 代表成功                                                     */
#endif

#ifndef EPDK_FAIL
#define EPDK_FAIL       (-1)                /* 代表失败                                                     */
#endif

#ifndef EPDK_TRUE
#define EPDK_TRUE       1                   /* 真                                                           */
#endif

#ifndef EPDK_FALSE
#define EPDK_FALSE      0                   /* 假                                                           */
#endif

#ifndef EPDK_DISABLED
#define EPDK_DISABLED   0
#endif

#ifndef EPDK_ENABLED
#define EPDK_ENABLED    1
#endif

#ifndef EPDK_NO
#define EPDK_NO         0
#endif

#ifndef EPDK_YES
#define EPDK_YES        1
#endif

#ifndef EPDK_OFF
#define EPDK_OFF        0
#endif

#ifndef EPDK_ON
#define EPDK_ON         1
#endif

#ifndef EPDK_CLR
#define EPDK_CLR        0
#endif

#ifndef EPDK_SET
#define EPDK_SET        1
#endif

#ifndef NULL
#define NULL            0                   /* 指针空                                                  */
#endif

/*
**********************************************************************************************************************
*                                              DATA TYPES
**********************************************************************************************************************
*/
//**************************************************
//normal typedef
#ifndef DATA_TYPE_X___u64
#define DATA_TYPE_X___u64
typedef unsigned __int64    __u64;
#endif

#ifndef DATA_TYPE_X___u32
#define DATA_TYPE_X___u32
typedef unsigned int        __u32;
#endif

#ifndef DATA_TYPE_X___u16
#define DATA_TYPE_X___u16
typedef unsigned short      __u16;
#endif

#ifndef DATA_TYPE_X___u8
#define DATA_TYPE_X___u8
typedef unsigned char       __u8;
#endif

#ifndef DATA_TYPE_X___s64
#define DATA_TYPE_X___s64
typedef signed __int64      __s64;
#endif

#ifndef DATA_TYPE_X___s32
#define DATA_TYPE_X___s32
typedef signed int          __s32;
#endif

#ifndef DATA_TYPE_X___s16
#define DATA_TYPE_X___s16
typedef signed short        __s16;
#endif

#ifndef DATA_TYPE_X___s8
#define DATA_TYPE_X___s8
typedef signed char         __s8;
#endif

#ifndef DATA_TYPE_X___bool
#define DATA_TYPE_X___bool
typedef signed char         __bool;
#endif

#ifndef DATA_TYPE_X___stk
#define DATA_TYPE_X___stk
typedef unsigned int        __stk;                  /* Each stack entry is 32-bit wide                              */
#endif

#ifndef DATA_TYPE_X___cpu_sr
#define DATA_TYPE_X___cpu_sr
typedef unsigned int        __cpu_sr;               /* Define size of CPU status register (PSR = 32 bits)           */
#endif


#ifndef DATA_TYPE_X___fp32
#define DATA_TYPE_X___fp32
typedef float               __fp32;                 /* Single precision floating point                              */
#endif

#ifndef DATA_TYPE_X___fp64
#define DATA_TYPE_X___fp64
typedef double              __fp64;                 /* Double precision floating point                              */
#endif

#ifndef DATA_TYPE_X___hdle
#define DATA_TYPE_X___hdle
typedef unsigned int        __hdle;
#endif

#ifndef DATA_TYPE_X___size
#define DATA_TYPE_X___size
typedef unsigned int        __size;
#endif

#ifndef DATA_TYPE_X___size_t
#define DATA_TYPE_X___size_t
typedef unsigned int        __size_t;
#endif

#ifndef DATA_TYPE_X___sector_t
#define DATA_TYPE_X___sector_t
typedef unsigned int        __sector_t;
#endif

typedef __s32   (* __pCBK_t )(void * /*p_arg*/);    /* call back function pointer                                   */
typedef __bool  (* __pISR_t )(void * /*p_arg*/);    /* isr function pointer                                         */
typedef void    (* __pTHD_t )(void * /*p_arg*/);    /* thread prototype                                             */
typedef __s32   (* __pSWI_t )(__u32 /*r0*/, __u32 /*r1*/, __u32 /*r2*/, __u32 /*r3*/);/* swi handler pointer        */
typedef __s32   (* __pCB_ClkCtl_t   )(__u32 /*cmd*/, __s32 /*aux*/);
typedef __s32   (* __pCB_DPMCtl_t   )(__u32 /*cmd*/, void * /*parg*/);

#if(EPDK_OS == EPDK_OS_EPOS)
//**************************************************
//display struct typedef
typedef struct
{
    __u8  alpha;
    __u8 red;
    __u8 green;
    __u8 blue;
} COLOR;        /* 32-bit (ARGB) color                  */
#ifndef WIN32_LEAN_AND_MEAN
typedef struct
{
    __s32 x;
    __s32 y;
    __u32 width;
    __u32 height;
} RECT;         /* rect attrib                          */
typedef struct
{
    __u32 width;
    __u32 height;
} SIZE;         /* rect size                            */
#endif // SIM_PC_WIN
typedef struct
{
    __s32 x;
    __s32 y;
} OFFSET;       /* coordinate (x, y)                    */
typedef COLOR  __color_t;
typedef RECT   __rect_t;
typedef OFFSET __pos_t;
typedef SIZE   __rectsz_t;

typedef enum __PIXEL_RGBFMT                         /* pixel format(rgb)                                            */
{
    PIXEL_MONO_1BPP = 0,                            /* only used in normal framebuffer                              */
    PIXEL_MONO_2BPP,
    PIXEL_MONO_4BPP,
    PIXEL_MONO_8BPP,
    PIXEL_COLOR_RGB655,
    PIXEL_COLOR_RGB565,
    PIXEL_COLOR_RGB556,
    PIXEL_COLOR_ARGB1555,
    PIXEL_COLOR_RGBA5551,
    PIXEL_COLOR_RGB0888,                            /* used in normal framebuffer and scaler framebuffer            */
    PIXEL_COLOR_ARGB8888,
} __pixel_rgbfmt_t;
typedef enum __PIXEL_YUVFMT                         /* pixel format(yuv)                                            */
{
    PIXEL_YUV444 = 0x10,                            /* only used in scaler framebuffer                              */
    PIXEL_YUV422,
    PIXEL_YUV420,
    PIXEL_YUV411,
    PIXEL_CSIRGB,
    PIXEL_OTHERFMT,
} __pixel_yuvfmt_t;
typedef enum __YUV_MODE                             /* Frame buffer data mode definition                            */
{
    YUV_MOD_INTERLEAVED = 0,
    YUV_MOD_NON_MB_PLANAR,                          /* 无宏块平面模式                                               */
    YUV_MOD_MB_PLANAR,                              /* 宏块平面模式                                                 */
    YUV_MOD_UV_NON_MB_COMBINED,                     /* 无宏块UV打包模式                                             */
    YUV_MOD_UV_MB_COMBINED                          /* 宏块UV打包模式                                               */
} __yuv_mod_t;
typedef enum                                        /* yuv seq                                                      */
{
    YUV_SEQ_UYVY = 0,                               /* 以下4种指适合于yuv422 加 interleaved的组合方式               */
    YUV_SEQ_YUYV,
    YUV_SEQ_VYUY,
    YUV_SEQ_YVYU,
    YUV_SEQ_AYUV = 0x10,                            /* 以下2种只适合于yuv444 加 interleaved的组合方式               */
    YUV_SEQ_VUYA,
    YUV_SEQ_UVUV = 0x20,                            /* 以下2种只适合于yuv420 加 uv_combined的组合方式               */
    YUV_SEQ_VUVU,
    YUV_SEQ_OTHRS = 0xff,                           /* 其他的pixelfmt和mod的组合方式                                */
} __yuv_seq_t;
typedef enum
{
    FB_TYPE_RGB = 0,
    FB_TYPE_YUV = 1
} __fb_type_t;

typedef enum
{
    BT601 = 0,
    BT709,
    YCC,
    VXYCC,
    MONO
} __cs_mode_t;

typedef enum
{
    RGB_SEQ_ARGB = 0x00,//for 32bpp
    RGB_SEQ_BRGA = 0x02,
    RGB_SEQ_P10 = 0x10,//for 16bpp
    RGB_SEQ_P01 = 0x11,
    RGB_SEQ_P3210 = 0x20,//for 8bpp
    RGB_SEQ_P0123 = 0x21,
    RGB_SEQ_P76543210 = 0x30,//for 4bpp
    RGB_SEQ_P67452301 = 0x31,
    RGB_SEQ_P10325476 = 0x32,
    RGB_SEQ_P01234567 = 0x33,
    RGB_SEQ_2BPP_BIG_BIG = 0x40,//for 2bpp
    RGB_SEQ_2BPP_BIG_LITTER = 0x41,
    RGB_SEQ_2BPP_LITTER_BIG = 0x42,
    RGB_SEQ_2BPP_LITTER_LITTER = 0x43,
    RGB_SEQ_1BPP_BIG_BIG = 0x50,//for 1bpp
    RGB_SEQ_1BPP_BIG_LITTER = 0x51,
    RGB_SEQ_1BPP_LITTER_BIG = 0x52,
    RGB_SEQ_1BPP_LITTER_LITTER = 0x53,
} __rgb_seq_t;

typedef struct
{
    __fb_type_t                 type;               /* 0 rgb, 1 yuv                                                 */
    union
    {
        struct
        {
            __pixel_rgbfmt_t    pixelfmt;           /* 像素的格式                                                   */
            __bool              br_swap;            /* blue red color swap flag                                     */
            __rgb_seq_t         pixseq;             /* 图像流的存储顺序                                             */
            struct                                  /* 调色板                                                       */
            {
                void           *addr;               /* 如果pixel为非bpp格式，调色板指针为0                          */
                __u32           size;
            } palette;
        } rgb;
        struct
        {
            __pixel_yuvfmt_t    pixelfmt;           /* 像素的格式                                                   */
            __yuv_mod_t         mod;                /* 图像的格式                                                   */
            __yuv_seq_t         yuvseq;             /* yuv的排列顺序                                                */
        } yuv;
    } fmt;
    __cs_mode_t                    cs_mode;            /*color space                                                     */
} __fb_format_t;

typedef struct __FB                                 /* frame buffer                                                 */
{
    SIZE                        size;               /* frame buffer的长宽                                           */
    void                       *addr[3];            /* frame buffer的内容地址，对于rgb类型，只有addr[0]有效         */
    __fb_format_t               fmt;
} FB;


/*
**********************************************************************************************************************
*                                             宏定义
**********************************************************************************************************************
*/
#define SWINO(base,type,isr) (base+(((__u32)(&(((type*)0)->isr)))>>2))

#endif
#ifdef SIM_PC_WIN
#define __FUNCTION__ __FILE__
#endif
#endif  /*#ifndef __TYPEDEF_H__*/


