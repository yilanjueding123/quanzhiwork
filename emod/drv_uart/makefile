#/*
#****************************************************************************************************
#*                                               MELIS
#*                               the Easy Portable/Player Develop Kits
#*                                           UART Driver
#*
#*                           (c) Copyright 2011-2014, All winners Co,Ld.
#*                                       All Rights Reserved
#*
#* File    : makefile
#* By      : james.deng
#* Version : 1.0.0
#* Date    : 2011-12-24
#* Descript: 
#* Update  : <date>          <author>            <version>           <notes>
#*           2011-12-24      james.deng          1.0.0               build the file.
#****************************************************************************************************
#*/


#导入编译器配置
include make.cfg


#从所有子目录中得到源代码的列表
SRCCS=$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
SRCSS=$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.s))

#得到源代码对应的目标文件的列表
OBJS=$(SRCCS:.c=.o) $(SRCSS:.s=.o)


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
