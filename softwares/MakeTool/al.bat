@ECHO OFF
GOTO START

******************************************************************************
*
* File      : AL.bat
* Parameters: %1 Name of file to compile (without extension; .s is added)
* Purpose   : Compile one file and add it to the list of files to put in
*             Library
*
* This file as is uses the AC320 Compiler
*
******************************************************************************
:ERROR
	chdir
	pause
	goto EXIT
:START

@ECHO AL %1.s ...
	armasm  --debug --keep --via=%avia% -o%1.o %1.s
	if errorlevel 2 goto ERROR
	if errorlevel 1 goto ERROR
	if not exist tmplib.a goto create
	armar -r tmplib.a %1.o
	if errorlevel 2 goto ERROR
	if errorlevel 1 goto ERROR

	goto exit
:create
	armar --create tmplib.a %1.o
	if errorlevel 2 goto ERROR
	if errorlevel 1 goto ERROR
:exit
	if exist %1.o del %1.o