/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
* File    	: 		music_lyrics.c
*descriptions:		显示歌词
*lib function list:	robin_lyric_exist();robin_get_lyric_format();robin_get_lyric_item_count();robin_get_lyric_item_list();
*
*		   		用户指定图层和显示区域
*              		显示方式
* By      	: 		Libaiao
* Func	:
* Version	: 		v1.0
* ============================================================================================================
* 2010-6-1 15:24:52  Libaiao  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "beetles_app.h"

#include "music.h"
#include "music_ui.h"
#include "music_list.h"
#include "music_player.h"

#include "music_lyrics.h"


/***************************************************************************************************
*Name        : __bool music_lyrics_exist(void)
*Prototype   :
*Arguments   : void
*Return      :  EPDK_TRUE      there are lyrics corresponding to the current playing file.
*              	EPDK_FALSE     there is NO lyric corresponding to the current playing file.
*Description : check whether there are lyrics corresponding to the current playing file or not.
*Other       :
***************************************************************************************************/

__bool music_lyrics_exist(void)
{

    return robin_lyric_exist();
}

static __epdk_charset_enm_e  _EncodingMap( __s32 cedar_encode )
{
    switch( cedar_encode )
    {
    case CEDAR_SUB_ENCODE_UTF8:
        return EPDK_CHARSET_ENM_UTF8;

    case CEDAR_SUB_ENCODE_GB2312:
        return EPDK_CHARSET_ENM_GBK;

    case CEDAR_SUB_ENCODE_UTF16LE:
        return EPDK_CHARSET_ENM_UTF16LE;

    case CEDAR_SUB_ENCODE_UTF16BE:
        return EPDK_CHARSET_ENM_UTF16BE;

    case CEDAR_SUB_ENCODE_BIG5:
        return EPDK_CHARSET_ENM_BIG5;

    case CEDAR_SUB_ENCODE_GBK:
        return EPDK_CHARSET_ENM_GBK;

    default:
        return EPDK_CHARSET_ENM_GBK;
        //return SPSC_GetSysDefaultCharset();//dsk_get_isn_charset();//112357接口返回错误
    }
}

