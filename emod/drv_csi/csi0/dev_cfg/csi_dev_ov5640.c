
#include  "csi_dev_i.h"

#if(CSI_DEV_TYPE == CSI_DEV_TYPE_OV5640)


#define MIRROR  0//0 = normal, 1 = mirror
#define FLIP        1//0 = normal, 1 = flip

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
__bool				bProgressiveSrc = 1;   // Indicating the source is progressive or not
__bool				bTopFieldFirst = 0;	  // VPO should check this flag when bProgressiveSrc is FALSE
__u16				eAspectRatio = 1000;		 //the source picture aspect ratio

__u32 csi_mclk = 24; //MHz

__csi_mode_t csi_mode;//当前使用模式
__csi_mode_all_t csi_mode_all;//支持的模式集
__csi_conf_t conf;//csi配置参数

__csi_color_effect_t csi_color_effect = CSI_COLOR_EFFECT_NORMAL;   //当前的csi_color_effect
__csi_awb_t csi_awb = CSI_AWB_AUTO;   //当前的csi_awb
__csi_ae_t csi_ae = CSI_AE_0;	//当前的csi_ae
__csi_bright_t csi_bright = CSI_BRIGHT_0;	//当前的csi_bright
__csi_contrast_t csi_contrast = CSI_CONTRAST_0;   //当前的csi_contrast
__csi_band_t csi_band = CSI_BAND_50HZ;							//当前的csi_band
//==================================================================================

extern ES_FILE	 *fiic;//i2c句柄

//==================全局变量但仅在此文件中用到========================
__u32  last_mode_num = 0xff;
__buf_scrab_mode_t	last_buf_mode = CSI_SC_NA;//区分单双csi工作状态，last_buf_mode为双，切换不reset power，单->单不reset，单->双reset一次

static	__u8 da = 0x78 >> 1; //设备地址，为i2c写地址左移 1 bit
static __u32 reg_step = 3; //一次写操作所包含的寄存器地址+寄存器值的byte数
//====================================================================

//#define TEST_10_FPS


