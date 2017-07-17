/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : doxcopy.c
* By      : sunny
* Version : V2.00                                                                         2009.6.30
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/* copy data buffer size */
#define     DOCOPY_BUFFER_SIZE        0x8000

__s32 _Esh_CopyFile(char *src_path, char *dst_path)
{
    ES_FILE *src_file, *dst_file;

    char *path = esMEMS_Malloc(0, ESH_MAX_FILENAME);
    char *data = esMEMS_Malloc(0, 32 * 1024);
    __u32 len;

    src_file = eLIBs_fopen(src_path, "r");
    if(!src_file)
    {
        __wrn("source file %s not exist\n", src_path);
        esMEMS_Mfree(0, path);
        esMEMS_Mfree(0, data);
        return EPDK_FAIL;
    }

    eLIBs_strcpy(path, dst_path);
    eLIBs_strcat(path, eLIBs_strchrlast(src_path, '\\'));

    /*if file exist. rename */
    while((dst_file = eLIBs_fopen(path, "r")) != NULL)
    {
        eLIBs_fclose(dst_file);
        eLIBs_strcpy(data, "new_");
        eLIBs_strcat(data, eLIBs_strchrlast(path, '\\') + 1);

        if(eLIBs_strlen(path) + 4 > ESH_MAX_FILENAME)
            break;

        else
            eLIBs_strcpy(eLIBs_strchrlast(path, '\\') + 1, data);

    }

    dst_file = eLIBs_fopen(path, "w+");
    if(!dst_file)
    {
        __wrn("create file %s fail\n", path);
        esMEMS_Mfree(0, path);
        esMEMS_Mfree(0, data);
        eLIBs_fclose(src_file);
        return EPDK_FAIL;
    }

    while(1)
    {
        len = eLIBs_fread(data, 1, 32 * 1024, src_file);
        eLIBs_fwrite(data, len, 1, dst_file);

        if(len < 32 * 1024)
            break;
    }

    eLIBs_fclose(src_file);
    eLIBs_fclose(dst_file);

    esMEMS_Mfree(0, path);
    esMEMS_Mfree(0, data);

    return EPDK_OK;
}

__s32 _Esh_CopyDir(char *src_path, char *dst_path)
{
    ES_DIR	*dir;
    ES_DIRENT *dirent;

    char *dpath = esMEMS_Malloc(0, ESH_MAX_FILENAME);
    char *fpath = esMEMS_Malloc(0, ESH_MAX_FILENAME);

    /*set dst path*/
    eLIBs_strcpy(dpath, dst_path);
    eLIBs_strcat(dpath, eLIBs_strchrlast(src_path, '\\'));

    /*if dir exist. rename */
    while((	dir = eLIBs_opendir(dpath)) != NULL)
    {
        eLIBs_closedir(dir);

        eLIBs_strcpy(fpath, "new_");
        eLIBs_strcat(fpath, eLIBs_strchrlast(dpath, '\\') + 1);

        if(eLIBs_strlen(dpath) + 4 > ESH_MAX_FILENAME)
            break;

        else
            eLIBs_strcpy(eLIBs_strchrlast(dpath, '\\') + 1, fpath);

    }

    eLIBs_mkdir(dpath);
    /*open dir*/
    dir = eLIBs_opendir(src_path);
    if(dir == NULL)
    {
        return EPDK_FAIL;
    }
    while(1)
    {
        /*read dir*/
        dirent = eLIBs_readdir(dir);
        if (dirent == NULL)
        {
            eLIBs_closedir(dir);
            break;
        }

        eLIBs_strcpy(fpath, src_path);
        eLIBs_strcat(fpath, "\\");
        eLIBs_strcat(fpath, (const char *)dirent->d_name);

        if (dirent->fatdirattr & FSYS_ATTR_DIRECTORY)
        {
            if (dirent->d_name[0] == '.')
                continue;

            eLIBs_closedir(dir);
            _Esh_CopyDir(fpath, dpath);
        }
        else
            _Esh_CopyFile(fpath, dpath);
    }

    esMEMS_Mfree(0, fpath);
    esMEMS_Mfree(0, dpath);

    return EPDK_OK;
}



/*
*****************************************************************************
*                            doxcopy
*
*  Description:copy specific directory and sub directory to another
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int doxcopy(struct op *t, const char **args)
{
    char *SrcDir = NULL;
    char *DstDir = NULL;
    char *DataBuffer;
    int  DataLen;
    int  CopyRes;

    /* check input arguments */
    if ( (NULL == args[1]) || (NULL == args[2]) )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    /* get source file name */
    SrcDir = Esh_get_name(args[1]);
    if ( NULL == SrcDir )
    {
        Esh_Error("Getname : %s failed\n", args[1]);
        return DOFAILED;
    }
    /* get source file name */
    DstDir = Esh_get_name(args[2]);
    if ( NULL == DstDir )
    {
        Esh_Error("Getname : %s failed\n", args[2]);
        Esh_free_name(SrcDir);
        return DOFAILED;
    }
    /* allocate data buffer */
    DataLen = DOCOPY_BUFFER_SIZE;
    DataBuffer = Esh_palloc(DataLen);
    if ( NULL == DataBuffer )
    {
        Esh_Error("Malloc failed\n");
        Esh_free_name(SrcDir);
        Esh_free_name(DstDir);
        Esh_Global->ErrorNum = ESH_ERROR_NO_MEM;
        return DOFAILED;
    }
    Esh_memset(DataBuffer, 0, DataLen);

    /* copy file content from source to dest */
    CopyRes = _Esh_CopyDir(SrcDir, DstDir);

    Esh_free_name(SrcDir);
    Esh_free_name(DstDir);
    Esh_pfree(DataBuffer, DataLen);
    return CopyRes;
}
