/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eBIOS Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : power bios module
* File    : drv_power.h
*
* By      : Jerry
* Version : v1.00
* Date    : 2008-8-19 18:38:16
**********************************************************************************************************************
*/
#ifndef _DRV_POWER_H_
#define _DRV_POWER_H_

#define POWER_BOARD1_DEV_ADDR_19    (0x68>>1)
#define POWER_BOARD1_DEV_ADDR_18    (0x2C>>1)

#define DRV_POWER_BAT_CAP           2200
#define POWER_RDC_COUNT             10
#define DISCHARGE_CURRENT_RATE      35          //放电时补偿外部并联的MOSFET
#define EXT_PMOS                    115         //如果无外部PMOSFET，此处定义为100
#define CHG_RDC_RATE                100

#define MUSIC_CURRENT_LCD_OFF       50          //实际测试关屏后的音乐播放电流，20100222 Add


typedef struct __DRV_POWER_PARA_T
{
    __u8    byte_addr;
    __u8    reserved1;
    __u16   reserved2;
    __u8    *data;

} __drv_power_para_t;


/*
    电压读取
    电压设置
            aux ＝ 电压通道号
            需要设置的电压使用毫伏单位，使用(void *)V，V就是需要设置的电压
            读取的电压数值直接返回，成功返回电压数值，失败返回-1
    电压状态
            aux == 电压通道号
            使用一个32位传递状态信息，只有开关信息的直接用最低位的0和1表示
                                      包括模式信息的，如DCDC1，用第0位表示开和关，
                                      第一位表示   0：自动模式；1：由bit2控制
                                      第2位表示    0：PWM模式； 1：PFM模式
                                      没有开关信息的第一位不关心，其它位意义不变
*/

/*
    功率器件状态读取
    功率器件状态设置
            aux = 功率器件号
            设置参数传递使用(void *)para的方式，para是需要传递的参数
            读取的时候使用返回值，如果失败返回-1
            参数第0和1位表示开和关       0：关闭；   1：打开
            参数第2和3位表示N还是P       0：N型开关；1：P型开关
            设置的时候如果对应bit传递2，则表示该位不关心
*/

/*
****************************************************************************************
*                                   关于电池状态
*
*   和状态1有关的命令有 DRV_POWER_CMD_BATADC_GET, DRV_POWER_CMD_BATADC_SET
*
*   这个结构体用于设置电池检测的相关状态，以及获取当前电池的电压和电流
*   使用的时候建议先使用命令 DRV_POWER_CMD_BATADC_SET
*再使用DRV_POWER_CMD_BATADC_GET，以免无效数据造成干扰
*****************************************************************************************
*/
typedef struct __DRV_POWER_BATTERY_STATUS1
{
    __u16   battery_exist       : 2;    //电池存在状态       0:不存在,    1：电池存在
    __u16   bat_voladc_enable   : 2;    //电池电压检测使能   0:禁止检测,  1：允许检测
    __u16   bat_crntadc_enable  : 2;    //电池电流检测使能   0:禁止检测,  1：允许检测
    __u16   bat_status          : 2;    //电池当前工作状态   0:充电,      1：放电
    __u16   reserved            : 8;
    __u16   adc_freq;                   //电池检测频率
    __u16   bat_vol;                    //当前电池电压，单位mV
    __u16   bat_crnt;                   //当前电池电流，单位mA

} __drv_power_battery_status_t1;


typedef struct __DRV_POWER_BATTERY_STATUS2
{
    __u8    dcin_valid;
    __u8    charge_status;
    __u8    battery_exist;
    __u8    battery_status;             //0: 充电  1：放电
    __u16   rest_vol;                   //剩余电池电量，百分比
    __u16   rest_time;                  //剩余电池时间或者剩余充电时间
    __u16   bat_vol;
    __u16   bat_crnt;
    __u16   bat_cap;
    __u16   bat_rdc;
    __u8    dcin_not_enough;            //表示 DCIN 负载能力不足
    __u16   bat_vol_correct;

} __drv_power_battery_status_t2;


