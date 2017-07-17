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
* File    : Esh_init.c
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#include    "Esh_shell_private.h"

/* global varible structure */
Esh_Global_Varible *Esh_Global;
/*
*********************************************************************************
*                                   Esh_init()
* Description: initialize shell global
*
*
* Parameters:   None
*
*
* Returns    :  None
*
*********************************************************************************
*/
int Esh_init(void)
{
    /* allocate global varibles structure body */
    Esh_Global = Esh_malloc(sizeof(Esh_Global_Varible));

    if ( NULL == Esh_Global )
    {
        Esh_Error("Malloc failed\n");
        return ESH_FAILED;
    }

    /* set exit flag false while initialize */
    Esh_Global->ExitFlag = ESH_FALSE;

    /* initialize Eshell global varibles */
    Esh_Global->ErrorNum = ESH_NO_ERROR;                    /* no error */

    Esh_strcpy(Esh_Global->PWdir, "C:\\APPS");              /* set work directory */

    Esh_strcpy(Esh_Global->AppPath1, BEETLES_APP_ROOT"APPS");           /* set seach app directory */

    Esh_strcpy(Esh_Global->StartUpScript, "C:\\startup.esh"); /* set startup script file */

    /* initialize script */
    Esh_Script_Initi();

    /* init succeeded */
    return ESH_SUCCEED;
}
/*
*********************************************************************************
*                                   Esh_Setup()
* Description: Execute all system start work
*
*
* Parameters:   None
*
*
* Returns    :  None
*
*********************************************************************************
*/
void Esh_StartUp(void)
{
    FILE *fp;
    const char *args[2];
    const char *StartUp_Script;

    /* get startup script file name */
    StartUp_Script = (const char *)(Esh_Global->StartUpScript);

    /* open setup script file for check */
    fp = Esh_fopen(StartUp_Script, "r");
    if ( NULL == fp )
    {
        Esh_Error("Open startup script file: \"%s\" failed\n", StartUp_Script);
        return;
    }
    Esh_fclose(fp);

    /* build arguments for "doscript" */
    args[0] = NULL;                             /* no use for "doscript" */
    args[1] = StartUp_Script;                   /* argument for "doscript" */

    /* execute setup script */
    doscript(NULL,  args);
}
