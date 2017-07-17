/*
********************************************************************************
*                                USB Hid Driver
*
*                (c) Copyright 2006-2010, All winners Co,Ld.
*                        All Right Reserved
*
* FileName		:  usbMouse.c
*
* Author		:  Javen
*
* Date			:  2010.06.02
*
* Description	:  USB Mouse
*
* Others		:  NULL
*
* History:
*		<time> 		<version >		<author>	 	<desc>
*	   2010.06.02	   1.0			 Javen			build this file
*
********************************************************************************
*/

#include  "usb_host_config.h"
#include  "usb_host_base_types.h"
#include  "usb_os_platform.h"
#include  "error.h"

#include  "misc_lib.h"

#include  "HidSpec.h"
#include  "Hid_i.h"
#include  "HidFunDrv.h"

#include  "usbMouse.h"
#include  "UsbMouse_DriftControl.h"

//---------------------------------------------------------
//   函数定义区
//---------------------------------------------------------
extern __u32 usbh_get_usbd_port(void);

static void usbMouse_StartWork(usbMouse_t *usbMouse);
static void usbMouse_StopWork(usbMouse_t *usbMouse);

/*
*******************************************************************************
*                     usbMouseDone
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
static void usbMouseDone(HidRequest_t *HidReq)
{
    usbMouse_t *usbMouse = NULL;

    usbMouse = (usbMouse_t *)HidReq->Extern;
    if(usbMouse == NULL)
    {
        DMSG_PANIC("ERR: usbMouseDone: usbMouse == NULL\n");
        return;
    }

    UsbThreadWakeUp(usbMouse->MouseThreadSemi);

    return;
}

/*
*******************************************************************************
*                     usbMouseOpen
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
static USB_OS_HANDLE usbMouseOpen(void *open_arg, __u32 mode)
{
    usbMouse_t *usbMouse = NULL;

    if(open_arg == NULL)
    {
        DMSG_PANIC("ERR: usbMouseOpen: input error, open_arg = %x\n", open_arg);
        return NULL;
    }

    usbMouse = (usbMouse_t *)open_arg;
    if(usbMouse->Magic != USB_HID_DEV_MAGIC)
    {
        DMSG_PANIC("ERR: usbMouseOpen: BlkDev Magic(%x) is invalid\n", usbMouse->Magic);
        return NULL;
    }

    return (USB_OS_HANDLE)open_arg;
}

/*
*******************************************************************************
*                     usbMouseClose
*
* Description:
*
*
* Parameters:
*
*
* Return value:
*    0	：成功
*	!0	：失败
*
* note:
*    无
*
*******************************************************************************
*/
static __s32 usbMouseClose(USB_OS_HANDLE hDev)
{
    usbMouse_t *usbMouse = NULL;

    if(hDev == NULL)
    {
        DMSG_PANIC("ERR: usbMouseClose: input error\n");
        return NULL;
    }

    usbMouse = (usbMouse_t *)hDev;
    if(usbMouse->Magic != USB_HID_DEV_MAGIC)
    {
        DMSG_PANIC("ERR: usbMouseClose: BlkDev Magic(%x) is invalid\n", usbMouse->Magic);
        return NULL;
    }

    return EPDK_OK;
}
/*
*******************************************************************************
*                     usbMouseRead
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
static __u32 usbMouseRead(void *pBuffer, __u32 blk, __u32 n, USB_OS_HANDLE hDev)
{
    DMSG_PANIC("ERR: usbMouseRead not support\n");
    return 0;
}

/*
*******************************************************************************
*                     usbMouseWrite
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
static __u32 usbMouseWrite(const void *pBuffer, __u32 blk, __u32 n, USB_OS_HANDLE hDev)
{
    DMSG_PANIC("ERR: usbMouseWrite not support\n");
    return 0;
}

/*
*******************************************************************************
*                     usbMouseIoctl
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
static __s32 usbMouseIoctl(USB_OS_HANDLE hDev, __u32 Cmd, __s32 Aux, void *pBuffer)
{
    usbMouse_t *usbMouse = NULL;
    HidDev_t *HidDev = NULL;
    __cpu_sr cup_sr	= 0;

    if(hDev == NULL)
    {
        DMSG_PANIC("ERR: usbMouseClose: input error\n");
        return NULL;
    }

    usbMouse = (usbMouse_t *)hDev;
    if(usbMouse->Magic != USB_HID_DEV_MAGIC)
    {
        DMSG_PANIC("ERR: usbMouseClose: BlkDev Magic(%x) is invalid\n", usbMouse->Magic);
        return EPDK_FAIL;
    }

    HidDev = usbMouse->HidDev;
    if(HidDev == NULL)
    {
        DMSG_PANIC("ERR: HidDev == NULL\n");
        return EPDK_FAIL;
    }

    switch(Cmd)
    {
    case USBH_HID_USER_CTRL_CMD_TEST_START:
    {
        DMSG_INFO("usb mouse test command: start-s\n");

        if(pBuffer == NULL)
        {
            DMSG_PANIC("ERR: Execute usb mouse test cmd START failed, for pBuffer is NULL\n");
            return EPDK_FAIL;
        }

        USB_OS_ENTER_CRITICAL(cup_sr);
        usbMouse->USBHMouseTest 	= (USBHMouseTest_t *)pBuffer;
        usbMouse->USBMouseTestFlag 	= 1;
        USB_OS_EXIT_CRITICAL(cup_sr);

        usbMouse_StartWork(usbMouse);

        DMSG_INFO("usb mouse test command: start-e\n");
    }
    break;

    case USBH_HID_USER_CTRL_CMD_TEST_STOP:
        DMSG_INFO("usb mouse test command: stop-s\n");

        if(usbMouse->USBMouseTestFlag)
        {
            USB_OS_ENTER_CRITICAL(cup_sr);
            usbMouse->USBHMouseTest 	= NULL;
            usbMouse->USBMouseTestFlag 	= 0;
            USB_OS_EXIT_CRITICAL(cup_sr);

            usbMouse_StopWork(usbMouse);
        }
        else
        {
            DMSG_PANIC("ERR: usb mouse test, can not stop test, for have not start test\n");
            return EPDK_FAIL;
        }

        DMSG_INFO("usb mouse test command: stop-e\n");
        break;

    case USBH_HID_USER_CTRL_CMD_REG:
        usbMouse->CallBack = (USBHMouse_CallBack)USB_OS_esKRNL_GetCallBack((__pCBK_t)(pBuffer));
        if(usbMouse->CallBack == NULL)
        {
            DMSG_PANIC("ERR: usb mouse CallBack is NULL\n");
            return EPDK_FAIL;
        }

        usbMouse_StartWork(usbMouse);
        break;

    case USBH_HID_USER_CTRL_CMD_UNREG:
        usbMouse->CallBack = NULL;
        usbMouse_StopWork(usbMouse);
        break;

    default:
        DMSG_PANIC("ERR: unkown cmd(%x)\n", Cmd);
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

/*
*******************************************************************************
*                     usbMouse_StartWork
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
static void usbMouse_StartWork(usbMouse_t *usbMouse)
{
    HidDev_t *HidDev = NULL;
    __cpu_sr cup_sr	= 0;
    __s32 ret = 0;

    if(usbMouse == NULL)
    {
        DMSG_PANIC("ERR: input error\n");
        return ;
    }

    HidDev = usbMouse->HidDev;
    if(HidDev == NULL)
    {
        DMSG_PANIC("ERR: HidDev == NULL\n");
        return ;
    }

    if(HidDev->State == HID_DEV_OFFLINE)
    {
        DMSG_PANIC("ERR: device is offline, mouse can not start work\n");
        return ;
    }

    /* 清除鼠标的状态 */
    ret = HidDev->SoftReset(HidDev);
    if(ret != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: SoftReset failed\n");
        /* 由于有些鼠标不支持SetIdle命令，SoftReset可能会失败，因此不能直接return */
    }

    USB_OS_ENTER_CRITICAL(cup_sr);
    usbMouse->StopWork       = 0;
    usbMouse->HidReq.Result = USB_HID_TRANSPORT_DEVICE_RESET;
    USB_OS_EXIT_CRITICAL(cup_sr);

    UsbThreadWakeUp(usbMouse->MouseThreadSemi);

    return;
}