typedef struct _DRV_POWER_CHARGE_STATUS
{
    __u8    acin_valid;
    __u8    vbus_valid;
    __u8    ext_power_valid;
    __u8    bat_current_direction;      //电池电流方向,0-放电,1-充电
    __u8    in_Short;                   //1-short
    __u8    int_over_temp;
    __u8    charge_indication;
    __u8    battery_exist;
    __u8    battery_active;
    __u8    low_charge_current;
    __u16   battery_current;
    __u16   battery_vol;
    __u16   acin_vol;
    __u16   vbus_vol;
    __u8    dcin_not_enough;
    __u16   Ichgset;
    __u16   Iendchg;

} __drv_power_charge_stat;


/*
****************************************************************************************
*                                   关于电池状态
*
*   和状态1有关的命令有 DRV_POWER_CMD_GET_DCIN
*   只能获取状态，不能设置
*
*****************************************************************************************
*/
typedef struct __DRV_POWER_BATTERY_STATUS3
{
    __u8    dcin_valid;                 //0:不可用,    1：可用
    __u8    charge_status;              //0:没有充电,  1：充电
    __u8    acin_en;                    //0:没有,      1:有

} __drv_power_battery_status_t3;


typedef struct __DRV_POWER_FUELGUAGE    //对应POWER命令：DRV_POWER_CMD_GET_FUELGUAGE
{
    __u32   charge_status;              //当前状态，bit2:电池是否存在，  bit1:dcin存在，  bit0: 充电状态，1为充电，0为没有充电
    //其它状态，意义保留
    __u32   rest_vol;                   //剩余电量，百分比
    __u32   rest_time;                  //剩余时间，分钟数
    __u32   bat_cap;
    __u32   bat_power;
    __u32   rest_time_music;
    __u32   rest_time_video;
    __u32   rest_time_pic;              //剩余时间，分钟数
    __u32   rest_time_book;             //剩余时间，分钟数
    __u32   rest_time_standby;          //剩余时间，分钟数
    __s32   is_low_power;               //电池是否低电
    __s32   is_full_charge;             //电池是否充满

} __drv_power_fuelguage_t;

typedef struct __DRV_POWER_DCIN_STATUS
{
    __u16   dcin_exist          : 2;
    __u16   dcin_voladc_enable  : 2;
    __u16   dcin_crntadc_enable : 2;
    __u16   reserved            : 10;
    __u16   adc_freq;
    __u16   dcin_vol;
    __u16   dcin_crnt;
} __drv_power_dcin_status_t;

/*
*********************************************************************************************
*                                   电压设置相关
*   和充电相关的命令有两个，DRV_POWER_CMD_VOUT_GET, DRV_POWER_CMD_VOUT_SET
*
*   一个标准的命令格式是  eLIBs_fioctrl(hdle, DRV_POWER_CMD_VOUT_GET, aux, pbuffer);
*   hdle是句柄
*   aux是参数，指明电源名称,都是由宏定义完成
*   pbuffer是一个指针,通常用于传递一个数据。
*   对于DRV_POWER_CMD_VOUT_GET命令，使用返回值表示电压，mV单位，如果失败将返回-1，使用的时候需要注意
*
*   对于DRV_POWER_CMD_VOUT_SET
*   命令格式  eLIBs_fioctrl(hdle, DRV_POWER_CMD_VOUT_SET, aux, pbuffer);
*   hdle是句柄
*   aux是参数，指明电源名称，都由宏定义完成
*   pbuffer是一个指针，通常用于传递一个数据
*   对于DRV_POWER_CMD_VOUT_SET命令，使用pbuffer指针传进一个电压数据，mV单位
*   设置成功返回0，设置失败（如：电压不可达，电源名称不存在）返回-1
*
**********************************************************************************************
*/

