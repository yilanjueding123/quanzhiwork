/*
********************************************************************************************************************
*                                              USB Host Drvier
*
*                              (c) Copyright 2007-2009, softwinners.China
*										All	Rights Reserved
*
* File Name 	: platform_debug.c
*
* Author 		: javen
*
* Version 		: 1.0
*
* Date 			: 2009.10.12
*
* Description 	:
*
* History 		:
*
********************************************************************************************************************
*/
#include  "usb_host_config.h"
#include  "usb_os_platform.h"
#include  "usb_host_base_types.h"

#include  "usb_ch9.h"
#include  "usb_host_common.h"

#include  "usb_list.h"
#include  "list_head_ext.h"

#include  "platform_debug.h"


static struct list_head pf;
static __u32 malloc_nr = 0;

static void add_to_pf_list(__u32 nr, __u32 *addr, __u8 *file, __u32 line)
{
    struct list_head *node = NULL;
    __memory_debug_t *memory = NULL;
    __cpu_sr lock = 0;

    DMSG_INFO("[pf_malloc]: nr:%d, addr:%x, file:%s, line:%d\n", nr, addr, file, line);

    /* create a new data */
    memory = (__memory_debug_t *)esMEMS_Malloc(0 , sizeof(__memory_debug_t));
    if(memory == NULL)
    {
        DMSG_PANIC("ERR: esMEMS_Malloc failed\n");
        return;
    }

    USB_OS_MEMSET(memory, 0, sizeof(__memory_debug_t));
    memory->addr = addr;
    eLIBs_strcpy((char *)memory->file, (const char *)file);
    memory->line = line;
    memory->nr	 = nr;

    /* create a new list_node */
    node = (struct list_head *)esMEMS_Malloc(0 , sizeof(struct list_head));
    if(node == NULL)
    {
        DMSG_PANIC("ERR: esMEMS_Malloc failed\n");

        USB_OS_FREE(memory);
        memory = NULL;

        return;
    }

    INIT_LIST_HEAD(node);

    /* add the new node to list */
    USB_OS_ENTER_CRITICAL(lock);
    node->data = memory;
    list_add_tail(node, &pf);
    USB_OS_EXIT_CRITICAL(lock);

    return;
}

static void del_from_pf_list(__u32 *addr)
{
    struct list_head *list_head = &pf;
    struct list_head *list_now 	= NULL;
    struct list_head *list_next = NULL;
    __memory_debug_t *memory 	= NULL;
    __cpu_sr lock = 0;

    /* find node */
    list_now = list_head->next;
    while(list_now != list_head)
    {
        list_next = list_now->next;

        /* del the node from list */
        memory = (__memory_debug_t *)list_now->data;
        if(memory)
        {
            if(memory->addr == addr)
            {
                USB_OS_ENTER_CRITICAL(lock);
                list_del(list_now);
                USB_OS_EXIT_CRITICAL(lock);

                if(list_now->data)
                {
                    memory = (__memory_debug_t *)list_now->data;
                    if(memory)
                    {
                        DMSG_INFO("[pf_free]: nr:%d, addr:%x, file:%s, line:%d\n",
                                  memory->nr, memory->addr, memory->file, memory->line);
                        esMEMS_Mfree(0 , (void *)memory);
                    }

                    esMEMS_Mfree(0 , (void *)list_now);
                    list_now = NULL;
                }
                break;
            }
        }

        list_now = list_next;
    }

    return;
}

static void print_pf_list(void)
{
    struct list_head *list_head = &pf;
    struct list_head *list_now 	= NULL;
    struct list_head *list_next = NULL;
    __memory_debug_t *memory 	= NULL;

    list_now = list_head->next;
    while(list_now != list_head)
    {
        list_next = list_now->next;

        memory = (__memory_debug_t *)list_now->data;
        if(memory)
        {
            DMSG_INFO("addr: %x, \"%s\", line%d\n", memory->addr, memory->file, memory->line);
        }

        list_now = list_next;
    }

    return;
}

void *pf_malloc(u32 size, u8 *file_name , u32 line_nr)
{
    void *addr = NULL;

    addr = (void *)esMEMS_Malloc(0 , size);
    if(addr == NULL)
    {
        DMSG_PANIC("ERR: esMEMS_Malloc failed\n");
        return NULL;
    }

    malloc_nr++;
    add_to_pf_list(malloc_nr, addr, file_name, line_nr);

    return addr;
}

void pf_free(void *addr)
{
    malloc_nr--;
    del_from_pf_list(addr);

    esMEMS_Mfree(0 , (void *)addr );
}

__s32 platform_debug_init(void)
{
    INIT_LIST_HEAD(&pf);

    return 0;
}

__s32 platform_debug_exit(void)
{
    DMSG_INFO("[platform_debug_exit]: malloc_nr = %d\n", malloc_nr);

    print_pf_list();

    return 0;
}

