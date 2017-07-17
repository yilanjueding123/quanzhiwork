
#include  "csi_dev_i.h"

#if(CSI_DEV_TYPE == CSI_DEV_TYPE_S5K4AWFX)

#define MIRROR 1//0 = normal, 1 = mirror
#define FLIP   1//0 = normal, 1 = flip

//每个device私有特性的配置都在此文件完成
//包括reset pin，power down pin的操作
//mclk的配置
//各个模式下的i2c配置以及对应的csi配置
//思路如下：
//通常sensor的设置有
//1.初始化
//2.从任何模式到模式A
//3.从任何模式到模式B
//...
//因此定义几个参数表：reg_init， reg_A，reg_B，...
//在csi_dev_init函数中一次性配置reg_init
//在需要的时候可以多次配置reg_A或reg_B...

//==================全局变量dev_csi.c要调用========================
//每个模组必须要有这些参数
__bool              bProgressiveSrc = 1;   // Indicating the source is progressive or not
__bool              bTopFieldFirst = 0;    // VPO should check this flag when bProgressiveSrc is FALSE
__u16               eAspectRatio = 1000;     //the source picture aspect ratio

__u32 csi_mclk = 27; //MHz

__csi_mode_t csi_mode;//当前使用模式
__csi_mode_all_t csi_mode_all;//支持的模式集
__csi_conf_t conf;//csi配置参数

__csi_color_effect_t csi_color_effect = CSI_COLOR_EFFECT_NORMAL;   	//当前的csi_color_effect
__csi_awb_t          csi_awb 					= CSI_AWB_AUTO;   						//当前的csi_awb
__csi_ae_t					 csi_ae 					= CSI_AE_0;   								//当前的csi_ae
__csi_bright_t			 csi_bright 			= CSI_BRIGHT_0;   						//当前的csi_bright
__csi_contrast_t		 csi_contrast 		= CSI_CONTRAST_0;   					//当前的csi_contrast
__csi_band_t				 csi_band 				= CSI_BAND_50HZ;   						//当前的csi_band
//==================================================================================

extern ES_FILE   *fiic;//i2c句柄


//==================非全局变量仅在此文件中用到========================
__u32  last_mode_num = 0xff;
__buf_scrab_mode_t  last_buf_mode = CSI_SC_NA;//区分单双csi工作状态，last_buf_mode为双，切换不reset power，单->单不reset，单->双reset一次

//static  __u8 da = 0x5a>>1;//0x5a>>1;//0x78>>1;//设备地址，为i2c写地址左移 1 bit
static  __u8 da = 0x78 >> 1; //0x5a>>1;//0x78>>1;//设备地址，为i2c写地址左移 1 bit
static __u32 reg_step = 3; //一次写操作所包含的寄存器地址+寄存器值的byte数

//static __u16 reg_init[]=//just for test
//{
//16,0x0010,0x0001,	// Reset
//16,0x1030,0x0000,	// Clear host interrupt so main will wait
//16,0x0014,0x0001,	// ARM go
//00,0xffff,100,	// Wait100mSec
//32,0x0F12,0x3488,0x7654,
//16,0x0028,0x89ab,
//};


