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
* File    : Esh_script_domisc.c
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#include    "doscript_private.h"

/* just for get partition  free size command */

/* static function */
//static void rdexp(const char **wp, void (*f) (struct var *), int key);
//static void badid(const char *s);
static int  OpLookup (const char *op);

int IsBreakOut;   //control break command extenal var

int doecho(struct op *t , const char **args)
{
    /* input arguments check */
    if ( NULL == args[1] )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    Esh_printf("%s\n", args[1]);
    return DOSUCCEED;  //do succeed
}
/*
 * set IsBreakOut to 1
 */
int dobreak(struct op *t , const char **args)
{
    IsBreakOut = 1;
    return DOSUCCEED;  //do succeed
}

int doexport(struct op *t , const char **args)
{
    //rdexp(args + 1, export, EXPORT);
    return DOSUCCEED;
}

int doreadonly(struct op *t , const char **args)
{
    //rdexp(args + 1, ronly, RONLY);
    return DOSUCCEED;
}
//
//static void rdexp(const char **wp, void (*f) (struct var *), int key)
//{
//	if (*wp != NULL) {
//		for (; *wp != NULL; wp++) {
//			if (isassign(*wp)) {
//				const char *cp;
//
//				assign(*wp, COPYV);
//				for (cp = *wp; *cp != '='; cp++)
//					continue;
//				*cp = '\0';
//			}
//			if (checkname(*wp)) {
//				(*f) (lookup(*wp));
//			} else {
//				badid(*wp);
//			}
//		}
//	}
////	} else
////		putvlist(key, 1);
//}

//static void badid(const char *s)
//{
//	Esh_prs(s);
//	Esh_Error(": bad identifier");
//}

/*
 * Op_Type Value find
 */
static int OpLookup (const char *op)
{
    /* table list of Op_Type */
    struct OpType OpType_List[] =
    {
        { "+"    , OP_TYPE_ADD           },
        { "-"    , OP_TYPE_SUB           },
        { "*"    , OP_TYPE_MUL           },
        { "/"    , OP_TYPE_DIV           },
        //        { "%"    , OP_TYPE_MOD           },
        { "++"   , OP_TYPE_INC           },
        { "--"   , OP_TYPE_DEC           },
        //        { "&&"   , OP_TYPE_LOGICAL_AND   },
        //        { "||"   , OP_TYPE_LOGICAL_OR    },
        //        { "!"    , OP_TYPE_LOGICAL_NOT   },
        { "=="   , OP_TYPE_EQUAL         },
        { "!="   , OP_TYPE_NOT_EQUAL     },
        { "<"    , OP_TYPE_LESS          },
        { ">"    , OP_TYPE_GREATER       },
        { "<="   , OP_TYPE_LESS_EQUAL    },
        { ">="   , OP_TYPE_GREATER_EQUAL },
    };

    struct OpType *Op_list;

    /* Find Op_type value */
    for (Op_list = OpType_List; Op_list->OpType_name[0]; Op_list++)
    {
        if (strcmp(Op_list->OpType_name, op) == 0)
        {
            return Op_list->OpType_val;	  //Return Op_Type Value
        }
    }
    return 0;					      //Not Op_Type
}
/*
 * simple arithmetic operation
 */
int doexpr(struct op *t , const char **args)
{
    /* Get Oprate var value */
    int Op_type;
    int Op_Result;
    int L_operand = 0;
    int R_operand = 0;
    char Str_Reslut[12];
    struct var *vp;

    const char **cp;
    int arg_num = 0;
    cp = args;

    /* input arguments check */
    if ( (NULL == args[1]) || (NULL == args[2]) || (NULL == args[3]) )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    /* get args number */
    while ( *cp )
    {
        arg_num++;
        cp++;
    }
    /* get Operation type */
    Op_type = OpLookup(args[2]);

    /* convert input string to int */
    L_operand = Esh_atoi(args[1]);
    if ( arg_num > 4 )
        R_operand = Esh_atoi(args[3]);

    /* operation */
    switch ( Op_type )
    {

    case OP_TYPE_ADD:    // +
        Op_Result = L_operand + R_operand;
        break;

    case OP_TYPE_SUB:    // -
        Op_Result = L_operand - R_operand;
        break;

    case OP_TYPE_INC:    // ++
        Op_Result = L_operand + 1;
        break;

    case OP_TYPE_DEC:    // --
        Op_Result = L_operand - 1;
        break;

    case OP_TYPE_MUL:    // *
        Op_Result = L_operand * R_operand;
        break;

    default:
        Op_Result = 0;
        break;
    }
    /* save the operation result to */
    Esh_itoa(Op_Result, Str_Reslut);    // convert int to string
    if ( args[arg_num - 1] )
    {
        vp = lookup(args[arg_num - 1]);             // Search var
        nameval(vp, Str_Reslut, NONAME);            // Save Result to var args[4]
    }
    return DOSUCCEED;
}

