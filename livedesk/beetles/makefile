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
all:
	make -C elibs
	make -C res
	make -C shell	
	make -C mod_desktop
	make -C libapps
	make -C init
	make -C applets
	
	
    
# 删除生成的中间文件
clean:
	make -C elibs       clean
	make -C shell       clean
	make -C res         clean
	make -C mod_desktop clean
	make -C libapps     clean
	make -C init        clean
	make -C applets     clean