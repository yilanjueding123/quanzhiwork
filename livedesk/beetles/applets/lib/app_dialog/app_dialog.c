/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: app_dialog.c
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/06/02 21:10  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#include "app_dialog_i.h"

__u8   dialog_jh_tran_data;
__u8   dialog_current_state;

#if  0
#define __inf(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
								 eLIBs_printf(__VA_ARGS__)											)

//#define __msg(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
//								 eLIBs_printf(__VA_ARGS__)											)
#else
#define __msg(...)   
#define __inf(...)
#endif


//////////////////////////////////////////////////////////////////////////
#if 0

//temp//if(!(0<=(_x) && (_x)<=400 && 0<=(_y) && (_y)<= 240))
//re define GUI_BMP_Draw
#ifdef GUI_BMP_Draw
#define MY_GUI_BMP_DRAW                      (ORANGEMOD_FUNC_ENTRY->ptr_GUI_BMP_Draw         	)
#undef GUI_BMP_Draw
#define GUI_BMP_Draw(_p, _x, _y)             do \
{\
	{__msg("    GUI_BMP_Draw(%d, %d)\n", _x, _y);} \
	MY_GUI_BMP_DRAW(_p, _x, _y); \
}while(0)
#endif

//re define GUI_DispStringAt
#ifdef GUI_DispStringAt
#define MY_GUI_DispStringAt                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringAt       	)
#undef GUI_DispStringAt
#define GUI_DispStringAt(_p, _x, _y)         \
	do\
{ \
	{__msg("    GUI_DispStringAt(%d, %d)\n", _x, _y);} \
	MY_GUI_DispStringAt(_p, _x, _y); \
}while(0)
#endif

//re define GUI_DispStringInRect
#ifdef GUI_DispStringInRect
#define MY_GUI_DispStringInRect              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringInRect   	)
#undef GUI_DispStringInRect
#define GUI_DispStringInRect(_p, _prt, _fmt)  \
	do \
{ \
	{__msg("    GUI_DispStringInRect(%d, %d, %d, %d)(%x)\n", (_prt)->x0, (_prt)->y0, (_prt)->x1, (_prt)->y1, _fmt);} \
	MY_GUI_DispStringInRect(_p, _prt, _fmt); \
} while (0)
#endif

#endif
//////////////////////////////////////////////////////////////////////////
#define BTN_Y_END_OFFSET         16//24// 8
#define BTN_X_OFFSET              8
#define TITLE_X_OFFSET            4
#define TITLE_Y_OFFSET            4
//#define TITLE_WIDTH               60
#define TITLE_HEIGHT              20
#define ICON_CONTENT_X_OFFSET     4
#define CONTENT_X_OFFSET          5
#define CONTENT_X_END_OFFSET      5//CONTENT_X_OFFSET
#define CONTENT_Y_OFFSET          8
#define CONTENT_Y_END_OFFSET      5
#define ICON_X_OFFSET             5
#define ICON_Y_OFFSET             35

#define IsDialogTimerValid(_v)    ((_v) > 0)
#define ID_TIMER_DIALOG_TIMEOUT   100

//按键间的间隔
#define calc_btn_sp_w(_rt_w, _offset, _btn_w, _count)   (((_count) > 1) ? (((_rt_w) - (_offset) * 2 - (_btn_w) * (_count)) / (_count - 1)) : 0)

//取窗口私有数据
#define app_dialog_GetWndPara(_p, _t, _hwnd)  {(_p = (_t *)GUI_WinGetAttr(_hwnd)); __msg("app_dialog para ====== %x\n", _p);}

//去图片资源宽高
#define GET_RES_BMP_W_H_(_hbmp, _res, _w, _h) do \
{\
	_hbmp = bmp_open(ResHandle2Data(_res)); \
	if (_hbmp != NULL) \
	{ \
		_w = bmp_get_width(_hbmp); \
		_h = bmp_get_height(_hbmp); \
		bmp_close(_hbmp); \
	} \
	else \
	{ \
	    _w = 0; \
        _h = 0; \
		__msg("open bmp fail\n"); \
	} \
}while(0)
//////////////////////////////////////////////////////////////////////////
//创建资源
static __s32 app_dialog_res_create(APP_DIALOG_RES_T *res, __u32 style, __s32 bmp_id[], __s32 str_id[])
{
    create_bmp_res(bmp_id[ADLG_BMP_SZID_BG], res->bmp_bg);
    create_bmp_res(bmp_id[ADLG_BMP_SZID_BTN_F], res->bmp_btn_f);
    create_bmp_res(bmp_id[ADLG_BMP_SZID_BTN_N], res->bmp_btn_n);

    create_bmp_res(bmp_id[ADLG_BMP_SZID_L_N], res->bmp_l_n);
    create_bmp_res(bmp_id[ADLG_BMP_SZID_L_Y], res->bmp_l_y);
    create_bmp_res(bmp_id[ADLG_BMP_SZID_R_N], res->bmp_r_n);
    create_bmp_res(bmp_id[ADLG_BMP_SZID_R_Y], res->bmp_r_y);
    create_bmp_res(bmp_id[ADLG_BMP_SZID_SELECT], res->bmp_select);

    create_bmp_res(bmp_id[ADLG_BMP_SZID_BTN_Y], res->bmp_btn_yes);
    create_bmp_res(bmp_id[ADLG_BMP_SZID_BTN_NO], res->bmp_btn_no);
    create_bmp_res(bmp_id[ADLG_BMP_SZID_STORAGE], res->bmp_storage);
	
#ifdef	ADLG_BKLT_UI
	create_bmp_res(ID_DIALOG_BKLT_SELECT_BMP, res->bmp_bklt_select);
	create_bmp_res(ID_DIALOG_BKLT_UNSELECT_BMP, res->bmp_bklt_unselect);
#endif

    //
    if (bmp_id[ADLG_BMP_SZID_ICON] && bmp_id[ADLG_BMP_SZID_ICON] != BMP_RES_NULL)
    {
        create_bmp_res(bmp_id[ADLG_BMP_SZID_ICON], res->bmp_icon);
        res->icon_enable = 1;
    }
    else
    {
        res->icon_enable = 0;
    }

    if (str_id[ADLG_STR_SZID_TITLE] != 0)
    {
        get_lang_res(str_id[ADLG_STR_SZID_TITLE], res->str_title);
    }
    else
    {
        get_lang_res(STRING_DIALOG_DIALOG, res->str_title);
    }
    get_lang_res(str_id[ADLG_STR_SZID_CONTENT], res->str_content);

    switch (ADLG_GET_BTN_STYLE(style))
    {
    case ADLG_OK:
    {
        get_lang_res(STRING_DIALOG_OK, res->str_btn[0]);
        res->btn_result[0] = ADLG_IDOK;
        res->btn_count = 1;
    }
    break;
    case ADLG_OKCANCEL:
    {
        get_lang_res(STRING_DIALOG_OK, res->str_btn[0]);
        get_lang_res(STRING_DIALOG_CANCEL, res->str_btn[1]);
        res->btn_result[0] = ADLG_IDOK;
        res->btn_result[1] = ADLG_IDCANCEL;
        res->btn_count = 2;
    }
    break;
    case ADLG_YESNO:
    {
        get_lang_res(STRING_DIALOG_YES, res->str_btn[0]);
        get_lang_res(STRING_DIALOG_NO, res->str_btn[1]);
        res->btn_result[0] = ADLG_IDYES;
        res->btn_result[1] = ADLG_IDNO;
        res->btn_count = 2;
    }
    break;
    case ADLG_RETRYCANCEL:
    {
        get_lang_res(STRING_DIALOG_RETRY, res->str_btn[0]);
        get_lang_res(STRING_DIALOG_CANCEL, res->str_btn[1]);
        res->btn_result[0] = ADLG_IDRETRY;
        res->btn_result[1] = ADLG_IDCANCEL;
        res->btn_count = 2;
    }
    break;
    case ADLG_ABORTRETRYIGNORE:
    {
        get_lang_res(STRING_DIALOG_ABORT, res->str_btn[0]);
        get_lang_res(STRING_DIALOG_RETRY, res->str_btn[1]);
        get_lang_res(STRING_DIALOG_IGNORE, res->str_btn[2]);
        res->btn_result[0] = ADLG_IDABORT;
        res->btn_result[1] = ADLG_IDRETRY;
        res->btn_result[2] = ADLG_IDIGNORE;
        res->btn_count = 3;
    }
    break;
    case ADLG_YESNOCANCEL:
    {
        get_lang_res(STRING_DIALOG_YES, res->str_btn[0]);
        get_lang_res(STRING_DIALOG_NO, res->str_btn[1]);
        get_lang_res(STRING_DIALOG_CANCEL, res->str_btn[2]);
        res->btn_result[0] = ADLG_IDYES;
        res->btn_result[1] = ADLG_IDNO;
        res->btn_result[2] = ADLG_IDCANCEL;
        res->btn_count = 3;
    }
    break;
    default:
        ASSERT(0);
        break;
    }

    return EPDK_OK;
}

