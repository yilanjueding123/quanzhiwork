@ECHO OFF
goto START
******************************************************************************
*
* File      : PL.bat
* Parameters: %1 Name of file to compile (without extension; .c is added)
* Purpose   : Compile one file and add it to the list of files to put in
*             Library
*
* This file as is uses the AC320 Compiler
*
******************************************************************************
:ERROR
	pause
	goto EXIT
:START
	pushd .
	cd %1
	chdir
	if exist tmplib.a del tmplib.a
	call makefile /l %2
	popd

	if not exist tmplib.a goto create
	armar -r tmplib.a %1\tmplib.a
	if errorlevel 2 goto ERROR
	if errorlevel 1 goto ERROR

	goto exit
:create
	armar --create tmplib.a %1\tmplib.a
	if errorlevel 2 goto ERROR
	if errorlevel 1 goto ERROR

:EXIT
	if exist %1\tmplib.a del %1\tmplib.a

