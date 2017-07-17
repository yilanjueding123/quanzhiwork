/*
********************************************************************************
*                                USB Hid Driver
*
*                (c) Copyright 2006-2010, All winners Co,Ld.
*                        All Right Reserved
*
* FileName		:  HidProtocol.c
*
* Author		:  Javen
*
* Date			:  2010/06/02
*
* Description	:  Hid 协议
*
* Others		:  NULL
*
* History:
*		<time> 			<author>	 <version >		<desc>
*	   2010.06.02		Javen			1.0			build this file
*
********************************************************************************
*/

#include  "usb_host_config.h"
#include  "usb_host_base_types.h"
#include  "usb_os_platform.h"
#include  "error.h"

#include  "usb_host_common.h"
#include  "usb_msg.h"
#include  "urb.h"

#include  "HidSpec.h"
#include  "Hid_i.h"
#include  "HidProtocol.h"
#include  "HidTransport.h"

#define  USBH_HID_PRINT_REPORT

#ifdef USBH_HID_PRINT_REPORT

static void print_field(usbHidField_t *Field)
{
    __u32 i = 0;

    DMSG_INFO("\n------------------field---------------\n");
    DMSG_INFO("Index            = %d\n", Field->Index);
    DMSG_INFO("HidReport        = %x\n", Field->HidReport);
    DMSG_INFO("physical         = %x\n", Field->physical);
    DMSG_INFO("logical          = %x\n", Field->logical);
    DMSG_INFO("application      = %x\n", Field->application);

    for (i = 0; i < Field->maxusage; i++)
    {
        DMSG_INFO("\n");
        DMSG_INFO("usage[%d].hid              = %x\n", i, Field->usage[i].hid);
        DMSG_INFO("usage[%d].collection_index = %x\n", i, Field->usage[i].collection_index);
        DMSG_INFO("usage[%d].code             = %x\n", i, Field->usage[i].code);
        DMSG_INFO("usage[%d].type             = %x\n", i, Field->usage[i].type);
        DMSG_INFO("usage[%d].hat_min          = %x\n", i, Field->usage[i].hat_min);
        DMSG_INFO("usage[%d].hat_max          = %x\n", i, Field->usage[i].hat_max);
        DMSG_INFO("usage[%d].hat_dir          = %x\n", i, Field->usage[i].hat_dir);
    }

    DMSG_INFO("usage            = %x\n", Field->usage);
    DMSG_INFO("maxusage         = %x\n", Field->maxusage);
    DMSG_INFO("flags            = %x\n", Field->flags);
    DMSG_INFO("report_offset    = %x\n", Field->report_offset);
    DMSG_INFO("report_size      = %x\n", Field->report_size);
    DMSG_INFO("report_count     = %x\n", Field->report_count);
    DMSG_INFO("report_type      = %x\n", Field->report_type);
    DMSG_INFO("value            = %x\n", Field->value);
    DMSG_INFO("logical_maximum  = %x\n", Field->logical_maximum);
    DMSG_INFO("logical_minimum  = %x\n", Field->logical_minimum);
    DMSG_INFO("physical_maximum = %x\n", Field->physical_maximum);
    DMSG_INFO("physical_minimum = %x\n", Field->physical_minimum);
    DMSG_INFO("unit_exponent    = %x\n", Field->unit_exponent);
    DMSG_INFO("unit             = %x\n", Field->unit);
    DMSG_INFO("--------------------------------------\n");
}

static void print_item(usbHidItem_t *item)
{
    DMSG_INFO("\n------------------item-----------------\n");
    DMSG_INFO("format = %d\n", item->format);
    DMSG_INFO("size   = %d\n", item->size);
    DMSG_INFO("type   = %d\n", item->type);
    DMSG_INFO("tag    = %d\n", item->tag);
    DMSG_INFO("data   = %x\n", item->data);
    DMSG_INFO("--------------------------------------\n");

}

static void print_Report(usbHidReport_t *Report)
{
    __u32 i = 0;
    __u32 j = 0;

    if(Report == NULL)
    {
        DMSG_PANIC("Report is NULL\n");
        return;
    }

    DMSG_INFO("\n");

    DMSG_INFO("\n Report num  = %d\n", i);
    DMSG_INFO("Report Id       = %d\n", Report->Id);
    DMSG_INFO("Report Type     = %d\n", Report->Type);
    DMSG_INFO("Report Maxfield = %d\n", Report->Maxfield);

    for(j = 0; j < Report->Maxfield; j++)
    {
        DMSG_INFO("\n field num  = %d\n", j);
        print_field(Report->Field[j]);
    }

    DMSG_INFO("Report Size = %d\n", Report->Size);
}

static void print_ReportEnum(HidDev_t *HidDev)
{
    __u32 i = 0;
    __u32 j = 0;
    usbHidReportEnum_t *ReportEnum = NULL;

    DMSG_INFO("\n\n-----------print_ReportEnum--------\n");
    for(i = 0; i < USB_HID_REPORT_TYPES; i++)
    {
        ReportEnum = &HidDev->HidReportEnum[i];
        if(ReportEnum == NULL)
        {
            continue;
        }

        DMSG_INFO("\n ReportEnum num  = %d\n", i);
        DMSG_INFO("ReportEnum numbered = %d\n", ReportEnum->numbered);
        DMSG_INFO("ReportEnum ReportNum = %d\n", ReportEnum->ReportNum);

        for(j = 0; j < USB_HID_REPORT_MAX_NUM; j++)
        {
            print_Report(ReportEnum->Report[j]);
        }
    }
    DMSG_INFO("-----------print_ReportEnum--------\n\n");
}

