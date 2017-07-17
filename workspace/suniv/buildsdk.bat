@echo off
@echo *************************************************************************
@echo *                 EPDK
@echo * the Easy Portable/Player Developent Kits
@echo * platform build tool!
@echo *
@echo * by  : steven.zgj
@echo * date: 2008/09/04
@echo * ver : 1.0
@echo * start time:%TIME%
@echo ************************************************************************

    set EPDK_CHIP=EPDK_CHIP_SUNII
    call D:\Winners\ePDK\softwares\MakeTool\setpath


::=========================================================================
::elibs
::=========================================================================
    call pb ..\..\elibs
::=========================================================================
::ebios
::=========================================================================
    call pb ..\..\ebios
::=========================================================================
::os
::=========================================================================
start   call pb ..\..\epos
::=========================================================================
::emod
::=========================================================================
start   call pb ..\..\emod\drv_audio
start   call pb ..\..\emod\drv_audio2.0
start   call pb ..\..\emod\drv_hdmi
start   call pb ..\..\emod\drv_iic
start   call pb ..\..\emod\drv_keyboard
start   call pb ..\..\emod\drv_rtc
start   call pb ..\..\emod\drv_touchpanel
start   call pb ..\..\emod\drv_power
start   call pb ..\..\emod\drv_monitor
start   call pb ..\..\emod\drv_usbd
start   call pb ..\..\emod\drv_usbh
start   call pb ..\..\emod\drv_fm
start   call pb ..\..\emod\drv_spi
start   call pb ..\..\emod\drv_orchid
start   call pb ..\..\emod\drv_orchid2.0
start   call pb ..\..\emod\drv_display

start   call pb ..\..\emod\mod_cedar
start   call pb ..\..\emod\mod_ginkgo
start   call pb ..\..\emod\mod_willow
start   call pb ..\..\emod\mod_herb
start   call pb ..\..\emod\mod_update
start   call pb ..\..\emod\mod_standby
start   call pb ..\..\emod\mod_dbms
start   call pb ..\..\emod\mod_bookengine
start   call pb ..\..\emod\mod_lemon
start   call pb ..\..\emod\mod_orange

start   /w call pb ..\..\emod\drv_uart
start   /w call pb ..\..\eFex\fes\efex_libs
start   /w call pb ..\..\emod\drv_storages

::=========================================================================
::egon
::=========================================================================
start   call pb ..\..\eGon
start   call pb ..\..\eFex
@echo =========================================================================
@echo Finished!
@echo End time:%DATE%%TIME%
@echo =========================================================================
    if "%1"=="" pause
