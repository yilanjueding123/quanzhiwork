/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                              Read one command
*
* File    : Esh_read_command.h
* By      : sunny
* Version : V1.00                                                           2009.3.2
*********************************************************************************************************
*/

#ifndef     _READ_COMMAND_H_
#define     _READ_COMMAND_H_

#include    "Esh_shell.h"

#define     Esh_MAX_HISTORY_LEN     30  /* max history length */
#define     CTL_CH(c)		        ((c) - 'a' + 1)

/* command history struct */
typedef  struct CMD_HISTORY
{
    char   cmd[ESH_MAX_CMD_LEN];
    struct CMD_HISTORY *next;
    struct CMD_HISTORY *prior;
} cmd_history;

extern     cmd_history Esh_CMD_Table[Esh_MAX_HISTORY_LEN];

extern     void Esh_ReaderLoop(void);

#endif     /* _READ_COMMAND_H_ */
