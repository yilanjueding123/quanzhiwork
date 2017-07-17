/*
*********************************************************************************************************
*                                                           eBase
*                                           the Abstract of Hardware
*
*                                   (c) Copyright 2006-2010, AW China
*                                                   All Rights Reserved
*
* File        :  bsp_lradc.h
* Date        :  2011-01-24
* Author      :  Victor
* Version     :  v1.00
* Description:
*                                   Operation for LRADC module,AW1619
* History     :
*      <author>          <time>             <version >          <desc>
*       Victor         2011-01-24              1.0           create this file
*
*********************************************************************************************************
*/
#ifndef _BSP_LRADC_H_
#define _BSP_LRADC_H_

/* set channel number */
typedef enum
{
    LRADC_ONE_CHANNEL   =  0x1, //default is ADC0
    LRADC_TWO_CHANNEL   =  0x2, //ADC0 and ADC1，warning: AW1619没有ADC1，只有ADC0
}
LRADC_CHANNEL_SELECT_SET;

/* set lradc module sample frequency */
typedef enum
{
    LRADC_SAMPLE_250HZ  = 0x1,  //4ms
    LRADC_SAMPLE_125HZ  = 0x2,  //8ms
    LRADC_SAMPLE_63HZ   = 0x3,  //16ms
    LRADC_SAMPLE_32HZ   = 0x4,  //32ms
}
LRADC_SAMPLE_SELECT_SET;

/* set lradc module level b voltage */
typedef enum
{
    LRADC_LEVELB_1_6V  =  0x1, //level b voltage
    LRADC_LEVELB_1_7V  =  0x2,
    LRADC_LEVELB_1_8V  =  0x3,
    LRADC_LEVELB_1_9V  =  0x4,
}
LRADC_LEVEL_B_SELECT_SET;

/*line control-return value for callback function */
typedef enum
{
    LRADC_LINE_INT_COMING = 0x0,
    LRADC_LINE_INT_GOING  = 0x1,
}
LRADC_LINE_NOTIFIER_SET;

/*
****************************************************************************************************
*
*             lradck_proc
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
typedef void (*lradc_proc)(__u32 key_value);

typedef struct tag_lradck_scancode_info
{

    __u32	keyMin;
    __u32	keyMax;
    __u32	keyCode; //keyMin-keyMax Scancode mapping to keyCode

} LRADC_KEY_SCANCODE;

typedef struct lradc_key_setting
{
    //irq variable，内部申请中断使用
    __u32 irq_no;
    __u32 irq_flags;
    __u32 irq_prio;

    //设置通道数，采样率，电压B的值
    __u32 channel;// 1:ADC0,2:ADC0 & ADC1
    __u32 sample;// 1,2,3,4
    __u32 voltageB;//1,2,3,4

    __u32  first_key_time;
    __u32  first_key_delay;
    __u32  repeat_key_time;

    //按键使用
    __u32 key_adc;//0:NOTUSED,1:ADC0,2:ADC1.
    lradc_proc key_down;//
    lradc_proc key_up;
    //设置键值映射表
    LRADC_KEY_SCANCODE *pKeycode;
    __u32 kc_size;//8个

    //线控使用
    __u32 line_adc;//0:NOTUSED,1:ADC0,2:ADC1.
    lradc_proc line_notifier;
} KEYC_SETTING;

/*
****************************************************************************************************
*
*  FunctionName:           eBSP_lradc_key_init
*
*  Description:
*               设置时钟，申请中断处理函数，键值映射表的初始化。
*
*
*  Parameters:
*       ioremap :  虚拟地址。
*       pKeyc   :  外部按键资源，包含中断申请号，寄存器配置：电压B，采样率，通道的选择。
*
*  Return value:
*            成功返回0，失败返回-1。
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_LRADCK_init(KEYC_SETTING *pKeyc);

/*
****************************************************************************************************
*
*  FunctionName:           eBSP_lradc_key_exit
*
*  Description:
*               停止时钟，取消注册中断处理函数，关闭lradc模块。
*
*
*  Parameters:
*
*       pKeyc   :  外部按键资源，包含中断申请号，寄存器配置：电压B，采样率，通道的选择。
*
*  Return value:
*            成功返回0，失败返回-1。
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_LRADCK_exit (KEYC_SETTING *pKeyc);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_LRADCK_set_firstKeyTime
*
*  Description:
*                   设置按键第一次按下时的响应时间，
*               在模块初始化时指定的采样频率*first_time为按键第一次按下的响应时间。
*
*  Parameters:
*       first_time   : 按键第一次按下时的响应时间。
*
*  Return value:
*            成功返回0，失败返回-1。
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_LRADCK_set_firstKeyTime(__u32 first_time);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_LRADCK_get_firstKeyTime
*
*  Description:
*                   获取按键第一次按下时的响应时间，
*
*  Parameters:
*       无。
*
*  Return value:
*           返回按键第一次按下时的响应时间。返回0表示错误，其他值正确。
*  Notes:
*
****************************************************************************************************
*/
__u32 BSP_LRADCK_get_firstKeyTime(void);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_LRADCK_set_firstKeyDelay
*
*  Description:
*                   设置第一次按键按下后发第二次按键消息的间隔时间。也是长按键的阈值时间。
*  Parameters:
*       无。
*
*  Return value:
*            成功返回0，失败返回-1。
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_LRADCK_set_firstKeyDelay(__u32 first_delay);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_LRADCK_get_firstKeyDelay
*
*  Description:
*                   获取第一次按键按下后发第二次按键消息的间隔时间。也是长按键的阈值时间。
*  Parameters:
*       无。
*
*  Return value:
*           返回第一次按键按下后发第二次按键消息的间隔时间。返回0表示错误，其他值正确。
*  Notes:
*
****************************************************************************************************
*/
__u32 BSP_LRADCK_get_firstKeyDelay(void);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_LRADCK_set_rpKeyTime
*
*  Description:
*                   设置按键按住不放时的按键发送消息频率时间。
*  Parameters:
*       无。
*
*  Return value:
*           成功返回0，失败返回-1。
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_LRADCK_set_rpKeyTime(__u32 repeat_time);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_LRADCK_get_rpKeyTime
*
*  Description:
*                   获取按键按住不放时的按键发送消息频率时间。
*  Parameters:
*       无。
*
*  Return value:
*           返回按键按住不放时的按键发送消息频率时间。返回0表示错误，其他值正确。
*  Notes:
*
****************************************************************************************************
*/
__u32 BSP_LRADCK_get_rpKeyTime(void);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_LRADCK_lineCtrl_irq
*
*  Description:
*               线控开启或关闭中断类型,线控一般需要开启的中断为first irq和last irq
*
*  Parameters:
*
*       irq_type   :  三种中断类型，first irq,data irq,keyup irq.
*       ctl: 0:disable；1:enable.
*
*  Return value:
*            成功返回0，失败返回-1。
*  Notes:
*           只有在初始化函数中开启线控功能此函数才能使用，否则返回-1.
****************************************************************************************************
*/
__s32 BSP_LRADCK_lineCtrl_irq(__u32 irq_type, __u32 ctl);


#endif
