/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : docat.c
* By      : sunny
* Version : V2.00                                                                         2009.3.3
*********************************************************************************************************
*/
#include    "Esh_shell_private.h"

__u32 Esh_ShowRowNumber;            /* record row number  */

static void __Esh_Buffer_Print(char *buf, __u32 len)
{
    int col;
    int j, i = 0;
    char cache[5 * 16 + 1];

    cache[5 * 16] = 0;                   /* print 16 Byte data in one line, One byte data in 5 row */

    while(i < len)
    {

        for(j = 0; j < 16 * 5; j++)cache[j] = 0;

        for (col = 0; col < 16 && i < len; col++, i++)
        {
            char a, b;
            a = (buf[i] & 0x0f);
            b = (buf[i] >> 4);
            a = a < 0x0a ? '0' + a : 'a' + a - 0x0a;    /* low bit */
            b = b < 0x0a ? '0' + b : 'a' + b - 0x0a;    /* high bit */
            cache[col * 5 + 0] = ' ';
            cache[col * 5 + 1] = '0';
            cache[col * 5 + 2] = 'x';
            cache[col * 5 + 3] = b;
            cache[col * 5 + 4] = a;
        }
        Esh_printf("\n %x : %s", Esh_ShowRowNumber, cache);   /* print one line */
        Esh_ShowRowNumber += 16;
    }
}
/*
*****************************************************************************
*                            docat
*
*  Description: show specific text file content
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int docat(struct op *t, const char **args)
{
    int HexFormat = 0;
    ES_FILE *fp;
    char *filename;
    char *databuffer;
    int  len;
    int  Readtime;
    char *pst;

    /*The input directory path is a full path or not*/
    if ( NULL == args[1] )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    /* get print format */
    if ( args[2] )
    {
        if ( !Esh_strcmp(args[2], "/x") || !Esh_strcmp(args[2], "/X") )
            HexFormat = 1;
    }
    /* get file name */
    filename = Esh_get_name(args[1]);

    if ( NULL == filename )
    {
        Esh_Error("Getname : %s failed\n", args[1]);
        return DOFAILED;
    }
    /*read content from file*/
    Esh_printf("File -- %s: \n", filename);
    fp = Esh_fopen(filename, "r");
    if ( !fp )
    {
        Esh_Error("Open file failed", filename);
        Esh_free(filename);
        return DOFAILED;
    }
    databuffer = Esh_malloc(0x100 + 1);

    if ( !databuffer )
    {
        Esh_Error("Malloc failed\n");
        Esh_free(filename);
        Esh_Global->ErrorNum = ESH_ERROR_NO_MEM;
        return DOFAILED;
    }
    databuffer[0x100] = 0;
    Esh_ShowRowNumber = 0;
    if ( HexFormat )
    {
        Esh_printf("\n  Offset       0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F");
    }
    while ( (Readtime = Esh_fread(databuffer, 1, 0x100, fp)) != 0 )
    {
        pst = databuffer;

        /* display content of file */
        if ( HexFormat )
        {

            /* print in hex format */
            __Esh_Buffer_Print(databuffer, Readtime);

        }
        else
        {
            while ( pst < &databuffer[0x100] && pst )
            {
                Esh_printf("%s", pst);
                len = Esh_strlen(pst);
                pst += len + 1;
            }
        }
        /* buffer set zero */
        Esh_memset(databuffer, 0x0, 0x100);
    }
    Esh_printf("\n");

    Esh_fclose(fp);

    /* free memory */
    Esh_free_name(filename);
    Esh_free(databuffer);

    /* no any error */
    return DOSUCCEED;
}
