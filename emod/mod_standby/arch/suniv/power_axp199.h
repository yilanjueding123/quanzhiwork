/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                                standby module
*
*                                   (c) Copyright 2008-2009, kevin China
*                                            All Rights Reserved
*
* File    : power_axp199.h
* By      : kevin
* Version : V1.0
* Date    : 2009-6-16 17:25
*********************************************************************************************************
*/
#ifndef _POWER_AXP199_H_
#define _POWER_AX_199_H_

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

/******************AXP19X的寄存器定义*************************/
#define AXP19X_POWER_STATUS                     (0x00)
#define AXP19X_POWER_MODE_CHGSTATUS             (0x01)
#define AXP19X_POWER_OTG_STATUS                 (0x02)
#define AXP19X_POWER_DATA_BUFFER1               (0x06)
#define AXP19X_POWER_DATA_BUFFER2               (0x07)
#define AXP19X_POWER_DATA_BUFFER3               (0x08)
#define AXP19X_POWER_DATA_BUFFER4               (0x09)
#define AXP19X_POWER_VERSION                    (0x0C)
#define AXP19X_POWER_LDO3_DC2_CTL               (0x10)
#define AXP19X_POWER_LDO24_DC13_CTL             (0x12)
#define AXP19X_POWER_DC2OUT_VOL                 (0x23)
#define AXP19X_POWER_LDO3_DC2_DVM               (0x25)
#define AXP19X_POWER_DC1OUT_VOL                 (0x26)
#define AXP19X_POWER_DC3OUT_VOL                 (0x27)
#define AXP19X_POWER_LDO24OUT_VOL               (0x28)
#define AXP19X_POWER_LDO3OUT_VOL                (0x29)
#define AXP19X_POWER_IPS_SET                    (0x30)
#define AXP19X_POWER_VOFF_SET                   (0x31)
#define AXP19X_POWER_OFF_CTL                    (0x32)
#define AXP19X_POWER_CHARGE1                    (0x33)
#define AXP19X_POWER_CHARGE2                    (0x34)
#define AXP19X_POWER_BACKUP_CHG                 (0x35)
#define AXP19X_POWER_POK_SET                    (0x36)
#define AXP19X_POWER_DCDC_FREQSET               (0x37)
#define AXP19X_POWER_VLTF_CHGSET                (0x38)
#define AXP19X_POWER_VHTF_CHGSET                (0x39)
#define AXP19X_POWER_APS_WARNING1               (0x3A)
#define AXP19X_POWER_APS_WARNING2               (0x3B)
#define AXP19X_POWER_VLTF_DISCHGSET             (0x3C)
#define AXP19X_POWER_VHTF_DISCHGSET             (0x3D)
#define AXP19X_POWER_DCDC_MODESET               (0x80)
#define AXP19X_POWER_VOUT_MONITOR               (0x81)
#define AXP19X_POWER_ADC_EN1                    (0x82)
#define AXP19X_POWER_ADC_EN2                    (0x83)
#define AXP19X_POWER_ADC_SPEED                  (0x84)
#define AXP19X_POWER_ADC_INPUTRANGE             (0x85)
#define AXP19X_POWER_TIMER_CTL                  (0x8A)
#define AXP19X_POWER_VBUS_DET_SRP               (0x8B)
#define AXP19X_POWER_HOTOVER_CTL                (0x8F)
#define AXP19X_POWER_GPIO0_CTL                  (0x90)
#define AXP19X_POWER_GPIO0_VOL                  (0x91)
#define AXP19X_POWER_GPIO1_CTL                  (0x92)
#define AXP19X_POWER_GPIO2_CTL                  (0x93)
#define AXP19X_POWER_GPIO012_SIGNAL             (0x94)
#define AXP19X_POWER_SENSE_CTL                  (0x95)
#define AXP19X_POWER_SENSE_SIGNAL               (0x96)
#define AXP19X_POWER_GPIO20_PDCTL               (0x97)
#define AXP19X_POWER_PWM1_FREQ                  (0x98)
#define AXP19X_POWER_PWM1_DUTYDE                (0x99)
#define AXP19X_POWER_PWM1_DUTY                  (0x9A)
#define AXP19X_POWER_PWM2_FREQ                  (0x9B)
#define AXP19X_POWER_PWM2_DUTYDE                (0x9C)
#define AXP19X_POWER_PWM2_DUTY                  (0x9D)
#define AXP19X_POWER_RSTO_CTL                   (0x9E)
#define AXP19X_POWER_GPIO67_CTL                 (0x9F)
#define AXP19X_POWER_INTEN1                     (0x40)
#define AXP19X_POWER_INTEN2                     (0x41)
#define AXP19X_POWER_INTEN3                     (0x42)
#define AXP19X_POWER_INTEN4                     (0x43)
#define AXP19X_POWER_INTSTS1                    (0x44)
#define AXP19X_POWER_INTSTS2                    (0x45)
#define AXP19X_POWER_INTSTS3                    (0x46)
#define AXP19X_POWER_INTSTS4                    (0x47)
#define AXP19X_POWER_COULOMB_CTL                (0xB8)

