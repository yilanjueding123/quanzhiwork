/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
* File    	: 	music_list.c,
*								music_list.h
* By      	: 	Libaiao
* Func			:
* Version		: 	v1.0
* ============================================================================================================
* 2010-5-3 15:24:52  Libaiao  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "beetles_app.h"
#include "music_player.h"

#include "music_list.h"
#include "music_ui.h"


__u32 music_last_start_id = 0;

static __s32 music_listbar_on_next_key(void *list_ctrl);
static __s32 music_listbar_on_previous_key(void *list_ctrl);

__s32 MusicGetListItemFileFullPath(HRAT rat_handle, __s32 ItemIndex, char *FilePath);

__s32 music_list_long_string_init(music_list_ctrl_t *this);
__s32 music_list_long_string_uninit(music_list_ctrl_t *this);
__s32 music_list_long_string_start_roll_fast(__lbar_draw_para_t *draw_param, char *string);
__s32 music_list_long_string_start_roll(music_list_ctrl_t *this);
static __s32 music_list_long_string_stop_roll(music_list_ctrl_t *this);


__s32 music_list_get_last_para(music_list_ctrl_t *This);
__s32 music_list_save_last_para(music_list_ctrl_t *This);

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

//根据root_type获得全盘搜索路径
__s32  music_rat_get_search_path(music_list_ctrl_t *list_ctrl)
{
    __s32 ret = 0;
    char root_type[50];
    char disk_name[RAT_MAX_PARTITION][4];

    eLIBs_memset(root_type, 0, sizeof(root_type));
    eLIBs_memset(disk_name, 0, sizeof(disk_name));
    eLIBs_memset(list_ctrl->search_path, 0, sizeof(list_ctrl->search_path));

    __msg("list_ctrl->root_type=%d\n", list_ctrl->root_type);
    switch(list_ctrl->root_type)
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
        eLIBs_strcpy(root_type, RAT_SD_CARD);
        break;
    }
    ret = rat_get_partition_name(root_type, disk_name);
    eLIBs_strcpy(list_ctrl->search_path, disk_name[0]);

    __msg("disk_name[0]=%s\n", disk_name[0]);
    //eLIBs_strcpy(list_ctrl->search_path,"f:");//112350

    return ret;
}

//Rat 初始化
static __s32 music_rat_init(music_list_ctrl_t *list_ctrl)
{
    __s32 ret;
    ret = music_rat_get_search_path(list_ctrl);
    if(ret == EPDK_FAIL)
    {
        return EPDK_FAIL;
    }

    __msg("list_ctrl->rat_handle=%d\n", list_ctrl->rat_handle);
    if(list_ctrl->rat_handle == NULL)
    {
        list_ctrl->rat_handle  = rat_open(list_ctrl->search_path, list_ctrl->media_type, C_SEARCH_FILE_CNT_AT_FIRST);
    }
    else
    {
        __wrn("!!!!!!!!!!!!!please dont open rat again!!!!!!!!!!!!!!!!!\n");
    }

    if(list_ctrl->rat_handle == NULL)
    {
        __wrn("!!!!!!!!!!!!!open rat fail!!!!!!!!!!!!!!!!!\n");
        return EPDK_FAIL;
    }
    //rat_move_cursor_to_first(list_para->rat.handle);

    list_ctrl->total = rat_get_cur_scan_cnt(list_ctrl->rat_handle);;

    //rat_start_miniature_decode();	//开始缩略图线程
    return EPDK_OK;
}

