
#include  "csi_dev_i.h"

#if(CSI_DEV_TYPE == CSI_DEV_TYPE_OV2643)


#define MIRROR 0//0 = normal, 1 = mirror
#define FLIP   0//0 = normal, 1 = flip

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

__u32 csi_mclk = 24; //MHz

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

//==================全局变量但仅在此文件中用到========================
__u32  last_mode_num = 0xff;
__buf_scrab_mode_t  last_buf_mode = CSI_SC_NA;//区分单双csi工作状态，last_buf_mode为双，切换不reset power，单->单不reset，单->双reset一次

static __u8  da = 0x30;//设备地址，为i2c写地址左移 1 bit
static __u32 reg_step = 2; //一次写操作所包含的寄存器地址+寄存器值的byte数
//====================================================================

static __u8 reg_640_480_30000[] = //org
{
    //;pclk=72mhz,30fps/pclk=36mhz,15fps
    0x12, 0x80,
    0xc3, 0x1f,
    0xc4, 0xff,
    0x3d, 0x48,
    0xdd, 0xa5,
    //;windows setup
    0x20, 0x01,
    0x21, 0x98,
    0x22, 0x00,
    0x23, 0x06,
    0x24, 0x28, //;0x280=640
    0x25, 0x04,
    0x26, 0x1e, //;0x1e0=480
    0x27, 0x04,
    0x28, 0x40,
    //;format setting
    0x12, 0x09 + MIRROR * 0x20 + FLIP * 0x10, //mirror and flip
    0x39, 0xd0,
    0xcd, 0x13,
    //;frame setting
    0x0e, 0x10, //clock 48Mhz PCLK
    0x0f, 0x14,
    0x10, 0x0a,
    0x11, 0x00,

    0x29, 0x07, //;dummy pixels//0x29,0x07,//->ov setting //0x29,0x04,//F18
    0x2a, 0xd0,              //0x2a,0x93,//->ov setting //0x2a,0xce,//F18
    0x2b, 0x02, //;dummy lines //0x2b,0x02,//->ov setting //0x2b,0x02,//F18
    0x2c, 0x6a,              //0x2c,0x6a,//->ov setting //0x2c,0x8a,//F18->24MHz
    //for 25fps 0x26a*1.2=0x2e6
    0x1c, 0x25, //vsync width
    0x1d, 0x02,
    0x1e, 0x00,
    0x1f, 0xb9,


    //common part except 0x3d,
    0x13, 0xff,
    0x14, 0x87, //band filter bit7: 1:50Hz 0:60Hz bit4:
    0x15, 0x42,
    0x3c, 0xa4,
    0x18, 0x78, //set to default then set to csi_ae value to wake up quickly
    0x19, 0x68, //
    0x1a, 0x71,
    0x37, 0xe8,
    0x16, 0x90,
    0x43, 0x00,
    0x40, 0xfb,
    0xa9, 0x44,
    0x2f, 0xec,
    0x35, 0x10,
    0x36, 0x10,
    0x0c, 0x00,
    0x0d, 0x00,
    0xd0, 0x93,
    0xdc, 0x2b,
    0xd9, 0x41,
    0xd3, 0x02,
    0xde, 0x7c,
    0x3d, 0x08,
    0x0c, 0x00,
    0x9b, 0x69,
    0x9c, 0x7d,
    0x9d, 0x7d,
    0x9e, 0x69,
    0x35, 0x04,
    0x36, 0x04,
    //;gamma
    //normal  ////enhanced////zqh3    ////zqh2    ////zqh1    ////reset   //
    0x65, 0x04, //0x65,0x12,//0x65,0x04,//0x65,0x04,//0x65,0x07,//0x65,0x05,//
    0x66, 0x07, //0x66,0x20,//0x66,0x07,//0x66,0x07,//0x66,0x12,//0x66,0x0c,//
    0x67, 0x19, //0x67,0x39,//0x67,0x19,//0x67,0x19,//0x67,0x1f,//0x67,0x1c,//
    0x68, 0x34, //0x68,0x4e,//0x68,0x34,//0x68,0x34,//0x68,0x35,//0x68,0x2a,//
    0x69, 0x4a, //0x69,0x62,//0x69,0x4a,//0x69,0x4a,//0x69,0x4a,//0x69,0x39,//
    0x6a, 0x5a, //0x6a,0x74,//0x6a,0x5a,//0x6a,0x5a,//0x6a,0x5d,//0x6a,0x45,//
    0x6b, 0x67, //0x6b,0x85,//0x6b,0x67,//0x6b,0x6b,//0x6b,0x6f,//0x6b,0x52,//
    0x6c, 0x71, //0x6c,0x92,//0x6c,0x71,//0x6c,0x78,//0x6c,0x7d,//0x6c,0x5d,//
    0x6d, 0x7c, //0x6d,0x9e,//0x6d,0x7c,//0x6d,0x84,//0x6d,0x8a,//0x6d,0x68,//
    0x6e, 0x8c, //0x6e,0xb2,//0x6e,0x8c,//0x6e,0x98,//0x6e,0x9f,//0x6e,0x7f,//
    0x6f, 0x9b, //0x6f,0xc0,//0x6f,0x9b,//0x6f,0xa6,//0x6f,0xae,//0x6f,0x91,//
    0x70, 0xa9, //0x70,0xcc,//0x70,0xa9,//0x70,0xb2,//0x70,0xbb,//0x70,0xa5,//
    0x71, 0xc0, //0x71,0xe0,//0x71,0xc0,//0x71,0xc6,//0x71,0xd0,//0x71,0xc6,//
    0x72, 0xd5, //0x72,0xee,//0x72,0xd5,//0x72,0xd5,//0x72,0xdf,//0x72,0xde,//
    0x73, 0xe8, //0x73,0xf6,//0x73,0xe8,//0x73,0xe8,//0x73,0xe8,//0x73,0xef,//
    0x74, 0x20, //0x74,0x11,//0x74,0x20,//0x74,0x20,//0x74,0x20,//0x74,0x16,//
    //;color matrix
    //ttune   //ov seeting //
    0xab, 0x20, //0xab,0x28,//
    0xac, 0x5b, //0xac,0x48,//
    0xad, 0x05, //0xad,0x10,//
    0xae, 0x1b, //0xae,0x18,//
    0xaf, 0x76, //0xaf,0x75,//
    0xb0, 0x90, //0xb0,0x8c,//
    0xb1, 0x90, //0xb1,0x8d,//
    0xb2, 0x8c, //0xb2,0x8c,//
    0xb3, 0x04, //0xb3,0x00,//
    0xb4, 0x98, //0xb4,0x98,//
    0xb5, 0x00, //0xb5,0x00,//
    //;lens shading
    0x40, 0xFB, //0x40,0x08,//
    0x4c, 0x03, //0x4c,0x03,//
    0x4d, 0x30, //0x4d,0xd0,//
    0x4e, 0x02, //0x4e,0x02,//
    0x4f, 0x5c, //0x4f,0x5c,//

    0x50, 0x3e, //0x50,0x3e,//


    0x51, 0x00, //0x51,0x00,//
    0x52, 0x66, //0x52,0x66,//
    0x53, 0x03, //0x53,0x03,//
    0x54, 0x30, //0x54,0xd0,//
    0x55, 0x02, //0x55,0x02,//
    0x56, 0x5c, //0x56,0x5c,//

    0x57, 0x47, //0x57,0x47,//

    0x58, 0x00, //0x58,0x00,//
    0x59, 0x66, //0x59,0x66,//
    0x5a, 0x03, //0x5a,0x03,//

    0x5b, 0x20, //0x5b,0xd0,//
    0x5c, 0x02, //0x5c,0x02,//
    0x5d, 0x5c, //0x5d,0x5c,/

    0x5e, 0x3e, //0x5e,0x3e,//

    0x5f, 0x00, //0x5f,0x00,//
    0x60, 0x66, //0x60,0x66,//

    0x41, 0x1f,

    0xb5, 0x01,
    0xb6, 0x07,
    0xb9, 0x3c,
    0xba, 0x28,
    0xb7, 0x90,
    0xb8, 0x08,
    0xbf, 0x0c,
    0xc0, 0x3e,
    0xa3, 0x0a,
    0xa4, 0x0f,
    0xa5, 0x09, //denoise threshold
    0xa6, 0x16,
    0x9f, 0x0a,
    0xa0, 0x0f,
    0xa7, 0x0a,
    0xa8, 0x0f,
    0xa1, 0x18, //0xa1,0x10,
    0xa2, 0x10, //0xa2,0x04,
    0xa9, 0x00, //0xa9,0x04,
    0xaa, 0xa6,
    //;awb
    0x75, 0x68, //0x75,0x6a,//
    0x76, 0x11, //0x76,0x11,//
    0x77, 0x92, //0x77,0x92,//
    0x78, 0xa1, //0x78,0xa1,//
    0x79, 0xe1, //0x79,0xe1,//
    0x7a, 0x02, //0x7a,0x02,//
    0x7c, 0x0e, //0x7c,0x05,//
    0x7d, 0x12, //0x7d,0x08,//
    0x7e, 0x12, //0x7e,0x08,//
    0x7f, 0x54, //0x7f,0x7c,//
    0x80, 0x78, //0x80,0x58,//
    0x81, 0xa2, //0x81,0x2a,//
    0x82, 0x80, //0x82,0xc5,//
    0x83, 0x4e, //0x83,0x46,//
    0x84, 0x40, //0x84,0x3a,//
    0x85, 0x4c, //0x85,0x54,//
    0x86, 0x43, //0x86,0x44,//
    0x87, 0xf8, //0x87,0xf8,//
    0x88, 0x08, //0x88,0x08,//
    0x89, 0x70, //0x89,0x70,//
    0x8a, 0xf0, //0x8a,0xf0,//
    0x8b, 0xf0, //0x8b,0xf0,//

    0x90, 0xe3,
    0x93, 0x10,
    0x94, 0x20,
    0x95, 0x10,
    0x96, 0x18,



    //0x18, 0x40,
    //0x19, 0x48,
    //0x1a, 0x61,


};


