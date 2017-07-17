/*
************************************************************************************************************************
*                                                        CUCKOO
*                                                   the Audio Player
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : misc.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2008.11.08
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2008.11.08       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __misc_c
#define  __misc_c

#include "Misc.h"


//libaiao, 2011.5.15
//根据指定长度从src中截取字符串
//返回值为窃取字符串的长度(byte为单位)
__s32 GUI_GetStringByWidth(char *dst, const char GUI_UNI_PTR *src, __u32 width)
{
    const char GUI_UNI_PTR *s = src;
    U16 r = 0, w = 0, len = 0;


    r   = GUI_UC_GetCharCode(s);
    if(r == 0)
    {
        return 0;
    }
    while(r != 0)
    {
        if(w >= width)
        {
            break;
        }
        r   = GUI_UC_GetCharCode(s + len);
        len  += GUI_UC_GetCharSize(s + len);		//指向下一个字符
        w += GUI_GetCharDistX(r);
    }
    eLIBs_strncpy(dst, src, len);
    *(dst + len) = 0;
    return len;
}

//libaiao, 2011.5.15
//extern __s32 GUI_GetStringByWidth(char *dst, const char GUI_UNI_PTR *src, __u32 width);
//根据可显示区域截取字串
//如果字串宽度大于显示区域,则在可显示字串后加三个省略号
//注意该函数只判断宽度，不判断高度
//append为追加在后面的省略号
__s32 GetClippedString(GUI_RECT *region, char *src_str, char *dst_str, const char *append)
{
    __u32 src_width = 0, dst_width = 0, append_width = 0;
    __u32 len = 0, char_width = 0;
    __u32 gap = 0;

    dst_width = region->x1 - region->x0;
    src_width = GUI_GetStringDistX(src_str);
    if(dst_width >= src_width)
    {
        eLIBs_strcpy(dst_str, src_str);
        return EPDK_OK;
    }
    append_width = GUI_GetStringDistX(append);
    if(append_width >= dst_width)
    {
        eLIBs_strcpy(dst_str, append);
        return EPDK_OK;
    }
    gap = dst_width - append_width;
    GUI_GetStringByWidth(dst_str, src_str, gap);
    //GUI_DispStringLen();
    eLIBs_strcat(dst_str, append);
    return EPDK_OK;
}


//////////////////////////////////////////////////////////////////////////
#if DEBUG_TICK_TIME
__u32 g_DebugTickCount = 0;
#endif
//////////////////////////////////////////////////////////////////////////

void time2time( __u32 second_time, cuckoo_time_t *format_time )
{
    __u32  total_minute;

    total_minute = second_time / 60;
    format_time->second = second_time % 60;
    format_time->minute = total_minute % 60;
    format_time->hour   = total_minute / 60;
}

void   uint2str( __u32 input, char *str )
{
    char stack[11] ;
    int i ;
    int j ;
    int count;


    if( input == 0 )
    {
        str[0] = '0';
        str[1] = '0';
        str[2] = '\0';                   // 'str' must end with '\0'
        return ;
    }

    for( i = 0; input > 0; ++i )
    {
        stack[i] = input % 10 + '0';     // characters in reverse order are put in 'stack' .
        input /= 10;
    }                                    // at the end of 'for' loop, 'i' is the number of characters.
    count = i;

    for( j = 0, i = 0; i < ( 2 - count); i++ )  // pad '0'
        str[j++] = '0';
    for( i = count, --i; i >= 0; --i, ++j )
        str[j] = stack[i];
    str[j] = '\0';		                 // 'str' must end with '\0'
}

static void __time2str( const cuckoo_time_t *time, char *str )
{
    uint2str( time->hour,   str );
    str[2] = ':';
    uint2str( time->minute, str + 3 );
    str[5] = ':';
    uint2str( time->second, str + 6 );
}

void  time2str( __u32 ms, char *str )
{
    __u32  second;
    cuckoo_time_t  time;

    second = ms / 1000;
    time2time( second, &time );
    __time2str( &time, str );
}

void time2str_by_format_hour_show(__u32 ms, char *str, time_format_e format)
{
	__u32  second;
	cuckoo_time_t  time;

	second = ms / 1000;
	time2time( second, &time );

	switch(format)
	{
	case TIME_S:
		uint2str( time.second, str);
		break;
	case TIME_MS:
		uint2str( time.minute, str);
		str[2] = ':';
		uint2str( time.second, str + 3);
		break;
	case TIME_HMS:
		time2str(ms, str);
		break;
	case TIME_AUTO_HMS:
		uint2str( time.minute, str);
		str[2] = ':';
		uint2str( time.second, str + 3);
//		  if(time.hour != 0)
//		  {
			time2str(ms, str);
//		  }
		break;
	default:
		break;
	}

}


//根据要显示的格式转换
void time2str_by_format(__u32 ms, char *str, time_format_e format)
{
    __u32  second;
    cuckoo_time_t  time;

    second = ms / 1000;
    time2time( second, &time );

    switch(format)
    {
    case TIME_S:
        uint2str( time.second, str);
        break;
    case TIME_MS:
        uint2str( time.minute, str);
        str[2] = ':';
        uint2str( time.second, str + 3);
        break;
    case TIME_HMS:
        time2str(ms, str);
        break;
    case TIME_AUTO_HMS:
        uint2str( time.minute, str);
        str[2] = ':';
        uint2str( time.second, str + 3);
        if(time.hour != 0)
        {
            time2str(ms, str);
        }
        break;
    default:
        break;
    }

}


//转换文件size 到字符串
//add by libaiao, 2011.5.8
void filesize2str(__u32 size, char *str)
{
    __msg("size = %d\n" , size );
    if (size < 1024)	// 小于1 k
    {
        eLIBs_sprintf(str, "%d B", size);
    }
    else if ( size < (1024 * 1024))	// 小于 1 M
    {
        eLIBs_sprintf(str, "%d K", size / 1024);
    }
    else if (size < (1024 * 1024 * 1024))	// 小于 1 G
    {
        //eLIBs_sprintf(str, "%d.%d M",size/(1024*1024), (size%(1024*1024))*100/(1024*1024)/10);
        eLIBs_sprintf(str, "%d.%d%d M", size / (1024 * 1024), ((size % (1024 * 1024)) / 1024) * 1000 / 1024 / 100, ((size % (1024 * 1024)) / 1024) * 1000 / 1024 % 100 / 10);	//保留两位小数

    }
    else
    {
        //eLIBs_sprintf(str, "%d.%d G",size/(1024*1024*1024),(size%(1024*1024*1024))*100/(1024*1024*1024)/10);
        eLIBs_sprintf(str, "%d.%d%d G", size / (1024 * 1024 * 1024), (size % (1024 * 1024 * 1024)) / (1024 * 1024) * 1000 / 1024 / 100, (size % (1024 * 1024 * 1024)) / (1024 * 1024) * 1000 / 1024 % 100 / 10);

    }
}





//传入24时制小时值，传出12时制小时值，返回0为am，非0为pm
__s32 hour24_to_hour12(__s32 *hour)
{
    if (*hour == 0)
    {
        //凌晨12点
        *hour = 12;
    }
    else if (*hour == 12)
    {
        //中午12点
        return 1;
    }
    else if (12 < *hour)
    {
        *hour -= 12;
        return 1;
    }
    return 0;
}

//查询app_name名称的应用是否存在
//查询背景音乐是否存在可以用is_app_exist(APP_MUSIC)
__s32 is_app_exist(char *app_name)
{
    H_WIN root, child;
    char  winname[256];
    __bool exist;

    root  = GUI_WinGetHandFromName(APP_ROOT);

    if(!root)
    {
        return 0;
    }

    child = GUI_WinGetFirstChild(root);

    exist = 0;

    while( child != NULL )
    {
        eLIBs_memset(winname, 0, 256);
        GUI_WinGetName(child, winname);

        __msg("winname = %s \n", winname);

        if( 0 == eLIBs_strcmp( app_name, winname))
        {
            exist = 1;
            break;
        }

        child = GUI_WinGetNextBro(child);
    }

    return exist;
}

__u32 get_memory_info(char *title, void *pExtPara)
{
    static __u32 last_free_size = 0;
    __u32 free_size = 0, total_size = 0;
    __u32 size;

    //__msg("\n");
    //__msg("=====================title: %s====================\n", title);
    free_size = esMEMS_FreeMemSize();
    if(free_size > last_free_size)
    {
        size = free_size - last_free_size;
        if(last_free_size == 0)
        {
            __msg("=====================malloc memory size is %d=======\n", last_free_size);
        }
        else
        {
            __msg("=====================release memory size is %d=======\n", size);
        }
    }
    else if(free_size <= last_free_size)
    {
        size = last_free_size - free_size;
        __msg("=====================malloc memory size is %d=======\n", size);
    }

    total_size = esMEMS_TotalMemSize();
    __msg("=====================free memory size is %d=======\n", free_size);
    __msg("=====================used memory size is %d=======\n", total_size - free_size);
    __msg("=====================total memory size is %d=======\n", total_size);
    last_free_size = free_size;
}


void *My_Palloc(__u32 npage, __u32 mode)
{
    void *p;

    p = esMEMS_Palloc( npage, mode);
    __msg("palloc p=%x\n", p);
    return p;
}

void My_Pfree(void *mblk, __u32 npage)
{
    __msg("p=%x\n", mblk);
    esMEMS_Pfree(mblk, npage);
}

void *My_Balloc(__u32 size)
{
    static __u32 index = 0;
    void *p = esMEMS_Balloc(size);
    __msg("Balloc index=%d p=%x, size=%d\n", index, (__u32)p, size);
    index++;
    return p;
}

void  My_Bfree(void *mblk, __u32 size)
{
    __msg("Bfree p=%x, size=%d\n", (__u32)mblk, size);
    esMEMS_Bfree(mblk, size);
}

void *My_Malloc(__mems_heap_t *heap, __u32 size)
{
    static __u32 index = 0;
    void *p = esMEMS_Malloc(heap,  size);
    __msg("Malloc index=%d p=%x\n", index, (__u32)p);
    index++;
    return p;
}


void  My_Mfree(__mems_heap_t *heap, void *f)
{
    __msg("Mfree p=%x\n", (__u32)f);
    esMEMS_Mfree(heap, f);
}


__s32 test_flush_nor(__s32 i)
{
    __s32 result;
    ES_FILE *fp = NULL;
    char buf[3000] = {0};
    char path[] = "e:\\test.bin";

    eLIBs_printf("i=%d\n", i);

    if(1)//
    {
        fp = eLIBs_fopen(path, "wb");
        if(fp == NULL)
        {
            eLIBs_printf("open file fail:%s\n", path);
            return EPDK_FAIL;
        }

        result = eLIBs_fwrite(buf, 1, sizeof(buf), fp);
        if(result < sizeof(buf))
        {
            eLIBs_fclose(fp);
            eLIBs_printf("write file fail:%s\n", path);
            return EPDK_FAIL;
        }

        eLIBs_fclose(fp);


        {
            //
            __s32 i, num;
            ES_FILE *hFile;

            for(i = 0 ; i < 1 ; i++)
            {
                //flush user disk data to NOR flash
                hFile = eLIBs_fopen("b:\\STORAGE\\SPINOR", "rb");
                if (hFile == NULL)
                {
                    eLIBs_printf("open NOR file failed\n");
                    return EPDK_FAIL;
                }
                eLIBs_printf("i=%d flush file ....\n", i);
                eLIBs_fioctrl(hFile, DEV_IOC_USR_FLUSH_CACHE, 0, 0);

                eLIBs_printf("i=%d flush end ....\n", i);
                eLIBs_fclose(hFile);

                esKRNL_TimeDly(2);
            }
        }
    }

    return EPDK_OK;
}


int pt_part_name(void)
{
    __s32   ret = -1;
    __s32 i;
    __s32   status = 0;
    char    equipment_name[MAX_FS_NAME_LEN];
    char partition[32];
    partition[1] = ':';
    partition[2] = 0;
    eLIBs_printf("--------pt_part_name2--------\n");
    for(i = 0; i < 26 ; i++)
    {
        partition[0] = 'a' + i;
        eLIBs_memset(equipment_name, 0, MAX_FS_NAME_LEN);
        eLIBs_GetPTName(partition, equipment_name);
        status = eLIBs_IsPartFormated(partition);
        if(status == ELIBS_PART_FMT_FORMATED)
        {
            eLIBs_printf("%s, %s\n", partition, equipment_name);
        }
    }
    return 0;
}
#endif     //  ifndef __misc_c

/* end of misc.c */
