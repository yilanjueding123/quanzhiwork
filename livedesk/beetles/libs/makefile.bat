@echo off
goto START
/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eLibs Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : app common
* File    : makefile.bat
*
* By      : Steven
* Version : v1.0
* Date    : 2008-9-8 14:23:22
**********************************************************************************************************************
*/
:ERROR
	@echo Please input arg for chip select!
	pause
	goto EXIT

:START
	if not "%1"=="/l" @echo ======================
	@echo eLIBs lib build Start!
	if not "%1"=="/l" @echo ======================

	if exist tmplib.a del tmplib.a
	if exist app_libs.a del app_libs.a

	if "%1"=="/l" goto link
	if "%1"=="/b" goto build

	call D:\Winners\ePDK\softwares\MakeTool\setpath
	call setchip
:build
	call via D:\winners\ePDK\livedesk\compass\libapps
:link

	call pl module_adapter

	if "%1"=="/l" goto EXIT
	@echo to generate app_libs.a
	move tmplib.a %libspath%\app_libs.a
	goto EXIT


:EXIT
	@echo Finished!
if "%1"=="" pause
