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
#if 1
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
#else

    eLIBs_memset(info, 0, sizeof(__panel_para_t));

    /*屏的基本信息*/
    info->lcd_x                   		= 400;//400;
    info->lcd_y                   		= 240;// 240;
    info->lcd_dclk_freq        			= 13;  //MHz
    info->lcd_pwm_freq       			= 1; //////1; //KHz
    info->lcd_srgb               			= 0x00202020;
    info->lcd_swap              			= 0;

    /*屏的接口配置信息*/
    info->lcd_if                  		= 1;			//0=hv;1=cpu;2=ttl

    /*屏的HV模块相关信息*/
    info->lcd_hv_if               		= 0;
    info->lcd_hv_hspw             		= 0;
    info->lcd_hv_lde_iovalue      		= 0;
    info->lcd_hv_lde_used         		= 0;
    info->lcd_hv_smode            		= 0;
    info->lcd_hv_syuv_if          		= 0;
    info->lcd_hv_vspw             		= 0;

    /*屏的HV配置信息*/
    info->lcd_hbp           		= 2;
    info->lcd_ht            		= 240;
    info->lcd_vbp           		= 2;
    info->lcd_vt            		= (2 * 810);

    /*屏的IO配置信息*/
    info->lcd_io_cfg0             		= 0x00000000;
    info->lcd_io_cfg1             		= 0x00000000;
    info->lcd_io_strength         		= 0;

    /*CPU屏幕的配置信息*/
    info->lcd_cpu_if				= 0x4;	//0=18bits mode
    info->lcd_cpu_da                              = 0x1;

    /*TTL屏幕的配置信息*/
    info->lcd_ttl_ckvd            		= 0;
    info->lcd_ttl_ckvh            		= 0;
    info->lcd_ttl_ckvt            		= 0;
    info->lcd_ttl_datainv_en      		= 0;
    info->lcd_ttl_datainv_sel    			= 0;
    info->lcd_ttl_datarate        		= 0;
    info->lcd_ttl_oehd            		= 0;
    info->lcd_ttl_oehh            		= 0;
    info->lcd_ttl_oevd            		= 0;
    info->lcd_ttl_oevh            		= 0;
    info->lcd_ttl_oevt            		= 0;
    info->lcd_ttl_revd            		= 0;
    info->lcd_ttl_revsel          		= 0;
    info->lcd_ttl_sthd            		= 0;
    info->lcd_ttl_sthh            		= 0;
    info->lcd_ttl_stvdl           		= 0;
    info->lcd_ttl_stvdp           		= 0;
    info->lcd_ttl_stvh            		= 0;
    //cpu屏幕的配置信息
    info->lcd_frm 			 = 2;	//0: disable; 1: enable rgb666 dither; 2:enable rgb656 dither
    info->lcd_cpu_if         = 4;
