@echo off
	goto START

:START
    @echo *********************************************
    @echo *   select board                            *
    @echo *********************************************
    @echo  0: InnoLux_at043
    @echo  1: R61509_cpupanel_16bit
    @echo  2: OTA5180A
    @echo *********************************************
    
    set /p SEL=Please Select:
    if %SEL%==0     goto LCD0
    if %SEL%==1     goto LCD1
    if %SEL%==2     goto LCD2
    goto ERROR

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:LCD0
    copy lcd_bak\InnoLux_at043.c lcd_panel_cfg.c
    goto conti
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:LCD1
    copy lcd_bak\R61509.c lcd_panel_cfg.c
    goto conti
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:LCD2
    copy lcd_bak\OTA5180A.c lcd_panel_cfg.c
    goto conti

:ERROR
    @echo sel error!!
    goto conti
    
:conti
    @echo *********************************************
    pause

