/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: main_menu.c
* By      	: Andy.zhang
* Func		: desk main thread
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "app_home_i.h"
#include "main_menu.h"

#if  0
//#define __here__            eLIBs_printf("@L%d(%s)\n", __LINE__, __FILE__);
#define __msg(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
						     eLIBs_printf(__VA_ARGS__)									        )
//#define __log(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
//								 eLIBs_printf(__VA_ARGS__)											)
#else
#define __msg(...)   
#define __log(...)
#endif


static __s32 main_menu_timmer_id = 11;

enum
{
    MMENU_PAINT_ALL,
    MMENU_PAINT_ITEM
};

typedef struct tag_res_id_t
{
    __s32 lang_id;
    __s32 unfocus_bmp_id;
    __s32 focus_bmp_id;
} mm_res_id_t;

typedef struct tag_mmenu_attr
{
    HTHEME focus_bmp[MAX_ITEM_NUM];
    HTHEME unfocus_bmp[MAX_ITEM_NUM];
#ifdef MENU_BAR_ITEM
	HTHEME item_bar[2];			// 0:lose, 1:focus
#endif
    char item_str[MAX_ITEM_NUM][128];

    __s32 first_item;
    __s32 focus_item;
    __s32 item_nr;//总个数
    __s32 item_per_screen;//一屏显示的个数

    __s32 item_w;

    __s8	  paint_flag;

    __s32 focus_txt_color;
    __s32 unfocus_txt_color;

    GUI_FONT *font;
    H_LYR	layer;
    __bool  res_init;

} mmenu_attr_t;


static mm_res_id_t uipara_400_240[] =
{
    {STRING_HOME_FM, ID_HOME_MAIN_FM_LOSE_BMP, ID_HOME_MAIN_FM_FOCUS_BMP},
    {STRING_HOME_RECORD, ID_HOME_MAIN_RECORD_LOSE_BMP, ID_HOME_MAIN_RECORD_FOCUS_BMP},
    {STRING_HOME_MOVIE, ID_HOME_MAIN_MOVIE_LOSE_BMP, ID_HOME_MAIN_MOVIE_FOCUS_BMP},
    {STRING_HOME_PHOTO, ID_HOME_MAIN_PHOTO_LOSE_BMP, ID_HOME_MAIN_PHOTO_FOCUS_BMP},
    {STRING_HOME_CAPTURE, ID_HOME_MAIN_MUSIC_LOSE_BMP, ID_HOME_MAIN_MUSIC_FOCUS_BMP},
    {STRING_HOME_EBOOK, ID_HOME_MAIN_EBOOK_LOSE_BMP, ID_HOME_MAIN_EBOOK_FOCUS_BMP},
    {STRING_HOME_OTHERS, ID_HOME_MAIN_OTHERS_LOSE_BMP, ID_HOME_MAIN_OTHERS_FOCUS_BMP},
};

static mm_res_id_t uipara_480_272[] =
{
    {STRING_HOME_MOVIE, ID_HOME_MAIN_MOVIE_LOSE_BMP, ID_HOME_MAIN_MOVIE_FOCUS_BMP},
    {STRING_HOME_PHOTO, ID_HOME_MAIN_PHOTO_LOSE_BMP, ID_HOME_MAIN_PHOTO_FOCUS_BMP},
    {STRING_HOME_CAPTURE, ID_HOME_MAIN_MUSIC_LOSE_BMP, ID_HOME_MAIN_MUSIC_FOCUS_BMP},
    {STRING_HOME_OTHERS, ID_HOME_MAIN_OTHERS_LOSE_BMP, ID_HOME_MAIN_OTHERS_FOCUS_BMP},
    
};

static mm_res_id_t *main_get_ui_para(__s32 rotate)
{
    __s32           screen_width;
    __s32 			screen_height;

    /* get lcd size*/
    dsk_display_get_size(&screen_width, &screen_height);
    switch(rotate)
    {
    case GUI_SCNDIR_NORMAL:
    case GUI_SCNDIR_ROTATE180:
    {
        if((screen_width == 400 ) && ( screen_height == 240))
            return uipara_400_240;
        else if((screen_width == 480 ) && ( screen_height == 272))
            return uipara_480_272;

    }
    case GUI_SCNDIR_ROTATE90:
    case GUI_SCNDIR_ROTATE270:

        break;
    }

    return uipara_400_240;
}


