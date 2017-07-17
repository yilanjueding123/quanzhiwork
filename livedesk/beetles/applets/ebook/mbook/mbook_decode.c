/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           record app sample
*
*						        	(c) Copyright 2006-2009, China
*											 All Rights Reserved
*
* File    : mbook_decode.c
* By      : lyn
* Version : V1.00
*********************************************************************************************************
*/

#include "mbook_private.h"

extern void *file_core;

#define     MAX_FILE_NAME       1024

typedef	enum tag__DECODE_MODE
{
    DECODE_NULL = 0,								/* 不解码任何页			*/
    DECODE_CURR,									/* 解码当前页			*/
    DECODE_PREV,									/* 解码当前页的前面页	*/
    DECODE_NEXT,									/* 解码所有页的后面页	*/
} __decode_mode_e;

typedef struct tag_MBOOK_CACHE               // 页缓冲
{
    __s32       page_sta;                   // 页面解码成功标示：1 成功、 0 失败
    __s32       page_no;                    // 缓存里的页面号
    H_WIN       page_lyr;                   // 缓存页对应的软图层
} __page_cache_t;


typedef struct tag_MBOOK_DECODE
{
    char				    file_path[MAX_FILE_NAME];       // 文件路径
    ES_FILE                *file_hdle;                      // 文件句柄
    H_ANALYSIS              analysis_hdle;                  // 文件分析句柄
    H_SHOW                  show_hdle;                      // 页面显示句柄

    H_FONT                  font_hdle;                      // 字体操作句柄

    __s64                   file_size;                      // 文件大小
    __epdk_charset_enm_e    default_charset;                // 默认编码格式
    GUI_FONT               *char_font;                      // 字库句柄
    __s32                  *char_table;                     // 字符字宽表

    H_WIN                   show_lyr;                       // 显示图层
    __page_cache_t          page[DECODE_MBOOK_NUM];          // 页面缓冲器
    __u32                   data_len;                       // 一页数据的大小
    __u8                   *page_data;                      // 一页数据存放地址

    __s32                   total_page;                     // 文件总页数
    __s32                   current_page;                   // 当前显示页号

    __s32                   move_state;                     // 页面移动控制标示
    __s32                   move_height;                    // 页面移动高度

    __u8				    decode_tsk;                     // 预解线程
    __s32				    decode_cnt;                     // 预解页循环计数
    __krnl_event_t		    *decode_sem;                     // 预解控制信号量
    __decode_mode_e		    decode_sta;                     // 预解模式

    __decode_config_t       config;                         // 配置信息
    __epdk_charset_enm_e    charset;                        // 文件编码
} __mbook_decode_t;


