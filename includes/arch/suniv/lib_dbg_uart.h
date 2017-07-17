/*
********************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*
*                                     (c) Copyright 2008-2009, Kevin China
*                                             				All Rights Reserved
*
* File    : d:\winners\eBase\eDRV\Melis\Drv\uart\sun_20\lib_dbg_uart\lib_dbg_uart.h
* By      : Sam.Wu
* Version : V1.00
* Date    : 2010/11/9 9:40
* Description :
* Update   :  date      author      version     notes
********************************************************************************
*/
#ifndef _LIB_DBG_UART_H_
#define _LIB_DBG_UART_H_

#define PHY_ADDR_BASE_PIOC          0x01c20800
#define PHY_ADDR_BASE_CCM           0X01C20000

int UART_DbgInit(unsigned pioMemBase, unsigned ccmMemBase, void *gpio_set, int uart_port);

int UART_DbgExit(void);

/*********************************************************************
* Method	 :    		UART_DbgLock
* Description: Lock uart for source clock changed, before adjust the source clock,
            this method should be called to lock the uart port.
* Parameters :
	@void
* Returns    :   int
* Note       :
*********************************************************************/
int UART_DbgLock(void);

int UART_DbgSetBaudRate(unsigned dbgBaudRate);

/*********************************************************************
* Method	 :    		UART_DbgUnlock
* Description: change baudrate to @dbgBaudRate and unlock the uart port.
* Parameters :
	@unsigned dbgBaudRate : baud rate for debug.
* Returns    :   int
* Note       :
*********************************************************************/
int UART_DbgUnlock(void);

/*********************************************************************
* Method	 :    		LIB_DBG_UART_put_data
* Description: request the uart to transmit data
* Parameters :
	@const char * data
	@unsigned int dataLen
* Returns    :   data size successful transmitted
* Note       :
*********************************************************************/
int UART_DBGPutStr(const char *data);

/*********************************************************************
* Method	 :    		LIB_DBG_UART_get_data
* Description: receive data from the uart and copied to capacity
* Parameters :
	@char * container :
	@unsigned int capacity
* Returns    :   how many data in byte copied successful to the @container,
                  always not larger than capacity
* Note       :
*********************************************************************/
int UART_DBGGetStr( char *container, unsigned int capacity );


/*
*********************************************************************************************************
*                                   UART DEBUG PUT ARGUMENT
*
* Description: uart debug put argument.
*
* Arguments  : arg      argument for output.
*              format   format for output.
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void UART_DbgPutArg(unsigned arg, char format);


/*
*********************************************************************************************************
*                                   UART DEBUG GET CHAR
*
* Description: uart debug get char;
*
* Arguments  : none;
*
* Returns    : charactor got from uart;
*
* Note       :
*********************************************************************************************************
*/
char UART_DbgGetChar(void);


#endif //#ifndef _LIB_DBG_UART_H_