//释放资源
static __s32 app_dialog_res_destroy(APP_DIALOG_RES_T *res)
{
    destroy_bmp_res(res->bmp_bg);
    destroy_bmp_res(res->bmp_btn_n);
    destroy_bmp_res(res->bmp_btn_f);
    destroy_bmp_res(res->bmp_l_n);
    destroy_bmp_res(res->bmp_l_y);
    destroy_bmp_res(res->bmp_r_n);
    destroy_bmp_res(res->bmp_r_y);
    destroy_bmp_res(res->bmp_select);
    destroy_bmp_res(res->bmp_btn_no);
    destroy_bmp_res(res->bmp_btn_yes);
    destroy_bmp_res(res->bmp_storage);
#ifdef	ADLG_BKLT_UI
	destroy_bmp_res(res->bmp_bklt_select);
	destroy_bmp_res(res->bmp_bklt_unselect);
#endif
	
    if (res->icon_enable)
    {
        destroy_bmp_res(res->bmp_icon);
    }

    return EPDK_OK;
}

static __s32 app_dialog_ui_init(dialog_wnd_t *wnd_para, GUI_RECT *dialog_rt)
{
    HBMP hbmp;
    __s32 lcd_w, lcd_h;
    APP_DIALOG_UI_T  *ui;
    APP_DIALOG_RES_T *res;

    ui = &wnd_para->ui;
    res = &wnd_para->res;

    ui->colour.txt_n = GUI_GRAY;
    ui->colour.txt_f = GUI_WHITE;

    dsk_display_get_size(&lcd_w, &lcd_h);
	__inf("lcd_w = %d, lcd_h = %d\n", lcd_w,lcd_h);
    if (dialog_rt == NULL)
    {
        GET_RES_BMP_W_H_(hbmp, res->bmp_bg, ui->rt.width, ui->rt.height);
		__inf("ui->rt.width = %d, ui->rt.height = %d\n", ui->rt.width, ui->rt.height);
        switch (ADLG_GET_ALIGN_H(wnd_para->style))
        {
        case ADLG_TA_LEFT:
            ui->rt.x = 0;
            break;
        case ADLG_TA_RIGHT:
            ui->rt.x = lcd_w - ui->rt.width;
            break;
        default:				//ADLG_TA_HCENTER
            ui->rt.x = (lcd_w - ui->rt.width) / 2;
            break;
        }

        switch (ADLG_GET_ALIGN_V(wnd_para->style))
        {
        case ADLG_TA_TOP:
            ui->rt.y = 0;
            break;
        case ADLG_TA_BOTTOM:
            ui->rt.y = lcd_h - ui->rt.height;
            break;
        default://ADLG_TA_VCENTER
            ui->rt.y = (lcd_h - ui->rt.height) / 2;
            break;
        }
    }
    else
    {
        //user define rect
        ui->rt.x      = dialog_rt->x0;
        ui->rt.width  = dialog_rt->x1 - dialog_rt->x0;
        ui->rt.y      = dialog_rt->y0;
        ui->rt.height = dialog_rt->y1 - dialog_rt->y0;
    }
/////////////////////////////////////////
#ifdef ADLG_BKLT_UI
	if(dialog_current_state == 0)
	{
//		ui->rt.x      = 115;
//        ui->rt.width  = 250;
//        ui->rt.y      = 44;
//        ui->rt.height = 168;
		ui->rt.x      = 160;
        ui->rt.width  = 157;
        ui->rt.y      = 44;
        ui->rt.height = 168;
	}
	else if(dialog_current_state == 1)
	{
//		ui->rt.x      = 115;
//        ui->rt.width  = 250;
//        ui->rt.y      = 32;
//        ui->rt.height = 168+50;
		ui->rt.x      = 160;
        ui->rt.width  = 157;
        ui->rt.y      = 34;
        ui->rt.height = 168+50;
	}
	
#endif
/////////////////////////////////////////
	__inf("ui->rt.x = %d, ui->rt.y = %d, ui->rt.width = %d, ui->rt.height = %d\n", ui->rt.x, ui->rt.y, ui->rt.width, ui->rt.height);
    ASSERT(res->btn_count > 0);
    GET_RES_BMP_W_H_(hbmp, res->bmp_btn_n, ui->size.btn.width, ui->size.btn.height);
    ui->size.btn_sp.height = 0;
    ui->size.btn_sp.width  = (ui->rt.width / res->btn_count - ui->size.btn.width);

    if(dialog_current_state == 3)
    {
        ui->pos.btn_start.x = ui->size.btn_sp.width;
		__inf("ui->pos.btn_start.x = %d\n", ui->pos.btn_start.x);
    }
    else
    {
        ui->pos.btn_start.x = ui->size.btn_sp.width / 2;
		__inf("ui->pos.btn_start.x = %d\n", ui->pos.btn_start.x);
    }
    ui->pos.btn_start.y = ui->rt.height - BTN_Y_END_OFFSET - ui->size.btn.height;
	__inf("ui->pos.btn_start.y = %d\n", ui->pos.btn_start.y);
    if(dialog_current_state == 6)
    {
        ui->pos.title.x       = TITLE_X_OFFSET;
    }
    else if(dialog_current_state == 5)
    {
        ui->pos.title.x    = (ui->rt.width - 120) / 2;
    }
    else if((dialog_current_state != 2))
    {
        ui->pos.title.x       = 2;
    }
    else if(dialog_current_state == 2)
    {
        ui->pos.title.x    = (ui->rt.width - 160) / 2-40;
    }
	__inf("dialog_current_state = %d,ui->pos.title.x:%d\n",dialog_current_state, ui->pos.title.x);

    if(dialog_current_state == 3)
    {
        ui->pos.title.y       = TITLE_Y_OFFSET + 12;
    }
    else if(dialog_current_state == 4)
    {
        ui->pos.title.y       = (ui->rt.height - 20) / 2;
    }
    else
    {
        ui->pos.title.y       = TITLE_Y_OFFSET;
    }
	
	__inf("dialog_current_state = %d,ui->pos.title.y:%d\n",dialog_current_state, ui->pos.title.y);
	
    ui->size.title.width  = ui->rt.width - ui->pos.title.x;
    ui->size.title.height = TITLE_HEIGHT;
	__inf("ui->size.title.width = %d,ui->size.title.height = %d\n", ui->size.title.width, ui->size.title.height);
    if (res->icon_enable)
    {
        GET_RES_BMP_W_H_(hbmp, res->bmp_icon, ui->size.icon.width, ui->size.icon.height);
        ui->pos.icon.x = ICON_X_OFFSET;
        ui->pos.icon.y = ICON_Y_OFFSET;
		__inf("ui->size.icon.width = %d, ui->size.icon.height = %d\n", ui->size.icon.width, ui->size.icon.height);
    }
    if(dialog_current_state == 6)
    {
        ui->pos.content.x       = res->icon_enable ? (ui->pos.icon.x + ui->size.icon.width + ICON_CONTENT_X_OFFSET ) : CONTENT_X_OFFSET;
    }
    else
    {
        ui->pos.content.x = (ui->rt.width - 20) / 2;
    }
	
	__inf("ui->pos.content.x = %d\n", ui->pos.content.x);
	
    ui->pos.content.y       = ui->pos.title.y + ui->size.title.height + CONTENT_Y_OFFSET;
	__inf("ui->pos.content.y = %d\n", ui->pos.content.y);
	
    if(dialog_current_state == 0)
    {
        ui->size.content.width  = 20; 
    }
    else if(dialog_current_state == 1)
    {
        ui->size.content.width = 60;
    }
    else if(dialog_current_state == 6)
    {
        ui->size.content.width  = ui->rt.width - ui->pos.content.x - CONTENT_X_END_OFFSET;
    }
    ui->size.content.height = ui->pos.btn_start.y - CONTENT_Y_END_OFFSET - ui->pos.content.y;
	
	__inf("ui->size.content.width = %d, ui->size.content.height = %d\n", ui->size.content.width, ui->size.content.height);
	
    return EPDK_OK;
}

