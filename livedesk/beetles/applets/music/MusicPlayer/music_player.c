/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
* File    	: 	music_player.h
*
* By      	: 	Libaiao
* Func	:
* Version	: 	v1.0
* ============================================================================================================
* 2010-5-3 15:24:52  Libaiao  create this file, implements the fundemental interface;
**************************************************************************************************************
*/




#include "beetles_app.h"
#include "music.h"
#include "music_ui.h"
#include "music_list.h"
#include "music_player.h"

#include "player_face.h"

#include "spectrum_i.h"
#include "MusicVolumeBar.h"

#include "music_lyrics.h"

//#include "Music_spectrum.h"

static __u32 MusicTimerId = 0x01;
static __u32 Music_FF_RR_TimerId = 0x05;
static __u32 MusicSysTimerId = 0x10;

static __u32 MusicCheckFocusTimerId = 0x10;				//检测当前条目是否与实际播放条目相同


H_WIN 			music_player_win_create(H_WIN h_parent, music_player_ctrl_t *para);
static __s32 	_music_player_win_cb(__gui_msg_t *msg);
static __s32 	music_player_win_key_proc(__gui_msg_t *msg);
static __s32 	music_player_scene_create(H_WIN  music_frame_win);								//创建场景
static __s32 	music_player_win_on_close(__gui_msg_t *msg);
static __s32  	music_player_win_on_destroy(__gui_msg_t *msg);
static __s32 	music_player_win_on_paint(H_WIN  music_frame_win);


static __s32 	music_play_file(music_player_ctrl_t *player_ctrl, char *filename);

static __s32 	music_player_device_init(music_player_ctrl_t *player_ctrl);	//播放音频设备初始化
static __s32 	music_player_device_uninit(music_player_ctrl_t *player_ctrl);//关闭音频设备

static __s32 	music_player_win_on_timer(__gui_msg_t *msg);

static __s32 	music_player_check_play_status_on_timer(music_player_ctrl_t *this);
__s32 			music_send_command(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);
static __s32 	music_player_set_robin_EQ_mode(music_player_ctrl_t *this);


__s32 			music_player_get_last_para(music_player_ctrl_t *This);
__s32 			music_player_save_last_para(music_player_ctrl_t *This);

__s32 			create_music_lyrics_scene(music_player_ctrl_t *This);
__s32 			delete_music_lyrics_scene(music_player_ctrl_t *This);


__s32 music_player_win_on_sys_timer(music_player_ctrl_t *this);


__s32 music_restore_play_file_info(music_player_ctrl_t *this);

//extern __s32 draw_music_player_background(music_player_ctrl_t *player_ctrl);

