#include "record_list_view.h"
#include "record_comm.h"

static void record_list_view_time( precord_list_view_t prec_list_view )
{
    char ucDateBuf[32] = {0};
    char ucTimeBuf[16] = {0};
    __time_t time;
    __date_t date;

    eLIBs_GetDate( &date );
    eLIBs_GetTime( &time );

    record_date_to_char(ucDateBuf, date, '-');
    record_time_to_char(ucTimeBuf, time, ':');

    eLIBs_strcat(ucDateBuf, " ");
    eLIBs_strcat(ucDateBuf, ucTimeBuf);

    GUI_SetColor(APP_COLOR_WHITE);
    GUI_DispStringAt( ucDateBuf, 115, 0 );
}

static void record_list_view_Top( precord_list_view_t prec_list_view )
{
    char vol[5] = {0};
    power_level_e batterySt = DSK_POWER_LEVEL_0;

    draw_record_bmp( prec_list_view->pui, RECORD_BAR_TOP_BMP );
    draw_record_lang( prec_list_view->pui, STRING_RECORD_PLAY, APP_COLOR_WHITE );

    if( dsk_volume_is_on() )
        draw_record_bmp( prec_list_view->pui, RECORD_VOL_ON_BMP );
    else
        draw_record_bmp( prec_list_view->pui, RECORD_VOL_OFF_BMP );
    eLIBs_int2str_dec(dsk_volume_get(), vol);
    GUI_SetColor(APP_COLOR_WHITE);
    GUI_DispStringAt( vol, 300, 0 );

    dsk_power_get_voltage_level(&batterySt);
    draw_record_bmp( prec_list_view->pui, RECORD_BATTERY_0_BMP + batterySt );
    record_list_view_time(prec_list_view);
}

static void record_list_view_MusicList( precord_list_view_t prec_list_view )
{
    __s32 		cnt = 0, curIndex, topIndex, total;
    __s32		scrollStartY, scrollEndY, height = 194;
    GUI_RECT 	rect = {0, 42, 160, 58};
    char 		str[50];

    //ÒôÀÖ¾íÖá
    draw_record_bmp( prec_list_view->pui, RECORD_MUSIC_PROGRESS_BG_BMP );
    draw_record_bmp( prec_list_view->pui, RECORD_MUSIC_PROGRESS_UP_BMP );
    draw_record_bmp( prec_list_view->pui, RECORD_MUSIC_PROGRESS_DOWN_BMP );
    //¸èÇú
    if( prec_list_view->precord_list_doc == NULL )
        return;

    curIndex 	= prec_list_view->precord_list_doc->curIndex;
    topIndex 	= prec_list_view->precord_list_doc->topIndex;
    total    	= prec_list_view->precord_list_doc->total;

    for(cnt = topIndex;	cnt < total && cnt < RECORD_MUSIC_LIST_NUM + topIndex; cnt++)
    {
        record_list_getFileName( prec_list_view->precord_list_doc, cnt, str);

        if( curIndex == cnt )
        {
            draw_record_bmp_coor( prec_list_view->pui, RECORD_MUSIC_PROGRESS_FOCUS_BMP, rect.x0, rect.y0 - 2 );
        }
        draw_record_str( prec_list_view->pui, str, APP_COLOR_WHITE, &rect );

        rect.y0 += RECORD_MUSIC_NAME_HEIGHT;
        rect.y1 += RECORD_MUSIC_NAME_HEIGHT;
    }
    if( total > RECORD_MUSIC_LIST_NUM )
    {
        scrollStartY = height * topIndex / total;
        scrollEndY = height * (RECORD_MUSIC_LIST_NUM + topIndex) / total;
        for( ; scrollStartY < scrollEndY; scrollStartY++)
        {
            draw_record_bmp_coor( prec_list_view->pui, RECORD_MUSIC_PROGRESS_FLAG_BMP, 163, scrollStartY + 33 );
        }
    }
}