/*
*******************************************************************************
*                     usbMouse_StopWork
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
static void usbMouse_StopWork(usbMouse_t *usbMouse)
{
    HidDev_t *HidDev = NULL;
    __cpu_sr cup_sr	= 0;

    if(usbMouse == NULL)
    {
        DMSG_PANIC("ERR: input error\n");
        return ;
    }

    HidDev = usbMouse->HidDev;
    if(HidDev == NULL)
    {
        DMSG_PANIC("ERR: HidDev == NULL\n");
        return ;
    }

    USB_OS_ENTER_CRITICAL(cup_sr);
    usbMouse->StopWork = 1;
    USB_OS_EXIT_CRITICAL(cup_sr);

    HidDev->StopTransport(HidDev);

    return ;
}

/*
*******************************************************************************
*                     usbMouse_StopWork
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
static void usbMouseEvent(usbMouse_t *usbMouse)
{
    HidRequest_t *HidReq = NULL;
    __s32 status = 0;
    USBHMouseEvent_t Event;
    __s32 TempData = 0;

    if(usbMouse == NULL)
    {
        DMSG_PANIC("ERR: input error\n");
        return ;
    }

    HidReq = &(usbMouse->HidReq);
    if(HidReq == NULL)
    {
        DMSG_PANIC("ERR: HidReq == NULL\n");
        return ;
    }

    /* check request result */
    switch(HidReq->Result)
    {
    case USB_HID_TRANSPORT_SUCCESS:
        //nothing to do
        break;

    case USB_HID_TRANSPORT_DEVICE_DISCONNECT:
        DMSG_PANIC("ERR: usbMouseEvent: mouse is disconnect\n");
        return;
    //break;

    case USB_HID_TRANSPORT_CANCEL_CMD:
        DMSG_PANIC("ERR: usbMouseEvent: driver cancel command\n");
        return;
    //break;

    default:
        DMSG_PANIC("ERR: unkown result %x\n", HidReq->Result);
        goto next;
    }

    DMSG_MOUSE_TEST("\n");
    DMSG_MOUSE_TEST("[0]:%x, [1]:%x, [2]:%x, [3]:%x, [4]:%x, [5]:%x, [6]:%x\n",
                    usbMouse->Data[0], usbMouse->Data[1], usbMouse->Data[2],
                    usbMouse->Data[3], usbMouse->Data[4], usbMouse->Data[5],
                    usbMouse->Data[6]);

    /* paser usb mouse event */
    USB_OS_MEMSET(&Event, 0, sizeof(USBHMouseEvent_t));

    /* button */
    if(usbMouse->DataDef.Button.BitCount)
    {
        TempData = 0;
        xGetDataFromBuffer(usbMouse->Data,
                           usbMouse->DataDef.Button.BitOffset,
                           usbMouse->DataDef.Button.BitCount,
                           &TempData);
        USB_OS_MEMCPY(&Event.Button, &TempData, 8);
    }

    /* X坐标 */
    if(usbMouse->DataDef.X.BitCount)
    {
        TempData = 0;
        xGetDataFromBuffer(usbMouse->Data,
                           usbMouse->DataDef.X.BitOffset,
                           usbMouse->DataDef.X.BitCount,
                           &TempData);
        if(test_bit(TempData, (usbMouse->DataDef.X.BitCount - 1)))
        {
            TempData |= (0xffffffff << (usbMouse->DataDef.X.BitCount - 1));
        }

        /* 根据鼠标原厂的建议, 把超过+127设置为+127, 超过-127设置为-127 */
        if(TempData <= -127)
        {
            Event.X = -127;
        }
        else if(TempData > 127)
        {
            Event.X = 127;
        }
        else
        {
            Event.X = TempData;
        }
    }

    /* Y坐标 */
    if(usbMouse->DataDef.Y.BitCount)
    {
        TempData = 0;
        xGetDataFromBuffer(usbMouse->Data,
                           usbMouse->DataDef.Y.BitOffset,
                           usbMouse->DataDef.Y.BitCount,
                           &TempData);
        if(test_bit(TempData, (usbMouse->DataDef.Y.BitCount - 1)))
        {
            TempData |= (0xffffffff << (usbMouse->DataDef.Y.BitCount - 1));
        }

        if(TempData <= -127)
        {
            Event.Y = -127;
        }
        else if(TempData > 127)
        {
            Event.Y = 127;
        }
        else
        {
            Event.Y = TempData;
        }
    }

    /* wheel */
    if(usbMouse->DataDef.Wheel.BitCount)
    {
        TempData = 0;
        xGetDataFromBuffer(usbMouse->Data,
                           usbMouse->DataDef.Wheel.BitOffset,
                           usbMouse->DataDef.Wheel.BitCount,
                           &TempData);
        if(test_bit(TempData, (usbMouse->DataDef.Wheel.BitCount - 1)))
        {
            TempData |= (0xffffffff << (usbMouse->DataDef.Wheel.BitCount - 1));
        }

        if(TempData <= -127)
        {
            Event.Wheel = -127;
        }
        else if(TempData > 127)
        {
            Event.Wheel = 127;
        }
        else
        {
            Event.Wheel = TempData;
        }
    }

    DMSG_MOUSE_TEST("Button 1 = %d\n", Event.Button.LeftButton);
    DMSG_MOUSE_TEST("Button 2 = %d\n", Event.Button.RightButton);
    DMSG_MOUSE_TEST("Button 3 = %d\n", Event.Button.MiddleButton);
    DMSG_MOUSE_TEST("Button 4 = %d\n", Event.Button.Button4);
    DMSG_MOUSE_TEST("Button 5 = %d\n", Event.Button.Button5);
    DMSG_MOUSE_TEST("Button 6 = %d\n", Event.Button.Button6);
    DMSG_MOUSE_TEST("Button 7 = %d\n", Event.Button.Button7);
    DMSG_MOUSE_TEST("Button 8 = %d\n", Event.Button.Button8);

    DMSG_MOUSE_TEST("X        = %d\n", Event.X);
    DMSG_MOUSE_TEST("Y        = %d\n", Event.Y);
    DMSG_MOUSE_TEST("Wheel    = %d\n", Event.Wheel);
    DMSG_MOUSE_TEST("\n");

    UsbMouse_AddToDriftArray(usbMouse, &Event);

