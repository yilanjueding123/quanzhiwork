/*
********************************************************************************************************************
*                                              usb host driver
*
*                              (c) Copyright 2007-2010, javen.China
*										All	Rights Reserved
*
* File Name 	: Scsi2.h
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
#ifndef  __SCSI2_H__
#define  __SCSI2_H__

//----------------------------------------------------------------------------
// scsi-2
//----------------------------------------------------------------------------

// timeout values (milliseconds)
#define  SCSI_MEDIA_POLL_INTERVAL    1250	/* 1.25s, 查询media change间隔时间 	*/
#define  SCSI_MEDIA_OP_TIMEOUT    	 10000	/* 10s, 读/写等介质操作超时时间 	*/
#define  SCSI_COMMAND_TIMEOUT        5000	/* 2s, 普通命令超时时间 			*/

#define  SCSI_CMD_RETRY				 3

/* CDB size */
#define SCSI_CDB_6                  6
#define SCSI_CDB_10                 10
#define SCSI_CDB_16                 16
#define UFI_CDB                     12
#define ATAPI_CDB                   12
#define MAX_CDB                     SCSI_CDB_16

// SCSI-2 device types
#define SCSI_DEVICE_DIRECT_ACCESS     0x00
#define SCSI_DEVICE_SEQUENTIAL_ACCESS 0x01
#define SCSI_DEVICE_PRINTER           0x02
#define SCSI_DEVICE_PROCESSOR         0x03
#define SCSI_DEVICE_WRITE_ONCE        0x04
#define SCSI_DEVICE_CDROM             0x05
#define SCSI_DEVICE_SCANNER           0x06
#define SCSI_DEVICE_OPTICAL           0x07
#define SCSI_DEVICE_MEDIUM_CHANGER    0x08
#define SCSI_DEVICE_COMM              0x09
#define SCSI_DEVICE_UNKNOWN           0x1F

// SCSI-2 medium types
#define SCSI_MEDIUM_UNKNOWN           0xFF

// CD-ROM medium types
#define MEDIUM_CD_ROM_UNKNOWN     					0x00
#define MEDIUM_CD_ROM_DATA_ONLY_120     			0x01
#define MEDIUM_CD_DA_AUDIO_ONLY_120     			0x02
#define MEDIUM_CD_ROM_DATA_AUDIO_COMBINED_120   	0x03
#define MEDIUM_CD_PHOTO_CD_120     					0x04
#define MEDIUM_CD_ROM_DATA_ONLY_80     				0x05
#define MEDIUM_CD_DA_AUDIO_ONLY_80     				0x06
#define MEDIUM_CD_ROM_DATA_AUDIO_COMBINED_80     	0x07
#define MEDIUM_CD_PHOTO_CD_80        				0x08

#define MEDIUM_CDR_ROM_UNKNOWN      				0x10
#define MEDIUM_CDR_ROM_DATA_ONLY_120     			0x11
#define MEDIUM_CDR_DA_AUDIO_ONLY_120     			0x12
#define MEDIUM_CDR_ROM_DATA_AUDIO_COMBINED_120   	0x13
#define MEDIUM_CDR_PHOTO_CD_120     				0x14
#define MEDIUM_CDR_ROM_DATA_ONLY_80     			0x15
#define MEDIUM_CDR_DA_AUDIO_ONLY_80     			0x16
#define MEDIUM_CDR_ROM_DATA_AUDIO_COMBINED_80     	0x17
#define MEDIUM_CDR_PHOTO_CD_80        				0x18

#define MEDIUM_CDE_ROM_UNKNOWN      				0x20
#define MEDIUM_CDE_ROM_DATA_ONLY_120     			0x21
#define MEDIUM_CDE_DA_AUDIO_ONLY_120     			0x22
#define MEDIUM_CDE_ROM_DATA_AUDIO_COMBINED_120   	0x23
#define MEDIUM_CDE_PHOTO_CD_120     				0x24
#define MEDIUM_CDE_ROM_DATA_ONLY_80     			0x25
#define MEDIUM_CDE_DA_AUDIO_ONLY_80     			0x26
#define MEDIUM_CDE_ROM_DATA_AUDIO_COMBINED_80     	0x27
#define MEDIUM_CDE_PHOTO_CD_80        				0x28


// mandatory SCSI-2 commands
#define SCSI_TEST_UNIT_READY      0x00
#define SCSI_REQUEST_SENSE        0x03
#define SCSI_INQUIRY              0x12
#define SCSI_SEND_DIAGNOSTIC      0x1D