//rat release
static __s32 music_rat_uninit(music_list_ctrl_t *list_ctrl)
{
    if(list_ctrl->rat_handle != NULL)
    {
        //rat_close(list_ctrl->rat_handle);
    }
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
static __s32 MusicExtractFileName(char *FileName, char *FilePath)
{
    char *pFileName;
    /*
    if(eLIBs_strlen(FilePath) == 0)
    {
    	return EPDK_FAIL;
    }
    */

    pFileName = eLIBs_strchrlast(FilePath, '\\');
    if(pFileName != NULL)
    {
        pFileName++;
        eLIBs_strcpy(FileName, pFileName);
    }
    return EPDK_OK;
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
__s32 MusicGetListItemFileFullPath(HRAT rat_handle, __s32 ItemIndex, char *FilePath)
{
    int ret;
    rat_entry_t ItemInfo;

    eLIBs_memset(&ItemInfo, 0, sizeof(rat_entry_t));

    ret = rat_get_item_info_by_index(rat_handle, ItemIndex, &ItemInfo);
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
static __s32 MusicGetListItemFileName(HRAT rat_handle, __s32 ItemIndex, char *FileName)
{
    int ret;
    char FilePath[RAT_MAX_FULL_PATH_LEN] = "";
    //eLIBs_memset(FilePath,0,sizeof(FilePath));
    if(rat_handle == NULL)
    {
        return EPDK_FAIL;
    }
    ret = MusicGetListItemFileFullPath(rat_handle, ItemIndex, FilePath);
    if(ret == EPDK_FAIL)
    {
        return EPDK_FAIL;
    }

    MusicExtractFileName(FileName, FilePath);
    return EPDK_OK;
}



//普通条目图标
static __s32 draw_music_list_normal_item_icon(__lbar_draw_para_t *draw_param)
{
    void *pic_buf;
    __s32 picX, picY;
    music_ui_t *ui_para;

    ui_para = music_get_ui_param();

    pic_buf = music_get_icon_res(ID_MUSIC_FILE_UNFOCUSED_ICON);				//draw select border

    picX = draw_param->rect.x + ui_para->list_ctrl.file_icon_rect.x;
    picY = draw_param->rect.y + ui_para->list_ctrl.file_icon_rect.y;
    GUI_BMP_Draw(pic_buf, picX, picY);

    return EPDK_OK;
}
//焦点条目图标
static __s32 draw_music_list_focus_item_icon(__lbar_draw_para_t *draw_param)
{
    void *pic_buf;
    __s32 picX, picY;
    music_ui_t *ui_para;

    ui_para = music_get_ui_param();

    pic_buf = music_get_icon_res(ID_MUSIC_FILE_FOCUSED_ICON);				//draw select border

    picX = draw_param->rect.x + ui_para->list_ctrl.file_icon_rect.x;
    picY = draw_param->rect.y + ui_para->list_ctrl.file_icon_rect.y;
    GUI_BMP_Draw(pic_buf, picX, picY);

    return EPDK_OK;
}

//画焦点条目背景选择框
static __s32 draw_music_list_focus_item_BG(__lbar_draw_para_t *draw_param)
{
    void *pic_buf;
    __s32 picX, picY;
    music_ui_t *ui_para;

    ui_para = music_get_ui_param();

    pic_buf = music_get_icon_res(ID_MUSIC_FOCUS_ITEM_BORDER);				//draw select border

    picX = draw_param->rect.x + ui_para->list_ctrl.focus_icon_rect.x;
    picY = draw_param->rect.y + ui_para->list_ctrl.focus_icon_rect.y;
    GUI_BMP_Draw(pic_buf, picX, picY);

    return EPDK_OK;
}

static __s32 draw_music_list_item_text(__lbar_draw_para_t *draw_param)
{
    int ret;
    char FileName[RAT_MAX_FULL_PATH_LEN];
    GUI_RECT GUI_TextRect;
    music_list_ctrl_t *list_ctrl;
    music_ui_t *ui_para;

    ui_para = music_get_ui_param();
    list_ctrl = (music_list_ctrl_t *)draw_param->attr;		//for draw the picture  in different media type
    if(list_ctrl == NULL)
    {
        return EPDK_FAIL;
    }
    eLIBs_memset(FileName, 0, sizeof(FileName));
    __here__;
    ret = MusicGetListItemFileName(list_ctrl->rat_handle, draw_param->index, FileName);
    if(ret == EPDK_FAIL)
    {
        __wrn("get file information form rat fail!!");
        return EPDK_FAIL;
    }

    GUI_SetFont(list_ctrl->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetColor(GUI_WHITE);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    __here__;
    GUI_TextRect.x0 = draw_param->rect.x  + ui_para->list_ctrl.text_rect.x;
    GUI_TextRect.y0 = draw_param->rect.y  + ui_para->list_ctrl.text_rect.y;
    GUI_TextRect.x1 = draw_param->rect.x  + (ui_para->list_ctrl.text_rect.x + ui_para->list_ctrl.text_rect.width);
    GUI_TextRect.y1 = draw_param->rect.y  + (ui_para->list_ctrl.text_rect.y + ui_para->list_ctrl.text_rect.height);

    //GetClippedString(&GUI_TextRect, FileName, NULL, "~");

    __msg("FileName=%s\n", FileName);

    GUI_DispStringInRect(FileName, &GUI_TextRect, GUI_TA_VCENTER);
    __here__;
    if(draw_param->mode == LBAR_MODE_FOCUS)
    {
        music_list_long_string_start_roll_fast(draw_param, FileName);
    }
    __here__;
    return EPDK_OK;
}

//draw item in list mode, 不要在Listbar的回调函数中选择图层
static __s32 music_list_normal_item_on_draw(__lbar_draw_para_t *draw_param)
{

    //__msg("listbar item id =: %d\n", draw_param->index);		//focus id

    //draw_music_list_focus_item_BG(draw_param);
    __here__;
    draw_music_list_normal_item_icon(draw_param);
    __here__;
    draw_music_list_item_text(draw_param);
    __here__;
    return EPDK_OK;
}

static __s32 music_list_focus_item_on_draw(__lbar_draw_para_t *draw_param)
{
#if 1
    GUI_MEMDEV_Handle	draw_mem;

    draw_mem = GUI_MEMDEV_Create(draw_param->rect.x, draw_param->rect.y, draw_param->rect.width, draw_param->rect.height);
    GUI_MEMDEV_Select(draw_mem);
#endif
    //---------------------------------------------------------------------------------
    draw_music_list_focus_item_BG(draw_param);
    draw_music_list_focus_item_icon(draw_param);
    draw_music_list_item_text(draw_param);

    //---------------------------------------------------------------------------------
#if 1
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select( NULL );
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
#endif
    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:       static __s32 _music_listbar_on_draw(__lbar_draw_para_t *draw_param)
*
*Description	:	 ListBar绘制消息处理函数
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 _music_listbar_on_draw(__lbar_draw_para_t *draw_param)
{

    music_list_ctrl_t *list_ctrl;
    list_ctrl = (music_list_ctrl_t *)draw_param->attr;		//for draw the picture  in different media type
    //__wait__;
    if(list_ctrl == NULL)
    {
        return EPDK_FAIL;
    }

    if(list_ctrl->total <= 0)
    {
        return EPDK_FAIL;
    }

    __msg("!!!!!!!!!!!!!listbar->index = %d!!!!!!!!!!!!!!!!\n", draw_param->index);

    __msg("xxxxxxxxxxlistbar->X = %d\n", draw_param->rect.x);
    __msg("yyyyyyyyyylistbar->Y = %d\n", draw_param->rect.y);
    __msg("wwwwwwwwwwlistbar->w = %d\n", draw_param->rect.width);
    __msg("hhhhhhhhhhlistbar->h = %d\n", draw_param->rect.height);

    switch(draw_param->mode)
    {
    case LBAR_MODE_NORMAL:
        __msg("\n~~~~draw listbar in normal mode!~~~~");
        music_list_normal_item_on_draw(draw_param);
        break;
    case LBAR_MODE_FOCUS:
        __msg("\n~~~~draw focus item in listbar!~~~~");
        music_list_focus_item_on_draw(draw_param);
        //music_list_long_string_start_roll(list_ctrl);
        break;
    case LBAR_MODE_INVALID:
        __msg("\n~~~~draw invalid item in listbar!~~~~");
        break;
    case LBAR_MODE_VACANT:
        //__msg("\n~~~~draw vacant item in listbar!~~~~");
        //draw_list_vacant_item(draw_param);
        break;
    case LBAR_MODE_DIFFUSE:
        __msg("listbar item id =: %d\n", draw_param->index);		//focus id
        break;
    }
    return EPDK_OK;
}

static __s32 music_listbar_on_next_key(void *list_ctrl)
{
    music_list_ctrl_t *list_para = (music_list_ctrl_t *)list_ctrl;
    if(list_para == NULL)
    {
        return EPDK_FAIL;
    }
    if(list_para->total == 0)
    {
        return EPDK_FAIL;
    }
    if(!list_para->visiable)
    {
        return EPDK_FAIL;
    }
    music_list_long_string_stop_roll(list_para);
    LISTBAR_NextItem(list_para->listbar_handle);
    //LISTBAR_NextPage(list_para->listbar_handle);
    return EPDK_OK;
}

static __s32 music_listbar_on_previous_key(void *list_ctrl)
{
    music_list_ctrl_t *list_para = (music_list_ctrl_t *)list_ctrl;
    if(list_para == NULL)
    {
        return EPDK_FAIL;
    }
    if(list_para->total == 0)
    {
        return EPDK_FAIL;
    }
    if(!list_para->visiable)
    {
        return EPDK_FAIL;
    }
    music_list_long_string_stop_roll(list_ctrl);
    LISTBAR_PrevItem(list_para->listbar_handle);
    //LISTBAR_PrevPage(list_para->listbar_handle);
    return EPDK_OK;
}
/*
//listbar 失去焦点控制
static __s32 music_listbar_lost_focus(music_list_ctrl_t *list_ctrl)
{
	__listbar_scene_t ListBarScene;
	if(list_ctrl == NULL)
	{
		return EPDK_FAIL;
	}
	LISTBAR_GetScene(list_ctrl->listbar_handle, &ListBarScene);
	__msg("listbar start id = %d\n", ListBarScene.start_id);
	__msg("listbar focus id = %d\n", ListBarScene.focus_id);
	LISTBAR_SetNormalItem(list_ctrl->listbar_handle,ListBarScene.focus_id);
	return EPDK_OK;
}
*/

//返回相应的索引号
__s32 get_music_list_focus_item_full_filename(music_list_ctrl_t *list_ctrl, char *filename)
{
    //__s32 focus_id = 0;
    __listbar_scene_t ListBarScene;
    if(list_ctrl == NULL)
    {
        return EPDK_FAIL;
    }
    if(list_ctrl->total == 0)
    {
        return EPDK_FAIL;
    }

    {
        LISTBAR_GetScene(list_ctrl->listbar_handle, &ListBarScene);
        //focus_id = ListBarScene.focus_id;
        MusicGetListItemFileFullPath(list_ctrl->rat_handle, ListBarScene.focus_id, filename);
    }
    return ListBarScene.focus_id;
}


void clear_music_list_area(void)
{
    music_ui_t *ui_param;

    ui_param = music_get_ui_param();

    __msg("\n~~~~~~~~~~clear Listbar area~~~~~~~~~~~\n");
    GUI_ClearRect(ui_param->list_ctrl.listbar_rect.x, ui_param->list_ctrl.listbar_rect.y,
                  ui_param->list_ctrl.listbar_rect.width, ui_param->list_ctrl.listbar_rect.height);

    return;
}

static __s32 music_listbar_init(music_list_ctrl_t *list_ctrl)
{

    __listbar_config_t config;
    __scroll_bar_t scroll_param;						//new scroll bar
    __s32 item_width = 0, item_height = 0;
    music_ui_t *ui_param;

    ui_param = music_get_ui_param();

    __msg("\n~~~~~~~~~~Listbar is initializing~~~~~~~~~~~\n");

    eLIBs_memset(&config, 0, sizeof(__listbar_config_t));
    eLIBs_memset(&scroll_param, 0, sizeof(__scroll_bar_t));

    //if(list_ctrl->view_mode == EXPLR_LIST_MODE)
    {
        item_width = ui_param->list_ctrl.item_rect.width;						//根据list or square模式不同
        item_height = ui_param->list_ctrl.item_rect.height;
        config.list_rect.x = ui_param->list_ctrl.listbar_rect.x;			//ui_param->menu == listbar area
        config.list_rect.y = ui_param->list_ctrl.listbar_rect.y;
        config.list_rect.width = ui_param->list_ctrl.listbar_rect.width;
        config.list_rect.height = ui_param->list_ctrl.listbar_rect.height;
    }

    config.item_width = item_width;
    config.item_height = item_height;			//these are numbers only for test

    //__msg("----view_param->playing_media_type: %d\n", view_param->playing_media_type);
    config.start_id = list_ctrl->last_start_id;
    config.focus_id = list_ctrl->last_focused_id;
    config.bk_color = 0;
    config.alpha_status = 1;					//打开Alpha开关
    config.list_attr = (void *)list_ctrl;

    //list_ctrl->play_index = 0;						//初始化起始条目

    config.item_cnt = list_ctrl->total;   //Rat.total必须已经获得


    scroll_param.show_rect.x = ui_param->scrollbar.BG_rect.x;
    scroll_param.show_rect.y = ui_param->scrollbar.BG_rect.y;
    scroll_param.show_rect.width = ui_param->scrollbar.BG_rect.width;
    scroll_param.show_rect.height = ui_param->scrollbar.BG_rect.height;		//整个Scoll bar显示区域，Head, body,tail

    //滚动条区域(除去head,body,tail)
    scroll_param.scroll_rect.x = ui_param->scrollbar.scroll_rect.x;
    scroll_param.scroll_rect.y = ui_param->scrollbar.scroll_rect.y;
    scroll_param.scroll_rect.width = ui_param->scrollbar.scroll_rect.width;
    scroll_param.scroll_rect.height = ui_param->scrollbar.scroll_rect.height;			//滚动条区域(除去head,body,tail)

    scroll_param.head_height = ui_param->scrollbar.head_height;
    scroll_param.tail_height = ui_param->scrollbar.tail_height;				//head, tail

    scroll_param.back_bmp = music_get_icon_res(ID_MUSIC_LIST_SCROLL_BG);
    scroll_param.head_bmp = music_get_icon_res(ID_MUSIC_LIST_SCROLL_HEAD);
    scroll_param.body_bmp = music_get_icon_res(ID_MUSIC_LIST_SCROLL_BODY);
    scroll_param.tail_bmp = music_get_icon_res(ID_MUSIC_LIST_SCROLL_TAIL);

    // __msg("----back_bmp: %x, head_bmp: %x, body_bmp: %x, tail_bmp: %x\n", scroll_param.back_bmp, scroll_param.head_bmp, scroll_param.body_bmp, scroll_param.tail_bmp);

    list_ctrl->listbar_handle = LISTBAR_Create(list_ctrl->framewin);  				//new listbar
    LISTBAR_Config(list_ctrl->listbar_handle, _music_listbar_on_draw, &config);    //config parametters for listbar
    LISTBAR_ScrollConfig(list_ctrl->listbar_handle, &scroll_param); 	//scrollbar belongs to listbar
    //LISTBAR_ShowPage(list_ctrl->listbar_handle);
    return EPDK_OK;
}

__s32 music_listbar_restore(music_list_ctrl_t *list_ctrl)
{
    __scroll_bar_t scroll_param;						//new scroll bar
    __s32 item_width = 0, item_height = 0;
    music_ui_t *ui_param;

    ui_param = music_get_ui_param();

    if(list_ctrl == NULL)
    {
        return EPDK_FAIL;
    }
    scroll_param.show_rect.x = ui_param->scrollbar.BG_rect.x;
    scroll_param.show_rect.y = ui_param->scrollbar.BG_rect.y;
    scroll_param.show_rect.width = ui_param->scrollbar.BG_rect.width;
    scroll_param.show_rect.height = ui_param->scrollbar.BG_rect.height;		//整个Scoll bar显示区域，Head, body,tail

    //滚动条区域(除去head,body,tail)
    scroll_param.scroll_rect.x = ui_param->scrollbar.scroll_rect.x;
    scroll_param.scroll_rect.y = ui_param->scrollbar.scroll_rect.y;
    scroll_param.scroll_rect.width = ui_param->scrollbar.scroll_rect.width;
    scroll_param.scroll_rect.height = ui_param->scrollbar.scroll_rect.height;			//滚动条区域(除去head,body,tail)

    scroll_param.head_height = ui_param->scrollbar.head_height;
    scroll_param.tail_height = ui_param->scrollbar.tail_height;				//head, tail

    scroll_param.back_bmp = music_get_icon_res(ID_MUSIC_LIST_SCROLL_BG);
    scroll_param.head_bmp = music_get_icon_res(ID_MUSIC_LIST_SCROLL_HEAD);
    scroll_param.body_bmp = music_get_icon_res(ID_MUSIC_LIST_SCROLL_BODY);
    scroll_param.tail_bmp = music_get_icon_res(ID_MUSIC_LIST_SCROLL_TAIL);

    LISTBAR_ScrollConfig(list_ctrl->listbar_handle, &scroll_param); 	//scrollbar belongs to listbar
    return EPDK_OK;
}

__s32 music_listbar_uninit(music_list_ctrl_t *list_ctrl)
{
    if(list_ctrl->listbar_handle == NULL)
    {
        return EPDK_FAIL;
    }
    __msg("----LISTBAR_Delete begin: %x\n", list_ctrl->listbar_handle);

    LISTBAR_SuspendPage(list_ctrl->listbar_handle);
    __here__;
    LISTBAR_Delete(list_ctrl->listbar_handle);
    __here__;
    return EPDK_OK;
}

__s32 show_music_list(music_list_ctrl_t *list_ctrl)
{
    if(list_ctrl == NULL)
    {
        return EPDK_OK;
    }
    if(list_ctrl->listbar_handle == NULL)
    {
        return EPDK_FAIL;
    }
    __msg("list bar show page\n");
    LISTBAR_ShowPage(list_ctrl->listbar_handle);
    return EPDK_OK;
}
__s32 music_list_set_focus_item(music_list_ctrl_t *list_ctrl, __u32 item_num)
{
    __s32 ret;
    if(list_ctrl == NULL)
    {
        return EPDK_FAIL;
    }
    music_list_long_string_stop_roll(list_ctrl);		//
    //__u32 old_focus_item= 0;
    //old_focus_item = LISTBAR_GetFocusItem(list_ctrl->listbar_handle);
    //LISTBAR_SetNormalItem(list_ctrl->listbar_handle, old_focus_item);
    LISTBAR_LostFocus(list_ctrl->listbar_handle);
    ret = LISTBAR_SetFocusItemExt(list_ctrl->listbar_handle, item_num);
    LISTBAR_ShowPage(list_ctrl->listbar_handle);
    //LISTBAR_UpdatePage(list_ctrl->listbar_handle);
    if(EPDK_FAIL == ret)
    {
        return EPDK_FAIL;
    }
    return EPDK_OK;
}



__s32 music_list_set_item_scene(music_list_ctrl_t *this, __listbar_scene_t *scene)
{
    if(this == NULL)
    {
        return EPDK_FAIL;
    }
    if(this->listbar_handle == NULL)
    {
        return EPDK_FAIL;
    }
    __msg("\n\n\n-----------------listbar scene start id = %d, focus id = %d\n", scene->start_id, scene->focus_id);
    LISTBAR_SuspendPage(this->listbar_handle);
    LISTBAR_SetScene(this->listbar_handle, scene);
    LISTBAR_ResumePage(this->listbar_handle);
    return EPDK_OK;
}

__s32 music_list_get_item_scene(music_list_ctrl_t *this, __listbar_scene_t *scene)
{
    if(this == NULL)
    {
        return EPDK_FAIL;
    }
    if(this->listbar_handle == NULL)
    {
        return EPDK_FAIL;
    }
    LISTBAR_SuspendPage(this->listbar_handle);
    LISTBAR_GetScene(this->listbar_handle, scene);
    LISTBAR_ResumePage(this->listbar_handle);
    __msg("\n\n\n-----------------listbar scene start id = %d, focus id = %d\n", scene->start_id, scene->focus_id);
    return EPDK_OK;
}

__s32 music_list_get_last_para(music_list_ctrl_t *This)
{
    __s32 ret = 0;
    reg_root_para_t *root_para = NULL;
    char FileName[RAT_MAX_FULL_PATH_LEN] = "";
    __u32 page_item_cnt = 0;


    {
        //page_item_cnt = LISTBAR_GetPageItemCount(This->listbar_handle);
    }
    {
        page_item_cnt = music_count_listbar_pagenum();
    }

    root_para = dsk_reg_get_para_by_app(REG_APP_ROOT);

    if(This->rat_handle == NULL)
    {
        __wrn("music list get last para should be at the end of rat initial!!\n");
        return EPDK_FAIL;
    }

    if(This->root_type == RAT_TF)
    {
        This->last_start_id = root_para->last_music_start_index_sd;
        This->last_focused_id = root_para->last_music_index_sd;		//上一次播放的序列号
        This->last_filename = root_para->last_music_path_sd;
    }
    else if(This->root_type == RAT_USB)
    {
        This->last_start_id = root_para->last_music_start_index_ud;
        This->last_focused_id = root_para->last_music_index_ud;
        This->last_filename = root_para->last_music_path_ud;
    }
    __msg("\n======================================this->last_focusedid= %d", This->last_focused_id);
    __msg("\n====================================this->last_startid= %d", This->last_start_id);
    __msg("\n====================================this->total= %d", This->total);
    __msg("\n====================================this->play_index= %d", This->play_index);
    if((This->last_start_id < This->total)
            && (This->last_focused_id < This->total))
    {
        ret = MusicGetListItemFileFullPath(This->rat_handle, This->last_focused_id, FileName);
        if(ret == EPDK_OK)
        {
            __msg("\n======================================this->last_focusedid= %d", This->last_focused_id);
            __msg("\n====================================this->last_startid= %d", This->last_start_id);
            __msg("\n====================================page_item_cnt= %d", page_item_cnt);
            __msg("\n====================================filename = %s", FileName);
            __msg("\n====================================last filename = %s", This->last_filename);
            ret = eLIBs_strcmp(FileName, This->last_filename);
            if(ret == 0)
            {
                __here__;
                if(This->last_focused_id < This->last_start_id)
                {
                    This->last_start_id = This->last_focused_id;
                    This->play_index = This->last_focused_id;
                    __here__;
                }
                else if(This->last_focused_id - This->last_start_id >= page_item_cnt)
                {
                    This->last_start_id = This->last_focused_id;
                    This->play_index = This->last_focused_id;
                    __here__;
                }
                else
                {
                    This->play_index = This->last_focused_id;
                    __here__;
                }
            }
            else
            {
                __here__;
                ret = robin_npl_get_cur();
                __msg("--------------robin npl get cur item index = %d\n", ret);
                if(ret != -1)
                {
                    This->last_start_id = ret;
                    This->last_focused_id = ret;
                    This->play_index = ret;
                }
                else
                {
                    __here__;
                    This->last_start_id = 0;
                    This->last_focused_id = 0;
                    This->play_index = 0;
                }
            }
        }
        else
        {
            This->last_start_id = 0;
            This->last_focused_id = 0;
            This->play_index = 0;
        }
    }
    else			//当文件被删除后，上次记录的start id 和focus id 可能会大于当前total的值
    {
        __here__;
        ret = robin_npl_get_cur();
        __msg("--------------robin npl get cur item index = %d\n", ret);
        if(ret != -1)
        {
            __here__;
            This->last_start_id = ret;
            This->last_focused_id = ret;
            This->play_index = ret;
        }
        else
        {
            __here__;
            This->last_start_id = 0;
            This->last_focused_id = 0;
            This->play_index = 0;
        }
    }
    return EPDK_OK;
}

__s32 music_list_save_last_para(music_list_ctrl_t *This)
{
    __s32 ret = 0;
    reg_app_e AppId = 0;
    reg_root_para_t *root_para = NULL;
    char FileName[RAT_MAX_FULL_PATH_LEN] = "";
    __listbar_scene_t ListBarScene = {0, 0};
    LISTBAR_GetScene(This->listbar_handle, &ListBarScene);

    root_para = dsk_reg_get_para_by_app(REG_APP_ROOT);

    if(This->root_type == RAT_TF)
    {
        root_para->last_music_start_index_sd = ListBarScene.start_id;
    }
    else if(This->root_type == RAT_USB)
    {
        root_para->last_music_start_index_ud = ListBarScene.start_id;
    }

    //eLIBs_memset(FileName,0,sizeof(FileName));

    ret = MusicGetListItemFileFullPath(This->rat_handle, This->play_index, FileName);

    if(ret == EPDK_OK)
    {
        dsk_reg_save_cur_play_info(REG_APP_MUSIC, This->play_index, FileName, This->root_type);
        return EPDK_OK;
    }

    return EPDK_OK;
}


//创建music list
music_list_ctrl_t *new_music_list(music_list_para_t *list_para)
{
    music_list_ctrl_t *list_ctrl = NULL;

    //app_print_memory_usage("new music list", NULL);

    list_ctrl = (music_list_ctrl_t *)esMEMS_Balloc(sizeof(music_list_ctrl_t));
    if(list_ctrl == NULL)
    {
        return NULL;
    }
    eLIBs_memset((void *)list_ctrl, 0, sizeof(music_list_ctrl_t));

    list_ctrl->framewin = list_para->framewin;
    list_ctrl->layer = list_para->layer;
    list_ctrl->root_type = list_para->root_type;
    //list_ctrl->explorer_play_music = list_para->explorer_play_music;
    //list_ctrl->music_file_name = list_para->music_file_name;


    list_ctrl->media_type = list_para->media_type;
    list_ctrl->visiable = list_para->visiable;
    list_ctrl->font = list_para->font;
    list_ctrl->OnNextKey = music_listbar_on_next_key;
    list_ctrl->OnPreviousKey = music_listbar_on_previous_key;

    __msg("music_rat_init(list_ctrl)\n");
    if(music_rat_init(list_ctrl) == EPDK_FAIL)
    {
        //return NULL;
        __msg("please insert a sdcard or USB device\n");
    }
    __msg("list_para->explorer_play_music = %d\n", list_para->explorer_play_music);
    if(list_para->explorer_play_music != 0)
    {
        //是否为背景播放文件在restore已经做了判断

        __msg("list_para->music_file_name = %s\n", list_para->music_file_name);
        rat_set_file_for_play(list_ctrl->rat_handle, list_para->music_file_name);

        //	list_ctrl->last_focused_id = rat_npl_file2index(list_para->music_file_name);
        //	__msg("--------------robin npl get cur item index = %d\n", list_ctrl->last_focused_id);

        {
            HRATNPL hRatNpl;
            hRatNpl = rat_npl_open(RAT_MEDIA_TYPE_AUDIO);
            if( !hRatNpl )
            {
                __msg( "rat_npl_open fail \n" );
            }
            //	list_ctrl->last_focused_id = rat_npl_file2index(hRatNpl , list_para->music_file_name);
            //	__msg("--------------robin npl get cur item index = %d\n", list_ctrl->last_focused_id);

            list_ctrl->last_focused_id = rat_npl_get_cur(hRatNpl);
        }

        __msg("--------------robin npl get cur item index = %d\n", list_ctrl->last_focused_id);
        if(list_ctrl->last_focused_id != -1)
        {
            __here__;
            list_ctrl->last_start_id = list_ctrl->last_focused_id;
            list_ctrl->play_index = list_ctrl->last_focused_id;
        }
        else
        {
            __here__;
            list_ctrl->last_focused_id = 0;
            list_ctrl->last_start_id = 0;
            list_ctrl->play_index = 0;
        }
    }
    else
    {
        __here__;
        music_list_get_last_para(list_ctrl);		//获得上一次浏览的参数
    }

    //if(list_ctrl->visiable)
    {
        music_listbar_init(list_ctrl);
    }
    music_list_long_string_init(list_ctrl);			//long string
    //app_print_memory_usage("new music list finished", NULL);
    return list_ctrl;
}

//删除music list ,并释放相关资源
__s32 delete_music_list(music_list_ctrl_t *list_ctrl)
{
    if(list_ctrl == NULL)
    {
        return EPDK_FAIL;
    }
    //app_print_memory_usage("delete music list ", NULL);
    music_list_save_last_para(list_ctrl);			//保存参数

    music_list_long_string_uninit(list_ctrl);

    music_listbar_uninit(list_ctrl);
    __here__;

    music_rat_uninit(list_ctrl);
    __here__;

    esMEMS_Bfree(list_ctrl, sizeof(music_list_ctrl_t));
    __here__;
    list_ctrl = NULL;
    //app_print_memory_usage("delete music list finished", NULL);
    return EPDK_OK;
}

__s32 music_list_long_string_init(music_list_ctrl_t *this)
{
    if(this->long_str_handle == NULL)
    {
        __show_info_move_t show_info;

        eLIBs_memset(&show_info, 0, sizeof(__show_info_move_t));
        show_info.hlayer = this->layer;
        show_info.alpha_en = EPDK_FALSE;
        show_info.pFont = NULL;
        show_info.string = "";
        show_info.fontColor = GUI_WHITE;
        show_info.align = GUI_TA_LEFT | GUI_TA_VCENTER;
        show_info.bmp = music_get_icon_res(ID_MUSIC_FOCUS_ITEM_BORDER);
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

__s32 music_list_long_string_uninit(music_list_ctrl_t *this)
{
    if(this->long_str_handle != NULL)
    {
        GUI_LongStringDelete(this->long_str_handle);
        this->long_str_handle = NULL;
    }
    return EPDK_OK;
}

__s32 music_list_long_string_start_roll_fast(__lbar_draw_para_t *draw_param, char *string)
{
    __show_info_move_t show_info;
    __epdk_charset_enm_e encode = EPDK_CHARSET_ENM_UTF8;
    RECT roll_rect;
    music_ui_t *ui_para;
    music_list_ctrl_t *this;
    this = (music_list_ctrl_t *)draw_param->attr;		//for draw the picture  in different media type


    if(this->long_str_handle != NULL)
    {
        eLIBs_memset(&show_info, 0, sizeof(__show_info_move_t));

        ui_para = music_get_ui_param();

        roll_rect.x = draw_param->rect.x  + ui_para->list_ctrl.text_rect.x;
        roll_rect.y = draw_param->rect.y  + ui_para->list_ctrl.text_rect.y;
        roll_rect.width = ui_para->list_ctrl.text_rect.width + 1;
        roll_rect.height = ui_para->list_ctrl.text_rect.height + 1;

        //roll_rect.x = draw_param->rect.x  + ui_para->list_ctrl.focus_icon_rect.x;
        //roll_rect.y = draw_param->rect.y  + ui_para->list_ctrl.focus_icon_rect.y;
        //roll_rect.width = ui_para->list_ctrl.focus_icon_rect.width;
        //roll_rect.height = ui_para->list_ctrl.focus_icon_rect.height;

        show_info.hlayer = this->layer;
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
        show_info.bmp = music_get_icon_res(ID_MUSIC_LONG_STR_SCROLL);
        //show_info.bmp_pos.x = draw_param->rect.x + ui_para->list_ctrl.focus_icon_rect.x;
        //show_info.bmp_pos.y = draw_param->rect.y + ui_para->list_ctrl.focus_icon_rect.y;
        show_info.bmp_pos.x = 0;
        show_info.bmp_pos.y = ui_para->list_ctrl.focus_icon_rect.y;
        //__msg("long string scroll star!");
        if(show_info.bmp != NULL)
        {
            GUI_LongStringStop(this->long_str_handle);
            GUI_LongStringReset(this->long_str_handle, &show_info);
            GUI_LongStringStart(this->long_str_handle);
        }
        //__msg("long string scroll started!");
        //__wait__;
    }
    return EPDK_OK;
}
//
__s32 music_list_long_string_start_roll(music_list_ctrl_t *this)
{
    __show_info_move_t show_info;
    RECT item_rect, roll_rect;
    __epdk_charset_enm_e encode = EPDK_CHARSET_ENM_UTF8;
    __s32 item_num;
    __s32 ret;
    char string[RAT_MAX_FULL_PATH_LEN];
    music_ui_t *ui_para;


    if(this->long_str_handle != NULL)
    {
        eLIBs_memset(&show_info, 0, sizeof(__show_info_move_t));
        eLIBs_memset(string, 0, sizeof(string));

        item_num = LISTBAR_GetFocusItem(this->listbar_handle);
        if(item_num == -1)
        {
            return EPDK_FAIL;
        }
        ret = MusicGetListItemFileName(this->rat_handle, item_num, string);
        if(ret == EPDK_FAIL)
        {
            return EPDK_FAIL;
        }

        LISTBAR_GetItemRect(this->listbar_handle, item_num, &item_rect);
        ui_para = music_get_ui_param();

        roll_rect.x = item_rect.x + ui_para->list_ctrl.text_rect.x;
        roll_rect.y = item_rect.y + ui_para->list_ctrl.text_rect.y;
        roll_rect.width = ui_para->list_ctrl.text_rect.width;
        roll_rect.height = ui_para->list_ctrl.text_rect.height;

        show_info.hlayer = this->layer;
        show_info.region.x = roll_rect.x;
        show_info.region.y = roll_rect.y;
        show_info.region.width = roll_rect.width;
        show_info.region.height = roll_rect.height;
        show_info.alpha_en = EPDK_TRUE;
        show_info.pFont = this->font;
        show_info.string = string;
        show_info.encode_id = encode;

        show_info.fontColor = GUI_BLACK;
        show_info.align = GUI_TA_LEFT | GUI_TA_VCENTER;

        show_info.bmp_pos.x = 0;
        show_info.bmp_pos.y = 0;

        GUI_LongStringStop(this->long_str_handle);
        GUI_LongStringReset(this->long_str_handle, &show_info);
        GUI_LongStringStart(this->long_str_handle);
    }
    return EPDK_OK;
}



__s32 music_list_long_string_stop_roll(music_list_ctrl_t *this)
{
    if(this->long_str_handle != NULL)
    {
        return GUI_LongStringStop(this->long_str_handle);
    }
    return EPDK_FAIL;
}
__s32 music_list_long_string_stop(music_list_ctrl_t *this)
{
    if(this->long_str_handle != NULL)
    {
        return GUI_LongStringStop(this->long_str_handle);
    }
    return EPDK_FAIL;
}
