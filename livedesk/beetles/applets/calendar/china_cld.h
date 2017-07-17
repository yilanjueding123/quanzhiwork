/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           calendar app sample
*
*						        	(c) Copyright 2006-2009, ANDY, China
*											 All Rights Reserved
*
* File    : china_cld.c
* By      :lyn
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __CHINA_CLD_H__
#define __CHINA_CLD_H__

void Cld_GetChinaCalendarStr(__u32  year, __u8 month, __u8 day, char *str);

void Cld_GetChinaCalendarStrEx( __u32  year, __u8 month, __u8 day, char *str);

void Cld_GetChinaDayStr( __u32  year, __u8 month, __u8 day, char *str);

void Cld_GetChinaDayStrEx( __u32  year, __u8 month, __u8 day, char *str);

__u8 Cld_GetJieQiStr(__u32 year, __u8 month, __u8 day, char *str);

__u8 Cld_GetJieQiStrEx(__u32 year, __u8 month, __u8 day, char *str);

void Cld_GetShengXiaoStr(__u32  year, __u8 month, __u8 day, char *str);

#endif /* __CHINA_CLD_H__ */

