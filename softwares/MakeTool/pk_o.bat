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
:EXIT



