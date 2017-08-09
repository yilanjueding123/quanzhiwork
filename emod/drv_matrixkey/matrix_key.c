/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                           Matrix Key Driver Module
*
*                                    (c) Copyright 2006-2010, All winners Co,Ld.
*                                             All Rights Reserved
*
* File    : matrix_key.c
* By      : james deng <csjamesdeng@gmail.com>
* Version : 1.0.0
* Date    : 2011-04-25
* Descript:
* Update  : date              author         ver       notes
*           2011-04-25        james deng     1.0.0     build the file.
*********************************************************************************************************
*/

#include "drv_matrix_key_i.h"
#include "bsp_rk.h"
#include "csi_init.h"

#if  0
//#define __here__            eLIBs_printf("@L%d(%s)\n", __LINE__, __FILE__);
#define __msg(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
						     eLIBs_printf(__VA_ARGS__)									        )
#else
#define __msg(...)    	
#endif


#define MENU        0x00000001
#define VOL_ADD     0x00000002
#define VOL_DEC     0x00000004
#define LEFT        0x00000008
#define RIGHT       0x00000010
#define ENTER       0x00000020


#define			NOKEY		0xffff
static u32		PrevKeyValue = NOKEY;
static __hdle	input_y2 = 0;
static ES_FILE  *iic_file;
static __u32 g_pw_off_support = 1;//为1则代表使用PA3口检测关机，和jtag口复用

static __u32 input_row1 = 0;
static __u32 input_row2 = 0;
static __u32 input_row3 = 0;
static __u32 x2_output = 0;
static __u32 y1_output = 0;
static __u32 first_power_on_cnt = 0;
static __u8  first_poweron_flag = 0;

__u8 adc_key_init_ok = 0;
__u8 key_time_count = 0;
__u8 adc_key_down = 0;
__u8 ex_key_down = 0;
static __u32 pw_off = 0;
__u32  sinle_level_value = 0;
__u32  prev_single_level_value = 0x55;

/* 扫描周期 */
static __s32 period;
/* 关机时间 */
static __s32 pw_off_time;
/* 按键按下延迟的时间，设为扫描周期的倍数，提高用户体验 */
static __s32 key_down_delay;

static __s32 volatile is_power_off = EPDK_FALSE;
static __krnl_stmr_t *timer = NULL;

static __u8 matrix_key_tid = 0;

//以下定义为了解决在组合按键检测情况下可能出现的按键down、up消息不配对的情况
static __u32 g_key_down_tbl[KEY_CNT] = {0};

void Matrixkey_ScanThread();//(void *p_arg)
__u32 adc_key_scan();
/********************************************************************************************************
* Function:    matrix_key_read
* Description: read routine
* Input：
* Output：
* Other：
*********************************************************************************************************/
static __u32 Request_GPIO(const char *ioname, int downup)
{
    __u32			 GPIO;
    __s32            ret;
    user_gpio_set_t  gpio_set[1];

    eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
    ret = esCFG_GetKeyValue("matrixkey_para", (char *)ioname, (int *)gpio_set, sizeof(user_gpio_set_t) / 4);
    if (!ret)
    {
        GPIO = esPINS_PinGrpReq(gpio_set, 1);
        if (!GPIO)
        {
            __wrn("request %s pin failed\n", ioname);
            return 0;
        }
    }

    else

    {
        __wrn("%s fetch para from script failed\n", ioname);
        return 0;
    }

    if(downup == 2)
    {
        ret = esPINS_SetPinPull(GPIO, PIN_PULL_DOWN, NULL);

        if (ret)
        {
            __wrn("pull %s failed\n", ioname);
            return 0;
        }
    }
    else if(downup == 1)
    {
        ret = esPINS_SetPinPull(GPIO, PIN_PULL_UP, NULL);

        if (ret)
        {
            __wrn("pull %s failed\n", ioname);
            return 0;
        }

    }

    return GPIO;
}