static __u8 reg_default_640_480_30000[] =
{
#if 1
    // YUV VGA 30fps, night mode 5fps
    // Input Clock = 24Mhz, PCLK = ??MHz
    0x31, 0x03, 0x11, // system clock from pad, bit[1]
    0x30, 0x08, 0x82, // software reset, bit[7]
    0xff, 0xff, 0xc8, //delay 200ms
    0x30, 0x08, 0x42, // software power down, bit[6]
    0x31, 0x03, 0x03, // system clock from PLL, bit[1]
    0x30, 0x17, 0xff, // FREX, Vsync, HREF, PCLK, D[9:6] output enable
    0x30, 0x18, 0xff, // D[5:0], GPIO[1:0] output enable
    //PLL and CLOCK setting
    0x30, 0x34, 0x1a, // MIPI 10-bit
#ifdef TEST_10_FPS
    0x30, 0x35, 0x31, // PLL
#else
    0x30, 0x35, 0x11, // PLL
#endif
    0x30, 0x36, 0x46, // PLL 0x46->30fps
    0x30, 0x37, 0x13, // PLL root divider, bit[4], PLL pre-divider, bit[3:0]
    0x31, 0x08, 0x01, // PCLK root divider, bit[5:4], SCLK2x root divider, bit[3:2]
    0x38, 0x24, 0x01, // DVP CLK divider
    // SCLK root divider, bit[1:0]
    0x36, 0x30, 0x36,
    0x36, 0x31, 0x0e,
    0x36, 0x32, 0xe2,
    0x36, 0x33, 0x12,
    0x36, 0x21, 0xe0,
    0x37, 0x04, 0xa0,
    0x37, 0x03, 0x5a,
    0x37, 0x15, 0x78,
    0x37, 0x17, 0x01,
    0x37, 0x0b, 0x60,
    0x37, 0x05, 0x1a,
    0x39, 0x05, 0x02,
    0x39, 0x06, 0x10,
    0x39, 0x01, 0x0a,
    0x37, 0x31, 0x12,
    0x36, 0x00, 0x08, // VCM control
    0x36, 0x01, 0x33, // VCM control
    //0x30,0x2d, 0x60, // system control
    0x36, 0x20, 0x52,
    0x37, 0x1b, 0x20,
    0x47, 0x1c, 0x50,
    0x3a, 0x13, 0x43, // pre-gain = 1.047x
    0x3a, 0x18, 0x00, // gain ceiling
    0x3a, 0x19, 0xf8, // gain ceiling = 15.5x
    0x36, 0x35, 0x13,
    0x36, 0x36, 0x03,
    0x36, 0x34, 0x40,
    0x36, 0x22, 0x01,
    // 50/60Hz detection
    0x3c, 0x01, 0x34, // Band auto, bit[7]
    0x3c, 0x04, 0x28, // threshold low sum
    0x3c, 0x05, 0x98, // threshold high sum
    0x3c, 0x06, 0x00, // light meter 1 threshold[15:8]
    0x3c, 0x07, 0x08, // light meter 1 threshold[7:0]
    0x3c, 0x08, 0x00, // light meter 2 threshold[15:8]
    0x3c, 0x09, 0x1c, // light meter 2 threshold[7:0]
    0x3c, 0x0a, 0x9c, // sample number[15:8]
    0x3c, 0x0b, 0x40, // sample number[7:0]
    0x38, 0x20, 0x40 + FLIP * 0x06, // Bit[2] ISP flip, Bit[1] Sensor flip
    0x38, 0x21, 0x00 + MIRROR * 0x06, // Bit[2] ISP mirror, Bit[1] Sensor mirror, Bit[0] H binning enable
    0x38, 0x14, 0x31, // X INC
    0x38, 0x15, 0x31, // Y INC
    0x38, 0x00, 0x00, // x address start high byte		// HS 0
    0x38, 0x01, 0x00, // x address start low byte		// HS
    0x38, 0x02, 0x00, // y address start high byte		// VS 250
    0x38, 0x03, 0xfa, // y address start low byte		// VS
    0x38, 0x04, 0x0a, // x address end high byte		// HW (HE) 2623
    0x38, 0x05, 0x3f, // x address end low byte		// HW (HE)
    0x38, 0x06, 0x06, // y address end high byte 		// VH (VE) 1705
    0x38, 0x07, 0xa9, // y address end low byte		// VH (VE)
    0x38, 0x08, 0x02, // DVPHO 640
    0x38, 0x09, 0x80, // DVPHO
    0x38, 0x0a, 0x01, // DVPVO 480
    0x38, 0x0b, 0xe0, // DVPVO
    0x38, 0x0c, 0x07, // HTS 1896
    0x38, 0x0d, 0x68, // HTS
    0x38, 0x0e, 0x03, // VTS 984
    0x38, 0x0f, 0xd8, // VTS
    0x38, 0x10, 0x00, // Timing Hoffset[11:8]
    0x38, 0x11, 0x10, // Timing Hoffset[7:0]
    0x38, 0x12, 0x00, // Timing Voffset[11:8]
    0x38, 0x13, 0x06, // Timing Voffset[7:0]
    0x36, 0x18, 0x00,
    0x36, 0x12, 0x29,
    0x37, 0x08, 0x64,
    0x37, 0x09, 0x52,
    0x37, 0x0c, 0x03,
    0x3a, 0x00, 0x78,
    0x3a, 0x02, 0x03,
    0x3a, 0x03, 0xd8,
    0x3a, 0x08, 0x01, // 50HZ step MSB
    0x3a, 0x09, 0x27, // 50HZ step LSB
    0x3a, 0x0a, 0x00, // 60HZ step MSB
    0x3a, 0x0b, 0xf6, // 60HZ step LSB
    0x3a, 0x0e, 0x03, // 50HZ step max
    0x3a, 0x0d, 0x04, // 60HZ step max
    0x3a, 0x14, 0x03,
    0x3a, 0x15, 0xd8,
    0x40, 0x01, 0x02, // BLC start from line 2
    0x40, 0x04, 0x02, // BLC 2 lines
    0x30, 0x00, 0x00, // enable blocks
    0x30, 0x02, 0x1c, // reset JFIFO, SFIFO, JPEG
    0x30, 0x04, 0xff, // enable clocks
    0x30, 0x06, 0xc3, // disable clock of JPEG2x, JPEG
    0x30, 0x0e, 0x58, // MIPI power down, DVP enable
    //0x30,0x2e, 0x00,

    0x30, 0x2c, 0xc2, //bit[7:6]: output drive capability
    //00: 1x   01: 2x  10: 3x  11: 4x

    0x43, 0x00, 0x30, // YUV 422, YUYV
    0x50, 0x1f, 0x00, // YUV 422
    0x47, 0x13, 0x03, // JPEG mode 3
    0x44, 0x07, 0x04, // Quantization scale
    0x44, 0x0e, 0x00,
    0x46, 0x0b, 0x35,
    0x46, 0x0c, 0x20,
    0x48, 0x37, 0x22, // DVP CLK divider
    0x50, 0x00, 0xa7, // Lenc on, raw gamma on, BPC on, WPC on, CIP on
    0x50, 0x01, 0xa3, // SDE on, scale on, UV average off, color matrix on, AWB on

    0x47, 0x40, 0x21, //hsync,vsync,clock pol,reference to application note,spec is wrong

    //AWB
    0x51, 0x80, 0xff, // AWB B block
    0x51, 0x81, 0xf2, // AWB control
    0x51, 0x82, 0x00, // [7:4] max local counter, [3:0] max fast counter
    0x51, 0x83, 0x14, // AWB advanced
    0x51, 0x84, 0x25,
    0x51, 0x85, 0x24,
    0x51, 0x86, 0x09,
    0x51, 0x87, 0x09,
    0x51, 0x88, 0x16,
    0x51, 0x89, 0x8e,
    0x51, 0x8a, 0x56,
    0x51, 0x8b, 0xe0,
    0x51, 0x8c, 0xb2,
    0x51, 0x8d, 0x28,
    0x51, 0x8e, 0x1d,
    0x51, 0x8f, 0x56,
    0x51, 0x90, 0x46,
    0x51, 0x91, 0xf8, // AWB top limit
    0x51, 0x92, 0x04, // AWB bottom limit
    0x51, 0x93, 0x70, // red limit
    0x51, 0x94, 0xf0, // green limit
    0x51, 0x95, 0xf0, // blue limit
    0x51, 0x96, 0x03, // AWB control
    0x51, 0x97, 0x01, // local limit
    0x51, 0x98, 0x05,
    0x51, 0x99, 0x7c,
    0x51, 0x9a, 0x04,
    0x51, 0x9b, 0x00,
    0x51, 0x9c, 0x06,
    0x51, 0x9d, 0x79,
    0x51, 0x9e, 0x38, // AWB control
    //Color matrix
    0x53, 0x81, 0x1e, // CMX1 for Y
    0x53, 0x82, 0x5b, // CMX2 for Y
    0x53, 0x83, 0x08, // CMX3 for Y
    0x53, 0x84, 0x05, // CMX4 for U
    0x53, 0x85, 0x72, // CMX5 for U
    0x53, 0x86, 0x77, // CMX6 for U
    0x53, 0x87, 0x6d, // CMX7 for V
    0x53, 0x88, 0x4d, // CMX8 for V
    0x53, 0x89, 0x20, // CMX9 for V
    0x53, 0x8a, 0x01, // sign[9]
    0x53, 0x8b, 0x98, // sign[8:1]
    //Sharpness/Denoise
    0x53, 0x00, 0x08, // CIP sharpen MT threshold 1
    0x53, 0x01, 0x30, // CIP sharpen MT threshold 2
    0x53, 0x02, 0x2c, // CIP sharpen MT offset 1
    0x53, 0x03, 0x1c, // CIP sharpen MT offset 2
    0x53, 0x04, 0x08, // CIP DNS threshold 1
    0x53, 0x05, 0x30, // CIP DNS threshold 2
    0x53, 0x06, 0x1c, // CIP DNS offset 1
    0x53, 0x07, 0x2c, // CIP DNS offset 2
    0x53, 0x09, 0x08, // CIP sharpen TH threshold 1
    0x53, 0x0a, 0x30, // CIP sharpen TH threshold 2
    0x53, 0x0b, 0x04, // CIP sharpen TH offset 1
    0x53, 0x0c, 0x06, // CIP sharpen TH offset 2
    //Gamma
    0x54, 0x80, 0x01, // Gamma bias plus on, bit[0]
    0x54, 0x81, 0x06,
    0x54, 0x82, 0x0e,
    0x54, 0x83, 0x19,
    0x54, 0x84, 0x3b,
    0x54, 0x85, 0x50,
    0x54, 0x86, 0x5d,
    0x54, 0x87, 0x6a,
    0x54, 0x88, 0x75,
    0x54, 0x89, 0x80,
    0x54, 0x8a, 0x8a,
    0x54, 0x8b, 0x9b,
    0x54, 0x8c, 0xaa,
    0x54, 0x8d, 0xc0,
    0x54, 0x8e, 0xd5,
    0x54, 0x8f, 0xe8,
    0x54, 0x90, 0x20,
    // UV adjsut
    0x55, 0x80, 0x04, // saturation on, bit[1]
    0x55, 0x83, 0x40,
    0x55, 0x84, 0x10,
    0x55, 0x89, 0x10,
    0x55, 0x8a, 0x00,
    0x55, 0x8b, 0xf8,
    //0x55,0x87, 0x05,
    //0x55,0x88, 0x09,
    //Lens Shading
    0x50, 0x00, 0xa7,
    0x58, 0x00, 0x17,
    0x58, 0x01, 0x10,
    0x58, 0x02, 0x0e,
    0x58, 0x03, 0x0e,
    0x58, 0x04, 0x11,
    0x58, 0x05, 0x1b,
    0x58, 0x06, 0x0b,
    0x58, 0x07, 0x07,
    0x58, 0x08, 0x05,
    0x58, 0x09, 0x06,
    0x58, 0x0a, 0x09,
    0x58, 0x0b, 0x0e,
    0x58, 0x0c, 0x06,
    0x58, 0x0d, 0x02,
    0x58, 0x0e, 0x00,
    0x58, 0x0f, 0x00,
    0x58, 0x10, 0x03,
    0x58, 0x11, 0x09,
    0x58, 0x12, 0x06,
    0x58, 0x13, 0x03,
    0x58, 0x14, 0x00,
    0x58, 0x15, 0x00,
    0x58, 0x16, 0x03,
    0x58, 0x17, 0x09,
    0x58, 0x18, 0x0b,
    0x58, 0x19, 0x08,
    0x58, 0x1a, 0x05,
    0x58, 0x1b, 0x05,
    0x58, 0x1c, 0x08,
    0x58, 0x1d, 0x0e,
    0x58, 0x1e, 0x18,
    0x58, 0x1f, 0x12,
    0x58, 0x20, 0x0f,
    0x58, 0x21, 0x0f,
    0x58, 0x22, 0x12,
    0x58, 0x23, 0x1a,
    0x58, 0x24, 0x0a,
    0x58, 0x25, 0x0a,
    0x58, 0x26, 0x0a,
    0x58, 0x27, 0x0a,
    0x58, 0x28, 0x46,
    0x58, 0x29, 0x2a,
    0x58, 0x2a, 0x24,
    0x58, 0x2b, 0x44,
    0x58, 0x2c, 0x24,
    0x58, 0x2d, 0x28,
    0x58, 0x2e, 0x08,
    0x58, 0x2f, 0x42,
    0x58, 0x30, 0x40,
    0x58, 0x31, 0x42,
    0x58, 0x32, 0x28,
    0x58, 0x33, 0x0a,
    0x58, 0x34, 0x26,
    0x58, 0x35, 0x24,
    0x58, 0x36, 0x26,
    0x58, 0x37, 0x28,
    0x58, 0x38, 0x4a,
    0x58, 0x39, 0x0a,
    0x58, 0x3a, 0x0c,
    0x58, 0x3b, 0x2a,
    0x58, 0x3c, 0x28,
    0x58, 0x3d, 0xce, // lenc BR offset
    //0x50,0x25, 0x00,
    // AEC target
    0x3a, 0x0f, 0x40, // stable range in high
    0x3a, 0x10, 0x38, // stable range in low
    0x3a, 0x1b, 0x40, // stable range out high
    0x3a, 0x1e, 0x38, // stable range out low
    0x3a, 0x11, 0x70, // fast zone high
    0x3a, 0x1f, 0x14, // fast zone low

    //power down release
    0x30, 0x08, 0x02, // wake up from standby, bit[6]
#else
    // YUV VGA 30fps, night mode 5fps
    // Input Clock = 24Mhz, PCLK = 56MHz
    0x31, 0x03, 0x11, // system clock from pad, bit[1]
    0x30, 0x08, 0x82, // software reset, bit[7]
    // delay 5ms
    0x30, 0x08, 0x42, // software power down, bit[6]
    0x31, 0x03, 0x03, // system clock from PLL, bit[1]
    0x30, 0x17, 0xff, // FREX, Vsync, HREF, PCLK, D[9:6] output enable
    0x30, 0x18, 0xff, // D[5:0], GPIO[1:0] output enable
    0x30, 0x34, 0x1a, // MIPI 10-bit
    0x30, 0x35, 0x11, // PLL
    0x30, 0x36, 0x46, // PLL
    0x30, 0x37, 0x13, // PLL root divider, bit[4], PLL pre-divider, bit[3:0]
    0x31, 0x08, 0x01, // PCLK root divider, bit[5:4], SCLK2x root divider, bit[3:2]
    // SCLK root divider, bit[1:0]
    0x36, 0x30, 0x36,
    0x36, 0x31, 0x0e,
    0x36, 0x32, 0xe2,
    0x36, 0x33, 0x12,
    0x36, 0x21, 0xe0,
    0x37, 0x04, 0xa0,
    0x37, 0x03, 0x5a,
    0x37, 0x15, 0x78,
    0x37, 0x17, 0x01,
    0x37, 0x0b, 0x60,
    0x37, 0x05, 0x1a,
    0x39, 0x05, 0x02,
    0x39, 0x06, 0x10,
    0x39, 0x01, 0x0a,
    0x37, 0x31, 0x12,
    0x36, 0x00, 0x08, // VCM control
    0x36, 0x01, 0x33, // VCM control
    0x30, 0x2d, 0x60, // system control
    0x36, 0x20, 0x52,
    0x37, 0x1b, 0x20,
    0x47, 0x1c, 0x50,
    0x3a, 0x13, 0x43, // pre-gain = 1.047x
    0x3a, 0x18, 0x00, // gain ceiling
    0x3a, 0x19, 0xf8, // gain ceiling = 15.5x
    0x36, 0x35, 0x13,
    0x36, 0x36, 0x03,
    0x36, 0x34, 0x40,
    0x36, 0x22, 0x01,
    // 50/60Hz detection
    0x3c, 0x01, 0x34, // Band auto, bit[7]
    0x3c, 0x04, 0x28, // threshold low sum
    0x3c, 0x05, 0x98, // threshold high sum
    0x3c, 0x06, 0x00, // light meter 1 threshold[15:8]
    0x3c, 0x07, 0x08, // light meter 1 threshold[7:0]
    0x3c, 0x08, 0x00, // light meter 2 threshold[15:8]
    0x3c, 0x09, 0x1c, // light meter 2 threshold[7:0]
    0x3c, 0x0a, 0x9c, // sample number[15:8]
    0x3c, 0x0b, 0x40, // sample number[7:0]
    0x38, 0x20, 0x41, // Sensor flip off, ISP flip on
    0x38, 0x21, 0x07, // Sensor mirror on, ISP mirror on, H binning on
    0x38, 0x14, 0x31, // X INC
    0x38, 0x15, 0x31, // Y INC
    0x38, 0x00, 0x00, // HS 0
    0x38, 0x01, 0x00, // HS
    0x38, 0x02, 0x00, // VS 4
    0x38, 0x03, 0x04, // VS
    0x38, 0x04, 0x0a, // HW (HE) 2623
    0x38, 0x05, 0x3f, // HW (HE)
    0x38, 0x06, 0x07, // VH (VE) 1947
    0x38, 0x07, 0x9b, // VH (VE)
    0x38, 0x08, 0x02, // DVPHO 640
    0x38, 0x09, 0x80, // DVPHO
    0x38, 0x0a, 0x01, // DVPVO 480
    0x38, 0x0b, 0xe0, // DVPVO
    0x38, 0x0c, 0x07, // HTS 1896
    0x38, 0x0d, 0x68, // HTS
    0x38, 0x0e, 0x03, // VTS 984
    0x38, 0x0f, 0xd8, // VTS
    0x38, 0x10, 0x00, // Timing Hoffset[11:8]
    0x38, 0x11, 0x10, // Timing Hoffset[7:0]
    0x38, 0x12, 0x00, // Timing Voffset[10:8]
    0x38, 0x13, 0x06, // Timing Voffset
    0x36, 0x18, 0x00,
    0x36, 0x12, 0x29,
    0x37, 0x08, 0x64,
    0x37, 0x09, 0x52,
    0x37, 0x0c, 0x03,
    0x3a, 0x02, 0x17, // 60Hz max exposure, night mode 5fps
    0x3a, 0x03, 0x10, // 60Hz max exposure
    0x3a, 0x14, 0x17, // 50Hz max exposure, night mode 5fps
    0x3a, 0x15, 0x10, // 50Hz max exposure
    0x40, 0x01, 0x02, // BLC start from line 2
    0x40, 0x04, 0x02, // BLC 2 lines
    0x40, 0x05, 0x1a, // BLC always update
    0x30, 0x00, 0x00, // enable blocks
    0x30, 0x02, 0x1c, // reset JFIFO, SFIFO, JPEG
    0x30, 0x04, 0xff, // enable clocks
    0x30, 0x06, 0xc3, // disable clock of JPEG2x, JPEG
    0x30, 0x0e, 0x58, // MIPI power down, DVP enable
    0x30, 0x2e, 0x00,
    0x43, 0x00, 0x30, // YUV 422, YUYV
    0x50, 0x1f, 0x00, // YUV 422
    0x47, 0x13, 0x03, // JPEG mode 3
    0x44, 0x07, 0x04, // Quantization scale
    0x44, 0x0e, 0x00,
    0x50, 0x00, 0xa7, // Lenc on, raw gamma on, BPC on, WPC on, CIP on
    0x50, 0x01, 0xa3, // SDE on, scale on, UV average off, color matrix on, AWB on
    // AEC target
    0x3a, 0x0f, 0x30, // stable range in high
    0x3a, 0x10, 0x28, // stable range in low
    0x3a, 0x1b, 0x30, // stable range out high
    0x3a, 0x1e, 0x26, // stable range out low
    0x3a, 0x11, 0x60, // fast zone high
    0x3a, 0x1f, 0x14, // fast zone low
    0x46, 0x0b, 0x35,
    0x46, 0x0c, 0x22,
    0x48, 0x37, 0x22, // DVP CLK divider
    0x38, 0x24, 0x02, // DVP CLK divider
    0x35, 0x03, 0x00, // AEC/AGC on
    // Lens correction for ?
    0x58, 0x00, 0x23,
    0x58, 0x01, 0x14,
    0x58, 0x02, 0x0f,
    0x58, 0x03, 0x0f,
    0x58, 0x04, 0x12,
    0x58, 0x05, 0x26,
    0x58, 0x06, 0x0c,
    0x58, 0x07, 0x08,
    0x58, 0x08, 0x05,
    0x58, 0x09, 0x05,
    0x58, 0x0a, 0x08,
    0x58, 0x0b, 0x0d,
    0x58, 0x0c, 0x08,
    0x58, 0x0d, 0x03,
    0x58, 0x0e, 0x00,
    0x58, 0x0f, 0x00,
    0x58, 0x10, 0x03,
    0x58, 0x11, 0x09,
    0x58, 0x12, 0x07,
    0x58, 0x13, 0x03,
    0x58, 0x14, 0x00,
    0x58, 0x15, 0x01,
    0x58, 0x16, 0x03,
    0x58, 0x17, 0x08,
    0x58, 0x18, 0x0d,
    0x58, 0x19, 0x08,
    0x58, 0x1a, 0x05,
    0x58, 0x1b, 0x06,
    0x58, 0x1c, 0x08,
    0x58, 0x1d, 0x0e,
    0x58, 0x1e, 0x29,
    0x58, 0x1f, 0x17,
    0x58, 0x20, 0x11,
    0x58, 0x21, 0x11,
    0x58, 0x22, 0x15,
    0x58, 0x23, 0x28,
    0x58, 0x24, 0x46,
    0x58, 0x25, 0x26,
    0x58, 0x26, 0x08,
    0x58, 0x27, 0x26,
    0x58, 0x28, 0x64,
    0x58, 0x29, 0x26,
    0x58, 0x2a, 0x24,
    0x58, 0x2b, 0x22,
    0x58, 0x2c, 0x24,
    0x58, 0x2d, 0x24,
    0x58, 0x2e, 0x06,
    0x58, 0x2f, 0x22,
    0x58, 0x30, 0x40,
    0x58, 0x31, 0x42,
    0x58, 0x32, 0x24,
    0x58, 0x33, 0x26,
    0x58, 0x34, 0x24,
    0x58, 0x35, 0x22,
    0x58, 0x36, 0x22,
    0x58, 0x37, 0x26,
    0x58, 0x38, 0x44,
    0x58, 0x39, 0x24,
    0x58, 0x3a, 0x26,
    0x58, 0x3b, 0x28,
    0x58, 0x3c, 0x42,
    0x58, 0x3d, 0xce, // lenc BR offset
    // AWB
    0x51, 0x80, 0xff, // AWB B block
    0x51, 0x81, 0xf2, // AWB control
    0x51, 0x82, 0x00, // [7:4] max local counter, [3:0] max fast counter
    0x51, 0x83, 0x14, // AWB advanced
    0x51, 0x84, 0x25,
    0x51, 0x85, 0x24,
    0x51, 0x86, 0x09,
    0x51, 0x87, 0x09,
    0x51, 0x88, 0x09,
    0x51, 0x89, 0x75,
    0x51, 0x8a, 0x54,
    0x51, 0x8b, 0xe0,
    0x51, 0x8c, 0xb2,
    0x51, 0x8d, 0x42,
    0x51, 0x8e, 0x3d,
    0x51, 0x8f, 0x56,
    0x51, 0x90, 0x46,
    0x51, 0x91, 0xf8, // AWB top limit
    0x51, 0x92, 0x04, // AWB bottom limit
    0x51, 0x93, 0x70, // red limit
    0x51, 0x94, 0xf0, // green limit
    0x51, 0x95, 0xf0, // blue limit
    0x51, 0x96, 0x03, // AWB control
    0x51, 0x97, 0x01, // local limit
    0x51, 0x98, 0x04,
    0x51, 0x99, 0x12,
    0x51, 0x9a, 0x04,
    0x51, 0x9b, 0x00,
    0x51, 0x9c, 0x06,
    0x51, 0x9d, 0x82,
    0x51, 0x9e, 0x38, // AWB control
    // Gamma
    0x54, 0x80, 0x01, // Gamma bias plus on, bit[0]
    0x54, 0x81, 0x08,
    0x54, 0x82, 0x14,
    0x54, 0x83, 0x28,
    0x54, 0x84, 0x51,
    0x54, 0x85, 0x65,
    0x54, 0x86, 0x71,
    0x54, 0x87, 0x7d,
    0x54, 0x88, 0x87,
    0x54, 0x89, 0x91,
    0x54, 0x8a, 0x9a,
    0x54, 0x8b, 0xaa,
    0x54, 0x8c, 0xb8,
    0x54, 0x8d, 0xcd,
    0x54, 0x8e, 0xdd,
    0x54, 0x8f, 0xea,
    0x54, 0x90, 0x1d,
    // color matrix
    0x53, 0x81, 0x1e, // CMX1 for Y
    0x53, 0x82, 0x5b, // CMX2 for Y
    0x53, 0x83, 0x08, // CMX3 for Y
    0x53, 0x84, 0x0a, // CMX4 for U
    0x53, 0x85, 0x7e, // CMX5 for U
    0x53, 0x86, 0x88, // CMX6 for U
    0x53, 0x87, 0x7c, // CMX7 for V
    0x53, 0x88, 0x6c, // CMX8 for V
    0x53, 0x89, 0x10, // CMX9 for V
    0x53, 0x8a, 0x01, // sign[9]
    0x53, 0x8b, 0x98, // sign[8:1]
    // UV adjsut
    0x55, 0x80, 0x06, // saturation on, bit[1]
    0x55, 0x83, 0x40,
    0x55, 0x84, 0x10,
    0x55, 0x89, 0x10,
    0x55, 0x8a, 0x00,
    0x55, 0x8b, 0xf8,
    0x50, 0x1d, 0x40, // enable manual offset of contrast
    // CIP
    0x53, 0x00, 0x08, // CIP sharpen MT threshold 1
    0x53, 0x01, 0x30, // CIP sharpen MT threshold 2
    0x53, 0x02, 0x10, // CIP sharpen MT offset 1
    0x53, 0x03, 0x00, // CIP sharpen MT offset 2
    0x53, 0x04, 0x08, // CIP DNS threshold 1
    0x53, 0x05, 0x30, // CIP DNS threshold 2
    0x53, 0x06, 0x08, // CIP DNS offset 1
    0x53, 0x07, 0x16, // CIP DNS offset 2
    0x53, 0x09, 0x08, // CIP sharpen TH threshold 1
    0x53, 0x0a, 0x30, // CIP sharpen TH threshold 2
    0x53, 0x0b, 0x04, // CIP sharpen TH offset 1
    0x53, 0x0c, 0x06, // CIP sharpen TH offset 2
    0x50, 0x25, 0x00,
    0x30, 0x08, 0x02, // wake up from standby, bit[6]
#endif
};

