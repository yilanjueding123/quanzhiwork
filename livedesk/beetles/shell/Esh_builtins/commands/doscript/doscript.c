/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              apps Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : Shell
* File    : doscript.c
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#include  "doscript_private.h"

int  gflg;                /* Global flag ,indicate line buffer overflow */
char *null = (char *)"";  /* null value for variable */

char line[LINELIM];
char *linep;
char *elinep;             /* done in main(): = line + sizeof(line) - 5 */

char *g_SourceCode;
char *g_SourceCodeBase;
char *g_SourceCodeTop;

static long Dis_Get_File_Size(const char *filename)
{
    FILE *fp;
    long filesize = 0;

    /* open file for read */
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("Error: open file %s failed when get filesize\n", filename);
        return 0;
    }
    /* get the file size */
    if (fseek(fp, 0, SEEK_END) != 0 )           // seek to file end
    {
        printf("Error: fseek error\n");
        fclose(fp);
        return 0;
    }
    filesize = ftell(fp);
    if ( filesize == -1 )                       // ftell have error
    {
        printf("Error: ftell error\n");
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return filesize;
}
static long LoadSourceFile (char *Source, char *SourceFileName)
{
    /* Open the input file */
    FILE *pSourceFile;
    long filesize;

    /* get file size */
    filesize = Dis_Get_File_Size(SourceFileName);
    if ( filesize <= 0 )
    {
        printf("Error: get file size failed\n");
        return 0;
    }
    /* check filesize */
    if ( filesize >  MAX_SOURCE_SIZE )
    {
        printf("Error: source file oversize\n");
        return 0;
    }
    if ( (pSourceFile = fopen( SourceFileName, "rb")) == NULL )
    {
        printf("Error:can't open source file for read\n");
        return 0;   // open file failed
    }
    /* Read source script */
    if ( fread(Source, filesize, 1, pSourceFile) != 1 )
    {
        printf("Error: read source failed\n");
        fclose(pSourceFile);
        return 0;
    }
    /* add end indicater to "Source" */
    Source[filesize] = '\0';

    fclose(pSourceFile);
    return 1;
}
int doscript(struct op *t, const char **args)
{
    int OneCommandResult;
    int SourceLen;
    char *SourceFile;
    gflg = 0;
    OneCommandResult = 0;
    SourceLen = 0;
    elinep = line + sizeof(line) - 5;

    /* get source script file name */
    if ( args[1] == NULL )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    /* get app name */
    SourceFile = Esh_get_name(args[1]);
    if ( NULL == SourceFile )
    {
        Esh_Error("Getname : %s failed\n", args[1]);
        return DOFAILED;
    }
    SourceLen = Dis_Get_File_Size(SourceFile);
    if ( SourceLen <= 0 )
    {
        printf("Error: get file size failed\n");
        Esh_free_name(SourceFile);
        return 0;
    }
    /* check file size */ // not do
    memset(g_SourceCodeBase, '\0', MAX_SOURCE_SIZE + 1);
    if ( LoadSourceFile(g_SourceCodeBase, SourceFile) )             /* load succeeded */
    {

        g_SourceCode = g_SourceCodeBase;                        /* base position */
        g_SourceCodeTop = (g_SourceCodeBase + SourceLen);       /* top position  */

        /* initialize area */
        areanum = 1;
        linenumber = 0;

        /* parse script file ,execute commmand */
        do
        {
            OneCommandResult = onecommand();

            /* the file end ? */
            if ( EndOfFile() )
            {
                break;
            }
            /* check eixt or not */
            if (Esh_Global->ExitFlag == ESH_TURE)
            {
                break;
            }
        }
        while ( OneCommandResult );      /* Result Is Ture */
    }
    Esh_free_name(SourceFile);

    /* no any error */
    return DOSUCCEED;
}
void Esh_Script_Initi(void)
{
    g_SourceCodeBase = (char *)malloc(MAX_SOURCE_SIZE + 1);
    initarea();
}
void Esh_Script_Exit(void)
{
    /* clean area */
    cleanarea();
    free(g_SourceCodeBase);
}
