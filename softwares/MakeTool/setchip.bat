@echo off
goto START
/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                            Software Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : maketool
* File    : makefile.bat
*
* By      : Steven
* Version : v1.0
* Date    : 2008-9-8 14:23:22
**********************************************************************************************************************
*/
:ERROR
    @echo chip select is invalid!!!
    pause

:EXIT
    exit

:START

	if "%1"=="/b" goto conti

:select_chip
    @echo *******************************************
    @echo * please select chip                      *
    @echo *******************************************
    @echo 0: exit
    @echo 1: EPDK_CHIP_SUNI
    @echo 2: EPDK_CHIP_SUNII
    @echo 2: EPDK_CHIP_SUNIII
    @echo *********************************************
    set /p SEL=Please Select:
    if %SEL%==0     goto EXIT
    if %SEL%==1     goto set_suni
    if %SEL%==2     goto set_sunii
    if %SEL%==3     goto set_suniii
    @echo chip select is invalid!!!
    goto select_chip

:set_suni
    set EPDK_CHIP=EPDK_CHIP_SUNI
    set libspath=D:\Winners\ePDK\libs\suni
    set interlibpath=D:\Winners\ePDK\interlib\suni
    set workspacepath=D:\winners\ePDK\workspace\suni
    goto conti

:set_sunii
    set EPDK_CHIP=EPDK_CHIP_SUNII
    set libspath=D:\Winners\ePDK\libs\sunii
    set interlibpath=D:\Winners\ePDK\interlib\sunii
    set workspacepath=D:\winners\ePDK\workspace\sunii
    goto conti

:set_suniii
    set EPDK_CHIP=EPDK_CHIP_SUNIII
    set libspath=D:\Winners\ePDK\libs\suniii
    set interlibpath=D:\Winners\ePDK\interlib\suniii
    set workspacepath=D:\winners\ePDK\workspace\suniii
    goto conti

:conti

