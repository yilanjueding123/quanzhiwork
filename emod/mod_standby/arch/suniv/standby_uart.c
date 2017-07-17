/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                             	Standby system
*
*                                    (c) Copyright 2012-2016, Sunny China
*                                             All Rights Reserved
*
* File    : standby_uart.c
* By      : Sunny
* Version : v1.0
* Date    : 2012-2-22
* Descript: uart operations for standby.
* Update  : date                auther      ver     notes
*           2012-2-22 15:24:27	Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "standby_i.h"
#include "standby_uart.h"

/*
*********************************************************************************************************
*                                       STANDBY PUT CHAR
*
* Description: standby put char.
*
* Arguments  : none.
*
* Returns    : none;
*********************************************************************************************************
*/
void standby_putc(char c)
{
    __u32 reg_val;

    //check fifo not full
    do
    {
        reg_val = STANDBY_RREG(UART_USR_REG);

    }
    while((reg_val & TXFIFO_FULL) == 0);

    //put a charset
    STANDBY_WREG(UART_THR_REG, c);
}

/*
*********************************************************************************************************
*                                       STANDBY PUT STRING
*
* Description: standby put string.
*
* Arguments  : none.
*
* Returns    : none;
*********************************************************************************************************
*/
void standby_puts(const char *str)
{
    while(*str != '\0')
    {
        if(*str == '\n')
        {
            // if current character is '\n', insert and output '\r'
            standby_putc('\r');
        }
        standby_putc(*str++);
    }
}




