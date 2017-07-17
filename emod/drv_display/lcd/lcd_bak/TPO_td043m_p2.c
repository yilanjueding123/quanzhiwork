/*
*********************************************************************************************************
*                            sys_config.fex 配置示例
lcd_bl_en_used           = 0
lcd_bl_en                = 0

lcd_power_used           = 1
lcd_power                = port:Ph27<1><1><1><0>

lcd_pwm_used             = 1
lcd_pwm                  = port:PA08<2><0><default><default>
*********************************************************************************************************
*/
#include "lcd_panel_cfg.h"

static __hdle hdl_1, hdl_2, hdl_3;

static void LCD_cfg_panel_info(__panel_para_t *info)
{
    memset(info, 0, sizeof(__panel_para_t));

    //屏的基本信息
    info->lcd_x                   = 800;
    info->lcd_y                   = 480;
    info->lcd_dclk_freq           = 33;  //MHz
    info->lcd_pwm_freq            = 1;  //KHz
    info->lcd_srgb                = 0x00202020;
    info->lcd_swap                = 0;

    //屏的接口配置信息
    info->lcd_if                  = 0;//0:HV , 1:8080 I/F, 2:TTL I/F, 3:LVDS

    //屏的HV模块相关信息
    info->lcd_hv_if               = 0;
    info->lcd_hv_hspw             = 0;
    info->lcd_hv_lde_iovalue      = 0;
    info->lcd_hv_lde_used         = 0;
    info->lcd_hv_smode            = 0;
    info->lcd_hv_syuv_if          = 0;
    info->lcd_hv_vspw             = 0;

    //屏的HV配置信息
    info->lcd_hbp           = 215;
    info->lcd_ht            = 1055;
    info->lcd_vbp           = 34;
    info->lcd_vt            = (2 * 525);

    //屏的IO配置信息
    info->lcd_io_cfg0             = 0x00000000;
    info->lcd_io_cfg1             = 0x00000000;
    info->lcd_io_strength         = 0;

    //TTL屏幕的配置信息
    info->lcd_ttl_ckvd            = 0;
    info->lcd_ttl_ckvh            = 0;
    info->lcd_ttl_ckvt            = 0;
    info->lcd_ttl_datainv_en      = 0;
    info->lcd_ttl_datainv_sel     = 0;
    info->lcd_ttl_datarate        = 0;
    info->lcd_ttl_oehd            = 0;
    info->lcd_ttl_oehh            = 0;
    info->lcd_ttl_oevd            = 0;
    info->lcd_ttl_oevh            = 0;
    info->lcd_ttl_oevt            = 0;
    info->lcd_ttl_revd            = 0;
    info->lcd_ttl_revsel          = 0;
    info->lcd_ttl_sthd            = 0;
    info->lcd_ttl_sthh            = 0;
    info->lcd_ttl_stvdl           = 0;
    info->lcd_ttl_stvdp           = 0;
    info->lcd_ttl_stvh            = 0;

    //cpu屏幕的配置信息
    info->lcd_frm 			 = 0;	//0: disable; 1: enable rgb666 dither; 2:enable rgb656 dither
}

static __s32 LCD_open_flow(__u32 sel)
{
    LCD_OPEN_FUNC(sel, LCD_power_on	, 10); 	//打开LCD供电，并延时10ms
    LCD_OPEN_FUNC(sel, LCD_io_init	, 20); 	//申请和初始化IO，并延时20ms
    LCD_OPEN_FUNC(sel, TCON_open	, 200); //打开LCD控制器，并延时200ms
    LCD_OPEN_FUNC(sel, LCD_open_cmd	, 10); 	//通过IO配置LCD模组为工作模式，并延时10ms
    LCD_OPEN_FUNC(sel, LCD_bl_open	, 0); 	//打开背光，并延时0ms

    return 0;
}

static __s32 LCD_close_flow(__u32 sel)
{
    LCD_CLOSE_FUNC(sel, LCD_bl_close    , 0); 	//关闭背光，并延时0ms
    LCD_CLOSE_FUNC(sel, LCD_close_cmd   , 0); 	//通过IO配置LCD模组为掉电/休眠模式，并延时0ms
    LCD_CLOSE_FUNC(sel, TCON_close	    , 0); 	//关闭LCD 控制器，并延时0ms
    LCD_CLOSE_FUNC(sel, LCD_io_exit	    , 0); 	//关屏时IO的状态并释放，并延时0ms
    LCD_CLOSE_FUNC(sel, LCD_power_off   , 1700); //关闭LCD供电，并延时1700ms

    return 0;
}


////////////////////////////////////////   POWER   ////////////////////////////////////////////////////////////////////
static void LCD_power_on(__u32 sel)//0 active
{
    LCD_POWER_EN(sel, 1);
}

static void LCD_power_off(__u32 sel)//0 active
{
    LCD_POWER_EN(sel, 0);
}