__s32 g_fsize;
ES_FILE *g_f;
unsigned char *g_p;
__s32 g_ret = EPDK_FAIL;

static __u32 seedtbl[] =
{
    16381163, 902867507, 703006358, 825601098, 702943101, 600362569, 824180876, 928050656,
    500242512, 600364801, 928023969, 680398149, 672661988, 825601527, 825601527, 825613410,
    823090756, 526599359,   8342805, 631233585, 302865607, 926952182, 232272027, 824145274,
    926933440, 919200125, 631206441, 631245671, 726297021, 726289272,  85829891, 631270516,
    726269366, 727066702, 726269355, 750013275, 726212604, 672729451,  85809036, 825660596,
    417735750,  18473775, 631290206
};
static __u8         index = 0;
static __u32        ramdomcnt = 15;          /* used by ramdom function              */
static __u32        ramdomseed = 16;

static __u32 my_random(__u32 max)
{
    __u32 ret;

    if(index > sizeof(seedtbl) / sizeof(seedtbl[0]))
    {
        index = 0;
    }
    ramdomcnt = ramdomcnt * 29 + 314;

    ramdomseed += ramdomcnt;

    ret = ramdomcnt;

    ret = ret + seedtbl[index++];

    ret = ret - (ret / max) * max;

    return ret;
}

static __s32 test_read_nor_open(__s32 *fsize, ES_FILE **f, unsigned char **p)
{
    __s32 ret;
    __s32 i, j;
    __s32 cur_data;
    __s32 page_num;
    __s32 once_count;
    __s32 start_pos;
    __s32 file_size;
    unsigned char *pbuf;
    char path[] = BEETLES_APP_ROOT"apps\\nor_reading_test.bin";
    ES_FILE *fp;

    //open file
    fp = eLIBs_fopen(path, "rb");
    if(!fp)
    {
        eLIBs_printf("open file fail, path=%s\n", path);
        return EPDK_FAIL;
    }

    //get file size
    eLIBs_fseek(fp, 0, SEEK_END);
    file_size = eLIBs_ftell(fp);
    eLIBs_printf("file_size=%d\n", file_size);

    //cal page size
    page_num = (file_size + 1023) >> 10;
    eLIBs_printf("page_num=%d\n", page_num);

    //alloc page mem
    pbuf = esMEMS_Palloc(page_num, 0);
    if(!pbuf)
    {
        eLIBs_printf("esMEMS_Palloc fail\n");
        eLIBs_fclose(fp);
        return EPDK_FAIL;
    }

    *fsize = file_size;
    *f = fp;
    *p = pbuf;
    return EPDK_OK;
}

