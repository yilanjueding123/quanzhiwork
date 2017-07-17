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
lcd_power                = port:PC08<1><1><1><1>

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
    info->lcd_hbp           		= 3;
    info->lcd_ht            		= 480;
    info->lcd_vbp           		= 3;
    info->lcd_vt            		= (2 * 810);

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
    LCD_CPU_WR(0, 0x0000, 0x0000);      // 同步
    LCD_CPU_WR(0, 0x0000, 0x0000);      // 同步
    LCD_CPU_WR(0, 0x0000, 0x0000);      // 同步
    LCD_CPU_WR(0, 0x0000, 0x0000);      // 同步L
    LCD_delay(60000);			        //10ms
    LCD_CPU_WR(0, 0x0400, 0x6200);      //GS=1H0,NL=6H31=>line number=400,SCN=6H00
    LCD_CPU_WR(0, 0x0008, 0x0404);      //set bp=8lines and fp=8lines,

    //gamma setting
    LCD_CPU_WR(0, 0x0300, 0x0C00);      //gamma
    LCD_CPU_WR(0, 0x0301, 0x5A0B);
    LCD_CPU_WR(0, 0x0302, 0x0906);
    LCD_CPU_WR(0, 0x0303, 0x1017);
    LCD_CPU_WR(0, 0x0304, 0x2300);
    LCD_CPU_WR(0, 0x0305, 0x1700);
    LCD_CPU_WR(0, 0x0306, 0x6309);
    LCD_CPU_WR(0, 0x0307, 0x0C09);
    LCD_CPU_WR(0, 0x0308, 0x100C);
    LCD_CPU_WR(0, 0x0309, 0x2232);

    LCD_CPU_WR(0, 0x0010, 0x0010);      // DIVI=2H0=>internal clock =1OSC,	RTNI=5H16=>clock cycles per line=22clocks
    LCD_CPU_WR(0, 0x0011, 0x0101);      // NOWI=3H1=>no-overlap period=1 internal clock,	STDI=3H1=>source output delay time=1 internal clock
    LCD_CPU_WR(0, 0x0012, 0x0000);      // VEQWI=3H0=>VCOM equalize period=0 internal clock, VEQWI=3H0=>Source equalize period=0 internal clock
    LCD_CPU_WR(0, 0x0013, 0x0001); 		// MCPI=3H1=>VCOM alternating timing=1 internal clock

    //  POWER CONTROL
    LCD_CPU_WR(0, 0x0100, 0x0330);      //AP=2H3=>electricity amp=1,	BT=3H3=>SETUP FACTOR //0730
    LCD_CPU_WR(0, 0x0101, 0x0247); 		//DC1=3H2=>setup2 frequency=line frequency/4					//0117
    LCD_CPU_WR(0, 0x0103, 0x0f00); 		//VDV=5HA=>VCOM amplitude=VREG1OUT*9							//0700
    LCD_CPU_WR(0, 0x0280, 0x6800);      //VCM=7H40=>VCOMH voltage=VREG1OUT*0.748				//	4d00
    LCD_CPU_WR(0, 0x0102, 0xc1B0);      //VRH,VCMR,PSON,PON//c1b0
    LCD_delay(60000);			        //10ms

    //  以下是DISPLAY 模式设置
    LCD_CPU_WR(0, 0x0001, 0x0100);       //set scan mode,SM=0,SS=1
    LCD_CPU_WR(0, 0x0002, 0x0100);       //set line inversion or frame inversion:=line inversion,BC=1//0100
    LCD_CPU_WR(0, 0x0003, 0x50A8);       //set entry mode:BGR=1,ORG=0,ID=2H2,AM=1
    LCD_CPU_WR(0, 0x0009, 0x0001);       //PTS=0,PTV=0
    LCD_CPU_WR(0, 0x000b, 0x0000);
    LCD_CPU_WR(0, 0x000C, 0x0000);
    LCD_CPU_WR(0, 0x0090, 0x8000);       //external if
    LCD_CPU_WR(0, 0x000F, 0x0000);

    //set windows
    LCD_CPU_WR(0, 0x0210, 0x0000);       //
    LCD_CPU_WR(0, 0x0211, 0x00EF);
    LCD_CPU_WR(0, 0x0212, 0x0000);
    LCD_CPU_WR(0, 0x0213, 0x018F);      //432=1AF, 400=18F
    LCD_CPU_WR(0, 0x0500, 0x0000);      //Partial Display control
    LCD_CPU_WR(0, 0x0501, 0x0000);      //Partial Display control
    LCD_CPU_WR(0, 0x0502, 0x005F);
    LCD_CPU_WR(0, 0x0401, 0x0001);      //NDL=0,VLE=0,REV=1
    LCD_CPU_WR(0, 0x0404, 0x0000);      //VL=9H0
    LCD_delay(60000);			        //10ms

    //DISPLAY ON
    LCD_CPU_WR(0, 0x0007, 0x0100);      //DISPLAY ON!!!!!PTDE=0,BASEE=1
    LCD_delay(60000);			        //10ms
    LCD_CPU_WR(0, 0x0200, 0x0000);		//INT GRAM address
    LCD_CPU_WR(0, 0x0201, 0x0000);      //INT GRAM address
    //LCD_CPU_WR(0,0x0201,0x0000);		//INT GRAM address
    //LCD_CPU_WR(0,0x0211, 0x00ef);     //
    //LCD_CPU_WR(0,0x0212, 0x0000);     //
    //LCD_CPU_WR(0,0x0213, 0x018f);     //
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
