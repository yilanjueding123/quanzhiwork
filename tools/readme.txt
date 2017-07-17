文件说明：
    1. Melis平台编译环境安装配置说明.doc    编译环境安装及使用说明；
    2. cygwin.2.00.exe                      cygwin安装文件；

    3. dram_DDRII_init_180M.axf             DRAM初始化程序；
    4. fel_pro.axf                          烧码引导程序；
    4. boot0.axf                            启动引导程序；

使用说明：
    1. 烧码：
        step1: 板子上电，并reset；
        step2: 通过AXD工具下载dram_DDRII_init_180M.axf，运行该程序进行DRAM初始化；
        step3: 通过AXD工具下载fel_pro.axf，运行该程序引导烧码，注意此时需要打开Livesuit工具及连接USB；
        step4: 等待烧码完成；

    2. 启动：
        step1: reset开发板；
        step2: 通过AXD工具下载dram_DDRII_init_180M.axf，运行该程序进行DRAM初始化；
        step3: 通过AXD工具下载boot0.axf，运行该程序引导系统启动；

