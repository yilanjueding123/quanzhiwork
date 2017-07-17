/*
********************************************************************************
*                                USB Hid Driver
*
*                (c) Copyright 2006-2010, All winners Co,Ld.
*                        All Right Reserved
*
* FileName		:  HidProtocol.h
*
* Author		:  Javen
*
* Date			:  2010/06/02
*
* Description	:  Hid –≠“È
*
* Others		:  NULL
*
* History:
*		<time> 			<author>	 <version >		<desc>
*	   2010.06.02		Javen			1.0			build this file
*
********************************************************************************
*/
#ifndef  __HIDPROTOCOL_H__
#define  __HIDPROTOCOL_H__

//----------------------------------------------------------------------
//
//
//----------------------------------------------------------------------
__s32 HidGetClassDescriptor(HidDev_t *HidDev,
                            __u32 InterfaceNo,
                            __u32 DescriptorType,
                            void *Buffer,
                            __u32 TransferLength);
__s32 HidGetIdle(HidDev_t *HidDev,
                 __u32 InterfaceNo,
                 __u32 ReportId,
                 void *IdleRate);
__s32 HidSetIlde(HidDev_t *HidDev,
                 __u32 InterfaceNo,
                 __u32 Duration,
                 __u32 ReportId);
__s32 HidGetReport(HidDev_t *HidDev,
                   __u32 InterfaceNo,
                   __u32 ReportType,
                   __u32 ReportId,
                   __u32 ReportLength,
                   void  *ReportData);
__s32 HidSetReport(HidDev_t *HidDev,
                   __u32 InterfaceNo,
                   __u32 ReportType,
                   __u32 ReportId,
                   __u32 ReportLength,
                   void *ReportData);
__s32 HidGetProtocol(HidDev_t *HidDev, __u32 InterfaceNo, void *ProtocolData);
__s32 HidSetProtocol(HidDev_t *HidDev, __u32 InterfaceNo, __u32 Protocoltype);

__s32 HidParseReport(__u8 *ReportData, __u32 ReportSize, HidDev_t *HidDev);
__s32 HidFreeReport(HidDev_t *HidDev);

#endif   //__HIDPROTOCOL_H__

