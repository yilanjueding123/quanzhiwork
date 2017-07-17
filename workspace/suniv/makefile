#/*
#*********************************************************************************************************
#*                                                    MELIS
#*                                    the Easy Portable/Player Develop Kits
#*                                               makefile Module
#*
#*                                    (c) Copyright 2006-2010, kevin.z China
#*                                             All Rights Reserved
#*
#* File    : makefile
#* By      : kevin.z
#* Version : v1.0
#* Date    : 2010-9-9 20:29
#* Descript:
#* Update  : date                auther      ver     notes
#*********************************************************************************************************
#*/

ROOT    = .
SDKROOT = $(ROOT)/../..

#编译所有模块
all:
	@echo =========================================================================
	@echo Start build melis platform...
	@echo =========================================================================
	##=========================================================================
	## build libs
	##=========================================================================
	make -C $(SDKROOT)/elibs/libc
	make -C $(SDKROOT)/elibs/slib
	
	##=========================================================================
	## build kernel
	##=========================================================================
	make -C $(SDKROOT)/epos
	
	##=========================================================================
	## build driver modules
	##=========================================================================
	make -C $(SDKROOT)/emod/drv_audio
	make -C $(SDKROOT)/emod/drv_display
	make -C $(SDKROOT)/emod/drv_fm
	make -C $(SDKROOT)/emod/drv_iic
	make -C $(SDKROOT)/emod/drv_ir
	make -C $(SDKROOT)/emod/drv_keyboard
	make -C $(SDKROOT)/emod/drv_matrixkey
	make -C $(SDKROOT)/emod/drv_monitor
	#make -C $(SDKROOT)/emod/drv_mp
	#make -C $(SDKROOT)/emod/drv_orchid
	make -C $(SDKROOT)/emod/drv_power2
	make -C $(SDKROOT)/emod/drv_rtc
	make -C $(SDKROOT)/emod/drv_spi
	make -C $(SDKROOT)/emod/drv_storages
	#make -C $(SDKROOT)/emod/drv_streaming
	make -C $(SDKROOT)/emod/drv_touchpanel
	make -C $(SDKROOT)/emod/drv_uart
	make -C $(SDKROOT)/emod/drv_usbd
	make -C $(SDKROOT)/emod/drv_usbh
	
	##=========================================================================
	## build middleware modules
	##=========================================================================
	#make -C $(SDKROOT)/emod/mod_bookengine
	make -C $(SDKROOT)/emod/mod_slib
	make -C $(SDKROOT)/emod/mod_cedar
	make -C $(SDKROOT)/emod/mod_charset
	#make -C $(SDKROOT)/emod/mod_dbms
	#make -C $(SDKROOT)/emod/mod_duckweed
	#make -C $(SDKROOT)/emod/mod_ginkgo
	#make -C $(SDKROOT)/emod/mod_flash
	#make -C $(SDKROOT)/emod/mod_lotus
	make -C $(SDKROOT)/emod/mod_orange
	#make -C $(SDKROOT)/emod/mod_poplar
	make -C $(SDKROOT)/emod/mod_standby
	#make -C $(SDKROOT)/emod/mod_update
	make -C $(SDKROOT)/emod/mod_willow


# 删除生成的中间文件
clean:
	##=========================================================================
	## clean libs
	##=========================================================================
	make -C $(SDKROOT)/elibs                clean
	
	##=========================================================================
	## clean kernel
	##=========================================================================
	make -C $(SDKROOT)/epos                 clean
	
	##=========================================================================
	## clean driver module
	##=========================================================================
	make -C $(SDKROOT)/emod/drv_audio       clean
	make -C $(SDKROOT)/emod/drv_display     clean
	make -C $(SDKROOT)/emod/drv_fm          clean
	make -C $(SDKROOT)/emod/drv_iic         clean
	make -C $(SDKROOT)/emod/drv_ir          clean
	make -C $(SDKROOT)/emod/drv_keyboard    clean
	make -C $(SDKROOT)/emod/drv_monitor     clean
	#make -C $(SDKROOT)/emod/drv_orchid      clean
	make -C $(SDKROOT)/emod/drv_power2       clean
	make -C $(SDKROOT)/emod/drv_rtc         clean
	make -C $(SDKROOT)/emod/drv_storages    clean
	make -C $(SDKROOT)/emod/drv_spi         clean
	make -C $(SDKROOT)/emod/drv_touchpanel  clean
	make -C $(SDKROOT)/emod/drv_uart        clean
	make -C $(SDKROOT)/emod/drv_usbd        clean
	make -C $(SDKROOT)/emod/drv_usbh        clean
	
	##=========================================================================
	## clean middleware module
	##=========================================================================
	#make -C $(SDKROOT)/emod/mod_bookengine  clean
	make -C $(SDKROOT)/emod/mod_slib        clean
	make -C $(SDKROOT)/emod/mod_charset     clean
	make -C $(SDKROOT)/emod/mod_cedar       clean
	#make -C $(SDKROOT)/emod/mod_dbms        clean
	#make -C $(SDKROOT)/emod/mod_ginkgo      clean
	make -C $(SDKROOT)/emod/mod_orange      clean
	make -C $(SDKROOT)/emod/mod_standby     clean
	#make -C $(SDKROOT)/emod/mod_update      clean
	make -C $(SDKROOT)/emod/mod_willow      clean

