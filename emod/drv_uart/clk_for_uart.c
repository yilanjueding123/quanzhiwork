/*
****************************************************************************************************
*                                               MELIS
*                               the Easy Portable/Player Develop Kits
*                                           SDMMC Module
*
*                           (c) Copyright 2011-2014, All winners Co,Ld.
*                                       All Rights Reserved
*
* File    : clk_for_uart.c
* By      : james.deng
* Version : 1.0.0
* Date    : 2011-12-25
* Descript:
* Update  : <date>          <author>            <version>           <notes>
*           2011-12-25      james.deng          1.0.0               build the file.
****************************************************************************************************
*/

#include "drv_uart_i.h"


static __krnl_event_t *uart_clk_adjust_sem[UART_PORT_COUNT] = {NULL};

static CSP_CCM_modClkNo_t uart_mod_clk[UART_PORT_COUNT] =
{
    CSP_CCM_MOD_CLK_APB_UART0,
    CSP_CCM_MOD_CLK_APB_UART1,
    CSP_CCM_MOD_CLK_APB_UART2
};

static __s32 cb_UART_ClockChange(__u32 cmd, __s32 aux, __u32 port)
{
    switch (cmd)
    {
    case CLK_CMD_SCLKCHG_REQ:
    {
        uart_lock(port);

        return EPDK_OK;
    }

    case CLK_CMD_SCLKCHG_DONE:
    {
        BSP_UART_SetBaudrate(dev_uart[port].hdle, dev_uart[port].baudrate);
        uart_unlock(port);

        return EPDK_OK;
    }
    }

    return EPDK_FAIL;
}

#define cb_UART_ClockChangeGen(n) \
static __s32 cb_UART##n##_ClockChange(__u32 cmd, __s32 aux) \
{ \
    return cb_UART_ClockChange(cmd, aux, n); \
}

cb_UART_ClockChangeGen(0)
cb_UART_ClockChangeGen(1)
cb_UART_ClockChangeGen(2)

static __s32 (*Callback[])(__u32 cmd, __s32 aux) =
{
    cb_UART0_ClockChange,
    cb_UART1_ClockChange,
    cb_UART2_ClockChange
};

/*
****************************************************************************************************
*                                       uart_pm_init
*
* Description: Init the uart pm.
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
****************************************************************************************************
*/
__s32 uart_pm_init(__u32 nPort)
{
    __s32 ret;
    __u8  err;

    // create semaphore for uart
    uart_clk_adjust_sem[nPort] = esKRNL_SemCreate(1);
    if (NULL == uart_clk_adjust_sem[nPort])
    {
        __wrn("crate semaphore for uart%d failed\n", nPort);
        return EPDK_FAIL;
    }

    // register callback
    ret = esCLK_MclkRegCb(uart_mod_clk[nPort], Callback[nPort]);
    if (ret)
    {
        esKRNL_SemDel(uart_clk_adjust_sem[nPort], OS_DEL_ALWAYS, &err);
    }

    return ret;
}

/*
****************************************************************************************************
*                                       uart_pm_exit
*
* Description: Exit the uart pm.
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
****************************************************************************************************
*/
__s32 uart_pm_exit(__u32 nPort)
{
    __u8 err;

    // delete semaphore for uart
    if (uart_clk_adjust_sem[nPort])
    {
        esKRNL_SemDel(uart_clk_adjust_sem[nPort], OS_DEL_ALWAYS, &err);
    }

    // unregister callback
    esCLK_MclkUnregCb(uart_mod_clk[nPort], Callback[nPort]);

    return EPDK_OK;
}

/*
****************************************************************************************************
*                                       uart_lock
*
* Description: Lock the uart.
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
****************************************************************************************************
*/
__s32 uart_lock(__u32 nPort)
{
    __u8 err;

    esKRNL_SemPend(uart_clk_adjust_sem[nPort], 0, &err);

    return EPDK_OK;
}

/*
****************************************************************************************************
*                                       uart_unlock
*
* Description: Unlock the uart.
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
****************************************************************************************************
*/
__s32 uart_unlock(__u32 nPort)
{
    esKRNL_SemPost(uart_clk_adjust_sem[nPort]);

    return EPDK_OK;
}
