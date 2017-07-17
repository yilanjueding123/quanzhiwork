/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    		: app_root_win.c
* By      		: Bayden
* Func		: app_root applets plugin interface define
* Version	: v1.0
* ============================================================================================================
* 2010-9-28 17:11:20  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "beetles_app.h"
#include "app_root.h"
#include "app_root_init.h"
#include "app_root_scene.h"
/**********************************************************************************************************************/


/**********************************************************************************************************************
插件接口注册
**********************************************************************************************************************/
/* 声明 */
static __s32 app_root_start(Activity  *thiz);
static __s32 app_root_suspend(Activity *thiz);
static __s32 app_root_resume(Activity *thiz);
static __s32 app_root_background(Activity *thiz);
static __s32 app_root_stop(Activity *thiz);

/* 注册插件接口 */
AppletPlugin APPLET_PLUGIN_INTERFACE =
{
    app_root_start,
    app_root_suspend,
    app_root_resume,
    app_root_background,
    app_root_stop,
};

Activity *root_activity = NULL; //应用程序框架插件指针
H_WIN happ_root_manwin = NULL;//应用程序框架manwin句柄

static __s32 test_flush_nor2(__s32 i)
{
    __s32 result;
    ES_FILE *fp = NULL;
    char buf[3000] = {0};
    char path[] = "e:\\test2.bin";

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

static __s32 test_data[][2] =
{
    121545, 43251,
    40907, 64809,
    14724, 171149,
    115631, 73756
};

static __s32 test_read_nor_serial(void)
{
    __s32 result;

    result = EPDK_TRUE;

    eLIBs_printf("********begin test_read_nor***********\n");

    do //nor 读操作测试代码
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
        __s32 test_num;

        test_num = sizeof(test_data) / sizeof(test_data[0]);

        //open file
        fp = eLIBs_fopen(path, "rb");
        if(!fp)
        {
            result = EPDK_FALSE;
            eLIBs_printf("open file fail, path=%s\n", path);
            break;
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
            result = EPDK_FALSE;
            eLIBs_printf("esMEMS_Palloc fail\n");
            eLIBs_fclose(fp);
            break;
        }

        //begin test
        for(i = 0 ; i < test_num; i++)
        {
            eLIBs_printf("test3\n");
            //generate start reading pos
            start_pos = test_data[i][0];//eLIBs_ramdom(file_size-2);
            //start_pos = 80;
            eLIBs_printf("test4\n");
            //generate reading count
            once_count = test_data[i][1];//eLIBs_ramdom(file_size-start_pos);
            //once_count = 45699;
            eLIBs_printf("test5\n");
            if(0 == once_count)
            {
                once_count = 1;
            }

            //seek and read
            eLIBs_printf("before eLIBs_fseek\n");
            eLIBs_fseek(fp, start_pos, SEEK_SET);
            eLIBs_printf("after eLIBs_fseek\n");
            eLIBs_printf("before eLIBs_fread\n");
            ret = eLIBs_fread(pbuf, 1, once_count, fp);
            eLIBs_printf("after eLIBs_fread\n");
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
                eLIBs_printf("***************err encounter, begin dump****************\n");
                eLIBs_printf("***************err encounter, begin dump****************\n");
                eLIBs_printf("***************err encounter, begin dump****************\n");
                eLIBs_printf("***************err encounter, begin dump****************\n");

                eLIBs_printf("test bad, i=%d, start_pos=%d, once_count=%d, cur_data=%d, pbuf[%d]=%d, err_pos=%d, (start_pos+j)/1024=%f\n"
                             , i, start_pos, once_count, cur_data, j, pbuf[j], start_pos + j, (start_pos + j) / 1024.0);
                __wait__;

                if(1) //dump data
                {
                    __s32 k;
                    eLIBs_printf("***************err encounter, begin dump****************\n");
                    eLIBs_printf("***************err encounter, begin dump****************\n");
                    eLIBs_printf("***************err encounter, begin dump****************\n");
                    eLIBs_printf("***************err encounter, begin dump****************\n");
                    for(k = 0 ; k < once_count ; k++)
                    {
                        eLIBs_printf("pbuf[%.6d]=%X\n", k, pbuf[k]);
                    }
                    eLIBs_printf("***************end dump****************\n");
                }

                //break;
            }
            else//test alright
            {
                eLIBs_printf("test alright, i=%d, start_pos=%d, once_count=%d\n"
                             , i, start_pos, once_count);
            }
            eLIBs_printf("test1\n");
            esKRNL_TimeDly(2);
            eLIBs_printf("test2\n");

        }

        eLIBs_fclose(fp);
        esMEMS_Pfree(pbuf, page_num);

    }
    while(0);

    eLIBs_printf("********end test_read_nor, result=%d***********\n", result);
    __wait__;
    return result;
}


