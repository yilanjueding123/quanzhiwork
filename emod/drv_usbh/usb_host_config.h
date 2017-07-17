#ifndef	_USB_HOST_CONFIG_H_
#define	_USB_HOST_CONFIG_H_

#include "usb_host_base_types.h"
#include "epdk.h"

//-----------------------------------------------------------
//  usb host gate
//-----------------------------------------------------------
//#define  USBH_MSC_DEBUG_GATE     	/* msc层的调试开关 		*/
#define  USBH_HUB_DEBUG_GATE     	/* hub层的调试开关 		*/
//#define  USBH_HCD_DEBUG_GATE     	/* hcd层的调试开关 		*/
//#define  USBH_PF_DEBUG_GATE     	/* 检查内存泄漏调试开关 */
//#define  USBH_DEBUG_GATE     		/* 调试总开关 			*/
#define  USBH_HID_MOUSE_TEST     	/* USB 鼠标测试开关 */


//-----------------------------------------------------------
// USB HOST 线程优先级
//-----------------------------------------------------------
#define  THREAD_LEVEL_MSC_MEDIA_CHANGE		(0x00 | 0x4)
#define  THREAD_LEVEL_MSC_SCAN				(0x00 | 0x2)
#define  THREAD_LEVEL_MSC_MAIN				(0x00 | 0x1)

#define	 HUB_THREAD_PID_PRIO_LEVEL			(0x00 | 0x1)

#define  THREAD_LEVEL_HID_CLIENT			(0x00 | 0x1)

//-----------------------------------------------------------
// USB HOST 时间处理
//-----------------------------------------------------------
#define	USB_HOST_RH_TIMEROUT			10000		//目前临时为5s，将来正式版本为0.5s


//-----------------------------------------------------------
// USB HOST debug
//-----------------------------------------------------------

#define USB_ASSERT(is_true,msg)  {if(! is_true){DMSG_PANIC("ASSERT: fail,%s",msg ? msg : "");}}


#if 0
#define __msg(...) (eLIBs_printf("L%d(%s):", __LINE__, __FILE__),\
    						     eLIBs_printf(__VA_ARGS__))
#define __wrn(...) (eLIBs_printf("L%d(%s):", __LINE__, __FILE__),\
  						     eLIBs_printf(__VA_ARGS__))
#define __inf(...) (eLIBs_printf("L%d(%s):", __LINE__, __FILE__),\
  						     eLIBs_printf(__VA_ARGS__))
#endif



#if	(0 )
#define USBH_MEM_DEBUG   __msg
#else
#define USBH_MEM_DEBUG(...)
#endif

//看似错误，其实不是错误的打印
#if  0
#define DMSG_HOLD   __msg
#else
#define DMSG_HOLD(...)
#endif

#if  0
#define DMSG_TEST_GATEWAY   __msg
#else
#define DMSG_TEST_GATEWAY(...)
#endif

#if  1
#define DMSG_TEST   __inf
#else
#define DMSG_TEST(...)
#endif

#if  0
#define DMSG_TEMP_TEST   __inf
#else
#define DMSG_TEMP_TEST(...)
#endif

#if  1
#define DMSG_MSC_TEST   __inf
#else
#define DMSG_MSC_TEST(...)
#endif

#if  1
#define DMSG_HUB_TEST   __inf
#else
#define DMSG_HUB_TEST(...)
#endif

#if  0
#define DMSG_HCD_TEST   __inf
#else
#define DMSG_HCD_TEST(...)
#endif

#if  1
#define DMSG_INFO   __inf
#else
#define DMSG_INFO(...)
#endif

#if	 1
#define DMSG_WRN   __wrn
#else
#define DMSG_WRN(...)
#endif

#if	 1
#define DMSG_PANIC   __wrn
#else
#define DMSG_PANIC(...)
#endif


#if	 1
#define DMSG_MOUSE_TEST   __inf
#else
#define DMSG_MOUSE_TEST(...)
#endif


#if	 1
#define DMSG_HID_TEST   __inf
#else
#define DMSG_HID_TEST(...)
#endif


#endif	//_USB_HOST_CONFIG_H_





