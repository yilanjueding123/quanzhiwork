/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Desktop Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : desktop_api.c
* By      : kevin.z
* Version : v1.0
* Date    : 2010-9-14 20:26
* Descript:
* Update  : date                auther      ver     notes
*********************************************************************************************************
*/

#include "mod_desktop_i.h"

#pragma arm section rodata="DESKTOP_API_TBL"

const desktop_api_entry_t desktop_api =
{
    //===============================================================//
    //===============						reg							============//
    //===============================================================//
    dsk_reg_init_para					,
    dsk_reg_deinit_para					,
    dsk_reg_flush						,
    dsk_reg_get_para_by_app				,
    dsk_reg_get_default_para_by_app		,
    dsk_reg_get_app_restore_flag		,
    dsk_reg_set_app_restore_flag		,
    dsk_reg_save_cur_play_info			,
    dsk_reg_set_app_restore_all			,
    NULL								,
    NULL								,
    NULL								,

    //===============================================================//
    //==================					functions					============//
    //===============================================================//
    //dsk_display_gamma.h
    /*dsk_set_gamma						,
    dsk_gamma_on						,
    dsk_gamma_off						,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,*/
    //dsk_display_lcd.h
    dsk_display_lcd_on					,
    dsk_display_lcd_off					,
    dsk_display_set_lcd_bright			,
    dsk_display_get_lcd_bright			,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    //dsk_display_misc.h
    dsk_display_hdmi_audio_enable		,
    dsk_display_get_size				,
    dsk_display_set_backcolor			,
    dsk_display_get_backcolor			,
	dsk_set_contrast					,
	dsk_set_bright						,
	dsk_set_saturation					,
	dsk_set_hue							,

    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    //dsk_display_output.h
    dsk_display_get_format				,
    dsk_display_set_format				,
    dsk_display_get_output_type			,
    dsk_display_dskfmt2epdkfmt			,
    dsk_display_epdkfmt2dskfmt			,
    dsk_display_on,
    dsk_display_off,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    //dsk_display_enhance.h
    /*dsk_display_enhance_on				,
    dsk_display_enhance_off				,
    dsk_display_set_bright				,
    dsk_display_get_bright				,
    dsk_display_set_contrast			,
    dsk_display_get_contrast			,
    dsk_display_set_saturation			,
    dsk_display_get_saturation			,*/
    //dsk_fw_update.h
    dsk_fw_update						,
    dsk_start_fw_update					,
    NULL								,
    NULL								,
    NULL								,
    NULL								,

    //dsk_keytone.h
    dsk_keytone_init					,
    dsk_keytone_exit					,
    dsk_keytone_on						,
    dsk_keytone_set_state				,
    dsk_keytone_get_state				,
    dsk_keytone_set_key_type            ,
    dsk_keytone_get_key_type    		,
    NULL								,
    NULL								,
    NULL								,

    //dsk_usbh.h
    dsk_send_insmod_usbh_cmd			,
    dsk_send_rmmod_usbh_cmd				,
    dsk_usbh_is_working					,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,

    //dsk_audio_if.h
    dsk_set_audio_if					,
    dsk_get_audio_if					,
    dsk_set_hdmi_audio_gate				,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,

    //dsk_volume.h
    dsk_volume_set						,
    dsk_volume_get						,
    dsk_volume_inc						,
    dsk_volume_dec						,
    dsk_volume_off						,
    dsk_volume_on						,
    dsk_volume_is_on						,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,

    //dsk_charset.h
    dsk_get_langres_charset				,
    dsk_set_fs_charset					,
    dsk_get_fs_charset					,
    dsk_set_isn_charset					,
    dsk_get_isn_charset					,

    //dsk_langres.h
    dsk_langres_init					,
    dsk_langres_exit					,
    dsk_langres_set_type				,
    dsk_langres_get_type				,
    dsk_langres_get_menu_len			,
    dsk_langres_get_menu_text			,

    //dsk_theme.h
    dsk_theme_init						,
    dsk_theme_exit						,
    dsk_theme_set_style					,
    dsk_theme_get_style					,
    dsk_theme_open						,
    dsk_theme_close						,
    dsk_theme_hdl2size					,
    dsk_theme_hdl2buf					,

    // ./dsk_sys/dsk_orchid.h
    dsk_orchid_check					,
    dsk_orchid_createDisk				,
    dsk_orchid_save_npl					,
    dsk_orchid_load_last_npl			,
    dsk_orchid_update					,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,

    //./dsk_sys/dsk_power.h
    dsk_power_get_battery_level			,
    dsk_power_get_voltage_level			,
    dsk_power_get_battary_state			,
    dsk_power_set_off_time				,
    dsk_power_is_low					,
    dsk_speaker_turnoff					,
    dsk_speaker_turnon					,
    dsk_speaker_resume					,
    //dsk_speak_init						,
    //dsk_speak_exit						,
    dsk_get_charge_state				,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,

    // ./dsk_sys/dsk_usbd.h
    dsk_usbd_remove						,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    //dsk_auto_off.h
    dsk_set_auto_off_time				,
    dsk_power_off						,
    //===============================================================//
    //==================					framework					============//
    //===============================================================//
    //framework/activity.h
    //activity_create                    	,
    activity_set_attr                  	,
    activity_get_attr                  	,
    activity_set_load_para				,
    activity_get_load_para				,
    activity_load_file                 	,
    activity_load_app                  	,
    activity_get_root_win              	,
    activity_background_top             ,
    activity_return_home               	,
    activity_finish                    	,
    //activity_kill_backgrd_except_home	,
    //activity_kill_all_backgrd			,
    activity_finish_all					,
    activity_suspend_top				,
    activity_resume_top					,
    activity_notify_top					,
    activity_all_able_standby			,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,

    //framework/applet_info_manager.h
    applet_info_manager_create         	,
    applet_info_manager_load_file      	,
    applet_info_manager_load_dir       	,
    applet_info_manager_get_count      	,
    applet_info_manager_get            	,
    applet_info_manager_printf			,
    applet_info_manager_destroy        	,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,

    //framework/activity_manager.h
    activity_manager_create				,
    activity_manager_top				,
    activity_manager_push				,
    activity_manager_pop				,
    activity_manager_top_to_backgrd			,
    activity_manager_add_backgrd			,
    activity_manager_delete_backgrd			,
    activity_manager_backgrd_to_top			,
    activity_manager_foreach			,
    activity_manager_foreach_suspend	,
    activity_manager_foreach_backgrd		,
    activity_manager_destroy			,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,

    //framework/globals.h
    activity_manager_get_default		,
    applet_info_manager_get_default 	,
    message_loop_win_get_default		,
    activity_manager_set_default		,
    applet_info_manager_set_defualt		,
    message_loop_win_set_default		,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,

    //msg_srv/msg_emit.h
    msg_emit_init						,
    msg_emit_deinit						,
    msg_emit_register_hook				,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,
    NULL								,

    //===============================================================//
    //==================					engine					============//
    //===============================================================//
    //====================			walkman		=======================//
    //dsk_walkman.h
    dsk_wkm_init						,
    dsk_wkm_exit						,
    dsk_wkm_open						,
    dsk_wkm_close						,
    dsk_wkm_is_open						,
    dsk_wkm_set_cb						,
    dsk_wkm_get_cb						,
    dsk_wkm_set_attr 					,
    dsk_wkm_get_attr					,
    //dsk_wkm_ab.h
    dsk_wkm_set_ab_a					,
    dsk_wkm_set_ab_b					,
    dsk_wkm_set_ab_loop_count			,
    dsk_wkm_cancle_ab					,
    //dsk_wkm_star.h
    dsk_wkm_set_star					,
    dsk_wkm_get_star					,
    //dsk_wkm_play_speed.h
    dsk_wkm_set_play_speed				,
    dsk_wkm_get_play_speed				,
    //dsk_wkm_lyric.h
    dsk_wkm_lyric_exist					,
    dsk_wkm_get_lyric_format			,
    dsk_wkm_get_lyric_item_count		,
    dsk_wkm_get_lyric_item_list			,
    //dsk_wkm_npl.h
    dsk_wkm_npl_add_item				,
    dsk_wkm_npl_delete_item				,
    dsk_wkm_npl_clear					,
    dsk_wkm_npl_set_cur					,
    dsk_wkm_npl_get_cur					,
    dsk_wkm_npl_get_next				,
    dsk_wkm_npl_file2index				,
    dsk_wkm_npl_index2file				,
    dsk_wkm_npl_get_total_count			,
    dsk_wkm_npl_save_list				,
    dsk_wkm_npl_load_last_list			,
    //dsk_wkm_play_mode.h
    dsk_wkm_set_play_mode				,
    dsk_wkm_get_play_mode				,
    //dsk_wkm_eq.h
    dsk_wkm_set_eq_mode					,
    dsk_wkm_get_eq_mode					,
    dsk_wkm_set_eq_value				,
    dsk_wkm_get_eq_value				,
    dsk_wkm_eq_mode2value				,
    //dsk_wkm_media_info.h
    dsk_wkm_get_now_playing_file		,
    dsk_wkm_get_tag						,
    dsk_wkm_get_cur_time				,
    dsk_wkm_get_total_time				,
    dsk_wkm_get_audio_encoding			,
    dsk_wkm_get_audio_bps				,
    dsk_wkm_get_audio_sample_rate		,
    dsk_wkm_get_audio_info				,
    dsk_wkm_get_album_cover				,
    //dsk_wkm_ff_rr_speed.h
    dsk_wkm_set_ff_rr_speed				,
    dsk_wkm_get_ff_rr_speed				,
    //dsk_wkm_spectrum.h
    dsk_wkm_enable_spectrum				,
    dsk_wkm_disable_spectrum			,
    dsk_wkm_get_spectrum_info			,
    //dsk_wkm_fsm_ctrl.h
    dsk_wkm_play_file					,
    dsk_wkm_play_next 					,
    dsk_wkm_play_prev					,
    dsk_wkm_stop						,
    dsk_wkm_pause						,
    dsk_wkm_resume_play					,
    dsk_wkm_ff							,
    dsk_wkm_rr							,
    dsk_wkm_jump						,
    dsk_wkm_get_fsm_state				,
    dsk_wkm_is_playing					,
    dsk_wkm_get_pause_state             ,
    dsk_wkm_clear_feedback_msgQ			,
    dsk_wkm_get_feedback_msg			,
    //dsk_wkm_monitor_gate.h
    dsk_wkm_monitor_enable              ,
    dsk_wkm_monitor_disable             ,
    //====================			radio		=======================//
    //dsk_radio.h
    dsk_radio_open						,
    dsk_radio_is_open					,
    dsk_radio_set_cb					,
    dsk_radio_get_cb					,
    dsk_radio_mute   	                        ,
    dsk_radio_unmute  	                        ,
    dsk_radio_set_band	                        ,
    dsk_radio_set_threshold	                    ,
    dsk_radio_set_audio_mode	                ,
    dsk_radio_get_band	                        ,
    dsk_radio_get_audio_mode	                ,
    dsk_radio_get_min_freq	                    ,
    dsk_radio_get_max_freq                      ,
    dsk_radio_close						        ,
    //dsk_radio_receive.h
    dsk_radio_rcv_open                          ,
    dsk_radio_rcv_close                         ,
    dsk_radio_rcv_is_open						,
    dsk_radio_rcv_set_freq_play,
    dsk_radio_rcv_auto_search,
    dsk_radio_rcv_get_cur_freq,
    dsk_radio_rcv_set_search_mode,
    dsk_radio_set_manual_way,//add by Kingvan
    dsk_radio_set_automanual_mode,
    dsk_radio_rcv_search_all,
    dsk_radio_rcv_next_freq_play,
    dsk_radio_rcv_pre_freq_play,
    dsk_radio_rcv_set_play_list_type,
    dsk_radio_rcv_get_play_list_type,
    dsk_radio_rcv_get_search_result_vn,
    dsk_radio_rcv_get_favorite_vn,
    dsk_radio_rcv_get_search_result_chaninfo,
    dsk_radio_rcv_get_favorite_chaninfo,
    dsk_radio_rcv_get_favorite,
    dsk_radio_rcv_save_favorite,
    dsk_radio_rcv_get_search_result,
    dsk_radio_rcv_save_search_result,
    dsk_radio_rcv_add_chan_to_favorite,
    dsk_radio_rcv_del_chan_from_favorite,
    dsk_radio_send_open                         ,
    dsk_radio_send_close		                ,
    dsk_radio_send_is_open						,
    dsk_radio_send_set_freq_play	            ,
    dsk_radio_send_get_cur_freq	                ,
    dsk_radio_send_set_gain		                ,
    dsk_radio_send_get_gain		                ,
};

#pragma arm section



