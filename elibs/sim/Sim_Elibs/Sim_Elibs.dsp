# Microsoft Developer Studio Project File - Name="Sim_Elibs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Sim_Elibs - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Sim_Elibs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Sim_Elibs.mak" CFG="Sim_Elibs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Sim_Elibs - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Sim_Elibs - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Sim_Elibs - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIM_ELIBS_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\libc\ansic\\" /I "..\..\libc\misc\pub0\board\\" /I "..\..\libc\misc\pub0\cacheop\\" /I "..\..\libc\misc\pub0\critical\\" /I "..\..\libc\misc\pub0\reg\\" /I "..\..\libc\misc\pub0\abilib\\" /I "..\..\libc\misc\pub0\\" /I "..\..\libc\misc\pub1\reg\\" /I "..\..\libc\misc\pub1\common\\" /I "..\..\libc\misc\pub1\\" /I "..\..\libc\misc\\" /I "..\..\libc\char_enc\\" /I "..\..\libc\charset\\" /I "..\..\libc\mediainfo\\" /I "..\..\libc\\" /I "..\..\libc\azlib\methods\lzma\\" /I "..\..\libc\azlib\methods\\" /I "..\..\libc\azlib\stream\blkdevstream\\" /I "..\..\libc\azlib\stream\filestream\\" /I "..\..\libc\azlib\stream\ramdevstream\\" /I "..\..\libc\azlib\stream\\" /I "..\..\libc\azlib\\" /I "..\..\anole\\" /I "..\..\robin\lib\\" /I "..\..\robin\foundation\\" /I "..\..\robin\palette\\" /I "..\..\robin\disp_output\\" /I "..\..\robin\ab\\" /I "..\..\robin\channel\\" /I "..\..\robin\eq\\" /I "..\..\robin\ff_rr_speed\\" /I "..\..\robin\fsm_ctrl\\" /I "..\..\robin\lyric\\" /I "..\..\robin\media_info\\" /I "..\..\robin\npl\\" /I "..\..\robin\play_mode\\" /I "..\..\robin\play_speed\\" /I "..\..\robin\spectrum\\" /I "..\..\robin\star\\" /I "..\..\robin\subtitle\\" /I "..\..\robin\track\\" /I "..\..\robin\video_layer\\" /I "..\..\robin\video_win\\" /I "..\..\robin\vision_effect\\" /I "..\..\robin\volume\\" /I "..\..\robin\zoom\\" /I "..\..\robin\\" /I "..\..\cat\\" /I "..\..\lark\rec\\" /I "..\..\lark\send\\" /I "..\..\lark\\" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIM_ELIBS_EXPORTS" /Yu"stdafx.h" /FD /c
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

!ELSEIF  "$(CFG)" == "Sim_Elibs - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIM_ELIBS_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\eLibs\anole\\" /I "..\..\..\eLibs\robin\lib\\" /I "..\..\..\eLibs\robin\foundation\\" /I "..\..\..\eLibs\robin\palette\\" /I "..\..\..\eLibs\robin\disp_output\\" /I "..\..\..\eLibs\robin\ab\\" /I "..\..\..\eLibs\robin\channel\\" /I "..\..\..\eLibs\robin\eq\\" /I "..\..\..\eLibs\robin\ff_rr_speed\\" /I "..\..\..\eLibs\robin\fsm_ctrl\\" /I "..\..\..\eLibs\robin\lyric\\" /I "..\..\..\eLibs\robin\media_info\\" /I "..\..\..\eLibs\robin\npl\\" /I "..\..\..\eLibs\robin\play_mode\\" /I "..\..\..\eLibs\robin\play_speed\\" /I "..\..\..\eLibs\robin\spectrum\\" /I "..\..\..\eLibs\robin\star\\" /I "..\..\..\eLibs\robin\subtitle\\" /I "..\..\..\eLibs\robin\track\\" /I "..\..\..\eLibs\robin\video_layer\\" /I "..\..\..\eLibs\robin\video_win\\" /I "..\..\..\eLibs\robin\vision_effect\\" /I "..\..\..\eLibs\robin\volume\\" /I "..\..\..\eLibs\robin\zoom\\" /I "..\..\..\eLibs\robin\\" /I "..\..\..\eLibs\cat\\" /I "..\..\..\eLibs\lark\rec\\" /I "..\..\..\eLibs\lark\send\\" /I "..\..\..\eLibs\lark\\" /I "..\..\..\eLibs\\" /I "..\..\..\includes\\" /I "..\..\..\INCLUDES\ELIBS\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIM_ELIBS_EXPORTS" /D "SIM_PC_WIN" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\workspace\sim\bin\sim_elibs.dll" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\workspace\sim\bin\sim_elibs.dll" /implib:"..\..\..\workspace\sim\bin\sim_elibs.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Sim_Elibs - Win32 Release"
# Name "Sim_Elibs - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Sim_Elibs.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /I "..\..\l" /Yc"stdafx.h"
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
# Begin Group "eLibs"

