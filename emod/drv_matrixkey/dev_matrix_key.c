/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                           Matrix Key Driver Module
*
*                                    (c) Copyright 2006-2010, All winners Co,Ld.
*                                             All Rights Reserved
*
* File    : dev_matrix_key.c
* By      : james deng <csjamesdeng@gmail.com>
* Version : 1.0.0
* Date    : 2011-04-25
* Descript:
* Update  : date              author         ver       notes
*           2011-04-25        james deng     1.0.0     build the file.
*********************************************************************************************************
*/

#include "drv_matrix_key_i.h"

/********************************************************************************************************
* Function:    dev_matrix_key_open
* Description: open matrix key device
* Input£º
* Output£º
* Other£º
*********************************************************************************************************/
static __hdle dev_matrix_key_open(void *open_arg, __u32 mode)
{
    __s32       cpu_sr;
    __matrix_key_dev_t *pKeyDev = (__matrix_key_dev_t *)open_arg;

    ENTER_CRITICAL(cpu_sr);
    if (pKeyDev->used)
    {
        EXIT_CRITICAL(cpu_sr);
        __wrn("matrix key dev open failed, used by someone else\n");
        return NULL;
    }
    pKeyDev->used = 1;
    EXIT_CRITICAL(cpu_sr);

    return (__hdle)pKeyDev;
}

/********************************************************************************************************
* Function:    dev_matrix_key_close
* Description: close matrix key device
* Input£º
* Output£º
* Other£º
*********************************************************************************************************/
static int dev_matrix_key_close(__hdle hKey)
{
    __s32              cpu_sr;
    __matrix_key_dev_t *pKeyDev = (__matrix_key_dev_t *)hKey;

    ENTER_CRITICAL(cpu_sr);
    if (pKeyDev->used == 0)
    {
        EXIT_CRITICAL(cpu_sr);
        return EPDK_FAIL;
    }
    pKeyDev->used = 0;
    EXIT_CRITICAL(cpu_sr);

    return EPDK_OK;
}

/********************************************************************************************************
* Function:    dev_matrix_key_read
* Description: read routine of matrix key device
* Input£º
* Output£º
* Other£º
*********************************************************************************************************/
static __u32 dev_matrix_key_read(void *pdata, __u32 size, __u32 n, __hdle hKey)
{
    __wrn("you can't read matrix key device\n");
    return 0;
}

/********************************************************************************************************
* Function:    dev_matrix_key_write
* Description: write routine of matrix key device
* Input£º
* Output£º
* Other£º
*********************************************************************************************************/
static __u32 dev_matrix_key_write(const void *pdata, __u32 size, __u32 n, __hdle hKey)
{
    __wrn("you can't write matrix key device\n");
    return 0;
}

/********************************************************************************************************
* Function:    dev_matrix_key_ioctrl
* Description: io ctrl routine of matrix key device
* Input£º
* Output£º
* Other£º
*********************************************************************************************************/
typedef struct   k_valuet
{
    __u32   single_value;
    __u32   resverd;
} k_single_t;

extern __u8 read_single_flag;


__s32 dev_matrix_key_ioctrl(__hdle hKey, __u32 cmd, __s32 aux, void *pbuffer)
{
    __s32 ret = EPDK_FAIL;

    switch (cmd)
    {
    case DRV_KEY_CMD_PUTKEY:
    {
        if(aux != 255)
        {
            __key_msg_t *msg = (__key_msg_t *)pbuffer;
            if(msg == NULL)
            {
                return ret;
            }

            /* send event to input system */
            __inf("keyborad send event to input system, "
                  "[type:%d, code:%x, value:%d]\n",
                  EV_KEY, msg->key_value, msg->flag);

            esINPUT_SendEvent(matrix_key_drv.pKeyc_dev,
                              EV_KEY,
                              msg->key_value,
                              msg->flag
                             );
            esINPUT_SendEvent(matrix_key_drv.pKeyc_dev,
                              EV_SYN,
                              SYN_REPORT,
                              0
                             );
        }
        else
        {
            __u32 key_value = (__u32)pbuffer;

            /* send event to input system */
            __inf("virtual keyboard send event to input system, "
                  "[type:%d, code:%x, value:%d]\n",
                  EV_KEY, key_value, 1);

            esINPUT_SendEvent(matrix_key_drv.pKeyc_dev,
                              EV_KEY,
                              key_value,
                              1 /*key down, no key up*/
                             );
            esINPUT_SendEvent(matrix_key_drv.pKeyc_dev,
                              EV_SYN,
                              SYN_REPORT,
                              0
                             );
        }

        break;
    }

    case DRV_KEY_CMD_SET_FIRST_DEBOUNCE_TIME:
    {
        //__u32 first_time = (__u32)pbuffer;
        k_single_t *k_single_value =  (k_single_t *)pbuffer;

        k_single_value->single_value = get_single_value();

        //BSP_LRADCK_set_firstKeyTime(first_time);
        //__log("jh_dbg_0216_1\n");
        break;
    }
	case DRV_KEY_CMD_AUTO_SEARCH_SPI:
	{
		__u8* channel = (__u8*)pbuffer;
		//eLIBs_printf("----channel = %d\n", *channel);
		spi_write_chanelfreq(*channel);
		//read_single_flag = 1;
		break;
	}
	case DRV_KEY_CMD_AUTO_SEARCH_SPI_BEGIN:
	{
		read_single_flag = 1;
		//eLIBs_printf("DRV_KEY_CMD_AUTO_SEARCH_SPI_BEGIN\n");
		break;
	}
	case DRV_KEY_CMD_AUTO_SEARCH_SPI_FINSH:
	{
		read_single_flag = 0;
		//eLIBs_printf("DRV_KEY_CMD_AUTO_SEARCH_SPI_BEGIN\n");
		break;
	}
    case DRV_KEY_CMD_SET_FIRST_RPT_TIME:
    {
        //__u32 first_delay = (__u32)pbuffer;
        //BSP_LRADCK_set_firstKeyDelay(first_delay);
        break;
    }

    case DRV_KEY_CMD_SET_SBSEQ_RPT_TIME:
    {
        //__u32 repeat_time = (__u32)pbuffer;
        //BSP_LRADCK_set_rpKeyTime(repeat_time);
        break;
    }

    case DRV_KEY_CMD_GET_FIRST_DEBOUNCE_TIME:
    {
        //ret = BSP_LRADCK_get_firstKeyTime();
        break;
    }

    case DRV_KEY_CMD_GET_FIRST_RPT_TIME:
    {
        //ret = BSP_LRADCK_get_firstKeyDelay();
        break;
    }

    case DRV_KEY_CMD_GET_SBSEQ_RPT_TIME:
    {
        //ret = BSP_LRADCK_get_rpKeyTime();
        break;
    }

    case DRV_KEY_CMD_GET_HOLDKEYVALUEMAX:
    {
        __wrn("holdkey temporary not defined\n");
        break;
    }

    case DRV_KEY_CMD_GET_HOLDKEYVALUEMIN:
    {
        __wrn("holdkey temporary not defined!\n");
        break;
    }

    default:
    {
        ret = EPDK_FAIL;
        break;
    }
    }

    return ret;
}

__dev_devop_t matrix_key_dev_ops =
{
    dev_matrix_key_open,
    dev_matrix_key_close,
    dev_matrix_key_read,
    dev_matrix_key_write,
    dev_matrix_key_ioctrl
};
