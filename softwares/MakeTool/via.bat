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
    @set cvia=%1\config\suni\compiler.via
    @set avia=%1\config\suni\assembler.via
    @set lvia=%1\config\suni\linker.via
    @set fvia=%1\config\suni\fromelf.via
    set libspath=D:\Winners\ePDK\libs\suni
    set interlibpath=D:\Winners\ePDK\interlib\suni
    set workspacepath=D:\winners\ePDK\workspace\suni
    goto conti

:y_sunii
    @set cvia=%1\config\sunii\compiler.via
    @set avia=%1\config\sunii\assembler.via
    @set lvia=%1\config\sunii\linker.via
    @set fvia=%1\config\sunii\fromelf.via
    set libspath=D:\Winners\ePDK\libs\sunii
    set interlibpath=D:\Winners\ePDK\interlib\sunii
    set workspacepath=D:\winners\ePDK\workspace\sunii
    goto conti

:y_suniii
    @set cvia=%1\config\suniii\compiler.via
    @set avia=%1\config\suniii\assembler.via
    @set lvia=%1\config\suniii\linker.via
    @set fvia=%1\config\suniii\fromelf.via
    set libspath=D:\Winners\ePDK\libs\suniii
    set interlibpath=D:\Winners\ePDK\interlib\suniii
    set workspacepath=D:\winners\ePDK\workspace\suniii
    goto conti

:conti