static void print_global(usbHidGlobalItems_t *Global)
{
    DMSG_INFO("\n");
    DMSG_INFO("UsagePage       = %x\n", Global->UsagePage);
    DMSG_INFO("LogicalMaximum  = %d\n", Global->LogicalMaximum);
    DMSG_INFO("LogicalMinimum  = %d\n", Global->LogicalMinimum);
    DMSG_INFO("PhysicalMinimum = %d\n", Global->PhysicalMinimum);
    DMSG_INFO("PhysicalMaximum = %d\n", Global->PhysicalMaximum);
    DMSG_INFO("UnitExponent    = %d\n", Global->UnitExponent);
    DMSG_INFO("Unit            = %d\n", Global->Unit);
    DMSG_INFO("ReportSize      = %d\n", Global->ReportSize);
    DMSG_INFO("ReportId        = %d\n", Global->ReportId);
    DMSG_INFO("ReportCount     = %d\n", Global->ReportCount);
    DMSG_INFO("Push            = %x\n", Global->Push);
    DMSG_INFO("Pop             = %x\n", Global->Pop);
}

static void print_local(usbHidLocalItems_t *Local)
{
    __u32 i = 0;

    if(Local == NULL)
    {
        DMSG_PANIC("Local is NULL\n");
        return;
    }

    for(i = 0; i < Local->usage_index; i++)
    {
        DMSG_INFO("Local->usage[%d] = %x\n", i, Local->usage[i]);
    }
    /*
    	 for(i = 0; i < USB_HID_MAX_USAGES; i++){
    	 	DMSG_INFO("Local->collection_index[%d] = %x\n", i, Local->collection_index[i]);
    	 }
    */
    DMSG_INFO("Local->usage_index      = %d\n", Local->usage_index);
    DMSG_INFO("Local->usage_minimum    = %d\n", Local->usage_minimum);
    DMSG_INFO("Local->delimiter_depth  = %d\n", Local->delimiter_depth);
    DMSG_INFO("Local->delimiter_branch = %d\n", Local->delimiter_branch);
}

static void print_parser(usbHidParser_t *Parser)
{
    __u32 i = 0;

    DMSG_INFO("\n");
    DMSG_INFO("\n");
    DMSG_INFO("--------------print_parser-------------\n");
    print_global(&Parser->global);
    for(i = 0; i < USB_HID_GLOBAL_STACK_SIZE; i++)
    {
        print_global(&Parser->global_stack[i]);
    }

    DMSG_INFO("global_stack_ptr = %d\n", Parser->global_stack_ptr);
    print_local(&Parser->local);

    for(i = 0; i < USB_HID_COLLECTION_STACK_SIZE; i++)
    {
        DMSG_INFO("collection_stack[%d] = %x\n", i, Parser->collection_stack[i]);
    }

    DMSG_INFO("global_stack_ptr = %d\n", Parser->collection_stack_ptr);
    DMSG_INFO("--------------------------------------\n");
    DMSG_INFO("\n");
}

#else

static void print_field(usbHidField_t *Field)
{

}

static void print_item(usbHidItem_t *item)
{

}

static void print_ReportEnum(HidDev_t *HidDev)
{

}

static void print_local(usbHidLocalItems_t *Local)
{

}

static void print_parser(usbHidParser_t *Parser)
{

}

#endif

/*
*******************************************************************************
*                     HidGetIdle
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
__s32 HidGetClassDescriptor(HidDev_t *HidDev,
                            __u32 InterfaceNo,
                            __u32 DescriptorType,
                            void *Buffer,
                            __u32 TransferLength)
{
    __s32 ret = 0;
    __u32 retries = 4;  /* retry */

    do
    {
        ret = usb_control_msg(HidDev->pusb_dev,
                              usb_rcvctrlpipe(HidDev->pusb_dev, 0),
                              USB_REQ_GET_DESCRIPTOR,
                              USB_RECIP_INTERFACE | USB_TYPE_STANDARD | USB_DIR_IN,
                              (DescriptorType << 8),
                              InterfaceNo,
                              Buffer,
                              TransferLength,
                              USB_CTRL_GET_TIMEOUT);
        retries--;
    }
    while (ret < TransferLength && retries);

    if(ret == TransferLength)
    {
        return USB_ERR_SUCCESS;
    }
    else
    {
        return USB_ERR_COMMAND_EXECUTE_FAILED;
    }
}

