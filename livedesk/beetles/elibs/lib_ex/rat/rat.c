#include "epdk.h"
#include "rat_common.h"
#include "ScanPri.h"
#include "Scan.h"
#include "rat_npl.h"
#include "rat.h"

extern __rat_list_t *g_rat_npl[RAT_MEDIA_TYPE_MAX];
extern __rat_list_t g_null_rat_npl;

#define RAT_MAX_LIST_CNT	(RAT_MEDIA_TYPE_MAX*2)

__rat_list_t g_rat_list[RAT_MAX_LIST_CNT] = {0};

__krnl_event_t *g_rat_sem = NULL;

static __bool has_init = 0;

/*
****************************************************************************************************
*
*
*
*  Description:
*				-	open the rat process.
*
*  Parameters:
*   pDir		-  	pointer to directory(e:\ f:\....  ).
*   media_type	-  	types of media file.
*   OnceCnt		-  	read items number when read once.
*					if zero, I will read all items one time.
*
*  Return value:
*   NULL   		-  	setup rat process failed.
*	others		-	handle to rat process.
****************************************************************************************************
*/
HRAT rat_open(const char *pDir, rat_media_type_t media_type, __u32 OnceCnt)
{
    __s32 i;
    HRAT  hrat;
    HSCAN hscan;
    __u64 file_type;
    __bool rat_npl_need_update;
    __bool rat_list_research;

    rat_list_research = EPDK_FALSE;//文件列表是否被重新搜索
    rat_npl_need_update = EPDK_FALSE;//文件列表被重新搜索后，播放列表是否需要更新

    //media_type = RAT_MEDIA_TYPE_VIDEO_AND_AUDIO;	// for test
    eLIBs_printf("rat_open: pDir=%s media_type=%d OnceCnt=%d\n", pDir, media_type, OnceCnt);
    RAT_ENTER_CRITICAL_SECTION;

    for (i = 0 ; i < RAT_MAX_LIST_CNT ; i++)
    {
        if (0 == eLIBs_strcmp(pDir, g_rat_list[i].str_path)
                && media_type == g_rat_list[i].media_type)
        {
            if (0 == g_rat_list[i].open_cnt)
            {
                RAT_LEAVE_CRITICAL_SECTION;
                //__log("||---rt---0||\n");
                __msg("error, open_cnt=0\n");
                return NULL;
            }
            if (NULL == g_rat_list[i].hscan)
            {
                RAT_LEAVE_CRITICAL_SECTION;
                __msg("error, g_rat_list[i].hrat=NULL\n");
                //__log("||---rt---1||\n");
                return NULL;
            }

            if(0 == g_rat_list[i].modify_flag)//如果该列表未被复制或删除过，则无需重新搜索
            {
                g_rat_list[i].open_cnt++;
                ScanMoveToFirst(g_rat_list[i].hscan);
                RAT_LEAVE_CRITICAL_SECTION;
                //__log("||---rt---2||\n");
                return (HRAT)&g_rat_list[i];
            }
            else
            {

                rat_list_research = EPDK_TRUE;

                //如果播放列表所在文件列表被重新搜索了，则需把文件列表同步到播放列表
                if(NULL != g_rat_npl[media_type]
                        && g_rat_npl[media_type] == &g_rat_list[i])
                {
                    rat_npl_need_update = EPDK_TRUE;
                }

                ScanFree(g_rat_list[i].hscan);
                g_rat_list[i].hscan = NULL;
                g_rat_list[i].open_cnt = 0;
                g_rat_list[i].str_path[0] = 0;
                g_rat_list[i].modify_flag = 0;
                break;
            }
        }


    }

    if (RAT_MEDIA_TYPE_PIC == media_type)
    {
        file_type = PHOTO_SCAN_ALL;
    }
    else if (RAT_MEDIA_TYPE_VIDEO == media_type)
    {
        file_type = MOVIE_SCAN_ALL;
    }
    else if (RAT_MEDIA_TYPE_AUDIO == media_type)
    {
        file_type = MUSIC_SCAN_ALL;
    }
    else if (RAT_MEDIA_TYPE_EBOOK == media_type)
    {
        file_type = EBOOK_SCAN_ALL;
    }
    else if(RAT_MEDIA_TYPE_ALL == media_type)
    {
        file_type = 0;
    }
    else if(RAT_MEDIA_TYPE_VIDEO_AND_AUDIO == media_type)
    {
        file_type = MOVIE_SCAN_ALL | MUSIC_SCAN_ALL;
    }

    else
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return NULL;
    }
    //__log("************jhdbg0304-3************\n");

    hscan = ScanFilesBegin(pDir, file_type, OnceCnt);
    rat_set_search_cb(NULL, NULL);
    if (NULL == hscan)
    {
        RAT_LEAVE_CRITICAL_SECTION;
        //__msg("ScanFilesBegin fail...\n");
        //__log("||---rt---3||\n");
        return NULL;
    }

    //文件列表被重新搜索后，其链表首地址被存储的位置不变，不需要重新查找存储位置
    //这样可以避免文件列表重新搜索后，播放列表没有被同步的问题
    if(EPDK_FALSE == rat_list_research)
    {
        for (i = 0 ; i < RAT_MAX_LIST_CNT ; i++)
        {
            if (NULL == g_rat_list[i].hscan
                    && 0 == g_rat_list[i].open_cnt)
            {
                break;
            }
        }
    }

    if (i >= RAT_MAX_LIST_CNT)
    {
        //__msg("no free space for rat list...\n");
        ScanFree(hscan);
        RAT_LEAVE_CRITICAL_SECTION;
        return NULL;
    }

    ScanMoveToFirst(hscan);
    __here__;

    g_rat_list[i].hscan = hscan;
    __here__;
    g_rat_list[i].media_type = media_type;
    __here__;

    //如果列表被重新搜索，说明不是第一次搜索，引用计数加1。
    //否则引用计数为1
    if(EPDK_FALSE == rat_list_research)
    {
        g_rat_list[i].open_cnt = 1;
    }
    else
    {
        g_rat_list[i].open_cnt++;
    }

    if(g_rat_list[i].open_cnt < 1)//保护一下
    {
        g_rat_list[i].open_cnt = 1;
    }
    __here__;
    eLIBs_strcpy(g_rat_list[i].str_path, pDir);
    __here__;

    //如果播放列表被同步，则需重新设定正确的npl_cur_index
    if(EPDK_TRUE == rat_npl_need_update)
    {
        __s32 total_cnt;

        total_cnt = ScanGetCurCnt(hscan);

        //g_rat_list[i].npl_cur_index = 0;	//不在这里做，由应用程序从robin取得文件名，重新调用set_file_for_play

        if(g_rat_list[i].npl_cur_index > total_cnt - 1)
        {
            g_rat_list[i].npl_cur_index = total_cnt - 1;
        }

        if(g_rat_list[i].npl_cur_index < 0)
        {
            g_rat_list[i].npl_cur_index = 0;
        }
    }
    else
    {
        g_rat_list[i].npl_cur_index = 0;
    }

    __here__;

    RAT_LEAVE_CRITICAL_SECTION;

    return (HRAT)&g_rat_list[i];
}