static __s32 test_read_nor_read(__s32 fsize, ES_FILE *f, unsigned char *p)
{
    __s32 i, j;
    __s32 start_pos, once_count;
    __s32 file_size;
    __s32 cur_data;
    __s32 result;
    ES_FILE *fp;
    __s32 ret;
    unsigned char *pbuf;

    file_size = fsize;
    fp = f;
    pbuf = p;

    result = EPDK_TRUE;

    //begin test
    for(i = 0 ; i < 1; i++)
    {
        //generate start reading pos
        start_pos = my_random(file_size - 2);

        //generate reading count
        once_count = my_random(file_size - start_pos);

        if(0 == once_count)
        {
            once_count = 1;
        }

        //seek and read

        eLIBs_fseek(fp, start_pos, SEEK_SET);

        ret = eLIBs_fread(pbuf, 1, once_count, fp);

        if(ret != once_count)
        {
            result = EPDK_FALSE;
            eLIBs_printf("eLIBs_fread fail, once_count=%d, ret=%d\n", once_count, ret);
            break;
        }

        //veryfy data
        cur_data = start_pos % 256;
        for(j = 0 ; j < once_count ; j++)
        {
            if(cur_data != pbuf[j])
            {
                break;
            }
            cur_data++;
            if(cur_data >= 256)
            {
                cur_data = 0;
            }
        }

        if(j < once_count)//has err data
        {
            result = EPDK_FALSE;

            eLIBs_printf("***************err encounter, ****************\n");
            eLIBs_printf("test bad, i=%d, start_pos=%d, once_count=%d, cur_data=%d, pbuf[%d]=%d, err_pos=%d, (start_pos+j)/1024=%f\n"
                         , i, start_pos, once_count, cur_data, j, pbuf[j], start_pos + j, (start_pos + j) / 1024.0);

            if(0) //dump data
            {
                __s32 k;
                eLIBs_printf("***************err encounter, begin dump****************\n");

                for(k = 0 ; k < once_count ; k++)
                {
                    eLIBs_printf("pbuf[%.6d]=%X\n", k, pbuf[k]);
                }
                eLIBs_printf("***************end dump****************\n");
            }

            __wait__;
            //break;
        }
        else//test alright
        {
            eLIBs_printf("test alright, i=%d, start_pos=%d, once_count=%d\n"
                         , i, start_pos, once_count);
        }

        esKRNL_TimeDly(10);
    }

    return EPDK_OK;
}


/*
	申请资源
*/
static __s32 init_mmenu_res(mmenu_attr_t *mmenu_attr)
{
    __u32 	i;
    home_uipara_t *home_ui_para;
    mm_res_id_t *main_ui_para;

    if(EPDK_TRUE == mmenu_attr->res_init)
    {
        return EPDK_OK;
    }

    home_ui_para = home_get_ui_para(GUI_GetScnDir());
    if(!home_ui_para)
    {
        __err("home_ui_para is null..\n");
        return EPDK_FAIL;
    }

    main_ui_para = main_get_ui_para(GUI_GetScnDir());
    if(!main_ui_para)
    {
        __err("main_ui_para is null...\n");
        return EPDK_FAIL;
    }

    mmenu_attr->item_nr = home_ui_para->total_item_num;
    mmenu_attr->item_per_screen = home_ui_para->item_per_screen;

    for(i = 0; i < mmenu_attr->item_nr; i++)
    {
        mmenu_attr->unfocus_bmp[i] = dsk_theme_open(main_ui_para[i].unfocus_bmp_id);

        mmenu_attr->focus_bmp[i] = dsk_theme_open(main_ui_para[i].focus_bmp_id);
    }
#ifdef MENU_BAR_ITEM
	mmenu_attr->item_bar[0] = dsk_theme_open(ID_HOME_MENU_BAR_LOSE_BMP);
	mmenu_attr->item_bar[1] = dsk_theme_open(ID_HOME_MENU_BAR_FOCUS_BMP );
	
#endif

    mmenu_attr->res_init = EPDK_TRUE;

    return EPDK_OK;
}

/*
	释放资源
*/
static __s32 uninit_mmenu_res(mmenu_attr_t *mmenu_attr)
{
    __u32 	i;

    if(EPDK_FALSE == mmenu_attr->res_init)
    {
        return EPDK_OK;
    }

    for (i = 0; i < mmenu_attr->item_nr; i++)
    {
        dsk_theme_close(mmenu_attr->unfocus_bmp[i]);
        dsk_theme_close(mmenu_attr->focus_bmp[i]);
    }
#ifdef MENU_BAR_ITEM
	if(mmenu_attr->item_bar[0])
	{
		__msg("release mmenu_attr->item_bar[0]\n");
		dsk_theme_close(mmenu_attr->item_bar[0]);
	}
	if(mmenu_attr->item_bar[1])
	{
		__msg("release mmenu_attr->item_bar[1]\n");
		dsk_theme_close(mmenu_attr->item_bar[1] );
	}
#endif

    mmenu_attr->res_init = EPDK_FALSE;

    return EPDK_OK;
}