/*
*******************************************************************************
*                     HidGetIdle
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
__s32 HidGetIdle(HidDev_t *HidDev,
                 __u32 InterfaceNo,
                 __u32 ReportId,
                 void *IdleRate)
{
    DMSG_PANIC("ERR: HidGetIdle not support\n");

    return USB_ERR_SUCCESS;
}

/*
*******************************************************************************
*                     HidSetIlde
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
__s32 HidSetIlde(HidDev_t *HidDev,
                 __u32 InterfaceNo,
                 __u32 Duration,
                 __u32 ReportId)
{
    __s32 ret = 0;

    ret = usb_control_msg(HidDev->pusb_dev,
                          usb_sndctrlpipe(HidDev->pusb_dev, 0),
                          USB_HID_REQ_SET_IDLE,
                          USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
                          (Duration << 8) | ReportId,
                          InterfaceNo,
                          NULL,
                          0,
                          USB_CTRL_SET_TIMEOUT);
    if(ret < 0)
    {
        DMSG_PANIC("ERR: HidSetIlde: usb_control_msg failed\n");
        return USB_ERR_COMMAND_EXECUTE_FAILED;
    }

    return USB_ERR_SUCCESS;
}

/*
*******************************************************************************
*                     HidGetReport
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
__s32 HidGetReport(HidDev_t *HidDev,
                   __u32 InterfaceNo,
                   __u32 ReportType,
                   __u32 ReportId,
                   __u32 ReportLength,
                   void  *ReportData)
{
    __s32 ret = 0;

    ret = usb_control_msg(HidDev->pusb_dev,
                          usb_rcvctrlpipe(HidDev->pusb_dev, 0),
                          USB_HID_HID_REQ_GET_REPORT,
                          USB_RECIP_INTERFACE | USB_TYPE_CLASS | USB_DIR_IN,
                          (ReportType << 8) | ReportId,
                          InterfaceNo,
                          ReportData,
                          ReportLength,
                          USB_CTRL_GET_TIMEOUT);

    if(ret == ReportLength)
    {
        return USB_ERR_SUCCESS;
    }
    else
    {
        return USB_ERR_COMMAND_EXECUTE_FAILED;
    }
}

/*
*******************************************************************************
*                     HidSetReport
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
__s32 HidSetReport(HidDev_t *HidDev,
                   __u32 InterfaceNo,
                   __u32 ReportType,
                   __u32 ReportId,
                   __u32 ReportLength,
                   void *ReportData)
{
    DMSG_PANIC("ERR: HidGetIdle not support\n");

    return USB_ERR_SUCCESS;
}

/*
*******************************************************************************
*                     HidGetProtocol
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
__s32 HidGetProtocol(HidDev_t *HidDev, __u32 InterfaceNo, void *ProtocolData)
{
    DMSG_PANIC("ERR: HidGetIdle not support\n");

    return USB_ERR_SUCCESS;
}

/*
*******************************************************************************
*                     HidSetProtocol
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
__s32 HidSetProtocol(HidDev_t *HidDev, __u32 InterfaceNo, __u32 Protocoltype)
{
    DMSG_PANIC("ERR: HidGetIdle not support\n");

    return USB_ERR_SUCCESS;
}

/*
*******************************************************************************
*                     HidRegisterReport
*
* Description:
*     Register a new report for a device.
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
static usbHidReport_t *HidRegisterReport(HidDev_t *HidDev, __u32 type, __u32 id)
{
    usbHidReportEnum_t *ReportEnum = HidDev->HidReportEnum + type;
    usbHidReport_t *Report = NULL;

    if (ReportEnum->Report[id])
    {
        return ReportEnum->Report[id];
    }

    Report = USB_OS_MALLOC(sizeof(usbHidReport_t),
                           USB_MEM_FILE_TRIGGER,
                           USB_MEM_LINE_TRIGGER);
    if (Report == NULL)
    {
        DMSG_PANIC("ERR: malloc failed\n");
        return NULL;
    }

    ReportEnum->ReportNum  += 1;

    USB_OS_MEMSET(Report, 0, sizeof(usbHidReport_t));

    if (id != 0)
    {
        ReportEnum->numbered = 1;
    }

    Report->Id   			= id;
    Report->Type			= type;
    Report->Size 			= 0;
    ReportEnum->Report[id] 	= Report;

    return Report;
}

/*
*******************************************************************************
*                     HidRegisterField
*
* Description:
*	  Register a new field for this report.
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
static usbHidField_t *HidRegisterField(usbHidReport_t *Report, __u32 Usages, __u32 Values)
{
    usbHidField_t *Field = NULL;

    if (Report->Maxfield >= USB_HID_MAX_FIELDS)
    {
        DMSG_PANIC("ERR: too many fields in report\n");
        return NULL;
    }

    Field = (usbHidField_t *)USB_OS_MALLOC(sizeof(usbHidField_t),
                                           USB_MEM_FILE_TRIGGER,
                                           USB_MEM_LINE_TRIGGER);
    if (Field == NULL)
    {
        DMSG_PANIC("ERR: malloc failed\n");
        return NULL;
    }

    USB_OS_MEMSET(Field, 0, sizeof(usbHidField_t));

    Field->usage = (usbHidUsage_t *)USB_OS_MALLOC(Usages * sizeof(usbHidUsage_t),
                   USB_MEM_FILE_TRIGGER,
                   USB_MEM_LINE_TRIGGER);
    if (Field->usage == NULL)
    {
        DMSG_PANIC("ERR: malloc failed\n");
        goto ERR;
    }
    USB_OS_MEMSET(Field->usage, 0, Usages * sizeof(usbHidUsage_t));

    Field->value = (__u32 *)USB_OS_MALLOC(Values * sizeof(__u32),
                                          USB_MEM_FILE_TRIGGER,
                                          USB_MEM_LINE_TRIGGER);
    if (Field->value == NULL)
    {
        DMSG_PANIC("ERR: malloc failed\n");
        goto ERR;
    }
    USB_OS_MEMSET(Field->value, 0, Values * sizeof(__u32));

    Field->Index 				= Report->Maxfield++;
    Report->Field[Field->Index] = Field;
    Field->HidReport 			= Report;

    return Field;

ERR:
    if(Field->usage)
    {
        USB_OS_FREE(Field->usage);
        Field->usage = NULL;
    }

    USB_OS_FREE(Field);
    Field = NULL;

    return NULL;
}

/*
*******************************************************************************
*                     open_collection
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
static int open_collection(usbHidParser_t *parser, unsigned type)
{
    usbHidCollectionItems_t *collection = NULL;
    __u32 usage = 0;

    // 从取出最先进入local的usage
    // 在处理local usage的函数hid_parser_local==>HID_LOCAL_ITEM_TAG_USAGE中
    // data = (parser->global.usage_page local.usage[0];
    // 高16位对应global.usage_page,低16位才对应local的usage
    // 比如报告描述符的前6个字节数据如下：
    // parser->global.usage_page = 0x01;  0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    // parser->local.usage[0] = (0x01 local.usage[0] = (0x01 local.usage[0];
    usage = parser->local.usage[0];

    if (parser->collection_stack_ptr == USB_HID_COLLECTION_STACK_SIZE)
    {
        DMSG_PANIC("ERR: collection stack overflow\n");
        return -1;
    }

    if (parser->HidDev->maxcollection == parser->HidDev->collection_size)
    {
        collection = USB_OS_MALLOC(sizeof(usbHidCollectionItems_t) * parser->HidDev->collection_size * 2,
                                   USB_MEM_FILE_TRIGGER,
                                   USB_MEM_LINE_TRIGGER);
        if (collection == NULL)
        {
            DMSG_PANIC("ERR: failed to reallocate collection array\n");
            return -1;
        }

        USB_OS_MEMSET(collection, 0, sizeof(usbHidCollectionItems_t) * parser->HidDev->collection_size * 2);

        USB_OS_MEMCPY(collection,
                      parser->HidDev->collection,
                      sizeof(usbHidCollectionItems_t) * parser->HidDev->collection_size);
        USB_OS_MEMSET(collection + parser->HidDev->collection_size,
                      0,
                      sizeof(usbHidCollectionItems_t) * parser->HidDev->collection_size);

        USB_OS_FREE(parser->HidDev->collection);
        parser->HidDev->collection = collection;
        parser->HidDev->collection_size *= 2;
    }

    parser->collection_stack[parser->collection_stack_ptr++] = parser->HidDev->maxcollection;
    collection 			= parser->HidDev->collection + parser->HidDev->maxcollection++;
    collection->Type 	= type;
    collection->Usage 	= usage;
    collection->Level 	= parser->collection_stack_ptr - 1;

    if (type == USB_HID_COLLECTION_APPLICATION)
    {
        parser->HidDev->maxapplication++;
    }

    return 0;
}

/*
*******************************************************************************
*                     close_collection
*
* Description:
*	  Close a collection.
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
static int close_collection(usbHidParser_t *parser)
{
    if (!parser->collection_stack_ptr)
    {
        DMSG_PANIC("ERR: collection stack underflow\n");
        return -1;
    }

    parser->collection_stack_ptr--;

    return 0;
}

/*
*******************************************************************************
*                     hid_lookup_collection
*
* Description:
*     Climb up the stack, search for the specified collection type
* and return the usage.
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
static unsigned hid_lookup_collection(usbHidParser_t *parser, __u32 type)
{
    __s32 n = 0;

    for (n = parser->collection_stack_ptr - 1; n >= 0; n--)
    {
        if (parser->HidDev->collection[parser->collection_stack[n]].Type == type)
        {
            return parser->HidDev->collection[parser->collection_stack[n]].Usage;
        }
    }

    return 0; /* we know nothing about this usage type */
}