static __u8 reg_640_480_30000[] =
{
    // YUV VGA 30fps
    // Input Clock = 24Mhz, PCLK = ??MHz
    // PLL and CLOCK setting
    0x30, 0x34, 0x1a,
#ifdef TEST_10_FPS
    0x30, 0x35, 0x31, // 0x11:30fps 0x21:15fps
#else
    0x30, 0x35, 0x11, // 0x11:30fps 0x21:15fps
#endif
    0x30, 0x36, 0x46, // PLL
    0x30, 0x37, 0x13, // PLL root divider, bit[4], PLL pre-divider, bit[3:0]
    0x31, 0x08, 0x01, // PCLK root divider, bit[5:4], SCLK2x root divider, bit[3:2]
    0x38, 0x24, 0x01,
    0xff, 0xff, 0x64, // delay 100mse
    //Timing
    //640x480
    0x38, 0x08, 0x02, // DVPHO 640
    0x38, 0x09, 0x80, // DVPHO
    0x38, 0x0a, 0x01, // DVPVO 480
    0x38, 0x0b, 0xe0, // DVPVO
    0x38, 0x0c, 0x07, // HTS 1896
    0x38, 0x0d, 0x68, // HTS
    0x38, 0x0e, 0x03, // VTS 984
    0x38, 0x0f, 0xd8, // VTS

    //Banding step
    0x3a, 0x08, 0x01, // 50HZ step MSB
    0x3a, 0x09, 0x27, // 50HZ step LSB
    0x3a, 0x0a, 0x00, // 60HZ step MSB
    0x3a, 0x0b, 0xf6, // 60HZ step LSB
    0x3a, 0x0e, 0x03, // 50HZ step max
    0x3a, 0x0d, 0x04, // 60HZ step max

    0x3c, 0x07, 0x08, // light meter 1 threshold[7:0]
    0x38, 0x14, 0x31, // horizton subsample
    0x38, 0x15, 0x31, // vertical subsample
    0x38, 0x00, 0x00, // x address start high byte		// HS 0
    0x38, 0x01, 0x00, // x address start low byte		// HS
    0x38, 0x02, 0x00, // y address start high byte		// VS 250
    0x38, 0x03, 0xfa, // y address start low byte		// VS
    0x38, 0x04, 0x0a, // x address end high byte 		// HW (HE) 2623
    0x38, 0x05, 0x3f, // x address end low byte		// HW (HE)
    0x38, 0x06, 0x06, // y address end high byte 		// VH (VE) 1705
    0x38, 0x07, 0xa9, // y address end low byte		// VH (VE)
    0x38, 0x10, 0x00, // isp hortizontal offset high byte
    0x38, 0x11, 0x10, // isp hortizontal offset low byte
    0x38, 0x12, 0x00, // isp vertical offset high byte
    0x38, 0x13, 0x06, // isp vertical offset low byte

    0x53, 0x08, 0x65, // sharpen manual
    0x53, 0x02, 0x00, // sharpen offset 1
    0x40, 0x02, 0x45, // BLC related
    0x40, 0x05, 0x18, // BLC related

    0x36, 0x18, 0x00,
    0x36, 0x12, 0x29,
    0x37, 0x09, 0x52,
    0x37, 0x0c, 0x03,
    0x3a, 0x02, 0x03, // 60HZ max exposure limit MSB
    0x3a, 0x03, 0xd8, // 60HZ max exposure limit LSB
    0x3a, 0x14, 0x03, // 50HZ max exposure limit MSB
    0x3a, 0x15, 0xd8, // 50HZ max exposure limit LSB
    0x40, 0x04, 0x02, // BLC line number

    0x48, 0x37, 0x22, // PCLK period
    0x50, 0x01, 0xa3, // ISP effect
    0x35, 0x03, 0x00, // AEC eanble

    //0x30,0x2c, 0x42, // bit[7:6]: output drive capability
    0x30, 0x2c, 0xC2, // bit[7:6]: output drive capability
    //00: 1x   01: 2x  10: 3x  11: 4x
};

