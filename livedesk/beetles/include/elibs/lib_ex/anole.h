/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           anole app sample
*
*						        (c) Copyright 2006-2007, John, China
*											All	Rights Reserved
*
* File    : anole.h
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

//#include "epdk.h"

#ifndef __ANOLE_H__
#define __ANOLE_H__

typedef __willow_switch_mode_t __anole_switch_mode_t;

#define ANOLE_FLAG_RANDOM_MODE  0xff

/*work mode*/
typedef enum tag__ANOLE_MODE
{
    ANOLE_MODE_NORMAL, 		//still and no scale no zoom
    ANOLE_MODE_PLAY,     	//is playing
    ANOLE_MODE_ZOOM,  		//zoom
} __anole_sta_t;

typedef struct tag__ANOLE_THUMBS_FMT
{
    __u8			format;						    /* thumbs format                        */
    __rectsz_t 		size;
} __anole_thumbs_fmt_t;


typedef enum __TAG_PLAYORDER
{
    ANOLE_PLAY_NEXT,
    ANOLE_PLAY_PREV,
    ANOLE_PLAY_RANDOM
} __anole_playorder_t;

typedef enum __tag_anole_layer_sta
{
    ANOLE_LAYER_TOP,
    ANOLE_LAYER_BOTTOM,
    ANOLE_LAYER_SHOW,
    ANOLE_LAYER_HIDE
} __anole_layer_sta;

//#define ANOLE_RANDOM_SWITCH (EXTENT_SWITCH5 + 1)

// typedef struct tag__ANOLE_PIC_INFO
// {
//    	char	*name;
// 	__u32	file_len;
//
// 	SIZE    size;
// 	__u8	format;
// 	__u8	color_depth;
// 	__u32	aperture;
// 	__u32	shutter;
// }__anole_pic_info_t;
typedef __willow_img_info_t __anole_pic_info_t;

typedef struct tag__ANOLE_PARA
{
    __anole_playorder_t 		PlayOrder;
    __anole_switch_mode_t 		SwitchMode;		/*switch mode*/
    __u32 						interval;
    __s32 						*switchmodearray;
    __s32 						switchnum;
} __anole_para_t;

typedef enum tag_anole_event_e
{
    ANOLE_EVENT_SHOW_PIC_FAIL,
    ANOLE_EVENT_SHOW_PIC_NEXT,
    ANOLE_EVENT_
} anole_event_e;

typedef enum
{
    ANOLE_FAST = 2,
    ANOLE_MIDDLE = 5,
    ANOLE_SLOW = 10,
    ANOLE_CUSTOM = 20,
} __anole_interval_t;

#define ANOLE_MAX_INTERVAL         15
#define ANOLE_MIN_INTERVAL         1

typedef enum
{
    ANOLE_MOVE_UP = 0,
    ANOLE_MOVE_DOWN,
    ANOLE_MOVE_RIGHT,
    ANOLE_MOVE_LEFT,
    ANOLE_MOVE_NUM
} __anole_diretion_t;

/*willow open and close*/
__s32                  	anole_init(/*__anole_para_t *para*/void);
void                   	anole_exit(void);
__pCBK_t 				anole_set_cb( anole_event_e event, __pCBK_t cb , void *ctx);
__pCBK_t 				anole_get_cb( anole_event_e event );

//npl
__s32  					anole_npl_playing_seek(  __s32 index );
__s32  					anole_npl_get_next( void );
__s32  					anole_npl_get_cur( void );
__s32  					anole_npl_file2index( const char *file );
__s32  					anole_npl_index2file( __u32 index, char *file );
__s32  					anole_npl_get_total_count( void );

//play file
__s32 					anole_play_file_by_index(__s32 index); //add for random effect
__s32 					anole_play_file(char *filename);
__bool 				   	anole_play_prev(void);
__bool				   	anole_play_next(void);


/*control*/
__anole_sta_t 		    anole_get_sta(void);
__bool				    anole_backnormal(void);
__bool 				    anole_auto_play(void);
__bool  			    anole_rotate_left(void);
__bool				    anole_rotate_right(void);
__bool				    anole_zoom_inc(void);
__bool				    anole_zoom_dec(void);
__bool                  anole_zoom_set(__s32 level);
__s32                   anole_zoom_get(void);
__bool				    anole_show_pic_from_buffer(__u8 *buffer, __s32 width, __s32 height);

/*config*/
__s32 				    anole_set_scn_rect        (RECT                  *rect);
__s32 				    anole_set_play_interval   (__u32 sec);
__s32 				    anole_set_switch_mode     (__anole_switch_mode_t  mode);
__s32 					anole_set_switch_mode_array_for_play(__s32 *switch_mode_array, __s32 array_num);
__s32 				    anole_set_play_order      (__anole_playorder_t    order);
__bool 				    anole_get_scn_rect        (RECT *rect);
__u32                   anole_get_play_interval   (void);
__anole_switch_mode_t   anole_get_switch_mode     (void);
__anole_playorder_t     anole_get_play_order      (void);

/*thumb nail*/
__s32				    anole_get_thumb          (char *filename, void *buf, __anole_thumbs_fmt_t *param);

/*pic info*/
__s32				    anole_get_pic_info        (__anole_pic_info_t *info);

/*back pic capture */
__s32 				    anole_set_bg_pic(FB *fb);

//slider
//__s32 				    anole_slider_init(void);
__s32 					anole_slider_init(FB *decoding_fb, FB *unsupported_fb);
__s32 				    anole_slider_start(__s16 x, __s16 y);
__s32 				    anole_slider_move(__s16 x, __s16 y);
__s32 				    anole_slider_finish(__s16 x, __s16 y);
//__s32 				    anole_slider_exit(void);

__s32 				    anole_set_layer_sta(__anole_layer_sta layer_sta);

__bool                  anole_set_show_mode(__s32 index);

__bool                  anole_scale_move(__anole_diretion_t way);

#endif /* __ANOLE_H__ */