// mandatory device-specific SCSI-2 commands
#define SCSI_READ6                0x08
#define SCSI_READ10               0x28
#define SCSI_READ16               0x88
#define SCSI_READ_CAPACITY        0x25

// optional device-specific SCSI-2 commands
#define SCSI_MODE_SELECT6         0x15
#define SCSI_MODE_SENSE6          0x1A
#define SCSI_START_STOP           0x1B
#define SCSI_WRITE6               0x0A
#define SCSI_WRITE10              0x2A
#define SCSI_WRITE16              0x8A
#define SCSI_SYNCHRONIZE_CACHE	  0x35
#define SCSI_MODE_SELECT10        0x55
#define SCSI_MODE_SENSE10         0x5A

// ATAPI (CD-ROM) commands
#define SCSI_CD_PAUSE_RESUME      				0x4B
#define SCSI_CD_PLAY_AUDIO10       				0x45
#define SCSI_CD_PLAY_AUDIO_MSF     				0x47
#define SCSI_CD_PLAY_CD							0xBC
#define SCSI_CD_READ_TOC          				0x43
#define SCSI_CD_STOP_PLAY_SCAN     				0x4E
#define SCSI_CD_SET_SPEED						0xBB

// ATAPI (DVD) commands
#define SCSI_DVD_GET_EVENT_STATUS_NOTIFICATION	0x4A
#define SCSI_DVD_LOAD_UNLOAD					0xA6
#define SCSI_DVD_PREVENT_ALLOW_MEDIUM_REMOVAL	0x1E
#define SCSI_DVD_READ_DISC_INFORMATION			0x51
#define SCSI_DVD_READ_TRACK_INFORMATION			0x52

// mode pages
#define MODE_PAGE_FLEXIBLE_DISK   	0x05
#define MODE_PAGE_WCACHING_PAGE		0x08
#define MODE_PAGE_CDROM           	0x0D
#define MODE_PAGE_CDROM_AUDIO     	0x0E
#define MODE_PAGE_POWER_CONDITION	0x1A
#define MODE_PAGE_CDROM_CAPS      	0x2A
#define MODE_PAGE_ALL_PAGES			0x3F

// SCSI-2 sense keys
#define SENSE_NONE                0x00
#define SENSE_RECOVERED_ERROR     0x01
#define SENSE_NOT_READY           0x02
#define SENSE_MEDIUM_ERROR        0x03
#define SENSE_HARDWARE_ERROR      0x04
#define SENSE_ILLEGAL_REQUEST     0x05
#define SENSE_UNIT_ATTENTION      0x06
#define SENSE_DATA_PROTECT        0x07
#define SENSE_BLANK_CHECK         0x08

// SCSI-2 ASC
#define ASC_LUN                   0x04
#define ASC_INVALID_COMMAND_FIELD 0x24
#define ASC_MEDIA_CHANGED         0x28
#define ASC_RESET                 0x29
#define ASC_COMMANDS_CLEARED      0x2F
#define ASC_MEDIUM_NOT_PRESENT    0x3A



//----------------------------------------------------------------------------
// 命令执行的结果
//----------------------------------------------------------------------------
#define  USB_STATUS_SUCCESS									0	/* 命令执行成功 				*/

/* 输入参数 */
#define  USB_STATUS_BAD_ARGUMENTS							1	/* 参数错误 					*/
#define  USB_STATUS_BUFFER_TOO_SMALL						2	/* 输入buffer太小 				*/

/* 设备相关 */
#define  USB_STATUS_DEVICE_DISCONNECTED						30	/* 设备已经断开连接 			*/
#define  USB_STATUS_IO_TIMEOUT								31	/* IO操作超时 					*/
#define  USB_STATUS_IO_DEVICE_ERROR							32	/* 设备错误 					*/
#define  USB_STATUS_DEVICE_BUSY								33	/* 设备忙 						*/

/* 命令相关 */
#define  USB_STATUS_INVALID_COMMAND							60	/* 不支持的命令 				*/
#define  USB_STATUS_INVALID_FIELD_IN_COMMAND				61	/* 命令中有不支持的字段 		*/
#define  USB_STATUS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE		62	/* 超过最大逻辑块 				*/
#define  USB_STATUS_COMMAND_FAILED							63	/* 命令执行失败 				*/

/* 命令结果相关 */
#define  USB_STATUS_UNIT_NOT_READY							91	/* 介质单元没有就绪				*/
#define  USB_STATUS_MEDIA_NOT_PRESENT						92	/* 媒体介质不存在 				*/
#define  USB_STATUS_NOT_READY_TO_READY_TRANSITION			93	/* Media从没有准备就绪到就绪 	*/
#define  USB_STATUS_UNRECOGNIZED_MEDIA						94	/* 不支持的Media 				*/
#define  USB_STATUS_MEDIA_CHANGE							95	/* Media发生变化 				*/
#define  USB_STATUS_UNKOWN_SENSE							96	/* 未知sense 					*/

