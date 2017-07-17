/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: dsk_app.h
* By      	: Andy.zhang
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2009-7-8 10:02:17  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __DSK_APP_H__
#define __DSK_APP_H__

/***************************************************************************************************
*Name        : dsk_load_app
*Prototype   : __s32 dsk_load_app( char *file_name)
*Arguments   : file_name : 运行应用程序解析 file_name 文件。
*Return      : 成功 ：EPDK_OK
               失败 : EPDK_FAIL
*Description : 文件和加载应用程序绑定，用户可以修改 desktop 注册表更改 文件和应用程序的绑定关系;
               eg : aaa.mp3 文件，系统会默认运行 music.app 应用程序；
               注意 ：1. 如果应用程序尚未运行，则直接加载应用程序; 如果应用程序已经运行，则会发送异步消息 DSK_APP_RESTORE;
                      2. 如果传入文件名为 "xxxx.app" ，系统会理解用户想直接运行"xxxx.app”应用程序，运行成功的前提是 "xxxx.app“
                         已经在桌面系统注册表中注册。

*Other       :
***************************************************************************************************/
__s32 dsk_load_app( char *file_name);


/***************************************************************************************************
*Name        : dsk_kill_app
*Prototype   : __s32 dsk_kill_app( char *file_name)
*Arguments   : file_name : 带杀掉应用程序名称，如杀掉电子书应用程序为  dsk_kill_app("ebook.app");
*Return      : 成功 ：EPDK_OK
               失败 : EPDK_FAIL
*Description : file_name 应用程序必须响应 DSK_MSG_KILL 桌面消息;
*Other       :
***************************************************************************************************/
__s32 dsk_kill_app( char *file_name);

/***************************************************************************************************
*Name        : dsk_app_exist
*Prototype   : __bool dsk_app_exist(char *file_name);
*Arguments   : file_name : 应用程序名称
*Return      :
*Description : 查询应用程序是否存在
*Other       :
***************************************************************************************************/
__bool dsk_app_exist(char *file_name);

/***************************************************************************************************
*Name        : dsk_set_load_arg
*Prototype   : __s32 dsk_set_load_arg(char *app_name, void *p_arg, __u32 size);
*Arguments   : app_name ：应用程序名称
               p_arg    : 传递参数地址
               size     : 传递参数大小
*Return      :
*Description : 设置应用程序加载参数，注意，目前传递参数不能多级指针传递
*Other       :
***************************************************************************************************/
__s32 dsk_set_load_arg(char *app_name, void *p_arg, __u32 size);

/***************************************************************************************************
*Name        : dsk_set_load_arg
*Prototype   : __s32 dsk_set_load_arg(char *app_name, void *p_arg, __u32 size);
*Arguments   : app_name ：应用程序名称
               p_arg    : 传递参数地址
               size     : 传递参数大小
*Return      :
*Description : 查询应用程序加载参数
*Other       :
***************************************************************************************************/
__s32 dsk_get_load_arg(char *app_name, void *p_arg, __u32 size);

/***************************************************************************************************
*Name        : dsk_set_load_args
*Prototype   : __s32 dsk_set_load_args(char *app_name, char *src_name, void *p_arg, __u32 size)
*Arguments   : app_name ：应用程序名称
			   src_name : 源应用程序名称
               p_arg    : 传递参数地址
               size     : 传递参数大小
*Return      :
*Description : 查询应用程序加载参数
*Other       :
***************************************************************************************************/
__s32 dsk_set_load_args(char *app_name, char *src_name, void *p_arg, __u32 size);

/***************************************************************************************************
*Name        : dsk_set_load_args
*Prototype   : __s32 dsk_set_load_args(char *app_name, char *src_name, void *p_arg, __u32 size);
*Arguments   : app_name ：应用程序名称
			   src_name : 源应用程序名称
               p_arg    : 传递参数地址
               size     : 传递参数大小
*Return      :
*Description : 查询应用程序加载参数
*Other       :
***************************************************************************************************/
__s32 dsk_get_load_args(char *app_name, char *src_name, void *p_arg, __u32 size);


/* 回到主界面 */
__s32 dsk_notify_app_home(void);

/* 给所有应用程序(包含桌面系统)广播信息 */
void dsk_broadcast_msg(__gui_msg_t *msg);

/* 给所有应用程序广播信息 */
void dsk_broadcast_app_msg(__gui_msg_t *msg);

__s32 dsk_set_standby_enable(char *app_name, __bool enable);

#endif /* __DSK_APP_H__ */

