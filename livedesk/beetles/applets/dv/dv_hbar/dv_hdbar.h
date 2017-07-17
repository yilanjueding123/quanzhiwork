#ifndef 	__DV_HDBAR_H__
#define     __DV_HDBAR_H__

#include "beetles_app.h"
#include "dv_hdbar_ui.h"

typedef	struct tag_dv_hdbar_para
{
    H_LYR		layer;
    GUI_FONT 	*font;
} dv_hdbar_para_t;

typedef struct tag_dv_hdbar_ctrl
{
    dv_hdbar_para_t *dv_scene_para;
    HTHEME h_bmp_loading;
	
    //__u32 dv_signal_id[2];
    __u32 	dv_signal_id[6];
    HTHEME	dv_signal_htheme[6];
    void   *dv_signal_bmp[6];
	
    __u32 	dv_batt_id[6];
    HTHEME	dv_batt_htheme[6];
    void 	*dv_batt_bmp[6];
	
    __u8 dv_timer_id;
} hdbar_ctrl_t;



H_WIN dv_hdbar_create(H_WIN hparent);
void  dv_hdbar_delete(H_WIN dv_win);

#endif
























