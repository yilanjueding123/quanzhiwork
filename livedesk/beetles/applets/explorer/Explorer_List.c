
/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
* File    	: 	Browser.c,
*			Browser.h
* By      	: 	Libaiao
* Func	:
* Version	: 	v1.0
* ============================================================================================================
* 2010-4-14 15:24:52  Libaiao  create this file, implements the fundemental interface;
**************************************************************************************************************
*/


#include "beetles_app.h"

#include "Explorer.h"
#include "Explorer_UI.h"
#include "Explorer_List.h"

#include "FileList.h"

#if  1
#define __msg(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
						     eLIBs_printf(__VA_ARGS__)									        )
#else
#define __msg(...)    	
#endif

static   __u32 ExplorerTimerId = 0x16;
#define	 C_EXP_TIMER_INTERVAL	300				// 2second



H_WIN explorer_list_win_create(H_WIN h_parent, explr_list_para_t *para);
static __s32 _explorer_list_win_cb(__gui_msg_t *msg);			//下划线开始表示为回调函数

static __s32 explorer_listview_create(H_WIN  list_win);
static __s32 explorer_listview_onpaint(H_WIN  list_win);
static __s32  explorer_list_win_on_close(__gui_msg_t *msg);
static __s32  explorer_list_win_on_destroy(__gui_msg_t *msg);
static __s32  explorer_list_win_key_proc(__gui_msg_t *msg);

static __s32 explorer_rat_init(explr_list_para_t *list_para);
static __s32 explorer_rat_uninit(explr_list_para_t *list_para);

static void explorer_listbar_init(H_WIN  list_win);
static void explorer_listbar_uninit(H_WIN  list_win);

static __s32 _explorer_listbar_item_draw(__lbar_draw_para_t *draw_param);
static __s32 draw_miniature_view_item(__lbar_draw_para_t *draw_param);
static __s32 draw_listview_item(__lbar_draw_para_t *draw_param);

__s32 ExplorerListWinOnEnterKey(explr_list_para_t *list_para);
__s32 ExplorerListWinOnNextItem(explr_list_para_t *list_para);
__s32 ExplorerListWinOnPreviousItem(explr_list_para_t *list_para);
__s32 ExplorerListWinOnNextRow(explr_list_para_t *list_para);
__s32 ExplorerListWinOnPreviousRow(explr_list_para_t *list_para);
__s32 ExplorerListWinOnNextPage(explr_list_para_t *list_para);
__s32 ExplorerListWinOnPreviousPage(explr_list_para_t *list_para);


void  ExplorerListWinGetSearchPath(explr_list_para_t *para);


static __s32 ExtractFileName(char *FileName, char *FilePath);
static __s32 GetListItemFileFullPath(explr_list_para_t *list_para, __s32 ItemIndex, char *FilePath);
static __s32 GetListItemFileName(explr_list_para_t *list_para, __s32 ItemIndex, char *FileName);
rat_media_type_t GetListItemFileMediaType(HRAT rat_handle, __s32 ItemIndex);
void ExplorerSendMessage(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);

//改变显示方式
void ExplorerSetViewMode(explr_list_para_t *list_para, explorer_view_mode_e view_mode);



//长文件名滚动显示
__s32 explorer_list_long_string_init(explr_list_para_t *this);
__s32 explorer_list_long_string_uninit(explr_list_para_t *this);
__s32 explorer_list_long_string_start_roll_fast(__lbar_draw_para_t *draw_param, char *string)	;
__s32 explorer_list_long_string_start_roll(explr_list_para_t *this);
static __s32 explorer_list_long_string_stop_roll(explr_list_para_t *this);


//获得保存上一次浏览的参数
static __s32 explorer_get_last_para(explr_list_para_t *list_para);
static __s32 explorer_save_last_para(explr_list_para_t *list_para)	;

__s32 explorer_draw_file_info(explr_list_para_t *list_para);
void explorer_list_on_timer(__gui_msg_t *msg);

__s32 File_manager_play_file(explr_list_para_t *list_para)
{
    __s32 ret;
    char FileName[RAT_MAX_FULL_PATH_LEN] = { 0 };	//全路径
    rat_media_type_t media_type ;
    __u8 root_type = 0;
    __u8  fatdirattr;					//add by libaiao,2011.8.24
    file_item_t *file_item = NULL;
    char  path[RAT_MAX_FULL_PATH_LEN] = { 0 };

    file_item = list_para->cur_file_list->cur_item;
    eLIBs_strcpy(path, list_para->cur_file_list->file_path);
    eLIBs_strcat(path, "\\");
    eLIBs_strcat(path, file_item->name);
    __msg("File_manager_play_file  path = %s \n " , path );

    eLIBs_strcpy( FileName , path ) ;
    __msg("full_file_name=%s\n", FileName );

    fatdirattr = list_para->top_file_list->cur_item->fatdirattr;
    if(fatdirattr == RAT_MEDIA_TYPE_USB_DEVICE)
    {
        root_type = RAT_USB;
    }
    else if(fatdirattr == RAT_MEDIA_TYPE_SD_DEVICE)
    {
        root_type = RAT_TF;
    }
    else
    {
        root_type = 0;
    }

    media_type = rat_get_file_type( file_item->name );
    __msg( "media_type = %d\n" , media_type );
    if( RAT_MEDIA_TYPE_AUDIO == media_type )
    {
        __msg("audio media type break \n");
        return EPDK_OK ;
    }
    list_para->root_type = root_type ;
    ExplorerListWinGetSearchPath( list_para );
    list_para->rat.handle = rat_open(list_para->search_path , media_type , 0 );	//全部搜索
    list_para->rat.total = rat_get_cur_scan_cnt ( list_para->rat.handle ) ;

    ret = rat_set_file_for_play(list_para->rat.handle, FileName );

    if(EPDK_FAIL == ret)
    {
        __msg( " rat set file for play fail \n");
        return EPDK_FAIL;
    }
    return EPDK_OK;
}
static __s32  file_manager_set_current_file_for_play ( __gui_msg_t *msg )
{
    __s32 ret;
    H_WIN hExpWin = NULL;
    explr_list_para_t *list_para;
    rat_media_type_t media_type;
    file_item_t *file_item ;

    __u8 root_type = 0;
    __u8  fatdirattr;					//add by libaiao,2011.8.24

    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    ret = File_manager_play_file(list_para);
    if(EPDK_FAIL == ret)
    {
        return EPDK_FAIL ;
    }
    __here__ ;

    //add by libaiao,2011.8.24
    fatdirattr = list_para->top_file_list->cur_item->fatdirattr;
    if(fatdirattr == RAT_MEDIA_TYPE_USB_DEVICE)
    {
        root_type = RAT_USB;
    }
    else if(fatdirattr == RAT_MEDIA_TYPE_SD_DEVICE)
    {
        root_type = RAT_TF;
    }
    else
    {
        root_type = 0;
    }
    list_para->root_para->explr_root = root_type ;
    __msg("file_manager_set_current_file_for_play root_type = %d\n" , root_type );
    file_item = get_file_list_item( list_para->cur_file_list, list_para->rat.index );
    if( ! file_item )
    {
        __msg( " file item is null \n" );
        return EPDK_FAIL ;
    }
    __msg("list_para->rat.index = %d file_item->name = %s \n " , list_para->rat.index , file_item->name );

    media_type = rat_get_file_type( file_item->name );
    __msg( "media_type = %d\n" , media_type );

    //media_type = get_file_list_item_file_type(list_para->file_item);
    switch( media_type )
    {
    case RAT_MEDIA_TYPE_ALL:
        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
        break;
    case RAT_MEDIA_TYPE_PIC:
        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP , EXPLR_SW_TO_PHOTO , 0);
        break;
    case RAT_MEDIA_TYPE_AUDIO:
    {
        //char full_file_name[RAT_MAX_FULL_PATH_LEN] = { 0 } ;
        file_item_t *file_item = NULL;
        char  path[RAT_MAX_FULL_PATH_LEN] = { 0 };

        __here__ ;
        file_item = list_para->cur_file_list->cur_item;
        //eLIBs_memset(path, 0, sizeof(path));
        eLIBs_strcpy(path, list_para->cur_file_list->file_path);
        eLIBs_strcat(path, "\\");
        eLIBs_strcat(path, file_item->name);
        __msg("file_manager_set_current_file_for_play  path = %s \n " , path );
        //	eLIBs_memset(list_para->music_play_file,0,sizeof(list_para->music_play_file));

        //	eLIBs_strcpy(list_para->music_play_file , path ) ;
        eLIBs_strcpy( list_para->root_para->music_file_name , path ) ;

        __msg("full_file_name=%s\n", list_para->root_para->music_file_name );
        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MUSIC , 0);
    }
    break;
    case RAT_MEDIA_TYPE_VIDEO:
        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MOVIE , 0 );
        break;
    case RAT_MEDIA_TYPE_EBOOK:
        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_EBOOK,  0);
        break;
    default:
        break;
    }

    hExpWin = GUI_WinGetParent(msg->h_deswin);
    ExplorerSendMessage(hExpWin, GUI_MSG_CLOSE, 0, 0);
    return EPDK_OK;
}

