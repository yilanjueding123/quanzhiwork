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
#ifndef	__USB_OS_PLATFORM_H__
#define	__USB_OS_PLATFORM_H__

#include "usb_host_config.h"


//---------------------------------------------------------------
//  typedef
//---------------------------------------------------------------
#define	USB_OS_TRUE				1
#define	USB_OS_FALSE			0


#define	USB_MEM_FILE_TRIGGER						__FILE__
#define	USB_MEM_LINE_TRIGGER						__LINE__

#define	 USB_OS_KERNEL_EVENT						__krnl_event_t
#define	 USB_OS_KERNEL_SOFT_TIMER					__krnl_stmr_t
#define	 USB_TIMER_CALLBACK							OS_TMR_CALLBACK
#define	 USB_TIMER_PERIOD_ONE						OS_TMR_OPT_ONE_SHOT	//只触发一次
#define	 USB_TIMER_PERIOD_MORE						OS_TMR_OPT_PERIODIC	//触发多次
#define	 USB_OS_EPDK_TRUE							EPDK_TRUE
#define  USB_OS_EPDK_FALSE							EPDK_FALSE
#define	 USB_OS_SEM_DATA							OS_SEM_DATA
#define	 USB_OS_ISR_FORMAT							__pISR_t
#define	 USB_OS_HANDLE								__hdle		//epos的handle

#define  USB_OS_ENTER_CRITICAL(sr)					ENTER_CRITICAL(sr)
#define	 USB_OS_EXIT_CRITICAL(sr)					EXIT_CRITICAL(sr)

void *USB_OS_MALLOC(u32 size, u8 *file_name , u32 line_nr);
void USB_OS_FREE(void *addr );


/* 字符串操作 */
#define	 USB_OS_STRLEN(str)	                   		eLIBs_strlen(str)
#define	 USB_OS_STRCPY(pdest, psrc)               	eLIBs_strcpy(pdest, psrc)
#define	 USB_OS_STRNCPY(pdest, psrc, len_max)     	eLIBs_strncpy(pdest, psrc, len_max)
#define	 USB_OS_STRCAT(pdest, pstr_cat)				eLIBs_strcat(pdest, pstr_cat)
#define	 USB_OS_STRNCAT(pdest, pstr_cat, len_max)	eLIBs_strncat(pdest, pstr_cat, len_max)
#define	 USB_OS_STRCMP(p1_str, p2_str)	            eLIBs_strcmp(p1_str, p2_str)
#define	 USB_OS_STRNCMP(p1_str, p2_str, len_max)	eLIBs_strncmp(p1_str, p2_str, len_max)
#define	 USB_OS_STRCHR(pstr, srch_char)         	eLIBs_strchr(pstr, srch_char)
#define	 USB_OS_STRNCHR(pstr, len_max, srch_char)	eLIBs_strnchr(pstr, len_max, srch_char)
#define	 USB_OS_STRCHRLAST(pstr, srch_char)	        eLIBs_strchrlast(pstr, srch_char)
#define	 USB_OS_STRSTR(pstr, srch_str)	         	eLIBs_strstr(pstr, srch_str)
#define	 USB_OS_uint2str(uint32, str)             	eLIBs_uint2str_dec(uint32, str)

/* 内存管理 */
#define	 USB_OS_PAGE_MALLOC_PAGE_SIZE				(1024)
#define	 USB_OS_PAGE_MALLOC(npage)					(esMEMS_Palloc(npage,(MEMS_PALLOC_MODE_BNK_NONE | MEMS_PALLOC_MODE_BND_2K)))	// 1k对其
#define	 USB_OS_PAGE_FREE(addr , npage)				(esMEMS_Pfree((void *)addr , (__u32)npage))

#define  USB_OS_MEMSET(addr , value , size)			eLIBs_memset(addr , value , size)
#define  USB_OS_MEMCPY(dest , source , count)	    eLIBs_memcpy(dest , source ,count)
#define  USB_OS_MEM_CMP(dest , source , count)	    eLIBs_memcmp(dest , source ,count)

/* 时钟管理 */
#define  USB_esCLK_Open(mclkno)  	                esCLK_OpenMclk(mclkno)
#define  USB_esCLK_Close(hMclk)                  	esCLK_CloseMclk(hMclk)
#define  USB_esCLK_OnOff(hMclk, onoff)            	esCLK_MclkOnOff(hMclk, onoff)

