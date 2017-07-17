::/*
::*********************************************************************************************************
::*                                                    MELIS
::*                                    the Easy Portable/Player Develop Kits
::*                                                Board config
::*
::*                                    (c) Copyright 2011-2014, Sunny China
::*                                             All Rights Reserved
::*
::* File    : config.bat
::* By      : Sunny
::* Version : v1.0
::* Date    : 2011-4-19
::* Descript: melis100 sdk board config file.
::* Update  : date                auther      ver     notes
::*           2011-4-19 9:46:31   Sunny       1.0     Create this file.
::*********************************************************************************************************
::*/

@ECHO OFF

:START
    @echo *********************************************
    @echo *   select board                            *
    @echo *********************************************
    @echo  0: EPDK_BID_EVB_16_BIT(开发板3寸cpu屏)
    @echo  1: EPDK_BID_XWBOARD(芯舞小机)
    @echo *********************************************
    set /p SEL=Please Select:
    if %SEL%==0     goto EPDK_BID_EVB_16
    if %SEL%==1     goto EPDK_BID_XWBOARD
    @echo invalid board select, please input(0 ~ 1)!
    goto START

    ::----------------------------------------------------------------
    :: copy p3 board config file
    ::----------------------------------------------------------------
:EPDK_BID_EVB_16
    copy efex\sys_config_bak\sys_config_evb_16.ini  efex\sys_config.fex
    goto FINISH
    

::----------------------------------------------------------------
    :: copy board config file
    ::----------------------------------------------------------------
:EPDK_BID_XWBOARD
    copy efex\sys_config_bak\sys_config_xw.ini  efex\sys_config.fex
    goto FINISH

:FINISH
    @echo *********************************************
    pause
