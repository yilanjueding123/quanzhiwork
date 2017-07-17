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

#include "csi.h"

#define W(offset,value)          (*((volatile __u32 *)(offset))=(value))    //
#define R(offset)                (*((volatile __u32 *)(offset)))            //
#define S(offset,bit)            (*((volatile __u32 *)(offset)) |=(bit))    //
#define C(offset,bit)            (*((volatile __u32 *)(offset)) &=(~(bit)))   //

static __u32 csi0_reg_base_pa = 0x01cb0000;
static __u32 csi0_reg_base_va;

__u32 CSIC_REGS_BASE      ;
__u32 CSI_REG_EN          ;
__u32 CSI_REG_CONF        ;
__u32 CSI_REG_CTRL        ;
__u32 CSI_REG_SCALE       ;

__u32 CSI_REG_BUF_0_A     ;//>>----common part for 4CH TDM
__u32 CSI_REG_BUF_0_B     ;
__u32 CSI_REG_BUF_1_A     ;
__u32 CSI_REG_BUF_1_B     ;
__u32 CSI_REG_BUF_2_A     ;
__u32 CSI_REG_BUF_2_B     ;
__u32 CSI_REG_BUF_CTRL    ;
__u32 CSI_REG_STATUS      ;
__u32 CSI_REG_INT_EN      ;
__u32 CSI_REG_INT_STATUS  ;
__u32 CSI_REG_RESIZE_H    ;
__u32 CSI_REG_RESIZE_V    ;
__u32 CSI_REG_BUF_LENGTH  ;//common part for 4CH TDM----<<

//__u32 CSI_REG_BUF_0_A_CH1     ;
//__u32 CSI_REG_BUF_0_B_CH1     ;
//__u32 CSI_REG_BUF_1_A_CH1     ;
//__u32 CSI_REG_BUF_1_B_CH1     ;
//__u32 CSI_REG_BUF_2_A_CH1     ;
//__u32 CSI_REG_BUF_2_B_CH1     ;
//__u32 CSI_REG_BUF_CTRL    ;
//__u32 CSI_REG_STATUS      ;
//__u32 CSI_REG_INT_EN      ;
//__u32 CSI_REG_INT_STATUS  ;
//__u32 CSI_REG_RESIZE_H    ;
//__u32 CSI_REG_RESIZE_V    ;
//__u32 CSI_REG_BUF_LENGTH  ;



__u32 CSI_4CH_TDM_REG_STEP = 0x100;//only for CSI0 interface


/* set base module */
//////////////////////////////////////////////////////////////////////////////
void CSI_set_base(__u32 sel)
{
    __u32 ch_num = sel & 0x3f;
    CSIC_REGS_BASE = csi0_reg_base_va;

    ch_num = 0 ;

    CSI_REG_EN           = (CSIC_REGS_BASE + 0x00);
    CSI_REG_CONF         = (CSIC_REGS_BASE + 0x04);
    CSI_REG_CTRL         = (CSIC_REGS_BASE + 0x08);
    CSI_REG_SCALE        = (CSIC_REGS_BASE + 0x0C);
    CSI_REG_BUF_0_A      = (CSIC_REGS_BASE + ch_num * CSI_4CH_TDM_REG_STEP + 0x10);
    CSI_REG_BUF_0_B      = (CSIC_REGS_BASE + ch_num * CSI_4CH_TDM_REG_STEP + 0x14);
    CSI_REG_BUF_1_A      = (CSIC_REGS_BASE + ch_num * CSI_4CH_TDM_REG_STEP + 0x18);
    CSI_REG_BUF_1_B      = (CSIC_REGS_BASE + ch_num * CSI_4CH_TDM_REG_STEP + 0x1C);
    CSI_REG_BUF_2_A      = (CSIC_REGS_BASE + ch_num * CSI_4CH_TDM_REG_STEP + 0x20);
    CSI_REG_BUF_2_B      = (CSIC_REGS_BASE + ch_num * CSI_4CH_TDM_REG_STEP + 0x24);
    CSI_REG_BUF_CTRL     = (CSIC_REGS_BASE + ch_num * CSI_4CH_TDM_REG_STEP + 0x28);
    CSI_REG_STATUS       = (CSIC_REGS_BASE + ch_num * CSI_4CH_TDM_REG_STEP + 0x2C);
    CSI_REG_INT_EN       = (CSIC_REGS_BASE + ch_num * CSI_4CH_TDM_REG_STEP + 0x30);
    CSI_REG_INT_STATUS   = (CSIC_REGS_BASE + ch_num * CSI_4CH_TDM_REG_STEP + 0x34);
    CSI_REG_RESIZE_H     = (CSIC_REGS_BASE + ch_num * CSI_4CH_TDM_REG_STEP + 0x40);
    CSI_REG_RESIZE_V     = (CSIC_REGS_BASE + ch_num * CSI_4CH_TDM_REG_STEP + 0x44);
    CSI_REG_BUF_LENGTH   = (CSIC_REGS_BASE + ch_num * CSI_4CH_TDM_REG_STEP + 0x48);
}
/* init module */
////////////////////////////////////////////////////////////////////////////////
__s32  CSI_init(void)
{
    csi0_reg_base_va = csi_pa2va(csi0_reg_base_pa, 1); //transfer PA to VA
    return EPDK_OK;
}

__s32  CSI_exit(void)
{
    return EPDK_OK;
}

/* open module */
////////////////////////////////////////////////////////////////////////////////
__s32  CSI_open(void)
{
    S(CSI_REG_EN, 0X1 << 0);

    return EPDK_OK;
}

__s32  CSI_close(void)
{
    C(CSI_REG_EN, 0X1 << 0);

    return EPDK_OK;
}