/*
**********************************************************************************************************************
*                                               __decode_get_charset
*
* Description: 获取当前文件编码格式
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_get_charset(__mbook_decode_t *hdle)
{
    char 	            buff[3];
    __mbook_decode_t	*p_decode;

    p_decode = hdle;
    eLIBs_fseek(p_decode->file_hdle, 0, SEEK_SET);
    eLIBs_fread(buff, 3, sizeof(char), p_decode->file_hdle);

    if((buff[0] == 0xff) && (buff[1] == 0xfe))
        p_decode->charset = EPDK_CHARSET_ENM_UTF16LE;
    else if((buff[0] == 0xfe) && (buff[1] == 0xff))
        p_decode->charset = EPDK_CHARSET_ENM_UTF16BE;
    else if((buff[0] == 0xef) && (buff[1] == 0xbb) && (buff[2] == 0xbf))
        p_decode->charset = EPDK_CHARSET_ENM_UTF8;
    else
        p_decode->charset = p_decode->default_charset;
}

/*
**********************************************************************************************************************
*                                               __decode_read_page
*
* Description: 读取一页数据到缓存BUF里。
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_read_page(__mbook_decode_t *hdle)
{
    __u32               size;
    __one_page_t        tmp_page;
    __mbook_decode_t	*p_decode;

    p_decode = hdle;
    // 从文件中读取数据
    MBOOK_Analysis_GetInfo(p_decode->analysis_hdle, p_decode->current_page + p_decode->decode_cnt - DECODE_MBOOK_NUM / 2, &tmp_page);
    __msg("start = %d, end = %d, page = %d \n", tmp_page.page_start, tmp_page.page_end, tmp_page.page_no);
    size = tmp_page.page_end - tmp_page.page_start;
    eLIBs_fseek(p_decode->file_hdle, tmp_page.page_start, ELIBS_SEEK_SET);
    eLIBs_fread(p_decode->page_data, 1, size, p_decode->file_hdle);
    *(p_decode->page_data + size) = 0xff;
    *(p_decode->page_data + size + 1) = 0xfe;

    // 将读取的数据写到软图层上
    if(p_decode->config.show_rotate == 0)
    {
        MBOOK_Layer_Rotate(p_decode->page[p_decode->decode_cnt].page_lyr, p_decode->config.show_width,
                           p_decode->config.show_height - p_decode->config.bottom_width, 0);
    }
    else
    {
        MBOOK_Layer_Rotate(p_decode->page[p_decode->decode_cnt].page_lyr,
                           p_decode->config.show_width - p_decode->config.bottom_width, p_decode->config.show_height, 1);
    }
    p_decode->page[p_decode->decode_cnt].page_sta = 1;
    p_decode->page[p_decode->decode_cnt].page_no  = p_decode->current_page + p_decode->decode_cnt - DECODE_MBOOK_NUM / 2;
    MBOOK_Show_Page(p_decode->show_hdle, p_decode->page[p_decode->decode_cnt].page_lyr, p_decode->page_data);
}

/*
**********************************************************************************************************************
*                                               __decode_show_page
*
* Description: 读取一页数据到缓存BUF里。
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_show_page(__mbook_decode_t *hdle)
{
    __s32               i, tmp;
    __u8               *pdes;
    __u8               *psrc1;
    __u8               *psrc2;
    FB                  fb_des;
    FB                  fb_src1;
    FB                  fb_src2;
    __mbook_decode_t	*p_decode;

    p_decode = hdle;
    if(p_decode->config.show_rotate == 0)
    {
        GUI_LyrWinGetFB(p_decode->page[DECODE_MBOOK_NUM / 2    ].page_lyr, &fb_src1);
        GUI_LyrWinGetFB(p_decode->page[DECODE_MBOOK_NUM / 2 + 1].page_lyr, &fb_src2);
        GUI_LyrWinGetFB(p_decode->show_lyr, &fb_des);

        eLIBs_memcpy(fb_des.addr[0], (void *)((__u32)fb_src1.addr[0] + p_decode->move_height * fb_des.size.width),
                     fb_des.size.width * (fb_des.size.height - p_decode->move_height));
        eLIBs_memcpy((void *)((__u32)fb_des.addr[0] + (fb_des.size.height - p_decode->move_height) * fb_des.size.width), fb_src2.addr[0],
                     fb_des.size.width * p_decode->move_height);
    }
    else
    {
        GUI_LyrWinGetFB(p_decode->page[DECODE_MBOOK_NUM / 2    ].page_lyr, &fb_src1);
        GUI_LyrWinGetFB(p_decode->page[DECODE_MBOOK_NUM / 2 + 1].page_lyr, &fb_src2);
        GUI_LyrWinGetFB(p_decode->show_lyr, &fb_des);

        tmp     = fb_des.size.width - p_decode->move_height;
        pdes    = fb_des.addr[0];
        psrc1   = fb_src1.addr[0];
        psrc2   = fb_src2.addr[0];
        psrc1   = psrc1 + p_decode->move_height;
        for(i = 0; i < fb_des.size.height; i++)
        {
            eLIBs_memcpy(pdes, psrc1, tmp);
            pdes    = pdes + tmp;
            psrc1   = psrc1 + fb_des.size.width;
            eLIBs_memcpy(pdes, psrc2, p_decode->move_height);
            pdes    = pdes + p_decode->move_height;
            psrc2   = psrc2 + fb_des.size.width;
        }
    }
}

/*
**********************************************************************************************************************
*                                               __decode_brush_page
*
* Description: 刷新页面缓存里的信息
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_brush_page(__mbook_decode_t *hdle, __decode_mode_e mode)
{
    __u32               i;
    __page_cache_t      tmp_page;
    __mbook_decode_t	*p_decode;

    p_decode = hdle;

    if(mode == DECODE_PREV)
    {
        tmp_page.page_no	= p_decode->page[DECODE_MBOOK_NUM - 1].page_no;
        tmp_page.page_lyr	= p_decode->page[DECODE_MBOOK_NUM - 1].page_lyr;
        for(i = DECODE_MBOOK_NUM - 1; i > 0; i--)
        {
            p_decode->page[i].page_sta	= p_decode->page[i - 1].page_sta;
            p_decode->page[i].page_no	= p_decode->page[i - 1].page_no;
            p_decode->page[i].page_lyr	= p_decode->page[i - 1].page_lyr;
        }
        p_decode->page[0].page_sta	= 0;
        p_decode->page[0].page_no	= tmp_page.page_no;
        p_decode->page[0].page_lyr	= tmp_page.page_lyr;
    }
    else if(mode == DECODE_NEXT)
    {
        tmp_page.page_no	= p_decode->page[0].page_no;
        tmp_page.page_lyr	= p_decode->page[0].page_lyr;
        for(i = 0; i < DECODE_MBOOK_NUM - 1; i++)
        {
            p_decode->page[i].page_sta	= p_decode->page[i + 1].page_sta;
            p_decode->page[i].page_no	= p_decode->page[i + 1].page_no;
            p_decode->page[i].page_lyr	= p_decode->page[i + 1].page_lyr;
        }
        p_decode->page[DECODE_MBOOK_NUM - 1].page_sta	= 0;
        p_decode->page[DECODE_MBOOK_NUM - 1].page_no	= tmp_page.page_no;
        p_decode->page[DECODE_MBOOK_NUM - 1].page_lyr	= tmp_page.page_lyr;
    }
    else if(mode == DECODE_CURR)
    {
        for(i = 0; i < DECODE_MBOOK_NUM; i++)
        {
            p_decode->page[i].page_sta = 0;
        }
    }
}

/*
**********************************************************************************************************************
*                                               __decode_get_page
*
* Description: 获得一个已解好的页面
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_get_page(__mbook_decode_t *hdle, __decode_mode_e mode)
{
    __u8 			    err;
    __mbook_decode_t	*p_decode;

    p_decode = hdle;
    // 拿住解码信号量decode_sem。
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);
    // 刷新页面缓冲池
    if(mode == DECODE_CURR)
    {
        __decode_brush_page(p_decode, DECODE_CURR);
    }
    else if(mode == DECODE_PREV)
    {
        p_decode->current_page--;
        __decode_brush_page(p_decode, DECODE_PREV);
    }
    else if(mode == DECODE_NEXT)
    {
        p_decode->current_page++;
        __decode_brush_page(p_decode, DECODE_NEXT);
    }
    // 判断当前页解码是否完成，未完成则立刻解当前页。
    p_decode->decode_cnt = DECODE_MBOOK_NUM / 2;
    if((p_decode->page[p_decode->decode_cnt].page_sta == 0)
            && ((p_decode->current_page + p_decode->decode_cnt - DECODE_MBOOK_NUM / 2) >= 1)
            && ((p_decode->current_page + p_decode->decode_cnt - DECODE_MBOOK_NUM / 2) <= p_decode->total_page))
    {
        __decode_read_page(p_decode);
    }
    p_decode->decode_sta = mode;
    // 释放解码信号量decode_sem。
    esKRNL_SemPost(p_decode->decode_sem);
}

/*
**********************************************************************************************************************
*                                               __decode_get_next_page
*
* Description: 获得当前页的下一页面
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_get_next_page(__mbook_decode_t *hdle)
{
    __u8 			    err;
    __mbook_decode_t	*p_decode;

    p_decode = hdle;

    esKRNL_SemPend(p_decode->decode_sem, 0, &err);

    p_decode->decode_cnt = DECODE_MBOOK_NUM / 2 + 1;
    if((p_decode->page[p_decode->decode_cnt].page_sta == 0)
            && ((p_decode->current_page + p_decode->decode_cnt - DECODE_MBOOK_NUM / 2) > 0))
    {
        __decode_read_page(p_decode);
    }

    esKRNL_SemPost(p_decode->decode_sem);
}

/*
**********************************************************************************************************************
*                                               __decode_get_prev_page
*
* Description: 获得当前页的上一页面
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_get_prev_page(__mbook_decode_t *hdle)
{
    __u8 			    err;
    __mbook_decode_t	*p_decode;

    p_decode = hdle;

    esKRNL_SemPend(p_decode->decode_sem, 0, &err);

    p_decode->decode_cnt = DECODE_MBOOK_NUM / 2 - 1;
    if((p_decode->page[p_decode->decode_cnt].page_sta == 0)
            && ((p_decode->current_page + p_decode->decode_cnt - DECODE_MBOOK_NUM / 2) > 0))
    {
        __decode_read_page(p_decode);
    }

    esKRNL_SemPost(p_decode->decode_sem);
}

/*
**********************************************************************************************************************
*                                               __decode_move_down
*
* Description: 页面解析写软图层线程
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_move_down(__mbook_decode_t *hdle, __s32 step)
{
    __s32                   tmp_h;
    __mbook_decode_t       *p_decode;

    p_decode = hdle;

    // 获取当前显示页的屏幕窗口起始Y。
    tmp_h   = MBOOK_Layer_GetSrcH(p_decode->show_lyr);

    if((p_decode->current_page >= p_decode->total_page) && (p_decode->move_state == 0))
    {
        return;
    }

    p_decode->move_state = 1;
    if((p_decode->move_height + step) >= tmp_h)
    {
        // 翻页
        __decode_get_page(p_decode, DECODE_NEXT);
        if(p_decode->current_page < p_decode->total_page)
        {
            __decode_get_next_page(p_decode);
            p_decode->move_height = p_decode->move_height + step - tmp_h;

            __decode_show_page(p_decode);
        }
        else
        {
            p_decode->move_height = 0;
            p_decode->move_state = 0;

            __decode_show_page(p_decode);
        }
    }
    else
    {
        p_decode->move_height = p_decode->move_height + step;

        __decode_show_page(p_decode);
    }
}

/*
**********************************************************************************************************************
*                                               __decode_move_up
*
* Description: 页面解析写软图层线程
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_move_up(__mbook_decode_t *hdle, __s32 step)
{
    __s32                   tmp_h;
    __mbook_decode_t       *p_decode;

    p_decode = hdle;

    // 获取当前显示页的屏幕窗口起始Y。
    tmp_h   = MBOOK_Layer_GetSrcH(p_decode->show_lyr);

    if((p_decode->current_page <= 1) && (p_decode->move_state == 0))
    {
        return;
    }

    p_decode->move_state = -1;
    if((p_decode->move_height - step) <= 0)
    {
        // 翻页
        if(p_decode->current_page > 1)
        {
            __decode_get_page(p_decode, DECODE_PREV);
            p_decode->move_height = p_decode->move_height - step + tmp_h;

            __decode_show_page(p_decode);
        }
        else
        {
            p_decode->move_height = 0;
            p_decode->move_state = 0;

            __decode_show_page(p_decode);
        }
    }
    else
    {
        p_decode->move_height = p_decode->move_height - step;

        __decode_show_page(p_decode);
    }
}

/*
**********************************************************************************************************************
*                                               __decode_task
*
* Description: 页面解析写软图层线程
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_task(void *p_arg)
{
    __u8 			    err;
    __mbook_decode_t	*p_decode;

    p_decode = (__mbook_decode_t *)p_arg;
    while(1)
    {
        if(esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            goto EXIT_DEC_TASK;
        }
        esKRNL_TimeDly(5);

        esKRNL_SemPend(p_decode->decode_sem, 0, &err);

        if(p_decode->decode_sta != DECODE_NULL)
        {
            if((p_decode->page[p_decode->decode_cnt].page_sta == 0)
                    && ((__s32)(p_decode->current_page + p_decode->decode_cnt - DECODE_MBOOK_NUM / 2) >= 1)
                    && ((__s32)(p_decode->current_page + p_decode->decode_cnt - DECODE_MBOOK_NUM / 2) <= p_decode->total_page))
            {
                __decode_read_page(p_decode);
            }
            p_decode->decode_cnt++;
            if(p_decode->decode_cnt == DECODE_MBOOK_NUM)
            {
                p_decode->decode_cnt = 0;
            }
        }

        esKRNL_SemPost(p_decode->decode_sem);
    }

EXIT_DEC_TASK:
    esKRNL_TDel(EXEC_prioself);

    return;
}

/*
************************************************************************************************************************
*                       				__decode_show_config
*
*Description: 配置绘制模块相关参数
*
*Arguments  : hdle： 操作句柄
*
*
*Return     : NULL
*
************************************************************************************************************************
*/
static void __decode_show_config(__mbook_decode_t *hdle)
{
    __show_config_t         show_cfg;
    __mbook_decode_t       *p_decode;
    __decode_config_t      *p_config;

    p_decode = hdle;
    p_config = &p_decode->config;
    // 配置数据读写模块
    if(p_config->show_rotate == 0)
    {
        show_cfg.show_start = p_config->border_width;
        show_cfg.show_width = p_config->show_width - 2 * p_config->border_width + p_config->row_space - p_config->scroll_width;
        show_cfg.page_row   = (p_config->show_height - p_config->bottom_width) / (p_config->font_size + p_config->row_space);
        show_cfg.row_height = p_config->font_size + p_config->row_space;
        show_cfg.col_width  = p_config->font_size + p_config->col_space;
        show_cfg.font_color = p_config->font_color;
        show_cfg.back_color = p_config->back_color;
        show_cfg.char_font  = p_decode->char_font;
        show_cfg.char_table = p_decode->char_table;
        show_cfg.charset    = p_decode->charset;
    }
    else
    {
        show_cfg.show_start = p_config->border_width;
        show_cfg.show_width = p_config->show_height - 2 * p_config->border_width + p_config->row_space - p_config->scroll_width;
        show_cfg.page_row   = (p_config->show_width - p_config->bottom_width) / (p_config->font_size + p_config->row_space);
        show_cfg.row_height = p_config->font_size + p_config->row_space;
        show_cfg.col_width  = p_config->font_size + p_config->col_space;
        show_cfg.font_color = p_config->font_color;
        show_cfg.back_color = p_config->back_color;
        show_cfg.char_font  = p_decode->char_font;
        show_cfg.char_table = p_decode->char_table;
        show_cfg.charset    = p_decode->charset;
    }
    MBOOK_Show_Config(p_decode->show_hdle, &show_cfg);
}

