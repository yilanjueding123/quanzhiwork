/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: core_keytone.c
* By      	: Andy.zhang
* Func		: keytone
* Version	: v1.0
* ============================================================================================================
* 2009-10-15 15:27:26 andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "epdk.h"
#include "Dsk_keytone.h"
#define   SOCKET_SIZE   (1024 * 512)
static __s32 g_wav_sample_size = 0;
typedef struct __WAVE_HEADER1
{
    __u32       uRiffFcc;       // four character code, "RIFF"
    __u32       uFileLen;       // file total length, don't care it

    __u32       uWaveFcc;       // four character code, "WAVE"

    __u32       uFmtFcc;        // four character code, "fmt "
    __u32       uFmtDataLen;    // Length of the fmt data (=16)
    __u16       uWavEncodeTag;  // WAVE File Encoding Tag
    __u16       uChannels;      // Channels: 1 = mono, 2 = stereo
    __u32       uSampleRate;    // Samples per second: e.g., 44100
    __u32       uBytesPerSec;   // sample rate * block align
    __u16       uBlockAlign;    // channels * bits/sample / 8
    __u16       uBitsPerSample; // 8 or 16

    __u32       uDataFcc;       // four character code "data"
    __u32       uSampDataSize;  // Sample data size(n)

} __wave_header_t1;

/*********************************************************************************************************************/
typedef struct tag_DKTone
{
    ES_FILE  *f_audiodev;
    ES_FILE  *f_audioctrl;
    __u8     *tonebuf;
    __set_keytone_t	 state;
    __u32    wavsize;
} DKTone;
#define g_malloc( size )      esMEMS_Malloc( 0, size )
#define g_free( p )           esMEMS_Mfree( 0, p )
static DKTone *keytone = NULL;
static __u32 g_key_type = KEYTONE_KEY_TYPE_DOWN | KEYTONE_KEY_TYPE_REPEAT;
/*********************************************************************************************************************/


/*
****************************************************************************************************
*Name        : dsk_keytone_init
*Prototype   : __s32 dsk_keytone_init( const char *keytone_file )
*Arguments   : keytone_file  input. the keytone file.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : init the keytone module.
*Other       :
****************************************************************************************************
*/
__s32  dsk_keytone_init_1( const char *keytone_file )
{
    ES_FILE                 *pfile2;

    __audio_dev_para_t    	pbuf2;
    __wave_header_t1      	wav;
    __s32						headwavcntp;

    keytone = (DKTone *)g_malloc(sizeof(DKTone));
    if( !keytone )
    {
        __inf(" DKTone malloc error \n");
        return EPDK_FAIL;
    }

    keytone->f_audiodev = eLIBs_fopen("b:\\AUDIO\\PLAY", "r+");
    keytone->f_audioctrl = eLIBs_fopen("b:\\AUDIO\\CTRL", "r+");
    if(!keytone->f_audioctrl)
    {
        __inf(" DKTone malloc error \n");
        return EPDK_FAIL;
    }


    if(!keytone->f_audiodev)
    {
        __inf(" open the device play fail \n");

    }
    else
        __inf(" open the device play sucess\n");
    keytone->state 		= SET_KEYTONE_ON;

    pfile2 = eLIBs_fopen(keytone_file, "rb");

    if(pfile2 == 0)
    {
        __inf("%s cannot open \n", keytone_file);
        return EPDK_OK;
    }
    else
        __inf(" open the wav file  sucess\n");

    headwavcntp = eLIBs_fread(&wav, 1, sizeof(__wave_header_t1), pfile2);
    if(headwavcntp == sizeof(__wave_header_t1))
        __inf(" eLIBs_fread wav file head sucess \n");
    g_wav_sample_size = wav.uSampDataSize;
    keytone->wavsize = wav.uSampDataSize;

    keytone->tonebuf 	= (__u8 *)esMEMS_Palloc(((g_wav_sample_size + 1023) >> 10), MEMS_PALLOC_MODE_BND_NONE | MEMS_PALLOC_MODE_BNK_NONE);
    if( !keytone->tonebuf )
    {
        __inf(" tonebuf malloc error \n");
        return EPDK_FAIL;
    }


    headwavcntp = eLIBs_fread(keytone->tonebuf, 1, g_wav_sample_size, pfile2);


    eLIBs_fclose(pfile2);


    pbuf2.bps  	= wav.uBitsPerSample;
    pbuf2.chn 	= wav.uChannels;
    pbuf2.fs  	= wav.uSampleRate;

    eLIBs_fioctrl(keytone->f_audiodev, AUDIO_DEV_CMD_SET_PARA, 0, &pbuf2);

    eLIBs_fioctrl(keytone->f_audiodev, AUDIO_DEV_CMD_REG_USERMODE, AUDIO_PLAY_USR_KEY, 0);

    eLIBs_fioctrl(keytone->f_audiodev, AUDIO_DEV_CMD_START, 0, 0);

    //	eLIBs_fioctrl(keytone->f_audioctrl, AUDIO_DEV_CMD_CHANGE_IF, 2, 0);



    return EPDK_OK;
}