/*
 * Relational operation
 */
int dotest(struct op *t , const char **args)
{
    /* Get Oprate var value */
    int Op_type;
    __int64 L_operand;
    __int64 R_operand;
    int Op_Result;

    /* input arguments check */
    if ( (NULL == args[1]) || (NULL == args[2]) || (NULL == args[3]) )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    /* first check parameters of test command : args[1] and args[3] must digit value */
    if ( !Esh_Is_Digit_String(args[1]) || !Esh_Is_Digit_String(args[3]) )
    {
        Esh_Error("Parameters for test command, parameter[1] and parameter[3] must be digit value\n");
        return DOFAILED;
    }
    /* get Operation type */
    Op_type = OpLookup(args[2]);

    /* convert input string to long long type */
    L_operand = Esh_atoll(args[1]);
    R_operand = Esh_atoll(args[3]);

    /* Relational operation */
    switch ( Op_type )
    {
    case OP_TYPE_EQUAL:            // ==
        Op_Result = (L_operand == R_operand);
        break;

    case OP_TYPE_NOT_EQUAL:        // !=
        Op_Result = ( L_operand != R_operand );
        break;

    case OP_TYPE_LESS:             // <
        Op_Result = ( L_operand < R_operand );
        break;

    case OP_TYPE_GREATER:         // >
        Op_Result = ( L_operand > R_operand );
        break;

    case OP_TYPE_LESS_EQUAL:      // <=
        Op_Result = ( L_operand <= R_operand );
        break;

    case OP_TYPE_GREATER_EQUAL:   // >=
        Op_Result = ( L_operand >= R_operand );
        break;

    default:
        Op_Result = 1;           // default is succeessful
        break;
    }
    return (Op_Result == 1) ? 0 : 1; // 0 : DO SUCCEED
}
/*
 * var define
 */
int dovar(struct op *t , const char **args)
{
    struct var *vp;
    if ( NULL == args[1] )
    {
        printf("invalid argument!\n");
        return DOSUCCEED;
    }
    vp = lookup(args[1]);
    if ( args[2] && (*args[2] == '=') )      // set value of var
    {
        nameval(vp, args[3], NONAME);
    }
    return DOSUCCEED;
}
/*
 * set var value
 */
int doset(struct op *t , const char **args)
{
    struct var *vp;
    if ( (NULL == args[1]) || (NULL == args[2]) )
    {
        printf("invalid argument!\n");
        return DOSUCCEED;
    }
    vp = lookup(args[1]);
    if ( vp && args[2] )   // set value
    {
        nameval(vp, args[2], NONAME);
    }
    return DOSUCCEED;
}
/*
 * put out var list
 */
int doputv(struct op *t , const char **args)
{
    /* put out var list */
    putvlist();
    return DOSUCCEED;
}
static int Is_Dot_DotDot(const char *name)
{
    char Dot[] = ".";
    char DotDot[] = "..";
    if (strcmp(name, Dot) && strcmp(name, DotDot))
    {
        return 0;
    }
    return 1;
}
/*
 * Check one directroy is empty or not
 */
static int Is_Empty_Dir(const char *dir)
{
    ES_DIR		*dirpt;
    ES_DIRENT   *direntpt;
    if ( dir == NULL )
    {
        printf("Error: directory name is null\n");
        return -1;
    }
    /* make the point of read to start */
    dirpt = eLIBs_opendir(dir);
    if ( dirpt == NULL )
    {
        printf("Error: open directory:\"%s\" failed", dir);
        return -1;
    }
    while((direntpt = readdir(dirpt)) != NULL)
    {
        if ( direntpt->fatdirattr != FSYS_ATTR_DIRECTORY )
        {

            /* find file under directory */
            goto NotEmpty;
        }
        /* check subdirectory */
        if ( !Is_Dot_DotDot((char *)direntpt->d_name) )
        {
            goto NotEmpty;
        }
    }
    /* not find any subdirectory expect dot and dotdot */
    eLIBs_closedir(dirpt);
    return 1;

NotEmpty:
    eLIBs_closedir(dirpt);
    return 0;
}
/*
 * Delete some leaves of one subtree
 */