/* configure */
////////////////////////////////////////////////////////////////////////////////
__s32  CSI_configure(__csi_conf_t *mode)
{
    W(CSI_REG_CONF, mode->input_fmt << 20 | /* [21:20] */
      mode->output_fmt << 16 | /* [18:16] */
      mode->field_sel << 10 | /* [11:10] */
      mode->seq       << 8  | /* [9:8] */
      mode->vref      << 2  | /* [2] */
      mode->href      << 1  | /* [1] */
      mode->clock     << 0   /* [0] */
     );
    return EPDK_OK;
}

/* buffer */
////////////////////////////////////////////////////////////////////////////////
__s32  CSI_set_buffer_address(__csi_buf_t buf, __u32 addr)
{
    W(CSI_REG_BUF_0_A + (buf << 2), addr); /* bufer0a +4 = buffer0b, bufer0a +8 = buffer1a, ...  */
    return EPDK_OK;
}

__u32  CSI_get_buffer_address(__csi_buf_t buf)
{
    __u32 t;
    t = R(CSI_REG_BUF_0_A + (buf << 2));
    return t;
}

__s32  CSI_double_buffer_enable(void)
{
    S(CSI_REG_BUF_CTRL, 0X1 << 0);

    return EPDK_OK;
}

__s32  CSI_double_buffer_disable(void)
{
    C(CSI_REG_BUF_CTRL, 0X1 << 0);

    return EPDK_OK;
}

__s32  CSI_double_buffer_select_next(__csi_double_buf_t type)
{
    if (CSI_BUF_A == type)
        C(CSI_REG_BUF_CTRL, 0x1 << 2); /* [2] */
    else
        S(CSI_REG_BUF_CTRL, 0x1 << 2);
    return EPDK_OK;
}

__s32  CSI_double_buffer_get_status(__csi_double_buf_status_t *status)
{
    __u32 t;
    t = R(CSI_REG_BUF_CTRL);
    status->enable  = t & 0x1;
    status->current = (__csi_double_buf_t)(t & (0x1 << 1));
    status->next    = (__csi_double_buf_t)(t & (0x1 << 2));

    return EPDK_OK;
}

/* capture */
////////////////////////////////////////////////////////////////////////////////
__s32  CSI_capture_video_start(void)
{
    S(CSI_REG_CTRL, 0X1 << 1); /* [1] */

    return EPDK_OK;
}

__s32  CSI_capture_video_stop(void)
{
    C(CSI_REG_CTRL, 0X1 << 1); /* [1] */

    return EPDK_OK;
}

__s32  CSI_capture_picture(void)
{
    S(CSI_REG_CTRL, 0X1 << 0); /* [0] */

    return EPDK_OK;
}

__u32  CSI_capture_get_status(__csi_capture_status *status)
{
    __u32 t;
    t = R(CSI_REG_STATUS);
    status->picture_in_progress = t & 0x1;
    status->video_in_progress = (t >> 1) & 0x1;

    return EPDK_OK;
}

/* size */
////////////////////////////////////////////////////////////////////////////////
__s32  CSI_set_size(__u32 start_h, __u32 length_h, __u32 start_v, __u32 length_v, __u32 buf_length_h)
{
    /* make sure yuv422 input 2 byte(clock) output 1 pixel */
    W(CSI_REG_RESIZE_H, (length_h << 16) + start_h);
    W(CSI_REG_RESIZE_V, (length_v << 16) + start_v);
    W(CSI_REG_BUF_LENGTH, buf_length_h);

    return EPDK_OK;
}

/* scale down */
////////////////////////////////////////////////////////////////////////////////
__s32  CSI_scale_down(__csi_scale_down_t scale_h, __csi_scale_down_t scale_v)
{
    __u32 t = 0;

    if (scale_h == CSI_ORIGIN)
        t = 0xf << 24;
    if (scale_h == CSI_HALF)
        t = 0x5 << 24;
    if (scale_h == CSI_QUARTER)
        t = 0x1 << 24;

    if (scale_v == CSI_ORIGIN)
        t += 0xffff;
    if (scale_v == CSI_HALF)
        t += 0x0f0f;
    if (scale_v == CSI_QUARTER)
        t += 0x000f;

    W(CSI_REG_SCALE, t);

    return EPDK_OK;
}

/* interrupt */
////////////////////////////////////////////////////////////////////////////////
__s32  CSI_int_enable(__csi_int_t interrupt)
{
    S(CSI_REG_INT_EN, interrupt);

    return EPDK_OK;
}

__s32  CSI_int_disable(__csi_int_t interrupt)
{
    C(CSI_REG_INT_EN, interrupt);

    return EPDK_OK;
}

__s32  CSI_int_get_status(__csi_int_status_t *status)
{
    __u32 t;
    t = R(CSI_REG_INT_STATUS);
    status->capture_done     = t & CSI_INT_CAPTURE_DONE     ;
    status->frame_done       = t & CSI_INT_FRAME_DONE       ;
    status->buf_0_overflow   = t & CSI_INT_BUF_0_OVERFLOW   ;
    status->buf_1_overflow   = t & CSI_INT_BUF_1_OVERFLOW   ;
    status->buf_2_overflow   = t & CSI_INT_BUF_2_OVERFLOW   ;
    status->protection_error = t & CSI_INT_PROTECTION_ERROR ;
    status->hblank_overflow  = t & CSI_INT_HBLANK_OVERFLOW  ;
    status->vsync_trig			 = t & CSI_INT_VSYNC_TRIG				;

    return EPDK_OK;
}


__s32  CSI_int_clear_status(void)
{
    W(CSI_REG_INT_STATUS, 0xffffffff);

    return EPDK_OK;
}