static __u16 reg_init__[] =
{
    16, 0x0010, 0x0001,	// Reset
    16, 0x1030, 0x0000,	// Clear host interrupt so main will wait
    16, 0x0014, 0x0001,	// ARM go
    00, 0xffff, 100,	// Wait100mSec
    // Start T&P part
    //0028	7000	// start add MSW
    //002A	0000	// start add LSW
    // End T&P part

    // Start tuning part

    //	Write memory example:
    //	m70003000 10CE
    //	Write HW register ex2ample:
    //	1004 5678
    // EVT1 new Analog settings

    16, 0xF418, 0x0007,	//ATOP sram internal delay
    //F402	001F	//aig_ana_misc_aps

    16, 0x0028, 0x7000, //0x0028: maybe base addr reg
    16, 0x002A, 0x0FE0, //0x002a: offset addr reg
    16, 0x0F12, 0x001E, //0x0f12: data reg
    16, 0x002A, 0x0FC4,
    16, 0x0F12, 0x6447,
    16, 0x002A, 0x0FFC,
    16, 0x0F12, 0x0002,
    16, 0x002A, 0x0FC8,
    16, 0x0F12, 0x012E,
    16, 0x002A, 0x0FD0,
    16, 0x0F12, 0x0030,
    16, 0x002A, 0x0FEE,
    16, 0x0F12, 0x028A,

    //msm calibration
    16, 0x002A, 0x1004,
    16, 0x0F12, 0x0000,
    16, 0x0F12, 0x0000,
    16, 0x0F12, 0x0000,
    16, 0x0F12, 0x0000,
    16, 0x0F12, 0x0000,
    16, 0x0F12, 0x0000,
    16, 0x0F12, 0x0000,
    16, 0x0F12, 0x0000,

    16, 0x002A, 0x101C,
    16, 0x0F12, 0x1000,
    16, 0x0F12, 0x1000,
    16, 0x0F12, 0x1000,
    16, 0x0F12, 0x1000,
    16, 0x0F12, 0x1000,
    16, 0x0F12, 0x1000,
    16, 0x0F12, 0x1000,
    16, 0x0F12, 0x1000,

    16, 0x002A, 0x1076,
    16, 0x0F12, 0x0000,
    16, 0x002A, 0x0A34,
    16, 0x0F12, 0x0000,


    //============= TUNE Gains ===========
    16, 0x002A, 0x0444,
    16, 0x0F12, 0x0113,
    16, 0x0F12, 0x0200,
    16, 0x0F12, 0x0400,
    16, 0x002A, 0x0434,
    16, 0x0F12, 0x3390,	// 68B0 67ms; 3390 33ms
    16, 0x002A, 0x0438,
    16, 0x0F12, 0x6720,	// 68B0 67ms; 3390 33ms
    16, 0x002A, 0x044A,
    16, 0x0F12, 0x1000,

    ////End TUNE Gains
    16, 0x002A, 0x042A,
    16, 0x0F12, 0x0C80,
    16, 0x0F12, 0x0BB8,

    //============ Pointers to Tuning tables GAS / Base CCM / OutDoor CCM ==============

    16, 0x002A, 0x0708,
    16, 0x0F12, 0x3488,
    16, 0x0F12, 0x7000,	//#TVAR_ash_pGAS
    16, 0x002A, 0x0698,
    16, 0x0F12, 0x33B0,
    16, 0x0F12, 0x7000,	//#TVAR_wbt_pBaseCcms
    16, 0x002A, 0x06A0,
    16, 0x0F12, 0x338C,
    16, 0x0F12, 0x7000,	//#TVAR_wbt_pOutdoorCcm
    //////////////////////////////////
    // ASH Calibration
    //////////////////////////////////
    //	param_start	TVAR_ash_GASAlpha
    16, 0x002A, 0x06C6,
    16, 0x0F12, 0x00B2,	//TVAR_ash_GASAlpha[0]
    16, 0x0F12, 0x00B8,	//TVAR_ash_GASAlpha[1]
    16, 0x0F12, 0x00B7,	//TVAR_ash_GASAlpha[2]
    16, 0x0F12, 0x0094,	//TVAR_ash_GASAlpha[3]
    16, 0x0F12, 0x00B2,	//TVAR_ash_GASAlpha[4]
    16, 0x0F12, 0x00B8,	//TVAR_ash_GASAlpha[5]
    16, 0x0F12, 0x00B7,	//TVAR_ash_GASAlpha[6]
    16, 0x0F12, 0x0094,	//TVAR_ash_GASAlpha[7]
    16, 0x0F12, 0x00B2,	//TVAR_ash_GASAlpha[8]
    16, 0x0F12, 0x00B8,	//TVAR_ash_GASAlpha[9]
    16, 0x0F12, 0x00B7,	//TVAR_ash_GASAlpha[10]
    16, 0x0F12, 0x0094,	//TVAR_ash_GASAlpha[11]
    16, 0x0F12, 0x00AA,	//TVAR_ash_GASAlpha[12]
    16, 0x0F12, 0x00B3,	//TVAR_ash_GASAlpha[13]
    16, 0x0F12, 0x00B2,	//TVAR_ash_GASAlpha[14]
    16, 0x0F12, 0x0095,	//TVAR_ash_GASAlpha[15]
    16, 0x0F12, 0x011C,	//TVAR_ash_GASAlpha[16]
    16, 0x0F12, 0x0102,	//TVAR_ash_GASAlpha[17]
    16, 0x0F12, 0x0101,	//TVAR_ash_GASAlpha[18]
    16, 0x0F12, 0x00F9,	//TVAR_ash_GASAlpha[19]
    16, 0x0F12, 0x013A,	//TVAR_ash_GASAlpha[20]
    16, 0x0F12, 0x0123,	//TVAR_ash_GASAlpha[21]
    16, 0x0F12, 0x0126,	//TVAR_ash_GASAlpha[22]
    16, 0x0F12, 0x0129,	//TVAR_ash_GASAlpha[23]
    16, 0x0F12, 0x012C,	//TVAR_ash_GASAlpha[24]
    16, 0x0F12, 0x0116,	//TVAR_ash_GASAlpha[25]
    16, 0x0F12, 0x0117,	//TVAR_ash_GASAlpha[26]
    16, 0x0F12, 0x011B,	//TVAR_ash_GASAlpha[27]
    //	param_end	TVAR_ash_GASAlpha
    //	param_start	TVAR_ash_GASOutdoorAlpha
    16, 0x0F12, 0x012C,	//TVAR_ash_GASOutdoorAlpha[0]
    16, 0x0F12, 0x0116,	//TVAR_ash_GASOutdoorAlpha[1]
    16, 0x0F12, 0x0117,	//TVAR_ash_GASOutdoorAlpha[2]
    16, 0x0F12, 0x011B,	//TVAR_ash_GASOutdoorAlpha[3]
    //	param_end	TVAR_ash_GASOutdoorAlpha
    16, 0x002A, 0x070E,
    16, 0x0F12, 0x0001,	//ash_bParabolicEstimation
    16, 0x0F12, 0x02B1,	//ash_uParabolicCenterX
    16, 0x0F12, 0x019E,	//ash_uParabolicCenterY
    16, 0x0F12, 0x000D,	//ash_uParabolicScalingA
    16, 0x0F12, 0x000F,	//ash_uParabolicScalingB
    //	param_start	TVAR_ash_pGAS
    16, 0x002A, 0x3488,
    16, 0x0F12, 0x0046,	//TVAR_ash_pGAS[0]
    16, 0x0F12, 0x0041,	//TVAR_ash_pGAS[1]
    16, 0x0F12, 0x004F,	//TVAR_ash_pGAS[2]
    16, 0x0F12, 0x0057,	//TVAR_ash_pGAS[3]
    16, 0x0F12, 0x005A,	//TVAR_ash_pGAS[4]
    16, 0x0F12, 0x0054,	//TVAR_ash_pGAS[5]
    16, 0x0F12, 0x004E,	//TVAR_ash_pGAS[6]
    16, 0x0F12, 0x0047,	//TVAR_ash_pGAS[7]
    16, 0x0F12, 0x003F,	//TVAR_ash_pGAS[8]
    16, 0x0F12, 0x0036,	//TVAR_ash_pGAS[9]
    16, 0x0F12, 0x002B,	//TVAR_ash_pGAS[10]
    16, 0x0F12, 0x0026,	//TVAR_ash_pGAS[11]
    16, 0x0F12, 0x002D,	//TVAR_ash_pGAS[12]
    16, 0x0F12, 0x0039,	//TVAR_ash_pGAS[13]
    16, 0x0F12, 0x003D,	//TVAR_ash_pGAS[14]
    16, 0x0F12, 0x004B,	//TVAR_ash_pGAS[15]
    16, 0x0F12, 0x0051,	//TVAR_ash_pGAS[16]
    16, 0x0F12, 0x004C,	//TVAR_ash_pGAS[17]
    16, 0x0F12, 0x003F,	//TVAR_ash_pGAS[18]
    16, 0x0F12, 0x0036,	//TVAR_ash_pGAS[19]
    16, 0x0F12, 0x0033,	//TVAR_ash_pGAS[20]
    16, 0x0F12, 0x0034,	//TVAR_ash_pGAS[21]
    16, 0x0F12, 0x0031,	//TVAR_ash_pGAS[22]
    16, 0x0F12, 0x002A,	//TVAR_ash_pGAS[23]
    16, 0x0F12, 0x0023,	//TVAR_ash_pGAS[24]
    16, 0x0F12, 0x0022,	//TVAR_ash_pGAS[25]
    16, 0x0F12, 0x0030,	//TVAR_ash_pGAS[26]
    16, 0x0F12, 0x003C,	//TVAR_ash_pGAS[27]
    16, 0x0F12, 0x0047,	//TVAR_ash_pGAS[28]
    16, 0x0F12, 0x0042,	//TVAR_ash_pGAS[29]
    16, 0x0F12, 0x0031,	//TVAR_ash_pGAS[30]
    16, 0x0F12, 0x001F,	//TVAR_ash_pGAS[31]
    16, 0x0F12, 0x0017,	//TVAR_ash_pGAS[32]
    16, 0x0F12, 0x0017,	//TVAR_ash_pGAS[33]
    16, 0x0F12, 0x001E,	//TVAR_ash_pGAS[34]
    16, 0x0F12, 0x0029,	//TVAR_ash_pGAS[35]
    16, 0x0F12, 0x002D,	//TVAR_ash_pGAS[36]
    16, 0x0F12, 0x0026,	//TVAR_ash_pGAS[37]
    16, 0x0F12, 0x001E,	//TVAR_ash_pGAS[38]
    16, 0x0F12, 0x0030,	//TVAR_ash_pGAS[39]
    16, 0x0F12, 0x0040,	//TVAR_ash_pGAS[40]
    16, 0x0F12, 0x0042,	//TVAR_ash_pGAS[41]
    16, 0x0F12, 0x0030,	//TVAR_ash_pGAS[42]
    16, 0x0F12, 0x001B,	//TVAR_ash_pGAS[43]
    16, 0x0F12, 0x000C,	//TVAR_ash_pGAS[44]
    16, 0x0F12, 0x0006,	//TVAR_ash_pGAS[45]
    16, 0x0F12, 0x0007,	//TVAR_ash_pGAS[46]
    16, 0x0F12, 0x000F,	//TVAR_ash_pGAS[47]
    16, 0x0F12, 0x0020,	//TVAR_ash_pGAS[48]
    16, 0x0F12, 0x002D,	//TVAR_ash_pGAS[49]
    16, 0x0F12, 0x002C,	//TVAR_ash_pGAS[50]
    16, 0x0F12, 0x0023,	//TVAR_ash_pGAS[51]
    16, 0x0F12, 0x0037,	//TVAR_ash_pGAS[52]
    16, 0x0F12, 0x0047,	//TVAR_ash_pGAS[53]
    16, 0x0F12, 0x003F,	//TVAR_ash_pGAS[54]
    16, 0x0F12, 0x0025,	//TVAR_ash_pGAS[55]
    16, 0x0F12, 0x000F,	//TVAR_ash_pGAS[56]
    16, 0x0F12, 0x0004,	//TVAR_ash_pGAS[57]
    16, 0x0F12, 0x0000,	//TVAR_ash_pGAS[58]
    16, 0x0F12, 0x0001,	//TVAR_ash_pGAS[59]
    16, 0x0F12, 0x000A,	//TVAR_ash_pGAS[60]
    16, 0x0F12, 0x001C,	//TVAR_ash_pGAS[61]
    16, 0x0F12, 0x002F,	//TVAR_ash_pGAS[62]
    16, 0x0F12, 0x0034,	//TVAR_ash_pGAS[63]
    16, 0x0F12, 0x002A,	//TVAR_ash_pGAS[64]
    16, 0x0F12, 0x003C,	//TVAR_ash_pGAS[65]
    16, 0x0F12, 0x0050,	//TVAR_ash_pGAS[66]
    16, 0x0F12, 0x0040,	//TVAR_ash_pGAS[67]
    16, 0x0F12, 0x001F,	//TVAR_ash_pGAS[68]
    16, 0x0F12, 0x000A,	//TVAR_ash_pGAS[69]
    16, 0x0F12, 0x0000,	//TVAR_ash_pGAS[70]
    16, 0x0F12, 0x0000,	//TVAR_ash_pGAS[71]
    16, 0x0F12, 0x0000,	//TVAR_ash_pGAS[72]
    16, 0x0F12, 0x000A,	//TVAR_ash_pGAS[73]
    16, 0x0F12, 0x001D,	//TVAR_ash_pGAS[74]
    16, 0x0F12, 0x0033,	//TVAR_ash_pGAS[75]
    16, 0x0F12, 0x003A,	//TVAR_ash_pGAS[76]
    16, 0x0F12, 0x0033,	//TVAR_ash_pGAS[77]
    16, 0x0F12, 0x0047,	//TVAR_ash_pGAS[78]
    16, 0x0F12, 0x0056,	//TVAR_ash_pGAS[79]
    16, 0x0F12, 0x0044,	//TVAR_ash_pGAS[80]
    16, 0x0F12, 0x0027,	//TVAR_ash_pGAS[81]
    16, 0x0F12, 0x000F,	//TVAR_ash_pGAS[82]
    16, 0x0F12, 0x0004,	//TVAR_ash_pGAS[83]
    16, 0x0F12, 0x0002,	//TVAR_ash_pGAS[84]
    16, 0x0F12, 0x0005,	//TVAR_ash_pGAS[85]
    16, 0x0F12, 0x000F,	//TVAR_ash_pGAS[86]
    16, 0x0F12, 0x0024,	//TVAR_ash_pGAS[87]
    16, 0x0F12, 0x0038,	//TVAR_ash_pGAS[88]
    16, 0x0F12, 0x0042,	//TVAR_ash_pGAS[89]
    16, 0x0F12, 0x003E,	//TVAR_ash_pGAS[90]
    16, 0x0F12, 0x0056,	//TVAR_ash_pGAS[91]
    16, 0x0F12, 0x005B,	//TVAR_ash_pGAS[92]
    16, 0x0F12, 0x004D,	//TVAR_ash_pGAS[93]
    16, 0x0F12, 0x0033,	//TVAR_ash_pGAS[94]
    16, 0x0F12, 0x001B,	//TVAR_ash_pGAS[95]
    16, 0x0F12, 0x0010,	//TVAR_ash_pGAS[96]
    16, 0x0F12, 0x000E,	//TVAR_ash_pGAS[97]
    16, 0x0F12, 0x0013,	//TVAR_ash_pGAS[98]
    16, 0x0F12, 0x0020,	//TVAR_ash_pGAS[99]
    16, 0x0F12, 0x0030,	//TVAR_ash_pGAS[100]
    16, 0x0F12, 0x0042,	//TVAR_ash_pGAS[101]
    16, 0x0F12, 0x0048,	//TVAR_ash_pGAS[102]
    16, 0x0F12, 0x0048,	//TVAR_ash_pGAS[103]
    16, 0x0F12, 0x0069,	//TVAR_ash_pGAS[104]
    16, 0x0F12, 0x0065,	//TVAR_ash_pGAS[105]
    16, 0x0F12, 0x005B,	//TVAR_ash_pGAS[106]
    16, 0x0F12, 0x004F,	//TVAR_ash_pGAS[107]
    16, 0x0F12, 0x003E,	//TVAR_ash_pGAS[108]
    16, 0x0F12, 0x0036,	//TVAR_ash_pGAS[109]
    16, 0x0F12, 0x0035,	//TVAR_ash_pGAS[110]
    16, 0x0F12, 0x0038,	//TVAR_ash_pGAS[111]
    16, 0x0F12, 0x0043,	//TVAR_ash_pGAS[112]
    16, 0x0F12, 0x0051,	//TVAR_ash_pGAS[113]
    16, 0x0F12, 0x0056,	//TVAR_ash_pGAS[114]
    16, 0x0F12, 0x0052,	//TVAR_ash_pGAS[115]
    16, 0x0F12, 0x0058,	//TVAR_ash_pGAS[116]
    16, 0x0F12, 0x0090,	//TVAR_ash_pGAS[117]
    16, 0x0F12, 0x0074,	//TVAR_ash_pGAS[118]
    16, 0x0F12, 0x0065,	//TVAR_ash_pGAS[119]
    16, 0x0F12, 0x006B,	//TVAR_ash_pGAS[120]
    16, 0x0F12, 0x006F,	//TVAR_ash_pGAS[121]
    16, 0x0F12, 0x0070,	//TVAR_ash_pGAS[122]
    16, 0x0F12, 0x0077,	//TVAR_ash_pGAS[123]
    16, 0x0F12, 0x0073,	//TVAR_ash_pGAS[124]
    16, 0x0F12, 0x0076,	//TVAR_ash_pGAS[125]
    16, 0x0F12, 0x007A,	//TVAR_ash_pGAS[126]
    16, 0x0F12, 0x006D,	//TVAR_ash_pGAS[127]
    16, 0x0F12, 0x005F,	//TVAR_ash_pGAS[128]
    16, 0x0F12, 0x0077,	//TVAR_ash_pGAS[129]
    16, 0x0F12, 0x00D4,	//TVAR_ash_pGAS[130]
    16, 0x0F12, 0x009B,	//TVAR_ash_pGAS[131]
    16, 0x0F12, 0x007F,	//TVAR_ash_pGAS[132]
    16, 0x0F12, 0x0082,	//TVAR_ash_pGAS[133]
    16, 0x0F12, 0x0093,	//TVAR_ash_pGAS[134]
    16, 0x0F12, 0x009F,	//TVAR_ash_pGAS[135]
    16, 0x0F12, 0x00A9,	//TVAR_ash_pGAS[136]
    16, 0x0F12, 0x00A3,	//TVAR_ash_pGAS[137]
    16, 0x0F12, 0x00A1,	//TVAR_ash_pGAS[138]
    16, 0x0F12, 0x0098,	//TVAR_ash_pGAS[139]
    16, 0x0F12, 0x0082,	//TVAR_ash_pGAS[140]
    16, 0x0F12, 0x007E,	//TVAR_ash_pGAS[141]
    16, 0x0F12, 0x00A7,	//TVAR_ash_pGAS[142]
    16, 0x0F12, 0x0035,	//TVAR_ash_pGAS[143]
    16, 0x0F12, 0x0037,	//TVAR_ash_pGAS[144]
    16, 0x0F12, 0x004E,	//TVAR_ash_pGAS[145]
    16, 0x0F12, 0x005C,	//TVAR_ash_pGAS[146]
    16, 0x0F12, 0x0062,	//TVAR_ash_pGAS[147]
    16, 0x0F12, 0x0061,	//TVAR_ash_pGAS[148]
    16, 0x0F12, 0x0059,	//TVAR_ash_pGAS[149]
    16, 0x0F12, 0x004E,	//TVAR_ash_pGAS[150]
    16, 0x0F12, 0x0043,	//TVAR_ash_pGAS[151]
    16, 0x0F12, 0x0035,	//TVAR_ash_pGAS[152]
    16, 0x0F12, 0x0025,	//TVAR_ash_pGAS[153]
    16, 0x0F12, 0x001A,	//TVAR_ash_pGAS[154]
    16, 0x0F12, 0x001E,	//TVAR_ash_pGAS[155]
    16, 0x0F12, 0x002E,	//TVAR_ash_pGAS[156]
    16, 0x0F12, 0x0038,	//TVAR_ash_pGAS[157]
    16, 0x0F12, 0x004E,	//TVAR_ash_pGAS[158]
    16, 0x0F12, 0x0058,	//TVAR_ash_pGAS[159]
    16, 0x0F12, 0x0055,	//TVAR_ash_pGAS[160]
    16, 0x0F12, 0x0049,	//TVAR_ash_pGAS[161]
    16, 0x0F12, 0x003F,	//TVAR_ash_pGAS[162]
    16, 0x0F12, 0x0039,	//TVAR_ash_pGAS[163]
    16, 0x0F12, 0x0039,	//TVAR_ash_pGAS[164]
    16, 0x0F12, 0x0032,	//TVAR_ash_pGAS[165]
    16, 0x0F12, 0x0029,	//TVAR_ash_pGAS[166]
    16, 0x0F12, 0x001A,	//TVAR_ash_pGAS[167]
    16, 0x0F12, 0x0015,	//TVAR_ash_pGAS[168]
    16, 0x0F12, 0x0029,	//TVAR_ash_pGAS[169]
    16, 0x0F12, 0x003C,	//TVAR_ash_pGAS[170]
    16, 0x0F12, 0x004D,	//TVAR_ash_pGAS[171]
    16, 0x0F12, 0x004A,	//TVAR_ash_pGAS[172]
    16, 0x0F12, 0x0038,	//TVAR_ash_pGAS[173]
    16, 0x0F12, 0x0026,	//TVAR_ash_pGAS[174]
    16, 0x0F12, 0x001D,	//TVAR_ash_pGAS[175]
    16, 0x0F12, 0x001A,	//TVAR_ash_pGAS[176]
    16, 0x0F12, 0x0021,	//TVAR_ash_pGAS[177]
    16, 0x0F12, 0x002C,	//TVAR_ash_pGAS[178]
    16, 0x0F12, 0x002E,	//TVAR_ash_pGAS[179]
    16, 0x0F12, 0x0022,	//TVAR_ash_pGAS[180]
    16, 0x0F12, 0x0016,	//TVAR_ash_pGAS[181]
    16, 0x0F12, 0x002E,	//TVAR_ash_pGAS[182]
    16, 0x0F12, 0x0045,	//TVAR_ash_pGAS[183]
    16, 0x0F12, 0x004B,	//TVAR_ash_pGAS[184]
    16, 0x0F12, 0x0037,	//TVAR_ash_pGAS[185]
    16, 0x0F12, 0x001F,	//TVAR_ash_pGAS[186]
    16, 0x0F12, 0x0010,	//TVAR_ash_pGAS[187]
    16, 0x0F12, 0x0008,	//TVAR_ash_pGAS[188]
    16, 0x0F12, 0x0009,	//TVAR_ash_pGAS[189]
    16, 0x0F12, 0x0010,	//TVAR_ash_pGAS[190]
    16, 0x0F12, 0x0023,	//TVAR_ash_pGAS[191]
    16, 0x0F12, 0x0030,	//TVAR_ash_pGAS[192]
    16, 0x0F12, 0x002B,	//TVAR_ash_pGAS[193]
    16, 0x0F12, 0x001D,	//TVAR_ash_pGAS[194]
    16, 0x0F12, 0x0038,	//TVAR_ash_pGAS[195]
    16, 0x0F12, 0x004E,	//TVAR_ash_pGAS[196]
    16, 0x0F12, 0x0049,	//TVAR_ash_pGAS[197]
    16, 0x0F12, 0x002B,	//TVAR_ash_pGAS[198]
    16, 0x0F12, 0x0012,	//TVAR_ash_pGAS[199]
    16, 0x0F12, 0x0006,	//TVAR_ash_pGAS[200]
    16, 0x0F12, 0x0001,	//TVAR_ash_pGAS[201]
    16, 0x0F12, 0x0002,	//TVAR_ash_pGAS[202]
    16, 0x0F12, 0x000B,	//TVAR_ash_pGAS[203]
    16, 0x0F12, 0x001F,	//TVAR_ash_pGAS[204]
    16, 0x0F12, 0x0034,	//TVAR_ash_pGAS[205]
    16, 0x0F12, 0x0035,	//TVAR_ash_pGAS[206]
    16, 0x0F12, 0x0028,	//TVAR_ash_pGAS[207]
    16, 0x0F12, 0x003F,	//TVAR_ash_pGAS[208]
    16, 0x0F12, 0x005A,	//TVAR_ash_pGAS[209]
    16, 0x0F12, 0x004C,	//TVAR_ash_pGAS[210]
    16, 0x0F12, 0x0026,	//TVAR_ash_pGAS[211]
    16, 0x0F12, 0x000D,	//TVAR_ash_pGAS[212]
    16, 0x0F12, 0x0002,	//TVAR_ash_pGAS[213]
    16, 0x0F12, 0x0000,	//TVAR_ash_pGAS[214]
    16, 0x0F12, 0x0001,	//TVAR_ash_pGAS[215]
    16, 0x0F12, 0x000C,	//TVAR_ash_pGAS[216]
    16, 0x0F12, 0x0021,	//TVAR_ash_pGAS[217]
    16, 0x0F12, 0x0039,	//TVAR_ash_pGAS[218]
    16, 0x0F12, 0x003E,	//TVAR_ash_pGAS[219]
    16, 0x0F12, 0x0032,	//TVAR_ash_pGAS[220]
    16, 0x0F12, 0x0048,	//TVAR_ash_pGAS[221]
    16, 0x0F12, 0x005F,	//TVAR_ash_pGAS[222]
    16, 0x0F12, 0x004F,	//TVAR_ash_pGAS[223]
    16, 0x0F12, 0x002D,	//TVAR_ash_pGAS[224]
    16, 0x0F12, 0x0011,	//TVAR_ash_pGAS[225]
    16, 0x0F12, 0x0005,	//TVAR_ash_pGAS[226]
    16, 0x0F12, 0x0003,	//TVAR_ash_pGAS[227]
    16, 0x0F12, 0x0006,	//TVAR_ash_pGAS[228]
    16, 0x0F12, 0x0011,	//TVAR_ash_pGAS[229]
    16, 0x0F12, 0x0028,	//TVAR_ash_pGAS[230]
    16, 0x0F12, 0x003F,	//TVAR_ash_pGAS[231]
    16, 0x0F12, 0x0045,	//TVAR_ash_pGAS[232]
    16, 0x0F12, 0x003C,	//TVAR_ash_pGAS[233]
    16, 0x0F12, 0x0058,	//TVAR_ash_pGAS[234]
    16, 0x0F12, 0x0062,	//TVAR_ash_pGAS[235]
    16, 0x0F12, 0x0057,	//TVAR_ash_pGAS[236]
    16, 0x0F12, 0x003A,	//TVAR_ash_pGAS[237]
    16, 0x0F12, 0x001F,	//TVAR_ash_pGAS[238]
    16, 0x0F12, 0x0013,	//TVAR_ash_pGAS[239]
    16, 0x0F12, 0x0011,	//TVAR_ash_pGAS[240]
    16, 0x0F12, 0x0016,	//TVAR_ash_pGAS[241]
    16, 0x0F12, 0x0025,	//TVAR_ash_pGAS[242]
    16, 0x0F12, 0x0037,	//TVAR_ash_pGAS[243]
    16, 0x0F12, 0x0049,	//TVAR_ash_pGAS[244]
    16, 0x0F12, 0x004B,	//TVAR_ash_pGAS[245]
    16, 0x0F12, 0x0046,	//TVAR_ash_pGAS[246]
    16, 0x0F12, 0x0065,	//TVAR_ash_pGAS[247]
    16, 0x0F12, 0x0067,	//TVAR_ash_pGAS[248]
    16, 0x0F12, 0x0061,	//TVAR_ash_pGAS[249]
    16, 0x0F12, 0x0054,	//TVAR_ash_pGAS[250]
    16, 0x0F12, 0x0042,	//TVAR_ash_pGAS[251]
    16, 0x0F12, 0x003B,	//TVAR_ash_pGAS[252]
    16, 0x0F12, 0x003C,	//TVAR_ash_pGAS[253]
    16, 0x0F12, 0x003F,	//TVAR_ash_pGAS[254]
    16, 0x0F12, 0x004B,	//TVAR_ash_pGAS[255]
    16, 0x0F12, 0x005B,	//TVAR_ash_pGAS[256]
    16, 0x0F12, 0x005D,	//TVAR_ash_pGAS[257]
    16, 0x0F12, 0x0052,	//TVAR_ash_pGAS[258]
    16, 0x0F12, 0x0052,	//TVAR_ash_pGAS[259]
    16, 0x0F12, 0x008A,	//TVAR_ash_pGAS[260]
    16, 0x0F12, 0x0072,	//TVAR_ash_pGAS[261]
    16, 0x0F12, 0x0068,	//TVAR_ash_pGAS[262]
    16, 0x0F12, 0x0071,	//TVAR_ash_pGAS[263]
    16, 0x0F12, 0x0075,	//TVAR_ash_pGAS[264]
    16, 0x0F12, 0x007C,	//TVAR_ash_pGAS[265]
    16, 0x0F12, 0x0084,	//TVAR_ash_pGAS[266]
    16, 0x0F12, 0x0082,	//TVAR_ash_pGAS[267]
    16, 0x0F12, 0x0084,	//TVAR_ash_pGAS[268]
    16, 0x0F12, 0x0084,	//TVAR_ash_pGAS[269]
    16, 0x0F12, 0x0071,	//TVAR_ash_pGAS[270]
    16, 0x0F12, 0x005E,	//TVAR_ash_pGAS[271]
    16, 0x0F12, 0x006F,	//TVAR_ash_pGAS[272]
    16, 0x0F12, 0x00CA,	//TVAR_ash_pGAS[273]
    16, 0x0F12, 0x0097,	//TVAR_ash_pGAS[274]
    16, 0x0F12, 0x007E,	//TVAR_ash_pGAS[275]
    16, 0x0F12, 0x0085,	//TVAR_ash_pGAS[276]
    16, 0x0F12, 0x009A,	//TVAR_ash_pGAS[277]
    16, 0x0F12, 0x00AB,	//TVAR_ash_pGAS[278]
    16, 0x0F12, 0x00BC,	//TVAR_ash_pGAS[279]
    16, 0x0F12, 0x00B6,	//TVAR_ash_pGAS[280]
    16, 0x0F12, 0x00AF,	//TVAR_ash_pGAS[281]
    16, 0x0F12, 0x009E,	//TVAR_ash_pGAS[282]
    16, 0x0F12, 0x0081,	//TVAR_ash_pGAS[283]
    16, 0x0F12, 0x0075,	//TVAR_ash_pGAS[284]
    16, 0x0F12, 0x0099,	//TVAR_ash_pGAS[285]
    16, 0x0F12, 0x002D,	//TVAR_ash_pGAS[286]
    16, 0x0F12, 0x002A,	//TVAR_ash_pGAS[287]
    16, 0x0F12, 0x0035,	//TVAR_ash_pGAS[288]
    16, 0x0F12, 0x003D,	//TVAR_ash_pGAS[289]
    16, 0x0F12, 0x0047,	//TVAR_ash_pGAS[290]
    16, 0x0F12, 0x004F,	//TVAR_ash_pGAS[291]
    16, 0x0F12, 0x0052,	//TVAR_ash_pGAS[292]
    16, 0x0F12, 0x004E,	//TVAR_ash_pGAS[293]
    16, 0x0F12, 0x0046,	//TVAR_ash_pGAS[294]
    16, 0x0F12, 0x0037,	//TVAR_ash_pGAS[295]
    16, 0x0F12, 0x0027,	//TVAR_ash_pGAS[296]
    16, 0x0F12, 0x001A,	//TVAR_ash_pGAS[297]
    16, 0x0F12, 0x001E,	//TVAR_ash_pGAS[298]
    16, 0x0F12, 0x0025,	//TVAR_ash_pGAS[299]
    16, 0x0F12, 0x0029,	//TVAR_ash_pGAS[300]
    16, 0x0F12, 0x0035,	//TVAR_ash_pGAS[301]
    16, 0x0F12, 0x003A,	//TVAR_ash_pGAS[302]
    16, 0x0F12, 0x003B,	//TVAR_ash_pGAS[303]
    16, 0x0F12, 0x0039,	//TVAR_ash_pGAS[304]
    16, 0x0F12, 0x0039,	//TVAR_ash_pGAS[305]
    16, 0x0F12, 0x003A,	//TVAR_ash_pGAS[306]
    16, 0x0F12, 0x003B,	//TVAR_ash_pGAS[307]
    16, 0x0F12, 0x0036,	//TVAR_ash_pGAS[308]
    16, 0x0F12, 0x002A,	//TVAR_ash_pGAS[309]
    16, 0x0F12, 0x001B,	//TVAR_ash_pGAS[310]
    16, 0x0F12, 0x0015,	//TVAR_ash_pGAS[311]
    16, 0x0F12, 0x0021,	//TVAR_ash_pGAS[312]
    16, 0x0F12, 0x002E,	//TVAR_ash_pGAS[313]
    16, 0x0F12, 0x0038,	//TVAR_ash_pGAS[314]
    16, 0x0F12, 0x0031,	//TVAR_ash_pGAS[315]
    16, 0x0F12, 0x0024,	//TVAR_ash_pGAS[316]
    16, 0x0F12, 0x001A,	//TVAR_ash_pGAS[317]
    16, 0x0F12, 0x0019,	//TVAR_ash_pGAS[318]
    16, 0x0F12, 0x001B,	//TVAR_ash_pGAS[319]
    16, 0x0F12, 0x0022,	//TVAR_ash_pGAS[320]
    16, 0x0F12, 0x002E,	//TVAR_ash_pGAS[321]
    16, 0x0F12, 0x002F,	//TVAR_ash_pGAS[322]
    16, 0x0F12, 0x0022,	//TVAR_ash_pGAS[323]
    16, 0x0F12, 0x0015,	//TVAR_ash_pGAS[324]
    16, 0x0F12, 0x0027,	//TVAR_ash_pGAS[325]
    16, 0x0F12, 0x003B,	//TVAR_ash_pGAS[326]
    16, 0x0F12, 0x003C,	//TVAR_ash_pGAS[327]
    16, 0x0F12, 0x0029,	//TVAR_ash_pGAS[328]
    16, 0x0F12, 0x0014,	//TVAR_ash_pGAS[329]
    16, 0x0F12, 0x0009,	//TVAR_ash_pGAS[330]
    16, 0x0F12, 0x0005,	//TVAR_ash_pGAS[331]
    16, 0x0F12, 0x0008,	//TVAR_ash_pGAS[332]
    16, 0x0F12, 0x0011,	//TVAR_ash_pGAS[333]
    16, 0x0F12, 0x0023,	//TVAR_ash_pGAS[334]
    16, 0x0F12, 0x002F,	//TVAR_ash_pGAS[335]
    16, 0x0F12, 0x002A,	//TVAR_ash_pGAS[336]
    16, 0x0F12, 0x001B,	//TVAR_ash_pGAS[337]
    16, 0x0F12, 0x0034,	//TVAR_ash_pGAS[338]
    16, 0x0F12, 0x004A,	//TVAR_ash_pGAS[339]
    16, 0x0F12, 0x0043,	//TVAR_ash_pGAS[340]
    16, 0x0F12, 0x0026,	//TVAR_ash_pGAS[341]
    16, 0x0F12, 0x000E,	//TVAR_ash_pGAS[342]
    16, 0x0F12, 0x0003,	//TVAR_ash_pGAS[343]
    16, 0x0F12, 0x0000,	//TVAR_ash_pGAS[344]
    16, 0x0F12, 0x0002,	//TVAR_ash_pGAS[345]
    16, 0x0F12, 0x000B,	//TVAR_ash_pGAS[346]
    16, 0x0F12, 0x001E,	//TVAR_ash_pGAS[347]
    16, 0x0F12, 0x0032,	//TVAR_ash_pGAS[348]
    16, 0x0F12, 0x0033,	//TVAR_ash_pGAS[349]
    16, 0x0F12, 0x0025,	//TVAR_ash_pGAS[350]
    16, 0x0F12, 0x003F,	//TVAR_ash_pGAS[351]
    16, 0x0F12, 0x005B,	//TVAR_ash_pGAS[352]
    16, 0x0F12, 0x004E,	//TVAR_ash_pGAS[353]
    16, 0x0F12, 0x0029,	//TVAR_ash_pGAS[354]
    16, 0x0F12, 0x000F,	//TVAR_ash_pGAS[355]
    16, 0x0F12, 0x0002,	//TVAR_ash_pGAS[356]
    16, 0x0F12, 0x0000,	//TVAR_ash_pGAS[357]
    16, 0x0F12, 0x0001,	//TVAR_ash_pGAS[358]
    16, 0x0F12, 0x000B,	//TVAR_ash_pGAS[359]
    16, 0x0F12, 0x001F,	//TVAR_ash_pGAS[360]
    16, 0x0F12, 0x0036,	//TVAR_ash_pGAS[361]
    16, 0x0F12, 0x003A,	//TVAR_ash_pGAS[362]
    16, 0x0F12, 0x002F,	//TVAR_ash_pGAS[363]
    16, 0x0F12, 0x004D,	//TVAR_ash_pGAS[364]
    16, 0x0F12, 0x0067,	//TVAR_ash_pGAS[365]
    16, 0x0F12, 0x0059,	//TVAR_ash_pGAS[366]
    16, 0x0F12, 0x0038,	//TVAR_ash_pGAS[367]
    16, 0x0F12, 0x001A,	//TVAR_ash_pGAS[368]
    16, 0x0F12, 0x0009,	//TVAR_ash_pGAS[369]
    16, 0x0F12, 0x0004,	//TVAR_ash_pGAS[370]
    16, 0x0F12, 0x0005,	//TVAR_ash_pGAS[371]
    16, 0x0F12, 0x000E,	//TVAR_ash_pGAS[372]
    16, 0x0F12, 0x0024,	//TVAR_ash_pGAS[373]
    16, 0x0F12, 0x0039,	//TVAR_ash_pGAS[374]
    16, 0x0F12, 0x0040,	//TVAR_ash_pGAS[375]
    16, 0x0F12, 0x0037,	//TVAR_ash_pGAS[376]
    16, 0x0F12, 0x0062,	//TVAR_ash_pGAS[377]
    16, 0x0F12, 0x0072,	//TVAR_ash_pGAS[378]
    16, 0x0F12, 0x006C,	//TVAR_ash_pGAS[379]
    16, 0x0F12, 0x004F,	//TVAR_ash_pGAS[380]
    16, 0x0F12, 0x0030,	//TVAR_ash_pGAS[381]
    16, 0x0F12, 0x001B,	//TVAR_ash_pGAS[382]
    16, 0x0F12, 0x0013,	//TVAR_ash_pGAS[383]
    16, 0x0F12, 0x0014,	//TVAR_ash_pGAS[384]
    16, 0x0F12, 0x0020,	//TVAR_ash_pGAS[385]
    16, 0x0F12, 0x002F,	//TVAR_ash_pGAS[386]
    16, 0x0F12, 0x0040,	//TVAR_ash_pGAS[387]
    16, 0x0F12, 0x0043,	//TVAR_ash_pGAS[388]
    16, 0x0F12, 0x003F,	//TVAR_ash_pGAS[389]
    16, 0x0F12, 0x0073,	//TVAR_ash_pGAS[390]
    16, 0x0F12, 0x007E,	//TVAR_ash_pGAS[391]
    16, 0x0F12, 0x0082,	//TVAR_ash_pGAS[392]
    16, 0x0F12, 0x0078,	//TVAR_ash_pGAS[393]
    16, 0x0F12, 0x005F,	//TVAR_ash_pGAS[394]
    16, 0x0F12, 0x004A,	//TVAR_ash_pGAS[395]
    16, 0x0F12, 0x0040,	//TVAR_ash_pGAS[396]
    16, 0x0F12, 0x003C,	//TVAR_ash_pGAS[397]
    16, 0x0F12, 0x0042,	//TVAR_ash_pGAS[398]
    16, 0x0F12, 0x004D,	//TVAR_ash_pGAS[399]
    16, 0x0F12, 0x004F,	//TVAR_ash_pGAS[400]
    16, 0x0F12, 0x0048,	//TVAR_ash_pGAS[401]
    16, 0x0F12, 0x004A,	//TVAR_ash_pGAS[402]
    16, 0x0F12, 0x009A,	//TVAR_ash_pGAS[403]
    16, 0x0F12, 0x008C,	//TVAR_ash_pGAS[404]
    16, 0x0F12, 0x008E,	//TVAR_ash_pGAS[405]
    16, 0x0F12, 0x009D,	//TVAR_ash_pGAS[406]
    16, 0x0F12, 0x009C,	//TVAR_ash_pGAS[407]
    16, 0x0F12, 0x0090,	//TVAR_ash_pGAS[408]
    16, 0x0F12, 0x008A,	//TVAR_ash_pGAS[409]
    16, 0x0F12, 0x007B,	//TVAR_ash_pGAS[410]
    16, 0x0F12, 0x0076,	//TVAR_ash_pGAS[411]
    16, 0x0F12, 0x0072,	//TVAR_ash_pGAS[412]
    16, 0x0F12, 0x0060,	//TVAR_ash_pGAS[413]
    16, 0x0F12, 0x0051,	//TVAR_ash_pGAS[414]
    16, 0x0F12, 0x0066,	//TVAR_ash_pGAS[415]
    16, 0x0F12, 0x00DC,	//TVAR_ash_pGAS[416]
    16, 0x0F12, 0x00AF,	//TVAR_ash_pGAS[417]
    16, 0x0F12, 0x00A4,	//TVAR_ash_pGAS[418]
    16, 0x0F12, 0x00B3,	//TVAR_ash_pGAS[419]
    16, 0x0F12, 0x00C3,	//TVAR_ash_pGAS[420]
    16, 0x0F12, 0x00C2,	//TVAR_ash_pGAS[421]
    16, 0x0F12, 0x00C3,	//TVAR_ash_pGAS[422]
    16, 0x0F12, 0x00B1,	//TVAR_ash_pGAS[423]
    16, 0x0F12, 0x00A1,	//TVAR_ash_pGAS[424]
    16, 0x0F12, 0x008D,	//TVAR_ash_pGAS[425]
    16, 0x0F12, 0x0072,	//TVAR_ash_pGAS[426]
    16, 0x0F12, 0x006B,	//TVAR_ash_pGAS[427]
    16, 0x0F12, 0x0090,	//TVAR_ash_pGAS[428]
    16, 0x0F12, 0x0012,	//TVAR_ash_pGAS[429]
    16, 0x0F12, 0x0011,	//TVAR_ash_pGAS[430]
    16, 0x0F12, 0x001B,	//TVAR_ash_pGAS[431]
    16, 0x0F12, 0x0020,	//TVAR_ash_pGAS[432]
    16, 0x0F12, 0x0028,	//TVAR_ash_pGAS[433]
    16, 0x0F12, 0x002E,	//TVAR_ash_pGAS[434]
    16, 0x0F12, 0x0030,	//TVAR_ash_pGAS[435]
    16, 0x0F12, 0x002B,	//TVAR_ash_pGAS[436]
    16, 0x0F12, 0x0026,	//TVAR_ash_pGAS[437]
    16, 0x0F12, 0x001C,	//TVAR_ash_pGAS[438]
    16, 0x0F12, 0x0010,	//TVAR_ash_pGAS[439]
    16, 0x0F12, 0x0007,	//TVAR_ash_pGAS[440]
    16, 0x0F12, 0x000C,	//TVAR_ash_pGAS[441]
    16, 0x0F12, 0x000C,	//TVAR_ash_pGAS[442]
    16, 0x0F12, 0x0014,	//TVAR_ash_pGAS[443]
    16, 0x0F12, 0x001E,	//TVAR_ash_pGAS[444]
    16, 0x0F12, 0x0022,	//TVAR_ash_pGAS[445]
    16, 0x0F12, 0x0023,	//TVAR_ash_pGAS[446]
    16, 0x0F12, 0x0023,	//TVAR_ash_pGAS[447]
    16, 0x0F12, 0x0022,	//TVAR_ash_pGAS[448]
    16, 0x0F12, 0x0022,	//TVAR_ash_pGAS[449]
    16, 0x0F12, 0x0023,	//TVAR_ash_pGAS[450]
    16, 0x0F12, 0x0020,	//TVAR_ash_pGAS[451]
    16, 0x0F12, 0x0018,	//TVAR_ash_pGAS[452]
    16, 0x0F12, 0x000B,	//TVAR_ash_pGAS[453]
    16, 0x0F12, 0x0006,	//TVAR_ash_pGAS[454]
    16, 0x0F12, 0x000A,	//TVAR_ash_pGAS[455]
    16, 0x0F12, 0x001B,	//TVAR_ash_pGAS[456]
    16, 0x0F12, 0x0023,	//TVAR_ash_pGAS[457]
    16, 0x0F12, 0x001E,	//TVAR_ash_pGAS[458]
    16, 0x0F12, 0x0015,	//TVAR_ash_pGAS[459]
    16, 0x0F12, 0x000E,	//TVAR_ash_pGAS[460]
    16, 0x0F12, 0x000D,	//TVAR_ash_pGAS[461]
    16, 0x0F12, 0x000F,	//TVAR_ash_pGAS[462]
    16, 0x0F12, 0x0015,	//TVAR_ash_pGAS[463]
    16, 0x0F12, 0x001F,	//TVAR_ash_pGAS[464]
    16, 0x0F12, 0x001F,	//TVAR_ash_pGAS[465]
    16, 0x0F12, 0x0013,	//TVAR_ash_pGAS[466]
    16, 0x0F12, 0x0007,	//TVAR_ash_pGAS[467]
    16, 0x0F12, 0x0011,	//TVAR_ash_pGAS[468]
    16, 0x0F12, 0x0027,	//TVAR_ash_pGAS[469]
    16, 0x0F12, 0x0029,	//TVAR_ash_pGAS[470]
    16, 0x0F12, 0x001A,	//TVAR_ash_pGAS[471]
    16, 0x0F12, 0x000B,	//TVAR_ash_pGAS[472]
    16, 0x0F12, 0x0004,	//TVAR_ash_pGAS[473]
    16, 0x0F12, 0x0002,	//TVAR_ash_pGAS[474]
    16, 0x0F12, 0x0004,	//TVAR_ash_pGAS[475]
    16, 0x0F12, 0x000B,	//TVAR_ash_pGAS[476]
    16, 0x0F12, 0x0018,	//TVAR_ash_pGAS[477]
    16, 0x0F12, 0x0022,	//TVAR_ash_pGAS[478]
    16, 0x0F12, 0x001B,	//TVAR_ash_pGAS[479]
    16, 0x0F12, 0x000D,	//TVAR_ash_pGAS[480]
    16, 0x0F12, 0x001E,	//TVAR_ash_pGAS[481]
    16, 0x0F12, 0x0035,	//TVAR_ash_pGAS[482]
    16, 0x0F12, 0x0031,	//TVAR_ash_pGAS[483]
    16, 0x0F12, 0x0019,	//TVAR_ash_pGAS[484]
    16, 0x0F12, 0x0008,	//TVAR_ash_pGAS[485]
    16, 0x0F12, 0x0001,	//TVAR_ash_pGAS[486]
    16, 0x0F12, 0x0000,	//TVAR_ash_pGAS[487]
    16, 0x0F12, 0x0001,	//TVAR_ash_pGAS[488]
    16, 0x0F12, 0x0007,	//TVAR_ash_pGAS[489]
    16, 0x0F12, 0x0016,	//TVAR_ash_pGAS[490]
    16, 0x0F12, 0x0025,	//TVAR_ash_pGAS[491]
    16, 0x0F12, 0x0023,	//TVAR_ash_pGAS[492]
    16, 0x0F12, 0x0016,	//TVAR_ash_pGAS[493]
    16, 0x0F12, 0x0028,	//TVAR_ash_pGAS[494]
    16, 0x0F12, 0x0047,	//TVAR_ash_pGAS[495]
    16, 0x0F12, 0x003C,	//TVAR_ash_pGAS[496]
    16, 0x0F12, 0x001D,	//TVAR_ash_pGAS[497]
    16, 0x0F12, 0x000A,	//TVAR_ash_pGAS[498]
    16, 0x0F12, 0x0001,	//TVAR_ash_pGAS[499]
    16, 0x0F12, 0x0000,	//TVAR_ash_pGAS[500]
    16, 0x0F12, 0x0000,	//TVAR_ash_pGAS[501]
    16, 0x0F12, 0x0008,	//TVAR_ash_pGAS[502]
    16, 0x0F12, 0x0018,	//TVAR_ash_pGAS[503]
    16, 0x0F12, 0x0029,	//TVAR_ash_pGAS[504]
    16, 0x0F12, 0x002A,	//TVAR_ash_pGAS[505]
    16, 0x0F12, 0x001F,	//TVAR_ash_pGAS[506]
    16, 0x0F12, 0x0037,	//TVAR_ash_pGAS[507]
    16, 0x0F12, 0x0052,	//TVAR_ash_pGAS[508]
    16, 0x0F12, 0x0047,	//TVAR_ash_pGAS[509]
    16, 0x0F12, 0x002B,	//TVAR_ash_pGAS[510]
    16, 0x0F12, 0x0013,	//TVAR_ash_pGAS[511]
    16, 0x0F12, 0x0007,	//TVAR_ash_pGAS[512]
    16, 0x0F12, 0x0004,	//TVAR_ash_pGAS[513]
    16, 0x0F12, 0x0004,	//TVAR_ash_pGAS[514]
    16, 0x0F12, 0x000B,	//TVAR_ash_pGAS[515]
    16, 0x0F12, 0x001C,	//TVAR_ash_pGAS[516]
    16, 0x0F12, 0x002B,	//TVAR_ash_pGAS[517]
    16, 0x0F12, 0x002E,	//TVAR_ash_pGAS[518]
    16, 0x0F12, 0x0027,	//TVAR_ash_pGAS[519]
    16, 0x0F12, 0x004C,	//TVAR_ash_pGAS[520]
    16, 0x0F12, 0x005C,	//TVAR_ash_pGAS[521]
    16, 0x0F12, 0x0055,	//TVAR_ash_pGAS[522]
    16, 0x0F12, 0x003D,	//TVAR_ash_pGAS[523]
    16, 0x0F12, 0x0023,	//TVAR_ash_pGAS[524]
    16, 0x0F12, 0x0014,	//TVAR_ash_pGAS[525]
    16, 0x0F12, 0x000E,	//TVAR_ash_pGAS[526]
    16, 0x0F12, 0x000E,	//TVAR_ash_pGAS[527]
    16, 0x0F12, 0x0019,	//TVAR_ash_pGAS[528]
    16, 0x0F12, 0x0024,	//TVAR_ash_pGAS[529]
    16, 0x0F12, 0x0030,	//TVAR_ash_pGAS[530]
    16, 0x0F12, 0x0033,	//TVAR_ash_pGAS[531]
    16, 0x0F12, 0x0030,	//TVAR_ash_pGAS[532]
    16, 0x0F12, 0x005D,	//TVAR_ash_pGAS[533]
    16, 0x0F12, 0x0067,	//TVAR_ash_pGAS[534]
    16, 0x0F12, 0x0066,	//TVAR_ash_pGAS[535]
    16, 0x0F12, 0x005B,	//TVAR_ash_pGAS[536]
    16, 0x0F12, 0x0047,	//TVAR_ash_pGAS[537]
    16, 0x0F12, 0x0037,	//TVAR_ash_pGAS[538]
    16, 0x0F12, 0x0030,	//TVAR_ash_pGAS[539]
    16, 0x0F12, 0x002D,	//TVAR_ash_pGAS[540]
    16, 0x0F12, 0x0032,	//TVAR_ash_pGAS[541]
    16, 0x0F12, 0x003B,	//TVAR_ash_pGAS[542]
    16, 0x0F12, 0x003B,	//TVAR_ash_pGAS[543]
    16, 0x0F12, 0x0036,	//TVAR_ash_pGAS[544]
    16, 0x0F12, 0x003A,	//TVAR_ash_pGAS[545]
    16, 0x0F12, 0x0080,	//TVAR_ash_pGAS[546]
    16, 0x0F12, 0x0073,	//TVAR_ash_pGAS[547]
    16, 0x0F12, 0x006D,	//TVAR_ash_pGAS[548]
    16, 0x0F12, 0x0075,	//TVAR_ash_pGAS[549]
    16, 0x0F12, 0x0073,	//TVAR_ash_pGAS[550]
    16, 0x0F12, 0x006D,	//TVAR_ash_pGAS[551]
    16, 0x0F12, 0x006A,	//TVAR_ash_pGAS[552]
    16, 0x0F12, 0x0060,	//TVAR_ash_pGAS[553]
    16, 0x0F12, 0x005D,	//TVAR_ash_pGAS[554]
    16, 0x0F12, 0x005A,	//TVAR_ash_pGAS[555]
    16, 0x0F12, 0x004C,	//TVAR_ash_pGAS[556]
    16, 0x0F12, 0x0042,	//TVAR_ash_pGAS[557]
    16, 0x0F12, 0x0058,	//TVAR_ash_pGAS[558]
    16, 0x0F12, 0x00B0,	//TVAR_ash_pGAS[559]
    16, 0x0F12, 0x008C,	//TVAR_ash_pGAS[560]
    16, 0x0F12, 0x0078,	//TVAR_ash_pGAS[561]
    16, 0x0F12, 0x007E,	//TVAR_ash_pGAS[562]
    16, 0x0F12, 0x0089,	//TVAR_ash_pGAS[563]
    16, 0x0F12, 0x008A,	//TVAR_ash_pGAS[564]
    16, 0x0F12, 0x008F,	//TVAR_ash_pGAS[565]
    16, 0x0F12, 0x0081,	//TVAR_ash_pGAS[566]
    16, 0x0F12, 0x0077,	//TVAR_ash_pGAS[567]
    16, 0x0F12, 0x0069,	//TVAR_ash_pGAS[568]
    16, 0x0F12, 0x0056,	//TVAR_ash_pGAS[569]
    16, 0x0F12, 0x0055,	//TVAR_ash_pGAS[570]
    16, 0x0F12, 0x007C,	//TVAR_ash_pGAS[571]

    16, 0x002A, 0x06B8,
    16, 0x0F12, 0x00AD,
    16, 0x002A, 0x06BA,
    16, 0x0F12, 0x00D0,
    16, 0x002A, 0x06BC,
    16, 0x0F12, 0x00E0,
    16, 0x002A, 0x06BE,
    16, 0x0F12, 0x012E,
    16, 0x002A, 0x06C0,
    16, 0x0F12, 0x016B,
    16, 0x002A, 0x06C2,
    16, 0x0F12, 0x0195,
    16, 0x002A, 0x06C4,
    16, 0x0F12, 0x01A7,

    //////////////////////////////////////////
    //Illumination Type & Factot Calibration
    //////////////////////////////////////////
    16, 0x002A, 0x0F66,
    16, 0x0F12, 0x00BF,	//SARR_IllumType[0]
    16, 0x0F12, 0x00D9,	//SARR_IllumType[1]
    16, 0x0F12, 0x00F0,	//SARR_IllumType[2]
    16, 0x0F12, 0x012E,	//SARR_IllumType[3]
    16, 0x0F12, 0x0167,	//SARR_IllumType[4]
    16, 0x0F12, 0x018E,	//SARR_IllumType[5]
    16, 0x0F12, 0x01A2,	//SARR_IllumType[6]

    16, 0x0F12, 0x00E6,	//SARR_IllumTypeF[0]
    16, 0x0F12, 0x00E6,	//SARR_IllumTypeF[1]
    16, 0x0F12, 0x00E6,	//SARR_IllumTypeF[2]
    16, 0x0F12, 0x00E6,	//SARR_IllumTypeF[3]
    16, 0x0F12, 0x0100,	//SARR_IllumTypeF[4]
    16, 0x0F12, 0x00E6,	//SARR_IllumTypeF[5]
    16, 0x0F12, 0x00E6,	//SARR_IllumTypeF[6]

    16, 0x002A, 0x0B94,
    16, 0x0F12, 0x0389,	//awbb_IndoorGrZones_m_BGrid[0]
    16, 0x0F12, 0x0394,	//awbb_IndoorGrZones_m_BGrid[1]
    16, 0x0F12, 0x0367,	//awbb_IndoorGrZones_m_BGrid[2]
    16, 0x0F12, 0x03A2,	//awbb_IndoorGrZones_m_BGrid[3]
    16, 0x0F12, 0x0341,	//awbb_IndoorGrZones_m_BGrid[4]
    16, 0x0F12, 0x03A6,	//awbb_IndoorGrZones_m_BGrid[5]
    16, 0x0F12, 0x0310,	//awbb_IndoorGrZones_m_BGrid[6]
    16, 0x0F12, 0x039F,	//awbb_IndoorGrZones_m_BGrid[7]
    16, 0x0F12, 0x02D3,	//awbb_IndoorGrZones_m_BGrid[8]
    16, 0x0F12, 0x0389,	//awbb_IndoorGrZones_m_BGrid[9]
    16, 0x0F12, 0x02BB,	//awbb_IndoorGrZones_m_BGrid[10]
    16, 0x0F12, 0x0372,	//awbb_IndoorGrZones_m_BGrid[11]
    16, 0x0F12, 0x02A7,	//awbb_IndoorGrZones_m_BGrid[12]
    16, 0x0F12, 0x0357,	//awbb_IndoorGrZones_m_BGrid[13]
    16, 0x0F12, 0x028F,	//awbb_IndoorGrZones_m_BGrid[14]
    16, 0x0F12, 0x033B,	//awbb_IndoorGrZones_m_BGrid[15]
    16, 0x0F12, 0x027D,	//awbb_IndoorGrZones_m_BGrid[16]
    16, 0x0F12, 0x031B,	//awbb_IndoorGrZones_m_BGrid[17]
    16, 0x0F12, 0x0267,	//awbb_IndoorGrZones_m_BGrid[18]
    16, 0x0F12, 0x0301,	//awbb_IndoorGrZones_m_BGrid[19]
    16, 0x0F12, 0x0255,	//awbb_IndoorGrZones_m_BGrid[20]
    16, 0x0F12, 0x02E9,	//awbb_IndoorGrZones_m_BGrid[21]
    16, 0x0F12, 0x023F,	//awbb_IndoorGrZones_m_BGrid[22]
    16, 0x0F12, 0x02DF,	//awbb_IndoorGrZones_m_BGrid[23]
    16, 0x0F12, 0x022B,	//awbb_IndoorGrZones_m_BGrid[24]
    16, 0x0F12, 0x02D1,	//awbb_IndoorGrZones_m_BGrid[25]
    16, 0x0F12, 0x0217,	//awbb_IndoorGrZones_m_BGrid[26]
    16, 0x0F12, 0x02BF,	//awbb_IndoorGrZones_m_BGrid[27]
    16, 0x0F12, 0x020F,	//awbb_IndoorGrZones_m_BGrid[28]
    16, 0x0F12, 0x02B1,	//awbb_IndoorGrZones_m_BGrid[29]
    16, 0x0F12, 0x01FF,	//awbb_IndoorGrZones_m_BGrid[30]
    16, 0x0F12, 0x02A9,	//awbb_IndoorGrZones_m_BGrid[31]
    16, 0x0F12, 0x01F7,	//awbb_IndoorGrZones_m_BGrid[32]
    16, 0x0F12, 0x0299,	//awbb_IndoorGrZones_m_BGrid[33]
    16, 0x0F12, 0x01ED,	//awbb_IndoorGrZones_m_BGrid[34]
    16, 0x0F12, 0x028B,	//awbb_IndoorGrZones_m_BGrid[35]
    16, 0x0F12, 0x01E1,	//awbb_IndoorGrZones_m_BGrid[36]
    16, 0x0F12, 0x027F,	//awbb_IndoorGrZones_m_BGrid[37]
    16, 0x0F12, 0x01D5,	//awbb_IndoorGrZones_m_BGrid[38]
    16, 0x0F12, 0x0279,	//awbb_IndoorGrZones_m_BGrid[39]
    16, 0x0F12, 0x01CB,	//awbb_IndoorGrZones_m_BGrid[40]
    16, 0x0F12, 0x0271,	//awbb_IndoorGrZones_m_BGrid[41]
    16, 0x0F12, 0x01C3,	//awbb_IndoorGrZones_m_BGrid[42]
    16, 0x0F12, 0x0267,	//awbb_IndoorGrZones_m_BGrid[43]
    16, 0x0F12, 0x01BB,	//awbb_IndoorGrZones_m_BGrid[44]
    16, 0x0F12, 0x025D,	//awbb_IndoorGrZones_m_BGrid[45]
    16, 0x0F12, 0x01B3,	//awbb_IndoorGrZones_m_BGrid[46]
    16, 0x0F12, 0x0253,	//awbb_IndoorGrZones_m_BGrid[47]
    16, 0x0F12, 0x01AF,	//awbb_IndoorGrZones_m_BGrid[48]
    16, 0x0F12, 0x0249,	//awbb_IndoorGrZones_m_BGrid[49]
    16, 0x0F12, 0x01AD,	//awbb_IndoorGrZones_m_BGrid[50]
    16, 0x0F12, 0x023B,	//awbb_IndoorGrZones_m_BGrid[51]
    16, 0x0F12, 0x01AD,	//awbb_IndoorGrZones_m_BGrid[52]
    16, 0x0F12, 0x022F,	//awbb_IndoorGrZones_m_BGrid[53]
    16, 0x0F12, 0x01B3,	//awbb_IndoorGrZones_m_BGrid[54]
    16, 0x0F12, 0x0227,	//awbb_IndoorGrZones_m_BGrid[55]
    16, 0x0F12, 0x01BB,	//awbb_IndoorGrZones_m_BGrid[56]
    16, 0x0F12, 0x0221,	//awbb_IndoorGrZones_m_BGrid[57]
    16, 0x0F12, 0x01CD,	//awbb_IndoorGrZones_m_BGrid[58]
    16, 0x0F12, 0x0219,	//awbb_IndoorGrZones_m_BGrid[59]
    16, 0x0F12, 0x01E7,	//awbb_IndoorGrZones_m_BGrid[60]
    16, 0x0F12, 0x0209,	//awbb_IndoorGrZones_m_BGrid[61]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[62]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[63]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[64]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[65]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[66]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[67]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[68]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[69]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[70]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[71]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[72]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[73]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[74]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[75]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[76]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[77]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[78]
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_BGrid[79]

    16, 0x002A, 0x0C34,
    16, 0x0F12, 0x0004,
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_GridStep
    16, 0x0F12, 0x0022,
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_ZInfo_m_GridSz
    16, 0x002A, 0x0C3C,
    16, 0x0F12, 0x00F3,
    16, 0x0F12, 0x0000,	//awbb_IndoorGrZones_m_Boffs
    16, 0x002A, 0x0C40,
    16, 0x0F12, 0x0000,		//awbb_IndoorGrZones_ZInfo_y_low
    16, 0x0F12, 0x00E2,		//awbb_IndoorGrZones_ZInfo_y_high

    //	param_start	awbb_LowBrGrZones_m_BGrid
    16, 0x002A, 0x0CB8,
    16, 0x0F12, 0x02D0,   //awbb_LowBrGrZones_m_BGrid[0]
    16, 0x0F12, 0x0325,   //awbb_LowBrGrZones_m_BGrid[1]
    16, 0x0F12, 0x0273,   //awbb_LowBrGrZones_m_BGrid[2]
    16, 0x0F12, 0x03BC,   //awbb_LowBrGrZones_m_BGrid[3]
    16, 0x0F12, 0x024E,   //awbb_LowBrGrZones_m_BGrid[4]
    16, 0x0F12, 0x03CC,   //awbb_LowBrGrZones_m_BGrid[5]
    16, 0x0F12, 0x022C,   //awbb_LowBrGrZones_m_BGrid[6]
    16, 0x0F12, 0x03C4,   //awbb_LowBrGrZones_m_BGrid[7]
    16, 0x0F12, 0x0213,   //awbb_LowBrGrZones_m_BGrid[8]
    16, 0x0F12, 0x03AF,   //awbb_LowBrGrZones_m_BGrid[9]
    16, 0x0F12, 0x01F6,   //awbb_LowBrGrZones_m_BGrid[10]
    16, 0x0F12, 0x0398,   //awbb_LowBrGrZones_m_BGrid[11]
    16, 0x0F12, 0x01DC,   //awbb_LowBrGrZones_m_BGrid[12]
    16, 0x0F12, 0x0372,   //awbb_LowBrGrZones_m_BGrid[13]
    16, 0x0F12, 0x01C2,   //awbb_LowBrGrZones_m_BGrid[14]
    16, 0x0F12, 0x032E,   //awbb_LowBrGrZones_m_BGrid[15]
    16, 0x0F12, 0x01A8,   //awbb_LowBrGrZones_m_BGrid[16]
    16, 0x0F12, 0x02F6,   //awbb_LowBrGrZones_m_BGrid[17]
    16, 0x0F12, 0x0194,   //awbb_LowBrGrZones_m_BGrid[18]
    16, 0x0F12, 0x02D2,   //awbb_LowBrGrZones_m_BGrid[19]
    16, 0x0F12, 0x0180,   //awbb_LowBrGrZones_m_BGrid[20]
    16, 0x0F12, 0x02BA,   //awbb_LowBrGrZones_m_BGrid[21]
    16, 0x0F12, 0x016C,   //awbb_LowBrGrZones_m_BGrid[22]
    16, 0x0F12, 0x02A6,   //awbb_LowBrGrZones_m_BGrid[23]
    16, 0x0F12, 0x015C,   //awbb_LowBrGrZones_m_BGrid[24]
    16, 0x0F12, 0x0298,   //awbb_LowBrGrZones_m_BGrid[25]
    16, 0x0F12, 0x0156,   //awbb_LowBrGrZones_m_BGrid[26]
    16, 0x0F12, 0x0282,   //awbb_LowBrGrZones_m_BGrid[27]
    16, 0x0F12, 0x0152,   //awbb_LowBrGrZones_m_BGrid[28]
    16, 0x0F12, 0x026A,   //awbb_LowBrGrZones_m_BGrid[29]
    16, 0x0F12, 0x0174,   //awbb_LowBrGrZones_m_BGrid[30]
    16, 0x0F12, 0x0258,   //awbb_LowBrGrZones_m_BGrid[31]
    16, 0x0F12, 0x01C2,   //awbb_LowBrGrZones_m_BGrid[32]
    16, 0x0F12, 0x0240,   //awbb_LowBrGrZones_m_BGrid[33]
    16, 0x0F12, 0x0000,   //awbb_LowBrGrZones_m_BGrid[34]
    16, 0x0F12, 0x0000,   //awbb_LowBrGrZones_m_BGrid[35]
    16, 0x0F12, 0x0000,   //awbb_LowBrGrZones_m_BGrid[36]
    16, 0x0F12, 0x0000,   //awbb_LowBrGrZones_m_BGrid[37]
    16, 0x0F12, 0x0000,   //awbb_LowBrGrZones_m_BGrid[38]
    16, 0x0F12, 0x0000,   //awbb_LowBrGrZones_m_BGrid[39]
    16, 0x0F12, 0x0000,   //awbb_LowBrGrZones_m_BGrid[40]
    16, 0x0F12, 0x0000,   //awbb_LowBrGrZones_m_BGrid[41]
    16, 0x0F12, 0x0000,   //awbb_LowBrGrZones_m_BGrid[42]
    16, 0x0F12, 0x0000,   //awbb_LowBrGrZones_m_BGrid[43]
    16, 0x0F12, 0x0000,   //awbb_LowBrGrZones_m_BGrid[44]
    16, 0x0F12, 0x0000,   //awbb_LowBrGrZones_m_BGrid[45]
    16, 0x0F12, 0x0000,   //awbb_LowBrGrZones_m_BGrid[46]
    16, 0x0F12, 0x0000,   //awbb_LowBrGrZones_m_BGrid[47]
    16, 0x0F12, 0x0000,   //awbb_LowBrGrZones_m_BGrid[48]
    16, 0x0F12, 0x0000,   //awbb_LowBrGrZones_m_BGrid[49]

    16, 0x002A, 0x0D1C,
    16, 0x0F12, 0x0005,
    16, 0x0F12, 0x0000,	//awbb_LowBrGrZones_m_GridStep
    16, 0x002A, 0x0D24,
    16, 0x0F12, 0x00CA,
    16, 0x0F12, 0x0000,	//awbb_LowBrGrZones_m_Boffs

    16, 0x002A, 0x0C44,
    16, 0x0F12, 0x0249,	//awbb_OutdoorGrZones_m_BGrid[0]
    16, 0x0F12, 0x025F,	//awbb_OutdoorGrZones_m_BGrid[1]
    16, 0x0F12, 0x022B,	//awbb_OutdoorGrZones_m_BGrid[2]
    16, 0x0F12, 0x0279,	//awbb_OutdoorGrZones_m_BGrid[3]
    16, 0x0F12, 0x0215,	//awbb_OutdoorGrZones_m_BGrid[4]
    16, 0x0F12, 0x027A,	//awbb_OutdoorGrZones_m_BGrid[5]
    16, 0x0F12, 0x0209,	//awbb_OutdoorGrZones_m_BGrid[6]
    16, 0x0F12, 0x0279,	//awbb_OutdoorGrZones_m_BGrid[7]
    16, 0x0F12, 0x01FF,	//awbb_OutdoorGrZones_m_BGrid[8]
    16, 0x0F12, 0x0271,	//awbb_OutdoorGrZones_m_BGrid[9]
    16, 0x0F12, 0x01F3,	//awbb_OutdoorGrZones_m_BGrid[10]
    16, 0x0F12, 0x0268,	//awbb_OutdoorGrZones_m_BGrid[11]
    16, 0x0F12, 0x01E8,	//awbb_OutdoorGrZones_m_BGrid[12]
    16, 0x0F12, 0x025E,	//awbb_OutdoorGrZones_m_BGrid[13]
    16, 0x0F12, 0x01DD,	//awbb_OutdoorGrZones_m_BGrid[14]
    16, 0x0F12, 0x0254,	//awbb_OutdoorGrZones_m_BGrid[15]
    16, 0x0F12, 0x01D2,	//awbb_OutdoorGrZones_m_BGrid[16]
    16, 0x0F12, 0x0246,	//awbb_OutdoorGrZones_m_BGrid[17]
    16, 0x0F12, 0x01CC,	//awbb_OutdoorGrZones_m_BGrid[18]
    16, 0x0F12, 0x023A,	//awbb_OutdoorGrZones_m_BGrid[19]
    16, 0x0F12, 0x01C7,	//awbb_OutdoorGrZones_m_BGrid[20]
    16, 0x0F12, 0x022F,	//awbb_OutdoorGrZones_m_BGrid[21]
    16, 0x0F12, 0x01C2,	//awbb_OutdoorGrZones_m_BGrid[22]
    16, 0x0F12, 0x0228,	//awbb_OutdoorGrZones_m_BGrid[23]
    16, 0x0F12, 0x01C2,	//awbb_OutdoorGrZones_m_BGrid[24]
    16, 0x0F12, 0x021E,	//awbb_OutdoorGrZones_m_BGrid[25]
    16, 0x0F12, 0x01C6,	//awbb_OutdoorGrZones_m_BGrid[26]
    16, 0x0F12, 0x0216,	//awbb_OutdoorGrZones_m_BGrid[27]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[28]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[29]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[30]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[31]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[32]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[33]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[34]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[35]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[36]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[37]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[38]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[39]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[40]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[41]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[42]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[43]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[44]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[45]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[46]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[47]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[48]
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_BGrid[49]

    16, 0x002A, 0x0CA8,
    16, 0x0F12, 0x0004,
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_GridStep
    16, 0x002A, 0x0CB0,
    16, 0x0F12, 0x01E6,
    16, 0x0F12, 0x0000,	//awbb_OutdoorGrZones_m_Boffs
    16, 0x002A, 0x0D2C,
    16, 0x0F12, 0x0354,
    16, 0x0F12, 0x0000,	//awbb_CrclLowT_R_c
    16, 0x002A, 0x0D30,
    16, 0x0F12, 0x0116,
    16, 0x0F12, 0x0000,	//awbb_CrclLowT_B_c
    16, 0x002A, 0x0D34,
    16, 0x0F12, 0x8F57,
    16, 0x0F12, 0x0000,	//awbb_CrclLowT_Rad_c
    16, 0x002A, 0x0D4A,
    16, 0x0F12, 0x0136,	//awbb_IntcR
    16, 0x0F12, 0x00F5,	//awbb_IntcB
    16, 0x002A, 0x0D5C,
    16, 0x0F12, 0x0492,	//awbb_MvEq_RBthresh
    16, 0x002A, 0x0D92,
    16, 0x0F12, 0x055E,	//awbb_LowTempRB

    16, 0x002A, 0x0D58,	//Awbb_ChromaMaxCoef
    16, 0x0F12, 0x0096,

    16, 0x002A, 0x0D74,
    16, 0x0F12, 0x0002,	//awbb_GridEnable
    16, 0x002A, 0x0E0A,
    16, 0x0F12, 0x00B5,	//awbb_GridCoeff_R_2
    16, 0x002A, 0x0E0C,
    16, 0x0F12, 0x00B5,	//awbb_GridCoeff_B_2
    16, 0x002A, 0x0DFA,
    16, 0x0F12, 0x0F47,	//awbb_GridConst_2[0]
    16, 0x002A, 0x0DFC,
    16, 0x0F12, 0x0FF0,	//awbb_GridConst_2[1]
    16, 0x002A, 0x0DFE,
    16, 0x0F12, 0x1064,	//awbb_GridConst_2[2]
    16, 0x002A, 0x0E00,
    16, 0x0F12, 0x10C8,	//awbb_GridConst_2[3]
    16, 0x002A, 0x0E02,
    16, 0x0F12, 0x115E,	//awbb_GridConst_2[4]
    16, 0x002A, 0x0E04,
    16, 0x0F12, 0x11F9,	//awbb_GridConst_2[5]
    16, 0x002A, 0x0E06,
    16, 0x0F12, 0x00B2,	//awbb_GridCoeff_R_1
    16, 0x002A, 0x0E08,
    16, 0x0F12, 0x00B8,	//awbb_GridCoeff_B_1
    16, 0x002A, 0x0DF4,
    16, 0x0F12, 0x029B,	//awbb_GridConst_1[0]
    16, 0x002A, 0x0DF6,
    16, 0x0F12, 0x031D,	//awbb_GridConst_1[1]
    16, 0x002A, 0x0DF8,
    16, 0x0F12, 0x0390,	//awbb_GridConst_1[2]
    16, 0x002A, 0x0DAC,
    16, 0x0F12, 0xFFFE,	//awbb_GridCorr_R[0][0]
    16, 0x002A, 0x0DD0,
    16, 0x0F12, 0x000C,	//awbb_GridCorr_B[0][0]
    16, 0x002A, 0x0DB8,
    16, 0x0F12, 0x0057,	//awbb_GridCorr_R[1][0]
    16, 0x002A, 0x0DDC,
    16, 0x0F12, 0x0000,	//awbb_GridCorr_B[1][0]
    16, 0x002A, 0x0DC4,
    16, 0x0F12, 0x0032,	//awbb_GridCorr_R[2][0]
    16, 0x002A, 0x0DE8,
    16, 0x0F12, 0xFFDD,	//awbb_GridCorr_B[2][0]
    16, 0x002A, 0x0DAE,
    16, 0x0F12, 0xFFFE,	//awbb_GridCorr_R[0][1]
    16, 0x002A, 0x0DD2,
    16, 0x0F12, 0x0006,	//awbb_GridCorr_B[0][1]
    16, 0x002A, 0x0DBA,
    16, 0x0F12, 0xFFEC,	//awbb_GridCorr_R[1][1]
    16, 0x002A, 0x0DDE,
    16, 0x0F12, 0x0000,	//awbb_GridCorr_B[1][1]
    16, 0x002A, 0x0DC6,
    16, 0x0F12, 0xFFBA,	//awbb_GridCorr_R[2][1]
    16, 0x002A, 0x0DEA,
    16, 0x0F12, 0xFFDD,	//awbb_GridCorr_B[2][1]
    16, 0x002A, 0x0DB0,
    16, 0x0F12, 0xFFB2,	//awbb_GridCorr_R[0][2]
    16, 0x002A, 0x0DD4,
    16, 0x0F12, 0x0000,	//awbb_GridCorr_B[0][2]
    16, 0x002A, 0x0DBC,
    16, 0x0F12, 0x000A,	//awbb_GridCorr_R[1][2]
    16, 0x002A, 0x0DE0,
    16, 0x0F12, 0x0000,	//awbb_GridCorr_B[1][2]
    16, 0x002A, 0x0DC8,
    16, 0x0F12, 0xFF9C,	//awbb_GridCorr_R[2][2]
    16, 0x002A, 0x0DEC,
    16, 0x0F12, 0x0000,	//awbb_GridCorr_B[2][2]
    16, 0x002A, 0x0DB2,
    16, 0x0F12, 0x0046,	//awbb_GridCorr_R[0][3]
    16, 0x002A, 0x0DD6,
    16, 0x0F12, 0xFFCE,	//awbb_GridCorr_B[0][3]
    16, 0x002A, 0x0DBE,
    16, 0x0F12, 0x0026,	//awbb_GridCorr_R[1][3]
    16, 0x002A, 0x0DE2,
    16, 0x0F12, 0xFF9C,	//awbb_GridCorr_B[1][3]
    16, 0x002A, 0x0DCA,
    16, 0x0F12, 0x0000,	//awbb_GridCorr_R[2][3]
    16, 0x002A, 0x0DEE,
    16, 0x0F12, 0x0000,	//awbb_GridCorr_B[2][3]
    16, 0x002A, 0x0DB4,
    16, 0x0F12, 0x0000,	//awbb_GridCorr_R[0][4]
    16, 0x002A, 0x0DD8,
    16, 0x0F12, 0x0000,	//awbb_GridCorr_B[0][4]
    16, 0x002A, 0x0DC0,
    16, 0x0F12, 0x0032,	//awbb_GridCorr_R[1][4]
    16, 0x002A, 0x0DE4,
    16, 0x0F12, 0x0015,	//awbb_GridCorr_B[1][4]
    16, 0x002A, 0x0DCC,
    16, 0x0F12, 0x0000,	//awbb_GridCorr_R[2][4]
    16, 0x002A, 0x0DF0,
    16, 0x0F12, 0x0000,	//awbb_GridCorr_B[2][4]
    16, 0x002A, 0x0DB6,
    16, 0x0F12, 0x0000,	//awbb_GridCorr_R[0][5]
    16, 0x002A, 0x0DDA,
    16, 0x0F12, 0x0000,	//awbb_GridCorr_B[0][5]
    16, 0x002A, 0x0DC2,
    16, 0x0F12, 0x0000,	//awbb_GridCorr_R[1][5]
    16, 0x002A, 0x0DE6,
    16, 0x0F12, 0x0020,	//awbb_GridCorr_B[1][5]
    16, 0x002A, 0x0DCE,
    16, 0x0F12, 0x0000,	//awbb_GridCorr_R[2][5]
    16, 0x002A, 0x0DF2,
    16, 0x0F12, 0x0000,	//awbb_GridCorr_B[2][5]

    16, 0x002A, 0x046C,
    16, 0x0F12, 0x0000,	//SARR_usGammaLutRGBIndoor[0]
    16, 0x0F12, 0x0006,	//SARR_usGammaLutRGBIndoor[1]
    16, 0x0F12, 0x0010,	//SARR_usGammaLutRGBIndoor[2]
    16, 0x0F12, 0x002A,	//SARR_usGammaLutRGBIndoor[3]
    16, 0x0F12, 0x0064,	//SARR_usGammaLutRGBIndoor[4]
    16, 0x0F12, 0x00CB,	//SARR_usGammaLutRGBIndoor[5]
    16, 0x0F12, 0x011F,	//SARR_usGammaLutRGBIndoor[6]
    16, 0x0F12, 0x0146,	//SARR_usGammaLutRGBIndoor[7]
    16, 0x0F12, 0x0168,	//SARR_usGammaLutRGBIndoor[8]
    16, 0x0F12, 0x01A2,	//SARR_usGammaLutRGBIndoor[9]
    16, 0x0F12, 0x01D2,	//SARR_usGammaLutRGBIndoor[10]
    16, 0x0F12, 0x01FA,	//SARR_usGammaLutRGBIndoor[11]
    16, 0x0F12, 0x0220,	//SARR_usGammaLutRGBIndoor[12]
    16, 0x0F12, 0x0266,	//SARR_usGammaLutRGBIndoor[13]
    16, 0x0F12, 0x029D,	//SARR_usGammaLutRGBIndoor[14]
    16, 0x0F12, 0x02F8,	//SARR_usGammaLutRGBIndoor[15]
    16, 0x0F12, 0x0347,	//SARR_usGammaLutRGBIndoor[16]
    16, 0x0F12, 0x038E,	//SARR_usGammaLutRGBIndoor[17]
    16, 0x0F12, 0x03CB,	//SARR_usGammaLutRGBIndoor[18]
    16, 0x0F12, 0x03FF,	//SARR_usGammaLutRGBIndoor[19]
    16, 0x0F12, 0x0000,	//SARR_usGammaLutRGBIndoor[20]
    16, 0x0F12, 0x0006,	//SARR_usGammaLutRGBIndoor[21]
    16, 0x0F12, 0x0010,	//SARR_usGammaLutRGBIndoor[22]
    16, 0x0F12, 0x002A,	//SARR_usGammaLutRGBIndoor[23]
    16, 0x0F12, 0x0064,	//SARR_usGammaLutRGBIndoor[24]
    16, 0x0F12, 0x00CB,	//SARR_usGammaLutRGBIndoor[25]
    16, 0x0F12, 0x011F,	//SARR_usGammaLutRGBIndoor[26]
    16, 0x0F12, 0x0146,	//SARR_usGammaLutRGBIndoor[27]
    16, 0x0F12, 0x0168,	//SARR_usGammaLutRGBIndoor[28]
    16, 0x0F12, 0x01A2,	//SARR_usGammaLutRGBIndoor[29]
    16, 0x0F12, 0x01D2,	//SARR_usGammaLutRGBIndoor[30]
    16, 0x0F12, 0x01FA,	//SARR_usGammaLutRGBIndoor[31]
    16, 0x0F12, 0x0220,	//SARR_usGammaLutRGBIndoor[32]
    16, 0x0F12, 0x0266,	//SARR_usGammaLutRGBIndoor[33]
    16, 0x0F12, 0x029D,	//SARR_usGammaLutRGBIndoor[34]
    16, 0x0F12, 0x02F8,	//SARR_usGammaLutRGBIndoor[35]
    16, 0x0F12, 0x0347,	//SARR_usGammaLutRGBIndoor[36]
    16, 0x0F12, 0x038E,	//SARR_usGammaLutRGBIndoor[37]
    16, 0x0F12, 0x03CB,	//SARR_usGammaLutRGBIndoor[38]
    16, 0x0F12, 0x03FF,	//SARR_usGammaLutRGBIndoor[39]
    16, 0x0F12, 0x0000,	//SARR_usGammaLutRGBIndoor[40]
    16, 0x0F12, 0x0006,	//SARR_usGammaLutRGBIndoor[41]
    16, 0x0F12, 0x0010,	//SARR_usGammaLutRGBIndoor[42]
    16, 0x0F12, 0x002A,	//SARR_usGammaLutRGBIndoor[43]
    16, 0x0F12, 0x0064,	//SARR_usGammaLutRGBIndoor[44]
    16, 0x0F12, 0x00CB,	//SARR_usGammaLutRGBIndoor[45]
    16, 0x0F12, 0x011F,	//SARR_usGammaLutRGBIndoor[46]
    16, 0x0F12, 0x0146,	//SARR_usGammaLutRGBIndoor[47]
    16, 0x0F12, 0x0168,	//SARR_usGammaLutRGBIndoor[48]
    16, 0x0F12, 0x01A2,	//SARR_usGammaLutRGBIndoor[49]
    16, 0x0F12, 0x01D2,	//SARR_usGammaLutRGBIndoor[50]
    16, 0x0F12, 0x01FA,	//SARR_usGammaLutRGBIndoor[51]
    16, 0x0F12, 0x0220,	//SARR_usGammaLutRGBIndoor[52]
    16, 0x0F12, 0x0266,	//SARR_usGammaLutRGBIndoor[53]
    16, 0x0F12, 0x029D,	//SARR_usGammaLutRGBIndoor[54]
    16, 0x0F12, 0x02F8,	//SARR_usGammaLutRGBIndoor[55]
    16, 0x0F12, 0x0347,	//SARR_usGammaLutRGBIndoor[56]
    16, 0x0F12, 0x038E,	//SARR_usGammaLutRGBIndoor[57]
    16, 0x0F12, 0x03CB,	//SARR_usGammaLutRGBIndoor[58]
    16, 0x0F12, 0x03FF,	//SARR_usGammaLutRGBIndoor[59]

    16, 0x002A, 0x0E98,
    16, 0x0F12, 0x0042,	//TVAR_ae_BrAve

    16, 0x002A, 0x06A6,
    16, 0x0F12, 0x00AD,	//SARR_AwbCcmCord[0]
    16, 0x0F12, 0x00D0,	//SARR_AwbCcmCord[1]
    16, 0x0F12, 0x00E0,	//SARR_AwbCcmCord[2]
    16, 0x0F12, 0x012E,	//SARR_AwbCcmCord[3]
    16, 0x0F12, 0x016B,	//SARR_AwbCcmCord[4]
    16, 0x0F12, 0x0195,	//SARR_AwbCcmCord[5]

    16, 0x002A, 0x33B0,
    16, 0x0F12, 0x0226,	//TVAR_wbt_pBaseCcms[0] HORIZON
    16, 0x0F12, 0xFFAB,	//TVAR_wbt_pBaseCcms[1]
    16, 0x0F12, 0xFFB2,	//TVAR_wbt_pBaseCcms[2]
    16, 0x0F12, 0xFED5,	//TVAR_wbt_pBaseCcms[3]
    16, 0x0F12, 0x01C4,	//TVAR_wbt_pBaseCcms[4]
    16, 0x0F12, 0xFEE8,	//TVAR_wbt_pBaseCcms[5]
    16, 0x0F12, 0xFFA8,	//TVAR_wbt_pBaseCcms[6]
    16, 0x0F12, 0xFFD8,	//TVAR_wbt_pBaseCcms[7]
    16, 0x0F12, 0x01F6,	//TVAR_wbt_pBaseCcms[8]
    16, 0x0F12, 0x012C,	//TVAR_wbt_pBaseCcms[9]
    16, 0x0F12, 0x0120,	//TVAR_wbt_pBaseCcms[10]
    16, 0x0F12, 0xFF2D,	//TVAR_wbt_pBaseCcms[11]
    16, 0x0F12, 0x0239,	//TVAR_wbt_pBaseCcms[12]
    16, 0x0F12, 0xFF4C,	//TVAR_wbt_pBaseCcms[13]
    16, 0x0F12, 0x0134,	//TVAR_wbt_pBaseCcms[14]
    16, 0x0F12, 0xFF27,	//TVAR_wbt_pBaseCcms[15]
    16, 0x0F12, 0x017F,	//TVAR_wbt_pBaseCcms[16]
    16, 0x0F12, 0x00D3,	//TVAR_wbt_pBaseCcms[17]

    16, 0x0F12, 0x0226,	//TVAR_wbt_pBaseCcms[18] INCA A
    16, 0x0F12, 0xFFAB,	//TVAR_wbt_pBaseCcms[19]
    16, 0x0F12, 0xFFB2,	//TVAR_wbt_pBaseCcms[20]
    16, 0x0F12, 0xFED5,	//TVAR_wbt_pBaseCcms[21]
    16, 0x0F12, 0x01C4,	//TVAR_wbt_pBaseCcms[22]
    16, 0x0F12, 0xFEE8,	//TVAR_wbt_pBaseCcms[23]
    16, 0x0F12, 0xFFA8,	//TVAR_wbt_pBaseCcms[24]
    16, 0x0F12, 0xFFD8,	//TVAR_wbt_pBaseCcms[25]
    16, 0x0F12, 0x01F6,	//TVAR_wbt_pBaseCcms[26]
    16, 0x0F12, 0x012C,	//TVAR_wbt_pBaseCcms[27]
    16, 0x0F12, 0x0120,	//TVAR_wbt_pBaseCcms[28]
    16, 0x0F12, 0xFF2D,	//TVAR_wbt_pBaseCcms[29]
    16, 0x0F12, 0x0239,	//TVAR_wbt_pBaseCcms[30]
    16, 0x0F12, 0xFF4C,	//TVAR_wbt_pBaseCcms[31]
    16, 0x0F12, 0x0134,	//TVAR_wbt_pBaseCcms[32]
    16, 0x0F12, 0xFF27,	//TVAR_wbt_pBaseCcms[33]
    16, 0x0F12, 0x017F,	//TVAR_wbt_pBaseCcms[34]
    16, 0x0F12, 0x00D3,	//TVAR_wbt_pBaseCcms[35]

    16, 0x0F12, 0x0226,	//TVAR_wbt_pBaseCcms[36] WARM WHITE
    16, 0x0F12, 0xFFAB,	//TVAR_wbt_pBaseCcms[37]
    16, 0x0F12, 0xFFB2,	//TVAR_wbt_pBaseCcms[38]
    16, 0x0F12, 0xFED5,	//TVAR_wbt_pBaseCcms[39]
    16, 0x0F12, 0x01C4,	//TVAR_wbt_pBaseCcms[40]
    16, 0x0F12, 0xFEE8,	//TVAR_wbt_pBaseCcms[41]
    16, 0x0F12, 0xFFA8,	//TVAR_wbt_pBaseCcms[42]
    16, 0x0F12, 0xFFD8,	//TVAR_wbt_pBaseCcms[43]
    16, 0x0F12, 0x01F6,	//TVAR_wbt_pBaseCcms[44]
    16, 0x0F12, 0x012C,	//TVAR_wbt_pBaseCcms[45]
    16, 0x0F12, 0x0120,	//TVAR_wbt_pBaseCcms[46]
    16, 0x0F12, 0xFF2D,	//TVAR_wbt_pBaseCcms[47]
    16, 0x0F12, 0x0239,	//TVAR_wbt_pBaseCcms[48]
    16, 0x0F12, 0xFF4C,	//TVAR_wbt_pBaseCcms[49]
    16, 0x0F12, 0x0134,	//TVAR_wbt_pBaseCcms[50]
    16, 0x0F12, 0xFF27,	//TVAR_wbt_pBaseCcms[51]
    16, 0x0F12, 0x017F,	//TVAR_wbt_pBaseCcms[52]
    16, 0x0F12, 0x00D3,	//TVAR_wbt_pBaseCcms[53]

    16, 0x0F12, 0x0226,	//TVAR_wbt_pBaseCcms[54] COOL WHITE
    16, 0x0F12, 0xFFAB,	//TVAR_wbt_pBaseCcms[55]
    16, 0x0F12, 0xFFB2,	//TVAR_wbt_pBaseCcms[56]
    16, 0x0F12, 0xFED5,	//TVAR_wbt_pBaseCcms[57]
    16, 0x0F12, 0x01C4,	//TVAR_wbt_pBaseCcms[58]
    16, 0x0F12, 0xFEE8,	//TVAR_wbt_pBaseCcms[59]
    16, 0x0F12, 0xFFA8,	//TVAR_wbt_pBaseCcms[60]
    16, 0x0F12, 0xFFD8,	//TVAR_wbt_pBaseCcms[61]
    16, 0x0F12, 0x01F6,	//TVAR_wbt_pBaseCcms[62]
    16, 0x0F12, 0x012C,	//TVAR_wbt_pBaseCcms[63]
    16, 0x0F12, 0x0120,	//TVAR_wbt_pBaseCcms[64]
    16, 0x0F12, 0xFF2D,	//TVAR_wbt_pBaseCcms[65]
    16, 0x0F12, 0x0239,	//TVAR_wbt_pBaseCcms[66]
    16, 0x0F12, 0xFF4C,	//TVAR_wbt_pBaseCcms[67]
    16, 0x0F12, 0x0134,	//TVAR_wbt_pBaseCcms[68]
    16, 0x0F12, 0xFF27,	//TVAR_wbt_pBaseCcms[69]
    16, 0x0F12, 0x017F,	//TVAR_wbt_pBaseCcms[70]
    16, 0x0F12, 0x00D3,	//TVAR_wbt_pBaseCcms[71]

    16, 0x0F12, 0x0226,	//TVAR_wbt_pBaseCcms[72] D50
    16, 0x0F12, 0xFFAB,	//TVAR_wbt_pBaseCcms[73]
    16, 0x0F12, 0xFFB2,	//TVAR_wbt_pBaseCcms[74]
    16, 0x0F12, 0xFED5,	//TVAR_wbt_pBaseCcms[75]
    16, 0x0F12, 0x01C4,	//TVAR_wbt_pBaseCcms[76]
    16, 0x0F12, 0xFEE8,	//TVAR_wbt_pBaseCcms[77]
    16, 0x0F12, 0xFFA8,	//TVAR_wbt_pBaseCcms[78]
    16, 0x0F12, 0xFFD8,	//TVAR_wbt_pBaseCcms[79]
    16, 0x0F12, 0x01F6,	//TVAR_wbt_pBaseCcms[80]
    16, 0x0F12, 0x012C,	//TVAR_wbt_pBaseCcms[81]
    16, 0x0F12, 0x0120,	//TVAR_wbt_pBaseCcms[82]
    16, 0x0F12, 0xFF2D,	//TVAR_wbt_pBaseCcms[83]
    16, 0x0F12, 0x0239,	//TVAR_wbt_pBaseCcms[84]
    16, 0x0F12, 0xFF4C,	//TVAR_wbt_pBaseCcms[85]
    16, 0x0F12, 0x0134,	//TVAR_wbt_pBaseCcms[86]
    16, 0x0F12, 0xFF27,	//TVAR_wbt_pBaseCcms[87]
    16, 0x0F12, 0x017F,	//TVAR_wbt_pBaseCcms[88]
    16, 0x0F12, 0x00D3,	//TVAR_wbt_pBaseCcms[89]

    16, 0x0F12, 0x0226,	//TVAR_wbt_pBaseCcms[90] D65
    16, 0x0F12, 0xFFAB,	//TVAR_wbt_pBaseCcms[91]
    16, 0x0F12, 0xFFB2,	//TVAR_wbt_pBaseCcms[92]
    16, 0x0F12, 0xFED5,	//TVAR_wbt_pBaseCcms[93]
    16, 0x0F12, 0x01C4,	//TVAR_wbt_pBaseCcms[94]
    16, 0x0F12, 0xFEE8,	//TVAR_wbt_pBaseCcms[95]
    16, 0x0F12, 0xFFA8,	//TVAR_wbt_pBaseCcms[96]
    16, 0x0F12, 0xFFD8,	//TVAR_wbt_pBaseCcms[97]
    16, 0x0F12, 0x01F6,	//TVAR_wbt_pBaseCcms[98]
    16, 0x0F12, 0x012C,	//TVAR_wbt_pBaseCcms[99]
    16, 0x0F12, 0x0120,	//TVAR_wbt_pBaseCcms[100]
    16, 0x0F12, 0xFF2D,	//TVAR_wbt_pBaseCcms[101]
    16, 0x0F12, 0x0239,	//TVAR_wbt_pBaseCcms[102]
    16, 0x0F12, 0xFF4C,	//TVAR_wbt_pBaseCcms[103]
    16, 0x0F12, 0x0134,	//TVAR_wbt_pBaseCcms[104]
    16, 0x0F12, 0xFF27,	//TVAR_wbt_pBaseCcms[105]
    16, 0x0F12, 0x017F,	//TVAR_wbt_pBaseCcms[106]
    16, 0x0F12, 0x00D3,	//TVAR_wbt_pBaseCcms[107]

    16, 0x002A, 0x338C,
    16, 0x0F12, 0x01D3,	//TVAR_wbt_pOutdoorCcm[0]
    16, 0x0F12, 0xFFBF,	//TVAR_wbt_pOutdoorCcm[1]
    16, 0x0F12, 0xFFC0,	//TVAR_wbt_pOutdoorCcm[2]
    16, 0x0F12, 0xFDEA,	//TVAR_wbt_pOutdoorCcm[3]
    16, 0x0F12, 0x0141,	//TVAR_wbt_pOutdoorCcm[4]
    16, 0x0F12, 0xFE91,	//TVAR_wbt_pOutdoorCcm[5]
    16, 0x0F12, 0x000C,	//TVAR_wbt_pOutdoorCcm[6]
    16, 0x0F12, 0xFFDB,	//TVAR_wbt_pOutdoorCcm[7]
    16, 0x0F12, 0x01DB,	//TVAR_wbt_pOutdoorCcm[8]
    16, 0x0F12, 0x00E1,	//TVAR_wbt_pOutdoorCcm[9]
    16, 0x0F12, 0x011B,	//TVAR_wbt_pOutdoorCcm[10]
    16, 0x0F12, 0xFF5A,	//TVAR_wbt_pOutdoorCcm[11]
    16, 0x0F12, 0x01F2,	//TVAR_wbt_pOutdoorCcm[12]
    16, 0x0F12, 0xFFE2,	//TVAR_wbt_pOutdoorCcm[13]
    16, 0x0F12, 0x01A5,	//TVAR_wbt_pOutdoorCcm[14]
    16, 0x0F12, 0xFF2F,	//TVAR_wbt_pOutdoorCcm[15]
    16, 0x0F12, 0x013D,	//TVAR_wbt_pOutdoorCcm[16]
    16, 0x0F12, 0x015F,	//TVAR_wbt_pOutdoorCcm[17]

    16, 0x002A, 0x0724,
    16, 0x0F12, 0x0004,	//afit_uNoiseIndInDoor[0]
    16, 0x0F12, 0x000F,	//afit_uNoiseIndInDoor[1]
    16, 0x0F12, 0x002F,	//afit_uNoiseIndInDoor[2]
    16, 0x0F12, 0x0082,	//afit_uNoiseIndInDoor[3]
    16, 0x0F12, 0x0266,	//afit_uNoiseIndInDoor[4]
    //	param_end	afit_uNoiseIndInDoor
    //	param_start	TVAR_afit_pBaseVals
    16, 0x002A, 0x0788,
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[0]
    16, 0x0F12, 0xFFE1,	//TVAR_afit_pBaseVals[1]
    16, 0x0F12, 0xFFBA,	//TVAR_afit_pBaseVals[2]
    16, 0x0F12, 0x005B,	//TVAR_afit_pBaseVals[3]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[4]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[5]
    16, 0x0F12, 0x0028,	//TVAR_afit_pBaseVals[6]
    16, 0x0F12, 0x0028,	//TVAR_afit_pBaseVals[7]
    16, 0x0F12, 0x00FF,	//TVAR_afit_pBaseVals[8]
    16, 0x0F12, 0x00FF,	//TVAR_afit_pBaseVals[9]
    16, 0x0F12, 0x0004,	//TVAR_afit_pBaseVals[10]
    16, 0x0F12, 0x003C,	//TVAR_afit_pBaseVals[11]
    16, 0x0F12, 0x000A,	//TVAR_afit_pBaseVals[12]
    16, 0x0F12, 0x001E,	//TVAR_afit_pBaseVals[13]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[14]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[15]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[16]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[17]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[18]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[19]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[20]
    16, 0x0F12, 0x0064,	//TVAR_afit_pBaseVals[21]
    16, 0x0F12, 0x0064,	//TVAR_afit_pBaseVals[22]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[23]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[24]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[25]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[26]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[27]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[28]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[29]
    16, 0x0F12, 0x0362,	//TVAR_afit_pBaseVals[30]
    16, 0x0F12, 0x4C4C,	//TVAR_afit_pBaseVals[31]
    16, 0x0F12, 0x4C4C,	//TVAR_afit_pBaseVals[32]
    16, 0x0F12, 0x0303,	//TVAR_afit_pBaseVals[33]
    16, 0x0F12, 0x0303,	//TVAR_afit_pBaseVals[34]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[35]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[36]
    16, 0x0F12, 0x2828,	//TVAR_afit_pBaseVals[37]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[38]
    16, 0x0F12, 0x0220,	//TVAR_afit_pBaseVals[39]
    16, 0x0F12, 0x0880,	//TVAR_afit_pBaseVals[40]
    16, 0x0F12, 0x0205,	//TVAR_afit_pBaseVals[41]
    16, 0x0F12, 0x030A,	//TVAR_afit_pBaseVals[42]
    16, 0x0F12, 0x0003,	//TVAR_afit_pBaseVals[43]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[44]
    16, 0x0F12, 0x0500,	//TVAR_afit_pBaseVals[45]
    16, 0x0F12, 0x0805,	//TVAR_afit_pBaseVals[46]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[47]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[48]
    16, 0x0F12, 0x0505,	//TVAR_afit_pBaseVals[49]
    16, 0x0F12, 0x0208,	//TVAR_afit_pBaseVals[50]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[51]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[52]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[53]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[54]
    16, 0x0F12, 0x0600,	//TVAR_afit_pBaseVals[55]
    16, 0x0F12, 0x100A,	//TVAR_afit_pBaseVals[56]
    16, 0x0F12, 0x016E,	//TVAR_afit_pBaseVals[57]
    16, 0x0F12, 0x01B7,	//TVAR_afit_pBaseVals[58]
    16, 0x0F12, 0x012F,	//TVAR_afit_pBaseVals[59]
    16, 0x0F12, 0x8058,	//TVAR_afit_pBaseVals[60]
    16, 0x0F12, 0x0180,	//TVAR_afit_pBaseVals[61]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[62]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[63]
    16, 0x0F12, 0xFFF5,	//TVAR_afit_pBaseVals[64]
    16, 0x0F12, 0xFFD3,	//TVAR_afit_pBaseVals[65]
    16, 0x0F12, 0x0037,	//TVAR_afit_pBaseVals[66]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[67]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[68]
    16, 0x0F12, 0x0014,	//TVAR_afit_pBaseVals[69]
    16, 0x0F12, 0x0014,	//TVAR_afit_pBaseVals[70]
    16, 0x0F12, 0x00FF,	//TVAR_afit_pBaseVals[71]
    16, 0x0F12, 0x00FF,	//TVAR_afit_pBaseVals[72]
    16, 0x0F12, 0x0004,	//TVAR_afit_pBaseVals[73]
    16, 0x0F12, 0x003C,	//TVAR_afit_pBaseVals[74]
    16, 0x0F12, 0x000A,	//TVAR_afit_pBaseVals[75]
    16, 0x0F12, 0x001E,	//TVAR_afit_pBaseVals[76]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[77]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[78]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[79]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[80]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[81]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[82]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[83]
    16, 0x0F12, 0x0064,	//TVAR_afit_pBaseVals[84]
    16, 0x0F12, 0x0064,	//TVAR_afit_pBaseVals[85]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[86]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[87]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[88]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[89]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[90]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[91]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[92]
    16, 0x0F12, 0x0362,	//TVAR_afit_pBaseVals[93]
    16, 0x0F12, 0x4C4C,	//TVAR_afit_pBaseVals[94]
    16, 0x0F12, 0x4C4C,	//TVAR_afit_pBaseVals[95]
    16, 0x0F12, 0x0303,	//TVAR_afit_pBaseVals[96]
    16, 0x0F12, 0x0303,	//TVAR_afit_pBaseVals[97]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[98]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[99]
    16, 0x0F12, 0x2828,	//TVAR_afit_pBaseVals[100]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[101]
    16, 0x0F12, 0x0214,	//TVAR_afit_pBaseVals[102]
    16, 0x0F12, 0x0880,	//TVAR_afit_pBaseVals[103]
    16, 0x0F12, 0x0205,	//TVAR_afit_pBaseVals[104]
    16, 0x0F12, 0x030A,	//TVAR_afit_pBaseVals[105]
    16, 0x0F12, 0x0003,	//TVAR_afit_pBaseVals[106]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[107]
    16, 0x0F12, 0x0500,	//TVAR_afit_pBaseVals[108]
    16, 0x0F12, 0x0805,	//TVAR_afit_pBaseVals[109]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[110]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[111]
    16, 0x0F12, 0x0505,	//TVAR_afit_pBaseVals[112]
    16, 0x0F12, 0x0208,	//TVAR_afit_pBaseVals[113]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[114]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[115]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[116]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[117]
    16, 0x0F12, 0x0600,	//TVAR_afit_pBaseVals[118]
    16, 0x0F12, 0x100A,	//TVAR_afit_pBaseVals[119]
    16, 0x0F12, 0x016E,	//TVAR_afit_pBaseVals[120]
    16, 0x0F12, 0x0188,	//TVAR_afit_pBaseVals[121]
    16, 0x0F12, 0x012C,	//TVAR_afit_pBaseVals[122]
    16, 0x0F12, 0x8058,	//TVAR_afit_pBaseVals[123]
    16, 0x0F12, 0x0180,	//TVAR_afit_pBaseVals[124]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[125]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[126]
    16, 0x0F12, 0xFFFE,	//TVAR_afit_pBaseVals[127]
    16, 0x0F12, 0xFFEC,	//TVAR_afit_pBaseVals[128]
    16, 0x0F12, 0x001C,	//TVAR_afit_pBaseVals[129]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[130]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[131]
    16, 0x0F12, 0x000A,	//TVAR_afit_pBaseVals[132]
    16, 0x0F12, 0x000A,	//TVAR_afit_pBaseVals[133]
    16, 0x0F12, 0x00FF,	//TVAR_afit_pBaseVals[134]
    16, 0x0F12, 0x00FF,	//TVAR_afit_pBaseVals[135]
    16, 0x0F12, 0x0004,	//TVAR_afit_pBaseVals[136]
    16, 0x0F12, 0x003C,	//TVAR_afit_pBaseVals[137]
    16, 0x0F12, 0x000A,	//TVAR_afit_pBaseVals[138]
    16, 0x0F12, 0x001E,	//TVAR_afit_pBaseVals[139]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[140]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[141]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[142]
    16, 0x0F12, 0x0014,	//TVAR_afit_pBaseVals[143]
    16, 0x0F12, 0x00F4,	//TVAR_afit_pBaseVals[144]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[145]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[146]
    16, 0x0F12, 0x0064,	//TVAR_afit_pBaseVals[147]
    16, 0x0F12, 0x0064,	//TVAR_afit_pBaseVals[148]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[149]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[150]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[151]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[152]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[153]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[154]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[155]
    16, 0x0F12, 0x0362,	//TVAR_afit_pBaseVals[156]
    16, 0x0F12, 0x1919,	//TVAR_afit_pBaseVals[157]
    16, 0x0F12, 0x3232,	//TVAR_afit_pBaseVals[158]
    16, 0x0F12, 0x0303,	//TVAR_afit_pBaseVals[159]
    16, 0x0F12, 0x0303,	//TVAR_afit_pBaseVals[160]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[161]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[162]
    16, 0x0F12, 0x2828,	//TVAR_afit_pBaseVals[163]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[164]
    16, 0x0F12, 0x020A,	//TVAR_afit_pBaseVals[165]
    16, 0x0F12, 0x0880,	//TVAR_afit_pBaseVals[166]
    16, 0x0F12, 0x0205,	//TVAR_afit_pBaseVals[167]
    16, 0x0F12, 0x030A,	//TVAR_afit_pBaseVals[168]
    16, 0x0F12, 0x1103,	//TVAR_afit_pBaseVals[169]
    16, 0x0F12, 0x0A11,	//TVAR_afit_pBaseVals[170]
    16, 0x0F12, 0x050A,	//TVAR_afit_pBaseVals[171]
    16, 0x0F12, 0x0805,	//TVAR_afit_pBaseVals[172]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[173]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[174]
    16, 0x0F12, 0x0505,	//TVAR_afit_pBaseVals[175]
    16, 0x0F12, 0x0208,	//TVAR_afit_pBaseVals[176]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[177]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[178]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[179]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[180]
    16, 0x0F12, 0x0600,	//TVAR_afit_pBaseVals[181]
    16, 0x0F12, 0x100A,	//TVAR_afit_pBaseVals[182]
    16, 0x0F12, 0x016E,	//TVAR_afit_pBaseVals[183]
    16, 0x0F12, 0x0180,	//TVAR_afit_pBaseVals[184]
    16, 0x0F12, 0x0127,	//TVAR_afit_pBaseVals[185]
    16, 0x0F12, 0x8058,	//TVAR_afit_pBaseVals[186]
    16, 0x0F12, 0x0180,	//TVAR_afit_pBaseVals[187]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[188]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[189]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[190]
    16, 0x0F12, 0x0005,	//TVAR_afit_pBaseVals[191]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[192]
    16, 0x0F12, 0xFF80,	//TVAR_afit_pBaseVals[193]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[194]
    16, 0x0F12, 0x0008,	//TVAR_afit_pBaseVals[195]
    16, 0x0F12, 0x0008,	//TVAR_afit_pBaseVals[196]
    16, 0x0F12, 0x00A0,	//TVAR_afit_pBaseVals[197]
    16, 0x0F12, 0x00A0,	//TVAR_afit_pBaseVals[198]
    16, 0x0F12, 0x0004,	//TVAR_afit_pBaseVals[199]
    16, 0x0F12, 0x003C,	//TVAR_afit_pBaseVals[200]
    16, 0x0F12, 0x000A,	//TVAR_afit_pBaseVals[201]
    16, 0x0F12, 0x001E,	//TVAR_afit_pBaseVals[202]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[203]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[204]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[205]
    16, 0x0F12, 0x0014,	//TVAR_afit_pBaseVals[206]
    16, 0x0F12, 0x00F4,	//TVAR_afit_pBaseVals[207]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[208]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[209]
    16, 0x0F12, 0x0064,	//TVAR_afit_pBaseVals[210]
    16, 0x0F12, 0x0064,	//TVAR_afit_pBaseVals[211]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[212]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[213]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[214]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[215]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[216]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[217]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[218]
    16, 0x0F12, 0x0362,	//TVAR_afit_pBaseVals[219]
    16, 0x0F12, 0x0505,	//TVAR_afit_pBaseVals[220]
    16, 0x0F12, 0x1919,	//TVAR_afit_pBaseVals[221]
    16, 0x0F12, 0x0303,	//TVAR_afit_pBaseVals[222]
    16, 0x0F12, 0x0303,	//TVAR_afit_pBaseVals[223]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[224]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[225]
    16, 0x0F12, 0x2828,	//TVAR_afit_pBaseVals[226]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[227]
    16, 0x0F12, 0x020A,	//TVAR_afit_pBaseVals[228]
    16, 0x0F12, 0x0880,	//TVAR_afit_pBaseVals[229]
    16, 0x0F12, 0x0205,	//TVAR_afit_pBaseVals[230]
    16, 0x0F12, 0x0305,	//TVAR_afit_pBaseVals[231]
    16, 0x0F12, 0x1103,	//TVAR_afit_pBaseVals[232]
    16, 0x0F12, 0x0F11,	//TVAR_afit_pBaseVals[233]
    16, 0x0F12, 0x050F,	//TVAR_afit_pBaseVals[234]
    16, 0x0F12, 0x0805,	//TVAR_afit_pBaseVals[235]
    16, 0x0F12, 0x1212,	//TVAR_afit_pBaseVals[236]
    16, 0x0F12, 0x0909,	//TVAR_afit_pBaseVals[237]
    16, 0x0F12, 0x0505,	//TVAR_afit_pBaseVals[238]
    16, 0x0F12, 0x0208,	//TVAR_afit_pBaseVals[239]
    16, 0x0F12, 0x0404,	//TVAR_afit_pBaseVals[240]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[241]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[242]
    16, 0x0F12, 0x0604,	//TVAR_afit_pBaseVals[243]
    16, 0x0F12, 0x0600,	//TVAR_afit_pBaseVals[244]
    16, 0x0F12, 0x100A,	//TVAR_afit_pBaseVals[245]
    16, 0x0F12, 0x016E,	//TVAR_afit_pBaseVals[246]
    16, 0x0F12, 0x0180,	//TVAR_afit_pBaseVals[247]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[248]
    16, 0x0F12, 0x8058,	//TVAR_afit_pBaseVals[249]
    16, 0x0F12, 0x0180,	//TVAR_afit_pBaseVals[250]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[251]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[252]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[253]
    16, 0x0F12, 0x0005,	//TVAR_afit_pBaseVals[254]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[255]
    16, 0x0F12, 0xFF80,	//TVAR_afit_pBaseVals[256]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[257]
    16, 0x0F12, 0x0006,	//TVAR_afit_pBaseVals[258]
    16, 0x0F12, 0x0006,	//TVAR_afit_pBaseVals[259]
    16, 0x0F12, 0x0028,	//TVAR_afit_pBaseVals[260]
    16, 0x0F12, 0x0028,	//TVAR_afit_pBaseVals[261]
    16, 0x0F12, 0x0004,	//TVAR_afit_pBaseVals[262]
    16, 0x0F12, 0x003C,	//TVAR_afit_pBaseVals[263]
    16, 0x0F12, 0x000A,	//TVAR_afit_pBaseVals[264]
    16, 0x0F12, 0x001E,	//TVAR_afit_pBaseVals[265]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[266]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[267]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[268]
    16, 0x0F12, 0x0014,	//TVAR_afit_pBaseVals[269]
    16, 0x0F12, 0x00F4,	//TVAR_afit_pBaseVals[270]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[271]
    16, 0x0F12, 0x03FF,	//TVAR_afit_pBaseVals[272]
    16, 0x0F12, 0x0064,	//TVAR_afit_pBaseVals[273]
    16, 0x0F12, 0x0064,	//TVAR_afit_pBaseVals[274]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[275]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[276]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[277]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[278]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[279]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[280]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[281]
    16, 0x0F12, 0x0362,	//TVAR_afit_pBaseVals[282]
    16, 0x0F12, 0x0505,	//TVAR_afit_pBaseVals[283]
    16, 0x0F12, 0x1919,	//TVAR_afit_pBaseVals[284]
    16, 0x0F12, 0x0202,	//TVAR_afit_pBaseVals[285]
    16, 0x0F12, 0x0303,	//TVAR_afit_pBaseVals[286]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[287]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[288]
    16, 0x0F12, 0x2828,	//TVAR_afit_pBaseVals[289]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[290]
    16, 0x0F12, 0x0205,	//TVAR_afit_pBaseVals[291]
    16, 0x0F12, 0x0880,	//TVAR_afit_pBaseVals[292]
    16, 0x0F12, 0x0205,	//TVAR_afit_pBaseVals[293]
    16, 0x0F12, 0x0303,	//TVAR_afit_pBaseVals[294]
    16, 0x0F12, 0x1103,	//TVAR_afit_pBaseVals[295]
    16, 0x0F12, 0x0F11,	//TVAR_afit_pBaseVals[296]
    16, 0x0F12, 0x050F,	//TVAR_afit_pBaseVals[297]
    16, 0x0F12, 0x0805,	//TVAR_afit_pBaseVals[298]
    16, 0x0F12, 0x1212,	//TVAR_afit_pBaseVals[299]
    16, 0x0F12, 0x0909,	//TVAR_afit_pBaseVals[300]
    16, 0x0F12, 0x0505,	//TVAR_afit_pBaseVals[301]
    16, 0x0F12, 0x0208,	//TVAR_afit_pBaseVals[302]
    16, 0x0F12, 0x0404,	//TVAR_afit_pBaseVals[303]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[304]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[305]
    16, 0x0F12, 0x0604,	//TVAR_afit_pBaseVals[306]
    16, 0x0F12, 0x0600,	//TVAR_afit_pBaseVals[307]
    16, 0x0F12, 0x100A,	//TVAR_afit_pBaseVals[308]
    16, 0x0F12, 0x0180,	//TVAR_afit_pBaseVals[309]
    16, 0x0F12, 0x0180,	//TVAR_afit_pBaseVals[310]
    16, 0x0F12, 0x0100,	//TVAR_afit_pBaseVals[311]
    16, 0x0F12, 0x8058,	//TVAR_afit_pBaseVals[312]
    16, 0x0F12, 0x0180,	//TVAR_afit_pBaseVals[313]
    16, 0x0F12, 0x0000,	//TVAR_afit_pBaseVals[314]
    //	param_end	TVAR_afit_pBaseVals
    //	param_start	afit_pConstBaseVals
    16, 0x0F12, 0x00FF,	//afit_pConstBaseVals[0]
    16, 0x0F12, 0x00FF,	//afit_pConstBaseVals[1]
    16, 0x0F12, 0x0500,	//afit_pConstBaseVals[2]
    16, 0x0F12, 0x03C0,	//afit_pConstBaseVals[3]
    16, 0x0F12, 0x0000,	//afit_pConstBaseVals[4]
    16, 0x0F12, 0x0000,	//afit_pConstBaseVals[5]
    16, 0x0F12, 0x0008,	//afit_pConstBaseVals[6]
    16, 0x0F12, 0x0300,	//afit_pConstBaseVals[7]
    16, 0x0F12, 0x0002,	//afit_pConstBaseVals[8]
    16, 0x0F12, 0x0400,	//afit_pConstBaseVals[9]
    16, 0x0F12, 0x0106,	//afit_pConstBaseVals[10]
    16, 0x0F12, 0x0005,	//afit_pConstBaseVals[11]
    16, 0x0F12, 0x0000,	//afit_pConstBaseVals[12]
    16, 0x0F12, 0x0003,	//afit_pConstBaseVals[13]
    16, 0x0F12, 0x0000,	//afit_pConstBaseVals[14]
    16, 0x0F12, 0xFDD6,	//afit_pConstBaseVals[15]
    16, 0x0F12, 0xF3D1,	//afit_pConstBaseVals[16]
    16, 0x0F12, 0xE1FE,	//afit_pConstBaseVals[17]
    16, 0x0F12, 0x0001,	//afit_pConstBaseVals[18]
    //	param_end	afit_pConstBaseVals


    // End tuning part
    16, 0x1000, 0x0001,	// Set host interrupt so main start run
    00, 0xffff, 10,	// Wait10mSec
    // Start user init script

    // End user init script
    ///////////////////////////////////////////
    //clk Settings
    16, 0x002A, 0x01BA,
    16, 0x0F12, 0x6978,	//Input=0x6978: 27MHz //0x5DC0 :24MHz  not use 24.75MHz:0x60AE
    16, 0x0F12, 0x0000,
    16, 0x002A, 0x01C2,
    16, 0x0F12, 0x0001, //2
    16, 0x002A, 0x01C8,
    16, 0x0F12, 0x2ee0,	//1st system CLK 48MHz
    16, 0x0F12, 0x5DC0,	//27MHz //2EE0 :48MHz //5DC0 :96MHz
    16, 0x0F12, 0x5DC0,	//27MHz //2EE0 :48MHz //5DC0 :96MHz
    16, 0x0F12, 0x2ee0,	//2nd system CLK 48MHz
    16, 0x0F12, 0x2ee0,	//27MHz //2EE0 :48MHz //5DC0 :96MHz
    16, 0x0F12, 0x2ee0,	//27MHz //2EE0 :48MHz //5DC0 :96MHz
    16, 0x002A, 0x01DC,
    16, 0x0F12, 0x0001,


    ////////////////////////////////////////////////////////
    //PREVIEW CONFIGURATION 0 (VGA, YUV, Fixed 5fps)
    ////////////////////////////////////////////////////////
    16, 0x002A, 0x023C,
    16, 0x0F12, 0x0280,	//640
    16, 0x0F12, 0x01E0,	//480
    16, 0x0F12, 0x0005,	//YUV
    16, 0x002A, 0x0248,
    16, 0x0F12, 0x0000,
    16, 0x002A, 0x0242,
    16, 0x0F12, 0x5DC0,	//PCLK max
    16, 0x0F12, 0x5DC0,	//PCLK min
    16, 0x0F12, 0x0042, //PVI Mask CbYCrY
    16, 0x002A, 0x024C, //frame rate quality
    16, 0x0F12, 0x0002,
    16, 0x002A, 0x024A,
    16, 0x0F12, 0x0002,
    16, 0x002A, 0x024E,
    16, 0x0F12, 0x07D0,	//029A : 15fps //01F4 : 20fps //0190 : 25fps
    16, 0x0F12, 0x07D0,

    16, 0x002A, 0x025E,
    16, 0x0F12, 0x0000 + 0x1 * MIRROR + 0x2 * FLIP, //preview mirror & flip

    ////////////////////////////////////////////////////////
    //PREVIEW CONFIGURATION 1 (VGA, YUV, fixed 30fps)
    ////////////////////////////////////////////////////////

    16, 0x002A, 0x0264,
    16, 0x0F12, 0x0280,	//640
    16, 0x0F12, 0x01E0,	//480
    16, 0x0F12, 0x0005,	//YUV
    16, 0x002A, 0x0270,
    16, 0x0F12, 0x0000, //1
    16, 0x002A, 0x026A,
    16, 0x0F12, 0x5DC0, //0x2ee0,	//PCLK max 27MHz //2EE0 :48MHz //5DC0 :96MHz
    16, 0x0F12, 0x5DC0, //0x2ee0,	//PCLK min
    16, 0x0F12, 0x0042,     //PVI Mask CbYCrY
    16, 0x002A, 0x0274, //frame rate quality
    16, 0x0F12, 0x0002, //dont know why it's 0x0002
    16, 0x002A, 0x0272, //dyanmic or fix rate
    16, 0x0F12, 0x0002, //0=dynamic, 1=fixed
    16, 0x002A, 0x0276, //frame time
    16, 0x0F12, 0x014F,	//20fps  //029A : 15fps //01F4 : 20fps //0190 : 25fps
    16, 0x0F12, 0x014F,

    16, 0x002A, 0x0286,
    16, 0x0F12, 0x0000 + 0x1 * MIRROR + 0x2 * FLIP, //preview mirror & flip

    //////////////////////////////////////////////////////////
    ////PREVIEW CONFIGURATION 2 (1280*960, YUV, 15fps)
    //////////////////////////////////////////////////////////

    16, 0x002A, 0x028C,
    16, 0x0F12, 0x0500,	//1280
    16, 0x0F12, 0x03c0,	//960
    16, 0x0F12, 0x0005,	//YUV
    16, 0x002A, 0x0298,
    16, 0x0F12, 0x0000,
    16, 0x002A, 0x0292,
    16, 0x0F12, 0x5DC0,	//PCLK max 27MHz //2EE0 :48MHz //5DC0 :96MHz
    16, 0x0F12, 0x5DC0,	//PCLK min
    16, 0x0F12, 0x0042,     //PVI Mask CbYCrY
    16, 0x002A, 0x029C,
    16, 0x0F12, 0x0002, //frame rate quality
    16, 0x002A, 0x029A,
    16, 0x0F12, 0x0000,
    16, 0x002A, 0x029E,
    16, 0x0F12, 0x029A,	//15fps  //029A : 15fps //01F4 : 20fps //0190 : 25fps
    16, 0x0F12, 0x029A,

    16, 0x002A, 0x02AE,
    16, 0x0F12, 0x0000 + 0x1 * MIRROR + 0x2 * FLIP, //preview mirror & flip
    //////////////////////////////////////////////////////////
    ////PREVIEW CONFIGURATION 3 (VGA, YUV, 15fps)
    //////////////////////////////////////////////////////////

    16, 0x002A, 0x02B4,
    16, 0x0F12, 0x0280,	//640
    16, 0x0F12, 0x01E0,	//480
    16, 0x0F12, 0x0005,	//YUV
    16, 0x002A, 0x02C0,
    16, 0x0F12, 0x0000,
    16, 0x002A, 0x02BA,
    16, 0x0F12, 0x2ee0,	//PCLK max 27MHz //0x2EE0 :48MHz //0x5DC0 :96MHz
    16, 0x0F12, 0x2ee0,	//PCLK min
    16, 0x0F12, 0x0042,     //PVI Mask CbYCrY
    16, 0x002A, 0x02C4,
    16, 0x0F12, 0x0002, //frame rate quality
    16, 0x002A, 0x02C2,
    16, 0x0F12, 0x0000,
    16, 0x002A, 0x02C6,
    16, 0x0F12, 0x029A,	//10fps  //029A : 15fps //01F4 : 20fps //0190 : 25fps
    16, 0x0F12, 0x029A,

    16, 0x002A, 0x02D6,
    16, 0x0F12, 0x0000 + 0x1 * MIRROR + 0x2 * FLIP, //preview mirror & flip
    //////////////////////////////////////////////////////////
    ////PREVIEW CONFIGURATION 4 (1280*960, YUV, Fixed 30fps)
    //////////////////////////////////////////////////////////

    16, 0x002A, 0x02DC,
    16, 0x0F12, 0x0500,	//1280
    16, 0x0F12, 0x03c0,	//960
    16, 0x0F12, 0x0005,	//YUV
    16, 0x002A, 0x02E8,
    16, 0x0F12, 0x0000,
    16, 0x002A, 0x02E2,
    16, 0x0F12, 0x5DC0,	//PCLK max 27MHz //2EE0 :48MHz //5DC0 :96MHz
    16, 0x0F12, 0x5DC0,	//PCLK min
    16, 0x0F12, 0x0042,     //PVI Mask CbYCrY
    16, 0x002A, 0x02EC,
    16, 0x0F12, 0x0002, //frame rate quality
    16, 0x002A, 0x02EA,
    16, 0x0F12, 0x0000,
    16, 0x002A, 0x02EE,
    16, 0x0F12, 0x014F,	//5fps  //029A : 15fps //01F4 : 20fps //0190 : 25fps
    16, 0x0F12, 0x014F,

    16, 0x002A, 0x02FE,
    16, 0x0F12, 0x0000 + 0x1 * MIRROR + 0x2 * FLIP, //preview mirror & flip

    ////////////////////////////////////////////////////////////
    //////Capture CONFIGURATION 0 (640*480, YUV, 10-30fps)
    ////////////////////////////////////////////////////////////
    //16,0x002A,0x0306,
    //16,0x0F12,0x0000,
    //16,0x0F12,0x0280,
    //16,0x0F12,0x01E0,
    //16,0x0F12,0x0005,
    //16,0x0F12,0x2EE0,
    //16,0x0F12,0x2EE0,
    //16,0x0F12,0x0042,
    //16,0x0F12,0x0001,//clock index
    //16,0x0F12,0x0000,//frame type
    //16,0x0F12,0x0002,//quality//0
    //16,0x0F12,0x03E8,
    //16,0x0F12,0x014F,
    //16,0x0F12,0x0000,
    //16,0x0F12,0x0000,
    //16,0x0F12,0x0000,
    //16,0x0F12,0x0000,
    //16,0x0F12,0x0000,
    //16,0x0F12,0x0000,
    //
    //16,0x002A,0x0260,
    //16,0x0F12,0x0000+0x1*MIRROR+0x2*FLIP, //capture mirror & flip
    ////////////////////////////////////////////////////////////
    //////Capture CONFIGURATION 1 (1280*960, YUV, 10~ 30fps)
    ////////////////////////////////////////////////////////////
    //16,0x002A,0x032A,
    //16,0x0F12,0x0000,
    //16,0x0F12,0x0500,
    //16,0x0F12,0x03C0,
    //16,0x0F12,0x0005,
    //16,0x0F12,0x5DC0,
    //16,0x0F12,0x5DC0,
    //16,0x0F12,0x0042,
    //16,0x0F12,0x0001,//clock index//0
    //16,0x0F12,0x0000,//frame type
    //16,0x0F12,0x0002,//quality//
    //16,0x0F12,0x03E8,
    //16,0x0F12,0x014F,
    //16,0x0F12,0x0000,
    //16,0x0F12,0x0000,
    //16,0x0F12,0x0000,
    //16,0x0F12,0x0000,
    //16,0x0F12,0x0000,
    //16,0x0F12,0x0000,
    //
    //16,0x002A,0x0288,
    //16,0x0F12,0x0000+0x1*MIRROR+0x2*FLIP, //capture mirror & flip

    ////////////////////////////////////////////////////////
    //PREVIEW
    ////////////////////////////////////////////////////////

    //16,0x002A,0x021E,
    //16,0x0F12,0x0001, 	// Select preview configuration 0 for fix 30fps
    //
    //16,0x002A,0x0222,
    //16,0x0F12,0x0001,
    //
    //16,0x002A,0x01FA,
    //16,0x0F12,0x0001,
    //
    //16,0x002A,0x0220,
    //16,0x0F12,0x0001,
    //
    //16,0x002A,0x01F0,
    //16,0x0F12,0x0001,
    //16,0x0F12,0x0001,

    16, 0xF402, 0x001D,	//aig_ana_misc_aps
};

