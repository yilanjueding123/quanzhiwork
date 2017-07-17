/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	:app_main.h
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __APP_HOME_H_
#define  __APP_HOME_H_

H_WIN app_home_create(root_para_t *para);

//应用退出到桌面时设置回当前背景
void __app_home_change_to_home_bg(void);


#endif// __APP_HOME_H_
