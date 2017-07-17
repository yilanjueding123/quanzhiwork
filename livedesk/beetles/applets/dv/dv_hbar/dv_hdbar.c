#include "dv_hdbar.h"

#define DV_SCREEN_WIDTH 480
#define DV_SCREEN_HEIGHT 60

/**********************************************************************************************************************
* 图层创建接口
**********************************************************************************************************************/
static H_WIN dv_hdbar_layer_create( RECT *rect)
{
	//H_LYR layer = NULL;
	FB	fb =
	{
		{0, 0}, 										/* size 	 */
		{0, 0, 0},										/* buffer	 */
		{FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},	 /* fmt 	  */
	};
	__disp_layer_para_t lstlyr =
	{
		DISP_LAYER_WORK_MODE_NORMAL,					/* mode 	 */
		0,												/* ck_mode	 */
		0,												/* alpha_en  */
		0,												/* alpha_val */
		1,												/* pipe 	 */
		0xff,											/* prio 	 */
		{0, 0, 0, 0},									/* screen	 */
		{0, 0, 0, 0},									/* source	 */
		DISP_LAYER_OUTPUT_CHN_DE_CH1,					/* channel	 */
		NULL											/* fb		 */
	};
	__layerwincreate_para_t lyrcreate_info =
	{
		"subset menu layer",
		NULL,
		GUI_LYRWIN_STA_SUSPEND,
		GUI_LYRWIN_NORMAL
	};
	
	fb.size.width		= rect->width;
	fb.size.height		= rect->height;
	
	lstlyr.src_win.x		= 0;
	lstlyr.src_win.y		= 0;
	lstlyr.src_win.width	= rect->width;
	lstlyr.src_win.height	= rect->height;
	
	lstlyr.scn_win.x		= rect->x;
	lstlyr.scn_win.y		= rect->y;
	lstlyr.scn_win.width	= rect->width;
	lstlyr.scn_win.height	= rect->height;
	
	lstlyr.pipe = 1;
	lstlyr.fb = &fb;
	lyrcreate_info.lyrpara = &lstlyr;
	//layer = GUI_LyrWinCreate(&lyrcreate_info);
	return (GUI_LyrWinCreate(&lyrcreate_info));
}
static __s32 dv_loading_paint(H_WIN h_win)
{
	hdbar_ctrl_t *dv_ctrl;
	dv_hdbar_uipara_t *dv_uipara;
	GUI_RECT gui_rect;

	__msg("dv_loading_paint\n");
	if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(h_win)) == GUI_LYRWIN_STA_SUSPEND)
		return EPDK_FAIL;

	dv_ctrl = (hdbar_ctrl_t *)GUI_WinGetAddData(h_win);
    dv_uipara = (dv_hdbar_uipara_t *)dv_hdbar_get_uipara();
	GUI_LyrWinSel(dv_ctrl->dv_scene_para->layer);
    GUI_SetFont(dv_ctrl->dv_scene_para->font);
    GUI_UC_SetEncodeUTF8();

	if(dv_ctrl->dv_signal_htheme[0])
	{
		GUI_BMP_Draw(dv_ctrl->dv_signal_bmp[0], dv_uipara->dv_signal_rect.x, dv_uipara->dv_signal_rect.y);
	}

	if(dv_ctrl->dv_batt_htheme[0])
	{
		GUI_BMP_Draw(dv_ctrl->dv_batt_bmp[0], dv_uipara->dv_batt_rect.x, dv_uipara->dv_batt_rect.y);
	}
	
}