static __u8 reg_1280_720_30000[] =
{
    // YUV 720P 30fps
    // Input Clock = 24Mhz, PCLK = ??MHz
    // PLL and CLOCK setting
    0x30, 0x34, 0x1a,
    0x30, 0x35, 0x21, // 0x11:60fps 0x21:30fps 0x41:15fps
    0x30, 0x36, 0x69, // PLL
    0x30, 0x37, 0x13, // PLL root divider, bit[4], PLL pre-divider, bit[3:0]
    0x31, 0x08, 0x01, // PCLK root divider, bit[5:4], SCLK2x root divider, bit[3:2]
    0x38, 0x24, 0x01, // DVP CLK divider
    0xff, 0xff, 0x64, // delay 100ms
    //Timing
    //1280x720
    0x38, 0x08, 0x05, // DVPHO 1280
    0x38, 0x09, 0x00, // DVPHO
    0x38, 0x0a, 0x02, // DVPVO 720
    0x38, 0x0b, 0xd0, // DVPVO
    0x38, 0x0c, 0x07, // HTS 1892
    0x38, 0x0d, 0x64, // HTS
    0x38, 0x0e, 0x02, // VTS 740
    0x38, 0x0f, 0xe4, // VTS

    //Banding step
    0x3a, 0x08, 0x00, // 50HZ step MSB
    0x3a, 0x09, 0xdd, // 50HZ step LSB
    0x3a, 0x0a, 0x00, // 60HZ step MSB
    0x3a, 0x0b, 0xb8, // 60HZ step LSB
    0x3a, 0x0e, 0x03, // 50HZ step max
    0x3a, 0x0d, 0x04, // 60HZ step max

    0x3c, 0x07, 0x07, // light meter 1 threshold[7:0]
    0x38, 0x14, 0x31, // horizton subsample
    0x38, 0x15, 0x31, // vertical subsample
    0x38, 0x00, 0x00, // x address start high byte		// HS 0
    0x38, 0x01, 0x00, // x address start low byte		// HS
    0x38, 0x02, 0x00, // y address start high byte		// VS 250
    0x38, 0x03, 0xfa, // y address start low byte		// VS
    0x38, 0x04, 0x0a, // x address end high byte 		// HW (HE) 2623
    0x38, 0x05, 0x3f, // x address end low byte		// HW (HE)
    0x38, 0x06, 0x06, // y address end high byte 		// VH (VE) 1705
    0x38, 0x07, 0xa9, // y address end low byte		// VH (VE)
    0x38, 0x10, 0x00, // isp hortizontal offset high byte
    0x38, 0x11, 0x10, // isp hortizontal offset low byte
    0x38, 0x12, 0x00, // isp vertical offset high byte
    0x38, 0x13, 0x04, // isp vertical offset low byte

    0x53, 0x08, 0x65, // sharpen manual
    0x53, 0x02, 0x00, // sharpen offset 1
    0x40, 0x02, 0x45, // BLC related
    0x40, 0x05, 0x18, // BLC related

    0x36, 0x18, 0x00,
    0x36, 0x12, 0x29,
    0x37, 0x09, 0x52,
    0x37, 0x0c, 0x03,
    0x3a, 0x02, 0x02, // 60HZ max exposure limit MSB
    0x3a, 0x03, 0xe0, // 60HZ max exposure limit LSB
    0x3a, 0x14, 0x02, // 50HZ max exposure limit MSB
    0x3a, 0x15, 0xe0, // 50HZ max exposure limit LSB

    0x40, 0x04, 0x02, // BLC line number
    0x30, 0x02, 0x1c, // reset JFIFO SFIFO JPG
    0x30, 0x06, 0xc3, // enable xx clock
    0x46, 0x0b, 0x37, // debug mode
    0x46, 0x0c, 0x20, // PCLK Manuale
    0x48, 0x37, 0x16, // PCLK period
    0x50, 0x01, 0x83, // ISP effect
    0x35, 0x03, 0x00, // AEC enable

    //0x30,0x2c, 0x82, // bit[7:6]: output drive capability
    0x30, 0x2c, 0xc2, // bit[7:6]: output drive capability
    // 00: 1x	 01: 2x  10: 3x  11: 4x
};