/*
*********************************************************************************************
*                                   充电相关设置
*   和电压设置相关的命令有两个，DRV_POWER_CMD_CHARGE_GET, DRV_POWER_CMD_CHARGE_SET
*   如果要进行设置，通常先进行读取，避免某些参数的初始值对设置造成影响
*
*   可以设置的有如下结构体的内容
**********************************************************************************************
*/
typedef struct __DRV_POWER_CHARGE_STATUS
{
    __u8    battery_status;                   //00: 电池不存在
    //01：电池存在，允许充电，正在充电
    //02：电池存在，允许充电，不在充电
    //03：电池存在，禁止充电
    __u8    charge_end;                       //充电目标设置， 0：10％结束   1：15％结束
    __u8    charge_autolimit;                 //充电自动限流   0: 禁止       1: 使能
    __u8    charge_enable;                    //充电使能       0: 禁止充电   1：使能充电
    __u16   charge_vtarget;                   //充电目标电压, mV单位
    __u16   charge_current;                   //充电电流设置, mA单位
    __u16   charge_timer1;                    //充电计时器1设置
    __u16   charge_timer2;                    //充电计时器2设置

} __drv_power_charge_status_t;


/*
*********************************************************************************************
*                                   电路基本状态读取
*   和电压设置相关的命令只有一个，DRV_POWER_CMD_BASIC_STATUS
*
*   可以获取的有如下结构体的内容
**********************************************************************************************
*/
typedef struct __DRV_POWER_BASIC_STATUS
{
    __u32   acin_input          : 1;    //ACINEN状态            0：没有AC adapter输入；1：有AC adapter输入

    __u32   charge_source       : 2;    //00:没有充电  01：圆口火牛充电  10：USB线充电  11：USB火牛充电
    __u32   vout_exist          : 1;    //指示外部电源是否存在  0：外部电源没有接入；  1：外部电源存在
    __u32   vout_enable         : 1;    //指示外部电源是否可用  0：外部电源不可用；    1：外部电源可用
    __u32   vout_choose         : 1;    //DCIN可用时是否选择DCIN作为系统电源 0：不选择；1：选择

    __u32   bat_exist           : 1;    //电池是否存在          0：没有电池连接到AXP180；1：有电池连接到AXP180
    __u32   bat_work_status     : 1;    //电池电流方向指示：    0：电池充电；1：电池供电
    __u32   bat_charge_status   : 1;    //充电状态指示          0：在充电；1：不在充电

} __drv_power_basic_status_t;


/*
****************************************************************************************
*                                   关于控制器状态设置以及获取
*
*   和控制器状态有关的命令有 DRV_POWER_CMD_IPS_GET, DRV_POWER_CMD_IPS_SET
*
*   这个结构体用于设置电池检测的相关状态，以及获取当前电池的电压和电流
*   使用的时候建议先使用命令 DRV_POWER_CMD_IPS_SET
*再使用DRV_POWER_CMD_IPS_GET，以免无效数据造成干扰
*****************************************************************************************
*/
typedef struct __DRV_POWER_IPS_STATUS
{
    __u16   dcin_choose     : 2;        //DCIN可用时是否选择DCIN作为系统电源, 0：不选择；1：选择
    __u16   vhold_mode      : 2;        //限压模式控制位                      0：不限压；1：限压
    __u16   reserved1       : 4;        //
    __u16   vhold;                      //设置VHold电压
    __u16   vwarning;                   //设置VWarning电压
    __u16   voff;                       //设置VOff电压           这3个电压的单位都是mV

} __drv_power_ips_status_t;


/*
****************************************************************************************
*                                   关于开关机时间设置以及获取
*
*   和控制器状态有关的命令有 DRV_POWER_CMD_ONOFFTIME_GET, DRV_POWER_CMD_ONOFFTIME_SET
*
*   这个结构体用于设置开机的时间，关机的时间
*
*   开机时间：断电情况下，按下按键，超过一定时间后开机，这个时间就是开机时间
*   关机时间：通电情况下，按下按键，超过一定时间后将触发中断关机，这个时间就是关机时间
*
*****************************************************************************************
*/
typedef struct  __DRV_POWER_ONOFFTIME
{
    __u16   on_time;                //开机时间，128ms， 512ms， 1000ms， 2000ms
    __s16   on_status;              //开机设置状态，如果时间设置成功，将被写0，否则-1
    __u16   off_time;               //关机时间，1000ms, 1500ms, 2000ms,  2500ms
    __s16   off_status;             //关机设置状态，如果时间设置成功，将被写0，否则-1

} __drv_power_onofftime_t;