static __s32  charset_enm2gui( __s32 charset_enm )
{
    switch( charset_enm )
    {
    case EPDK_CHARSET_ENM_GBK         :
        GUI_UC_SetEncodeGBK();
        return 0;
    case EPDK_CHARSET_ENM_BIG5        :
        GUI_UC_SetEncodeBig5();
        return 0;
    case EPDK_CHARSET_ENM_UTF8        :
        GUI_UC_SetEncodeUTF8();
        return 0;
    case EPDK_CHARSET_ENM_UTF16BE     :
    case EPDK_CHARSET_ENM_UTF16LE     :
        return -1;
    case EPDK_CHARSET_ENM_SJIS        :
        GUI_UC_SetEncodeSJIS();
        return 0;
    case EPDK_CHARSET_ENM_EUC_JP      :
        GUI_UC_SetEncodeEUCJP();
        return 0;
    case EPDK_CHARSET_ENM_EUC_KR      :
        GUI_UC_SetEncodeEUCKR();
        return 0;
    case EPDK_CHARSET_ENM_KIO8_R      :
        GUI_UC_SetEncodeKIO8R();
        return 0;
    case EPDK_CHARSET_ENM_ISO_8859_1  :
        GUI_UC_SetEncodeISO88591();
        return 0;
    case EPDK_CHARSET_ENM_ISO_8859_2  :
        GUI_UC_SetEncodeISO88592();
        return 0;
    case EPDK_CHARSET_ENM_ISO_8859_3  :
        GUI_UC_SetEncodeISO88593();
        return 0;
    case EPDK_CHARSET_ENM_ISO_8859_4  :
        GUI_UC_SetEncodeISO88594();
        return 0;
    case EPDK_CHARSET_ENM_ISO_8859_5  :
        GUI_UC_SetEncodeISO88595();
        return 0;
    case EPDK_CHARSET_ENM_ISO_8859_6  :
        GUI_UC_SetEncodeISO88596();
        return 0;
    case EPDK_CHARSET_ENM_ISO_8859_7  :
        GUI_UC_SetEncodeISO88597();
        return 0;
    case EPDK_CHARSET_ENM_ISO_8859_8  :
        GUI_UC_SetEncodeISO88598();
        return 0;
    case EPDK_CHARSET_ENM_ISO_8859_9  :
        GUI_UC_SetEncodeISO88599();
        return 0;
    case EPDK_CHARSET_ENM_ISO_8859_10 :
        GUI_UC_SetEncodeISO885910();
        return 0;
    case EPDK_CHARSET_ENM_ISO_8859_11 :
        GUI_UC_SetEncodeISO885911();
        return 0;
    case EPDK_CHARSET_ENM_ISO_8859_12 :
        GUI_UC_SetEncodeISO885912();
        return 0;
    case EPDK_CHARSET_ENM_ISO_8859_13 :
        GUI_UC_SetEncodeISO885913();
        return 0;
    case EPDK_CHARSET_ENM_ISO_8859_14 :
        GUI_UC_SetEncodeISO885914();
        return 0;
    case EPDK_CHARSET_ENM_ISO_8859_15 :
        GUI_UC_SetEncodeISO885915();
        return 0;
    case EPDK_CHARSET_ENM_ISO_8859_16 :
        GUI_UC_SetEncodeISO885916();
        return 0;
    case EPDK_CHARSET_ENM_CP874       :
        GUI_UC_SetEncodeCP874();
        return 0;
    case EPDK_CHARSET_ENM_CP1250      :
        GUI_UC_SetEncodeCP1250();
        return 0;
    case EPDK_CHARSET_ENM_CP1251      :
        GUI_UC_SetEncodeCP1251();
        return 0;
    case EPDK_CHARSET_ENM_CP1253      :
        GUI_UC_SetEncodeCP1253();
        return 0;
    case EPDK_CHARSET_ENM_CP1255      :
        GUI_UC_SetEncodeCP1255();
        return 0;
    case EPDK_CHARSET_ENM_CP1256      :
        GUI_UC_SetEncodeCP1256();
        return 0;
    case EPDK_CHARSET_ENM_CP1257      :
        GUI_UC_SetEncodeCP1257();
        return 0;
    case EPDK_CHARSET_ENM_CP1258      :
        GUI_UC_SetEncodeCP1258();
        return 0;
    default :
        GUI_UC_SetEncodeGBK();
        return -1;
    }
}


/***************************************************************************************************
*Name       :  robin_get_lyric_item
*Prototype : 	__cedar_subtitle_item_t  robin_get_lyric_item( __cedar_subtitle_item_t  *lyric_item_list, __u32 time )
*Arguments:
*time        : 	input. the current time
*item_count: total number of item list
*Return     : 	string of the correspond item
*
*Description: 	get lyric item point
*Other       :
***************************************************************************************************/
#define	 C_BACKWARD_ITEM_COUNT		5			//向后搜索时每次倒退5个条目

__cedar_subtitle_item_t  *robin_get_lyric_item( __cedar_subtitle_item_t  *lyric_item_list, __u32 item_count, __u32 time)
{
    static __u32 last_item_index = 0;
    __cedar_subtitle_item_t *item = NULL;
    __u32 i = 0;
    __u32 out_cnt = 0;
    if(last_item_index > item_count)
    {
        last_item_index = 0;
    }

    for(i = last_item_index; i < item_count; i++)
    {
        if(time >= lyric_item_list[i].uPts)
        {
            if(time < lyric_item_list[i].uPts + lyric_item_list[i].uDuration)
            {
                last_item_index = i;
                return &(lyric_item_list[i]);
            }
        }
        else		//当前时间小于上一次条目的时间，那么表示有快退动作
        {
            if(i == 0)
            {
                last_item_index = i;
                return &(lyric_item_list[i]);
            }
            if(i > C_BACKWARD_ITEM_COUNT)
            {
                i = i - C_BACKWARD_ITEM_COUNT;
            }
            else
            {
                i = 0;
                out_cnt++;
                if(out_cnt > 3)			//时间还未到第一个条目
                {
                    last_item_index = i;
                    return &(lyric_item_list[i]);
                }
            }
        }
    }
    return item;
}



