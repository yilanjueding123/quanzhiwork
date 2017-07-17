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
#* Date    : 2010-9-7 11:24
#* Descript:
#* Update  : date                auther      ver     notes
#*********************************************************************************************************
#*/

all:
	make -C libc
	make -C CUnit
	make -C slib
	
	@echo -------------------------------
	@echo libs build finish!
	@echo -------------------------------

# 删除生成的中间文件
clean:
	make -C libc   clean
	make -C CUnit  clean
	make -C slib   clean
	

