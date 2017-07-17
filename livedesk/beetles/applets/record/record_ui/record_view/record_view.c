

#include "record_view.h"
#include "record_comm.h"


static void record_view_Bottom( precord_view_t prec_view )
{
    FOCUSE_STATE menuList[MENU_MAX] = {FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL};
    GUI_RECT rect = {145, 215, 198, 240};
    GUI_COLOR txt_color;
    rect.x0 += (prec_view->menuOsffset * 91);
    rect.x1 += (prec_view->menuOsffset * 91);

    menuList[prec_view->curMenu] = prec_view->curMenuSt;

    draw_record_bmp( prec_view->pui, RECORD_BAR_BOTTOM_BMP );

    if( prec_view->menuOsffset == 0 )
        draw_record_mapping( prec_view->pui, MENU_QUALITY - prec_view->menuOsffset,    menuList[MENU_QUALITY - prec_view->menuOsffset], prec_view->menuOsffset );
    else
        draw_record_mapping( prec_view->pui, MENU_RECORD - prec_view->menuOsffset,    menuList[MENU_RECORD - prec_view->menuOsffset], prec_view->menuOsffset );

    if( prec_view->curMenu == MENU_RECORD )
        txt_color = APP_COLOR_YELLOW;
    else
        txt_color = APP_COLOR_WHITE;
    if( prec_view->precord_doc->recState == RECORD_STOP )
    {
        draw_record_bmp_ext( prec_view->pui, RECORD_MENU_RECORD_STOP, prec_view->menuOsffset * 94, 0 );
        draw_record_lang_ext( prec_view->pui, STRING_MENU_STOP, txt_color, &rect );
    }
    else if( prec_view->precord_doc->recState == RECORD_START )
    {
        draw_record_bmp_ext( prec_view->pui, RECORD_MENU_RECORD_START, prec_view->menuOsffset * 94, 0 );
        draw_record_lang_ext( prec_view->pui, STRING_MENU_RECORD, txt_color, &rect );
    }
    else
    {
        draw_record_bmp_ext( prec_view->pui, RECORD_MENU_RECORD_PAUSE, prec_view->menuOsffset * 94, 0 );
        draw_record_lang_ext( prec_view->pui, STRING_MENU_PAUSE, txt_color, &rect );
    }

    draw_record_mapping( prec_view->pui, MENU_SAVE - prec_view->menuOsffset,       menuList[MENU_SAVE - prec_view->menuOsffset], prec_view->menuOsffset );
    draw_record_mapping( prec_view->pui, MENU_MUSIC_LIST - prec_view->menuOsffset, menuList[MENU_MUSIC_LIST - prec_view->menuOsffset], prec_view->menuOsffset );
}

static void record_view_time( precord_view_t prec_view )
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

static void record_view_Top( precord_view_t prec_view )
{
    power_level_e batterySt = DSK_POWER_LEVEL_0;

    draw_record_bmp( prec_view->pui, RECORD_BAR_TOP_BMP );
    draw_record_lang( prec_view->pui, STRING_RECORDER, APP_COLOR_WHITE );
    //draw_record_bmp( prec_view->pui, RECORD_VOL_ON_BMP );
    dsk_power_get_voltage_level(&batterySt);
    draw_record_bmp( prec_view->pui, RECORD_BATTERY_0_BMP + batterySt );
    record_view_time(prec_view);
}

static void record_view_recInfo( precord_view_t prec_view )
{
    char curRecTimes[10] = {0};
    GUI_RECT rectName = {108, 114, 280, 136};
    GUI_RECT rectSize = {200, 114, 466, 136};
    GUI_RECT rectQuty = {2, 112, 106, 133};
    __s16 offsetX = 0, offsetY = 0;
    //可录时间
    draw_record_lang( prec_view->pui, STRING_RECORD_TIME, APP_COLOR_WHITE);
    GUI_SetColor(APP_COLOR_WHITE);
    if( prec_view->precord_doc->diskFreeTime >= 100 * 3600 )
    {
        GUI_DispStringAt( "> 100h  ", 100, 26 );
    }
    else
    {
        record_s_to_char(curRecTimes, prec_view->precord_doc->diskFreeTime, ':');
        GUI_DispStringAt( curRecTimes, 100, 26 );
    }
    //已录时间00:00:01
    record_ms_to_char(curRecTimes, prec_view->precord_doc->recordCurTimes, ':');
    draw_record_bmp_ext( prec_view->pui, curRecTimes[0] - '0' + RECORD_0_BMP, offsetX, offsetY );
    offsetX += 22;
    draw_record_bmp_ext( prec_view->pui, curRecTimes[1] - '0' + RECORD_0_BMP, offsetX, offsetY );
    offsetX += 22;

    draw_record_bmp_ext( prec_view->pui, curRecTimes[2] - '0' + RECORD_0_BMP, offsetX, offsetY );
    offsetX += 22;

    draw_record_bmp_ext( prec_view->pui, curRecTimes[3] - '0' + RECORD_0_BMP, offsetX, offsetY );
    offsetX += 22;
    draw_record_bmp_ext( prec_view->pui, curRecTimes[4] - '0' + RECORD_0_BMP, offsetX, offsetY );
    offsetX += 22;

    draw_record_bmp_ext( prec_view->pui, curRecTimes[5] - '0' + RECORD_0_BMP, offsetX, offsetY );
    offsetX += 22;

    draw_record_bmp_ext( prec_view->pui, curRecTimes[6] - '0' + RECORD_0_BMP, offsetX, offsetY );
    offsetX += 22;
    draw_record_bmp_ext( prec_view->pui, curRecTimes[7] - '0' + RECORD_0_BMP, offsetX, offsetY );
    offsetX += 22;
    //录制文件信息
    draw_record_lang_ext( prec_view->pui, prec_view->precord_doc->recQuality + STRING_POOR_QUALITY, APP_COLOR_WHITE, &rectQuty );
    draw_record_str( prec_view->pui, prec_view->precord_doc->recordName, APP_COLOR_WHITE, &rectName );
    eLIBs_int2str_dec(prec_view->precord_doc->fileSize, curRecTimes);
    eLIBs_strcat(curRecTimes, " KB");
    draw_record_str( prec_view->pui, curRecTimes, APP_COLOR_WHITE, &rectSize );
}