/********************************************************************************************************
* Function:    matrix_key_down
* Description: matrix key down
* Input：
* Output：
* Other：
*********************************************************************************************************/
static void matrix_key_down(__u32 key_value)
{
    __key_msg_t msg;

    msg.key_value = key_value;
    msg.flag = 1;

    //eLIBs_printf("&&&&&&&&&&& KEY DOWN &&&&&&&&&&\n");
    dev_matrix_key_ioctrl(0, DRV_KEY_CMD_PUTKEY, 0, &msg);

    g_key_down_tbl[key_value]++;
}

/********************************************************************************************************
* Function:    matrix_key_up
* Description: matrix key up
* Input：
* Output：
* Other：
*********************************************************************************************************/
static void matrix_key_up(__u32 key_value)
{
    __key_msg_t msg;

    if(g_key_down_tbl[key_value] > 0)
    {
        g_key_down_tbl[key_value]--;
    }
    else
    {
        return ;
    }

    //eLIBs_printf("&&&&&&&&&&& KEY UP &&&&&&&&&&\n");

    msg.key_value = key_value;
    msg.flag = 0;

    dev_matrix_key_ioctrl(0, DRV_KEY_CMD_PUTKEY, 0, &msg);
}

/********************************************************************************************************
* Function:    matrix_key_read
* Description: read routine
* Input：
* Output：
* Other：
*********************************************************************************************************/
static __u32 matrix_key_read()
{
    __s32 d1 = 0, d2 = 0, d3 = 0;
    __u32 input_row2 = 0, input_row3 = 0;
    __u32 output_col1 = 0, output_col2 = 0;
    if(1 == g_pw_off_support)
    {
        d1 = esPINS_ReadPinData(pw_off, 0);
        if (d1)
        {
            return KPAD_POWER;
        }
    }
    input_row2 = Request_GPIO((const char *)"input_row2", 2);
    if(input_row2 == 0)
    {
        return 0;
    }
    input_row3 = Request_GPIO((const char *)"input_row3", 2);
    if(input_row3 == 0)
    {
        esPINS_PinGrpRel(input_row2, 0);
        return 0;
    }
    d1 = esPINS_ReadPinData(input_row1, 0);
    d2 = esPINS_ReadPinData(input_row2, 0);
    d3 = esPINS_ReadPinData(input_row3, 0);
    if (!(d1 | d2 | d3))
    {
        esPINS_PinGrpRel(input_row3, 0);
        input_row3 = 0;
        output_col2 = Request_GPIO((const char *)"output_col2", 1);
        if(output_col2 == 0)
        {
            esPINS_PinGrpRel(input_row2, 0);
            return 0;
        }
        esPINS_WritePinData(output_col2, 1, NULL);
        d1 = esPINS_ReadPinData(input_row1, 0) ? LEFT : 0;
        d2 = esPINS_ReadPinData(input_row2, 0) ? RIGHT : 0;
        if (!(d1 | d2))
        {
            esPINS_PinGrpRel(input_row2, 0);
            input_row2 = 0;
            output_col1 = Request_GPIO((const char *)"output_col1", 1);
            if(output_col1 == 0)
            {
                esPINS_PinGrpRel(output_col2, 0);
                return 0;
            }
            esPINS_WritePinData(output_col1, 1, NULL);
            d1 = esPINS_ReadPinData(input_row1, 0) ? MENU : 0;
        }
        else
        {
            d1 = d1 | d2;
        }
    }
    else
    {
        if (d1)
        {
            d1 = esPINS_ReadPinData(input_row1, 0) ? VOL_ADD : 0;
        }
        if (d2)
        {
            d2 = esPINS_ReadPinData(input_row2, 0) ? ENTER : 0;
        }
        if (d3)
        {
            d3 = esPINS_ReadPinData(input_row3, 0) ? VOL_DEC : 0;
        }
        d1 = d1 | d2 | d3;
    }
    switch (d1)
    {
    case LEFT:
        d1 = KPAD_LEFT;
        break;
    case RIGHT:
        d1 = KPAD_RIGHT;
        break;
    case MENU:
        d1 = KPAD_MENU;
        break;
    case ENTER:
        d1 = KPAD_ENTER;
        break;
    case VOL_ADD:
        d1 = KPAD_VOICEUP;
        break;
    case VOL_DEC:
        d1 = KPAD_VOICEDOWN;
        break;
    default:
        d1 = 0;
    }
    if(input_row2)
        esPINS_PinGrpRel(input_row2, 0);
    if(input_row3)
        esPINS_PinGrpRel(input_row3, 0);
    if(output_col1)
        esPINS_PinGrpRel(output_col1, 0);
    if(output_col2)
        esPINS_PinGrpRel(output_col2, 0);
    return d1;
}
static __s32 get_adc_valume(void)
{
#define AD_DATA_NUM 8
    __u32 ret;
    __s32 data[AD_DATA_NUM];
    __s32 data_out[AD_DATA_NUM];
    __s32 average;
    __s32 i;
    __s32 cnt;
    __s32 valid_num;
    cnt = 0;
    while(1)
    {
        ret = RK_QueryIntPending();
        if(ret & RK_DATA_PENDING)
        {
            RK_GetData(data, sizeof(data) / sizeof(data[0]));
            RK_ClearIntPending(ret);
            RK_ClearFifo();
            break;
        }
        if(cnt++ > 2)
        {
            RK_ClearIntPending(ret);
            RK_ClearFifo();
            break;
        }
        esKRNL_TimeDly(1);
    }
    if(!(ret & RK_DATA_PENDING))
    {
        return 0x7ff;//直接返回最大值，默认为没有。
    }
#if 1//C500采样精度比C100多1bit，所以除以2
    {
        for(i = 0 ; i < sizeof(data) / sizeof(data[0]) ; i++)
        {
            data[i] /= 2;
        }
    }
#endif
    {
        valid_num = 0;
        for(i = 0 ; i < sizeof(data) / sizeof(data[0]) ; i++)
        {
            if(data[i] < 0x7ff)
            {
                data[valid_num++]  = data[i];
            }
        }
    }
    if(valid_num <= 0)
    {
        return 0x7ff;
    }
    valid_num = data_filter(data, data_out, valid_num);
    average = 0;
    for(i = 0 ; i < valid_num ; i++)
    {
        average += data_out[i];
    }
    average /= valid_num;
    return average;
}
__u32 read_data_filter(__u32 *input, __s32 arr_len)
{
	__u32 ret = 0;
	__s32 i, j;
	__u32 *data, tmp, half;

	data = input;
	i = arr_len;
	for(i = 0; i < arr_len - 1; i++)
	{
        for(j = 0; j < arr_len - i - 1; j++)
        {
            if(data[j] > data[j + 1])
            {
                tmp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = tmp;
            }
        }
	}

//	for(i = 0; i < arr_len; i++)
//	{
//		eLIBs_printf("input[%d] = %d, ", i, input[i]);
//	}
//	eLIBs_printf("\n");

	tmp = 0;
	for(i = 1; i < arr_len; i++)
	{
		tmp += input[i];
	}
	ret = tmp/(arr_len-1);

	return ret;
	
}
__s32 data_filter(__s32 *input, __s32  *output , int arr_len)
{
    __s32 tmp, i, j, half, rec, k = 0;
    __s32 *data;
    data = input;
    i = arr_len;
    for(i = 0; i < arr_len - 1; i++)
        for(j = 0; j < arr_len - i - 1; j++)
        {
            if(data[j] > data[j + 1])
            {
                tmp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = tmp;
            }
        }
    half = arr_len / 2;
    for(i = 0; i < arr_len; i++)
    {
        if(data[half] > data[i])
            rec = data[half] - data[i];
        else
            rec = data[i] - data[half];
        if(rec < 30)
        {
            output[k] = data[i];
            k++;
        }
    }
    return k;
}
static __u32 adc_single_scan_X1()
{
    __s32 adc_valume;
    __u32  level = 0;

    adc_valume = get_adc_valume();
//    if(adc_valume < 300) //420
//        level = KPAD_SINGLE0;
//    else if(adc_valume < 510)
//        level = KPAD_SINGLE1;
//    else if(adc_valume < 590)
//        level = KPAD_SINGLE2;
//    else if(adc_valume < 670)
//        level = KPAD_SINGLE3;
//    else
//        level = KPAD_SINGLE4;

    return adc_valume;
}
static __u32 adc_key_scan_X2()
{
    __s32 adc_valume;
    __u32 key = 0;
    adc_valume = get_adc_valume();

	if(adc_valume > 1850)
	{
		return EPDK_OK;
	}
    //__msg("ad_key_value=%d\n",adc_valume);
    if(adc_valume < 1820)
    {
        esKRNL_TimeDly(2);
        adc_valume = get_adc_valume();
        if(adc_valume < 1820)
        {
            if(adc_valume >= 80 && adc_valume < 380) //184
            {
            	__msg("kpad_prev key pressed\n");
                //key = KPAD_PREV;
                key = KPAD_NEXT;
            }
            else if(adc_valume >= 380 && adc_valume < 780) //367
            {
            	__msg("kpad_next key pressed\n");
                //key = KPAD_NEXT;
                key = KPAD_PREV;
            }
            else if(adc_valume >= 780 && adc_valume < 1100) //549
            {
            	__msg("ad_key_value=%d\n",adc_valume);
                key = KPAD_MODE; 
            }
            else if(adc_valume >= 1100 && adc_valume < 1400) //738
            {
            	__msg("kpad_mode key pressed\n");
                key = KPAD_SETTING;
            }
			else if((adc_valume >= 1400) && (adc_valume < 1600))
			{
				__msg("KPAD_ENTER\n");
				key = KPAD_ENTER; 
			}
            if(key)
            {
                adc_key_down = 1;
            }
        }
    }
    return key;
}
static __u32 adc_key_scan_Y1()
{
    __s32 adc_valume;
    __u32 key = 0;
    adc_valume = get_adc_valume();
	__msg("ad_key_value=%d\n",adc_valume);
    if(adc_valume <= 1820)
    {
        esKRNL_TimeDly(2);
        adc_valume = get_adc_valume();
        if(adc_valume <= 1820)
        {
            if(adc_valume >= 0 && adc_valume < 60) //tv 150 // fm25
            {
                key = KPAD_NUM1;
            }
            else if(adc_valume >= 100 && adc_valume < 200) //movie 150
            {
                key = KPAD_MUSIC;
            }
            else if(adc_valume >= 250 && adc_valume < 350) //movie 291
            {
                key = KPAD_MOVIE;
            }
            else if(adc_valume >= 400 && adc_valume < 500) //play 442
            {
                key = KPAD_MENU;//m
            }
            else if(adc_valume >= 500 && adc_valume < 600) //next 574
            {
                key = KPAD_RETURN;//r
            }
            else if(adc_valume >= 650 && adc_valume < 750) //right 704
            {
            }
            else if(adc_valume >= 800 && adc_valume < 900) //eq+846
            {
            }
            else if(adc_valume >= 950 && adc_valume < 1050) //eq-977
            {
                key = KPAD_LEFT;//r
            }
            else if(adc_valume >= 1050 && adc_valume < 1150) //return 1080
            {
                key = KPAD_RIGHT;//l
            }
            else if(adc_valume >= 1150 && adc_valume < 1250) //menu 1198
            {
            }
            else //if(adc_valume>=1710&&adc_valume<1820)
            {
                key = 0;
            }
            if(key)
            {
                adc_key_down = 1;
            }
        }
        eLIBs_printf("&&&&&&&&&&&&&& adc_valume_y1 = %d &&&&&&&&&&&&&\n", adc_valume);
    }
    return key;
}
static __u32 adc_key_scan_Y2()
{
    __s32 adc_valume;
    __u32 key = 0;
    adc_valume = get_adc_valume();
    if(adc_valume <= 1820)
    {
        esKRNL_TimeDly(2);
        adc_valume = get_adc_valume();
        if(adc_valume <= 1820)
        {
            if(adc_valume >= 0 && adc_valume < 60)
            {
                key = KPAD_NUM5;
            }
            if(key)
            {
                adc_key_down = 1;
            }
        }
        eLIBs_printf("&&&&&&&&&&&&&& adc_valume_y2 = %d &&&&&&&&&&&&&\n", adc_valume);
    }
    return key;
}