static __s32 app_dialog_data_init(dialog_wnd_t *wnd_para, __s32 bmp_id[], __s32 str_id[], GUI_RECT *dialog_rt)
{
    app_dialog_res_create(&wnd_para->res, ADLG_GET_WND_STYLE(wnd_para->style), bmp_id, str_id);
    app_dialog_ui_init(wnd_para, dialog_rt);
    wnd_para->result = ADLG_IDM_TIMEROUT;

    return EPDK_OK;
}

static __s32 app_dialog_data_release(dialog_wnd_t *wnd_para)
{
    app_dialog_res_destroy(&wnd_para->res);
    BFreeType(wnd_para, dialog_wnd_t);
    return EPDK_OK;
}

static __s32 notify_to_close_dialog(H_WIN hwnd/*, __s32 result*/)
{
    __gui_msg_t msg;
    dialog_wnd_t *wnd_para;

    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, hwnd);

    msg.h_deswin    = wnd_para->parent;
    msg.h_srcwin    = wnd_para->hwnd;
    msg.id          = GUI_MSG_COMMAND;
    msg.dwAddData1  = MAKELONG(wnd_para->ID, ADLG_CMD_EXIT);
    msg.dwAddData2  = wnd_para->result;
    msg.dwReserved  = wnd_para->temp;//0;
    msg.p_arg       = NULL;

    __msg("%x => %x (%d, %x, %x)\n", msg.h_srcwin, msg.h_deswin, msg.id, msg.dwAddData1, msg.dwAddData2);

    return GUI_SendNotifyMessage(&msg);
}
#define     USER_DISK           "F:"

typedef struct disk_cluster_sz_S
{
    __u32	disk_sz_min ;	// unit:MByte
    __u32	disk_sz_max ;
    __u32	cluster_sz ;	// unit:Byte
} disk_cluster_sz_t;
static disk_cluster_sz_t disk_cluster_sz_array[] =
{
    {0L,	1024,	4096},			// <=1G , format to 4K
    {1024,	2048,	16384},			// >1G && <= 2G ,format to 16K
    {2048,	4096,	32768},			// >2G && <= 4G ,format to 32K
    {4096,	0xffff,	65536},			// >4G , format to 64K
};

