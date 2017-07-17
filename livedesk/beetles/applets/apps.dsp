# Microsoft Developer Studio Project File - Name="apps" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=apps - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "apps.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "apps.mak" CFG="apps - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "apps - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "apps - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "apps - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "apps - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "apps - Win32 Release"
# Name "apps - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "movie"

# PROP Default_Filter ""
# Begin Group "bright"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\movie\bright\bright.c
# End Source File
# Begin Source File

SOURCE=.\movie\bright\bright.h
# End Source File
# Begin Source File

SOURCE=.\movie\bright\bright_ui.c
# End Source File
# Begin Source File

SOURCE=.\movie\bright\bright_ui.h
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\movie\common\common.c
# End Source File
# Begin Source File

SOURCE=.\movie\common\common.h
# End Source File
# End Group
# Begin Group "fifo_db"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\movie\fifo_db\fifo_db.c
# End Source File
# Begin Source File

SOURCE=.\movie\fifo_db\fifo_db.h
# End Source File
# Begin Source File

SOURCE=.\movie\fifo_db\gp_def.h
# End Source File
# End Group
# Begin Group "play_sta"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\movie\play_sta\play_sta.c
# End Source File
# Begin Source File

SOURCE=.\movie\play_sta\play_sta.h
# End Source File
# Begin Source File

SOURCE=.\movie\play_sta\play_sta_ui.c
# End Source File
# Begin Source File

SOURCE=.\movie\play_sta\play_sta_ui.h
# End Source File
# End Group
# Begin Group "spsc"

# PROP Default_Filter ""
# Begin Group "spsc_ctrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\movie\spsc\spsc_ctrl\spsc_ctrl.c
# End Source File
# Begin Source File

SOURCE=.\movie\spsc\spsc_ctrl\spsc_ctrl.h
# End Source File
# Begin Source File

SOURCE=.\movie\spsc\spsc_ctrl\spsc_ctrl_ui.c
# End Source File
# Begin Source File

SOURCE=.\movie\spsc\spsc_ctrl\spsc_ctrl_ui.h
# End Source File
# End Group
# Begin Group "spsc_prog"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\movie\spsc\spsc_prog\spsc_prog.c
# End Source File
# Begin Source File

SOURCE=.\movie\spsc\spsc_prog\spsc_prog.h
# End Source File
# Begin Source File

SOURCE=.\movie\spsc\spsc_prog\spsc_prog_ui.c
# End Source File
# Begin Source File

SOURCE=.\movie\spsc\spsc_prog\spsc_prog_ui.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\movie\spsc\spsc_man.c
# End Source File
# Begin Source File

SOURCE=.\movie\spsc\spsc_man.h
# End Source File
# Begin Source File

SOURCE=.\movie\spsc\spsc_ui.c
# End Source File
# Begin Source File

SOURCE=.\movie\spsc\spsc_ui.h
# End Source File
# End Group
# Begin Group "sub"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\movie\sub\sub.c
# End Source File
# Begin Source File

SOURCE=.\movie\sub\sub.h
# End Source File
# Begin Source File

SOURCE=.\movie\sub\sub_ui.c
# End Source File
# Begin Source File

SOURCE=.\movie\sub\sub_ui.h
# End Source File
# End Group
# Begin Group "volume"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\movie\volume\volume.c
# End Source File
# Begin Source File

SOURCE=.\movie\volume\volume.h
# End Source File
# Begin Source File

SOURCE=.\movie\volume\volume_ui.c
# End Source File
# Begin Source File

SOURCE=.\movie\volume\volume_ui.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\movie\app_movie.c
# End Source File
# Begin Source File

SOURCE=.\movie\app_movie.h
# End Source File
# Begin Source File

SOURCE=.\movie\app_movie_i.h
# End Source File
# End Group
# Begin Group "app_root"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\app_root\app_root.c
# End Source File
# Begin Source File

SOURCE=.\app_root\app_root.h
# End Source File
# Begin Source File

SOURCE=.\app_root\app_root_init.c
# End Source File
# Begin Source File

SOURCE=.\app_root\app_root_init.h
# End Source File
# Begin Source File

SOURCE=.\app_root\app_root_scene.c
# End Source File
# Begin Source File

SOURCE=.\app_root\app_root_scene.h
# End Source File
# Begin Source File

SOURCE=.\app_root\app_test.c
# End Source File
# End Group
# Begin Group "explorer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\explorer\Explorer.c
# End Source File
# Begin Source File

SOURCE=.\explorer\Explorer.h
# End Source File
# Begin Source File

SOURCE=.\explorer\Explorer_List.c
# End Source File
# Begin Source File

SOURCE=.\explorer\Explorer_List.h
# End Source File
# Begin Source File

SOURCE=.\explorer\Explorer_UI.c
# End Source File
# Begin Source File

SOURCE=.\explorer\Explorer_UI.h
# End Source File
# End Group
# Begin Group "home"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\home\app_home.c
# End Source File
# Begin Source File

SOURCE=.\home\app_home.h
# End Source File
# Begin Source File

SOURCE=.\home\app_home_i.h
# End Source File
# Begin Source File

SOURCE=.\home\home_uipara.c
# End Source File
# Begin Source File

SOURCE=.\home\home_uipara.h
# End Source File
# Begin Source File

SOURCE=.\home\main_menu.c
# End Source File
# Begin Source File

SOURCE=.\home\main_menu.h
# End Source File
# Begin Source File

SOURCE=.\home\msgbox.c
# End Source File
# Begin Source File

SOURCE=.\home\msgbox.h
# End Source File
# Begin Source File

SOURCE=.\home\sub_menu.c
# End Source File
# Begin Source File

SOURCE=.\home\sub_menu.h
# End Source File
# End Group
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Group "common No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib\common\mem_dev.c
# End Source File
# Begin Source File

SOURCE=.\lib\common\mem_dev.h
# End Source File
# Begin Source File

SOURCE=.\lib\common\misc.c
# End Source File
# Begin Source File

SOURCE=.\lib\common\misc.h
# End Source File
# Begin Source File

SOURCE=.\lib\common\palette.c
# End Source File
# Begin Source File

SOURCE=.\lib\common\palette.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\lib\beetles_app.h
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
