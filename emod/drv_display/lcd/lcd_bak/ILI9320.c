/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                Display Module
*
*                                    (c) Copyright 2011-2014, Vito China
*                                             All Rights Reserved
*
* File    : ILI9325.c
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
    info->lcd_x              = 320;
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
    info->lcd_frm 			 = 2;	//0: disable; 1: enable rgb666 dither; 2:enable rgb656 dither
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

void  lcd_cpu_cmdset()
{



    //************* Start Initial Sequence **********//


    LCD_CPU_WR(0, 0x00E3, 0x3008); // Set internal timing
    LCD_CPU_WR(0, 0x00E7, 0x0012); // Set internal timing
    LCD_CPU_WR(0, 0x00EF, 0x1231); // Set internal timing
    LCD_CPU_WR(0, 0x0001, 0x0100); // set SS and SM bit
    LCD_CPU_WR(0, 0x0002, 0x0700); // set 1 line inversion
#if 0// (PANELSCANFORM == MCATCH_ROTATE_90)

    LCD_CPU_WR(0, 0x0003, 0x1030); // set GRAM write direction and BGR=1.
#else
    LCD_CPU_WR(0, 0x0003, 0x1008); // set GRAM write direction and BGR=1.
#endif
    //LCD_CPU_WR(0x0003, 0x1020); // set GRAM write direction and BGR=1.
    LCD_CPU_WR(0, 0x0004, 0x0000); // Resize register
    LCD_CPU_WR(0, 0x0008, 0x0207); // set the back porch and front porch
    LCD_CPU_WR(0, 0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
    LCD_CPU_WR(0, 0x000A, 0x0000); // FMARK function
    LCD_CPU_WR(0, 0x000C, 0x0000); // RGB interface setting
    LCD_CPU_WR(0, 0x000D, 0x0000); // Frame marker Position
    LCD_CPU_WR(0, 0x000F, 0x0000); // RGB interface polarity
    //*************Power On sequence ****************//
    LCD_CPU_WR(0, 0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
    LCD_CPU_WR(0, 0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
    LCD_CPU_WR(0, 0x0012, 0x0000); // VREG1OUT voltage
    LCD_CPU_WR(0, 0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
    //LCD_CPU_WR(0,0x0007, 0x0001);

    LCD_delay(60000);			        //10ms


    LCD_CPU_WR(0, 0x0010, 0x1490); //1590 SAP, BT[3:0], AP, DSTB, SLP, STB
    LCD_CPU_WR(0, 0x0011, 0x0227); // DC1[2:0], DC0[2:0], VC[2:0]
    LCD_delay(60000);			        //10ms
    LCD_CPU_WR(0, 0x0012, 0x001c); // Internal reference voltage= Vci;
    LCD_delay(60000);			        //10ms
    LCD_CPU_WR(0, 0x0013, 0x1A00); // Set VDV[4:0] for VCOM amplitude
    LCD_CPU_WR(0, 0x0029, 0x0025); // Set VCM[5:0] for VCOMH
    LCD_CPU_WR(0, 0x002B, 0x000C); // Set Frame Rate
    LCD_delay(60000);			        //10ms
    LCD_CPU_WR(0, 0x0020, 0x0000); // GRAM horizontal Address
    LCD_CPU_WR(0, 0x0021, 0x0000); // GRAM Vertical Address
    //	LCD_CPU_WR(0x002B, 0x000F); // Set Frame Rate
    // ----------- Adjust the Gamma Curve ----------//
    LCD_CPU_WR(0, 0x0030, 0x0000);
    LCD_CPU_WR(0, 0x0031, 0x0506);
    LCD_CPU_WR(0, 0x0032, 0x0104);
    LCD_CPU_WR(0, 0x0035, 0x0707);
    LCD_CPU_WR(0, 0x0036, 0x000F);
    LCD_CPU_WR(0, 0x0037, 0x0306);
    LCD_CPU_WR(0, 0x0038, 0x0102);
    LCD_CPU_WR(0, 0x0039, 0x0707);
    LCD_CPU_WR(0, 0x003C, 0x0702);
    LCD_CPU_WR(0, 0x003D, 0x1604);
    //------------------ Set GRAM area ---------------//
    LCD_CPU_WR(0, 0x0050, 0x0000); // Horizontal GRAM Start Address
    LCD_CPU_WR(0, 0x0051, 0x00EF); // Horizontal GRAM End Address
    LCD_CPU_WR(0, 0x0052, 0x0000); // Vertical GRAM Start Address
    LCD_CPU_WR(0, 0x0053, 0x013F); // Vertical GRAM Start Address
    LCD_CPU_WR(0, 0x0060, 0xA700); // Gate Scan Line
    LCD_CPU_WR(0, 0x0061, 0x0001); // NDL,VLE, REV
    LCD_CPU_WR(0, 0x006A, 0x0000); // set scrolling line
    //-------------- Partial Display Control ---------//
    LCD_CPU_WR(0, 0x0080, 0x0000);
    LCD_CPU_WR(0, 0x0081, 0x0000);
    LCD_CPU_WR(0, 0x0082, 0x0000);
    LCD_CPU_WR(0, 0x0083, 0x0000);
    LCD_CPU_WR(0, 0x0084, 0x0000);
    LCD_CPU_WR(0, 0x0085, 0x0000);
    //-------------- Panel Control -------------------//
    LCD_CPU_WR(0, 0x0090, 0x0010);
    LCD_CPU_WR(0, 0x0092, 0x0600);
    //	LCD_CPU_WR(0,0x0093, 0x0003);
    ///	LCD_CPU_WR(0,0x0095, 0x0110);
    //	LCD_CPU_WR(0,0x0097, 0x0000);
    //	LCD_CPU_WR(0,0x0098, 0x0000);
    LCD_CPU_WR(0, 0x0007, 0x0133); // 262K color and display ON

    //LCD_CPU_WR_INDEX(0,0x0022);




}

static void  LCD_open_cmd(__u32 sel)
{
    __u32 id;

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
    LCD_CPU_WR_INDEX(0, 0x0022);
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
        LCD_CPU_WR(0, 0x0020, 0x0000);   //INT GRAM address
        LCD_CPU_WR(0, 0x0021, 0x0000);   //INT GRAM address
        LCD_CPU_WR_INDEX(0, 0x0022);
        //__msg("LCD: Lcd_cpuisr_proc\n");
    }
}

void LCD_get_panel_funs_0(__lcd_panel_fun_t *fun)
{
    fun->cfg_panel_info = LCD_cfg_panel_info;
    fun->cfg_open_flow = LCD_open_flow;
    fun->cfg_close_flow = LCD_close_flow;
}