static __s32 format_card()
{
    struct fmt_para
    {
        int cluster_size;           //cluster size
        int fs_size;                //file system size
        int tracks;                 //tracek per cylinder
        int sectors;                //sectors per tracks
        int force_umnt;             //force unmount the part if mounted
        int quick;                  //quick format
        int compress;               //set filesystem to have compress attrib
        int subtype;                //sub fs type like fat12/fat16
        int reserve[2];             //reserved space
        char label[12];             //volume label, max len is 11 bytes
    } format_para ;

    __u32 disk_size ;	// Unit :MByte
    __u32 i ;

    eLIBs_memset((void *)(&format_para), 0, sizeof(struct fmt_para));
    disk_size = (eLIBs_GetVolTSpace(USER_DISK) >> 20) ;

    for( i = 0 ; i < sizeof(disk_cluster_sz_array) / sizeof(disk_cluster_sz_array[0]) ; i++ )
    {
        if ( (disk_cluster_sz_array[i].disk_sz_min < disk_size) && (disk_size <= disk_cluster_sz_array[i].disk_sz_max ) )
        {
            format_para.cluster_size = disk_cluster_sz_array[i].cluster_sz ;
            break ;
        }
    }

    if(EPDK_OK == eLIBs_format(USER_DISK, "fat", (__hdle)(&format_para) ) )
    {
        return EPDK_OK;
    }

    return	EPDK_FAIL;
}

static __s32 on_dialog_key_up_action(H_WIN hwnd, __u32 keycode)
{
    APP_DIALOG_UI_CTRL_PAPA_T *ctrl;
    dialog_wnd_t *wnd_para;

    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, hwnd);
    ctrl = &wnd_para->ui.ctrl;

    switch (keycode)
    {
    case VK_LEFT:
    {
        if(dialog_current_state == 4)
            break;
#if 1
        if(dialog_current_state > 1)
        {
            if (0 < ctrl->focus_id)
            {
                ctrl->focus_id--;
            }
            else
            {
                ctrl->focus_id = wnd_para->res.btn_count - 1;
            }
        }
#endif
        if(dialog_jh_tran_data > 0)
        {
            dialog_jh_tran_data --;
        }
        if(dialog_current_state == 0)
        {
            dsk_display_set_lcd_bright((__lion_bright_t)(3 * (4 - dialog_jh_tran_data) + 2));
        }
        GUI_WinUpdate(hwnd, EPDK_TRUE);
    }
    break;
    case VK_RIGHT:
    {
        if(dialog_current_state == 4)
            break;
#if 1
        if(dialog_current_state > 1)
        {
            if (ctrl->focus_id < wnd_para->res.btn_count - 1)
            {
                ctrl->focus_id++;
            }
            else
            {
                ctrl->focus_id = 0;
            }
        }
#endif
        if(dialog_current_state == 0)
        {
            if(dialog_jh_tran_data < 4)
            {
                dialog_jh_tran_data++;
            }
            dsk_display_set_lcd_bright((__lion_bright_t)(3 * (4 - dialog_jh_tran_data) + 2));
        }
        else if(dialog_current_state == 1)
        {
            if(dialog_jh_tran_data < 6)
            {
                dialog_jh_tran_data++;
            }
        }
        GUI_WinUpdate(hwnd, EPDK_TRUE);
    }
    break;
    case VK_ENTER:
    {
        if(dialog_current_state == 4)
        {       	
            break;
        }
        ASSERT(0 <= ctrl->focus_id && ctrl->focus_id < wnd_para->res.btn_count);

        wnd_para->result = ADLG_IDYES;
        if(dialog_current_state > 1)
        {
            wnd_para->result = wnd_para->res.btn_result[ctrl->focus_id];
        }
		//__inf("dialog_current_state = %d\n", dialog_current_state);
        if(dialog_current_state == 3)
        {
            if(wnd_para->result == ADLG_IDYES)
            {
                __lion_bright_t  da_back_light;
				GUI_RECT	gui_rect;
				
				GUI_LyrWinSel(wnd_para->hlyr);
				GUI_SetFont(SWFFont);
                GUI_SetColor(wnd_para->ui.colour.txt_f);
				GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
				GUI_UC_SetEncodeUTF8();
                get_lang_res(STRING_DIALOG_DET, wnd_para->res.str_content);

				__inf("Format card, %s\n", wnd_para->res.str_content);
				gui_rect.x0 = 2;
				gui_rect.y0 = 1+10;
				gui_rect.x1 = gui_rect.x0 + 100;
				gui_rect.y1 = gui_rect.y0 + 16;
				GUI_ClearRectEx(&gui_rect);
                GUI_DispStringAt(wnd_para->res.str_content, gui_rect.x0, gui_rect.y0);
                da_back_light = dsk_display_get_lcd_bright();
                dsk_display_set_lcd_bright((__lion_bright_t)(14));
                format_card();
                dsk_display_set_lcd_bright((__lion_bright_t)(da_back_light));
            }
        }
		else if(dialog_current_state == 5)
		{
			if(wnd_para->result == ADLG_IDYES)
            {
                __lion_bright_t  da_back_light;
				GUI_RECT	gui_rect;
				
				GUI_LyrWinSel(wnd_para->hlyr);
				GUI_SetFont(SWFFont);
                GUI_SetColor(wnd_para->ui.colour.txt_f);
				GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
				GUI_UC_SetEncodeUTF8();
                get_lang_res(STRING_EXPLR_DELETE_HINT, wnd_para->res.str_content);

				__inf("delete file, %s\n", wnd_para->res.str_content);
				gui_rect.x0 = 20;
				gui_rect.y0 = 1;
				gui_rect.x1 = gui_rect.x0 + 150;
				gui_rect.y1 = gui_rect.y0 + 16;
				GUI_ClearRectEx(&gui_rect);
                GUI_DispStringAt(wnd_para->res.str_content, gui_rect.x0, gui_rect.y0);
            }
		}

        wnd_para->temp = 0;
        if(dialog_current_state == 1)
        {
            wnd_para->temp	= dialog_jh_tran_data;
        }
        notify_to_close_dialog(hwnd);
    }
    break;
    case VK_ESCAPE:
    case GUI_MSG_KEY_ESCAPE:
    {
        wnd_para->result = ADLG_IDNO;
        notify_to_close_dialog(hwnd);
    }
    break;
    }

    return EPDK_OK;
}

//
static __s32 dialog_key_proc(__gui_msg_t *msg)
{
    static __u32 lastkey = VK_NULL;

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        on_dialog_key_up_action(msg->h_deswin, lastkey);
        lastkey = VK_NULL;
    }
    else
    {
        lastkey = msg->dwAddData1;
    }

    {
        //有按键，重新计时
        dialog_wnd_t *wnd_para;
        app_dialog_GetWndPara(wnd_para, dialog_wnd_t, msg->h_deswin);
        if (IsDialogTimerValid(wnd_para->timeout))
        {
            GUI_ResetTimer(msg->h_deswin, ID_TIMER_DIALOG_TIMEOUT, wnd_para->timeout, NULL);
        }
    }

    return EPDK_OK;
}

