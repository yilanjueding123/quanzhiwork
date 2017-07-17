/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                           update execute command
*
* File    : doupdate.c
* By      : sunny
* Version : V2.00                                                                         2010.7.3
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"
#include    "config_parser.h"
#include    "config.h"

#define     CONFIG_INI_PATH     "y:\\config\\config.bin"
#define     UPDATE_BUFFER_LEN   (32 * 1024)

static __s32 __UpdateFile(char *src_path, char *dst_path, char *data_buffer);
static __s32 __UpdateDir(char *src_path, char *dst_path, char *data_buffer);

/*
*****************************************************************************
*                            doupdate
*
*  Description:update bootfs partition information to rootfs partition.
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int doupdate(struct op *t, const char **args)
{
    int     i;
    int     ret;
    __hdle  hConfig;
    __u32   fileListNum;
    char    secPrefix[MAX_VALUE];
    char    *DataBuffer;

    hConfig = OpenConfigFile(CONFIG_INI_PATH);
    if (NULL == hConfig)
    {
        Esh_Error("OpenConfigFile %s failed.\n", CONFIG_INI_PATH);
        return DOFAILED;
    }

    ret = GetFileListNum(hConfig, &fileListNum);
    if (EPDK_OK != ret)
    {
        Esh_Error("GetFileListNum failed.\n");
        CloseConfigFile(hConfig);
        return DOFAILED;
    }

    ret = GetFileListPrefix(hConfig, secPrefix);
    if (EPDK_OK != ret)
    {
        Esh_Error("GetFileListNum failed.\n");
        CloseConfigFile(hConfig);
        return DOFAILED;
    }

    DataBuffer = Esh_palloc(UPDATE_BUFFER_LEN);
    if (NULL == DataBuffer)
    {
        Esh_Error("Esh_palloc failed.\n");
        CloseConfigFile(hConfig);
        return DOFAILED;
    }

    /* update bootfs partition information to rootfs partition */
    i = 0;
    while (i < fileListNum)
    {
        char  secName[MAX_VALUE];
        char  type[MAX_VALUE];
        char  srcPath[MAX_VALUE];
        char  desPath[MAX_VALUE];

        Esh_sprintf(secName, "%s%d", secPrefix, i);

        ret = GetFileListInfo(hConfig, secName, type, srcPath, desPath);
        if (EPDK_OK != ret)
        {
            Esh_Error("GetFileListInfo %s failed.\n", secName);
            continue;
        }
        if (0 == Esh_Stricmp(type, "DIR"))
        {
            /* copy directory */
            __UpdateDir(srcPath, desPath, DataBuffer);
        }
        else
        {
            /* copy file */
            __UpdateFile(srcPath, desPath, DataBuffer);
        }
        i++;
    }
    Esh_pfree(DataBuffer, UPDATE_BUFFER_LEN);
    CloseConfigFile(hConfig);

    /* delete config file here to avoid next time update again */
    eLIBs_remove(CONFIG_INI_PATH);
    return DOSUCCEED;
}

static __s32 __UpdateFile(char *src_path, char *dst_path, char *data_buffer)
{
    __u32    len;
    ES_FILE *src_file;
    ES_FILE *dst_file;

    src_file = eLIBs_fopen(src_path, "rb");
    if(!src_file)
    {
        __wrn("source file %s not exist\n", src_path);
        return EPDK_FAIL;
    }
    dst_file = eLIBs_fopen(dst_path, "w+");
    if(!dst_file)
    {
        __wrn("create file %s fail\n", dst_path);
        eLIBs_fclose(src_file);
        return EPDK_FAIL;
    }

    while(1)
    {
        len = eLIBs_fread(data_buffer, 1, UPDATE_BUFFER_LEN, src_file);
        eLIBs_fwrite(data_buffer, len, 1, dst_file);
        if(len < UPDATE_BUFFER_LEN)
            break;
    }
    eLIBs_fclose(src_file);
    eLIBs_fclose(dst_file);

    return EPDK_OK;
}

static __s32 __UpdateDir(char *src_path, char *dst_path, char *data_buffer)
{
    ES_DIR	*dir;
    ES_DIRENT *dirent;

    char *dpath = esMEMS_Malloc(0, ESH_MAX_FILENAME);
    char *fpath = esMEMS_Malloc(0, ESH_MAX_FILENAME);

    /*open dir*/
    dir = eLIBs_opendir(src_path);
    if(!dir)
    {
        return EPDK_FAIL;
    }
    while(1)
    {
        /*read dir*/
        dirent = eLIBs_readdir(dir);
        if (!dirent)
        {
            eLIBs_closedir(dir);
            break;
        }

        Esh_sprintf(fpath, "%s\\%s", src_path, dirent->d_name);
        Esh_sprintf(dpath, "%s\\%s", dst_path, dirent->d_name);
        if (dirent->fatdirattr & FSYS_ATTR_DIRECTORY)
        {
            if (dirent->d_name[0] == '.')
                continue;

            eLIBs_closedir(dir);
            __UpdateDir(fpath, dpath, data_buffer);
        }
        else
        {
            __UpdateFile(fpath, dpath, data_buffer);
        }
    }

    esMEMS_Mfree(0, fpath);
    esMEMS_Mfree(0, dpath);

    return EPDK_OK;
}