static void paint_mmain_item_ex(mmenu_attr_t *mmenu_attr, __s32 index
                                , __s32 bfocus)
{
    GUI_RECT gui_rect;
    __u32   i;
    void *pbmp, *pbmp1;
    __s32 bmp_width, bmp_width1;
    __s32 bmp_height, bmp_height1;
    HBMP hbmp, hbmp1;
    __s32 bmp_x;
    __s32 bmp_y;
    home_uipara_t *home_ui_para;
    mm_res_id_t *main_ui_para;

    if(EPDK_FALSE == mmenu_attr->res_init)
    {
        init_mmenu_res(mmenu_attr);
    }

    home_ui_para = home_get_ui_para(GUI_GetScnDir());
    if(!home_ui_para)
    {
        __err("home_ui_para is null..\n");
        return ;
    }

    main_ui_para = main_get_ui_para(GUI_GetScnDir());
    if(!main_ui_para)
    {
        __err("main_ui_para is null...\n");
        return ;
    }

    if (index < mmenu_attr->first_item || index >= mmenu_attr->item_nr)
    {
        __err("invalid para...\n");
        return ;
    }

    GUI_LyrWinSel(mmenu_attr->layer);
    GUI_SetFont(mmenu_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetBkColor(0);

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL); //GUI_DRAWMODE_NORMAL
    if(bfocus)
    {
        pbmp = dsk_theme_hdl2buf(mmenu_attr->focus_bmp[index]);
		pbmp1 = dsk_theme_hdl2buf(mmenu_attr->item_bar[1]);
    }
    else
    {
        pbmp = dsk_theme_hdl2buf(mmenu_attr->unfocus_bmp[index]);
		pbmp1 = dsk_theme_hdl2buf(mmenu_attr->item_bar[0]);
    }

    if((NULL == pbmp)||(NULL == pbmp1))
    {
        __err("dsk_theme_hdl2buf fail\n");
        return ;
    }

    hbmp = bmp_open(pbmp);
	hbmp1 = bmp_open(pbmp1);
    if((NULL == hbmp)||(NULL == hbmp1))
    {
        __msg("open bmp fail\n");
        return;
    }

    bmp_width = bmp_get_width(hbmp);
    bmp_height = bmp_get_height(hbmp);
	
    bmp_width1 = bmp_get_width(hbmp1);
    bmp_height1 = bmp_get_height(hbmp1);
	
    bmp_close(hbmp);
    bmp_close(hbmp1);
    hbmp = 0;
	hbmp1 = 0;
    if(bmp_width > home_ui_para->max_main_bmp_width || bmp_height > home_ui_para->max_main_bmp_height)
    {
        __err("width or height of bmp too big..\n");
        return ;
    }

    //clear bg rect	
    __msg("\n----------------------1------------------------\n");
	__msg("index = %d, mmenu_attr->first_item = %d\n", index, mmenu_attr->first_item);
#ifdef MENU_BAR_ITEM
	gui_rect.x0 = 6;
	gui_rect.y0 = mmenu_attr->item_w * (index - mmenu_attr->first_item) + 2;
	gui_rect.x1 = 472;
	gui_rect.y1 = gui_rect.y0 + mmenu_attr->item_w;
	GUI_ClearRectEx(&gui_rect);

	
    //draw bmp
    bmp_x = gui_rect.x0;
    bmp_y = gui_rect.y0;
    GUI_BMP_Draw(pbmp1, bmp_x, bmp_y);
#endif
    gui_rect.x0 = 15;
    gui_rect.y0 = mmenu_attr->item_w * (index - mmenu_attr->first_item) + 2;
    gui_rect.x1 = gui_rect.x0 + home_ui_para->max_main_bmp_width + 5;
    gui_rect.y1 = gui_rect.y0 + mmenu_attr->item_w;
	
    __msg("draw:gui_rect.x0=%d,gui_rect.y0=%d,gui_rect.x1=%d,gui_rect.y1=%d\n\n", gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1 );
    //GUI_ClearRectEx(&gui_rect);

    //draw bmp
    bmp_x = gui_rect.x0 + (home_ui_para->max_main_bmp_width - bmp_width) / 2;
    bmp_y = gui_rect.y0 + (home_ui_para->max_main_bmp_height - bmp_height) / 2;
    GUI_BMP_Draw(pbmp, bmp_x, bmp_y);
    //draw text
    if(bfocus)
    {
        GUI_SetColor(GUI_WHITE);
    }
    else
    {
        GUI_SetColor(GUI_DARKBLUE);
    }
    
    gui_rect.x0 = (480 - home_ui_para->max_main_bmp_width)/2 + 15;
    gui_rect.y0 = mmenu_attr->item_w * (index - mmenu_attr->first_item)+10;
    gui_rect.x1 = gui_rect.x0 + 70; 
    gui_rect.y1 = gui_rect.y0 + 32; 
    __msg("txt:gui_rect.x0=%d,gui_rect.y0=%d,gui_rect.x1=%d,gui_rect.y1=%d\n\n", gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1 );
    //GUI_ClearRectEx(&gui_rect);
    dsk_langres_get_menu_text(main_ui_para[index].lang_id, mmenu_attr->item_str[index], GUI_TITLE_MAX);
    GUI_DispStringInRect(mmenu_attr->item_str[index], &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    __msg("\n--------------------2--------------------------\n");
}

/*
	显示全部ITEM
*/
static void paint_mmain_item_all(mmenu_attr_t *mmenu_attr)
{
    __s32 i;
    __s32 max_paint_index;

    if(EPDK_FALSE == mmenu_attr->res_init)
    {
        init_mmenu_res(mmenu_attr);
    }

    max_paint_index = mmenu_attr->first_item + mmenu_attr->item_per_screen - 1;
    if (max_paint_index >= mmenu_attr->item_nr)
    {
        max_paint_index = mmenu_attr->item_nr - 1;
    }

    for(i = mmenu_attr->first_item; i <= max_paint_index; i++)
    {
        if (i == mmenu_attr->focus_item)
        {
            paint_mmain_item_ex(mmenu_attr, i, 1);		//画焦点图片
        }
        else
        {
            paint_mmain_item_ex(mmenu_attr, i, 0);		//画非焦点图片
        }
    }
}

static __s32  mmenu_key_proc(__gui_msg_t *msg)
{
    static __s32 last_key = -1;
    mmenu_attr_t *mmenu_attr;

    mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    if (!mmenu_attr)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        if (GUI_MSG_KEY_ENTER == last_key)
        {
            __msg("mmenu_key_proc: focus: %d,first: %d\n", mmenu_attr->focus_item, mmenu_attr->first_item);
            main_cmd2parent(msg->h_deswin, ID_OP_SUB_ENTER, mmenu_attr->focus_item, mmenu_attr->first_item);
        }
    }
    else
    {
        switch( msg->dwAddData1 )
        {
        case GUI_MSG_KEY_LONGENTER:
            if(last_key == GUI_MSG_KEY_ENTER)
            {
                if(mmenu_attr->focus_item < 2)
                {
                    __log("---enter list----\n");
                    main_cmd2parent(msg->h_deswin, ID_OP_SUB_LIS, 0, 0);
                }
            }
            break;

        case GUI_MSG_KEY_LEFT:
        case GUI_MSG_KEY_LONGLEFT:
        {
            if(mmenu_attr->focus_item <= 0)
            {
            	paint_mmain_item_ex(mmenu_attr, 0, 0);
                mmenu_attr->focus_item = mmenu_attr->item_nr - 1;
                mmenu_attr->first_item = mmenu_attr->item_nr - mmenu_attr->item_per_screen;
                //paint_mmain_item_all(mmenu_attr);
                paint_mmain_item_ex(mmenu_attr, mmenu_attr->focus_item, 1);
                
            }
            else
            {
                if (mmenu_attr->focus_item - 1 < mmenu_attr->first_item)
                {
                    mmenu_attr->focus_item--;
                    mmenu_attr->first_item = mmenu_attr->focus_item;
                    paint_mmain_item_all(mmenu_attr);
                }
                else
                {
                    paint_mmain_item_ex(mmenu_attr, mmenu_attr->focus_item, 0);
                    mmenu_attr->focus_item--;
                    paint_mmain_item_ex(mmenu_attr, mmenu_attr->focus_item, 1);
                }
            }
            main_cmd2parent(msg->h_deswin, ID_SWITCH_ITEM, mmenu_attr->focus_item, mmenu_attr->first_item);
        }
        break;
        case GUI_MSG_KEY_RIGHT:
        case GUI_MSG_KEY_LONGRIGHT:
        {
			__msg("GUI_MSG_KEY_RIGHT\n");
            if(mmenu_attr->focus_item >= mmenu_attr->item_nr - 1)
            {
				paint_mmain_item_ex(mmenu_attr, mmenu_attr->item_nr - 1, 0);
                mmenu_attr->focus_item = 0;
                mmenu_attr->first_item = 0;
                //paint_mmain_item_all(mmenu_attr);		
                paint_mmain_item_ex(mmenu_attr, mmenu_attr->focus_item, 1);
                
            }
            else
            {
                if (mmenu_attr->focus_item + 1 - mmenu_attr->first_item >= mmenu_attr->item_per_screen)
                {
                    mmenu_attr->focus_item++;
                    mmenu_attr->first_item = mmenu_attr->focus_item - mmenu_attr->item_per_screen + 1;
                    paint_mmain_item_all(mmenu_attr);
                }
                else
                {
                    paint_mmain_item_ex(mmenu_attr, mmenu_attr->focus_item, 0);
                    mmenu_attr->focus_item++;
                    paint_mmain_item_ex(mmenu_attr, mmenu_attr->focus_item, 1);
                }
            }

            main_cmd2parent(msg->h_deswin, ID_SWITCH_ITEM, mmenu_attr->focus_item, mmenu_attr->first_item);
        }
        break;
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LONGUP:
        case GUI_MSG_KEY_VADD:
        case GUI_MSG_KEY_LONGVADD:
        {
            main_cmd2parent(msg->h_deswin, ID_OP_SUB_UP, mmenu_attr->focus_item, mmenu_attr->first_item);
            break;
        }
        case GUI_MSG_KEY_LONGDOWN:
        case GUI_MSG_KEY_DOWN:
        {
            main_cmd2parent(msg->h_deswin, ID_OP_SUB_DOWN, mmenu_attr->focus_item, mmenu_attr->first_item);
            break;
        }
        }
    }

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        last_key = -1;
    }
    else
    {
        last_key = msg->dwAddData1;
    }

    return EPDK_OK;
}