/*
*******************************************************************************
*                     HidAddUsage
*
* Description:
*     Add a usage to the temporary parser table.
*
* Parameters:
*     Parser : output.
*     Usage  : input.
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
static __s32 HidAddUsage(usbHidParser_t *Parser, __u32 Usage)
{
    if (Parser->local.usage_index >= USB_HID_MAX_USAGES)
    {
        DMSG_PANIC("ERR: usage index exceeded\n");
        return USB_ERR_DATA_OVERFLOW;
    }

    Parser->local.usage[Parser->local.usage_index] = Usage;
    Parser->local.collection_index[Parser->local.usage_index] =
        Parser->collection_stack_ptr ?
        Parser->collection_stack[Parser->collection_stack_ptr - 1] : 0;
    Parser->local.usage_index++;

    return 0;
}

/*
*******************************************************************************
*                     HidAddField
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
static __s32 HidAddField(usbHidParser_t *Parser, __u32 ReportType, __u32 Flags)
{
    usbHidReport_t *Report = NULL;
    usbHidField_t *Field = NULL;
    __s32 usages = 0;
    __u32 offset = 0;
    __s32 i = 0;

    Report = HidRegisterReport(Parser->HidDev, ReportType, Parser->global.ReportId);
    if (Report == NULL)
    {
        DMSG_PANIC("ERR: hid_register_report failed\n");
        return -1;
    }

    if (Parser->global.LogicalMaximum < Parser->global.LogicalMinimum)
    {
        DMSG_PANIC("ERR: logical range invalid (%d, %d)\n", Parser->global.LogicalMinimum,
                   Parser->global.LogicalMaximum);
        return USB_ERR_BAD_ARGUMENTS;
    }

    offset = Report->Size;
    Report->Size += Parser->global.ReportSize * Parser->global.ReportCount;

    if (!Parser->local.usage_index)  /* Ignore padding fields */
    {
        return 0;
    }

    usages = max(Parser->local.usage_index, Parser->global.ReportCount);

    Field = HidRegisterField(Report, usages, Parser->global.ReportCount);
    if (Field == NULL)
    {
        DMSG_PANIC("ERR: HidRegisterField failed\n");
        return 0;
    }

    Field->physical = hid_lookup_collection(Parser, USB_HID_COLLECTION_PHYSICAL);
    Field->logical = hid_lookup_collection(Parser, USB_HID_COLLECTION_LOGICAL);
    Field->application = hid_lookup_collection(Parser, USB_HID_COLLECTION_APPLICATION);

    for (i = 0; i < usages; i++)
    {
        int j = i;

        /* Duplicate the last usage we parsed if we have excess values */
        if (i >= Parser->local.usage_index)
        {
            j = Parser->local.usage_index - 1;
        }

        Field->usage[i].hid = Parser->local.usage[j];
        Field->usage[i].collection_index = Parser->local.collection_index[j];
    }

    Field->maxusage 		= usages;
    Field->flags 			= Flags;
    Field->report_offset 	= offset;
    Field->report_type 		= ReportType;
    Field->report_size 		= Parser->global.ReportSize;
    Field->report_count 	= Parser->global.ReportCount;
    Field->logical_minimum 	= Parser->global.LogicalMinimum;
    Field->logical_maximum 	= Parser->global.LogicalMaximum;
    Field->physical_minimum = Parser->global.PhysicalMinimum;
    Field->physical_maximum = Parser->global.PhysicalMaximum;
    Field->unit_exponent 	= Parser->global.UnitExponent;
    Field->unit 			= Parser->global.Unit;

    print_field(Field);

    return 0;
}

