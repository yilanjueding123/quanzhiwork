/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           willow app sample
*
*						        (c) Copyright 2006-2007, RICHARD, China
*											All	Rights Reserved
*
* File    : anole.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/


#include "anole_i.h"
//#include "TestTimer.h"


__mp *mod_willow;
__u8 mid_willow;
static __anole_core_t anole_core;
static void *decoding_buf = NULL;
static void *unsupported_buf = NULL;
static __pCBK_t 	volatile  	cb_show_pic_fail  		= NULL;
static void 		*volatile 	cb_ctx_show_pic_fail 	= NULL;

//自动播放回调
static __pCBK_t 	volatile  	cb_show_pic_next  		= NULL;
static void 		*volatile 	cb_ctx_show_pic_next 	= NULL;


static HRATNPL     volatile  	anole_npl       = NULL;

static __s32 		LCD_WIDTH = 0;
static __s32 		LCD_HEIGHT = 0;

#define ANOLE_SYN_DELAY_TIME 50//
#define ANOLE_AUTO_PLAY_THREAD_DELAY_TIME	5

#define trans_to_willow_switchmode()  ((anole_core.SwitchMode == ANOLE_FLAG_RANDOM_MODE) ? (anole_core.switchmodearray[esKSRV_Random(100) % anole_core.arraynum]) : (anole_core.SwitchMode))

#define anole_error_proc(_cur_file_name, _cb_err_show, _cb_para) if (_cb_err_show) {esKRNL_CallBack( _cb_err_show, _cb_para );}

__krnl_event_t *g_anole_sem = NULL;

#define ANOLE_ENTER_CRITICAL_SECTION\
{\
	__u8 err;\
	if(g_anole_sem)\
	{\
        __msg("**********ANOLE_ENTER_CRITICAL_SECTION*****************\n");\
	    esKRNL_SemPend(g_anole_sem, 0, &err);\
	    __here__;\
	}\
}

#define ANOLE_LEAVE_CRITICAL_SECTION\
{\
    if(g_anole_sem)\
    {\
        __msg("**********ANOLE_LEAVE_CRITICAL_SECTION*****************\n");\
	    esKRNL_SemPost(g_anole_sem);\
	    __here__;\
    }\
}


__s32 anole_init_screen_size(__s32 *p_width, __s32 *p_height)
{
    ES_FILE 			 *p_disp = NULL;

    /* open display driver */
    p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if( p_disp == NULL )
    {
        __err("Error in opening display driver.\n");
        return EPDK_FAIL;
    }

    *p_width  = eLIBs_fioctrl(p_disp, DISP_CMD_SCN_GET_WIDTH, 0, 0);
    *p_height = eLIBs_fioctrl(p_disp, DISP_CMD_SCN_GET_HEIGHT, 0, 0);


    eLIBs_fclose(p_disp);

    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               __wait_willow_ready
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static void __wait_willow_ready(void)
{
    __u8 status;
    __s32 count;

    count = ANOLE_SYN_DELAY_TIME;

    /*check willow status*/
#ifndef SIM_PC_WIN
    while(count)
    {
        __here__;
        status = esMODS_MIoctrl(mod_willow, WILLOW_CMD_QUERY_STATUS, 0, 0);
        __here__;
        if (status != WILLOW_STATUS_BUSY)
            break;

        //esKRNL_TimeDly(10);
        esKRNL_TimeDly(1);
        count--;
        __msg("count=%d\n", count);
    }
#endif
}
/*
**********************************************************************************************************************
*                                               __wait_willow_finish
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

static void __wait_willow_finish(void)
{
    __u8 status;

    __s32 count;

    count = ANOLE_SYN_DELAY_TIME;

    /*check willow status*/
    while(count)
    {
        __here__;
        status = esMODS_MIoctrl(mod_willow, WILLOW_CMD_QUERY_STATUS, 0, 0);
        __here__;
        if (status == WILLOW_STATUS_FINISH)
            break;
        //esKRNL_TimeDly(10);
        esKRNL_TimeDly(1);		///by linyaoshu
        count--;
    }
}

/***************************************************************************************************
*Name        : create_npl
*Prototype   : __s32 create_npl( __s32 media_type )
*Arguments   : media_type     input. media type, such as audio, video and so on
*Return      : ==0    succeed
*              !=0    fail
*Description : create now playlist
*Other       :
***************************************************************************************************/
static __s32 __npl_create( rat_media_type_t media_type )
{
    if( anole_npl == NULL )
    {
        anole_npl = rat_npl_open(media_type);

        if( anole_npl == NULL )
        {
            __err("Error in creating now play list!\n");
            return -1;
        }
    }

    return 0;
}