#define  USB_STATUS_UNKOWN_ERROR							255	/* 未知错误 					*/



//----------------------------------------------------------------------------
// Inquiry返回的数据
//----------------------------------------------------------------------------
typedef __packed struct __InquiryData
{
    __u8 Device_Type : 5;			/* 设备类型 					*/
    __u8 Qualifier   : 3;			/* 外围标识符 					*/

    __u8 scsi_1 			: 7;	/* 命令集版本 					*/
    __u8 Removable_Media 	: 1;	/* 可移动位 					*/

    __u8 ANSI_Version	: 4;		/* 设备支持的命令集版本 		*/
    __u8 ECMA_Version	: 2;		/* 设备支持的ECMA-111版本 		*/
    __u8 ISO_Version	: 2;		/* 设备支持的ISO IS-9316版本 	*/

    __u8 Data_Format;				/* 标准INQUIRY数据的响应格式 	*/
    __u8 Additional_Length;			/* 附加长度 					*/
    __u8 res[3];

    __u8 Vendor_Info[8];			/* 设备制造商 					*/
    __u8 Product_Info[16];			/* 产品信息 					*/
    __u8 Product_Revision[4];		/* 产品修订版 					*/
} __InquiryData_t;


//----------------------------------------------------------------------------
// request sense 返回的数据
//----------------------------------------------------------------------------
typedef __packed struct __SenseData
{
    __u8 ErrorCode: 7;
    __u8 Valid: 1;
    __u8 SegmentNumber;
    __u8 SenseKey: 4;
    __u8 Reserved: 1;
    __u8 IncorrectLength: 1;
    __u8 EndOfMedia: 1;
    __u8 FileMark: 1;
    __u8 Information[4];
    __u8 AdditionalSenseLength;
    __u8 CommandSpecificInformation[4];
    __u8 AdditionalSenseCode;
    __u8 AdditionalSenseCodeQualifier;
    __u8 FieldReplaceableUnitCode;
    __u8 SenseKeySpecific[3];
} __SenseData_t;

//----------------------------------------------------------------------------
// mode sense 返回的数据
//----------------------------------------------------------------------------
#define  SCSI_MODE_SENSE_MAX_DATA_LEN		192

typedef __packed struct __BlockDevSpecPara
{
    __u8 res_1 : 4;
    __u8 DpoFua : 1;		/* 是否支持禁止页输出和强迫单元存取 */
    __u8 res_2 : 2;
    __u8 WP: 1;			/* write protect */
} __BlockDevSpecPara_t;

typedef __packed struct __ModeSense6Header
{
    __u8 ModeDataLength;
    __u8 MediumType;
    __u8 DevSpecPara;
    __u8 BlockDescLength;
} __ModeSense6Header_t;

typedef __packed struct __ModeSense10Header
{
    __u16 ModeDataLength;
    __u8  MediumType;
    __u8  DeviceSpecPara;
    __u8  res[2];
    __u16 BlockDescLength;
} __ModeSense10Header_t;

//----------------------------------------------------------------------------
// CD_Rom
//----------------------------------------------------------------------------

/* Play audio starting at MSF and ending at MSF */
typedef struct _CDROM_PlayAudioMsf
{
    __u8 StartingM;
    __u8 StartingS;
    __u8 StartingF;
    __u8 EndingM;
    __u8 EndingS;
    __u8 EndingF;
} CDROM_PlayAudioMsf_t;

/*
 *
 * C/DVD Read TOC
 *
 */

/* Format code definitions for readtoc command */
#define  CDROM_READTOC_FORMAT_CODE_TOC			0x00
#define  CDROM_READTOC_FORMAT_CODE_Session		0x01
#define  CDROM_READTOC_FORMAT_CODE_FULL_TOC		0x02
#define  CDROM_READTOC_FORMAT_CODE_PMA			0x03
#define  CDROM_READTOC_FORMAT_CODE_ATIP			0x04
#define  CDROM_READTOC_FORMAT_CODE_CD_TEXT		0x05

/* Maximum CD Rom size */
#define  MAXIMUM_NUMBER_TRACKS 			100
#define  MAXIMUM_CDROM_SIZE 			804

#define MAXIMUM_NUMBER_TRACKS_LARGE 	0xAA

/* toc header */
typedef __packed struct _CDROM_TocHeader
{
    __u16 TocDataLength;
    __u8  FirstTrack;
    __u8  LastTrack;
} CDROM_TocHeader_t;

