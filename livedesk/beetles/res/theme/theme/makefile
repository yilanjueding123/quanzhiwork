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


buildtheme:
	# make scripts for every application
	$(ESTUDIOROOT)/Softwares/Face200/MakeScript/MakeScript.exe ./cvr       1       1000

    
	# create theme for the whole case
	-rm cvr_theme.script
	cat ./cvr_themehdr.script      \
		./cvr/cvr.script            \
        >> cvr_theme.script

	# build the theme
	$(ESTUDIOROOT)/Softwares/Face200/FaceBuilderCmd/FaceBuilderCmd.exe ./config.ini
	# copy the theme to workspace
	cp ./theme.h        $(ROOT)/../cvr_theme.h
	cp ./theme.bin      $(WORKSPACEPATH)/cvr_v2.0/res/icon/theme.bin

# 删除生成的中间文件
clean:
	-rm ./theme.bin
	-rm ./cvr_theme.face
	-rm ./theme.h
	-rm ./cvr_theme.script
	-rm ./cvr/cvr.script
