/*
********************************************************************************************************************
*                                              usb host driver
*
*                              (c) Copyright 2007-2010, javen.China
*										All	Rights Reserved
*
* File Name 	: LunMgr_i.h
*
* Author 		: javen
*
* Version 		: 2.0
*
* Date 			: 2010.03.02
*
* Description 	:
*
* History 		:
*
********************************************************************************************************************
*/
#ifndef  __LUNMGR_I_H__
#define  __LUNMGR_I_H__

__s32 DiskProbe(__mscLun_t *mscLun);
__s32 DiskRemove(__mscLun_t *mscLun);

__s32 CDProbe(__mscLun_t *mscLun);
__s32 CDRemove(__mscLun_t *mscLun);


#endif   //__LUNMGR_I_H__


