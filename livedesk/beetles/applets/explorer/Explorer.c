/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
* File    	: 	Explorer.c,
*			Explorer.h
* By      	: 	Libaiao
* Func	:
* Version	: 	v1.0
* ============================================================================================================
* 2010-4-14 15:24:52  Libaiao  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "beetles_app.h"

#include "Explorer.h"
#include "Explorer_List.h"
#include "Explorer_UI.h"

#if  0
#define __msg(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
						     eLIBs_printf(__VA_ARGS__)									        )
#else
#define __msg(...)    	
#endif


typedef enum
{

    EXP_MEDIA_TYPE_ALL,				//所有媒体类型
    EXP_MEDIA_TYPE_PIC,				//所有图片文件
    EXP_MEDIA_TYPE_AUDIO,			//所有音频文件
    EXP_MEDIA_TYPE_VIDEO,			//所有视频文件
    EXP_MEDIA_TYPE_EBOOK,			//所有电子书文件
    EXP_MEDIA_TYPE_GAME,			//所有游戏文件
    EXP_MEDIA_TYPE_FIRMWARE,		//所有固件文件
    EXP_MEDIA_TYPE_UNKNOWN,			//所有系统不支持的文件
} explorer_media_type_t;


typedef struct tag_explorer_ctrl
{
    H_WIN h_explr;
#ifdef EXPLORE_BG_ON
    H_LYR BG_layer;			//background layer 8bpp(注意背景图需要设置为8bpp)
#endif
    H_LYR list_layer;		//layer for listbar, 32bpp(32bpp,因为listbar缩略图为32bpp)
    H_WIN h_list_framewin;

    __s32 root_type;					//USB Device, or  SDCard
    __u32 last_start_id;				//上次浏览起始ID
    __u32 last_focused_id;				//上次选中文件id
    char  *last_filename;				//上次浏览文件 名

    explorer_view_mode_e view_mode;		//显示模式
    //explorer_media_type_t media_type;
    rat_media_type_t media_type;
    GUI_FONT *explr_font;
    char root[4];                          //记录搜索的盘符
    root_para_t  *root_para ;
} explorer_ctrl_t;

//----------------------------------------------------------------------------------------------------
//define the global variables
//static explorer_param_t *explorer_param = NULL;


//private function
static __s32 _explorer_manager_win_cb(__gui_msg_t *msg);//process the  message of the manager window from system

static __s32 explorer_scene_create(__gui_msg_t *msg);

static H_LYR explorer_8bpp_layer_create(void);
static H_LYR explorer_32bpp_layer_create(void);
//static H_LYR explorer_layer_create(void);
static __s32 explorer_layer_delete(H_LYR layer);

static __s32 app_explorer_on_close(__gui_msg_t *msg);
static __s32 app_explorer_on_destroy(H_WIN h_exp_win);

//
//----------------------------------------------------------------------------------------------------

