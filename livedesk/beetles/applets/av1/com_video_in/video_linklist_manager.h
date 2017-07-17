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
#ifndef _LINKLIST_MANAGER_H_
#define _LINKLIST_MANAGER_H_

#define VPLY_FRM_Q_SIZE     TVD_BUFFER_NUM
#define VIDEO_FRMID_CNT (VPLY_FRM_Q_SIZE+1)

typedef enum
{
    LISTTYPE_FREE = 0,
    LISTTYPE_FULL = 1,
} LINKLIST_TYPE;

typedef struct tag_LINKLIST_MANAGER __video_linklist_manager_t;
typedef void    (*VIDEO_LINKLIST_MANAGER_Initial)     (__video_linklist_manager_t *thiz, LINKLIST_TYPE type);
typedef __s32   (*VIDEO_LINKLIST_MANAGER_Insert)      (__video_linklist_manager_t *thiz, __s32 frame_id);
typedef __s32   (*VIDEO_LINKLIST_MANAGER_Delete)      (__video_linklist_manager_t *thiz);
typedef __s32   (*VIDEO_LINKLIST_MANAGER_Exit)        (__video_linklist_manager_t *thiz);

typedef struct tag_LINKLIST_MANAGER
{
    LINKLIST_TYPE list_type;
    __s32 frmid_array[VIDEO_FRMID_CNT];
    __s32 wt;
    __s32 rd;
    VIDEO_LINKLIST_MANAGER_Initial    initial;
    VIDEO_LINKLIST_MANAGER_Insert     insert_element;
    VIDEO_LINKLIST_MANAGER_Delete     delete_element;
    VIDEO_LINKLIST_MANAGER_Exit       exit;
} __video_linklist_manager_t; //只允许使用VPLY_FRM_Q_SIZE个元素，避免wt,rd重合时的歧义(满还是空).
extern __video_linklist_manager_t *video_linklist_manager_init(void);

#endif  /* _VIDEO_LINKLIST_MANAGER_H_ */

