#include "drv_pwm_i.h"

#define pwm_get_wvalue(n)   (*((volatile __u32 *)(n)))          /* word input */
#define pwm_put_wvalue(n,c) (*((volatile __u32 *)(n))  = (c))   /* word output */
#define	PWM_ACTIVE_CYCLE_NUM	(64)
#define	PWM_MAX_FREQUENCY		(24000)

static __u32    pwm_hd;
static	__u32	activity_level = 0	;
static	__u32	frequency_level = 0	;
static	__u32	base_addr = 0xf1c21000;
static	volatile __hdle hpin[2]; // gpio handle


__hdle DEV_PWM_MOpen(void *open_arg, __u32 mode)
{
    return (__hdle)(&pwm_hd);
}

__s32 DEV_PWM_MClose(__hdle     hdev)
{
    return EPDK_OK;
}

__u32 DEV_PWM_MRead(void *pBuffer, __u32 size, __u32 count, __hdle hdev)
{
    return EPDK_OK;
}

__u32 DEV_PWM_MWrite(const void *pBuffer, __u32 size, __u32 count, __hdle hdev)
{
    return EPDK_OK;
}
static __s32 pwm_pio_config( __u32 pwm_channel, __bool on_off )
{
    char *pwm_name[] = {"pwm0_para", "pwm1_para"};
    user_gpio_set_t  gpio_set[1];
    __s32  ret;

    if(on_off == 0)
    {
        if(hpin[pwm_channel])
        {
            esPINS_PinGrpRel(hpin[pwm_channel], 1);
            hpin[pwm_channel] = NULL ;
        }
    }
    else
    {
        ret = esCFG_GetGPIOSecData(pwm_name[pwm_channel], (void *)gpio_set, 1);
        if(!ret)
        {
            hpin[pwm_channel] = esPINS_PinGrpReq(gpio_set, 1);
        }
        else
        {
            return EPDK_FAIL ;
        }
    }

    return EPDK_OK ;
}

static __s32 pwm_module_start( __u32 pwm_channel )
{
    __u32 tmp;
    __u32 pwm_div;
    __u8 lcd_pwm_div ;
    __u32 PWM_SOUCE_FREQ = 24000;	/*	The source clock of pwm is 24M osc , the unit here is K*/
    __u32 PWM_LEVEL = PWM_ACTIVE_CYCLE_NUM;


    __u32 entire_cycles;
    __u32 active_cycles;




    pwm_pio_config(pwm_channel, 1);



    entire_cycles = PWM_SOUCE_FREQ / frequency_level - 1;
    active_cycles = activity_level * (entire_cycles + 1) / (PWM_ACTIVE_CYCLE_NUM - 1);


    //eLIBs_printf("entire_cycles = %d\n",entire_cycles);
    //eLIBs_printf("active_cycles = %d\n",active_cycles);



    tmp = pwm_get_wvalue(base_addr + 0x00);
    tmp &= ~(0xf << (0 + pwm_channel * 15));  //clear default pwm_pre_scale , for pwm0 , prescale bit is 0-4, for pwm1, prescale bit is 15-18
    //bit6:gatting the special clock for pwm0
    //bit5:pwm channel0  active state is high level
    //bit4:pwm channel0  en
    tmp |= ((1 << (6 + pwm_channel * 15)) | (1 << (5 + pwm_channel * 15)) | (0xf << (0 + pwm_channel * 15)));
    pwm_put_wvalue(base_addr + 0x00, tmp);

    tmp = (entire_cycles << 16) | (active_cycles << 0);

    pwm_put_wvalue(base_addr + (0x04 + pwm_channel * 0x04), tmp);
    tmp = pwm_get_wvalue(base_addr + 0x00);
    tmp |= (1 << (4 + pwm_channel * 15));
    pwm_put_wvalue(base_addr + 0x00, tmp);

    return EPDK_OK;
}

static __s32 pwm_module_stop( __u32 pwm_channel )
{
    __u32 tmp;

    //bit4:pwm channel0  en
    tmp = pwm_get_wvalue(base_addr + 0x00);
    tmp &= ~(1 << (4 + pwm_channel * 15));
    pwm_put_wvalue(base_addr + 0x00, tmp);

    pwm_pio_config(pwm_channel, 0);

    return EPDK_OK;
}

__s32 DEV_PWM_MIoctrl(__hdle hDev, __u32 cmd, __s32 aux, void *pbuffer)
{
    __s32 ret = EPDK_FAIL ;

    switch(cmd)
    {
    case PWM_CMD_START:
        pwm_module_start((__u32)pbuffer);
        break ;
    case PWM_CMD_STOP:
        pwm_module_stop((__u32)pbuffer);
        break ;
    case PWM_CMD_SET_ACTIVY_CYCLE:
    {
        if( aux >= 0 && aux < PWM_ACTIVE_CYCLE_NUM )
        {
            activity_level = aux;
        }
        break ;
    }
    case PWM_CMD_SET_PWM_FRE:
    {
        if( aux >= 0 && aux < PWM_MAX_FREQUENCY )
        {
            frequency_level = aux;
        }
        break ;
    }
    default:
        break ;

    }

    return ret;
}

__dev_devop_t pwmdev_ops =
{
    DEV_PWM_MOpen,
    DEV_PWM_MClose,
    DEV_PWM_MRead,
    DEV_PWM_MWrite,
    DEV_PWM_MIoctrl
};


