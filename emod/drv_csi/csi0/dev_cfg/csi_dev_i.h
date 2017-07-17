#ifndef _CSI_DEV_I_H
#define _CSI_DEV_I_H

#include "csi_inc.h"
#include "csi_iic.h"

extern __bool              bProgressiveSrc;    // Indicating the source is progressive or not
extern __bool              bTopFieldFirst;     // VPO should check this flag when bProgressiveSrc is FALSE
extern __u16               eAspectRatio;       //the source picture aspect ratio

//extern __csi_conf_t conf;//only use in device file no need to declare

extern __u32 csi_mclk;

extern void csi_mclk_on_off(__bool para);
extern void csi_pwdn(__bool para);
extern void csi_rst(__bool para);
extern void csi_pwren(__bool para);

extern __csi_mode_t csi_mode;   								//符合用户设置的当前的csi_mode
extern __csi_mode_all_t csi_mode_all;   				//本摄像头设备所支持的所有的csi_mode

extern __csi_color_effect_t csi_color_effect;   //符合用户设置的当前的csi_color_effect
extern __csi_awb_t csi_awb;   									//符合用户设置的当前的csi_awb
extern __csi_ae_t csi_ae;   										//符合用户设置的当前的csi_ae
extern __csi_bright_t csi_bright;   						//符合用户设置的当前的csi_bright
extern __csi_contrast_t csi_contrast;   				//符合用户设置的当前的csi_contrast
extern __csi_band_t csi_band;   								//符合用户设置的当前的csi_band

extern __s32  csi_dev_init(void);
extern __s32  csi_dev_exit(void);
extern __s32  csi_dev_set_mode(__u32 mode);
extern __s32 csi_dev_set_color_effect(__csi_color_effect_t color_effect);
extern __s32 csi_dev_set_awb(__csi_awb_t awb);
extern __s32 csi_dev_set_ae(__csi_ae_t ae);
extern __s32 csi_dev_set_bright(__csi_bright_t bright);
extern __s32 csi_dev_set_contrast(__csi_contrast_t csi_contrast);
extern __s32 csi_dev_set_band(__csi_band_t csi_band);


#endif