/*
*******************************************************************************
*                     item_udata
*
* Description:
*	  Read data value from item.
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
static __u32 item_udata(usbHidItem_t *Item)
{
    switch (Item->size)
    {
    case 1:
        return Item->data.Data_u8;
    //break;

    case 2:
        return Item->data.Data_u16;
    //break;

    case 4:
        return Item->data.Data_u32;
    //break;

    default:
        DMSG_PANIC("ERR: unkown item size(%d)\n", Item->size);
    }

    return 0;
}

/*
*******************************************************************************
*                     item_sdata
*
* Description:
*	  Read data value from item.
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
static __s32 item_sdata(usbHidItem_t *Item)
{
    switch (Item->size)
    {
    case 1:
        return Item->data.Data_s8;
    //break;

    case 2:
        return Item->data.Data_s16;
    //break;

    case 4:
        return Item->data.Data_s32;
    //break;

    default:
        DMSG_PANIC("ERR: unkown item size(%d)\n", Item->size);
    }

    return 0;
}

/*
*******************************************************************************
*                     HidParserGlobal
*
* Description:
*     解析全局目录项
*
* Parameters:
*     Parser  :  output. 解析的结果
*     Item    :  input.  待解析的目录项
*
* Return value:
*    成功或者失败
*
* note:
*    无
*
*******************************************************************************
*/
static __s32 HidParserGlobal(usbHidParser_t *Parser, usbHidItem_t *Item)
{
    switch(Item->tag)
    {
    case USB_HID_GLOBAL_ITEM_TAG_PUSH:

        if (Parser->global_stack_ptr == USB_HID_GLOBAL_STACK_SIZE)
        {
            DMSG_PANIC("ERR: global enviroment stack overflow\n");
            return USB_ERR_DATA_OVERFLOW;
        }

        /* 将parser->global压入堆栈parser->global_stack[global_stack_ptr] */
        USB_OS_MEMCPY(Parser->global_stack + Parser->global_stack_ptr++,
                      &Parser->global,
                      sizeof(usbHidGlobalItems_t));
        return USB_ERR_SUCCESS;
    //break;

    case USB_HID_GLOBAL_ITEM_TAG_POP:

        if (!Parser->global_stack_ptr)
        {
            DMSG_PANIC("ERR: global enviroment stack underflow\n");
            return -1;
        }

        /* 将parser->global_stack[--global_stack_ptr]出栈[luther.gliethttp] */
        USB_OS_MEMCPY(&Parser->global,
                      Parser->global_stack + --Parser->global_stack_ptr,
                      sizeof(usbHidGlobalItems_t));
        return USB_ERR_SUCCESS;
    //break;

    case USB_HID_GLOBAL_ITEM_TAG_USAGE_PAGE:
        /* 全局量global.usage_page, 比如:
           0x05, 0x01, --- USAGE_PAGE (Generic Desktop)
           0x05, 0x09, --- USAGE_PAGE (Button)
         */
        Parser->global.UsagePage = item_udata(Item);
        return USB_ERR_SUCCESS;
    //break;

    case USB_HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM:
        Parser->global.LogicalMinimum = item_sdata(Item);
        return USB_ERR_SUCCESS;
    //break;

    case USB_HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM:
        if (Parser->global.LogicalMinimum < 0)
        {
            Parser->global.LogicalMaximum = item_sdata(Item);
        }
        else
        {
            Parser->global.LogicalMaximum = item_udata(Item);
        }

        return USB_ERR_SUCCESS;
    //break;

    case USB_HID_GLOBAL_ITEM_TAG_PHYSICAL_MINIMUM:
        Parser->global.PhysicalMinimum = item_sdata(Item);
        return USB_ERR_SUCCESS;
    //break;

    case USB_HID_GLOBAL_ITEM_TAG_PHYSICAL_MAXIMUM:
        if (Parser->global.PhysicalMinimum < 0)
        {
            Parser->global.PhysicalMaximum = item_sdata(Item);
        }
        else
        {
            Parser->global.PhysicalMaximum = item_udata(Item);
        }

        return USB_ERR_SUCCESS;
    //break;

    case USB_HID_GLOBAL_ITEM_TAG_UNIT_EXPONENT:
        Parser->global.UnitExponent = item_sdata(Item);
        return USB_ERR_SUCCESS;
    //break;

    case USB_HID_GLOBAL_ITEM_TAG_UNIT:
        Parser->global.Unit = item_udata(Item);
        return USB_ERR_SUCCESS;
    //break;

    case USB_HID_GLOBAL_ITEM_TAG_REPORT_SIZE:
        /* 比如：0x75, 0x01, REPORT_SIZE (1), 表示1个bit */
        if ((Parser->global.ReportSize = item_udata(Item)) > 32)
        {
            DMSG_PANIC("ERR: invalid report_size %d\n", Parser->global.ReportSize);
            return USB_ERR_UNKOWN_ERROR;
        }
        return USB_ERR_SUCCESS;
    //break;

    case USB_HID_GLOBAL_ITEM_TAG_REPORT_COUNT:
        if ((Parser->global.ReportCount = item_udata(Item)) > USB_HID_MAX_USAGES)
        {
            DMSG_PANIC("ERR: invalid report_count %d\n", Parser->global.ReportSize);
            return USB_ERR_UNKOWN_ERROR;
        }
        return USB_ERR_SUCCESS;
    //break;

    case USB_HID_GLOBAL_ITEM_TAG_REPORT_ID:
        if ((Parser->global.ReportId = item_udata(Item)) == 0)
        {
            DMSG_PANIC("ERR: report_id 0 is invalid\n");
            return USB_ERR_UNKOWN_ERROR;
        }
        return USB_ERR_SUCCESS;
    //break;

    default:
        DMSG_PANIC("ERR: unknown global tag 0x%x\n", Item->tag);
        //return USB_ERR_UNKOWN_ERROR;
    }

    return USB_ERR_UNKOWN_ERROR;
}

