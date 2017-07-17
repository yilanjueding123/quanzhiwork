/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                                standby module
*
*                                   (c) Copyright 2008-2009, kevin China
*                                            All Rights Reserved
*
* File    : power_axp189.h
* By      : kevin
* Version : V1.0
* Date    : 2009-6-16 17:25
*********************************************************************************************************
*/
#ifndef _POWER_AXP189_H_
#define _POWER_AXP189_H_

#include "ePDK.h"

//config power parameter for standby
//Adjust DCDC2 Voltage
#define TARGET_VOL_DC2  1120    //1.12V,this value should be decided by the host's verify report
//Adjust DCDC1 Voltage
#define TARGET_VOL_DC1  3000    //1.12V,this value should be decided by the host's verify report
//Adjust DCDC3 Voltage
#define TARGET_VOL_DC3  2300    //1.12V,this value should be decided by the host's verify report
//low power voltage
#define LOW_BAT_VOL     3450


//register byte address accessed by iic interface
#define POWER_STATUS                (0x00)
#define POWER_IPS_SET               (0x01)
#define POWER_ONOFF                 (0x02)
#define POWER_CHARGE1               (0x03)
#define POWER_CHARGE2               (0x04)
#define POWER_PEK                   (0x05)
#define POWER_INTEN1                (0x06)
#define POWER_INTEN2                (0x07)
#define POWER_INTEN3                (0x08)
#define POWER_INTSTS1               (0x09)
#define POWER_INTSTS2               (0x0A)
#define POWER_INTSTS3               (0x0B)
#define POWER_VENDER_USED1          (0x0C)
#define POWER_DCDCCTL               (0x0D)
#define POWER_DC12OUT_VOL           (0x0E)
#define POWER_LDOOUT_VOL            (0x0F)
#define POWER_SW_CTL                (0x10)
#define POWER_BATTERY_VOL           (0x11)
#define POWER_BATTERY_CURRENT       (0x12)
#define POWER_DCIN_VOL              (0x13)
#define POWER_DCIN_CURRENT          (0x14)
#define POWER_ADCSW_CTL             (0x15)
#define POWER_VENDER_USED2          (0x16)
#define POWER_EPT_SW                (0x17)
#define POWER_DATA_BUFFER1          (0x18)
#define POWER_DATA_BUFFER2          (0x19)
#define POWER_VENDER_USED3          (0x1A)


//slave address of p1 accessed from iic interface
#define P1_SLAVE_ADDR               (0x16)


extern void  axp189_set_voltage(__s32 type, __s32 voltage);
extern __u32 axp189_get_voltage(__s32 type);
extern __s32 axp189_low_power_dectect(void);
extern __s32 axp189_ext_power_detect(void);
extern void  axp189_power_off(void);
extern __s32 axp189_power_key_detect(void);

#endif  //_POWER_AXP189_H_

