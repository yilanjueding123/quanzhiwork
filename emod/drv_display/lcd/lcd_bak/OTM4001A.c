/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                Display Module
*
*                                    (c) Copyright 2011-2014, Vito China
*                                             All Rights Reserved
*
* File    : ILI9327.c
* By      : Vito
* Version : v1.0
* Date    : 2011-4-27
* Descript: .
* Update  : date                auther      ver     notes
*           2011-4-27 14:07:21  Vito        1.0     Create this file.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                            sys_config.fex 配置示例
lcd_bl_en_used           = 0
lcd_bl_en                = 0

lcd_power_used           = 1
lcd_power                = port:PC04<1><1><1><0>

lcd_pwm_used             = 1
lcd_pwm                  = port:PA08<2><0><default><default>
*********************************************************************************************************
*/
#include "lcd_panel_cfg.h"

static void LCD_cfg_panel_info(__panel_para_t *info)
{
    memset(info, 0, sizeof(__panel_para_t));

    //屏的基本信息
    info->lcd_x              = 400;
    info->lcd_y              = 240;
    info->lcd_dclk_freq      = 8;  //MHz
    info->lcd_pwm_freq       = 1;  //KHz
    info->lcd_srgb           = 0x00202020;
    info->lcd_swap           = 0;

    //屏的接口配置信息
    info->lcd_if             = 1;//0:HV , 1:8080 I/F, 2:TTL I/F, 3:LVDS

    //屏的HV模块相关信息
    info->lcd_hv_if          = 0;
    info->lcd_hv_hspw        = 0;
    info->lcd_hv_lde_iovalue = 0;
    info->lcd_hv_lde_used    = 0;
    info->lcd_hv_smode       = 0;
    info->lcd_hv_syuv_if     = 0;
    info->lcd_hv_vspw        = 0;

    //屏的HV配置信息
    info->lcd_hbp            = 3;
    info->lcd_ht             = 480;
    info->lcd_vbp            = 3;
    info->lcd_vt             = (2 * 450);

    //屏的IO配置信息
    info->lcd_io_cfg0        = 0x00000000;
    info->lcd_io_cfg1        = 0x00000000;
    info->lcd_io_strength    = 0;

    //TTL屏幕的配置信息
    info->lcd_ttl_ckvd       = 0;
    info->lcd_ttl_ckvh       = 0;
    info->lcd_ttl_ckvt       = 0;
    info->lcd_ttl_datainv_en = 0;
    info->lcd_ttl_datainv_sel = 0;
    info->lcd_ttl_datarate   = 0;
    info->lcd_ttl_oehd       = 0;
    info->lcd_ttl_oehh       = 0;
    info->lcd_ttl_oevd       = 0;
    info->lcd_ttl_oevh       = 0;
    info->lcd_ttl_oevt       = 0;
    info->lcd_ttl_revd       = 0;
    info->lcd_ttl_revsel     = 0;
    info->lcd_ttl_sthd       = 0;
    info->lcd_ttl_sthh       = 0;
    info->lcd_ttl_stvdl      = 0;
    info->lcd_ttl_stvdp      = 0;
    info->lcd_ttl_stvh       = 0;

    //cpu屏幕的配置信息
    info->lcd_frm 			 = 2;	//0: disable; 1: enable rgb666 dither; 2:enable rgb565 dither
    info->lcd_cpu_if         = 4;
}

void  Lcd_cpuisr_proc(void);

static void LCD_init_isr(__u32 sel)
{
    LCD_cpu_register_irq(sel, Lcd_cpuisr_proc);
}


static void LCD_exit_isr(__u32 sel)
{
    LCD_cpu_register_irq(sel, 0);
}

