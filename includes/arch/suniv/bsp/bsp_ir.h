/*
*********************************************************************************************************
*                                                           eBase
*                                           the Abstract of Hardware
*
*                                   (c) Copyright 2006-2010, holigun China
*                                                   All Rights Reserved
*
* File        :  bsp_ir.h
* Date        :  2011-02-15
* Author      :  Victor
* Version     :  v1.00
* Description:
*                         Operation for IR module,AW1619
*
* History     :
*      <author>          <time>             <version >          <desc>
*       Victor         2011-02-15              1.0           create this file
*********************************************************************************************************
*/
#ifndef _BSP_IR_H_
#define _BSP_IR_H_

/*
****************************************************************************************************
*
*             ir_proc
*
*  Description:
*       发送按键抬起消息函数接口
*		功能描述：发送按键抬起消息，借助各个os的按键发送消息函数完成功能，
*		比如melis下的DEV_KEY_Ioctrl，wince的kbd_event，linux的input_event，input_syn等函数。
*
*  Parameters:
*		key_value	:	按键值，即哪一个按键抬起了。
*
*  Return value:
*		         无
*  Notes:
*
****************************************************************************************************
*/
typedef void (*ir_proc)(u32 key_value);


/* 红外按键选择，暂时只支持NEC格式  */
typedef enum
{
    NEC_REMOTE_CONTROLLER = 1,
    RC5_REMOTE_CONTROLLER    ,
    RC6_REMOTE_CONTROLLER    ,
    RC7_REMOTE_CONTROLLER    ,

} IR_REMOTE_CONTROLLER;


typedef struct tag_ir_key_setting
{
    //irq variable，内部申请中断使用
    __u32 irq_no;
    __u32 irq_flags;
    __u32 irq_prio;

    __u32 remote_controller;// 1:NEC 2: RC5 3: RC6; 4:RC7 ;default is NEC，暂时支持NEC解码。
    __u32 user_code;//user code or address code,0:不检查usercode或addr code，其他值须检查
    __u32 rp_keytime;//连续按下的按键发送消息频率；单位ms，根据不同平台的timer粒度不同有所区别
    __u32 keyup_times;//按键释放时间为 rp_keytime*keyup_cnt
    __u32 holdkey;

    //按键使用例程，向上层发送按键消息
    ir_proc key_down;
    ir_proc key_up;

    //设置键值映射表,在driver层做,bsp层只将扫描值给上层。

} __ir_key_setting_t;


/*
****************************************************************************************************
*
*  FunctionName:           BSP_IR_key_init
*
*  Description:
*               设置时钟，申请中断处理函数，没有做红外按键表的映射，直接在driver层做，这样更加灵活。
*           申请一个timer做按键抬起检测和连按键处理。
*
*  Parameters:
*       ioremap :  虚拟地址。
*       pIrKey   :  外部按键资源，包含中断申请号,
*
*  Return value:
*            成功返回EBSP_TRUE，失败返回EBSP_FALSE。
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_IR_key_init(__ir_key_setting_t *pIrKey);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_IR_key_exit
*
*  Description:
*               停止时钟，释放GPIO ,取消注册中断处理函数，关闭ir模块。
*
*  Parameters:
*
*       pIrKey   :  外部红外按键资源，包含中断申请号等。
*
*  Return value:
*            成功返回EBSP_TRUE，失败返回EBSP_FALSE。
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_IR_key_exit (__ir_key_setting_t *pIrKey);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_IR_key_set_remoter_usercode
*
*  Description:
*              设置遥控器的用户码或者地址码，如果为0则表示不检测用户码或地址码。用户码或地址码
*           用来区分不同的遥控器。获取地址码或用户码，直接调用BSP_IR_key_display_scancode打印出来即可以。
*  Parameters:
*               user_code: 0表示不检查用户码或地址码，其他值表示需要检查用户码
*  Return value:
*            成功返回0，失败返回-1.
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_IR_key_set_remoter_usercode(__u32 user_code);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_IR_key_display_scancode
*
*  Description:
*              显示扫描码，如果为1则控制台打印显示得到的扫描码，如果为0则关闭打印显示。
*           主要方便用户映射扫描码和按键消息。扫描码和用户码一起打印出来的。
*  Parameters:
*               on_off: 1为打印扫描码，0为关闭打印扫描码
*  Return value:
*            成功返回0，失败返回-1.
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_IR_key_display_scancode(__u32 on_off);
/*
****************************************************************************************************
*
*  FunctionName:           BSP_IR_key_get_rp_keytime
*
*  Description:
*              获取红外按键抬起的检测时间，
*             (定时器的周期)*(操作系统的定时器的刻度)为实际的按键连续按下时发出按键消息的频率。
*  Parameters:
*               无。
*  Return value:
*            成功返回按键连续按下时发出按键消息的频率。
*  Notes:
*
****************************************************************************************************
*/
__u32 BSP_IR_key_get_rp_keytime(void);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_IR_key_set_rp_keytime
*
*  Description:
*               设置按键连续按下时的发送按键消息频率。注意，不是简单的设置，
*           而要把之间申请的timer删除后，再重新创建一个timer才能设置这个时间。
*
*  Parameters:
*
*       keyup_times   :  红外按键抬起的检测时间,
*
*  Return value:
*            成功返回EBSP_TRUE，失败返回EBSP_FALSE。
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_IR_key_set_rp_keytime(__u32 rp_keytime);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_IR_key_get_keyup_times
*
*  Description:
*              获取红外按键抬起的检测时间。如果为N则表示
*             则N*(定时器的周期)*(操作系统的定时器的刻度)为实际的按键抬起检测。
*  Parameters:
*               无。
*  Return value:
*            成功返回红外按键抬起的检测时间。
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_IR_key_get_keyup_times(void);
/*
****************************************************************************************************
*
*  FunctionName:           BSP_IR_key_set_keyup_times
*
*  Description:
*               设置红外按键抬起的检测时间。如果为N则表示
*             则N*(定时器的周期)*(操作系统的定时器的刻度)为实际的按键抬起检测。
*  Parameters:
*
*       keyup_times   :  红外按键抬起的检测时间,
*
*  Return value:
*            成功返回EBSP_TRUE，失败返回EBSP_FALSE。
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_IR_key_set_keyup_times(__s32 keyup_times);

#endif