/* 信号量 */
#define  USB_OS_SEMI_DEL_NO_PEND 					OS_DEL_NO_PEND
#define  USB_OS_SEMI_DEL_ALWAYS						OS_DEL_ALWAYS
#define  USB_OS_SEM_DATA							OS_SEM_DATA

#define  USB_OS_SemCreate(x)						esKRNL_SemCreate(x)
#define  USB_OS_SemDel(x, err)						esKRNL_SemDel(x, USB_OS_SEMI_DEL_ALWAYS, err)
#define  USB_OS_SemPend(x, timeout, perr_u8)		esKRNL_SemPend(x, timeout, perr_u8)
#define  USB_OS_SemPost(x)							esKRNL_SemPost(x)
#define	 USB_OS_SemQuery      		  				esKRNL_SemQuery

#define	USB_OS_SEMI_CREATE(x)						esKRNL_SemCreate(x)
#define	USB_OS_SEMI_DELETE(x,err)					esKRNL_SemDel(x,USB_OS_SEMI_DEL_ALWAYS,err)
#define	USB_OS_SEMI_PEND(x,timeout,perr_u8)			esKRNL_SemPend(x,timeout,perr_u8)
#define	USB_OS_SEMI_POST(x)							esKRNL_SemPost(x)		//

#define	USB_OS_COMPLETE_CREATE()					esKRNL_SemCreate(0)	// return == NULL ，失败
#define	USB_OS_COMPLETE_DELETE(pevnet,err)			esKRNL_SemDel(pevnet,USB_OS_SEMI_DEL_ALWAYS,err)
#define	USB_OS_COMPLETE_PEND(pevent,timeout,perror)	esKRNL_SemPend(pevent,timeout,perror)	//
#define	USB_OS_COMPLETE_POST(pevnet)				esKRNL_SemPost(pevnet)		//

/* timer */
#define	USB_OS_ePOS_TICK_n_ms	2					//epos一次tick的时间间隔为2.5ms
#define	USB_esKRNL_TmrCreate(period_ms,one_or_more,callback,callback_arg) 	esKRNL_TmrCreate(  (period_ms /USB_OS_ePOS_TICK_n_ms) , one_or_more,callback,callback_arg)
#define	USB_esKRNL_TmrDel(ptmr)						esKRNL_TmrDel (ptmr)
#define	USB_esKRNL_TmrStart(ptmr)					esKRNL_TmrStart(ptmr)
#define	USB_esKRNL_TmrStop(ptmr)					esKRNL_TmrStop(ptmr, OS_TMR_OPT_NONE, NULL)
#define	USB_esKRNL_TmrError(ptmr)					esKRNL_TmrError(ptmr)

#define	USB_OS_TIME_DELAY_10MS(ms)					esKRNL_TimeDly(ms)

/* 线程管理 */
#define	USB_OS_THREAD_CREATE(thread_proc , proc_para , stack_size , id_priolevel)	esKRNL_TCreate(thread_proc , proc_para , stack_size , id_priolevel)
#define	USB_OS_THREAD_DELREQ(p)						    esKRNL_TDelReq(p)
#define	USB_OS_THREAD_DELETE(p)						    esKRNL_TDel(p)
#define	USB_OS_THREAD_DEL_REQ						    OS_TASK_DEL_REQ
#define	USB_OS_THREAD_NOT_EXIST						    OS_TASK_NOT_EXIST

/* 中断管理, return 全为u32 */
#define	USB_OS_ISR_INSTALL(irq_nr_u8 ,p_isr , p_arg)	esINT_InsISR(irq_nr_u8, p_isr, p_arg)
#define	USB_OS_ISR_UNINSTALL(irq_nr_u8 ,p_isr )			esINT_UniISR(irq_nr_u8, p_isr)
#define	USB_OS_ISR_ENABLE_INT(irq_nr_u8)				esINT_EnableINT(irq_nr_u8)
#define	USB_OS_ISR_DISABLE_INT(irq_nr_u8)				esINT_DisableINT(irq_nr_u8)

/* cache */
#define   USB_OS_CLEAN_DCACHE(addr, len)     		    eLIBs_CleanDCacheRegion(addr, len)
#define   USB_OS_FLUSH_DCACHE(addr, len)     		    eLIBs_FlushDCacheRegion(addr, len)