/***************************************************************************************************
*Name        : delete_npl
*Prototype   : void  delete_npl( void )
*Arguments   : void
*Return      : void
*Description : delete now playlist
*Other       :
***************************************************************************************************/
static void  __npl_delete( void )
{
    if( anole_npl != NULL )
    {
        rat_npl_close(anole_npl);
        anole_npl = NULL;
    }
}

/***************************************************************************************************
*Name        : anole_npl_get_next
*Prototype   : __s32  anole_npl_get_next( void )
*Arguments   : void
*Return      : the index of the next file to be played in the now play list. if return -1, that means
*              no item to play in current "play mode" or some error occured.
*Description : get the index of the next file to be played in the now play list according to the
*              current "play mode" and the current item. this operation will adjust the playing
*              pointer to the next item.
*Other       :
***************************************************************************************************/
__s32  anole_npl_get_next( void )
{
    __s32  ret;
    __s32  index;

    index = rat_npl_get_next(anole_npl);

    return index;
}

/***************************************************************************************************
*Name        : anole_npl_get_prev
*Prototype   : __s32  anole_npl_get_prev( void )
*Arguments   : void
*Return      : the index of the item that the playing ponter points to. if return -1, that means fail
*Description : get the index of the item that the playing ponter points to.
*Other       :
***************************************************************************************************/
__s32  anole_npl_get_prev( void )
{
    __s32  ret;
    __s32  index;

    index = rat_npl_get_prev(anole_npl);

    return index;

}

/***************************************************************************************************
*Name        : anole_npl_get_cur
*Prototype   : __s32  anole_npl_get_prev( void )
*Arguments   : void
*Return      : the index of the item that the playing ponter points to. if return -1, that means fail
*Description : get the index of the item that the playing ponter points to.
*Other       :
***************************************************************************************************/
__s32  anole_npl_get_cur( void )
{
    __s32  ret;
    __s32  index;

    __here__;
    index = rat_npl_get_cur(anole_npl);
    __here__;

    return index;

}


/***************************************************************************************************
*Name        : anole_npl_file2index
*Prototype   : __s32  anole_npl_file2index( const char *file )
*Arguments   : file     input. full path.
*Return      : the index value of the file in the now play list. If return -1. that means fail.
*Description : get the index value of the file in the now play list.
*Other       :
***************************************************************************************************/
__s32  anole_npl_file2index( const char *file )
{
    __u32  index;

    index = rat_npl_file2index(anole_npl, file);
    return index;
}

/***************************************************************************************************
*Name        : anole_npl_get_total_count
*Prototype   : __s32  anole_npl_get_total_count( void )
*Arguments   :
*Return      : total count in npl
*Description :
*Other       :
***************************************************************************************************/
__s32 anole_npl_get_total_count(void)
{
    return rat_npl_get_total_count(anole_npl);
}


/***************************************************************************************************
*Name        : anole_npl_index2file
*Prototype   : __s32  anole_npl_index2file( __u32 index, char *file )
*Arguments   : index     input. the item index in the now play list.
*              file      output. the file corresponding to the index.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the file corresponding to its index in the now play list.
*Other       :
***************************************************************************************************/
__s32  anole_npl_index2file( __u32 index, char *file )
{
    __s32 ret;
    __here__;
    ret = rat_npl_index2file(anole_npl, index, file);
    __here__;
    return ret;
}

/***************************************************************************************************
*Name        : anole_npl_playing_seek
*Prototype   : __s32  anole_npl_playing_seek( __s32 index)
*Arguments   :
*Return      : seek npl
*Description :
*Other       :
***************************************************************************************************/
__s32 anole_npl_playing_seek(__s32 index)
{
    return rat_npl_set_cur(anole_npl, index);
}


/*
**********************************************************************************************************************
*                                               AutoPlay_ThreadDel
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

static __s32  anole_auto_play_thread_del( void )
{
    /* delete autoplay thread */
    if( anole_core.maintask != 0 )
    {
        while( esKRNL_TDelReq(anole_core.maintask) != OS_TASK_NOT_EXIST )
        {
            __here__
            esKRNL_TimeDly(5);
            //esKRNL_TimeDlyResume(anole_core.maintask);
        }
        anole_core.maintask = 0;
        __msg("\"loading\" thread has been deleted.\n");
    }

    return 0;
}

