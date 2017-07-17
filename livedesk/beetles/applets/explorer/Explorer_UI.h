


#ifndef __EXPLORER_UI_H
#define	__EXPLORER_UI_H

#include "beetles_app.h"
#include "Explorer.h"
#include "Explorer_List.h"
typedef enum explr_list_icon_id_s
{
    ID_EXP_PALETTE = 0,
    ID_EXP_BACKGROUND,

    ID_EXP_USB,
    ID_EXP_USB_ICON_UNFOCUSED,
    ID_EXP_USB_ICON_FOCUSED,

    ID_EXP_SD,
    ID_EXP_SD_ICON_UNFOCUSED,
    ID_EXP_SD_ICON_FOCUSED,

    ID_EXP_FOLDER,
    ID_EXP_FOLDER_ICON_UNFOCUSED,
    ID_EXP_FOLDER_ICON_FOCUSED,

    ID_EXP_UNKNOWN_FILE,
    ID_EXP_UNKNOWN_FILE_ICON_UNFOCUSED,
    ID_EXP_UNKNOWN_FILE_ICON_FOCUSED,

    ID_EXP_VIDEO_FILE,
    ID_EXP_VIDEO_ICON_UNFOCUSED,
    ID_EXP_VIDEO_ICON_FOCUSED,

    ID_EXP_MUSIC_FILE,
    ID_EXP_MUSIC_ICON_UNFOCUSED,
    ID_EXP_MUSIC_ICON_FOCUSED,

    ID_EXP_PHOTO_FILE,
    ID_EXP_PHOTO_ICON_UNFOCUSED,
    ID_EXP_PHOTO_ICON_FOCUSED,

    ID_EXP_EBOOK_FILE,
    ID_EXP_EBOOK_ICON_UNFOCUSED,
    ID_EXP_EBOOK_ICON_FOCUSED,

    ID_EXP_DELETE_ICON_UNFOCUSED,
    ID_EXP_DELETE_ICON_FOCUSED,

    ID_EXP_LIST_ITEM_FOCUS_BG,
    ID_EXP_SQUARE_FOCUS,

    ID_EXP_LIST_SCROLL_BG,
    ID_EXP_LIST_SCROLL_HEAD,
    ID_EXP_LIST_SCROLL_BODY,
    ID_EXP_LIST_SCROLL_TAIL,

    ID_EXP_LONG_STR_SCROLL_BG,
    ID_EXP_ERROR_BMP ,
    BROWSER_ICON_MAX,
} explr_list_icon_id_e;							//e:代表enum,  s:代表struct

typedef struct explorer_layer_ui_param_s
{
    __pixel_rgbfmt_t fb_fmt;        //图层格式
    SIZE fb_size;                   //framebuffer大小
    RECT src_rect;                  //srcwin区域
    RECT scn_rect;                  //scnwin区域
    __u32 reserved[32];             //预留
} explorer_layer_ui_param_t;


typedef struct explorer_list_item_ui_param_s
{
    RECT ListBarArea;						//list display area in list mode
    RECT item_rect;														//list item  UI 参数

    RECT item_top_line;
    RECT item_bottom_line;
    RECT item_dividing_line;

    RECT icon_rect;
    RECT miniature_rect;

    RECT text_rect;

    RECT focus_icon_rect;
    RECT media_type_rect; 			//媒体类型图标

    RECT title_rect;
    RECT file_size_rect;			//文件大小
    RECT time_rect;
    RECT number_rect;

} explorer_list_item_ui_param_t;

typedef struct explorer_square_item_ui_param_s
{
    RECT ListBarArea;						//list display area in square mode
    RECT item_rect;							//条目框
    RECT focus_rect;						//焦点框
    RECT icon_rect;							//图标
    RECT miniature_rect;					//缩略图宽高
    RECT title_rect;
} explorer_square_item_ui_param_t;


typedef struct explorer_icon_data_s
{
    __u32 res_id;
    HTHEME res_handle;
    void *res_buffer;
} explorer_icon_data_t;			//icon data struct


typedef struct explorer_viewer_ui_s
{
    explorer_layer_ui_param_t layer;    //layer window
    RECT listview;                      //menuview(frame window)
    RECT listview_bg_rect;
    RECT listview_bg_top_rect;
    RECT listview_bg_bottom_rect;
    RECT listview_bg_left_rect;

    RECT scroll_bg_rect;
    RECT scroll_rect;
    __u32 scroll_head_height;
    __u32 scroll_body_height;
    __s32 scroll_tail_height;


    RECT ListBarArea;      //listbar content

    explorer_list_item_ui_param_t 		list_item_ui_param;     					//UI in list mode
    explorer_square_item_ui_param_t 	square_item_ui_param;    			//UI in square mode

    explorer_icon_data_t 					icon_table[BROWSER_ICON_MAX + 1];

} explorer_viewer_ui_t;



//获得整个explorer AP设计的UI参数
extern explorer_viewer_ui_t *explorer_get_viewer_ui_param(void);

extern RECT *explorer_get_listview_ui_param(void);

//获得list 中 item 设计的UI参数
extern explorer_list_item_ui_param_t *explorer_get_list_item_ui_param(void);


//获得list 中 square 设计的UI参数
extern explorer_square_item_ui_param_t *explorer_get_square_item_ui_param(void);


extern void *explorer_get_listview_icon_res(__u32 index);
extern __s32 explorer_free_listview_icon_res(void);


//获得Explorer Layer rectangle
__s32 explorer_get_screen_rect(RECT *screen_rect);

//显示title字符串
extern __s32 explorer_draw_file_manager_title(void);
//获得缩略图参数(according to the viewmode of list)
extern __s32 explorer_get_item_miniature_rect(explr_list_para_t *para, RECT *miniature_rect);
extern __s32 explorer_get_item_text_rect(explr_list_para_t *para, RECT *text_rect);
extern __s32 explorer_get_item_icon_rect(explr_list_para_t *para, RECT *icon_rect);
extern __s32 explorer_get_item_focus_icon_rect(explr_list_para_t *para, RECT *focus_icon_rect);

extern __s32 explorer_clear_filesize_area(void);
extern __s32 explorer_draw_FileSize(file_item_t *item);
extern void explorer_clean_delete_file_hint_area(void);
extern void  explorer_delete_file_draw_process(file_op_info_t *op_info);
extern void  explorer_delete_file_draw_process_ext(void);
extern void  explorer_draw_delete_file_failed(void);

extern __s32 explorer_clear_file_create_time_area(void);
extern __s32 explorer_draw_file_create_time(char *file_name);
extern __s32 explorer_draw_background(explr_list_para_t *list_para);
extern __s32 explorer_draw_FileTypeIcon(explr_list_para_t *list_para, rat_media_type_t media_type);
extern void *explorer_get_file_item_icon(explorer_view_mode_e view_mode, rat_media_type_t media_type, exp_icon_status_e status);

extern __u32 explorer_get_listbar_pagenum(explr_list_para_t *list_para);


#endif