////////////////////////////////////////   back light   ////////////////////////////////////////////////////////////////////
static void LCD_bl_open(__u32 sel)
{
    LCD_BL_EN(sel, 1);
    LCD_PWM_EN(sel, 1);
}

static void LCD_bl_close(__u32 sel)
{
    LCD_BL_EN(sel, 0);
    LCD_PWM_EN(sel, 0);
}


////////////////////////////////////////   IO   ////////////////////////////////////////////////////////////////////
/*static void MPIO1_INEN(void)
{
	__u32 is_output = 0;

	OSAL_PIN_GPIO_direction_ops(hdl_1, &is_output, 0);

	LCD_delay(400);
}*/

static void MPIO1_OUTEN(void)
{
    __u32 is_output = 1;

    OSAL_GPIO_Set_One_PIN_IO_Status(hdl_1, is_output, 0);

    LCD_delay(400);
}

/*static int  MPIO1_INVALUE(void)
{
	__u32 value = 0;

	OSAL_PIN_GPIO_data_ops(hdl_1, &value, 1);

    return value;
}*/

static void MPIO1_OUTVALUE(__u32 value)
{
    OSAL_GPIO_Write_One_PIN_Value(hdl_1, value & 0x01, 0);
    LCD_delay(400);
}

/*static void MPIO2_INEN(void)
{
	__u32 is_output = 0;

	OSAL_PIN_GPIO_direction_ops(hdl_2, &is_output, 0);

	LCD_delay(400);
}*/

static void MPIO2_OUTEN(void)
{
    __u32 is_output = 1;

    OSAL_GPIO_Set_One_PIN_IO_Status(hdl_2, is_output, 0);

    LCD_delay(400);
}

/*static int  MPIO2_INVALUE(void)
{
	__u32 value = 0;

	OSAL_PIN_GPIO_data_ops(hdl_2, &value, 1);

    return value;
}*/

static void MPIO2_OUTVALUE(__u32 value)
{
    OSAL_GPIO_Write_One_PIN_Value(hdl_2, value & 0x01, 0);
    LCD_delay(400);
}

/*static void MPIO3_INEN(void)
{
	__u32 is_output = 0;

	OSAL_PIN_GPIO_direction_ops(hdl_3, &is_output, 0);

	LCD_delay(400);
}*/

static void MPIO3_OUTEN(void)
{
    __u32 is_output = 1;

    OSAL_GPIO_Set_One_PIN_IO_Status(hdl_3, is_output, 0);

    LCD_delay(400);
}

/*static int  MPIO3_INVALUE(void)
{
	__u32 value = 0;

	OSAL_PIN_GPIO_data_ops(hdl_3, &value, 1);

    return value;
}*/

static void MPIO3_OUTVALUE(__u32 value)
{
    OSAL_GPIO_Write_One_PIN_Value(hdl_3, value & 0x01, 0);
    LCD_delay(400);
}


////////////////////////////////////////HV PARALLEL:TD043MGEB1 800_480 ////////////////////////////////////////////////////////////////////
#define CS_1    MPIO3_OUTVALUE(1)
#define SCL_1   MPIO2_OUTVALUE(1)
#define SDA_1   MPIO1_OUTVALUE(1)

#define CS_0    MPIO3_OUTVALUE(0)
#define SCL_0   MPIO2_OUTVALUE(0)
#define SDA_0   MPIO1_OUTVALUE(0)

#define SDA_tri MPIO1_OUTEN()
#define SDA_va  MPIO1_INVALUE()
#define SDA_out MPIO1_OUTEN()

//序列 read 操作
/*static __u32 IIC_TPO_RREG(__u32 addr)
{
	__u32 i,data = 0;
	//cmd to start
	CS_1;
	SDA_0;
	SCL_0;
	CS_0;

	//Send addr(6bit)
	for(i=0;i<6;i++)
	{
		SCL_0;

		if(addr&BIT5)
		SDA_1;
		else
		SDA_0;

		SCL_1;
		addr = addr<<1;
	}
	//read bit
	SCL_0;
	SDA_1;
	SCL_1;

	//turn around bit
	SCL_0;
	SDA_tri;
	SCL_1;
	//read Data
	for(i=0;i<8;i++)
	{
		SCL_0;
		if(SDA_va)
		data = (data<<1) + 1;
		else
		data = (data<<1) + 0;

		SCL_1;
	}

	SCL_0;
	CS_1;
	SDA_out;
	//put_value(0x80000000,data);

	return data;
}*/

