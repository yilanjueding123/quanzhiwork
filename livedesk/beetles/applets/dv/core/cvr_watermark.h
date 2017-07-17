/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           home app sample
*
*						        	(c) Copyright 2006-2009, LYN, China
*											 All Rights Reserved
*
* File    : cvr_watermark.h
* By      : lyn
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __CVR_WATERMARK_H__
#define __CVR_WATERMARK_H__

#include "..\\dv_i.h"

void    cvr_water_mark_res_init(void);

void    cvr_water_mark_res_uninit(void);

__s32   cvr_water_mark_get_overlay_data(__lotus_overlay_src_init_t  *overlay_src_pic, __u32 level);

#endif  /* __CVR_WATERMARK_H__ */