//
static __s32 on_dialog_command(__gui_msg_t *msg)
{
    return EPDK_OK;
}

/************************************************************
 *dialog_current_state:
 *value: 0-->//背光亮度; 1-->//背光时间; 2-->//恢复出厂设置
 *3-->//卡格式化; 4-->//无卡; 5-->//从非设置界面来的; 6-->other
 ************************************************************/
#ifdef ADLG_BKLT_UI
/*char *pbuff_str[7] = {
	"OFF",
	"5 seconds",
	"10 seconds",
	"20 seconds",
	"30 seconds",
	"1 minutes",
	"5 minutes",
};
*/
char *pbuff_str[7] = {
	"OFF",
	"5 s",
	"10 s",
	"20 s",
	"30 s",
	"1 m",
	"5 m",
};

static void draw_dialog(APP_DIALOG_RES_T *res, APP_DIALOG_UI_T *ui)
{
	__s32			  i, x, y;
	void			 *bmp_data;
	GUI_RECT		  rect;

	//GUI_SetColor(ui->colour.txt_n);

	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	//背景

	__inf("dialog_current_state = %d\n", dialog_current_state);

	if(dialog_current_state == 0)
	{
		RECT rect, rect1;
		char str_content[10];
		for(i = 0; i < 5; i++)
		{
//			rect.x = (250 - 128) / 2 - 15;
//			rect.y = 30*i + 4;
//			rect1.x = rect.x + 50 + 10;
//			rect1.y = rect.y + 5;
			rect.x = 1;
			rect.y = 30*i + 4;
			rect1.x = rect.x + 50 + 10;
			rect1.y = rect.y + 5;

			eLIBs_memset(str_content, 0, 10);
			SLIB_int2str_dec(i+1, str_content);

			if(i == dialog_jh_tran_data)
			{
				GUI_BMP_RES_Draw(res->bmp_bklt_select, rect.x, rect.y);
				GUI_SetColor(ui->colour.txt_f);
				GUI_DispStringAt(str_content, rect1.x, rect1.y);
			}
			else
			{
				GUI_BMP_RES_Draw(res->bmp_bklt_unselect, rect.x, rect.y);
				GUI_SetColor(ui->colour.txt_n);
				GUI_DispStringAt(str_content, rect1.x, rect1.y);
			}
		}
		
	}
	else if(dialog_current_state == 1)
	{
		RECT rect, rect1;
		
		for(i = 0; i < 7; i++)
		{
			//rect.x = (250 - 128) / 2 - 15;
			rect.x = 1;
			rect.y = 30*i + 4;
			rect1.x = rect.x + 50 - 5;
			rect1.y = rect.y + 5;

			if(i == dialog_jh_tran_data)//res->bklt_focus)
			{
				GUI_BMP_RES_Draw(res->bmp_bklt_select, rect.x, rect.y);
				GUI_SetColor(ui->colour.txt_f);
				GUI_DispStringAt(pbuff_str[i], rect1.x, rect1.y);
			}
			else
			{
				GUI_BMP_RES_Draw(res->bmp_bklt_unselect, rect.x, rect.y);	
				GUI_SetColor(ui->colour.txt_n);
				GUI_DispStringAt(pbuff_str[i], rect1.x, rect1.y);
			}
		}
	}

	if((dialog_current_state >= 2) && (dialog_current_state <6))
	{
		GUI_SetColor(ui->colour.txt_n);
		GUI_DispStringAt(res->str_title, ui->pos.title.x, ui->pos.title.y-6);
	}

	if(dialog_current_state == 0)
	{
		ui->pos.content.x = (ui->rt.width - 20) / 2;
		ui->size.content.width	= 20; 
	}
	else if(dialog_current_state == 1)
	{
		ui->size.content.width = 60;
		ui->pos.content.x = (ui->rt.width - 60) / 2;
	}
	//__inf("ui->size.content.width: %d, ui->pos.content.x: %d\n", ui->size.content.width, ui->pos.content.x);

	//内容
	rect.x0 = ui->pos.content.x;
	rect.y0 = ui->pos.content.y;
	rect.x1 = rect.x0 + ui->size.content.width;
	rect.y1 = rect.y0 + ui->size.content.height;
	//__inf("rect.x0:%d, rect.y0:%d, rect.x1:%d, rect.y1:%d\n", rect.x0, rect.y0, rect.x1, rect.y1);
	
	if(dialog_current_state != 6)
	{
		eLIBs_memset(res->str_content, 0, ADLG_STR_CONTENT_MEM_SIZE);
	}

	if(dialog_current_state == 6)
	{
		GUI_SetColor(ui->colour.txt_n);
		GUI_DispStringInRectWrap(res->str_content, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER, GUI_WRAPMODE_WORD);
		__inf("res->str_content = %s\n", res->str_content);
	}

	if((dialog_current_state != 4)&& (dialog_current_state != 0)\
		&&(dialog_current_state !=6 )\
		&&(dialog_current_state != 1))
	{
		for (i = 0; i < res->btn_count; i++)
		{
			x = ui->pos.btn_start.x;
			y = ui->pos.btn_start.y - 50 + i*35;
			
			if (ui->ctrl.focus_id == i)
			{
				bmp_data = ResHandle2Data(res->bmp_bklt_select);
			}
			else
			{
				bmp_data = ResHandle2Data(res->bmp_bklt_unselect);
			}
			if(dialog_current_state == 3)
			{
				GUI_BMP_Draw(bmp_data, x - 20 - 5, y);	
			}
			else
			{
				GUI_BMP_Draw(bmp_data, x + 5, y);
			}
			if(dialog_current_state == 3)
			{
				rect.x0 = x + 23;
				rect.y0 = rect.y0 - 15 + i*50;
				rect.x1 = rect.x0 + ui->size.btn.width;
				rect.y1 = rect.y1 -15 + i*50;
			}
			else if((dialog_current_state == 2)||(dialog_current_state == 5))
			{
				rect.x0 = x + 53;
				rect.y0 = rect.y0 - 15 + 5 + i*48;
				rect.x1 = rect.x0 + ui->size.btn.width;
				rect.y1 = rect.y1 -15 + 5 + i*48;
			}
			
			if(dialog_current_state > 1)
			{
				__inf("res->str_btn[%d]: %s\n",i, res->str_btn[i]);
				if(ui->ctrl.focus_id == i)
				{
					GUI_SetColor(ui->colour.txt_f);
				}
				else
				{
					GUI_SetColor(ui->colour.txt_n);
				}
				GUI_DispStringInRect(res->str_btn[i], &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
			}
		}
	}
}

#else
static void draw_dialog(APP_DIALOG_RES_T *res, APP_DIALOG_UI_T *ui)
{
    __s32             i, x, y;
    void             *bmp_data;
    GUI_RECT          rect;

    GUI_SetColor(ui->colour.txt_n);

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    //背景
    
    GUI_BMP_RES_Draw(res->bmp_bg, 0, 0);

	__inf("dialog_current_state = %d\n", dialog_current_state);

    if(dialog_current_state < 2)
    {
        GUI_BMP_RES_Draw(res->bmp_select, (250 - 128) / 2, (126 - 28) / 2 - 10);
        GUI_BMP_RES_Draw(res->bmp_l_n, (250 - 128) / 2 - 37, (126 - 28) / 2 - 9);
        GUI_BMP_RES_Draw(res->bmp_r_n, (250 - 128) / 2 + 128 + 10, (126 - 28) / 2 - 9);
    }

    if(dialog_current_state == 3)
    {
        GUI_BMP_RES_Draw(res->bmp_storage, 2, (126 - 45) / 2);
    }

    //标题
    //GUI_DispStringAt(res->str_title, ui->pos.title.x, ui->pos.title.y);
	__inf("res->str_title:%s----\n", res->str_title);

	if((dialog_current_state == 4)||(dialog_current_state == 3))
	{
		GUI_DispStringAt(res->str_title, ui->pos.title.x, ui->pos.title.y);
	}

    if(dialog_current_state == 0)
    {
        ui->pos.content.x = (ui->rt.width - 20) / 2;
        ui->size.content.width	= 20; 
    }
    else if(dialog_current_state == 1)
    {
        ui->size.content.width = 60;
        ui->pos.content.x = (ui->rt.width - 60) / 2;
    }
	__inf("ui->size.content.width: %d, ui->pos.content.x: %d\n", ui->size.content.width, ui->pos.content.x);

    //内容
    rect.x0 = ui->pos.content.x;
    rect.y0 = ui->pos.content.y;
    rect.x1 = rect.x0 + ui->size.content.width;
    rect.y1 = rect.y0 + ui->size.content.height;
	__inf("rect.x0:%d, rect.y0:%d, rect.x1:%d, rect.y1:%d\n", rect.x0, rect.y0, rect.x1, rect.y1);
	
    //GUI_DispStringInRect(res->str_content, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    if(dialog_current_state != 6)
    {
        eLIBs_memset(res->str_content, 0, ADLG_STR_CONTENT_MEM_SIZE);
    }

    if(dialog_current_state == 0)
    {
        res->str_content[0] = dialog_jh_tran_data + 0x30 + 1;
    }
    else if(dialog_current_state == 1)
    {
        if(dialog_jh_tran_data == 0)
        {
            res->str_content[0] = 'O';
            res->str_content[1] = 'F';
            res->str_content[2] = 'F';

        }
        else if(dialog_jh_tran_data == 1)
        {
            res->str_content[0] = '5';
            res->str_content[1] = 's';
        }
        else if(dialog_jh_tran_data == 2)
        {
            res->str_content[0] = '1';
            res->str_content[1] = '0';
            res->str_content[2] = 's';
        }
        else if(dialog_jh_tran_data == 3)
        {
            res->str_content[0] = '2';
            res->str_content[1] = '0';
            res->str_content[2] = 's';
        }
        else if(dialog_jh_tran_data == 4)
        {
            res->str_content[0] = '3';
            res->str_content[1] = '0';
            res->str_content[2] = 's';
        }

        else if(dialog_jh_tran_data == 5)
        {
            res->str_content[0] = '1';
            res->str_content[1] = 'm';
        }
        else if(dialog_jh_tran_data == 6)
        {
            res->str_content[0] = '5';
            res->str_content[1] = 'm';
        }
    }

    if((dialog_current_state < 2) || (dialog_current_state == 6))
    {
        GUI_DispStringInRectWrap(res->str_content, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER, GUI_WRAPMODE_WORD);
    }
    if(dialog_current_state != 4)
    {
        //按钮
        x = ui->pos.btn_start.x;
        y = ui->pos.btn_start.y;
        rect.y0 = y;
        rect.y1 = y + ui->size.btn.height;

		__inf("x:Y:y0:y1,[%d:%d:%d:%d]\n", x, y, rect.y0, rect.y1);
        for (i = 0; i < res->btn_count; i++)
        {
        	x = ui->pos.btn_start.x;
        	y = ui->pos.btn_start.y - 50 + i*35;
            if(dialog_current_state < 2)
            {
                if(i == 0)
                {
                    bmp_data = ResHandle2Data(res->bmp_btn_f);
                }
                else
                {
                    bmp_data = ResHandle2Data(res->bmp_btn_n);
                }
            }
            else
            {
                if (ui->ctrl.focus_id == i)
                {
                    bmp_data = ResHandle2Data(res->bmp_btn_yes);
                }
                else
                {
                    bmp_data = ResHandle2Data(res->bmp_btn_no);
                }
            }
            GUI_BMP_Draw(bmp_data, x, y);
            rect.x0 = x;
            rect.x1 = rect.x0 + ui->size.btn.width;
            if(dialog_current_state > 1)
            {
            	__inf("res->str_btn[%d]: %s\n",i, res->str_btn[i]);
                GUI_DispStringInRect(res->str_btn[i], &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            }
            if(dialog_current_state == 3)
            {
                x = rect.x1 + ui->size.btn_sp.width / 2;
            }
            else
            {
                x = rect.x1 + ui->size.btn_sp.width;
            }
        }
    }
    //GUI_CloseAlphaBlend();
}
#endif
//
static __s32 on_dialog_paint(__gui_msg_t *msg)
{
    GUI_MEMDEV_Handle draw_mem;
    dialog_wnd_t *wnd_para;

    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, msg->h_deswin);
    ASSERT(wnd_para);

    if (GUI_LyrWinGetSta(wnd_para->hlyr) != GUI_LYR_STA_ON)
    {
        return EPDK_OK;
    }

    GUI_LyrWinSel(wnd_para->hlyr);
    GUI_SetFont(SWFFont);
    GUI_UC_SetEncodeUTF8();
    draw_mem = GUI_MEMDEV_Create(0, 0, wnd_para->ui.rt.width, wnd_para->ui.rt.height);
    ASSERT(draw_mem);
    GUI_MEMDEV_Select(draw_mem);

    draw_dialog(&wnd_para->res, &wnd_para->ui);

    GUI_MEMDEV_CopyToLCD_Ex(draw_mem);
    GUI_MEMDEV_Select( NULL );
    GUI_MEMDEV_Delete(draw_mem);

    return EPDK_OK;
}

static __s32 on_dialog_timer(__gui_msg_t *msg)
{
    if (LOWORD(msg->dwAddData1) == ID_TIMER_DIALOG_TIMEOUT)
    {
        dialog_wnd_t *wnd_para;

        app_dialog_GetWndPara(wnd_para, dialog_wnd_t, msg->h_deswin);
        wnd_para->result = ADLG_IDM_TIMEROUT;
        notify_to_close_dialog(msg->h_deswin);
        return EPDK_OK;
    }
    else
    {
        return EPDK_FAIL;
    }
}

//
static __s32 on_dialog_create(__gui_msg_t *msg)
{
    dialog_wnd_t *wnd_para;
    //__bool   setid_flag;

    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, msg->h_deswin);
    if (wnd_para->style & ADLG_DS_VISIBLE)
    {
    	__msg("app_show_layer\n");
        app_show_layer(wnd_para->hlyr);
        __here__;
    }
    else
    {
    	__msg("app_unshow_layer\n");
        SEND_MSG(GUI_MSG_COMMAND, wnd_para->hwnd, wnd_para->parent, MAKELONG(wnd_para->ID, ADLG_CMD_SET_UI), 0);
    }

    GUI_LyrWinSetTop(wnd_para->hlyr);

    GUI_WinSetFocusChild(msg->h_deswin);

    if (IsDialogTimerValid(wnd_para->timeout))
    {
        GUI_SetTimer(/*wnd_para->hwnd*/msg->h_deswin, ID_TIMER_DIALOG_TIMEOUT, wnd_para->timeout, NULL);

    }

    return EPDK_OK;
}