/*
*******************************************************************************
*                     HidParserGlobal
*
* Description:
*     解析全局目录项
*
* Parameters:
*     Parser  :  output. 解析的结果
*     Item    :  input.  待解析的目录项
*
* Return value:
*    成功或者失败
*
* note:
*    无
*
*******************************************************************************
*/
static int HidParserLocal(usbHidParser_t *Parser, usbHidItem_t *Item)
{
    __u32 data = 0;
    __u32 n = 0;

    if (Item->size == 0)
    {
        DMSG_PANIC("ERR: item data expected for local item\n");
        return -1;
    }

    data = item_udata(Item);

    switch (Item->tag)
    {
    case USB_HID_LOCAL_ITEM_TAG_DELIMITER:
        if (data)
        {
            /*
             * We treat items before the first delimiter
             * as global to all usage sets (branch 0).
             * In the moment we process only these global
             * items and the first delimiter set.
             */
            if (Parser->local.delimiter_depth != 0)
            {
                DMSG_PANIC("ERR: nested delimiters\n");
                return -1;
            }

            Parser->local.delimiter_depth++;
            Parser->local.delimiter_branch++;
        }
        else
        {
            if (Parser->local.delimiter_depth < 1)
            {
                DMSG_PANIC("ERR: bogus close delimiter\n");
                return -1;
            }

            Parser->local.delimiter_depth--;
        }
        return 1;
    //break;

    case USB_HID_LOCAL_ITEM_TAG_USAGE:
        if (Parser->local.delimiter_branch > 1)
        {
            DMSG_PANIC("ERR: alternative usage ignored\n");
            return 0;
        }

        if (Item->size <= 2)
        {
            data = (Parser->global.UsagePage << 16) + data;
        }

        return HidAddUsage(Parser, data);
    //break;

    case USB_HID_LOCAL_ITEM_TAG_USAGE_MINIMUM:
        if (Parser->local.delimiter_branch > 1)
        {
            DMSG_PANIC("ERR: alternative usage ignored\n");
            return 0;
        }

        if (Item->size <= 2)
        {
            data = (Parser->global.UsagePage << 16) + data;
        }

        Parser->local.usage_minimum = data;

        return 0;
    //break;

    case USB_HID_LOCAL_ITEM_TAG_USAGE_MAXIMUM:
        if (Parser->local.delimiter_branch > 1)
        {
            DMSG_PANIC("ERR: alternative usage ignored\n");
            return 0;
        }

        if (Item->size <= 2)
        {
            data = (Parser->global.UsagePage << 16) + data;
        }

        for (n = Parser->local.usage_minimum; n <= data; n++)
        {
            if (HidAddUsage(Parser, n))
            {
                DMSG_PANIC("ERR: hid_add_usage failed\n");
                return -1;
            }
        }

        return 0;
    //break;

    default:
        DMSG_PANIC("ERR: unknown local item tag 0x%x\n", Item->tag);
        //return 0;
    }

    return 0;
}

/*
*******************************************************************************
*                     hid_parser_main
*
* Description:
*     Process a main item.
*
* Parameters:
*
*
* Return value:
*    成功或者失败
*
* note:
*    无
*
*******************************************************************************
*/
static int HidParserMain(usbHidParser_t *parser, usbHidItem_t *item)
{
    __u32 data = 0;
    __s32 ret = 0;

    data = item_udata(item);	//返回该Short Item数据,数据有4种,0,1,2,4字节

    switch (item->tag)
    {
    case USB_HID_MAIN_ITEM_TAG_BEGIN_COLLECTION:
        //Open a collection. 入栈push操作The type/usage is pushed on the stack
        ret = open_collection(parser, data & 0xff);
        break;

    case USB_HID_MAIN_ITEM_TAG_END_COLLECTION:
        // Close a collection.出栈pop操作parser->collection_stack_ptr
        ret = close_collection(parser);
        break;

    case USB_HID_MAIN_ITEM_TAG_INPUT:
        ret = HidAddField(parser, USB_HID_REPORT_INPUT, data);
        break;

    case USB_HID_MAIN_ITEM_TAG_OUTPUT:
        ret = HidAddField(parser, USB_HID_REPORT_OUTPUT, data);
        break;

    case USB_HID_MAIN_ITEM_TAG_FEATURE:
        ret = HidAddField(parser, USB_HID_REPORT_FEATURE, data);
        break;

    default:
        DMSG_PANIC("ERR: unknown main item tag 0x%x\n", item->tag);
        ret = 0;
    }

    // 清0所有parser->local内存数据,local包括:
    // struct hid_local {
    //    unsigned usage[HID_MAX_USAGES];               /* usage array */
    //    unsigned collection_index[HID_MAX_USAGES];    /* collection index array */
    //    unsigned usage_index;
    //    unsigned usage_minimum;
    //    unsigned delimiter_depth;
    //    unsigned delimiter_branch;
    // };
    // 所以parser->local.usage_index在遇到main item时全部清0.
    print_local(&(parser->local));
    USB_OS_MEMSET(&(parser->local), 0, sizeof(parser->local));	/* Reset the local parser environment */

    return ret;
}

