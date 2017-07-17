/*
*******************************************************************************************************************
*                                                         	Module Orange
*                                         			the embedded Graphical module
*
*                             				 Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*											               All Rights Reserved
*
*File Name： 	gui_def.h
*
*Author： 		William Wang
*
*Version :   	1.1.0
*
*Date:  		2009-11-12
*
*Description : 	gui general struct definition
*
*Others : 		None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2009-11-12         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __GUI_DEF_H__
#define __GUI_DEF_H__

#define ORANGE_SUPPORT_SPRITE          (0)

#if ORANGE_SUPPORT_SPRITE
#define ORANGE_SPRITE_NUM              (32)
#endif

#ifndef I8
#define I8    	signed char
#endif
#ifndef U8
#define U8  	unsigned char     /* unsigned 8  bits. */
#endif
#ifndef I16
#define I16   	signed short    /*   signed 16 bits. */
#endif
#ifndef U16
#define U16 	unsigned short    /* unsigned 16 bits. */
#endif
#ifndef I32
#define I32   	signed long   /*   signed 32 bits. */
#endif
#ifndef U32
#define U32 	unsigned long   /* unsigned 32 bits. */
#endif
#define I16P 	I16              /*   signed 16 bits OR MORE ! */
#define U16P 	U16              /* unsigned 16 bits OR MORE ! */

/*通用GUI kernel定义*/
#define ORANGE_ALLOC_SUPPORT           (1)
#define ORANGE_SUPPORT_CURSOR          (0)
//#define ORANGE_ALLOC_SIZE          	   (800*480*4*2)     /* Size of dynamic memory ... For WM and memory devices         */
#define ORANGE_ALLOC_SIZE          	   (200)     /* Size of dynamic memory ... For WM and memory devices         */
#define ORANGE_ALLOC_DATATYPE          I32
#define ORANGE_ALLOC_DATATYPE_U        I32
#define ORANGE_HMEM                    I32
#define ORANGE_HMEM_NULL               (0)
typedef ORANGE_HMEM                     ORANGE_HWIN;
typedef ORANGE_HMEM                     ORANGE_HSPRITE;

/*GDI 相关配置信息*/
#define GDI_SUPPORT_UNICODE         (1)             /* Support mixed ASCII/UNICODE strings 		                    */
#define GDI_SUPPORT_MEMDEV       	(1)  	/* Memory devices available */
#define GDI_NUM_LAYERS              (16)
#define GDI_DEFAULT_FONT          	&GUI_Font6x8
#define GDI_SUPPORT_CHINESE   		0   /*added by mcl*/
#define GDI_AA_SUPPORT            	0  	/* Anti aliasing available */
#define GDI_SKIN_SUPPORT            0   /* skin switch */

/*
 * @internal
 *
 * Encodes format constants in the following way (bit 31 - 0):
 *
 * lkjj:hhgg | gfff:eeed | cccc:bbbb | baaa:aaaa
 *
 * a) pixelformat index<br>
 * b) effective color (or index) bits per pixel of format<br>
 * c) effective alpha bits per pixel of format<br>
 * d) alpha channel present<br>
 * e) bytes per "pixel in a row" (1/8 fragment, i.e. bits)<br>
 * f) bytes per "pixel in a row" (decimal part, i.e. bytes)<br>
 * g) smallest number of pixels aligned to byte boundary (minus one)<br>
 * h) multiplier for planes minus one (1/4 fragment)<br>
 * j) multiplier for planes minus one (decimal part)<br>
 * k) color and/or alpha lookup table present<br>
 * l) alpha channel is inverted
 */
#define GUI_PIXELFORMAT( index, color_bits, alpha_bits, has_alpha,     \
                                 row_bits, row_bytes, align, mul_f, mul_d,     \
                                 has_lut, inv_alpha )                          \
     ( (((index     ) & 0x7F)      ) |                                         \
       (((color_bits) & 0x1F) <<  7) |                                         \
       (((alpha_bits) & 0x0F) << 12) |                                         \
       (((has_alpha ) ? 1 :0) << 16) |                                         \
       (((row_bits  ) & 0x07) << 17) |                                         \
       (((row_bytes ) & 0x07) << 20) |                                         \
       (((align     ) & 0x07) << 23) |                                         \
       (((mul_f     ) & 0x03) << 26) |                                         \
       (((mul_d     ) & 0x03) << 28) |                                         \
       (((has_lut   ) ? 1 :0) << 30) |                                         \
       (((inv_alpha ) ? 1 :0) << 31) )

/*
 * Pixel format of a surface.
 */
