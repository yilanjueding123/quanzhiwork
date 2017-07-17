/*
********************************************************************************
*                                USB Hid Driver
*
*                (c) Copyright 2006-2010, All winners Co,Ld.
*                        All Right Reserved
*
* FileName		:  HidTransport.h
*
* Author		:  Javen
*
* Date			:  2010/06/02
*
* Description	:  Hid ´«Êä
*
* Others		:  NULL
*
* History:
*		<time> 			<author>	 <version >		<desc>
*	   2010.06.02		Javen			1.0			build this file
*
********************************************************************************
*/
#ifndef  __HIDTRANSPORT_H__
#define  __HIDTRANSPORT_H__

__s32 HidSoftReset(HidDev_t *HidDev);
__s32 HidResetRecovery(HidDev_t *HidDev);
__s32 HidTransport(HidDev_t *HidDev, HidRequest_t *HidReq);
__s32 HidStopTransport(HidDev_t *HidDev);

#endif   //__HIDTRANSPORT_H__