static __u16 reg_1280_960_15000[] =
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x021E,
    16, 0x0F12, 0x0002,	// Select preview configuration 2 for fullsize 10~30fps

    16, 0x002A, 0x0222, // ??1 means need to enable output, 0 means no need to????
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x01FA, //flag when going to sync new config ?may stop something?
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x0220, //sync config
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x01F0, //enable preview output
    16, 0x0F12, 0x0001,

    16, 0x0F12, 0x0001, //sychronize FW with enable preview request

    /*========================================*/
    16, 0x0028, 0x7000,
    16, 0x002A, 0x020C,
    16, 0x0F12, 0x0500, //preview zoom width
    16, 0x0F12, 0x03C0,

    16, 0x002A, 0x01FC,
    16, 0x0F12, 0x0500, //preview input width
    16, 0x0F12, 0x03C0,
    16, 0x0F12, 0x0000, //offset X/Y
    16, 0x0F12, 0x0000, //(960-720)/2

    16, 0x002A, 0x021C, //sync fw
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x0220, //sync config
    16, 0x0F12, 0x0001,
    /*========================================*/

    16, 0xF402, 0x001D,	//aig_ana_misc_aps
    //00,0xffff,100,//p1000
};

static __u16 reg_1280_960_30000[] =
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x021E,
    16, 0x0F12, 0x0004,	// Select preview configuration 4 for fullsize fix 30fps

    16, 0x002A, 0x0222, // ??1 means need to enable output, 0 means no need to????
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x01FA, //flag when going to sync new config ?may stop something?
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x0220, //sync config
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x01F0, //enable preview output
    16, 0x0F12, 0x0001,

    16, 0x0F12, 0x0001, //sychronize FW with enable preview request

    /*========================================*/
    16, 0x0028, 0x7000,
    16, 0x002A, 0x020C,
    16, 0x0F12, 0x0500, //preview zoom width
    16, 0x0F12, 0x02D0,

    16, 0x002A, 0x01FC,
    16, 0x0F12, 0x0500, //preview input width
    16, 0x0F12, 0x03C0,
    16, 0x0F12, 0x0000, //offset X/Y
    16, 0x0F12, 0x0000, //(960-720)/2

    16, 0x002A, 0x021C, //sync fw
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x0220, //sync config
    16, 0x0F12, 0x0001,
    /*========================================*/

    16, 0xF402, 0x001D,	//aig_ana_misc_aps
    //00,0xffff,100,//p1000
};


