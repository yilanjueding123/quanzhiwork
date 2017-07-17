/*
********************************************************************************************************************
*                                              usb host driver
*
*                              (c) Copyright 2007-2010, javen.China
*										All	Rights Reserved
*
* File Name 	: mscProtocol.h
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
#ifndef  __MSCPROTOCOL_H__
#define  __MSCPROTOCOL_H__

__s32 mscQIC157Command(__mscDev_t *mscDev, __ScsiCmnd_t *ScsiCmnd);
__s32 mscATAPICommand(__mscDev_t *mscDev, __ScsiCmnd_t *ScsiCmnd);
__s32 mscUFICommand(__mscDev_t *mscDev, __ScsiCmnd_t *ScsiCmnd);
__s32 mscScsiCommand(__mscDev_t *mscDev, __ScsiCmnd_t *ScsiCmnd);

#endif   //__MSCPROTOCOL_H__

