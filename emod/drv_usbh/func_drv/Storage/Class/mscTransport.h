/*
********************************************************************************************************************
*                                              usb host driver
*
*                              (c) Copyright 2007-2010, javen.China
*										All	Rights Reserved
*
* File Name 	: mscTransport.h
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
#ifndef  __MSCTRANSPORT_H__
#define  __MSCTRANSPORT_H__

/* ¥´ ‰¥ÌŒÛ∫≈ */
#define  USB_STOR_XFER_GOOD				0
#define  USB_STOR_XFER_ERROR			1
#define  USB_STOR_XFER_SHORT			2
#define  USB_STOR_XFER_STALLED			3
#define  USB_STOR_XFER_LONG				4
#define  USB_STOR_XFER_TIME_OUT			5

/* mass storage transport time out */
#define  USB_STOR_REST_TIME    			4000   	//4s, ∏¥Œª«Î«Û
#define  USB_STOR_CTRL_MSG_TIME			4000	//4s, øÿ÷∆√¸¡Ó
#define  USB_STOR_CBW_CSW_TIME			4000	//4s, cbw, csw

#define  MAX_CBWCB_SIZE 				16
#define  MAX_BOT_CSW_REPEAT				2

/* bulk-only class specific requests */
#define  USB_BULK_RESET_REQUEST			0xff
#define  USB_BULK_GET_MAX_LUN			0xfe

// Command Block Wrapper Signature 'USBC'
#define CBW_SIGNATURE               0x43425355
#define CBW_FLAGS_DATA_IN           0x80
#define CBW_FLAGS_DATA_OUT          0x00

// Command Status Wrapper Signature 'USBS'
#define CSW_SIGNATURE               0x53425355
#define CSW_STATUS_GOOD             0x00
#define CSW_STATUS_FAILED           0x01
#define CSW_STATUS_PHASE_ERROR      0x02

// Command Block Wrapper
typedef __packed struct __CBW
{
    __u32 dCBWSignature; 			// 0-3
    __u32 dCBWTag;    				// 4-7
    __u32 dCBWDataTransferLength; 	// 8-11
    __u8  bmCBWFlags; 				// 12
    __u8  bCBWLUN: 4; 				// 13
    __u8  bReserved1: 4;
    __u8  bCBWCBLength: 5; 			// 14
    __u8  bReserved2: 3;
    __u8  CBWCB[MAX_CBWCB_SIZE]; 	// 15-30
} __CBW_t;

// Command Status Wrapper
typedef __packed struct __CSW
{
    __u32 dCSWSignature; 		// 0-3
    __u32 dCSWTag; 				// 4-7
    __u32 dCSWDataResidue; 		// 8-11
    __u8  bCSWStatus; 			// 12
} __CSW_t;


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
__u32 mscGetMaxLun(__mscDev_t *mscDev);
__s32 mscResetRecovery(__mscDev_t *mscDev);
__s32 mscBoTransport(__mscDev_t *mscDev, __ScsiCmnd_t *ScsiCmnd);
__s32 mscBoStopTransport(__mscDev_t *mscDev);

__s32 mscGetDataTransportReason(__mscLun_t *mscLun, __ScsiCmnd_t *ScsiCmnd);


#endif   //__MSCTRANSPORT_H__