/*

*/
static __s32 _main_menu_Proc(__gui_msg_t *msg)
{
    home_uipara_t *home_ui_para;

    home_ui_para = home_get_ui_para(GUI_GetScnDir());
    if(!home_ui_para)
    {
        __err("home_ui_para is null..\n");
        return ;
    }

    switch(msg->id)
    {
    case GUI_MSG_CREATE:
    {
        mmenu_attr_t *mmenu_attr;
        mmenu_para_t *mmenu_para;
		
        __log("menu_GUI_MSG_CREATE: %d\n", home_ui_para->item_width);
        mmenu_para = (mmenu_para_t *)GUI_WinGetAttr(msg->h_deswin);

        mmenu_attr = (mmenu_attr_t *)esMEMS_Balloc(sizeof(mmenu_attr_t));
        if(!mmenu_attr)
        {
            return EPDK_FAIL;
        }
        eLIBs_memset((void *)mmenu_attr, 0, sizeof(mmenu_attr));
        init_mmenu_res(mmenu_attr);

        mmenu_attr->font = mmenu_para->mmenu_font;
        mmenu_attr->layer = mmenu_para->layer;
        mmenu_attr->item_w = home_ui_para->item_width;
        mmenu_attr->focus_item = mmenu_para->focus_id;
        mmenu_attr->first_item = 0;
        mmenu_attr->focus_txt_color = APP_COLOR_YELLOW;			//焦点颜色
        mmenu_attr->unfocus_txt_color = APP_COLOR_WHITE;		//非焦点颜色
        mmenu_attr->res_init = EPDK_FALSE;


        GUI_WinSetAddData(msg->h_deswin, (__s32)mmenu_attr);

#if EPDK_AUTO_PLAY_MOVIE_ENABLE
        main_cmd2parent(msg->h_deswin, ID_OP_SUB_ENTER, mmenu_attr->focus_item, mmenu_attr->first_item);
#endif
    }
    return EPDK_OK;
    case GUI_MSG_CLOSE:
    {
        GUI_FrmWinDelete(msg->h_deswin);
    }
    return EPDK_OK;
    case GUI_MSG_DESTROY:
    {
        mmenu_para_t *mmenu_para;
        mmenu_attr_t *mmenu_attr;

        if(GUI_IsTimerInstalled(msg->h_deswin, main_menu_timmer_id))
        {
            GUI_KillTimer(msg->h_deswin, main_menu_timmer_id);
        }

        mmenu_para = (mmenu_para_t *)GUI_WinGetAttr(msg->h_deswin);
        mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);
        uninit_mmenu_res(mmenu_attr);

        esMEMS_Bfree(mmenu_attr, sizeof(mmenu_attr_t));
        esMEMS_Bfree(mmenu_para, sizeof(mmenu_para));
    }
    return EPDK_OK;
    case GUI_MSG_PAINT:
    {
        mmenu_attr_t *mmenu_attr;

        if (GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)))
        {
            __msg("layer not on, not need paint...\n");
            return EPDK_OK;
        }

        mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(msg->h_deswin);

        if (!mmenu_attr)
        {
            __err("invalid para...\n");
            return EPDK_FAIL;
        }
		__msg("mmenu_attr->focus_item = %d\n", mmenu_attr->focus_item);

        init_mmenu_res(mmenu_attr);

        paint_mmain_item_all(mmenu_attr);
        return EPDK_OK;
    }
    case GUI_MSG_KEY:
    {
        __here__;
        mmenu_key_proc(msg);
        return EPDK_OK;
    }
    case GUI_MSG_TOUCH:
    {
        //mmenu_touch_proc(msg);
        return EPDK_OK;
    }
    case GUI_MSG_TIMER:
    {
        //__msg("main_menu_proc, GUI_MSG_TIMER\n");
        return EPDK_OK;
    }
    case GUI_MSG_COMMAND:
        return EPDK_OK;
    case GUI_MSG_WIN_WAKEUP:
        GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
        return EPDK_OK;
    }

    return GUI_FrmWinDefaultProc(msg);
}