#endif

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
    LCD_OPEN_FUNC(sel, LCD_open_cmd	, 200); 	//通过IO配置LCD模组为工作模式，并延时100ms
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
#if 1

    //************* Start Initial Sequence **********//
    LCD_CPU_WR_INDEX(0, 0X00E9);
    LCD_CPU_WR_DATA(0, 0X0020);

    LCD_CPU_WR_INDEX(0, 0X0011); //Exit Sleep
    LCD_delay(120000);

    LCD_CPU_WR_INDEX(0, 0X00D1); //VCOM Control
    LCD_CPU_WR_DATA(0, 0X0000); //00
    LCD_CPU_WR_DATA(0, 0X004e); //6b
    LCD_CPU_WR_DATA(0, 0X0019); //19

    LCD_CPU_WR_INDEX(0, 0X00D0); //Power_Setting
    LCD_CPU_WR_DATA(0, 0X0007);	//7
    LCD_CPU_WR_DATA(0, 0X0001); //4
    LCD_CPU_WR_DATA(0, 0X0002); //4

    LCD_CPU_WR_INDEX(0, 0X003A); //
    LCD_CPU_WR_DATA(0, 0X0055); //

    LCD_CPU_WR_INDEX(0, 0X0036); //set_address_mode	横竖屏转换模式设置
    LCD_CPU_WR_DATA(0, 0X006A); //0X0028

    LCD_CPU_WR_INDEX(0, 0X00C1); //Display_Timing_Setting for Normal/Partial Mode
    LCD_CPU_WR_DATA(0, 0X0010);
    LCD_CPU_WR_DATA(0, 0X0010);
    LCD_CPU_WR_DATA(0, 0X0002);
    LCD_CPU_WR_DATA(0, 0X0002);

    LCD_CPU_WR_INDEX(0, 0X00C0); //Panel Driving Setting
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0031);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0002);

    LCD_CPU_WR_INDEX(0, 0X00C5); //Set frame rate
    LCD_CPU_WR_DATA(0, 0X0004); //4

    LCD_CPU_WR_INDEX(0, 0X00D2); //power setting
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0044);

    LCD_CPU_WR_INDEX(0, 0X00C8); //Set Gamma
    LCD_CPU_WR_DATA(0, 0X0004);
    LCD_CPU_WR_DATA(0, 0X0067);
    LCD_CPU_WR_DATA(0, 0X0035);
    LCD_CPU_WR_DATA(0, 0X0004);
    LCD_CPU_WR_DATA(0, 0X0008);
    LCD_CPU_WR_DATA(0, 0X0006);
    LCD_CPU_WR_DATA(0, 0X0024);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0037);
    LCD_CPU_WR_DATA(0, 0X0040);
    LCD_CPU_WR_DATA(0, 0X0003);
    LCD_CPU_WR_DATA(0, 0X0010);
    LCD_CPU_WR_DATA(0, 0X0008);
    LCD_CPU_WR_DATA(0, 0X0080);
    LCD_CPU_WR_DATA(0, 0X0000);

    /*****************2A / 2B寄存器的设置在横竖屏转换时需要呼唤，它们设置数据从显示屏的RAM中搬取的起始地址*/
    LCD_CPU_WR_INDEX(0, 0X002A); //Enable 3 Gamma
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X008F);

    LCD_CPU_WR_INDEX(0, 0X002B); //Enable 3 Gamma
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X00EF);

    LCD_CPU_WR_INDEX(0, 0X0029); //display on