static __u8 reg_1280_720_30000[] = //30fps
{
    //;pclk=72mhz,30fps/pclk=36mhz,15fps
    //0x12,0x80,
    0xc3, 0x1f,
    0xc4, 0xff,
    0x3d, 0x48,

    0xdd, 0xa5,
    //;windows setup
    0x20, 0x01, //0x01,//
    0x21, 0x25, //0x60,//
    0x22, 0x00,
    0x23, 0x0c,
    0x24, 0x50, //;0x500=1280
    0x25, 0x04,
    0x26, 0x2d, //;0x2d0=720
    0x27, 0x04,
    0x28, 0x42,

    //format
    0x12, 0x48 + MIRROR * 0x20 + FLIP * 0x10, //mirror and flip
    0x39, 0x10,
    0xcd, 0x12,
    //;frame setting
    0x0e, 0x10, //0x10,//0xb4,//clock 48Mhz PCLK
    0x0f, 0x14,
    0x10, 0x0a,
    0x11, 0x00,

    0x29, 0x06, //;dummy pixels //0x29,0x06,//
    0x2a, 0x72,               //0x2a,0x40,//
    0x2b, 0x02, //;dummy lines  //0x2b,0x02,//
    0x2c, 0xee,               //0x2c,0xee,//
    //for 25fps 0x2ee*1.2=0x384

    0x1c, 0x25, //vsync width
    0x1d, 0x02, //0x04,//0x02,
    0x1e, 0x00,
    0x1f, 0xe1,


    0x3d, 0x08,
};