/*
****************************************************************************************************
*Name        : dsk_keytone_exit
*Prototype   : __s32 dsk_keytone_exit(void)
*Arguments   : void
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : exit the keytone module.
*Other       :
****************************************************************************************************
*/
__s32 dsk_keytone_exit_1(void)
{
    if(keytone)
    {
        eLIBs_fioctrl(keytone->f_audiodev, AUDIO_DEV_CMD_STOP, 0, 0);
        eLIBs_fclose(keytone->f_audiodev);
        esMEMS_Pfree((void *)keytone->tonebuf, ((g_wav_sample_size + 1023) >> 10));
        g_free(keytone);
        keytone = NULL;

        return EPDK_OK;
    }

    return EPDK_FAIL;
}


/*
****************************************************************************************************
*Name        : dsk_keytone_on
*Prototype   : __s32 dsk_keytone_on(void)
*Arguments   : void
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : ring the tone.
*Other       :
****************************************************************************************************
*/
__s32 dsk_keytone_on_1(void)
{
    __s32 i ;
    if( (keytone->state == SET_KEYTONE_ON) && (keytone->tonebuf) && (keytone->f_audiodev) )
    {
        eLIBs_fioctrl(keytone->f_audiodev, AUDIO_DEV_CMD_FLUSH_BUF, 0, 0);
        for(i = 0; i < 10; i++)
        {
            eLIBs_fwrite(keytone->tonebuf + (keytone->wavsize / 10)*i, 1, keytone->wavsize / 10, keytone->f_audiodev);
        }
        eLIBs_fwrite(keytone->tonebuf + (keytone->wavsize / 10)*i, 1, keytone->wavsize - (keytone->wavsize / 10) * 10, keytone->f_audiodev);
    }
    return EPDK_OK;
}



/*
****************************************************************************************************
*Name        : dsk_keytone_set_state
*Prototype   : dsk_keytone_set_state(__set_keytone_t state)
*Arguments   : state       input. gate status, on or off.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : set the keytone gate status. on or off.
*Other       :
****************************************************************************************************
*/
__s32 dsk_keytone_set_state_1(__set_keytone_t state)
{
    keytone->state = state;
    return EPDK_OK;
}



/*
****************************************************************************************************
*Name        : dsk_keytone_get_state
*Prototype   : __s32 dsk_keytone_get_state(__set_keytone_t *p_state)
*Arguments   : p_state     output. the keytone gate status. on or off.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : get the keytone gate status. on or off.
*Other       :
****************************************************************************************************
*/
__s32 dsk_keytone_get_state_1(__set_keytone_t *p_state)
{
    *p_state = keytone->state;
    return EPDK_OK;
}

void dsk_keytone_set_key_type(__u32 key_type)
{
    g_key_type = key_type;
}

__u32 dsk_keytone_get_key_type(void)
{
    return g_key_type;
}

__s32 dsk_keytone_set_path_1(char *path)
{
    ES_FILE             	*pfile2;
    __audio_dev_para_t    	pbuf2;
    __wave_header_t1      	wav;

    pfile2 = eLIBs_fopen(path, "rb");
    if(pfile2 == NULL)
    {
        __wrn("chord.wav cannot open \n");
        return EPDK_FAIL;
    }

    eLIBs_fioctrl(keytone->f_audiodev, AUDIO_DEV_CMD_STOP, 0, 0);

    eLIBs_memset((void *)&wav, 0, sizeof(__wave_header_t1));
    eLIBs_fread(&wav, 1, sizeof(__wave_header_t1), pfile2);
    keytone->wavsize = wav.uSampDataSize;

    if(wav.uSampDataSize > ((SOCKET_SIZE >> 10) << 10))
    {
        __wrn("space is not enough...\n ");
        eLIBs_fclose(pfile2);
        return EPDK_FAIL;
    }

    eLIBs_memset((void *)keytone->tonebuf, 0, SOCKET_SIZE);
    eLIBs_fread(keytone->tonebuf, 1, wav.uSampDataSize, pfile2);

    eLIBs_fclose(pfile2);

    pbuf2.bps  	= wav.uBitsPerSample;
    pbuf2.chn 	= wav.uChannels;
    pbuf2.fs  	= wav.uSampleRate;

    eLIBs_fioctrl(keytone->f_audiodev, AUDIO_DEV_CMD_SET_PARA, 0, &pbuf2);
    eLIBs_fioctrl(keytone->f_audiodev, AUDIO_DEV_CMD_REG_USERMODE, AUDIO_PLAY_USR_KEY, 0);
    eLIBs_fioctrl(keytone->f_audiodev, AUDIO_DEV_CMD_START, 0, 0);

    return EPDK_OK;
}
__s32 dsk_keytone_stop(void)
{

    eLIBs_fioctrl(keytone->f_audiodev, AUDIO_DEV_CMD_STOP, 0, 0);


}

