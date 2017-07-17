/*
****************************************************************************************************
*                                               MELIS
*                               the Easy Portable/Player Develop Kits
*                                           SDMMC Module
*
*                           (c) Copyright 2011-2014, All winners Co,Ld.
*                                       All Rights Reserved
*
* File    :
* By      : james.deng
* Version : 1.0.0
* Date    : 2011-12-25
* Descript:
* Update  : <date>          <author>            <version>           <notes>
*           2011-12-25      james.deng          1.0.0               build the file.
****************************************************************************************************
*/

#include "drv_uart_i.h"

static __hdle DEV_UART_Open(void *open_arg, __u32 mode)
{
    __s32        cpu_sr;
    __dev_uart_t *pDev = (__dev_uart_t *)open_arg;

    ENTER_CRITICAL(cpu_sr);
    if (1 == pDev->used)
    {
        __wrn("uart%d open failed, maybe used by someone else\n", pDev->port);
        EXIT_CRITICAL(cpu_sr);
        return (__hdle)0;
    }
    pDev->used = 1;
    EXIT_CRITICAL(cpu_sr);

    return (__hdle)pDev;
}

static __s32 DEV_UART_Close(__hdle hDev)
{
    __s32        cpu_sr;
    __dev_uart_t *pDev = (__dev_uart_t *)hDev;

    ENTER_CRITICAL(cpu_sr);
    if (0 == pDev->used)
    {
        EXIT_CRITICAL(cpu_sr);
        return EPDK_FAIL;
    }
    pDev->used = 0;
    EXIT_CRITICAL(cpu_sr);

    return EPDK_OK;
}

static __u32 DEV_UART_Read(void *pBuffer, __u32 nSize, __u32 nCount, __hdle hDev)
{
    __dev_uart_t *pDev = (__dev_uart_t *)hDev;
    __u32        ret;

    __inf("read from uart%d...\n", pDev->port);
    uart_lock(pDev->port);

    ret = BSP_UART_Read(pDev->hdle, nSize, nCount, pBuffer);

    uart_unlock(pDev->port);
    __inf("read from uart%d end\n", pDev->port);

    return ret;
}

static __u32 DEV_UART_Write(const void *pBuffer, __u32 nSize, __u32 nCount, __hdle hDev)
{
    __dev_uart_t *pDev = (__dev_uart_t *)hDev;
    __u32        ret;

    __inf("write to uart%d: %s\n", pDev->port, (const char *)pBuffer);
    uart_lock(pDev->port);

    ret = BSP_UART_Write(pDev->hdle, nSize, nCount, pBuffer);

    uart_unlock(pDev->port);
    __inf("write to uart%d end\n", pDev->port);

    return ret;
}

static __s32 DEV_UART_Ioctl(__hdle hDev, __u32 Cmd, __s32 Aux, void *pBuffer)
{
    __dev_uart_t *pDev = (__dev_uart_t *)hDev;
    __s32        ret;

    switch (Cmd)
    {
    case UART_CMD_SET_PARA:
    {
        __uart_para_t *pUartPara = (__uart_para_t *)pBuffer;

        ret = BSP_UART_SetPara(pDev->hdle, pUartPara);

        return ret;
    }

    case UART_CMD_SET_BAUDRATE:
    {
        __u32 baudrate = (__u32)pBuffer;

        // check the baudrate is valid
        if (baudrate)
        {
            pDev->baudrate = baudrate;
        }
        else
        {
            __wrn("invalid baudrate: of uart%d\n", baudrate);
            return EPDK_FAIL;
        }

        __msg("begin setting the baudrate of uart%d\n", pDev->port);
        uart_lock(pDev->port);
        ret = BSP_UART_SetBaudrate(pDev->hdle, baudrate);
        uart_unlock(pDev->port);
        __msg("end setting the baudrate of uart%d\n", pDev->port);

        return ret;
    }

    case UART_CMD_FLUSH:
    {
        __msg("begin flush the recv buffer of uart%d\n", pDev->port);
        uart_lock(pDev->port);
        ret = BSP_UART_FlushRecvBuffer(pDev->hdle);
        uart_unlock(pDev->port);
        __msg("end flush the recv buffer of uart%d\n", pDev->port);

        return ret;
    }

    default:
        break;
    }

    return EPDK_FAIL;
}

__dev_devop_t uart_devop =
{
    DEV_UART_Open,
    DEV_UART_Close,
    DEV_UART_Read,
    DEV_UART_Write,
    DEV_UART_Ioctl
};