static __u8 reg_1920_1080_30000[] =
{
    // YUV 1080P 30fps
    // Input Clock = 24Mhz, PCLK = ??MHz
    // PLL and CLOCK setting
    0x30, 0x34, 0x1a,
    0x30, 0x35, 0x11, // 0x11:30fps 0x21:15fps
    0x30, 0x36, 0x46, // PLL
    0x30, 0x37, 0x13, // PLL root divider, bit[4], PLL pre-divider, bit[3:0]
    0x31, 0x08, 0x01, // PCLK root divider, bit[5:4], SCLK2x root divider, bit[3:2]
    0x38, 0x24, 0x01,
    0xff, 0xff, 0x64, // delay 100ms
    //Timing
    //1920x1080
    0x38, 0x08, 0x07, // DVPHO 1920
    0x38, 0x09, 0x80, // DVPHO
    0x38, 0x0a, 0x04, // DVPVO 1080
    0x38, 0x0b, 0x38, // DVPVO
    0x38, 0x0c, 0x09, // HTS 2500
    0x38, 0x0d, 0xc4, // HTS
    0x38, 0x0e, 0x04, // VTS 1120
    0x38, 0x0f, 0x60, // VTS
    //Banding step
    0x3a, 0x08, 0x01, // 50HZ step MSB
    0x3a, 0x09, 0x50, // 50HZ step LSB
    0x3a, 0x0a, 0x01, // 60HZ step MSB
    0x3a, 0x0b, 0x18, // 60HZ step LSB
    0x3a, 0x0e, 0x03, // 50HZ step max
    0x3a, 0x0d, 0x04, // 60HZ step max

    0x3c, 0x07, 0x07, // light meter 1 threshold[7:0]
    0x38, 0x14, 0x11, // horizton subsample
    0x38, 0x15, 0x11, // vertical subsample
#if 1
    0x38, 0x00, 0x01, // x address start high byte		// HS 336
    0x38, 0x01, 0x50, // x address start low byte		// HS
    0x38, 0x02, 0x01, // y address start high byte		// VS 434
    0x38, 0x03, 0xb2, // y address start low byte		// VS
    0x38, 0x04, 0x08, // x address end high byte 		// HW (HE) 2287
    0x38, 0x05, 0xef, // x address end low byte		// HW (HE)
    0x38, 0x06, 0x05, // y address end high byte 		// VH (VE) 1521
    0x38, 0x07, 0xf1, // y address end low byte		// VH (VE)
#else
    0x38, 0x00, 0x00, // x address start high byte		// HS 0
    0x38, 0x01, 0x00, // x address start low byte		// HS
    0x38, 0x02, 0x00, // y address start high byte		// VS 250
    0x38, 0x03, 0xfa, // y address start low byte		// VS
    0x38, 0x04, 0x0a, // x address end high byte 		// HW (HE) 2623
    0x38, 0x05, 0x3f, // x address end low byte		// HW (HE)
    0x38, 0x06, 0x06, // y address end high byte 		// VH (VE) 1705
    0x38, 0x07, 0xa9, // y address end low byte		// VH (VE)
#endif

    0x38, 0x10, 0x00, // isp hortizontal offset high byte
    0x38, 0x11, 0x10, // isp hortizontal offset low byte
    0x38, 0x12, 0x00, // isp vertical offset high byte
    0x38, 0x13, 0x04, // isp vertical offset low byte

    0x53, 0x08, 0x65, // sharpen manual
    0x53, 0x02, 0x00, // sharpen offset 1
    0x40, 0x02, 0x45, // BLC related
    0x40, 0x05, 0x18, // BLC related

    0x36, 0x18, 0x04,
    0x36, 0x12, 0x2b,
    0x37, 0x09, 0x12,
    0x37, 0x0c, 0x00,
    0x3a, 0x02, 0x04, // 60HZ max exposure limit MSB
    0x3a, 0x03, 0x60, // 60HZ max exposure limit LSB
    0x3a, 0x14, 0x04, // 50HZ max exposure limit MSB
    0x3a, 0x15, 0x60, // 50HZ max exposure limit LSB

    0x40, 0x04, 0x06, // BLC line number
    0x30, 0x02, 0x1c, // reset JFIFO SFIFO JPG
    0x30, 0x06, 0xc3, // enable xx clock
    0x46, 0x0b, 0x37, // debug mode
    0x46, 0x0c, 0x20, // PCLK Manuale
    0x48, 0x37, 0x16, // PCLK period
    0x50, 0x01, 0x83, // ISP effect
    0x35, 0x03, 0x00, // AEC enable

    //0x30,0x2c, 0x82, // bit[7:6]: output drive capability
    0x30, 0x2c, 0xc2, // bit[7:6]: output drive capability
    //00: 1x   01: 2x  10: 3x  11: 4x
};

