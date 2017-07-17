
#include  "csi_dev_i.h"

#if(CSI_DEV_TYPE == CSI_DEV_TYPE_OV7725)

#define MIRROR 0//0 = normal, 1 = mirror
#define FLIP   0//0 = normal, 1 = flip

#define	TEST_CSI_OV7725	1
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

static  __u8 da = 0x42 >> 1; //设备地址，为i2c写地址左移 1 bit
static __u32 reg_step = 2; //一次写操作所包含的寄存器地址+寄存器值的byte数

static __u8 reg_init[] =
{
    //init VGA@25fps
    0x12, 0x80,

    0x09, 0x03, //drive capability 00/01/02/03

    0x0c, 0x10 + MIRROR * 0x40 + FLIP * 0x80, //mirror and flip,

    0x3d, 0x03,
    0x17, 0x22,
    0x18, 0xa4,
    0x19, 0x07,
    0x1a, 0xf0,
    0x32, 0x00,
    0x11, 0x01, //00/01/03/07 for 60/30/15/7.5fps

    0x42, 0x7f,
    0x4d, 0x09, //0x09//0x00
    0x63, 0xe0,
    0x64, 0xff,
    0x65, 0x20,
    0x66, 0x00,
    0x67, 0x48,

    0x13, 0xf0,
    0x0d, 0x41, //51/61/71 for different AEC/AGC window
    0x0f, 0xc5,
    0x14, 0x11, //0x11//0x21

    0x22, 0x7f, //ff/7f/3f/1f for 60/30/15/7.5fps
    0x23, 0x03, //01/03/07/0f for 60/30/15/7.5fps

    0x24, 0x40,
    0x25, 0x30,
    0x26, 0x82, //0xa1,

    0x29, 0xa0,
    0x2c, 0xf0,

    //0x28,0x40,
    0x2a, 0x00, //dummy pixel MSB
    0x2b, 0x9e, //dummy pixle LSB //24MHz->00/9e for 60/50Hz, will change fps, 60hz->60fps series, 50->50

    0x2e, 0x00, //dummy row MSB in Vsync
    0x2d, 0x00, //0x0f,//dummy row LSB//24MHz/24.75MHz->00/0f

    0x34, 0x00,
    0x33, 0x0f,

    0x6b, 0xaa,
    0x13, 0xff,

    0x90, 0x05,
    0x91, 0x01,
    0x92, 0x03,
    0x93, 0x00,
    0x94, 0xb0,
    0x95, 0x9d,
    0x96, 0x13,
    0x97, 0x16,
    0x98, 0x7b,
    0x99, 0x91,
    0x9a, 0x1e,
    0x9b, 0x08,
    0x9c, 0x20,
    0x9e, 0x81,
    0xa6, 0x04,

    0x7e, 0x0c,
    0x7f, 0x16,
    0x80, 0x2a,
    0x81, 0x4e,
    0x82, 0x61,
    0x83, 0x6f,
    0x84, 0x7b,
    0x85, 0x86,
    0x86, 0x8e,
    0x87, 0x97,
    0x88, 0xa4,
    0x89, 0xaf,
    0x8a, 0xc5,
    0x8b, 0xd7,
    0x8c, 0xe8,
    0x8d, 0x20,

    //sat
    0xa7, 0x30,
    0xa8, 0x30,
    0xa6, 0x02 | 0x04, //0x02,contrast+sat

    //lens
    0x47, 0x07,
    0x48, 0xa2,
    0x4a, 0x05,
    0x49, 0x20,
    0x4b, 0x1f,
    0x4c, 0x26,
    0x46, 0x05,


};


static __u8 reg_7725_640_480_25000[] =
{
    0x11, 0x01, //00/01/03/07 for 60/30/15/7.5fps

    0x22, 0x7f, //ff/7f/3f/1f for 60/30/15/7.5fps
    0x23, 0x03, //01/03/07/0f for 60/30/15/7.5fps

    0x2a, 0x00, //dummy pixel MSB
    0x2b, 0x9e, //dummy pixle LSB //24MHz->00/9e for 60/50Hz, will change fps, 60hz->60fps series, 50->50

    0x2e, 0x00, //dummy row MSB in Vsync
    0x2d, 0x00, //0x0f,//dummy row LSB//24MHz/24.75MHz->00/0f, 0x0f=510*(1.03-1)

    0x34, 0x00,
    0x33, 0x0f,
};


