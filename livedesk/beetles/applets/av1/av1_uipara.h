
#ifndef __APP_AV1_PARA_H_
#define  __APP_AV1_PARA_H_
#include "app_av1_i.h"

typedef struct av1_uipara
{
    RECT av1_scene_uipara;
    RECT av1_tips_rect;
} av1_uipara_t;

av1_uipara_t *app_av1_get_uipara(void);

#endif

