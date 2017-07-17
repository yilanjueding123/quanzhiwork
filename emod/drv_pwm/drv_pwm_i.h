
#ifndef  _DRV_PWM_I_H_
#define  _DRV_PWM_I_H_

#include "epdk.h"

__s32 DRV_PWM_MInit(void);
__s32 DRV_PWM_MExit(void);
__mp *DRV_PWM_MOpen(__u32 mid, __u32 mode);
__s32 DRV_PWM_MClose(__mp *mp);
__u32 DRV_PWM_MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 DRV_PWM_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
__s32 DRV_PWM_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);

extern __dev_devop_t pwmdev_ops;

#endif
