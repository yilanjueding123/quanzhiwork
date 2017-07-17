/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: common.c
* By      	:
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#include "beetles_app.h"

static __krnl_event_t *g_movie_robin_sem = NULL;

__s32 __movie_robin_sem_init(void)
{
    if (0 == g_movie_robin_sem)
    {
        g_movie_robin_sem = esKRNL_SemCreate(1);
    }

    if(g_movie_robin_sem)
    {
        return EPDK_OK;
    }
    else
    {
        return EPDK_FAIL;
    }
}

__s32 __movie_robin_sem_deinit(void)
{
    if (g_movie_robin_sem)
    {
        __u8 err;

        esKRNL_SemDel(g_movie_robin_sem, OS_DEL_ALWAYS, &err);
        g_movie_robin_sem = NULL;
    }

    return EPDK_OK;
}

__s32 __movie_robin_pend(void)
{
    __u8 err;

    esKRNL_SemPend(g_movie_robin_sem, 0, &err);

    return 0;
}

__s32 __movie_robin_accept(void)
{
    __u16 sem_nr;

    sem_nr = esKRNL_SemAccept(g_movie_robin_sem);
    return sem_nr;
}

__s32 __movie_robin_post(void)
{
    esKRNL_SemPost(g_movie_robin_sem);
    return 0;
}

__s32  movie_cmd2parent(H_WIN hwin, __s32 id, __s32 data2, __s32 reserved)
{
    H_WIN hparent;
    __gui_msg_t msg;

    hparent = GUI_WinGetParent(hwin);
    if (!hparent)
    {
        __err("hparent is null...\n");
        return EPDK_FAIL;
    }

    msg.h_deswin = hparent;
    msg.h_srcwin = hwin;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data2;
    msg.dwReserved = reserved;

    GUI_SendNotifyMessage(&msg);

    return EPDK_OK;
}

__s32  movie_cmd2parent_syn(H_WIN hwin, __s32 id, __s32 data2, __s32 reserved)
{
    H_WIN hparent;
    __gui_msg_t msg;

    hparent = GUI_WinGetParent(hwin);
    if (!hparent)
    {
        __err("hparent is null...\n");
        return EPDK_FAIL;
    }

    msg.h_deswin = hparent;
    msg.h_srcwin = hwin;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data2;
    msg.dwReserved = reserved;

    GUI_SendMessage(&msg);

    return EPDK_OK;
}

//等待cedar为play的状态，如果期间发生了错误，或者超时则清掉错误并返回EPDK_FAIL
//如果成功则返回EDPK_OK；
__s32 movie_wait_for_in_cedar_play_sta(void)
{
    __s32 ret;
    __s32 delay_cnt;
    __cedar_status_t cedar_sta;

    //播放文件已经是同步操作了，无需做此操作
    return EPDK_OK;//112358

    delay_cnt = 0;
    while(1)
    {
        __here__;
        ret = robin_get_feedback_msg();
        if (ret < 0)//错误退出
        {
            //robin_clear_feedback_msgQ();//不要清空，否则检测定时器例程检测不到
            return EPDK_FAIL;
        }

        __here__;
        cedar_sta = robin_get_fsm_status();
        __here__;
        if(CEDAR_STAT_PLAY == cedar_sta
                || CEDAR_STAT_PAUSE == cedar_sta
                || CEDAR_STAT_FF == cedar_sta
                || CEDAR_STAT_RR == cedar_sta)
        {
            return EPDK_OK;
        }

        delay_cnt++;
        if(delay_cnt > 20)
        {
            break;
        }

        __here__;
        esKRNL_TimeDly(20);
    }

    return EPDK_FAIL;
}


