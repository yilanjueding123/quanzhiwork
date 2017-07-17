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


#导入编译器配置
include make.cfg


#从所有子目录中得到源代码的列表
SRCCS=magic.c mod_orange.c orange_mod_api.c
SRCSS=
include $(ROOT)/resreader/make.cfg
include $(ROOT)/gui/make.cfg


#得到源代码对应的目标文件的列表
OBJS=$(SRCCS:.c=.o)

#all:
#	make buildapi
#	make driver
#
#
##生成api相关文件
#buildapi:
#	$(ESTUDIOROOT)/Softwares/ApiTabMaker/ApiTabMaker.exe mod_orange_api_i.h orangemod 0xe1800000 orange_api_entry.h orange_api_entry.cc
#	@echo ------------------------------------
#	@echo build api table for orange mod finish
#	@echo ------------------------------------
#
#
##生成输出目标文件
#driver:$(LOCALTARGET)
#	$(LOAD) $(LDFLAGS) $(TARGET) $(LOCALTARGET)
#	@echo ----------------------------
#	@echo target make finish
#	@echo ----------------------------
#生成输出目标文件
all:$(LOCALTARGET)
	$(LOAD) $(LDFLAGS) $(TARGET) $(LOCALTARGET)
	@echo ----------------------------
	@echo target make finish
	@echo ----------------------------

#生成本地目标文件
$(LOCALTARGET):$(OBJS)
	$(LINK) $(LKFLAGS) -o $@ $(filter %.o ,$+) $(LIBS)


# 删除生成的中间文件
clean:
	-rm $(OBJS) $(LOCALTARGET)