static __u8 reg_1600_1200_15000[] = //15fps
{
    //;pclk=72mhz,30fps/pclk=36mhz,15fps
    //0x12,0x80,
    0xc3, 0x1f,
    0xc4, 0xff,
    0x3d, 0x48,
    0xdd, 0xa5,
    //;windows setup
    0x20, 0x01,
    0x21, 0x25,
    0x22, 0x00,
    0x23, 0x0c,
    0x24, 0x64, //;0x640=1600
    0x25, 0x08,
    0x26, 0x4b, //;0x4b0=1200
    0x27, 0x06,
    0x28, 0x42,
    //;format setting
    0x12, 0x08 + MIRROR * 0x20 + FLIP * 0x10, //mirror and flip
    0x39, 0x10,
    0xcd, 0x12,
    0x3d, 0x08,
    //;frame setting
    0x0e, 0x10, //clock 48Mhz PCLK
    0x0f, 0x14,
    0x10, 0x0a,
    0x11, 0x00,

    0x29, 0x07, //;dummy pixels
    0x2a, 0xd0,
    0x2b, 0x04, //;dummy lines//0x04
    0x2c, 0xd4, //        //0xd4
    //for 12.5fps 0x4d4*1.2=0x5cb
    0x2d, 0x00,
    0x2e, 0x00,

    0x1c, 0x25, //vsync width
    0x1d, 0x06, //;banding
    0x1e, 0x00,
    0x1f, 0xb9, //0xba,//0x5d,

    0xde, 0xc4, //added 2011-9-2 9:21:54
    //0x75,0x6a,//advanced AWB for 2M
};



static __u8 reg_color_effect_normal[] =
{
    0xb6, 0x00 | 0x04,
};
static __u8 reg_color_effect_grayscale[] =
{
    0xb6, 0x20 | 0x04,
};
static __u8 reg_color_effect_colorinv[] =
{
    0xb6, 0x40 | 0x04,
};
static __u8 reg_color_effect_sepia[] =
{
    0xb6, 0x18 | 0x04,
    0xb9, 0x40,
    0xba, 0xa0,
};
static __u8 reg_color_effect_sepiagreen[] =
{
    0xb6, 0x18 | 0x04,
    0xb9, 0x60,
    0xba, 0x60,
};
static __u8 reg_color_effect_sepiablue[] =
{
    0xb6, 0x18 | 0x04,
    0xb9, 0xa0,
    0xba, 0x40,
};
static __u8 reg_color_effect_sepiared[] =
{
    0xb6, 0x18 | 0x04,
    0xb9, 0x80,
    0xba, 0xc0,
};
static __u8 reg_color_effect_sepiayellow[] =
{
    0xb6, 0x18 | 0x04,
    0xb9, 0xa0,
    0xba, 0x80,
};