static __s32 LCD_open_flow(__u32 sel)
{
    LCD_OPEN_FUNC(sel, LCD_power_on	, 10); 	//打开LCD供电，并延时10ms
    LCD_OPEN_FUNC(sel, TCON_open	, 200); //打开LCD控制器，并延时200ms
    LCD_OPEN_FUNC(sel, LCD_open_cmd	, 100); 	//通过IO配置LCD模组为工作模式，并延时100ms
    LCD_OPEN_FUNC(sel, LCD_bl_open	, 0); 	//打开背光，并延时0ms
    LCD_OPEN_FUNC(sel, LCD_init_isr, 0); //初始化CPU PANEL 中断服务程序，并延时 0ms

    return 0;
}

static __s32 LCD_close_flow(__u32 sel)
{
    LCD_CLOSE_FUNC(sel, LCD_exit_isr, 0); //关闭CPU PANEL中断服务程序,并延时0ms
    LCD_CLOSE_FUNC(sel, LCD_bl_close    , 0); 	//关闭背光，并延时0ms
    LCD_CLOSE_FUNC(sel, LCD_close_cmd   , 0); 	//通过IO配置LCD模组为掉电/休眠模式，并延时0ms
    LCD_CLOSE_FUNC(sel, TCON_close	    , 0); 	//关闭LCD 控制器，并延时0ms
    LCD_CLOSE_FUNC(sel, LCD_power_off   , 1700); //关闭LCD供电，并延时1700ms

    return 0;
}

