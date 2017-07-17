/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2011, CHIPHD, China
*											 All Rights Reserved
*
* File    	: calendar_adapter.c
* By      	: CQQ
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011/05/13 17:56  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#include "beetles_app.h"
#include "calendar_adapter.h"

#if TEST_TIME_FORMAT
__s32 g_time_format = 0;
#endif

//////////////////////////////////////////////////////////////////////////
__u16 get_week_day( __u16 year, __u8 month, __u8 day )
{
    __u16 	temp;
    __u8 	yearH, yearL;
    __u8 	table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};

    yearH = year / 100;
    yearL = year % 100;
    if (yearH > 19)
    {
        yearL += 100;
    }

    temp = yearL + yearL / 4;
    temp = temp % 7;
    temp = temp + day + table_week[month - 1];

    if (yearL % 4 == 0 && month < 3)
    {
        temp--;
    }

    return (temp % 7);
}


__s32 _get_time_format(void)
{
    reg_calendar_para_t *para;

    get_reg_pointer(para, reg_calendar_para_t, REG_APP_CALENDAR);

    if (para)
    {
        return para->time_format;
    }
    else
    {
        __err("dsk_reg_get_para_by_app fail...\n");
        return 0;
    }
}

__s32 _set_time_format(__s32 format)
{
    reg_calendar_para_t *para;

    get_reg_pointer(para, reg_calendar_para_t, REG_APP_CALENDAR);

    para->time_format = format;

    return update_to_reg();
}