static __s32 __do_play_file(__willow_show_file_param_t *param)
{
    __msg("index = %d, file name = %s\n", param->img_no, param->filename);

    //set show img node
    __wait_willow_ready();
    __here__;
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SHOW_IMG_FROM_FILE, 0, param);
    __here__;
    __wait_willow_finish();
    __here__;

    //check if the format is supported
    if(esMODS_MIoctrl(mod_willow, WILLOW_CMD_CHECK_IMG, 0, param) == EPDK_FAIL)
    {
        __msg("the file format is not supported!\n");
        __wait_willow_finish();
        __here__;
        return EPDK_FAIL;
    }
    __here__;

    //start decoding
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_START_DEC, 0, 0);
    __here__;
    __wait_willow_finish();
    __here__;
    //TestTimerReport(__FILE__, __LINE__);
    //start show
    __wait_willow_ready();
    __here__;
    //TestTimerReport(__FILE__, __LINE__);
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_START_SHOW, 0, 0);
    __here__;
    //TestTimerReport(__FILE__, __LINE__);
    __wait_willow_finish();
    __here__;
    //TestTimerReport(__FILE__, __LINE__);
    return EPDK_OK;
}

__s32 anole_play_file(char *filename)
{
    __s32 ret;
    __willow_show_file_param_t param;

    if(filename == NULL)
    {
        __msg("filename is NULL!\n");
        return EPDK_FAIL;
    }
    __here__;
    param.img_no 	= anole_npl_file2index(filename);
    param.filename 	= filename;
    __here__;
    ret = __do_play_file(&param);
    __here__;
    if(ret == -1)
    {
        __here__;
        ANOLE_ENTER_CRITICAL_SECTION
        anole_error_proc(filename, cb_show_pic_fail, cb_ctx_show_pic_fail);//esKRNL_CallBack( cb_show_pic_fail, cb_ctx_show_pic_fail );
        ANOLE_LEAVE_CRITICAL_SECTION
        __here__;
    }
    __here__;

    return ret;
}

__s32 anole_play_file_by_index(__s32 index)
{
    __s32 ret;
    char  filename[512];

    ret = anole_npl_index2file(index, filename);

    if(-1 == ret)
    {
        __msg("anole_npl_index2file fail...\n");
        return EPDK_FAIL;
    }

    if (anole_core.SwitchMode == ANOLE_FLAG_RANDOM_MODE)
    {
        __s32 switch_mode;
        switch_mode = trans_to_willow_switchmode();

        esMODS_MIoctrl(mod_willow, WILLOW_CMD_SET_SWITCH_MODE, switch_mode, NULL);
    }

    ret = anole_play_file(filename);
    if (ret == EPDK_OK)
    {
        anole_npl_playing_seek(index);
    }
    return ret;
}

