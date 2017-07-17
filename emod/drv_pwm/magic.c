

#include "drv_pwm_i.h"

#pragma arm section rodata="MAGIC"

const __mods_mgsec_t modinfo =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'},		//.magic
    0x01000000,                				//.version
    EMOD_TYPE_DRV_PWM,                     //.type
    0xF0000,                                //.heapaddr
    0x400,                      			//.heapsize
    {
        //.mif
        &DRV_PWM_MInit,
        &DRV_PWM_MExit,
        &DRV_PWM_MOpen,
        &DRV_PWM_MClose,
        &DRV_PWM_MRead,
        &DRV_PWM_MWrite,
        &DRV_PWM_MIoctrl
    }
};
#pragma arm section
