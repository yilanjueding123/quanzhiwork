/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : dosendkey.c
* By      : Steven
* Version : V1.00
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

struct EshStringKeyMap
{
    char   *KeyName;
    __u32   KeyValue;
};

struct EshStringKeyMap EshStringKeyMapTable[] =
{
    {   "KEY_ENTER",            KMSG_USR_KEY_ENTER       },
    {   "KEY_LEFT",             KMSG_USR_KEY_LEFT        },
    {   "KEY_RIGHT",            KMSG_USR_KEY_RIGHT       },
    {   "KEY_UP",               KMSG_USR_KEY_UP          },
    {   "KEY_DOWN",             KMSG_USR_KEY_DOWN        },
    {   "KEY_VOICE_UP",         KMSG_USR_KEY_VOICE_UP    },
    {   "KEY_VOICE_DOWN",       KMSG_USR_KEY_VOICE_DOWN  },
    {   "KEY_POWEROFF",         KMSG_USR_KEY_POWEROFF    },
    {   "KEY_RST",              KMSG_USR_KEY_RST         },
    {   NULL,                   0                        },
};

__u32 _Esh_GetKeyValue(const char *keytype)
{
    struct EshStringKeyMap *pt;

    for (pt = EshStringKeyMapTable; pt->KeyName; pt++)
    {
        if ( !Esh_Stricmp(keytype, pt->KeyName) )
        {
            return pt->KeyValue;
        }
    }
    Esh_Error("Unkown Key type:%s\n", keytype);
    return 0;
}

int __Esh_SendKey_To_KeyDriver(__u32 key_value)
{
    ES_FILE *pKeyFile;

    /* open key driver file */
    pKeyFile = Esh_fopen("b:\\HID\\KEY", "w");

    if( NULL == pKeyFile )
    {
        Esh_Error("key driver file connot be openned!\n");
        return DOFAILED;
    }
    Esh_fioctrl(pKeyFile, DRV_KEY_CMD_PUTKEY, NULL, (void *)(key_value));

    Esh_fclose(pKeyFile);

    /* delay some time */
    esKRNL_TimeDly(200);
    return DOSUCCEED;
}
/*
*****************************************************************************
*                            dosendkey
*
*  Description: send one key_value to "KeyDriver"
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int dosendkey(struct op *t , const char **args)
{
    __u32 KeyValue;
    int UnSilent = 1;

    /* check arguments */
    if ( NULL == args[1] )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    /* get keyvalue */
    if ( Esh_isdigit(args[1][0]) )
    {
        KeyValue = Esh_atou(args[1]);
    }
    else
    {
        KeyValue = _Esh_GetKeyValue(args[1]);
    }
    if ( args[2] )
    {
        if ( Esh_strcmp(args[2], "/s") || Esh_strcmp(args[2], "/S") )
        {
            UnSilent = 0;
        }
    }
    if ( UnSilent )
    {
        Esh_printf("Send key: \"%u\" to KeyDriver\n", KeyValue);
    }
    /* send keyvalue to "KeyDriver" */
    return __Esh_SendKey_To_KeyDriver(KeyValue);
}