static int Delete_Subtree_Leaves(const char *dir, char *pathbuf, char *namebuf, int unsilent)
{
    int res;
    int count = 0;
    ES_DIR		*dirpt;
    ES_DIRENT   *direntpt;
    char *__path;
    strcpy(pathbuf, dir);
    __path = pathbuf;
    dirpt = NULL;

Try_Again:
    if ( dirpt != NULL )
        eLIBs_closedir(dirpt);

    /* check is empty or not */
    res = Is_Empty_Dir(pathbuf);
    if (res == -1)
    {
        return -1;
    }
    if ( res == 1 )
    {
        if ( !eLIBs_rmdir(pathbuf) )
        {
            if ( unsilent )
                printf("Del ditectory: \"%s\".....\n", pathbuf);
            count++;
            return count;
        }
        printf("Error: del directory: \"%s\" failed\n", pathbuf);
        return count;
    }
    /* open directory */
    dirpt = eLIBs_opendir(pathbuf);
    if ( dirpt == NULL )
    {
        printf("Error: open directory : \"%s\" failed\n", pathbuf);
        return 0;
    }
    while((direntpt = readdir(dirpt)) != NULL)
    {

        /* delete file directly */
        if ( direntpt->fatdirattr != FSYS_ATTR_DIRECTORY )
        {

            /* delete file encount */
            Esh_sprintf(namebuf, "%s\\%s", pathbuf, (char *)direntpt->d_name);
            if ( !eLIBs_remove(namebuf) )
            {
                if ( unsilent )
                    printf("Del file: \"%s\"\n", namebuf);
                count++;
            }
            continue;
        }
        /* skip "." and ".." */
        if ( Is_Dot_DotDot((char *)direntpt->d_name) )
            continue;

        /* deal with directory */
        __path += strlen(pathbuf);
        //Dispather_sprintf(__path, "\\%s", (char *)direntpt->d_name);
        strcat(pathbuf, "\\");
        strcat(pathbuf, (char *)direntpt->d_name);
        goto Try_Again;
    }
    /* can't remove "dir" */
    if ( strcmp(pathbuf, dir) == 0 )
    {
        if ( dirpt != NULL )
            eLIBs_closedir(dirpt);
        return count;
    }
    goto Try_Again;
}
/*
 * clear subdirectories of one directroy
 */
int doclear(struct op *t , const char **args)
{
    int i = 200;
    int res;
    int count = 0;
    int unsilent = 0;
    ES_DIR *dirpt;
    static char pathbuf[264];
    static char namebuf[264];

    /* check argument */
    if ( args[1] == NULL )
    {
        printf("invalid argument!\n");
        return DOSUCCEED;
    }
    if ( args[2] )
    {
        if ( strcmp(args[2], "unsilent") == 0 )
            unsilent = 1;
    }
    /* check directory can open or not */
    dirpt = eLIBs_opendir(args[1]);
    if ( dirpt == NULL )
    {
        printf("Error: open directory:\"%s\" failed\n", args[1]);
        return DOSUCCEED;
    }
    eLIBs_closedir(dirpt);

    printf("Delete files and directories under \"%s\" begin.......\n", args[1]);
    /* delete sundirectories */
    while ( !Is_Empty_Dir(args[1]) )
    {

        /* delete some leaves of one subtree */
        res = Delete_Subtree_Leaves(args[1], pathbuf, namebuf, unsilent);
        if ( res <= 0 )
        {
            printf("Delete document \"%s\" failed one time\n", args[1]);
            return DOSUCCEED;
        }
        count += res;

        /* deleting process indicater */
        if ( count > i )
        {
            printf("||>>>>>>>>||");
            if ( !(i % 1000) )
                printf("\n");
            i += 200;
        }
    }
    printf("....Delete: \"%u\" files and directories under:\"%s\"\n", count, args[1]);
    return DOSUCCEED;
}
static int __dofastclear(const char *dir, char *pathbuf, char *namebuf, int unsilent)
{
    int count = 0;
    ES_DIR		*dirpt;
    ES_DIRENT   *direntpt;
    char *path_pos = &pathbuf[strlen(pathbuf)];

    dirpt = opendir(pathbuf);
    if ( dirpt == NULL )
    {
        printf("Error: open directory : \"%s\" failed\n", pathbuf);
        return 0;
    }
    while((direntpt = readdir(dirpt)) != NULL)
    {

        /* delete file directly */
        if ( direntpt->fatdirattr != FSYS_ATTR_DIRECTORY )
        {

            /* delete file encount */
            Esh_sprintf(namebuf, "%s\\%s", pathbuf, (char *)direntpt->d_name);
            if ( !remove(namebuf) )
            {
                if ( unsilent )
                    printf("Del file: \"%s\"\n", namebuf);
                count++;
            }
            continue;
        }
        /* skip "." and ".." */
        if ( Is_Dot_DotDot((char *)direntpt->d_name) )
            continue;

        /* deal with directory */
        Esh_sprintf(namebuf, "%s\\%s", pathbuf, (char *)direntpt->d_name);
        if ( Is_Empty_Dir(namebuf) )
        {
            if ( !rmdir(namebuf) )
            {
                if ( unsilent )
                    printf("Del ditectory: \"%s\".....\n", namebuf);
                count++;
            }
            continue;
        }
        if ( !Is_Empty_Dir(dir) )
        {
            closedir(dirpt);
            strcat(pathbuf, "\\");
            strcat(pathbuf, (char *)direntpt->d_name);
            //Esh_sprintf(path_pos, "\\%s", (char *)direntpt->d_name);
            __dofastclear(dir, pathbuf, namebuf, unsilent);
        }
    }
    closedir(dirpt);
    return count;
}
/*
 * clear one document
 */
