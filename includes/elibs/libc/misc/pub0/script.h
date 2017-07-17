/*
**********************************************************************************************************************
*											        eGon
*						                     the Embedded System
*									       script parser sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : script.c
*
* By      : Jerry
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#ifndef  _SCRIPT_H_
#define  _SCRIPT_H_

//#define   DATA_TYPE_SINGLE_WORD  (1)
//#define   DATA_TYPE_STRING       (2)
//#define   DATA_TYPE_MULTI_WORD   (3)
//#define   DATA_TYPE_GPIO_WORD    (4)
//
//#define   SCRIPT_PARSER_OK       (0)
//#define   SCRIPT_PARSER_EMPTY_BUFFER   	(-1)
//#define   SCRIPT_PARSER_KEYNAME_NULL   	(-2)
//#define   SCRIPT_PARSER_DATA_VALUE_NULL	(-3)
//#define   SCRIPT_PARSER_KEY_NOT_FIND    (-4)
//
//typedef   struct
//{
//	char  gpio_name[32];
//	int port;
//	int port_num;
//	int mul_sel;
//	int pull;
//	int drv_level;
//	int data;
//}
//script_gpio_set_t;

typedef unsigned long H_PARSER;

extern  H_PARSER script_parser_init(char *script_buf, unsigned long size);
extern  int script_parser_exit(H_PARSER hparser);
extern  int script_parser_fetch(H_PARSER hparser, char *main_name, char *sub_name, int value[], int count);
extern  int script_parser_subkey_count(H_PARSER hparser, char *main_name);
extern  int script_parser_mainkey_count(H_PARSER hparser);
extern  int script_parser_mainkey_get_gpio_count(H_PARSER hparser, char *main_name);
extern  int script_parser_mainkey_get_gpio_cfg(H_PARSER hparser, char *main_name, void *gpio_cfg, int gpio_count);

#endif  //_SCRIPT_H_