static void record_view_menuBox( precord_view_t prec_view )
{
    if( prec_view->curMenuSt == FOCUSE_PRESS )
    {
        switch(prec_view->curMenu)
        {
        case MENU_QUALITY:
            draw_record_bmp( prec_view->pui, RECORD_QUALITY_BK_BMP );
            draw_record_bmp_ext( prec_view->pui, RECORD_QUALITY_FLAG_BMP, 0, prec_view->precord_doc->recQuality * -27 );
            if( prec_view->precord_doc->recQualityTemp == 0 )
            {
                draw_record_lang( prec_view->pui, STRING_POOR_QUALITY, APP_COLOR_YELLOW );
                draw_record_lang( prec_view->pui, STRING_NORMAL_QUALITY, APP_COLOR_WHITE );
                draw_record_lang( prec_view->pui, STRING_GOOD_QUALITY, APP_COLOR_WHITE );
            }
            else if( prec_view->precord_doc->recQualityTemp == 1 )
            {
                draw_record_lang( prec_view->pui, STRING_POOR_QUALITY, APP_COLOR_WHITE );
                draw_record_lang( prec_view->pui, STRING_NORMAL_QUALITY, APP_COLOR_YELLOW );
                draw_record_lang( prec_view->pui, STRING_GOOD_QUALITY, APP_COLOR_WHITE );
            }
            else
            {
                draw_record_lang( prec_view->pui, STRING_POOR_QUALITY, APP_COLOR_WHITE );
                draw_record_lang( prec_view->pui, STRING_NORMAL_QUALITY, APP_COLOR_WHITE );
                draw_record_lang( prec_view->pui, STRING_GOOD_QUALITY, APP_COLOR_YELLOW );
            }
            break;
        case MENU_RECORD:
            break;
        case MENU_SAVE:
            break;
        case MENU_MUSIC_LIST:
            break;
        case MENU_FORMAT:
            draw_record_bmp_ext( prec_view->pui, RECORD_FORMAT_BK_BMP/*RECORD_QUALITY_BK_BMP*/, 276, 0 );
            draw_record_bmp_ext( prec_view->pui, RECORD_QUALITY_FLAG_BMP, 276, prec_view->precord_doc->audioType * -27 );
            if( prec_view->precord_doc->audioTypeTemp == 0 )
            {
                GUI_SetColor(APP_COLOR_WHITE);
                //GUI_DispStringAt( "WMA", 328, 129-4 );
                GUI_DispStringAt( "WAV", 328, 156 - 4 );
                GUI_SetColor(APP_COLOR_YELLOW);
                GUI_DispStringAt( "MP3", 328, 183 - 4 );
            }
            else if( prec_view->precord_doc->audioTypeTemp == 1 )
            {
                GUI_SetColor(APP_COLOR_WHITE);
                //GUI_DispStringAt( "WMA", 328, 129-4 );
                GUI_DispStringAt( "MP3", 328, 183 - 4 );
                GUI_SetColor(APP_COLOR_YELLOW);
                GUI_DispStringAt( "WAV", 328, 156 - 4 );
            }
            else
            {
                GUI_SetColor(APP_COLOR_WHITE);
                GUI_DispStringAt( "WAV", 328, 156 - 4 );
                GUI_DispStringAt( "MP3", 328, 183 - 4 );
                GUI_SetColor(APP_COLOR_YELLOW);
                //GUI_DispStringAt( "WMA", 328, 129 );
            }
            break;
        default:
            break;
        }
    }
}

static void record_view_rec_ani( precord_view_t prec_view )
{
    __u16 fp = 0, i, aniCnt = RECORD_ANI_4_BMP;
    __s16 aniX = 4, aniY = 148;

    draw_record_bmp( prec_view->pui, RECORD_ANI_BG_BMP );

    fp = prec_view->precord_doc->spctrmLevel[0] / 100;

    if( fp > 24 )
        fp = 24;

    aniX += 15 * fp;

    for( i = fp; i > 0 && i <= 24; i-- )
    {
        draw_record_bmp_coor( prec_view->pui, aniCnt, aniX, aniY );
        if( aniCnt > RECORD_ANI_0_BMP )
            aniCnt -= 1;
        aniX -= 15;
    }
}

static void record_view_rec_nofiles( precord_view_t prec_view )
{
    if( prec_view->precord_doc->timeCnt < NO_FILES_TIMES )
    {
        draw_record_lang( prec_view->pui, STRING_MSG3, APP_COLOR_WHITE );
    }
}

void record_view_show( precord_view_t prec_view, H_WIN layer )
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

    record_view_Top(prec_view);
    record_view_rec_ani(prec_view);
    record_view_recInfo(prec_view);
    record_view_menuBox(prec_view);
    record_view_Bottom(prec_view);
    record_view_rec_nofiles(prec_view);

    //GUI_SetColor(APP_COLOR_WHITE|0xF);
    //GUI_FillCircle(100, 100, 50);

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select( NULL );
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
}

void record_view_exit( precord_view_t prec_view, H_WIN layer )
{
    GUI_ClearRect(0, 0, 400,  240);

    //flush_record_res(prec_view->pui);

    //GUI_LyrWinSetSta(layer, GUI_LYRWIN_STA_OFF);
}
