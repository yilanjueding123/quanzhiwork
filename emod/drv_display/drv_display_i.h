
#ifndef  _DRV_DISPLAY_I_H_
#define  _DRV_DISPLAY_I_H_

#include "epdk.h"

#define FALSE 0
#define TRUE 1


#define MAX_EVENT_SEM 20
typedef struct
{
    __u32         	mid;
    __u32         	used;
    __u32         	status;
    __u32    		exit_mode;//0:clean all  1:disable interrupt
    __krnl_event_t *scaler_finished_sem[2];
    __krnl_event_t *event_sem[2][MAX_EVENT_SEM];
    __bool			de_cfg_rdy[2][MAX_EVENT_SEM];
    __bool			event_used[2][MAX_EVENT_SEM];
    __bool          b_cache[2];
    __bool			reg_operate_vaild; //0:can't read/write registers ; 1: can read/write registers
} __disp_drv_t;


#define sys_get_value(n)    (*((volatile __u8 *)(n)))          /* byte input */
#define sys_put_value(n,c)  (*((volatile __u8 *)(n))  = (c))   /* byte output */
#define sys_get_hvalue(n)   (*((volatile __u16 *)(n)))         /* half word input */
#define sys_put_hvalue(n,c) (*((volatile __u16 *)(n)) = (c))   /* half word output */
#define sys_get_wvalue(n)   (*((volatile __u32 *)(n)))          /* word input */
#define sys_put_wvalue(n,c) (*((volatile __u32 *)(n))  = (c))   /* word output */
#define sys_set_bit(n,c)    (*((volatile __u8 *)(n)) |= (c))   /* byte bit set */
#define sys_clr_bit(n,c)    (*((volatile __u8 *)(n)) &=~(c))   /* byte bit clear */
#define sys_set_hbit(n,c)   (*((volatile __u16 *)(n))|= (c))   /* half word bit set */
#define sys_clr_hbit(n,c)   (*((volatile __u16 *)(n))&=~(c))   /* half word bit clear */
#define sys_set_wbit(n,c)   (*((volatile __u32 *)(n))|= (c))    /* word bit set */
#define sys_cmp_wvalue(n,c) (c == (*((volatile __u32 *) (n))))
#define sys_clr_wbit(n,c)   (*((volatile __u32 *)(n))&=~(c))


__s32 DRV_lcd_open(__u32 sel);
__s32 DRV_lcd_close(__u32 sel);
__s32 disp_int_process(__u32 sel);

__s32 DRV_DISP_MInit(void);
__s32 DRV_DISP_MExit(void);
__mp *DRV_DISP_MOpen(__u32 mid, __u32 mode);
__s32 DRV_DISP_MClose(__mp *mp);
__u32 DRV_DISP_MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 DRV_DISP_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
__s32 DRV_DISP_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);

extern __dev_devop_t dispdev_ops;

#endif
