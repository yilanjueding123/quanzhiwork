/*
*******************************************************************************
*                                              usb host module
*
*                             Copyright(C), 2006-2008, SoftWinners Co., Ltd.
*											       All Rights Reserved
*
* File Name :
*
* Author : GLHuang(HoLiGun)
*
* Version : 1.0
*
* Date : 2008.05.xx
*
* Description :
*
* History :
********************************************************************************************************************
*/
#include "usb_host_config.h"
#include "usb_host_base_types.h"
#include "usb_os_platform.h"
#include "error.h"
#include "usbh_debug.h"

//-------------------------------------
//  Arm 大小端转化
//-------------------------------------
u16  le16_to_cpu( u16 x)
{
    return x;
}

u32  le32_to_cpu(u32 x)
{
    return x;
}

//-------------------------------------
//  Arm 大小端转化
//-------------------------------------

void usb_set_bit32(int bit_nr, volatile u32 *addr)
{
    __cpu_sr sr = 0 ;
    int	mask;

    if(bit_nr > 32 || bit_nr < 0 || addr == NULL)
    {
        DMSG_PANIC("PANIC : usb_set_bit32() input bit_nr > 32 || bit_nr < 0\n");
        return;
    }

    mask = 1 << (bit_nr & 0x1f);
    USB_OS_ENTER_CRITICAL(sr);
    (*addr) |= mask;
    USB_OS_EXIT_CRITICAL(sr);
}

//return 0	:	该bit没有设置
//return 1	:	该bit设置了
u32 usb_test_bit32(int bit_nr, volatile u32 *addr)
{
    int	mask;

    if(bit_nr > 32 || bit_nr < 0 || addr == NULL)
    {
        DMSG_PANIC("PANIC : usb_test_bit32() input bit_nr > 32 || bit_nr < 0\n");
        return 0;
    }

    mask = 1 << (bit_nr & 0x1f);
    if((*addr) & mask)
    {
        return 1;
    }

    return 0;
}

void usb_clear_bit32(int bit_nr, volatile u32 *addr)
{
    __cpu_sr sr = 0 ;
    int	mask;

    if(bit_nr > 32 || bit_nr < 0 || addr == NULL)
    {
        DMSG_PANIC("PANIC : usb_clear_bit32() input bit_nr > 32 || bit_nr < 0\n");
        return;
    }

    mask = 1 << (bit_nr & 0x1f);
    USB_OS_ENTER_CRITICAL(sr);
    *addr &= ~mask;
    USB_OS_EXIT_CRITICAL(sr);
}


void USB_OS_DELAY_x_MS(__u32 ms)
{
    __u32 time = 0;

    if(ms == 0)
    {
        return;
    }

    /* 不满10ms就算成10ms */
    if(ms % 10)
    {
        time++;
    }

    time += ms / 10;

    //esKRNL_TimeDlyHMSM(0,0,0,ms);
    esKRNL_TimeDly(time);
}

//-------------------------------------
//  内存管理
//-------------------------------------

#ifdef USBH_PF_DEBUG_GATE

void *USB_OS_MALLOC(u32 size, u8 *file_name , u32 line_nr)
{
    return pf_malloc(size, file_name, line_nr);
}

void USB_OS_FREE(void *addr )
{
    pf_free(addr);
}

#else

void *USB_OS_MALLOC(u32 size, u8 *file_name , u32 line_nr)
{
    return esMEMS_Malloc(0 , size);
}

void USB_OS_FREE(void *addr )
{
    esMEMS_Mfree(0 , (void *)addr );
}

#endif

//-------------------------------------
//  线程管理
//-------------------------------------
/* 创建线程 */
__u8 UsbCreateThread(void *Thd, void *p_arg, __u32 stksz, __u16 plvl)
{
    return USB_OS_THREAD_CREATE((__pTHD_t)Thd, p_arg, stksz, plvl);
}

/*

说明: 在某些应用里, 线程没有sleep, 而其他模块回去wake up线程, 这会造成OSCnt一直累加,
      线程需要尝试很OSCnt次sleep，才会真正的睡眠状态。这样不能够真实的反映出程序的
      执行情况，因此这里需要根据OSCnt的值，来判断线程是否真的需要睡眠。

*/
__s32 UsbThreadWakeUp(USB_OS_KERNEL_EVENT *ThreadSemi)
{
    OS_SEM_DATA sem_data;
    __u8 ret = 0;

    //--<1>--获得信号量的计数
    ret = USB_OS_SemQuery(ThreadSemi, &sem_data);
    if(ret != USB_OS_NO_ERR)
    {
        DMSG_PANIC("ERR: UsbThreadWakeUp: ret = %d\n", ret);
        return USB_ERR_UNKOWN_ERROR;
    }

    //--<2>--OSCnt == 0，才能加1
    if(sem_data.OSCnt == 0)
    {
        USB_OS_SemPost(ThreadSemi);
    }
    else
    {
        //DMSG_PANIC("Wrn: thread is not sleep, need not wake up. OSCnt = %d\n", sem_data.OSCnt);
    }

    return USB_ERR_SUCCESS;
}


__s32 UsbThreadSleep(USB_OS_KERNEL_EVENT *ThreadSemi)
{
    __u8 err = 0;

    USB_OS_SEMI_PEND(ThreadSemi, 0, &err);
    if(err != USB_OS_NO_ERR)
    {
        DMSG_PANIC("ERR: USB_OS_SEMI_PEND failed\n");
        return USB_ERR_UNKOWN_ERROR;
    }

    return USB_ERR_SUCCESS;
}

void KillThreadSelf(void)
{
    USB_OS_THREAD_DELETE(OS_PRIO_SELF);
}

/* 请求杀死线程 */
void TryToKillThreadSelf(char *ThrdName)
{
    if(USB_OS_THREAD_DELREQ(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
    {
        DMSG_INFO("[Thread]: \"%s\"thread kill itself\n", ThrdName);
        USB_OS_THREAD_DELETE(OS_PRIO_SELF);
    }
}

/* 杀死线程 */
void UsbKillThread(__u8 ThreadId, USB_OS_KERNEL_EVENT *ThreadSemi)
{
    /* kill thread */
    while(USB_OS_THREAD_DELREQ(ThreadId) != OS_TASK_NOT_EXIST)
    {
        if(ThreadSemi)
        {
            UsbThreadWakeUp(ThreadSemi);
        }

        esKRNL_TimeDly(3);
    }
}