/* dma */
#define  USB_OS_DMA_Request(user, type)                 esDMA_Request(user, type)
#define  USB_OS_DMA_Release(dma)                        esDMA_Release(dma)
#define  USB_OS_DMA_Setting(dma, pArg)                  esDMA_Setting(dma, pArg)
#define  USB_OS_DMA_Start(dma, saddr, daddr, len)       esDMA_Start(dma, saddr, daddr, len)
#define  USB_OS_DMA_Stop(dma)                           esDMA_Stop(dma)
#define  USB_OS_DMA_Restart(dma)                        esDMA_Restart(dma)
#define  USB_OS_DMA_QueryStat(dma)                      esDMA_QueryStat(dma)
#define  USB_OS_DMA_QuerySrc(dma)                       esDMA_QuerySrc(dma)
#define  USB_OS_DMA_QueryDst(dma)                       esDMA_QueryDst(dma)
#define  USB_OS_DMA_EnableINT(dma, type)                esDMA_EnableINT(dma, type)
#define  USB_OS_DMA_DisableINT(dma, type)               esDMA_DisableINT(dma, type)
#define  USB_OS_DMA_QueryRestCount(dma)                 esDMA_QueryRestCount(dma)
#define  USB_OS_DMA_ChangeMode(dma, mode)               esDMA_ChangeMode(dma, mode)
#define  USB_OS_DMA_RegDmaHdler(dma, type, hdler, arg)  esDMA_RegDmaHdler(dma, type, hdler, arg)
#define  USB_OS_DMA_UnregDmaHdler(dma, type, hdler)     esDMA_UnregDmaHdler(dma, type, hdler)
#define  USB_OS_DMA_Information(void)                   esDMA_Information(void)

#define   USB_OS_VA2PA(addr)                            esMEMS_VA2PA(addr)

/* 设备管理 */
#define	USB_OS_DEV_REG(node_class, node_sub_dev_name, dev_ops, p_arg)	esDEV_DevReg(node_class, node_sub_dev_name, dev_ops, p_arg)
#define	USB_OS_DEV_UNREG(dev_handl)										esDEV_DevUnreg(dev_handl)

#define	USB_OS_DEV_DEVOPS							    __dev_devop_t
#define	USB_OS_NO_ERR								    OS_NO_ERR

/* 消息管理 */
#define  USB_OS_esKRNL_GetCallBack(cb)					esKRNL_GetCallBack(cb)
#define  USB_OS_esKRNL_CallBack(cb, p_arg)				esKRNL_CallBack(cb, p_arg)
#define  USB_OS_esKSRV_SendMsg(msgid, prio)				esKSRV_SendMsg(msgid, prio)

//---------------------------------------------------------------
//
//---------------------------------------------------------------
static int usb_atomic_read32(int *addr)
{
    __cpu_sr cpu_sr = 0 ;
    int ret = 0;

    USB_OS_ENTER_CRITICAL(cpu_sr);
    ret = *addr;
    USB_OS_EXIT_CRITICAL(cpu_sr);
    return ret;
}
static void usb_atomic_write32(int *addr , int value)
{
    __cpu_sr cpu_sr = 0 ;

    USB_OS_ENTER_CRITICAL(cpu_sr);
    *addr = value;
    USB_OS_EXIT_CRITICAL(cpu_sr);
}

static  void  usb_atomic_set_bit(int bit_nr, volatile u32 *addr)
{
    __cpu_sr cpu_sr = 0 ;
    int	mask = 0;
    volatile u32 *p = addr;

    p += ((bit_nr & 0xffffffe0) >> 5);
    mask = 1 << (bit_nr & 0x1f);

    USB_OS_ENTER_CRITICAL(cpu_sr);
    (*p) |= mask;
    USB_OS_EXIT_CRITICAL(cpu_sr);
}

static  void  usb_atomic_clear_bit(int bit_nr, volatile u32 *addr)
{
    __cpu_sr cpu_sr = 0 ;
    int	mask;
    volatile u32 *p = addr;

    p += ((bit_nr & 0xffffffe0) >> 5);
    mask = 1 << (bit_nr & 0x1f);

    USB_OS_ENTER_CRITICAL(cpu_sr);
    (*p) &= (~mask);
    USB_OS_EXIT_CRITICAL(cpu_sr);
}

//判断p的第nr个bit是否为1，这里以0为base
static int usb_atomic_test_bit(int bit_nr,  u32 *addr)
{
    __cpu_sr cpu_sr = 0 ;
    u32 *p = addr;
    s32 ret = 0;

    p += ((bit_nr & 0xffffffe0) >> 5);

    USB_OS_ENTER_CRITICAL(cpu_sr);
    ret = ((*p) >> (bit_nr & 31)) & 1;
    USB_OS_EXIT_CRITICAL(cpu_sr);

    return ret ;
}