static __u8 reg_7725_640_480_50000[] =
{
    0x11, 0x00, //00/01/03/07 for 60/30/15/7.5fps

    0x22, 0xff, //ff/7f/3f/1f for 60/30/15/7.5fps
    0x23, 0x01, //01/03/07/0f for 60/30/15/7.5fps

    0x2a, 0x00, //dummy pixel MSB
    0x2b, 0x9e, //dummy pixle LSB //24MHz->00/9e for 60/50Hz, will change fps, 60hz->60fps series, 50->50

    0x2e, 0x00, //dummy row MSB in Vsync
    0x2d, 0x00, //0x0f,//dummy row LSB//24MHz/24.75MHz->00/0f

    0x34, 0x00,
    0x33, 0x0f,
};


static __u8 reg_band_50hz[] =
{
    0x2b, 0x9e, //dummy pixle LSB //24MHz->00/9e for 60/50Hz, will change fps, 60hz->60fps series, 50->50
};

static __u8 reg_band_60hz[] =
{
    0x2b, 0x00, //dummy pixle LSB //24MHz->00/9e for 60/50Hz, will change fps, 60hz->60fps series, 50->50
};


static __u8 reg_color_effect_normal[] =
{
    0xa6, 0x04 | 0x02, //SDE disable,contrast+sat
};
static __u8 reg_color_effect_grayscale[] =
{
    0xa6, 0x24 | 0x02, //
};
static __u8 reg_color_effect_colorinv[] =
{
    0xa6, 0x44 | 0x02, //
};
static __u8 reg_color_effect_sepia[] =
{
    0x60, 0x18, //fix U
    0x61, 0xa0, //fix V
    0xa6, 0x1c | 0x02, //fix UV en
};
static __u8 reg_color_effect_sepiagreen[] =
{
    0x60, 0x48, //fix U
    0x61, 0x60, //fix V
    0xa6, 0x1c | 0x02, //fix UV en
};
static __u8 reg_color_effect_sepiablue[] =
{
    0x60, 0x88, //fix U
    0x61, 0x88, //fix V
    0xa6, 0x1c | 0x02, //fix UV en
};
static __u8 reg_color_effect_sepiared[] =
{
    0x60, 0x88, //fix U
    0x61, 0x88, //fix V
    0xa6, 0x1c | 0x02, //fix UV en
};
static __u8 reg_color_effect_sepiayellow[] =
{
    0x60, 0x88, //fix U
    0x61, 0x88, //fix V
    0xa6, 0x1c | 0x02, //fix UV en
};


static __u8 reg_awb_auto[] =
{
    0x13, 0xff,
    0x0e, 0x65,
};
static __u8 reg_awb_daylight[] = //6500k
{
    //0x13,0xef,
    //0x0e,0x65,
    //WB sunny
    0x13, 0xfd,
    0x01, 0x5a,
    0x02, 0x5c,
    0x0e, 0x65,
};
static __u8 reg_awb_fluorescent[] = //5000k
{
    //0x13,0xef,
    //0x0e,0x65,
    //WB Office
    0x13, 0xfd,
    0x01, 0x84,
    0x02, 0x4c,
    0x0e, 0x65,
};
static __u8 reg_awb_cloudy[] = //7500k
{
    //0x13,0xef,
    //0x0e,0x65,
    //WB Cloudy
    0x13, 0xfd,
    0x01, 0x58,
    0x02, 0x60,
    0x0e, 0x65,
};
static __u8 reg_awb_tungsten[] = //
{
    //0x13,0xfe,
    //0x0e,0x65,
    //WB HOME
    0x13, 0xfd,
    0x01, 0x96,
    0x02, 0x40,
    0x0e, 0x65,
};

//static __u8 reg_awb_night[]=//
//{
////WB NIGHT
//0x13,0xff,
//0x0e,0xe5,
//};



