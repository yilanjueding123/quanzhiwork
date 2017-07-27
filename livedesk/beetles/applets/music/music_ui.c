
//libaiao

#include "beetles_app.h"
#include "music_ui.h"

static music_ui_t  music_ui_400_240 =
{
    //layer window
    {
        PIXEL_COLOR_ARGB8888,       //fmt
        { 400, 240 },               //fb size
        { 0, 0, 400, 240 },         //src rect
        { 0, 0, 400, 240 },         //scn rect
        { 0 },                      //reserved
    },

    //list window, 浏览文件
    {
        {0, 7, 188, 216},						//listbar area
        {0, 0, 188, 36},						//item rectangle
        {5, 9, 181, 22},						//focus icon(select border)
        {11, 12, 15, 15 },						//file icon
        {11 + 15 + 10, 0, 188 - 11 - 15 - 10 - 3 - 1, 36},		//text rectangle
    },

    //scroll bar ui
    {
        {189,	7,	7,	224},					//scroll bar background
        {191,	22,	3,	194},					//scroll
        6,										//head
        1,										//body
        6,										//tail
    },

    //head bar ui
    {
        {210, 40, 400 - 210, 150 - 40},					//headbar background
#if BEETLES_RTC_EXIST
        {198, 5, 26, 16},								//播放模式状态图标
        {226, 5, 50, 16},								//EQ模式状态图标
        {226, 5, 26, 16},								//背光关闭时间图标
        {278, 5, 26, 16},								//音量图标
        {304, 3, 18, 16},								//音量text
        {326, 6, 31, 12},								//电池图标
        {359, 3, 40, 16},								//时间显示区域
#else
        {198, 5, 26, 16},								//播放模式状态图标
        {229, 5, 50, 16},								//EQ模式状态图标
        {284, 5, 26, 16},								//背光关闭时间图标
        {315, 5, 26, 16},								//音量图标
        {341, 4, 18, 16},								//音量text
        {364, 6, 31, 12},								//电池图标
        {0, 0, 0, 16},									//时间显示区域
#endif
    },
    //palyer ui
    {
        {210, 40, 400 - 210, 120 - 40},					//控制按钮背景区域()与频谱共用
        {230, 120, 150, 16},							//按键提示字符区域
        {254, 68, 26, 16},								//播放模式按钮
        {288, 68, 26, 16},								//EQ模式按钮
        {324, 68, 26, 16},								// 背光时间控制按钮

        {228, 100, 26, 16},							//上一首按钮
        {261, 100, 26, 16},							//下一首按钮
        {294, 100, 26, 16},							//播放暂停按钮
        {327, 100, 26, 16},							//快退按钮
        {360, 100, 26, 16},							//快进按钮

        {214, 154, 26, 20},							//播放状态图标(暂停或播放)

        {239 + 3, 160 - 21, 70, 20},	              		//当前播放时间
        {239 + 146 - 70 - 3 + 28 , 160 - 21, 60, 20},			//总时间显示区域

        {239, 160, 5, 4},								//进度条head
        {239, 160, 146, 4},							//进度条body
        {239 + 5, 160, 5, 4},							//进度条point,从head之后，因此加5
        {239 + 146 - 5, 160, 5, 4},						//进度条tail

        {213,		183, 26, 16},						//演唱者图标
        {213 + 26,	183, 70, 16},						//演唱者名称
        {310,		183, 26, 16},						//文件大小图标
        {310 + 26,	183, 70, 16},						//文件大小显示

        {213,		213, 26, 16},						//专辑名称图标
        {213 + 26,	213, 70, 16},						//专辑名称
        {310,		213, 26, 16},						//bitrate图标
        {310 + 26,	213, 70, 16},						//bitrate大小显示
    },

    //volume bar ui
    {
        {0, 0, 0, 0},									//整个音量控制显示区域
        {0, 0, 0, 0},									//左边speaker图标显示，有两种状态
        {0, 0, 0, 0},									//中间音量显示进度条
        {0, 0, 0, 0},    								//右边speaker图标显示
    },

    //spectrum
    {
        {210, 40, 400 - 210, 150 - 40}
    },
    //icon table
    {
        //ID															res	handle,    	res buffer
        //音乐浏览器
        { ID_MUSIC_PALETTE_BMP,           			NULL,  				 NULL },    //
        { NULL,           	NULL,  				 NULL },    //
        { ID_MUSIC_FILE_UNFOCUSED_BMP,      		NULL,  				 NULL }, 	//
        { ID_MUSIC_FILE_UNFOCUSED_BMP,      		NULL,  				 NULL }, 	//文件图标，未被选中时
        { ID_MUSIC_FILE_FOCUSED_BMP,      			NULL,  				 NULL }, 	//文件图标，被选中时
        { ID_MUSIC_LIST_ITEM_FOCUS_BMP,    			NULL,  				 NULL }, 	//选择框
        { ID_MUSIC_LONG_STR_SCROLL_BMP,				NULL,  			 	 NULL },     //长文件名滚动背景条
        //header bar
        //play mode
        { ID_MUSIC_PLAY_ALL_BMP,    				NULL,  				 NULL }, 	//	播放全部,不重复
        { ID_MUSIC_PLAY_ONE_BMP,    				NULL,  				 NULL }, 	//播放一首
        { ID_MUSIC_PLAY_RANDOM_BMP,    				NULL,  				 NULL }, 	//随即播放
        { ID_MUSIC_REPEAT_ALL_BMP,    				NULL,  				 NULL }, 	//重复所有
        { ID_MUSIC_REPEAT_ONE_BMP,    				NULL,  				 NULL }, 	//重复一首

        //EQ mode
        { ID_MUSIC_EQ_NORMAL_BMP,    				NULL,  				 NULL }, 	//EQ normal
        { ID_MUSIC_EQ_CLASSIC_BMP,    				NULL,  				 NULL }, 	//EQ classic
        { ID_MUSIC_EQ_ROCK_BMP,    					NULL,  				 NULL }, 	//	EQ Rock
        { ID_MUSIC_EQ_JAZZ_BMP,    					NULL,  				 NULL }, 	//	EQ JAZZ
        { ID_MUSIC_EQ_POP_BMP,    					NULL,  				 NULL }, 	//	EQ POP

        //BL Time
        { ID_MUSIC_BL_ON_BMP,    					NULL,  				 NULL }, 	//	Backlight never off
        { ID_MUSIC_BL_10S_BMP,    					NULL,  				 NULL }, 	//last 10 second
        { ID_MUSIC_BL_30S_BMP,    					NULL,  				 NULL }, 	//
        { ID_MUSIC_BL_60S_BMP,    					NULL,  				 NULL }, 	//
        { ID_MUSIC_BL_90S_BMP,    					NULL,  				 NULL }, 	//

        //volume
        { ID_MUSIC_VOLUME_STATUS_BMP,    			NULL,  				 NULL }, 	//

        //bettery
        { ID_MUSIC_BETTERY0_BMP,    				NULL,  				 NULL }, 	//
        { ID_MUSIC_BETTERY1_BMP,    				NULL,  				 NULL }, 	//
        { ID_MUSIC_BETTERY2_BMP,    				NULL,  				 NULL }, 	//
        { ID_MUSIC_BETTERY3_BMP,    				NULL,  				 NULL }, 	//
        { ID_MUSIC_BETTERY4_BMP,    				NULL,  				 NULL }, 	//
        { ID_MUSIC_BETTERY5_BMP,    				NULL,  				 NULL }, 	//

        { ID_MUSIC_CHARGE_BETTERY0_BMP,    				NULL,  				 NULL }, 	//
        { ID_MUSIC_CHARGE_BETTERY1_BMP,    				NULL,  				 NULL }, 	//
        { ID_MUSIC_CHARGE_BETTERY2_BMP,    				NULL,  				 NULL }, 	//
        { ID_MUSIC_CHARGE_BETTERY3_BMP,    				NULL,  				 NULL }, 	//
        { ID_MUSIC_CHARGE_BETTERY4_BMP,    				NULL,  				 NULL }, 	//
        { ID_MUSIC_CHARGE_BETTERY5_BMP,    				NULL,  				 NULL }, 	//

        //播放器
        //play mode button
        { ID_MUSIC_PLAY_ALL_BMP,    				NULL,  				 NULL }, 		//播放全部, unfocused
        { ID_MUSIC_PLAY_ALL_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//播放全部, focused
        { ID_MUSIC_PLAY_ALL_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//播放全部, pressed

        { ID_MUSIC_PLAY_ONE_BMP,    				NULL,  				 NULL }, 		//播放一首, unfocused
        { ID_MUSIC_PLAY_ONE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//播放一首, focused
        { ID_MUSIC_PLAY_ONE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//播放一首, pressed

        { ID_MUSIC_PLAY_RANDOM_BMP,    				NULL,  				 NULL }, 		//随机播放, unfocused
        { ID_MUSIC_PLAY_RANDOM_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//随机播放, focused
        { ID_MUSIC_PLAY_RANDOM_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//随机播放, pressed

        { ID_MUSIC_REPEAT_ALL_BMP,    				NULL,  				 NULL }, 		//重复播放全部, unfocused
        { ID_MUSIC_REPEAT_ALL_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//重复播放全部, focused
        { ID_MUSIC_REPEAT_ALL_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//重复播放全部, pressed

        { ID_MUSIC_REPEAT_ONE_BMP,    				NULL,  				 NULL }, 		//重复播放一首, unfocused
        { ID_MUSIC_REPEAT_ONE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 		//重复播放一首, focused
        { ID_MUSIC_REPEAT_ONE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//重复播放一首, pressed

        //EQ mode button
        { ID_MUSIC_EQ_MODE_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 		//EQ mode button, unfocused
        { ID_MUSIC_EQ_MODE_BTN_FOCUSED_BMP,			NULL,  				 NULL }, 		//focused
        { ID_MUSIC_EQ_MODE_BTN_PRESSED_BMP,			NULL,  				 NULL }, 		//pressed

        //BL time button
        { ID_MUSIC_BL_TIME_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 		//EQ mode button, unfocused
        { ID_MUSIC_BL_TIME_BTN_FOCUSED_BMP,			NULL,  				 NULL }, 		//focused
        { ID_MUSIC_BL_TIME_BTN_PRESSED_BMP,			NULL,  				 NULL }, 		//pressed

        //play previous button
        { ID_MUSIC_PLAY_PRE_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 	//播放上一曲, unfocused
        { ID_MUSIC_PLAY_PRE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 	//focused
        { ID_MUSIC_PLAY_PRE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 		//pressed
        //play next button
        { ID_MUSIC_PLAY_NEXT_BTN_UNFOCUSED_BMP,		NULL,  				 NULL }, 	//播放下一曲, unfocused
        { ID_MUSIC_PLAY_NEXT_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 	//focused
        { ID_MUSIC_PLAY_NEXT_BTN_PRESSED_BMP,		NULL,  				 NULL }, 	//pressed
        //play pause button
        { ID_MUSIC_PLAY_PAUSE_BTN_UNFOCUSED_BMP,	NULL,  				 NULL }, 	//播放暂停, unfocused
        { ID_MUSIC_PLAY_PAUSE_BTN_FOCUSED_BMP,		NULL,  				 NULL }, 	//focused
        { ID_MUSIC_PLAY_PAUSE_BTN_PRESSED_BMP,		NULL,  				 NULL }, 	//pressed

        //play backward button
        { ID_MUSIC_PLAY_BACKWARD_BTN_UNFOCUSED_BMP,	NULL,  				 NULL }, 	//快退, unfocused
        { ID_MUSIC_PLAY_BACKWARD_BTN_FOCUSED_BMP,	NULL,  				 NULL }, 	//focused
        { ID_MUSIC_PLAY_BACKWARD_BTN_PRESSED_BMP,	NULL,  				 NULL }, 	//pressed

        //play forward button
        { ID_MUSIC_PLAY_FORWARD_BTN_UNFOCUSED_BMP,	NULL,  				 NULL }, 	//快进, unfocused
        { ID_MUSIC_PLAY_FORWARD_BTN_FOCUSED_BMP,	NULL,  				 NULL }, 	//focused
        { ID_MUSIC_PLAY_FORWARD_BTN_PRESSED_BMP,	NULL,  				 NULL }, 	//pressed

        //play status button,
        { ID_MUSIC_PLAYING_STATUS_BMP,				NULL,				NULL }, 								//正在播放状态
        { ID_MUSIC_PAUSE_STATUS_BMP,					NULL,  				NULL }, 								//暂停状态
        { ID_MUSIC_PREVIOUS_STATUS_BMP,				NULL,  		 		NULL }, 								//上一曲状态
        { ID_MUSIC_NEXT_STATUS_BMP,					NULL,  				NULL }, 								//下一曲状态

        //process bar
        { ID_MUSIC_PROCESS_BAR_HEAD_BMP,				NULL,  			 	NULL }, 								//process bar 头部
        { ID_MUSIC_PROCESS_BAR_BODY_BMP,				NULL,  				NULL }, 								//body
        { ID_MUSIC_PROCESS_BAR_TAIL_BMP,				NULL,  		 		NULL }, 								//尾部
        { ID_MUSIC_PROCESS_BAR_POINT_BMP,			NULL,  				NULL }, 								//进度点

        //歌曲信息图标
        { ID_MUSIC_PERFORMER_NAME_BMP,				NULL,  			 	NULL }, 								//作者图标
        { ID_MUSIC_FILE_SIZE_BMP,					NULL,  				NULL }, 								//
        { ID_MUSIC_ALBUM_NAME_BMP,					NULL,  		 		NULL }, 								//
        { ID_MUSIC_BIT_RATE_BMP,						NULL,  				NULL }, 								//

        //频谱图图标
        { ID_MUSIC_PARTICLE_BMP,						NULL,  			 	NULL },
        { ID_MUSIC_BOUY_BMP,							NULL,  			 	NULL },

        //浏览器，滚动条
        { ID_MUSIC_LIST_SCROLL_BG_BMP,				NULL,  			 	NULL },
        { ID_MUSIC_LIST_SCROLL_HEAD_BMP,				NULL,  			 	NULL },
        { ID_MUSIC_LIST_SCROLL_BODY_BMP,				NULL,  			 	NULL },
        { ID_MUSIC_LIST_SCROLL_TAIL_BMP,				NULL,  			 	NULL },


        { 0,                                    		NULL,  				NULL }      	//browser_icon_max
    },
};

//获得整个explorer AP设计的UI参数
music_ui_t *music_get_ui_param(void)
{
    return &(music_ui_400_240);
}

//获得音乐浏览器UI参数
music_list_ui_t *music_get_list_ui_param(void)
{
    return &(music_ui_400_240.list_ctrl);
}

//获得顶部HeadbarUI参数
music_headbar_ui_t *music_get_headbar_ui_param(void)
{
    return 	&(music_ui_400_240.headbar);
}

//获得顶部playerUI参数
music_palyer_ui_t *music_get_player_ui_param(void)
{
    return 	&(music_ui_400_240.player);
}

//获得volume barUI参数
music_volume_bar_ui_t *music_get_volume_bar_ui_param(void)
{
    return 	&(music_ui_400_240.volume_bar);
}
/*
************************************************************************************************************************
*Function	:         void* explorer_get_listview_icon_res(__u32 index)
*
*Description	:
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
void *music_get_icon_res(__u32 index)
{
    music_icon_data_t *icon_table = NULL;


    icon_table = music_ui_400_240.icon_table;

    if(index > MUSIC_ICON_MAX)
    {
        return NULL;
    }
    //__msg("----icon_id: %d, res_id: %d\n", index, icon_table[index].res_id);
    if(icon_table == NULL)
    {
        return NULL;
    }
    else if(icon_table[index].res_id == 0)
    {
        return NULL;
    }
    else if(icon_table[index].res_handle == NULL)
    {
#if 1
        icon_table[index].res_handle = theme_open(icon_table[index].res_id);
        icon_table[index].res_buffer = theme_hdl2buf(icon_table[index].res_handle);
        return icon_table[index].res_buffer;
#else
        {
            int i = 0;
            __msg("------------open all of the picture at first\n");
            app_print_memory_usage("new memory for picture resource", NULL);
            app_print_memory_usage("new memory for picture resource", NULL);
            for(i = 0; i < MUSIC_ICON_MAX; i++)
            {
                if(icon_table[i].res_handle == NULL)
                {
                    icon_table[i].res_handle = theme_open(icon_table[i].res_id);
                    icon_table[i].res_buffer = theme_hdl2buf(icon_table[i].res_handle);
                }
            }
            app_print_memory_usage("new memory for picture resource finished", NULL);

            return icon_table[index].res_buffer;
        }
#endif

    }
    else
    {
        return icon_table[index].res_buffer;
    }
}

/*
************************************************************************************************************************
*Function	:         __s32 explorer_free_listview_icon_res(void)
*
*Description	:
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
__s32 music_free_icon_res(void)
{
    __s32 i = 0;
    music_icon_data_t *icon_table = NULL;
    icon_table = music_ui_400_240.icon_table;

    if(icon_table == NULL)
    {
        return EPDK_FAIL;
    }
    else
    {
        //app_print_memory_usage("release memory for picture resource", NULL);
        //app_print_memory_usage("release memory for picture resource", NULL);
        for(i = 0; i < MUSIC_ICON_MAX; i++)
        {
            if(icon_table[i].res_handle != NULL)
            {
                theme_close(icon_table[i].res_handle);
                icon_table[i].res_handle = NULL;
                icon_table[i].res_buffer = NULL;
            }
        }
        //app_print_memory_usage("release memory for picture resource finished", NULL);
        return EPDK_OK;
    }
}

//获得Explorer 窗口layer size
__s32 music_get_screen_rect(RECT *screen_rect)
{
    music_ui_t *ui_param;
    ui_param = music_get_ui_param();

    screen_rect->x = ui_param->layer.scn_rect.x;
    screen_rect->y = ui_param->layer.scn_rect.y;
    screen_rect->width = ui_param->layer.scn_rect.width;
    screen_rect->height = ui_param->layer.scn_rect.height;
    return EPDK_OK;
}

//计算listbar每页可以显示的条目数量
__u32 music_count_listbar_pagenum(void)
{
    __u32 pagenum = 0;

    music_ui_t *ui_param;
    ui_param = music_get_ui_param();
    pagenum = ui_param->list_ctrl.listbar_rect.height / ui_param->list_ctrl.item_rect.height;
    return pagenum;
}













