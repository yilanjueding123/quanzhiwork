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
* File   : csi_linklist_manager.h
* Version: V1.0
* By     : Eric_wang
* Date   : 2010-1-4
* Description:
********************************************************************************
*/
#ifndef _CSI_LINKLIST_MANAGER_H_
#define _CSI_LINKLIST_MANAGER_H_
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
#define FRMID_CNT (CSI_BUFFER_NUM+1)
typedef enum
{
    CSI_LISTTYPE_FREE = 0,
    CSI_LISTTYPE_FULL = 1,
} CSI_LINKLIST_TYPE;
typedef struct tag_CSI_LINKLIST_MANAGER __csi_linklist_manager_t;
typedef void    (*CSI_LINKLIST_MANAGER_Initial)     (__csi_linklist_manager_t *thiz, CSI_LINKLIST_TYPE type);
typedef __s32   (*CSI_LINKLIST_MANAGER_Insert)      (__csi_linklist_manager_t *thiz, __s32 frame_id);
typedef __s32   (*CSI_LINKLIST_MANAGER_Delete)      (__csi_linklist_manager_t *thiz);
typedef __s32   (*CSI_LINKLIST_MANAGER_Exit)        (__csi_linklist_manager_t *thiz);
typedef struct tag_CSI_LINKLIST_MANAGER
{
    CSI_LINKLIST_TYPE list_type;
    __s32 frmid_array[FRMID_CNT];  //存index号的数组,  index是__csi_frame_t csi_frame[CSI_BUFFER_NUM]的index
    __s32 wt;
    __s32 rd;
    CSI_LINKLIST_MANAGER_Initial    initial;
    CSI_LINKLIST_MANAGER_Insert     insert_element;
    CSI_LINKLIST_MANAGER_Delete     delete_element;
    CSI_LINKLIST_MANAGER_Exit       exit;
} __csi_linklist_manager_t; //只允许使用CSI_BUFFER_NUM个元素，避免wt,rd重合时的歧义(满还是空).
extern __csi_linklist_manager_t *csi_linklist_manager_init(void);

#endif  /* _CSI_LINKLIST_MANAGER_H_ */