next:
    /* do next hid request */
    status = HidSentRequest(&(usbMouse->HidReq));
    if(status != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: HidSentRequest failed\n");
        return ;
    }

    return ;
}

/*
*******************************************************************************
*                     usbMouseThd
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
static void usbMouseThread(void *p_arg)
{
    usbMouse_t *usbMouse = (usbMouse_t *)p_arg;

    USB_OS_SemPost(usbMouse->notify_complete);

    while(1)
    {
        //--<1>--杀死线程
        TryToKillThreadSelf("usbMouseThread");

        /* sleep */
        UsbThreadSleep(usbMouse->MouseThreadSemi);

        if(usbMouse->HidDev->State == HID_DEV_OFFLINE || usbMouse->StopWork)
        {
            /* nothing to do */
            DMSG_INFO("WRN: Usb mouse is offline(%d) or stop work(%d)\n",
                      usbMouse->HidDev->State, usbMouse->StopWork);
        }
        else
        {
            usbMouseEvent(usbMouse);
        }
    }
}

/*
*******************************************************************************
*                     usbMouseProbe
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
__s32 usbMouseProbe(HidDev_t *HidDev)
{
    usbMouse_t *usbMouse = NULL;
    __u8 err = 0;
    __s32 ret = 0;

    if(HidDev == NULL)
    {
        DMSG_PANIC("ERR: usbMouseProbe: input error\n");
        return USB_ERR_BAD_ARGUMENTS;
    }

    /* 创建一个mouse设备，并且初始化 */
    usbMouse = (usbMouse_t *)USB_OS_MALLOC(sizeof(usbMouse_t),
                                           USB_MEM_FILE_TRIGGER,
                                           USB_MEM_LINE_TRIGGER);
    if(usbMouse == NULL)
    {
        DMSG_PANIC("ERR: USB_OS_MALLOC failed\n");
        return USB_ERR_MALLOC_FAILED;
    }

    USB_OS_MEMSET(usbMouse, 0, sizeof(usbMouse_t));
    usbMouse->HidDev = HidDev;
    HidDev->Extern = usbMouse;

    /*
    	if(HidDev->pusb_dev->manufacturer){
    		USB_OS_STRCPY((char *)usbMouse->Vendor, HidDev->pusb_dev->manufacturer);
    	}

    	if(HidDev->pusb_dev->product){
    		USB_OS_STRCPY((char *)usbMouse->Product, HidDev->pusb_dev->product);
    	}

    	if(HidDev->pusb_dev->serial){
    		USB_OS_STRCPY((char *)usbMouse->Serial, HidDev->pusb_dev->serial);
    	}
    */

    /* init mouse data tansfer */
    if(HidDev->OnceTransferLength > USB_HID_MOUSE_DATA_LEN)
    {
        DMSG_PANIC("ERR: HidDev OnceTransferLength is too big.(%d, %d)\n",
                   HidDev->OnceTransferLength, USB_HID_MOUSE_DATA_LEN);
        goto error0;
    }

    /* get button BitOffset and BitCount */
    ret = HidGetInputReport(HidDev,
                            USB_HID_USAGE_PAGE_BUTTON,
                            USB_HID_GENERIC_DESKTOP_PAGE_UNDEFINED,
                            &usbMouse->DataDef.Button.BitOffset,
                            &usbMouse->DataDef.Button.BitCount);
    if(ret != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: Get mouse button bitoffset and bitcount field\n");
        goto error0;
    }

    /* get X BitOffset and BitCount */
    ret = HidGetInputReport(HidDev,
                            USB_HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS,
                            USB_HID_GENERIC_DESKTOP_PAGE_X,
                            &usbMouse->DataDef.X.BitOffset,
                            &usbMouse->DataDef.X.BitCount);
    if(ret != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: Get mouse button bitoffset and bitcount field\n");
        goto error0;
    }

    /* get Y BitOffset and BitCount */
    ret = HidGetInputReport(HidDev,
                            USB_HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS,
                            USB_HID_GENERIC_DESKTOP_PAGE_Y,
                            &usbMouse->DataDef.Y.BitOffset,
                            &usbMouse->DataDef.Y.BitCount);
    if(ret != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: Get mouse button bitoffset and bitcount field\n");
        goto error0;
    }

    /* get wheel BitOffset and BitCount */
    ret = HidGetInputReport(HidDev,
                            USB_HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS,
                            USB_HID_GENERIC_DESKTOP_PAGE_WHEEL,
                            &usbMouse->DataDef.Wheel.BitOffset,
                            &usbMouse->DataDef.Wheel.BitCount);
    if(ret != USB_ERR_SUCCESS)
    {
        DMSG_PANIC("ERR: Get mouse button bitoffset and bitcount field\n");
        goto error0;
    }

    DMSG_INFO("Button: BitOffset = %d, BitCount = %d\n",
              usbMouse->DataDef.Button.BitOffset, usbMouse->DataDef.Button.BitCount);
    DMSG_INFO("X: BitOffset = %d, BitCount = %d\n",
              usbMouse->DataDef.X.BitOffset, usbMouse->DataDef.X.BitCount);
    DMSG_INFO("Y: BitOffset = %d, BitCount = %d\n",
              usbMouse->DataDef.Y.BitOffset, usbMouse->DataDef.Y.BitCount);
    DMSG_INFO("Wheel: BitOffset = %d, BitCount = %d\n",
              usbMouse->DataDef.Wheel.BitOffset, usbMouse->DataDef.Wheel.BitCount);

    /* init device operation function */
    usbMouse->MouseOp.Open  = usbMouseOpen;
    usbMouse->MouseOp.Close = usbMouseClose;
    usbMouse->MouseOp.Read  = usbMouseRead;
    usbMouse->MouseOp.Write = usbMouseWrite;
    usbMouse->MouseOp.Ioctl = usbMouseIoctl;

    /* init hid request */
    usbMouse->HidReq.HidDev = HidDev;
    usbMouse->HidReq.DataTransferLength = HidDev->OnceTransferLength;
    usbMouse->HidReq.buffer = usbMouse->Data;
    usbMouse->HidReq.Done = usbMouseDone;

    usbMouse->HidReq.Extern = (void *)usbMouse;
    usbMouse->Magic = USB_HID_DEV_MAGIC;

    /* stop mouse */
    usbMouse->StopWork = 1;

    UsbMouse_DriftControl_Init(usbMouse);

    /* create mouse thread */
    usbMouse->MouseThreadSemi = USB_OS_SemCreate(0);
    if(usbMouse->MouseThreadSemi == NULL)
    {
        DMSG_PANIC("ERR: USB_OS_SemCreate MouseThreadSemi failed\n");
        goto error1;
    }

    usbMouse->notify_complete = USB_OS_SemCreate(0);
    if(usbMouse->notify_complete == NULL)
    {
        DMSG_PANIC("ERR: USB_OS_SemCreate notify_complete failed\n");
        goto error2;
    }

    usbMouse->MouseThdId = UsbCreateThread((void *)usbMouseThread,
                                           (void *)usbMouse,
                                           0x400,
                                           THREAD_LEVEL_HID_CLIENT);
    if(usbMouse->MouseThdId == OS_NO_ERR)
    {
        DMSG_PANIC("ERR: create usbMouseThd failed\n");
        goto error3;
    }

    USB_OS_SemPend(usbMouse->notify_complete, 0, &err);

    /* 生成设备名 */
    USB_OS_STRCPY((char *)usbMouse->ClassName, "HID");
    USB_OS_STRCPY((char *)usbMouse->DevName, "USBMOUSE");

    {
        __u8 temp_buff[32];
        __u32 temp = 0;

        USB_OS_STRCAT((char *)usbMouse->DevName, "_");

        /* usb controler number */
        temp = usbh_get_usbd_port();
        USB_OS_MEMSET(temp_buff, 0, 32);
        USB_OS_uint2str(temp, (char *)temp_buff);
        USB_OS_STRCAT((char *)usbMouse->DevName, (const char *)temp_buff);

        /* hid device number */
        USB_OS_MEMSET(temp_buff, 0, 32);
        USB_OS_uint2str(HidDev->DevNo, (char *)temp_buff);
        USB_OS_STRCAT((char *)usbMouse->DevName, (const char *)temp_buff);
    }

    /* 向系统注册Hid设备 */
    usbMouse->MouseRegHdle = USB_OS_DEV_REG((const char *)usbMouse->ClassName,
                                            (const char *)usbMouse->DevName,
                                            &(usbMouse->MouseOp),
                                            (void *)usbMouse);
    if(usbMouse->MouseRegHdle == NULL)
    {
        DMSG_PANIC("ERR: Mouse USB_OS_DEV_REG failed\n");
        goto error4;
    }

    DMSG_INFO("\n");
    DMSG_INFO("*****************************************************************\n");
    DMSG_INFO("*                                                               *\n");
    DMSG_INFO("* USB HOST Regist \"%s\" \"%s\" successful\n",
              usbMouse->ClassName, usbMouse->DevName);
    DMSG_INFO("*                                                               *\n");
    DMSG_INFO("*****************************************************************\n");
    DMSG_INFO("\n");

    /* Notice: 由于新的设备上来以后，系统的设备管理告知应用程序，因此只能由驱动告知应用程序 */
    USB_OS_esKSRV_SendMsg(KMSG_USR_SYSTEM_MOUSE_PLUGIN, KMSG_PRIO_HIGH);