/*
*******************************************************************************
*                     hid_parser_main
*
* Description:
*     Process a reserved item..
*
* Parameters:
*
*
* Return value:
*    成功或者失败
*
* note:
*    无
*
*******************************************************************************
*/
static int HidParserReserved(usbHidParser_t *parser, usbHidItem_t *item)
{
    DMSG_PANIC("ERR: reserved item type, tag 0x%x\n", item->tag);

    return 0;
}

/*
 * Free a report and all registered fields. The field->usage and
 * field->value table's are allocated behind the field, so we need
 * only to free(field) itself.
 */

static void hid_free_report(usbHidReport_t *report)
{
    __u32 n;

    for (n = 0; n < report->Maxfield; n++)
    {
        if(report->Field[n])
        {
            if(report->Field[n]->usage)
            {
                USB_OS_FREE(report->Field[n]->usage);
                report->Field[n]->usage = NULL;
            }

            if(report->Field[n]->value)
            {
                USB_OS_FREE(report->Field[n]->value);
                report->Field[n]->value = NULL;
            }

            USB_OS_FREE(report->Field[n]);
            report->Field[n] = NULL;
        }
    }

    USB_OS_FREE(report);

    return;
}

/*
 * Free a device structure, all reports, and all fields.
 */

void hid_free_device(HidDev_t *HidDev)
{
    __u32 i = 0;
    __u32 j = 0;

    for (i = 0; i < USB_HID_REPORT_TYPES; i++)
    {
        usbHidReportEnum_t *report_enum = HidDev->HidReportEnum + i;

        for (j = 0; j < 256; j++)
        {
            usbHidReport_t *report = report_enum->Report[j];

            if (report)
            {
                report_enum->ReportNum  -= 1;
                hid_free_report(report);
                report_enum->Report[j] = NULL;
            }
        }
    }

    if(HidDev->collection)
    {
        USB_OS_FREE(HidDev->collection);
        HidDev->collection = NULL;
    }

    return ;
}

/*
*******************************************************************************
*                     HidFetchItem
*
* Description:
*     取出Item，并且解析
*
* Parameters:
*     Start  :  input.  本次带解析的report起始地址
*     End    :  input.  report的结束地址
*     Item   :  output. 解析后的Item
*
* Return value:
*    返回解析后的Item
*
* note:
*    无
*
*******************************************************************************
*/
static __u8 *HidFetchItem(__u8 *Start, __u8 *End, usbHidItem_t *Item)
{
    __u8 b = 0;

    if(Start == NULL || End == NULL || Item == NULL)
    {
        DMSG_PANIC("ERR: input error\n");
        return NULL;
    }

    USB_OS_MEMSET(Item, 0, sizeof(usbHidItem_t));

    if ((End - Start) <= 0)
    {
        DMSG_PANIC("ERR: the item is invalid. Start = %x, End = %x\n", Start, End);
        return NULL;
    }

    b = *Start++;

    Item->type = (b >> 2) & 3;	// 取出类型
    Item->tag  = (b >> 4) & 15;	// 取出tag信息

    if (Item->tag == USB_HID_ITEM_TAG_LONG)
    {
        Item->format = USB_HID_ITEM_FORMAT_LONG;

        if ((End - Start) < 2)
        {
            DMSG_PANIC("ERR: the item is invalid. Start = %x, End = %x\n", Start, End);
            return NULL;
        }

        Item->size = *Start++;
        Item->tag  = *Start++;

        // 保证该Long item拥有所需的足够数据
        if ((End - Start) < Item->size)
        {
            DMSG_PANIC("ERR: the item is invalid. Start = %x, End = %x, Item->size = %x\n",
                       Start, End, Item->size);
            return NULL;
        }

        Item->data.longdata = Start;	// 从第4个字节开始就是数据区
        Start += Item->size;			// start指向下一个item开始处

        return Start;
    }

    Item->format = USB_HID_ITEM_FORMAT_SHORT;
    Item->size = b & 3;	 //取出数据大小

    switch(Item->size)
    {
    case 0:	// 没有数据区,start指向下一个item开始处
        return Start;

    case 1:
        if ((End - Start) < 1)
            return NULL;
        Item->data.Data_u8 = *Start++;	 // 取出1字节数据
        return Start;

    case 2:
        if ((End - Start) < 2)
            return NULL;
        Item->data.Data_u16 = le16_to_cpu(*((__u16 *)Start));
        Start = (__u8 *)((__u16 *)Start + 1);	// start加2
        return Start;

    case 3:
        Item->size++;	// 强制调整到4字节
        if ((End - Start) < 4)
            return NULL;
        Item->data.Data_u32 = le32_to_cpu(*((__u32 *)Start));
        Start = (__u8 *)((__u32 *)Start + 1);	// start加4
        return Start;
    }

    return NULL;
}