/* data track */
typedef __packed struct _CDROM_TrackData
{
    __u8 Reserved1;
    __u8 Control : 4;
    __u8 Adr 	 : 4;
    __u8 TrackNumber;
    __u8 Reserved2;

    __u32 Address;
} CDROM_TrackData_t;

/* MSF address Format. see SFF-8020i page78 Table27 */
typedef __packed struct _CDROM_MSF_ADDR
{
    __u8 Reserved1;
    __u8 Minute;
    __u8 Second;
    __u8 Frame;
} CDROM_MSF_ADDR_t;

/* MSF track */
typedef __packed struct _CDROM_TrackMSF
{
    __u8 Reserved1;
    __u8 Control : 4;
    __u8 Adr 	 : 4;
    __u8 TrackNumber;
    __u8 Reserved2;

    CDROM_MSF_ADDR_t MSF;
} CDROM_TrackMSF_t;

/*
 *
 * C/DVD mode sense
 *
 */
/* PageCode 1Ah. c/dvd power condition mode page format */
typedef __packed struct _CDROM_PowerConditionPage
{
    __u8 PageCode  : 6;
    __u8 reserved1 : 1;
    __u8 PS        : 1;

    __u8 PageLength;

    __u8 Standby   : 1;
    __u8 Idle 	   : 1;
    __u8 reserved2 : 6;

    __u32 IdleTimer;
    __u32 StandbyTimer;
} CDROM_PowerConditionPage_t;

/* PageCode 0Eh. cd audio control mode page format */
typedef __packed struct _CDROM_ParameterPage
{
    __u8 PageCode  : 6;
    __u8 reserved1 : 1;
    __u8 PS        : 1;

    __u8 PageLength;

    __u8 reserved2;
    __u8 InactivityTimeMultiplier 	: 4;
    __u8 reserved3 					: 4;
    __u16 NumberOfMSF_SUnitPerMUnit;
    __u16 NumberOfMSF_FUnitPerSUnit;
} CDROM_ParameterPage_t;

/* PageCode 0Eh. cd audio control mode page format */
typedef __packed struct _CDROM_AudioControlPage
{
    __u8 PageCode  : 6;
    __u8 reserved1 : 1;
    __u8 PS        : 1;

    __u8 PageLength;

    __u8 reserved2 : 1;
    __u8 SOTC      : 1;
    __u8 Immed	   : 1;
    __u8 reserved3 : 5;

    __u8 reserved4[3];
    __u8 Obsolete;

    __u8 Port0Selection : 4;
    __u8 reserved5		: 4;
    __u8 Port0Volume;

    __u8 Port1Selection : 4;
    __u8 reserved6		: 4;
    __u8 Port1Volume;

    __u8 Port2Selection : 4;
    __u8 reserved7		: 4;
    __u8 Port2Volume;

    __u8 Port3Selection : 4;
    __u8 reserved8		: 4;
    __u8 Port3Volume;
} CDROM_AudioControlPage_t;