/*
**********************************************************************************************************************
*                                               anole_play_prev
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__bool anole_play_prev(void)
{
    __willow_show_file_param_t param;
    char path[ANOLE_MAX_FULL_FILE_PATH_LEN];
    __s32 filenum;
    __s32 total;
    __s32 ret;

    filenum = anole_npl_get_cur();
    if (filenum == -1)
    {
        return EPDK_FALSE;
    }

    total = anole_npl_get_total_count();
    if(-1 == total)
    {
        __msg("anole_npl_get_total_count = -1...\n");
        return EPDK_FALSE;
    }
    filenum--;
    if(filenum < 0)
    {
        filenum = total - 1;
    }
    if(filenum < 0)
    {
        filenum = 0;
    }

    param.img_no = filenum;
    param.filename = path;
    //ret = _show_pic_file(&param);
    ret = __do_play_file(&param);
    if (ret == -1)
    {
        ANOLE_ENTER_CRITICAL_SECTION
        anole_error_proc(param.filename, cb_show_pic_fail, cb_ctx_show_pic_fail);//esKRNL_CallBack( cb_show_pic_fail, cb_ctx_show_pic_fail );
        ANOLE_LEAVE_CRITICAL_SECTION
        return EPDK_FALSE;
    }
    return EPDK_TRUE;
}

/*
**********************************************************************************************************************
*                                               anole_play_next
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__bool anole_play_next(void)
{
    __willow_show_file_param_t param;
    char path[ANOLE_MAX_FULL_FILE_PATH_LEN];
    __s32 filenum;
    __s32 total;
    __s32 ret;

    filenum = anole_npl_get_cur();
    if (filenum == -1)
    {
        return EPDK_FALSE;
    }

    total = anole_npl_get_total_count();
    if(-1 == total)
    {
        __msg("anole_npl_get_total_count = -1...\n");
        return EPDK_FALSE;
    }
    filenum++;
    if(filenum >= total)
    {
        filenum = 0;
    }

    param.img_no = filenum;
    param.filename = path;
    //ret = _show_pic_file(&param);
    ret = __do_play_file(&param);
    if (ret == -1)
    {
        ANOLE_ENTER_CRITICAL_SECTION
        anole_error_proc(param.filename, cb_show_pic_fail, cb_ctx_show_pic_fail);//esKRNL_CallBack( cb_show_pic_fail, cb_ctx_show_pic_fail );
        ANOLE_LEAVE_CRITICAL_SECTION
        return EPDK_FALSE;
    }
    return EPDK_TRUE;
}

__s32 anole_stop(void)
{
    return 0;
}

/*
**********************************************************************************************************************
*                                               __do_autoplay
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

static __s32 __do_autoplay(void)
{
    char	filename[512];
    __s32 	index = 0;
    __s32 total;
    __s32 ret;

    __msg("__do_autoplay begin...\n");
    index = anole_npl_get_cur();
    __msg("anole_npl_get_cur() = %d \n", index);
    if (index == -1)
    {
        __here__;
        return EPDK_FAIL;
    }

    total = anole_npl_get_total_count();
    if(-1 == total)
    {
        __msg("anole_npl_get_total_count = -1...\n");
        return EPDK_FAIL;
    }
    index++;
    if(index >= total)
    {
        __here__;
        index = 0;
    }

    ret = anole_npl_index2file(index, filename);
    __msg("get photo(%d) name = %s\n", index, filename);
    if(-1 == ret)
    {
        __msg("anole_npl_index2file fail...\n");
        return EPDK_FAIL;
    }
    anole_npl_playing_seek(index);

    {
        __s32 switch_mode;
        switch_mode = trans_to_willow_switchmode();

        esMODS_MIoctrl(mod_willow, WILLOW_CMD_SET_SWITCH_MODE, switch_mode, NULL);
        //anole_SetSwitchMode(switch_mode);
    }
    anole_play_file(filename);

    __msg("__do_autoplay end...\n");

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               __auto_play_maintsk
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

static void __auto_play_maintsk(void *arg)
{
    __u32 orign_delay;

    while(1)
    {
        __here__;
        //if(esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        if(esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
        {
            __here__;
            goto _exit_AUTO_maintsk;
        }

        __here__;
        //orign_delay = anole_GetPlayInterval();
        if (anole_get_sta() == ANOLE_MODE_PLAY)
        {
            orign_delay = anole_get_play_interval();

            __here__;
            if (orign_delay != 1)
            {
                __u8 count = (orign_delay - 1) * 100 / ANOLE_AUTO_PLAY_THREAD_DELAY_TIME;	//延时多次
                while( count )
                {
                    esKRNL_TimeDly( ANOLE_AUTO_PLAY_THREAD_DELAY_TIME);	//每次延时ANOLE_AUTO_PLAY_THREAD_DELAY_TIME
                    if (anole_get_sta() != ANOLE_MODE_PLAY)
                    {
                        break ;
                    }
                    else
                    {
                        count-- ;
                    }
                }
            }

            __here__;
            /*adjust again*/
            if (anole_get_sta() == ANOLE_MODE_PLAY)
            {
                //				__do_autoplay();
                __here__;

                if (__do_autoplay() == EPDK_OK)
                {
                    __here__;
                    ANOLE_ENTER_CRITICAL_SECTION
                    if(cb_show_pic_next)
                    {
                        __msg("*********before esKRNL_CallBack( cb_show_pic_next*********\n");
                        esKRNL_CallBack( cb_show_pic_next, cb_ctx_show_pic_next );
                        __msg("*********after esKRNL_CallBack( cb_show_pic_next*********\n");
                    }
                    ANOLE_LEAVE_CRITICAL_SECTION
                    __here__;
                }
            }
        }
        esKRNL_TimeDly(5);
        __here__;
    }

_exit_AUTO_maintsk:
    __inf("__auto_play_maintsk thread delete!\n");
    //esKRNL_TDel(EXEC_prioself);
    esKRNL_TDel(OS_PRIO_SELF);
    return;

}


