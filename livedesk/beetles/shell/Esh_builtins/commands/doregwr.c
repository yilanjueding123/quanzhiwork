/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : doregwr.c
* By      : sunny
* Version : V2.00                                                                         2009.6.30
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

int doregwr(struct op *t, const char **args)
{
    int reg_addr , reg_value ;
    const char  *err = NULL;
    /* check input arguments */
    if ( (NULL == args[1]) || (NULL == args[2]) )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    //eLIBs_printf("arg[1]=%s arg[2]=%s\n",args[1],args[2]);

    reg_addr = SLIB_strtol(args[1], &err, 0);
    reg_value = SLIB_strtol(args[2], &err, 0);

    //eLIBs_printf("reg_addr=%x reg_value=%x\n",reg_addr,reg_value);
    if(reg_addr < 0xf0000000)
    {
        reg_addr |= 0xf0000000 ;
    }
    *((volatile unsigned int *)(reg_addr)) = reg_value;
    return DOSUCCEED;
}