H_LYRIC new_music_lyrics(music_lyrics_para_t *para)
{
    H_LYRIC hLyric = NULL;
    __s32 i = 0, total = 0;


    total = robin_get_lyric_item_count();
    if(total == -1)
    {
        __msg("no lyric file!\n");
        return NULL;					//没有歌词文件
    }
    if(para == NULL)
    {
        return NULL;
    }
    //app_print_memory_usage("new music lyrics", NULL);

    hLyric = (H_LYRIC)esMEMS_Balloc(sizeof(music_lyrics_t));			//
    if(hLyric == NULL)
    {
        __msg("memory is not enough for displaying lyrics!\n");
        return NULL;
    }
    eLIBs_memset((void *)hLyric, 0, sizeof(music_lyrics_t));

    hLyric->total_item = total;
    hLyric->items = (__cedar_subtitle_item_t *)esMEMS_Balloc(sizeof(__cedar_subtitle_item_t) * total);
    eLIBs_memset((void *)(hLyric->items), 0, sizeof(__cedar_subtitle_item_t) * total);

    if(hLyric->items == NULL)
    {
        __msg("memory is not enough for displaying lyrics!\n");
        esMEMS_Bfree(hLyric, sizeof(music_lyrics_t));
        return NULL;
    }
    if(para != NULL)
    {
        eLIBs_memcpy((void *)(&(hLyric->UI)), para, sizeof(music_lyrics_para_t));	//UI 参数
    }
    else
    {
        __wrn("there is no parametters!\n");
    }
    robin_get_lyric_item_list(hLyric->items, total);			//获得所有的歌词列表

    hLyric->char_code = _EncodingMap(hLyric->items->uDataType);	//获得字体编码

    //GUI_ClearRect(hLyric->UI.area.x,hLyric->UI.area.y,hLyric->UI.area.x + hLyric->UI.area.width, hLyric->UI.area.y + hLyric->UI.area.height);
    {
        __u32 font_height = 0, i = 0;
        __u32 X, Y;
        font_height = GUI_GetFontSizeY();
        hLyric->line_count = hLyric->UI.area.height / font_height;
        //__msg();
        X =  hLyric->UI.area.x;
        Y =  hLyric->UI.area.y;
        for(i = 0; i < hLyric->line_count; i++)
        {
            hLyric->line[i].area.x0 = X;
            hLyric->line[i].area.y0 = Y;
            hLyric->line[i].area.x1 = X + hLyric->UI.area.width;
            hLyric->line[i].area.y1 = Y + font_height;
            Y += font_height;
        }
    }

    return hLyric;
}

__s32 delete_music_lyrics(H_LYRIC hLyric)
{
    if(hLyric == NULL)
    {
        return EPDK_FAIL;
    }
    if(hLyric->total_item != 0)
    {
        esMEMS_Bfree((void *)(hLyric->items), sizeof(__cedar_subtitle_item_t) * hLyric->total_item);
    }
    esMEMS_Bfree(hLyric, sizeof(music_lyrics_t));

    GUI_UC_SetEncodeUTF8();

    return EPDK_OK;
}

__s32 clear_music_lyrics_area(H_LYRIC hLyric)
{
    if(hLyric == NULL)
    {
        return EPDK_FAIL;
    }

    GUI_ClearRect(hLyric->UI.area.x, hLyric->UI.area.y, hLyric->UI.area.x + hLyric->UI.area.width, hLyric->UI.area.y + hLyric->UI.area.height);
    return EPDK_OK;
}

