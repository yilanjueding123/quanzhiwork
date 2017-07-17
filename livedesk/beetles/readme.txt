V3
1.目录结构：
shell:		shell进程	
applets: 		测试应用插件如:home, explore, ebook, music等。
res:  		应用插件的资源文件，包含语言和图片资源。

include: 		所有应用插件都需要包含此目录下的apps.h

init:		应用程序入口，挂载home插件，消息循环，响应桌面事件，同时提供公共场景单元如background和headbar供所有应用插件共享
libapps: 		编译生成公共静态库app_libs.a，applets目录下面的每个插件都需要链接app_libs.a

mod_desktop:	桌面共享库，applets目录下面的每个插件都需要链接desktop.mod.
libutils:		c工具库，提供一下基本的包装函数.
