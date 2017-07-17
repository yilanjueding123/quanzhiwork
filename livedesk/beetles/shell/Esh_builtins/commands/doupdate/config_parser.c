/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                             config file parser
*
* File    : config_parser.c
* By      : sunny
* Version : V2.00                                                                         2010.7.3
*********************************************************************************************************
*/

#include    "epdk.h"
#include    "config.h"

#define     FILE_LIST_SECTION_NAME          "FILELIST"
#define     FILE_LIST_SECTION_COUNT_NAME    "count"
#define     FILE_LIST_SECTION_PREFIX_NAME   "sec"
#define     FILE_LIST_TYPE_NAME             "type"
#define     FILE_LIST_SRC_PATH_NAME         "srcPath"
#define     FILE_LIST_DES_PATH_NAME         "desPath"

__hdle OpenConfigFile(const char *filepath)
{
    HCONFIG hConfig;

    if (NULL == filepath)
    {
        return NULL;
    }
    hConfig = OpenConfig((char *)filepath, NULL);

    return (__hdle)hConfig;
}

__s32 GetFileListNum(__hdle hConfig, __u32 *Num)
{
    __u32 ret;
    if ((NULL == hConfig) || (NULL == Num))
    {
        return EPDK_FAIL;
    }
    ret = GetKeyValue((HCONFIG)hConfig,
                      (s8 *)FILE_LIST_SECTION_NAME,
                      (s8 *)FILE_LIST_SECTION_COUNT_NAME,
                      Num);
    if (ret != OK)
    {
        __inf("GetKeyValue failed\n");
        return EPDK_FAIL;
    }
    return EPDK_OK;
}

__s32 GetFileListPrefix(__hdle hConfig, char *secPrefix)
{
    __u32 ret;
    if ((NULL == hConfig) || (NULL == secPrefix))
    {
        return EPDK_FAIL;
    }
    ret = GetKeyString((HCONFIG)hConfig,
                       (s8 *)FILE_LIST_SECTION_NAME,
                       (s8 *)FILE_LIST_SECTION_PREFIX_NAME,
                       (s8 *)secPrefix);
    if (ret != OK)
    {
        __inf("GetKeyValue failed\n");
        return EPDK_FAIL;
    }
    return EPDK_OK;
}


__s32 GetFileListInfo(__hdle hConfig, const char *secName,
                      char *type, char *SrcPath, char *DesPath)
{
    __u32 ret;
    if ((NULL == hConfig) || (NULL == secName) ||
            (NULL == type) || (NULL == SrcPath) || (NULL == DesPath))
    {
        return EPDK_FAIL;
    }

    /* get filelist type information */
    ret = GetKeyString((HCONFIG)hConfig,
                       (s8 *)secName,
                       (s8 *)FILE_LIST_TYPE_NAME,
                       (s8 *)type);
    if (ret != OK)
    {
        __inf("GetKeyValue failed\n");
        return EPDK_FAIL;
    }

    /* get filelist source path information */
    ret = GetKeyString((HCONFIG)hConfig,
                       (s8 *)secName,
                       (s8 *)FILE_LIST_SRC_PATH_NAME,
                       (s8 *)SrcPath);
    if (ret != OK)
    {
        __inf("GetKeyValue failed\n");
        return EPDK_FAIL;
    }

    /* get filelist dest path information */
    ret = GetKeyString((HCONFIG)hConfig,
                       (s8 *)secName,
                       (s8 *)FILE_LIST_DES_PATH_NAME,
                       (s8 *)DesPath);
    if (ret != OK)
    {
        __inf("GetKeyValue failed\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

__s32 CloseConfigFile(__hdle hConfig)
{
    if (OK != CloseConfig((HCONFIG)hConfig))
    {
        return EPDK_FAIL;
    }
    return EPDK_OK;
}
