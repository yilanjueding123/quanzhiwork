/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    		: init.c
* By      		: Andy.zhang
* Func		: init thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "mod_init_i.h"
#include "ui\\init_server.h"
#include "ui\\tp_adjust_scene\\scene_adjust.h"

#define XML_PATH	BEETLES_APP_ROOT"apps\\desktop"

static H_WIN init_mainwin;

/* DBMS module path */
#define	 DBMS_MOD_PATH	BEETLES_APP_ROOT"mod\\dbms.mod"


static int 		ModDBMSID;
static __mp    *ModDBMS;


static __s32 _install_dbms_mod(void)
{
    /* install dbms module */
    ModDBMSID = esMODS_MInstall(DBMS_MOD_PATH, 0);
    if(ModDBMSID == 0)
    {
        __wrn("Install dbms module failed");
        return EPDK_FAIL;
    }
    __inf("Install dbms module succeeded\n");

    /* open dbms module */
    ModDBMS = esMODS_MOpen(ModDBMSID, 0);
    if(ModDBMS == (__mp *)0)
    {
        __wrn("Open dbms module failed");
        return EPDK_FAIL;
    }
    __inf("Open dbms module succeeded\n");

    return EPDK_OK;
}


static void _uninstall_dbms_mod(void)
{
    /* uninstall dbms module    */
    if (ModDBMS)
    {
        esMODS_MClose(ModDBMS);
    }
    if (ModDBMSID)
    {
        esMODS_MUninstall(ModDBMSID);
    }
    ModDBMS   = NULL;
    ModDBMSID = 0;
}
static void _close_audio_dev(void)
{
    ES_FILE	  *p_dac;

    p_dac = eLIBs_fopen("b:\\AUDIO\\CTRL", "r+");
    if(!p_dac)
    {
        __err("open dac device fail!\n");
    }

    eLIBs_fioctrl(p_dac, AUDIO_DEV_CMD_CLOSE_DEV, AUDIO_DEV_PLAY, 0);
    eLIBs_fclose(p_dac);
}

static void _close_lcd(void)
{
    ES_FILE *disp;
    __u32 outputtype;
    __u32 	arg[3];

    disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if(!disp)
    {
        __wrn("open display fail\n");
        return ;
    }

    outputtype = eLIBs_fioctrl(disp, DISP_CMD_GET_OUTPUT_TYPE, 0, (void *)arg);
    if(outputtype == DISP_OUTPUT_TYPE_LCD)
    {
        arg[0] = 0;
        arg[1] = 0;
        arg[2] = 0;
        eLIBs_fioctrl(disp, DISP_CMD_LCD_OFF, 0, (void *)arg);
    }

    eLIBs_fclose(disp);
}

static void _framework_init(void)
{
    AppletInfoManager	*info_manager = NULL;
    ActivityManager 	*activity_manager = NULL;

    info_manager = applet_info_manager_create();
    applet_info_manager_load_dir(info_manager, XML_PATH);
    //applet_info_manager_printf(info_manager);		// 打印applet info 信息
    applet_info_manager_set_defualt(info_manager);

    activity_manager = activity_manager_create();
    activity_manager_set_default(activity_manager);
}

static void _framework_exit(void)
{
    AppletInfoManager 	*info_manager = applet_info_manager_get_default();
    ActivityManager 	*activity_manager = activity_manager_get_default();

    activity_manager_destroy(activity_manager);
    applet_info_manager_destroy(info_manager);

    applet_info_manager_set_defualt(NULL);
    activity_manager_set_default(NULL);
}

__s32 key_hook_cb(__gui_msg_t *msg)
{
    __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(init_mainwin);

    init_lock(msg);
    if( !init_ctr->key_msg_enble )				// 禁止按键
    {
        init_unlock(msg);
        return -1;
    }

    if( init_ctr->screen_close_done)// 已经关屏
    {
        init_unlock(msg);
        if(/*GUI_MSG_KEY_ENTER == msg->dwAddData1 && */(KEY_UP_ACTION == msg->dwAddData2) && (!(msg->dwAddData1 >= GUI_MASG_SINGLE0) && (msg->dwAddData1 <= GUI_MASG_SINGLE4)))
        {
            __here__;
            NOTIFY_MSG(DSK_MSG_SCREEN_OPEN, NULL, msg->h_deswin, 0, 0);
        }
        __here__;
        return 0;
    }

    if( init_ctr->screen_close_done )
    {
        init_unlock(msg);
        return -1;
    }

    init_unlock(msg);
    esPWM_UsrEventNotify();
    __msg("msg->h_deswin=%x msg->id=%d msg->dwAddData1=%d msg->dwAddData2=%d\n"
          , msg->h_deswin, msg->id, msg->dwAddData1, msg->dwAddData2);

    GUI_PostMessage(msg);

    {
        //按键enter键down和up键同时到达，导致消息丢失，之间加一个延时
        if(GUI_MSG_KEY_ENTER == msg->dwAddData1
                && KEY_DOWN_ACTION == msg->dwAddData2)
        {
            esKRNL_TimeDly(10);
        }
    }

    return 0;
}