/*
static __u8 reg_5M[]=
{


};
*/

static __u8 reg_color_effect_normal[] =
{
    0x50, 0x01, 0x7f,
    0x55, 0x80, 0x04,
};
static __u8 reg_color_effect_grayscale[] =
{
    0x50, 0x01, 0xff,
    0x55, 0x80, 0x18,
    0x55, 0x83, 0x80,
    0x55, 0x84, 0x80,
};
static __u8 reg_color_effect_colorinv[] =
{
    0x50, 0x01, 0xff,
    0x55, 0x80, 0x40,
};
static __u8 reg_color_effect_sepia[] =
{
    0x50, 0x01, 0xff,
    0x55, 0x80, 0x18,
    0x55, 0x83, 0x40,
    0x55, 0x84, 0xa0,
};

/*
static __u8 reg_color_effect_sepiagreen[]=
{
// Not Implement
};
static __u8 reg_color_effect_sepiablue[]=
{
// Not Implement
};
static __u8 reg_color_effect_sepiared[]=
{
// Not Implement
};
static __u8 reg_color_effect_sepiayellow[]=
{
// Not Implement
};
*/

static __u8 reg_awb_auto[] =
{
    //simple awb
    //0x34,0x06, 0x00,
    //0x51,0x83, 0x94,
    //0x51,0x91, 0xff,
    //0x51,0x92, 0x00,

    //advanced awb
    0x34, 0x06, 0x00,
    0x51, 0x92, 0x04,
    0x51, 0x91, 0xf8,
    0x51, 0x93, 0x70,
    0x51, 0x94, 0xf0,
    0x51, 0x95, 0xf0,
    0x51, 0x8d, 0x3d,
    0x51, 0x8f, 0x54,
    0x51, 0x8e, 0x3d,
    0x51, 0x90, 0x54,
    0x51, 0x8b, 0xa8,
    0x51, 0x8c, 0xa8,
    0x51, 0x87, 0x18,
    0x51, 0x88, 0x18,
    0x51, 0x89, 0x6e,
    0x51, 0x8a, 0x68,
    0x51, 0x86, 0x1c,
    0x51, 0x81, 0x50,
    0x51, 0x84, 0x25,
    0x51, 0x82, 0x11,
    0x51, 0x83, 0x14,
    0x51, 0x84, 0x25,
    0x51, 0x85, 0x24,
};
static __u8 reg_awb_daylight[] = //6500k
{
    //WB sunny
    0x34, 0x06, 0x01,
    0x34, 0x00, 0x06,
    0x34, 0x01, 0x1c,
    0x34, 0x02, 0x04,
    0x34, 0x03, 0x00,
    0x34, 0x04, 0x04,
    0x34, 0x05, 0xf3,
};
static __u8 reg_awb_fluorescent[] = //5000k
{
    //WB Office
    0x34, 0x06, 0x01,
    0x34, 0x00, 0x05,
    0x34, 0x01, 0x48,
    0x34, 0x02, 0x04,
    0x34, 0x03, 0x00,
    0x34, 0x04, 0x07,
    0x34, 0x05, 0xcf,
};
static __u8 reg_awb_cloudy[] = //7500k
{
    //WB Cloudy
    0x34, 0x06, 0x01,
    0x34, 0x00, 0x06,
    0x34, 0x01, 0x48,
    0x34, 0x02, 0x04,
    0x34, 0x03, 0x00,
    0x34, 0x04, 0x04,
    0x34, 0x05, 0xd3,
};

static __u8 reg_awb_tungsten[] = //
{
    //WB HOME
    0x34, 0x06, 0x01,
    0x34, 0x00, 0x04,
    0x34, 0x01, 0x10,
    0x34, 0x02, 0x04,
    0x34, 0x03, 0x00,
    0x34, 0x04, 0x08,
    0x34, 0x05, 0x40,
};


static __u8 reg_ae_n3[] = //-3
{
    // -1.3EV
    0x3a, 0x0f, 0x18,
    0x3a, 0x10, 0x10,
    0x3a, 0x1b, 0x18,
    0x3a, 0x1e, 0x10,
    0x3a, 0x11, 0x30,
    0x3a, 0x1f, 0x10,
};
static __u8 reg_ae_n2[] = //-2
{
    // -1.0EV
    0x3a, 0x0f, 0x20,
    0x3a, 0x10, 0x18,
    0x3a, 0x11, 0x41,
    0x3a, 0x1b, 0x20,
    0x3a, 0x1e, 0x18,
    0x3a, 0x1f, 0x10,
};
static __u8 reg_ae_n1[] = //-1
{
    // -0.7EV
    0x3a, 0x0f, 0x28,
    0x3a, 0x10, 0x20,
    0x3a, 0x11, 0x51,
    0x3a, 0x1b, 0x28,
    0x3a, 0x1e, 0x20,
    0x3a, 0x1f, 0x10,
};
static __u8 reg_ae_0[] = //0
{
    // Default
    0x3a, 0x0f, 0x38,
    0x3a, 0x10, 0x30,
    0x3a, 0x11, 0x61,
    0x3a, 0x1b, 0x38,
    0x3a, 0x1e, 0x30,
    0x3a, 0x1f, 0x10,
};
static __u8 reg_ae_p1[] = //1
{
    // +0.7EV
    0x3a, 0x0f, 0x48,
    0x3a, 0x10, 0x40,
    0x3a, 0x11, 0x81,
    0x3a, 0x1b, 0x48,
    0x3a, 0x1e, 0x40,
    0x3a, 0x1f, 0x20,
};
static __u8 reg_ae_p2[] = //2
{
    // +1.0EV
    0x3a, 0x0f, 0x50,
    0x3a, 0x10, 0x48,
    0x3a, 0x11, 0x90,
    0x3a, 0x1b, 0x50,
    0x3a, 0x1e, 0x48,
    0x3a, 0x1f, 0x20,
};
static __u8 reg_ae_p3[] = //3
{
    // +1.3EV
    0x3a, 0x0f, 0x58,
    0x3a, 0x10, 0x50,
    0x3a, 0x11, 0x91,
    0x3a, 0x1b, 0x58,
    0x3a, 0x1e, 0x50,
    0x3a, 0x1f, 0x20,
};