# PROP Default_Filter ""
# Begin Group "anole"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\anole\anole.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "robin"

# PROP Default_Filter ""
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\lib\robin_misc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "foundation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\foundation\cmdq.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\robin\foundation\robin_cedar_mutex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\robin\foundation\robin_cmd_mutex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\robin\foundation\robin_feedbackq.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\robin\foundation\robin_monitor.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\robin\foundation\robin_syn_op.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "palette"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\palette\robin_palette.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "disp_output"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\disp_output\robin_disp_output.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "ab"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\ab\robin_ab.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "channel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\channel\robin_channel.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "eq"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\eq\robin_eq.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "ff_rr_speed"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\ff_rr_speed\robin_ff_rr_speed.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "fsm_ctrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\fsm_ctrl\robin_fsm_ctrl.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "lyric"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\lyric\robin_lyric.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "media_info"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\media_info\robin_media_info.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "npl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\npl\robin_npl.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "play_mode"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\play_mode\robin_play_mode.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "play_speed"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\play_speed\robin_play_speed.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "spectrum"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\spectrum\robin_spectrum.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "star"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\star\robin_star.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "subtitle"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\subtitle\robin_subtitle.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "track"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\track\robin_track.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "video_layer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\video_layer\robin_video_layer.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "video_win"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\video_win\robin_video_win.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "vision_effect"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\vision_effect\robin_vision_effect.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "volume"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\volume\robin_volume.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "zoom"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\robin\zoom\robin_zoom.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\robin\robin.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\robin\robin_i.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "cat"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\cat\cat.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\cat\cat_cache.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\cat\cat_dir.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\cat\cat_partition.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\cat\cat_playlist.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\cat\cat_root.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\cat\cat_sort.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\cat\pic_miniature.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "lark"

# PROP Default_Filter ""
# Begin Group "rec"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\lark\rec\format_output.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\lark\rec\lark_list_file.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\lark\rec\lark_rec.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "send"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\lark\send\lark_send.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\lark\lark.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# End Group
# Begin Group "awslibs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\workspace\sim\bin\Sim_PC_WinApi.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\WORKSPACE\SIM\BIN\SIM_SYSTEMFUN.LIB
# End Source File
# Begin Source File

SOURCE=..\..\..\WORKSPACE\SIM\BIN\ORCHID.LIB
# End Source File
# Begin Source File

SOURCE=..\..\..\WORKSPACE\SIM\BIN\POWER.LIB
# End Source File
# Begin Source File

SOURCE=..\..\..\WORKSPACE\SIM\BIN\SIM_DE_DRV.LIB
# End Source File
# Begin Source File

SOURCE=..\..\..\WORKSPACE\SIM\BIN\SIM_EBIOS.LIB
# End Source File
# Begin Source File

SOURCE=..\..\..\WORKSPACE\SIM\BIN\SIM_EPOS.LIB
# End Source File
# Begin Source File

SOURCE=..\..\..\WORKSPACE\SIM\BIN\SIM_PC_KERNEL.LIB
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