static __s32 _dv_hdbar_proc(__gui_msg_t *msg)
{
	switch(msg->id)
    {
    	case GUI_MSG_CREATE:
		{
			
			hdbar_ctrl_t *dv_ctrl;
			dv_hdbar_para_t *dv_para;
			int i = 0;
			dv_ctrl = (hdbar_ctrl_t *)My_Balloc(sizeof(hdbar_ctrl_t));
			if(!dv_para)
			{
				__msg("memory balloc fail.........\n");
				return EPDK_FAIL;
			}

			gscene_bgd_set_state(BGD_STATUS_SHOW);
			
			eLIBs_memset(dv_ctrl, 0, sizeof(hdbar_ctrl_t));
			dv_para = (dv_hdbar_para_t *)GUI_WinGetAttr(msg->h_deswin);

			dv_ctrl->dv_scene_para = dv_para;
			GUI_LyrWinSetSta(dv_ctrl->dv_scene_para->layer, GUI_LYRWIN_STA_ON);
        	GUI_LyrWinSetTop(dv_ctrl->dv_scene_para->layer);

			dv_ctrl->dv_signal_id[0] = ID_DV_SIGNAL_LEVEL_NONE_BMP;
            dv_ctrl->dv_signal_id[1] = ID_DV_SIGNAL_LEVEL_ONE_BMP;
			dv_ctrl->dv_signal_id[2] = ID_DV_SIGNAL_LEVEL_TWO_BMP;
            dv_ctrl->dv_signal_id[3] = ID_DV_SIGNAL_LEVEL_THREE_BMP;
			dv_ctrl->dv_signal_id[4] = ID_DV_SIGNAL_LEVEL_FOUR_BMP;
            dv_ctrl->dv_signal_id[5] = ID_DV_SIGNAL_LEVEL_FIVE_BMP;

			for(i = 0; i<6; i++)
			{
				dv_ctrl->dv_signal_htheme[i] = dsk_theme_open(dv_ctrl->dv_signal_id[i]);
			}
			for(i = 0; i<6; i++)
			{
				dv_ctrl->dv_signal_bmp[i]	= dsk_theme_hdl2buf(dv_ctrl->dv_signal_htheme[i]);
			}

			
			dv_ctrl->dv_batt_id[0] = ID_DV_SIGNAL_LEVEL_NONE_BMP;
            dv_ctrl->dv_batt_id[1] = ID_DV_SIGNAL_LEVEL_ONE_BMP;
			dv_ctrl->dv_batt_id[2] = ID_DV_SIGNAL_LEVEL_TWO_BMP;
            dv_ctrl->dv_batt_id[3] = ID_DV_SIGNAL_LEVEL_THREE_BMP;
			dv_ctrl->dv_batt_id[4] = ID_DV_SIGNAL_LEVEL_FOUR_BMP;
            dv_ctrl->dv_batt_id[5] = ID_DV_SIGNAL_LEVEL_FIVE_BMP;

			for(i = 0; i<6; i++)
			{
				dv_ctrl->dv_batt_htheme[i] = dsk_theme_open(dv_ctrl->dv_batt_id[i]);
			}
			for(i = 0; i<6; i++)
			{
				dv_ctrl->dv_batt_bmp[i]	= dsk_theme_hdl2buf(dv_ctrl->dv_batt_htheme[i]);
			}

			return EPDK_OK;
		}
		case GUI_MSG_CLOSE:
		{
			return EPDK_OK;
		}
		case GUI_MSG_DESTROY:
		{
			__s32 i = 0;
			hdbar_ctrl_t *dv_ctrl;
			dv_ctrl = (hdbar_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
			if(!dv_ctrl)
	        {
	            __wrn("dv_ctrl is null.......\n");
	            return EPDK_FAIL;
	        }
			for(i = 0; i < 6; i++)
			{
				dsk_theme_close(dv_ctrl->dv_batt_htheme[i]);
				dv_ctrl->dv_batt_htheme[i] = NULL;
			}

			for(i = 0; i < 6; i++)
			{
				dsk_theme_close(dv_ctrl->dv_signal_htheme[i]);
				dv_ctrl->dv_signal_htheme[i] = NULL;
			}
			if(dv_ctrl->dv_scene_para)
	        {
	            My_Bfree(dv_ctrl->dv_scene_para, sizeof(dv_hdbar_para_t));
	        }

	        if(dv_ctrl)
	        {
	            My_Bfree(dv_ctrl, sizeof(hdbar_ctrl_t));
	        }
			return EPDK_OK;
		}
		case GUI_MSG_PAINT:
		{
			return dv_loading_paint(msg->h_deswin);
		}
		case GUI_MSG_KEY:
		{
			__msg("dv_hdbar GUI_MSG_KEY\n");
			return EPDK_OK;
		}
		case GUI_MSG_COMMAND:
	    {
			__msg("GUI_MSG_COMMAND\n");
			return EPDK_OK;
	    }
		case GUI_MSG_TIMER:
		{
			return EPDK_OK;
		}
		default:
        	break;
	}

	return GUI_FrmWinDefaultProc(msg);
}



/*
	建立窗口
*/
static H_WIN dv_hdbar_win_create(H_WIN h_parent, dv_hdbar_para_t *para)
{
	__gui_framewincreate_para_t framewin_para;
	dv_hdbar_para_t *dv_scene_para;
    FB fb;

    __inf("****************************************************************************************\n");
    __inf("********  enter dv_sub application  **************\n");
    __inf("****************************************************************************************\n");

	GUI_LyrWinGetFB(para->layer, &fb);
	dv_scene_para = (dv_hdbar_para_t *)My_Balloc(sizeof(dv_hdbar_para_t));
    if(!dv_scene_para)
    {
        __err("memory balloc fail.........\n");
        return EPDK_FAIL;
    }

	dv_scene_para->font = para->font;
    dv_scene_para->layer = para->layer;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));

	framewin_para.name 			= APP_DV_HDBAR;
    framewin_para.dwExStyle 	= WS_EX_NONE;
    framewin_para.dwStyle 		= WS_NONE | WS_VISIBLE;
    framewin_para.spCaption 	= NULL;
    framewin_para.hOwner		= NULL;
    framewin_para.hHosting 		= h_parent;
    framewin_para.id        	= APP_DV_HABR_ID;
    framewin_para.FrameWinProc 	= (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_dv_hdbar_proc);
    framewin_para.rect.x 		= 0;
    framewin_para.rect.y 		= 0;
    framewin_para.rect.width	= fb.size.width;
    framewin_para.rect.height 	= fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red 	= 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue 	= 0;
    framewin_para.attr 			=  (void *)dv_scene_para;
    framewin_para.hLayer 		= para->layer;

	return (GUI_FrmWinCreate(&framewin_para));
}

H_WIN dv_hdbar_create(H_WIN hparent)
{
	dv_hdbar_para_t dv_scene_para;
	dv_hdbar_uipara_t *dv_uipara;
	RECT rect;
    H_WIN h_wnd;

	rect.x = 0;
    rect.y = 0;
    rect.width = DV_SCREEN_WIDTH;
    rect.height = DV_SCREEN_HEIGHT;

	dv_scene_para.layer = dv_hdbar_layer_create(&rect);
    dv_scene_para.font = SWFFont;

	h_wnd = dv_hdbar_win_create(hparent, &dv_scene_para);

	return h_wnd;
}

void  dv_hdbar_delete(H_WIN dv_win)
{
    if(dv_win != NULL)
    {
        GUI_LyrWinDelete(GUI_WinGetLyrWin(dv_win));
    }
}