/*
**********************************************************************************************************************
*                                               __open_show
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

static void __open_show(void)
{
    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_OPEN_SHOW, 0, 0);
    __wait_willow_finish();
}

/*
**********************************************************************************************************************
*                                               anole_GetShowParam
*
* Description: get anole show para, such as show region and picture layer
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
void anole_get_show_param(__willow_showing_img_info_t *info)
{
    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_GET_SHOW_PARAM, 0, info);
    __wait_willow_finish();
}




__hdle anole_get_pic_lyr()
{
    __willow_showing_img_info_t inf;

    anole_get_show_param(&inf);

    return (inf.layer_hdle);
}


__bool anole_show_pic_from_buffer(__u8 *buffer, __s32 width, __s32 height)
{
    __willow_show_buffer_param_t param;

    param.img_buf = buffer;
    param.img_format = PIXEL_COLOR_ARGB8888;
    param.buf_size =   width * height * 4;
    param.img_size.width = width;
    param.img_size.height = height;

    __here__;
    __wait_willow_ready();
    __here__;
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SHOW_IMG_FROM_BUFFER, 0, &param);
    __here__;
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_START_DEC, 0, 0);
    __here__;
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_START_SHOW, 0, 0);
    __here__;
    __wait_willow_finish();
    __here__;

    return 0;

}


/*
**********************************************************************************************************************
*                                               anole_Init
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__s32 anole_init(/*__anole_para_t *para*/void)
{
    __s32 ret = 0;


    if (NULL == g_anole_sem)
    {
        g_anole_sem = esKRNL_SemCreate(1);
    }

    /* install willow */
    mid_willow = esMODS_MInstall("d:\\mod\\willow\\willow.mod", 0);
    if(!mid_willow)
    {
        __wrn("install willow fail!\n");
        return -1;
    }

    /*open willow*/
    mod_willow = esMODS_MOpen(mid_willow, 0);
    if(mod_willow == 0)
    {
        __wrn("open willow fail!\n");
        return -1;
    }

    //create npl
    if (__npl_create(RAT_MEDIA_TYPE_PIC) != 0)
    {
        __msg("create npl failed\n");
        return -1;
    }

    __inf("open npl success!\n");

    __open_show();

    /* start thread */
    anole_core.maintask = esKRNL_TCreate(__auto_play_maintsk, 0, 0x2000, KRNL_priolevel4);
    if (anole_core.maintask == 0)
    {
        __inf("create maintask thread failed!\n");
        return -1;
    }

    anole_init_screen_size(&LCD_WIDTH, &LCD_HEIGHT);

    return ret;

}

__pCBK_t anole_set_cb( anole_event_e event, __pCBK_t cb , void *ctx)
{
    __pCBK_t  cb_old;

    ANOLE_ENTER_CRITICAL_SECTION

    switch( event )
    {
    case ANOLE_EVENT_SHOW_PIC_FAIL:
    {
        cb_old = cb_show_pic_fail;
        cb_show_pic_fail = cb;
        cb_ctx_show_pic_fail = ctx;
        break;
    }
    case ANOLE_EVENT_SHOW_PIC_NEXT:
    {
        cb_old = cb_show_pic_next;
        cb_show_pic_next = cb;
        cb_ctx_show_pic_next = ctx;
        break;
    }
    default :
    {
        cb_old = NULL;
        break;
    }
    }

    ANOLE_LEAVE_CRITICAL_SECTION

    return cb_old;
}

__pCBK_t anole_get_cb( anole_event_e event )
{
    __pCBK_t   cb;

    ANOLE_ENTER_CRITICAL_SECTION
    switch( event )
    {
    case ANOLE_EVENT_SHOW_PIC_FAIL:
    {
        cb = cb_show_pic_fail;
        break;
    }
    case ANOLE_EVENT_SHOW_PIC_NEXT:
    {
        cb = cb_show_pic_next;
        break;
    }
    default :
    {
        cb = NULL;
        break;
    }
    }

    ANOLE_LEAVE_CRITICAL_SECTION

    return cb;
}

/*
**********************************************************************************************************************
*                                               anole_AWILLOW_Uninitial
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/


void anole_exit(void)
{
    //__hdle   hnode = NULL;
    //char     key_value[ELIBS_KEY_VALUE_LEN + 1];

    anole_auto_play_thread_del();

    __msg("********before __wait_willow_ready********\n");
    __wait_willow_ready();
    __msg("********after __wait_willow_ready********\n");

    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SHUT_SHOW, 0, 0);
    __msg("********before __wait_willow_finish********\n");
    __wait_willow_finish();
    __msg("********after __wait_willow_finish********\n");

    /* 关闭和卸载中间件willow   */
    esMODS_MClose(mod_willow);
    __here__;
    esMODS_MUninstall(mid_willow);

    __here__;
    //delete npl
    __npl_delete();

    __here__;

    if (g_anole_sem)
    {
        __u8 err;
        esKRNL_SemDel(g_anole_sem, OS_DEL_ALWAYS, &err);
        g_anole_sem = NULL;
    }

    __here__
    return;
}