//序列 write 操作
static void IIC_TPO_WREG(__u32 addr, __u32 data)
{
    __u32 i;

    //cmd to start
    CS_1;
    SDA_0;
    SCL_0;
    CS_0;

    //Send addr(6bit)
    for(i = 0; i < 6; i++)
    {
        SCL_0;

        if(addr & BIT5)
            SDA_1;
        else
            SDA_0;

        SCL_1;
        addr = addr << 1;
    }
    //write bit
    SCL_0;
    SDA_0;
    SCL_1;

    //turn around bit
    SCL_0;
    SDA_tri;
    SCL_1;

    SDA_out;
    //write Data
    for(i = 0; i < 8; i++)
    {
        SCL_0;

        if(data & BIT7)
            SDA_1;
        else
            SDA_0;

        SCL_1;
        data = data << 1;
    }

    SCL_0;
    CS_1;

}

//申请IO,设置IO状态为输出HIGHT
static void  LCD_io_init(__u32 sel)
{
    user_gpio_set_t  gpio_set[3];

    gpio_set[0].port = 8;
    gpio_set[0].port_num = 23;
    gpio_set[0].mul_sel = 1;
    gpio_set[0].pull = 1;
    gpio_set[0].drv_level = 1;
    gpio_set[0].data = 0;
    hdl_1 = OSAL_GPIO_Request(&gpio_set[0], 1);

    gpio_set[1].port = 8;
    gpio_set[1].port_num = 24;
    gpio_set[1].mul_sel = 1;
    gpio_set[1].pull = 1;
    gpio_set[1].drv_level = 1;
    gpio_set[1].data = 0;
    hdl_2 = OSAL_GPIO_Request(&gpio_set[1], 1);

    gpio_set[2].port = 8;
    gpio_set[2].port_num = 25;
    gpio_set[2].mul_sel = 1;
    gpio_set[2].pull = 1;
    gpio_set[2].drv_level = 1;
    gpio_set[2].data = 0;
    hdl_2 = OSAL_GPIO_Request(&gpio_set[2], 1);

    MPIO1_OUTEN();
    MPIO2_OUTEN();
    MPIO3_OUTEN();

    MPIO1_OUTVALUE(1);
    MPIO2_OUTVALUE(1);
    MPIO3_OUTVALUE(1);
}

//设置IO状态为输出LOW,释放IO
static void LCD_io_exit(__u32 sel)
{
    MPIO1_OUTVALUE(0);
    MPIO2_OUTVALUE(0);
    MPIO3_OUTVALUE(0);

    OSAL_GPIO_Release(hdl_1, 0);
    OSAL_GPIO_Release(hdl_2, 0);
    OSAL_GPIO_Release(hdl_3, 0);
}

static void  LCD_open_cmd(__u32 sel)
{
    IIC_TPO_WREG(0x02, 0x07);
    IIC_TPO_WREG(0x03, 0x5F);
    IIC_TPO_WREG(0x04, 0x17);
    IIC_TPO_WREG(0x05, 0x20);
    IIC_TPO_WREG(0x06, 0x08);
    IIC_TPO_WREG(0x07, 0x20);
    IIC_TPO_WREG(0x08, 0x20);
    IIC_TPO_WREG(0x09, 0x20);
    IIC_TPO_WREG(0x0A, 0x20);
    IIC_TPO_WREG(0x0B, 0x20);
    IIC_TPO_WREG(0x0C, 0x20);
    IIC_TPO_WREG(0x0D, 0x20);
    IIC_TPO_WREG(0x0E, 0x10);
    IIC_TPO_WREG(0x0F, 0x10);
    IIC_TPO_WREG(0x10, 0x10);
    IIC_TPO_WREG(0x11, 0x15);
    IIC_TPO_WREG(0x12, 0xAA);
    IIC_TPO_WREG(0x13, 0xFF);
    IIC_TPO_WREG(0x14, 0x86);
    IIC_TPO_WREG(0x15, 0x8E);
    IIC_TPO_WREG(0x16, 0xD6);
    IIC_TPO_WREG(0x17, 0xFE);
    IIC_TPO_WREG(0x18, 0x28);
    IIC_TPO_WREG(0x19, 0x52);
    IIC_TPO_WREG(0x1A, 0x7C);
    IIC_TPO_WREG(0x1B, 0xE9);
    IIC_TPO_WREG(0x1C, 0x42);
    IIC_TPO_WREG(0x1D, 0x88);
    IIC_TPO_WREG(0x1E, 0xB8);
    IIC_TPO_WREG(0x1F, 0xFF);
    IIC_TPO_WREG(0x20, 0xF0);
    IIC_TPO_WREG(0x21, 0xF0);
    IIC_TPO_WREG(0x22, 0x08);

}

static void  LCD_close_cmd(__u32 sel)
{
    IIC_TPO_WREG(0x03, 0xde);
}

void LCD_get_panel_funs_0(__lcd_panel_fun_t *fun)
{
    fun->cfg_panel_info = LCD_cfg_panel_info;
    fun->cfg_open_flow = LCD_open_flow;
    fun->cfg_close_flow = LCD_close_flow;
}

