@echo off
goto start
/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : desktop
* File    : makefile.bat
*
* By      : Steven
* Version : v1.0
* Date    : 2008-9-8 14:23:22
**********************************************************************************************************************
*/
:error
	@echo Please input arg for chip select!
	pause
	goto exit

:start
	@echo ========================
	@echo Desktop build tool......
	@echo ========================

	if exist tmplib.o del tmplib.o
	
	if "%1"=="/b" goto build
	call D:\Winners\ePDK\softwares\MakeTool\setpath
	call setchip
:build
	call via D:\winners\ePDK\livedesk\compass\mod_desktop

	if exist tmplib.o del tmplib.o
	if exist __image.axf del __image.axf
	if exist %libspath%\desktop.sym del %libspath%\desktop.sym
	

	call cc magic	
	call cc desktop_api
	call cc mod_desktop	
	call pk util
	call pk framework
	call pk msg_srv	
	::call pk dsk_lion
	::call pk dsk_sys
	call pk functions
	call pk engine

	@echo LK Linking......
	armlink --noremove  --via=%lvia% tmplib.o %libspath%\elibs.a %libspath%\elibs_ex.a
	::--map --symbols --info=sizes --info=totals

	@if errorlevel 2 goto ERROR
	@if errorlevel 1 goto ERROR
	
	fromelf --via=%fvia% __image.axf

	@if errorlevel 2 goto ERROR
	@if errorlevel 1 goto ERROR

	if exist tmplib.o del tmplib.o
	if exist tmplib.a del tmplib.a

::if exist __image.axf del __image.axf

    @echo Finished!
	if "%1"=="" pause