/*
*******************************************************************************
*                     HidParseReport
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
__s32 HidParseReport(__u8 *ReportData, __u32 ReportSize, HidDev_t *HidDev)
{
    usbHidParser_t *parser = NULL;  /* 因为usbHidParser_t结构大于16k, 因此使用malloc */
    usbHidItem_t item;
    __u8 *end = NULL;
    __s32 ret = 0;

    parser = (usbHidParser_t *)USB_OS_MALLOC(sizeof(usbHidParser_t),
             USB_MEM_FILE_TRIGGER,
             USB_MEM_LINE_TRIGGER);
    if(parser == NULL)
    {
        DMSG_PANIC("ERR: malloc failed\n");
        ret = USB_ERR_MALLOC_FAILED;

        goto ERR0;
    }
    USB_OS_MEMSET(parser, 0, sizeof(usbHidParser_t));

    HidDev->collection = USB_OS_MALLOC(sizeof(usbHidCollectionItems_t) *
                                       USB_HID_DEFAULT_NUM_COLLECTIONS,
                                       USB_MEM_FILE_TRIGGER,
                                       USB_MEM_LINE_TRIGGER);
    if (HidDev->collection == NULL)
    {
        DMSG_PANIC("ERR: malloc failed\n");
        ret = USB_ERR_MALLOC_FAILED;

        goto ERR1;
    }
    USB_OS_MEMSET(HidDev->collection,
                  0,
                  sizeof(usbHidCollectionItems_t) * USB_HID_DEFAULT_NUM_COLLECTIONS);

    HidDev->collection_size = USB_HID_DEFAULT_NUM_COLLECTIONS;
    parser->HidDev = HidDev;

    // 从设备报告描述符中读取一个item项
    end = ReportData + ReportSize;
    while ((ReportData = HidFetchItem(ReportData, end, &item)) != NULL)
    {
        print_item(&item);

        /* 现在Long item项还没有使用,所以这里不支持 */
        if (item.format != USB_HID_ITEM_FORMAT_SHORT)
        {
            DMSG_PANIC("ERR: unexpected long global item\n");
            ret = USB_ERR_UNKOWN_ERROR;
            goto ERR2;
        }

        switch(item.type)
        {
        case USB_HID_ITEM_TYPE_MAIN:
            if (HidParserMain(parser, &item))
            {
                DMSG_PANIC("ERR: item %u %u %u %u parsing failed\n",
                           item.format,
                           (unsigned)item.size,
                           (unsigned)item.type,
                           (unsigned)item.tag);
                ret = USB_ERR_UNKOWN_ERROR;
                goto ERR2;
            }
            break;

        case USB_HID_ITEM_TYPE_GLOBAL:
            if (HidParserGlobal(parser, &item))
            {
                DMSG_PANIC("ERR: item %u %u %u %u parsing failed\n",
                           item.format,
                           (unsigned)item.size,
                           (unsigned)item.type,
                           (unsigned)item.tag);
                ret = USB_ERR_UNKOWN_ERROR;
                goto ERR2;
            }
            break;

        case USB_HID_ITEM_TYPE_LOCAL:
            if (HidParserLocal(parser, &item))
            {
                DMSG_PANIC("ERR: item %u %u %u %u parsing failed\n",
                           item.format,
                           (unsigned)item.size,
                           (unsigned)item.type,
                           (unsigned)item.tag);
                ret = USB_ERR_UNKOWN_ERROR;
                goto ERR2;
            }
            break;

        default:
            if (HidParserReserved(parser, &item))
            {
                DMSG_PANIC("ERR: item %u %u %u %u parsing failed\n",
                           item.format,
                           (unsigned)item.size,
                           (unsigned)item.type,
                           (unsigned)item.tag);
                ret = USB_ERR_UNKOWN_ERROR;
                goto ERR2;
            }
        }

        // 解析完了
        if (ReportData == end)
        {
            // 入栈操作多于出栈操作,比如COLLECTION (Application)就是入栈, END_COLLECTION对应出栈
            // 目前定义堆栈大小为4个, #define HID_COLLECTION_STACK_SIZE 4
            // 所以报告描述符脚本书写有误,返回NULL,失败
            if (parser->collection_stack_ptr)
            {
                DMSG_PANIC("ERR: unbalanced collection at end of report description\n");
                ret = USB_ERR_UNKOWN_ERROR;
                goto ERR2;
            }

            // 该变量也是通过入栈,出栈收集的,所以也必须配对
            if (parser->local.delimiter_depth)
            {
                DMSG_PANIC("ERR: unbalanced delimiter at end of report description\n");
                ret = USB_ERR_UNKOWN_ERROR;
                goto ERR2;
            }

            print_ReportEnum(HidDev);
            print_parser(parser);

            if(parser)
            {
                USB_OS_FREE(parser);
            }

            //正常解析成功,释放vmalloc到的parser解释器结构体内存.
            return USB_ERR_SUCCESS;
        }
    }

    // 报告描述脚本有误
    DMSG_PANIC("ERR: item fetching failed at offset %d\n", (int)(end - ReportData));

    ret = USB_ERR_UNKOWN_ERROR;

ERR2:
    hid_free_device(HidDev);
    USB_OS_FREE(HidDev->collection);
    HidDev->collection = NULL;

ERR1:
    USB_OS_FREE(parser);

ERR0:

    return ret;
}

/*
*******************************************************************************
*                     HidParseReport
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*
*
* note:
*    无
*
*******************************************************************************
*/
__s32 HidFreeReport(HidDev_t *HidDev)
{

    hid_free_device(HidDev);

    if(HidDev->collection)
    {
        USB_OS_FREE(HidDev->collection);
        HidDev->collection = NULL;
    }

    return USB_ERR_SUCCESS;

}

