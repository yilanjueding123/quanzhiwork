/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                           Matrix Key Driver Module
*
*                                    (c) Copyright 2006-2010, All winners Co,Ld.
*                                             All Rights Reserved
*
* File    : drv_matrix_key.c
* By      : james deng <csjamesdeng@gmail.com>
* Version : 1.0.0
* Date    : 2011-04-25
* Descript:
* Update  : date              author         ver       notes
*           2011-04-25        james deng     1.0.0     build the file.
*********************************************************************************************************
*/

#include "drv_matrix_key_i.h"
#include "csi_init.h"

#define MATRIX_KEY_DEV_NAME "MATRIX_KEY"
#define KEY_CODE_TABLE_SIZE 14//8

__matrix_key_drv_t matrix_key_drv;
__matrix_key_dev_t matrix_key_dev;

static int matrix_keymapping[KEY_CODE_TABLE_SIZE] =
{
    KPAD_MENU,
    KPAD_PREV,
    KPAD_NEXT,
    KPAD_ENTER,
    KPAD_MODE,
    KPAD_PHOTO,
    KPAD_POWER,
    KPAD_POWEROFF,
    KPAD_SINGLE0,
    KPAD_SINGLE1,
    KPAD_SINGLE2,
    KPAD_SINGLE3,
    KPAD_SINGLE4,
    KPAD_SETTING
};

/********************************************************************************************************
* Function:    drv_matrix_key_init
* Description: initial matrix key driver
* Input£º
* Output£º
* Other£º
*********************************************************************************************************/
__s32 drv_matrix_key_init(void)
{
    matrix_key_drv.used = 0;
    return matrix_key_init();;
}

/********************************************************************************************************
* Function:    drv_matrix_key_exit
* Description: exit matrix key driver
* Input£º
* Output£º
* Other£º
*********************************************************************************************************/
__s32 drv_matrix_key_exit(void)
{
    matrix_key_drv.used = 0;
    return matrix_key_exit();;
}

/********************************************************************************************************
* Function:    drv_matrix_key_open
* Description: open matrix key driver
* Input£º
* Output£º
* Other£º
*********************************************************************************************************/
__mp *drv_matrix_key_open(__u32 mid, __u32 mode)
{
    __s32 cpu_sr;

    ENTER_CRITICAL(cpu_sr);
    if (matrix_key_drv.used)
    {
        __wrn("driver already openned by someone else\n");
        return (__mp *)0;
    }
    matrix_key_drv.used = 1;
    EXIT_CRITICAL(cpu_sr);

    matrix_key_drv.mid = mid;
	
	CSI_init_req();
	
    return (__mp *)&matrix_key_drv;
}

/********************************************************************************************************
* Function:    drv_matrix_key_close
* Description: close matrix key driver
* Input£º
* Output£º
* Other£º
*********************************************************************************************************/
__s32 drv_matrix_key_close(__mp *mp)
{
    __matrix_key_drv_t *drv = (__matrix_key_drv_t *)mp;
    if (drv->used == 0)
    {
        return EPDK_FAIL;
    }
    drv->used = 0;

    return EPDK_OK;
}

/********************************************************************************************************
* Function:    drv_matrix_key_read
* Description: read routine of matrix key driver
* Input£º
* Output£º
* Other£º
*********************************************************************************************************/
__u32 drv_matrix_key_read(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return 0;
}

/********************************************************************************************************
* Function:    drv_matrix_key_write
* Description: write routine of matrix key driver
* Input£º
* Output£º
* Other£º
*********************************************************************************************************/
__u32 drv_matrix_key_write(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return 0;
}

/********************************************************************************************************
* Function:    drv_matrix_key_ioctrl
* Description: io ctrl routine of matrix key driver
* Input£º
* Output£º
* Other£º
*********************************************************************************************************/
__s32 drv_matrix_key_ioctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    __s32 cpu_sr = 0;

    switch (cmd)
    {
    case DRV_CMD_PLUGIN:
    {
        __u32 keyCodeNum = 0;
        __u32 i = 0;
        __input_dev_t *pKeycDev = NULL;

        ENTER_CRITICAL(cpu_sr);
        if (matrix_key_drv.status)
        {
            __wrn("Driver cannot support more than one matrix key\n");
        }
        matrix_key_drv.status = 1;
        EXIT_CRITICAL(cpu_sr);

        pKeycDev = (__input_dev_t *)eLIBs_malloc(sizeof(__input_dev_t));
        if (!pKeycDev)
        {
            __wrn("allocate memory for input device hanle fail\n");
            return EPDK_FAIL;
        }

        eLIBs_memset(pKeycDev, 0, sizeof(__input_dev_t));
        matrix_key_dev.used = 0; /* for device manager */

        /* general managment system information */
        pKeycDev->classname = INPUT_CLASS_NAME;
        pKeycDev->name      = MATRIX_KEY_DEV_NAME;
        pKeycDev->ops       = &matrix_key_dev_ops;
        pKeycDev->parg      = &matrix_key_dev;

        pKeycDev->id.product = 0x0001;
        pKeycDev->id.version = 0x0001;

        /* KEYBOARD DEVICE */
        pKeycDev->device_class = INPUT_KEYBOARD_CLASS;

        /* set keyboard support event types */
        pKeycDev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);

        /* set keyboard support event codes */
        keyCodeNum = KEY_CODE_TABLE_SIZE;
        for (i = 0; i < keyCodeNum; i++)
        {
            __set_bit(matrix_keymapping[i], pKeycDev->keybit);
        }

        /* set key event handler, handle the feedback events */
        pKeycDev->event = drv_keyc_event;

        /* register keyboard device to INPUT managemant system */
        if (esINPUT_RegDev(pKeycDev) != EPDK_OK)
        {
            __wrn("register matrix key device to input system failed\n");
            eLIBs_free(pKeycDev);
            return EPDK_FAIL;
        }

        /* attach pKeycDev to keydrv */
        matrix_key_drv.pKeyc_dev = pKeycDev;

        return EPDK_OK;
    }

    case DRV_CMD_PLUGOUT:
    {
        /* unregister keyboard deivce from INPUT manage system,
         * free keyboard device handle.
         */
        if (matrix_key_dev.used)
        {
            __wrn("matrix key is using now\n");
            return EPDK_FAIL;
        }

        esINPUT_UnregDev(matrix_key_drv.pKeyc_dev);
        eLIBs_free(matrix_key_drv.pKeyc_dev);

        return EPDK_OK;
    }

    case DRV_CMD_GET_STATUS:
    {
        if (matrix_key_drv.status)
        {
            return DRV_STA_BUSY;
        }
        else
        {
            return DRV_STA_FREE;
        }
    }

    default:
        break;
    }

    return EPDK_OK;
}

/********************************************************************************************************
* Function:    drv_keyc_event
* Description: deal with matrix key feedback event
* Input£º
* Output£º
* Other£º
*********************************************************************************************************/
__s32 drv_keyc_event(__input_dev_t *dev, __u32 type, __u32 code, __s32 value)
{
    eLIBs_printf("matrix key feedback event [type:%d, code:%d, value:%d]\n", type, code, value);
    return EPDK_OK;
}
