/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : doxcopy.c
* By      : sunny
* Version : V2.00                                                                         2009.6.30
*********************************************************************************************************
*/
#include    "Esh_shell_private.h"

int doregrd(struct op *t, const char **args)
{
    int reg_addr  ;
    const char  *err = NULL;
    /* check input arguments */
    if ( NULL == args[1] )
    {
        Esh_Error("invalid argument!\n");
        return -1;
    }
    //eLIBs_printf("arg[1]=%s\n",args[1]);

    reg_addr = SLIB_strtol(args[1], &err, 0);

    //eLIBs_printf("reg_addr=%x \n",reg_addr);
    if(reg_addr < 0xf0000000)
    {
        reg_addr |= 0xf0000000 ;
    }
    eLIBs_printf("reg_addr[%x]=%x \n", reg_addr, *((volatile __u32 *)(reg_addr)));
    return 0;
}


int doregdump(struct op *t, const char **args)
{
    int reg_addr , reg_addr1 , tmp_addr;
    int reg_count = 0;
    int i = 0;
    const char  *err = NULL;
    /* check input arguments */
    if ( NULL == args[1] )
    {
        Esh_Error("invalid argument!\n");
        return -1;
    }
    //eLIBs_printf("arg[1]=%s\n",args[1]);

    reg_addr = SLIB_strtol(args[1], &err, 0);
    reg_addr1 = SLIB_strtol(args[2], &err, 0);
    if(reg_addr > reg_addr1)
    {
        tmp_addr = reg_addr;
        reg_addr = reg_addr1;
        reg_addr1 = tmp_addr;
    }
    reg_count = (reg_addr1 - reg_addr) / 4 + 1;

    //eLIBs_printf("reg_addr=%x \n",reg_addr);
    if(reg_addr < 0xf0000000)
    {
        reg_addr |= 0xf0000000 ;
    }
    if(reg_addr1 < 0xf0000000)
    {
        reg_addr1 |= 0xf0000000 ;
    }
    if(reg_count > 0)
    {
        for(i = 0; i < reg_count;)
        {
            if(i % 4 == 0)
            {
                eLIBs_printf("reg_addr[%8x]: %8x ", reg_addr + i * 0x4, *((volatile __u32 *)(reg_addr + i * 0x4)));
            }
            else
            {
                eLIBs_printf("%8x ", *((volatile __u32 *)(reg_addr + i * 0x4)));
            }

            i++;
            if(i % 4 == 0)
            {
                eLIBs_printf("\n");
            }
        }
        eLIBs_printf("\n");
    }
    else
    {
        eLIBs_printf("reg_addr[%8x]=%8x \n", reg_addr, *((volatile __u32 *)(reg_addr)));
    }


    return 0;
}



#include    "Esh_shell_private.h"