/*
************************************************************************************************************************
*                       				__decode_analysis_config
*
*Description: 配置解码模块相关参数
*
*Arguments  : hdle： 操作句柄
*
*
*Return     : NULL
*
************************************************************************************************************************
*/
static void __decode_analysis_config(__mbook_decode_t *hdle)
{
    __one_page_t            tmp_page;
    __analysis_config_t     analysis_cfg;
    __mbook_decode_t       *p_decode;
    __decode_config_t      *p_config;

    p_decode = hdle;
    p_config = &p_decode->config;
    // 读取当前页的信息
    MBOOK_Analysis_GetInfo(p_decode->analysis_hdle, p_decode->current_page, &tmp_page);
    // 配置文件解析模块
    if(p_config->show_rotate == 0)
    {
        analysis_cfg.show_width = p_config->show_width - 2 * p_config->border_width + p_config->row_space;
        analysis_cfg.page_row   = (p_config->show_height - p_config->bottom_width) / (p_config->font_size + p_config->row_space);
        analysis_cfg.col_width  = p_config->font_size + p_config->col_space;
        analysis_cfg.char_table = p_decode->char_table;
        analysis_cfg.charset    = p_decode->charset;
    }
    else
    {
        analysis_cfg.show_width = p_config->show_height - 2 * p_config->border_width + p_config->row_space;
        analysis_cfg.page_row   = (p_config->show_width - p_config->bottom_width) / (p_config->font_size + p_config->row_space);
        analysis_cfg.col_width  = p_config->font_size + p_config->col_space;
        analysis_cfg.char_table = p_decode->char_table;
        analysis_cfg.charset    = p_decode->charset;
    }
    MBOOK_Analysis_Config(p_decode->analysis_hdle, &analysis_cfg);
    // 启动文件分析
    MBOOK_Analysis_Work(p_decode->analysis_hdle);
    // 获取总页数
    p_decode->total_page = MBOOK_Analysis_GetTotalPage(p_decode->analysis_hdle);
    // 定位当前页
    p_decode->current_page = MBOOK_Analysis_GetPage(p_decode->analysis_hdle, tmp_page.page_start);
}