//
/*
************************************************************************************************************************
*Function	:           H_WIN explorer_list_win_create(H_WIN h_parent, explr_list_para_t *para)
*
*Description	:		根据ManagerWin传递过来的参数创建FrameWIn场景
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
H_WIN explorer_list_win_create(H_WIN h_parent, explr_list_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    explr_list_para_t *list_para;
    FB fb;

	__msg("explorer_list_win_create\n");
	
    GUI_LyrWinGetFB(para->list_layer, &fb);

    list_para = (explr_list_para_t *)esMEMS_Balloc(sizeof(explr_list_para_t));
    eLIBs_memset((void *)list_para, 0, sizeof(explr_list_para_t));

    list_para->explr_list_font = para->explr_list_font;
    list_para->list_layer = para->list_layer;
    list_para->core = para->core;
    list_para->media_type = para->media_type;		//photo, music, movie
    list_para->root_type = para->root_type;			//SD or USB device
    list_para->view_mode = para->view_mode;			//list or square
    list_para->root_para = para->root_para ;
    ExplorerListWinGetSearchPath(list_para);

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =	"Explorer List window",
                  framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner	= NULL;
    framewin_para.id        = EXPLR_LIST_WIN_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_explorer_list_win_cb);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 255;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr =  (void *)list_para;
    framewin_para.hLayer = list_para->list_layer;			//会被listbar调用
    return (GUI_FrmWinCreate(&framewin_para));
}

/*
************************************************************************************************************************
*Function	:           static __s32 explorer_listview_onpaint(H_WIN  list_win)
*
*Description	:		OnPaint处理函数,画背景图，前景ListBar showpage, 其他icon元素
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 explorer_listview_onpaint(H_WIN  list_win)
{

    //GUI_RECT title_rect;
    __s32 ret = 0;
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(list_win);
    if (GUI_LyrWinGetSta(list_para->list_layer) == GUI_LYRWIN_STA_SUSPEND)
        return EPDK_FAIL;
    if (GUI_LyrWinGetSta(list_para->list_layer) == GUI_LYRWIN_STA_ON)
    {
        __msg("to make sure layer status on\n");

    }

    GUI_LyrWinSetTop(list_para->list_layer);
    GUI_SetFont(SWFFont);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);

    GUI_WinSetFocusChild(list_win);		//frame win get focus
    GUI_LyrWinSel(list_para->list_layer);


    //显示文件名称在这里
    LISTBAR_ShowPage(list_para->listbar_handle);

    if(list_para->media_type == RAT_MEDIA_TYPE_ALL)
    {
        explorer_draw_file_manager_title();
    }
    else
    {
    	__msg("explorer_draw_FileTypeIcon\n");
        //explorer_draw_FileTypeIcon(list_para, list_para->media_type);
    }
    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:           static __s32  explorer_list_win_on_close(__gui_msg_t *msg)
*
*Description	:		OnClose处理函数，释放FrameWin
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32  explorer_list_win_on_close(__gui_msg_t *msg)
{
    //theme_close();
    GUI_FrmWinDelete(msg->h_deswin);			//send destroy message
    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:           static __s32  explorer_list_win_on_destroy(__gui_msg_t *msg)
*
*Description	:		OnDestroy处理函数，释放framewin 所有申请的资源
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32  explorer_list_win_on_destroy(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if(GUI_IsTimerInstalled(list_para->list_win, ExplorerTimerId))
    {
        GUI_KillTimer(list_para->list_win, ExplorerTimerId);
    }

    if(list_para->h_dialog != NULL)
    {
        app_dialog_destroy(list_para->h_dialog); 		//to delete dialog
    }

    if(list_para->h_dialog_msg != NULL)
    {
        app_dialog_destroy(list_para->h_dialog_msg); 		//to delete dialog
    }
    if(list_para->media_type == RAT_MEDIA_TYPE_ALL)
    {
        __here__;
        explorer_file_list_uninit(list_para);
        __here__;
    }
    else
    {
        __here__;
        explorer_save_last_para(list_para);
    }
    __here__;

    explorer_list_long_string_uninit(list_para);		//释放long string scroll

    __here__;
    explorer_listbar_uninit(msg->h_deswin);

    if(list_para->media_type == RAT_MEDIA_TYPE_ALL)
    {
    }
    else
    {
        explorer_rat_uninit(list_para);
    }

    __here__;
    explorer_free_listview_icon_res();
    __here__;

    esMEMS_Bfree(list_para, sizeof(explr_list_para_t));
    __here__;
    //explorer_gui_long_string_deinit();
    //_browser_scene_layer_deinit();
    //cat_core_close(browser_param->cat_handle);
    //browser_param->cat_handle = NULL;

    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:          __s32 ExplorerListWinOnEnterKey(explr_list_para_t *list_para)
*
*Description	:		EnterKey处理函数
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
__s32 ExplorerListWinOnEnterKey(explr_list_para_t *list_para)
{
    char FileName[RAT_MAX_FULL_PATH_LEN];
    __s32 ret;

    eLIBs_memset(FileName, 0, sizeof(FileName));
    ret = GetListItemFileFullPath(list_para, list_para->rat.index, FileName);
    if(EPDK_FAIL == ret)
    {
        return EPDK_FAIL;
    }

    rat_set_file_for_play(list_para->rat.handle, FileName);
    __msg("~~~~Set play file name is  = %s\n", FileName);
    /*
    	if(list_para->view_mode == EXPLR_LIST_MODE)
    	{
    		ExplorerSetViewMode(list_para, EXPLR_SQUARE_MODE);
    	}
    	else if(list_para->view_mode == EXPLR_SQUARE_MODE)
    	{
    		ExplorerSetViewMode(list_para, EXPLR_LIST_MODE);
    	}
    */
    return EPDK_OK;
}
/*
************************************************************************************************************************
*Function	:          __s32 ExplorerListWinOnNextItem(explr_list_para_t *list_para)
*
*Description	:		向下一个条目处理函数,在NextKey中被调用
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/

__s32 ExplorerListWinOnNextItem(explr_list_para_t *list_para)
{
    __s32 ret;
    __s32 nStep = 1;
    //__s32 PageCount = LISTBAR_GetPageItemCount(list_para->listbar_handle);	//获得一行的条目数

    /*
    	if(list_para->rat.index + 1 >= list_para->rat.total)
    	{
    		return EPDK_FAIL;			//only for test
    	}
    	ret = rat_move_cursor_forward(list_para->rat.handle, nStep);//返回实际移动的步数
    	if(ret == 0)	//0，表示已经找不到文件了
    	{
    		if(rat_is_cursor_end(list_para->rat.handle))
    		{
    			return EPDK_FAIL;
    		}
    		else
    		{
    			__wrn("there is a exception in rat!!!\n");
    			return EPDK_FAIL;
    		}
    	}
    */
    {
        __listbar_scene_t ListBarScene;

        LISTBAR_NextItem(list_para->listbar_handle);

        LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
        __msg("listbar start id = %d\n", ListBarScene.start_id);
        __msg("listbar focus id = %d\n", ListBarScene.focus_id);
        list_para->rat.index = ListBarScene.focus_id;
    }
    return EPDK_OK;
}
/*
************************************************************************************************************************
*Function	:          __s32 ExplorerListWinOnNextItem(explr_list_para_t *list_para)
*
*Description	:		向上一个条目处理函数,在PreviousKey中被调用
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
__s32 ExplorerListWinOnPreviousItem(explr_list_para_t *list_para)
{
    __s32 ret;
    __s32 nStep = 1;
    /*
    if(list_para->rat.index <= 0)
    {
    	return EPDK_FAIL;
    }

    ret = rat_move_cursor_backward(list_para->rat.handle, nStep);
    if(ret == 0)//0，表示已经找不到文件了
    {
    	return EPDK_FAIL;
    }
    //list_para->rat.index--;
    */
    {
        __listbar_scene_t ListBarScene;

        LISTBAR_PrevItem(list_para->listbar_handle);
        LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
        __msg("listbar start id = %d\n", ListBarScene.start_id);
        __msg("listbar focus id = %d\n", ListBarScene.focus_id);
        list_para->rat.index = ListBarScene.focus_id;
    }

    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:          __s32 ExplorerListWinOnNextItem(explr_list_para_t *list_para)
