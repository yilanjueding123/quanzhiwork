#include "dv_hdbar_ui.h"
static dv_hdbar_uipara_t dv_hdbar_uipara_480_240 = 
{
	{0, 0, 480, 60},
	{120, 0, 100, 20},		//mode
	{0, 0, 120, 20},		//freq
	{220, 0, 80, 20},			//time
	{320, 0, 80, 20},		//batt
	{400, 0, 60, 20}		//signal
	
};

dv_hdbar_uipara_t *dv_hdbar_get_uipara(void)
{
    return &dv_hdbar_uipara_480_240;
}

