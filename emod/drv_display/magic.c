

#include "drv_display_i.h"

#pragma arm section rodata="MAGIC"

const __mods_mgsec_t modinfo =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'},		//.magic
    0x01000000,                				//.version
    EMOD_TYPE_DRV_DISP,                     //.type
    0xF0000,                                //.heapaddr
    0x400,                      			//.heapsize
    {
        //.mif
        &DRV_DISP_MInit,
        &DRV_DISP_MExit,
        &DRV_DISP_MOpen,
        &DRV_DISP_MClose,
        &DRV_DISP_MRead,
        &DRV_DISP_MWrite,
        &DRV_DISP_MIoctrl
    }
};
#pragma arm section
