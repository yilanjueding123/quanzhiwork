/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                              Mussy support
*
* File    : Esh_support.c
* By      : sunny
* Version : V1.00                                                           2009.3.2
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                            __Esh_IsFullPath
*
*  Description:  check input path is full path or not
*
*
*  Parameters:
*                path : input path
*
*  Return value:
*               1 : check succeeded    0 : failed
****************************************************************************
*/
static int __Esh_IsFullPath(const char *path)
{
    int PathLen = 0;

    /*chech the input directory path is a full path or not*/
    if ( !path )
    {
        Esh_Error("__Esh_IsFullPath: input argument is NULL!\n");
    }
    /* get path string length */
    PathLen = Esh_strlen(path);

    /* check path length */
    if ( PathLen < 2 )
        return 0;

    /* check is full path or not */
    if ( ( (path[0] >= 'a' && path[0] <= 'z') ||
            (path[0] >= 'A' && path[0] <= 'Z') ) &&
            path[1] == ':' )
    {

        return 1;
    }
    return 0;
}
char *Esh_alloc_name(unsigned int size)
{
    return Esh_malloc(size);
}
/*
*****************************************************************************
*                            Esh_get_name
*
*  Description:  get one full path name
*
*
*  Parameters:
*                name : input name
*
*  Return value:
*               the full path name pointer
****************************************************************************
*/
char *Esh_get_name(const char *name)
{
    char *tempname;

    if ( !name )
    {
        Esh_Error("Esh_get_name: input argument is NULL!\n");
        return NULL;
    }
    /* get space for name */
    tempname = Esh_alloc_name(ESH_MAX_FILENAME);

    Esh_memset(tempname, '\0', ESH_MAX_FILENAME);
    if ( !tempname )
    {
        Esh_Error("Malloc failed\n");
        Esh_Global->ErrorNum = ESH_ERROR_NO_MEM;
        return NULL;
    }
    /* check is full path or not */
    if ( __Esh_IsFullPath(name) )
    {
        Esh_strcpy(tempname, name);
    }
    else
    {
        Esh_sprintf(tempname, "%s\\%s", Esh_Global->PWdir, name);
    }
    return tempname;
}
/*
*****************************************************************************
*                            Esh_free_name
*
*  Description:  free one full path name
*
*
*  Parameters:
*                name : input name
*
*  Return value: None
*
****************************************************************************
*/
void Esh_free_name(char *name)
{
    Esh_free(name);
}

