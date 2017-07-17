#ifndef __BSP_MP_H__
#define __BSP_MP_H__

#ifndef __BSP_DRV_MP_H__
#define __BSP_DRV_MP_H__

typedef enum
{
    MP_BLOCK_ALPHA	= 0,
    MP_BLOCK_COPY	= 1,
    MP_BLOCK_FILL	= 2,
} __mp_block_mode_t;

typedef enum
{
    MP_NONE			= 0,
    MP_PIXEL_ALPHA	= 1,
    MP_PLANE_ALPHA	= 2,
    MP_MULTI_ALPHA	= 3,
} __mp_flag_t;

typedef struct
{
    __s32 x;
    __s32 y;
    __u32 w;
    __u32 h;
} __mp_rect_t;

typedef struct
{
    __u32	addr[3];
    __u32	stride;
    __u32	h;
} __mp_imge_t;

typedef struct
{
    __mp_flag_t			flag;
    __mp_imge_t 		src;
    __mp_rect_t 		src_rect;
    __u32				seq;		//0:ARGB,1:RGBA
    __mp_imge_t 		dst;
    __s32				dx;
    __s32				dy;
    __u32				alpha_value;
} __mp_alpha_blend_t;

typedef enum
{
    MP_FMT_YUV_420,
    MP_FMT_YUV_411,
    MP_FMT_YUV_422,
    MP_FMT_ARGB
} __mp_format_t;

typedef enum
{
    MP_ANGLE_NONE,
    MP_ANGLE_RIGHT_90,
    MP_ANGLE_RIGHT_180,
    MP_ANGLE_RIGHT_270,
    MP_ANGLE_FLIP_HOR,
    MP_ANGLE_FLIP_VER
} __mp_angle_t;

/* format:
 * 0: 4:2:0
 * 1: 4:1:1
 * 2: 4:2:2
 * 3: ARGB
 *
 * rot_angle
 * 0: no rotation
 * 1: right 90 degree
 * 2: right 180 degree
 * 3: right 270 degree
 * 4: flip horizontal
 * 5: flip vertical
 */
typedef struct
{
    __u32				rot_src_y;
    __u32				rot_src_c;
    __u32				rot_dst_y;
    __u32				rot_dst_c;
    __u32				src_width;
    __u32				src_height;
    __mp_format_t		rot_format;
    __mp_angle_t		rot_angle;
} __mp_rotate_mirror_t;

typedef struct
{
    __mp_imge_t 		src;
    __mp_rect_t 		src_rect;
    __u32				seq;		//0:ARGB,1:RGBA
    __mp_imge_t 		dst;
    __s32				dx;
    __s32				dy;
} __mp_copy_t;

typedef struct
{
    __mp_rect_t 		dst_rect;
    __u32 				fill_color;
    __u32				seq;		//0:ARGB,1:RGBA
    __mp_imge_t 		dst;
} __mp_fill_t;

#endif

typedef struct
{
    __u32 mp_base;
    __u32 base_pioc;
    __u32 base_sdram;
    __u32 base_sram;
    __u32 base_ccmu;
    void (*mp_begin)(void);
    void (*mp_finish)(void);
} __mp_init_para_t;

__s32 Mp_Init(__mp_init_para_t *para);
__s32 Mp_Exit(void);
__s32 Mp_Request(void);
__s32 Mp_Release(void);
__s32 Mp_Rotate_Mirror(__mp_rotate_mirror_t *para);
__s32 Mp_Copy(__mp_copy_t *para);
__s32 Mp_Fill(__mp_fill_t *para);
__s32 Mp_Alpha_Blending(__mp_alpha_blend_t *para);

#endif