//全部按键松开后，用于补充发送未发送的key up消息
static void matrix_match_key_down(void)
{
    __s32 i;
    __s32 key_cnt;

    key_cnt = sizeof(g_key_down_tbl) / sizeof(g_key_down_tbl[0]);

    for(i = 0 ; i < key_cnt ; i++)
    {
        while(g_key_down_tbl[i] > 0)
        {
            matrix_key_up(i);
        }
    }
}

/********************************************************************************************************
* Function:    power_off_timer
* Description:
* Input：
* Output：
* Other：
*********************************************************************************************************/
static __u8  power_on_end = 0;
//__hdle hPowerOn = NULL;
static __u8 pw_key_flag = 0;

static void power_off_timer(void *arg)
{
    static int count = 0;
    static int count_relase = 0;
    ES_FILE *power_hd;
    //eLIBs_printf("power_off_timer 111\n");

    if(pw_off)
    {

        if(esPINS_ReadPinData(pw_off, 0))
        {
            //开机长按松开后处理
            if(power_on_end == 0 )
                return;
            //   eLIBs_printf("power_off_timer esPINS_ReadPinData count = %d\n",count);
            if(is_power_off == EPDK_FALSE)
            {
                if(++count >= 2)
                {
                    __log("----pw_off_key\n");

                    //matrix_key_down(KPAD_POWEROFF);
                    is_power_off = EPDK_TRUE;
                    count = 0;
                    pw_key_flag = 1;
                }
            }
            count_relase = 0;
        }
        else
        {

            if(power_on_end == 0 )
            {
                power_on_end = 1;
            }

            if(is_power_off == EPDK_TRUE )
            {
                if(pw_key_flag)
                {
                    pw_key_flag = 0;
                    //matrix_key_up(KPAD_POWEROFF);
                }
            }

            count = 0;
        }
    }
}
__s32 adc_key_init()
{
    RK_Init();
    RK_SetFifoDepth(RK_FIFO_DEPTH_08);
    RK_SetSampleRate(RK_SAMPLE_RATE_16K);
    RK_EnableInt(RK_DATA_IRQ);

    RK_SelectChannel(RK_CHANNEL_X1);
    RK_ClearFifo();
}
/********************************************************************************************************
* Function:    matrix_key_thread
* Description: matrix key running thread
* Input：
* Output：
* Other：
*********************************************************************************************************/
__u8 read_single_flag = 0;
__u32 get_single_value(void)
{
	//__u32 value;
    ////esKRNL_TimeDly(1);
    // RK_SelectChannel(RK_CHANNEL_X2);//x1
    // RK_ClearFifo();
    // esKRNL_TimeDly(2);//3 //1
    //value=adc_single_scan_X1();
    //esKRNL_TimeDly(2);//3 //1

    //read_single_flag = 1;
    return sinle_level_value;
}