/*
************************************************************************************************************************
*Function	:           __s32 Explorer_create(root_para_t *para)
*
*Description	: 		Explorer Application Entry interface
*					create explorer manager window
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/

__s32 app_explorer_create(root_para_t  *para)
{
    __gui_manwincreate_para_t create_info;
    explorer_ctrl_t	*explr_ctrl;

    __inf("****************************************************************************************\n");
    __inf("********  enter explorer app  **************\n");
    __inf("****************************************************************************************\n");

    explr_ctrl = (explorer_ctrl_t *)esMEMS_Balloc(sizeof(explorer_ctrl_t));		//为私有属性申请内存
    eLIBs_memset(explr_ctrl, 0, sizeof(explorer_ctrl_t));
	rat_set_modify_flag_all(1);

    explr_ctrl->root_para = para ;
    explr_ctrl->explr_font = para->font;
	
    __msg("para->root_type = %d\n", para->root_type);
	
    switch(para->data)
    {
    case ID_EXPLORER_ALL:
        explr_ctrl->view_mode = EXPLR_LIST_MODE;
        explr_ctrl->media_type = RAT_MEDIA_TYPE_ALL;
        explr_ctrl->root_type = para->root_type;
        break;
		
    case ID_EXPLORER_MOVIE:
        explr_ctrl->view_mode = EXPLR_LIST_MODE;
        explr_ctrl->media_type = RAT_MEDIA_TYPE_VIDEO;
        explr_ctrl->root_type = para->root_type;
        break;
		
    case ID_EXPLORER_PHOTO:
        explr_ctrl->view_mode = EXPLR_LIST_MODE;		//EXPLR_SQUARE_MODE;
        explr_ctrl->media_type = RAT_MEDIA_TYPE_PIC;
        explr_ctrl->root_type = para->root_type;
        break;
		
    case ID_EXPLORER_EBOOK:
        explr_ctrl->view_mode = EXPLR_LIST_MODE;
        explr_ctrl->media_type = RAT_MEDIA_TYPE_EBOOK;
        explr_ctrl->root_type = para->root_type;
        break;
		
    case ID_EXPLORER_MUSIC:
        explr_ctrl->view_mode = EXPLR_LIST_MODE;
        explr_ctrl->media_type = RAT_MEDIA_TYPE_AUDIO;
        explr_ctrl->root_type = para->root_type;
        break;
		
    case ID_EXPLORER_RESTORE:
        explr_ctrl->media_type = RAT_MEDIA_TYPE_UNKNOWN;
        break;
    default:
        break;
    }

#if EPDK_PLAY_VIDEO_AUDIO_ENABLE
    explr_ctrl->media_type = RAT_MEDIA_TYPE_VIDEO_AND_AUDIO ;
#endif
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));

    create_info.name            = APP_EXPLORER;
    create_info.hParent         = para->h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_explorer_manager_win_cb);
    create_info.attr            = (void *)explr_ctrl;			//设置Managerwin 私有属性
    create_info.id				= APP_EXPLORER_ID;
    create_info.hHosting        = NULL;

    return(GUI_ManWinCreate(&create_info));
}

/*
************************************************************************************************************************
*Function	:           __s32 app_explorer_on_close(H_WIN h_exp_win)
*
*Description	: 		设置EXPLORER之后的AP, 保存设置,释放窗口
*					(释放窗口函数之后将响应destroy消息)
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
__s32 app_explorer_on_close(__gui_msg_t *msg)
{
    explorer_ctrl_t	*explr_ctrl;

    explr_ctrl	 = (explorer_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    //save_setting(explr_ctrl);
    GUI_ManWinDelete(msg->h_deswin);				//it will send detroy message after call this function.
    //cat_item = (__cat_item_t *)msg->dwAddData1;

    //if (cat_item == 0)
    {
        //explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
    }
    //else
    {
        switch(LOWORD(msg->dwAddData2))
        {
        case EXP_MEDIA_TYPE_VIDEO:
            //explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MOVIE, HIWORD(msg->dwAddData2));
            break;
        case EXP_MEDIA_TYPE_PIC:
            //explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_PHOTO, HIWORD(msg->dwAddData2));
            break;
        case EXP_MEDIA_TYPE_EBOOK:
            //explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_EBOOK, HIWORD(msg->dwAddData2));
            break;
        case EXP_MEDIA_TYPE_AUDIO:
            //explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MUSIC, HIWORD(msg->dwAddData2));
            break;
        default:
            break;
        }
    }

    return EPDK_OK;

}
/*
************************************************************************************************************************
*Function	:           __s32 app_explorer_on_destroy(H_WIN h_exp_win)
*
*Description	: 		释放图层，释放内存
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 app_explorer_on_destroy(H_WIN h_exp_win)
{
    explorer_ctrl_t	*explr_ctrl;
    explr_ctrl	 = (explorer_ctrl_t *)GUI_WinGetAttr(h_exp_win);

    explorer_layer_delete(explr_ctrl->list_layer);
#ifdef EXPLORE_BG_ON
    if(explr_ctrl->BG_layer)
    {
        GUI_LyrWinSetTop(explr_ctrl->BG_layer);
    }
    gscene_bgd_set_state(BGD_STATUS_SHOW);
    explorer_layer_delete(explr_ctrl->BG_layer);
#endif	

    esMEMS_Bfree(explr_ctrl, sizeof(explorer_ctrl_t));

    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:           static __s32 app_explorer_on_command(__gui_msg_t *msg)
*
*Description	: 		用户自定义命令处理函数
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 app_explorer_on_command(__gui_msg_t *msg)
{
    __gui_msg_t my_msg;
    explorer_ctrl_t	*explr_ctrl;
    explr_ctrl	 = (explorer_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (LOWORD(msg->dwAddData1) == DELETE_FILE_DIALOG_ID
            || LOWORD(msg->dwAddData1) == FW_UPDATE_DIALOG_ID)
    {
        if(explr_ctrl->h_list_framewin == NULL)
        {
            return EPDK_OK;
        }
        my_msg.h_deswin = explr_ctrl->h_list_framewin;

        my_msg.h_srcwin = msg->h_deswin;

        my_msg.id = GUI_MSG_COMMAND;
        my_msg.dwAddData1 = msg->dwAddData1;//
        my_msg.dwAddData2 = msg->dwAddData2;
        my_msg.dwReserved = msg->dwReserved;

        __here__;
        //GUI_SendNotifyMessage(msg);
        GUI_SendMessage(&my_msg);
        __here__;
    }
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

void  app_explorer_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    char buf[128];
    //H_WIN hExpWin;
    //hExpWin = GUI_WinGetParent(hwin);
    msg.h_deswin = GUI_WinGetParent(hwin);
    //msg.h_srcwin = NULL;
    msg.h_srcwin = hwin;

    __here__;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;
    GUI_WinGetName(msg.h_deswin, buf);
    __msg("%s\n", buf);
    GUI_SendNotifyMessage(&msg);
}

static __s32  ExplorerGetSearchPath(__s32 root_type, char *path)
{
    __s32 ret = 0;
    char str_root[50];
    char disk_name[RAT_MAX_PARTITION][4];

    eLIBs_memset(str_root, 0, sizeof(str_root));
    eLIBs_memset(disk_name, 0, sizeof(disk_name));

    switch(root_type)
    {
    case RAT_LOCAL:
        eLIBs_strcpy(str_root, RAT_LOCAL_DISK);
        break;
    case RAT_TF:
        eLIBs_strcpy(str_root, RAT_SD_CARD);
        break;
    case RAT_USB:
        eLIBs_strcpy(str_root, RAT_USB_DISK);
        break;
    default:
        break;
    }

    __msg("root_type=%s\n", root_type);
    ret = rat_get_partition_name(str_root, disk_name);

    eLIBs_strcpy(path, disk_name[0]);
    __msg("disk_name[0]=%s\n", disk_name[0]);

    if(path[0])
    {
        return EPDK_OK;
    }
    else
    {
        return EPDK_FAIL;
    }
}

/*
************************************************************************************************************************
*Function	:           static __s32 _explorer_manager_win_cb(__gui_msg_t *msg)
*
*Description	: 		manager window contrl block
*					gets message from system, & dispatch command to child
*Arguments  	: 		get a message from system
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 _explorer_manager_win_cb(__gui_msg_t *msg)
{
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
        __msg("----explorer manager window GUI_MSG_CREATE begin----\n");
		gscene_hbar_set_state(HBAR_ST_SHOW);
        explorer_scene_create(msg);
        return EPDK_OK;
    case GUI_MSG_CLOSE:
        __msg("----explorer manager window GUI_MSG_CLOSE begin----\n");
        app_explorer_on_close(msg);
        return EPDK_OK;
    case GUI_MSG_DESTROY:
        __msg("----explorer manager window GUI_MSG_DESTROY begin----\n");
		gscene_hbar_set_state(HBAR_ST_HIDE);
        app_explorer_on_destroy(msg->h_deswin);			//release  layer,memory
        return EPDK_OK;
    case GUI_MSG_KEY:
        break;
    case GUI_MSG_TOUCH:
        break;
    case GUI_MSG_COMMAND:
        __msg("----explorer manager window GUI_MSG_CLOSE begin----\n");
        app_explorer_on_command(msg);        
        return EPDK_OK;
    case DSK_MSG_FS_PART_PLUGOUT:
    {
        explorer_ctrl_t	*explr_ctrl;

        explr_ctrl = (explorer_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

        __msg("----explorer manager window DSK_MSG_FS_PART_PLUGOUT begin----\n");
        if(RAT_MEDIA_TYPE_ALL == explr_ctrl->media_type)//如果是文件管理
        {
            app_explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
        }
        else//如果是浏览
        {
            char diskname[2] = {0};

            diskname[0] = (char)(msg->dwAddData2);
            diskname[1] = 0;

            __msg("explr_ctrl->root=%s\n", explr_ctrl->root);
            __msg("diskname=%s\n", diskname);

            __msg("explr_ctrl->root[0]=%d\n", explr_ctrl->root[0]);
            //如果已经拔卡，可能来不及读取到卡的盘符，这个时候explr_ctrl->root为空，也要退出
            if(0 == explr_ctrl->root[0]
                    || 0 == eLIBs_strnicmp(explr_ctrl->root, diskname, 1))
            {
                __here__;
                app_explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
            }
        }

        __msg("----explorer manager window DSK_MSG_FS_PART_PLUGOUT end----\n");

        return EPDK_OK;
    }
    default:
        break;
    }

    return GUI_ManWinDefaultProc(msg);
}


/*
************************************************************************************************************************
*Function	:          static H_LYR explorer_8bpp_layer_create(void)
*
*Description	: 		背景图层，主要用来显示8bpp 背景图片
*					注意与前景图应处于不同的pipe
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static H_LYR explorer_8bpp_layer_create(void)
{
    H_LYR layer = NULL;
    RECT LayerRect;
    //int ret;
    //__layerwincreate_para_t lyrcreate_info;
    FB  fb =
    {
        {0, 0},                                   		/* size      */
        {0, 0, 0},                                      /* buffer    */
        //{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
        //{FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},    /* fmt       */
        {FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},    /* fmt       */
    };

    __disp_layer_para_t lstlyr =
    {
        //DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
        DISP_LAYER_WORK_MODE_NORMAL,
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        0,                                              /* pipe      */
        0xff,                                           /* prio      */
        {0, 0, 0, 0},                           		/* screen    */
        {0, 0, 0, 0},                               	/* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "APP_EXPLORER",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };

    explorer_get_screen_rect(&LayerRect);

    fb.size.width		= LayerRect.width;
    fb.size.height		= LayerRect.height;
    fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888; // PIXEL_MONO_8BPP; //PIXEL_COLOR_ARGB8888;

    lstlyr.src_win.x  		= LayerRect.x;
    lstlyr.src_win.y  		= LayerRect.y;
    lstlyr.src_win.width 	= LayerRect.width;
    lstlyr.src_win.height 	= LayerRect.height;

    lstlyr.scn_win.x		= LayerRect.x;
    lstlyr.scn_win.y		= LayerRect.y;
    lstlyr.scn_win.width  	= LayerRect.width;
    lstlyr.scn_win.height 	= LayerRect.height;

    lstlyr.pipe = 0;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;

    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if( !layer )
    {
        __err("app bar layer create error !\n");
    }
    return layer;
}

/*
************************************************************************************************************************
*Function	:          static H_LYR explorer_32bpp_layer_create(void)
*
*Description	: 		前景图层，主要用来显示前景图片，ListBar组件item, 缩略图
*					注意与背景图应处于不同的pipe
*
*Arguments  	:
*
*Return     	: 	     图层句柄
*
************************************************************************************************************************
*/
static H_LYR explorer_32bpp_layer_create(void)
{
    H_LYR layer = NULL;
    RECT LayerRect;
    //	int ret;
    //__layerwincreate_para_t lyrcreate_info;
    FB  fb =
    {
        {0, 0},                                   		/* size      */
        {0, 0, 0},                                      /* buffer    */
        //{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
        {FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},    /* fmt       */

    };

    __disp_layer_para_t lstlyr =
    {
        //DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
        DISP_LAYER_WORK_MODE_NORMAL,
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        0xff,                                           /* prio      */
        {0, 0, 0, 0},                           		/* screen    */
        {0, 0, 0, 0},                               	/* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "APP_EXPLORER",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };

    explorer_get_screen_rect(&LayerRect);

    fb.size.width		= LayerRect.width;
    fb.size.height		= LayerRect.height;
    fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888; //PIXEL_MONO_8BPP; //PIXEL_COLOR_ARGB8888;

    lstlyr.src_win.x  		= LayerRect.x;
    lstlyr.src_win.y  		= LayerRect.y;
    lstlyr.src_win.width 	= LayerRect.width;
    lstlyr.src_win.height 	= LayerRect.height;

    lstlyr.scn_win.x		= LayerRect.x;
    lstlyr.scn_win.y		= LayerRect.y;
    lstlyr.scn_win.width  	= LayerRect.width;
    lstlyr.scn_win.height 	= LayerRect.height;

    lstlyr.pipe = 1;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;

    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if( !layer )
    {
        __err("app bar layer create error !\n");
    }
    return layer;
}
/*
************************************************************************************************************************
*Function	:           static __s32 explorer_layer_delete(H_LYR layer)
*
*Description	: 		删除图层
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 explorer_layer_delete(H_LYR layer)
{
    if(layer != NULL)
    {
        GUI_LyrWinDelete(layer);
    }
    return EPDK_OK;
}
#ifdef EXPLORE_BG_ON
static __s32 explorer_draw_bg(explorer_ctrl_t	*explr_ctrl)
{
    if(!explr_ctrl || !explr_ctrl->BG_layer)
    {
        __msg("invalid para...\n");
        return EPDK_FAIL;
    }

    {
        __s32 index;
        __s32 bg_id;
        reg_system_para_t *para;
        HTHEME hdl;

        para = dsk_reg_get_para_by_app(REG_APP_SYSTEM);
        if(para)
        {
            index = para->style;
        }
        else
        {
            index = 0;
        }
#if 0
        switch(index)
        {
        case 0:
        {
            bg_id = ID_EXPLORER_BG0_BMP;
            break;
        }
        case 1:
        {
            bg_id = ID_EXPLORER_BG1_BMP;
            break;
        }
        case 2:
        {
            bg_id = ID_EXPLORER_BG2_BMP;
            break;
        }
        default:
        {
            bg_id = ID_EXPLORER_BG0_BMP;
            break;
        }
        }
#endif
        bg_id = ID_EXPLORER_BG2_BMP;

        GUI_LyrWinSetSta(explr_ctrl->BG_layer, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSetBottom(explr_ctrl->BG_layer);

        GUI_LyrWinSel(explr_ctrl->BG_layer);
        // com_set_palette_by_id(ID_EXPLORER_BG_PAL_BMP);
        hdl = dsk_theme_open(bg_id);
        if(hdl)
        {
            RECT rect;

            GUI_LyrWinGetScnWindow(explr_ctrl->BG_layer, &rect);
            GUI_BMP_Draw(dsk_theme_hdl2buf(hdl), rect.x, rect.y);
            dsk_theme_close(hdl);
            hdl = NULL;
            gscene_bgd_set_state(BGD_STATUS_HIDE);
        }
    }

    return EPDK_OK;
}
#endif


/*
************************************************************************************************************************
*Function	:           static __s32 explorer_scene_create()
*
*Description	: 		创建浏览器场景，ListBar
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/

static __s32 explorer_scene_create(__gui_msg_t *msg)
{
    explorer_ctrl_t	*explr_ctrl;
    //RECT LayerRect;
    explr_list_para_t ListPara;

    eLIBs_memset(&ListPara, 0, sizeof(explr_list_para_t));

	__msg("explorer_scene_create\n");
	
    explr_ctrl = (explorer_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    ListPara.root_type = explr_ctrl->root_type;
    ListPara.last_start_id = explr_ctrl->last_start_id;
    ListPara.last_focused_id = explr_ctrl->last_focused_id;
    ListPara.last_filename = explr_ctrl->last_filename;		//获得上一次浏览信息

    ListPara.media_type = explr_ctrl->media_type;
    ListPara.view_mode = explr_ctrl->view_mode;		//view mode,
    ListPara.root_para = explr_ctrl->root_para ;
    if(RAT_MEDIA_TYPE_ALL == explr_ctrl->media_type)
    {
        eLIBs_strcpy(explr_ctrl->root, "");
    }
    else
    {
        ExplorerGetSearchPath(explr_ctrl->root_type, explr_ctrl->root);
        __msg("explr_ctrl->root=%s\n", explr_ctrl->root);
    }
#ifdef EXPLORE_BG_ON
    explr_ctrl->BG_layer = explorer_8bpp_layer_create();
#endif
    explr_ctrl->list_layer = explorer_32bpp_layer_create();

#ifdef EXPLORE_BG_ON
    explorer_draw_bg(explr_ctrl);
    ListPara.BG_layer = explr_ctrl->BG_layer;		//背景图层
#endif
    ListPara.list_layer = explr_ctrl->list_layer;	//listbar layer

    explr_ctrl->h_list_framewin = explorer_list_win_create(msg->h_deswin, &ListPara);
    GUI_WinSetFocusChild(explr_ctrl->h_list_framewin);		//frame win get focus.					//set picture layer as top level
#ifdef EXPLORE_BG_ON
    GUI_LyrWinSetSta(explr_ctrl->BG_layer, GUI_LYRWIN_STA_ON);
#endif
    GUI_LyrWinSetSta(explr_ctrl->list_layer, GUI_LYRWIN_STA_ON);

    return EPDK_OK;
}