static __s32 on_dialog_destroy(__gui_msg_t *msg)
{
    dialog_wnd_t *wnd_para;

    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, msg->h_deswin);

    if (IsDialogTimerValid(wnd_para->timeout))
    {
        GUI_KillTimer(wnd_para->hwnd, ID_TIMER_DIALOG_TIMEOUT);
    }

    app_dialog_data_release(wnd_para);
    __msg("on_dialog_destroy end\n");
    return EPDK_OK;
}

//对话框回调
static __s32 app_dialog_proc(__gui_msg_t *msg)
{
    DEBUG_CBMSGEx(app_dialog_proc);

    switch(msg->id)
    {
    case GUI_MSG_CREATE:
        return on_dialog_create(msg);
    case GUI_MSG_CLOSE:
        GUI_FrmWinDelete(msg->h_deswin);
        break;
    case GUI_MSG_DESTROY:
        return on_dialog_destroy(msg);
    case GUI_MSG_PAINT:
        return on_dialog_paint(msg);
    case GUI_MSG_KEY:
        return dialog_key_proc(msg);
    case GUI_MSG_TOUCH:
        break;
    case GUI_MSG_TIMER:
        __inf("---jh_dbg1018_timer---\n");
        if (on_dialog_timer(msg) == EPDK_OK)
        {
            return EPDK_OK;
        }
        break;
    case GUI_MSG_COMMAND:
        on_dialog_command(msg);
        break;
    case GUI_MSG_WIN_WAKEUP:
        GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
        break;
    default:
        break;
    }
    return GUI_FrmWinDefaultProc(msg);
}