static __u8 reg_bright_n3[] = //-3
{
    0x55, 0x87, 0x30,
    0x55, 0x88, 0x01,
};
static __u8 reg_bright_n2[] = //-2
{
    0x55, 0x87, 0x20,
    0x55, 0x88, 0x01,
};
static __u8 reg_bright_n1[] = //-1
{
    0x55, 0x87, 0x10,
    0x55, 0x88, 0x01,
};
static __u8 reg_bright_0[] = //0
{
    0x55, 0x87, 0x00,
    0x55, 0x88, 0x01,
};
static __u8 reg_bright_p1[] = //1
{
    0x55, 0x87, 0x10,
    0x55, 0x88, 0x09,
};
static __u8 reg_bright_p2[] = //2
{
    0x55, 0x87, 0x20,
    0x55, 0x88, 0x09,
};
static __u8 reg_bright_p3[] = //3
{
    0x55, 0x87, 0x30,
    0x55, 0x88, 0x09,
};

/*
static __u8 reg_contrast_n3[]=//-3
{
//Not Implement
};
static __u8 reg_contrast_n2[]=//-2
{
//Not Implement
};
static __u8 reg_contrast_n1[]=//-1
{
//Not Implement
};
static __u8 reg_contrast_0[]=//0
{
//Not Implement
};
static __u8 reg_contrast_p1[]=//1
{
//Not Implement
};
static __u8 reg_contrast_p2[]=//2
{
//Not Implement
};
static __u8 reg_contrast_p3[]=//3
{
//Not Implement
};
*/

__s32  csi_power_on(void)
{

    //	csi_pwdn(1);//pwdn=1
    //	csi_rst(1);//rst=1

    //	csi_pwren(0);//pwr=0
    //	esKRNL_TimeDly(1);//shut sensor power first in case system already open power before pull pwdn and reset
    //	csi_pwren(1);//pwr=1
    esKRNL_TimeDly(1);
    csi_pwdn(0);//pwdn=0
    esKRNL_TimeDly(1);
    csi_rst(1);//rst=1
    esKRNL_TimeDly(1);
    csi_rst(0);//rst=0
    esKRNL_TimeDly(1);
    csi_rst(1);//rst=1

    return EPDK_OK;
}

__s32  csi_power_off(void)
{
    csi_pwdn(1);//pwdn=1
    csi_rst(0);//rst=0
    //	csi_pwren(0);//pwr=0
    esKRNL_TimeDly(1);
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

    for (i = 0; i < length; i += reg_step)
    {
        if(iic_write_16_8(hdl_iic, slv_addr, reg[i], reg[i + 1], reg[i + 2]) != EPDK_OK)
        {
            __wrn("csi[0] iic write fail 0x%02X%02X = 0x%02X\n", reg[i], reg[i + 1], reg[i + 2]);
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
        __inf("no need to change csi[0] mode!\n");
    }

    else
    {
        __inf("csi[0] csi_dev_set_mode[%d]!\n", mode);
        csi_mode = csi_mode_all.csi_mode[mode];

        switch (mode)
        {
        case 0:
        {
            reg_table = reg_default_640_480_30000;
            reg_table_len = sizeof(reg_default_640_480_30000);
            conf.input_fmt	= CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV422;//CSI_PLANAR_YUV422;//420 for encode;422 for picture
            conf.field_sel	= CSI_EVEN;
            conf.seq		= CSI_YUYV;
            conf.vref		= CSI_HIGH;
            conf.href		= CSI_HIGH;
            conf.clock	= CSI_RISING;
            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width);
            break;
        }
        case 1:
        {
            reg_table = reg_640_480_30000;
            reg_table_len = sizeof(reg_640_480_30000);
            conf.input_fmt	= CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV420;//420 for encode;422 for picture
            conf.field_sel	= CSI_EVEN;
            conf.seq		= CSI_YUYV;
            conf.vref		= CSI_HIGH;
            conf.href		= CSI_HIGH;
            conf.clock	= CSI_RISING;
            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width);
            break;
        }
        case 2:
        {
            reg_table = reg_1280_720_30000;
            reg_table_len = sizeof(reg_1280_720_30000);
            conf.input_fmt	= CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV422;//420 for encode;422 for picture
            conf.field_sel	= CSI_EVEN;
            conf.seq		= CSI_YUYV;
            conf.vref		= CSI_HIGH;
            conf.href		= CSI_HIGH;
            conf.clock	= CSI_RISING;
            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width);
            break;
        }
        case 3:
        {
            reg_table = reg_1920_1080_30000;
            reg_table_len = sizeof(reg_1920_1080_30000);
            conf.input_fmt	= CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV420;//420 for encode;422 for picture
            conf.field_sel	= CSI_EVEN;
            conf.seq		= CSI_YUYV;
            conf.vref		= CSI_HIGH;
            conf.href		= CSI_HIGH;
            conf.clock	= CSI_RISING;
            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width);
            break;
        }
        }


        //设备的设置
        block_write(fiic, da, reg_table, reg_table_len);

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
    //__u32 i = 0;

    iic_set_clock(fiic, 200 * 1000); //200*1000=200kHz

    //=======针对5642，init中暂时只做mode structure的初始化，不配置sensor，电源、使能等操作全部在set_mode中配置========
    __inf("OV5640 csi_0_dev_init......\n");

    //填写本设备支持的模式
    csi_mode_all.number = 4;//所支持模式数量
    //这些设置都是指输出的数据参数，为了传递给显示或者编码时的帧参数
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
#ifdef TEST_10_FPS
    csi_mode_all.csi_mode[1].frame_rate = 10000;
    csi_mode_all.csi_mode[1].frame_period = 100000;
#else
    csi_mode_all.csi_mode[1].frame_rate = 30000;
    csi_mode_all.csi_mode[1].frame_period = 33333;
#endif
    csi_mode_all.csi_mode[1].color_space = BT601;
    csi_mode_all.csi_mode[1].csi_buf_scrab_mode = CSI0_FULL;
    //1280*720@30#yuv420
    csi_mode_all.csi_mode[2].color_format = PIXEL_YUV420;
    csi_mode_all.csi_mode[2].component_seq = YUV_SEQ_UVUV;
    csi_mode_all.csi_mode[2].store_mode = YUV_MOD_UV_NON_MB_COMBINED;
    csi_mode_all.csi_mode[2].size.width = 1280;
    csi_mode_all.csi_mode[2].size.height = 720;
#ifdef TEST_10_FPS
    csi_mode_all.csi_mode[2].frame_rate = 10000;
    csi_mode_all.csi_mode[2].frame_period = 100000;
#else
    csi_mode_all.csi_mode[2].frame_rate = 30000;
    csi_mode_all.csi_mode[2].frame_period = 33333;
#endif
    csi_mode_all.csi_mode[2].color_space = BT601;
    csi_mode_all.csi_mode[2].csi_buf_scrab_mode = CSI0_FULL;
    //1920*1080@30#yuv420

    csi_mode_all.csi_mode[3].color_format = PIXEL_YUV420;
    csi_mode_all.csi_mode[3].component_seq = YUV_SEQ_UVUV;
    csi_mode_all.csi_mode[3].store_mode = YUV_MOD_UV_NON_MB_COMBINED;
    csi_mode_all.csi_mode[3].size.width = 1920;
    csi_mode_all.csi_mode[3].size.height = 1080;
#ifdef TEST_10_FPS
    csi_mode_all.csi_mode[3].frame_rate = 10000;
    csi_mode_all.csi_mode[3].frame_period = 100000;