/*
****************************************************************************************************
*
*
*
*  Description:
*				-	move rat scan handle cursor position to first entry.
*
*  Parameters:
*	hRat		-	handle to rat information.
*
*  Return value:
*   EPDK_OK   	-  	move succeeded.
*	EPDK_FAIL	-	move failed.
****************************************************************************************************
*/
__s32 rat_move_cursor_to_first(HRAT hRat)
{
    __s32 ret;

    __rat_list_t *pRatList;

    pRatList = (__rat_list_t *)hRat;
    if (0 == pRatList || 0 == pRatList->hscan)
    {
        return -1;
    }
    RAT_ENTER_CRITICAL_SECTION;
    ret = ScanMoveToFirst(pRatList->hscan);
    RAT_LEAVE_CRITICAL_SECTION;
    return ret;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*				-	move rat handle cursor position forward.
*
*  Parameters:
*	hRat		-	handle to rat information.
*
*  Return value:
*   __u32   	-  	count of cursor has actually move.
*
****************************************************************************************************
*/
__u32 rat_move_cursor_forward(HRAT hRat, __u32 cnt)
{
    __u32 i;
    __s32 ret;

    __rat_list_t *pRatList;

    pRatList = (__rat_list_t *)hRat;
    if (0 == pRatList || 0 == pRatList->hscan)
    {
        return 0;
    }

    i = 0;
    RAT_ENTER_CRITICAL_SECTION;
    while(!ScanIsDone(pRatList->hscan))
    {
        if(i >= cnt)
        {
            break;
        }
        ret = ScanMoveToNext(pRatList->hscan);
        if(EPDK_FAIL == ret)
        {
            break;
        }
        i++;
    }

    RAT_LEAVE_CRITICAL_SECTION;
    return i;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*				-	move rat handle cursor backward
*
*  Parameters:
*	hRat		-	handle to rat information.
*
*  Return value:
*   __u32   	-  	count of cursor has actually move.
*
****************************************************************************************************
*/
__u32 rat_move_cursor_backward(HRAT hRat, __u32 cnt)
{
    __u32 i;
    __s32 ret;
    __rat_list_t *pRatList;

    pRatList = (__rat_list_t *)hRat;
    if (0 == pRatList || 0 == pRatList->hscan)
    {
        return 0;
    }
    i = 0;
    RAT_ENTER_CRITICAL_SECTION;
    while(i < cnt)
    {
        ret = ScanMoveToPrev(pRatList->hscan);
        if(EPDK_FAIL == ret)
        {
            break;
        }
        i++;
    }
    RAT_LEAVE_CRITICAL_SECTION;
    return i;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*				-	check npl handle cursor position pointing after the end entry.
*
*  Parameters:
*	hRat		-	handle to npl information.
*
*  Return value:
*   EPDK_TRUE   -  	cursor position pointing the end entry.
*	EPDK_FALSE	-	cursor position not pointing at the end entry.
****************************************************************************************************
*/
__bool rat_is_cursor_end(HRAT hRat)
{
    __s32 ret;

    __rat_list_t *pRatList;

    pRatList = (__rat_list_t *)hRat;
    if (0 == pRatList || 0 == pRatList->hscan)
    {
        return -1;
    }

    RAT_ENTER_CRITICAL_SECTION;
    ret = ScanIsDone(pRatList->hscan);
    RAT_LEAVE_CRITICAL_SECTION;
    return ret;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*				-	free npl handle.
*
*  Parameters:
*	hRat		-	handle to npl information.
*
*  Return value:
*   	   		NONE
****************************************************************************************************
*/
__s32 rat_close(HRAT hRat)
{
    __s32 i;
    __s32 ret;
    __rat_list_t *pRatList;

    pRatList = (__rat_list_t *)hRat;
    if (0 == pRatList || 0 == pRatList->hscan)
    {
        return -1;
    }

    RAT_ENTER_CRITICAL_SECTION;

    for (i = 0 ; i < RAT_MAX_LIST_CNT ; i++)
    {
        if (pRatList->hscan == g_rat_list[i].hscan)
        {
            g_rat_list[i].open_cnt--;
            if (0 == g_rat_list[i].open_cnt)
            {
                ScanFree((HSCAN)hRat);
                g_rat_list[i].hscan = NULL;
                g_rat_list[i].open_cnt = 0;
                g_rat_list[i].str_path[0] = 0;
                g_rat_list[i].modify_flag = 0;
                if (g_rat_npl[g_rat_list[i].media_type] == pRatList)//删除播放列表
                {
                    g_rat_npl[g_rat_list[i].media_type] = NULL;
                }
            }
            break;
        }
    }
    if (i >= RAT_MAX_LIST_CNT)
    {
        __msg("error: hrat not found in list...\n");
        RAT_LEAVE_CRITICAL_SECTION;
        return -1;
    }
    RAT_LEAVE_CRITICAL_SECTION;

    return 0;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*				-	get current npled file entry.
*
*  Parameters:
*	hRat		-	handle to npl information.
*	entry		-	to store entry info
*  Return value:
*   EPDK_OK   		-  	get current entry ok.
*	EPDK_FAIL		-	get current entry failed.
****************************************************************************************************
*/
__s32 rat_get_cur_item_info(HRAT hRat, rat_entry_t *entry)
{
    __s32 ret;
    __rat_list_t *pRatList;
    rat_entry_t *pEntry;
    pRatList = (__rat_list_t *)hRat;
    if (0 == pRatList || 0 == pRatList->hscan)
    {
        return EPDK_FAIL;
    }

    RAT_ENTER_CRITICAL_SECTION;
    pEntry = ScanGetCurrent(pRatList->hscan);
    if(pEntry)
    {
        eLIBs_memcpy(entry, pEntry, sizeof(rat_entry_t));
        ret = EPDK_OK;
    }
    else
    {
        ret = EPDK_FAIL;
    }
    RAT_LEAVE_CRITICAL_SECTION;

    return ret;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*				-	set current file to be play.
*
*  Parameters:
*	hRat		-	handle to rat.
*	file		-   full file path
*
*  Return value:
*   EPDK_FAIL   		-  	no such file
*   EPDK_TRUE   		-  	ok
*	others		-
****************************************************************************************************
*/
__s32 rat_set_file_for_play(HRAT hRat, char *file)
{
    __s32 i;
    __s32 ret;
    __rat_list_t *pRatList;
    HRATNPL hnpl;
    pRatList = (__rat_list_t *)hRat;
    if (0 == pRatList || 0 == pRatList->hscan)
    {
        return -1;
    }

    RAT_ENTER_CRITICAL_SECTION;

    g_rat_npl[pRatList->media_type] = pRatList;

    RAT_LEAVE_CRITICAL_SECTION;

    //设置当前播放条目
    hnpl = rat_npl_open(pRatList->media_type);
    if (NULL != hnpl)
    {
        ret = rat_npl_file2index(hnpl, file);
        if (-1 == ret)
        {
            ret = 0;
        }

        rat_npl_set_cur(hnpl, ret);
        rat_npl_close(hnpl);
    }
    return 0;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*				-	get current item info by index.
*
*  Parameters:
*	hRat		-	handle to rat.
*	index		-   zero base index
*	entry		-	to store entry info
*  Return value:
*   EPDK_OK		   		-  	current item info
*   EPDK_FAIL	   		-  	no such item
*	others		-
****************************************************************************************************
*/
__s32 rat_get_item_info_by_index(HRAT hRat, __s32 index, rat_entry_t *entry)
{
    __rat_list_t *rat_list;
    ScanInfo_t *pScanInfo;
    rat_entry_t *pEntry;
    __s32 n;
    __s32 ret;

    __msg("index=%d\n", index);

    if (NULL == hRat)
    {
        __here__;
        return EPDK_FAIL;
    }

    RAT_ENTER_CRITICAL_SECTION;

    rat_list = (__rat_list_t *)hRat;
    if (NULL == rat_list->hscan)
    {
        __here__;
        RAT_LEAVE_CRITICAL_SECTION;
        return EPDK_FAIL;
    }

    pScanInfo = (ScanInfo_t *)(rat_list->hscan);

    if (pScanInfo->CurCnt <= 0 || index >= pScanInfo->CurCnt)
    {
        __msg("pScanInfo->CurCnt=%d\n", pScanInfo->CurCnt);
        RAT_LEAVE_CRITICAL_SECTION;
        return EPDK_FAIL;
    }

    __ScanMoveCursorToFirst(rat_list->hscan, &pScanInfo->TmpCursor);
    n = 0;
    while (n < index)
    {
        if (EPDK_OK != __ScanMoveCursorToNext(rat_list->hscan, &pScanInfo->TmpCursor))
        {
            break;
        }
        n++;
    }
    if (n == index)
    {
        pEntry = __ScanGetCurrentCursorData(rat_list->hscan, &pScanInfo->TmpCursor);
        if(pEntry)
        {
            eLIBs_memcpy(entry, pEntry, sizeof(rat_entry_t));
            ret = EPDK_OK;
        }
        else
        {
            __here__;
            ret = EPDK_FAIL;
        }
        RAT_LEAVE_CRITICAL_SECTION;
        return ret;
    }

    RAT_LEAVE_CRITICAL_SECTION;
    return EPDK_FAIL;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*				-	get current item info by index.
*
*  Parameters:
*	hRat		-	handle to rat.
*	index		-   zero base index
*
*  Return value:
*   rat_entry_t*   		-  	current item info
*   NULL		   		-  	no such item
*	others		-
****************************************************************************************************
*/
__u32 rat_get_cur_scan_cnt(HRAT hRat)
{
    __rat_list_t *pRatList;
    ScanInfo_t *pScanInfo;
    __u32 cnt;

    pRatList = (__rat_list_t *)hRat;
    if (0 == pRatList || 0 == pRatList->hscan)
    {
        return 0;
    }

    RAT_ENTER_CRITICAL_SECTION;
    pScanInfo = (ScanInfo_t *)(pRatList->hscan);
    cnt = pScanInfo->CurCnt;
    RAT_LEAVE_CRITICAL_SECTION;

    return cnt;
}

__s32 rat_init(void)
{
    if (1 == has_init)
    {
        return EPDK_OK;
    }

    if (0 == g_rat_sem)
    {
        g_rat_sem = esKRNL_SemCreate(1);
    }

    eLIBs_memset(g_rat_list, 0, sizeof(g_rat_list));
    eLIBs_memset(&g_null_rat_npl, 0, sizeof(g_null_rat_npl));

    rat_set_search_cb(NULL, NULL);

    has_init = 1;

    return EPDK_OK;
}

__s32 rat_deinit(void)
{
    __u8    err;

    if (0 == has_init)
    {
        return EPDK_OK;
    }

    rat_clear_all_list();

    if (g_rat_sem)
    {
        esKRNL_SemDel(g_rat_sem, OS_DEL_ALWAYS, &err);
        g_rat_sem = NULL;
    }

    rat_set_search_cb(NULL, NULL);

    has_init = 0;

    return EPDK_OK;
}

//clear all rat list and npl
__s32 rat_clear_all_list_by_root(void *arg)
{
    __s32 i;
    char str_cur[2] = {0};

    str_cur[0] = (char)arg;
    RAT_ENTER_CRITICAL_SECTION;

    __msg("rat_clear_all_list, str_cur=%s\n", str_cur);

    //删除播放列表
    for(i = 0 ; i < RAT_MEDIA_TYPE_MAX ; i++)
    {
        if(g_rat_npl[i])
        {
            __msg("g_rat_npl[%d]->str_path=%s\n", i, g_rat_npl[i]->str_path);

            if(0 == eLIBs_strnicmp(str_cur, g_rat_npl[i]->str_path, 1))
            {
                __msg("clear rat npl:g_rat_npl[%d]->str_path=%s\n", i, g_rat_npl[i]->str_path);
                g_rat_npl[i] = NULL;
            }
        }
        else
        {
            __msg("g_rat_npl[%d] is null\n", i);
        }
    }

    __here__;

    //删除文件列表
    for (i = 0 ; i < RAT_MAX_LIST_CNT ; i++)
    {
        if (g_rat_list[i].hscan)
        {
            __msg("g_rat_list[%d].str_path=%s\n", i, g_rat_list[i].str_path);

            if(0 == eLIBs_strnicmp(str_cur, g_rat_list[i].str_path, 1))
            {
                __msg("clear rat list,g_rat_list[%d].str_path=%s\n", i, g_rat_list[i].str_path);
                ScanFree(g_rat_list[i].hscan);
                g_rat_list[i].hscan = NULL;
                g_rat_list[i].open_cnt = 0;
                g_rat_list[i].str_path[0] = 0;
                g_rat_list[i].modify_flag = 0;
            }
        }
        else
        {
            __msg("g_rat_list[%d].hscan is null\n", i);
        }
    }
    __here__;
    RAT_LEAVE_CRITICAL_SECTION;

    return 0;
}


//clear all rat list and npl
__s32 rat_clear_all_list(void)
{
    __s32 i;

    RAT_ENTER_CRITICAL_SECTION;

    __msg("rat_clear_all_list\n");

    //删除播放列表
    for(i = 0 ; i < RAT_MEDIA_TYPE_MAX ; i++)
    {
        if(g_rat_npl[i])
        {
            __msg("g_rat_npl[%d]->str_path=%s\n", i, g_rat_npl[i]->str_path);

            //if(0 == eLIBs_strnicmp(str_cur, g_rat_npl[i]->str_path, 1))
            {
                __msg("clear rat npl:g_rat_npl[%d]->str_path=%s\n", i, g_rat_npl[i]->str_path);
                g_rat_npl[i] = NULL;
            }
        }
        else
        {
            __msg("g_rat_npl[%d] is null\n", i);
        }
    }

    __here__;

    //删除文件列表
    for (i = 0 ; i < RAT_MAX_LIST_CNT ; i++)
    {
        if (g_rat_list[i].hscan)
        {
            __msg("g_rat_list[%d].str_path=%s\n", i, g_rat_list[i].str_path);

            //if(0 == eLIBs_strnicmp(str_cur, g_rat_list[i].str_path, 1))
            {
                __msg("clear rat list,g_rat_list[%d].str_path=%s\n", i, g_rat_list[i].str_path);
                ScanFree(g_rat_list[i].hscan);
                g_rat_list[i].hscan = NULL;
                g_rat_list[i].open_cnt = 0;
                g_rat_list[i].str_path[0] = 0;
                g_rat_list[i].modify_flag = 0;
            }
        }
        else
        {
            __msg("g_rat_list[%d].hscan is null\n", i);
        }
    }
    __here__;
    RAT_LEAVE_CRITICAL_SECTION;

    return 0;
}


rat_media_type_t rat_get_file_type(const char *file_name)
{
    rat_media_type_t media_type = RAT_MEDIA_TYPE_UNKNOWN;

    __ValidFileType(file_name, 0, (int *)(&media_type));

    return media_type;
}

__s32 rat_set_modify_flag(rat_media_type_t media_type, __s32 flag)
{
    __s32 i;

    __msg("rat_set_modify_flag: media_type=%d, flag = %d\n", media_type, flag);

    RAT_ENTER_CRITICAL_SECTION;

    for (i = 0 ; i < RAT_MAX_LIST_CNT ; i++)
    {
        if (g_rat_list[i].hscan
                && media_type == g_rat_list[i].media_type)
        {
            g_rat_list[i].modify_flag = flag;
        }
    }

    RAT_LEAVE_CRITICAL_SECTION

    return EPDK_OK;
}

__s32 rat_set_modify_flag_all(__s32 flag)
{
    __s32 i;

    __msg("rat_set_modify_flag_all: flag = %d\n", flag);

    RAT_ENTER_CRITICAL_SECTION;

    for (i = 0 ; i < RAT_MAX_LIST_CNT ; i++)
    {
        if (g_rat_list[i].hscan)
        {
            g_rat_list[i].modify_flag = flag;
        }
    }

    RAT_LEAVE_CRITICAL_SECTION;

    return EPDK_OK;
}

__s32 rat_set_search_cb(prat_search_cb cb, void *arg)
{
    g_rat_search_cb = cb;
    g_rat_search_arg = arg;
    return EPDK_OK;
}


