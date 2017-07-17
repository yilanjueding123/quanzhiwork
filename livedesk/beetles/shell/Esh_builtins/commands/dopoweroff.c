/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : dopoweroff.c
* By      : sunny
* Version : V2.00                                                                         2009.3.3
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

static void __exec_poweroff(void)
{
    //do some thing here
    {
        ES_FILE *fp;
        fp = eLIBs_fopen("b:\\HID\\POWER", "w");
        if(!fp)
        {
            __inf("power control connot be openned!\n");
        }
        //eLIBs_fioctrl(fp,
        //eLIBs_fioctrl(fp, DRV_POWER_OFF, 0, 0);
        eLIBs_fclose(fp);
    }
    //esKSRV_PowerOff();
    __inf("Sorry 'PowerOff' system not realized\n");
}

/*
*****************************************************************************
*                            dopoweroff
*
*  Description: Poweroff system
*
*
*  Parameters: None
*
*
*  Return value:None
*****************************************************************************
*/
int dopoweroff(struct op *t , const char **args)
{
    /* call system poweroff */
    Esh_printf("Poweroff system....................................................\n");

    __exec_poweroff();

    return DOSUCCEED;
}
