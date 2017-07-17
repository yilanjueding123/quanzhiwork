#ifndef __RECORD_LIST_VIEW_FRAME_H__
#define __RECORD_LIST_VIEW_FRAME_H__

#include "app_record.h"


typedef struct tag_rec_list_view_frame
{
    H_WIN				layer;
    GUI_FONT 			*sys_font;

    __u32	 			root_type;
    precord_list_view_t	plistView;
    precord_list_doc_t	plistDoc;

} rec_list_view_frame_t, *prec_list_view_frame_t;



extern H_WIN rec_list_view_frm_create(H_WIN h_parent, precord_ctrl_t para);













#endif//__RECORD_LIST_VIEW_FRAME_H__

