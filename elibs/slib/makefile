#/*
#*********************************************************************************************************
#*                                                    MELIS
#*                                    the Easy Portable/Player Develop Kits
#*                                             Share Libary Makefile
#*
#*                                    (c) Copyright 2011-2014, Sunny China
#*                                             All Rights Reserved
#*
#* File    : makefile
#* By      : Sunny
#* Version : v1.0
#* Date    : 2011-4-30
#* Descript: share libary makefile.
#* Update  : date                auther      ver     notes
#*           2011-4-30 16:01:20  Sunny       1.0     Create this file.
#*********************************************************************************************************
#*/

#导入编译器配置
include make.cfg


#从所有子目录中得到源代码的列表
SRCCS=$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
SRCSS=$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.s))

#得到源代码对应的目标文件的列表
OBJS=$(SRCCS:.c=.o) $(SRCSS:.s=.o)

$(LOCALTARGET):$(OBJS)
	$(AR) $(ARFLAGS) $(LOCALTARGET) $(filter %.o ,$+)
	cp $(LOCALTARGET) $(TARGET)
	-rm $(LOCALTARGET)


# 删除生成的中间文件
clean:
	-rm $(OBJS) $(LOCALTARGET)
