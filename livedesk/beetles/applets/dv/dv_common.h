/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           home app sample
*
*						        	(c) Copyright 2006-2009, China
*											 All Rights Reserved
*
* File    : dv_uipara.h
* By      : lincoln
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __DV_COMMON_H__
#define __DV_COMMON_H__

#define		TEXT_COLOR	0xffffffff

H_LYR dv_layer_create(RECT *rect, __disp_layer_work_mode_t mode);
void dv_layer_destroy( H_LYR lyr_hdl);
__s32 Dv_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);

#endif /* __DV_COMMON_H__ */