static __u8 reg_ae_n3[] = //-3
{
    0x24, 0x30 + 8,
    0x25, 0x20 - 8,
    0x26, 0xb2, //
};
static __u8 reg_ae_n2[] = //-2
{
    0x24, 0x38 + 8,
    0x25, 0x28 - 8,
    0x26, 0xb2, //
};
static __u8 reg_ae_n1[] = //-1
{
    0x24, 0x40 + 8,
    0x25, 0x30 - 8,
    0x26, 0xb2, //
};
static __u8 reg_ae_0[] = //0
{
    0x24, 0x48 + 8,
    0x25, 0x38 - 8,
    0x26, 0xb2, //
};
static __u8 reg_ae_p1[] = //1
{
    0x24, 0x50 + 8,
    0x25, 0x40 - 8,
    0x26, 0xb2, //
};
static __u8 reg_ae_p2[] = //2
{
    0x24, 0x58 + 8,
    0x25, 0x48 - 8,
    0x26, 0xb2, //
};
static __u8 reg_ae_p3[] = //3
{
    0x24, 0x68 + 8,
    0x25, 0x50 - 8,
    0x26, 0xb2, //
};


static __u8 reg_bright_n3[] = //-3
{
    //0xa6,0x04,
    0xab, 0x06 + 0x08,
    0x9b, 0x28,
};
static __u8 reg_bright_n2[] = //-2
{
    //0xa6,0x04,
    0xab, 0x06 + 0x08,
    0x9b, 0x18,
};
static __u8 reg_bright_n1[] = //-1
{
    //0xa6,0x04,
    0xab, 0x06 + 0x08,
    0x9b, 0x08,
};
static __u8 reg_bright_0[] = //0
{
    //0xa6,0x04,
    0xab, 0x06,
    0x9b, 0x00, //0x08
};
static __u8 reg_bright_p1[] = //1
{
    //0xa6,0x04,
    0xab, 0x06,
    0x9b, 0x08,
};
static __u8 reg_bright_p2[] = //2
{
    //0xa6,0x04,
    0xab, 0x06,
    0x9b, 0x18,
};
static __u8 reg_bright_p3[] = //3
{
    //0xa6,0x04,
    0xab, 0x06,
    0x9b, 0x28,
};


