/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                             config file parser
*
* File    : config.c
* By      : sunny
* Version : V2.00                                                                         2010.7.5
*********************************************************************************************************
*/

#ifndef __CONFIG_H__
#define __CONFIG_H__

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#define MAX_ALLSECTIONS (260 * 1000)	//全部的段名
#define MAX_SECTION		260				//一个段名长度
#define MAX_ALLKEYS		(260 * 2000)	//全部的键名
#define MAX_KEY			260				//一个键名长度
#define MAX_VALUE		260

// config plug return succeeded value
#define OK              0

/* typedef */
typedef signed char			  s8;
typedef signed char			  __s8;
typedef signed char		      int8;

typedef unsigned char		  u8;
typedef unsigned char		  __u8;
typedef unsigned char		  uint8;


typedef short int			  s16;
typedef short int			  __s16;
typedef short int			  int16;

typedef unsigned short		  u16;
typedef unsigned short		  __u16;
typedef unsigned short		  uint16;

typedef int					  s32;
typedef int					  __s32;
typedef int					  int32;

typedef unsigned int		  u32;
typedef unsigned int		  __u32;
typedef unsigned int		  uint32;

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
typedef void *HCONFIG;
typedef void *POS;

HCONFIG OpenConfig(char *szFilePath, void *Param);
u32     CloseConfig(HCONFIG hcfg);
u32     GetKeyValue(HCONFIG hcfg, s8 *szSection, s8 *szKey, u32 *pValue);
u32     GetKeyString(HCONFIG hcfg, s8 *szSection, s8 *szKey, s8 *String);

#endif /* __CONFIG_H__ */

