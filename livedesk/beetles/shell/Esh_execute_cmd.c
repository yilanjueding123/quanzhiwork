/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                              Execute one command
*
* File    : Esh_execute_cmd.c
* By      : sunny
* Version : V1.00                                                           2009.3.2
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/* check one file is one executable program fileor not */
static int __Esh_Is_Executable_Program(const char *filename)
{
    int len;

    if ( NULL == filename )
    {
        Esh_Error("input argument is NULL!\n");
        return 0;
    }
    /* get file name length */
    len = Esh_strlen(filename);
    if ( len < 4 )
        return 0;

    if ( !Esh_Stricmp( (filename + len - 4), ".esh") )
        return ESH_ESH_TYPE_FILE;

    if ( !Esh_Stricmp( (filename + len - 4), ".axf") )
        return ESH_AXF_TYPE_FILE;

    return 0;
}
/*
*********************************************************************************
*                                   Esh_execute_command()
* Description: execute one command
*
*
* Parameters:
*               agrv: execute command parater pointer
*
* Returns    :  Comand execute function pointer except
*
*********************************************************************************
*/
void Esh_execute_command(const char **argv)
{
    builtin_func_ptr bltin = NULL;
    int  Pro_Type;
    char *new_arg = NULL;
    int DoResult = 0;                       /* defult failed, is not a command */

    if( NULL == argv )
        goto Error_Command;

    /* find execute command */
    bltin = Esh_inbuilt(argv[0]);
    if ( bltin )
        goto  Execute_Command;

    /* get executable program type : .axf or .esh */
    Pro_Type = __Esh_Is_Executable_Program(argv[0]);
    if ( !Pro_Type )
        goto Error_Command;

    /* second seach ".axf" executable program */
    new_arg = Esh_Seach_Executable_Program(argv[0]);
    if ( new_arg )
    {

        argv[1] = new_arg;

        /* .axf type file: dorun */
        if ( ESH_AXF_TYPE_FILE == Pro_Type )
            bltin = dorun;
        if ( ESH_ESH_TYPE_FILE == Pro_Type )
            bltin = doscript;

        /* set executable program name to argv[1] */
        goto  Execute_Command;
    }
    /* find fail  */
Error_Command:
    Esh_Error("\"%s\" not a command, not a executable program, not a executable scirpt file\n", argv[0]);
    Esh_printf("Please type \"help\" for more informations!\n");
    return;

Execute_Command:

    /* check builtin command pointer */
    if ( NULL == bltin )
        goto Error_Command;

    /* execute builtin command */
    DoResult = bltin(NULL, argv);

    /* remove waring: command result no use now */
    DoResult = DoResult;

    if ( new_arg )
        Esh_free_name(new_arg);

    esKRNL_TimeDly(10);
    return;
}