static void record_list_view_MusicMenu( precord_list_view_t prec_list_view )
{
    FOCUSE_STATE musicList[MUSIC_MENU_MAX] = {FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL};
    RECORD_LIST_FOCUSE focuse;

    if( prec_list_view->curPart == RECORD_MUSIC_VOLUME )
        focuse = prec_list_view->oldPart;
    else
        focuse = prec_list_view->curPart;

    if(focuse == RECORD_MUSIC_LIST )
    {
        __s16 xCoor = 170, yCoor = 50, i, imgH = 60;
        __u32 height;
        for( i = 0; i < RECORD_LIST_SPCTRM_LEVEL; i++)
        {
            height = prec_list_view->precord_list_doc->spctrmLevel[i] * imgH / 0xF;
            if( prec_list_view->precord_list_doc->spctrmLevel[i] > 0xF )
            {
                height = imgH;
            }
            draw_record_bmp_cut( prec_list_view->pui, RECORD_MUSIC_PARTICLE, xCoor, yCoor, height );
            height = prec_list_view->precord_list_doc->oldSpctrmLevel[i] * imgH / 0xF;
            if( prec_list_view->precord_list_doc->oldSpctrmLevel[i] > 0xF )
            {
                height = imgH;
            }
            draw_record_bmp_coor( prec_list_view->pui, RECORD_MUSIC_BOUY, xCoor, yCoor + imgH - height );
            xCoor += 11;
        }
    }
    else
    {
        musicList[prec_list_view->curMenu] = prec_list_view->curMenuSt;

        draw_record_music_mapping( prec_list_view->pui, MUSIC_MENU_BACK,  musicList[MUSIC_MENU_BACK] );
        draw_record_music_mapping( prec_list_view->pui, MUSIC_MENU_BACKWARD,   musicList[MUSIC_MENU_BACKWARD] );

        draw_record_music_mapping( prec_list_view->pui, prec_list_view->playMenu,	musicList[prec_list_view->playMenu] );

        draw_record_music_mapping( prec_list_view->pui, MUSIC_MENU_FORWARD,	musicList[MUSIC_MENU_FORWARD] );
        draw_record_music_mapping( prec_list_view->pui, MUSIC_MENU_DEL, 	musicList[MUSIC_MENU_DEL] );
    }
}

static void record_list_view_MusicPlayer( precord_list_view_t prec_list_view )
{
    char times[10] = {0};
    __u32 cnt = 0, len = 0;
    __s16 offsetX = 0, offsetY = 0;

    //²¥·Å½ø¶È±³¾°
    draw_record_bmp( prec_list_view->pui, RECORD_66_BMP );
    if( prec_list_view->precord_list_doc->musicMaxTimes == 0 )
    {
        return;
    }

    if( prec_list_view->playMenu == MUSIC_MENU_PAUSE )
    {
        draw_record_lang( prec_list_view->pui, STRING_PLAYING, APP_COLOR_WHITE );
    }
    else if( prec_list_view->playMenu == MUSIC_MENU_PLAY )
    {
        draw_record_lang( prec_list_view->pui, STRING_PAUSE, APP_COLOR_WHITE );
    }

    GUI_SetColor(APP_COLOR_WHITE);
    record_ms_to_char(times, prec_list_view->precord_list_doc->musicMaxTimes, ':');
    GUI_DispStringAt( times, 334, 193 );
    record_ms_to_char(times, prec_list_view->precord_list_doc->musicCurTimes, ':');
    GUI_DispStringAt( times, 170, 193 );

    //42 = 170/4
    len = (prec_list_view->precord_list_doc->musicCurTimes * 42) / prec_list_view->precord_list_doc->musicMaxTimes;
    for( cnt = 0; cnt < len; cnt++ )
    {
        if( cnt == 0 )
        {
            draw_record_bmp_ext( prec_list_view->pui, RECORD_66_LEFT_BMP,  offsetX, offsetY );
        }
        else if( cnt == 42 )
        {
            draw_record_bmp_ext( prec_list_view->pui, RECORD_66_MID_BMP,  offsetX, offsetY );
            offsetX += 2;
            draw_record_bmp_ext( prec_list_view->pui, RECORD_66_RIGHT_BMP,  offsetX, offsetY );
        }
        else
        {
            draw_record_bmp_ext( prec_list_view->pui, RECORD_66_MID_BMP,  offsetX, offsetY );
        }
        offsetX += 4;
    }
}