/* PageCode 2Ah. c/dvd capabilities and mechanical status page. see SFF-8020i & SFF-8090 */
typedef __packed struct _CDROM_Capitilities
{
    __u8 PageCode  : 6;
    __u8 reserved1 : 1;
    __u8 PS        : 1;

    __u8 PageLength;

    /* Byte2. see SFF-8090 */
    __u8 CD_R_Rd	  : 1;
    __u8 CD_E_Rd	  : 1;
    __u8 Method2	  : 1;
    __u8 DVD_ROM_Read : 1;
    __u8 DVD_R_Read   : 1;
    __u8 DVD_RAM_Read : 1;
    __u8 reserved2    : 2;

    /* Byte3. see SFF-8090 */
    __u8 CD_R_Wr	  : 1;
    __u8 CD_E_Wr      : 1;
    __u8 TestWrite    : 1;
    __u8 reserved3    : 1;
    __u8 DVD_R_Write  : 1;
    __u8 DVD_RAM_Wr   : 1;
    __u8 reserved4    : 2;

    /* Byte4 */
    __u8 AudioPlay 		: 1;
    __u8 Composite		: 1;
    __u8 DigitalPort1	: 1;
    __u8 DigitalPort2	: 1;
    __u8 Mode2_Form1	: 1;
    __u8 Mode2_Form2	: 1;
    __u8 MultiSession	: 1;
    __u8 reserved5		: 1;

    /* Byte5 */
    __u8 CD_DA				: 1;
    __u8 DAAccu				: 1;
    __u8 RW_supported		: 1;
    __u8 RW_De_Interleaved 	: 1;
    __u8 C2_Pointers		: 1;
    __u8 ISRC				: 1;
    __u8 UPC				: 1;
    __u8 ReadBarCodeCapable	: 1;

    /* Byte6 */
    __u8 Lock					: 1;
    __u8 LockState				: 1;
    __u8 PreventJumper			: 1;
    __u8 Eject					: 1;
    __u8 reserved6				: 1;
    __u8 LoadingMechanismType	: 3;

    /* Byte7 */
    __u8 SeparateVolume			: 1;
    __u8 SeparateChannelMute	: 1;
    __u8 SupportsDiscPresent	: 1;
    __u8 SW_SlotSelection		: 1;
    __u8 SideChangeCapable		: 1;
    __u8 reserved7				: 3;

    __u16 MaximumSpeedSupport;				/* Byte8-9. see SFF-8020i	*/
    __u16 NumberOfVolumelevelsSupported;	/* Byte10-11 				*/
    __u16 BufferSizeSupportedByLogicUnit;	/* Byte12-13 				*/
    __u16 CurrentSpeedSelected;				/* Byte14-15 see SFF-8020i	*/

    __u8  ObsoleteReserved3;				/* Byte16 					*/

    /* Byte17 */
    __u8 reserved8	: 1;
    __u8 BCK		: 1;
    __u8 RCK		: 1;
    __u8 LSBF		: 1;
    __u8 Length		: 2;
    __u8 reserved9	: 2;

    __u16 ObsoleteReserved4;				/* Byte18-19 	*/
    __u16 CopyManagementRevisionsupport;	/* Byte20-21 	*/
    __u8  reserved10;
    __u8  reserved11;
} CDROM_Capitilities_t;

/*
 *
 * C/DVD envent status
 *
 */
#define  CDROM_GET_EVENT_CMD_MAX_RETURN_DATA_LEN		0xff /* 通常都是8个byte */

/* Notification class request */
#define  CDROM_NOTIFICATION_CLASS_REQUEST_POWER_MANAGEMENT	0x04
#define  CDROM_NOTIFICATION_CLASS_REQUEST_MEDIA_STATUS		0x10
#define  CDROM_NOTIFICATION_CLASS_REQUEST_DEVICE_BUSY		0x40

/* Notification class field */
#define  CDROM_NOTIFICATION_CLASS_FIELD_POWER_MANAGEMENT	0x02
#define  CDROM_NOTIFICATION_CLASS_FIELD_MEDIA_STATUS		0x04
#define  CDROM_NOTIFICATION_CLASS_FIELD_DEVICE_BUSY			0x06

/* Event Status Header */
typedef __packed struct _CDROM_EventHeader
{
    __u16 EventDataLength;

    __u8 NotificationClass 	: 3;
    __u8 reserved1 			: 4;
    __u8 NEA 				: 1;

    __u8 SupportEventClass;
} CDROM_EventHeader_t;

/* Power Management Status Class Returned Data */
#define  CDROM_POWER_EVENT_NOCHG			0x00
#define  CDROM_POWER_EVENT_PWRCHG_SUCC		0x01
#define  CDROM_POWER_EVENT_PWRCHG_FAIL		0x02

#define  CDROM_POWER_STATUS_ACTIVE			0x01
#define  CDROM_POWER_STATUS_IDLE			0x02
#define  CDROM_POWER_STATUS_STANDBY			0x03

typedef __packed struct _CDROM_PowerStatus
{
    __u8 PowerEvent : 4;
    __u8 reserved1  : 4;

    __u8 PowerStatus;
    __u8 reserved2;
    __u8 reserved3;
} CDROM_PowerStatus_t;

/* Media Status Class Returned Data */
#define  CDROM_MEDIA_STATUS_EJECT_REQUEST	0x01
#define  CDROM_MEDIA_STATUS_NEW_MEDIA		0x02
#define  CDROM_MEDIA_STATUS_MEDIA_REMOVAL	0x03

typedef __packed struct _CDROM_MediaStatus
{
    __u8 MediaStatus : 4;
    __u8 reserved1   : 4;

    __u8 DoorOpen 	 : 1;
    __u8 MediaPesent : 1;
    __u8 reserved 	 : 6;

    __u8 StartSlot;
    __u8 EndSlot;
} CDROM_MediaStatus_t;

/* Device Busy Status Class Returned Data */
#define  CDROM_DEVICE_BUSY_EVENT_TIMEOUT_OCCURED	0x03

#define  CDROM_DEVICE_BUSY_STATUS_NOEVENT			0x00
#define  CDROM_DEVICE_BUSY_STATUS_POWER				0x01
#define  CDROM_DEVICE_BUSY_STATUS_IMMEDIATE			0x02
#define  CDROM_DEVICE_BUSY_STATUS_DEFERED			0x03