*
*Description	:		响应上一行条目处理函数,在PreviousKey中被调用
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
__s32 ExplorerListWinOnNextRow(explr_list_para_t *list_para)
{
    __s32 ret;
    __s32 RowCount = LISTBAR_GetRowItemCount(list_para->listbar_handle);	//获得一行的条目数


    ret = rat_move_cursor_forward(list_para->rat.handle, RowCount);
    if(ret == 0)
    {
        if(rat_is_cursor_end(list_para->rat.handle))
        {
            return EPDK_FAIL;
        }
        else
        {
            __wrn("there is a exception in rat!!!\n");
            return EPDK_FAIL;
        }
    }

    list_para->rat.index += ret;			//ret为实际向下移动的条目数

    if(list_para->rat.index >= list_para->rat.total)
    {
        list_para->rat.total = list_para->rat.index;
        LISTBAR_SetTotalItem(list_para->listbar_handle, list_para->rat.total);
    }

    LISTBAR_NextRow(list_para->listbar_handle);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:          __s32 ExplorerListWinOnNextItem(explr_list_para_t *list_para)
*
*Description	:		向上一行条目处理函数,在PreviousKey中被调用
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
__s32 ExplorerListWinOnPreviousRow(explr_list_para_t *list_para)
{
    __s32 ret;
    __s32 RowCount = LISTBAR_GetRowItemCount(list_para->listbar_handle);	//获得一行的条目数

    if(list_para->rat.index <= 0)
    {
        return EPDK_FAIL;
    }


    ret = rat_move_cursor_backward(list_para->rat.handle, RowCount);
    if(ret == 0)
    {
        if(rat_is_cursor_end(list_para->rat.handle))
        {
            return EPDK_FAIL;
        }
        else
        {
            __wrn("there is a exception in rat!!!\n");
            return EPDK_FAIL;
        }
    }
    if(list_para->rat.index >= ret)
    {
        list_para->rat.index -= ret;			//ret为实际向下移动的条目数
    }
    else
    {
        list_para->rat.index = 0;				//已经在第一页，定位到第一个条目
        LISTBAR_SetFocusItem(list_para->listbar_handle, list_para->rat.index);
        //__wrn("Explorer_list.c, line296, there is a exception with listview!!!\n");
        return EPDK_OK;
    }

    LISTBAR_PrevRow(list_para->listbar_handle);
    return EPDK_OK;
}


//向下一页处理函数
__s32 ExplorerListWinOnNextPage(explr_list_para_t *list_para)
{
    __s32 ret;
    __s32 PageCount = LISTBAR_GetPageItemCount(list_para->listbar_handle);	//获得一行的条目数

    if(list_para->rat.index + PageCount >= list_para->rat.total)
    {
        return EPDK_FAIL;			//only for test
    }
    ret = rat_move_cursor_forward(list_para->rat.handle, PageCount);

    if(ret == 0)
    {
        if(rat_is_cursor_end(list_para->rat.handle))
        {
            return EPDK_FAIL;
        }
        else
        {
            __wrn("there is a exception in rat!!!\n");
            return EPDK_FAIL;
        }
    }
    /*
    if(list_para->rat.index + ret >= list_para->rat.total)
    {
    	list_para->rat.total = list_para->rat.index;
    	LISTBAR_SetTotalItem(list_para->listbar_handle,list_para->rat.total);
    }
    */
    {
        __listbar_scene_t ListBarScene;
        LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
        __msg("listbar start id = %d\n", ListBarScene.start_id);
        __msg("listbar focus id = %d\n", ListBarScene.focus_id);
    }

    //list_para->rat.index += ret;			//ret为实际向下移动的条目数
    {
        __listbar_scene_t ListBarScene;
        LISTBAR_NextPage(list_para->listbar_handle);

        LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
        __msg("listbar start id = %d\n", ListBarScene.start_id);
        __msg("listbar focus id = %d\n", ListBarScene.focus_id);

        list_para->rat.index = ListBarScene.focus_id;
    }
    //LISTBAR_SetFocusItem(list_para->listbar_handle, list_para->rat.index);
    return EPDK_OK;
}


//向上一页处理函数
__s32 ExplorerListWinOnPreviousPage(explr_list_para_t *list_para)
{
    __s32 ret;
    __s32 PageCount = LISTBAR_GetPageItemCount(list_para->listbar_handle);	//获得一行的条目数

    if(list_para->rat.index <= 0)
    {
        return EPDK_FAIL;
    }
    if(list_para->rat.index < PageCount)		//only for test
    {
        return EPDK_FAIL;
    }

    ret = rat_move_cursor_backward(list_para->rat.handle, PageCount);
    if(ret == 0)
    {
        if(rat_is_cursor_end(list_para->rat.handle))
        {
            return EPDK_FAIL;
        }
        else
        {
            __wrn("there is a exception in rat!!!\n");
            return EPDK_FAIL;
        }
    }

    {
        __listbar_scene_t ListBarScene;
        LISTBAR_PrevPage(list_para->listbar_handle);

        LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
        __msg("listbar start id = %d\n", ListBarScene.start_id);
        __msg("listbar focus id = %d\n", ListBarScene.focus_id);
        list_para->rat.index = ListBarScene.focus_id;
    }

    //LISTBAR_SetFocusItem(list_para->listbar_handle, list_para->rat.index);
    return EPDK_OK;
}


__s32 explorer_create_delete_failed_dialog(H_WIN list_win)
{
    explr_list_para_t *list_para;

    if(list_win == NULL)
    {
        return EPDK_FAIL;
    }
    list_para = (explr_list_para_t *)GUI_WinGetAttr(list_win);


    list_para->del_dlg_open = EPDK_TRUE;
    explorer_draw_delete_file_failed();
    __msg("--------------draw del file dialog!\n");

    //if(GUI_IsTimerInstalled(list_win, ExplorerTimerId))
    //{
    //	GUI_ResetTimer(list_win, ExplorerTimerId, C_EXP_TIMER_INTERVAL, NULL);
    //GUI_KillTimer(list_win, ExplorerTimerId);
    //	__msg("GUI_reset timer\n");
    //}
}

static __s32  file_manager_list_on_delete(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    file_item_t *file_item = NULL;
    int ret = 0;
    __listbar_scene_t ListBarScene;
    char  path[RAT_MAX_FULL_PATH_LEN];

    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    if(list_para == NULL)
    {
        return EPDK_FAIL;
    }
    if(list_para->cur_file_list == NULL)
    {
        return EPDK_FAIL;
    }


    if(list_para->cur_file_list->total < 1)
    {
        return EPDK_FAIL;
    }
    file_item = list_para->cur_file_list->cur_item;
    if(file_item == NULL)
    {
        return EPDK_FAIL;
    }
    eLIBs_memset(path, 0, sizeof(path));
    eLIBs_strcpy(path, list_para->cur_file_list->file_path);
    eLIBs_strcat(path, "\\");
    eLIBs_strcat(path, file_item->name);

    if(file_item->fatdirattr & FSYS_ATTR_DIRECTORY)
    {
        if(file_item->name_len != 0)
        {
            //ret = eLIBs_rmdir(path);
            {
                file_op_info_t op_info;
                eLIBs_memset(&op_info, 0, sizeof(file_op_info_t));
                ret = get_item_num_in_dir(path, &(op_info.total_num));
                if(EPDK_FAIL == ret)
                {
                    return EPDK_FAIL;
                }
                //op_info.OnDraw = explorer_delete_file_draw_process;
                explorer_delete_file_draw_process_ext();
                ret = delete_dir(path, &op_info);
                if(ret != 0)
                {
#if 0
                    {
                        __s32 str[] = {0, STRING_EXPLR_DELETE_FAILED};
                        H_WIN parent = GUI_WinGetParent(msg->h_deswin);
                        //__wait__;
                        default_dialog_timeout(list_para->h_dialog_msg, parent, DELETE_FILE_DIALOG_ID, ADLG_OK, str, 18000);
                    }
#endif
                    //explorer_draw_delete_file_failed();
                    LISTBAR_ShowPage(list_para->listbar_handle);
                    explorer_create_delete_failed_dialog(list_para->list_win);
                    //LISTBAR_ShowPage(list_para->listbar_handle);
                    __here__;
                    return EPDK_FAIL;
                }
            }
        }
    }
    else
    {
        if(file_item->name_len != 0)
        {
            explorer_delete_file_draw_process_ext();
            ret = eLIBs_remove(path);
            if(ret != 0)
            {
#if 0
                {
                    __s32 str[] = {0, STRING_EXPLR_DELETE_FAILED};
                    H_WIN parent = GUI_WinGetParent(msg->h_deswin);
                    //__wait__;
                    default_dialog_timeout(list_para->h_dialog_msg, parent, DELETE_FILE_DIALOG_ID, ADLG_OK, str, 18000);
                }
#endif

                //explorer_draw_delete_file_failed();
                LISTBAR_ShowPage(list_para->listbar_handle);
                explorer_create_delete_failed_dialog(list_para->list_win);
                //LISTBAR_ShowPage(list_para->listbar_handle);
                __here__;
                return EPDK_FAIL;
            }
        }
    }
    __here__;
    explorer_clean_delete_file_hint_area();

    //保护现场
    LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
    list_para->cur_file_list->start_id = ListBarScene.start_id;
    list_para->cur_file_list->focus_id = ListBarScene.focus_id;

    list_para->cur_file_list->total -= 1;
    list_para->rat.total = list_para->cur_file_list->total;			//only for test here

    list_para->last_start_id = ListBarScene.start_id;
    list_para->last_focused_id = ListBarScene.focus_id;
    if(list_para->last_focused_id >= list_para->rat.total)
    {
        if(list_para->rat.total > 1)
        {
            if(list_para->last_start_id > 0)
            {
                list_para->last_start_id--;
            }
            list_para->last_focused_id = list_para->rat.total - 1;
        }
        else
        {
            list_para->last_focused_id = 0;
        }
    }

    ret = delete_file_item_from_list(file_item, list_para->cur_file_list);	// 删除条目

    explorer_listbar_uninit(list_para->list_win);							//重新创建listbar,因为Square为全屏模式
    explorer_listbar_init(list_para->list_win);
    LISTBAR_ShowPage(list_para->listbar_handle);
    return EPDK_OK;
}

//文件管理回到上一级菜单
static __s32  file_manager_list_on_backspace(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    file_list_t *parent_file_list = NULL;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    if(list_para == NULL)
    {
        return EPDK_FAIL;
    }
    if(list_para->cur_file_list == NULL)
    {
        return EPDK_FAIL;
    }

    if(list_para->cur_file_list == list_para->top_file_list)
    {
        __here__;
        parent_file_list = list_para->cur_file_list->parent;

        delete_file_list_nod(list_para->cur_file_list);
        list_para->cur_file_list = parent_file_list;
        if(list_para->cur_file_list)
        {
            list_para->cur_file_list->child = NULL;
        }
        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
        __here__;
        return EPDK_OK;
    }
    if(list_para->cur_file_list->parent == NULL)
    {
        __here__;
        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
        __here__;
        return EPDK_OK;
    }

    parent_file_list = list_para->cur_file_list->parent;

    delete_file_list_nod(list_para->cur_file_list);
    list_para->cur_file_list = parent_file_list;
    if(list_para->cur_file_list)
    {
        list_para->cur_file_list->child = NULL;
    }
    list_para->last_start_id = list_para->cur_file_list->start_id;
    list_para->last_focused_id = list_para->cur_file_list->focus_id;
    list_para->rat.index = list_para->last_focused_id;
    list_para->rat.total = list_para->cur_file_list->total;

    explorer_listbar_uninit(list_para->list_win);
    explorer_listbar_init(list_para->list_win);
    LISTBAR_ShowPage(list_para->listbar_handle);
    __here__;
    return EPDK_OK;
}

//文件管理进入下一级菜单
static __s32  file_manager_list_on_enter_key(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    __listbar_scene_t ListBarScene;
    file_list_t *temp_file_list = NULL;
    char  path[RAT_MAX_FULL_PATH_LEN];
    __u8  fatdirattr;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if(list_para == NULL)
    {
        return EPDK_FAIL;
    }
    if(list_para->cur_file_list == NULL)
    {
        return EPDK_FAIL;
    }
    if(list_para->cur_file_list->cur_item == NULL)
    {
        return EPDK_FAIL;
    }
    fatdirattr = list_para->cur_file_list->cur_item->fatdirattr;
    if((fatdirattr & FSYS_ATTR_DIRECTORY)
            || (fatdirattr == RAT_MEDIA_TYPE_USB_DEVICE)
            || (fatdirattr == RAT_MEDIA_TYPE_SD_DEVICE))
    {
        explorer_clean_delete_file_hint_area();
        explorer_list_long_string_stop_roll(list_para);
        //进入下一级子文件夹
        LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
        list_para->cur_file_list->start_id = ListBarScene.start_id;
        list_para->cur_file_list->focus_id = ListBarScene.focus_id;
        if(list_para->cur_file_list == list_para->top_file_list)
        {
            eLIBs_strcpy(path, list_para->cur_file_list->cur_item->name);
            //eLIBs_strcat(path, "\\");
        }
        else
        {
            eLIBs_strcpy(path, list_para->cur_file_list->file_path);
            eLIBs_strcat(path, "\\");
            eLIBs_strcat(path, list_para->cur_file_list->cur_item->name);
        }

        temp_file_list = new_file_list_nod(path, NULL);
        if(temp_file_list == NULL)
        {
            return EPDK_FAIL;
        }

        list_para->rat.total = temp_file_list->total;			//only for test here
        list_para->cur_file_list->child = temp_file_list;
        temp_file_list->parent = list_para->cur_file_list;
        list_para->cur_file_list = temp_file_list;

        list_para->last_start_id = 0;
        list_para->last_focused_id = 0;
        list_para->rat.index = 0;

        explorer_listbar_uninit(list_para->list_win);		//重新创建listbar,因为Square为全屏模式
        explorer_listbar_init(list_para->list_win);
        LISTBAR_ShowPage(list_para->listbar_handle);

    }
    else
    {
        rat_media_type_t media_type;
        file_item_t *file_item ;

        file_item = get_file_list_item( list_para->cur_file_list, list_para->rat.index );
        media_type = rat_get_file_type( file_item->name );
		__msg("media_type = %d\n", media_type);
        if( /*(media_type == RAT_MEDIA_TYPE_AUDIO) || */(media_type == RAT_MEDIA_TYPE_VIDEO)
                || (media_type == RAT_MEDIA_TYPE_PIC) /*|| (media_type == RAT_MEDIA_TYPE_EBOOK )*/ )
        {
            file_manager_set_current_file_for_play( msg );
        }
        else if(media_type == RAT_MEDIA_TYPE_FIRMWARE)// 固件升级
        {
            if(1)
            {
                file_item_t *file_item = NULL;
                char  path[RAT_MAX_FULL_PATH_LEN] = { 0 };

                __here__ ;
                file_item = list_para->cur_file_list->cur_item;
                //eLIBs_memset(path, 0, sizeof(path));
                eLIBs_strcpy(path, list_para->cur_file_list->file_path);
                eLIBs_strcat(path, "\\");
                eLIBs_strcat(path, file_item->name);
                eLIBs_strcpy(list_para->fw_update_file, path);

                {
                    __s32 str[] = {0, STRING_EXPLR_FW_UPDATE_CONFIRM};
                    H_WIN parent = GUI_WinGetParent(list_para->list_win);
                    default_dialog(list_para->h_fw_update_dlg, parent, FW_UPDATE_DIALOG_ID, ADLG_YESNO, str);
                }

                return EPDK_FAIL;
            }

        }
    }

    return EPDK_OK;
}

static __s32  explorer_list_win_on_enter_key(__gui_msg_t *msg)
{
    __s32 ret;
    H_WIN hExpWin = NULL;
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    ret = ExplorerListWinOnEnterKey(list_para);
    if(EPDK_FAIL == ret)
    {
        return EPDK_OK;
    }

    switch(list_para->media_type)
    {
    case RAT_MEDIA_TYPE_ALL:
        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
        break;
    case RAT_MEDIA_TYPE_PIC:
        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_PHOTO, 0);
        break;
    case RAT_MEDIA_TYPE_AUDIO:
        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MUSIC, 0);
        break;
    case RAT_MEDIA_TYPE_VIDEO:
        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MOVIE, 0);
        break;
    case RAT_MEDIA_TYPE_EBOOK:
        eLIBs_printf("[L%d](%s).\n", __LINE__, __FILE__);
        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_EBOOK, 0);
        break;
    default:
        break;
    }

    hExpWin = GUI_WinGetParent(msg->h_deswin);
    ExplorerSendMessage(hExpWin, GUI_MSG_CLOSE, 0, 0);
    return EPDK_OK;
}
//////aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

