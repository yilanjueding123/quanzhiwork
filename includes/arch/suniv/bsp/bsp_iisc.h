/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eBIOS Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : iis ebios module
* File    : iisc.h
*
* By      : Steven
* Version : v1.00
* Date    : 2008-8-19 18:42:49
**********************************************************************************************************************
*/
#ifndef _BSP_IISC_H_
#define _BSP_IISC_H_

//#include  "audio.h"

typedef struct __AUDIO_IISC_PARA
{
    __u32 slave;                // 0: master, 1: slave
    __u32 mono;                 // 0: stereo, 1: mono
    __u32 samp_fs;              // audio sample rate (unit in kHz)
    __u32 samp_res;             // 16 bits, 20 bits , 24 bits, 32 bits)
    __u32 samp_format;          // audio sample format (0: standard I2S, 1: left-justified, 2: right-justified, 3: pcm)
    __u32 ws_size;              // 16 BCLK, 20 BCLK, 24 BCLK, 32 BCLK)
    __u32 mclk_rate;            // mclk frequency divide by fs (128fs, 192fs, 256fs, 384fs, 512fs, 768fs)
    __u32 lrc_pol;              // LRC clock polarity (0: normal ,1: inverted)
    __u32 bclk_pol;             // BCLK polarity (0: normal, 1: inverted)
    __u32 pcm_txtype;           // PCM transmitter type (0: 16-bits linear mode, 1: 8-bits linear mode, 2: u-law, 3: A-law)
    __u32 pcm_rxtype;           // PCM receiver type  (0: 16-bits linear mode, 1: 8-bits linear mode, 2: u-law, 3: A-law)
    __u32 pcm_sw;               // PCM slot width (8: 8 bits, 16: 16 bits)
    __u32 pcm_sync_period;      // PCM sync period (16/32/64/128/256)
    __u32 pcm_sync_type;        // PCM sync symbol size (0: short sync, 1: long sync)
    __u32 pcm_start_slot;       // PCM start slot index (1--4)
    __u32 pcm_lsb_first;        // 0: MSB first, 1: LSB first
    __u32 pcm_ch_num;           // PCM channel number (1: one channel, 2: two channel)
    __u32 fifo_txim;            // TX FIFO input mode (0: valid data at MSB, 1: vallid data at LSB)
    __u32 fifo_rxom;            // RX FIFO output mode
    __u32 fifo_tx_tlevel;       // TX FIFO trigger level
    __u32 fifo_rx_tlevel;       // RX FIFO trigger level
} __audio_iisc_para_t;

typedef enum __GET_IIS_PARA_TYPE
{
    GET_IIS_PARA_TYPE_NONE = 0,
    GET_IIS_PARA_TYPE_FSBETWEEN,
    GET_IIS_PARA_TYPE_

} __get_iis_para_type_t;


extern __s32 BSP_IIS_Init(void);
extern __s32 BSP_IIS_Exit(void);
extern __s32 BSP_IIS_Enable(__u32 uDevType);
extern __s32 BSP_IIS_Disable(__u32 uDevType);
extern __s32 BSP_IIS_Start(__u32 uDevType);
extern __s32 BSP_IIS_Stop(__u32 uDevType);
extern __s32 BSP_IIS_Pause(__u32 uDevType, __s32 nMode);
extern __s32 BSP_IIS_SetSampWidth(__u32 uDevType, __u32 nSampWidth);
extern __s32 BSP_IIS_SetSampRate(__u32 uDevType, __s32 nSampRate);
extern __s32 BSP_IIS_SetChannel(__u32 uDevType, __u32 nChnCnt);
extern __s32 BSP_IIS_GetSampleCnt(__u32 uDevType);
extern __s32 BSP_IIS_SetSampleCnt(__u32 uDevType, __u32 nCnt);
extern __s32 BSP_IIS_GetVolume(__u32 uDevType);
extern __s32 BSP_IIS_SetVolume(__u32 uDevType, __s32 nVolume);
extern __s32 BSP_IIS_GetDmaCfg(__u32 uDevType);
extern __s32 BSP_IIS_GetFifoAddr(__u32 uDevType);
extern __s32 BSP_IIS_EnableINT(__u16 int_no);
extern __s32 BSP_IIS_DisableINT (__u16 int_no);
extern __u32 BSP_IIS_QuerryINT(void);
extern __s32 BSP_IIS_GetPara(__u32 uDevType, __u32 uParType, __s32 para, void *pRet);


#endif /* #ifndef _BSP_IISC_H_ */