#define POWER_EPT_1                 0x1f00
#define POWER_EPT_2                 0x1f01
#define POWER_EPT_3                 0x1f02
#define POWER_EPT_4                 0x1f03
#define POWER_EPT_5                 0x1f04
#define POWER_EPT_6                 0x1f05
#define POWER_EPT_7                 0x1f06
#define POWER_EPT_8                 0x1f07

#define POWER_EPT_MOS_N             (0x00 << 2)
#define POWER_EPT_MOS_P             (0x01 << 2)
#define POWER_EPT_MOS_NOTCARE       (0x02 << 2)

#define POWER_EPT_GATE_OFF          (0x00 << 0)
#define POWER_EPT_GATE_ON           (0x01 << 0)
#define POWER_EPT_GATE_NOTCARE      (0x02 << 0)

#define POWER_VOL_DCDC1             0x0f00
#define POWER_VOL_DCDC2             0x0f01
#define POWER_VOL_DCDC3             0x0f02
#define POWER_VOL_LDO1              0x0f03
#define POWER_VOL_LDO2              0x0f04
#define POWER_VOL_LDO3              0x0f05
#define POWER_VOL_LDO4              0x0f06
#define POWER_VOL_GPIO0             0x0f0a
#define POWER_VOL_EXTEN             0x0f0b

#define POWER_VOL_LDO5              0x0f07
#define POWER_VOL_SW1               0x0f08
#define POWER_VOL_SW2               0x0f09

#define POWER_VOL_ON                0x01
#define POWER_VOL_OFF               0x00

#define POWER_INT_VOUT_OP           (1<<7)      //IRQ1， 外部电源超压，向此位写1或DCIN电压恢复到正常范围会清除此位
#define POWER_INT_VOUT_PLUGIN       (1<<6)      //IRQ2， 外部电源插入，向此位写1或DCIN由高到低消失会清除此位
#define POWER_INT_VOUT_PLUGOUT      (1<<5)      //IRQ3， 外部电源移除，向此位写1或DCIN由低到高出现会清除此位
#define POWER_INT_VOOUT_L_VHOLD     (1<<4)      //IRQ4， 外部电源电压低于VHOLD，向此位写1或DCIN由高到低消失会清除此位
#define POWER_INT_BAT_TMP_HIGH      (1<<2)      //IRQ5， 电池温度过高，向此位写1或电池温度恢复到正常范围会清除此位
#define POWER_INT_BAT_TMP_LOW       (1<<1)      //IRQ6， 电池温度过低，向此位写1或电池温度恢复到正常范围会清除此位
#define POWER_INT_VDCDC1_LOW        (1<<16)     //IRQ7， DC-DC1输出电压过低，向此位写1或输出电压恢复到正常范围会清除此位
#define POWER_INT_VDCDC2_LOW        (1<<15)     //IRQ8， DC-DC2输出电压过低，向此位写1或输出电压恢复到正常范围会清除此位
#define POWER_INT_VDCDC3_LOW        (1<<14)     //IRQ9,  DC-DC3输出电压过低，向此位写1或输出电压恢复到正常范围会清除此位
#define POWER_INT_VLDO3_LOW         (1<<13)     //IRQ10，LDO3输出电压过低，向此位写1或输出电压恢复到正常范围会清除此位
#define POWER_INT_PEK_SHORT         (1<<12)     //IRQ11，PEK短按，向此位写1可清除此位
#define POWER_INT_PEK_LONG          (1<<11)     //IRQ12，PEK长按，向此位写1可清除此位
#define POWER_INT_BAT_IN            (1<<24)     //IRQ13，电池接入，向此位写1或电池被拔出会清除此位
#define POWER_INT_BAT_OUT           (1<<23)     //IRQ14，电池移除，向此位写1或电池被接入会清除此位
#define POWER_INT_BAT_ACTIVE_IN     (1<<22)     //IRQ15, 进入电池激活模式，向此位写1或退出电池激活模式会清除此位
#define POWER_INT_BAT_ACTIVE_OUT    (1<<21)     //IRQ16，退出电池激活模式，向此位写1或进入电池激活模式会清除此位
#define POWER_INT_CHARGE_START      (1<<20)     //IRQ17，开始充电，向此位写1或停止充电会清除此位
#define POWER_INT_CHARGE_END        (1<<19)     //IRQ18，充电完成，向此位写1或充电重新开始会清除此位
#define POWER_INT_LOWPWR_WARN       (1<<18)     //IRQ19，低电警告，向此位写1或AIPS电压恢复到正常范围会清除此位

