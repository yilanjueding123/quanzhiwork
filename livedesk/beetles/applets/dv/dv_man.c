/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           home app sample
*
*						        	(c) Copyright 2006-2009, China
*											 All Rights Reserved
*
* File    : home.c
* By      : lyn
* Version : V1.00
*********************************************************************************************************
*/

#include "dv_i.h"
#include "dv_uipara.h"
#include "dv_frm.h"
#include "dv_mset.h"

#define __getc()            esSIOS_getchar()
#define __putc(c)           esSIOS_putarg(c, 'c')
#define __gets(s)          	esSIOS_gets(s)

#if  1
//#define __here__            eLIBs_printf("@L%d(%s)\n", __LINE__, __FILE__);
//#define __log(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
//						     eLIBs_printf(__VA_ARGS__)									        )
#define __mymsg(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
								 eLIBs_printf(__VA_ARGS__)											)
//#define __msg(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
//									 eLIBs_printf(__VA_ARGS__)											)
#else
#define __log(...)   
#define __msg(...)
#define __mymsg(...)
#endif


typedef struct tag_dv_ctrl
{
    GUI_FONT *font;
    __s32 root_type;

    H_WIN				win_hdl ;
    H_WIN				h_child_frm ;
    H_WIN				h_child_mset ;

    dv_uipara_para_t 	*uipara ;
    dv_frmwin_para_t	*pfrm_para ;
    dv_mset_para_t		*pmset_para ;
#ifdef APP_DV_SUPOORT_BREAK
	DV_APP_CONVERT		switch_dv;
#endif								
	
} dv_ctrl_t;

