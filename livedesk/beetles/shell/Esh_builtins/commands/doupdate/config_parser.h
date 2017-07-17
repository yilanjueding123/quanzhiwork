/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                             config file parser
*
* File    : config_parser.h
* By      : sunny
* Version : V2.00                                                                         2010.7.3
*********************************************************************************************************
*/

#ifndef __CONFIG_PARSER_H__
#define __CONFIG_PARSER_H__

__s32   OpenConfigFile(const char *filepath);
__s32   GetFileListNum(__hdle hConfig, __u32 *Num);
__s32   GetFileListPrefix(__hdle hConfig, char *secPrefix);
__s32   GetFileListInfo(__hdle hConfig, const char *secName,
                        char *type, char *SrcPath, char *DesPath);
__s32   CloseConfigFile(__hdle hConfig);

#endif  /* __CONFIG_PARSER_H__ */
