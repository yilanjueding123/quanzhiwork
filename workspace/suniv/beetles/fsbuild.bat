::/*
::*********************************************************************************************************
::*                                                    MELIS
::*                                    the Easy Portable/Player Develop Kits
::*                                               file system build
::*
::*                                    (c) Copyright 2011-2014, Sunny China
::*                                             All Rights Reserved
::*
::* File    : fsbuild.bat
::* By      : Sunny
::* Version : v1.0
::* Date    : 2011-4-19
::* Descript: melis100 sdk file system build.
::* Update  : date                auther      ver     notes
::*           2011-4-19 13:06:56  Sunny       1.0     Create this file.
::*********************************************************************************************************
::*/

    @ECHO OFF
    
    set MINFSTOOLPATH=.\..\..\..\tools\build_tools\minfs
    
    ::----------------------------------------------------------------
    :: build ramfs, use minfs file system.
    ::----------------------------------------------------------------
    %MINFSTOOLPATH%\minfs.exe make ramfs .\\rootfs\\ramdisk.iso ramfs.ini
    
    ::----------------------------------------------------------------
    :: build rootfs, use minfs file system.
    ::----------------------------------------------------------------
    %MINFSTOOLPATH%\minfs.exe make rootfs rootfs.iso rootfs.ini
    
    ::----------------------------------------------------------------
    :: build udisk
    ::----------------------------------------------------------------
    ..\..\..\..\eStudio\Softwares\fsbuild200\fsbuild.exe .\udisk.ini
