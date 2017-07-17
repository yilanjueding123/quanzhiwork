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
* File    : setvia.bat
*
* By      : Steven
* Version : v1.0
* Date    : 2008-9-8 14:23:22
**********************************************************************************************************************
*/
:ERROR
    pause
    goto EXIT

:START
    if "%EPDK_CHIP%"=="EPDK_CHIP_SUNI"      goto y_suni
    if "%EPDK_CHIP%"=="EPDK_CHIP_SUNII"     goto y_sunii
    if "%EPDK_CHIP%"=="EPDK_CHIP_SUNIII"    goto y_suniii
    goto ERROR

:y_suni
    @set cvia=%1\config_phy\suni\compiler.via
    @set avia=%1\config_phy\suni\assembler.via
    @set lvia=%1\config_phy\suni\linker.via
    @set fvia=%1\config_phy\suni\fromelf.via
    set libspath=D:\Winners\ePDK\libs\suni
    set interlibpath=D:\Winners\ePDK\interlib\suni
    goto conti

:y_sunii
    @set cvia=%1\config_phy\sunii\compiler.via
    @set avia=%1\config_phy\sunii\assembler.via
    @set lvia=%1\config_phy\sunii\linker.via
    @set fvia=%1\config_phy\sunii\fromelf.via
    set libspath=D:\Winners\ePDK\libs\sunii
    set interlibpath=D:\Winners\ePDK\interlib\sunii
    goto conti

:y_suniii
    @set cvia=%1\config_phy\suniii\compiler.via
    @set avia=%1\config_phy\suniii\assembler.via
    @set lvia=%1\config_phy\suniii\linker.via
    @set fvia=%1\config_phy\suniii\fromelf.via
    set libspath=D:\Winners\ePDK\libs\suniii
    set interlibpath=D:\Winners\ePDK\interlib\suniii
    goto conti

:conti
