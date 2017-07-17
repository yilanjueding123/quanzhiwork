
#ifndef __DRV_DI_H__
#define __DRV_DI_H__

typedef struct tag_DI_Source_Config_s
{
    __u32 src_width;
    __u32 src_height;
    __di_src_type_t type;
} DI_Source_Config_t;

typedef struct tag_DI_Fb_Addr_s
{
    __di_buf_addr_t cur_frame_buf;
    __di_buf_addr_t pre_frame_buf;
    __di_buf_addr_t out_frame_buf;
} DI_Fb_Addr_t;

typedef enum tag_DI_CMD
{
    DI_CMD_RESERVE0 = 0x00,
    DI_CMD_START = 0x01,
    DI_CMD_SET_FB_ADDR = 0x02,		/* pbuffer = DI_Fb_Addr_t*/
    DI_CMD_CONFIG_SRC = 0x03,		/* pbuffer = DI_Source_Config_t*/
    DI_CMD_GET_OUTPUT_INFO = 0x04,
} __di_cmd_t;


#endif