#if 0
//更新显示歌词
__s32 update_music_lyrics(H_LYRIC hLyric)
{
    __u32 time = 0, width = 0;
    __cedar_subtitle_item_t *item = NULL;
    if(hLyric == NULL)
    {
        return EPDK_FAIL;
    }

    time = robin_get_cur_time();
    if(time < 500)
    {
        return EPDK_OK;
    }

    item = robin_get_lyric_item(hLyric->items, hLyric->total_item, time);
    if(item != NULL)
    {
        if(item == hLyric->last_item)
        {
            return EPDK_OK;
        }
        if(item < hLyric->last_item)
        {
            hLyric->hl_count = 0;
        }
        hLyric->last_item = item;

        //char text[128];
        if(hLyric->UI.layer != NULL)				//选择用户图层
        {
            //GUI_LyrWinSel(hLyric->UI.layer);
        }

        GUI_SetBkColor(0x00000000);
        //GUI_SetColor(hLyric->UI.font_color);

        //GUI_ClearRect(area.x0,area.y0, area.x1, area.y1);
        charset_enm2gui(hLyric->char_code);

        //eLIBs_strcpy(text, (char *)(item->pSubtitle));
        //GUI_DispStringInRect(text, &area, GUI_TA_HCENTER|GUI_TA_VCENTER);

        //if(hLyric->line_count >= 5)					//only for test
        {
            __u32 len = 0, i  = 0;
            GUI_MEMDEV_Handle	draw_mem;
            draw_mem = GUI_MEMDEV_Create(hLyric->UI.area.x, hLyric->UI.area.y, hLyric->UI.area.width, hLyric->UI.area.height);
            GUI_MEMDEV_Select(draw_mem);

            if(hLyric->hl_count > 2)
            {
                hLyric->hl_count = 0;
            }

            GUI_ClearRect(hLyric->UI.area.x, hLyric->UI.area.y, hLyric->UI.area.x + hLyric->UI.area.width, hLyric->UI.area.y + hLyric->UI.area.height);

            width  = hLyric->UI.area.width;
            if(width > 10)
            {
                width -= 10;
            }
            if(hLyric->hl_count == 0)			//从新开始显示歌词
            {
                hLyric->hl_count = 1;
                i = 0;
                hLyric->last_focus_item = item;
                //width  = hLyric->UI.area.width;
                GUI_SetColor(hLyric->UI.hight_light_color);
                len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle), width);
                if(len != 0)
                {
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);

                    i++;
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle + len), width);
                    if(len != 0)
                    {
                        GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    }
                }

                i = 2;

                item ++;//获得下一个条目,要判断是否结束
                hLyric->last_normal_item = item;
                //width  = hLyric->UI.area.width;
                GUI_SetColor(hLyric->UI.font_color);
                len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle), width);
                if(len != 0)
                {
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    i++;
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle + len), width);
                    if(len != 0)
                    {
                        GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    }
                }
            }
            else if(hLyric->hl_count == 1)		//normal 与 focus互换
            {

                i = 0;
                //focus 变成normal
                //width  = hLyric->UI.area.width;
                GUI_SetColor(hLyric->UI.font_color);
                len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(hLyric->last_focus_item->pSubtitle), width);
                if(len != 0)
                {
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    i++;
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(hLyric->last_focus_item->pSubtitle + len), width);
                    if(len != 0)
                    {
                        GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    }
                }


                i = 2;

                hLyric->last_normal_item = hLyric->last_focus_item;
                hLyric->last_focus_item = item;

                //width  = hLyric->UI.area.width;
                GUI_SetColor(hLyric->UI.hight_light_color);
                len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle), width);
                if(len != 0)
                {
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    i++;
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle + len), width);
                    if(len != 0)
                    {
                        GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    }
                }

                hLyric->hl_count = 2;

            }
            else if(hLyric->hl_count == 2)
            {
                hLyric->hl_count = 1;
                i = 2;
                //上一次的焦点变为普通

                //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                //width  = hLyric->UI.area.width;
                GUI_SetColor(hLyric->UI.font_color);
                len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(hLyric->last_focus_item->pSubtitle), width);
                if(len != 0)
                {
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    i++;
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(hLyric->last_focus_item->pSubtitle + len), width);
                    if(len != 0)
                    {
                        GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    }
                }
                hLyric->last_normal_item = hLyric->last_focus_item;

                i = 0;
                hLyric->last_focus_item = item;
                //width  = hLyric->UI.area.width;
                GUI_SetColor(hLyric->UI.hight_light_color);
                len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle), width);
                if(len != 0)
                {
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    i++;
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle + len), width);
                    if(len != 0)
                    {
                        GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    }
                }
            }
            GUI_MEMDEV_CopyToLCD(draw_mem);
            GUI_MEMDEV_Select( NULL );
            GUI_MEMDEV_Delete(draw_mem);
            draw_mem = NULL;

        }
    }
    return EPDK_OK;
}
#endif