static __u8 reg_contrast_n3[] = //-3
{
    //0xa6,0x04,
    0x9c, 0x08,
};
static __u8 reg_contrast_n2[] = //-2
{
    //0xa6,0x04,
    0x9c, 0x10,
};
static __u8 reg_contrast_n1[] = //-1
{
    //0xa6,0x04,
    0x9c, 0x18,
};
static __u8 reg_contrast_0[] = //0
{
    //0xa6,0x04,
    0x9c, 0x20,
};
static __u8 reg_contrast_p1[] = //1
{
    //0xa6,0x04,
    0x9c, 0x30,
};
static __u8 reg_contrast_p2[] = //2
{
    //0xa6,0x04,
    0x9c, 0x40,
};
static __u8 reg_contrast_p3[] = //3
{
    //0xa6,0x04,
    0x9c, 0x50,
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
    return 0;	// for test
    reg_step = 2;

    for (i = 0; i < length; i += reg_step)
    {
        //__u8 iic_val=0;
        //__u8 tmp=0;
        //__inf("8_8\n");
        if(iic_write_8_8(hdl_iic, slv_addr, reg[i], reg[i + 1]) != EPDK_OK)
        {
            __wrn("csi0 iic write fail %x = %x\n", reg[i], reg[i + 1]);
        }
        if (i == 0)esKRNL_TimeDly(1); //must wait
        //__inf("csi0 iic write %d, %x = %x\n", ret, reg[i],reg[i+1]);
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
        case 0: // for CSI X1
        {
            reg_table = reg_7725_640_480_25000;
            reg_table_len = sizeof( reg_7725_640_480_25000 );
            conf.input_fmt  = CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV422;//CSI_PLANAR_YUV422;//420 for preview and encode;422 for picture
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_YUYV;//根据实际修改
            conf.vref       = CSI_LOW;//根据实际修改
            conf.href       = CSI_HIGH;//根据实际修改
            conf.clock      = CSI_RISING;//根据实际修改

            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width);
            break;
        }
        case 1: // for CSI X1
        {
            reg_table = reg_7725_640_480_25000;
            reg_table_len = sizeof( reg_7725_640_480_25000 );
            conf.input_fmt  = CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV420;//420 for preview and encode;422 for picture
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
        case 2: // for CSI X1
        {
            reg_table = reg_7725_640_480_50000;
            reg_table_len = sizeof( reg_7725_640_480_50000 );
            conf.input_fmt  = CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV420;//420 for preview and encode;422 for picture
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
        case 3: //for pc cam
        {
            reg_table = reg_7725_640_480_25000;
            reg_table_len = sizeof(reg_7725_640_480_25000);
            conf.input_fmt  = CSI_RAW;
            conf.output_fmt = CSI_PASS_THROUTH;//CSI_PLANAR_YUV422;//420 for preview and encode;422 for picture
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_YUYV;//根据实际修改
            conf.vref       = CSI_LOW;//根据实际修改
            conf.href       = CSI_HIGH;//根据实际修改
            conf.clock      = CSI_RISING;//根据实际修改

            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width * 2);

            break;
        }
            //        case 4:{//for CSI X2
            //            reg_table = reg_7725_640_480_25000;
            //            reg_table_len = sizeof( reg_7725_640_480_25000 );
            //            conf.input_fmt  = CSI_YUV422;
            //            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV420;//420 for preview and encode;422 for picture
            //            conf.field_sel  = CSI_EVEN;
            //            conf.seq        = CSI_YUYV;
            //            conf.vref       = CSI_LOW;
            //            conf.href       = CSI_HIGH;
            //            conf.clock      = CSI_RISING;
            //            CSI_configure(&conf);
            //            csi_mode = csi_mode_all.csi_mode[mode];
            //            CSI_set_size(0, csi_mode.size.width, 0, csi_mode.size.height, csi_mode.size.width);
            //
            ////            CSI_configure_1(&conf);
            ////            CSI_set_size_1(0, csi_mode.size.width, 0, csi_mode.size.height, csi_mode.size.width);
            //
            //            break;
            //        }
            //        case 5:{//for CSI X2
            //            reg_table = reg_7725_640_480_50000;
            //            reg_table_len = sizeof( reg_7725_640_480_50000 );
            //            conf.input_fmt  = CSI_YUV422;
            //            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV420;//420 for preview and encode;422 for picture
            //            conf.field_sel  = CSI_EVEN;
            //            conf.seq        = CSI_YUYV;
            //            conf.vref       = CSI_LOW;
            //            conf.href       = CSI_HIGH;
            //            conf.clock      = CSI_RISING;
            //            CSI_configure(&conf);
            //            csi_mode = csi_mode_all.csi_mode[mode];
            //            CSI_set_size(0, csi_mode.size.width, 0, csi_mode.size.height, csi_mode.size.width);
            //
            ////            CSI_configure_1(&conf);
            ////            CSI_set_size_1(0, csi_mode.size.width, 0, csi_mode.size.height, csi_mode.size.width);
            //
            //            break;
            //        }

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

static __s32 ov7725_iic_read(ES_FILE *hdl_iic, __u8 slv_addr, __u8 addr, __u8 *data)
{
    __u8 iic_ret;
    __twi_dev_para_ex_t  siic;
    __u8 iic_addr[1];
    __u8 *iic_data;

    iic_addr[0] = addr;
    iic_data = data;

    siic.slave_addr        = slv_addr;
    siic.slave_addr_flag   = TWI_SLAVE_ADDRESS_07BIT;
    siic.byte_addr         = iic_addr;
    siic.byte_addr_width   = 1;
    siic.data              = iic_data;
    siic.byte_count        = 1;

    iic_ret = csi_fioctrl(hdl_iic, TWI_READ_EX_STP_RS,  0, (void *)&siic);
    //__inf("csi iic read data %s %x=%x\n",(iic_ret==EPDK_OK)?"OK":"FAIL", iic_addr[0],iic_data[0]);

    return iic_ret;
}

__s32  csi_dev_init(void)
{
    //__u32 i = 0;
    __u8 *reg_table;
    __u32 reg_table_len;

    iic_set_clock(fiic, 200 * 1000); //200*1000=200kHz

    __inf("csi_dev_init......\n");

    //填写本设备支持的模式
    csi_mode_all.number = 6;//所支持模式数量
    //手动改，视方案需求，最大为8个，至少保证2个mode，一个pic用YUV422_NONMB_UVCB，一个video用YUV420_NONMB_UVCB
    //有pccam需要加多一个YUV422_YUV_MOD_INTERLEAVED的VGA或更小尺寸的模式

    //这些设置都是指输出的数据参数，为了传递给显示或者编码时的帧参数
    //=====================csi X1================================
    //640*400@25#yuv420
    csi_mode_all.csi_mode[0].color_format = PIXEL_YUV422;//格式
    csi_mode_all.csi_mode[0].component_seq = YUV_SEQ_UVUV;//排列顺序
    csi_mode_all.csi_mode[0].store_mode = YUV_MOD_UV_NON_MB_COMBINED;//YUV_MOD_NON_MB_PLANAR;//存储格式
    csi_mode_all.csi_mode[0].size.width = 640;//宽
    csi_mode_all.csi_mode[0].size.height = 480;//高
    csi_mode_all.csi_mode[0].frame_rate = 25000 * 1; //帧率，单位Hz
    csi_mode_all.csi_mode[0].frame_period = 40000 / 1; //帧长（=1/帧率，单位us）
    csi_mode_all.csi_mode[0].color_space = BT601;//色彩空间
    csi_mode_all.csi_mode[0].csi_buf_scrab_mode = CSI0_FULL;
    //640*400@25#yuv422
    csi_mode_all.csi_mode[1].color_format = PIXEL_YUV420;
    csi_mode_all.csi_mode[1].component_seq = YUV_SEQ_UVUV;
    csi_mode_all.csi_mode[1].store_mode = YUV_MOD_UV_NON_MB_COMBINED;
    csi_mode_all.csi_mode[1].size.width = 640;
    csi_mode_all.csi_mode[1].size.height = 480;
    csi_mode_all.csi_mode[1].frame_rate = 25000 * 1;
    csi_mode_all.csi_mode[1].frame_period = 40000 / 1;
    csi_mode_all.csi_mode[1].color_space = BT601;
    csi_mode_all.csi_mode[1].csi_buf_scrab_mode = CSI0_FULL;
    //640*400@50#yuv422
    csi_mode_all.csi_mode[2].color_format = PIXEL_YUV420;
    csi_mode_all.csi_mode[2].component_seq = YUV_SEQ_UVUV;
    csi_mode_all.csi_mode[2].store_mode = YUV_MOD_UV_NON_MB_COMBINED;
    csi_mode_all.csi_mode[2].size.width = 640;
    csi_mode_all.csi_mode[2].size.height = 480;
    csi_mode_all.csi_mode[2].frame_rate = 25000 * 2;
    csi_mode_all.csi_mode[2].frame_period = 40000 / 2;
    csi_mode_all.csi_mode[2].color_space = BT601;
    csi_mode_all.csi_mode[2].csi_buf_scrab_mode = CSI0_FULL;

    //=====================pc cam================================
    //640*400@50#interleave yuv420
    csi_mode_all.csi_mode[3].color_format = PIXEL_YUV422;
    csi_mode_all.csi_mode[3].component_seq = YUV_SEQ_YUYV;
    csi_mode_all.csi_mode[3].store_mode = YUV_MOD_INTERLEAVED;
    csi_mode_all.csi_mode[3].size.width = 640;
    csi_mode_all.csi_mode[3].size.height = 480;
    csi_mode_all.csi_mode[3].frame_rate = 25000;
    csi_mode_all.csi_mode[3].frame_period = 40000;
    csi_mode_all.csi_mode[3].color_space = BT601;
    csi_mode_all.csi_mode[3].csi_buf_scrab_mode = CSI0_FULL;

    ////=====================csi X2================================
    //   //640*480@25#yuv422
    //   csi_mode_all.csi_mode[4].color_format = PIXEL_YUV420;
    //   csi_mode_all.csi_mode[4].component_seq = YUV_SEQ_UVUV;
    //   csi_mode_all.csi_mode[4].store_mode = YUV_MOD_UV_NON_MB_COMBINED;
    //   csi_mode_all.csi_mode[4].size.width = 640*2;
    //   csi_mode_all.csi_mode[4].size.height = 480;
    //   csi_mode_all.csi_mode[4].frame_rate = 25000*1;
    //   csi_mode_all.csi_mode[4].frame_period = 40000/1;
    //   csi_mode_all.csi_mode[4].color_space = BT601;
    //   csi_mode_all.csi_mode[4].csi_buf_scrab_mode = CSI_01_LR;
    //   //640*480@50#yuv422
    //   csi_mode_all.csi_mode[5].color_format = PIXEL_YUV420;
    //   csi_mode_all.csi_mode[5].component_seq = YUV_SEQ_UVUV;
    //   csi_mode_all.csi_mode[5].store_mode = YUV_MOD_UV_NON_MB_COMBINED;
    //   csi_mode_all.csi_mode[5].size.width = 640*2;
    //   csi_mode_all.csi_mode[5].size.height = 480;
    //   csi_mode_all.csi_mode[5].frame_rate = 25000*2;
    //   csi_mode_all.csi_mode[5].frame_period = 40000/2;
    //   csi_mode_all.csi_mode[5].color_space = BT601;
    //   csi_mode_all.csi_mode[5].csi_buf_scrab_mode = CSI_01_LR;

#if 0//TEST_CSI_OV7725
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
#else

    csi_clk_off();
    csi_power_off();

    esKRNL_TimeDly(1);

    csi_clk_on();
    csi_power_on();

    esKRNL_TimeDly(1);

    csi_clk_on();
#endif

    __inf("7725_init...\n");

    //配置设备的初始化设置
    reg_table = reg_init;
    reg_table_len = sizeof(reg_init);
    __inf("7725_reg_size=%d\n", reg_table_len);
    reg_table = reg_init;
    block_write(fiic, da, reg_table, reg_table_len);

#if TEST_CSI_OV7725
    {

        __u8 reg_value ;
        if(ov7725_iic_read(fiic, da, 0x0A, &reg_value) == EPDK_OK)
        {
            __wrn("csi0 iic read PID =%x\n", reg_value);
        }
        else
        {
            __wrn("csi0 iic read fail %x \n", 0x0A);
        }
        if(ov7725_iic_read(fiic, da, 0x0B, &reg_value) == EPDK_OK)
        {
            __wrn("csi0 iic read VER =%x\n", reg_value);
        }
        else
        {
            __wrn("csi0 iic read fail %x\n", 0x0B);
        }

    }
#endif
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
    __u8 *reg_table;
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
    //__inf("set color\n");
    block_write(fiic, da, reg_table, reg_table_len);
    return EPDK_OK;
}
//设备awb设置函数
__s32 csi_dev_set_awb(__csi_awb_t awb)
{
    //__u32 i = 0;
    __u8 *reg_table;
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
    //__inf("set awb\n");
    block_write(fiic, da, reg_table, reg_table_len);
    return EPDK_OK;
}
//设备ae设置函数
__s32 csi_dev_set_ae(__csi_ae_t ae)
{
    //__u32 i = 0;
    __u8 *reg_table;
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
    //__inf("set ae\n");
    block_write(fiic, da, reg_table, reg_table_len);
    return EPDK_OK;
}
//设备bright设置函数
__s32 csi_dev_set_bright(__csi_bright_t bright)
{
    //__u32 i = 0;
    __u8 *reg_table;
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
    //__inf("set bright\n");
    block_write(fiic, da, reg_table, reg_table_len);
    return EPDK_OK;
}

//设备contrast设置函数
__s32 csi_dev_set_contrast(__csi_contrast_t contrast)
{
    //__u32 i = 0;
    __u8 *reg_table;
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
    //__inf("set contrast\n");
    block_write(fiic, da, reg_table, reg_table_len);
    return EPDK_OK;
}

//设备band设置函数
__s32 csi_dev_set_band(__csi_band_t band)
{
    //__u32 i = 0;
    __u8 *reg_table;
    __u32 reg_table_len;

    switch (band)
    {
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

    block_write(fiic, da, reg_table, reg_table_len);
    return EPDK_OK;
}


#endif