//adc data register
#define AXP19X_POWER_BAT_AVERVOL_H8             (0x78)
#define AXP19X_POWER_BAT_AVERVOL_L4             (0x79)
#define AXP19X_POWER_BAT_AVERCHGCUR_H8          (0x7A)
#define AXP19X_POWER_BAT_AVERCHGCUR_L5          (0x7B)
#define AXP19X_POWER_BAT_VOL_H8                 (0x50)
#define AXP19X_POWER_BAT_VOL_L4                 (0x51)
#define AXP19X_POWER_ACIN_VOL_H8                (0x56)
#define AXP19X_POWER_ACIN_VOL_L4                (0x57)
#define AXP19X_POWER_ACIN_CUR_H8                (0x58)
#define AXP19X_POWER_ACIN_CUR_L4                (0x59)
#define AXP19X_POWER_VBUS_VOL_H8                (0x5A)
#define AXP19X_POWER_VBUS_VOL_L4                (0x5B)
#define AXP19X_POWER_VBUS_CUR_H8                (0x5C)
#define AXP19X_POWER_VBUS_CUR_L4                (0x5D)
#define AXP19X_POWER_BAT_AVERDISCHGCUR_H8       (0x7C)
#define AXP19X_POWER_BAT_AVERDISCHGCUR_L5       (0x7D)
#define AXP19X_POWER_APS_AVERVOL_H8             (0x7E)
#define AXP19X_POWER_APS_AVERVOL_L4             (0x7F)
#define AXP19X_POWER_BAT_CHGCOULOMB3            (0xB0)
#define AXP19X_POWER_BAT_CHGCOULOMB2            (0xB1)
#define AXP19X_POWER_BAT_CHGCOULOMB1            (0xB2)
#define AXP19X_POWER_BAT_CHGCOULOMB0            (0xB3)
#define AXP19X_POWER_BAT_DISCHGCOULOMB3         (0xB4)
#define AXP19X_POWER_BAT_DISCHGCOULOMB2         (0xB5)
#define AXP19X_POWER_BAT_DISCHGCOULOMB1         (0xB6)
#define AXP19X_POWER_BAT_DISCHGCOULOMB0         (0xB7)
#define AXP19X_POWER_BAT_POWERH8                (0x70)
#define AXP19X_POWER_BAT_POWERM8                (0x71)
#define AXP19X_POWER_BAT_POWERL8                (0x72)
/**********************AXP19X的寄存器定义完成*****************************/

//slave address of p1 accessed from iic interface
#define AXP19X_SLAVE_ADDR                       (0x34)


extern void  axp199_set_voltage(__s32 type, __s32 voltage);
extern __u32 axp199_get_voltage(__s32 type);
extern __s32 axp199_low_power_dectect(void);
extern __s32 axp199_ext_power_detect(void);
extern void  axp199_power_off(void);
extern __s32 axp199_power_key_detect(void);

#endif  //_POWER_AXP199_H_