static __u16 reg_640_480_5000[] =
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x021E,
    16, 0x0F12, 0x0000,	// Select preview configuration 0 for VGA 5fps

    16, 0x002A, 0x0222, // ??1 means need to enable output, 0 means no need to????
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x01FA, //flag when going to sync new config ?may stop something?
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x0220, //sync config
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x01F0, //enable preview output
    16, 0x0F12, 0x0001,

    16, 0x0F12, 0x0001, //sychronize FW with enable preview request

    /*========================================*/
    16, 0x0028, 0x7000,
    16, 0x002A, 0x020C,
    16, 0x0F12, 0x0280 * 2, //preview zoom width
    16, 0x0F12, 0x01E0 * 2,

    16, 0x002A, 0x01FC,
    16, 0x0F12, 0x0280 * 1, //preview input width
    16, 0x0F12, 0x01E0 * 1,
    16, 0x0F12, 0x0000, //offset X/Y
    16, 0x0F12, 0x0000, //(960-720)/2

    16, 0x002A, 0x021C, //sync fw
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x0220, //sync config
    16, 0x0F12, 0x0001,
    /*========================================*/

    16, 0xF402, 0x001D,	//aig_ana_misc_aps
    //00,0xffff,100,//p1000
};

static __u16 reg_640_480_15000[] =
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x021E,
    16, 0x0F12, 0x0003,	// Select preview configuration 3 for VGA 15fps

    16, 0x002A, 0x0222, // ??1 means need to enable output, 0 means no need to????
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x01FA, //flag when going to sync new config ?may stop something?
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x0220, //sync config
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x01F0, //enable preview output
    16, 0x0F12, 0x0001,

    16, 0x0F12, 0x0001, //sychronize FW with enable preview request

    /*========================================*/
    16, 0x0028, 0x7000,
    16, 0x002A, 0x020C,
    16, 0x0F12, 0x0280 * 2, //preview zoom width
    16, 0x0F12, 0x01E0 * 2,

    16, 0x002A, 0x01FC,
    16, 0x0F12, 0x0280 * 1, //preview input width
    16, 0x0F12, 0x01E0 * 1,
    16, 0x0F12, 0x0000, //offset X/Y
    16, 0x0F12, 0x0000, //(960-720)/2

    16, 0x002A, 0x021C, //sync fw
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x0220, //sync config
    16, 0x0F12, 0x0001,
    /*========================================*/

    16, 0xF402, 0x001D,	//aig_ana_misc_aps
    //00,0xffff,100,//p1000
};