/**********************************************************************************************************************
插件接口实现
**********************************************************************************************************************/
static __s32 app_root_start(Activity *activity)
{
    if(0)
    {
        __s32 i;

        for(i = 0 ; i < 300 ; i++)
        {
            test_flush_nor2(i);
        }
    }
    __msg("**************app_root plugin start!**************** \n");
    __msg("****************************\n");
    //	__msg("**********TEST test_rat_partition**********\n");
    //	__msg("****************************\n");
    //	test_rat_partition(RAT_LOCAL_DISK);
    //	test_rat_partition(RAT_USB_DISK);
    //	test_rat_partition(RAT_SD_CARD);
    //   {
    //       __hdle fp;
    //       fp = esFSYS_fopen(BEETLES_APP_ROOT"apps\\lang.bin", "r");
    //        __msg("fp=%x file=%s\n", fp, BEETLES_APP_ROOT"apps\\lang.bin");
    //   esFSYS_fclose(fp);
    //    }

    //__wait__;
    //  test_read_nor();
    app_root_init_res();

    root_activity = activity;
    happ_root_manwin = app_root_wincreate(activity);

    //  test_read_nor();
    //test_read_nor_serial();
    //test_read_nor_serial();
    //test_read_nor_serial();
    //test_read_nor_serial();
    if (NULL == happ_root_manwin)
    {
        __msg("	 app_root_wincreate fail! \n");
        return EPDK_FAIL;
    }
    else
    {
        GUI_WinSetFocusChild(happ_root_manwin);
        __msg("	 app_root_wincreate success! \n");
        return EPDK_OK;
    }

}

static __s32 app_root_suspend(Activity *activity)
{
    __msg("	 app_root plugin suspend! \n");
    if(happ_root_manwin)
    {
        GUI_ManWinDelete(happ_root_manwin);
        happ_root_manwin = NULL;
        app_root_deinit_res();
    }

    return EPDK_OK;
}

static __s32 app_root_resume(Activity *activity)
{
    __msg("	 app_root plugin reseum! \n");
    __msg("**************app_root plugin start!**************** \n");
    app_root_init_res();

    root_activity = activity;
    happ_root_manwin = app_root_wincreate(activity);

    if (NULL == happ_root_manwin)
    {
        __msg("	 app_root_wincreate fail! \n");
        return EPDK_FAIL;
    }
    else
    {
        __msg("	 app_root_wincreate success! \n");
        return EPDK_OK;
    }

}

static __s32 app_root_background(Activity *activity)
{
    __msg("	 app_root plugin background! \n");
    if(happ_root_manwin)
    {
        GUI_ManWinDelete(happ_root_manwin);
        happ_root_manwin = NULL;
        app_root_deinit_res();
    }
    return EPDK_OK;
}

static __s32 app_root_stop(Activity *activity)
{
    __msg("	********************app_root plugin stop!**************** \n");

    if(happ_root_manwin)
    {
        GUI_ManWinDelete(happ_root_manwin);
        happ_root_manwin = NULL;
        app_root_deinit_res();
    }
    return EPDK_OK;
}

