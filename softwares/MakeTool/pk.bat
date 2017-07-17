@ECHO OFF
GOTO START

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
	if exist tmplib.o del tmplib.o
	call makefile /l 
	popd

	if not exist tmplib.o goto create
	armlink -p --noremove --via=%lvia% -otmplib.o tmplib.o %1\tmplib.o
	if errorlevel 2 goto ERROR
	if errorlevel 1 goto ERROR

	goto exit
:create
	armlink -p --noremove --via=%lvia% -otmplib.o %1\tmplib.o
	if errorlevel 2 goto ERROR
	if errorlevel 1 goto ERROR

:EXIT
	if exist %1\tmplib.o del %1\tmplib.o