/* tp msg hook */
__s32 tp_hook_cb(__gui_msg_t *msg)
{
    __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(init_mainwin);

    init_lock(msg);
    if( !init_ctr->tp_msg_enble )			// 禁止 touch
    {

        init_unlock(msg);
        return -1;
    }
    if( init_ctr->screen_close_done )
    {

        init_unlock(msg);
        return -1;
    }

    init_unlock(msg);
    esPWM_UsrEventNotify();
    //__msg("msg->h_deswin = %x, msg->id = %d\n", msg->h_deswin, msg->id);
    __here__;
    GUI_PostMessage(msg);

    return 0;
}

/* desktop msg hook */
__s32 init_hook_cb(__gui_msg_t *msg)
{
    __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(init_mainwin);

    /* 请求开屏 */
    if( init_ctr->screen_close_done )
    {

        if(init_ctr->power_off == 0)
            NOTIFY_MSG(DSK_MSG_SCREEN_OPEN, NULL, msg->h_deswin, 0, 0);
    }
    /*
    eLIBs_printf("init_hook_cb msg->id=%d  DSK_MSG_APP_EXIT=%d\n",msg->dwAddData1,DSK_MSG_APP_EXIT);
    if(msg->id == DSK_MSG_APP_EXIT)
    {
    	while(1)
    	{
    		esKRNL_TimeDly(10);
    	}
    }
    */
    __msg("msg->h_deswin=%x msg->id=%d msg->dwAddData1=%d msg->dwAddData2=%d\n"
          , msg->h_deswin, msg->id, msg->dwAddData1, msg->dwAddData2);
    GUI_PostMessage(msg);
    //GUI_SendNotifyMessage(msg);
    __here__;
    return 0;
}

static void _process_init(void)
{
    __memit_hook_t 	emit_hook;
    H_WIN			scene_adjust;

    /* orchid */
    //	dsk_orchid_createDisk();
    //	dsk_orchid_check();

    /* framework init */

    _framework_init();

    if(0)
    {

        reg_system_para_t *para;
        para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
        if(!para)
        {
            __msg("init get reg para fail............\n");
        }
        else
        {
            __msg("para->tp_adjust_finish_flag=%d\n", para->tp_adjust_finish_flag);
            if(para->tp_adjust_finish_flag == 0)
            {
                H_WIN	scene_adjust;
                __here__;
                scene_adjust = scene_adjust_create(init_mainwin);
                scene_adjust_destroy(scene_adjust);
                para->tp_adjust_finish_flag = 1;
            }
        }
    }
    /* load main*/
#ifndef  BOOT_FROM_SDMMC	   //卡量产	
    activity_load_app("application://app_root");
#endif
    //__wait__;
    /* messge server init*/
    msg_emit_init();

    emit_hook.key_hook 			= key_hook_cb;
    emit_hook.tp_hook 	   		= tp_hook_cb;
    emit_hook.init_hook 		= init_hook_cb;

    msg_emit_register_hook(&emit_hook);
}

static void _process_exit(void)
{
    bookengine_close();
    _uninstall_dbms_mod();

    //卸载USB
    //	App_RmmodUsbHostDriver(1);
    //	App_RmmodUsbHostDriver(2);

    /* deinit message server */
    msg_emit_deinit();

    /* framework deinit */
    _framework_exit();

    /* close device */
    _close_audio_dev();
    _close_lcd();

    /* plug out driver  */
    //	esDEV_Plugout("\\drv\\mp.drv", 0);
    //	esDEV_Plugout("\\drv\\touchpanel.drv", 0);
    //	esDEV_Plugout("\\drv\\orchid.drv",0);
    //	esDEV_Plugout("\\drv\\audio.drv", 0 );
    //    esDEV_Plugout("\\drv\\display.drv", 0 );
    //	esDEV_Plugout("\\drv\\ir.drv", 0 );

    //	App_WaitForUsbHostReomveComplete(1);
    //	App_WaitForUsbHostReomveComplete(2);

}

