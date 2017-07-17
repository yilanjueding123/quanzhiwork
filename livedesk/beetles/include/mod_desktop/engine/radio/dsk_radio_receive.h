/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           LARK sample
*
*						        (c) Copyright 2006-2007, terry, China
*											All	Rights Reserved
*
* File    : dsk_radio_p.h
* By      : terry.zeng
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __DSK_RADIO_REC_I_H__
#define __DSK_RADIO_REC_I_H__

#include "apps.h"

#define	CHANNEL_NAME_LEN	128

typedef struct
{
    char name[CHANNEL_NAME_LEN];
    __u32 freq;
} dsk_radio_chan_t;

typedef enum
{
    PLAY_LIST_SEARCH_RESULT = 0,
    PLAY_LIST_FAVORITE
} __play_list_type_e;

extern __pCBK_t volatile  cb_search_success;
extern void    *volatile  cb_ctx_search_success;

extern __pCBK_t volatile  cb_search_fail;
extern void    *volatile  cb_ctx_search_fail;

extern __pCBK_t volatile  cb_search_over;
extern void *volatile  cb_ctx_search_over;

/*手动搜索时,波段为FM_APP_BAND_JAPAN的搜索步长*/
#define LARK_SEARCH_STEP_JAPAN 50
/*手动搜索时,波段为FM_APP_BAND_US_EUP的搜索步长*/
#define LARK_SEARCH_STEP_US 100
#define CHANNEL_SIZE (CHANNEL_NAME_LEN+14)


__s32 dsk_radio_rcv_open(void);

__bool dsk_radio_rcv_is_open(void);

__s32 dsk_radio_rcv_set_freq_play(__s32 freq);

__s32 dsk_radio_rcv_auto_search(void);

__s32 dsk_radio_rcv_get_cur_freq(__u32 *freq);

__s32 dsk_radio_rcv_set_search_mode(__u32 search_mode);

__s32 dsk_radio_set_manual_way(__u32 manual_way);//add by Kingvan

__s32 dsk_radio_set_automanual_mode(__u32 search_mode);

//__s32 dsk_radio_rcv_search_all(void);
__s32 dsk_radio_rcv_search_all(__u32 freq_cur, __s32 channel_cur);

__s32 dsk_radio_rcv_next_freq_play(void);

__s32 dsk_radio_rcv_pre_freq_play(void);

__s32 dsk_radio_rcv_set_play_list_type(__play_list_type_e play_list_type );

__s32 dsk_radio_rcv_get_play_list_type(__s32 *play_list_type);

__s32 dsk_radio_rcv_get_search_result_vn(void);

__s32 dsk_radio_rcv_get_favorite_vn(__s32 *valid_num);

__s32 dsk_radio_rcv_get_search_result_chaninfo(__s32 id, dsk_radio_chan_t *chan_info);

__s32 dsk_radio_rcv_get_favorite_chaninfo(__s32 id, dsk_radio_chan_t *chan_info);

__s32 dsk_radio_rcv_get_favorite(char *file_name);

__s32 dsk_radio_rcv_save_favorite(char *file_name);

//__s32 dsk_radio_rcv_get_search_result(char *file_name);
__s32 dsk_radio_rcv_get_search_result(reg_fm_para_t *para);

//__s32 dsk_radio_rcv_save_search_result(char *file_name);
__s32 dsk_radio_rcv_save_search_result(void);

__s32 dsk_radio_rcv_add_chan_to_favorite(__u32 id, dsk_radio_chan_t *chan_info);

__s32 dsk_radio_rcv_del_chan_from_favorite(__u32 id);

__s32 dsk_radio_rcv_close(void);


#endif