#ifdef USBH_HID_MOUSE_TEST
    usbMouse->StopWork       = 0;
    usbMouse->HidReq.Result = USB_HID_TRANSPORT_DEVICE_RESET;
    UsbThreadWakeUp(usbMouse->MouseThreadSemi);
#endif

    return USB_ERR_SUCCESS;

error4:
    UsbKillThread(usbMouse->MouseThdId , usbMouse->MouseThreadSemi);

error3:
    USB_OS_SemDel(usbMouse->notify_complete, &err);
    usbMouse->notify_complete = NULL;

error2:
    USB_OS_SemDel(usbMouse->MouseThreadSemi, &err);
    usbMouse->MouseThreadSemi = NULL;

error1:
error0:
    USB_OS_FREE(usbMouse);
    usbMouse = NULL;

    return USB_ERR_DEVICE_PROBE_FAILED;
}

/*
*******************************************************************************
*                     usbMouseRemove
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
__s32 usbMouseRemove(HidDev_t *HidDev)
{
    usbMouse_t *usbMouse = NULL;
    __cpu_sr cup_sr	= 0;
    __u8 err = 0;

    if(HidDev == NULL)
    {
        DMSG_PANIC("ERR: usbMouseRemove: input error\n");
        return USB_ERR_BAD_ARGUMENTS;
    }

    usbMouse = HidDev->Extern;
    if(usbMouse == NULL)
    {
        DMSG_PANIC("ERR: usbMouseRemove: usbMouse == NULL\n");
        return USB_ERR_BAD_ARGUMENTS;
    }

    USB_OS_esKSRV_SendMsg(KMSG_USR_SYSTEM_MOUSE_PLUGOUT, KMSG_PRIO_HIGH);

    usbMouse_StopWork(usbMouse);

    DMSG_INFO("\n");
    DMSG_INFO("*****************************************************************\n");
    DMSG_INFO("*                                                               *\n");
    DMSG_INFO("* USB HOST UnRegist \"%s\" \"%s\" successful\n",
              usbMouse->ClassName, usbMouse->DevName);
    DMSG_INFO("*                                                               *\n");
    DMSG_INFO("*****************************************************************\n");
    DMSG_INFO("\n");

    /* unregist device */
    if(usbMouse->MouseRegHdle)
    {
        USB_OS_DEV_UNREG(usbMouse->MouseRegHdle);
    }

    /* kill thread */
    UsbKillThread(usbMouse->MouseThdId , usbMouse->MouseThreadSemi);

    if(usbMouse->notify_complete)
    {
        USB_OS_SemDel(usbMouse->notify_complete, &err);
        usbMouse->notify_complete = NULL;
    }

    if(usbMouse->MouseThreadSemi)
    {
        USB_OS_SemDel(usbMouse->MouseThreadSemi, &err);
        usbMouse->MouseThreadSemi = NULL;
    }

    UsbMouse_DriftControl_Exit(usbMouse);

    USB_OS_ENTER_CRITICAL(cup_sr);
    HidDev->Extern = NULL;
    USB_OS_EXIT_CRITICAL(cup_sr);

    USB_OS_FREE(usbMouse);

    return USB_ERR_SUCCESS;
}

