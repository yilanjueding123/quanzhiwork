	@ECHO OFF
	goto start
****************************************************************************************
*
****************************************************************************************
:error
	@echo Please input arg for chip select!
	pause
	goto exit

:start
	@echo ===================
	@echo SHELL build Start!
	@echo ===================

	if exist tmplib.a del tmplib.a
	
	if "%1"=="/b" goto build
	call D:\Winners\ePDK\softwares\MakeTool\setpath
	call setchip
:build
	call via D:\winners\ePDK\livedesk\beetles\shell

	if exist tmplib.o del tmplib.o
	if exist __image.axf del __image.axf
    
	call cc magic
	call cc Esh_shell
	call cc Esh_execute_cmd
	call cc Esh_init
	call cc Esh_exit
	call cc Esh_read_command
	call pk Esh_builtins
	call pk Esh_error
	call pk Esh_support

	@echo LK Linking......
	armlink --noremove --entry 0 --via=%lvia% tmplib.o %libspath%\elibs.a
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
	exit