static __u8 reg_awb_auto[] =
{
    0x13, 0xff,
};
static __u8 reg_awb_daylight[] = //6500k
{
    //WB sunny
    0x13, 0xfb, //AWB off
    0x05, 0x5e,
    0x06, 0x41,
    0x07, 0x54,
    0x08, 0x00,
    0x09, 0x00,

};
static __u8 reg_awb_fluorescent[] = //5000k
{
    //WB Office
    0x13, 0xfb, //AWB off
    0x05, 0x52,
    0x06, 0x41,
    0x07, 0x66,
    0x08, 0x00,
    0x09, 0x00,
};
static __u8 reg_awb_cloudy[] = //7500k
{
    //WB Cloudy
    0x13, 0xfb, //AWB off
    0x05, 0x65,
    0x06, 0x41,
    0x07, 0x4f,
    0x08, 0x00,
    0x09, 0x00,
};
static __u8 reg_awb_tungsten[] = //
{
    //WB HOME
    0x13, 0xfb, //AWB off
    0x05, 0x42,
    0x06, 0x3f,
    0x07, 0x71,
    0x08, 0x00,
    0x09, 0x00,
};


//
//static __u8 reg_ae_n3[]=//-3
//{
//0x18,0x10,
//0x19,0x08,
//0x1a,0x31,
//};
//static __u8 reg_ae_n2[]=//-2
//{
//0x18,0x18,
//0x19,0x10,
//0x1a,0x41,
//};
//static __u8 reg_ae_n1[]=//-1
//{
//0x18,0x20,
//0x19,0x18,
//0x1a,0x51,
//};
//static __u8 reg_ae_0[]=//0
//{
//0x18,0x28,
//0x19,0x20,
//0x1a,0x61,
//};
//static __u8 reg_ae_p1[]=//1
//{
//0x18,0x2c,
//0x19,0x24,
//0x1a,0x71,
//};
//static __u8 reg_ae_p2[]=//2
//{
//0x18,0x38,
//0x19,0x30,
//0x1a,0x81,
//};
//static __u8 reg_ae_p3[]=//3
//{
//0x18,0x48,
//0x19,0x40,
//0x1a,0x91,
//};

static __u8 reg_ae_n3[] = //-3
{
    0x18, 0x28,
    0x19, 0x20,
    0x1a, 0x31,
};
static __u8 reg_ae_n2[] = //-2
{
    0x18, 0x30,
    0x19, 0x28,
    0x1a, 0x41,
};
static __u8 reg_ae_n1[] = //-1
{
    0x18, 0x38,
    0x19, 0x30,
    0x1a, 0x51,
};
static __u8 reg_ae_0[] = //0
{
    0x18, 0x40,
    0x19, 0x38,
    0x1a, 0x61,
};
static __u8 reg_ae_p1[] = //1
{
    0x18, 0x48,
    0x19, 0x40,
    0x1a, 0x71,
};
static __u8 reg_ae_p2[] = //2
{
    0x18, 0x50,
    0x19, 0x48,
    0x1a, 0x81,
};
static __u8 reg_ae_p3[] = //3
{
    0x18, 0x58,
    0x19, 0x50,
    0x1a, 0x91,
};


static __u8 reg_bright_n3[] = //-3
{
    //0x41,0x1f,//08      //; SDE_En
    0xbd, 0x30, //
    //0xb6,0x04,//04
    0xbe, 0x08, //08
};
static __u8 reg_bright_n2[] = //-2
{
    //0x41,0x1f,//08      //; SDE_En
    0xbd, 0x20, //
    //0xb6,0x04,//04
    0xbe, 0x08, //08
};
static __u8 reg_bright_n1[] = //-1
{
    //0x41,0x1f,//08      //; SDE_En
    0xbd, 0x10, //
    //0xb6,0x04,//04
    0xbe, 0x08, //08
};
static __u8 reg_bright_0[] = //0
{
    //0x41,0x1f,//08      //; SDE_En
    0xbd, 0x00, //
    //0xb6,0x04,//04
    0xbe, 0x00, //08
};
static __u8 reg_bright_p1[] = //1
{
    //0x41,0x1f,//08      //; SDE_En
    0xbd, 0x10, //
    //0xb6,0x04,//04
    0xbe, 0x00, //08
};
static __u8 reg_bright_p2[] = //2
{
    //0x41,0x1f,//08      //; SDE_En
    0xbd, 0x20, //
    //0xb6,0x04,//04
    0xbe, 0x00, //08
};
static __u8 reg_bright_p3[] = //3
{
    //0x41,0x1f,//08      //; SDE_En
    0xbd, 0x30, //
    //0xb6,0x04,//04
    0xbe, 0x00, //08
};


