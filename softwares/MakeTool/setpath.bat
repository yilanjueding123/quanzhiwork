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
	@echo Set Path Error!
	pause
	goto EXIT
:START
	@set path=D:\Winners\ePDK\softwares\MakeTool;C:\Program Files\ARM\RVCT\Programs\2.2\349\win_32-pentium
:EXIT