static void matrix_key_thread(void *p_arg)
{
    __u32 old_key, new_key, key = 0;
    __s32 delay;
	__s32           count = 1;		//select channel

    //以下定义用于自动测试
    __s32 adc_valume;
    //static __u32 lopp_cnt=100;
    static __u32 flag = 0;

    if(adc_key_init_ok == 0)
    {
        //如果是debug模式，则自动测试，随机向系统发按键消息
        adc_key_init();
        adc_key_init_ok = 1;
    }

    RK_SelectChannel(RK_CHANNEL_X1);
    RK_ClearFifo();

    while (1)
    {
        if (esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
        {
            break;
        }
        esKRNL_TimeDly(1);

        if(adc_key_down == 1)
        {
            esKRNL_TimeDly(3);
            adc_key_down = 0;
        }
		if(1 == count)
		{
	        RK_SelectChannel(RK_CHANNEL_X1);
	        RK_ClearFifo();
	        old_key = adc_key_scan_X2();

	        delay = 0;
	        while (old_key) //外层循环扫描到一个键值!!!
	        {
	            esKRNL_TimeDly(3);

	            RK_SelectChannel(RK_CHANNEL_X1);
	            RK_ClearFifo();
	            esKRNL_TimeDly(2);
	            new_key = adc_key_scan_X2();

	            if (new_key) /* 读到一个有效值 */
	            {
	                //0831  modify   fankun
	                prev_single_level_value = 0x55;

	                /* 前后两次读到的值不同，并且前面读到的值有效 */
	                if ((new_key ^ old_key) && old_key)
	                {
	                    __msg("new_key=%d\n", new_key);
	                    __msg("old_key=%d\n", old_key);
	                    break;
	                }

	                delay += period;
	                if (!(delay ^ key_down_delay))
	                {
	                    /* 发送消息前检查是否延迟是否达到事先设定的阈值 */
	                    key = new_key;
	                    matrix_key_down(key);
	                    delay = 0;
	                }
	            }
	            else if (key) /* 按键弹起 */
	            {
	                matrix_key_up(key);
	                key = 0;
	                break; /* 退出内循环 */
	            }
	            else /* 读到一个无效值 */
	            {
	                //__here__;
	                __wrn("new_key = %d\n", new_key);
	                delay = 0;

	                matrix_match_key_down();
	                break;
	            }
	        }

			count = 0;
		}
		else if(0 == count)
		{
			if(read_single_flag)
			{
				__s32 read_cnt = 5;
				__u32 single_temp_cnt = 0;
				__u32 data_buff[5] = {0}; 
				//eLIBs_printf("\n*********************************\n");
				while(read_cnt--)
				{
					RK_SelectChannel(RK_CHANNEL_X2);//x1
		            RK_ClearFifo();
		            esKRNL_TimeDly(1);
					data_buff[read_cnt] = adc_single_scan_X1();
					//single_temp_cnt += adc_single_scan_X1();
					//eLIBs_printf("data_buff[%d] = %d\n",read_cnt, data_buff[read_cnt]);
				}
				sinle_level_value = read_data_filter(data_buff, 5);
//				eLIBs_printf("sinle_level_value = %d\n", sinle_level_value);
//				eLIBs_printf("\n*********************************\n");
				//sinle_level_value = single_temp_cnt/5;
				//read_single_flag = 0;
			}
			count = 1;
		}
    }
}
__s32 matrix_iic_key_init(void)
{
    __wrn("*******drv_key_ic_init********************\n");
#if 0
    input_y2 = Request_GPIO((const char *)"input_key", 2);
    if (input_y2 == 0)
    {
        __wrn("request input io failed\n");
        return EPDK_FAIL;
    }
    if(input_y2 == 0)
    {
        __wrn("input_y2 fail\n");
        return EPDK_FAIL;
    }
    if( iic_file == NULL )
    {
        iic_file = (ES_FILE *)eLIBs_fopen("b:\\BUS\\twi2", "r+");
    }
    if(iic_file == NULL)
    {
        __wrn("OPEN i2c device fail\n");
        return EPDK_FAIL;
    }
    eLIBs_fioctrl(iic_file, TWI_SET_SCL_CLOCK, 0, (void *)TWI_SCLK_50KHZ);
    PrevKeyValue = NOKEY;
#endif

    return EPDK_OK;
}
static __hdle reqInterGpio(__u8 *name, __u8 pin, __u8 pull)
{
    user_gpio_set_t  PinStat;
    __hdle			 hPin;

    eLIBs_strcpy(PinStat.gpio_name, name);
    PinStat.port	  = 4;
    PinStat.port_num  = pin;
    PinStat.mul_sel   = 0;
    PinStat.pull	  = pull;
    PinStat.drv_level = 1;
    PinStat.data	  = 0;
    hPin = esPINS_PinGrpReq(&PinStat, 1);
    return hPin;
}

/********************************************************************************************************
* Function:    matrix_key_init
* Description: initial matrix key
* Input：
* Output：
* Other：
*********************************************************************************************************/


__s32 matrix_key_init(void)
{
    __s32            ret;

    user_gpio_set_t gpio_set[1];

    eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));

    if(1 == g_pw_off_support)
    {
        /* 申请Power Off */
        ret = esCFG_GetKeyValue("matrixkey_para", "pw_off", (int *)gpio_set, sizeof(user_gpio_set_t) / 4);
        if (!ret)
        {
            pw_off = esPINS_PinGrpReq(gpio_set, 1);//reqInterGpio("pw_off",14,0);
            if (!pw_off)
            {
                __msg("request Power Off pin failed\n");
                return EPDK_FAIL;
            }

        }
        else
        {
            __wrn("fetch para from script failed\n");
            return EPDK_FAIL;
        }

    }
    /* 读取扫描周期 */
    ret = esCFG_GetKeyValue("matrixkey_para", "period", &period, 1);
    if (ret)
    {
        __wrn("fetch para: period from script failed\n");
        period = 2;
    }
    __msg("matrix key update period: %d\n", period);

    /* 读取关机时间 */
    ret = esCFG_GetKeyValue("matrixkey_para", "pw_off_time", &pw_off_time, 1);
    if (ret)
    {
        __wrn("fetch para: pw_off_time from script failed\n");
        pw_off_time = 2000;
    }
    __msg("power off time: %d\n", pw_off_time);

    /* 读取按键按下延迟的时间 */
    ret = esCFG_GetKeyValue("matrixkey_para", "key_down_delay", &key_down_delay, 1);
    if (ret)
    {
        __wrn("fetch para: key_down_delay from script failed\n");
        key_down_delay = 1 * period;
    }

    __msg("key down delay: %d\n", key_down_delay);

    /* 创建timer */
    timer = esKRNL_TmrCreate(
                500,
                OS_TMR_OPT_PRIO_HIGH | OS_TMR_OPT_PERIODIC,
                (OS_TMR_CALLBACK)power_off_timer,
                NULL);
    if (timer)
    {
        esKRNL_TmrStart(timer);
    }

    matrix_key_tid = esKRNL_TCreate(matrix_key_thread, 0, 0x400, KRNL_priolevel3);

    return EPDK_OK;
}

/********************************************************************************************************
* Function:    matrix_key_exit
* Description: exit matrix key
* Input：
* Output：
* Other：
*********************************************************************************************************/
__s32 matrix_key_exit(void)
{
    esPINS_PinGrpRel(input_row1, 0);
    esPINS_PinGrpRel(input_row2, 0);
    if(input_y2)
    {
        esPINS_PinGrpRel(input_y2, 0);
        input_y2 = 0;
    }
    if(1 == g_pw_off_support)
    {
        esPINS_PinGrpRel(pw_off, 0);
    }

    if(iic_file)
    {
        eLIBs_fclose(iic_file);
        iic_file = 0;
    }

    esKRNL_TDel(matrix_key_tid);
    if(timer)
    {
        esKRNL_TmrStop(timer, OS_TMR_OPT_PRIO_HIGH | OS_TMR_OPT_PERIODIC, NULL);
        esKRNL_TmrDel(timer);
        timer = NULL;
    }

    RK_Exit();
    __msg("matrix key exit finish\n");

    return EPDK_OK;
}
