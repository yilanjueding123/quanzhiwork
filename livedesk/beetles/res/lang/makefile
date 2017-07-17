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

ROOT    = .
SDKROOT = $(ROOT)/../../../..

#导入交叉编译器通用配置
include $(SDKROOT)/includes/cfgs/CROSSTOOL.CFG


buildlang:
	$(ESTUDIOROOT)/Softwares/LangBuild/LangOSDBuild.exe ./lang.xml
	cp ./lang.h     $(ROOT)/../../include/res/lang.h
	cp ./lang.bin   $(WORKSPACEPATH)/beetles/rootfs/apps/lang.bin


# 删除生成的中间文件
clean:
	-rm ./lang.h
	-rm ./lang.bin