/*
**********************************************************************************************************************
*                                               anole_GetSta
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__anole_sta_t anole_get_sta(void)
{
    return(anole_core.status);
}

/*
**********************************************************************************************************************
*                                               anole_BackNormal
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__bool anole_backnormal(void)
{
    if (anole_core.status == ANOLE_MODE_ZOOM || anole_core.status == ANOLE_MODE_PLAY)
    {
        if (anole_core.status == ANOLE_MODE_ZOOM)
        {
            __wait_willow_ready();
            esMODS_MIoctrl(mod_willow, WILLOW_CMD_COME_BACK, 0, NULL);
            __wait_willow_finish();
        }
        anole_core.status = ANOLE_MODE_NORMAL;

        anole_core.scale_level = 0;

    }
    return 0;
}


/*
************************************************************************************************************************
*                       				anole_Play
*
*Description:
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/

__bool anole_auto_play(void)
{
    anole_core.status = ANOLE_MODE_PLAY; //let the thread to play pictrue
    return 0;
}


/*
**********************************************************************************************************************
*                                               anole_RotateLeft
*
* Description: picture rotate left
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__bool anole_rotate_left()
{
    if (anole_core.status == ANOLE_MODE_PLAY)
        return EPDK_FAIL;


    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_ROTATE, -90, NULL);
    __wait_willow_finish();

    anole_core.scale_level = 0;

    return 0;
}



/*
**********************************************************************************************************************
*                                               anole_RotateRight
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__bool anole_rotate_right(void)
{
    if (anole_core.status == ANOLE_MODE_PLAY)
        return EPDK_FAIL;

    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_ROTATE, 90, NULL);
    __wait_willow_finish();

    anole_core.scale_level = 0;

    return 0;
}


/*
**********************************************************************************************************************
*                                               anole_ZoomInc
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__bool anole_zoom_inc(void)
{
    if (anole_core.status == ANOLE_MODE_PLAY)
        return EPDK_FAIL;

    anole_core.status = ANOLE_MODE_ZOOM;

    if((anole_core.scale_level < 3))
    {
        anole_core.scale_level++;
        __wait_willow_ready();
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_SCALE, anole_core.scale_level, NULL);
        __wait_willow_finish();
    }
    if (anole_core.scale_level == 0)
        anole_core.status = ANOLE_MODE_NORMAL;

    return 0;

}

/*
**********************************************************************************************************************
*                                               anole_ZoomDec
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__bool anole_zoom_dec(void)
{
    if (anole_core.status == ANOLE_MODE_PLAY)
        return EPDK_FAIL;

    anole_core.status = ANOLE_MODE_ZOOM;

    if((anole_core.scale_level > -3))
    {
        anole_core.scale_level--;
        __wait_willow_ready();
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_SCALE, anole_core.scale_level, NULL);
        __wait_willow_finish();
    }

    if (anole_core.scale_level == 0)
        anole_core.status = ANOLE_MODE_NORMAL;
    return 0;
}

/*
**********************************************************************************************************************
*                                               anole_zoom_set
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__bool anole_zoom_set(__s32 level)
{
    if (anole_core.status == ANOLE_MODE_PLAY)
        return EPDK_FAIL;

    if(level < -3 || level > 3)
    {
        __msg("level is invalid...\n");
        return EPDK_FALSE;
    }

    anole_core.status = ANOLE_MODE_ZOOM;

    anole_core.scale_level = level;
    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SCALE, anole_core.scale_level, NULL);
    __wait_willow_finish();

    if (anole_core.scale_level == 0)
        anole_core.status = ANOLE_MODE_NORMAL;

    return 0;

}

/*
**********************************************************************************************************************
*                                               anole_zoom_get
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 anole_zoom_get(void)
{
    return anole_core.scale_level;
}

/*
**********************************************************************************************************************
*                                               anole_SetScrRect
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 anole_set_scn_rect(RECT *rect)
{
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_COME_BACK, 0, NULL);
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SET_SCN, 1, rect);

    return 0;
}

/*
**********************************************************************************************************************
*                                               anole_SetPlayInterval
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 anole_set_play_interval(__u32 sec)
{
    if (sec > 15 || sec < 1)
        return EPDK_FAIL;
    anole_core.interval = sec;
    return 0;
}



/*
**********************************************************************************************************************
*                                               anole_SetSwitchMode
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__s32 anole_set_switch_mode(__anole_switch_mode_t mode)
{
    anole_core.SwitchMode = mode;

    mode = trans_to_willow_switchmode();

    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SET_SWITCH_MODE, mode, 0);
    __wait_willow_finish();

    return 0;
}


__s32 anole_set_switch_mode_array_for_play(__s32 *switch_mode_array, __s32 array_num)
{
    anole_core.switchmodearray 	= switch_mode_array;
    anole_core.arraynum 		= array_num;

    return 0;
}


/*
**********************************************************************************************************************
*                                               anole_SetPlayOrder
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__s32 anole_set_play_order(__anole_playorder_t order)
{
    if(ANOLE_PLAY_NEXT == order || ANOLE_PLAY_PREV == order)
    {
        rat_npl_set_play_mode(anole_npl, RAT_PLAY_MODE_ROTATE_ALL);
    }
    else
    {
        rat_npl_set_play_mode(anole_npl, RAT_PLAY_MODE_RANDOM);
    }
    anole_core.PlayOrder = order;
    return 0;
}


/*
**********************************************************************************************************************
*                                               anole_GetSwichMode
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/


__anole_switch_mode_t anole_get_switch_mode(void)
{
    return(anole_core.SwitchMode);
}


/*
**********************************************************************************************************************
*                                               anole_GetScrRect
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__bool anole_get_scn_rect(RECT *rect)
{
    __willow_showing_img_info_t img_inf;

    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_GET_SHOW_PARAM, 0, &img_inf);
    __wait_willow_finish();

    rect->x = img_inf.show_region.x;
    rect->y = img_inf.show_region.y;
    rect->width = img_inf.show_region.width;
    rect->height = img_inf.show_region.height;

    return 0;
}

/*
**********************************************************************************************************************
*                                               anole_GetPlayInterval
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__u32 anole_get_play_interval(void)
{
    return(anole_core.interval);
}


/*
**********************************************************************************************************************
*                                               anole_SetBgPic
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 anole_set_bg_pic(FB *fb)
{
    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_CATCH_SCREEN, 0, fb);
    __wait_willow_finish();

    return 0;
}


/*
**********************************************************************************************************************
*                                               anole_GetPlayOrder
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__anole_playorder_t anole_get_play_order(void)
{
    return(anole_core.PlayOrder);
}


/*
**********************************************************************************************************************
*                                               anole_GetThumb
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 anole_get_thumb(char *filename, void *buf, __anole_thumbs_fmt_t *param)
{
    __willow_get_thumbs_param_t willow_para;

    if (buf == NULL || filename == NULL)
        return EPDK_FAIL;

    willow_para.buf = (__u8 *)buf;
    willow_para.filename = filename;
    willow_para.format = param->format;
    willow_para.size.width = param->size.width;
    willow_para.size.height = param->size.height;

    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_GET_THUMBS, 0, &willow_para);
    __wait_willow_finish();

    if(willow_para.filename)
    {

        if(esMODS_MIoctrl(mod_willow, WILLOW_CMD_CHECK_IMG, 0, &willow_para) == EPDK_FAIL)
        {
            return EPDK_FAIL;
        }

        __wait_willow_ready();
        //esMODS_MIoctrl(mod_willow,WILLOW_CMD_START_DEC,0,0);

        esMODS_MIoctrl(mod_willow, WILLOW_CMD_START_SHOW, 0, 0);

        __wait_willow_finish();
    }

    param->size.width = willow_para.size.width;
    param->size.height = willow_para.size.height;

    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               anole_GetPicInfo
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__s32 anole_get_pic_info(__anole_pic_info_t *info)
{
    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_GET_IMG_INFO, 0, (__willow_img_info_t *)info);
    __wait_willow_finish();

    return 0;
}

static __s32 __get_file_name_by_index(__willow_show_file_param_t *param)
{
    anole_npl_index2file(param->img_no, param->filename);

    return 0;
}

__s32 anole_slider_init(FB *decoding_fb, FB *unsupported_fb)
{
    __willow_img_plst_info_t playlist;

    playlist.total = anole_npl_get_total_count();
    playlist.get_imagename_by_index = esKRNL_GetCallBack((__pCBK_t)__get_file_name_by_index);
    playlist.decoding_fb[0] = decoding_fb;
    playlist.decoding_fb[1] = NULL;
    playlist.decoding_fb[2] = NULL;
    playlist.unsupported_fb = unsupported_fb;
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SET_PLAYLIST, 0, &playlist);

    return 0;
}

__s32 anole_slider_start(__s16 x, __s16 y)
{
    slider_step_para_t slider_cmd;

    slider_cmd.cmd 		= SLIDER_STEP_START;
    slider_cmd.offset.x = x;
    slider_cmd.offset.y = y;
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_MOV_TRACK, 0, &slider_cmd);

    return 0;
}

__s32 anole_slider_move(__s16 x, __s16 y)
{
    slider_step_para_t slider_cmd;
    slider_cmd.cmd 		= SLIDER_STEP_MOVE;
    slider_cmd.offset.x = x;
    slider_cmd.offset.y = y;
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_MOV_TRACK, 0, &slider_cmd);

    return 0;
}

__s32 anole_slider_finish(__s16 x, __s16 y)
{
    slider_step_para_t slider_cmd;
    __s32 index;

    slider_cmd.cmd		= SLIDER_STEP_FINISH;
    slider_cmd.offset.x = x;
    slider_cmd.offset.y = y;
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_MOV_TRACK, 0, &slider_cmd);

    __wait_willow_ready();
    index = esMODS_MIoctrl(mod_willow, WILLOW_CMD_GET_CUR_INDEX, 0, 0);
    __wait_willow_finish();

    anole_npl_playing_seek(index);

    return 0;
}

__s32 anole_set_layer_sta(__anole_layer_sta layer_sta)
{
    if(layer_sta == ANOLE_LAYER_TOP)
    {
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_CFG_LYR, WILLOW_LYR_TOP, 0);
    }
    else if(layer_sta == ANOLE_LAYER_BOTTOM)
    {
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_CFG_LYR, WILLOW_LYR_BOTTOM, 0);
    }
    else if(layer_sta == ANOLE_LAYER_SHOW)
    {
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_CFG_LYR, WILLOW_LYR_SHOW, 0);
    }
    else if(layer_sta == ANOLE_LAYER_HIDE)
    {
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_CFG_LYR, WILLOW_LYR_HIDE, 0);
    }
    return 0;
}

/*
**********************************************************************************************************************
*                                               anole_set_show_mode
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__bool anole_set_show_mode(__s32 index)
{
    __s32 tmp_index;
    if (index == 0)
        tmp_index = WILLOW_IMAGE_SHOW_AUTO;
    else if (index == 1)
        tmp_index = WILLOW_IMAGE_SHOW_ORISIZE;
    else if (index == 2)
        tmp_index = WILLOW_IMAGE_SHOW_CUTOFF;
    else
        tmp_index = WILLOW_IMAGE_SHOW_STRETCH;

    __wait_willow_ready();
    esMODS_MIoctrl(mod_willow, WILLOW_CMD_SET_SHOW_MODE, tmp_index, NULL);
    __wait_willow_finish();
    return 0;
}

/*
**********************************************************************************************************************
*                                               anole_scale_move
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__bool anole_scale_move(__anole_diretion_t way)
{
    __willow_showing_img_info_t info;
    __here__
    if (anole_core.status != ANOLE_MODE_ZOOM)
        return EPDK_FAIL;

    anole_get_show_param(&info);

    __inf("x = %d, y = %d, width = %d, height = %d\n", info.show_region.x, info.show_region.y , info.show_region.width, info.show_region.height);
    __here__
    switch(way)
    {
    case ANOLE_MOVE_UP:
        //			if( (info.show_region.y + info.show_region.height - 32) >= LCD_HEIGHT )
    {
        __pos_t param;
        param.x = 0;
        param.y = -32;

        __wait_willow_ready();
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_MOVE, 0, &param);
        __wait_willow_finish();;
    }
    break;
    case ANOLE_MOVE_DOWN:
        //			if( (info.show_region.y + 32) <= 0)
    {
        __pos_t param;
        param.x = 0;
        param.y = 32;

        __wait_willow_ready();
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_MOVE, 0, &param);
        __wait_willow_finish();
    }
    break;
    case ANOLE_MOVE_LEFT:
        //			if((info.show_region.x + info.show_region.width - 32) >= LCD_WIDTH)
    {
        __pos_t param;
        param.x = -32;
        param.y = 0;

        __wait_willow_ready();
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_MOVE, 0, &param);
        __wait_willow_finish();
    }
    break;
    case ANOLE_MOVE_RIGHT:
        //			if((info.show_region.x + 32) <= 0)
    {
        __pos_t param;
        param.x = 32;
        param.y = 0;

        __wait_willow_ready();
        esMODS_MIoctrl(mod_willow, WILLOW_CMD_MOVE, 0, &param);
        __wait_willow_finish();
    }
    break;
    default:
        break;
    }
    return 0;
}


