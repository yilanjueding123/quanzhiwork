# Microsoft Developer Studio Project File - Name="sim_shell" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=sim_shell - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sim_shell.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sim_shell.mak" CFG="sim_shell - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sim_shell - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sim_shell - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sim_shell - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIM_SHELL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\esh_builtins\commands\doscript\\" /I "..\..\esh_builtins\commands\\" /I "..\..\esh_builtins\\" /I "..\..\esh_error\\" /I "..\..\esh_support\\" /I "..\..\\" /I "..\..\..\..\..\includes\\" /I "..\..\..\..\..\INCLUDES\ELIBS\\" /I "..\..\..\INCLUDE\\" /I "..\..\..\..\..\INCLUDES\EMOD\\" /I "..\..\..\..\..\INCLUDES\EMOD\MOD_ORANGE\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIM_SHELL_EXPORTS" /D "SIM_PC_WIN" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "sim_shell - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIM_SHELL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\esh_builtins\commands\doscript\\" /I "..\..\esh_builtins\commands\\" /I "..\..\esh_builtins\\" /I "..\..\esh_error\\" /I "..\..\esh_support\\" /I "..\..\\" /I "..\..\..\..\..\includes\\" /I "..\..\..\..\..\INCLUDES\ELIBS\\" /I "..\..\..\INCLUDE\\" /I "..\..\..\..\..\INCLUDES\EMOD\\" /I "..\..\..\..\..\INCLUDES\EMOD\MOD_ORANGE\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIM_SHELL_EXPORTS" /D "SIM_PC_WIN" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\..\..\workspace\sim\shell.zgj" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\..\..\workspace\sim\bin\shell.zgj" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "sim_shell - Win32 Release"
# Name "sim_shell - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\sim_shell.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "shell"

# PROP Default_Filter ""
# Begin Group "esh_builtins"

# PROP Default_Filter ""
# Begin Group "commands"

# PROP Default_Filter ""
# Begin Group "doscript"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doscript\doscript.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doscript\esh_script_area.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doscript\esh_script_domisc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doscript\esh_script_eval.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doscript\esh_script_executer.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doscript\esh_script_lexer.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doscript\esh_script_onecommand.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doscript\esh_script_parser.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doscript\esh_script_var.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doassert.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\docat.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\docd.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doconfigfs.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\docopy.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\docpustat.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\docreate.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\dode.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\dodel.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\dodir.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doformat.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doformatcheck.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\dofs.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\dohelp.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\dohistory.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doinsmod.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\dokeysim.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\domeminfo.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\domkdir.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\domount.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\dopoweroff.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\Esh_builtins\commands\doquit.c

!IF  "$(CFG)" == "sim_shell - Win32 Release"

!ELSEIF  "$(CFG)" == "sim_shell - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doreset.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\dormdir.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\dorun.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\dosendkey.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\dosetpath.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doshowdsize.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doshowfsize.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\dostartx.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\dosysinfo.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\dowait.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_builtins\commands\doxcopy.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\esh_builtins\esh_builtins.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "esh_error"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\esh_error\esh_error.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "esh_support"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\esh_support\esh_libc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_support\esh_support.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\esh_execute_cmd.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_exit.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_init.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_read_command.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\esh_shell.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\magic.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "awslibs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\workspace\sim\bin\Sim_DE_Drv.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\workspace\sim\bin\Sim_EBios.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\workspace\sim\bin\sim_elibs.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\workspace\sim\bin\Sim_Epos.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\workspace\sim\bin\Sim_PC_Kernel.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\workspace\sim\bin\Sim_PC_WinApi.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\workspace\sim\bin\Sim_systemfun.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\workspace\sim\bin\SimAudio.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\workspace\sim\bin\display.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\workspace\sim\bin\orchid.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\workspace\sim\bin\mod\ceder.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\workspace\sim\bin\mod\desktop.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\workspace\sim\bin\mod\ginkgo.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\workspace\sim\bin\mod\orange.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\workspace\sim\bin\touchpanel.lib
# End Source File
# End Group
# Begin Source File

SOURCE=.\funExport.def
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
