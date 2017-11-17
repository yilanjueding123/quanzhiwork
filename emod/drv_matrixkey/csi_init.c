
#include "csi_init.h"

typedef struct 
{	
	__hdle           hdl_cs;
	__hdle           hdl_CLK;
	__hdle           hdl_DAT;
	__hdle           hdl_SW;	//reserved
}csi_pin_hd;

static csi_pin_hd req_csi_pin;

#define Spi_SetDatepin()						esPINS_SetPinIO(req_csi_pin.hdl_DAT, 1, NULL);				\
												esPINS_WritePinData(req_csi_pin.hdl_DAT, 1, NULL);
#define Spi_SetCsnpin()						esPINS_SetPinIO(req_csi_pin.hdl_cs, 1, NULL);				\
												esPINS_WritePinData(req_csi_pin.hdl_cs, 1, NULL);
#define Spi_SetClkpin()						esPINS_SetPinIO(req_csi_pin.hdl_CLK, 1, NULL);				\
												esPINS_WritePinData(req_csi_pin.hdl_CLK, 1, NULL);

#define Spi_ClrDatepin()					esPINS_SetPinIO(req_csi_pin.hdl_DAT, 1, NULL);				\
											esPINS_WritePinData(req_csi_pin.hdl_DAT, 0, NULL);
#define Spi_clrCsnpin()					esPINS_SetPinIO(req_csi_pin.hdl_cs, 1, NULL);				\
											esPINS_WritePinData(req_csi_pin.hdl_cs, 0, NULL);
#define Spi_ClrClkpin()					esPINS_SetPinIO(req_csi_pin.hdl_CLK, 1, NULL);				\
											esPINS_WritePinData(req_csi_pin.hdl_CLK, 0, NULL);



#define  Spi_Nop()             {__u8 count=150;while(count--);}

#ifdef DV_FREQ_UPPER_ARRANGEMENT
/*此处为下面的频点按顺序从小到大对应的值，并且是计算了结果的值*/
const __u32  chanel_freq[32]=
{
	10263,
	10369,
	10379,
	10389,
	10399,
	10499,
	10502,
	10505,
	10508,
	10512,
	10515,
	10518,
	10522,
	10525,
	10527,
	10628,
	10631,
	10633,
	10638,
	10641,
	10642,
	10648,
	10651,
	10652,
	10754,
	10757,
	10757,
	10764,
	10767,
	10777,
	10883,
	10893,
};
#elif 0
const __u32  chanel_freq[32]=
{
	0x02A8D,		// 1	5945
	0x02A83,		// 2	5925
	0x02A19,		// 3 	5905
	0x02A0F,		// 4	5885
	0x02A05,		// 5	5865

	0x0299B,		// 6 	5845
	0x02991,		// 7	5825
	0x02987,		// 8	5805
	0x0291D,		// 9	5785
	0x02913,		// 10	5765

	0x02909,		// 11	5745
	0x0289F,		// 12	5725
	0x02895,		// 13	5705
	0x0288B,		// 14	5685
	0x02881,		// 15	5665

	0x02817,		// 16  	5645
	0x0280D,		// 17	5625
	0x02803,		// 18	5605
	0x02799,		// 19 	5585
	0x0278F,		// 20	5565

	0x02785,		// 21	5545
	0x0271B,		// 22	5525
	0x02711,		// 23 	5505
	0x02707,		// 24	5485
	0x0269D,		// 25 	5465

	0x02693,		// 26	5445
	0x02689,		// 27	5425
	0x0261F,		// 28 	5405
	0x02615,		// 29	5385
	0x0260B,		// 30	5365
	0x02601,		// 31	5345
	0x02597,		// 32	5325
	
};
#else
const __u32  chanel_freq[DV_FREQ_CNT]=
{
//	0x02817,// 1							5645
	0x0241f,								//5150
	//0x0281d,// 2							5658
//	0x02881,// 3							5665
//	0x0288b,// 4							5685
	0x02489,								//5170	
	//0x02890,// 5							5695
//	0x02895,// 6							5705
//	0x0289f,// 7							5725
	0x02493,								//5190	
	//0x02902,// 8							5732
//	0x02903,// 9							5733
//	0x02906,// 10							5740
//	0x02909,// 11							5745
//	0x0290c,// 12							5752
//	0x02910,// 13							5760
//	0x02913,// 14							5765
	0x02498,								//5200	
	//0x02915,// 15							5769
//	0x02916,// 16							5771
//	0x0291a,// 17							5780
//	0x0291d,// 18							5785
//	0x0291f,// 19							5790
//	0x02984,// 20							5800
//	0x02987,// 21							5805
	0x0249d,								//5210
	//0x02987,// 22							5806
//	0x02989,// 23							5809
//	0x0298e,// 24							5820
//	0x02991,// 25							5825
//	0x02992,// 26							5828
//	0x02998,// 27							5840
	0x02507,								//5230
	//0x0299a,// 28							5843
//	0x0299b,// 29							5845
//	0x0299c,// 30							5847
//	0x02a02,// 31							5860
//	0x02a05,// 32							5865
//	0x02a05,// 33							5866
	0x0250c,								//5240
	//0x02a0c,// 34							5880
//	0x02a0f,// 35							5885
//	0x02a19,// 36							5905
	0x02511,								//5250
	//0x02a1f,// 37							5917
//	0x02a83,// 38							5925
//	0x02a8d,// 39							5945

};
/*{
  0x0281D,    //5657
  0x02886,    //5676
  0x02890,    //5695
  0x02899,    //5714
  0x02903,    //5733
  0x0290C,    //5752
  0x02916,    //5771
  0x0291F,    //5790 
  0x02989,    //5809
  0x02992,    //5828
  0x0299C,    //5847
  0x02A05,    //5866
  0x02A0F,    //5885
  0x02A18,    //5904
  0x02A82,    //5923 
  0x02A8B,    //5942
};
*/
//{
//	((84<<7)|5 ),// 1		5865
//	((83<<7)|27),// 6		5845
//	((83<<7)|17),// 11		5825
//	((83<<7)|7 ),// 16		5805
//	((82<<7)|29),// 21		5785
//	((82<<7)|19),// 26		5765
//	((82<<7)|9 ),// 31		5745
//	((81<<7)|31),// 36		5725

