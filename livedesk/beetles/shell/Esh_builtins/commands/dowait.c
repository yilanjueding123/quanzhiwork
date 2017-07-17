/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : dowait.c
* By      : Steven
* Version : V1.00
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                            dowait
*
*  Description: waiting specified time slot
*
*
*  Parameters:
*
*  Return value:None
*****************************************************************************
*/
int dowait(struct op *t , const char **args)
{
    __u32 TimeSlot = 0;
    int UnSilent = 1;

    /* check arguments */
    if ( NULL == args[1] )
    {
        Esh_Error("invalid argument!\n");
        return DOFAILED;
    }
    /* get time slot */
    TimeSlot = Esh_atou(args[1]);

    if ( args[2] )
    {
        if ( Esh_strcmp(args[2], "/s") || Esh_strcmp(args[2], "/S") )
        {
            UnSilent = 0;
        }
    }
    if ( UnSilent )
    {
        Esh_printf("Wait %d ms.............\n", TimeSlot);
    }

    /* call system delay : unit(10 ms) */
    TimeSlot /= 10;
    esKRNL_TimeDly(TimeSlot);

    /* no any error */
    return DOSUCCEED;
}