/*
************************************************************************************************************************
*Function	:          H_WIN music_player_win_create(H_WIN h_parent, music_player_ctrl_t *para)
*
*Description	: 		创建MusicPlayer framewin
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
H_WIN music_player_win_create(H_WIN h_parent, music_player_ctrl_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    music_player_ctrl_t *music_player_ctrl;
    FB fb;

    //app_print_memory_usage("music player win create", NULL);

    GUI_LyrWinGetFB(para->music_layer, &fb);

    music_player_ctrl = (music_player_ctrl_t *)esMEMS_Balloc(sizeof(music_player_ctrl_t));
    eLIBs_memset((void *)music_player_ctrl, 0, sizeof(music_player_ctrl_t));

    music_player_ctrl->font = para->font;
    //music_player_ctrl->background_layer = para->background_layer;
    music_player_ctrl->music_layer = para->music_layer;

    music_player_ctrl->media_type = para->media_type;		//photo, music, movie

    music_player_ctrl->root_type = para->root_type;							//SD or USB device
    music_player_ctrl->visiable = para->visiable;				//是否背景播放

    music_player_ctrl->explorer_play_music = para->explorer_play_music;
    music_player_ctrl->music_file_name = para->music_file_name;
    //music_player_ctrl->view_mode = para->view_mode;							//list or square
    //ExplorerListWinGetSearchPath(list_para);
    //eLIBs_memcpy(music_player_ctrl->search_path,"e:", sizeof("e:"));
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =	"Explorer List window",
                  framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner	= NULL;
    framewin_para.id        = MUSIC_LIST_WIN_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_music_player_win_cb);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 255;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr =  (void *)music_player_ctrl;
    framewin_para.hLayer = music_player_ctrl->music_layer;			//会被listbar调用
    return (GUI_FrmWinCreate(&framewin_para));						//在framewin 的create消息中将handle,传送给music_player_ctrl->h_framewin
}

__s32 music_player_on_power_charge(__gui_msg_t *msg)
{
    music_player_ctrl_t *this;
    power_level_e power_level = 0;

    this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    if(this == NULL)
    {
        return EPDK_FAIL;
    }

    if(!this->visiable)
    {
        return EPDK_FAIL;
    }
    power_level = msg->dwAddData1;
    DrawMusicBatteryStatus(power_level);
    return EPDK_OK;
}



__s32 music_player_win_on_command(__gui_msg_t *msg)
{
    switch(msg->dwAddData1)
    {
    case MUSICPLAYER_BACKGROUND:

        music_player_scene_delete(msg->h_deswin);

        //send message to main menu
        music_send_command(msg->h_deswin, GUI_MSG_COMMAND, SWITCH_TO_MMENU, MUSICPLAYER_BACKGROUND);
        break;
    }
    return EPDK_OK;
}
/*
************************************************************************************************************************
*Function	:          static __s32 _music_player_win_cb(__gui_msg_t *msg)
*
*Description	: 		framewin 消息处理函数
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 _music_player_win_cb(__gui_msg_t *msg)
{
    __s32 ret = EPDK_OK;
    music_player_ctrl_t *this;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    //__msg("music player win msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
    //  ,msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);
    switch(msg->id)
    {
    case GUI_MSG_CREATE:
        __msg("----explorer frame window GUI_MSG_CREATE begin----\n");
        music_player_scene_create(msg->h_deswin);   									//on create message
        return EPDK_OK;
    case GUI_MSG_PAINT:
        __msg("----explorer frame window GUI_MSG_ONPAINT----\n");
        music_player_win_on_paint(msg->h_deswin);
        return EPDK_OK;
    case GUI_MSG_CLOSE:
        __msg("----explorer frame window GUI_MSG_CLOSE begin----\n");
        music_player_win_on_close(msg);
        return EPDK_OK;
    case GUI_MSG_DESTROY:
        __msg("----explorer frame window GUI_MSG_DESTROY begin----\n");
        music_player_win_on_destroy(msg);
        __msg("----explorer frame window GUI_MSG_DESTROY end----\n");
        return EPDK_OK;
    case GUI_MSG_COMMAND:
        music_player_win_on_command(msg);
        break;
    case GUI_MSG_KEY:
        if(this->visiable)		//有界面显示才会按键操作
        {

            music_player_win_key_proc(msg);
        }
        break;
    case DSK_MSG_VOLTAGE_CHANGE:
        music_player_on_power_charge(msg);
        return EPDK_OK;
    case GUI_MSG_TIMER:
        if(MusicSysTimerId == msg->dwAddData1)	//定时去更新播放状态)
        {
            music_player_win_on_sys_timer(this);
        }
        else if(MusicCheckFocusTimerId == msg->dwAddData1)
        {
            //music_player_win_check_focus_on_timer(this);
        }
        else if(MusicTimerId == msg->dwAddData1)
        {
            music_player_win_on_timer(msg);
        }
        break;
    case DSK_MSG_FS_PART_PLUGOUT:
        __msg("SD card plugout!");
        //music_send_command(msg->h_deswin, GUI_MSG_COMMAND, SWITCH_TO_MMENU, 0);
        break;
    default:
        ret = GUI_FrmWinDefaultProc(msg);
        return ret;
    }
    return GUI_FrmWinDefaultProc(msg);
}

//NextKey处理函数
static __s32 music_explorer_on_next_key(__gui_msg_t *msg)
{
    music_player_ctrl_t *player_ctrl = NULL;
    music_list_ctrl_t *ListCtrl = NULL;
    char *filename = NULL;
    __listbar_scene_t scene;
    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    if(player_ctrl == NULL)
    {
        return EPDK_FAIL;
    }
    ListCtrl = player_ctrl->list_ctrl;
    if(ListCtrl == NULL)
    {
        return EPDK_FAIL;
    }
    filename = player_ctrl->play_file_info.filename;

    LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
    __msg("\n\nlistbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);

    ListCtrl->OnNextKey((void *)ListCtrl);				//listbar on nextkey

    LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
    __msg("\n\nlistbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
    //show_music_list(ListCtrl);
    //get_music_list_focus_item_full_filename(ListCtrl, filename);

    //music_play_file(player_ctrl, filename);
    player_ctrl->CheckFocusItemCnt = 0;		//有按键清除检测焦点条目定时计数器
    return EPDK_OK;
}

//NextKey处理函数
static __s32 music_explorer_on_previous_key(__gui_msg_t *msg)
{
    music_player_ctrl_t *player_ctrl;
    music_list_ctrl_t *ListCtrl;
    //char *filename = NULL;
    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    if(player_ctrl == NULL)
    {
        return EPDK_FAIL;
    }
    ListCtrl = player_ctrl->list_ctrl;
    if(ListCtrl == NULL)
    {
        return EPDK_FAIL;
    }
    //filename = player_ctrl->play_file_info.filename;

    ListCtrl->OnPreviousKey((void *)ListCtrl);			//listbar on previous key
    //show_music_list(ListCtrl);
    //get_music_list_focus_item_full_filename(ListCtrl, filename);

    //music_play_file(player_ctrl, filename);
    player_ctrl->CheckFocusItemCnt = 0;		//有按键清除检测焦点条目定时计数器
    return EPDK_OK;
}

static __s32 music_explorer_on_menu_key(__gui_msg_t *msg)
{
    music_player_ctrl_t *this;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if(this->menu_mode == MUSIC_EXPLORER_MODE)
    {
        {
            music_palyer_ui_t *player_ui;
            GUI_MEMDEV_Handle	draw_mem;
            player_ui = music_get_player_ui_param();

            draw_mem = GUI_MEMDEV_Create(player_ui->contrl_BG_rect.x, player_ui->contrl_BG_rect.y, player_ui->contrl_BG_rect.width, player_ui->contrl_BG_rect.height);

            GUI_MEMDEV_Select(draw_mem);

            this->menu_mode = MUSIC_SET_PLAYER_MODE;

            GUI_UC_SetEncodeUTF8();				//UTF8字体显示

            //GUI_LyrWinCacheOn();

            ClearMusicPlayerContrlButtonBG();
            DrawMusicPlayerContrlButton(this);

            GUI_MEMDEV_CopyToLCD(draw_mem);
            GUI_MEMDEV_Select( NULL );
            GUI_MEMDEV_Delete(draw_mem);

            DrawMusicPlayerContrlButtonByID(this, this->menu_id, FOCUSED);
        }
        //GUI_LyrWinCacheOff();
    }
    else
    {
        //this->menu_mode = MUSIC_EXPLORER_MODE;
        //__msg("error in music explorer");
    }
    return EPDK_OK;
}
static __s32 music_explorer_on_enter_key(__gui_msg_t *msg)
{
    __s32 ret = 0;
    music_player_ctrl_t *This;
    music_list_ctrl_t *ListCtrl;
    char *filename;
    This = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    ListCtrl = This->list_ctrl;
    filename = This->play_file_info.filename;

    ret = get_music_list_focus_item_full_filename(ListCtrl, filename);
    if(ret != EPDK_FAIL)
    {
        ListCtrl->play_index = ret;
    }

    music_play_file(This, filename);
    return EPDK_OK;
}
static __s32 music_explorer_key_proc(__gui_msg_t *msg)
{
    static __u32 last_key = 0;
    //music_player_ctrl_t *player_ctrl = NULL;
    //player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if((KEY_DOWN_ACTION == msg->dwAddData2) || (KEY_REPEAT_ACTION == msg->dwAddData2))
    {
        //GUI_ResetTimer(msg->h_deswin, MusicCheckFocusTimerId, MUSIC_CHECK_FOCUS_TIMER_INTERVAL, NULL);
        switch(msg->dwAddData1)
        {
        case GUI_MSG_KEY_VDEC:
            //ExplorerListWinOnPreviousPage(list_para);
            break;
        case GUI_MSG_KEY_ENTER:
            music_explorer_on_enter_key(msg);
            break;
        case GUI_MSG_KEY_VADD:
            last_key = GUI_MSG_KEY_VADD;
            break;
        case GUI_MSG_KEY_MENU:
            last_key = GUI_MSG_KEY_MENU;
            //case GUI_MSG_KEY_LONGMENU:
            //music_explorer_on_menu_key(msg);
            //music_send_command(msg->h_deswin, 0, EXPLR_SW_TO_MAIN, 0);
            break;

        case GUI_MSG_KEY_ESCAPE:
            music_send_command(msg->h_deswin, 0, SWITCH_TO_MMENU, 0);
            break;
        case GUI_MSG_KEY_RISE:
            break;
        default:
            last_key = msg->dwAddData1;
            break;
        }
    }
    else if(KEY_UP_ACTION == msg->dwAddData2)
    {
        switch(msg->dwAddData1)
        {
        //case GUI_MSG_KEY_ENTER:	//因为现在不支持定时长按键，因此
        //只能判断Menu Up时才会响应close消息
        case GUI_MSG_KEY_RIGHT:
        case GUI_MSG_KEY_LONGRIGHT:
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LONGUP:
            __msg("----explorer list window on UP key----\n");
            //_long_string_stop_roll();
            //ExplorerListWinOnNextItem(list_para);
            music_explorer_on_next_key(msg);
            break;
        case GUI_MSG_KEY_LEFT:
        case GUI_MSG_KEY_LONGLEFT:
        case GUI_MSG_KEY_DOWN:
        case GUI_MSG_KEY_LONGDOWN:
            //__msg("----explorer list window on DOWN key----\n");
            //ExplorerListWinOnPreviousItem(list_para);
            //ListCtrl->OnPreviousKey((void *)ListCtrl);
            music_explorer_on_previous_key(msg);
            break;

        case GUI_MSG_KEY_MENU:
            if(last_key == GUI_MSG_KEY_MENU)
            {
                if(robin_get_fsm_status() != CEDAR_STAT_PLAY)			//关闭音乐
                {
                    music_send_command(msg->h_deswin, GUI_MSG_COMMAND, SWITCH_TO_MMENU, 0);
                }
                else
                {
                    //退出时，正在播放则进入背景播放状?
                    //__wait__;
                    //music_send_command(msg->h_deswin, GUI_MSG_COMMAND, EXPLR_SW_TO_MAIN, MUSICPLAYER_BACKGROUND);
                    {
                        __gui_msg_t my_msg;
                        //my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                        my_msg.h_deswin = msg->h_deswin;
                        my_msg.h_srcwin = msg->h_deswin;
                        my_msg.id = GUI_MSG_COMMAND;
                        my_msg.dwAddData1 = MUSICPLAYER_BACKGROUND;//发送消息给音乐AP要进入背景播放状态，释放场景

                        //GUI_SendNotifyMessage(&msg);
                        GUI_SendMessage(&my_msg);
                    }
                }
            }
            break;
        case GUI_MSG_KEY_LONGMENU:
            if(last_key == GUI_MSG_KEY_MENU)
            {
                music_explorer_on_menu_key(msg);
                __msg("start volume bar\n");
            }
            break;
        case GUI_MSG_KEY_LONGVADD:
            //if(last_key == GUI_MSG_KEY_VADD)
        {
            music_player_ctrl_t *player_ctrl = NULL;
            player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
            MusicVolumeBarCreate(player_ctrl);
        }
        break;
        }
        last_key = 0;
    }
    return EPDK_OK;

}


//检测播放状态,并根据playmode获得相关的文件,启动播放器
static __s32 music_player_check_play_status_on_timer(music_player_ctrl_t *this)
{
    __cedar_status_t  status;
    __s32 index;
    char *filename;
    music_list_ctrl_t *ListCtrl = NULL;
    __listbar_scene_t scene;

    status = robin_get_fsm_status();
    if(status == CEDAR_STAT_STOP)
    {
        if(this->play_mode == RAT_PLAY_MODE_ONLY_ONCE)
        {
            if(this->menu_mode == MUSIC_EXPLORER_MODE)
            {
                if(this->h_lyrics != NULL)
                {

                }
                else
                {
                    SPCTRM_Clear(0);		//显示最大值
                }
            }
            return EPDK_OK;
        }
        //if(this->play_mode == RAT_PLAY_MODE_RANDOM)
        {
            //return EPDK_OK;
        }
        index = robin_npl_get_next();
        __msg("\n\n\n--------------------------------robin npl get index = %d\n", index);
        if(index != -1)
        {
            if(this->list_ctrl != NULL)
            {
                ListCtrl = this->list_ctrl;
                ListCtrl->play_index = index;
            }
            filename = this->play_file_info.filename;
            robin_npl_index2file(index, filename);
            __msg("\n\n\n--------------------------------robin npl get filename= %s\n", filename);
#if EPDK_PLAY_VIDEO_AUDIO_ENABLE
            {
                rat_media_type_t media_type;
                char filename[256] ;
                H_WIN	h_win ;
                robin_npl_index2file( index, filename);
                h_win = GUI_WinGetHandFromName(APP_MUSIC);
                media_type = rat_get_file_type( filename );
                DebugPrintf("filename=%s,media_type", filename, media_type);
                if( media_type == RAT_MEDIA_TYPE_VIDEO)
                {
                    music_send_command(h_win, 0, AUDIO_SW_TO_VIDEO, 0);
                }
            }
            return EPDK_OK ;
#endif
            //music_list_set_item_scene(ListCtrl, &(ListCtrl->listbar_play_scene));
            music_play_file(this, filename);
            if(this->play_mode == RAT_PLAY_MODE_RANDOM)
            {
                //music_play_file(this, filename);
                LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
                __msg("\n\nlistbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
                music_list_set_focus_item(ListCtrl, index);
                LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
                __msg("\n\nlistbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
                ListCtrl->play_index = scene.focus_id;
                return EPDK_OK;
            }

            if(this->visiable)		//背景播放则不显示listbar
            {
                if((this->play_mode == RAT_PLAY_MODE_ROTATE_ALL)
                        || (this->play_mode == RAT_PLAY_MODE_SEQUENCE))
                {
                    {
                        LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
                        __msg("\n\n-----------play index = %d\n", ListCtrl->play_index);
                        __msg("\n\n------------------listbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
                        if(ListCtrl->play_index != scene.focus_id + 1)
                        {
                            music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
                        }
                        else
                        {
                            ListCtrl->OnNextKey((void *)ListCtrl);	//listbar on previous key
                        }
                        LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
                        __msg("\n\n------------------listbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
                        ListCtrl->play_index = scene.focus_id;
                        __msg("\n\n-----------play index = %d\n", ListCtrl->play_index);
                    }
                }
            }
            //music_play_file(this, filename);
        }
        //robin_npl_get_cur(void);
    }
    return EPDK_OK;
}
__s32 music_player_get_last_para(music_player_ctrl_t *This)
{
    __s32 ret = 0;
    reg_music_para_t *music_last_para = NULL;
    reg_system_para_t *system_last_para = NULL;
    //__wait__;
    music_last_para = dsk_reg_get_para_by_app(REG_APP_MUSIC);
    system_last_para = dsk_reg_get_para_by_app(REG_APP_SYSTEM);
    //This->play_index = music_last_para->play_index;		//获得实际播放的索引号

    This->EQ_mode = music_last_para->EQ_mode;
    This->play_mode = music_last_para->play_mode;
    //This->BL_time = music_last_para->BL_time;


    switch(This->play_mode)
    {
    case RAT_PLAY_MODE_SEQUENCE:
        break;
    case RAT_PLAY_MODE_ROTATE_ONE:
        break;
    case RAT_PLAY_MODE_ROTATE_ALL:
        break;
    case RAT_PLAY_MODE_ONLY_ONCE:
        break;
    case RAT_PLAY_MODE_RANDOM:
        break;
    default:
        This->play_mode = RAT_PLAY_MODE_SEQUENCE;
        break;
    }

    switch(This->EQ_mode)
    {
    case MUSIC_EQ_NORMAL:
        break;
    case MUSIC_EQ_ROCK:
        break;
    case MUSIC_EQ_CLASSIC:
        break;
    case MUSIC_EQ_JAZZ:
        break;
    case MUSIC_EQ_POP:
        break;
    default:
        This->EQ_mode = MUSIC_EQ_NORMAL;
        break;
    }

    switch(system_last_para->backlightoff)
    {
    case 0:
        This->BL_time = MUSIC_BL_ON;
        break;
    case 10:
        This->BL_time = MUSIC_BL_LAST_10S;
        break;
    case 30:
        This->BL_time = MUSIC_BL_LAST_30S;
        break;
    case 60:
        This->BL_time = MUSIC_BL_LAST_60S;
        break;
    case 90:
        This->BL_time = MUSIC_BL_LAST_90S;
        break;
    default:
        This->BL_time = MUSIC_BL_ON;
        break;
    }
    return EPDK_OK;
}

//保存注册表信息直接操作结构体
__s32 music_player_save_last_para(music_player_ctrl_t *This)
{
    __s32 ret = 0;
    reg_music_para_t *music_last_para;
    reg_system_para_t *system_last_para = NULL;

    system_last_para = dsk_reg_get_para_by_app(REG_APP_SYSTEM);

    music_last_para = dsk_reg_get_para_by_app(REG_APP_MUSIC);

    music_last_para->EQ_mode = This->EQ_mode;
    music_last_para->play_mode = This->play_mode;
    //music_last_para->BL_time = ;
    switch(This->BL_time)
    {
    case MUSIC_BL_ON:
        system_last_para->backlightoff = 0;
        break;
    case MUSIC_BL_LAST_10S:
        system_last_para->backlightoff = 10;
        break;
    case MUSIC_BL_LAST_30S:
        system_last_para->backlightoff = 30;
        break;
    case MUSIC_BL_LAST_60S:
        system_last_para->backlightoff = 60;
        break;
    case MUSIC_BL_LAST_90S:
        system_last_para->backlightoff = 90;
        break;
    default:
        system_last_para->backlightoff = 0;
        break;
    }

    return EPDK_OK;
}

static __s32 music_player_set_play_mode(music_player_ctrl_t *this)
{
    switch(this->play_mode)
    {
    case RAT_PLAY_MODE_SEQUENCE:
        this->play_mode = RAT_PLAY_MODE_ROTATE_ONE;
        break;
    case RAT_PLAY_MODE_ROTATE_ONE:
        this->play_mode = RAT_PLAY_MODE_ROTATE_ALL;
        break;
    case RAT_PLAY_MODE_ROTATE_ALL:
        this->play_mode = RAT_PLAY_MODE_ONLY_ONCE;
        break;
    case RAT_PLAY_MODE_ONLY_ONCE:
        this->play_mode = RAT_PLAY_MODE_RANDOM;
        //this->play_mode = RAT_PLAY_MODE_SEQUENCE;
        break;
    case RAT_PLAY_MODE_RANDOM:
        this->play_mode = RAT_PLAY_MODE_SEQUENCE;
        break;
    default:
        break;
    }

    DrawMusicPlayModeButton(this->play_mode, PRESSED);
    DrawMusicPlayModeStatus(this->play_mode);

    robin_set_play_mode(this->play_mode);

    return EPDK_OK;
}

static __s32 music_player_set_robin_EQ_mode(music_player_ctrl_t *this)
{
    __cedar_audio_eq_t eq_mode;
    switch(this->EQ_mode)
    {
    case MUSIC_EQ_NORMAL:
        eq_mode = CEDAR_AUD_EQ_TYPE_ROCK;
        break;
    case MUSIC_EQ_ROCK:
        eq_mode = CEDAR_AUD_EQ_TYPE_CLASSIC;
        break;
    case MUSIC_EQ_CLASSIC:
        eq_mode = CEDAR_AUD_EQ_TYPE_JAZZ;
        break;
    case MUSIC_EQ_JAZZ:
        eq_mode = CEDAR_AUD_EQ_TYPE_POP;
        break;
    case MUSIC_EQ_POP:
        eq_mode = CEDAR_AUD_EQ_TYPE_NORMAL;
        break;
    }

    robin_set_eq_mode(eq_mode);
    return EPDK_OK;
}

static __s32 music_player_set_EQ_mode(music_player_ctrl_t *this)
{
    __cedar_audio_eq_t eq_mode;

    switch(this->EQ_mode)
    {
    case MUSIC_EQ_NORMAL:
        this->EQ_mode = MUSIC_EQ_ROCK;
        eq_mode = CEDAR_AUD_EQ_TYPE_ROCK;
        break;
    case MUSIC_EQ_ROCK:
        this->EQ_mode = MUSIC_EQ_CLASSIC;
        eq_mode = CEDAR_AUD_EQ_TYPE_CLASSIC;
        break;
    case MUSIC_EQ_CLASSIC:
        this->EQ_mode = MUSIC_EQ_JAZZ;
        eq_mode = CEDAR_AUD_EQ_TYPE_JAZZ;
        break;
    case MUSIC_EQ_JAZZ:
        this->EQ_mode = MUSIC_EQ_POP;
        eq_mode = CEDAR_AUD_EQ_TYPE_POP;
        break;
    case MUSIC_EQ_POP:
        this->EQ_mode = MUSIC_EQ_NORMAL;
        eq_mode = CEDAR_AUD_EQ_TYPE_NORMAL;
        break;
    }


    DrawMusicEQModeStatus(this->EQ_mode);
    DrawMusicEQModeButton(this->EQ_mode, PRESSED);

    robin_set_eq_mode(eq_mode);

    return EPDK_OK;
}
static __s32 music_player_set_BL_time(music_player_ctrl_t *this)
{
    __u32 time = 0;
    switch(this->BL_time)
    {
    case MUSIC_BL_ON:
        this->BL_time = MUSIC_BL_LAST_10S;
        time = 10;
        break;
    case MUSIC_BL_LAST_10S:
        this->BL_time = MUSIC_BL_LAST_30S;
        time = 30;
        break;
    case MUSIC_BL_LAST_30S:
        this->BL_time = MUSIC_BL_LAST_60S;
        time = 60;
        break;
    case MUSIC_BL_LAST_60S:
        this->BL_time = MUSIC_BL_LAST_90S;
        time = 90;
        break;
    case MUSIC_BL_LAST_90S:
        this->BL_time = MUSIC_BL_ON;
        time = 0;
        break;
    default:
        time = 0;
        break;
    }
    g_set_close_scn_time(time);
    DrawMusicBLTimeButton(this->BL_time, PRESSED);
#if BEETLES_RTC_EXIST

#else
    DrawMusicBLLastTimeStatus(this->BL_time);			//RTC 不存在显示背光时间
#endif

    return EPDK_OK;
}

static __s32 music_player_set_play_previous(music_player_ctrl_t *this)
{
    music_list_ctrl_t *ListCtrl;
    char *filename;
    __listbar_scene_t scene;

    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    robin_set_cmd_stop();
    DrawMusicPlayPreButton(PRESSED);

    ListCtrl = this->list_ctrl;
    filename = this->play_file_info.filename;

    __msg("------------play previous file\n");
    //__wait__;
    {

        LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
        if(ListCtrl->play_index != scene.focus_id)
        {
            __msg("------------------current focus item has been moved!\n");
            music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
        }
    }					//

    ListCtrl->OnPreviousKey((void *)ListCtrl);					//listbar on previous key
    //music_list_set_item_scene(this->list_ctrl, &(this->list_ctrl->listbar_play_scene));

    LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
    ListCtrl->play_index = scene.focus_id;
    get_music_list_focus_item_full_filename(ListCtrl, filename);
    music_play_file(this, filename);

    return EPDK_OK;
}
static __s32 music_player_set_play_next(music_player_ctrl_t *this)
{
    music_list_ctrl_t *ListCtrl;
    char *filename;
    __listbar_scene_t scene;
    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    robin_set_cmd_stop();

    DrawMusicPlayNextButton(PRESSED);

    ListCtrl = this->list_ctrl;
    filename = this->play_file_info.filename;

    //music_list_set_item_scene(this->list_ctrl, &(this->list_ctrl->listbar_play_scene));
    //show_music_list(ListCtrl);
    __msg("------------play next file\n");
    {

        LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
        if(ListCtrl->play_index != scene.focus_id)
        {
            music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
        }
    }
    ListCtrl->OnNextKey((void *)ListCtrl);					//listbar on previous key
    LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
    ListCtrl->play_index = scene.focus_id;
    //music_list_set_item_scene(this->list_ctrl, &(this->list_ctrl->listbar_play_scene));
    get_music_list_focus_item_full_filename(ListCtrl, filename);

    music_play_file(this, filename);

    return EPDK_OK;
}

static __s32 music_player_set_play_pause(music_player_ctrl_t *this)
{
    if(robin_get_fsm_status() == CEDAR_STAT_PLAY)
    {
        robin_set_cmd_pause();
        DrawMusicPlayPauseButton(PRESSED);
        DrawMusicPlayStatusButton(MUSIC_PAUSE);
    }
    else
    {
        robin_set_cmd_play();
        DrawMusicPlayPauseButton(PRESSED);
        DrawMusicPlayStatusButton(MUSIC_PLAYING);
    }
    return EPDK_OK;
}

//不管在什么模式，对是否存在歌词进行检测
__s32  music_player_check_lyrics(music_player_ctrl_t *this, __bool bUpdate)
{
    if(this == NULL)
    {
        return EPDK_FAIL;
    }
    delete_music_lyrics_scene(this);
    //if(this->menu_mode == MUSIC_EXPLORER_MODE)
    {
        if(music_lyrics_exist())		//检查是否有歌词
        {
            //有歌词则不显示频谱
            if(create_music_lyrics_scene(this) != EPDK_FAIL)
            {
                __msg("---------------create music lyrics scene successfully!\n");
            }
            else
            {
                __wrn("---------------create music lyrics scene failed!\n");
            }
        }
    }
    return EPDK_OK;
}
static __s32 music_player_set_play_backward(music_player_ctrl_t *this)
{
    __s32 ret = 0;
    __u32 total_time = 0, cur_time = 0;
    __cedar_status_t play_status;
    DrawMusicPlayBackwardButton(PRESSED);
    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    __here__;
    DrawMusicPlayStatusButton(MUSIC_PLAYING);
    total_time = robin_get_total_time();

    play_status = robin_get_fsm_status();
    if(play_status == CEDAR_STAT_PAUSE)
    {
        robin_set_cmd_play();			//后退失败,则继续播放
        music_player_check_lyrics(this, EPDK_FALSE);
        cur_time = robin_get_cur_time();

        if(cur_time > total_time)
        {
            cur_time = total_time;
        }
        DrawMusicPlayingTime(cur_time);
        DrawMusicProgressBar(total_time, cur_time);
        return EPDK_OK;
    }
    else if(play_status == CEDAR_STAT_STOP)
    {
        __here__;
        this->CheckFocusItemCnt = 0;
        __msg("2------------check focus item on timer\n");
        music_player_win_check_focus_on_timer(this);	//检测当前焦点是否播放条目

        robin_set_cmd_play();			//后退失败,则继续播放
        music_player_check_lyrics(this, EPDK_FALSE);

        cur_time = robin_get_cur_time();

        if(cur_time > total_time)
        {
            cur_time = total_time;
        }
        DrawMusicPlayingTime(cur_time);
        DrawMusicProgressBar(total_time, cur_time);

        return EPDK_OK;
    }
    else if(play_status == CEDAR_STAT_RR )
    {
        cur_time = robin_get_cur_time();
        if(cur_time > total_time)
        {
            cur_time = total_time;
        }
        DrawMusicPlayingTime(cur_time);
        DrawMusicProgressBar(total_time, cur_time);
        return EPDK_OK;
    }

    robin_set_ff_rr_speed(MUSIC_RR_SPEED);
    //robin_set_cmd_play();
    ret = robin_set_cmd_rr();
    if(ret == -1)
    {
        robin_set_cmd_play();			//后退失败,则继续播放
        music_player_check_lyrics(this, EPDK_FALSE);
        DrawMusicProgressBar(0, 0);
        GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
        return EPDK_OK;
    }
    this->setting_status = CEDAR_STAT_RR;
    cur_time = robin_get_cur_time();

    if(cur_time > total_time)
    {
        cur_time = total_time;
    }
    DrawMusicPlayingTime(cur_time);
    DrawMusicProgressBar(total_time, cur_time);

    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);

    return EPDK_OK;
}

static __s32 music_player_set_play_forward(music_player_ctrl_t *this)
{
    __s32 ret = 0;
    __u32 total_time = 0, cur_time = 0;
    __cedar_status_t play_status;
    DrawMusicPlayForwardButton(PRESSED);

    DrawMusicPlayStatusButton(MUSIC_PLAYING);

    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);

    total_time = robin_get_total_time();
    __here__;

    play_status = robin_get_fsm_status();
    if(play_status == CEDAR_STAT_PAUSE)
    {
        robin_set_cmd_play();
        music_player_check_lyrics(this, EPDK_FALSE);
        cur_time = robin_get_cur_time();
        __here__;
        if(cur_time > total_time)
        {
            cur_time = total_time;
        }
        DrawMusicPlayingTime(cur_time);
        DrawMusicProgressBar(total_time, cur_time);
        return EPDK_OK;
    }
    else if(play_status == CEDAR_STAT_STOP)
    {
        __here__;

        robin_set_cmd_stop();

        this->CheckFocusItemCnt = 0;
        __msg("2------------check focus item on timer\n");
        music_player_win_check_focus_on_timer(this);	//检测当前焦点是否播放条目

        music_player_check_lyrics(this, EPDK_FALSE);

        cur_time = robin_get_cur_time();
        if(cur_time > total_time)
        {
            cur_time = total_time;
        }
        DrawMusicPlayingTime(cur_time);
        DrawMusicProgressBar(total_time, cur_time);
        this->setting_status = CEDAR_STAT_PLAY;
        return EPDK_OK;
    }
    else if( play_status == CEDAR_STAT_FF)
    {
        cur_time = robin_get_cur_time();
        __here__;
        if(cur_time > total_time)
        {
            cur_time = total_time;
        }
        DrawMusicPlayingTime(cur_time);
        DrawMusicProgressBar(total_time, cur_time);
        return EPDK_OK;
    }
    robin_set_ff_rr_speed(MUSIC_FF_SPEED);
    __here__;
    //robin_set_cmd_play();
    __here__;
    ret = robin_set_cmd_ff();
    __here__;
    if(ret == -1)
    {
        robin_set_cmd_play();			//后退失败,则继续播放
        music_player_check_lyrics(this, EPDK_FALSE);
        DrawMusicProgressBar(total_time, total_time);
        GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
        return EPDK_OK;
    }
    __here__;
    this->setting_status = CEDAR_STAT_FF;
    cur_time = robin_get_cur_time();
    __here__;
    if(cur_time > total_time)
    {
        cur_time = total_time;
    }
    //GUI_LyrWinCacheOn();
    DrawMusicPlayingTime(cur_time);
    //DrawMusicTotalTime(total_time);
    //DrawMusicProgressBarPoint(total_time, cur_time);
    DrawMusicProgressBar(total_time, cur_time);
    //GUI_LyrWinCacheOff();
    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    return EPDK_OK;
}

static __s32 music_player_on_setting(music_player_ctrl_t *this)
{
    switch(this->menu_id)
    {
    case SET_PLAY_MODE:
        music_player_set_play_mode(this);
        break;
    case SET_EQ_MODE:
        music_player_set_EQ_mode(this);
        break;
    case SET_BL_TIME:
        music_player_set_BL_time(this);
        break;
    case SET_PLAY_PREVIOUS:
        music_player_set_play_previous(this);
        break;
    case SET_PLAY_NEXT:
        music_player_set_play_next(this);
        break;
    case SET_PLAY_PAUSE:
        music_player_set_play_pause(this);
        break;
    case SET_PLAY_BACKWARD:
        music_player_set_play_backward(this);
        break;
    case SET_PLAY_FORWARD:
        music_player_set_play_forward(this);
        break;
    }
    return EPDK_OK;
}
//设置播放器按键处理函数
static __s32 music_setting_player_key_proc(__gui_msg_t *msg)
{
    static __u32 last_key = 0;
    music_player_ctrl_t *this;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if((KEY_DOWN_ACTION == msg->dwAddData2) || (KEY_REPEAT_ACTION == msg->dwAddData2))
    {
        this->AutoExitMenuCnt = 0;
        switch(msg->dwAddData1)
        {
        case GUI_MSG_KEY_RIGHT:
        case GUI_MSG_KEY_LONGRIGHT:
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LONGUP:
            DrawMusicPlayerContrlButtonByID(this, this->menu_id, UNFOCUSED);
            if(this->menu_id == SET_PLAY_FORWARD)
            {
                this->menu_id = SET_PLAY_MODE;
            }
            else
            {
                this->menu_id++;
            }
            DrawMusicPlayerContrlButtonByID(this, this->menu_id, FOCUSED);
            break;
        case GUI_MSG_KEY_LEFT:
        case GUI_MSG_KEY_LONGLEFT:
        case GUI_MSG_KEY_DOWN:
        case GUI_MSG_KEY_LONGDOWN:
            DrawMusicPlayerContrlButtonByID(this, this->menu_id, UNFOCUSED);
            if(this->menu_id == SET_PLAY_MODE)
            {
                this->menu_id = SET_PLAY_FORWARD;
            }
            else
            {
                this->menu_id--;
            }
            DrawMusicPlayerContrlButtonByID(this, this->menu_id, FOCUSED);
            break;
        case GUI_MSG_KEY_VDEC:
            break;
        //case GUI_MSG_KEY_ENTER:
        case GUI_MSG_KEY_MENU:
            last_key = GUI_MSG_KEY_MENU;
            break;
        case GUI_MSG_KEY_VADD:
            last_key = GUI_MSG_KEY_VADD;
            break;

        case GUI_MSG_KEY_ENTER:
            last_key = GUI_MSG_KEY_ENTER;
            if(KEY_DOWN_ACTION == msg->dwAddData2)
            {
                music_player_on_setting(this);
            }
            break;
        //case GUI_MSG_KEY_VADD:
        case GUI_MSG_KEY_ESCAPE:
            music_send_command(msg->h_deswin, 0, SWITCH_TO_MMENU, 0);
            break;
        case GUI_MSG_KEY_RISE:
            break;
        default:
            break;
        }
    }
    else if(KEY_UP_ACTION == msg->dwAddData2)
    {
        switch(msg->dwAddData1)
        {
        //case GUI_MSG_KEY_ENTER:
        case GUI_MSG_KEY_MENU:
        case GUI_MSG_KEY_LONGMENU:
            if(last_key == GUI_MSG_KEY_MENU)
            {
                if(this->menu_mode == MUSIC_SET_PLAYER_MODE)
                {
                    //DrawMusicPlayerContrlButton(this);
                    ClearMusicPlayerContrlButtonBG();
                    if(this->h_lyrics != NULL)
                    {
                        //if(this->RefreshLyricsTimerCnt++ > MUSIC_REFRESH_LYRICS_CNT)
                        {
                            this->RefreshLyricsTimerCnt = 0;
                            update_music_lyrics(this->h_lyrics);
                        }
                    }
                    else
                    {
                        SPCTRM_Clear(0);
                        SPCTRM_Update((H_WIN)(this->h_framewin));
                        //draw spectrum();//画频谱图
                    }
                    this->menu_mode = MUSIC_EXPLORER_MODE;
                }
            }
            break;
        case GUI_MSG_KEY_ENTER:
            if(last_key == GUI_MSG_KEY_ENTER)
            {
                DrawMusicPlayerContrlButtonByID(this, this->menu_id, FOCUSED);	//短按键抬起
            }
            break;
        }
        last_key = 0;
    }
    return EPDK_OK;
}

//按键消息处理函数
static __s32 music_player_win_key_proc(__gui_msg_t *msg)
{
    music_player_ctrl_t *this;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    switch(this->menu_mode)
    {
    case MUSIC_EXPLORER_MODE:
        music_explorer_key_proc(msg);			//浏览器on key process
        break;
    case MUSIC_SET_PLAYER_MODE:
        music_setting_player_key_proc(msg);		//setting player on key process
        break;
    default:
        break;
    }
    return EPDK_OK;
}

__s32 music_player_win_check_focus_on_timer(music_player_ctrl_t *this)
{
    __listbar_scene_t scene;
    if(this == NULL)
    {
        return EPDK_FAIL;
    }

    if(!this->visiable)
    {
        return EPDK_FAIL;
    }
    if(this->list_ctrl == NULL)
    {
        return EPDK_FAIL;
    }

    LISTBAR_GetScene(this->list_ctrl->listbar_handle, &scene);
    __msg("\n\nlistbar get scene focusid = %d, start id = %d\n", scene.focus_id, scene.start_id);
    if(this->list_ctrl->play_index != scene.focus_id)
    {
        music_list_set_focus_item(this->list_ctrl, this->list_ctrl->play_index);
        __msg("----------------------------------------set focus item to play index\n");
    }
    return EPDK_OK;
}


__s32 music_player_win_on_sys_timer(music_player_ctrl_t *this)
{
#if BEETLES_RTC_EXIST
    {
        //更新系统时间
        __time_t sys_time = {0, 0, 0};

        if(this == NULL)
        {
            return EPDK_FAIL;
        }
        if(this->visiable)
        {
            this->RefreshSystimeCnt++;
            __msg("\n1-----music on systime....tick = %d\n", this->RefreshSystimeCnt);
            if(this->RefreshSystimeCnt > MUSIC_REFRESH_SYSTIME_CNT)
            {
                this->RefreshSystimeCnt = 0;
                esTIME_GetTime(&sys_time);
                __msg("\n2------------------------------music display systime, minute = %d\n", sys_time.minute);
                DrawMusicSystemTime(&sys_time);
            }
        }
    }
#endif
    {
        if(!this->visiable)
        {
            return EPDK_FAIL;
        }
        this->CheckFocusItemCnt++;
        __msg("\n1--------------check focus item on timer, tick = %d\n", this->CheckFocusItemCnt);
        if(this->CheckFocusItemCnt > MUSIC_CHECK_FOCUS_TIMER_CNT)
        {
            this->CheckFocusItemCnt = 0;
            __msg("2------------check focus item on timer\n");
            music_player_win_check_focus_on_timer(this);
        }
    }
    return EPDK_OK;
}
/*
************************************************************************************************************************
*Function	:           static __s32 music_player_win_on_timer(msg)
*
*Description	:
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 music_player_win_on_timer(__gui_msg_t *msg)
{
    music_player_ctrl_t *this;
    __u32 total_time = 0, cur_time = 0;
    __cedar_status_t play_status;
    GUI_MEMDEV_Handle	draw_mem;

    this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);


    if(this->visiable)
    {
    }
    //当后退超过0时，会进入stop状态，此时应重新play

    if((this->setting_status == CEDAR_STAT_FF) || (this->setting_status == CEDAR_STAT_RR))
    {
        play_status = robin_get_fsm_status();
        if((play_status == CEDAR_STAT_FF)
                || (play_status == CEDAR_STAT_RR))
        {
            robin_set_cmd_play();
        }
        if(play_status == CEDAR_STAT_STOP)
        {
            total_time = robin_get_total_time();
            if(this->visiable)
            {
                DrawMusicProgressBar(total_time, total_time);
                //__msg("FF OR RR stopped\n");
                //__wait__;
            }
            if(this->setting_status == CEDAR_STAT_RR)
            {
                robin_set_cmd_play();
            }
            this->setting_status = CEDAR_STAT_PLAY;
            return EPDK_OK;
        }
#if 0
        {
            draw_mem = GUI_MEMDEV_Create(C_LAYER_CACHE_AREA_X, C_LAYER_CACHE_AREA_Y, C_LAYER_CACHE_AREA_W, C_LAYER_CACHE_AREA_H);
            GUI_MEMDEV_Select(draw_mem);
            //__msg("apply for memory %d\n", draw_mem);
            //__wait__;
            this->RefreshPlayTimeCnt = 0;
            total_time = robin_get_total_time();
            cur_time = robin_get_cur_time();
            if(cur_time > total_time)
            {
                cur_time = total_time;
            }
            //GUI_SetColor(GUI_WHITE);
            //GUI_LyrWinCacheOff();
            DrawMusicPlayingTime(cur_time);
            DrawMusicTotalTime(total_time);
            DrawMusicProgressBar(total_time, cur_time);
            //GUI_LyrWinCacheOn();

            GUI_MEMDEV_CopyToLCD(draw_mem);
            GUI_MEMDEV_Select( NULL );
            GUI_MEMDEV_Delete(draw_mem);
            draw_mem = NULL;
        }
#endif
        this->setting_status = CEDAR_STAT_PLAY;
        return EPDK_OK;
    }
    else
    {
        music_player_check_play_status_on_timer(this);	//check status for play mode
    }

    if(this->visiable)
    {
        __msg("--------------------music player on timer\n");
        if(this->list_ctrl != NULL)
        {
            if(this->list_ctrl->total == 0)
            {
                return EPDK_FAIL;
            }
        }
        else
        {
            return EPDK_FAIL;
        }
        GUI_LyrWinSel(this->music_layer);
        if(this->RefreshPlayTimeCnt++ >= MUSIC_REFRESH_PLAYTIME_CNT)
        {
#if BEETLES_RTC_EXIST
            {
                //__time_t sys_time;
                //esTIME_GetTime(&sys_time);
                //DrawMusicSystemTime(&sys_time);
            }
#endif
            draw_mem = GUI_MEMDEV_Create(C_LAYER_CACHE_AREA_X, C_LAYER_CACHE_AREA_Y, C_LAYER_CACHE_AREA_W, C_LAYER_CACHE_AREA_H);
            GUI_MEMDEV_Select(draw_mem);
            //__msg("apply for memory %d\n", draw_mem);
            //__wait__;
            this->RefreshPlayTimeCnt = 0;
            total_time = robin_get_total_time();
            cur_time = robin_get_cur_time();
            if(cur_time > total_time)
            {
                cur_time = total_time;
            }
            //GUI_SetColor(GUI_WHITE);
            //GUI_LyrWinCacheOff();
            DrawMusicPlayingTime(cur_time);
            DrawMusicTotalTime(total_time);
            DrawMusicProgressBar(total_time, cur_time);
            //DrawMusicProgressBarPoint(total_time, cur_time);
            //GUI_LyrWinCacheOn();

            GUI_MEMDEV_CopyToLCD(draw_mem);
            GUI_MEMDEV_Select( NULL );
            GUI_MEMDEV_Delete(draw_mem);
            draw_mem = NULL;
        }

        if(this->menu_mode == MUSIC_EXPLORER_MODE)
        {
            if(this->h_lyrics != NULL)
            {
                if(this->RefreshLyricsTimerCnt++ > MUSIC_REFRESH_LYRICS_CNT)
                {
                    this->RefreshLyricsTimerCnt = 0;
                    update_music_lyrics(this->h_lyrics);
                }
            }
            else
            {
                if(robin_get_fsm_status() == CEDAR_STAT_PLAY)
                {
                    SPCTRM_Update((H_WIN)(this->h_framewin));
                }
            }
        }
        else
        {
            this->AutoExitMenuCnt++;
            if(this->AutoExitMenuCnt > C_AUTO_EXIT_MENU_TIME)
            {
                this->menu_mode = MUSIC_EXPLORER_MODE;
                this->AutoExitMenuCnt = 0;
                //__wait__;
                ClearMusicPlayerContrlButtonBG();
                if(this->h_lyrics != NULL)
                {
                    //if(this->RefreshLyricsTimerCnt++ > MUSIC_REFRESH_LYRICS_CNT)
                    {
                        this->RefreshLyricsTimerCnt = 0;
                        update_music_lyrics(this->h_lyrics);
                    }

                }
                else
                {
                    SPCTRM_Clear(0);		//显示最大值
                    SPCTRM_Update((H_WIN)(this->h_framewin));
                }
            }
        }
    }
    //MusicSpectrumUpdate(this);				//
    //GUI_LyrWinFlushFB(player_ctrl->music_layer);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:           static __s32 music_player_win_on_close(__gui_msg_t *msg)
*
*Description	:				OnClose处理函数，释放FrameWin
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 music_player_win_on_close(__gui_msg_t *msg)
{
    return GUI_FrmWinDelete(msg->h_deswin);			//send destroy message
}

/*
************************************************************************************************************************
*Function	:           static __s32  music_player_win_on_destroy(__gui_msg_t *msg)
*
*Description	:				OnDestroy处理函数，释放framewin 所有申请的资源
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32  music_player_win_on_destroy(__gui_msg_t *msg)
{
    music_player_ctrl_t *this;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if(this == NULL)
    {
        return EPDK_FAIL;
    }
    if(GUI_IsTimerInstalled(this->h_framewin, MusicSysTimerId))
    {
        GUI_KillTimer(this->h_framewin, MusicSysTimerId);
        __msg("--------------------------system time timer has been killed\n");
    }

    if(GUI_IsTimerInstalled(this->h_framewin, MusicCheckFocusTimerId))
    {
        GUI_KillTimer(this->h_framewin, MusicCheckFocusTimerId);
        __msg("--------------------------check focus item timer has been killed\n");
    }

    //app_print_memory_usage("music player on destroy", NULL);

    delete_music_lyrics_scene(this);
    music_player_save_last_para(this);

    delete_music_list(this->list_ctrl);


    //MusicSpectrumSceneDelete(this);

    music_player_device_uninit(this);

    esMEMS_Bfree(this, sizeof(music_player_ctrl_t));

    music_free_icon_res();

    //app_print_memory_usage("music player on destroy finished", NULL);

    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function	:          static __s32 music_player_scene_create(H_WIN  music_frame_win)
*
*Description	: 		music player 场景创建
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 music_player_scene_create(H_WIN  music_frame_win)
{
    music_player_ctrl_t *this;
    music_list_para_t list_para;
    __bool ret;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(music_frame_win);
    this->h_framewin = music_frame_win;										//保存窗口句柄
    __msg("music player is initializing!\n");
    //__wait__;

    list_para.framewin = this->h_framewin;
    list_para.layer = this->music_layer;
    list_para.visiable = this->visiable;
    list_para.root_type = this->root_type;
    list_para.media_type = this->media_type;
    list_para.font = this->font;
    list_para.explorer_play_music = this->explorer_play_music;
    list_para.music_file_name = this->music_file_name;
    //app_print_memory_usage("new music list", NULL);

    //music_player_device_init(this);			//move to here by libaiao,2011.8.21
    this->list_ctrl = new_music_list(&list_para);

    music_player_get_last_para(this);

    if(this->list_ctrl != NULL)
    {
        char *filename = this->play_file_info.filename;
        //get_music_list_focus_item_full_filename(this->list_ctrl, filename);
        MusicGetListItemFileFullPath(this->list_ctrl->rat_handle, this->list_ctrl->play_index, filename);
        rat_set_file_for_play(this->list_ctrl->rat_handle, filename);
    }
    else
    {
        __wrn("new music file list failed!\n");
    }
    //this->play_mode = RAT_PLAY_MODE_SEQUENCE;
    //this->EQ_mode = MUSIC_EQ_NORMAL;

    music_player_device_init(this);

    //app_print_memory_usage("music player win on create message end", NULL);

    /*
    	{
    		char *filename = player_ctrl->play_file_info.filename;
    		get_music_list_focus_item_full_filename(player_ctrl->list_ctrl, filename);
    		music_play_file(player_ctrl, filename);
    	}
    */

    if(this->visiable)
    {
        ret = GUI_SetTimer(music_frame_win, MusicSysTimerId, MUSIC_SYSTIMER_INTERVAL, NULL);						//系统时间刷新定时器
        if(!ret)
        {
            __wrn("creat timer for system time failed\n");
        }
        else
        {
            __msg("creat timer for system time successfully\n");
        }
        /*
        ret = GUI_SetTimer(music_frame_win, MusicCheckFocusTimerId, MUSIC_CHECK_FOCUS_TIMER_INTERVAL, NULL);		//检测是否foucus item 与当前播放index一致
        if(!ret)
        {
        	__wrn("creat timer for check focus item failed\n");
        }
        else
        {
        	__msg("creat timer for check focus item successfully\n");
        }
        */
    }

    return EPDK_OK;
}