//	
//	((82<<7)|3 ),// 2		5733
//	((82<<7)|12),// 7		5752
//	((82<<7)|22),// 12		5771
//	((82<<7)|31),// 17		5790
//	((83<<7)|9 ),// 22		5809
//	((83<<7)|18),// 27		5828
//	((83<<7)|28),// 32		5847
//	((84<<7)|5 ),// 37		5866
//	
//	((81<<7)|21),// 3		5705
//	((81<<7)|11),// 8		5685
//	((81<<7)|1 ),// 13		5665
//	((80<<7)|23),// 18		5645
//	((84<<7)|15),// 23		5885
//	((84<<7)|25),// 28		5905
//	((85<<7)|3 ),// 33		5925
//	((85<<7)|13),// 38		5945
//	
//	((82<<7)|6 ),// 4		5740
//	((82<<7)|16),// 9		5760
//	((82<<7)|26),// 14		5780
//	((83<<7)|4 ),// 19		5800
//	((83<<7)|14),// 24		5820
//	((83<<7)|24),// 29		5840
//	((84<<7)|2 ),// 34		5860
//	((84<<7)|12),// 39		5880
//	
//	0x0281D,// 5		5658
//	0x02890,// 10		5695
//	0x02902,// 15		5732
//	0x02915,// 20		5769
//	0x02987,// 25		5806
//	0x0299A,// 30		5843
//	0x02A0C,// 35		5880
//	0x02A1F,// 40		5917
//};

//{
//	((84<<7)|5 ),// 1		5865
//	((82<<7)|3 ),// 2		5733
//	((81<<7)|21),// 3		5705
//	((82<<7)|6 ),// 4		5740
//	0x0281D,// 5 		5658

//	((83<<7)|27),// 6		5845
//	((82<<7)|12),// 7		5752
//	((81<<7)|11),// 8		5685
//	((82<<7)|16),// 9 		5760
//	0x02890,// 10		5695

//	((83<<7)|17),// 11		5825
//	((82<<7)|22),// 12		5771
//	((81<<7)|1 ),// 13		5665
//	((82<<7)|26),// 14		5780
//	0x02902,// 15		5732

//	((83<<7)|7 ),// 16		5805
//	((82<<7)|31),// 17		5790
//	((80<<7)|23),// 18		5645
//	((83<<7)|4 ),// 19		5800
//	0x02915,// 20		5769

//	((82<<7)|29),// 21		5785
//	((83<<7)|9 ),// 22		5809
//	((84<<7)|15),// 23		5885
//	((83<<7)|14),// 24		5820
//	0x02987,// 25		5806

//	((82<<7)|19),// 26		5765
//	((83<<7)|18),// 27		5828
//	((84<<7)|25),// 28		5905
//	((83<<7)|24),// 29		5840
//	0x0299A,// 30		5843

