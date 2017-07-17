/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: dsk_volume.c
* By      	: Andy.zhang
* Func		: voice control
* Version	: v1.0
* ============================================================================================================
* 2009-7-8 10:02:17  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "..\\functions_i.h"

#define AUDIO_DRV_PATH "b:\\AUDIO\\PLAY"

static __bool dsk_volume_gate_on = EPDK_TRUE;
static __s32 dsk_volume = EPDK_TRUE;

static __s32 _set_volume( __s32 volume );
static __s32 _get_volume( void );


/*
****************************************************************************************************
*Name        : dsk_volume_set_volume
*Prototype   : __s32 dsk_volume_set_volume(__s32 volume)
*Arguments   : volume     input. volume value, ranges AUDIO_DEVICE_VOLUME_MIN~~AUDIO_DEVICE_VOLUME_MAX.
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : set the volume.
*Other       :
****************************************************************************************************
*/
__s32 dsk_volume_set(__s32 volume)	// 设置当前音量
{
    __s32        ret;

#if 0==EPDK_AUDIO_READY
    return EPDK_OK;
#endif


    if( !dsk_volume_gate_on )
    {
        return EPDK_FAIL;
    }

    ret = _set_volume( volume );

    if( ret != EPDK_FAIL )
    {
        dsk_volume = volume;

        {
            reg_system_para_t *para;
            para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
            if(para)
            {
                para->volume = volume;
            }
        }
    }

    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_volume_get_volume
*Prototype   : __s32 dsk_volume_get_volume(void)
*Arguments   : void
*Return      : the current volume value. -1 means fail.
*Description : get the curret volume value.
*Other       :
****************************************************************************************************
*/
__s32 dsk_volume_get(void)		// 当前音量大小
{
#if 0==EPDK_AUDIO_READY
    return EPDK_OK;
#endif

    if( !dsk_volume_gate_on )
    {
        return 0;
    }

    return _get_volume();
}



/*
****************************************************************************************************
*Name        : dsk_volume_inc_volume
*Prototype   : __s32 dsk_volume_inc_volume(void);
*Arguments   : void
*Return      : the current volume value. -1 means fail.
*Description : increase the volume.
*Other       :
****************************************************************************************************
*/
__s32 dsk_volume_inc(void)		// 增大音量
{
    __s32		volume;
    __s32       ret;

#if 0==EPDK_AUDIO_READY
    return EPDK_OK;
#endif


    if( !dsk_volume_gate_on )
    {
        return -1;
    }

    volume = _get_volume();
    if( volume == -1 )
    {
        return -1;
    }

    if( volume < AUDIO_DEVICE_VOLUME_MAX )
    {
        volume++;
        ret = _set_volume( volume );
        if( ret != EPDK_FAIL )
        {
            dsk_volume = volume;

            {
                reg_system_para_t *para;
                para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
                if(para)
                {
                    para->volume = volume;
                }
            }
            return volume;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return volume;
    }
}

/*
****************************************************************************************************
*Name        : dsk_volume_dec_volume
*Prototype   : __s32 dsk_volume_dec_volume(void);
*Arguments   : void
*Return      : the current volume value. -1 means fail.
*Description : increase the volume.
*Other       :
****************************************************************************************************
*/
__s32 dsk_volume_dec(void)		// 减小音量
{
    __s32		volume;
    __s32       ret;

#if 0==EPDK_AUDIO_READY
    return EPDK_OK;
#endif

    if( !dsk_volume_gate_on )
    {
        return -1;
    }

    volume = dsk_volume_get();
    if( volume == -1 )
    {
        return -1;
    }

    if( volume > AUDIO_DEVICE_VOLUME_MIN )
    {
        volume--;
        ret = _set_volume( volume );
        if( ret != EPDK_FAIL )
        {
            dsk_volume = volume;

            {
                reg_system_para_t *para;
                para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
                if(para)
                {
                    para->volume = volume;
                }
            }
            return volume;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return volume;
    }
}



/*
****************************************************************************************************
*Name        : dsk_volume_on
*Prototype   : __s32 dsk_volume_on(void);
*Arguments   : void
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : make the voice on.
*Other       :
****************************************************************************************************
*/
__s32 dsk_volume_on( void )
{
#if 0==EPDK_AUDIO_READY
    return EPDK_OK;
#endif

    if( dsk_volume_gate_on )
    {
        return EPDK_OK;
    }

    _set_volume( dsk_volume );

    dsk_volume_gate_on = EPDK_TRUE;

    return EPDK_OK;
}


/*
****************************************************************************************************
*Name        : dsk_volume_off
*Prototype   : __s32 dsk_volume_off(void);
*Arguments   : void
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : make the voice off(mute).
*Other       :
****************************************************************************************************
*/
__s32 dsk_volume_off( void )	         // close voice
{
#if 0==EPDK_AUDIO_READY
    return EPDK_OK;
#endif

    if( !dsk_volume_gate_on )
    {
        return EPDK_OK;
    }

    _set_volume( 0 );              // mute

    dsk_volume_gate_on = EPDK_FALSE;

    return EPDK_OK;
}



/*
****************************************************************************************************
*Name        : dsk_volume_is_on
*Prototype   : __bool dsk_volume_is_on( void )
*Arguments   : void
*Return      : the status of the voice, on or off(mute).
*Description : get the status of the voice, on or off(mute).
*Other       :
****************************************************************************************************
*/
__bool dsk_volume_is_on( void )
{
#if 0==EPDK_AUDIO_READY
    return EPDK_OK;
#endif

    return 	dsk_volume_gate_on;
}



static __s32 _set_volume( __s32 volume )
{
    ES_FILE		*p_dac;
    __s32        ret;

    if(volume < AUDIO_DEVICE_VOLUME_MIN
            || volume > AUDIO_DEVICE_VOLUME_MAX)
    {
        return EPDK_FAIL;
    }

    p_dac = eLIBs_fopen(AUDIO_DRV_PATH, "r+");
    if(!p_dac)
    {
        __err("open dac device fail!\n");
        return EPDK_FAIL;
    }

    ret = eLIBs_fioctrl(p_dac, AUDIO_DEV_CMD_SET_VOLUME, volume, 0);

    eLIBs_fclose(p_dac);

    return ret;
}

static __s32 _get_volume( void )
{
    ES_FILE		*p_dac;
    __s32        volume;

    p_dac = eLIBs_fopen(AUDIO_DRV_PATH, "r+");
    if(!p_dac)
    {
        __err("open dac device fail!\n");
        return -1;
    }

    volume = eLIBs_fioctrl(p_dac, AUDIO_DEV_CMD_GET_VOLUME, 0, 0);

    eLIBs_fclose(p_dac);

    return volume;
}




