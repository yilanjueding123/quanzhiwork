/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : dorun.c
* By      : sunny
* Version : V2.00                                                                         2009.3.3
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

static void __Esh_free_main_args(void *main_args[])
{
    int i;
    for ( i = 0; i < ESH_MAX_PRO_ARGS_NUM; i++ )
    {
        if ( main_args[i] )
            Esh_free(main_args[i]);
    }
}
static int __Esh_parse_main_args(const char **input_args, int max_args_num, void *main_args[])
{
    int i;
    int  argc = 0;                                  /* number of arguments are passed to the program  */

    /* allocate main_args[0] : argc */
    main_args[0] = (int *)Esh_malloc(sizeof(int));
    if ( NULL == main_args[0] )
    {
        Esh_Error("Malloc failed\n");
        Esh_Global->ErrorNum = ESH_ERROR_NO_MEM;
        return 0;
    }
    /* parse arguments for programe */
    for ( i = 0; (i < max_args_num) && (i < ESH_MAX_PRO_ARGS_NUM); i++ )
    {

        if ( NULL == input_args[i] )
            break;
        main_args[i + 1] = Esh_malloc(Esh_strlen(input_args[i]) + 1);

        if ( NULL == main_args[i + 1] )
        {
            Esh_Error("Malloc failed\n");
            Esh_Global->ErrorNum = ESH_ERROR_NO_MEM;

            /* free space allocated */
            __Esh_free_main_args(main_args);
            return 0;
        }
        /* save argument value */
        Esh_strcpy(main_args[i + 1], input_args[i]);
        argc++;
    }
    /* set arguments number */
    *(int *)(main_args[0]) = argc;
    return 1;
}
static void _Esh_Debug_Show_Main_Args(void *main_args[])
{
    int i;

    /* show argc */
    if ( main_args[0] )
        Esh_printf("argc:%d\n", *(int *)(main_args[0]));

    /* show arguments */
    for ( i = 1; i < ESH_MAX_PRO_ARGS_NUM; i++ )
    {
        if ( main_args[i] )
            Esh_printf("argument%d : \"%s\"\n", i, (char *)(main_args[i]));
    }
}
/*
*****************************************************************************
*                            dorun
*
*  Description: run app
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int dorun(struct op *t, const char **args)
{
    int i;
    int ArgBegin;
    __u32 Run_Mode = 0;
    char *AppName;
    void **Main_args;       /* epos main argument type */

    /* check input app name */
    if ( NULL == args[1] )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    /* get app name */
    AppName = Esh_get_app_name(args[1]);

    if ( NULL == AppName )
    {
        Esh_Error("Getname : \"%s\" failed\n", args[1]);
        return DOFAILED;
    }
    /* Get arguments table buffer */
    Main_args = (void **)Esh_malloc(sizeof(void *) * ESH_MAX_PRO_ARGS_NUM);

    if ( NULL == Main_args )
    {
        Esh_Error("Malloc failed\n");
        Esh_Global->ErrorNum = ESH_ERROR_NO_MEM;
        Esh_free_name(AppName);
        return DOFAILED;
    }
    /* init parameters */
    for ( i = 0; i < ESH_MAX_PRO_ARGS_NUM; i++ )
    {
        Main_args[i] = NULL;
    }
    /* check if create wait mode */
    ArgBegin = 2;                   /* if no /w or /W option£¬
	                                 *arguments of run command begin from args[2] */
    if ( args[2] )
    {
        if ( !Esh_strcmp(args[2], "/w")  || !Esh_strcmp(args[2], "/W") )
        {
            ArgBegin = 3;          /* arguments of Run command begin from args[3] */
            Run_Mode = EXEC_CREATE_WAIT_RET;
        }
    }
    /* parse main() parameter */
    if ( !__Esh_parse_main_args(&(args[ArgBegin]), (ESH_MAX_CMD_ARGS - ArgBegin), Main_args) )
    {

        Esh_Error("Parse programe arguments fail\n");
        Esh_free_name(AppName);
        Esh_free(Main_args);
        return DOFAILED;
    }
    /* show arguments */
    if ( 0 )
        _Esh_Debug_Show_Main_Args(Main_args);

    /* run app */
    esEXEC_PCreate(AppName, Main_args, Run_Mode, 0);

    /* if EXEC_CREATE_WAIT_RET, delay 100ms , give some time for system delete program */
    if ( EXEC_CREATE_WAIT_RET == Run_Mode )
        esKRNL_TimeDly(10);

    __Esh_free_main_args(Main_args);

    Esh_free(Main_args);
    Esh_free_name(AppName);

    /* no any error */
    return DOSUCCEED;
}
