/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    		: app_root_win_scene.c
* By      		: Bayden
* Func		: app_root applets plugin interface define
* Version	: v1.0
* ============================================================================================================
* 2010-9-28 17:11:20  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
//#define EPDK_DEBUG_LEVEL_GATE        4

#include "beetles_app.h"
#include "app_root.h"
#include "app_root_scene.h"
#include "app_root_init.h"
#include "home/app_home.h"
#include "explorer/explorer.h"
#include "movie/app_movie.h"
//#include "music/music.h"
#include "photo/app_photo.h"
#include "setting/app_setting.h"
//#include "fm/app_fm.h"
#include "ebook/app_ebook.h"
//#include "record/app_record.h"

#if  0
//#define __here__            eLIBs_printf("@L%d(%s)\n", __LINE__, __FILE__);
#define __msg(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
						     eLIBs_printf(__VA_ARGS__)									        )
#else
#define __msg(...)    	
#endif


typedef struct tag_root_ctrl
{
    H_WIN	h_app_home;
    H_WIN	h_app_explorer;
    H_WIN	h_app_fm;
    H_WIN	h_app_movie;
    H_WIN	h_app_music;
    H_WIN	h_app_photo;
    H_WIN	h_app_ebook;
    H_WIN	h_app_setting;
    H_WIN	h_app_calendar;
    H_WIN	h_app_record;
    H_WIN	h_app_dv;
    H_WIN	h_app_av1;

    root_para_t *root_para;
} root_ctrl_t;

/**********************************************************************************************************************/

typedef enum
{
    APP_ROOT_MSG_DO_ASYNC = GUI_MSG_ + 1,
} app_root_msg_id_t;

typedef __s32 (*p_app_root_fun)(void *arg);

dv_sub_res  sub_dv_res;


static __krnl_event_t *g_root_scene_sem = NULL;


#ifdef SUPPORT_TV_OUT
static __hdle tvout_detect_pin_hdl = 0;
static __s32 __request_tvout_detect_pin(void)
{
	__s32 ret;
	user_gpio_set_t  gpio_set[1];

	__msg("__request_tvout_detect_pin\n");

	eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));

	ret = esCFG_GetKeyValue("tv_in_para", "DETECT_PIN", (int *)gpio_set, sizeof(user_gpio_set_t)/4);
	if(ret)
	{
		__wrn("fetch DETECT_PIN script failed\n");
	}
	else
	{
		tvout_detect_pin_hdl = esPINS_PinGrpReq(gpio_set, 1);
		if(!tvout_detect_pin_hdl)
		{
			__msg("request DETECT_PIN failed\n");
			ret = EPDK_FAIL;
		}
		else
		{
			ret = esPINS_SetPinPull(tvout_detect_pin_hdl, PIN_PULL_UP, NULL);
			if(ret)
			{
				__wrn("pull DETECT_PIN failed\n");
			}
			esPINS_SetPinIO(tvout_detect_pin_hdl, 1, NULL);
		}
	}
	return ret;
}

void __release_tvout_detect_pin(void)
{
	if(tvout_detect_pin_hdl)
	{
		esPINS_PinGrpRel(tvout_detect_pin_hdl, 0);
	}
}



#if 1
#define PIOC_MEM_BASE   (0xf1c20800)

#define PIO_REG_CFG(n, i)               ((volatile unsigned int *)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x00))
#define PIO_REG_DLEVEL(n, i)            ((volatile unsigned int *)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x14))
#define PIO_REG_PULL(n, i)              ((volatile unsigned int *)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x1C))
#define PIO_REG_DATA(n) 	            ((volatile unsigned int *)( PIOC_MEM_BASE + ((n)-1)*0x24 + 0x10))

static __s32 get_gpio_status(__u32 port, __u32 port_num)
{
    volatile  __u32    *tmp_addr;
    __u32               tmp_data;
    __s32               ret;
    __u32               tmp1;
    __u32               tmp2;

    //设置为输入
    tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
    tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x07 << tmp1);
    tmp_data |=  (0x00 << tmp1);
    *tmp_addr = tmp_data;

    //设置为下拉
    tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
    tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x03 << tmp2);
    tmp_data |=  (0x02 << tmp2);
    *tmp_addr = tmp_data;


    tmp_addr = PIO_REG_DATA(port);
    tmp_data = *tmp_addr;

    __wrn("tmp_data=0x%x\n", tmp_data);
    ret = (tmp_data & (1 << port_num)) >> port_num;

    return ret;
}

static void pull_down_gpio(__u32 port, __u32 port_num)
{
    volatile  __u32    *tmp_addr;
    __u32               tmp_data;
    __u32               tmp1;
    __u32               tmp2;


    //设置为输出
    tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
    tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x07 << tmp1);
    tmp_data |=  (0x01 << tmp1);
    *tmp_addr = tmp_data;

    //设置为下拉
    tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
    tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x03 << tmp2);
    tmp_data |=  (0x02 << tmp2);
    *tmp_addr = tmp_data;

    //输出低电平
    tmp_addr = PIO_REG_DATA(port);
    tmp_data = *tmp_addr;
    tmp_data &= ~(1 << port_num);
    //tmp_data |=  (1 << port_num);
    *tmp_addr = tmp_data;

}

static void pull_up_gpio(__u32 port, __u32 port_num)
{
    volatile  __u32    *tmp_addr;
    __u32               tmp_data;
    __u32               tmp1;
    __u32               tmp2;


    //设置为输出
    tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
    tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x07 << tmp1);
    tmp_data |=  (0x01 << tmp1);
    *tmp_addr = tmp_data;

    //设置为上拉
    tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
    tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x03 << tmp2);
    tmp_data |=  (0x01 << tmp2);
    *tmp_addr = tmp_data;

    //输出高电平
    tmp_addr = PIO_REG_DATA(port);
    tmp_data = *tmp_addr;
    tmp_data &= ~(1 << port_num);
    tmp_data |=  (1 << port_num);
    *tmp_addr = tmp_data;

}

static __s32 __lcd_bl_open(void)
{

    __msg("__lcd_bl_open ok\n");
}
#endif
__u8 dsk_query_tvout_detect_pin(void)
{
	__u8 pin_valid;

	pin_valid = 0;
	/*if(tvout_detect_pin_hdl)
	{
		if(!esPINS_ReadPinData(tvout_detect_pin_hdl, 0))
		{	
			pin_valid = 1;
		}
		__msg("pin_valid = %d\n", pin_valid);
	}*/
	pin_valid = get_gpio_status(5,5);
	__msg("pin_valid = %d\n", pin_valid);
	return pin_valid;
}


#endif