typedef enum
{
    GUIPF_UNKNOWN   = 0x00000000,  /* unknown or unspecified format */

    GUIPF_MONO1BPP  = GUI_PIXELFORMAT(  0, 1, 0, 0, 1, 0, 7, 0, 0, 0, 0 ),

    GUIPF_LUT1BPP   = GUI_PIXELFORMAT(  1, 1, 1, 0, 1, 0, 7, 0, 0, 1, 0 ),

    GUIPF_MONO2BPP  = GUI_PIXELFORMAT(  2, 2, 0, 0, 1, 0, 3, 0, 0, 0, 0 ),

    GUIPF_LUT2BPP   = GUI_PIXELFORMAT(  3, 2, 1, 0, 1, 0, 3, 0, 0, 0, 0 ),

    GUIPF_MONO4BPP  = GUI_PIXELFORMAT(  4, 4, 0, 0, 1, 0, 1, 0, 0, 0, 0 ),

    GUIPF_LUT4BPP   = GUI_PIXELFORMAT(  5, 4, 1, 0, 1, 0, 1, 0, 0, 0, 0 ),

    GUIPF_MONO8BPP  = GUI_PIXELFORMAT(  6, 8, 0, 0, 1, 0, 0, 0, 0, 0, 0 ),

    GUIPF_LUT8BPP   = GUI_PIXELFORMAT(  7, 8, 1, 1, 0, 1, 0, 0, 0, 1, 0 ),

    /* 16 bit  ARGB (2 byte, alpha 1@15, red 5@10, green 5@5, blue 5@0) */
    GUIPF_ARGB1555  = GUI_PIXELFORMAT(  8, 15, 1, 1, 0, 2, 0, 0, 0, 0, 0 ),

    /* 16 bit  ARGB (2 byte, alpha 2@14, red 5@9, green 5@4, blue 4@0) */
    GUIPF_ARGB2554  = GUI_PIXELFORMAT( 9, 14, 2, 1, 0, 2, 0, 0, 0, 0, 0 ),

    /* 16 bit   RGB (2 byte, red 5@11, green 6@5, blue 5@0) */
    GUIPF_RGB565    = GUI_PIXELFORMAT( 10, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0 ),

    /* 24 bit   RGB (3 byte, red 8@16, green 8@8, blue 8@0) */
    GUIPF_RGB888    = GUI_PIXELFORMAT( 11, 24, 0, 0, 0, 3, 0, 0, 0, 0, 0 ),

    /* 24 bit   RGB (4 byte, nothing@24, red 8@16, green 8@8, blue 8@0) */
    GUIPF_RGB8888   = GUI_PIXELFORMAT( 12, 24, 0, 0, 0, 4, 0, 0, 0, 0, 0 ),

    /* 32 bit  ARGB (4 byte, alpha 8@24, red 8@16, green 8@8, blue 8@0) */
    GUIPF_ARGB8888  = GUI_PIXELFORMAT( 13, 24, 8, 1, 0, 4, 0, 0, 0, 0, 0 ),

    /*  8 bit   RGB (1 byte, red 3@5, green 3@2, blue 2@0) */
    GUIPF_RGB332    = GUI_PIXELFORMAT( 14, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0 ),

    /* 16 bit  ARGB (2 byte, alpha 4@12, red 4@8, green 4@4, blue 4@0) */
    GUIPF_ARGB4444  = GUI_PIXELFORMAT( 15, 12, 4, 1, 0, 2, 0, 0, 0, 0, 0 ),

    /* 16 bit  RGBA (2 byte, red 4@12, green 4@8, blue 4@4, alpha 4@0) */
    GUIPF_RGBA4444  = GUI_PIXELFORMAT( 16, 12, 4, 1, 0, 2, 0, 0, 0, 0, 0 ),

    /* 16 bit   YUV (4 byte/ 2 pixel, macropixel contains YCbYCr [31:0]) */
    GUIPF_UYVY      = GUI_PIXELFORMAT( 17, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0 ),

    /* 12 bit   YUV (8 bit Y plane followed by 8 bit quarter size U/V planes) */
    GUIPF_I420      = GUI_PIXELFORMAT( 18, 12, 0, 0, 0, 1, 0, 2, 0, 0, 0 ),

    /* 12 bit   YUV (8 bit Y plane followed by 8 bit quarter size V/U planes) */
    GUIPF_YV12      = GUI_PIXELFORMAT( 19, 12, 0, 0, 0, 1, 0, 2, 0, 0, 0 ),

    /* 12 bit   YUV (8 bit Y plane followed by one 16 bit quarter size Cb|Cr [7:0|7:0] plane) */
    GUIPF_NV12      = GUI_PIXELFORMAT( 20, 12, 0, 0, 0, 1, 0, 2, 0, 0, 0 ),

    /* 16 bit   YUV (8 bit Y plane followed by one 16 bit half width Cb|Cr [7:0|7:0] plane) */
    GUIPF_NV16      = GUI_PIXELFORMAT( 21, 24, 0, 0, 0, 1, 0, 0, 1, 0, 0 ),

    /* 12 bit   YUV (8 bit Y plane followed by one 16 bit quarter size Cr|Cb [7:0|7:0] plane) */
    GUIPF_NV21      = GUI_PIXELFORMAT( 22, 12, 0, 0, 0, 1, 0, 2, 0, 0, 0 ),

    /* 32 bit  AYUV (4 byte, alpha 8@24, Y 8@16, Cb 8@8, Cr 8@0) */
    GUIPF_AYUV      = GUI_PIXELFORMAT( 23, 24, 8, 1, 0, 4, 0, 0, 0, 0, 0 ),

    /* 16 bit   BGR (2 byte, nothing @15, blue 5@10, green 5@5, red 5@0) */
    GUIPF_BGRA5551  = GUI_PIXELFORMAT( 24, 16, 1, 1, 0, 2, 0, 0, 0, 0, 0 ),

    /* 16 bit   BGR (2 byte, nothing @15, blue 5@10, green 5@5, red 5@0) */
    GUIPF_BGRA8888  = GUI_PIXELFORMAT( 25, 32, 8, 1, 0, 4, 0, 0, 0, 0, 0 ),

    /* 16 bit   BGR (2 byte, nothing @15, blue 5@10, green 5@5, red 5@0) */
    GUIPF_BGR565    = GUI_PIXELFORMAT( 26, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0 ),

    /* 16 bit   BGR (2 byte, nothing @15, blue 5@10, green 5@5, red 5@0) */
    GUIPF_RGB565    = GUI_PIXELFORMAT( 27, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0 ),

    /* 16 bit   BGR (2 byte, nothing @15, blue 5@10, green 5@5, red 5@0) */
    GUIPF_RGB556    = GUI_PIXELFORMAT( 28, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0 ),

    /* 16 bit   BGR (2 byte, nothing @15, blue 5@10, green 5@5, red 5@0) */
    GUIPF_RGB655    = GUI_PIXELFORMAT( 29, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0 ),

    /* 16 bit   YUV (4 byte/ 2 pixel, macropixel contains CbYCrY [31:0]) */
    GUIPF_YUY2      = GUI_PIXELFORMAT( 31, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0 )
} GUI_PixelFormat;
/* Number of pixelformats defined */
#define GUI_NUM_PIXELFORMATS            36