static void record_list_view_delBox( precord_list_view_t prec_list_view )
{
    __s16 offsetX = 0, offsetY = 0;

    if( prec_list_view->delState == RECORD_DEL_NONE )
        return;

    draw_record_bmp( prec_list_view->pui, RECORD_DEL_BOX_BMP );

    draw_record_bmp_ext( prec_list_view->pui, RECORD_DEL_SEL0_0_BMP + (prec_list_view->delState == RECORD_DEL_OK), offsetX, offsetY );
    offsetX += 71;
    draw_record_bmp_ext( prec_list_view->pui, RECORD_DEL_SEL0_0_BMP + (prec_list_view->delState == RECORD_DEL_CAL), offsetX, offsetY );
    draw_record_lang( prec_list_view->pui, STRING_YES, APP_COLOR_WHITE );
    draw_record_lang( prec_list_view->pui, STRING_NO, APP_COLOR_WHITE );
    draw_record_lang( prec_list_view->pui, STRING_DELETE, APP_COLOR_WHITE );
    draw_record_lang_wrap( prec_list_view->pui, STRING_MSG4, APP_COLOR_WHITE );
}

static void record_list_view_volBar( precord_list_view_t prec_list_view )
{
    char volStr[5] = {0};
    __s16 volume = 0, i = 0, volX = 76, volY = 228;
    if( prec_list_view->curPart == RECORD_MUSIC_VOLUME )
    {
        draw_record_bmp( prec_list_view->pui, RECORD_VOL_BAR_BG );
        draw_record_bmp( prec_list_view->pui, RECORD_VOL_BAR_BODY );
        draw_record_bmp( prec_list_view->pui, RECORD_VOL_SPEAKER_RIGHT );
        draw_record_bmp_coor( prec_list_view->pui, RECORD_VOL_ON_BMP, 39, 224 );

        volume = dsk_volume_get();
        eLIBs_int2str_dec(volume, volStr);
        GUI_SetColor(APP_COLOR_WHITE);
        GUI_DispStringAt( volStr, 363, 222 );
        for( i = 0; i < (volume * 240 / 30) / 5; i++ )
        {
            if( i == 0 )
            {
                draw_record_bmp_coor( prec_list_view->pui, RECORD_VOL_LEFT, volX, volY );
            }
            else if( i == 47 )
            {
                draw_record_bmp_coor( prec_list_view->pui, RECORD_VOL_RIGHT, volX, volY );
            }
            else
            {
                draw_record_bmp_coor( prec_list_view->pui, RECORD_VOL_MID, volX, volY );
            }
            volX += 5;
        }
    }
}

void record_list_view_show(precord_list_view_t prec_list_view, H_WIN layer)
{
    GUI_MEMDEV_Handle	draw_mem;
    __s16 x1, y1;
    __s16 x0 = 0, y0 = 0, w = 400, h = 240;

    if (GUI_LyrWinGetSta(layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        __msg("&&&&&&&&&&&&&&&&&&&   GUI_LYRWIN_STA_SUSPEND! &&&&&&&&&&&&&&&&&&& \n");
        return;
    }

    x1 = w + x0 - 1;
    y1 = h + y0 - 1;
    GUI_LyrWinSel(layer);
    draw_mem = GUI_MEMDEV_Create(x0, y0, w,  h);
    if( draw_mem == 0 )
    {
        __msg("&&&&&&&&&&&&&&&&&&&   Create Memory drever fail! &&&&&&&&&&&&&&&&&&& \n");
        return;
    }
    GUI_MEMDEV_Select(draw_mem);

    GUI_SetBkColor(0x00);
    GUI_ClearRect(x0, y0, x1,  y1);


    record_list_view_Top(prec_list_view);
    record_list_view_MusicMenu(prec_list_view);
    record_list_view_MusicPlayer(prec_list_view);
    record_list_view_MusicList(prec_list_view);
    record_list_view_delBox( prec_list_view );
    record_list_view_volBar( prec_list_view );

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select( NULL );
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
}


