static __u16 reg_640_480_30000[] =
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x021E,
    16, 0x0F12, 0x0001,	// Select preview configuration 1 for VGA fix 30fps

    16, 0x002A, 0x0222, // ??1 means need to enable output, 0 means no need to????
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x01FA, //flag when going to sync new config ?may stop something?
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x0220, //sync config
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x01F0, //enable preview output
    16, 0x0F12, 0x0001,

    16, 0x0F12, 0x0001, //sychronize FW with enable preview request

    /*========================================*/
    16, 0x0028, 0x7000,
    16, 0x002A, 0x020C,
    16, 0x0F12, 0x0280 * 2, //preview zoom width
    16, 0x0F12, 0x01E0 * 2,

    16, 0x002A, 0x01FC,
    16, 0x0F12, 0x0280 * 1, //preview input width
    16, 0x0F12, 0x01E0 * 1,
    16, 0x0F12, 0x0000, //offset X/Y
    16, 0x0F12, 0x0000, //(960-720)/2

    16, 0x002A, 0x021C, //sync fw
    16, 0x0F12, 0x0001,

    16, 0x002A, 0x0220, //sync config
    16, 0x0F12, 0x0001,
    /*========================================*/

    16, 0xF402, 0x001D,	//aig_ana_misc_aps
    //00,0xffff,100,//p1000
};

