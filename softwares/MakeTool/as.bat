@ECHO OFF
GOTO START

******************************************************************************
*
* File      : AS.bat
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

ECHO AS %1.s ...
	armasm --debug --keep --via=%avia% -o%1.o %1.s
	if errorlevel 2 goto ERROR
	if errorlevel 1 goto ERROR
	if not exist tmplib.o goto create
	armlink -p --noremove --via=%lvia% -otmplib.o tmplib.o %1.o
	if errorlevel 2 goto ERROR
	if errorlevel 1 goto ERROR
	goto EXIT
:create
	armlink -p --noremove --via=%lvia% -otmplib.o %1.o
	if errorlevel 2 goto ERROR
	if errorlevel 1 goto ERROR
:EXIT
	if exist %1.o del %1.o