void application_init_process(void *arg)
{
    __gui_msg_t 	msg;
    __s32			ret = EPDK_FAIL;

    //eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
   // eLIBs_printf("&           application_init_process enter!          		       &\n");
    //eLIBs_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");

    //__wait__;


    /* drvier plugin */
    //	App_InsmodUsbHostDriver(1);
    //	App_InsmodUsbHostDriver(2);
    //	esDEV_Plugin("\\drv\\ir.drv", 0, 0, 1);
#ifndef  BOOT_FROM_SDMMC//卡量产	

    //    eLIBs_printf("before audio plugin\n");
    //	esDEV_Plugin("\\drv\\audio.drv", 0, 0, 1);
    //    eLIBs_printf("after audio plugin\n");
    //	esDEV_Plugin("\\drv\\orchid.drv", 0, 0, 1);
    //	esDEV_Plugin("\\drv\\touchpanel.drv", 0, 0, 1);
    //	esDEV_Plugin("\\drv\\mp.drv", 0, 0, 1);
    //	bookengine_open();
    //	_install_dbms_mod();
    //esDEV_Plugin("\\drv\\key.drv", 0, 0, 1);
    //esKSRV_SysInfo();
    /* create main win*/

    //__wait__;
#endif
    init_mainwin = init_mainwin_create();
    GUI_SetActiveManWin(init_mainwin);
    __msg("init_mainwin = %x\n", init_mainwin);
    /* set message loop win */
    message_loop_win_set_default(init_mainwin);

    /* drv or mod plugin */
    __here__;
    _process_init();
#ifdef  BOOT_FROM_SDMMC	   //卡量产
    DebugPrintf("start to run firmware update .........\n");
    {
        __s32 ret;
        __u32 card_flag;
        esDEV_Plugin("\\drv\\spi.drv", 0, 0, 1);

        esKRNL_TimeDly(2);
        GUI_LyrWinCacheOn();
        GUI_LyrWinCacheOff();
        card_flag = 1;
        ret = esDEV_Plugin("\\drv\\spinor.drv", 0, (void *)card_flag, 1);
        __msg("ret = %d --------------------------\n", ret);

        dsk_start_fw_update("d:\\ePDKv100.img");
    }
    __here__;
    //while(1)
    //	esKRNL_TimeDly();
#endif
    //dsk_reg_flush();

#ifndef  BOOT_FROM_SDMMC	   //卡量产
    /*enable device monitor */
    if(1)
    {
        ES_FILE     *pHwsc;

        pHwsc = eLIBs_fopen("b:\\HWSC\\hwsc", "rb+");
        if(pHwsc)
        {
            eLIBs_fioctrl(pHwsc, DEV_IOC_USR_HWSC_ENABLE_MONITOR, 0, NULL);
            eLIBs_fclose(pHwsc);
            eLIBs_printf(".start device monitor .......................................\n");
        }
        else
        {
            __err("try to open b:\\HWSC\\hwsc failed!\n");
        }
    }
#endif
    __msg("113\n");
    /* message loop*/
    while( GUI_GetMessageEx(&msg, init_mainwin) )	// 消息循环
    {
        //__msg("msg.h_deswin=%x, msg.id=%d, msg.dwAddData1=%d,msg.dwAddData2=%d\n"
        //    , msg.h_deswin, msg.id, msg.dwAddData1, msg.dwAddData2);
        ret = GUI_DispatchMessage(&msg);		// 分发消息到回调
        if( msg.p_arg )							// 同步消息回应
        {
            GUI_SetSyncMsgRetVal(&msg, ret);
            GUI_PostSyncSem(&msg);
        }
        // __here__;
    }
    GUI_WinThreadCleanup(init_mainwin);

    message_loop_win_set_default(NULL);

    /* unstall mod and driver */
    _process_exit();


    __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    __inf("&           application_init_process exit!                          &\n");
    __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");


    /* 关闭电源 */
    esKSRV_PowerOff();
    __msg("end of application_init_process\n");
}


