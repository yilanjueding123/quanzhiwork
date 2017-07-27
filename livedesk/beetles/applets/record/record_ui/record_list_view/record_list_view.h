#ifndef __RECORD_LIST_VIEW_H__
#define __RECORD_LIST_VIEW_H__

#include "record_ui.h"
#include "record_list_view_doc.h"


typedef enum RECORD_LIST_FOCUSE
{
    RECORD_MUSIC_LIST,
    RECORD_MUSIC_MENU,
    RECORD_MUSIC_DELETE,
    RECORD_MUSIC_VOLUME

} RECORD_LIST_FOCUSE;

typedef enum RECORD_DELETE_STATE
{
    RECORD_DEL_NONE,
    RECORD_DEL_OK,
    RECORD_DEL_CAL
} RECORD_DELETE_STATE;

typedef struct tag_record_list_view
{
    precord_ui_t		pui;

    MUSIC_LIST_MENU		curMenu;
    MUSIC_LIST_MENU		playMenu;
    FOCUSE_STATE		curMenuSt;

    RECORD_LIST_FOCUSE	oldPart;
    RECORD_LIST_FOCUSE	curPart;
    __u32				timeCnt;

    RECORD_DELETE_STATE delState;

    precord_list_doc_t	precord_list_doc;

} record_list_view_t, *precord_list_view_t;






extern void record_list_view_show(precord_list_view_t prec_list_view, H_WIN layer);

















#endif//__RECORD_LIST_VIEW_H__