static __s32 __dv_on_create(__gui_msg_t *msg)
{
    dv_ctrl_t        	*dv_ctrl;
    dv_frmwin_para_t dv_frmwin_para ;

    dv_ctrl = (dv_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    // 初始化图标资源
    Dv_Uipara_Init();

    eLIBs_memset((void *)(&dv_frmwin_para), 0, sizeof(dv_frmwin_para_t));
    dv_frmwin_para.user_data = esMEMS_Malloc(0, sizeof(reg_dv_para_t));
    eLIBs_memset((void *)(dv_frmwin_para.user_data), 0, sizeof(reg_dv_para_t));

    dv_frmwin_para.font = dv_ctrl->font ;
    dv_frmwin_para.txt_color = APP_COLOR_WHITE ;
#ifdef APP_DV_SUPOORT_BREAK
	dv_frmwin_para.switch_frm = dv_ctrl->switch_dv;
#endif	
    dv_frmwin_para.h_parent = msg->h_deswin;//From Kok
    dv_ctrl->h_child_frm = dv_frm_create(&dv_frmwin_para);
    GUI_WinSetFocusChild(dv_ctrl->h_child_frm);

    return EPDK_OK;
}

static __s32 __dv_on_destroy(__gui_msg_t *msg)
{
    dv_ctrl_t        	*dv_ctrl;

    dv_ctrl = (dv_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if( dv_ctrl->h_child_frm)
    {
        dv_frm_destroy(dv_ctrl->h_child_frm);
    }
    if( dv_ctrl->h_child_mset)
    {
        dv_mset_destroy(dv_ctrl->h_child_mset);
    }
    Dv_Uipara_UnInit();

    esMEMS_Mfree(0, dv_ctrl);

    return EPDK_OK;
}

static __s32 __dv_frm_command_proc(__gui_msg_t *msg)
{
    __gui_msg_t setmsg;
    dv_ctrl_t        	*dv_ctrl;

    dv_ctrl = (dv_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    switch(HIWORD(msg->dwAddData1))
    {
    case CMD_DV_FRM_POP_REC_SETTING_MENU:
    {
        dv_mset_para_t dv_mset_para ;
        eLIBs_memset((void *)(&dv_mset_para), 0, sizeof(dv_mset_para_t));
        dv_mset_para.dv_menu_id = DV_MENU_ID_REC_SET ;
        dv_mset_para.txt_color = APP_COLOR_WHITE ;
        dv_mset_create(&dv_mset_para);
        break;
    }
    case CMD_DV_FRM_POP_IMAGE_SETTING_MENU:
    {
        dv_mset_para_t dv_mset_para ;
        eLIBs_memset((void *)(&dv_mset_para), 0, sizeof(dv_mset_para_t));
        dv_mset_para.dv_menu_id = DV_MENU_ID_IMAGE_SET;
        dv_mset_para.txt_color = APP_COLOR_WHITE ;
        dv_mset_create(&dv_mset_para);
        break;
    }
    default:
        break;
    }

    return EPDK_OK ;
}

static __s32 __dv_mset_command_proc(__gui_msg_t *msg)
{
    __gui_msg_t setmsg;
    dv_ctrl_t        	*dv_ctrl;

    dv_ctrl = (dv_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    switch(HIWORD(msg->dwAddData1))
    {
    case CMD_DV_MSET_SET_RECORD_RESOLUTION:
    {
        __gui_msg_t new_msg ;

        new_msg.h_deswin = dv_ctrl->h_child_frm ;
        new_msg.h_srcwin = msg->h_deswin ;
        new_msg.id = CMD_DV_FRM_SET_RECORD_RESOLUTION ;
        new_msg.dwAddData1 = msg->dwAddData2 ;	// focus item index
        GUI_SendMessage(&new_msg);
        break;
    }
    case CMD_DV_MSET_EXIT:
    {
        __gui_msg_t new_msg ;
		__msg("dv_mset_destroy, CMD_DV_MSET_EXIT\n");
        if( dv_ctrl->h_child_mset)
        {
            dv_mset_destroy(dv_ctrl->h_child_mset);
            dv_ctrl->h_child_mset = NULL ;
        }
        new_msg.h_deswin = dv_ctrl->h_child_frm ;
        new_msg.h_srcwin = msg->h_deswin ;
        new_msg.id = CMD_DV_FRM_SETTING_MENU_EXIT ;
        new_msg.dwAddData1 = msg->dwAddData2 ;	// focus item index
        GUI_SendMessage(&new_msg);
        break;
    }
    default:
        break;
    }

    return EPDK_OK ;
}
static __s32 _app_dv_main_proc(__gui_msg_t *msg)
{
    dv_ctrl_t        	*dv_ctrl;

    dv_ctrl = (dv_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        __dv_on_create(msg);	
    }
    return EPDK_OK;
    case GUI_MSG_DESTROY:
    {
        __dv_on_destroy(msg);
    }
    return EPDK_OK;
    case GUI_MSG_KEY:
        break;
    case GUI_MSG_COMMAND:
    {
        if(LOWORD(msg->dwAddData1) == DV_FRM_WIN_ID)
        {
            switch(HIWORD(msg->dwAddData1))
            {
            case  ID_OP_DV_TO_HOME:
                Dv_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
                break;
            default :
                break;
            }
        }
    }
    return EPDK_OK;

    case DSK_MSG_FS_PART_PLUGIN:
    {
        //From Kok
        __gui_msg_t msg;
        __msg("dv_man DSK_MSG_FS_PART_PLUGIN\n");

        msg.id = DSK_MSG_FS_PART_PLUGIN;
        msg.h_deswin = dv_ctrl->h_child_frm;

        GUI_SendMessage(&msg);

    }
    break;
	case DSK_MSG_VOLTAGE_CHANGE:
	{
		 __gui_msg_t msg;
        __msg("dv_man DSK_MSG_FS_PART_PLUGIN\n");

        msg.id = DSK_MSG_VOLTAGE_CHANGE;
        msg.h_deswin = dv_ctrl->h_child_frm;

		GUI_SendMessage(&msg);
		
		break;
	}
    case DSK_MSG_FS_PART_PLUGOUT:
    {
        //From Kok
        __gui_msg_t msg;
        __msg("dv_man DSK_MSG_FS_PART_PLUGOUT\n");

        msg.id = DSK_MSG_FS_PART_PLUGOUT;
        msg.h_deswin = dv_ctrl->h_child_frm;

        GUI_SendMessage(&msg);
    }
    break;

    default:
        break;
    }
    return GUI_ManWinDefaultProc(msg);
}

__s32 app_dv_create(root_para_t *para)
{

    __gui_manwincreate_para_t create_info;
    dv_ctrl_t *dv_ctrl;

    __inf("=======================================================================\n");
    __inf("=============================| DV APP RUN |============================\n");
    __inf("=======================================================================\n");
	
    gscene_hbar_set_state(HBAR_ST_HIDE);
    gscene_bgd_set_state(BGD_STATUS_HIDE);

	
    dv_ctrl = (dv_ctrl_t *)esMEMS_Malloc(0, sizeof(dv_ctrl_t));
    if(dv_ctrl == NULL)
    {
        __wrn("malloc error \n");
        return NULL;
    }
    eLIBs_memset(dv_ctrl, 0, sizeof(dv_ctrl_t));
		
    dv_ctrl->font = para->font;
    dv_ctrl->root_type = para->root_type;
#ifdef APP_DV_SUPOORT_BREAK
	dv_ctrl->switch_dv = para->srch_switch;
#endif	
	__mymsg("dv_ctrl->switch_dv = %d, para->srch_switch = %d\n", dv_ctrl->switch_dv, para->srch_switch);
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));

    create_info.name            = APP_DV;
    create_info.hParent			= para->h_parent ;
    create_info.ManWindowProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_app_dv_main_proc);
    create_info.attr            = (void *)dv_ctrl;
    create_info.id			= APP_DV_ID;
    create_info.hHosting        = NULL;

    return(GUI_ManWinCreate(&create_info));

}