//static __u16 reg_1280_720_30000[]=//需要调整zoom的window in/out 720p不使用这个
//{
//16,0x0028,0x7000,
//16,0x002A,0x021E,
//16,0x0F12,0x0000,	// Select preview configuration 0 for 720p fix 30fps
//
//16,0x002A,0x0222, // ??1 means need to enable output, 0 means no need to????
//16,0x0F12,0x0001,
//
//16,0x002A,0x01FA, //flag when going to sync new config ?may stop something?
//16,0x0F12,0x0001,
//
//16,0x002A,0x0220, //sync config
//16,0x0F12,0x0001,
//
//16,0x002A,0x01F0, //enable preview output
//16,0x0F12,0x0001,
//
//16,0x0F12,0x0001, //sychronize FW with enable preview request
//
///*========================================*/
//16,0x0028,0x7000,
//16,0x002A,0x020C,
//16,0x0F12,0x0500, //preview zoom width
//16,0x0F12,0x02D0,
//
//16,0x002A,0x01FC,
//16,0x0F12,0x0500, //preview input width
//16,0x0F12,0x02D0,
//16,0x0F12,0x0000, //offset X/Y
//16,0x0F12,0x0078, //(960-720)/2
//
//16,0x002A,0x021C, //sync fw
//16,0x0F12,0x0001,
//
//16,0x002A,0x0220, //sync config
//16,0x0F12,0x0001,
///*========================================*/
//
//16,0xF402,0x001D,	//aig_ana_misc_aps
////00,0xffff,100,//p1000
//};