typedef enum __DRV_POWER_CMD_GROUP
{
    DRV_POWER_CMD_READ_REG     =      0xff00,   //特殊读命令，不需要通道
    DRV_POWER_CMD_WRITE_REG,                    //特殊写命令，不需要通道
    DRV_POWER_CMD_BASIC_STATUS,                 //P1基本状态
    DRV_POWER_CMD_POWER_ON,                     //开机（空的，无效）
    DRV_POWER_CMD_POWER_OFF,                    //关机
    DRV_POWER_CMD_INT_ENABLE,                   //中断使能
    DRV_POWER_CMD_INT_DISABLE,                  //中断禁止
    DRV_POWER_CMD_INT_QUERY,                    //查询中断
    DRV_POWER_CMD_INT_CLEAR,                    //清除中断
    DRV_POWER_CMD_VOUT_GET,                     //输出电压读取
    DRV_POWER_CMD_VOUT_SET,                     //输出电压设置
    DRV_POWER_CMD_VSTS_GET,                     //输出电压状态读取
    DRV_POWER_CMD_VSTS_SET,                     //输出电压状态设置
    DRV_POWER_CMD_PWREPT_GET,                   //功率器件状态读取
    DRV_POWER_CMD_PWREPT_SET,                   //功率器件状态设置
    DRV_POWER_CMD_BATADC_GET,                   //电池采样状态读取
    DRV_POWER_CMD_BATADC_SET,                   //电池采样开关设置
    DRV_POWER_CMD_DCINADC_GET,                  //DCIN采样状态读取
    DRV_POWER_CMD_DCINADC_SET,                  //DCIN采样开关设置
    DRV_POWER_CMD_CHARGE_GET,                   //充电状态读取
    DRV_POWER_CMD_CHARGE_SET,                   //充电状态设置
    DRV_POWER_CMD_IPS_GET,                      //IPS数据读取
    DRV_POWER_CMD_IPS_SET,                      //IPS状态设置
    DRV_POWER_CMD_BATSTATUS_TABLE_GET,          //状态表
    DRV_POWER_CMD_GET_BAT_RDC,
    DRV_POWER_CMD_GET_DCIN,
    DRV_POWER_CMD_GET_FUELGUAGE,                //读取电池电量，时间，状态,新加cmd
    //aux = 0, pbuffer = __drv_power_fuelguage_t数据类型指针，参见数据类型说明
    DRV_POWER_CMD_SET_USB_CHARGE,               //提供给USB设置充电状态使用
    DRV_POWER_CMD_SET_STANDBY_FLAG,             // standby = 1, 从standby 状态恢复。
    DRV_POWER_CMD_ONOFFTIME_GET,
    DRV_POWER_CMD_ONOFFTIME_SET,
    DRV_POWER_CMD_CHGSTAT_GET,                  //获得充电状态，对应 __drv_power_charge_stat 结构体
    DRV_POWER_CMD_GET_COULOMB,                   //获得库仑计值，单位mAh
    DRV_POWER_CMD_GET_PMU_TYPE
} __drv_power_cmd_group_t;


#endif /*_DRV_POWER_H_*/

