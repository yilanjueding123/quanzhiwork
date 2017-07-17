/*
**********************************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           hello world sample
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : helloworld.c
* By      : Steven.ZGJ
* Version : V1.00
**********************************************************************************************************************
*/
#include "helloworld.h"


void test_user_disk(void)
{
    ES_FILE *hFile;
    __u8    *pBuffer;

    hFile = eLIBs_fopen("e:\\test.bin", "wb+");
    if (hFile == NULL)
    {
        __wrn("create file failed\n");
        return ;
    }
    __msg("create file succeeded\n");
    pBuffer = esMEMS_Palloc(32, 0);
    eLIBs_memset(pBuffer, 0x5a, 32 * 1024);

    eLIBs_fwrite(pBuffer, 1, 32 * 1024, hFile);
    eLIBs_fwrite(pBuffer, 1, 32 * 1024, hFile);
    esMEMS_Pfree(pBuffer, 32);

    eLIBs_fclose(hFile);

    //flush user disk data to NOR flash
    hFile = eLIBs_fopen("b:\\STORAGE\\SPINOR", "rb");
    if (hFile == NULL)
    {
        __wrn("open NOR file failed\n");
        return ;
    }
    __msg("flush file ....\n");
    eLIBs_fioctrl(hFile, DEV_IOC_USR_FLUSH_CACHE, 0, 0);

    __msg("flush end ....\n");
    eLIBs_fclose(hFile);
}

/*
**********************************************************************************************************************
*                                            helloworld_main
*
* Description:
*
* Arguments  :
*
* Returns    : None
* Notes      : 此函数用于演示一个进程的主线程的流程
**********************************************************************************************************************
*/
__s32 helloworld_main(void *p_arg)
{

    __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    __inf("&            Hello World!                               &\n");
    __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    __inf("& type \"exit\" to exit and return to shell!            &\n");
    __inf("& type \"del n\" (n equ 0~3) to del thread!             &\n");
    __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    __inf("& v1.0                                                  &\n");
    __inf("& By steven.zgj @ 2008/08/08                            &\n");
    __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");


    //sdcard_test();
    test_user_disk();

    return 0;
}
