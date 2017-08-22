/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           home app sample
*
*						        	(c) Copyright 2006-2009, China
*											 All Rights Reserved
*
* File    : dv_uipara.h
* By      : lincoln
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __DV_FRMWIN_H__
#define __DV_FRMWIN_H__

#define APP_DV_HBAR
#define DV_FRM_SAVE_FREQ
#define MAX_FREQ_SAVE_VALUE  500


typedef struct tag_frmwin_para
{
    H_WIN				frmwin_hdl ;
    GUI_FONT 			*font;
    dv_state_e			cur_state;
    __s32				rec_status;
    __krnl_stmr_t  	*os_timer_hdl;
    __u8				delete_file_tsk ;
    __time_t			sys_time;
    __date_t			sys_date;
    __time_t			cur_rec_time;
    __lotus_overlay_dis_par_t   water_mark_para;
    reg_dv_para_t     *user_data;      // 用户存储数据
    __bool				recording_flash_flag ;	// 录像时图标更新
    H_LYR				frm_lyr_hdl ;
    dv_uipara_para_t 	*uipara ;
    H_WIN				h_parent ;
    H_LYR				dialoag_lyr_hdl ;
    __u32				txt_color ;
#ifdef	APP_DV_HBAR
	H_LYR				subset;
#endif
#ifdef APP_DV_SUPOORT_BREAK
	DV_APP_CONVERT		switch_frm;
#endif


} dv_frmwin_para_t;


typedef enum
{
    CMD_DV_FRM_POP_REC_SETTING_MENU,
    CMD_DV_FRM_POP_IMAGE_SETTING_MENU,
} dv_frm_to_mset_cmd_t;

typedef enum
{
    CMD_DV_FRM_SET_RECORD_RESOLUTION = GUI_MSG_USER_DEF,
    CMD_DV_FRM_SETTING_MENU_EXIT,
} dv_man_to_frm_cmd_t;

enum
{
    DV_LEVEL_NOSIGNAL = 2,
    DV_LEVEL_SIGNAL_1,
    DV_LEVEL_SIGNAL_2,
    DV_LEVEL_SIGNAL_3,
    DV_LEVEL_SIGNAL_4,
    DV_LEVEL_SIGNAL_5,
    DV_LEVEL_SIGNA_MAX
};

typedef struct
{
    __s32 		vol_level;			// 电压级别，供电池显示
    __bool      charge_sta;         //充电状态
    HTHEME res_handle;
    void *res_buffer;

} dv_headbar_data_t;

typedef struct
{
    __s32 		signal_level;			// 电压级别，供电池显示
    HTHEME res_handle;
    void *res_buffer;

} *dv_signal_data_t;


__s32 dv_frm_create(dv_frmwin_para_t *para);
__s32 dv_frm_destroy( H_WIN h_win );

extern __u32   record_file_index;
extern __u32   photo_file_index;

#define  ID_OP_DV_TO_HOME     0
#define 	LOOP_VIDEO_ONOFF

#endif /* __DV_FRMWIN_H__ */