void usb_set_bit32(int bit_nr, volatile u32 *addr);
u32  usb_test_bit32(int bit_nr, volatile u32 *addr);
void usb_clear_bit32(int bit_nr, volatile u32 *addr);

//判断p的第nr个bit是否为1,请clear该bit，这里以0为base
static int usb_atomic_test_and_clear_bit(int bit_nr, volatile u32 *addr)
{
    int	mask, retval;
    __cpu_sr cpu_sr = 0 ;

    mask = 1 << (bit_nr & 0x1f);

    USB_OS_ENTER_CRITICAL(cpu_sr);
    retval = ((mask & *addr) != 0);
    *addr &= ~mask;
    USB_OS_EXIT_CRITICAL(cpu_sr);

    return retval;
}

static void usb_atomic_inc32(u32   *addr)
{
    __cpu_sr cpu_sr = 0 ;

    USB_OS_ENTER_CRITICAL(cpu_sr);
    *addr = *addr + 1 ;
    USB_OS_EXIT_CRITICAL(cpu_sr);
}

static void usb_atomic_dec32(u32 *addr )
{
    __cpu_sr cpu_sr = 0 ;

    USB_OS_ENTER_CRITICAL(cpu_sr);
    *addr = *addr - 1 ;
    USB_OS_EXIT_CRITICAL(cpu_sr);
}

//------------------------------------------------------
//  数值转换
//------------------------------------------------------
static USB_INLINE u16 get_be16(u8 *buf)
{
    return ((__u16) buf[0] << 8) | ((__u16) buf[1]);
}

static USB_INLINE u32 get_be32(u8 *buf)
{
    return ((__u32) buf[0] << 24) | ((__u32) buf[1] << 16) |
           ((__u32) buf[2] << 8) | ((__u32) buf[3]);
}

static USB_INLINE void put_be16(__u8 *buf, __u16 val)
{
    buf[0] = (__u8)(val >> 8);
    buf[1] = (__u8)val;
}

static USB_INLINE void put_be32(__u8 *buf, __u32 val)
{
    buf[0] = (__u8)(val >> 24);
    buf[1] = (__u8)(val >> 16);
    buf[2] = (__u8)(val >> 8);
    buf[3] = (__u8)val;
}

//------------------------------------------------------
//  互斥锁
//------------------------------------------------------
static USB_INLINE void UsbLock(USB_OS_KERNEL_EVENT *Lock)
{
    __u8 err = 0;

    if(Lock == NULL)
    {
        DMSG_PANIC("ERR: input error\n");
        return ;
    }

    USB_OS_SEMI_PEND(Lock, 0, &err);
    if(err != USB_OS_NO_ERR)
    {
        DMSG_PANIC("ERR: USB_OS_SEMI_PEND failed\n");
        return ;
    }

    return ;
}

static USB_INLINE void UsbUnLock(USB_OS_KERNEL_EVENT *Lock)
{
    if(Lock)
    {
        USB_OS_SEMI_POST(Lock);
    }
    else
    {
        DMSG_PANIC("ERR: semi is NULL\n");
    }
}

u16  le16_to_cpu(u16 x);
u32  le32_to_cpu(u32 x);

#define cpu_to_le16(x) le16_to_cpu((x))
#define cpu_to_le32(x) le32_to_cpu(x)


#define USB_MIN(x,y) 	((x) < (y) ? (x) : (y))
#define USB_MAX(x,y)	((x > (y)) ? (x) : (y))

void 	USB_OS_DELAY_x_MS(u32 ms);


//------------------------------------------------------
//
//------------------------------------------------------
__u8  UsbCreateThread(void *Thd, void *p_arg, __u32 stksz, __u16 plvl);
__s32 UsbThreadWakeUp(USB_OS_KERNEL_EVENT *ThreadSemi);
__s32 UsbThreadSleep(USB_OS_KERNEL_EVENT *ThreadSemi);
void KillThreadSelf(void);
void TryToKillThreadSelf(char *ThrdName);
void UsbKillThread(__u8 ThreadId, USB_OS_KERNEL_EVENT *ThreadSemi);




#endif	//end __USB_OS_PLATFORM_H__



