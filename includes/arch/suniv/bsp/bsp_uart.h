/*
****************************************************************************************************
*                                               MELIS
*                               the Easy Portable/Player Develop Kits
*                                           UART Module
*
*                           (c) Copyright 2011-2014, All winners Co,Ld.
*                                       All Rights Reserved
*
* File    : bsp_uart.h
* By      : james.deng
* Version : 1.0.0
* Date    : 2011-12-19
* Descript: the header of uart module.
* Update  : <date>          <author>            <version>           <notes>
*           2011-12-19      james.deng          1.0.0               build the file.
****************************************************************************************************
*/

#ifndef __BSP_UART_H__
#define __BSP_UART_H__


#define UART_PORT_COUNT                 3

#define UART_DMA_MODE                   0           //add by libaiao

// The Parameter of uart
typedef struct __UART_PARA
{
    __u32 nEvenParity;                  // Even parity select
    __u32 nParityEnable;                // Parity enable
    __u32 nStopBit;                     // Number of stop bits
    __u32 nDataLen;                     // Data length select
} __uart_para_t;

// Uart FIFO type
typedef enum __UART_FIFO_TYPE
{
    UART_FIFO_TYPE_RX,
    UART_FIFO_TYPE_TX
} __uart_fifo_type_t;

/*
****************************************************************************************************
*                                       BSP_UART_Init
*
* Description: Init uart module.
*
* Arguments  :
*
* Return     : EPDK_OK
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_UART_Init(void);

/*
****************************************************************************************************
*                                       BSP_UART_Exit
*
* Description: Exit uart module.
*
* Arguments  :
*
* Return     : EPDK_OK
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_UART_Exit(void);

/*
****************************************************************************************************
*                                       BSP_UART_Request
*
* Description: Request a uart port.
*
* Arguments  : nPort        : the port.
*              nBaudrate    : the baudrate of port.
*              pUartPara    : the parameter of uart.
*              nBufferSize  : the size of uart sotf buffer.
*
* Return     : __hdle       : the handle of uart, NULL indicate fail to request a uart port.
*
* Notes      :
*
****************************************************************************************************
*/
__hdle BSP_UART_Request(__u32 nPort, __u32 nBaudrate, const __uart_para_t *pUartPara, __u32 nBufferSize);

/*
****************************************************************************************************
*                                       BSP_UART_Release
*
* Description: Release a uart port.
*
* Arguments  : hUart        : the handle of uart.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_UART_Release(__hdle hUart);

/*
****************************************************************************************************
*                                       BSP_UART_SetPara
*
* Description: Set the parameter of uart port.
*
* Arguments  : hUart        : the handle of uart.
*              pUartPara    : the parameter of uart.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_UART_SetPara(__hdle hUart, const __uart_para_t *pUartPara);

/*
****************************************************************************************************
*                                       BSP_UART_SetBaudrate
*
* Description: Set the baudrate of uart port.
*
* Arguments  : hUart        : the handle of uart.
*              nBaudRate    : the baudrate of uart.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_UART_SetBaudrate(__hdle hUart, __u32 nBaudrate);

/*
****************************************************************************************************
*                                       BSP_UART_Read
*
* Description: Read data from a uart port.
*
* Arguments  : hUart        : the handle of uart.
*              nSize        : item size in bytes.
*              nCount       : Maximum number of items to be read.
*              pBuffer      : Storage location for data.
*
* Return     : __u32        : the number of full items actually read, which may be less than count
*                             if buffer hasn't so much data.
*
* Notes      :
*
****************************************************************************************************
*/
__u32 BSP_UART_Read(__hdle hUart, __u32 nSize, __u32 nCount, __u8 *pBuffer);

/*
****************************************************************************************************
*                                       BSP_UART_Write
*
* Description: Write data to a uart port.
*
* Arguments  : hUart        : the handle of uart.
*              nSize        : item size in bytes.
*              nCount       : Maximum number of items to be write.
*              pBuffer      : Storage location for data.
*
* Return     : __u32        : the the number of full items actually written.
*
* Notes      :
*
****************************************************************************************************
*/
__u32 BSP_UART_Write(__hdle hUart, __u32 nSize, __u32 nCount, const __u8 *pBuffer);

/*
****************************************************************************************************
*                                       BSP_UART_ClearFIFO
*
* Description: Clear the uart port fifo.
*
* Arguments  : hUart        : the handle of uart.
*              type         : UART_FIFO_TYPE_RX or UART_FIFO_TYPE_TX.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_UART_ClearFIFO(__hdle hUart, __uart_fifo_type_t type);

/*
****************************************************************************************************
*                                       BSP_UART_EnableINT
*
* Description: Enable a uart port interrupt.
*
* Arguments  : hUart        : the handle of uart.
*              nIrqSrc      : the interrupt source of uart.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_UART_EnableINT(__hdle hUart, __u32 nIrqSrc);

/*
****************************************************************************************************
*                                       BSP_UART_DisableINT
*
* Description: Disable a uart port interrupt.
*
* Arguments  : hUart        : the handle of uart.
*              nIrqSrc      : the interrupt source of uart.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_UART_DisableINT(__hdle hUart, __u32 nIrqSrc);

/*
****************************************************************************************************
*                                       BSP_UART_QueryINT
*
* Description: Query a uart port interrupt state.
*
* Arguments  : hUart        : the handle of uart.
*              pStatus      : the status of interrupt. bit[31:24] main interrupt number. bit[4:0] interrupt source.
*
* Return     :
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_UART_QueryINT(__hdle hUart, __u32 *pStatus);

/*
****************************************************************************************************
*                                       BSP_UART_CheckStatus
*
* Description: Check uart port status.
*
* Arguments  : hUart        : the handle of uart.
*              pStatus      : the status of uart.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_UART_CheckStatus(__hdle hUart, __u32 *pStatus);

/*
****************************************************************************************************
*                                       BSP_UART_Lock
*
* Description: Lock the uart port.
*
* Arguments  : hUart        : the handle of uart.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_UART_Lock(__hdle hUart);

/*
****************************************************************************************************
*                                       BSP_UART_Unlock
*
* Description: Unlock the uart port.
*
* Arguments  : hUart        : the handle of uart.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_UART_Unlock(__hdle hUart);

/*
****************************************************************************************************
*                                       BSP_UART_DumpReg
*
* Description: Dump the uart register.
*
* Arguments  : hUart        : the handle of uart.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_UART_DumpReg(__hdle hUart, __u32 nLineNo, const char *strFileName);

/*
****************************************************************************************************
*                                       BSP_UART_FlushRecvBuffer
*
* Description: Only flush recv buffer.
*
* Arguments  : hUart        : the handle of uart.
*
* Return     : EPDK_OK
*              EPDK_FAIL
*
* Notes      :
*
****************************************************************************************************
*/
__s32 BSP_UART_FlushRecvBuffer(__hdle hUart);

#endif // __BSP_UART_H__
