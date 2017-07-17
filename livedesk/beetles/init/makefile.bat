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
	@echo =======================
	@echo Init build Start!
	@echo =======================

	if exist tmplib.a del tmplib.a
	
	if "%1"=="/b" goto build
	call D:\Winners\ePDK\softwares\MakeTool\setpath
	call setchip
:build
	call via D:\winners\ePDK\livedesk\compass\init

	if exist tmplib.o del tmplib.o
	if exist __image.axf del __image.axf	

	call cc magic
	call cc mod_init
	call cc init	
	call cc init_api
	call pk background
	call pk headbar
	call pk display
	call pk mini_music	
	call pk ui		
	
	@echo LK Linking......
	armlink --noremove --via=%lvia% tmplib.o %libspath%\elibs.a
	@if errorlevel 2 goto ERROR
	@if errorlevel 1 goto ERROR
	
	fromelf --via=%fvia% __image.axf
	@if errorlevel 2 goto ERROR
	@if errorlevel 1 goto ERROR

	if exist tmplib.o del tmplib.o
	if exist tmplib.a del tmplib.a
::	if exist __image.axf del __image.axf

	@pause

	