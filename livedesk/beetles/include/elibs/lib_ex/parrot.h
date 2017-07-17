/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           home app sample
*
*						        	(c) Copyright 2006-2009, LYN, China
*											 All Rights Reserved
*
* File    : parrot.h
* By      : lyn
* Version : V1.00
*********************************************************************************************************
*/
#ifndef __PARROT_H__
#define __PARROT_H__

__s32   parrot_init(void);

__s32   parrot_uninit(void);

__hdle	parrot_open(__s32 index);

__s32   parrot_close(__hdle hdle);

__s32   parrot_get_csi_mode(__hdle hdle, __csi_mode_all_t *mode);

__s32   parrot_set_mode(__hdle hdle, __lotus_record_mode_t mode);

__s32   parrot_set_media_file(__hdle hdle, __lotus_media_file_inf *info);

__s32   parrot_set_record_thumb(__hdle hdle, FB *fb);

__s32   parrot_set_preview_mode(__hdle hdle, __csi_mode_t *csi_mode);

__s32   parrot_start_preview(__hdle hdle);

__s32   parrot_stop_preview(__hdle hdle);

__s32   parrot_start_record(__hdle hdle);

__s32   parrot_halt_record(__hdle hdle);

__s32   parrot_stop_record(__hdle hdle);

__s32   parrot_get_record_time(__hdle hdle);

__s32   parrot_take_picture(__hdle hdle, __u32 cnt, FB *fb);

__s32   parrot_set_md_gate(__hdle hdle, __s32 enable);

__s32   parrot_set_md_para(__hdle hdle, __lotus_motion_dectect_para_t *para);

__s32   parrot_get_md_result(__hdle hdle);


__s32   parrot_request_preview_layer(RECT rect, __s32 pipe, __s32 prio);

__s32   parrot_release_preview_layer(__s32  hlayer);

__s32   parrot_set_show_layer(__hdle hdle, __s32 hlayer);

__s32   parrot_set_show_mode(__hdle hdle, __lotus_vid_win_ratio_mode_t mode);

__s32   parrot_set_show_rect(__hdle hdle, RECT rect);

__s32   parrot_set_ce(__hdle hdle, __csi_color_effect_t ce);

__s32   parrot_set_awb(__hdle hdle, __csi_awb_t awb);

__s32   parrot_set_ae(__hdle hdle, __csi_ae_t ae);

__s32   parrot_set_band(__hdle hdle, __csi_band_t band);

__csi_color_effect_t   parrot_get_ce(__hdle hdle);

__csi_awb_t   parrot_get_awb(__hdle hdle);

__csi_ae_t   parrot_get_ae(__hdle hdle);

__csi_band_t   parrot_get_band(__hdle hdle);

__s32   parrot_add_overlay_src(__hdle hdle, __lotus_overlay_src_init_t *src);

__s32   parrot_show_overlay_blk(__hdle hdle, __lotus_overlay_dis_par_t *para);

__s32   parrot_set_hit_holding_time(__hdle hdle, __s32 time);

__s32   parrot_seek_hit_seek_time(__hdle hdle, __s32 time);

__s32	parrot_set_record_mute(__hdle hdle, __s32 state);

__s32   parrot_set_output_mode(__hdle hdle, __lotus_muxer_type_t type);

__s32   parrot_set_sync_callback(__hdle hdle, __lotus_sync_callback_t  *sync_callback);

__s32   parrot_halt_audio_record(__hdle hdle);

__s32   parrot_continue_audio_record(__hdle hdle);

#endif  /* __PARROT_H__ */

