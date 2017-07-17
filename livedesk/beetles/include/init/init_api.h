#ifndef __MOD_INIT_API_H__
#define __MOD_INIT_API_H__

typedef struct INIT_API_ENTRY
{
    /* gscene_backgrd.h */
    __s32 		 			(*ptr_gscene_bgd_init)				(SIZE *p_size, bgd_status_t status, __fb_type_t ftype);
    void 		 			(*ptr_gscene_bgd_set_state)			(bgd_status_t status);
    bgd_status_t 			(*ptr_gscene_bgd_get_state)			(void);
    void 		 			(*ptr_gscene_bgd_set_top)			(void);
    void 		 			(*ptr_gscene_bgd_set_bottom)		(void);
    __s32 		 			(*ptr_gscene_bgd_save_fb)			(FB *fb, char *path);
    void 		 			(*ptr_gscene_bgd_set_fb_type)		(__fb_type_t ftype);
    void 		 			(*ptr_gscene_bgd_refresh)			(void);
    void 		 			(*ptr_gscene_bgd_restore)			(void);
    __s32 		 			(*ptr_gscene_bgd_deinit)			(void);
    __bool                  (*ptr_gscene_bgd_get_flag)          (void);
    void                    (*ptr_gscene_bgd_set_flag)          (__bool flag);
    __s32                   (*ptr_gscene_bgd_get_default_bg_index)(void);
    __s32                   (*ptr_gscene_bgd_set_default_bg_index)(__u32 index);
    void 					(*ptr_gscene_bgd_reserv[6])		(void);


    /* gscene_headbar.h */
    __s32 					(*ptr_gscene_hbar_create)			(H_WIN parent, __hbar_format_t format);
    __s32 					(*ptr_gscene_hbar_set_state)		(HBarState state);
    __s32 					(*ptr_gscene_hbar_get_state)		(HBarState *p_state);
    __s32 					(*ptr_gscene_hbar_set_title)		(char *title, __u32 len);	/* title Îªutf8±àÂë¸ñÊ½×Ö·û´®£¬len<=32 ,ÉèÖÃhbar title ÇøÓò×Ö·û´®*/
    __s32 					(*ptr_gscene_hbar_set_format)		(__hbar_format_t format);
    __s32 					(*ptr_gscene_hbar_get_screen_rect)	(RECT *rect);
    __s32 					(*ptr_gscene_hbar_delete)			(void);
    void				    (*ptr_headbar_tf_cardstate_set)	   (__u8 state);
    __u8 				    (*ptr_headbar_tf_cardstate_get)	 (void);

    __s32 					(*ptr_gscene_hbar_reserv[8])		(void);


    /*headbar_com.h*/


    //display\g_display_switch_output.h
    __s32 					(*ptr_g_display_switch_output)		(__lion_disp_format_e mode);
    void 					(*ptr_g_display_switch_output_reserved[5])		(void);

    //display\g_close_screen.h
    __s32 					(*ptr_g_enable_close_scn)			( void );
    __s32 					(*ptr_g_disable_close_scn)			( void );
    __s32 					(*ptr_g_set_close_scn_time)			( __u32 time );
    __s32 					(*ptr_g_close_screen)				( void );
    void  					(*ptr_g_enable_standby)				( void );
    void  					(*ptr_g_disable_standby)			( void );
    __bool  				(*ptr_g_is_enable_standby)			( void );
    void 					(*ptr_g_close_scn_reserved[1])		( void );

    //mini_music\mini_music.h
    __s32 					(*ptr_mini_music_open)				(H_WIN parent, __s32 x, __s32 y);
    __s32  					(*ptr_mini_music_close)				(void);
    __bool  				(*ptr_mini_music_scene_exist)		(void);

    //bookengine.h
    __s32 					(*ptr_bookengine_open)				(void);
    __s32 					(*ptr_bookengine_close)				(void);

    //tp_adjust_scene
    H_WIN					(*ptr_scene_adjust_create)			(H_WIN);
    __s32 					(*ptr_scene_adjust_destroy)			(H_WIN);
} init_api_entry_t;

#ifndef USED_BY_INIT     //not used by init module itsef

#define INIT_ENTRY_TBL   (0xe1700000)   //this address is depended on the link address of mod_desktop
#define INIT_FUNC_ENTRY  ((init_api_entry_t *)INIT_ENTRY_TBL)

