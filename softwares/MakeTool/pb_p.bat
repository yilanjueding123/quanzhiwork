@ECHO OFF
GOTO START

******************************************************************************
*
* File      : PB.bat
* Parameters: %1 Name of dir to be build
* Purpose   : Build project
*
******************************************************************************
:ERROR
	pause
	goto EXIT
:START
	pushd .
	cd %1
	call makefile /bp
	popd

:EXIT



