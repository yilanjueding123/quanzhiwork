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
	$(ESTUDIOROOT)/Softwares/Face200/MakeScript/MakeScript.exe ./init       1       1000
	$(ESTUDIOROOT)/Softwares/Face200/MakeScript/MakeScript.exe ./home       1001    2000
	$(ESTUDIOROOT)/Softwares/Face200/MakeScript/MakeScript.exe ./movie      2001    3000
	$(ESTUDIOROOT)/Softwares/Face200/MakeScript/MakeScript.exe ./explorer   3001    4000
	$(ESTUDIOROOT)/Softwares/Face200/MakeScript/MakeScript.exe ./music      4001    5000
	$(ESTUDIOROOT)/Softwares/Face200/MakeScript/MakeScript.exe ./photo      5001    6000
	$(ESTUDIOROOT)/Softwares/Face200/MakeScript/MakeScript.exe ./setting    6001    7000
	$(ESTUDIOROOT)/Softwares/Face200/MakeScript/MakeScript.exe ./ebook      7001    8000
	$(ESTUDIOROOT)/Softwares/Face200/MakeScript/MakeScript.exe ./fm         8001    9000
	$(ESTUDIOROOT)/Softwares/Face200/MakeScript/MakeScript.exe ./calendar   9001    10000
	$(ESTUDIOROOT)/Softwares/Face200/MakeScript/MakeScript.exe ./dialog     10001   11000
	$(ESTUDIOROOT)/Softwares/Face200/MakeScript/MakeScript.exe ./record     11001   12000
	$(ESTUDIOROOT)/Softwares/Face200/MakeScript/MakeScript.exe ./dv     		12001   13000
    
	# create theme for the whole case
	-rm touchtheme.script
	cat ./config/touchthemehdr.script   \
		./init/init.script              \
		./home/home.script              \
		./movie/movie.script            \
		./explorer/explorer.script            \
		./music/music.script            \
		./photo/photo.script            \
		./setting/setting.script            \
		./ebook/ebook.script            \
		./fm/fm.script            \
		./calendar/calendar.script            \
		./dialog/dialog.script            \
		./record/record.script            \
		./dv/dv.script            \
        >> touchtheme.script

	# build the theme
	$(ESTUDIOROOT)/Softwares/Face200/FaceBuilderCmd/FaceBuilderCmd.exe ./config/config.ini
	# copy the theme to workspace
	cp ./theme.h   $(ROOT)/../../include/res/theme.h
	cp ./theme.bin $(WORKSPACEPATH)/beetles/rootfs/apps/theme.bin

# 删除生成的中间文件
clean:
	-rm ./theme.bin
	-rm ./touchtheme.face
	-rm ./theme.h
	-rm ./touchtheme.script
	-rm ./init/init.script
	-rm ./home/home.script
	-rm ./explorer/explorer.script
	-rm ./movie/movie.script
	-rm ./music/music.script
	-rm ./ebook/ebook.script
	-rm ./setting/setting.script
	-rm ./record/record.script
	-rm ./calendar/calendar.script
	-rm ./fm/fm.script
	-rm ./dialog/dialog.script
	-rm ./photo/photo.script
	-rm ./record/record.script
	-rm ./dv/dv.script