/* These macros extract information about the pixel format. */
#define GUI_PIXELFORMAT_INDEX(fmt)      (((fmt) & 0x0000007F)      )
#define GUI_COLOR_BITS_PER_PIXEL(fmt)   (((fmt) & 0x00000F80) >>  7)
#define GUI_ALPHA_BITS_PER_PIXEL(fmt)   (((fmt) & 0x0000F000) >> 12)
#define GUI_PIXELFORMAT_HAS_ALPHA(fmt)  (((fmt) & 0x00010000) !=  0)
#define GUI_BITS_PER_PIXEL(fmt)         (((fmt) & 0x007E0000) >> 17)
#define GUI_BYTES_PER_PIXEL(fmt)        (((fmt) & 0x00700000) >> 20)
#define GUI_BYTES_PER_LINE(fmt,width)   (((((fmt) & 0x007E0000) >> 17) * (width) + 7) >> 3)
#define GUI_PIXELFORMAT_ALIGNMENT(fmt)  (((fmt) & 0x03800000) >> 23)
#define GUI_PLANE_MULTIPLY(fmt,height)  ((((((fmt) & 0x3C000000) >> 26) + 4) * (height)) >> 2)
#define GUI_PIXELFORMAT_IS_INDEXED(fmt) (((fmt) & 0x40000000) !=  0)
#define GUI_PLANAR_PIXELFORMAT(fmt)     (((fmt) & 0x3C000000) !=  0)
#define GUI_PIXELFORMAT_INV_ALPHA(fmt)  (((fmt) & 0x80000000) !=  0)

typedef unsigned int    GUI_COLOR;
typedef unsigned int    GUI_YUVCOLOR;
typedef RECT            GUI_REGION;
typedef __u32           GUI_PIXELINDEX;

typedef struct
{
    int           magic;

    __u32         NumEntries;
    GUI_COLOR     *pPalEntries;
    GUI_YUVCOLOR  *entries_yuv;

    struct
    {
        int      index;
        GUI_COLOR color;
    } search_cache;

    bool          hash_attached;
} GUI_PALETTE;

typedef struct
{
    void      (* pfDraw)  (int x0, int y0, int xsize, int ysize,
                           __u8   *pPixel, GUI_PALETTE *pLogPal,
                           int xMag, int yMag);

    GUI_COLOR (* pfIndex2Color)(int Index);
} GUI_BITMAP_METHODS;

typedef struct
{
    __u8                  bmType;
    __u8                  bmAlpha;
    __u32                 bmPitch;
    __u32                 colorkey;
    __u32                 XSize;
    __u32                 YSize;
    __u32                 BytesPerLine;
    __u32                 BitsPerPixel;
    __u8                   *pData;
    GUI_PALETTE            *pPal;
    GUI_BITMAP_METHODS     *pMethods;
} GUI_BITMAP;

/* This structure may not be changed because the data that it
   expects is read in binary form (via any kind of interface,
   at runtime).
   This structure should therefor not be changed.
*/
typedef struct
{
    __u16                 ID;           /* Version 1.00 => 100*/
    __u16                 Version;
    __u16                 XSize;
    __u16                 YSize;
    __u16                 BytesPerLine;
    __u16                 BitsPerPixel;
    __u16                 NumColors;
    __u16                 HasTrans;
} GUI_BITMAP_STREAM;

#endif