static H_WIN app_dialog_create_frmwin(dialog_wnd_t *wnd_para)
{
    __gui_framewincreate_para_t framewin_para;
    FB fb;

    GUI_LyrWinGetFB(wnd_para->hlyr, &fb);
    __msg("dialog size(%d, %d)\n", fb.size.width, fb.size.height);

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));

    framewin_para.name          = APP_DIALOG;
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = wnd_para->ID;
    framewin_para.hHosting      = wnd_para->parent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)app_dialog_proc);
    framewin_para.rect.x        = 0;
    framewin_para.rect.y        = 0;
    framewin_para.rect.width    = fb.size.width;
    framewin_para.rect.height   = fb.size.height;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr          = (void *)wnd_para;
    framewin_para.hLayer        = wnd_para->hlyr;

    return (GUI_FrmWinCreate(&framewin_para));
}

/************************************************************************/
// 建立图层
/************************************************************************/
#define app_dialog_layer_create(_layer, _prt)        \
	app_com_layer_create(_layer, _prt, 1, PIXEL_COLOR_ARGB8888, GUI_LYRWIN_STA_SUSPEND, "dialog layer")
// static H_LYR app_dialog_layer_create(RECT *rect)
// {
// 	H_LYR layer = NULL;
// 	FB  fb =
// 	{
// 		{0, 0},                                   		/* size      */
// 		{0, 0, 0},                                      /* buffer    */
// 		{FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},    /* fmt       */
// 	};
//
// 	__disp_layer_para_t lstlyr =
// 	{
// 		DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
// 		0,                                              /* ck_mode   */
// 		0,                                              /* alpha_en  */
// 		0,                                              /* alpha_val */
// 		1,                                              /* pipe      */
// 		0xff,                                           /* prio      */
// 		{0, 0, 0, 0},                           		/* screen    */
// 		{0, 0, 0, 0},                               	/* source    */
// 		DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
// 		NULL                                            /* fb        */
// 	};
//
// 	__layerwincreate_para_t lyrcreate_info =
// 	{
// 		"dialog layer",
// 		NULL,
// 		GUI_LYRWIN_STA_SUSPEND,
// 		GUI_LYRWIN_NORMAL
// 	};
//
// 	fb.size.width		= rect->width;
// 	fb.size.height		= rect->height;
//
// 	// framebuffer source window
// 	lstlyr.src_win.x  		= 0;
// 	lstlyr.src_win.y  		= 0;
// 	lstlyr.src_win.width 	= rect->width;
// 	lstlyr.src_win.height 	= rect->height;
//
// 	//screen region
// 	lstlyr.scn_win.x		= rect->x;
// 	lstlyr.scn_win.y		= rect->y;
// 	lstlyr.scn_win.width  	= rect->width;
// 	lstlyr.scn_win.height 	= rect->height;
//
// 	lstlyr.pipe = 1;
// 	lstlyr.fb = &fb;
// 	lyrcreate_info.lyrpara = &lstlyr;
//
// 	layer = GUI_LyrWinCreate(&lyrcreate_info);
// 	if( !layer )
// 	{
// 		__err("app layer create error !\n");
// 	}
// 	__msg("create layer = %x\n", layer);
//
// 	return layer;
// }

