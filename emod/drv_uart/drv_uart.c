/*
****************************************************************************************************
*                                               MELIS
*                               the Easy Portable/Player Develop Kits
*                                           SDMMC Module
*
*                           (c) Copyright 2011-2014, All winners Co,Ld.
*                                       All Rights Reserved
*
* File    : drv_uart.c
* By      : james.deng
* Version : 1.0.0
* Date    : 2011-12-24
* Descript:
* Update  : <date>          <author>            <version>           <notes>
*           2011-12-24      james.deng          1.0.0               build the file.
****************************************************************************************************
*/

#include <string.h>
#include "drv_uart_i.h"

__dev_uart_t dev_uart[UART_PORT_COUNT];

static __drv_uart_t drv_uart;

#define	ADJUST_CLOCK_FREQUENCY_ENABLE	0

__s32 DRV_UART_MInit(void)
{
    __msg("uart driver init\n");

    memset(&drv_uart, 0, sizeof(__drv_uart_t));
    memset(&dev_uart, 0, sizeof(__dev_uart_t) * UART_PORT_COUNT);

    return BSP_UART_Init();
}

__s32 DRV_UART_MExit(void)
{
    __msg("uart driver exit\n");

    memset(&drv_uart, 0, sizeof(__drv_uart_t));
    memset(&dev_uart, 0, sizeof(__dev_uart_t) * UART_PORT_COUNT);

    return BSP_UART_Exit();
}

__mp *DRV_UART_MOpen(__u32 mid, __u32 mod)
{
    __s32 cpu_sr;

    ENTER_CRITICAL(cpu_sr);
    if (1 == drv_uart.used)
    {
        __msg("uart used by someone else\n");
        EXIT_CRITICAL(cpu_sr);
        return (__mp *)0;
    }
    drv_uart.used = 1;
    EXIT_CRITICAL(cpu_sr);

    drv_uart.mid = mid;

    return (__mp *)&drv_uart;
}

__s32 DRV_UART_MClose(__mp *mp)
{
    __s32 cpu_sr;

    ENTER_CRITICAL(cpu_sr);
    if (0 == drv_uart.used)
    {
        EXIT_CRITICAL(cpu_sr);
        return EPDK_FAIL;
    }
    drv_uart.used = 0;
    EXIT_CRITICAL(cpu_sr);

    return EPDK_OK;
}

__u32 DRV_UART_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return 0;
}

__u32 DRV_UART_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return 0;
}

__s32 DRV_UART_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    __s32 cpu_sr;

    switch (cmd)
    {
    case DRV_CMD_PLUGIN:
    {
        char          uart_para[16] = "uart_para0";
        char          uart[8] = "UART0";
        __s32         uart_used;
        __s32         ret;
        __uart_para_t para;

        // fix when user request debug port
        // ...

        __msg("uart%d plugin...\n", aux);

        uart_para[9] = '0' + aux;
        uart_used = 0;
        ret = esCFG_GetKeyValue(uart_para, "uart_used", &uart_used, 1);
        if (ret || (0 == uart_used))
        {
            eLIBs_printf("check uart%d used: %d\n", aux, uart_used);
            return EPDK_FAIL;
        }

        // request uart
        dev_uart[aux].baudrate = (__u32)pbuffer;
        if (0 == dev_uart[aux].baudrate)
            dev_uart[aux].baudrate = 115200;

        para.nEvenParity   = 0;
        para.nParityEnable = 0;
        para.nStopBit      = 0;
        para.nDataLen      = 0x03;

        dev_uart[aux].port = aux;
        dev_uart[aux].hdle = BSP_UART_Request(aux, dev_uart[aux].baudrate, &para, UART_BUFFER_SIZE);
        if (NULL == dev_uart[aux].hdle)
        {
            eLIBs_printf("request uart%d failed\n", aux);
            return EPDK_FAIL;
        }
#if ADJUST_CLOCK_FREQUENCY_ENABLE
        // uart pm init
        ret = uart_pm_init(aux);
        if (ret)
        {
            eLIBs_printf("init uart%d pm failed\n", aux);
            return EPDK_FAIL;
        }
#endif

        // register uart
        uart[4] = '0' + aux;
        dev_uart[aux].hReg = esDEV_DevReg("BUS", uart, &uart_devop, (void *)&dev_uart[aux]);
        if (NULL == dev_uart[aux].hReg)
        {
            eLIBs_printf("uart%d register failed\n", aux);
            return EPDK_FAIL;
        }

        dev_uart[aux].used = 0;

        ENTER_CRITICAL(cpu_sr);
        drv_uart.busycnt++;
        EXIT_CRITICAL(cpu_sr);

        return EPDK_OK;
    }

    case DRV_CMD_PLUGOUT:
    {
        __msg("uart%d plugout...\n", aux);

        ENTER_CRITICAL(cpu_sr);
        drv_uart.busycnt--;
        EXIT_CRITICAL(cpu_sr);

        dev_uart[aux].used = 0;

        // unregister uart
        if (dev_uart[aux].hReg)
        {
            esDEV_DevUnreg(dev_uart[aux].hReg);
            dev_uart[aux].hReg = NULL;
        }
        else
        {
            eLIBs_printf("uart%d has not plugin\n", aux);
        }
#if ADJUST_CLOCK_FREQUENCY_ENABLE
        uart_pm_exit(aux);
#endif
        // release uart
        if (dev_uart[aux].hdle)
        {
            BSP_UART_Release(dev_uart[aux].hdle);
            dev_uart[aux].hdle = NULL;
        }

        return EPDK_OK;
    }

    case DRV_CMD_GET_STATUS:
    {
        __s32 status;

        ENTER_CRITICAL(cpu_sr);
        if (drv_uart.busycnt)
        {
            status = DRV_STA_BUSY;
        }
        else
        {
            status = DRV_STA_FREE;
        }
        EXIT_CRITICAL(cpu_sr);

        return status;
    }

    default:
        break;
    }

    return EPDK_FAIL;
}
