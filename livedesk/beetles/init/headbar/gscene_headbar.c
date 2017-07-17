/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: gscene_headbar.c
* By      	: Andy.zhang
* Func		: headbar manage
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "mod_init_i.h"
#include "headbar_uipara.h"
#include "headbar_com.h"

/**********************************************************************************************************************/
#define ID_SHBAR		4000
#define ID_VOLUME_COM		1001

/**********************************************************************************************************************/
H_WIN headbar_win = NULL;


typedef struct
{
    H_LYR			hbar_lyr;
    H_WIN 			hbar_frm;
} __shbar_ctrl_t;

static H_LYR __hbar_layer_create(__hbar_format_t format)
{
    int width;
    int height;
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                   		/* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},    /* fmt       */
    };

    __disp_layer_para_t lstlyr =
    {
        DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
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
        "headbar layer",
        NULL,
        GUI_LYRWIN_STA_SLEEP,
        GUI_LYRWIN_NORMAL
    };

    dsk_display_get_size(&width, &height);
    if( format == HBAR_FOARMAT_8BPP && width == 1920 && height == 1080)
    {
        fb.fmt.fmt.rgb.pixelfmt = PIXEL_MONO_8BPP;
        lstlyr.mode				= DISP_LAYER_WORK_MODE_PALETTE;
    }

    fb.size.width			= headbar_uipara.hbar_lyr.fb_w;
    fb.size.height			= headbar_uipara.hbar_lyr.fb_h;

    lstlyr.src_win.x  		= headbar_uipara.hbar_lyr.src_x;
    lstlyr.src_win.y  		= headbar_uipara.hbar_lyr.src_y;
    lstlyr.src_win.width 	= headbar_uipara.hbar_lyr.src_w;
    lstlyr.src_win.height 	= headbar_uipara.hbar_lyr.src_h;

    lstlyr.scn_win.x		= headbar_uipara.hbar_lyr.scn_x;
    lstlyr.scn_win.y		= headbar_uipara.hbar_lyr.scn_y;
    lstlyr.scn_win.width  	= headbar_uipara.hbar_lyr.scn_w;
    lstlyr.scn_win.height 	= headbar_uipara.hbar_lyr.scn_h;

    lstlyr.pipe 			= headbar_uipara.hbar_lyr.pipe;
    lstlyr.fb 				= &fb;
    lyrcreate_info.lyrpara 	= &lstlyr;

    layer = GUI_LyrWinCreate(&lyrcreate_info);
    if( !layer )
    {
        __err("headbar layer create error !\n");
    }

    return layer;
}

