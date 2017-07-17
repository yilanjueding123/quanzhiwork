
#ifndef  __APP_AV1_H_
#define  __APP_AV1_H_

#include "beetles_app.h"

typedef	struct tag_av1_para
{
    H_LYR		layer;
    GUI_FONT 	*font;
    HTHEME      single4_handle;
    HTHEME		single3_handle;
    HTHEME		single2_handle;
    HTHEME		single1_handle;
    HTHEME		single0_handle;
    HTHEME      no_single0_handle;
    HTHEME      no_single1_handle;

} av1_para_t;

H_WIN app_av1_create(root_para_t *para);






#endif

