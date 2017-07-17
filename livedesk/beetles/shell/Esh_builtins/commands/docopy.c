/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : docopy.c
* By      : sunny
* Version : V2.00                                                                         2009.3.3
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

#define     DOCOPY_BUFFER_SIZE        0x8000

static int __CopyData(char *src, char *dest, char *buffer, int buffersize)
{
    ES_FILE *Sourcefp;
    ES_FILE *Destfp;
    int len;

    /* open source file */
    Sourcefp = Esh_fopen(src, "rb");
    if ( NULL == Sourcefp )
    {
        Esh_Error("Open source file %s failed.\n", src);
        return DOFAILED;
    }
    /* open dest file */
    Destfp = Esh_fopen(dest, "wb+");
    if ( NULL == Destfp )
    {
        Esh_Error("Open dest file %s failed.\n", dest);
        Esh_fclose(Sourcefp);
        return DOFAILED;
    }
    /* copy file data */
    Esh_printf("Copy form file:\"%s\" to file: \"%s\"...\n", src, dest);
    while ( (len = Esh_fread(buffer, 1, buffersize, Sourcefp)) != 0 )
    {
        if ( Esh_fwrite(buffer, 1, len, Destfp) != len )
        {
            Esh_Error("Write dest file %s failed.\n", dest);
            Esh_fclose(Sourcefp);
            Esh_fclose(Destfp);
            return DOFAILED;
        }
    }
    Esh_fclose(Sourcefp);
    Esh_fclose(Destfp);
    return DOSUCCEED;
}
/*
*****************************************************************************
*                            docopy
*
*  Description:copy specific text file content to another
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int docopy(struct op *t, const char **args)
{
    char *SourceFile = NULL;
    char *DestFile = NULL;
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
    SourceFile = Esh_get_name(args[1]);
    if ( NULL == SourceFile )
    {
        Esh_Error("Getname : %s failed\n", args[1]);
        return DOFAILED;
    }
    /* get source file name */
    DestFile = Esh_get_name(args[2]);
    if ( NULL == DestFile )
    {
        Esh_Error("Getname : %s failed\n", args[2]);
        Esh_free_name(SourceFile);
        return DOFAILED;
    }
    /* allocate data buffer */
    DataLen = DOCOPY_BUFFER_SIZE;
    DataBuffer = Esh_palloc(DataLen);
    if ( NULL == DataBuffer )
    {
        Esh_Error("Malloc failed\n");
        Esh_free_name(SourceFile);
        Esh_free_name(DestFile);
        Esh_Global->ErrorNum = ESH_ERROR_NO_MEM;
        return DOFAILED;
    }
    Esh_memset(DataBuffer, 0, DataLen);

    /* copy file content from source to dest */
    CopyRes = __CopyData(SourceFile, DestFile, DataBuffer, DataLen);

    Esh_free_name(SourceFile);
    Esh_free_name(DestFile);
    Esh_pfree(DataBuffer, DataLen);
    return CopyRes;
}
