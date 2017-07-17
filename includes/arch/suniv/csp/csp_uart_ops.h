/*
*******************************************************************************
*           				eBSP
*                 the Abstract of Hardware
*
*
*              (c) Copyright 2006-2010, ALL WINNER TECH.
*           								All Rights Reserved
*
* File     :  d:\winners\eBase\eBSP\CSP\sun_18\SW_UART\sw_uart.h
* Date     :  2010/9/24 10:44
* By       :  Sam.Wu
* Version  :  V1.00
* Description :  csp uart is working at POLL mode, if it cannot send/get data , it just wait
* Update   :  date      author      version     notes
*******************************************************************************
*/
#ifndef _CSP_UART_OPS_H_
#define _CSP_UART_OPS_H_


/*********************************************************************
* Method	 :    		CSP_UART_init
* Description: init this CSP UART module. must called at first when use this csp uart!!
* Parameters :
* Returns    :   EBASE_TRUE/EBASE_FALSE
* Note       :1) the csp uart have no right to request PIN and clock,
                 You must make sure the pins and clock have been at work for the uart module!
              2)the user/application of this csp uart controller must
                read the chip configure script to get how many uart ports it
                must support and which port number used as debug
*********************************************************************/
s32 CSP_UART_init(void);

/*********************************************************************
* Method	 :    		CSP_UART_exit
* Description: called when need to uninstall this csp uart.
* Parameters :
* Returns    :   s32
* Note       :
*********************************************************************/
s32 CSP_UART_exit(void);

/*********************************************************************
* Method	 :    		CSP_UART_reuqest_port
* Description:  Get a handle represents the hardware uart port.
* Parameters :
	@u32 portNo : the port number read out from the chip configure script
* Returns    :   a handle
* Note       :Important! the user/application of this csp uart must read the chip configure
                script to see which  port number corresponding needed hardware uart port.
                for example, in F18, the debug uart port number is 0, so you call
                uart_get_port(0) if you want to request the debug uart.
*********************************************************************/
__hdle CSP_UART_reuqest_port(u32 portNo);

/*********************************************************************
* Method	 :    		CSP_UART_release_port.
* Description: release the uart port.
* Parameters :
	@__hdle huart :the handle returned by uart_reuqest_port
* Returns    :   EBSP_TRUE/EBSP_FALSE
* Note       :
*********************************************************************/
s32 CSP_UART_release_port(__hdle huart);

/*********************************************************************
* Method	 :    		CSP_UART_port_config
* Description: set/change the port parameters.
* Parameters :
	@__hdle hUart :the handle returned by uart_reuqest_port
	@u32 uartPortClk : clock for the uart port. usually is APB clock.
	@const CSP_UART_PortParas_t * pPortParas : see type CSP_UART_PortParas_t
* Returns    :  EBSP_TRUE/EBSP_FALSE
* Note       :the usual configure :
        frame format consists of 1 start bit,8 data bits,no parity check and 1 stop bit;
        the baud rate  is 115200 which usually for debug
*********************************************************************/
s32 CSP_UART_port_config(__hdle hUart,
                         u32    uartPortClk,
                         const CSP_UART_PortParas_t *pPortParas);

/*********************************************************************
* Method	 :    		CSP_UART_get_rx_data
* Description: get data in the hardware Receive FIFO
* Parameters :
    @__hdle hUart   : the handle returned by uart_reuqest_port
	@char * buffer : the buffer address
	@u32 bufLen    : the buffer length
* Returns    :   the real gotten data length in byte
* Note       :
*********************************************************************/
s32 CSP_UART_get_rx_data(__hdle hUart, char *buffer, u32 bufLen);

/*********************************************************************+
* Method	 :    		CSP_UART_xmit_data
* Description: write data to the UART Transmitter.
* Parameters :
	@__hdle hUart : the handle returned by uart_reuqest_port
    @char * data : address of data want to write to FIFO
	@u32 dataLen : data length of data want to write to FIFO
* Returns    :   EBSP_TRUE/EBSP_FALSE
* Note       :
*********************************************************************/
s32 CSP_UART_xmit_data(__hdle hUart, const char *data, u32 dataLen);

/*********************************************************************
* Method	 :    		CSP_UART_get_char
* Description: Get a char in the uart receiver.
* Parameters :
	@__hdle hUart : the handle returned by uart_reuqest_port
* Returns    :   a char received.
* Note       : Port receiver will not hold the character returned after you call this method.
*********************************************************************/
char CSP_UART_get_char(__hdle hUart);

/*********************************************************************
* Method	 :    		CSP_UART_lock_port
* Description: lock the port to disable transmit.
* Parameters :
	@__hdle hUart : the handle returned by uart_reuqest_port
* Returns    :   EBSP_TRUE/EBSP_FALSE
* Note       :
*********************************************************************/
s32 CSP_UART_lock_port(__hdle hUart);

/*********************************************************************
* Method	 :    		CSP_UART_unlock_port
* Description: unlock the port to locked.
* Parameters :
	@__hdle hUart : the handle returned by uart_reuqest_port
* Returns    :   EBSP_TRUE/EBSP_FALSE
* Note       :
*********************************************************************/
s32 CSP_UART_unlock_port(__hdle hUart);

#endif //#ifndef _CSP_UART_OPS_H_