static __u16 reg_color_effect_normal[] =
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01EE,
    16, 0x0F12, 0x0000,
};
static __u16 reg_color_effect_grayscale[] =
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01EE,
    16, 0x0F12, 0x0001,
};
static __u16 reg_color_effect_colorinv[] =
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01EE,
    16, 0x0F12, 0x0003,
};
static __u16 reg_color_effect_sepia[] =
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01EE,
    16, 0x0F12, 0x0004,
};
static __u16 reg_color_effect_sepiagreen[] =
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01EE,
    16, 0x0F12, 0x0005,
};
static __u16 reg_color_effect_sepiablue[] =
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01EE,
    16, 0x0F12, 0x0005,
};
static __u16 reg_color_effect_sepiared[] =
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01EE,
    16, 0x0F12, 0x0005,
};
static __u16 reg_color_effect_sepiayellow[] =
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01EE,
    16, 0x0F12, 0x0006,
};


static __u16 reg_awb_auto[] =
{
    16, 0x0028, 0x7000,
    16, 0x002a, 0x040e,
    16, 0x0f12, 0x077f,
};
static __u16 reg_awb_daylight[] = //6500k
{
    //WB sunny
    16, 0x0028, 0x7000,
    16, 0x002a, 0x040e,
    16, 0x0f12, 0x0777,
    16, 0x002a, 0x03d8,
    16, 0x0f12, 0x05e0,
    16, 0x0f12, 0x0001,
    16, 0x0f12, 0x0400,
    16, 0x0f12, 0x0001,
    16, 0x0f12, 0x0530,
    16, 0x0f12, 0x0001,
};
static __u16 reg_awb_fluorescent[] = //5000k
{
    //WB Office
    16, 0x0028, 0x7000,
    16, 0x002a, 0x040e,
    16, 0x0f12, 0x0777,
    16, 0x002a, 0x03d8,
    16, 0x0f12, 0x0575,
    16, 0x0f12, 0x0001,
    16, 0x0f12, 0x0400,
    16, 0x0f12, 0x0001,
    16, 0x0f12, 0x0800,
    16, 0x0f12, 0x0001,
};
static __u16 reg_awb_cloudy[] = //7500k
{
    //WB Cloudy
    16, 0x0028, 0x7000,
    16, 0x002a, 0x040e,
    16, 0x0f12, 0x0777,
    16, 0x002a, 0x03d8,
    16, 0x0f12, 0x0740,
    16, 0x0f12, 0x0001,
    16, 0x0f12, 0x0400,
    16, 0x0f12, 0x0001,
    16, 0x0f12, 0x0460,
    16, 0x0f12, 0x0001,
};
static __u16 reg_awb_tungsten[] = //
{
    //WB HOME
    16, 0x0028, 0x7000,
    16, 0x002a, 0x040e,
    16, 0x0f12, 0x0777,
    16, 0x002a, 0x03d8,
    16, 0x0f12, 0x0400,
    16, 0x0f12, 0x0001,
    16, 0x0f12, 0x0400,
    16, 0x0f12, 0x0001,
    16, 0x0f12, 0x0940,
    16, 0x0f12, 0x0001,
};


static __u16 reg_ae_n3[] = //-3
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E0,
    16, 0x0F12, 0xFFD0,
};
static __u16 reg_ae_n2[] = //-2
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E0,
    16, 0x0F12, 0xFFE0,
};
static __u16 reg_ae_n1[] = //-1
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E0,
    16, 0x0F12, 0xFFF0,
};
static __u16 reg_ae_0[] = //0
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E0,
    16, 0x0F12, 0x0000,
};
static __u16 reg_ae_p1[] = //1
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E0,
    16, 0x0F12, 0x0010,
};
static __u16 reg_ae_p2[] = //2
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E0,
    16, 0x0F12, 0x0020,
};
static __u16 reg_ae_p3[] = //3
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E0,
    16, 0x0F12, 0x0030,
};


static __u16 reg_bright_n3[] = //-3
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E0,
    16, 0x0F12, 0xFFD0,
};
static __u16 reg_bright_n2[] = //-2
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E0,
    16, 0x0F12, 0xFFE0,
};
static __u16 reg_bright_n1[] = //-1
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E0,
    16, 0x0F12, 0xFFF0,
};
static __u16 reg_bright_0[] = //0
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E0,
    16, 0x0F12, 0x0000,
};
static __u16 reg_bright_p1[] = //1
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E0,
    16, 0x0F12, 0x0010,
};
static __u16 reg_bright_p2[] = //2
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E0,
    16, 0x0F12, 0x0020,
};
static __u16 reg_bright_p3[] = //3
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E0,
    16, 0x0F12, 0x0030,
};


static __u16 reg_contrast_n3[] = //-3
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E2,
    16, 0x0F12, 0xFFD0,
};
static __u16 reg_contrast_n2[] = //-2
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E2,
    16, 0x0F12, 0xFFE0,
};
static __u16 reg_contrast_n1[] = //-1
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E2,
    16, 0x0F12, 0xFFF0,
};
static __u16 reg_contrast_0[] = //0
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E2,
    16, 0x0F12, 0x0000,
};
static __u16 reg_contrast_p1[] = //1
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E2,
    16, 0x0F12, 0x0010,
};
static __u16 reg_contrast_p2[] = //2
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E2,
    16, 0x0F12, 0x0020,
};
static __u16 reg_contrast_p3[] = //3
{
    16, 0x0028, 0x7000,
    16, 0x002A, 0x01E2,
    16, 0x0F12, 0x0030,
};


__s32  csi_power_on(void)
{
    csi_rst(0);
    csi_pwdn(1);//pwdn=1
    csi_clk_on();
    esKRNL_TimeDly(1);//must
    csi_rst(1);
    esKRNL_TimeDly(1);//must
    return EPDK_OK;
}

__s32  csi_power_off(void)
{
    csi_rst(0);//rst=0
    //csi_pwren(0);//pwr=0
    return EPDK_OK;
}


__s32  csi_clk_on(void)
{
    csi_mclk_on_off(1);
    return EPDK_OK;
}

__s32  csi_clk_off(void)
{
    csi_mclk_on_off(0);
    return EPDK_OK;
}

__s32 block_write(ES_FILE *hdl_iic, __u8 slv_addr, __u16 *reg, __s32 length)
{
    __s32 ret = EPDK_FAIL;

    __u16 *reg_table;
    __s32 reg_table_len;

    reg_step = 3; ///必须保证第一行是3个
    reg_table = reg;
    reg_table_len = length;
    __inf("reg_table_len = %d\n", reg_table_len);

    while(reg_table_len > 0)
    {

        if (reg_table[0] == 16)
        {
            __s32 ret;
            //__inf("writing %d bit data\n",reg_table[0]);
            ret = iic_write_16_16(hdl_iic, slv_addr, reg_table[1], reg_table[2]);
            //__inf("iic_write_16_16  %x=%x, length=%d\n",reg_table[1],reg_table[2],reg_table_len);
            if(ret != EPDK_OK)
            {
                __wrn("iic_write_16_16 fail! %x=%x\n", reg_table[1], reg_table[2]);
                break;
            }
            reg_step = 3;
        }

        else if (reg_table[0] == 32)
        {
            __s32 ret;
            //__inf("writing %d bit data\n",reg_table[0]);
            ret = iic_write_16_32(hdl_iic, slv_addr, reg_table[1], reg_table[2] << 16 | reg_table[3]);
            if(ret != EPDK_OK)
            {
                __wrn("iic_write_16_32 fail! %x=%x\n", reg_table[1], reg_table[2] << 16 | reg_table[3]);
                break;
            }
            reg_step = 4;
        }

        else if(reg_table[0] == 0)
        {
            if(reg_table[1] == 0xffff)
            {
                __inf("wait %dms\n", reg_table[2]);
                esKRNL_TimeDly(reg_table[2] / 10);
            }
            reg_step = 3;
        }

        else
        {
            __wrn("reading data array not in correct sequence!\n");
            break;
        }

        reg_table = reg_table + reg_step;
        //	  __inf("reg_table = %x, reg_table[0]=%d, reg_table[1]=%x\n", reg_table, reg_table[0], reg_table[1]);
        reg_table_len = reg_table_len - reg_step;
        //	  __inf("reg_table_len = %d\n\n", reg_table_len);
        //    __here__
        //    __getc();
    }
    ret = EPDK_OK;
    __inf("reg_table_len = %d\n\n", reg_table_len);
    return ret;
}


