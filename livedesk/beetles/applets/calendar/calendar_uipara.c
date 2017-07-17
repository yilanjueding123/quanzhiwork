/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: calendar_uipara.c
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/05/13 14:35  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#include "calendar_uipara.h"

//////////////////////////////////////////////////////////////////////////
//400x240 LCD
#define CLD_FONT_H                   16 //字体高
static calendar_uipara_t calendar_ui_400x240 =
{
    // 	{//rt_lcd
    // 		0, 0, 400, 240
    // 	},

    //CLD_UI_TIME_PARA_T
    {
        {0, 0, 400, 240},                        //ui界面区域(图层区域)
        {
            //CLD_UI_SIZE_TIME_T
            {42, 81},                            //数字图片大小
            {42, 81},                            //冒号图片大小
            {42, 81},                            //am / pm 图片大小
        },
        {11, 78},                                //数字时钟第一位数字的坐标
        {11 + 42 * 6 + 42 * 2, 78},              //am, pm图片的坐标
    },

    //CLD_UI_DATE_PARA_T
    {
        {0, 0, 400, 240},                        //ui界面区域(图层区域)
        {
            //CLD_UI_COLOUR_DATE_T
            APP_COLOR_WHITE,                     //一般文字颜色
            APP_COLOR_YELLOW,                    //date focus colour
            //			APP_COLOR_RED,                       //holiday colour
            // 			APP_COLOR_WHITE,                     //星期颜色
            // 			APP_COLOR_WHITE,                     //date normal colour
            // 			APP_COLOR_WHITE,                     //日期信息颜色
        },
        {
            //CLD_UI_SIZE_DATE_T
            {50, 0},                             //星期间的间隔
            {50, CLD_FONT_H * 2 + 2},            //日期间的间隔
            {0, CLD_FONT_H},                     //公历 / 农历 间的间隔
            {CLD_FONT_H * 2, CLD_FONT_H * 2},    //日期块(含公历 / 农历)的大小
        },
        {43, 9},                                 //星期开始的坐标
        {35, 26},                                //日期开始的坐标
        {12, 202},                               //日期信息的坐标
    },

    //CLD_UI_SETTING_PARA_T
    {
        {198, 8, 192, 94},                       //ui界面区域(图层区域)
        {
            //CLD_UI_COLOUR_SETTING_T
            APP_COLOR_BLACK,                     //一般文字颜色
            APP_COLOR_YELLOW,                    //focus line text colour
        },
        {
            //CLD_UI_SIZE_SETTING_T
            {172, 20},                           //一行文字宽高
        },
        {12, 8},                                 //"日期"的坐标
        {12, 8 + (CLD_FONT_H + 15)},             //"模式"的坐标
        {12, 8 + (CLD_FONT_H + 15) * 2},         //"时间"的坐标
    },
};

calendar_uipara_t *get_calendar_uipara(void)
{
    return &calendar_ui_400x240;
}



