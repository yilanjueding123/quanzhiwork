/*
********************************************************************************************************************
*
*
*                              (c) Copyright 2007-2010, javen.China
*										All	Rights Reserved
*
* File Name 	: memory_allot.c
*
* Author 		: javen
*
* Version 		: 2.0
*
* Date 			: 2010.03.02
*
* Description 	:
*
* History 		:
*
********************************************************************************************************************
*/
#include "usb_host_config.h"
#include "usb_os_platform.h"
#include "usb_host_base_types.h"

#include "usb_list.h"
#include "list_head_ext.h"

#include  "horse.h"
#include  "memory_allot.h"

static void *memory_base = NULL;

typedef union memory_block
{
    struct list_head  list;
} memory_block_t;


/*
*******************************************************************************
*                     memory_malloc
*
* Description:
*
*
* Parameters:
*
* Return value:
*
* note:
*
*******************************************************************************
*/
void *memory_malloc(__u32 size, u8 *file_name , u32 line_nr)
{
    void *addr = NULL;

    addr = HorseBlockMalloc(memory_base);
    if(addr == NULL)
    {
        DMSG_INFO("HorseBlockMalloc failed, must USB_OS_MALLOC\n");
        addr = USB_OS_MALLOC(size, file_name, line_nr);
    }

    return addr;
}

/*
*******************************************************************************
*                     memory_free
*
* Description:
*
*
* Parameters:
*
* Return value:
*
* note:
*
*******************************************************************************
*/
void memory_free(void *addr, u8 *file_name , u32 line_nr)
{
    if(HorseBlockFree(memory_base, addr) != 0)
    {
        USB_OS_FREE(addr);
    }
}

/*
*******************************************************************************
*                     memory_allot_init
*
* Description:
*
*
* Parameters:
*
* Return value:
*
* note:
*
*******************************************************************************
*/
__s32 memory_allot_init(void)
{
    memory_base = HorseHeadInit(sizeof(memory_block_t), MAX_MEMORY_BLOCK_NUM);
    if(memory_base == NULL)
    {
        DMSG_PANIC("ERR: HorseHeadInit failed\n");
        HorseExit();
        return -1;
    }

    return 0;
}

/*
*******************************************************************************
*                     memory_allot_exit
*
* Description:
*
*
* Parameters:
*
* Return value:
*
* note:
*
*******************************************************************************
*/
__s32 memory_allot_exit(void)
{
    if(memory_base)
    {
        HorseHeadExit(memory_base);
        memory_base = NULL;
    }

    return 0;
}