__s32 main_menu_win_create(H_WIN h_parent, mmenu_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    mmenu_para_t *mmenu_para;
    FB fb;

    GUI_LyrWinGetFB(para->layer, &fb);

    mmenu_para = (mmenu_para_t *)esMEMS_Balloc(sizeof(mmenu_para_t));
    if(!mmenu_para)
    {
        return EPDK_FAIL;
    }
    eLIBs_memset((void *)mmenu_para, 0, sizeof(mmenu_para_t));
    mmenu_para->mmenu_font = para->mmenu_font;
    mmenu_para->layer = para->layer;
    mmenu_para->focus_id = para->focus_id;

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =	"main_menu_win",
                  framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner	= NULL;
    framewin_para.id         = MAIN_MENU_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_main_menu_Proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr =  (void *)mmenu_para;
    framewin_para.hLayer = para->layer;

    return (GUI_FrmWinCreate(&framewin_para));
}

void main_menu_win_delete(H_WIN mm_win)
{
    GUI_FrmWinDelete(mm_win);
}

void main_menu_res_uninit(H_WIN hwin)
{
    mmenu_attr_t *mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(hwin);

    if(mmenu_attr)
    {
        uninit_mmenu_res(mmenu_attr);
    }
}

void main_menu_res_init(H_WIN hwin)
{
    mmenu_attr_t *mmenu_attr = (mmenu_attr_t *)GUI_WinGetAddData(hwin);

    if(mmenu_attr)
    {
        init_mmenu_res(mmenu_attr);
    }
}