int dofastclear(struct op *t , const char **args)
{
    int count = 0;
    int unsilent = 0;
    ES_DIR *dirpt;
    static char pathbuf[264];
    static char namebuf[264];

    /* check argument */
    if ( args[1] == NULL )
    {
        printf("invalid argument!\n");
        return DOSUCCEED;
    }
    if ( args[2] )
    {
        if ( strcmp(args[2], "unsilent") == 0 )
            unsilent = 1;
    }
    /* check directory can open or not */
    dirpt = opendir(args[1]);
    if ( dirpt == NULL )
    {
        printf("Error: open directory:\"%s\" failed\n", args[1]);
        return DOSUCCEED;
    }
    closedir(dirpt);

    printf("Clear files and directories under \"%s\" begin.......\n", args[1]);
    strcpy(pathbuf, args[1]);
    count = __dofastclear(args[1], pathbuf, namebuf, unsilent);
    printf("....Delete: \"%u\" files and directories under:\"%s\"\n", count, args[1]);
    return DOSUCCEED;
}
/*
 * random data generater
 */
int dorandom(struct op *t , const char **args)
{
    int Max;
    int RandomSeed;
    char RandomStr[12];
    struct var *vp;

    if ( !args[1] )
    {
        printf("Error: parameter 1 for random command invalid\n");
        return -1;
    }
    Max = Esh_atoi(args[1]);

    /* random generate */
    RandomSeed = eLIBs_ramdom(Max);
    Esh_itoa(RandomSeed, RandomStr);

    /* set random value to given varible */
    if ( !args[2] )
    {
        printf("Error: parameter 2 for random command invalid\n");
        return -1;
    }
    vp = lookup(args[2]);
    if ( vp && args[2] )   // set value
    {
        nameval(vp, RandomStr, NONAME);
    }
    return DOSUCCEED;
}
/*
 * get the size of specified partiton
 */

int dogetpartsize(struct op *t , const char **args)
{
    __u64 FreeSize = 0;                         /* sizeof( long long ) = 64 bits */
    __u32 Low32 = 0;
    __u32 High32 = 0;
    char LowStr[12];
    char HighStr[12];
    char FreeStr[24];
    struct var *vp;

    /* get partition free size */
    if ( !args[1] )
    {
        printf("Error: parameter 1 for getpartsize command invalid\n");
        return -1;
    }
    FreeSize = eLIBs_GetVolFSpace(args[1]);

    Low32 = FreeSize & 0x00000000FFFFFFFF;
    High32 = FreeSize >> 32;

    if ( High32 )
    {
        Esh_itoa(High32, HighStr);
        strcpy(FreeStr, HighStr);
    }
    else
    {
        FreeStr[0] = '\0';
    }
    Esh_itoa(Low32, LowStr);
    strcat(FreeStr, LowStr);

    /* set partition free size value to given varible */
    if ( !args[2] )
    {
        printf("Error: parameter 2 for getpartsize command invalid\n");
        return -1;
    }
    vp = lookup(args[2]);
    if ( vp && args[2] )
    {

        /* set string value to varible 'var' */
        nameval(vp, FreeStr, NONAME);
    }
    return DOSUCCEED;
}