/*****************************************************************************
 * 函 数 名  : DV_Uipara_Subset_Init
 * 负 责 人  : Zhibo
 * 创建日期  : 2017年6月26日
 * 函数功能  : 在dv模式下释放资源，会出错，暂时放在这里释放。以后视情况修改
 * 输入参数  : void  NULL
 * 输出参数  : 无
 * 返 回 值  : static
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
static void DV_Uipara_Subset_Init(void)
{
	create_bmp_res(ID_DV_SIGNAL_LEVEL_NONE_BMP, sub_dv_res->bmp_subset_singal[0]);
    create_bmp_res(ID_DV_SIGNAL_LEVEL_ONE_BMP, sub_dv_res->bmp_subset_singal[1]);
    create_bmp_res(ID_DV_SIGNAL_LEVEL_TWO_BMP, sub_dv_res->bmp_subset_singal[2]);
    create_bmp_res(ID_DV_SIGNAL_LEVEL_THREE_BMP, sub_dv_res->bmp_subset_singal[3]);
    create_bmp_res(ID_DV_SIGNAL_LEVEL_FOUR_BMP, sub_dv_res->bmp_subset_singal[4]);
    create_bmp_res(ID_DV_SIGNAL_LEVEL_FIVE_BMP, sub_dv_res->bmp_subset_singal[5]);

	
    create_bmp_res(ID_INIT_ENERGY_0_BMP, sub_dv_res->bmp_subset_vol[0]);
    create_bmp_res(ID_INIT_ENERGY_1_BMP, sub_dv_res->bmp_subset_vol[1]);
    create_bmp_res(ID_INIT_ENERGY_2_BMP, sub_dv_res->bmp_subset_vol[2]);
    create_bmp_res(ID_INIT_ENERGY_3_BMP, sub_dv_res->bmp_subset_vol[3]);
    create_bmp_res(ID_INIT_ENERGY_4_BMP, sub_dv_res->bmp_subset_vol[4]);
    create_bmp_res(ID_INIT_ENERGY_5_BMP, sub_dv_res->bmp_subset_vol[5]);

//	create_bmp_res(ID_DV_TOP_CAM_BMP, sub_dv_res->cam_play_pause[0]);
//	create_bmp_res(ID_DV_TOP1_CAM_BMP, sub_dv_res->cam_play_pause[1]);
	
	create_bmp_res(ID_DV_CAMERA_VIDEO_BMP, sub_dv_res->rec_cam_bmp[0]);
	create_bmp_res(ID_DV_CAMERA_PHOTO_BMP, sub_dv_res->rec_cam_bmp[1]);
	
	create_bmp_res(ID_DV_RECORD_STATUS_BMP, sub_dv_res->rec_status_bmp);
	
	create_bmp_res(ID_INIT_SD_N_BMP, sub_dv_res->sd_card_status[0]);
	create_bmp_res(ID_INIT_SD_Y_BMP, sub_dv_res->sd_card_status[1]);
//	create_bmp_res(ID_INIT_LOGO_BMP, sub_dv_res->set_logo);
}

/*****************************************************************************
 * 函 数 名  : DV_Uipara_Subset_UnInit
 * 负 责 人  : Zhibo
 * 创建日期  : 2017年6月26日
 * 函数功能  : 释放资源
 * 输入参数  : void  NULL
 * 输出参数  : 无
 * 返 回 值  : static
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
static void DV_Uipara_Subset_UnInit(void)
{
	destroy_bmp_res(sub_dv_res->bmp_subset_singal[0]);
	destroy_bmp_res(sub_dv_res->bmp_subset_singal[1]);
	destroy_bmp_res(sub_dv_res->bmp_subset_singal[2]);
	destroy_bmp_res(sub_dv_res->bmp_subset_singal[3]);
	destroy_bmp_res(sub_dv_res->bmp_subset_singal[4]);
	destroy_bmp_res(sub_dv_res->bmp_subset_singal[5]);
			
	destroy_bmp_res(sub_dv_res->bmp_subset_vol[0]);
	destroy_bmp_res(sub_dv_res->bmp_subset_vol[1]);
	destroy_bmp_res(sub_dv_res->bmp_subset_vol[2]);
	destroy_bmp_res(sub_dv_res->bmp_subset_vol[3]);
	destroy_bmp_res(sub_dv_res->bmp_subset_vol[4]);
	destroy_bmp_res(sub_dv_res->bmp_subset_vol[5]);

	
//	destroy_bmp_res(sub_dv_res->cam_play_pause[0]);
//	destroy_bmp_res(sub_dv_res->cam_play_pause[1]);
	
	destroy_bmp_res(sub_dv_res->rec_cam_bmp[0]);
	destroy_bmp_res(sub_dv_res->rec_cam_bmp[1]);
	
	destroy_bmp_res(sub_dv_res->rec_status_bmp);
	
	destroy_bmp_res(sub_dv_res->sd_card_status[0]);
	destroy_bmp_res(sub_dv_res->sd_card_status[1]);
//	destroy_bmp_res(sub_dv_res->set_logo);

}

/*****************************************************************************
 * 函 数 名  : dv_get_sub_res
 * 负 责 人  : Zhibo
 * 创建日期  : 2017年6月27日
 * 函数功能  : 返回资源
 * 输入参数  : void  NULL
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
dv_sub_res dv_get_sub_res(void)
{
	if(!sub_dv_res)
	{
		return sub_dv_res;
	}
}


__s32 __root_scene_sem_init(void)
{
    if (0 == g_root_scene_sem)
    {
        g_root_scene_sem = esKRNL_SemCreate(1);
    }

    if(g_root_scene_sem)
    {
        return EPDK_OK;
    }
    else
    {
        return EPDK_FAIL;
    }
}

__s32 __root_scene_sem_deinit(void)
{
    if (g_root_scene_sem)
    {
        __u8 err;

        esKRNL_SemDel(g_root_scene_sem, OS_DEL_ALWAYS, &err);
        g_root_scene_sem = NULL;
    }

    return EPDK_OK;
}

__s32 __root_scene_sem_pend(void)
{
    __u8 err;

    if(g_root_scene_sem)
    {
        __msg("before esKRNL_SemPend(g_root_scene_sem\n");
        esKRNL_SemPend(g_root_scene_sem, 0, &err);
        __msg("after esKRNL_SemPend(g_root_scene_sem\n");
    }

    return 0;
}

__s32 __root_scene_sem_accept(void)
{
    __u16 sem_nr;

    sem_nr = 0;

    if(g_root_scene_sem)
    {
        sem_nr = esKRNL_SemAccept(g_root_scene_sem);
    }

    return sem_nr;
}

__s32 __root_scene_sem_post(void)
{
    if(g_root_scene_sem)
    {
        esKRNL_SemPost(g_root_scene_sem);
    }

    return 0;
}


//向hwin发送异步消息，执行pfun函数
static __s32 __app_do_async(H_WIN hwin, __u32 msg_id, p_app_root_fun pfun, void *arg)
{
    if(0 == hwin)
    {
        return EPDK_FAIL;
    }

    {
        __gui_msg_t setmsg;
        setmsg.id			=  msg_id;
        setmsg.h_srcwin		=  NULL;
        setmsg.h_deswin		=  hwin;
        setmsg.dwAddData1	=  (__u32)pfun;
        setmsg.dwAddData2	=  (__u32)arg;
        setmsg.p_arg		=  NULL;

        __msg("setmsg.id=%x, setmsg.h_deswin=%x, setmsg.dwAddData1=%x\n, setmsg.dwAddData2=%x\n",
              setmsg.id, setmsg.h_deswin, setmsg.dwAddData1, setmsg.dwAddData2);
        GUI_SendNotifyMessage(&setmsg);
    }

    return EPDK_OK;
}

//向hwin发送setfocus消息，syn 为1代表需要直接调用GUI_WinSetFocusChild。
static void __app_set_focus_child(H_WIN hwin, __s32 syn)
{
    if (0 == hwin)
    {
        return;
    }

    if (1 == syn)
    {
        GUI_WinSetFocusChild(hwin);
    }
    else
    {
        __gui_msg_t setmsg;
        setmsg.id			=  GUI_MSG_SET_FOCUS;
        setmsg.h_srcwin		=  NULL;
        setmsg.h_deswin		=  hwin;
        setmsg.dwAddData1	=  NULL;
        setmsg.dwAddData2	=  NULL;
        setmsg.p_arg		=  NULL;
        GUI_SendNotifyMessage(&setmsg);
    }

}

static void __app_home_restore(__gui_msg_t *msg)
{
    root_ctrl_t   *root_ctrl;

    root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    if (root_ctrl->h_app_home)
    {
        __gui_msg_t mymsg;
        __log("----jh_dbg1020_1---\n");

        mymsg.h_deswin = root_ctrl->h_app_home;
        mymsg.id = DSK_APP_RESTORE;
        mymsg.dwAddData1 = 0;
        mymsg.dwAddData2 = 0;
        GUI_SendNotifyMessage(&mymsg);

        mymsg.h_deswin = root_ctrl->h_app_home;
        mymsg.id = DSK_MSG_FS_PART_PLUGOUT;
        mymsg.dwAddData1 = msg->dwAddData1;
        mymsg.dwAddData2 = msg->dwAddData2;
        GUI_SendNotifyMessage(&mymsg);

        __app_set_focus_child(root_ctrl->h_app_home, 0);
    }
    else
    {
        __log("----jh_dbg1020_2---\n");
        root_ctrl->h_app_home = app_home_create(root_ctrl->root_para);
        __app_set_focus_child(root_ctrl->h_app_home, 0);
    }
}

static __s32 __app_root_is_bg_music_exist(__gui_msg_t *msg, H_WIN *hMusic)
{
    H_WIN root, child;
    char  winname[256];
    __bool exist;

    root  = GUI_WinGetHandFromName(APP_ROOT);
    child = GUI_WinGetFirstChild(root);

    exist = 0;
    *hMusic = 0;
    while( child != NULL )
    {
        eLIBs_memset(winname, 0, 256);
        GUI_WinGetName(child, winname);

        __msg("winname = %s \n", winname);

        if( 0 == eLIBs_strcmp( APP_MUSIC, winname))
        {
            *hMusic = child;
            exist = 1;
            break;
        }

        child = GUI_WinGetNextBro(child);
    }

    return exist;
}


static void __app_root_delete_bg_music(__gui_msg_t *msg)
{
    H_WIN root, child, next_child;
    char  winname[256];
    root_para_t *root_para;
    root_ctrl_t   *root_ctrl;

    root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    root_para = root_ctrl->root_para;

    root  = GUI_WinGetHandFromName(APP_ROOT);
    if(!root)
    {
        __err("GUI_WinGetHandFromName fail...\n");
        return ;
    }
    child = GUI_WinGetFirstChild(root);

    while( child != NULL )
    {
        eLIBs_memset(winname, 0, 256);
        GUI_WinGetName(child, winname);

        next_child = GUI_WinGetNextBro(child);

        __msg("winname = %s \n", winname);

        if( 0 == eLIBs_strcmp( APP_MUSIC, winname))
        {
            GUI_ManWinDelete(child);
            root_ctrl->h_app_music = NULL;
            break;
        }

        child = next_child;
    }
}

static void __app_root_create_bg_music(__gui_msg_t *msg)
{
/*
    H_WIN root, child;
    char  winname[256];
    __bool exist;
    root_para_t *root_para;
    root_ctrl_t   *root_ctrl;

    root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    root_para = root_ctrl->root_para;

    root  = GUI_WinGetHandFromName(APP_ROOT);
    child = GUI_WinGetFirstChild(root);

    exist = 0;
    while( child != NULL )
    {
        eLIBs_memset(winname, 0, 256);
        GUI_WinGetName(child, winname);

        __msg("winname = %s \n", winname);

        if( 0 == eLIBs_strcmp( APP_MUSIC, winname))
        {
            exist = 1;
            break;
        }

        child = GUI_WinGetNextBro(child);
    }

    if(0 == exist)
    {
        root_para->data = MUSICPLAYER_BACKGROUND;
        root_ctrl->root_para->h_parent = GUI_WinGetParent(root_ctrl->h_app_home);
        root_ctrl->h_app_music =  app_music_create(root_para);
    }
	*/
}


