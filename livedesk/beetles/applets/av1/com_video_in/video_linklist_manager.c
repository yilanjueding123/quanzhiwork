/*
********************************************************************************
*                                    eMOD
*                   the Easy Portable/Player Develop Kits
*                               mod_duckweed sub-system
*                          (module name, e.g.mpeg4 decoder plug-in) module
*
*          (c) Copyright 2010-2012, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : csi_linklist_manager.c
* Version: V1.0
* By     : Eric_wang
* Date   : 2010-1-4
* Description:
********************************************************************************
*/
#include "epdk.h"
#include "video_linklist_manager.h"

//链表实现方式2
/*******************************************************************************
关于 链表实现方式2 的说明:
(1). 一个线程vdrv_task() 和一个中断处理程序csi_irq_handle()会操作2个链表full2和free2
     因此需要做互斥。
(2). 考虑到ISR是不会被打断的，所以只需要对vdrv_task()操作涉及的函数做互斥处理就行了
    full2_insert( isr ), wt
    full2_delete( vdrv_task ), rd
    free2_insert( vdrv_task ), wt
    free2_delete( isr ), rd

    所以，只需要对full2_delete()和free2_insert()做互斥处理就行了。所谓互斥，也就是
    在处理前，把一些可能会被改变的变量记下来而已。
*******************************************************************************/
static void video_Impl_initial_linklist_manager(__video_linklist_manager_t *thiz, LINKLIST_TYPE type)
{
    //csi_memset(pManager, 0, sizeof(__csi_linklist_manager_t));
    thiz->list_type = type;
    thiz->wt = thiz->rd = 0;

    return;
}
static __s32 video_Impl_linklist_manager_exit(__video_linklist_manager_t *thiz)
{
    eLIBs_memset(thiz, 0, sizeof(__video_linklist_manager_t));
    eLIBs_free(thiz);
    return 0;
}

/*******************************************************************************
Function name: full2_insert
Description:
    针对装满帧的数组做插入操作。
    isr调用
    修改wt
    rd由vdrv_task()修改
Parameters:
    1. idx:就是数组__csi_frame_t csi_frame[CSI_BUFFER_NUM]的 frame_id
Return:

Time: 2010/7/12
*******************************************************************************/
static __s32 video_Impl_linklist_manager_insert(__video_linklist_manager_t *thiz, __s32 frame_id)
{
    __s32 uTmpWt = thiz->wt + 1;

    uTmpWt %= VIDEO_FRMID_CNT;
    if(thiz->rd == uTmpWt)
    {
        eLIBs_printf("fatal error, [%d] array full!\n", thiz->list_type);
    }
    //eLIBs_printf("insert wt=%x frame_id=%x\n",thiz->wt,frame_id);
    thiz->frmid_array[thiz->wt] = frame_id;
    thiz->wt = uTmpWt;

    return EPDK_OK;
}
/*******************************************************************************
Function name: full2_delete
Description:
    取一个元素出来，
    vdrv_task()调用。可能会没有元素。
    修改rd,
    wt由ISR修改
Parameters:

Return:
    1.如果没有元素, 返回-1
    2.如果有，返回id号。
Time: 2010/7/12
*******************************************************************************/
static __s32 video_Impl_linklist_manager_delete(__video_linklist_manager_t *thiz)
{
    __s32 frame_id;
    __s32 nTmpRd = thiz->rd;
    if(thiz->rd == thiz->wt)
    {
        //eLIBs_printf("fatal error, [%d] array empty!\n", thiz->list_type);
        return -1;
    }
    else
    {
        frame_id = thiz->frmid_array[nTmpRd++];
        //eLIBs_printf("delete rd=%x frame_id=%x\n",thiz->rd,frame_id);



        nTmpRd %= VIDEO_FRMID_CNT;
        thiz->rd = nTmpRd;
        return frame_id;
    }
}
__video_linklist_manager_t *video_linklist_manager_init()
{
    __video_linklist_manager_t *p = (__video_linklist_manager_t *)eLIBs_malloc(sizeof(__video_linklist_manager_t));
    if(NULL == p)
    {
        __wrn("malloc __csi_linklist_manager_t fail\n");
        return NULL;
    }
    eLIBs_memset(p, 0, sizeof(__video_linklist_manager_t));
    p->initial = video_Impl_initial_linklist_manager;
    p->insert_element = video_Impl_linklist_manager_insert;
    p->delete_element = video_Impl_linklist_manager_delete;
    p->exit = video_Impl_linklist_manager_exit;
    return p;
}

