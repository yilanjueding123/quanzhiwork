#ifndef __BSP_DI_H__
#define __BSP_DI_H__

typedef enum __DI_INMODE
{
    DI_NON_MB_UVCOMBINE = 0,
    DI_MB_UVCOMBINE,
} __di_inmode_t;

typedef enum __DI_INOUTFMT
{
    DI_INOUT_FORMAT_YUV444 = 0,
    DI_INOUT_FORMAT_YUV422,
    DI_INOUT_FORMAT_YUV420,
    DI_INOUT_FORMAT_YUV411,
} __di_inoutfmt_t;

typedef struct _DI_BUF_ADDR
{
    __u32   ch0_addr;   //
    __u32   ch1_addr;
    __u32   ch2_addr;
} __di_buf_addr_t;

typedef struct __DI_SRC_TYPE
{
    __u8    mod;       //	__di_inmode_t
    __u8    ps;        //
} __di_src_type_t;



typedef struct
{
    __u32 di_base;	/* di module reg base addr */
    void (*di_begin)(void);
    void (*di_finish)(void);
} __di_init_para_t;


//*********************************************************************************************
// function         : DI_Set_Reg_Base(__u32 base)
// description      : Set DIT register base
// parameters       :
//                 	  base  <register base>
// return           : 0
//***********************************************************************************************
extern __s32 DI_Set_Reg_Base(__u32 base);


//*********************************************************************************************
// function         : DI_Get_Reg_Base(void)
// description      : Get DIT register base
// parameters       :
// return           : reg base
//***********************************************************************************************
extern __u32 DI_Get_Reg_Base(void);

//*********************************************************************************************
// function         : DI_Init(__di_init_para_t* para)
// description      : Set DIT initial register
// parameters       :
// return           : 0
//***********************************************************************************************
extern __s32 DI_Init( __di_init_para_t *para);
//*********************************************************************************************
// function         : DI_unInit(void)
// description      : Set DIT uninitial register
// parameters       :
// return           : 0
//***********************************************************************************************
extern __s32 DI_unInit(void);

//*********************************************************************************************
// function         : DI_Set_Fb_Addr(__scal_buf_addr_t *cur_frame_buf, __scal_buf_addr_t *pre_frame_buf, __scal_buf_addr_t *out_frame_buf)
// description      : DIT Source buffer, Output buffer address setting
// parameters       :
//                 	  cur_frame_buf	<current frame source buffer start address>
//                 	  pre_frame_buf <pre-frame source buffer start address>
//					  out_frame_buf <de-interlaced output frame buffer start address>
// return           : 0
// note				: All source buffer address are source buffer, but not the display start address
//***********************************************************************************************
extern __s32 DI_Set_Fb_Addr(__di_buf_addr_t *cur_frame_buf, __di_buf_addr_t *pre_frame_buf, __di_buf_addr_t *out_frame_buf);


//*********************************************************************************************
// function         : DI_Set_Config_Src(__u32 src_width, __u32 src_height, __scal_src_type_t *type)
// description      : DIT Source framebuffer size and format setting
// parameters       :
//                 	  src_width	<source frame buffer width>
//                 	  src_height <source frame buffer height>
//					  type <source frame buffer format>
// return           : 0
// note				: Src_width must be 2 pixel aligned
//					  src_height must be 4 pixel aligned
//					  type->byte_seq must be 0;
//***********************************************************************************************
extern __s32 DI_Set_Config_Src(__u32 src_width, __u32 src_height, __di_src_type_t *type);


//*********************************************************************************************
// function         : DI_Get_Output_Info(__u32 *stride0, __u32 *stride1, __scal_src_type_t *type)
// description      : Get DIT output buffer
// parameters       :
// return           : 0
// note             : Size of output frame is the same as source one
//***********************************************************************************************
extern __s32 DI_Get_Output_Info(__u32 *stride0, __u32 *stride1, __di_src_type_t *type);


//*********************************************************************************************
// function         : DI_Start(void)
// description      : Start one frame de-interlacing
// parameters       :
// return           : 0
//***********************************************************************************************
extern __s32 DI_Start(void);


//*********************************************************************************************
// function         : DI_Get_Status(void)
// description      : Get DIT status
// parameters       :
// return           : 0---OK
//					  -1---FAIL
//***********************************************************************************************
extern __s32 DI_Get_Status(void);

extern __s32 DI_init_flag_addr(void);

//*********************************************************************************************
// function         : DI_deinit_flag_addr(void)
// description      :free memory size of flag address
// parameters       :
// return           : 0---OK
//					  -1---FAIL
//***********************************************************************************************
extern __s32 DI_deinit_flag_addr(void);

#endif