/* gscene_backgrd.h */
#define gscene_bgd_init				(INIT_FUNC_ENTRY->ptr_gscene_bgd_init			)
#define gscene_bgd_set_state		(INIT_FUNC_ENTRY->ptr_gscene_bgd_set_state		)
#define gscene_bgd_get_state		(INIT_FUNC_ENTRY->ptr_gscene_bgd_get_state		)
#define gscene_bgd_set_top			(INIT_FUNC_ENTRY->ptr_gscene_bgd_set_top		)
#define gscene_bgd_set_bottom		(INIT_FUNC_ENTRY->ptr_gscene_bgd_set_bottom		)
#define gscene_bgd_save_fb			(INIT_FUNC_ENTRY->ptr_gscene_bgd_save_fb		)
#define gscene_bgd_set_fb_type		(INIT_FUNC_ENTRY->ptr_gscene_bgd_set_fb_type	)
#define gscene_bgd_refresh			(INIT_FUNC_ENTRY->ptr_gscene_bgd_refresh		)
#define gscene_bgd_restore			(INIT_FUNC_ENTRY->ptr_gscene_bgd_restore		)
#define gscene_bgd_deinit			(INIT_FUNC_ENTRY->ptr_gscene_bgd_deinit			)
#define gscene_bgd_get_flag			(INIT_FUNC_ENTRY->ptr_gscene_bgd_get_flag		)
#define gscene_bgd_set_flag			(INIT_FUNC_ENTRY->ptr_gscene_bgd_set_flag		)
#define gscene_bgd_get_default_bg_index			(INIT_FUNC_ENTRY->ptr_gscene_bgd_get_default_bg_index		)
#define gscene_bgd_set_default_bg_index			(INIT_FUNC_ENTRY->ptr_gscene_bgd_set_default_bg_index		)


/* gscene_headbar.h */
#define	gscene_hbar_create			(INIT_FUNC_ENTRY->ptr_gscene_hbar_create		)
#define gscene_hbar_set_state		(INIT_FUNC_ENTRY->ptr_gscene_hbar_set_state		)
#define	gscene_hbar_get_state		(INIT_FUNC_ENTRY->ptr_gscene_hbar_get_state		)
#define	gscene_hbar_set_title		(INIT_FUNC_ENTRY->ptr_gscene_hbar_set_title		)
#define gscene_hbar_set_format 		(INIT_FUNC_ENTRY->ptr_gscene_hbar_set_format	)
#define gscene_hbar_get_screen_rect (INIT_FUNC_ENTRY->ptr_gscene_hbar_get_screen_rect)
#define	gscene_hbar_delete			(INIT_FUNC_ENTRY->ptr_gscene_hbar_delete		)


/*headbar_com.h*/

#define tf_cardstate_set            (INIT_FUNC_ENTRY->ptr_headbar_tf_cardstate_set  )
#define tf_cardstate_get            (INIT_FUNC_ENTRY->ptr_headbar_tf_cardstate_get  )

//display\g_display_switch_output.h
#define g_display_switch_output		(INIT_FUNC_ENTRY->ptr_g_display_switch_output	)

//display\g_close_screen.h
#define g_enable_close_scn			(INIT_FUNC_ENTRY->ptr_g_enable_close_scn		)
#define g_disable_close_scn			(INIT_FUNC_ENTRY->ptr_g_disable_close_scn		)
#define g_set_close_scn_time		(INIT_FUNC_ENTRY->ptr_g_set_close_scn_time		)
#define g_close_screen 				(INIT_FUNC_ENTRY->ptr_g_close_screen			)
#define g_enable_standby 			(INIT_FUNC_ENTRY->ptr_g_enable_standby			)
#define g_disable_standby 			(INIT_FUNC_ENTRY->ptr_g_disable_standby			)
#define g_is_enable_standby 		(INIT_FUNC_ENTRY->ptr_g_is_enable_standby	    )




//mini_music\mini_music.h
#define mini_music_open				(INIT_FUNC_ENTRY->ptr_mini_music_open			)
#define mini_music_close			(INIT_FUNC_ENTRY->ptr_mini_music_close			)
#define mini_music_scene_exist 		(INIT_FUNC_ENTRY->ptr_mini_music_scene_exist	)

//bookengine.h
#define bookengine_open				(INIT_FUNC_ENTRY->ptr_bookengine_open			)
#define bookengine_close			(INIT_FUNC_ENTRY->ptr_bookengine_close			)

//tp_adjust_scene.h
#define scene_adjust_create			(INIT_FUNC_ENTRY->ptr_scene_adjust_create		)
#define scene_adjust_destroy		(INIT_FUNC_ENTRY->ptr_scene_adjust_destroy		)
#endif /* USED_BY_INIT */

#endif /* __MOD_INIT_API_H__ */