static __u8 reg_contrast_n3[] = //-3
{
    //0x41,0x1f,//08
    //0xb6,0x04,//04//;Enable BIT2 for contrast/brightness control
    0xbb, 0x14, //
    0xbc, 0x14, //
    0xbe, 0x00, //04
};
static __u8 reg_contrast_n2[] = //-2
{
    //0x41,0x1f,//08
    //0xb6,0x04,//04//;Enable BIT2 for contrast/brightness control
    0xbb, 0x18, //
    0xbc, 0x18, //
    0xbe, 0x00, //04
};
static __u8 reg_contrast_n1[] = //-1
{
    //0x41,0x1f,//08
    //0xb6,0x04,//04//;Enable BIT2 for contrast/brightness control
    0xbb, 0x1c, //
    0xbc, 0x1c, //
    0xbe, 0x00, //04
};
static __u8 reg_contrast_0[] = //0
{
    //0x41,0x1f,//08
    //0xb6,0x04,//04//;Enable BIT2 for contrast/brightness control
    0xbb, 0x20, //  //;Offset
    0xbc, 0x20, //  //;Gain
    0xbe, 0x00, //04//;Offset sign
};
static __u8 reg_contrast_p1[] = //1
{
    //0x41,0x1f,//08
    //0xb6,0x04,//04//;Enable BIT2 for contrast/brightness control
    0xbb, 0x24, //
    0xbc, 0x24, //
    0xbe, 0x00, //04
};
static __u8 reg_contrast_p2[] = //2
{
    //0x41,0x1f,//08
    //0xb6,0x04,//04//;Enable BIT2 for contrast/brightness control
    0xbb, 0x28, //
    0xbc, 0x28, //
    0xbe, 0x00, //04
};
static __u8 reg_contrast_p3[] = //3
{
    //0x41,0x1f,//08
    //0xb6,0x04,//04//;Enable BIT2 for contrast/brightness control
    0xbb, 0x2c, //
    0xbc, 0x2c, //
    0xbe, 0x00, //04
};


__s32  csi_power_on(void)
{

    csi_pwdn(1);//pwdn=1
    csi_rst(1);//rst=1

    csi_pwren(0);//pwr=0
    esKRNL_TimeDly(1);//shut sensor power first in case system already open power before pull pwdn and reset
    csi_pwren(1);//pwr=1
    esKRNL_TimeDly(1);
    csi_pwdn(0);//pwdn=0
    esKRNL_TimeDly(1);
    csi_rst(0);//rst=0
    esKRNL_TimeDly(1);
    csi_rst(1);//rst=1

    return EPDK_OK;
}