//	((82<<7)|9 ),// 31		5745
//	((83<<7)|28),// 32		5847
//	((85<<7)|3 ),// 33		5925
//	((84<<7)|2 ),// 34		5860
//	0x02A0C,// 35		5880

//	((81<<7)|31),// 36		5725
//	((84<<7)|5 ),// 37		5866
//	((85<<7)|13),// 38		5945
//	((84<<7)|12),// 39		5880
//	0x02A1F,// 40		5917
//	
//};

#endif

__u8  spi_write_chanelfreq(__u8 chanel)
{

	unsigned long    freq;
	__u8     i;
	Spi_ClrClkpin();
	Spi_ClrDatepin();
	Spi_clrCsnpin();
	Spi_Nop();
	freq=chanel_freq[chanel];
	
	freq <<= 5;
	freq |= 0x011;

	for(i=0;i<25;i++)
	{
		Spi_ClrClkpin();
		Spi_Nop();
	    if(freq & 0x01)
    	{
			Spi_SetDatepin();
    	}
		else
		{
			Spi_ClrDatepin();
		}
		freq >>= 1;
		Spi_Nop();
		Spi_SetClkpin();
		Spi_Nop();
	}

	Spi_ClrClkpin();
	Spi_ClrDatepin();
	Spi_SetCsnpin();
	return 1;
}

__u8 Search_Freq_Pro(void)
{
 	__u8 Ch=1;  

	while(Ch<=32)
	{
		spi_write_chanelfreq(Ch);
		esKRNL_TimeDly(35);
		Ch++;
	}
	
	return 1;
}



void CSI_init_req(void)
{
	__s32 ret;
	user_gpio_set_t  gpio_set[1];
	__msg("CSI_init_req()\n");
	eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));

	ret = esCFG_GetKeyValue("power_para", "csi_set2", (int *)gpio_set, sizeof(user_gpio_set_t)/4);
	if(ret)
	{
	  __msg("fetch DAT_PIN script failed\n");
	  return;
	}
	req_csi_pin.hdl_DAT = esPINS_PinGrpReq(gpio_set, 1);
	if(!req_csi_pin.hdl_DAT)
	{
		__msg("request DAT_PIN failed\n");
	  return;
	}
	else
	{
		ret = esPINS_SetPinPull(req_csi_pin.hdl_DAT, PIN_PULL_UP, NULL);
		if(ret)
		{
			__msg("pull DAT_PIN failed\n");
		}
		esPINS_SetPinIO(req_csi_pin.hdl_DAT, 1, NULL);
	}
	///////
	
	eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
	ret = esCFG_GetKeyValue("power_para", "csi_set1", (int *)gpio_set, sizeof(user_gpio_set_t)/4);
	if(ret)
	{
	  __msg("fetch STB_PIN script failed\n");
	  return;
	}
	req_csi_pin.hdl_cs = esPINS_PinGrpReq(gpio_set, 1);
	if(!req_csi_pin.hdl_cs)
	{
		__msg("request STB_PIN failed\n");
	  return;
	}
	else
	{
		ret = esPINS_SetPinPull(req_csi_pin.hdl_cs, PIN_PULL_UP, NULL);
		if(ret)
		{
			__msg("pull DAT_PIN failed\n");
		}
		esPINS_SetPinIO(req_csi_pin.hdl_cs, 1, NULL);
	}
	//////
	
	eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
	ret = esCFG_GetKeyValue("power_para", "csi_set0", (int *)gpio_set, sizeof(user_gpio_set_t)/4);
	if(ret)
	{
	  __msg("fetch CLK_PIN script failed\n");
	  return;
	}
	req_csi_pin.hdl_CLK = esPINS_PinGrpReq(gpio_set, 1);
	if(!req_csi_pin.hdl_CLK)
	{
		__msg("request CLK_PIN failed\n");
	  return;
	}
	else
	{
		ret = esPINS_SetPinPull(req_csi_pin.hdl_CLK, PIN_PULL_UP, NULL);
		if(ret)
		{
			__msg("pull CLK_PIN failed\n");
		}
		esPINS_SetPinIO(req_csi_pin.hdl_CLK, 1, NULL);
	}
	
}

void CSI_exit_req(void)
{
	if(req_csi_pin.hdl_DAT)
	{
		esPINS_PinGrpRel(req_csi_pin.hdl_DAT, 0);
	}
	if(req_csi_pin.hdl_cs)
	{
		esPINS_PinGrpRel(req_csi_pin.hdl_cs, 0);
	}
	if(req_csi_pin.hdl_CLK)
	{
		esPINS_PinGrpRel(req_csi_pin.hdl_CLK, 0);
	}
}





