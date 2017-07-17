/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : doassert.c
* By      : sunny
* Version : V2.00                                                                         2009.3.3
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

#define     ESH_ASSERT_FILE_EXIST   1       /* Assert file exist or not */
#define     ESH_ASSERT_DIR_EXIST    2       /* Assert directory exist or not */

static int  _Esh_Get_Assert_Type(const char *type_str)
{
    if ( !Esh_Stricmp(type_str, "existfile" ) )
    {
        return ESH_ASSERT_FILE_EXIST;
    }
    if ( !Esh_Stricmp(type_str, "existdir" ) )
    {
        return ESH_ASSERT_DIR_EXIST;
    }
    /* not support assert type */
    return -1;
}
static int _Esh_Assert_Dir_Exist(const char *dirname)
{
    ES_DIR *dirp;
    if ( (const char *)NULL == dirname )
    {
        Esh_Error("Assert filename is NULL\n");
        return 0;
    }
    dirp = Esh_opendir(dirname);
    if ( (ES_DIR *)NULL == dirp )
    {
        return 0;
    }
    Esh_closedir(dirp);
    return 1;
}
static int _Esh_Assert_File_Exist(const char *filename)
{
    ES_FILE *fp;

    if ( (const char *)NULL == filename )
    {
        Esh_Error("Assert filename is NULL\n");
        return 0;
    }
    fp = Esh_fopen(filename, "rb");
    if ( (ES_FILE *)NULL == fp )
    {
        return 0;
    }
    Esh_fclose(fp);
    return 1;
}
static int _Esh_Assert(const char **args, int is_opposite)
{
    int AssertType;
    if ( ((const char *)NULL == args[0]) || ((const char *)NULL == args[1]) )
    {
        Esh_Error("Assert Command NULL\n");
        return 0;
    }
    /* paser assert command type */
    AssertType = _Esh_Get_Assert_Type(args[0]);

    /* Assert file exist or not */
    if ( ESH_ASSERT_FILE_EXIST == AssertType )
    {

        /* file exist */
        if ( _Esh_Assert_File_Exist(args[1]) )
        {
            if ( is_opposite )
            {
                Esh_printf("Assert file:\"%s\" not exist failed\n", args[1]);
                return 0;
            }
            Esh_printf("Assert file:\"%s\" exist succeeded\n", args[1]);
            return 1;
        }
        /* file not exist */
        if ( is_opposite )
        {
            Esh_printf("Assert file:\"%s\" not exist succeeded\n", args[1]);
            return 1;
        }
        Esh_printf("Assert file:\"%s\" exist failed\n", args[1]);
        return 0;
    }
    /* Assert directory exist or not */
    if ( ESH_ASSERT_DIR_EXIST == AssertType )
    {

        /* directory exist */
        if ( _Esh_Assert_Dir_Exist(args[1]) )
        {
            if ( is_opposite )
            {
                Esh_printf("Assert directory:\"%s\" not exist failed\n", args[1]);
                return 0;
            }
            Esh_printf("Assert directory:\"%s\" exist succeeded\n", args[1]);
            return 1;
        }
        /* directory not exist */
        if ( is_opposite )
        {
            Esh_printf("Assert directory:\"%s\" not exist succeeded\n", args[1]);
            return 1;
        }
        Esh_printf("Assert directory:\"%s\" exist failed\n", args[1]);
        return 0;
    }
    Esh_Error("Not support assert command type\n");
    return 0;
}
/*
*****************************************************************************
*                            doassert
*
*  Description: assert some thing, mainly use under fs test
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int doassert(struct op *t, const char **args)
{
    int Opposite = 0;
    int AssertCommandBegin = 1;

    if ((const char **)NULL == args )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    if ( ((const char *)NULL != args[1]) && (!Esh_Stricmp(args[1], "not")) )
    {
        Opposite = 1;
        if ( (const char *)NULL != args[2] )
            AssertCommandBegin = 2;
    }
    if ( _Esh_Assert(&args[AssertCommandBegin], Opposite) )
        return DOSUCCEED;

    /* Assert failed */
    return DOFAILED;
}
