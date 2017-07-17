
#include "drv_pwm_i.h"


typedef struct
{
    __u32		mid;
    __u32		used;
    __u32		status;
    __hdle		hReg;

} __pwm_drv_t;

static __pwm_drv_t  pwm_ext_hd;

__s32 DRV_PWM_MInit(void)
{
    __inf("DRV_PWM_MInit\n");
    return EPDK_OK;
}

__s32 DRV_PWM_MExit(void)
{
    __msg("DRV_PWM_MExit\n");
    return EPDK_OK;
}

__mp *DRV_PWM_MOpen(__u32 mid, __u32 mode)
{
    pwm_ext_hd.mid = mid;
    pwm_ext_hd.used = 1;

    return (__mp *)&pwm_ext_hd;
}

__s32 DRV_PWM_MClose(__mp *pDrv)
{
    pwm_ext_hd.used = 0;
    return EPDK_OK;
}
__u32 DRV_PWM_MRead(void *pBuffer, __u32 size, __u32 count, __mp *pdev)
{
    return EPDK_OK;
}
__u32 DRV_PWM_MWrite(const void *pBuffer, __u32 size, __u32 count, __mp *pdev)
{
    return EPDK_OK;
}


__s32 DRV_PWM_MIoctrl(__mp *pDev, __u32 cmd, __s32 aux, void *pbuffer)
{
    __s32 ret = EPDK_OK;

    switch(cmd)
    {
    case DRV_CMD_PLUGIN:
    {
        pwm_ext_hd.hReg = esDEV_DevReg("USER", "PWM", &pwmdev_ops, 0);
        if(!pwm_ext_hd.hReg)
        {
            __err("esDEV_DevReg() fail!\n");
            return EPDK_FAIL;
        }
        pwm_ext_hd.status = DRV_STA_BUSY ;
        break;
    }
    case DRV_CMD_PLUGOUT:
    {
        if(pwm_ext_hd.hReg)
        {
            esDEV_DevUnreg(pwm_ext_hd.hReg);
            pwm_ext_hd.hReg = NULL ;
        }
        pwm_ext_hd.status = DRV_STA_FREE;
        break;
    }
    case DRV_CMD_GET_STATUS:
    {
        ret = pwm_ext_hd.status;
        break;
    }
    default:
    {
        ret = EPDK_FAIL;
        __wrn("unknown command!\n");
        break;
    }
    }

    return  ret;
}