__s32  csi_power_off(void)
{
    csi_rst(0);//rst=0
    csi_pwren(0);//pwr=0
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

__s32 block_write(ES_FILE *hdl_iic, __u8 slv_addr, __u8 *reg, __u32 length)
{
    __u32 i = 0;
    __s32 ret = EPDK_FAIL;

    reg_step = 2;

    for (i = 0; i < length; i += reg_step)
    {
        //__u8 iic_val=0;
        //__u8 tmp=0;
        //__inf("8_8\n");
        if(iic_write_8_8(hdl_iic, slv_addr, reg[i], reg[i + 1]) != EPDK_OK)
        {
            __wrn("csi[d%] iic write fail %x = %x\n", reg[i], reg[i + 1], CSI_PORT);
        }
        if (i == 0)esKRNL_TimeDly(1); //must wait
        //__inf("csi iic write %d, %x = %x\n", ret, reg[i],reg[i+1]);
    }

    return ret;
}


//设备模式设置函数，配置设备，同时根据设备配置相应的csi设置
__s32 csi_dev_set_mode(__u32 mode)
{
    //__u32 i = 0;
    __u8 *reg_table;//根据sensor的iic宽度而定__u8还是__u16或其他
    __u32 reg_table_len;

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
            reg_table = reg_640_480_30000;
            reg_table_len = sizeof(reg_640_480_30000);
            conf.input_fmt  = CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV422;//CSI_PLANAR_YUV422;//420 for encode;422 for picture
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_YUYV;
            conf.vref       = CSI_LOW;
            conf.href       = CSI_HIGH;
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
            conf.vref       = CSI_LOW;
            conf.href       = CSI_HIGH;
            conf.clock      = CSI_RISING;

            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width);

            break;
        }
        case 2:
        {
            reg_table = reg_1600_1200_15000;
            reg_table_len = sizeof(reg_1600_1200_15000);
            conf.input_fmt  = CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV422;//CSI_PLANAR_YUV422;//420 for encode;422 for picture
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_YUYV;
            conf.vref       = CSI_LOW;
            conf.href       = CSI_HIGH;
            conf.clock      = CSI_RISING;
            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width);

            break;
        }
        case 3:
        {
            reg_table = reg_1280_720_30000;
            reg_table_len = sizeof(reg_1280_720_30000);
            conf.input_fmt  = CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV420;//420 for encode;422 for picture
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_YUYV;
            conf.vref       = CSI_LOW;
            conf.href       = CSI_HIGH;
            conf.clock      = CSI_RISING;

            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width);

            break;
        }
        //4~5==> 2XCSI
        //        case 4:{
        //            reg_table = reg_640_480_30000;
        //            reg_table_len = sizeof(reg_640_480_30000);
        //            conf.input_fmt  = CSI_YUV422;
        //            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV420;//420 for encode;422 for picture
        //            conf.field_sel  = CSI_EVEN;
        //            conf.seq        = CSI_YUYV;
        //            conf.vref       = CSI_LOW;
        //            conf.href       = CSI_HIGH;
        //            conf.clock      = CSI_RISING;
        //            CSI_configure(&conf);
        //            csi_mode = csi_mode_all.csi_mode[mode];
        //            CSI_set_size(0, csi_mode.size.width, 0, csi_mode.size.height, csi_mode.size.width);
        //
        //            //CSI_configure_1(&conf);
        //            //CSI_set_size_1(0, csi_mode.size.width, 0, csi_mode.size.height, csi_mode.size.width);
        //
        //            break;
        //        }
        //        case 5:{
        //            reg_table = reg_1280_720_30000;
        //            reg_table_len = sizeof(reg_1280_720_30000);
        //            conf.input_fmt  = CSI_YUV422;
        //            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV420;//420 for encode;422 for picture
        //            conf.field_sel  = CSI_EVEN;
        //            conf.seq        = CSI_YUYV;
        //            conf.vref       = CSI_LOW;
        //            conf.href       = CSI_HIGH;
        //            conf.clock      = CSI_RISING;
        //            CSI_configure(&conf);
        //            csi_mode = csi_mode_all.csi_mode[mode];
        //            CSI_set_size(0, csi_mode.size.width*2, 0, csi_mode.size.height/2, csi_mode.size.width);
        //
        //            //CSI_configure_1(&conf);
        //            //CSI_set_size_1(0, csi_mode.size.width*2, 0, csi_mode.size.height/2, csi_mode.size.width);
        //
        //            break;
        //        }
        //mode 6 7 for pc cam
        case 6:
        {
            reg_table = reg_640_480_30000;
            reg_table_len = sizeof(reg_640_480_30000);
            conf.input_fmt  = CSI_RAW;
            conf.output_fmt = CSI_PASS_THROUTH;
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_YUYV;
            conf.vref       = CSI_LOW;
            conf.href       = CSI_HIGH;
            conf.clock      = CSI_RISING;

            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width * 2);
            break;
        }
            //        case 7:{
            //            reg_table = reg_1280_720_30000;
            //            reg_table_len = sizeof(reg_1280_720_30000);
            //            conf.input_fmt  = CSI_RAW;
            //            conf.output_fmt = CSI_PASS_THROUTH;
            //            conf.field_sel  = CSI_EVEN;
            //            conf.seq        = CSI_YUYV;
            //            conf.vref       = CSI_LOW;
            //            conf.href       = CSI_HIGH;
            //            conf.clock      = CSI_RISING;
            //            CSI_configure(&conf);
            //            csi_mode = csi_mode_all.csi_mode[mode];
            //            CSI_set_size(0, csi_mode.size.width*2, 0, csi_mode.size.height, csi_mode.size.width*2);
            //            break;
            //        }

        }


        //设备的设置
        block_write(fiic, da, reg_table, reg_table_len);
        if(mode == 0 || mode == 1)
        {
            esKRNL_TimeDly(10);
        }
        else if (mode == 2)esKRNL_TimeDly(65); //must greater than 600ms when changed to other resolution

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
        __inf("set mode %d finished! buf_scrab_mode=%d (0=CSI0, 2=01LR, 4=01UD, 5/7=TDM_2/4CH)\n", mode, last_buf_mode);
    }
    return EPDK_OK;
}