//在一个矩形区域内自动滚动显示所有歌词
__s32 update_music_lyrics(H_LYRIC hLyric)
{
    __u32 time = 0, width = 0;
    __cedar_subtitle_item_t *item = NULL;
    if(hLyric == NULL)
    {
        return EPDK_FAIL;
    }

    time = robin_get_cur_time();
    if(time < 500)
    {
        return EPDK_OK;							//前500ms获得不到歌词
    }

    item = robin_get_lyric_item(hLyric->items, hLyric->total_item, time);//获得高亮显示的item
    if(item != NULL)
    {
        if(item == hLyric->last_item)
        {
            return EPDK_OK;						//不需要更新歌词
        }
        if(item < hLyric->last_item)
        {
            hLyric->hl_count = 0;				//从零开始显示
        }
        hLyric->last_item = item;

        //char text[128];
        if(hLyric->UI.layer != NULL)				//选择用户图层
        {
            //GUI_LyrWinSel(hLyric->UI.layer);
        }

        GUI_SetBkColor(0x00000000);
        //GUI_SetColor(hLyric->UI.font_color);

        //GUI_ClearRect(area.x0,area.y0, area.x1, area.y1);
        charset_enm2gui(hLyric->char_code);

        //eLIBs_strcpy(text, (char *)(item->pSubtitle));
        //GUI_DispStringInRect(text, &area, GUI_TA_HCENTER|GUI_TA_VCENTER);

        //if(hLyric->line_count >= 5)					//only for test
        {
            __u32 len = 0, i  = 0;
            GUI_MEMDEV_Handle	draw_mem;
            draw_mem = GUI_MEMDEV_Create(hLyric->UI.area.x, hLyric->UI.area.y, hLyric->UI.area.width, hLyric->UI.area.height);
            GUI_MEMDEV_Select(draw_mem);

            if(hLyric->hl_count > 2)
            {
                hLyric->hl_count = 0;
            }

            GUI_ClearRect(hLyric->UI.area.x, hLyric->UI.area.y, hLyric->UI.area.x + hLyric->UI.area.width, hLyric->UI.area.y + hLyric->UI.area.height);

            width  = hLyric->UI.area.width;
            if(width > 16)
            {
                width -= 16;
            }

            {
                //hLyric->hl_count= 1;
                i = 0;
                hLyric->last_focus_item = item;
                //width  = hLyric->UI.area.width;
                GUI_SetColor(hLyric->UI.hight_light_color);
                len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle), width);
                if(len != 0)
                {
                    //__msg("first line test = %s\n", hLyric->line[i].text);
                    GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);

                    i++;
                    len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle + len), width);
                    if(len != 0)
                    {
                        //__msg("second line test = %s\n", hLyric->line[i].text);
                        GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    }
                }

                i = 2;

                if(item != (hLyric->items + (hLyric->total_item - 1)))
                {
                    item ++;//获得下一个条目,要判断是否结束
                    hLyric->last_normal_item = item;
                    GUI_SetColor(hLyric->UI.font_color);
                    len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle), width);
                    if(len != 0)
                    {
                        //__msg("third line test = %s\n", hLyric->line[i].text);
                        GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                        i++;
                        __msg("first line text len = %d\n", len);
                        len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle + len), width);
                        __msg("second line text len = %d, width = %d\n", len, width);
                        if(len != 0)
                        {
                            __msg("second line test = %s\n", hLyric->line[i].text);
                            __msg("second line area x0 = %d, y0 = %d, x1 = %d, y1 = %d\n", hLyric->line[i].area.x0, hLyric->line[i].area.y0, hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                            GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                        }
                    }
                }
            }
            GUI_MEMDEV_CopyToLCD(draw_mem);
            GUI_MEMDEV_Select( NULL );
            GUI_MEMDEV_Delete(draw_mem);
            draw_mem = NULL;

        }
    }
    return EPDK_OK;
}



