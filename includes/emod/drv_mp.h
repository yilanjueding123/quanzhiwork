#ifndef __DRV_MP_H__
#define __DRV_MP_H__

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

typedef enum
{
    MP_CMD_REQUEST 		= 0x0,
    MP_CMD_RELEASE 		= 0x1,
    MP_CMD_COPY 		= 0x2,
    MP_CMD_FILL			= 0x3,
    MP_CMD_ALPHA		= 0x4,
    MP_CMD_ROTATE       = 0x5		//此命令目前无需调用MP_CMD_REQUEST，MP_CMD_RELEASE命令
} __mp_cmd_t;

#endif