__s32  csi_dev_init(void)
{
    //	__u32 i,clock = 0;
    __u8 *reg_table;
    __u32 reg_table_len;

    iic_set_clock(fiic, 200 * 1000); //200*1000=200kHz

    //=======针对2643，init中暂时只做mode structure的初始化，不配置sensor，电源、使能等操作全部在set_mode中配置========
    __inf("csi_dev_init......\n");

    //填写本设备支持的模式
    csi_mode_all.number = 8;//所支持模式数量
    //这些设置都是指输出的数据参数，为了传递给显示或者编码时的帧参数
    //---------------------------------------------------------------------------------------------------------------
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
    //1600*1200@15#yuv422
    csi_mode_all.csi_mode[2].color_format = PIXEL_YUV422;//格式
    csi_mode_all.csi_mode[2].component_seq = YUV_SEQ_UVUV;//排列顺序
    csi_mode_all.csi_mode[2].store_mode = YUV_MOD_UV_NON_MB_COMBINED;//YUV_MOD_NON_MB_PLANAR;//存储格式
    csi_mode_all.csi_mode[2].size.width = 1600;//宽
    csi_mode_all.csi_mode[2].size.height = 1200;//高
    csi_mode_all.csi_mode[2].frame_rate = 15000;//帧率，单位Hz
    csi_mode_all.csi_mode[2].frame_period = 66666;//帧长（=1/帧率，单位us）
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

    //=========================for 2X CSI=======================================
    //  //640*480@30#yuv420
    //  csi_mode_all.csi_mode[4].color_format = PIXEL_YUV420;
    //  csi_mode_all.csi_mode[4].component_seq = YUV_SEQ_UVUV;
    //  csi_mode_all.csi_mode[4].store_mode = YUV_MOD_UV_NON_MB_COMBINED;
    //  csi_mode_all.csi_mode[4].size.width = 640*2;
    //  csi_mode_all.csi_mode[4].size.height = 480;
    //  csi_mode_all.csi_mode[4].frame_rate = 30000;
    //  csi_mode_all.csi_mode[4].frame_period = 33333;
    //  csi_mode_all.csi_mode[4].color_space = BT601;
    //  csi_mode_all.csi_mode[4].csi_buf_scrab_mode = CSI_01_LR;
    //  //1280*720@30#yuv420
    //  csi_mode_all.csi_mode[5].color_format = PIXEL_YUV420;
    //  csi_mode_all.csi_mode[5].component_seq = YUV_SEQ_UVUV;
    //  csi_mode_all.csi_mode[5].store_mode = YUV_MOD_UV_NON_MB_COMBINED;
    //  csi_mode_all.csi_mode[5].size.width = 1280;
    //  csi_mode_all.csi_mode[5].size.height = 720*2;
    //  csi_mode_all.csi_mode[5].frame_rate = 30000;
    //  csi_mode_all.csi_mode[5].frame_period = 33333;
    //  csi_mode_all.csi_mode[5].color_space = BT601;
    //  csi_mode_all.csi_mode[5].csi_buf_scrab_mode = CSI_01_UD;
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
    //  //1280*720@30#yuv420 2XCSI
    //  csi_mode_all.csi_mode[7].color_format = PIXEL_YUV420;//格式
    //  csi_mode_all.csi_mode[7].component_seq = YUV_SEQ_UVUV;//排列顺序
    //  csi_mode_all.csi_mode[7].store_mode = YUV_MOD_UV_NON_MB_COMBINED;//存储格式
    //  csi_mode_all.csi_mode[7].size.width = 960*2;//宽
    //  csi_mode_all.csi_mode[7].size.height = 720;//高
    //  csi_mode_all.csi_mode[7].frame_rate = 30000;//帧率，单位Hz
    //  csi_mode_all.csi_mode[7].frame_period = 33333;//帧长（=1/帧率，单位us）
    //  csi_mode_all.csi_mode[7].color_space = BT601;//色彩空间
    //  csi_mode_all.csi_mode[7].csi_buf_scrab_mode = CSI_01_LR;

    csi_clk_off();
    csi_power_off();
    esKRNL_TimeDly(1);

    csi_clk_on();
    csi_power_on();
    esKRNL_TimeDly(1);

    csi_mclk_on_off(1);
    //-----------------------------------------------------------------------------------------------------------
    //配置设备的初始化设置
    block_write(fiic, da, reg_640_480_30000, sizeof(reg_640_480_30000)); //视sensor的配置看是否需要，且注意size是按__u8还是__u16来计算

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
    //__u32 i = 0;
    __u8 *reg_table;//根据sensor的iic宽度而定__u8还是__u16或其他
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
    block_write(fiic, da, reg_table, reg_table_len);
    return EPDK_OK;
}
//设备awb设置函数
__s32 csi_dev_set_awb(__csi_awb_t awb)
{
    //__u32 i = 0;
    __u8 *reg_table;//根据sensor的iic宽度而定__u8还是__u16或其他
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
    block_write(fiic, da, reg_table, reg_table_len);
    return EPDK_OK;
}
//设备ae设置函数
__s32 csi_dev_set_ae(__csi_ae_t ae)
{
    //__u32 i = 0;
    __u8 *reg_table;//根据sensor的iic宽度而定__u8还是__u16或其他
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
    block_write(fiic, da, reg_table, reg_table_len);
    return EPDK_OK;
}
//设备bright设置函数
__s32 csi_dev_set_bright(__csi_bright_t bright)
{
    //__u32 i = 0;
    __u8 *reg_table;//根据sensor的iic宽度而定__u8还是__u16或其他
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
    block_write(fiic, da, reg_table, reg_table_len);
    return EPDK_OK;
}