//进入应用之前把背景设置为默认背景
static void __app_root_change_to_default_bg(void)
{
    reg_init_para_t *para;
    para = dsk_reg_get_para_by_app(REG_APP_INIT);
    if(para)
    {
        para->bgd_flag_in_main = gscene_bgd_get_flag();
        if(EPDK_TRUE == para->bgd_flag_in_main)//当前是默认背景，则不做任何事情
        {
        }
        else//当前不是默认背景，修改为默认背景
        {
            gscene_bgd_set_flag(EPDK_TRUE);
            gscene_bgd_refresh();
        }
    }
    else
    {
        __msg("dsk_reg_get_para_by_app fail...\n");
    }
}

static __s32 app_root_command_proc(__gui_msg_t *msg)
{
    root_para_t *root_para;
    root_ctrl_t   *root_ctrl;
    __gui_msg_t mymsg;
    static __u8  to_move_from = 0;

    root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    root_para = root_ctrl->root_para;

    //__msg("app_root_command_proc\n");

    switch (LOWORD(msg->dwAddData1))
    {
    case APP_HOME_ID:
    {
        switch (HIWORD(msg->dwAddData1))
        {
        case SWITCH_TO_OTHER_APP:
        {
            switch(msg->dwAddData2)
            {

            case ID_HOME_MOVIE:
            {
                gscene_hbar_set_state(HBAR_ST_HIDE);
                __app_root_change_to_default_bg();

                __msg("ID_HOME_MOVIE: enter movie explorer\n");

                //esMEMS_Info();
                to_move_from = 0;
                if(msg->dwReserved & 0x80)
                {
                    root_para->data = ID_EXPLORER_ALL;
                    to_move_from = 1;
                }
                else
                {              	
                    root_para->data = ID_EXPLORER_MOVIE;
                }
                msg->dwReserved = msg->dwReserved & 0x7f;
                root_para->root_type = msg->dwReserved;//RAT_TF;
                root_para->explr_root = msg->dwReserved;
                // root_para->data = ID_EXPLORER_MOVIE;
                root_ctrl->root_para->h_parent = GUI_WinGetParent(root_ctrl->h_app_home);
                root_ctrl->h_app_explorer =  app_explorer_create(root_para);
                GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
#if EPDK_AUTO_PLAY_MOVIE_ENABLE
                {
                    __gui_msg_t new_msg ;
                    eLIBs_printf("send enter key down message to explorer win\n");
                    new_msg.h_deswin = root_ctrl->h_app_explorer;
                    new_msg.id = GUI_MSG_KEY;
                    new_msg.dwAddData1 = GUI_MSG_KEY_ENTER;
                    new_msg.dwAddData2 = KEY_DOWN_ACTION;
                    GUI_SendAsyncMessage(&new_msg);
                    esKRNL_TimeDly(20);

                    eLIBs_printf("send enter key up message to explorer win\n");
                    new_msg.h_deswin = root_ctrl->h_app_explorer;
                    new_msg.id = GUI_MSG_KEY;
                    new_msg.dwAddData1 = GUI_MSG_KEY_ENTER;
                    new_msg.dwAddData2 = KEY_UP_ACTION;
                    GUI_SendAsyncMessage(&new_msg);
                }
#endif

                break;
            }
            break;
            case ID_HOME_PHOTO:
            {
                gscene_hbar_set_state(HBAR_ST_HIDE);
                __app_root_change_to_default_bg();
                __msg("enter photo explorer\n");

                __msg("root type=%d\n", msg->dwReserved);
                root_para->root_type = msg->dwReserved;
                root_para->explr_root = msg->dwReserved;
                root_para->data = ID_EXPLORER_PHOTO;
                root_ctrl->root_para->h_parent = GUI_WinGetParent(root_ctrl->h_app_home);
                root_ctrl->h_app_explorer =  app_explorer_create(root_para);
                __msg("root_ctrl->h_app_explorer = %x\n", root_ctrl->h_app_explorer);
                GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
                break;
            }
            break;
            case ID_HOME_DV:
            {
                __s32 ret;
                __s32 exist;
                H_WIN hmusic;

                gscene_hbar_set_state(HBAR_ST_HIDE);
                __app_root_change_to_default_bg();

				//__msg("ID_HOME_DV\n");
                root_para->font			= SWFFont;
                root_para->root_type	= 0;
//                root_ctrl->h_app_av1	= app_av1_create(root_para);
//                root_ctrl->root_para->h_parent = GUI_WinGetParent(root_ctrl->h_app_home);
				
                root_ctrl->h_app_dv	= app_dv_create(root_para);
                root_ctrl->root_para->h_parent = GUI_WinGetParent(root_ctrl->h_app_home);
                GUI_WinSetFocusChild(root_ctrl->h_app_dv);
            }
            break;
			case ID_ROOT_SETTIN_CMMD:
			{
				gscene_hbar_set_state(HBAR_ST_HIDE);
				if (0 == msg->dwReserved)
                {
                    __app_root_change_to_default_bg();
                    __msg("enter setting\n");
                    root_ctrl->root_para->setting_opt = 0;//通用设置
                    root_ctrl->root_para->h_parent = msg->h_deswin;
                    root_ctrl->h_app_setting =  app_setting_create(root_para);
                    GUI_WinSetFocusChild(root_ctrl->h_app_setting);
                    break;
                }
				else if (1 == msg->dwReserved)
                {
                    __app_root_change_to_default_bg();

                    __msg("enter setting\n");
                    root_ctrl->root_para->setting_opt = 1;//电源管理
                    root_ctrl->root_para->h_parent = msg->h_deswin;
                    root_ctrl->h_app_setting =  app_setting_create(root_para);
                    GUI_WinSetFocusChild(root_ctrl->h_app_setting);
                    break;
                }
				break;
			}
            case ID_HOME_OTHERS:
            {
                gscene_hbar_set_state(HBAR_ST_HIDE);
                //删除后台音乐

                __msg("**********enter other apps**********\n");
                if (0 == msg->dwReserved)
                {
                    __app_root_change_to_default_bg();
                    __msg("enter setting\n");
                    root_ctrl->root_para->setting_opt = 0;//通用设置
                    root_ctrl->root_para->h_parent = GUI_WinGetParent(root_ctrl->h_app_home);
                    root_ctrl->h_app_setting =  app_setting_create(root_para);
                    GUI_WinSetFocusChild(root_ctrl->h_app_setting);
                    break;
                }
                else if (1 == msg->dwReserved)
                {
                    __app_root_change_to_default_bg();

                    __msg("enter setting\n");
                    root_ctrl->root_para->setting_opt = 1;//电源管理
                    root_ctrl->root_para->h_parent = GUI_WinGetParent(root_ctrl->h_app_home);
                    root_ctrl->h_app_setting =  app_setting_create(root_para);
                    GUI_WinSetFocusChild(root_ctrl->h_app_setting);
                    break;
                }
                else if (2 == msg->dwReserved)
                {
#if BEETLES_RTC_EXIST
                    __msg("enter calendar\n");

                    __app_root_change_to_default_bg();
                    root_ctrl->root_para->h_parent = GUI_WinGetParent(root_ctrl->h_app_home);
                    root_ctrl->h_app_calendar =  app_calendar_create(root_para);
                    GUI_WinSetFocusChild(root_ctrl->h_app_calendar);

#else

                    __msg("**********enter explorer manage**********\n");
                    gscene_hbar_set_state(HBAR_ST_HIDE);
                    __app_root_change_to_default_bg();

                    __msg("**********enter movie explorer**********\n");

                    __msg("root type=%d\n", msg->dwReserved);
                    root_para->root_type = RAT_TF;//msg->dwReserved;//RAT_TF;
                    root_para->explr_root = RAT_TF;//msg->dwReserved;
                    root_para->data = ID_EXPLORER_ALL;
                    root_ctrl->root_para->h_parent = GUI_WinGetParent(root_ctrl->h_app_home);
                    root_ctrl->h_app_explorer =  app_explorer_create(root_para);
                    __msg("root_ctrl->h_app_explorer = %x\n", root_ctrl->h_app_explorer);
                    GUI_WinSetFocusChild(root_ctrl->h_app_explorer);

#endif
                    break;
                }
                else if (3 == msg->dwReserved)
                {
#if BEETLES_RTC_EXIST
                    __msg("**********enter explorer manage**********\n");
                    gscene_hbar_set_state(HBAR_ST_HIDE);
                    __app_root_change_to_default_bg();

                    __msg("**********enter movie explorer**********\n");

                    __msg("root type=%d\n", msg->dwReserved);
                    root_para->root_type = RAT_TF;//msg->dwReserved;//RAT_TF;
                    root_para->explr_root = RAT_TF;//msg->dwReserved;
                    root_para->data = ID_EXPLORER_ALL;
                    root_ctrl->root_para->h_parent = GUI_WinGetParent(root_ctrl->h_app_home);
                    root_ctrl->h_app_explorer =  app_explorer_create(root_para);
                    __msg("root_ctrl->h_app_explorer = %x\n", root_ctrl->h_app_explorer);
                    GUI_WinSetFocusChild(root_ctrl->h_app_explorer);

#else

                    __err("sub menu focus item invalid...\n");

#endif
                    break;
                }
                else
                {
                    __err("sub menu focus item invalid...\n");
                    break;
                }
            }
            break;
            default:
                break;
            }
        }
        break;
        default:
            break;
        }
    }
    break;
    case APP_EXPLORER_ID:
    {
        switch(HIWORD(msg->dwAddData1))
        {
        case SWITCH_TO_OTHER_APP:
        {
            switch( msg->dwAddData2 )
            {
            case EXPLR_SW_TO_MAIN:
            {
                if(root_ctrl->h_app_explorer)
                {
                    GUI_ManWinDelete(root_ctrl->h_app_explorer);
                    root_ctrl->h_app_explorer = 0;
                }
                __here__;

                __app_home_restore(msg);
            }
            break;
            case EXPLR_SW_TO_MOVIE:
            {
                //删除后台音乐
                __app_root_delete_bg_music(msg);

                __msg("**********explorer to movie**********\n");
                //esMEMS_Info();

                //root_para->root_type = msg->dwReserved;
                root_para->data = 0;
                root_para->root_type = root_para->explr_root  ;
                root_ctrl->h_app_movie = app_movie_create(root_para);
                GUI_WinSetFocusChild(root_ctrl->h_app_movie);
            }
            break;
            case EXPLR_SW_TO_PHOTO:
            {
                //删除后台音乐
                //__app_root_delete_bg_music(msg);

                __msg("**********explorer to photo**********\n");

                //root_para->root_type = msg->dwReserved;
                root_para->data = 0;
                root_para->root_type = root_para->explr_root  ;
                root_ctrl->h_app_photo = app_photo_create(root_para);
                GUI_WinSetFocusChild(root_ctrl->h_app_photo);

            }
            break;
            case EXPLR_SW_TO_MUSIC:

                break ;
            case EXPLR_SW_TO_EBOOK:

                break;
            default:
            {
                __err("media type err...\n");
            }
            break;
            }
            root_ctrl->h_app_explorer = NULL;//浏览器已自己删除
        }
        break;
        default:
            break;
        }
    }
    break;
    case APP_MOVIE_ID:
    {
        __msg("**********movie to explorer**********\n");
        __msg("msg->dwAddData1=%d\n", msg->dwAddData1);
        __msg("msg->dwAddData2=%d\n", msg->dwAddData2);
        __msg("msg->dwReserved=%d\n", msg->dwReserved);
        switch (HIWORD(msg->dwAddData1))
        {
        case SWITCH_TO_EXPLORER:
        {

            GUI_ManWinDelete(root_ctrl->h_app_movie);
            root_ctrl->h_app_movie = 0;
            __here__;
            root_para->root_type = root_para->explr_root;
            if(to_move_from)
                root_para->data = ID_EXPLORER_ALL;
            else
                root_para->data = ID_EXPLORER_MOVIE;
            root_ctrl->root_para->h_parent = GUI_WinGetParent(root_ctrl->h_app_home);
            root_ctrl->h_app_explorer =  app_explorer_create(root_para);
            GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
            break;
        }
        case SWITCH_TO_MMENU:
        {
            GUI_ManWinDelete(root_ctrl->h_app_movie);
            root_ctrl->h_app_movie = 0;
            __here__;
            //esMEMS_Info();
            __app_home_restore(msg);

            break;
        }
        case VIDEO_SW_TO_AUDIO:
        {
			/*
            GUI_ManWinDelete(root_ctrl->h_app_movie);
            root_ctrl->h_app_movie = 0;
            DebugPrintf("music switch to movie ...........root_para->explr_root=%d\n", root_para->explr_root);
            root_para->root_type = root_para->explr_root;
            root_para->data = MUSICPLAYER_NORMAL;
            root_ctrl->root_para->h_parent = GUI_WinGetParent(root_ctrl->h_app_home);
            root_ctrl->h_app_music =  app_music_create(root_para);
            GUI_WinSetFocusChild(root_ctrl->h_app_music);
			*/
            break;
        }
        default:
        {
            break;
        }
        }

        break;

    }
    case APP_DV_ID:
        __msg("APP_DV_ID\n");

        switch (HIWORD(msg->dwAddData1))
        {
        case SWITCH_TO_MMENU:
            GUI_ManWinDelete(root_ctrl->h_app_dv);
            root_ctrl->h_app_dv = 0;
            __app_home_restore(msg);
            break;
        default:
            break;
        }

        break;
    case  APP_AV1_ID:

        switch (HIWORD(msg->dwAddData1))
        {
        case SWITCH_TO_MMENU:
            GUI_ManWinDelete(root_ctrl->h_app_av1);
            root_ctrl->h_app_av1 = 0;
            __app_home_restore(msg);
            break;
        default:
            break;
        }

        break;
    case APP_MUSIC_ID:

        break;
    case APP_PHOTO_ID:
    {
        switch (HIWORD(msg->dwAddData1))
        {
        case SWITCH_TO_EXPLORER:
        {
            //删除后台音乐
            // __app_root_delete_bg_music(msg);

            __msg("**********photo to explorer**********\n");

            GUI_ManWinDelete(root_ctrl->h_app_photo);
            root_ctrl->h_app_photo = 0;

            gscene_bgd_set_flag(EPDK_TRUE);
            //gscene_bgd_refresh();

            __here__;
            root_para->root_type = root_para->explr_root;
            if(to_move_from)
                root_para->data = ID_EXPLORER_ALL;
            else
                root_para->data = ID_EXPLORER_PHOTO;
            root_ctrl->root_para->h_parent = GUI_WinGetParent(root_ctrl->h_app_home);
            root_ctrl->h_app_explorer =  app_explorer_create(root_para);
            GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
            break;
        }
        case OPEN_BG_MUSIC:
        {
            __app_root_create_bg_music(msg);
            break;
        }
        case CLOSE_BG_MUSIC:
        {
            __app_root_delete_bg_music(msg);
            break;
        }
        case SWITCH_TO_MMENU:
        {
            GUI_ManWinDelete(root_ctrl->h_app_photo);
            root_ctrl->h_app_photo = 0;
            __here__;
            __app_home_restore(msg);

            break;
        }
        default:
        {
            break;
        }
        }

        break;
    }
    case APP_EBOOK_ID:
        break;

    case APP_FM_ID:
        break;
    case APP_RECORD_ID:
        break;
    case APP_CALENDAR_ID:
    {
        switch (HIWORD(msg->dwAddData1))
        {
        case SWITCH_TO_MMENU:
        {
            __msg("**********calendar to home**********\n");

            GUI_ManWinDelete(root_ctrl->h_app_calendar);
            root_ctrl->h_app_calendar = 0;

            __app_home_restore(msg);

            break;
        }
        default:
        {
            break;
        }
        }

        break;
    }
    case APP_SETTING_ID:
    {
        switch(HIWORD(msg->dwAddData1))
        {
        case SWITCH_TO_OTHER_APP:
        {
            switch(msg->dwAddData2)
            {
            case SETTING_SW_TO_MAIN:
            {
                __msg("**********setting to home**********\n");
                GUI_ManWinDelete(root_ctrl->h_app_setting);
                root_ctrl->h_app_setting = 0;
                __app_home_restore(msg);

            }
            break;
            case SETTING_SW_TO_MULTIMEDIA:
            {

            }
            break;
            case SETTING_SW_TO_PROMPT_UPGRADE:
            {

            }
            break;
            case SETTING_SW_TO_PROMPT_DEFAULT:
            {
            }
            break;
            case EXPLR_SW_TO_UPGRADE:
            {

            }
            break;
            default:
                break;
            }
            //root_ctrl->h_app_setting = NULL;
        }
        break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }

    return EPDK_OK;
}

static __s32 __app_root_broadcast_msg(__gui_msg_t *msg)
{
    H_WIN root, child;
    char  winname[256];
    __gui_msg_t set_msg;

    eLIBs_memcpy(&set_msg, msg, sizeof(__gui_msg_t));

    root  = msg->h_deswin;
    child = GUI_WinGetFirstChild(root);

    while( child != NULL )
    {
        eLIBs_memset(winname, 0, 256);
        GUI_WinGetName(child, winname);

        __msg("app_root_broadcast_msg msg->id=%d,winname = %s \n",
              msg->id, winname);

        set_msg.h_deswin = child;
        GUI_SendMessage(&set_msg);
        // GUI_SendNotifyMessage(&set_msg);
        __here__;

        child = GUI_WinGetNextBro(child);
    }

    return EPDK_OK;
}

static __s32  __app_root_send_msg2focus_child(__gui_msg_t *msg)
{
    H_WIN root, child;
    __gui_msg_t set_msg;

    eLIBs_memcpy(&set_msg, msg, sizeof(__gui_msg_t));

    root  = msg->h_deswin;
    child = GUI_WinGetFocusChild(root);
    if(child)
    {
        GUI_SendMessage(&set_msg);
    }

    return EPDK_OK;
}

static __hdle           g_pe0 = 0;
static __hdle           g_pe1 = 0;
static __hdle           g_pe2 = 0;
static __hdle           g_pe3 = 0;

static __s32 __app_request_pins(void)
{
    __s32            ret;
    user_gpio_set_t  gpio_set[1];

    /* 申请input_row1 */
    eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
    ret = esCFG_GetKeyValue("matrixkey_para", "input_row1", (int *)gpio_set, sizeof(user_gpio_set_t) / 4);

    __msg("ret= %d\n", ret);

    {
        __u32 *pTPCtrl = NULL;
        __u32 data = 0;
        pTPCtrl = (__u32 *)0xf1c24800; //必须打开AD模块，PE2口才会有效
        data = *pTPCtrl;
        __msg("tp ctrl registor is %d\n", data);
        data |= 0x07;
        *pTPCtrl = data;
        __msg("tp ctrl registor is %d\n", data);
    }

    if(!g_pe0)
    {
        /* 申请tv_en */
        eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
        ret = esCFG_GetKeyValue("test_peport", "test_pea", (int *)gpio_set, sizeof(user_gpio_set_t) / 4);
        if (!ret)
        {
            g_pe0 = esPINS_PinGrpReq(gpio_set, 1);
            if (!g_pe0)
            {
                __msg("request output_col pin failed\n");
                return EPDK_FAIL;
            }
        }
        else
        {
            __msg("fetch para from script failed\n");
            return EPDK_FAIL;
        }
    }

    if(!g_pe1)
    {
        /* 申请tv_en */
        eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
        ret = esCFG_GetKeyValue("test_peport", "test_peb", (int *)gpio_set, sizeof(user_gpio_set_t) / 4);
        if (!ret)
        {
            g_pe1 = esPINS_PinGrpReq(gpio_set, 1);
            if (!g_pe1)
            {
                __msg("request output_col pin failed\n");
                return EPDK_FAIL;
            }
        }
        else
        {
            __msg("fetch para from script failed\n");
            return EPDK_FAIL;
        }
    }

    if(!g_pe2)
    {
        /* 申请tv_en */
        eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
        ret = esCFG_GetKeyValue("test_peport", "test_pec", (int *)gpio_set, sizeof(user_gpio_set_t) / 4);
        if (!ret)
        {
            g_pe2 = esPINS_PinGrpReq(gpio_set, 1);
            if (!g_pe2)
            {
                __msg("request output_col pin failed\n");
                return EPDK_FAIL;
            }
        }
        else
        {
            __msg("fetch para from script failed\n");
            return EPDK_FAIL;
        }
    }

    if(!g_pe3)
    {
        /* 申请tv_en */
        eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
        ret = esCFG_GetKeyValue("test_peport", "test_ped", (int *)gpio_set, sizeof(user_gpio_set_t) / 4);
        if (!ret)
        {
            g_pe3 = esPINS_PinGrpReq(gpio_set, 1);
            if (!g_pe3)
            {
                __msg("request output_col pin failed\n");
                return EPDK_FAIL;
            }
        }
        else
        {
            __msg("fetch para from script failed\n");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static __s32 __app_pullup_pe(void)
{
    __s32            ret;
    user_gpio_set_t  gpio_set[16];

    if(!g_pe0 || !g_pe1 || !g_pe2 || !g_pe3 )
    {
        __msg("__app_pullup_pe fail...\n");
        return EPDK_FAIL;
    }

    __msg("__app_pullup_pe...\n");
    esPINS_WritePinData(g_pe0, 1, 0);
    esPINS_WritePinData(g_pe1, 1, 0);
    esPINS_WritePinData(g_pe2, 1, 0);
    esPINS_WritePinData(g_pe3, 1, 0);

    return EPDK_OK;
}

static __s32 __app_pulldown_pe(void)
{
    __s32            ret;
    user_gpio_set_t  gpio_set[16];

    if(!g_pe0 || !g_pe1 || !g_pe2 || !g_pe3 )
    {
        __msg("__app_pullup_pe fail...\n");
        return EPDK_FAIL;
    }

    __msg("__app_pulldown_pe...\n");
    esPINS_WritePinData(g_pe0, 0, 0);
    esPINS_WritePinData(g_pe1, 0, 0);
    esPINS_WritePinData(g_pe2, 0, 0);
    esPINS_WritePinData(g_pe3, 0, 0);

    return EPDK_OK;
}

void exfat_read_test(void *arg)
{
    ES_FILE *fp = NULL;
    __u8 buff[128] = {0};
    __u8 name_buff[32];
    __size len = 0;
    ES_DIR *fdir = NULL;
    ES_DIRENT *fdirent = NULL;
    eLIBs_printf("exfat_read_test.\n");

    fdir = eLIBs_opendir("f:");
    if(fdir == NULL)
    {
        eLIBs_printf("open f: failed!\n");
        goto out;
    }
    if((fdirent = eLIBs_readdir(fdir)) != NULL)
    {
        eLIBs_printf("11 %s\n", fdirent->d_name);
        eLIBs_sprintf(name_buff, "f:\\\\%s", fdirent->d_name);
        eLIBs_printf("name_buff = %s\n", name_buff);
        fp = eLIBs_fopen(name_buff, "r+");
        if(fp == NULL)
        {
            eLIBs_printf("open f:\\\\1.txt failed!\n");
            goto out;
        }

    }
    else
    {
        goto out;
    }


    while(1)
    {
        len = eLIBs_fread(buff, 1, 120, fp);
        if(len > 0)
        {
            eLIBs_printf("%s\n", buff);
        }
        eLIBs_memset(buff, 0, 128);
        esKRNL_TimeDly(100);
    }
out:
    while(1)
    {
        esKRNL_TimeDly(100);
    }
}

void root_cmmd2root_send(__gui_msg_t *msg)
{
	__gui_msg_t mymsg;

	__msg("root_cmmd2root_send\n");
	mymsg.h_deswin = GUI_WinGetHandFromName(APP_ROOT);//GUI_WinGetParent(hwin);
    mymsg.h_srcwin = NULL;
    mymsg.id = GUI_MSG_COMMAND;
    mymsg.dwAddData1 = MAKELONG(APP_HOME_ID, SWITCH_TO_OTHER_APP);
    mymsg.dwAddData2 = ID_ROOT_SETTIN_CMMD;
    mymsg.dwReserved = 0;

    GUI_SendNotifyMessage(&mymsg);
}

static __s32 app_root_enter_setting(__gui_msg_t *msg)
{
	reg_root_para_t *root_para;
	root_ctrl_t   *root_ctrl;

    root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
	
	__msg("delete_all_app except home\n");
	if(root_ctrl == NULL)
    {
        return EPDK_FAIL;
    }
    root_para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

	if(root_ctrl->h_app_movie != NULL)
    {
        GUI_ManWinDelete(root_ctrl->h_app_movie);
        root_ctrl->h_app_movie = 0;
    }

	if(root_ctrl->h_app_photo != NULL)
    {
        GUI_ManWinDelete(root_ctrl->h_app_photo);
        root_ctrl->h_app_photo = 0;
    }

	if(root_ctrl->h_app_explorer != NULL)
    {
        GUI_ManWinDelete(root_ctrl->h_app_explorer);
        root_ctrl->h_app_explorer = 0;
    }

	if(root_ctrl->h_app_av1 != NULL)
    {
        GUI_ManWinDelete(root_ctrl->h_app_av1);
        root_ctrl->h_app_av1 = 0;
    }

	if(root_ctrl->h_app_dv != NULL)
    {
        GUI_ManWinDelete(root_ctrl->h_app_dv);
        root_ctrl->h_app_dv = 0;
    }

	if(root_ctrl->h_app_home != NULL)
	{
		GUI_ManWinDelete(root_ctrl->h_app_home);
        root_ctrl->h_app_home = 0;
	}

	if(root_ctrl->h_app_setting != NULL)
	{	
		__msg("app_setting exist\n");
		return EPDK_OK;
	}
	else
	{
		gscene_bgd_set_state(BGD_STATUS_SHOW);
		root_cmmd2root_send(msg);
//        __msg("enter setting\n");
//		gscene_hbar_set_state(HBAR_ST_HIDE);
//		gscene_bgd_set_state(BGD_STATUS_SHOW);
//        __app_root_change_to_default_bg();
//        root_ctrl->root_para->setting_opt = 0;//通用设置
//        root_ctrl->root_para->h_parent = msg->h_deswin;
//        root_ctrl->h_app_setting =  app_setting_create(root_ctrl->root_para);
//        GUI_WinSetFocusChild(root_ctrl->h_app_setting);
	}

	return EPDK_OK;
}

static void dsk_display_set_FLICKER( )
{
    ES_FILE *fp;

    fp = eLIBs_fopen("b:\\disp\\display", "r+");
    if(fp)
    {
        __u32 args[3] = {0};

        args[0] = 0;
        __msg("DISP_CMD_SET_DE_FLICKER, args[0]=%d\n", args[0]);
        eLIBs_fioctrl(fp, DISP_CMD_SET_DE_FLICKER, 0, args);
        eLIBs_fclose(fp);
    }
    else
    {
        __msg("open disp drv fail...\n");
    }
}

__s32 set_volue(__u8 val)
{
    ES_FILE *fp = NULL;
    __s32 tmpVolume = 0;
    fp = eLIBs_fopen("b:\\AUDIO\\CTRL", "rb+");
    if(fp == NULL)
    {
        __wrn("fopen failed.\n");
        return EPDK_FAIL;
    }

    tmpVolume = eLIBs_fioctrl(fp, AUDIO_DEV_CMD_SET_VOLUME, val, (void *)AUDIO_DEV_PLAY);
    if(tmpVolume != val)
    {
        __wrn("set paly volume failed!\n");
    }
    eLIBs_fclose(fp);
}

__s32 app_root_win_proc(__gui_msg_t *msg)
{
    //__msg("app_root_win_proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
    //      , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);

    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        root_para_t *root_para;
        root_ctrl_t   *root_ctrl;

        root_para = esMEMS_Balloc(sizeof(root_para_t));
        if(!root_para)
        {
            return EPDK_FAIL;
        }
		
		DV_Uipara_Subset_Init();
		
        eLIBs_memset(root_para, 0, sizeof(root_para_t));

        root_ctrl = esMEMS_Balloc(sizeof(root_ctrl_t));
        if(!root_ctrl)
        {
            esMEMS_Bfree(root_para, sizeof(root_para_t));
            return EPDK_FAIL;
        }
        eLIBs_memset(root_ctrl, 0, sizeof(root_ctrl_t));

        set_volue(45);

        root_para->h_parent		= msg->h_deswin;
        root_para->font			= SWFFont;
        root_para->root_type	= 0;
		
		root_ctrl->h_app_dv	= app_dv_create(root_para);
		GUI_WinSetFocusChild(root_ctrl->h_app_dv);

        root_ctrl->root_para = root_para;
        GUI_WinSetAddData(msg->h_deswin, (__u32)root_ctrl);
#ifdef 	SUPPORT_TV_OUT	
		__request_tvout_detect_pin();
#endif
		GUI_SetTimer(msg->h_deswin, APP_ROOT_CHECK_MEM_INFO_ID, APP_ROOT_CHECK_MEM_INFO_TIMER_ID, NULL);
        __root_scene_sem_init();

    }
    return EPDK_OK;
    case GUI_MSG_CLOSE:
    {
        GUI_ManWinDelete(msg->h_deswin);
    }
    return EPDK_OK;
    case GUI_MSG_DESTROY:
    {
        root_para_t *root_para;
        root_ctrl_t   *root_ctrl;

        root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        if (!root_ctrl)
        {
            __msg("******err****\n");
            return EPDK_OK;
        }
		DV_Uipara_Subset_UnInit();
        root_para = (root_para_t *)root_ctrl->root_para;
        if (!root_para)
        {
            __msg("******err****\n");
            return EPDK_OK;
        }
		if(GUI_IsTimerInstalled(msg->h_deswin, APP_ROOT_CHECK_MEM_INFO_ID))
    	{
        	GUI_KillTimer(msg->h_deswin, APP_ROOT_CHECK_MEM_INFO_ID);
    	}
#ifdef 	SUPPORT_TV_OUT	
		__release_tvout_detect_pin();
#endif
		
        esMEMS_Bfree(root_para, sizeof(root_para_t));
        esMEMS_Bfree(root_ctrl, sizeof(root_ctrl_t));

        __root_scene_sem_deinit();
    }
    return EPDK_OK;
	case GUI_MSG_TIMER:
	{
#ifdef SUPPORT_TV_OUT		
		static __u8 tvout_prv_status = -1, tvout_next_status = -1;

		if(LOWORD(msg->dwAddData1) == APP_ROOT_CHECK_MEM_INFO_ID)
		{
			//esMEMS_Info();
			tvout_next_status = dsk_query_tvout_detect_pin();

			__msg("tvout_next_status = %d,tvout_prv_status = %d\n", tvout_next_status, tvout_prv_status);
			if(tvout_next_status != tvout_prv_status)
			{
				if(0 == tvout_next_status)
				{
					dsk_display_lcd_off();
				}
				else
				{
					dsk_display_lcd_on();
				}
			}
			
			tvout_prv_status = tvout_next_status;
		}
#endif	
		return EPDK_OK;
	}
    case GUI_MSG_COMMAND:
    {
        app_root_command_proc(msg);

        return EPDK_OK;
    }
    case GUI_MSG_KEY:
    {
        __bool audio_engine_is_open;
        root_para_t *root_para;
        root_ctrl_t   *root_ctrl;

        static __u8 flag = 0; //by wanggang for test play change
        static __u8 flag1 = 0; //by wanggang for test play change
        static __u32 repeat_cnt = 0;

        root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        if (!root_ctrl)
        {
            __msg("******err****\n");
            return EPDK_OK;
        }

        //__msg("GUI_MSG_KEY, msg->dwAddData1 = 0x%x\n", msg->dwAddData1);
				
		if((GUI_MSG_KEY_SEETING == msg->dwAddData1)&&(KEY_UP_ACTION == msg->dwAddData2))
		{
			if(!root_ctrl->h_app_dv)
			{
				app_root_enter_setting(msg);
				return EPDK_OK;
			}
		}
		
        break;
    }
    case GUI_MSG_TOUCH:
    {
        __msg("app_root_win_proc GUI_MSG_TOUCH\n");
        break;
    }
    case DSK_MSG_FS_PART_PLUGOUT:
    {
        root_ctrl_t   *root_ctrl;
        H_WIN root, child, next_child;
        char  winname[256];

        eLIBs_printf("app_root_win_proc, DSK_MSG_FS_PART_PLUGOUT\n");
        if(('D' == msg->dwAddData2) && ('E' == msg->dwAddData2) && ('Z' == msg->dwAddData2))
        {
            __msg("internal disk, not need kill apps, msg->dwAddData1=%d\n", msg->dwAddData2);
            __root_scene_sem_post();
            return EPDK_OK;
        }

        eLIBs_printf("app_root_broadcast_msg, msg->dwAddData2=%d\n", msg->dwAddData2);
        __app_root_broadcast_msg(msg);

        __app_do_async(msg->h_deswin, APP_ROOT_MSG_DO_ASYNC, rat_clear_all_list_by_root, (void *)(msg->dwAddData2));

        return EPDK_OK;
    }
    case DSK_MSG_FS_PART_PLUGIN:
    {
        root_ctrl_t   *root_ctrl;
        root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

        eLIBs_printf("app_root_win_proc, DSK_MSG_FS_PART_PLUGIN\n");
        if(('D' == msg->dwAddData1) && ('E' == msg->dwAddData1) && ('Z' == msg->dwAddData1))
        {
            __msg("internal disk, not need notify home app, msg->dwAddData1=%d\n", msg->dwAddData1);
            return EPDK_OK;
        }

        eLIBs_printf("app_root_broadcast_msg, msg->dwAddData1=%d\n", msg->dwAddData1);
        __app_root_broadcast_msg(msg);

        return EPDK_OK;
    }
    case DSK_MSG_VOLTAGE_CHANGE:
    {
        __msg("app_root_broadcast_msg, msg->dwAddData1=%d\n", msg->dwAddData1);
        __app_root_broadcast_msg(msg);
        return EPDK_OK;
    }
    case DSK_MSG_TVDAC_PLUGOUT:
    case DSK_MSG_HDMI_PLUGOUT:
    {
        __msg("__app_root_broadcast_msg, msg->dwAddData1=%d\n", msg->dwAddData1);
        __app_root_broadcast_msg(msg);
        return EPDK_OK;
    }
    case APP_ROOT_MSG_DO_ASYNC:
    {
        p_app_root_fun pfun;

        __msg("msg->dwAddData1=%x, msg->dwAddData2=%x\n",
              msg->dwAddData1, msg->dwAddData2);

        if(!msg->dwAddData1)
        {
            return EPDK_OK;
        }

        pfun = (p_app_root_fun)(msg->dwAddData1);
        __msg("pfun=%x, msg->dwAddData2=%x\n", pfun, msg->dwAddData2);
        __here__;
        pfun((void *)(msg->dwAddData2));
        __here__;
        return EPDK_OK;
    }
    }

    return GUI_ManWinDefaultProc(msg);
}

/* 创建根窗口 */
H_WIN app_root_wincreate(Activity *activity)
{
    H_WIN  						hManWin;
    __gui_manwincreate_para_t  	create_info;

    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));

    create_info.attr            = activity;
    create_info.hParent			= activity_get_root_win(activity) ;
    create_info.hHosting 		= NULL;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)app_root_win_proc);
    create_info.name            = APP_ROOT;
    hManWin 					= GUI_ManWinCreate(&create_info);

    __msg("root win = %x\n", hManWin);

    if(hManWin == NULL)
    {
        __wrn("home: create movie windows failed!\n");
        return NULL;
    }

    return hManWin;
}
