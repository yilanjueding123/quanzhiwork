/*
*******************************************************************************
*           				eBase
*                 the Abstract of Hardware
*
*
*              (c) Copyright 2006-2010, ALL WINNER TECH.
*           								All Rights Reserved
*
* File     :  pin.h
* Date     :  2011/2/28 10:07
* By       :
* Version  :  V1.00
* Description :
* Update   :  date      author      version
*
* notes    :
*******************************************************************************/
#ifndef _PIN_OPS_H_
#define _PIN_OPS_H_


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
//typedef struct
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
//
//
//
//
//typedef struct
//{
//    char  gpio_name[32];
//    int port;
//    int port_num;
//    int mul_sel;
//    int pull;
//    int drv_level;
//    int data;
//}
//user_gpio_set_t;


__s32 CSP_GPIO_init(void) /*GPIO初始化*/;
__s32 CSP_GPIO_exit(void) /*GPIO推出函数接口 */;
__hdle CSP_GPIO_Request(user_gpio_set_t *gpio_list, __u32 group_count_max) /*设备申请GPIO函数接口 */;
__hdle CSP_GPIO_Request(user_gpio_set_t *gpio_list, __u32 group_count_max) /*设备申请GPIO函数接口 */;
__s32 CSP_GPIO_Release(__hdle p_handler, __s32 if_release_to_default_status);
__s32 CSP_GPIO_Get_All_PIN_Status(__hdle p_handler, user_gpio_set_t *gpio_status, __u32 gpio_count_max, __u32 if_get_from_hardware);
__s32 CSP_GPIO_Get_One_PIN_Status(__hdle p_handler, user_gpio_set_t *gpio_status, const char *gpio_name, __u32 if_get_from_hardware);
__s32 CSP_GPIO_Set_One_PIN_Status(__hdle p_handler, user_gpio_set_t *gpio_status, const char *gpio_name, __u32 if_set_to_current_input_status);
__s32 CSP_GPIO_Set_One_PIN_IO_Status(__hdle p_handler, __u32 if_set_to_output_status, const char *gpio_name);
__s32 CSP_GPIO_Set_One_PIN_Pull(__hdle p_handler, __u32 set_pull_status, const char *gpio_name);
__s32 CSP_GPIO_Set_One_PIN_driver_level(__hdle p_handler, __u32 set_driver_level, const char *gpio_name);
__s32 CSP_GPIO_Read_One_PIN_Value(__hdle p_handler, const char *gpio_name);
__s32 CSP_GPIO_Write_One_PIN_Value(__hdle p_handler, __u32 value_to_gpio, const char *gpio_name);

//new for INT support
__s32 CSP_GPIO_GetIrqNum(void);
__s32 CSP_GPIO_SetIntMode(__hdle p_handler, __u32 int_mode);
__s32 CSP_GPIO_QueryInt(__hdle p_handler);
__s32 CSP_GPIO_ClearPending(__hdle p_handler);
__s32 CSP_GPIO_EnableInt(__hdle p_handler);
__s32 CSP_GPIO_DisableInt(__hdle p_handler);
__s32 CSP_GPIO_SetIntClock(__u32 clock);
__s32 CSP_GPIO_SetIntDebounce(__u32 n);

#endif //#ifndef _PIN_OPS_H_