/* old fat file system name compare */
static int __OldFat_Name_Cmp(const char *name, const char *dname)
{
    int i;
    const char *pt;
    char OldName[14];
    OldName[0] = '\0';
    pt = name;

    for ( i = 0; i < 8; i++ )
    {
        if ( (*pt != '.') && (*pt != '\0') )
        {
            OldName[i] = *pt;
            pt++;
        }
        else
        {
            OldName[i] = ' ';
        }
    }
    OldName[8] = '.';
    if ( *pt == '.' )
        pt++;       // skip .

    for (i = 0; i < 3; i++)
    {
        if ( *pt )
        {
            OldName[9 + i] = *pt;
            pt++;
        }
        else
        {
            OldName[9 + i] = ' ';
        }
    }
    OldName[12] = '\0';
    pt = OldName;
    while ( ( (*pt == *dname) || (*pt == (*dname + 'a' - 'A')) || (*pt == (*dname - 'a' + 'A')) )
            && (*pt != '\0') && (*dname != '\0') )
    {
        pt++;
        dname++;
    }
    if( (*pt == '\0') && (*dname == '\0') )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
static int __Esh_Check_File_Exist(const char *path, const char *name, int is_old_fat)
{
    int res = 0;
    ES_DIR		*dirpt;
    ES_DIRENT   *direntpt;

    /* check created file */
    dirpt = Esh_opendir(path);
    if ( dirpt )
    {
        while((direntpt = Esh_readdir(dirpt)) != NULL)
        {
            if (direntpt)
            {

                /*read dirname for check*/
                if(direntpt->fatdirattr != FSYS_ATTR_DIRECTORY)
                {

                    /* check if old fat file system */
                    if (is_old_fat)
                    {
                        if(__OldFat_Name_Cmp(name, (char *)direntpt->d_name) == 0)
                        {
                            res = 1;
                            break;
                        }
                    }
                    else
                    {
                        if ( Esh_Stricmp(name, (char *)direntpt->d_name) == 0 )
                        {
                            res = 1;
                            break;
                        }
                    }
                }
            }
        }
    }
    else
    {
        Esh_Error("Open directory : %s failed\n", path);
        return 0;
    }
    Esh_closedir(dirpt);
    return res;
}
static int __Esh_check_file_exist(const char *dirname, const char *filename)
{
    int Is_Old_Fat = 0;
    if( (NULL == dirname) || (NULL == filename) )
    {
        Esh_Error("__Esh_check_file_exist: input argument is NULL!\n");
        return 0;
    }
    /* check is old fat file system or not                      ---old type
     * Old fat fs update now, no necessary support old fat fs
     * sunny 2009.6.2
     */
    /*
    if ( ( ('C' == dirname[0]) || ('c' == dirname[0]) ) && (':' == dirname[1]) )
        Is_Old_Fat = 1;
    */
    return __Esh_Check_File_Exist(dirname, filename, Is_Old_Fat);
}
char *Esh_get_app_name(const char *name)
{
    char *AppName;

    if ( !name )
    {
        Esh_Error("Esh_get_app_name: input argument is NULL!\n");
        return NULL;
    }
    /* get space for name */
    AppName = Esh_alloc_name(ESH_MAX_FILENAME);

    Esh_memset(AppName, '\0', ESH_MAX_FILENAME);
    if ( !AppName )
    {
        Esh_Error("Malloc failed\n");
        Esh_Global->ErrorNum = ESH_ERROR_NO_MEM;
        return NULL;
    }
    /* check is full path or not */
    if ( __Esh_IsFullPath(name) )
    {
        Esh_strcpy(AppName, name);
        return AppName;

        /* seach under current work directory */
    }
    else if ( __Esh_check_file_exist(Esh_Global->PWdir, name) )
    {
        Esh_sprintf(AppName, "%s\\%s", Esh_Global->PWdir, name);
        return AppName;

        /* seach under AppPath1 directory */
    }
    else if ( __Esh_check_file_exist(Esh_Global->AppPath1, name) )
    {
        Esh_sprintf(AppName, "%s\\%s", Esh_Global->AppPath1, name);
        return AppName;
    }
    /* not find anywhere */
    Esh_free_name(AppName);
    return NULL;
}
/* 字符串比较，忽略大小写 */
static int __Esh_Mystrcmp(const char *s1, const char *s2, __u32 len)
{
    __u32 count = 0;
    while ( ( (*s1 == *s2) || (*s1 == (*s2 + 'a' - 'A')) || (*s1 == (*s2 - 'a' + 'A')) )
            && (*s1 != '\0') && (*s2 != '\0') && (count < len) )
    {
        s1++;
        s2++;
        count++;
    }
    if( count == len )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
/* fat 文件名分析 */
static void Parse_Fat_Name(char *fatname_str, char *name, char *postfix)
{
    __u32 i;
    __u32 count;
    static char temp_str[ESH_MAX_CMD_LEN];
    static char name_str[ESH_MAX_CMD_LEN];
    static char postfix_str[ESH_MAX_CMD_LEN];
    Esh_memset(temp_str, 0, sizeof(temp_str));
    Esh_memset(name_str, 0, sizeof(name_str));
    Esh_memset(postfix_str, 0, sizeof(postfix_str));
    Esh_strcpy(temp_str, fatname_str);

    //skip the precede ' ' or '\t'
    i = 0;
    while( temp_str[i] == ' ' || temp_str[i] == '\t')
    {
        i++;
    }

    //get name
    count = 0;
    while ( temp_str[i] != ' ' && temp_str[i] != '\t' && temp_str[i] != '\0' )
    {
        count++;
        i++;
    }
    Esh_strncpy(name_str, (temp_str + i - count), count);
    name_str[count] = '\0';

    //skip the middle ' ' or '\t' and set temp_str at '.'
    while( temp_str[i] == ' ' || temp_str[i] == '\t')
    {
        i++;
    }

    //get postfix
    count = 0;
    while( temp_str[i] != ' ' && temp_str[i] != '\t' && temp_str[i] != '\0' )
    {
        count++;
        i++;
    }
    Esh_strncpy(postfix_str, (temp_str + i - count), count);
    postfix_str[count] = '\0';

    Esh_strcpy(name, name_str);
    Esh_strcpy(postfix, postfix_str);
}
/*
*****************************************************************************
*                            compensate_name
*
*  Description:compensate name full under current directory name
*
*
*  Parameters: fix_str: some front charaters of a full name
*              pwd:     current work directory
*              cps_len: add compensated name length
*              valid_len:string fix_str valid length,cut front and last space' '
*
*
*  Return value:None
*****************************************************************************
*/
void frt_compensate_name(char *cmd_str, char *pwdir, char *cps_str, __u32 *cps_len, __u32 *valid_len)
{
    __u32 total_len = 0;
    __u32 cmp_len = 0;
    __u32 space_location = 0;
    static char name_str[ESH_MAX_CMD_LEN];
    static char postfix_str[ESH_MAX_CMD_LEN];
    static char temp_str[0x60];
    ES_DIR *dirp;
    ES_DIRENT *direntp;
    Esh_memset(temp_str, 0, 0x60);
    Esh_memset(name_str, 0, sizeof(name_str));
    Esh_memset(postfix_str, 0, sizeof(postfix_str));
    Esh_strcpy(temp_str, cmd_str);

    /* get end location of fix_str,jump ' ' or '\t' first*/
    total_len = Esh_strlen(temp_str);
    while( (temp_str[total_len - 1] == ' ') && (temp_str[total_len - 1] == '\t') )
    {
        total_len--;
    }
    total_len = total_len - (*cps_len);           /*subtract last time add string length*/
    temp_str[total_len] = '\0';

    /*sreach ' ' and '\t' backwards*/
    space_location = total_len;
    while( (temp_str[space_location - 1] != ' ') && (temp_str[space_location - 1] != '\t') )
    {
        space_location--;
    }
    Esh_strcpy( (temp_str + 0X30), (temp_str + space_location));
    cmp_len = total_len - space_location;
    /*compare string*/
    dirp = Esh_opendir(pwdir);
    if (dirp)
    {

        /*get last compensate location*/
        do
        {
            if ( *cps_len )                   /*check the fist time compensate*/
            {
                direntp = Esh_readdir(dirp);
                if ( direntp )
                {
                    Parse_Fat_Name((char *)direntp->d_name, (char *)name_str, (char *)postfix_str);
                    if ( postfix_str && !(direntp->fatdirattr & FSYS_ATTR_DIRECTORY) )
                    {
                        Esh_strcat(name_str, postfix_str);
                    }
                    total_len = Esh_strlen(name_str);
                    if (__Esh_Mystrcmp(name_str, cps_str, total_len) == 0)
                    {
                        break;
                    }
                }
            }
            else
            {
                break;
            }
        }
        while (direntp);

        /*get new compensate d_name*/
        do
        {
            direntp = Esh_readdir(dirp);
            if (direntp)
            {

                /*pasre the FAT name string*/
                Parse_Fat_Name((char *)direntp->d_name, (char *)name_str, (char *)postfix_str);
                if ( postfix_str && !(direntp->fatdirattr & FSYS_ATTR_DIRECTORY) )
                {
                    Esh_strcat(name_str, postfix_str);
                }

                /*get new length*/
                total_len = Esh_strlen(name_str);
                if ( (__Esh_Mystrcmp( (char *)name_str, (temp_str + 0x30), cmp_len) == 0) )
                {
                    if (__Esh_Mystrcmp(name_str, cps_str, total_len) != 0)
                    {
                        Esh_strncpy(cps_str, name_str, total_len);        /*get new cps_str*/
                        cps_str[total_len] = '\0';
                        Esh_strcpy((temp_str + space_location), cps_str);
                        Esh_strcpy(cmd_str, temp_str);
                        *valid_len = (total_len + space_location);
                        *cps_len = (total_len - cmp_len);
                        break;
                    }
                }
            }
            else          //the end of the directory
            {
                Esh_strcpy((temp_str + space_location), (temp_str + 0x30));
                Esh_strcpy(cmd_str, temp_str);
                Esh_memset(cps_str, 0, sizeof(cps_str));
                *valid_len = space_location + cmp_len;
                *cps_len = 0;
            }
        }
        while (direntp);
        if ( Esh_closedir(dirp) )
        {
            return;
        }
    }
    else
    {
        return;
    }
}
/*
*****************************************************************************
*                            Esh_Seach_Executable_Program
*
*  Description:  Seach Executable Program or script file name
*
*
*  Parameters:
*
*
*  Return value:
*****************************************************************************
*/
char *Esh_Seach_Executable_Program(const char *name)
{
    char *AppName;

    if ( !name )
    {
        Esh_Error("input argument is NULL!\n");
        return NULL;
    }
    /* get space for name */
    AppName = Esh_alloc_name(ESH_MAX_FILENAME);

    Esh_memset(AppName, '\0', ESH_MAX_FILENAME);

    if ( !AppName )
    {
        Esh_Error("Malloc failed\n");
        Esh_Global->ErrorNum = ESH_ERROR_NO_MEM;
        return NULL;
    }
    /* check is full path or not */
    if ( __Esh_IsFullPath(name) )
    {
        Esh_strcpy(AppName, name);
        return AppName;
    }
    /* seach under current work directory */
    if ( __Esh_check_file_exist(Esh_Global->PWdir, name) )
    {
        Esh_sprintf(AppName, "%s\\%s", Esh_Global->PWdir, name);
        return AppName;
    }
    /* seach under AppPath1 directory */
    if ( __Esh_check_file_exist(Esh_Global->AppPath1, name) )
    {
        Esh_sprintf(AppName, "%s\\%s", Esh_Global->AppPath1, name);
        return AppName;
    }
    /* not find anywhere */
    return NULL;
}