#else
    csi_mode_all.csi_mode[3].frame_rate = 30000;
    csi_mode_all.csi_mode[3].frame_period = 33333;

#endif
    csi_mode_all.csi_mode[3].color_space = BT601;
    csi_mode_all.csi_mode[3].csi_buf_scrab_mode = CSI0_FULL;
    /*
    	//2592*1944@7.5#yuv422
    	csi_mode_all.csi_mode[4].color_format = PIXEL_YUV422;//格式
    	csi_mode_all.csi_mode[4].component_seq = YUV_SEQ_UVUV;//排列顺序
    	csi_mode_all.csi_mode[4].store_mode = YUV_MOD_NON_MB_PLANAR;//存储格式
    	csi_mode_all.csi_mode[4].size.width = 2560;//宽
    	csi_mode_all.csi_mode[4].size.height = 1920;//高
    	csi_mode_all.csi_mode[4].frame_rate = 7500;//帧率，单位Hz
    	csi_mode_all.csi_mode[4].frame_period = 133333;//帧长（=1/帧率，单位us）
    	csi_mode_all.csi_mode[4].color_space = BT601;//色彩空间
    	//=================mode 6 7 for pc cam============================
    	//640*480@30#yuv422
    	csi_mode_all.csi_mode[6].color_format = PIXEL_YUV422;//格式
    	csi_mode_all.csi_mode[6].component_seq = YUV_SEQ_YUYV;//排列顺序
    	csi_mode_all.csi_mode[6].store_mode = YUV_MOD_INTERLEAVED;//存储格式
    	csi_mode_all.csi_mode[6].size.width = 640;//宽
    	csi_mode_all.csi_mode[6].size.height = 480;//高
    	csi_mode_all.csi_mode[6].frame_rate = 30000;//帧率，单位Hz
    	csi_mode_all.csi_mode[6].frame_period = 33333;//帧长（=1/帧率，单位us）
    	csi_mode_all.csi_mode[6].color_space = BT601;//色彩空间
    */

    csi_clk_off();
    csi_power_off();
    esKRNL_TimeDly(1);

    csi_clk_on();
    csi_power_on();
    esKRNL_TimeDly(1);

    csi_clk_on();

    //-----------------------------------------------------------------------------------------------------------
    //配置设备的初始化设置
    // Debug sensor ID
    {

        __u8 reg_value ;
        __s32 ret ;
        ret = iic_read_16_8(fiic, da, 0x30, 0x0a, &reg_value);	// read reg value 0x300a(0X56) to debug whether sensor is ready or not
        eLIBs_printf("ret=%d ov5640 sensor id High Byte is:%x\n", ret, reg_value);
        ret = iic_read_16_8(fiic, da, 0x30, 0x0b, &reg_value);	// read reg value 0x300B(0X40) to debug whether sensor is ready or not
        eLIBs_printf("ret=%d ov5640 sensor id Low Byte is:%x\n", ret, reg_value);

    }
    block_write(fiic, da, reg_default_640_480_30000, sizeof(reg_default_640_480_30000)); //视sensor的配置看是否需要，且注意size是按__u8还是__u16来计算

    {
        __u8 temp1, temp2;

        if(iic_read_16_8(fiic, da, 0x30, 0x0A, &temp1) == EPDK_OK)
            __inf("Reg[0x300A] = 0x%02X\n", temp1);
        else
            __inf("Read Reg[0x300A] Error\n");

        if(iic_read_16_8(fiic, da, 0x30, 0x0B, &temp2) == EPDK_OK)
            __inf("Reg[0x300B] = 0x%02X\n", temp2);
        else
            __inf("Read Reg[0x300B] Error\n");

        if(iic_read_16_8(fiic, da, 0x30, 0x17, &temp2) == EPDK_OK)
            __inf("Reg[0x3017] = 0x%02X\n", temp2);
        else
            __inf("Read Reg[0x3017] Error\n");

        if(iic_read_16_8(fiic, da, 0x30, 0x18, &temp2) == EPDK_OK)
            __inf("Reg[0x3018] = 0x%02X\n", temp2);
        else
            __inf("Read Reg[0x3018] Error\n");

        if(iic_read_16_8(fiic, da, 0x38, 0x08, &temp2) == EPDK_OK)
            __inf("Reg[0x3808] = 0x%02X\n", temp2);
        else
            __inf("Read Reg[0x3808] Error\n");

        if(iic_read_16_8(fiic, da, 0x38, 0x09, &temp2) == EPDK_OK)
            __inf("Reg[0x3809] = 0x%02X\n", temp2);
        else
            __inf("Read Reg[0x3809] Error\n");

        if(iic_read_16_8(fiic, da, 0x38, 0x0a, &temp2) == EPDK_OK)
            __inf("Reg[0x380a] = 0x%02X\n", temp2);
        else
            __inf("Read Reg[0x380a] Error\n");

        if(iic_read_16_8(fiic, da, 0x38, 0x0b, &temp2) == EPDK_OK)
            __inf("Reg[0x380b] = 0x%02X\n", temp2);
        else
            __inf("Read Reg[0x380b] Error\n");
    }

    csi_dev_set_mode(1);//不要忘记设置一个默认模式


    return EPDK_OK;
}

__s32  csi_dev_exit(void)
{
    __inf("OV5640 csi_dev_exit......\n");
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
    /*
    		case CSI_COLOR_EFFECT_SEPIAGREEN:{
    			reg_table = reg_color_effect_sepiagreen;
    			reg_table_len = sizeof(reg_color_effect_sepiagreen);
    			break;
    		}
    		case CSI_COLOR_EFFECT_SEPIABLUE:{
    			reg_table = reg_color_effect_sepiablue;
    			reg_table_len = sizeof(reg_color_effect_sepiablue);
    			break;
    		}
    		case CSI_COLOR_EFFECT_SEPIARED:{
    			reg_table = reg_color_effect_sepiared;
    			reg_table_len = sizeof(reg_color_effect_sepiared);
    			break;
    		}
    		case CSI_COLOR_EFFECT_SEPIAYELLOW:{
    			reg_table = reg_color_effect_sepiayellow;
    			reg_table_len = sizeof(reg_color_effect_sepiayellow);
    			break;
    		}
    */
    default:
    {
        reg_table = reg_color_effect_normal;
        reg_table_len = sizeof(reg_color_effect_normal);
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
    default:
    {
        reg_table = reg_awb_auto;
        reg_table_len = sizeof(reg_awb_auto);
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
    /*
    	//__u32 i = 0;
    	__u8 *reg_table;//根据sensor的iic宽度而定__u8还是__u16或其他
    	__u32 reg_table_len;

    	switch (contrast){
    		case CSI_CONTRAST_N3:{
    			reg_table = reg_contrast_n3;
    			reg_table_len = sizeof(reg_contrast_n3);
    			break;
    		}
    		case CSI_CONTRAST_N2:{
    			reg_table = reg_contrast_n2;
    			reg_table_len = sizeof(reg_contrast_n2);
    			break;
    		}
    		case CSI_CONTRAST_N1:{
    			reg_table = reg_contrast_n1;
    			reg_table_len = sizeof(reg_contrast_n1);
    			break;
    		}
    		case CSI_CONTRAST_0:{
    			reg_table = reg_contrast_0;
    			reg_table_len = sizeof(reg_contrast_0);
    			break;
    		}
    		case CSI_CONTRAST_P1:{
    			reg_table = reg_contrast_p1;
    			reg_table_len = sizeof(reg_contrast_p1);
    			break;
    		}
    		case CSI_CONTRAST_P2:{
    			reg_table = reg_contrast_p2;
    			reg_table_len = sizeof(reg_contrast_p2);
    			break;
    		}
    		case CSI_CONTRAST_P3:{
    			reg_table = reg_contrast_p3;
    			reg_table_len = sizeof(reg_contrast_p3);
    			break;
    		}
    	}
    	block_write(fiic,da,reg_table,reg_table_len);
    */
    return EPDK_OK;
}


//设备band设置函数
__s32 csi_dev_set_band(__csi_band_t band)
{
    return EPDK_OK;
}


#endif