//设备模式设置函数，配置设备，同时根据设备配置相应的csi设置
__s32 csi_dev_set_mode(__u32 mode)
{
    __u16 *reg_table;
    __u32 reg_table_len;

    __log("*********************************************\n");
    __log("***********SELECT CSI MODE %d *****************\n\n", mode);
    __log("**********************************************\n");

    if (last_mode_num == mode)
    {
        __inf("no need to change csi mode!\n");
    }

    else
    {
        csi_mode = csi_mode_all.csi_mode[mode];

        switch (mode)
        {
        case 0:
        {
            reg_table = reg_640_480_15000;
            reg_table_len = sizeof(reg_640_480_15000);
            conf.input_fmt  = CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV422;//CSI_PLANAR_YUV420;//420 for encode;422 for picture
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_YUYV;
            conf.vref       = CSI_HIGH;//CSI_HIGH
            conf.href       = CSI_HIGH;//CSI_LOW
            conf.clock      = CSI_RISING;
            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width);
            break;
        }
        case 1:
        {
            reg_table = reg_640_480_30000;
            reg_table_len = sizeof(reg_640_480_30000);
            conf.input_fmt  = CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV420;//420 for encode;422 for picture
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_YUYV;
            conf.vref       = CSI_HIGH;//CSI_HIGH
            conf.href       = CSI_HIGH;//CSI_LOW
            conf.clock      = CSI_RISING;
            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width);
            break;
        }
        case 2:
        {
            reg_table = reg_1280_960_15000;//
            reg_table_len = sizeof(reg_1280_960_15000);
            conf.input_fmt  = CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV422;//CSI_PLANAR_YUV422;//420 for encode;422 for picture
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_YUYV;
            conf.vref       = CSI_HIGH;
            conf.href       = CSI_HIGH;
            conf.clock      = CSI_RISING;
            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width);
            break;
        }
        case 3:
        {
            reg_table = reg_1280_960_30000;/*注意这里720p是CSI crop出来的*/
            reg_table_len = sizeof(reg_1280_960_30000);//reg_1280_720_30000
            conf.input_fmt  = CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV420;//420 for encode;422 for picture
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_YUYV;
            conf.vref       = CSI_HIGH;//CSI_HIGH
            conf.href       = CSI_HIGH;//CSI_LOW
            conf.clock      = CSI_RISING;
            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0x78/*(960-720)/2=120*/, csi_mode.size.height, csi_mode.size.width);
            break;
        }
        case 4:
        {
            reg_table = reg_640_480_15000;
            reg_table_len = sizeof(reg_640_480_15000);
            conf.input_fmt  = CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV420;//420 for encode;422 for picture
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_YUYV;
            conf.vref       = CSI_HIGH;//CSI_HIGH
            conf.href       = CSI_HIGH;//CSI_LOW
            conf.clock      = CSI_RISING;

            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width);

            break;
        }
        case 5:
        {
            reg_table = reg_1280_960_15000;/*注意这里720p是CSI crop出来的*/
            reg_table_len = sizeof(reg_1280_960_15000);
            conf.input_fmt  = CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV420;//420 for encode;422 for picture
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_YUYV;
            conf.vref       = CSI_HIGH;//CSI_HIGH
            conf.href       = CSI_HIGH;//CSI_LOW
            conf.clock      = CSI_RISING;

            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0x78/*(960-720)/2=120*/, csi_mode.size.height, csi_mode.size.width);

            break;
        }
        //mode 6  for pc cam
        case 6:
        {
            reg_table = reg_640_480_30000;
            reg_table_len = sizeof(reg_640_480_30000);
            conf.input_fmt  = CSI_RAW;
            conf.output_fmt = CSI_PASS_THROUTH;
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_YUYV;
            conf.vref       = CSI_HIGH;
            conf.href       = CSI_HIGH;
            conf.clock      = CSI_RISING;

            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width * 2);
            break;
        }
        case 7:
        {
            reg_table = reg_640_480_5000;
            reg_table_len = sizeof(reg_640_480_5000);
            conf.input_fmt  = CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV420;//420 for encode;422 for picture
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_YUYV;
            conf.vref       = CSI_HIGH;//CSI_HIGH
            conf.href       = CSI_HIGH;//CSI_LOW
            conf.clock      = CSI_RISING;
            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width);
            break;
        }
        }

        //设备的设置
        block_write(fiic, da, reg_table, reg_table_len / 2);

        if(1)
        {
            csi_dev_set_color_effect(csi_color_effect);
            csi_dev_set_awb(csi_awb);
            csi_dev_set_ae(csi_ae);
            csi_dev_set_bright(csi_bright);
            csi_dev_set_contrast(csi_contrast);
            csi_dev_set_band(csi_band);
        }

        last_buf_mode = csi_mode.csi_buf_scrab_mode;
        last_mode_num = mode;
        __inf("set mode %d finished! buf_scrab_mode=%d (0=CSI0, 2=01LR, 4=01UD)\n", mode, last_buf_mode);
    }

    return EPDK_OK;
}



__s32  csi_dev_init(void)
{
    //	__u32 i,clock = 0;
    //	__u32 reg_size;
    //填写本设备支持的模式
    csi_mode_all.number = 8;//所支持模式数量
    //这些设置都是指输出的数据参数，为了传递给显示或者编码时的帧参数
    //---------------only csi0 mode--------------------------
    //--------------------------------------------------------------------------------------------------------------
    //640*480@30#yuv422
    csi_mode_all.csi_mode[0].color_format = PIXEL_YUV422;//格式
    csi_mode_all.csi_mode[0].component_seq = YUV_SEQ_UVUV;//排列顺序
    csi_mode_all.csi_mode[0].store_mode = YUV_MOD_UV_NON_MB_COMBINED;//YUV_MOD_NON_MB_PLANAR;//存储格式
    csi_mode_all.csi_mode[0].size.width = 640;//宽
    csi_mode_all.csi_mode[0].size.height = 480;//高
    csi_mode_all.csi_mode[0].frame_rate = 30000;//帧率，单位Hz
    csi_mode_all.csi_mode[0].frame_period = 33333;//帧长（=1/帧率，单位us）
    csi_mode_all.csi_mode[0].color_space = BT601;//色彩空间
    csi_mode_all.csi_mode[0].csi_buf_scrab_mode = CSI0_FULL;
    //640*480@30#yuv420
    csi_mode_all.csi_mode[1].color_format = PIXEL_YUV420;
    csi_mode_all.csi_mode[1].component_seq = YUV_SEQ_UVUV;
    csi_mode_all.csi_mode[1].store_mode = YUV_MOD_UV_NON_MB_COMBINED;
    csi_mode_all.csi_mode[1].size.width = 640;
    csi_mode_all.csi_mode[1].size.height = 480;
    csi_mode_all.csi_mode[1].frame_rate = 30000;
    csi_mode_all.csi_mode[1].frame_period = 33333;
    csi_mode_all.csi_mode[1].color_space = BT601;
    csi_mode_all.csi_mode[1].csi_buf_scrab_mode = CSI0_FULL;
    //1280*960@30#yuv422
    csi_mode_all.csi_mode[2].color_format = PIXEL_YUV422;//格式
    csi_mode_all.csi_mode[2].component_seq = YUV_SEQ_UVUV;//排列顺序
    csi_mode_all.csi_mode[2].store_mode = YUV_MOD_UV_NON_MB_COMBINED;//YUV_MOD_NON_MB_PLANAR;//存储格式
    csi_mode_all.csi_mode[2].size.width = 1280;//宽
    csi_mode_all.csi_mode[2].size.height = 960;//高
    csi_mode_all.csi_mode[2].frame_rate = 30000;//帧率，单位Hz
    csi_mode_all.csi_mode[2].frame_period = 33333;//帧长（=1/帧率，单位us）
    csi_mode_all.csi_mode[2].color_space = BT601;//色彩空间
    csi_mode_all.csi_mode[2].csi_buf_scrab_mode = CSI0_FULL;
    //1280*720@30#yuv420
    csi_mode_all.csi_mode[3].color_format = PIXEL_YUV420;
    csi_mode_all.csi_mode[3].component_seq = YUV_SEQ_UVUV;
    csi_mode_all.csi_mode[3].store_mode = YUV_MOD_UV_NON_MB_COMBINED;
    csi_mode_all.csi_mode[3].size.width = 1280;
    csi_mode_all.csi_mode[3].size.height = 720;
    csi_mode_all.csi_mode[3].frame_rate = 30000;
    csi_mode_all.csi_mode[3].frame_period = 33333;
    csi_mode_all.csi_mode[3].color_space = BT601;
    csi_mode_all.csi_mode[3].csi_buf_scrab_mode = CSI0_FULL;
    //========================15fps=======================================
    //640*480@30#yuv420
    csi_mode_all.csi_mode[4].color_format = PIXEL_YUV420;
    csi_mode_all.csi_mode[4].component_seq = YUV_SEQ_UVUV;
    csi_mode_all.csi_mode[4].store_mode = YUV_MOD_UV_NON_MB_COMBINED;
    csi_mode_all.csi_mode[4].size.width = 640;
    csi_mode_all.csi_mode[4].size.height = 480;
    csi_mode_all.csi_mode[4].frame_rate = 15000;
    csi_mode_all.csi_mode[4].frame_period = 33333 * 2;
    csi_mode_all.csi_mode[4].color_space = BT601;
    csi_mode_all.csi_mode[4].csi_buf_scrab_mode = CSI0_FULL;
    //1280*720@30#yuv420
    csi_mode_all.csi_mode[5].color_format = PIXEL_YUV420;
    csi_mode_all.csi_mode[5].component_seq = YUV_SEQ_UVUV;
    csi_mode_all.csi_mode[5].store_mode = YUV_MOD_UV_NON_MB_COMBINED;
    csi_mode_all.csi_mode[5].size.width = 1280;
    csi_mode_all.csi_mode[5].size.height = 720;
    csi_mode_all.csi_mode[5].frame_rate = 15000;
    csi_mode_all.csi_mode[5].frame_period = 33333 * 2;
    csi_mode_all.csi_mode[5].color_space = BT601;
    csi_mode_all.csi_mode[5].csi_buf_scrab_mode = CSI0_FULL;
    //=========================for pc cam=======================================
    //640*480@30#RAW//tmp change for pc cam csi0
    csi_mode_all.csi_mode[6].color_format = PIXEL_YUV422;//格式
    csi_mode_all.csi_mode[6].component_seq = YUV_SEQ_YUYV;//排列顺序
    csi_mode_all.csi_mode[6].store_mode = YUV_MOD_INTERLEAVED;//存储格式
    csi_mode_all.csi_mode[6].size.width = 640;//宽
    csi_mode_all.csi_mode[6].size.height = 480;//高
    csi_mode_all.csi_mode[6].frame_rate = 30000;//帧率，单位Hz
    csi_mode_all.csi_mode[6].frame_period = 33333;//帧长（=1/帧率，单位us）
    csi_mode_all.csi_mode[6].color_space = BT601;//色彩空间
    csi_mode_all.csi_mode[6].csi_buf_scrab_mode = CSI0_FULL;
    //640*480@5#yuv420
    csi_mode_all.csi_mode[7].color_format = PIXEL_YUV420;
    csi_mode_all.csi_mode[7].component_seq = YUV_SEQ_UVUV;
    csi_mode_all.csi_mode[7].store_mode = YUV_MOD_UV_NON_MB_COMBINED;
    csi_mode_all.csi_mode[7].size.width = 640;
    csi_mode_all.csi_mode[7].size.height = 480;
    csi_mode_all.csi_mode[7].frame_rate = 30000 / 6;
    csi_mode_all.csi_mode[7].frame_period = 33333 * 6;
    csi_mode_all.csi_mode[7].color_space = BT601;
    csi_mode_all.csi_mode[7].csi_buf_scrab_mode = CSI0_FULL;

    iic_set_clock(fiic, 200 * 1000);
    //================reset sensor=======
    csi_power_off();
    csi_power_on();

    // 	block_write(fiic, da, reg_init, sizeof(reg_init)/2);
    block_write(fiic, da, reg_init__, sizeof(reg_init__) / 2);

    csi_dev_set_mode(1);//不要忘记设置一个默认模式

    return EPDK_OK;
}

__s32  csi_dev_exit(void)
{
    csi_clk_off();
    csi_power_off();

    return EPDK_OK;
}


//设备color effect设置函数
__s32 csi_dev_set_color_effect(__csi_color_effect_t color_effect)
{
    __u16 *reg_table;
    __u32 reg_table_len;

    switch (color_effect)
    {
    case CSI_COLOR_EFFECT_NORMAL:
    {
        reg_table = reg_color_effect_normal;
        reg_table_len = sizeof(reg_color_effect_normal);
        break;
    }
    case CSI_COLOR_EFFECT_GRAYSCALE:
    {
        reg_table = reg_color_effect_grayscale;
        reg_table_len = sizeof(reg_color_effect_grayscale);
        break;
    }
    case CSI_COLOR_EFFECT_COLORINV:
    {
        reg_table = reg_color_effect_colorinv;
        reg_table_len = sizeof(reg_color_effect_colorinv);
        break;
    }
    case CSI_COLOR_EFFECT_SEPIA:
    {
        reg_table = reg_color_effect_sepia;
        reg_table_len = sizeof(reg_color_effect_sepia);
        break;
    }
    case CSI_COLOR_EFFECT_SEPIAGREEN:
    {
        reg_table = reg_color_effect_sepiagreen;
        reg_table_len = sizeof(reg_color_effect_sepiagreen);
        break;
    }
    case CSI_COLOR_EFFECT_SEPIABLUE:
    {
        reg_table = reg_color_effect_sepiablue;
        reg_table_len = sizeof(reg_color_effect_sepiablue);
        break;
    }
    case CSI_COLOR_EFFECT_SEPIARED:
    {
        reg_table = reg_color_effect_sepiared;
        reg_table_len = sizeof(reg_color_effect_sepiared);
        break;
    }
    case CSI_COLOR_EFFECT_SEPIAYELLOW:
    {
        reg_table = reg_color_effect_sepiayellow;
        reg_table_len = sizeof(reg_color_effect_sepiayellow);
        break;
    }
    }

    block_write(fiic, da, reg_table, reg_table_len / 2);

    return EPDK_OK;
}
//设备awb设置函数
__s32 csi_dev_set_awb(__csi_awb_t awb)
{
    __u16 *reg_table;
    __u32 reg_table_len;

    switch (awb)
    {
    case CSI_AWB_AUTO:
    {
        reg_table = reg_awb_auto;
        reg_table_len = sizeof(reg_awb_auto);
        break;
    }
    case CSI_AWB_DAYLIGHT:
    {
        reg_table = reg_awb_daylight;
        reg_table_len = sizeof(reg_awb_daylight);
        break;
    }
    case CSI_AWB_FLUORESCENT:
    {
        reg_table = reg_awb_fluorescent;
        reg_table_len = sizeof(reg_awb_fluorescent);
        break;
    }
    case CSI_AWB_CLOUDY:
    {
        reg_table = reg_awb_cloudy;
        reg_table_len = sizeof(reg_awb_cloudy);
        break;
    }
    case CSI_AWB_TUNGSTEN:
    {
        reg_table = reg_awb_tungsten;
        reg_table_len = sizeof(reg_awb_tungsten);
        break;
    }
    }

    block_write(fiic, da, reg_table, reg_table_len / 2);

    return EPDK_OK;
}
//设备ae设置函数
__s32 csi_dev_set_ae(__csi_ae_t ae)
{
    __u16 *reg_table;
    __u32 reg_table_len;

    switch (ae)
    {
    case CSI_AE_N3:
    {
        reg_table = reg_ae_n3;
        reg_table_len = sizeof(reg_ae_n3);
        break;
    }
    case CSI_AE_N2:
    {
        reg_table = reg_ae_n2;
        reg_table_len = sizeof(reg_ae_n2);
        break;
    }
    case CSI_AE_N1:
    {
        reg_table = reg_ae_n1;
        reg_table_len = sizeof(reg_ae_n1);
        break;
    }
    case CSI_AE_0:
    {
        reg_table = reg_ae_0;
        reg_table_len = sizeof(reg_ae_0);
        break;
    }
    case CSI_AE_P1:
    {
        reg_table = reg_ae_p1;
        reg_table_len = sizeof(reg_ae_p1);
        break;
    }
    case CSI_AE_P2:
    {
        reg_table = reg_ae_p2;
        reg_table_len = sizeof(reg_ae_p2);
        break;
    }
    case CSI_AE_P3:
    {
        reg_table = reg_ae_p3;
        reg_table_len = sizeof(reg_ae_p3);
        break;
    }
    }
    block_write(fiic, da, reg_table, reg_table_len / 2);
    return EPDK_OK;
}
//设备bright设置函数
__s32 csi_dev_set_bright(__csi_bright_t bright)
{
    __u16 *reg_table;
    __u32 reg_table_len;

    switch (bright)
    {
    case CSI_BRIGHT_N3:
    {
        reg_table = reg_bright_n3;
        reg_table_len = sizeof(reg_bright_n3);
        break;
    }
    case CSI_BRIGHT_N2:
    {
        reg_table = reg_bright_n2;
        reg_table_len = sizeof(reg_bright_n2);
        break;
    }
    case CSI_BRIGHT_N1:
    {
        reg_table = reg_bright_n1;
        reg_table_len = sizeof(reg_bright_n1);
        break;
    }
    case CSI_BRIGHT_0:
    {
        reg_table = reg_bright_0;
        reg_table_len = sizeof(reg_bright_0);
        break;
    }
    case CSI_BRIGHT_P1:
    {
        reg_table = reg_bright_p1;
        reg_table_len = sizeof(reg_bright_p1);
        break;
    }
    case CSI_BRIGHT_P2:
    {
        reg_table = reg_bright_p2;
        reg_table_len = sizeof(reg_bright_p2);
        break;
    }
    case CSI_BRIGHT_P3:
    {
        reg_table = reg_bright_p3;
        reg_table_len = sizeof(reg_bright_p3);
        break;
    }
    }
    block_write(fiic, da, reg_table, reg_table_len / 2);
    return EPDK_OK;
}

//设备contrast设置函数
__s32 csi_dev_set_contrast(__csi_contrast_t contrast)
{
    __u16 *reg_table;
    __u32 reg_table_len;

    switch (contrast)
    {
    case CSI_CONTRAST_N3:
    {
        reg_table = reg_contrast_n3;
        reg_table_len = sizeof(reg_contrast_n3);
        break;
    }
    case CSI_CONTRAST_N2:
    {
        reg_table = reg_contrast_n2;
        reg_table_len = sizeof(reg_contrast_n2);
        break;
    }
    case CSI_CONTRAST_N1:
    {
        reg_table = reg_contrast_n1;
        reg_table_len = sizeof(reg_contrast_n1);
        break;
    }
    case CSI_CONTRAST_0:
    {
        reg_table = reg_contrast_0;
        reg_table_len = sizeof(reg_contrast_0);
        break;
    }
    case CSI_CONTRAST_P1:
    {
        reg_table = reg_contrast_p1;
        reg_table_len = sizeof(reg_contrast_p1);
        break;
    }
    case CSI_CONTRAST_P2:
    {
        reg_table = reg_contrast_p2;
        reg_table_len = sizeof(reg_contrast_p2);
        break;
    }
    case CSI_CONTRAST_P3:
    {
        reg_table = reg_contrast_p3;
        reg_table_len = sizeof(reg_contrast_p3);
        break;
    }
    }
    block_write(fiic, da, reg_table, reg_table_len / 2);
    return EPDK_OK;
}


//设备band设置函数
__s32 csi_dev_set_band(__csi_band_t band)
{
    __u16 *reg_table;
    __u32 reg_table_len;

    __u16 reg_band_no[] =
    {
        16, 0x0028, 0x7000,
        16, 0x002A, 0x040E,
        16, 0x0F12, 0x077F,
        16, 0x002A, 0x03FC,
        16, 0x0F12, 0x0000,
        16, 0x0F12, 0x0001,
    };
    __u16 reg_band_50hz[] =
    {
        16, 0x0028, 0x7000,
        16, 0x002A, 0x040E,
        16, 0x0F12, 0x075F,
        16, 0x002A, 0x03FC,
        16, 0x0F12, 0x0001,
        16, 0x0F12, 0x0001,
    };

    __u16 reg_band_60hz[] =
    {
        16, 0x0028, 0x7000,
        16, 0x002A, 0x040E,
        16, 0x0F12, 0x075F,
        16, 0x002A, 0x03FC,
        16, 0x0F12, 0x0002,
        16, 0x0F12, 0x0001,
    };

    switch (band)
    {
    //        case CSI_BAND_AUTO:{
    //        	  reg_table = reg_band_auto;
    //            reg_table_len = sizeof(reg_band_auto);
    //            break;
    //        }
    case CSI_BAND_50HZ:
    {
        reg_table = reg_band_50hz;
        reg_table_len = sizeof(reg_band_50hz);
        break;
    }
    case CSI_BAND_60HZ:
    {
        reg_table = reg_band_60hz;
        reg_table_len = sizeof(reg_band_60hz);
        break;
    }
    }

    block_write(fiic, da, reg_table, reg_table_len / 2);

    return EPDK_OK;
}


#endif