/*
************************************************************************************************************************
*Function	:          static __s32 music_player_win_on_paint(H_WIN  music_frame_win)
*
*Description	: 		on paint message
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/

static __s32 music_player_win_on_paint(H_WIN  music_frame_win)
{
    __s32 ret = 0;
    music_player_ctrl_t *player_ctrl;
    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(music_frame_win);
    if (GUI_LyrWinGetSta(player_ctrl->music_layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    __msg("music player on paint message\n");

    //__wait__;

    if(player_ctrl->visiable)
    {
        GUI_LyrWinSetTop(player_ctrl->music_layer);
        MusicSetTransColor();

        //if(player_ctrl->list_ctrl != NULL)
        {
            show_music_list(player_ctrl->list_ctrl);
        }
        DrawMusicHeadbar(player_ctrl);
    }
    __msg("test volume bar\n");
    //__wait__;

    //app_print_memory_usage("music player on paint message, & ready to play mp3 file", NULL);
    //__here__;
    music_list_get_item_scene(player_ctrl->list_ctrl, &(player_ctrl->list_ctrl->listbar_play_scene));
    //DrawMusicPlayer(player_ctrl);
    {
        char *filename = player_ctrl->play_file_info.filename;
        if(player_ctrl->list_ctrl != NULL)
        {
            MusicGetListItemFileFullPath(player_ctrl->list_ctrl->rat_handle, player_ctrl->list_ctrl->play_index, filename);
        }
        //if(robin_get_fsm_status() != CEDAR_STAT_PLAY)
        {
#ifdef C_MUSIC_DEBUG
#else
            {
                __bool bplay;
                __s32 ret;
                char file[RAT_MAX_FULL_PATH_LEN] = {0};

                bplay = 1;
                ret = robin_get_fsm_cur_file(file, sizeof(file));
                if(EPDK_OK == ret)
                {
                    if(0 == eLIBs_strcmp(file, filename))
                    {
                        if(CEDAR_STAT_PLAY == robin_get_fsm_status())
                        {
                            bplay = 0;
                        }
                    }
                }

                if(bplay)
                {
                    ret = music_play_file(player_ctrl, filename);
                }
            }
#endif
        }
#ifdef C_MUSIC_DEBUG
#else
        GUI_SetTimer(music_frame_win, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);//play之后才可以打开timer
#endif
    }
    //app_print_memory_usage("player is playing", NULL);
    return EPDK_OK;
}

//extern __s32 music_list_long_string_stop_roll(music_list_ctrl_t *this);
static __s32 music_player_scene_delete(H_WIN  music_frame_win)
{
    __s32 ret = 0;
    music_player_ctrl_t *this;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(music_frame_win);

    if(!this->visiable)
    {
        return EPDK_FAIL;
    }
    this->visiable = EPDK_FALSE;
    /*
    if (GUI_LyrWinGetSta(this->music_layer) == GUI_LYRWIN_STA_ON)
    {
    	GUI_LyrWinSetSta(this->music_layer, GUI_LYRWIN_STA_OFF);
    }
    else
    {
    	//return EPDK_OK;
    }
    */

    if(GUI_IsTimerInstalled(music_frame_win, MusicSysTimerId))
    {
        GUI_KillTimer(music_frame_win, MusicSysTimerId);
        __msg("--------------------------system time timer has been killed\n");
    }

    if(GUI_IsTimerInstalled(music_frame_win, MusicCheckFocusTimerId))
    {
        GUI_KillTimer(music_frame_win, MusicCheckFocusTimerId);
        __msg("--------------------------check focus item timer has been killed\n");
    }

    delete_music_lyrics_scene(this);
    music_player_save_last_para(this);
    //clear_music_list_area();

    //delete_music_list(this->list_ctrl);
    //this->list_ctrl = NULL;
    music_list_long_string_stop(this->list_ctrl);
    //music_list_long_string_stop_roll(this->list_ctrl);

    //esMEMS_Bfree(this, sizeof(music_player_ctrl_t));

    music_free_icon_res();
    return EPDK_OK;
}