static __s32  explorer_list_win_on_command(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    int ret;
    char FileName[256];
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    if (LOWORD(msg->dwAddData1) == DELETE_FILE_DIALOG_ID)
    {
        __here__;
        switch(HIWORD(msg->dwAddData1))
        {
        case ADLG_CMD_EXIT:
            if(list_para->h_dialog != NULL)
            {
                app_dialog_destroy(list_para->h_dialog); 		//to delete dialog
                if (msg->dwAddData2 == ADLG_IDYES)
                {
                    //explorer_list_long_string_stop_roll(list_para);
                    //file_manager_list_on_delete(msg);			//删除一个条目
                    //rat_set_modify_flag_all(1);

                    GetListItemFileFullPath(list_para, list_para->select_id, FileName);
                    //	__log("%s",FileName);

                    explorer_list_long_string_stop_roll(list_para);
                    explorer_delete_file_draw_process_ext();
                    ret = eLIBs_remove(FileName);
                    if(ret == 0)
                    {
                        //explorer_list_long_string_stop_roll(list_para);
                        explorer_clean_delete_file_hint_area();
                        rat_set_modify_flag_all(1);
                        explorer_rat_det_uninit(list_para);
                        explorer_rat_init(list_para);
                        explorer_get_last_para(list_para);
                        explorer_listbar_uninit(list_para->list_win);							//重新创建listbar,因为Square为全屏模式
                        explorer_listbar_init(list_para->list_win);
                        LISTBAR_ShowPage(list_para->listbar_handle);

                    }
                    else
                    {
                        LISTBAR_ShowPage(list_para->listbar_handle);
                        explorer_create_delete_failed_dialog(list_para->list_win);
                    }
                }
                list_para->h_dialog = NULL;
            }
            if(list_para->h_dialog_msg != NULL)
            {
                app_dialog_destroy(list_para->h_dialog_msg); 		//to delete dialog
                list_para->h_dialog_msg = NULL;
                if(list_para->listbar_handle != NULL)
                {
                    LISTBAR_ShowPage(list_para->listbar_handle);
                }
            }
            break;
        case ADLG_CMD_SET_UI:
        {
            //set dialog ui para
            //app_dialog_set_ui(msg->h_srcwin, ADLG_UI_OK, 0, 0);
        }
        break;
        default:
            break;
        }

    }
    else if (LOWORD(msg->dwAddData1) == FW_UPDATE_DIALOG_ID)
    {
        __here__;
        switch(HIWORD(msg->dwAddData1))
        {
        case ADLG_CMD_EXIT:
            if(list_para->h_fw_update_dlg != NULL)
            {
                app_dialog_destroy(list_para->h_fw_update_dlg); 		//to delete dialog
                if (msg->dwAddData2 == ADLG_IDYES)
                {
                    __msg("fw update, file=%s\n", list_para->fw_update_file);
                    dsk_start_fw_update(list_para->fw_update_file);
                }
                else//ADLG_IDNO
                {
                    __msg("fw not update...\n");
                }
                list_para->h_fw_update_dlg = NULL;
            }
            break;
        case ADLG_CMD_SET_UI:
        {
            //set dialog ui para

        }
        break;
        default:
            break;
        }

    }
    return EPDK_OK;
}
/*
************************************************************************************************************************
*Function	:          static __s32  explorer_list_win_key_proc(__gui_msg_t *msg)
*
*Description	:		framewin 响应按键处理
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32  explorer_list_win_key_proc(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    static __u32 last_key = 0;

    static __u8  key_cnt = 0;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
	__msg("msg->dwAddData1 = 0x%x, msg->dwAddData2 = %d\n", msg->dwAddData1, msg->dwAddData2);
    if((KEY_DOWN_ACTION == msg->dwAddData2) || (KEY_REPEAT_ACTION == msg->dwAddData2))
    {
        switch(msg->dwAddData1)
        {
        case GUI_MSG_KEY_RIGHT:
        case GUI_MSG_KEY_LONGRIGHT:
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LONGUP:
            __msg("----explorer list window on UP key----\n");
            //_long_string_stop_roll();

            explorer_list_long_string_stop_roll(list_para);
            ExplorerListWinOnNextItem(list_para);
            if((list_para->select_id) < (list_para->rat.total - 1))
                list_para->select_id++;
            else
                list_para->select_id = 0;
            __log("--next:%d\n", list_para->select_id);
            break;
        case GUI_MSG_KEY_LEFT:
        case GUI_MSG_KEY_LONGLEFT:
        case GUI_MSG_KEY_DOWN:
        case GUI_MSG_KEY_LONGDOWN:
            __msg("----explorer list window on DOWN key----\n");
            explorer_list_long_string_stop_roll(list_para);
            ExplorerListWinOnPreviousItem(list_para);
            if(list_para->select_id > 0)
                list_para->select_id--;
            else
                list_para->select_id = list_para->rat.total - 1;
            __log("--prev:%d\n", list_para->select_id);
            break;
        case GUI_MSG_KEY_VADD:
            //ExplorerListWinOnNextPage(list_para);
            //__wait__;
            if(RAT_MEDIA_TYPE_PIC == list_para->media_type)
            {
                explorer_list_long_string_stop_roll(list_para);
                if(list_para->view_mode == EXPLR_LIST_MODE)
                {
                    ExplorerSetViewMode(list_para, EXPLR_SQUARE_MODE);
                }
                else if(list_para->view_mode == EXPLR_SQUARE_MODE)
                {
                    ExplorerSetViewMode(list_para, EXPLR_LIST_MODE);
                }
            }
            break;
        case GUI_MSG_KEY_VDEC:
            //ExplorerListWinOnPreviousPage(list_para);
            break;
        case GUI_MSG_KEY_ENTER:
            last_key = GUI_MSG_KEY_ENTER;
            eLIBs_printf("[L%d](%s)GUI_MSG_KEY_ENTER.\n", __LINE__, __FILE__);
            if(list_para->media_type == RAT_MEDIA_TYPE_ALL)
            {
                list_para->del_dlg_open = EPDK_FALSE;
                file_manager_list_on_enter_key(msg);
            }
            else
            {
                eLIBs_printf("[L%d](%s).\n", __LINE__, __FILE__);
                explorer_list_long_string_stop_roll(list_para);
                explorer_list_win_on_enter_key(msg);
            }
            break;
        case GUI_MSG_KEY_MENU:
            key_cnt = 0;
            __log("-----jh_dbg1019_5-----\n");
            //case GUI_MSG_KEY_ESCAPE:
            last_key = GUI_MSG_KEY_MENU;
            //explorer_list_long_string_stop_roll(list_para);
            //explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
            break;
        case GUI_MSG_KEY_RISE:
            break;
        case GUI_MSG_KEY_LONGMENU:
            key_cnt++;
            if(key_cnt > 3)
            {
                if(last_key == GUI_MSG_KEY_MENU)
                {
                    if((list_para->media_type == RAT_MEDIA_TYPE_VIDEO) || (list_para->media_type == RAT_MEDIA_TYPE_PIC))
                    {


                        if(list_para->del_dlg_open)
                        {

                            list_para->del_dlg_open = EPDK_FALSE;
                            explorer_clean_delete_file_hint_area();
                            // explorer_draw_file_info(list_para);
                        }
                        
                        {
                            __s32 str[] = {STRING_EXPLR_DELETE_CONFIRM, STRING_EXPLR_DELETE_CONFIRM};
                            H_WIN parent = GUI_WinGetParent(msg->h_deswin);
                            jh_default_dialog(list_para->h_dialog, parent, DELETE_FILE_DIALOG_ID, ADLG_YESNO, str, 512);

                        }
                    }
                }
                last_key = GUI_MSG_KEY_LONGMENU;
            }
            break;
        default:
            break;
        }
    }
    else if(KEY_UP_ACTION == msg->dwAddData2)
    {
        key_cnt = 0;
        switch(msg->dwAddData1)
        {
        case GUI_MSG_KEY_MENU:
            if(last_key == GUI_MSG_KEY_MENU)		//回到上一级菜单.
            {
                __here__;
                explorer_list_long_string_stop_roll(list_para);
                __here__;
                if(list_para->media_type == RAT_MEDIA_TYPE_ALL)
                {
                    __here__;
                    __log("-----jh_dbg1019_2-----\n");
                    file_manager_list_on_backspace(msg);
                    __here__;
                }
                else
                {
                    __log("-----jh_dbg1019_3-----\n");
                    explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
                }
            }
            break;
        //////////jh_dete_file
        case GUI_MSG_KEY_LONGMENU:

#if 0
            //__log("------log0227_1-----%d,%d\n",list_para->last_focused_id,list_para->last_start_id);

            //__log("--display:%d\n",list_para->select_id);
            if(last_key == GUI_MSG_KEY_MENU)
            {
                if((list_para->media_type == RAT_MEDIA_TYPE_VIDEO) || (list_para->media_type == RAT_MEDIA_TYPE_PIC))
                {
#if 0
                    GetListItemFileFullPath(list_para, list_para->select_id, FileName);
                    //	__log("%s",FileName);
                    ret = eLIBs_remove(FileName);
                    if(ret == 0)
                    {
                        explorer_list_long_string_stop_roll(list_para);
                        rat_set_modify_flag_all(1);
                        explorer_rat_det_uninit(list_para);
                        explorer_rat_init(list_para);
                        explorer_get_last_para(list_para);
                        explorer_listbar_uninit(list_para->list_win);							//重新创建listbar,因为Square为全屏模式
                        explorer_listbar_init(list_para->list_win);
                        LISTBAR_ShowPage(list_para->listbar_handle);

                        __log("--det_suc--\n");
                    }
                    else
                        __log("--det_fail--\n");
#endif

                    if(list_para->del_dlg_open)
                    {

                        list_para->del_dlg_open = EPDK_FALSE;
                        explorer_clean_delete_file_hint_area();
                        // explorer_draw_file_info(list_para);
                    }
                    __here__;
                    {
                        __s32 str[] = {STRING_EXPLR_DELETE_CONFIRM, STRING_EXPLR_DELETE_CONFIRM};
                        H_WIN parent = GUI_WinGetParent(msg->h_deswin);
                        jh_default_dialog(list_para->h_dialog, parent, DELETE_FILE_DIALOG_ID, ADLG_YESNO, str, 512);

                    }
                }
            }

#if 0
            if(last_key == GUI_MSG_KEY_MENU)		//删除文件夹或文件
            {
                if(list_para->cur_file_list == NULL)
                {
                    __log("------log0227_2-----\n");
                    return EPDK_OK;
                }
                if(list_para->cur_file_list == list_para->top_file_list)
                {
                    __log("------log0227_3-----\n");
                    return EPDK_OK;					//顶层目录不可以删除
                }
                if(list_para->cur_file_list->total < 1)
                {
                    __log("------log0227_4-----\n");
                    return EPDK_OK;
                }

                //explorer_list_on_timer(msg);
                __here__;
                if(list_para->del_dlg_open)
                {
                    __log("------log0227_5-----\n");
                    list_para->del_dlg_open = EPDK_FALSE;
                    explorer_draw_file_info(list_para);
                    __msg("explorer list destory del file failed dialog\n");
                }
                __here__;
                {
                    __s32 str[] = {STRING_EXPLR_DELETE_CONFIRM, STRING_EXPLR_DELETE_CONFIRM};
                    H_WIN parent = GUI_WinGetParent(msg->h_deswin);
                    __log("------log0227_6-----\n");
                    //__wait__;
                    jh_default_dialog(list_para->h_dialog, parent, DELETE_FILE_DIALOG_ID, ADLG_YESNO, str, 512);
                    //	default_dialog(list_para->h_dialog,parent, DELETE_FILE_DIALOG_ID, ADLG_YESNO, str);
                    //file_manager_list_on_delete(msg);	//删除一个条目
                }
            }
#endif
#endif
            break;
        }
    }
    return EPDK_OK;
}
/*
************************************************************************************************************************
*Function	:          static __s32 draw_list_vacant_item(__lbar_draw_para_t *draw_param)
*
*Description	:		画空白条目
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 draw_list_vacant_item(__lbar_draw_para_t *draw_param)
{
    GUI_RECT disp_rect;
    //char *filename;
    //int picX, picY, picW, picH;		//picture X, Y, width, height
    //void *pic_buf;

    //explr_list_para_t *list_para;
    //list_para = (explr_list_para_t *)draw_param->attr;		//for draw the picture  in different media type

    //GUI_LyrWinSel(list_para->list_layer);

    //GUI_SetBkColor(GUI_LIGHTGREEN);
    //GUI_SetColor(GUI_RED);

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);

    __msg("Vacant item id =: %d\n", draw_param->index);

    disp_rect.x0 = draw_param->rect.x;
    disp_rect.y0 = draw_param->rect.y;
    disp_rect.x1 = draw_param->rect.x + draw_param->rect.width;
    disp_rect.y1 = draw_param->rect.y + draw_param->rect.height;
    GUI_ClearRect(disp_rect.x0, disp_rect.y0, disp_rect.x1, disp_rect.y1);
    return EPDK_OK;
}
/*
************************************************************************************************************************
*Function	:          static __s32 ExtractFileName(char *FileName, char *FilePath)
*
*Description	:		从全路径中获取文件名
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 ExtractFileName(char *FileName, char *FilePath)
{
    char *pFileName;
    pFileName = eLIBs_strchrlast(FilePath, '\\');
    pFileName++;
    eLIBs_strcpy(FileName, pFileName);

    return EPDK_OK;
}

//目前使用ItemInfo.format返回mediatype
rat_media_type_t GetListItemFileMediaType(HRAT rat_handle, __s32 ItemIndex)
{

    int ret;
    rat_entry_t ItemInfo;
    eLIBs_memset(&ItemInfo, 0, sizeof(rat_entry_t));
    ret = rat_get_item_info_by_index(rat_handle, ItemIndex, &ItemInfo);

    return ItemInfo.Format;
}
/*
************************************************************************************************************************
*Function	:          static __s32 GetListItemFileFullPath(explr_list_para_t *list_para, __s32 ItemIndex, char *FilePath)
*
*Description	:		根据索引号获得条目文件名全路径
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 GetListItemFileFullPath(explr_list_para_t *list_para, __s32 ItemIndex, char *FilePath)
{
    int ret;
    rat_entry_t ItemInfo;

    eLIBs_memset(&ItemInfo, 0, sizeof(rat_entry_t));

    ret = rat_get_item_info_by_index(list_para->rat.handle, ItemIndex, &ItemInfo);
    if(ret == EPDK_FAIL)
    {
        __wrn("get file information form rat fail!!");
        return EPDK_FAIL;
    }
    eLIBs_strcpy(FilePath, ItemInfo.Path);			//不去操作rat 内存

    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:         static __s32 GetListItemFileName(explr_list_para_t *list_para, __s32 ItemIndex, char *FileName)
*
*Description	:		根据索引号获得条目文件名(只有文件名，供显示)
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 GetListItemFileName(explr_list_para_t *list_para, __s32 ItemIndex, char *FileName)
{
    int ret;
    char FilePath[RAT_MAX_FULL_PATH_LEN];
    eLIBs_memset(FilePath, 0, sizeof(FilePath));

    ret = GetListItemFileFullPath(list_para, ItemIndex, FilePath);
    if(ret == EPDK_FAIL)
    {
        return EPDK_FAIL;
    }

    ExtractFileName(FileName, FilePath);
    return EPDK_OK;
}
/*
************************************************************************************************************************
*Function	:         static __s32 draw_miniature_view_item(__lbar_draw_para_t *draw_param)
*
*Description	:		画缩略图in square mode
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 draw_miniature_view_item(__lbar_draw_para_t *draw_param)
{
    explr_list_para_t *list_para;

    char FilePath[RAT_MAX_FULL_PATH_LEN];
    int ret = 0;
    void *pBuf = NULL;
    //explorer_viewer_ui_t* ui_param;

    rat_miniature_para_t in_para;
    rat_pic_info_t out_para;
    RECT miniature_rect;

    list_para = (explr_list_para_t *)draw_param->attr;		//for draw the picture  in different media type
    if(!(list_para->media_type == RAT_MEDIA_TYPE_PIC))		//图片时才有缩略图
    {
        return EPDK_FAIL;
    }

    if(list_para->view_mode == EXPLR_LIST_MODE)
    {
        return EPDK_FAIL;
    }

    eLIBs_memset(FilePath, 0, sizeof(FilePath));
    ret = GetListItemFileFullPath(list_para, draw_param->index, FilePath);
    if(ret == EPDK_FAIL)
    {
        return EPDK_FAIL;
    }

    ret = explorer_get_item_miniature_rect(list_para, &miniature_rect);

    eLIBs_strcpy(in_para.file, FilePath);
    in_para.format = PIXEL_COLOR_ARGB8888;
    in_para.width = miniature_rect.width;
    in_para.height = miniature_rect.height;
    in_para.mode = 0;	// 1为拉伸模式


    pBuf = esMEMS_Balloc(in_para.width * in_para.height * 4);	//ARGB8888,因此4倍大小
    if(pBuf == NULL)										//可以放到开始申请，退出时释放
    {
        __wrn("memory is not enough!!");
        return EPDK_FAIL;
    }

    //rat_start_miniature_decode();

    eLIBs_memset(&out_para, 0, sizeof(out_para));
    eLIBs_memset(pBuf, 0, in_para.width * in_para.height * 4);
    out_para.miniature.buf = pBuf;

    ret = rat_get_pic_info(&in_para, &out_para);
	__msg("draw_miniature_view_item, ret = %d\n", ret);
    //rat_stop_miniature_decode();
    if (EPDK_OK == ret)
    {
        FB fb;
        eLIBs_memset(&fb, 0, sizeof(FB));

        fb.size.width = in_para.width;
        fb.size.height = in_para.height;
        fb.addr[0] = out_para.miniature.buf ;
        fb.fmt.type = FB_TYPE_RGB;
        fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
        fb.fmt.fmt.rgb.br_swap = 0;
        fb.fmt.fmt.rgb.pixseq = (__rgb_seq_t)0;
        GUI_BitString_DrawEx(&fb, 	miniature_rect.x + draw_param->rect.x, miniature_rect.y + draw_param->rect.y);
    }
    else		// 获取缩略图失败
    {
        void *p_bmp_buf ;
        __msg( "ge miniature fail \n" );
        p_bmp_buf = explorer_get_listview_icon_res( ID_EXP_ERROR_BMP ) ;
        if( !p_bmp_buf )
        {
            __msg("get error bmp res fail\n" );
        }
        GUI_BMP_Draw(p_bmp_buf , miniature_rect.x + draw_param->rect.x, miniature_rect.y + draw_param->rect.y );
    }
    esMEMS_Bfree(pBuf, in_para.width * in_para.height * 4);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:        static __s32 draw_listview_item_icon(__lbar_draw_para_t *draw_param)
*
*Description	:		画item icon图标(Item左边的Icon,或者是缩略图Icon)
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 draw_listview_item_icon(__lbar_draw_para_t *draw_param, rat_media_type_t media_type)
{
    RECT IconRect;
    void *pic_buf = NULL;
    //int ret;
    __s32 picX = 0, picY = 0;
    explr_list_para_t *this = NULL;


    this = (explr_list_para_t *)draw_param->attr;
    explorer_get_item_icon_rect(this, &IconRect);

    if(draw_param->mode == LBAR_MODE_NORMAL)
    {
        pic_buf = explorer_get_file_item_icon(this->view_mode, media_type, UNFOCUSED);
    }
    else if(draw_param->mode == LBAR_MODE_FOCUS)
    {
        pic_buf = explorer_get_file_item_icon(this->view_mode, media_type, FOCUSED);
    }

    if(pic_buf != NULL)
    {
        picX = draw_param->rect.x + IconRect.x;
        picY = draw_param->rect.y + IconRect.y;
		__msg("picX = %d, picY = %d\n", picX, picY);
        GUI_BMP_Draw(pic_buf, picX, picY);
    }
    return EPDK_OK;
}
/*
************************************************************************************************************************
*Function	:       static __s32 draw_square_item_focus_icon(__lbar_draw_para_t *draw_param)
*
*Description	:		画缩略图获得焦点时显示的图标
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 draw_square_item_focus_icon(__lbar_draw_para_t *draw_param)
{
    RECT FocusIconRect;
    //void *pic_buf;
    int ret;
    //__s32 picX, picY;
    __s32 X0, Y0, X1, Y1;
    explr_list_para_t *list_para;

    list_para = (explr_list_para_t *)draw_param->attr;

    ret = explorer_get_item_focus_icon_rect(list_para, &FocusIconRect);

	__msg("draw_square_item_focus_icon\n");
    //画焦点方框
    GUI_SetColor(GUI_YELLOW);
    X0 = draw_param->rect.x + FocusIconRect.x;
    Y0 = draw_param->rect.y + FocusIconRect.y;
    X1 = X0 + FocusIconRect.width;
    Y1 = Y0 + FocusIconRect.height;
    GUI_DrawLine(X0, Y0, X1, Y0);
    GUI_DrawLine(X0, Y0 + 1, X1, Y0 + 1);
    GUI_DrawLine(X0, Y0 + 2, X1, Y0 + 2);

    GUI_DrawLine(X0, Y0, X0, Y1);
    GUI_DrawLine(X0 + 1, Y0, X0 + 1, Y1);
    GUI_DrawLine(X0 + 2, Y0, X0 + 2, Y1);

    GUI_DrawLine(X0, Y1, X1, Y1);
    GUI_DrawLine(X0, Y1 - 1, X1, Y1 - 1);
    GUI_DrawLine(X0, Y1 - 2, X1, Y1 - 2);

    GUI_DrawLine(X1, Y0, X1, Y1);
    GUI_DrawLine(X1 - 1, Y0, X1 - 1, Y1);
    GUI_DrawLine(X1 - 2, Y0, X1 - 2, Y1);

    return ret;
}

//Draw focus icon, 可将上下两个函数优化为一个函数
static __s32 draw_list_item_unfocus_icon(__lbar_draw_para_t *draw_param)
{
    RECT FocusIconRect;
    void *pic_buf;
    int ret;
    __s32 picX, picY;
    explr_list_para_t *list_para;
	__msg("draw_list_item_unfocus_icon\n");
    list_para = (explr_list_para_t *)draw_param->attr;

    ret = explorer_get_item_focus_icon_rect(list_para, &FocusIconRect);

    pic_buf = explorer_get_listview_icon_res(ID_EXP_LIST_ITEM_UNFOCUS_BG);			//draw select border
    picX = draw_param->rect.x + FocusIconRect.x;
    picY = draw_param->rect.y + FocusIconRect.y;
	
    GUI_BMP_Draw(pic_buf, picX, picY);
    return ret;
}


//Draw focus icon, 可将上下两个函数优化为一个函数
static __s32 draw_list_item_focus_icon(__lbar_draw_para_t *draw_param)
{
    RECT FocusIconRect;
    void *pic_buf;
    int ret;
    __s32 picX, picY;
    explr_list_para_t *list_para;
	__msg("draw_list_item_focus_icon\n");
    list_para = (explr_list_para_t *)draw_param->attr;

    ret = explorer_get_item_focus_icon_rect(list_para, &FocusIconRect);

    pic_buf = explorer_get_listview_icon_res(ID_EXP_LIST_ITEM_FOCUS_BG);			//draw select border
	__msg("FocusIconRect.x = %d, FocusIconRect.y = %d\n", FocusIconRect.x, FocusIconRect.y);
	
	__msg("x = %d, y = %d, w = %d, h = %d\n", draw_param->rect.x, draw_param->rect.y, draw_param->rect.width, draw_param->rect.height);
    picX = draw_param->rect.x + FocusIconRect.x;
    picY = draw_param->rect.y + FocusIconRect.y;
	__msg("picX = %d, picY = %d\n", picX, picY);
    GUI_BMP_Draw(pic_buf, picX, picY);
    return ret;
}
/*
************************************************************************************************************************
*Function	:       static __s32 draw_listview_item_text(__lbar_draw_para_t *draw_param)
*
*Description	:	 显示条目文本
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 draw_listview_item_text(__lbar_draw_para_t *draw_param)
{
    int ret;
    explr_list_para_t *list_para;
    char FileName[RAT_MAX_FULL_PATH_LEN];
    char *name = NULL;
    GUI_RECT GUI_TextRect;
    RECT TextRect;
    file_item_t *file_item = NULL;

    list_para = (explr_list_para_t *)draw_param->attr;

    if(list_para->media_type == RAT_MEDIA_TYPE_ALL)
    {
        file_item = get_file_list_item(list_para->cur_file_list, draw_param->index);
        if(file_item != NULL)
        {
            list_para->file_item = file_item;
            if((file_item->fatdirattr == RAT_MEDIA_TYPE_USB_DEVICE)
                    || (file_item->fatdirattr == RAT_MEDIA_TYPE_SD_DEVICE))
            {
                get_root_device_disp_name(FileName, file_item->fatdirattr);
            }
            else
            {
                eLIBs_strcpy(FileName, file_item->name);
            }
			__msg("draw_listview_item_text: FileName: %s\n", FileName);
			
            if(draw_param->mode == LBAR_MODE_FOCUS)
            {
                list_para->cur_file_list->cur_item = file_item;		//保存当前条目信息
            }
        }
        else
        {
            __msg("error in file list\n");
            return EPDK_FAIL;
        }
    }
    else
    {
        eLIBs_memset(FileName, 0, sizeof(FileName));
        ret = GetListItemFileName(list_para, draw_param->index, FileName);
        if(ret == EPDK_FAIL)
        {
            __wrn("get file information form rat fail!!");
            return EPDK_FAIL;
        }
    }

    ret = explorer_get_item_text_rect(list_para, &TextRect);

    GUI_SetFont(SWFFont);
    //GUI_SetFont(GUI_GetDefaultFont());
    GUI_UC_SetEncodeUTF8();
    //GUI_SetBkColor(GUI_LIGHTGREEN);
    GUI_SetColor(GUI_WHITE);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);

    GUI_TextRect.x0 = draw_param->rect.x  + TextRect.x;
    GUI_TextRect.y0 = draw_param->rect.y  + TextRect.y;
    GUI_TextRect.x1 = draw_param->rect.x  + (TextRect.x + TextRect.width);
    GUI_TextRect.y1 = draw_param->rect.y  + (TextRect.y + TextRect.height);

    GUI_DispStringInRect(FileName, &GUI_TextRect, GUI_TA_VCENTER);

    if(draw_param->mode == LBAR_MODE_FOCUS)
    {
        explorer_list_long_string_start_roll_fast(draw_param, FileName);
    }

    return EPDK_OK;
}

__s32 explorer_draw_file_info(explr_list_para_t *list_para)
{
    rat_media_type_t media_type;
    char filename[RAT_MAX_FULL_PATH_LEN];
    file_item_t *cur_item = NULL;
    if(list_para == NULL)
    {
        return EPDK_FAIL;
    }
	return EPDK_OK;
    cur_item = list_para->cur_file_list->cur_item;
    if(cur_item == NULL)
    {
        return EPDK_FAIL;
    }
	__msg("explorer_draw_file_info\n");
    media_type = get_file_list_item_file_type(cur_item);
    explorer_draw_FileTypeIcon(list_para, media_type);

    if((media_type & FSYS_ATTR_DIRECTORY)
            || (media_type == RAT_MEDIA_TYPE_SD_DEVICE)
            || (media_type == RAT_MEDIA_TYPE_USB_DEVICE))
    {
        explorer_clear_filesize_area();
        explorer_clear_file_create_time_area();
    }
    else		//文件属性时显示文件信息
    {
        explorer_draw_FileSize(cur_item);
        eLIBs_strcpy(filename, list_para->cur_file_list->file_path);
        eLIBs_strcat(filename, "\\");
        eLIBs_strcat(filename, cur_item->name);
        explorer_draw_file_create_time(filename);
    }
    return EPDK_OK;
}
//draw item in list mode, 不要在Listbar的回调函数中选择图层
static __s32 draw_listview_item(__lbar_draw_para_t *draw_param)
{
    //int ret = 0;
    explr_list_para_t *list_para;
    rat_media_type_t media_type;

    __msg("listbar item id =: %d\n", draw_param->index);		//focus id
    list_para = (explr_list_para_t *)draw_param->attr;				//for draw the picture  in different media type

    if(draw_param->mode == LBAR_MODE_FOCUS)
    {
        if(list_para->view_mode == EXPLR_LIST_MODE)
        {
        	__msg("draw_list_item_focus_icon\n");
            draw_list_item_focus_icon(draw_param);		//画focus item 背景框图
        }
        else if(list_para->view_mode == EXPLR_SQUARE_MODE)
        {
        	__msg("draw_square_item_focus_icon");
            draw_square_item_focus_icon(draw_param);
        }
    }
	else
	{
		draw_list_item_unfocus_icon(draw_param);
	}

    if((list_para->view_mode == EXPLR_SQUARE_MODE)
            && (list_para->media_type == RAT_MEDIA_TYPE_PIC))
    {
    	__msg("explorer_list_long_string_stop_roll\n");
        explorer_list_long_string_stop_roll(list_para);		//停止长文件名滚动
    }
    else
    {

        draw_listview_item_text(draw_param);
        if(list_para->media_type != RAT_MEDIA_TYPE_ALL)
        {
        	__msg("GetListItemFileMediaType\n");
            media_type = GetListItemFileMediaType(list_para->rat.handle, draw_param->index);
        }
        else
        {
        	__msg("get_file_list_item_file_type\n");
            media_type = get_file_list_item_file_type(list_para->file_item);
        }
        {
            draw_listview_item_icon(draw_param, media_type);
        }
    }

    if(draw_param->mode == LBAR_MODE_FOCUS)
    {
        if(list_para->media_type == RAT_MEDIA_TYPE_ALL)		//文件管理
        {
            explorer_draw_file_info(list_para);
        }
    }

    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:       static __s32 _explorer_listbar_item_draw(__lbar_draw_para_t *draw_param)
*
*Description	:	 ListBar绘制消息处理函数
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 _explorer_listbar_item_draw(__lbar_draw_para_t *draw_param)
{

    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)draw_param->attr;		//for draw the picture  in different media type

    if(list_para->rat.total <= 0)
    {
        return EPDK_FAIL;
    }

    switch(draw_param->mode)
    {
    case LBAR_MODE_NORMAL:
        //__msg("~~~~draw listbar in normal mode!~~~~\n");
        draw_listview_item(draw_param);
        break;
    case LBAR_MODE_FOCUS:
        //__msg("~~~~draw focus item in listbar!~~~~\n");
        //GUI_LyrWinCacheOn();
        draw_listview_item(draw_param);
        draw_miniature_view_item(draw_param);
        //GUI_LyrWinCacheOff();
        break;
    case LBAR_MODE_INVALID:
        //__msg("~~~~draw invalid item in listbar!~~~~\n");
        break;
    case LBAR_MODE_VACANT:
        //__msg("~~~~draw vacant item in listbar!~~~~\n");
        //draw_list_vacant_item(draw_param);
        break;
    case LBAR_MODE_DIFFUSE:
        //__msg("listbar item id =: %d\n", draw_param->index);		//focus id
        //draw_listview_item(draw_param);
        draw_miniature_view_item(draw_param);
        //__msg("~~~~draw diffuse item in listbar!~~~~\n");
        break;
    }
    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:       void explorer_listbar_init(H_WIN  list_win)
*
*Description	:	 ListBar 初始化函数,注意在这之前Rat必须已经过初始化
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static void explorer_listbar_init(H_WIN  list_win)
{
    explr_list_para_t *list_para;
    __listbar_config_t config;
    __scroll_bar_t scroll_param;						//new scroll bar
    __s32 item_width = 0, item_height = 0;

    explorer_viewer_ui_t *ui_param = explorer_get_viewer_ui_param();			//获得整个AP的UI设计参数


    eLIBs_memset(&config, 0, sizeof(__listbar_config_t));
    eLIBs_memset(&scroll_param, 0, sizeof(__scroll_bar_t));

    list_para = (explr_list_para_t *)GUI_WinGetAttr(list_win);
    __msg("Listbar is initializing, list_para->view_mode = %d\n", list_para->view_mode);

    if(list_para->view_mode == EXPLR_LIST_MODE)
    {
        item_width = ui_param->list_item_ui_param.item_rect.width;						//根据list or square模式不同
        item_height = ui_param->list_item_ui_param.item_rect.height;
        config.list_rect.x = ui_param->list_item_ui_param.ListBarArea.x;			//ui_param->menu == listbar area
        config.list_rect.y = ui_param->list_item_ui_param.ListBarArea.y;
        config.list_rect.width = ui_param->list_item_ui_param.ListBarArea.width;
        config.list_rect.height = ui_param->list_item_ui_param.ListBarArea.height;
    }
    else if(list_para->view_mode == EXPLR_SQUARE_MODE)
    {
        item_width = ui_param->square_item_ui_param.item_rect.width;
        item_height = ui_param->square_item_ui_param.item_rect.height;
        config.list_rect.x = ui_param->square_item_ui_param.ListBarArea.x;			//ui_param->menu == listbar area
        config.list_rect.y = ui_param->square_item_ui_param.ListBarArea.y;
        config.list_rect.width = ui_param->square_item_ui_param.ListBarArea.width;
        config.list_rect.height = ui_param->square_item_ui_param.ListBarArea.height;
    }

    config.item_width = item_width;
    config.item_height = item_height;				//these are numbers only for test
    config.start_id = list_para->last_start_id;
    //config.focus_id = list_para->rat.index; 				//removed by libaiao, 2010.6.6
    config.focus_id = list_para->last_focused_id;
    config.bk_color = 0;
    config.alpha_status = 1;						//打开Alpha开关
    config.list_attr = (void *)list_para;
    config.item_cnt = list_para->rat.total;   //Rat.total必须已经获得

    scroll_param.show_rect.x = ui_param->scroll_bg_rect.x;
    scroll_param.show_rect.y = ui_param->scroll_bg_rect.y;
    scroll_param.show_rect.width = ui_param->scroll_bg_rect.width;
    scroll_param.show_rect.height = ui_param->scroll_bg_rect.height;		//整个Scoll bar显示区域，Head, body,tail

    scroll_param.scroll_rect.x = ui_param->scroll_rect.x;
    scroll_param.scroll_rect.y = ui_param->scroll_rect.y;
    scroll_param.scroll_rect.width = ui_param->scroll_rect.width;
    scroll_param.scroll_rect.height = ui_param->scroll_rect.height;			//滚动条区域(除去head,body,tail)

    scroll_param.head_height = ui_param->scroll_head_height;
    scroll_param.tail_height = ui_param->scroll_tail_height;				//head, tail

    scroll_param.back_bmp = explorer_get_listview_icon_res(ID_EXP_LIST_SCROLL_BG);
    scroll_param.head_bmp = explorer_get_listview_icon_res(ID_EXP_LIST_SCROLL_HEAD);
    scroll_param.body_bmp = explorer_get_listview_icon_res(ID_EXP_LIST_SCROLL_BODY);
    scroll_param.tail_bmp = explorer_get_listview_icon_res(ID_EXP_LIST_SCROLL_TAIL);

    list_para->listbar_handle = LISTBAR_Create(list_win);    				//new listbar
    LISTBAR_Config(list_para->listbar_handle, _explorer_listbar_item_draw, &config);    //config parametters for listbar
    LISTBAR_ScrollConfig(list_para->listbar_handle, &scroll_param); 	//scrollbar belongs to listbar
    //LISTBAR_ShowPage(list_para->listbar_handle);

}

/*
************************************************************************************************************************
*Function	:       void explorer_listbar_uninit(H_WIN  list_win)
*
*Description	:	 释放Listbar
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static void explorer_listbar_uninit(H_WIN  list_win)
{
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(list_win);
    if(list_para->listbar_handle != NULL)
    {
        __msg("----LISTBAR_Delete begin: %x\n", list_para->listbar_handle);
        LISTBAR_SuspendPage(list_para->listbar_handle);
        LISTBAR_Delete(list_para->listbar_handle);
        list_para->listbar_handle = NULL;
    }
    return;
}

/*
************************************************************************************************************************
*Function	:       void ExplorerSetViewMode(explr_list_para_t *list_para, explorer_view_mode_e view_mode)
*
*Description	:	 改变Explorer显示方式List mode or square mode
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
void ExplorerSetViewMode(explr_list_para_t *list_para, explorer_view_mode_e view_mode)
{

    if(view_mode == EXPLR_LIST_MODE)
    {
        list_para->view_mode = EXPLR_LIST_MODE;
    }
    else if(view_mode == EXPLR_SQUARE_MODE)
    {
        list_para->view_mode = EXPLR_SQUARE_MODE;
    }

    explorer_save_last_para( list_para );
    explorer_get_last_para( list_para) ;

    explorer_listbar_uninit(list_para->list_win);		//重新创建listbar,因为Square为全屏模式
    GUI_LyrWinSel(list_para->list_layer);
    GUI_SetBkColor(0);
    GUI_Clear();
    explorer_listbar_init(list_para->list_win);
    //rat_move_cursor_to_first(list_para->rat.handle);
    LISTBAR_ShowPage(list_para->listbar_handle);
    //LISTBAR_ChangeMode(	list_para->listbar_handle,item_width,item_height);
    //LISTBAR_UpdatePage(list_para->listbar_handle);
    if(list_para->view_mode == EXPLR_LIST_MODE)
    {
    	__msg("ExplorerSetViewMode\n");
        explorer_draw_FileTypeIcon(list_para, list_para->media_type);
    }
    return;
}

//创建文件列表顶部断点
static __s32 explorer_file_list_init(explr_list_para_t *list_para)
{
    __u32 total = 0;
    if(list_para == NULL)
    {
        return EPDK_FAIL;
    }
	__msg("explorer_file_list_init\n");
    if(list_para->top_file_list == NULL)
    {
#if 1
        list_para->top_file_list = new_file_root_list_nod(NULL);
        if(list_para->top_file_list == NULL)
        {
            return EPDK_FAIL;
        }
        list_para->cur_file_list = list_para->top_file_list;
        list_para->rat.total = list_para->cur_file_list->total;		//only for test, for listbar initial
#else
        list_para->top_file_list = new_file_list_nod(list_para->search_path, NULL);
        if(list_para->top_file_list == NULL)
        {
            return EPDK_FAIL;
        }
        list_para->cur_file_list = list_para->top_file_list;
        list_para->rat.total = list_para->cur_file_list->total;		//only for test, for listbar initial
#endif
    }
    rat_start_miniature_decode();	//开始缩略图线程
    return EPDK_OK;
}


//删除整个文件列表
static __s32 explorer_file_list_uninit(explr_list_para_t *list_para)
{
    if(list_para->top_file_list != NULL)
    {
        __here__;
        delete_file_list_chain(list_para->top_file_list);
        __here__;
        list_para->cur_file_list = NULL;
    }
    __here__;
    rat_stop_miniature_decode();	//停止 缩略图线程
    __here__;
    return EPDK_FAIL;
}

/*
************************************************************************************************************************
*Function	:       static __s32 explorer_rat_init(explr_list_para_t *list_para)
*
*Description	:	 搜索文件Module, Rat 初始化
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 explorer_rat_init(explr_list_para_t *list_para)
{
    /*
    {
    	file_item_t *item_list = NULL;
    	__u32 total = 0;
    	//__wait__;
    	total = new_file_list(list_para->search_path,&item_list, NULL);
    	delete_file_list(item_list,total);
    }
    */
    if(list_para->rat.handle == NULL)
    {
        __log("************jhdbg0304-1************\n");
        list_para->rat.handle = rat_open(list_para->search_path, list_para->media_type, 0);
        if(list_para->rat.handle == NULL)
        {
            __wrn("open rat failed\n");
        }
        else
        {
            __msg("open rat successed\n");
        }
    }
    else
    {
        __log("************jhdbg0304-2************\n");
        __wrn("!!!!!!!!!!!!!please dont open rat again!!!!!!!!!!!!!!!!!\n");
    }

    if(list_para->rat.handle == NULL)
    {
        __wrn("!!!!!!!!!!!!!open rat fail!!!!!!!!!!!!!!!!!\n");
        return EPDK_FAIL;
    }


    list_para->rat.index = 0;
    //rat_move_cursor_to_first(list_para->rat.handle);
    list_para->rat.total = rat_get_cur_scan_cnt(list_para->rat.handle);;
    //list_para->rat.total = rat_get_total_number(list_para->rat.handle);

    rat_start_miniature_decode();	//开始缩略图线程
    return EPDK_OK;
}
/*
************************************************************************************************************************
*Function	:      static __s32 explorer_rat_uninit(explr_list_para_t *list_para)
*
*Description	:	 释放rat
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/

static __s32 explorer_rat_uninit(explr_list_para_t *list_para)
{
    if(list_para->rat.handle != NULL)
    {
        //rat_close(list_para->rat.handle);
    }
    rat_stop_miniature_decode();
    return EPDK_OK;
}

static __s32 explorer_rat_det_uninit(explr_list_para_t *list_para)
{
    if(list_para->rat.handle != NULL)
    {
        rat_close(list_para->rat.handle);
        list_para->rat.handle = NULL;
    }
    rat_stop_miniature_decode();
    return EPDK_OK;
}

static __s32 explorer_get_last_para(explr_list_para_t *list_para)
{
    __u32 page_item_cnt = 0; 				//每页的条目总数
    char FileName[RAT_MAX_FULL_PATH_LEN];
    int ret = 0;
    reg_root_para_t *root_para = NULL;

    //page_item_cnt = LISTBAR_GetPageItemCount(list_para->listbar_handle);
    page_item_cnt = explorer_get_listbar_pagenum(list_para);
    root_para = dsk_reg_get_para_by_app(REG_APP_ROOT);
    list_para->select_id = 0;
    if(list_para->rat.handle != NULL)
    {
        switch(list_para->media_type)
        {
        case RAT_MEDIA_TYPE_ALL:
            list_para->last_start_id = 0;
            break;
        case RAT_MEDIA_TYPE_PIC:
            //list_para->last_start_id = photo_start_id;
            if(list_para->root_type == RAT_TF)
            {
                list_para->last_start_id = root_para->last_photo_start_index_sd;
                list_para->last_focused_id = root_para->last_photo_index_sd;
                list_para->last_filename = root_para->last_photo_path_sd;
            }
            else if(list_para->root_type == RAT_USB)
            {
                list_para->last_start_id = root_para->last_photo_start_index_ud;
                list_para->last_focused_id = root_para->last_photo_index_ud;
                list_para->last_filename = root_para->last_photo_path_ud;
            }
            break;
        case RAT_MEDIA_TYPE_AUDIO:
            if(list_para->root_type == RAT_TF)
            {
                list_para->last_start_id = root_para->last_music_start_index_sd;
                list_para->last_focused_id = root_para->last_music_index_sd;
                list_para->last_filename = root_para->last_music_path_sd;
            }
            else if(list_para->root_type == RAT_USB)
            {
                list_para->last_start_id = root_para->last_music_start_index_ud;
                list_para->last_focused_id = root_para->last_music_index_ud;
                list_para->last_filename = root_para->last_music_path_ud;
            }
            break;
        case RAT_MEDIA_TYPE_VIDEO:
            if(list_para->root_type == RAT_TF)
            {
                list_para->last_start_id = root_para->last_movie_start_index_sd;
                list_para->last_focused_id = root_para->last_movie_index_sd;
                list_para->last_filename = root_para->last_movie_path_sd;
            }
            else if(list_para->root_type == RAT_USB)
            {
                list_para->last_start_id = root_para->last_movie_start_index_ud;
                list_para->last_focused_id = root_para->last_movie_index_ud;
                list_para->last_filename = root_para->last_movie_path_ud;
            }
            break;
        case RAT_MEDIA_TYPE_EBOOK:
            if(list_para->root_type == RAT_TF)
            {
                list_para->last_start_id = root_para->last_ebook_start_index_sd;
                list_para->last_focused_id = root_para->last_ebook_index_sd;
                list_para->last_filename = root_para->last_ebook_path_sd;
            }
            else if(list_para->root_type == RAT_USB)
            {
                list_para->last_start_id = root_para->last_ebook_start_index_ud;
                list_para->last_focused_id = root_para->last_ebook_index_ud;
                list_para->last_filename = root_para->last_ebook_path_ud;
            }
            break;
        default:
            break;
        }

        if((list_para->last_focused_id < list_para->rat.total)
                && (list_para->last_start_id < list_para->rat.total))
        {
            eLIBs_memset(FileName, 0, sizeof(FileName));
            GetListItemFileFullPath(list_para, list_para->last_focused_id, FileName);
            list_para->select_id = list_para->last_focused_id;

            __log("jh--%s----%d:%d\n", FileName, list_para->last_focused_id, list_para->rat.total);
            ret = eLIBs_strcmp(FileName, list_para->last_filename);
            if(ret == 0)
            {
                if(list_para->last_focused_id < list_para->last_start_id)
                {
                    list_para->last_start_id = list_para->last_focused_id;
                    list_para->rat.index = list_para->last_focused_id;			//定位到第一行起始显示
                }
                else if((list_para->last_focused_id - list_para->last_start_id) >= page_item_cnt)
                {
                    list_para->last_start_id = list_para->last_focused_id;		//
                    list_para->rat.index = list_para->last_focused_id;			//定位到第一行起始显示
                }
                else
                {
                    list_para->rat.index = list_para->last_focused_id;//相等则定位到上一次浏览的文件
                }
                return EPDK_OK;
            }
        }

    }
    list_para->last_start_id = 0;
    list_para->last_focused_id = 0;
    list_para->rat.index = 0;
    return EPDK_OK;
}


static __s32 explorer_save_last_para(explr_list_para_t *list_para)
{
    __s32 ret = 0;
    reg_app_e AppId = 0;
    char FileName[RAT_MAX_FULL_PATH_LEN];
    __listbar_scene_t ListBarScene = {0, 0};
    reg_root_para_t *root_para = NULL;

    root_para = dsk_reg_get_para_by_app(REG_APP_ROOT);

    //__wait__;
    LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
    eLIBs_memset(FileName, 0, sizeof(FileName));

    switch(list_para->media_type)
    {
    case RAT_MEDIA_TYPE_VIDEO:
        AppId = REG_APP_MOVIE;
        //movie_start_id = ListBarScene.start_id;
        if(list_para->root_type == RAT_TF)
        {
            root_para->last_movie_start_index_sd = ListBarScene.start_id;
        }
        else if(list_para->root_type == RAT_USB)
        {
            root_para->last_movie_start_index_ud = ListBarScene.start_id;
        }
        break;
    case RAT_MEDIA_TYPE_PIC:
        AppId = REG_APP_PHOTO;
        if(list_para->root_type == RAT_TF)
        {
            root_para->last_photo_start_index_sd = ListBarScene.start_id;
        }
        else if(list_para->root_type == RAT_USB)
        {
            root_para->last_photo_start_index_ud = ListBarScene.start_id;
        }
        break;
    case RAT_MEDIA_TYPE_AUDIO:
        AppId = REG_APP_MUSIC;
        if(list_para->root_type == RAT_TF)
        {
            root_para->last_music_start_index_sd = ListBarScene.start_id;
        }
        else if(list_para->root_type == RAT_USB)
        {
            root_para->last_music_start_index_ud = ListBarScene.start_id;
        }
        break;
    case RAT_MEDIA_TYPE_EBOOK:
        AppId = REG_APP_EBOOK;
        if(list_para->root_type == RAT_TF)
        {
            root_para->last_ebook_start_index_sd = ListBarScene.start_id;
        }
        else if(list_para->root_type == RAT_USB)
        {
            root_para->last_ebook_start_index_ud = ListBarScene.start_id;
        }
        break;
    default:
        break;
    }

    ret = GetListItemFileFullPath(list_para, ListBarScene.focus_id, FileName);
    if(ret == EPDK_OK)
    {
        dsk_reg_save_cur_play_info(AppId, ListBarScene.focus_id, FileName, list_para->root_type);
        //dsk_reg_save_cur_play_info_ext(AppId,ListBarScene.start_id,list_para->rat.index, FileName, list_para->root_type);
    }
    return EPDK_OK;
}
static __s32 explorer_listview_create(H_WIN  list_win)
{
    explr_list_para_t *list_para;

    list_para = (explr_list_para_t *)GUI_WinGetAttr(list_win);
    list_para->list_win = list_win;		//保存窗口句柄

    list_para->font = SWFFont;
    __msg("explorer is initializing!\n");

    if(list_para->media_type == RAT_MEDIA_TYPE_ALL)
    {
        explorer_file_list_init(list_para);
        GUI_SetTimer(list_win, ExplorerTimerId, C_EXP_TIMER_INTERVAL, NULL);
    }
    else
    {
        explorer_rat_init(list_para);
        explorer_get_last_para(list_para);
    }

    explorer_listbar_init(list_win);
    explorer_list_long_string_init(list_para);

    return 0;
}

//only for file manager delete file dialog timer
void explorer_list_on_timer(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    if(list_para == NULL)
    {
        return;
    }
    /*
    if(GUI_IsTimerInstalled(list_para->list_win, ExplorerTimerId))
    {
    	GUI_KillTimer(list_para->list_win, ExplorerTimerId);
    	if(list_para->h_dialog != NULL)
    	{
    		return;
    	}
    	explorer_draw_file_info(list_para);
    }
    */
    //__msg("explorer list on timer\n");
    if(list_para->del_dlg_open)
    {
        list_para->del_dlg_open = EPDK_FALSE;
        //__msg("2explorer list on timer-draw file info\n");
        explorer_draw_file_info(list_para);
    }
    //LISTBAR_ShowPage(list_para->listbar_handle);
    return;
}