typedef __packed struct _CDROM_DeviceBusyStatus
{
    __u8  DeviceBusyEvent 	: 4;
    __u8  reserved1   		: 4;

    __u8  DeviceBusyStatus;
    __u16 Time;
} CDROM_DeviceBusyStatus_t;

/*
 *
 * C/DVD Get Configuration
 *
 */
#define  CDROM_FEATURE_LIST_PROFILE_LIST						0x00
#define  CDROM_FEATURE_LIST_CORE								0x01
#define  CDROM_FEATURE_LIST_MORPHING							0x02
#define  CDROM_FEATURE_LIST_REMOVABLE_MEDIUM					0x03
#define  CDROM_FEATURE_LIST_WRITE_PROTECT						0x04
#define  CDROM_FEATURE_LIST_RANDOM_READABLE						0x10
#define  CDROM_FEATURE_LIST_MULTIREAD							0x1D
#define  CDROM_FEATURE_LIST_CD_READ								0x1E
#define  CDROM_FEATURE_LIST_DVD_READ							0x1F
#define  CDROM_FEATURE_LIST_RANDOM_WRITABLE						0x20
#define  CDROM_FEATURE_LIST_INCREMENTAL_STREAMING_WRITABLE		0x21
#define  CDROM_FEATURE_LIST_FORMATTABLE							0x23
#define  CDROM_FEATURE_LIST_HARDWARE_DEFECT_MANAGEMENT			0x24
#define  CDROM_FEATURE_LIST_WRITE_ONCE							0x25
#define  CDROM_FEATURE_LIST_RESTRICTED_OVERWRITE				0x26
#define  CDROM_FEATURE_LIST_CD_RW_CAV_WRITE						0x27
#define  CDROM_FEATURE_LIST_MRW									0x28
#define  CDROM_FEATURE_LIST_ENHABCED_DEFECT_REPORTING			0x29
#define  CDROM_FEATURE_LIST_DVD_RW								0x2A
#define  CDROM_FEATURE_LIST_DVD_R								0x2B
#define  CDROM_FEATURE_LIST_RIGID_RESTRICTED_OVERWRITE			0x2C
#define  CDROM_FEATURE_LIST_CD_TRACK_AT_ONCE					0x2D
#define  CDROM_FEATURE_LIST_CD_MASTERING						0x2E
#define  CDROM_FEATURE_LIST_DVD_R_RW_WRITE						0x2F
#define  CDROM_FEATURE_LIST_LAYER_JUMP_RECORDING				0x33
#define  CDROM_FEATURE_LIST_LJ_RIGID_RESTRICTED_OVERWRITE		0x34
#define  CDROM_FEATURE_LIST_STOP_LONG_OPERATION					0x35
#define  CDROM_FEATURE_LIST_CD_RW_MEDIA_WRITE_SUPPORT			0x37
#define  CDROM_FEATURE_LIST_BD_R_PSEUDO_OVERWRITE_FEATURE		0x38
#define  CDROM_FEATURE_LIST_DVD_RW_DUAL_LAYER					0x3A
#define  CDROM_FEATURE_LIST_DVD_R_DUAL_LAYER					0x3B
#define  CDROM_FEATURE_LIST_BD_READ								0x40
#define  CDROM_FEATURE_LIST_BD_WRITE							0x41
#define  CDROM_FEATURE_LIST_TSR									0x42
#define  CDROM_FEATURE_LIST_HD_DVD_READ							0x50
#define  CDROM_FEATURE_LIST_HD_DVD_WRITE						0x51
#define  CDROM_FEATURE_LIST_HD_DVD_RW_FRAGMENT_RECORDING		0x52
#define  CDROM_FEATURE_LIST_HYBRID_DISC							0x80
#define  CDROM_FEATURE_LIST_POWER_MANAGEMENT					0x100
#define  CDROM_FEATURE_LIST_SMART								0x101
#define  CDROM_FEATURE_LIST_EMBEDDED_CHANGER					0x102
#define  CDROM_FEATURE_LIST_CD_AUDIO_ANALOG_PLAY				0x103
#define  CDROM_FEATURE_LIST_MICROCODE_UPGRADE					0x104
#define  CDROM_FEATURE_LIST_TIMEOUT								0x105
#define  CDROM_FEATURE_LIST_DVD_CSS								0x106
#define  CDROM_FEATURE_LIST_REAL_TIME_STREAMING					0x107
#define  CDROM_FEATURE_LIST_LOGICAL_UNIT_SERIAL_NUMBER			0x108
#define  CDROM_FEATURE_LIST_MEDIA_SERIAL_NUMBER					0x109
#define  CDROM_FEATURE_LIST_DISC_CONTROL_BLOCKS					0x10A
#define  CDROM_FEATURE_LIST_DVD_CPRM							0x10B
#define  CDROM_FEATURE_LIST_FIRMWARE_INFORMATION				0x10C
#define  CDROM_FEATURE_LIST_AACS								0x10D
#define  CDROM_FEATURE_LIST_DVD_CSS_MANAGED_RECORDING			0x10E
#define  CDROM_FEATURE_LIST_VCPS								0x110
#define  CDROM_FEATURE_LIST_SECUR_DISC							0x113