/************************************************************
 *dialog_current_state:
 *value: 0-->//背光亮度; 1-->//背光时间; 2-->//恢复出厂设置
 *3-->//卡格式化; 4-->//无卡; 5-->//从非设置界面来的; 6-->other
 ************************************************************/
H_WIN app_dialog_create(H_WIN h_parent, __s32 win_id, __u32 style,
                        __s32 bmp_id[], __s32 str_id[], __u32 timeout, __u32 jh_tran_num)
{
    //TODO:
    dialog_wnd_t *wnd_para;

    __inf("****************************************************************\n");
    __inf("********  enter dialog app  **************\n");
    __inf("****************************************************************\n");

	__msg("app_dialog_create\n");
    BallocType(wnd_para, dialog_wnd_t);
    if (wnd_para == NULL)
    {
        return NULL;
    }

    eLIBs_memset(wnd_para, 0, sizeof(dialog_wnd_t));

    wnd_para->parent  = h_parent;
    wnd_para->ID      = win_id;
    wnd_para->style   = style;
    wnd_para->timeout = timeout;
	
    if(jh_tran_num & 0x400)
    {
        dialog_current_state = 6;
    }
    else if(jh_tran_num & 0x200)
    {
        dialog_current_state = 5; //从非设置界面来的
    }
    else if(jh_tran_num & 0x100)
    {
        dialog_current_state = 4; //无卡
        wnd_para->timeout = 500; //5s
    }
    else
    {
        dialog_jh_tran_data = jh_tran_num;

        if((dialog_jh_tran_data & 0xc0) == 0x80)
        {
            dialog_current_state = 1; //背光时间
            dialog_jh_tran_data = dialog_jh_tran_data & 0x7f;
        }
        else if((dialog_jh_tran_data & 0xc0) == 0)
            dialog_current_state = 0; //背光亮度
        else if((dialog_jh_tran_data & 0xc0) == 0x40)
        {
            dialog_current_state = 2; //恢复出厂设置
        }
        else if((dialog_jh_tran_data & 0xc0) == 0xc0)
        {
            dialog_current_state = 3; //卡格式化
        }
    }

    __msg("jh_tran_num:%d, dialog_current_state = %d\n", jh_tran_num, dialog_current_state);

    app_dialog_data_init(wnd_para, bmp_id, str_id, NULL);
    app_dialog_layer_create(wnd_para->hlyr, &wnd_para->ui.rt);
	__msg("ui_rt:%d,%d,%d,%d\n", wnd_para->ui.rt.x, wnd_para->ui.rt.y,wnd_para->ui.rt.width, wnd_para->ui.rt.height);
    if (wnd_para->hlyr == NULL)
    {
        //TODO:release mem
        __err("app_dialog_layer_create error(%d, %d)\n", wnd_para->ui.rt.width, wnd_para->ui.rt.height);
    }

    wnd_para->hwnd = app_dialog_create_frmwin(wnd_para);

    GUI_WinSetFocusChild(wnd_para->hwnd);

    return wnd_para->hwnd;
}

H_WIN app_dialog_create_ex(H_WIN h_parent, __s32 win_id, __u32 btn_style,
                           __s32 bmp_id[], __s32 str_id[], __u32 timeout, GUI_RECT *dialog_rt)
{
    dialog_wnd_t *wnd_para;

    __inf("****************************************************************\n");
    __inf("********  enter dialog app  **************\n");
    __inf("****************************************************************\n");


    BallocType(wnd_para, dialog_wnd_t);
    if (wnd_para == NULL)
    {
        return NULL;
    }
    eLIBs_memset(wnd_para, 0, sizeof(dialog_wnd_t));

    wnd_para->parent  = h_parent;
    wnd_para->ID      = win_id;
    wnd_para->style   = btn_style;
    wnd_para->timeout = timeout;
    app_dialog_data_init(wnd_para, bmp_id, str_id, dialog_rt);

    return NULL;
}

__s32 app_dialog_destroy(H_WIN h_dialog)
{
    dialog_wnd_t *wnd_para;

    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, h_dialog);

    ASSERT(wnd_para);

    if (wnd_para->hlyr != NULL)
    {
        H_WIN parent = wnd_para->parent;

        app_com_layer_destroy(wnd_para->hlyr);//GUI_LyrWinDelete(wnd_para->hlyr);

        //		GUI_WinSetFocusChild(parent);

        __msg("end of dialog\n");
    }

    return 0;
}

__s32 app_dialog_set_ui(H_WIN h_dialog, __s32 ui_id, __s32 wParam, __s32 lParam)
{
    APP_DIALOG_UI_T *ui;
    dialog_wnd_t    *wnd_para;

    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, h_dialog);
    ASSERT(wnd_para);
    ui = &wnd_para->ui;

    switch (ui_id)
    {
    case ADLG_UI_CMD_TITLE:
    {
        ui->pos.title.x = wParam;
        ui->pos.title.y = lParam;
        ui->size.title.width  = ui->rt.width - wParam;
        //ui->size.title.height = ui->size.title.height;
    }
    break;
    case ADLG_UI_CMD_CONTENT:
    {
        RECT *pRt = (RECT *)lParam;
        ui->pos.content.x = pRt->x;
        ui->pos.content.y = pRt->y;
        ui->size.content.width  = pRt->width;
        ui->size.content.height = pRt->height;
    }
    break;
    case ADLG_UI_CMD_ICON:
    {
        ui->pos.icon.x = wParam;
        ui->pos.icon.y = lParam;
    }
    break;
    case ADLG_UI_CMD_START_BTN:
    {
        ui->pos.btn_start.x = wParam;
        ui->pos.btn_start.y = lParam;
        if (wnd_para->res.btn_count > 1)
        {
            // 				ui->size.btn_sp.width = ((ui->rt.width - wParam * 2)
            // 					-  ui->size.btn.width * wnd_para->res.btn_count) / (wnd_para->res.btn_count - 1);
            ui->size.btn_sp.width = calc_btn_sp_w(ui->rt.width, wParam, ui->size.btn.width, wnd_para->res.btn_count);
            ASSERT(0 < ui->size.btn_sp.width);
        }
    }
    break;
    case ADLG_END_UI_CMD:
    {
        app_show_layer(wnd_para->hlyr);
    }
    break;
    default:
        ASSERT(0);
        break;
    }
    return EPDK_OK;
}