__s32	MBOOK_Show_LastPage(H_DECODE hdle, __u32 page_no)
{
    __u8					err;
    __mbook_decode_t		*p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;

    if((page_no > p_decode->total_page) || (page_no < 1))
    {
        return p_decode->current_page;
    }
    if(p_decode->current_page == page_no)
    {
        return p_decode->current_page;
    }
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);

    p_decode->current_page = page_no;

    esKRNL_SemPost(p_decode->decode_sem);

    p_decode->move_height = 0;
    p_decode->move_state = 0;

    __decode_get_page(p_decode, DECODE_CURR);
    __decode_show_page(p_decode);

    MBOOK_Layer_OpenShow(p_decode->show_lyr);
    GUI_LyrWinSetTop(p_decode->show_lyr);
    return p_decode->current_page;
}

__s32  MBOOK_Decode_get_filename(char *filename)
{
    HRATNPL				h_rat_npl;
    __s32				npl_index;


    h_rat_npl = rat_npl_open(RAT_MEDIA_TYPE_EBOOK);
    npl_index = rat_npl_get_cur(h_rat_npl);
    rat_npl_index2file(h_rat_npl, npl_index, filename);
    __msg("------------filename=%s\n----------", filename);
    rat_npl_close(h_rat_npl);

    filename = eLIBs_strchrlast(filename, '\\') + 1;
    __msg("------------filename=%s\n----------", filename);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_Init
*
*Description: 初始化页面解码模块
*
*Arguments  : filename：文件全路径名
*
*
*Return     : H_DECODE: 返回解码操作句柄 err :0 代表无错误，1代表文件不存在 2代表其它错误
*
************************************************************************************************************************
*/
H_DECODE   MBOOK_Decode_Init(char *filename, __u8 *err)
{
    __mbook_decode_t	*p_decode;

    *err = 0;

    p_decode = (__mbook_decode_t *)My_Malloc(0, sizeof(__mbook_decode_t));	// 申请内存
    if(p_decode == 0)
    {
        *err = 2;
        __err(" anole core malloc error \n");
        return NULL;
    }
    eLIBs_memset(p_decode, 0, sizeof(__mbook_decode_t));

    if(filename == NULL)
    {
        __s32		npl_index;
        HRATNPL 	h_rat_npl;

        h_rat_npl = rat_npl_open(RAT_MEDIA_TYPE_EBOOK);
        npl_index = rat_npl_get_cur(h_rat_npl);
        rat_npl_index2file(h_rat_npl, npl_index, p_decode->file_path);
        rat_npl_close(h_rat_npl);
    }
    else
    {
        eLIBs_memcpy(p_decode->file_path, filename, MAX_FILE_NAME);
    }
    p_decode->file_hdle = eLIBs_fopen(p_decode->file_path, "rb");
    if(p_decode->file_hdle == NULL)
    {
        *err = 1;
        goto INIT_ERROR_1;
    }
    p_decode->file_size = eLIBs_GetFileSize(p_decode->file_path);

    p_decode->analysis_hdle = MBOOK_Analysis_Init(p_decode->file_hdle);
    if(p_decode->analysis_hdle == NULL)
    {
        *err = 2;
        goto INIT_ERROR_2;
    }

    p_decode->show_hdle = MBOOK_Show_Init();
    if(p_decode->show_hdle == NULL)
    {
        *err = 2;
        goto INIT_ERROR_3;
    }

    p_decode->data_len      = 15;
    p_decode->page_data = (void *)My_Palloc(p_decode->data_len, 0);		// 申请内存
    if(p_decode->page_data == 0)
    {
        *err = 2;
        goto INIT_ERROR_4;
    }

    p_decode->total_page        = 0;
    p_decode->current_page      = 0;

    p_decode->move_height       = 0;
    p_decode->move_state        = 0;
    p_decode->charset           = EPDK_CHARSET_ENM_GBK;
    p_decode->default_charset   = EPDK_CHARSET_ENM_GBK;

    p_decode->decode_cnt	= DECODE_MBOOK_NUM / 2;
    p_decode->decode_sta 	= DECODE_NULL;	// 解码状态位
    p_decode->decode_sem 	= esKRNL_SemCreate(1);	// 解码控制信号量
    p_decode->decode_tsk	= esKRNL_TCreate(__decode_task, (void *)p_decode, 0x2000, KRNL_priolevel5);		// 加载预解线程

    return p_decode;

INIT_ERROR_4:
    if(p_decode->show_hdle)
    {
        MBOOK_Show_Uninit(p_decode->show_hdle);
        p_decode->show_hdle = NULL;
    }

INIT_ERROR_3:
    if(p_decode->analysis_hdle)
    {
        MBOOK_Analysis_Uninit(p_decode->analysis_hdle);
        p_decode->analysis_hdle = NULL;
    }

INIT_ERROR_2:
    if(p_decode->file_hdle)
    {
        eLIBs_fclose(p_decode->file_hdle);
        p_decode->file_hdle = NULL;
    }

INIT_ERROR_1:

    if(p_decode)
    {
        My_Mfree(0, p_decode);
        p_decode = NULL;
    }

    return NULL;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_Config
*
*Description: 配置页面解码模块
*
*Arguments  : hdle: 操作句柄
*             config: 参数配置信息指针
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_Config(H_DECODE hdle, __decode_config_t *config)
{
    __u32                   i;
    __show_config_t         show_cfg;
    __analysis_config_t     analysis_cfg;
    __mbook_decode_t       *p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;
    // 获取文件编码格式
    __decode_get_charset(p_decode);
    // 获取字符字库信息
    eLIBs_memcpy(&(p_decode->config), config, sizeof(__decode_config_t));

    p_decode->font_hdle     = MBOOK_Font_Init(config->char_font, config->font_size);
    p_decode->char_font     = MBOOK_Font_GetFont(p_decode->font_hdle);
    p_decode->char_table    = MBOOK_Font_GetTable(p_decode->font_hdle);

    if(config->show_rotate == 0)
    {
        for(i = 0; i < DECODE_MBOOK_NUM; i++)
        {
            p_decode->page[i].page_sta  = 0;
            p_decode->page[i].page_no   = 0;
            p_decode->page[i].page_lyr  = MBOOK_Layer_CreatLayer(config->show_width, config->show_height - config->bottom_width, 1);	// 申请软图层
            if(p_decode->page[i].page_lyr == NULL)
            {
                goto INIT_ERROR_1;
            }
        }

        // 创建显示图层
        p_decode->show_lyr  = MBOOK_Layer_CreatLayer(config->show_width, config->show_height - config->bottom_width, 1);	// 申请显示图层
        if(p_decode->show_lyr == NULL)
        {
            goto INIT_ERROR_2;
        }

        // 配置文件解析模块
        analysis_cfg.show_width = config->show_width - 2 * config->border_width + config->row_space;
        analysis_cfg.page_row   = (config->show_height - config->bottom_width) / (config->font_size + config->row_space);
        analysis_cfg.col_width  = config->font_size + config->col_space;
        analysis_cfg.char_table = p_decode->char_table;
        analysis_cfg.charset    = p_decode->charset;
        MBOOK_Analysis_Config(p_decode->analysis_hdle, &analysis_cfg);

        // 配置数据读写模块
        show_cfg.show_start = config->border_width;
        show_cfg.show_width = config->show_width - 2 * config->border_width + config->row_space - config->scroll_width;
        show_cfg.page_row   = (config->show_height - config->bottom_width) / (config->font_size + config->row_space);
        show_cfg.row_height = config->font_size + config->row_space;
        show_cfg.col_width  = config->font_size + config->col_space;
        show_cfg.font_color = config->font_color;
        show_cfg.back_color = config->back_color;
        show_cfg.char_font  = p_decode->char_font;
        show_cfg.char_table = p_decode->char_table;
        show_cfg.charset    = p_decode->charset;
        MBOOK_Show_Config(p_decode->show_hdle, &show_cfg);

        // 启动文件分析
        MBOOK_Analysis_Work(p_decode->analysis_hdle);
    }
    else
    {
        for(i = 0; i < DECODE_MBOOK_NUM; i++)
        {
            p_decode->page[i].page_sta  = 0;
            p_decode->page[i].page_no   = 0;
            p_decode->page[i].page_lyr  = MBOOK_Layer_CreatLayer(config->show_width - config->bottom_width, config->show_height, 1);	// 申请软图层
            if(p_decode->page[i].page_lyr == NULL)
            {
                goto INIT_ERROR_1;
            }
        }

        p_decode->show_lyr  = MBOOK_Layer_CreatLayer(config->show_width - config->bottom_width, config->show_height, 1);	// 申请显示图层
        if(p_decode->show_lyr == NULL)
        {
            goto INIT_ERROR_2;
        }

        // 配置文件解析模块
        analysis_cfg.show_width = config->show_height - 2 * config->border_width + config->row_space - config->scroll_width;
        analysis_cfg.page_row   = (config->show_width - config->bottom_width) / (config->font_size + config->row_space);
        analysis_cfg.col_width  = config->font_size + config->col_space;
        analysis_cfg.char_table = p_decode->char_table;
        analysis_cfg.charset    = p_decode->charset;
        MBOOK_Analysis_Config(p_decode->analysis_hdle, &analysis_cfg);

        // 配置数据读写模块
        show_cfg.show_start = config->border_width;
        show_cfg.show_width = config->show_height - 2 * config->border_width + config->row_space;
        show_cfg.page_row   = (config->show_width - config->bottom_width) / (config->font_size + config->row_space);
        show_cfg.row_height = config->font_size + config->row_space;
        show_cfg.col_width  = config->font_size + config->col_space;
        show_cfg.font_color = config->font_color;
        show_cfg.back_color = config->back_color;
        show_cfg.char_font  = p_decode->char_font;
        show_cfg.char_table = p_decode->char_table;
        show_cfg.charset    = p_decode->charset;
        MBOOK_Show_Config(p_decode->show_hdle, &show_cfg);

        // 启动文件分析
        MBOOK_Analysis_Work(p_decode->analysis_hdle);
    }
    __here__
    // 获取总页数
    p_decode->current_page  = 1;
    p_decode->total_page    = MBOOK_Analysis_GetTotalPage(p_decode->analysis_hdle);
    __here__
    return EPDK_OK;

INIT_ERROR_2:
INIT_ERROR_1:
    for(i = 0; i < DECODE_MBOOK_NUM; i++)
    {
        if(p_decode->page[i].page_lyr)
        {
            MBOOK_Layer_DeleteLayer(p_decode->page[i].page_lyr);
            p_decode->page[i].page_lyr = NULL;
        }
    }

    return EPDK_FAIL;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_ShowPage
*
*Description: 显示当前页面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_ShowPage(H_DECODE hdle, __u32 offset)
{
    __mbook_decode_t       *p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;
    p_decode->current_page = MBOOK_Analysis_GetPage(p_decode->analysis_hdle, offset);

    __decode_get_page(p_decode, DECODE_CURR);

    __decode_show_page(p_decode);

    MBOOK_Layer_OpenShow(p_decode->show_lyr);
    GUI_LyrWinSetTop(p_decode->show_lyr);
    return p_decode->current_page;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_ShowNext
*
*Description: 显示下一页面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_ShowNext(H_DECODE hdle)
{
    __mbook_decode_t       *p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;
    if(p_decode->current_page < p_decode->total_page)
    {
        p_decode->move_height = 0;
        p_decode->move_state = 0;

        __decode_get_page(p_decode, DECODE_NEXT);
        __decode_show_page(p_decode);
    }

    return p_decode->current_page;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_ShowPrev
*
*Description: 显示上一页面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_ShowPrev(H_DECODE hdle)
{
    __mbook_decode_t       *p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;
    if(p_decode->current_page > 1)
    {
        p_decode->move_height = 0;
        p_decode->move_state = 0;

        __decode_get_page(p_decode, DECODE_PREV);
        __decode_show_page(p_decode);
    }

    return p_decode->current_page;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_SeekPage
*
*Description: 跳转到指定页面
*
*Arguments  : hdle: 操作句柄
*             page_no: 要显示的页码
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_SeekPage(H_DECODE hdle, __u32 page_no)
{
    __u8                    err;
    __mbook_decode_t       *p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;

    if((page_no > p_decode->total_page) || (page_no < 1))
    {
        return p_decode->current_page;
    }
    if(p_decode->current_page == page_no)
    {
        return p_decode->current_page;
    }
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);

    p_decode->current_page = page_no;

    esKRNL_SemPost(p_decode->decode_sem);

    p_decode->move_height = 0;
    p_decode->move_state = 0;

    __decode_get_page(p_decode, DECODE_CURR);
    __decode_show_page(p_decode);

    return p_decode->current_page;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_SeekOffset
*
*Description: 跳转到指定偏移量
*
*Arguments  : hdle: 操作句柄
*             offset: 文件偏移量
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_SeekOffset(H_DECODE hdle, __u32 offset)
{
    __u8                    err;
    __u32                   tmp_page;
    __mbook_decode_t       *p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;

    tmp_page = MBOOK_Analysis_GetPage(p_decode->analysis_hdle, offset);

    esKRNL_SemPend(p_decode->decode_sem, 0, &err);

    p_decode->current_page = tmp_page;

    esKRNL_SemPost(p_decode->decode_sem);

    p_decode->move_height = 0;
    p_decode->move_state = 0;

    __decode_get_page(p_decode, DECODE_CURR);
    __decode_show_page(p_decode);

    return p_decode->current_page;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_MovePage
*
*Description: 跳转到指定页面
*
*Arguments  : hdle: 操作句柄
*             x, y: 移动的偏移量
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_MovePage(H_DECODE hdle, __s32 x, __s32 y)
{
    __s32                   tmp_step;
    __mbook_decode_t       *p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;

    if(p_decode->config.show_rotate == 0)
    {
        tmp_step = y;
    }
    else
    {
        tmp_step = x;
    }

    if((tmp_step > 200) || ((tmp_step > 0) && (tmp_step < 20)))
    {
        tmp_step = 25;
    }
    else if(((tmp_step > -20) && (tmp_step < 0)) || (tmp_step < -200))
    {
        tmp_step = -25;
    }

    if(tmp_step < 0)
    {
        __decode_move_down(p_decode, 0 - tmp_step);
    }
    else
    {
        __decode_move_up(p_decode, tmp_step);
    }

    return p_decode->current_page;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_RotatePage
*
*Description: 旋转当前页面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 成功：0 - 0度，90 - 90度
*			  失败：-1
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_RotatePage(H_DECODE hdle)
{
    __u32                   i;
    __u8                    err;
    __mbook_decode_t       *p_decode;

    if(hdle == NULL)
    {
        return -1;
    }
    p_decode = (__mbook_decode_t *)hdle;
    if(p_decode->config.show_rotate == 0)
    {
        p_decode->config.show_rotate = 90;
    }
    else
    {
        p_decode->config.show_rotate = 0;
    }
    // 挂起预解线程
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);

    // 删除图层
    for(i = 0; i < DECODE_MBOOK_NUM; i++)
    {
        if(p_decode->page[i].page_lyr)
        {
            MBOOK_Layer_DeleteLayer(p_decode->page[i].page_lyr);
            p_decode->page[i].page_lyr = NULL;
        }
    }
    if(p_decode->show_lyr)
    {
        MBOOK_Layer_DeleteLayer(p_decode->show_lyr);
        p_decode->show_lyr = NULL;
    }
    //创建图层
    if(p_decode->config.show_rotate == 0)
    {
        for(i = 0; i < DECODE_MBOOK_NUM; i++)
        {
            p_decode->page[i].page_sta  = 0;
            p_decode->page[i].page_no   = 0;
            p_decode->page[i].page_lyr  = MBOOK_Layer_CreatLayer(p_decode->config.show_width, p_decode->config.show_height - p_decode->config.bottom_width, 1);	// 申请软图层
            if(p_decode->page[i].page_lyr == NULL)
            {
                goto INIT_ERROR_1;
            }
        }
        p_decode->show_lyr  = MBOOK_Layer_CreatLayer(p_decode->config.show_width, p_decode->config.show_height - p_decode->config.bottom_width, 1);	// 申请显示图层
        if(p_decode->show_lyr == NULL)
        {
            goto INIT_ERROR_2;
        }
    }
    else
    {
        for(i = 0; i < DECODE_MBOOK_NUM; i++)
        {
            p_decode->page[i].page_sta  = 0;
            p_decode->page[i].page_no   = 0;
            p_decode->page[i].page_lyr  = MBOOK_Layer_CreatLayer(p_decode->config.show_width - p_decode->config.bottom_width, p_decode->config.show_height, 1);	// 申请软图层
            if(p_decode->page[i].page_lyr == NULL)
            {
                goto INIT_ERROR_1;
            }
        }
        p_decode->show_lyr  = MBOOK_Layer_CreatLayer(p_decode->config.show_width - p_decode->config.bottom_width, p_decode->config.show_height, 1);	// 申请显示图层
        if(p_decode->show_lyr == NULL)
        {
            goto INIT_ERROR_2;
        }
    }

    // 配置解码
    __decode_analysis_config(p_decode);

    // 配置显示
    __decode_brush_page(p_decode, DECODE_CURR);
    __decode_show_config(p_decode);

    // 恢复预解线程
    esKRNL_SemPost(p_decode->decode_sem);

    // 获取当前页
    p_decode->move_height   = 0;
    p_decode->move_state    = 0;

    __decode_get_page(p_decode, DECODE_CURR);
    __decode_show_page(p_decode);
    MBOOK_Layer_OpenShow(p_decode->show_lyr);

    return p_decode->config.show_rotate;

INIT_ERROR_2:
INIT_ERROR_1:
    for(i = 0; i < DECODE_MBOOK_NUM; i++)
    {
        if(p_decode->page[i].page_lyr)
        {
            MBOOK_Layer_DeleteLayer(p_decode->page[i].page_lyr);
            p_decode->page[i].page_lyr = NULL;
        }
    }

    return -1;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_RotatePage
*
*Description: 旋转当前页面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 成功：0度，90度
*			  失败：返回 EPDK_FAIL
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_GetRotate(H_DECODE hdle)
{
    __mbook_decode_t        *p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;

    return p_decode->config.show_rotate;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_GetTotalPage
*
*Description: 旋转当前页面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 成功：回放总页数
*			  失败：返回 0
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_GetTotalPage(H_DECODE hdle)
{
    __mbook_decode_t        *p_decode;

    if(hdle == NULL)
    {
        return 0;
    }
    p_decode = (__mbook_decode_t *)hdle;

    return p_decode->total_page;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_GetCurrentPage
*
*Description: 旋转当前页面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 成功：回放当前页数
*			  失败：返回 0
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_GetCurrentPage(H_DECODE hdle)
{
    __mbook_decode_t        *p_decode;

    if(hdle == NULL)
    {
        return 0;
    }
    p_decode = (__mbook_decode_t *)hdle;

    return p_decode->current_page;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_GetCurrentPage
*
*Description: 获取指定页的页面偏移值
*
*Arguments  : hdle: 操作句柄
*             page: 指定页码
*
*Return     : 成功：指定页面偏移量
*			  失败：返回-1
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_GetPageOffset(H_DECODE hdle, __u32 page_no)
{
    __one_page_t        tmp_page;
    __mbook_decode_t   *p_decode;

    if(hdle == NULL)
    {
        return -1;
    }
    p_decode = (__mbook_decode_t *)hdle;
    if((page_no > p_decode->total_page) || (page_no < 1))
    {
        return -1;
    }
    MBOOK_Analysis_GetInfo(p_decode->analysis_hdle, page_no, &tmp_page);

    return tmp_page.page_start;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_GetFileName
*
*Description: 获取文件的文件名
*
*Arguments  : hdle: 操作句柄
*             name: 文件名
*
*Return     : 成功：PDK_OKK
*			  失败：EPDK_FAIL
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_GetFileName(H_DECODE hdle, char *name)
{
    char                   *p;
    __mbook_decode_t       *p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;

    p = eLIBs_strchrlast(p_decode->file_path, '\\');
    eLIBs_strcpy(name, p + 1);

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_GetFileSize
*
*Description: 获取文件的文件名
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 成功：总大小
*			  失败：-1
*
************************************************************************************************************************
*/
__s64   MBOOK_Decode_GetFileSize(H_DECODE hdle)
{
    __mbook_decode_t       *p_decode;

    if(hdle == NULL)
    {
        return -1;
    }
    p_decode = (__mbook_decode_t *)hdle;

    return p_decode->file_size;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_GetFilePath
*
*Description: 获取文件的全路径名
*
*Arguments  : hdle: 操作句柄
*             path: 路径名
*
*Return     : 成功：PDK_OKK
*			  失败：EPDK_FAIL
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_GetFilePath(H_DECODE hdle, char *path)
{
    __mbook_decode_t       *p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;

    eLIBs_strcpy(path, p_decode->file_path);

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_GetFileContext
*
*Description: 获取当前页面的信息
*
*Arguments  : hdle: 操作句柄
*             context: 文件内容
*
*Return     : 成功：PDK_OKK
*			  失败：EPDK_FAIL
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_GetFileContext(H_DECODE hdle, char *context)
{
    __one_page_t        tmp_page;
    __mbook_decode_t	*p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;

    MBOOK_Analysis_GetInfo(p_decode->analysis_hdle, p_decode->current_page, &tmp_page);
    eLIBs_fseek(p_decode->file_hdle, tmp_page.page_start, ELIBS_SEEK_SET);
    eLIBs_fread(context, 1, sizeof(context), p_decode->file_hdle);

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_SetRowSpace
*
*Description: 设置行间距
*
*Arguments  : hdle: 操作句柄
*             space: 行间距
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_SetRowSpace(H_DECODE hdle, __u32 space)
{
    __u8                    err;
    __mbook_decode_t       *p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;
    if(p_decode->config.row_space == space)
    {
        return EPDK_OK;
    }
    p_decode->config.row_space = space;

    // 挂起预解线程
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);

    // 配置解码
    __decode_analysis_config(p_decode);

    // 配置显示
    __decode_brush_page(p_decode, DECODE_CURR);
    __decode_show_config(p_decode);

    // 恢复预解线程
    esKRNL_SemPost(p_decode->decode_sem);

    // 获取当前页
    __decode_get_page(p_decode, DECODE_CURR);
    if(p_decode->move_height)
    {
        __decode_get_next_page(p_decode);
    }
    __decode_show_page(p_decode);

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_SetColSpace
*
*Description: 设置列间距
*
*Arguments  : hdle: 操作句柄
*             space: 列间距
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_SetColSpace(H_DECODE hdle, __u32 space)
{
    __u8                    err;
    __mbook_decode_t       *p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;
    if(p_decode->config.col_space == space)
    {
        return EPDK_OK;
    }
    p_decode->config.col_space = space;

    // 挂起预解线程
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);

    // 配置解码
    __decode_analysis_config(p_decode);

    // 配置显示
    __decode_brush_page(p_decode, DECODE_CURR);
    __decode_show_config(p_decode);

    // 恢复预解线程
    esKRNL_SemPost(p_decode->decode_sem);

    // 获取当前页
    __decode_get_page(p_decode, DECODE_CURR);
    if(p_decode->move_height)
    {
        __decode_get_next_page(p_decode);
    }
    __decode_show_page(p_decode);

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_SetBorderWidth
*
*Description: 设置边距
*
*Arguments  : hdle: 操作句柄
*             space: 边距
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_SetBorderWidth(H_DECODE hdle, __u32 width)
{
    __u8                    err;
    __mbook_decode_t       *p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;
    if(p_decode->config.border_width == width)
    {
        return EPDK_OK;
    }
    p_decode->config.border_width = width;
    __msg("p_decode->config.border_width = %d \n", p_decode->config.border_width);
    // 挂起预解线程
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);

    // 配置解码
    __decode_analysis_config(p_decode);

    // 配置显示
    __decode_show_config(p_decode);

    __decode_brush_page(p_decode, DECODE_CURR);

    // 恢复预解线程
    esKRNL_SemPost(p_decode->decode_sem);

    // 获取当前页
    __decode_get_page(p_decode, DECODE_CURR);
    if(p_decode->move_height)
    {
        __decode_get_next_page(p_decode);
    }
    __decode_show_page(p_decode);

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_SetBackColor
*
*Description: 设置显示的背景色
*
*Arguments  : hdle: 操作句柄
*             color: 颜色
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_SetBackColor(H_DECODE hdle, __u32 color)
{
    __mbook_decode_t       *p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;
    if(p_decode->config.back_color == color)
    {
        return EPDK_OK;
    }
    p_decode->config.back_color = color;

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_SetFontColor
*
*Description: 设置显示文字的颜色
*
*Arguments  : hdle: 操作句柄
*             color: 颜色
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_SetFontColor(H_DECODE hdle, __u32 color)
{
    __mbook_decode_t       *p_decode;
    __u8 err;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;
    if(p_decode->config.font_color == color)
    {
        return EPDK_OK;
    }
    p_decode->config.font_color = color;

    // 挂起预解线程
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);

    // 配置解码
    __decode_analysis_config(p_decode);

    // 配置显示
    __decode_brush_page(p_decode, DECODE_CURR);
    __decode_show_config(p_decode);

    // 恢复预解线程
    esKRNL_SemPost(p_decode->decode_sem);

    // 获取当前页
    __decode_get_page(p_decode, DECODE_CURR);

    __decode_show_page(p_decode);

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_SetDefaultCharset
*
*Description: 设置默认编码格式
*
*Arguments  : hdle: 操作句柄
*             charset: 默认编码
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_SetDefaultCharset(H_DECODE hdle, __epdk_charset_enm_e charset)
{
    __u8                    err;
    __mbook_decode_t       *p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;
    if(p_decode->default_charset == charset)
    {
        return EPDK_OK;
    }
    p_decode->default_charset = charset;

    if((p_decode->charset == EPDK_CHARSET_ENM_UTF16LE)
            || (p_decode->charset == EPDK_CHARSET_ENM_UTF16BE)
            || (p_decode->charset == EPDK_CHARSET_ENM_UTF8))
    {
        // 非未知编码
        return EPDK_OK;
    }
    if(p_decode->decode_sta == DECODE_NULL)
    {
        // 未开始解码
        return EPDK_OK;
    }

    // 挂起预解线程
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);

    // 获取文件编码格式
    __decode_get_charset(p_decode);

    // 配置解码
    __decode_analysis_config(p_decode);

    // 配置显示
    __decode_brush_page(p_decode, DECODE_CURR);
    __decode_show_config(p_decode);

    // 恢复预解线程
    esKRNL_SemPost(p_decode->decode_sem);

    // 获取当前页
    __decode_get_page(p_decode, DECODE_CURR);
    if(p_decode->move_height)
    {
        __decode_get_next_page(p_decode);
    }
    __decode_show_page(p_decode);

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                       				MBOOK_Decode_Uninit
*
*Description: 删除页码解码模块
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*			  EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_Uninit(H_DECODE hdle)
{
    __s32			        i;
    __u8 			        err;
    __mbook_decode_t        *p_decode;

    if(hdle == NULL)
    {
        return EPDK_FAIL;
    }
    p_decode = (__mbook_decode_t *)hdle;

    if(p_decode->decode_tsk)
    {
        while(1)
        {
            if(esKRNL_TDelReq(p_decode->decode_tsk) == OS_TASK_NOT_EXIST)
                break;
            esKRNL_TimeDly(1);
        }
        p_decode->decode_tsk = 0;
    }
    if(p_decode->decode_sem)
    {
        esKRNL_SemDel(p_decode->decode_sem, OS_DEL_ALWAYS, &err);
        p_decode->decode_sem = NULL;
    }

    if(p_decode->page_data)
    {
        My_Pfree(p_decode->page_data, p_decode->data_len);
        p_decode->page_data = NULL;
    }

    for(i = 0; i < DECODE_MBOOK_NUM; i++)
    {
        if(p_decode->page[i].page_lyr)
        {
            MBOOK_Layer_DeleteLayer(p_decode->page[i].page_lyr);
            p_decode->page[i].page_lyr = NULL;
        }
    }

    if(p_decode->show_lyr)
    {
        MBOOK_Layer_DeleteLayer(p_decode->show_lyr);
        p_decode->show_lyr = NULL;
    }

    if(p_decode->show_hdle)
    {
        MBOOK_Show_Uninit(p_decode->show_hdle);
        p_decode->show_hdle = NULL;
    }

    if(p_decode->analysis_hdle)
    {
        MBOOK_Analysis_Uninit(p_decode->analysis_hdle);
        p_decode->analysis_hdle = NULL;
    }

    if(p_decode->font_hdle)
    {
        MBOOK_Font_Uninit(p_decode->font_hdle);
        p_decode->font_hdle = NULL;
    }

    if(p_decode->file_hdle)
    {
        eLIBs_fclose(p_decode->file_hdle);
        p_decode->file_hdle = NULL;
    }

    if(p_decode)
    {
        My_Mfree(0, p_decode);
        p_decode = NULL;
    }

    return EPDK_OK;
}


