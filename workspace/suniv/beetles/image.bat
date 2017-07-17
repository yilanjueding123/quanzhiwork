::/*
::*********************************************************************************************************
::*                                                    MELIS
::*                                    the Easy Portable/Player Develop Kits
::*                                                 make image
::*
::*                                    (c) Copyright 2011-2014, Sunny China
::*                                             All Rights Reserved
::*
::* File    : image.bat
::* By      : Sunny
::* Version : v1.0
::* Date    : 2011-4-19
::* Descript: make a melis100 sdk image file.
::* Update  : date                auther      ver     notes
::*           2011-4-19 13:11:43  Sunny       1.0     Create this file.
::*********************************************************************************************************
::*/

    @ECHO OFF
    
    echo make image begin......
    if exist ePDKv100.img del ePDKv100.img

    set BUILDTOOLPATH=.\..\..\..\tools\build_tools
    set DRAGONTOOLPATH=..\..\..\..\eStudio\Softwares\eDragonEx220
    
    ::----------------------------------------------------------------
    ::      更新系统文件
    ::----------------------------------------------------------------
    echo ----------------------------------------------------------------
    echo                       update.bat
    echo ----------------------------------------------------------------
    call update.bat >update.txt
    
    ::----------------------------------------------------------------
    ::      解析配置脚本，系统
    ::----------------------------------------------------------------
    %BUILDTOOLPATH%\script.exe .\..\eFex\sys_config.fex
    %BUILDTOOLPATH%\script.exe .\..\eFex\sys_partition.fex

    ::----------------------------------------------------------------
    ::      解析配置脚本，应用
    ::----------------------------------------------------------------
    %BUILDTOOLPATH%\script.exe .\rootfs\app_config.fex    
    ::----------------------------------------------------------------
    ::      更新BOOT0和BOOT1文件头信息
    ::----------------------------------------------------------------
    ::%BUILDTOOLPATH%\update.exe  ..\eFex\sys_config.bin Boot0.bin Boot1.bin
    
    ::----------------------------------------------------------------
    ::      更新boot0、boot1文件头
    ::---------------------------------------------------------------- 
    echo ----------------------------------------------------------------
    echo                       update boot0\boot1
    echo ----------------------------------------------------------------
    %BUILDTOOLPATH%\update_boot0.exe Boot0.bin  ..\eFex\sys_config.bin  SPINOR_FLASH

    %BUILDTOOLPATH%\update_boot1.exe Boot1.bin  ..\eFex\sys_config.bin  

    ::----------------------------------------------------------------
    ::      生成MBR文件
    ::----------------------------------------------------------------
    %BUILDTOOLPATH%\BurnMBR.exe ..\eFex\sys_config.bin  mbr.bin

    ::----------------------------------------------------------------
    ::      生成烧写需要的MBR文件 ,这里必须是4，因为usbtool.fex已经写死
    ::----------------------------------------------------------------
    %BUILDTOOLPATH%\update_mbr.exe ..\eFex\sys_partition.bin  4


    ::----------------------------------------------------------------
    ::      fes1、uboot，用于USB烧录
    ::----------------------------------------------------------------
    @copy ..\eFex\usb\fes1_sun63i.bin          fes1.fex   
    @copy ..\eFex\usb\u-boot-sun63i.bin        u-boot.fex
    ::----------------------------------------------------------------
    ::      更新fes1、uboot文件头
    ::---------------------------------------------------------------- 
    echo ----------------------------------------------------------------
    echo                       update fes1 uboot
    echo ----------------------------------------------------------------
    %BUILDTOOLPATH%\update_fes1.exe  fes1.fex  ..\eFex\sys_config.bin
    %BUILDTOOLPATH%\update_uboot.exe u-boot.fex  ..\eFex\sys_config.bin

    
    ::----------------------------------------------------------------
    ::      打包rootfs\ramfs\udisk目录的镜像文件
    ::----------------------------------------------------------------
    call fsbuild.bat

    ::----------------------------------------------------------------
    ::      BOOT0和BOOT1文件大小填充对其
    ::----------------------------------------------------------------
    %BUILDTOOLPATH%\fix_file.exe boot0.bin  24
    %BUILDTOOLPATH%\fix_file.exe boot1.bin  120
    
    ::----------------------------------------------------------------
    ::      链接boot0+boo1+mbr+rootfs.iso+udisk.iso
    ::----------------------------------------------------------------
    type boot0.bin boot1.bin mbr.bin rootfs.iso udisk.iso > melis100.fex
    
    ::----------------------------------------------------------------
    ::      生成烧写文件校验和
    ::----------------------------------------------------------------
    %DRAGONTOOLPATH%\FileAddSum.exe melis100.fex verify.fex
    
    ::----------------------------------------------------------------
    ::      生成烧写镜像包文件ePDKv100.img
    ::----------------------------------------------------------------
    
    %DRAGONTOOLPATH%\compile -o image.bin image.cfg
    %DRAGONTOOLPATH%\dragon  image.cfg > image.txt
    
    ::----------------------------------------------------------------
    ::      清理临时文件
    ::----------------------------------------------------------------
    del boot0.bin
    del boot1.bin
    del mbr.bin
    del rootfs.iso
    del udisk.iso
    ::del melis100.fex
    del ..\eFex\sys_config.bin
    del sunxi_mbr.fex
    del u-boot.fex
    del ..\eFex\sys_partition.bin
    del fes1.fex
    del dlinfo.fex
    
    echo make image finished....
    
    PAUSE