__s32  musicplayer_rat_get_search_path(__s32 root_type, char *search_path)
{
    __s32 ret = 0;
    char s_root_type[50];
    char disk_name[RAT_MAX_PARTITION][4];

    eLIBs_memset(s_root_type, 0, sizeof(s_root_type));
    eLIBs_memset(disk_name, 0, sizeof(disk_name));
    //eLIBs_memset(list_ctrl->search_path, 0, sizeof(list_ctrl->search_path));

    __msg("root_type=%d\n", root_type);
    switch(root_type)
    {
    case RAT_LOCAL:
        eLIBs_strcpy(s_root_type, RAT_LOCAL_DISK);
        break;
    case RAT_TF:
        eLIBs_strcpy(s_root_type, RAT_SD_CARD);
        break;
    case RAT_USB:
        eLIBs_strcpy(s_root_type, RAT_USB_DISK);
        break;
    default:
        eLIBs_strcpy(s_root_type, RAT_SD_CARD);
        break;
    }
    ret = rat_get_partition_name(s_root_type, disk_name);
    eLIBs_strcpy(search_path, disk_name[0]);

    __msg("================================disk_name[0]=%s\n", disk_name[0]);

    return ret;
}

__s32 music_player_restore(H_WIN  music_frame_win, __s32 root_type)
{
    music_player_ctrl_t *this;
    music_list_para_t list_para;
    __s32 ret = 0;
    GUI_MEMDEV_Handle	draw_mem;

    if(music_frame_win == NULL)
    {
        return EPDK_FAIL;
    }

    //__wait__;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(music_frame_win);

    //GUI_LyrWinSetSta(this->music_layer, GUI_LYRWIN_STA_ON);

    if(robin_get_fsm_status() == CEDAR_STAT_STOP)
    {
        //return EPDK_FAIL;
    }
    this->visiable = EPDK_TRUE;

    if (GUI_LyrWinGetSta(this->music_layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        __here__;
        return EPDK_FAIL;
    }

    ret = GUI_LyrWinSetTop(this->music_layer);
    if(ret != EPDK_OK)
    {
        __msg("Set music layer as top failed\n");
        __wait__;
        return EPDK_FAIL;
    }
    else
    {
        __msg("--------------music player set layer status on, ret = %d\n", ret);
    }
    GUI_LyrWinSel(this->music_layer);
    GUI_SetBkColor(0x0);
    GUI_ClearRect(0, 0, 400, 50);
    //clear_music_list_area();
#if 0
    draw_mem = GUI_MEMDEV_Create(0, 0, 400, 240);
    GUI_MEMDEV_Select(draw_mem);
#endif

    this->visiable = EPDK_TRUE;
    if(this->list_ctrl != NULL)
    {
        this->list_ctrl->visiable = EPDK_TRUE;
    }



    music_player_get_last_para(this);

    if(this->list_ctrl != NULL)				//先删除列表，防止闪烁
    {
        delete_music_list(this->list_ctrl);
        this->list_ctrl = NULL;
    }

    {
        char filename[RAT_MAX_FULL_PATH_LEN];
        char search_path[RAT_MAX_FULL_PATH_LEN];
        int index = 0;
        eLIBs_memset(filename, 0, sizeof(filename));
        musicplayer_rat_get_search_path(root_type, search_path);
        if(robin_get_fsm_cur_file(filename, sizeof(filename)) != -1)
        {
            __msg("==========================robin get cur play file = %s!\n", filename);
            if(this->list_ctrl->rat_handle != 0)
            {
                this->list_ctrl->rat_handle = rat_open(search_path, RAT_MEDIA_TYPE_AUDIO, 0);
                __msg("==========================this->list_ctrl->rat_handle = %d!\n", this->list_ctrl->rat_handle);
                if(rat_set_file_for_play(this->list_ctrl->rat_handle, filename) == -1)
                {
                    return EPDK_FAIL;
                }
                index = robin_npl_get_cur();
                __msg("=================robin npl get cur index = %d!\n", index);
                __here__;
            }
            else
            {
                return EPDK_FAIL;
            }
        }
        else
        {
            __wrn("======================robin get cur play file failed!\n");
            return EPDK_FAIL;
        }
    }

    {
        music_list_para_t list_para;
        __msg("music list is initializing!\n");

        this->root_type = root_type;
        list_para.framewin = this->h_framewin;
        list_para.layer = this->music_layer;
        list_para.visiable = this->visiable;
        list_para.root_type = this->root_type;
        list_para.media_type = this->media_type;
        list_para.font = this->font;

        //赋值，用于创建list时检测是否从文件管理恢复后台播放
        list_para.explorer_play_music = this->explorer_play_music;
        list_para.music_file_name = this->music_file_name;
        __msg("list_para.explorer_play_music = %d\n", list_para.explorer_play_music);
        __msg("new_music_list(&list_para);\n");
        this->list_ctrl = new_music_list(&list_para);
        if(this->list_ctrl != NULL)
        {
            show_music_list(this->list_ctrl);
        }
        else
        {
#if 0
            //GUI_MEMDEV_CopyToLCD(draw_mem);
            GUI_MEMDEV_Select( NULL );
            GUI_MEMDEV_Delete(draw_mem);
            draw_mem = NULL;
#endif
            return EPDK_FAIL;
        }
    }

    //__wait__;
    MusicSetTransColor();
    {
        //__u32 cur_time = 0;
        //cur_time = robin_get_cur_time();
    }

    DrawMusicHeadbar(this);
    {
        //__u32 cur_time = 0;
        //cur_time = robin_get_cur_time();
    }
#if 0
    if(robin_get_fsm_status() == CEDAR_STAT_STOP)		//音乐已停止，则重新播放
    {
        {
            char *filename = this->play_file_info.filename;
            if(this->list_ctrl != NULL)
            {
                MusicGetListItemFileFullPath(this->list_ctrl->rat_handle, this->list_ctrl->play_index, filename);
            }
            {
                ret = music_play_file(this, filename);
            }
        }
    }
#endif

    music_restore_play_file_info(this);

#if 0
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select( NULL );
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
#endif
    {
        __bool timer_ret;
        if(!GUI_IsTimerInstalled(this->h_framewin, MusicSysTimerId))
        {
            timer_ret = GUI_SetTimer(this->h_framewin, MusicSysTimerId, MUSIC_SYSTIMER_INTERVAL, NULL);						//系统时间刷新定时器
            if(!timer_ret)
            {
                __wrn("creat timer for system time failed\n");
            }
            else
            {
                __msg("--------------creat timer for system time successfully\n");
            }
        }
        /*
        if(!GUI_IsTimerInstalled(this->h_framewin, MusicCheckFocusTimerId))
        {
        	timer_ret = GUI_SetTimer(this->h_framewin, MusicCheckFocusTimerId, MUSIC_CHECK_FOCUS_TIMER_INTERVAL, NULL);		//检测是否foucus item 与当前播放index一致
        	if(!timer_ret)
        	{
        		__wrn("creat timer for check focus item failed\n");
        	}
        	else
        	{
        		__msg("----------------creat timer for check focus item successfully\n");
        	}
        }
        */
    }
    return EPDK_OK;
}



//获得媒体文件信息
static __s32 music_get_audio_file_info(music_player_ctrl_t *this, char *filename)
{

    robin_audio_info_t audio_file_info;
    eLIBs_memset(&audio_file_info, 0, sizeof(robin_audio_info_t));
    robin_get_audio_info(filename, &audio_file_info);

    eLIBs_strcpy(this->play_file_info.artist, audio_file_info.artist);
    eLIBs_strcpy(this->play_file_info.album, audio_file_info.album);
    this->play_file_info.bitrate = audio_file_info.bps;
    this->play_file_info.filesize = eLIBs_GetFileSize(filename);
    this->play_file_info.artist_charset = audio_file_info.artist_charset ;
    this->play_file_info.album_charset = audio_file_info.album_charset ;
    return EPDK_OK;
}


static __s32 music_play_file(music_player_ctrl_t *this, char *filename)
{
    __u32 total_time = 0, cur_time = 0;
    __s32 ret = 0;
    //__wait__;
    //char *filename = player_ctrl->play_file_info.filename;
    if(this  == NULL)
    {
        return EPDK_FAIL;
    }

    //app_print_memory_usage("check memory in function of music_play_file", NULL);

    if((eLIBs_strlen(filename) == 0) || (this->list_ctrl->total == 0))
    {
        if(this->visiable)
        {
            DrawMusicPlayingTime(0);

            CleanMusicTotalTime();
            DrawMusicTotalTime(0);

            DrawMusicProgressBar(0, 0);					//显示进度条
            eLIBs_memset(&(this->play_file_info), 0, sizeof(audio_file_info_t));
            DrawMusicPlayerFileInfo(&(this->play_file_info));					//显示文件信息
            DrawMusicPlayStatusButton(MUSIC_PLAYING);

            if(this->menu_mode == MUSIC_EXPLORER_MODE)
            {
                SPCTRM_Clear((__u32)0x000f);		//显示最大值
            }
        }
        return EPDK_FAIL;
    }

    this->CheckFocusItemCnt = 0;		//有按键清除检测焦点条目定时计数器
    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    //GUI_ResetTimer(this->h_framewin, MusicCheckFocusTimerId, MUSIC_CHECK_FOCUS_TIMER_INTERVAL, NULL);

    music_list_get_item_scene(this->list_ctrl, &(this->list_ctrl->listbar_play_scene));//获得当前listbar场景
    robin_npl_set_cur(this->list_ctrl->play_index);
    robin_set_cmd_stop();
    //app_print_memory_usage("robin start to play file", NULL);
    ret = robin_set_cmd_play_file(filename, NULL);
    __msg("===================music  filename is %s, return value is %d===================\n", filename, ret);
    ret = robin_set_cmd_play();
    __msg("===================robin start to play ,  return value is %d===================\n", ret);
    //app_print_memory_usage("robin start to play file finished", NULL);
    if(this->visiable)
    {
        music_get_audio_file_info(this, filename);
        total_time = robin_get_total_time();
        cur_time = robin_get_cur_time();
        __msg("======================================current time = %d, total time = %d\n", cur_time, total_time);
        if(cur_time > total_time)
        {
            cur_time = total_time;
        }
        //	robin_set_cmd_stop();
        //	robin_set_cmd_play_file(filename, NULL);
        //	robin_set_cmd_play();

        //GUI_LyrWinCacheOff();
        DrawMusicPlayingTime(cur_time);

        CleanMusicTotalTime();
        DrawMusicTotalTime(total_time);

        DrawMusicProgressBar(0, 0);					//显示进度条

        DrawMusicPlayerFileInfo(&(this->play_file_info));					//显示文件信息
        DrawMusicPlayStatusButton(MUSIC_PLAYING);

        //GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    }
    if(this->visiable)
    {
        delete_music_lyrics_scene(this);

        if(this->menu_mode == MUSIC_EXPLORER_MODE)
        {
            ClearMusicPlayerContrlButtonBG();
            if(music_lyrics_exist())		//检查是否有歌词
            {
                //有歌词则不显示频谱
                if(create_music_lyrics_scene(this) != EPDK_FAIL)
                {
                    update_music_lyrics(this->h_lyrics);
                }
            }
            else
            {
                SPCTRM_Clear(0);		//显示最大值
                SPCTRM_Update((H_WIN)(this->h_framewin));
            }
        }
        else if(this->menu_mode == MUSIC_SET_PLAYER_MODE)
        {
            if(music_lyrics_exist())		//检查是否有歌词
            {
                //有歌词则不显示频谱
                if(create_music_lyrics_scene(this) != EPDK_FAIL)		//只创建，不显示
                {
                    //update_music_lyrics(this->h_lyrics);
                    __msg("create lyrics successed!\n");
                }
            }
        }
    }
    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    //app_print_memory_usage("check memory in function of music_play_file finished", NULL);
    //GUI_LyrWinCacheOn();
    return EPDK_OK;
    //robin_set_cmd_play();
}


__s32 music_restore_play_file_info(music_player_ctrl_t *this)
{
    __u32 total_time = 0, cur_time = 0;
    char *filename;
    if(this == NULL)
    {
        return EPDK_FAIL;
    }
    filename = this->play_file_info.filename;
    cur_time = robin_get_cur_time();
    MusicGetListItemFileFullPath(this->list_ctrl->rat_handle, this->list_ctrl->play_index, filename);

    cur_time = robin_get_cur_time();

    music_get_audio_file_info(this, filename);
    total_time = robin_get_total_time();
    cur_time = robin_get_cur_time();
    if(cur_time > total_time)
    {
        cur_time = total_time;
    }

    DrawMusicPlayingTime(cur_time);

    CleanMusicTotalTime();
    DrawMusicTotalTime(total_time);

    DrawMusicProgressBar(total_time, cur_time);					//显示进度条

    DrawMusicPlayerFileInfo(&(this->play_file_info));					//显示文件信息
    DrawMusicPlayStatusButton(MUSIC_PLAYING);

    //GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);


    delete_music_lyrics_scene(this);


    if(this->menu_mode == MUSIC_EXPLORER_MODE)
    {
        ClearMusicPlayerContrlButtonBG();
        if(music_lyrics_exist())		//检查是否有歌词
        {
            //有歌词则不显示频谱
            if(create_music_lyrics_scene(this) != EPDK_FAIL)
            {
                update_music_lyrics(this->h_lyrics);
                update_music_lyrics(this->h_lyrics);
            }
        }
        else
        {
            SPCTRM_Clear(0);		//显示最大值
            SPCTRM_Update((H_WIN)(this->h_framewin));
        }
    }
    else if(this->menu_mode == MUSIC_SET_PLAYER_MODE)
    {
        if(music_lyrics_exist())		//检查是否有歌词
        {
            //有歌词则不显示频谱
            if(create_music_lyrics_scene(this) != EPDK_FAIL)		//只创建，不显示
            {
                //update_music_lyrics(this->h_lyrics);
                __msg("create lyrics successed!\n");
            }
        }
    }
    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    return EPDK_OK;
}

//初始化音频设备
static __s32 music_player_device_init(music_player_ctrl_t *this)
{
    //__bool ret;
    __inf("_init begining!\n");

    //app_print_memory_usage("robin create", NULL);

    /*open display*/
    //ADE_Open();
    robin_open(ROBIN_MODE_AUDIO_MIN, NULL);
    robin_enable_spectrum();
    robin_set_eq_mode(this->EQ_mode);
    //music_player_set_robin_EQ_mode(this);
    robin_set_ff_rr_speed(MUSIC_FF_SPEED);
    //this->play_mode = RAT_PLAY_MODE_SEQUENCE;
    robin_set_play_mode(this->play_mode);
    //GUI_SetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    //app_print_memory_usage("robin create finished", NULL);
    return EPDK_OK;
}

//卸载音频设备
static __s32 music_player_device_uninit(music_player_ctrl_t *player_ctrl)
{
    //app_print_memory_usage("delete robin ", NULL);
    if(GUI_IsTimerInstalled(player_ctrl->h_framewin, MusicTimerId))
    {
        GUI_KillTimer(player_ctrl->h_framewin, MusicTimerId);
    }
    robin_set_cmd_stop();
    //GUI_KillTimer(player_ctrl->h_framewin, MusicSpectrumTimerId);
    /*close robin*/
    robin_close();
    /*close de*/
    //ADE_Close();
    //app_print_memory_usage("delete robin finished", NULL);
    return EPDK_OK;
}

//


//发送一个命令参数给主界面
__s32 music_send_command(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    char buf[128];
    H_WIN hDstWin;
    H_WIN hManagerWin;

    //robin_set_cmd_stop();

    hManagerWin = GUI_WinGetParent(hwin);
    hDstWin = GUI_WinGetParent(hManagerWin);

    msg.h_deswin = hDstWin;

    msg.h_srcwin = hwin;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hManagerWin), id);//
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;

    GUI_WinGetName(msg.h_deswin, buf);

    __msg("%s\n", buf);
    GUI_SendNotifyMessage(&msg);
    //GUI_SendMessage(&msg);
    return EPDK_OK;
}

