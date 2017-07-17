
#ifndef __DRV_PWM_H__
#define __DRV_PWM_H__


typedef enum tag_PWM_CMD
{
    PWM_CMD_RESERVE0 = 0x00,
    PWM_CMD_START = 0x01,
    PWM_CMD_STOP = 0x02,
    PWM_CMD_SET_ACTIVY_CYCLE = 0x03,		/* aux = activy cycle	*/
    PWM_CMD_SET_PWM_FRE = 0x04,				/* aux = pwm frequency, unit:Khz , from 1K hz to 24M	*/
} __pwm_cmd_t;


#endif

