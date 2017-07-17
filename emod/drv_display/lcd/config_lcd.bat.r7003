@echo off
	goto START

:START
    @echo *********************************************
    @echo *   select board                            *
    @echo *********************************************
    @echo  0: ChiHsin_1w500
    @echo  1: TPO_td043m_p2
    @echo  2: InnoLux_at043
    @echo  3: ILI9327_cpupanel
    @echo  4: ILI9327_cpupanel_car
    @echo  5: TPO_at070
    @echo *********************************************
    
    set /p SEL=Please Select:
    if %SEL%==0     goto LCD0
    if %SEL%==1     goto LCD1
    if %SEL%==2     goto LCD2
    if %SEL%==3     goto LCD3
    if %SEL%==4     goto LCD4
    if %SEL%==5     goto LCD5
    goto ERROR

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:LCD0
    copy lcd_bak\ChiHsin_lw500.c lcd_panel_cfg.c
    goto conti
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:LCD1
    copy lcd_bak\TPO_td043m_p2.c lcd_panel_cfg.c
    goto conti
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:LCD2
    copy lcd_bak\InnoLux_at043.c lcd_panel_cfg.c
    goto conti
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:LCD3
    copy lcd_bak\ILI9327.c lcd_panel_cfg.c
    goto conti
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:LCD4
    copy lcd_bak\ILI9327_car.c lcd_panel_cfg.c
    goto conti

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:LCD5
    copy lcd_bak\TPO_at070.c lcd_panel_cfg.c
    goto conti

:ERROR
    @echo sel error!!
    goto conti
    
:conti
    @echo *********************************************
    pause