/*
************************************************************************************************************************
*Function	:          static __s32 _explorer_list_win_cb(__gui_msg_t *msg)
*
*Description	: 		ListWindow contrl block
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 _explorer_list_win_cb(__gui_msg_t *msg)
{
    __s32 ret = EPDK_OK;
    __gui_msg_t send;

    eLIBs_memset(&send, 0, sizeof(__gui_msg_t));

    switch(msg->id)
    {
    case GUI_MSG_CREATE:
        __msg("----explorer frame window GUI_MSG_CREATE begin----\n");
        explorer_listview_create(msg->h_deswin);
        return EPDK_OK;
    case GUI_MSG_PAINT:
        __msg("----explorer frame window GUI_MSG_ONPAINT----\n");
        explorer_listview_onpaint(msg->h_deswin);
        return EPDK_OK;
    case GUI_MSG_CLOSE:
        __msg("----explorer frame window GUI_MSG_CLOSE begin----\n");
        explorer_list_win_on_close(msg);
        return EPDK_OK;
    case GUI_MSG_DESTROY:
        __msg("----explorer frame window GUI_MSG_DESTROY begin----\n");
        explorer_list_win_on_destroy(msg);
        return EPDK_OK;
    case GUI_MSG_COMMAND:
        explorer_list_win_on_command(msg);
        return EPDK_OK;
    case GUI_MSG_KEY:
        __msg("_explorer_list_win_cb:GUI_MSG_KEY\n");
        explorer_list_win_key_proc(msg);
        return EPDK_OK;
    case GUI_MSG_TIMER:
        explorer_list_on_timer(msg);
        break;
    case DSK_MSG_FS_PART_PLUGIN:
        break;
    case DSK_MSG_FS_PART_PLUGOUT:
        //explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
        break;
    default:
        ret = GUI_FrmWinDefaultProc(msg);
        break;
    }
    return ret;
}


//释放Listwindow
__s32 explorer_list_win_delete(H_WIN list_win)
{
    GUI_FrmWinDelete(list_win);
    return EPDK_OK;
}


/*
************************************************************************************************************************
*Function	:          void  explorer_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
*
*Description	: 		向父窗口传送命令消息
*
*Arguments  	: 		hwin, 主窗口句柄
*					id,     命令参数
*Return     	:
*
************************************************************************************************************************
*/
void  explorer_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    char buf[128];
    H_WIN hExpWin;
    hExpWin = GUI_WinGetParent(hwin);
    msg.h_deswin = GUI_WinGetParent(hExpWin);
    //msg.h_srcwin = NULL;
    msg.h_srcwin = hwin;
    __here__;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hExpWin), id);//
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;

    GUI_WinGetName(msg.h_deswin, buf);

    __msg("%s\n", buf);
    GUI_SendNotifyMessage(&msg);
}