/*
 *
 * C/DVD Read Disc Information
 *
 */

/* Data Type Field definition */
#define  CDROM_DISC_INFO_STANDARD				0x00
#define  CDROM_DISC_INFO_TRACK_RESOURCES		0x00
#define  CDROM_DISC_INFO_POW_RESOURCES			0x00

/* C/DVD Disc Information Block Data */
typedef __packed struct _CDROM_DiscInfo
{
    __u16 DataInformationLength;

    __u8  DiscStatus 				: 2;
    __u8  StatusOfLastSession		: 2;
    __u8  Erasable					: 1;
    __u8  DiscInformationDataType	: 3;

    __u8  FirstTrackNumberOnDisc;
    __u8  NumberOfSessions_LSB;
    __u8  FirstTrackNumberInLastSession_LSB;
    __u8  LastTrackNumberInLastSession_LSB;

    __u8  BG_FormatStatus	: 2;
    __u8  Dbit				: 1;
    __u8  reserved1			: 1;
    __u8  DAC_V				: 1;
    __u8  URU				: 1;
    __u8  DBC_V				: 1;
    __u8  DID_V				: 1;

    __u8  DiscType;
    __u8  NumberOfSessions_MSB;
    __u8  FirstTrackNumberInLastSession_MSB;
    __u8  LastTrackNumberInLastSession_MSB;

    __u32 DiscIdentification;
    __u32 LeadInStartTimeOfLastSession_MSF;
    __u32 LastPossibleStartTimeForStartOfLeadOut_MSF;
    __u8  DiscBarCode[8];
    __u8  DiscApplicationCode;
    __u8  NumberOfOPCTableEntries;

    /* OPCTableEntries */
} CDROM_DiscInfo_t;

/* Status of last session field definition */
#define  CDROM_DISC_INFO_STATUS_OF_LAST_SESSION_EMPTY			0x00
#define  CDROM_DISC_INFO_STATUS_OF_LAST_SESSION_IMCOMPLETE		0x01
#define  CDROM_DISC_INFO_STATUS_OF_LAST_SESSION_DAMAGED_BOARDER	0x02
#define  CDROM_DISC_INFO_STATUS_OF_LAST_SESSION_COMPLETE		0x03

/* Disc status field definition */
#define  CDROM_DISC_INFO_DISC_STATUS_EMPTY						0x00
#define  CDROM_DISC_INFO_DISC_STATUS_IMCOMPLETE					0x01
#define  CDROM_DISC_INFO_DISC_STATUS_COMPLETE					0x02
#define  CDROM_DISC_INFO_DISC_STATUS_FINALIZATION				0x03

/* BG foramt status field definition */
#define  CDROM_DISC_INFO_BG_FORMAT_STATUS_EMPTY					0x00
#define  CDROM_DISC_INFO_BG_FORMAT_STATUS_NOT_RUNNING			0x01
#define  CDROM_DISC_INFO_BG_FORMAT_STATUS_RUNNING				0x02
#define  CDROM_DISC_INFO_BG_FORMAT_STATUS_COMPLETE				0x03

/* Disk type field definition */
#define  CDROM_DISC_INFO_DISC_TYPE_CDDA_CDROM					0x00
#define  CDROM_DISC_INFO_DISC_TYPE_CDI							0x10
#define  CDROM_DISC_INFO_DISC_TYPE_CD_XA						0x20


/*
 *
 * C/DVD Read Track Information
 *
 */
#define  CDROM_TRACK_INFO_ADDRESS_NUMBER_TYPE_LBA				0x00
#define  CDROM_TRACK_INFO_ADDRESS_NUMBER_TYPE_TRACK_NUMBER		0x01
#define  CDROM_TRACK_INFO_ADDRESS_NUMBER_TYPE_BORDER_NUMBER		0x02