static __s32 cb_shbar_mwin(__gui_msg_t *msg)
{
    switch( msg->id )
    {
    case GUI_MSG_CREATE:
    {
        __shbar_ctrl_t	*scene;
        __hbar_format_t format;

        format = (__hbar_format_t)GUI_WinGetAttr(msg->h_deswin);

        scene = (__shbar_ctrl_t *)esMEMS_Malloc(0, sizeof(__shbar_ctrl_t));
        if( !scene )
        {
            __err(" __shbar_ctrl_t malloc error \n");
            return -1;
        }
        eLIBs_memset(scene, 0, sizeof(__shbar_ctrl_t));
        GUI_WinSetAddData(msg->h_deswin, (__u32)scene);

        if(!FONT16)
        {
            FONT16 = GUI_SFT_CreateFont(16, BEETLES_APP_ROOT"res\\fonts\\font16.sft" );
        }

        headbar_init_uipara(format);

        /*  创建 headbar frmwin */
        scene->hbar_lyr		= __hbar_layer_create(format);
        __msg("scene->hbar_lyr = %x\n", scene->hbar_lyr);
        scene->hbar_frm 	= headbar_frm_create(msg->h_deswin, scene->hbar_lyr);

    }
    return 0;

    case GUI_MSG_DESTROY:
    {
        __shbar_ctrl_t	*scene;
        GUI_FONT    *DefaultFont;

        scene = (__shbar_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

        //release font
        DefaultFont = GUI_GetDefaultFont();
        GUI_SetFont(DefaultFont);
        if(FONT16)
        {
            GUI_SFT_ReleaseFont(FONT16);
            FONT16 = NULL;
        }

        GUI_LyrWinDelete(scene->hbar_lyr);
        esMEMS_Mfree(0, scene);
    }
    return 0;

    case GUI_MSG_CLOSE:
        GUI_ManWinDelete(msg->h_deswin);
        return 0;

    /*case GUI_MSG_COMMAND:
    {
    	__shbar_ctrl_t	*scene;

    	scene = (__shbar_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    	switch(LOSWORD(msg->dwAddData1))
    	{
    		case ID_HEADBAR_COM:
    		{
    			switch( HISWORD(msg->dwAddData1) )
    			{
    				case HBAR_ADJUST_VOLUME:
    				{
    					if( NULL == scene->volume_frm )
    					{
    						RECT rect;

    						GUI_LyrWinCacheOn();
    						GUI_LyrWinSetTop(scene->shpara.layer);
    						GUI_LyrWinGetSrcWindow(scene->shpara.layer,&rect);
    						rect.height = headbar_uipara.headbar.h + headbar_uipara.volumebar.h;
    						GUI_LyrWinSetSrcWindow(scene->shpara.layer,&rect);

    						GUI_LyrWinGetScnWindow(scene->shpara.layer,&rect);
    						rect.height = headbar_uipara.headbar.h + headbar_uipara.volumebar.h;
    						GUI_LyrWinSetScnWindow(scene->shpara.layer,&rect);
    						GUI_LyrWinCacheOff();

    						scene->volume_frm = volume_frm_create(&(scene->vpara));
    					}
    					else
    					{
    						RECT rect;

    						GUI_LyrWinCacheOn();
    						GUI_LyrWinSetTop(scene->shpara.layer);
    						GUI_LyrWinGetSrcWindow(scene->shpara.layer,&rect);
    						rect.height = headbar_uipara.headbar.h;
    						GUI_LyrWinSetSrcWindow(scene->shpara.layer,&rect);

    						GUI_LyrWinGetScnWindow(scene->shpara.layer,&rect);
    						rect.height = headbar_uipara.headbar.h;
    						GUI_LyrWinSetScnWindow(scene->shpara.layer,&rect);
    						GUI_LyrWinCacheOff();

    						volume_frm_delete(scene->volume_frm);
    						scene->volume_frm = NULL;
    					}
    				}
    					break;
    				default:
    					break;
    			}
    		}
    			break;

    		case ID_VOLUME_COM:
    		{
    			switch( HISWORD(msg->dwAddData1) )
    			{
    				case VOLUME_COM_CHANGED:
    				{
    					__gui_msg_t tmp_msg;

    					tmp_msg.id 			= HBAR_VOLUME_PAINT;
    		        	tmp_msg.h_srcwin 	= NULL;
    		        	tmp_msg.h_deswin 	= scene->headbar_frm;
    		        	tmp_msg.dwAddData1 	= msg->dwAddData2;
    		        	tmp_msg.dwAddData2 	= 0;
    		        	tmp_msg.dwReserved 	= 0;
    		        	tmp_msg.p_arg	   	= NULL;

    		        	dsk_volume_set(msg->dwAddData2);

    		        	GUI_SendMessage(&tmp_msg);
    				}
    					break;

    				case VOLUME_COM_TIMEOUT:
    				{
    					RECT rect;

    					scene->volume_frm = NULL;

    					GUI_LyrWinCacheOn();
    					GUI_LyrWinSetTop(scene->shpara.layer);
    					GUI_LyrWinGetSrcWindow(scene->shpara.layer,&rect);
    					rect.height = headbar_uipara.headbar.h;
    					GUI_LyrWinSetSrcWindow(scene->shpara.layer,&rect);

    					GUI_LyrWinGetScnWindow(scene->shpara.layer,&rect);
    					rect.height = headbar_uipara.headbar.h;
    					GUI_LyrWinSetScnWindow(scene->shpara.layer,&rect);
    					GUI_LyrWinCacheOff();
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
    	return 0;
    }*/

    case HBAR_TITLE_PAINT:
    {
        __gui_msg_t msgex;
        __shbar_ctrl_t	*scene;

        scene = (__shbar_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
        eLIBs_memset(&msgex, 0, sizeof(__gui_msg_t));

        msgex.id 			= HBAR_TITLE_PAINT;
        msgex.h_srcwin 		= msg->h_deswin;
        msgex.h_deswin		= scene->hbar_frm;
        msgex.dwAddData1	= msg->dwAddData1;
        msgex.dwAddData2	= msg->dwAddData2;

        GUI_SendMessage(&msgex);

        return EPDK_OK;
    }

    /*case DSK_MSG_MAIN_CMD:
    {
    	__gui_msg_t 	msgex;
    	__shbar_ctrl_t	*scene;

    	scene = (__shbar_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    	eLIBs_memset(&msgex, 0, sizeof(__gui_msg_t));

    	if( msg->dwAddData1 == HEADBAR_CMD_SET_VOLUME )
    	{
    		if( scene->volume_frm )
    		{
    			msgex.id 			= DSK_MSG_MAIN_CMD;
    			msgex.h_srcwin 		= msg->h_srcwin;
    			msgex.h_deswin		= scene->volume_frm;
    			msgex.dwAddData1	= msg->dwAddData1;
    			msgex.dwAddData2	= msg->dwAddData2;

    			GUI_SendMessage(&msgex);
    		}
    	}
    	return EPDK_OK;
    }*/

    default:
        break;
    }

    return GUI_ManWinDefaultProc(msg);
}


__s32 gscene_hbar_create(H_WIN parent, __hbar_format_t format)
{
    H_WIN  						hManWin;
    __gui_manwincreate_para_t  	create_info;

    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));

    create_info.id				= ID_SHBAR;
    create_info.attr            = (void *)format;
    create_info.hParent			= parent;
    create_info.hHosting 		= NULL;
    create_info.ManWindowProc   = cb_shbar_mwin;
    create_info.name            = "headbar";
    hManWin 					= GUI_ManWinCreate(&create_info);

    if(hManWin == NULL)
    {
        __wrn("headbar: create main windows failed!\n");
        return NULL;
    }

    headbar_win = hManWin;


    return 0;
}

