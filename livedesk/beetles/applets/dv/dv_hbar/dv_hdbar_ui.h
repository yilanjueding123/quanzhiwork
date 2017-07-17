#ifndef 	__DV_HDBAR_UI_H__
#define     __DV_HDBAR_UI_H__

#include "beetles_app.h"

typedef struct dv_hdbar_uipara
{
    RECT dv_scene_uipara;
    RECT dv_mode_rect;
	RECT dv_freq_rect;
    RECT dv_time_rect;
	RECT dv_batt_rect;
	RECT dv_signal_rect;
} dv_hdbar_uipara_t;

dv_hdbar_uipara_t *dv_hdbar_get_uipara(void);

#endif