typedef __packed struct _CDROM_TrackInfo
{
    __u16 TrackInformationlength;
    __u8  TrackNumber_LSB;
    __u8  SessionNumber_LSB;
    __u8  reserved1;

    __u8  TrackMode : 4;
    __u8  Copy		: 1;
    __u8  Damage	: 1;
    __u8  LJRS		: 2;

    __u8  DataMode 	: 4;
    __u8  FP		: 1;
    __u8  Packet	: 1;
    __u8  Blank		: 1;
    __u8  RT		: 1;

    __u8  NWA_V		: 1;		/* next write address valid 	*/
    __u8  LRA_V		: 1;		/* last recorded address valid 	*/
    __u8  reserved2	: 6;

    __u32 TrackStartAddress;
    __u32 NextWritableAddress;
    __u32 FreeBlocks;
    __u32 FixedPacketSize;
    __u32 TrackSizeEndAddress;
    __u32 LastRecordedAddress;
    __u8  TrackNumber_MSB;
    __u8  SessionNumber_MSB;
    __u8  reserved3;
    __u32 ReadCompatibilityLBA;
    __u32 NextLayerJumpAddress;
    __u32 LastLayerJumpAddress;
} CDROM_TrackInfo_t;









//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
__s32 ScsiInquery(__mscLun_t *mscLun, void *buffer, __u32 buffer_len);
__s32 ScsiReadCapacity(__mscLun_t *mscLun);
__s32 ScsiModeSense6(__mscLun_t *mscLun,
                     __u32 PageCode,
                     __u32 DBD,
                     void *buffer,
                     __u32 buffer_len,
                     __u32 *ActLen);
__s32 ScsiModeSense10(__mscLun_t *mscLun,
                      __u32 PageCode,
                      __u32 DBD,
                      void *buffer,
                      __u32 buffer_len,
                      __u32 *ActLen);
__s32 ScsiTestUnitReady(__mscLun_t *mscLun);
__s32 ScsiRequestSense(__mscLun_t *mscLun, void *buffer, __u32 buffer_len);
__s32 ScsiGetSenseData(__mscLun_t *mscLun);
__s32 ScsiStartStopUnit(__mscLun_t *mscLun, __u32 LoEj, __u32 Start);
__s32 ScsiSynchronizeCache(__mscLun_t *mscLun);
__s32 ScsiRead(__mscLun_t *mscLun,
               __u32 Version,
               __u32 StartSector,
               __u32 NumSectors,
               void *buffer,
               __u32 buffer_len);
__s32 ScsiWrite(__mscLun_t *mscLun,
                __u32 Version,
                __u32 StartSector,
                __u32 NumSectors,
                void *buffer,
                __u32 buffer_len);
__s32 ScsiPreventAllowMedium(__mscLun_t *mscLun, __u32 Persistent, __u32 Prevent);

/* C/DVD */
__s32 ScsiCDPauseResume(__mscLun_t *mscLun, __u32 Resume);
__s32 ScsiCDPlayAudio10(__mscLun_t *mscLun, __u32 Lba, __u32 Lenght);
__s32 ScsiCDPlayAudioMSF(__mscLun_t *mscLun, CDROM_PlayAudioMsf_t *msf);
__s32 ScsiCDReadToc(__mscLun_t *mscLun,
                    __u32 MSF,
                    __u32 Format,
                    __u32 StartingNumber,
                    void *Buffer,
                    __u32 BufferLen);
__s32 ScsiCDStopPlayScan(__mscLun_t *mscLun);
__s32 ScsiCDReadDiscInfo(__mscLun_t *mscLun,
                         __u32 DataType,
                         void *Buffer,
                         __u32 BufferLen,
                         __u32 *ActLen);
__s32 ScsiCDReadTrackInfo(__mscLun_t *mscLun,
                          __u32 Open,
                          __u32 Address_or_Number,
                          __u32 LogicalBlockAddress,
                          void *Buffer,
                          __u32 BufferLen,
                          __u32 *ActLen);
__s32 ScsiDvdGetEventStatus(__mscLun_t *mscLun,
                            __u32 Immed,
                            __u32 ClassRequest,
                            void *Buffer,
                            __u32 BufferLen);
__s32 ScsiDvdLoad(__mscLun_t *mscLun,
                  __u32 Immed,
                  __u32 Start,
                  __u32 LoUnlo,
                  __u32 Slot);
__s32 ScsiCDSetSpeed(__mscLun_t *mscLun, __u32 ReadSpeed, __u32 WriteSpeed);
__s32 ScsiDVDGetConfiguration(__mscLun_t *mscLun,
                              __u32 RT,
                              __u32 StartingFeatureNumber,
                              void *Buffer,
                              __u32 BufferLen);


#endif   //__SCSI2_H__