/*
*********************************************************************************************************
*                                           POWER ON
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
static void LCD_power_on(__u32 sel)
{
    LCD_POWER_EN(sel, 1);
}

static void LCD_power_off(__u32 sel)
{
    LCD_POWER_EN(sel, 0);
}

/*
*********************************************************************************************************
*                                           BACK LIGHT ON
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
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


static void LCD_Reset(__u32 sel)
{
    //piod24
    __hdle           hdl;
    user_gpio_set_t  gpio_set[1];

    //esKRNL_TimeDly(20);

    gpio_set->port = 5;
    gpio_set->port_num = 3;
    gpio_set->mul_sel = 1;							//output
    gpio_set->pull = 1;									//pull down
    gpio_set->drv_level = 1;
    gpio_set->data = 1;									//

    hdl = OSAL_GPIO_Request(gpio_set, 1);
    OSAL_GPIO_Release(hdl, 2);
    __msg("LCD Reset high....\n");

    esKRNL_TimeDly(20);

    gpio_set->port = 5;
    gpio_set->port_num = 3;
    gpio_set->mul_sel = 1;							//output
    gpio_set->pull = 0;									//pull down
    gpio_set->drv_level = 1;
    gpio_set->data = 0;									//

    hdl = OSAL_GPIO_Request(gpio_set, 1);
    OSAL_GPIO_Release(hdl, 2);
    __msg("select panel is %d\n", sel);
    __msg("LCD Reset....\n");
    //__wait__;

    esKRNL_TimeDly(50);

    gpio_set->port = 4;
    gpio_set->port_num = 24;
    gpio_set->mul_sel = 1;							//output
    gpio_set->pull = 1;									//pull down
    gpio_set->drv_level = 1;
    gpio_set->data = 1;									//

    hdl = OSAL_GPIO_Request(gpio_set, 1);
    OSAL_GPIO_Release(hdl, 2);
    //sys_put_wvalue(0xf1c20848, (sys_get_wvalue(0xf1c20848)&0xfff0ffff)|(0x1<<16));
    //sys_put_wvalue(0xf1c20858, (sys_get_wvalue(0xf1c20858)&0xffffffef));
}


void  lcd_cpu_cmdset()
{
    //--------------- Power supply condition: VCC=IOVCC=VCI=2.8V ---------------//
    //----------------------Power on sequence-----------------------------------------------//-
    LCD_CPU_WR(0, 0x0606, 0x0000);
    LCD_delay(6000);
    LCD_CPU_WR(0, 0x0007, 0x0001);
    LCD_delay(6000);
    LCD_CPU_WR(0, 0x0110, 0x0001);
    LCD_delay(6000);
    LCD_CPU_WR(0, 0x0100, 0x17B0);
    LCD_CPU_WR(0, 0x0101, 0x0147);
    LCD_CPU_WR(0, 0x0102, 0x019D);
    LCD_CPU_WR(0, 0x0103, 0x3600);
    LCD_CPU_WR(0, 0x0281, 0x0010);
    LCD_delay(6000);
    LCD_CPU_WR(0, 0x0102, 0x01BD);
    LCD_delay(6000);
    LCD_CPU_WR(0, 0x0000, 0x0000);
    LCD_CPU_WR(0, 0x0001, 0x0100);
    LCD_CPU_WR(0, 0x0002, 0x0100);
    LCD_CPU_WR(0, 0x0003, 0x1018);
    LCD_CPU_WR(0, 0x0006, 0x0000);
    LCD_CPU_WR(0, 0x0008, 0x0503);
    LCD_CPU_WR(0, 0x0009, 0x0001);
    LCD_CPU_WR(0, 0x000B, 0x0010);
    LCD_CPU_WR(0, 0x000C, 0x0000);
    LCD_CPU_WR(0, 0x000F, 0x0000);
    LCD_CPU_WR(0, 0x0007, 0x0001);
    //-----------------------------Panel interface control-------------------------//
    LCD_CPU_WR(0, 0x0010, 0x0014);
    LCD_CPU_WR(0, 0x0011, 0x0202);
    LCD_CPU_WR(0, 0x0012, 0x0300);
    LCD_CPU_WR(0, 0x0020, 0x021e);
    LCD_CPU_WR(0, 0x0021, 0x0202);
    LCD_CPU_WR(0, 0x0090, 0x8000);
    LCD_CPU_WR(0, 0x0092, 0x0000);
    //------------------------------------Voltage setting----------------------//
    LCD_CPU_WR(0, 0x0100, 0x13b0);
    LCD_delay(6000);
    LCD_CPU_WR(0, 0x0101, 0x0147);
    LCD_delay(6000);
    LCD_CPU_WR(0, 0x0102, 0x018D);
    LCD_delay(6000);
    LCD_CPU_WR(0, 0x0103, 0x0200);
    LCD_delay(6000);
    LCD_CPU_WR(0, 0x0107, 0x0000);
    LCD_delay(6000);
    LCD_CPU_WR(0, 0x0110, 0x0001);
    LCD_delay(6000);
    //-----------------------------------------------------------------------//
    LCD_CPU_WR(0, 0x0210, 0x0000);
    LCD_CPU_WR(0, 0x0211, 0x00ef);
    LCD_CPU_WR(0, 0x0212, 0x0000);
    LCD_CPU_WR(0, 0x0213, 0x018f);
    LCD_CPU_WR(0, 0x0200, 0x0000);
    LCD_CPU_WR(0, 0x0201, 0x0000);
    LCD_CPU_WR(0, 0x0280, 0x0000);
    LCD_CPU_WR(0, 0x0281, 0x0000);
    LCD_CPU_WR(0, 0x0282, 0x0000);
    LCD_delay(6000);
    //--------------------------------Gamma setting---------------------------//
    LCD_CPU_WR(0, 0x0300, 0x0010);
    LCD_CPU_WR(0, 0x0301, 0x0234);
    LCD_CPU_WR(0, 0x0302, 0x0438);
    LCD_CPU_WR(0, 0x0303, 0x3704);
    LCD_CPU_WR(0, 0x0304, 0x3402);
    LCD_CPU_WR(0, 0x0305, 0x1000);
    LCD_CPU_WR(0, 0x0306, 0x1403);
    LCD_CPU_WR(0, 0x0307, 0x0314);
    LCD_CPU_WR(0, 0x0308, 0x0305);
    LCD_CPU_WR(0, 0x0309, 0x0305);
    LCD_CPU_WR(0, 0x030A, 0x0D06);
    LCD_CPU_WR(0, 0x030B, 0x0E02);
    LCD_CPU_WR(0, 0x030C, 0x020E);
    LCD_CPU_WR(0, 0x030D, 0x060D);
    LCD_CPU_WR(0, 0x030E, 0x0403);
    LCD_CPU_WR(0, 0x030F, 0x0503);
    //-----------------------------------------------------------------------//
    LCD_CPU_WR(0, 0x0400, 0x3500);
    LCD_CPU_WR(0, 0x0401, 0x0001);
    LCD_CPU_WR(0, 0x0404, 0x0000);
    LCD_CPU_WR(0, 0x0500, 0x0000);
    LCD_CPU_WR(0, 0x0501, 0x0000);
    LCD_CPU_WR(0, 0x0502, 0x0000);
    LCD_CPU_WR(0, 0x0503, 0x0000);
    LCD_CPU_WR(0, 0x0504, 0x0000);
    LCD_CPU_WR(0, 0x0505, 0x0000);
    LCD_CPU_WR(0, 0x0600, 0x0000);
    LCD_CPU_WR(0, 0x0606, 0x0000);
    LCD_CPU_WR(0, 0x06F0, 0x0000);
    //----------------------------- Orise mode ----------------------------------//
    LCD_CPU_WR(0, 0x07F0, 0x5420);
    LCD_CPU_WR(0, 0x07F3, 0x288b);
    LCD_CPU_WR(0, 0x07F4, 0x0022);
    LCD_CPU_WR(0, 0x07F5, 0x0011);
    LCD_CPU_WR(0, 0x07F0, 0x0000);
    LCD_CPU_WR(0, 0x0007, 0x0173);
    LCD_delay(6000);
    LCD_CPU_WR(0, 0x0007, 0x0061);
    LCD_delay(6000);
    LCD_CPU_WR(0, 0x0007, 0x0173);
    LCD_delay(6000);

    __msg("\n\n\n\n\n\n\n     lcd_cpu_cmdset()        \n\n\n\n\n\n\n\n\n");
}

static void  LCD_open_cmd(__u32 sel)
{
    __u32 id;
    //LCD_Reset(0);
    LCD_delay(60000);
    __inf("LCD_BOOT: wait to power on!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    LCD_CPU_AUTO_FLUSH(0, 0);
    __inf("LCD_BOOT: disable auto mode for setting!!!!!!!!!!!!!!!!\n");

    LCD_delay(600);

    //LCD_CPU_RD(0x0000,&id);
    __inf("LCD_BOOT: id to read back is %x\n", id);

    //setting panel
    lcd_cpu_cmdset();
    __inf("LCD_BOOT: lcd_cpu_cmdset!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    //command to send data
    LCD_CPU_WR_INDEX(0, 0x0202);
    __inf("LCD_BOOT: LCD_CPU_WR_INDEX!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    LCD_CPU_AUTO_FLUSH(0, 1);
    __inf("LCD_BOOT: LCD_CPU_AUTO_FLUSH !!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

static void  LCD_close_cmd(__u32 sel)
{

}

void  Lcd_cpuisr_proc(void)
{

    //if(lcd_onoff_status == 1)
    {
        LCD_CPU_WR(0, 0x0200, 0x0000);   //INT GRAM address
        LCD_CPU_WR(0, 0x0201, 0x0000);   //INT GRAM address
        LCD_CPU_WR_INDEX(0, 0x0202);
        //__msg("LCD: Lcd_cpuisr_proc\n");
    }
}

void LCD_get_panel_funs_0(__lcd_panel_fun_t *fun)
{
    fun->cfg_panel_info = LCD_cfg_panel_info;
    fun->cfg_open_flow = LCD_open_flow;
    fun->cfg_close_flow = LCD_close_flow;
}
