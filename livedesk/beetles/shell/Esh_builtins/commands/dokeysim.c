/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : dokeysim.c
* By      : Steven
* Version : V1.00
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                              __exec_keysim
*
* Description:
*
*
*  Parameters:None
*
*
*  Return value:None
*****************************************************************************
*/
static void __exec_keysim(void)
{
    __u32 key;
    ES_FILE *pKeyFile;
    pKeyFile = eLIBs_fopen("b:\\HID\\KEY", "w");
    if(!pKeyFile)
    {
        __inf("key connot be openned!\n");
    }
    else
    {
        __inf("put '`' to exit keysim mode!\n");
        while(1)
        {
            key = __getc();
            if(key == '`')
                break;
            if(key == '~')
            {
                key = __getc();
                switch(key)
                {
                case 'U':
                    key = GUI_KEY_UP;
                    break;
                case 'D':
                    key = GUI_KEY_DOWN;
                    break;
                case 'R':
                    key = GUI_KEY_RIGHT;
                    break;
                case 'L':
                    key = GUI_KEY_LEFT;
                    break;
                default:
                    break;
                }
            }
            __inf("%d", key);
            __inf("\n");
            eLIBs_fioctrl(pKeyFile, DRV_KEY_CMD_PUTKEY, key, 0);
        }
    }
    eLIBs_fclose(pKeyFile);
}
/*
*****************************************************************************
*                            dokeysim
*
*  Description: set key
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int dokeysim(struct op *t , const char **args)
{

    /* do keysim core: write by Steven */
    __exec_keysim();

    /* no any error */
    return DOSUCCEED;
}
