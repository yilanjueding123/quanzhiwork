

#ifndef __EXPLORER_LIST_H__
#define __EXPLORER_LIST_H__

#include "FileList.h"

typedef enum exp_icon_status_s
{
    UNFOCUSED = 0,
    FOCUSED = 1,
    PRESSED = 2,
} exp_icon_status_e;


//定义rat 文件搜索数据结构
typedef struct tag_rat_ctrl
{
    HRAT  handle;				//文件搜索句柄
    __s32 index;				//当前文件索引号
    __s32 total;				//文件总数
    //rat_media_type_t media_type;//搜索文件的类型
} rat_ctrl_t;


typedef struct tag_explr_list_para
{
    H_WIN      			list_win;			//窗口句柄
    GUI_FONT 			*explr_list_font;
    H_LYR 				BG_layer;			//背景图层
    H_LYR 				list_layer;				//listbar 图层
    void 				*core;
    __s32				root_type;
    __u32 				last_start_id;		//上次浏览起始ID
    __u32 				last_focused_id;	//上次选中文件id
    __u32               select_id;
    char  				*last_filename;		//上次浏览文件 名

    GUI_FONT			*font;

    char 				search_path[RAT_MAX_FULL_PATH_LEN];				//搜索路径
    H_LBAR 				listbar_handle;

    __hdle				long_str_handle;		//长字符串滚动句柄

    rat_ctrl_t 			rat;					//文件搜索数据结构
    rat_media_type_t 	media_type;				//搜索文件的类型
    explorer_view_mode_e view_mode;				//显示模式,List or square

    file_list_t        *top_file_list;			//top filelist
    file_list_t        *cur_file_list;			//current filelist
    file_item_t        *file_item;				//画icon的file_item;

    H_WIN				h_dialog;
    H_WIN				h_dialog_msg;

    __bool				del_dlg_open;			//删除对话框是否open

    H_WIN				h_fw_update_dlg;        //固件升级对话框

    char 				music_play_file[RAT_MAX_FULL_PATH_LEN];				//文件管理播放文件
    char 				fw_update_file[RAT_MAX_FULL_PATH_LEN];				//固件升级文件
    root_para_t  *root_para	;		//记录从root 传进来的参数
} explr_list_para_t;



#define EXPLR_LIST_ENTER			0
#define EXPLR_LIST_RETURN			1

#define MSG_EXPLR_UPDATE			(GUI_MSG_USER_DEF+0)


H_WIN explorer_list_win_create(H_WIN h_parent, explr_list_para_t *para);
__s32 explorer_list_win_delete(H_WIN list_win);

void  explorer_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);
#endif







