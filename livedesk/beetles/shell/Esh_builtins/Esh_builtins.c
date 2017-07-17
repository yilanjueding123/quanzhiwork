/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*
*
* File    : Esh_builtin.c
* By      : sunny
* Version : V1.00                                                                       2009.3.2
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/* builtin commands table */
static const struct builtincmd Commands_Table[] =
{

    /* Eshell execute commands */
    { "cat"             , docat             },
    { "cd"              , docd              },
    { "copy"            , docopy            },
    { "create"          , docreate          },
    { "del"             , dodel             },
    { "dir"             , dodir             },
    { "fs"              , dofs              },
    { "help"            , dohelp            },
    { "history"         , dohistory         },
    { "insmod"          , doinsmod          },
    { "keysim"          , dokeysim          },
    { "poweroff"        , dopoweroff        },
    { "mount"           , domount           },
    { "mkdir"           , domkdir           },
    { "reset"           , doreset           },
    { "rmdir"           , dormdir           },
    { "startx"          , dostartx          },
    { "sysinfo"         , dosysinfo         },
    { "run"             , dorun             },
    { "setpath"         , dosetpath         },

    /* script execute */
    { "src"             , doscript          },

    /* script support commands */
    { "test"            , dotest            },
    { "echo"            , doecho            },
    { "break"           , dobreak           },
    //  { "export"          , doexport          },
    { "readonly"        , doreadonly        },
    { "expr"            , doexpr            },
    { "var"             , dovar             },
    { "set"             , doset             },
    { "putv"            , doputv            },

    /* test support */
    { "clear"           , doclear           },
    { "fastclear"       , dofastclear       },
    { "RandomGen"       , dorandom          },
    { "GetPartFreeSize" , dogetpartsize     },

    { "sendkey"         , dosendkey         },
    { "wait"            , dowait            },
    { "quit"            , doquit            },

    { "assert"          , doassert          },
    { "xcopy"           , doxcopy           },

    { "formatcheck"     , doformatcheck     },
    { "format"          , doformat          },
    { "showdsize"       , doshowdsize       },
    { "showfsize"       , doshowfsize       },
    { "configfs"        , doconfigfs        },

    { "cpustatstart"    , startcpustat      },
    { "cpustatstop"     , stopcpustat       },
    { "cpustatreport"   , reportcpustat     },
    { "de"              , dode              },

    { "meminfo"         , domeminfo         },

    { "update"          , doupdate          },
    { "regwr"          , doregwr          },
    { "regrd"          , doregrd          },
    { "dump"          , doregdump          },
    { NULL              , NULL              },
};
/*
*********************************************************************************
*                                   Esh_inbuilt()
* Description: find one command execute function
*
*
* Parameters:
*               name: name of command
*
*
* Returns    :  Comand execute function pointer except
*
*********************************************************************************
*/
builtin_func_ptr Esh_inbuilt(const char *s)
{
    const struct builtincmd *bp;

    /* first seach command table */
    for (bp = Commands_Table; bp->name; bp++)
    {
        if ( 0 == Esh_Stricmp(bp->name, s) )
        {
            return bp->builtinfunc;
        }
    }
    return NULL;
}