__s32 gscene_hbar_delete(void)
{
    if(headbar_win)
    {
        GUI_ManWinDelete(headbar_win);
        headbar_win = NULL;
    }
    return EPDK_OK;
}

__s32 gscene_hbar_set_state(HBarState state)
{
    __shbar_ctrl_t	*pscene 	= (__shbar_ctrl_t *)GUI_WinGetAddData(headbar_win);
    H_WIN			hbar_layer 	= pscene->hbar_lyr;
    switch(state)
    {
    case HBAR_ST_SHOW:
        //GUI_LyrWinCacheOn();
        GUI_LyrWinSetSta(hbar_layer, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSetTop(hbar_layer);
        //GUI_LyrWinCacheOff();
        break;
    case HBAR_ST_HIDE:
        GUI_LyrWinSetSta(hbar_layer, GUI_LYRWIN_STA_SUSPEND);
        break;
    default:
        __wrn("HBarState is invalid! \n");
        break;
    }

    return 0;
}
static __u8 tf_state = 0;
void tf_cardstate_set(__u8 state)
{
    tf_state = state;

    __log("---jh_dbg1020_cardstae:%d", tf_state);

}
__u8 tf_cardstate_get(void)
{
    return tf_state;
}

__s32 gscene_hbar_get_state(HBarState *p_state)
{
    __gui_lyrwin_sta_t 	state;
    __shbar_ctrl_t		*pscene 	= (__shbar_ctrl_t *)GUI_WinGetAddData(headbar_win);
    H_WIN 				hbar_layer 	= pscene->hbar_lyr;

    state =  GUI_LyrWinGetSta(hbar_layer);
    if( state == GUI_LYRWIN_STA_ON)
    {
        *p_state = HBAR_ST_SHOW;
    }
    else
    {
        *p_state = HBAR_ST_HIDE;
    }

    return 0;
}

/* title 为utf8编码格式字符串，len<=32 ,设置hbar title 区域字符串*/
__s32 gscene_hbar_set_title(char *title, __u32 len)
{
    __gui_msg_t msgex;

    eLIBs_memset(&msgex, 0, sizeof(__gui_msg_t));

    msgex.id 			= HBAR_TITLE_PAINT;
    msgex.h_srcwin 		= NULL;
    msgex.h_deswin		= headbar_win;
    msgex.dwAddData1	= (__u32)title;
    msgex.dwAddData2	= len;

    GUI_SendMessage(&msgex);

    return 0;
}

/*__s32 gscene_hbar_set_volume(void)
{
	__gui_msg_t msgex;

	eLIBs_memset(&msgex, 0, sizeof(__gui_msg_t));

	msgex.id 			= DSK_MSG_MAIN_CMD;
	msgex.h_srcwin 		= NULL;
	msgex.h_deswin		= headbar_win;
	msgex.dwAddData1	= HEADBAR_CMD_SET_VOLUME;
	msgex.dwAddData2	= NULL;

	GUI_SendMessage(&msgex);

	return 0;
}*/

__s32 gscene_hbar_set_format(__hbar_format_t format)
{
    __gui_msg_t msg;
    __s32 ret;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
    msg.id 			= HEADBAR_CMD_SET_FORMAT;
    msg.h_srcwin 	= NULL;
    msg.h_deswin	= GUI_WinGetHandFromName("init");
    msg.dwAddData1	= (__u32)format;
    msg.dwAddData2	= 0;

    ret = GUI_SendMessage(&msg);
    return ret;
}

__s32 gscene_hbar_get_screen_rect(RECT *rect)
{
    __shbar_ctrl_t	*p_scene;

    p_scene = (__shbar_ctrl_t *)GUI_WinGetAddData(headbar_win);
    GUI_LyrWinGetScnWindow(p_scene->hbar_lyr, rect);

    return EPDK_OK;
}