__s32 create_music_lyrics_scene(music_player_ctrl_t *This)
{
    music_lyrics_para_t para;
    para.layer = This->music_layer;

    if(This->h_lyrics != NULL)
    {
        delete_music_lyrics_scene(This);
    }
    //app_print_memory_usage("new music lyrics", NULL);

    para.area.x = 210;
    para.area.y = 40;	//35;
    para.area.width = 399 - 210;
    para.area.height = 125 - 40;	//125 - 35; 	//显示4行歌词
    para.font = This->font;
    para.font_color = GUI_WHITE;
    para.hight_light_color = GUI_LIGHTRED;	//BEETLES_GRAY_COLOR;

    This->h_lyrics = new_music_lyrics(&para);
    if(This->h_lyrics == NULL)
    {
        return EPDK_FAIL;
    }

    //app_print_memory_usage("new music lyrics finished", NULL);
    return EPDK_OK;
}

__s32 delete_music_lyrics_scene(music_player_ctrl_t *This)
{
    //app_print_memory_usage("delete music lyrics", NULL);
    if(This->h_lyrics != NULL)
    {
        delete_music_lyrics(This->h_lyrics);
        This->h_lyrics = NULL;
    }
    //app_print_memory_usage("delete music lyrics finished", NULL);
    return EPDK_OK;
}

__s32 delete_music_player_volume_scene(H_WIN hfrm)
{
    music_player_ctrl_t *player_ctrl = NULL;

    if(!hfrm)
    {
        return EPDK_FAIL;
    }

    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(hfrm);
    if(player_ctrl && player_ctrl->hVolumeBar)
    {
        VolumeBarDelete(player_ctrl->hVolumeBar);
        player_ctrl->hVolumeBar = NULL;
    }

    return EPDK_TRUE;
}