#else
    //************* Start Initial Sequence **********//
    LCD_CPU_WR_INDEX(0, 0X00E9);
    LCD_CPU_WR_DATA(0, 0X0020);

    LCD_CPU_WR_INDEX(0, 0X0011); //Exit Sleep
    LCD_delay(60000);

    LCD_CPU_WR_INDEX(0, 0X00D1); //VCOM Control
    LCD_CPU_WR_DATA(0, 0X0000); //00
    LCD_CPU_WR_DATA(0, 0X003a); //6b
    LCD_CPU_WR_DATA(0, 0X0013); //19

    LCD_CPU_WR_INDEX(0, 0X00D0); //Power_Setting
    LCD_CPU_WR_DATA(0, 0X0007);	//7
    LCD_CPU_WR_DATA(0, 0X0001); //1
    LCD_CPU_WR_DATA(0, 0X0002); //4

    LCD_CPU_WR_INDEX(0, 0X0036); //set_address_mode
    LCD_CPU_WR_DATA(0, 0X0088); //0X0028

    LCD_CPU_WR_INDEX(0, 0X00C1); //Display_Timing_Setting for Normal/Partial Mode
    LCD_CPU_WR_DATA(0, 0X0010);
    LCD_CPU_WR_DATA(0, 0X0010);
    LCD_CPU_WR_DATA(0, 0X0002);
    LCD_CPU_WR_DATA(0, 0X0002);

    LCD_CPU_WR_INDEX(0, 0X00C0); //Panel Driving Setting
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0031);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0002);

    LCD_CPU_WR_INDEX(0, 0X00C5); //Set frame rate
    LCD_CPU_WR_DATA(0, 0X0004); //4

    LCD_CPU_WR_INDEX(0, 0X00D2); //power setting
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0044);

    LCD_CPU_WR_INDEX(0, 0X00C8); //Set Gamma
    LCD_CPU_WR_DATA(0, 0X0004);
    LCD_CPU_WR_DATA(0, 0X0067);
    LCD_CPU_WR_DATA(0, 0X0035);
    LCD_CPU_WR_DATA(0, 0X0004);
    LCD_CPU_WR_DATA(0, 0X0008);
    LCD_CPU_WR_DATA(0, 0X0006);
    LCD_CPU_WR_DATA(0, 0X0024);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0037);
    LCD_CPU_WR_DATA(0, 0X0040);
    LCD_CPU_WR_DATA(0, 0X0003);
    LCD_CPU_WR_DATA(0, 0X0010);
    LCD_CPU_WR_DATA(0, 0X0008);
    LCD_CPU_WR_DATA(0, 0X0080);
    LCD_CPU_WR_DATA(0, 0X0000);

    LCD_CPU_WR_INDEX(0, 0X00C9); //Set Gamma
    LCD_CPU_WR_DATA(0, 0X000D);
    LCD_CPU_WR_DATA(0, 0X0009);
    LCD_CPU_WR_DATA(0, 0X0009);
    LCD_CPU_WR_DATA(0, 0X0009);
    LCD_CPU_WR_DATA(0, 0X0009);
    LCD_CPU_WR_DATA(0, 0X0009);
    LCD_CPU_WR_DATA(0, 0X0009);
    LCD_CPU_WR_DATA(0, 0X000B);
    LCD_CPU_WR_DATA(0, 0X000B);
    LCD_CPU_WR_DATA(0, 0X000D);
    LCD_CPU_WR_DATA(0, 0X000D);
    LCD_CPU_WR_DATA(0, 0X000E);
    LCD_CPU_WR_DATA(0, 0X000E);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0004);
    LCD_CPU_WR_DATA(0, 0X000D);
    LCD_CPU_WR_DATA(0, 0X0009);
    LCD_CPU_WR_DATA(0, 0X0009);
    LCD_CPU_WR_DATA(0, 0X0009);
    LCD_CPU_WR_DATA(0, 0X0009);
    LCD_CPU_WR_DATA(0, 0X0009);
    LCD_CPU_WR_DATA(0, 0X0009);
    LCD_CPU_WR_DATA(0, 0X000B);
    LCD_CPU_WR_DATA(0, 0X000B);
    LCD_CPU_WR_DATA(0, 0X000D);
    LCD_CPU_WR_DATA(0, 0X000D);
    LCD_CPU_WR_DATA(0, 0X000E);
    LCD_CPU_WR_DATA(0, 0X000E);
    LCD_CPU_WR_DATA(0, 0X000E);
    LCD_CPU_WR_DATA(0, 0X000E);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X0004);
    LCD_CPU_WR_DATA(0, 0X0004);

    LCD_CPU_WR_INDEX(0, 0X0030); //Set_partial_area
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X00af);

    LCD_CPU_WR_INDEX(0, 0X0033); //Set_scroll_area
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X00ef);

    LCD_CPU_WR_INDEX(0, 0X003a); //Get_address_mode
    LCD_CPU_WR_DATA(0, 0X0055);

    LCD_CPU_WR_INDEX(0, 0X00EA); //Enable 3 Gamma
    LCD_CPU_WR_DATA(0, 0X0080);
    LCD_CPU_WR_INDEX(0, 0X0029); //display on
#endif

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
#if 0
    LCD_CPU_WR_INDEX(0, 0x0022);
#else
    LCD_CPU_WR_INDEX(0, 0x003c);
#endif
    __inf("LCD_BOOT: LCD_CPU_WR_INDEX!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    LCD_CPU_AUTO_FLUSH(0, 1);
    __inf("LCD_BOOT: LCD_CPU_AUTO_FLUSH !!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

static void  LCD_close_cmd(__u32 sel)
{

}

void  Lcd_cpuisr_proc(void)
{
#if 0
    //if(lcd_onoff_status == 1)
    {
        LCD_CPU_WR(0, 0x0020, 0x0000);   //INT GRAM address
        LCD_CPU_WR(0, 0x0021, 0x0000);   //INT GRAM address
        LCD_CPU_WR_INDEX(0, 0x0022);
        //__msg("LCD: Lcd_cpuisr_proc\n");
    }
#else
    LCD_CPU_WR_INDEX(0, 0X002A); //Set_column_address
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0001);
    LCD_CPU_WR_DATA(0, 0X008f);

    LCD_CPU_WR_INDEX(0, 0X002b); //Set_page_address
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X0000);
    LCD_CPU_WR_DATA(0, 0X00ef);

    LCD_CPU_WR_INDEX(0, 0x003c);
#endif
}

void LCD_get_panel_funs_0(__lcd_panel_fun_t *fun)
{
    fun->cfg_panel_info = LCD_cfg_panel_info;
    fun->cfg_open_flow = LCD_open_flow;
    fun->cfg_close_flow = LCD_close_flow;
}