//设备contrast设置函数
__s32 csi_dev_set_contrast(__csi_contrast_t contrast)
{
    //__u32 i = 0;
    __u8 *reg_table;//根据sensor的iic宽度而定__u8还是__u16或其他
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
    block_write(fiic, da, reg_table, reg_table_len);
    return EPDK_OK;
}


//设备band设置函数
__s32 csi_dev_set_band(__csi_band_t band)
{
    //__u32 i = 0;
    __u8 *reg_table;//根据sensor的iic宽度而定__u8还是__u16或其他
    __u32 reg_table_len;

    //__u8 reg_24[]={0x24,0x00};

    __u8 reg_band_50_60hz[] =
    {
        0x14, 0x00,
        0x1d, 0x00, //banding step 2*50/fps
        0x1e, 0x00, //banding filter MSB
        0x1f, 0x00, //banding filter LSB  50Hz: 640*480@30-> 0x26a*30/100= 0xb9  1280*720@30-> 0x2ee*30/100=0xE1 1600*1200@15->0x4d4*15/100=0xba
        //                    60Hz: 640*480@30-> 0x26a*30/120= 0x9a  1280*720@30-> 0x2ee*30/120=0xbc 1600*1200@15->0x4d4*15/120=0x9b
    };

    __u8 *reg_14;
    __u8 *reg_1d;
    __u8 *reg_1e;
    __u8 *reg_1f;

    reg_table = reg_band_50_60hz;
    reg_table_len = sizeof(reg_band_50_60hz);

    reg_14 = reg_table + 1; //50/60 select
    reg_1d = reg_table + 3; //band step
    reg_1e = reg_table + 5; //band filter MSB
    reg_1f = reg_table + 7; //band filter LSB

    //0-vga 1-720p 2-uxga

    //	  iic_read(reg_24);
    //	  __inf("%x=%x\n",reg_24[0],reg_24[1]);


    switch (band)
    {
    case CSI_BAND_50HZ:
    {
        *reg_14 = 0x87;

        if((csi_mode.size.width == 640 && csi_mode.size.height == 480) || (csi_mode.size.width == 640 * 2 && csi_mode.size.height == 480))
        {
            *reg_1d = 0x02;
            *reg_1e = 0x00;
            *reg_1f = 0xb9;
            __inf("banding 50Hz 640\n");
        }
        else if((csi_mode.size.width == 1280 && csi_mode.size.height == 720) || (csi_mode.size.width == 1280 && csi_mode.size.height == 720 * 2))
        {
            *reg_1d = 0x02;
            *reg_1e = 0x00;
            *reg_1f = 0xe1;
            __inf("banding 50Hz 720\n");
        }
        else
        {
            *reg_1d = 0x06;
            *reg_1e = 0x00;
            *reg_1f = 0xba;
            __inf("banding 50Hz 1600\n");
        }

        break;
    }
    case CSI_BAND_60HZ:
    {
        *reg_14 = 0x07;

        if( (csi_mode.size.width == 640 && csi_mode.size.height == 480) || (csi_mode.size.width == 640 * 2 && csi_mode.size.height == 480))
        {
            *reg_1d = 0x02;
            *reg_1e = 0x00;
            *reg_1f = 0x9a;
            __inf("banding 60Hz 640\n");
        }
        else if((csi_mode.size.width == 1280 && csi_mode.size.height == 720) || (csi_mode.size.width == 1280 && csi_mode.size.height == 720 * 2))
        {
            *reg_1d = 0x02;
            *reg_1e = 0x00;
            *reg_1f = 0xbc;
            __inf("banding 60Hz 720\n");
        }
        else
        {
            *reg_1d = 0x06;
            *reg_1e = 0x00;
            *reg_1f = 0x9b;
            __inf("banding 60Hz 1600\n");
        }

        break;
    }
    }

    block_write(fiic, da, reg_table, reg_table_len);
    return EPDK_OK;
}


#endif