/*
************************************************************************************************************************
*Function	:          void ExplorerSendMessage(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
*
*Description	: 		SendMessage,发送同步消息，改消息回调函数将会被立即执行
*
*Arguments  	: 		异步消息&NotifyMessage 将会被插入到消息队列中，不会被立即执行
*
*Return     	:
*
************************************************************************************************************************
*/
void ExplorerSendMessage(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    char buf[128];
    GUI_WinGetName(hwin, buf);
    __msg("%s\n", buf);

    msg.h_srcwin = NULL;
    msg.h_deswin = hwin;
    msg.id = id;
    msg.dwAddData1 = data1;//
    msg.dwAddData2 = data2;
    GUI_SendMessage(&msg);
    return;
}

/*
************************************************************************************************************************
*Function	:          void  ExplorerListWinGetSearchPath(explr_list_para_t *para)
*
*Description	: 		根据RootType 获得Search path
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
void  ExplorerListWinGetSearchPath(explr_list_para_t *para)
{
    __s32 ret = 0;
    char root_type[50];
    char disk_name[RAT_MAX_PARTITION][4];

    eLIBs_memset(root_type, 0, sizeof(root_type));
    eLIBs_memset(disk_name, 0, sizeof(disk_name));
    eLIBs_memset(para->search_path, 0, sizeof(para->search_path));
    //ret = rat_get_partition_name(RAT_SD_CARD, para->search_path);

    switch(para->root_type)
    {
    case RAT_LOCAL:
        eLIBs_strcpy(root_type, RAT_LOCAL_DISK);
        break;
    case RAT_TF:
        eLIBs_strcpy(root_type, RAT_SD_CARD);
        break;
    case RAT_USB:
        eLIBs_strcpy(root_type, RAT_USB_DISK);
        break;
    default:
        break;
    }

    __wrn("root_type=%d\n", root_type);
    ret = rat_get_partition_name(root_type, disk_name);
    __wrn("para->search_path=%s\n", para->search_path);
    eLIBs_strcpy(para->search_path, disk_name[0]);
    //__msg("");

    //eLIBs_memcpy(para->search_path,"f:", sizeof("f:"));
    return ;
}


__s32 explorer_list_long_string_init(explr_list_para_t *this)
{
    if(this->long_str_handle == NULL)
    {
        __show_info_move_t show_info;

        eLIBs_memset(&show_info, 0, sizeof(__show_info_move_t));
        show_info.hlayer = this->list_layer;
        show_info.alpha_en = EPDK_TRUE;
        show_info.pFont = NULL;
        show_info.string = "";
        show_info.fontColor = GUI_WHITE;
        show_info.align = GUI_TA_LEFT | GUI_TA_VCENTER;
        show_info.bmp = explorer_get_listview_icon_res(ID_EXP_LIST_ITEM_FOCUS_BG);
        show_info.bmp_pos.x = 0;
        show_info.bmp_pos.y = 0;

        this->long_str_handle = GUI_LongStringCreate(&show_info);
        if(this->long_str_handle == NULL)
        {
            __wrn("create long string failed!\n");
        }
    }
    else
    {
        __msg("long string scroll has already started!\n");
    }
    return EPDK_OK;
}

__s32 explorer_list_long_string_uninit(explr_list_para_t *this)
{
    if(this->long_str_handle != NULL)
    {
        GUI_LongStringDelete(this->long_str_handle);
        this->long_str_handle = NULL;
    }
    return EPDK_OK;
}

__s32 explorer_list_long_string_start_roll_fast(__lbar_draw_para_t *draw_param, char *string)
{
    __show_info_move_t show_info;
    __epdk_charset_enm_e encode = EPDK_CHARSET_ENM_UTF8;
    RECT roll_rect, text_rect;
    explorer_viewer_ui_t *ui_para;

    explr_list_para_t *this;
    __u32 font_size = 0;
    this = (explr_list_para_t *)draw_param->attr;		//for draw the picture  in different media type


    if(this->long_str_handle != NULL)
    {
        eLIBs_memset(&show_info, 0, sizeof(__show_info_move_t));

        ui_para = explorer_get_viewer_ui_param();

        //roll_rect.x = draw_param->rect.x  + ui_para->list_item_ui_param.text_rect.x;
        //roll_rect.y = draw_param->rect.y  + ui_para->list_item_ui_param.text_rect.y;
        //roll_rect.width = ui_para->list_item_ui_param.text_rect.width+1;
        //roll_rect.height = ui_para->list_item_ui_param.text_rect.height;

        explorer_get_item_text_rect(this, &text_rect);	//获得text rectangle

        roll_rect.x = draw_param->rect.x  + text_rect.x;
        roll_rect.y = draw_param->rect.y  + text_rect.y;
        roll_rect.width = text_rect.width + 1;
        roll_rect.height = text_rect.height + 1;
        font_size = GUI_GetFontDistY();


        show_info.hlayer = this->list_layer;
        show_info.region.x = roll_rect.x;
        show_info.region.y = roll_rect.y;
        show_info.region.width = roll_rect.width;
        show_info.region.height = roll_rect.height;
        show_info.alpha_en = EPDK_FALSE;
        show_info.pFont = this->font;
        show_info.string = string;
        show_info.encode_id = encode;

        show_info.fontColor = GUI_WHITE;
        //show_info.bkColor =GUI_BLACK;
        show_info.align = GUI_TA_LEFT | GUI_TA_VCENTER;

        if(this->view_mode == EXPLR_LIST_MODE)
        {
            show_info.bmp = explorer_get_listview_icon_res(ID_EXP_LONG_STR_SCROLL_BG);
            show_info.bmp_pos.x = 0;
            show_info.bmp_pos.y = ui_para->list_item_ui_param.focus_icon_rect.y;
        }
        else
        {
            show_info.bmp = NULL;
            show_info.bmp_pos.x = 0;
            show_info.bmp_pos.y = 0;
        }

        GUI_LongStringStop(this->long_str_handle);
        GUI_LongStringReset(this->long_str_handle, &show_info);
        GUI_LongStringStart(this->long_str_handle);

    }
    return EPDK_OK;
}
//
__s32 explorer_list_long_string_start_roll(explr_list_para_t *this)
{

    return EPDK_OK;
}

static __s32 explorer_list_long_string_stop_roll(explr_list_para_t *this)
{
    if(this->long_str_handle != NULL)
    {
        return GUI_LongStringStop(this->long_str_handle);
    }
    return EPDK_FAIL;
}

