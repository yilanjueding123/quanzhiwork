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
#* Date    : 2010-9-3 9:39
#* Descript:
#* Update  : date                auther      ver     notes
#*********************************************************************************************************
#*/


#生成输出目标文件
buildboot:
	make -C usbh_drv
	make -C host0_drv

	-rm usb_host_drv.a

# 删除生成的中间文件
clean:
	make -C usbh_drv    clean
	make -C host0_drv   clean



